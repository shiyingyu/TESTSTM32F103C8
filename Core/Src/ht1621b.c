#include "gpio.h"
#include "ht1621b.h"
#include "stm32f1xx_hal.h"
#include "rbt_delay.h"
#include <stdio.h>

/**
 * 数字编码
 * 0-9 . -
 */
uint8_t QYH04418_NUMBERS[] = { 0XEB, 0X60, 0XC7, 0XE5, 0X6C, 0XAD, 0XAF, 0XE0, 0XEF, 0XED, 0X10, 0X04};

/**
 * @brief 初始化HT1621B，包括GPIO端口和发送初始化命令
 * @return None
 */
void HT1621B_Init() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = HT1621_CS_Pin | HT1621_DATA_Pin | HT1621_WR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(HT1621_GPIO_PORT, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(HT1621_GPIO_PORT, HT1621_CS_Pin | HT1621_WR_Pin | HT1621_DATA_Pin, GPIO_PIN_SET);
	
	// 初始化命令
	HT1621B_SendCmd(Sys_en);
	HT1621B_SendCmd(RCosc);
	HT1621B_SendCmd(ComMode);
	HT1621B_SendCmd(LCD_on);
}

/**
 * @brief 向HT1621芯片发送一定数量比特的数据
 * @param sdata 保存要发送的数据
 * @param cnt 发送的比特数
 * @return None
 */
void HT1621B_SendBits(uint8_t sdata, uint8_t cnt) {
	while (cnt -- > 0) {
		HT1621_WR0;
		if (sdata & 0x80) // 10000000
			HT1621_DATA1;
		else
			HT1621_DATA0;
		
		// Delay 1 uS to lower the frequency
		RBT_Delay_US(1);
		HT1621_WR1;
		RBT_Delay_US(1);
		sdata <<= 1;
	}
}

/**
 * @brief 向HT1621B发送一个命令
 * @param command 命令
 * @return None
 */
void HT1621B_SendCmd(uint8_t command) {
	HT1621_CS0;
	HT1621B_SendBits(0x80, 4);
	HT1621B_SendBits(command, 8);
	HT1621_CS1;
}

/**
 * @brief 向HT1621B写内存
 * @param addr 要写入的地址
 * @param sdata 要写入的数据
 */
void HT1621B_WriteRAM(uint8_t addr, uint8_t sdata) {
	addr <<= 2;
	HT1621_CS0;
	// 命令ID：10100000 = 0xA0
	HT1621B_SendBits(0xa0, 3);
	// 6位地址
	HT1621B_SendBits(addr, 6);
	// 4位Data
	HT1621B_SendBits(sdata, 4);
	HT1621_CS1;
}

/**
 * @brief 读HT1621B内存
 * @param addr 要读取的地址
 * @return 读取的数据
 */
uint8_t HT1621B_ReadRAM(uint8_t addr) {
	addr <<= 2;
	HT1621_CS0;
	// 命令ID：10100000 = 0xA0
	HT1621B_SendBits(0xa0, 3);
	// 6位地址
	HT1621B_SendBits(addr, 6);
	// 4位Data
	HT1621B_SendBits(0, 4);
	HT1621_CS1;
	return 0;
}

/**
 * @brief 关闭所有的段码（将所有内存位置为0
 * @param num 内存地址数（每单个地址为4位）
 */
void HT1621B_TurnOff_All() {
	uint8_t i;
	for (i = 0; i < 32; i++) {
		HT1621B_WriteRAM(i, 0x00);
	}
}

/**
 * @brief 打开所有的段码（将所有内存位置为1
 * @param num 内存地址数（每单个地址为4位）
 */
void HT1621B_TurnOn_All() {
	uint8_t i;
	for (i = 0; i < 32; i++) {
		HT1621B_WriteRAM(i, 0xff);
	}
}

/**
 * @brief 扫描段码（逐段打开，用于建立对照表）
 * @param StartAddress 扫描开始地址（0~31）
 * @param EndAddress 扫描结束地址（0~31）
 * @return None
 */
void HT1621B_Scan(uint8_t StartAddress, uint8_t EndAddress)
{
	uint8_t i, mem;
	
	HT1621B_TurnOff_All();
	
	while (StartAddress <= EndAddress)
	{
		mem = 0;
		for (i = 0; i < 4; i ++)
		{
			mem = (mem << 1) + 1;
			HT1621B_WriteRAM(StartAddress, mem << 4);
			RBT_Delay_MS(500);
		}
		// 一个地址结束的时候，闪烁两次
		
		HT1621B_WriteRAM(StartAddress, 0);
		RBT_Delay_MS(250);
		HT1621B_WriteRAM(StartAddress, 0xf0);
		RBT_Delay_MS(250);
		HT1621B_WriteRAM(StartAddress, 0);
		RBT_Delay_MS(250);
		HT1621B_WriteRAM(StartAddress, 0xf0);
		RBT_Delay_MS(250);
		
		StartAddress ++;
	}
}

/**
 * @brief 显示一个最大不超过5位的数字（可含符号和点号）
 * @note 最多显示5位，超过5位只显示最后5位；最后一位不能是点号，否则会与摄氏度符号冲突
 * @param Number 要显示的数字
 * @param Length 字符串长度
 * @return None
 */
void QYH04418_Number(volatile char *Number, uint8_t Length)
{
	volatile int8_t i, addr = 9, idx;
	volatile uint8_t	value;
	volatile uint8_t dot_flag = 0;
	
	// 如果超过5位，只显示后面5位（含符号，小数点不占位数）
	for (i = Length - 1; i >= 0; i --) {
		idx = Number[i];
		if (idx >= '0' && idx <= '9') idx -= '0';
		else if (idx == '.') {
			dot_flag = 1;
			continue;
		}
		else if (idx == '-') idx = 11;
		else continue;
		
		value = QYH04418_NUMBERS[idx];
		// 看看前一位是否位'.'，若是，在本位上加上点号
		if (dot_flag) {
			value |= QYH04418_NUMBERS[10];
			dot_flag = 0;
		}
		HT1621B_WriteRAM(addr --, value);
		HT1621B_WriteRAM(addr --, value << 4);
		// 写完5位后就忽略后面的
		if (addr == 0xff) break;
	}
	
	// 把剩余的位数清掉
	for (i = addr; i >= 0; i --) {
		HT1621B_WriteRAM(i, 0);
	}
}

/**
 * @brief 显示摄氏度（带符号）
 * @param Value 要显示的摄氏度
 * @param Dot_Position 小数点的位置，从左起， 0表示没有小数点）
 * @note 小数位数与小数点是相反的，如果小数位数为1，则Dot_Position取4，若
 *       小数位数为4，则Dot_Position取1
 */
void QYH04418_Celsius(char *Number, uint8_t Length)
{
	// 清除百分比单位
	HT1621B_WriteRAM(9, 0);
	
	QYH04418_Number(Number, Length);
	// 整个第10段只有一个摄氏度符号，所以全部置1即可
	HT1621B_WriteRAM(10, 0xFF);
}


void QYH04418_Percent(char *Number, uint8_t Length)
{
	uint8_t num;
	// 清除摄氏度单位
	HT1621B_WriteRAM(10, 0);
	
	QYH04418_Number(Number, Length);
	
	// 把最后一位数字的内存上通过位运算把百分比符号加上
	// 跳过结尾处的'.'号
	while (!((Number[Length - 1] >= '0' && Number[Length - 1] <= '9') || (Number[Length - 1] == '-'))) Length --;
	num = Number[Length - 1];
	if (num >= '0' && num <= '9') num -= '0';
	else if (num == '-') num = 11;
	
	uint8_t value = QYH04418_NUMBERS[num];
	value = value | ( 1 << 4);
	HT1621B_WriteRAM(9, value);
}

/**
 * @brief 显示信号等级
 * @param Level 取值范围为0~3
 */
void QYH04418_Signal(uint8_t Level)
{
	// 电池轮廓默认显示
	uint8_t value = 8;
	
	if (Level == 0) {
	}
	else if (Level == 1) {
		value += 4;
	}
	else if (Level == 2) {
		value += 4 + 1;
	}
	else if (Level == 3) {
		value += 4 + 1 + 2;
	}
	HT1621B_WriteRAM(11, value << 4);
}
