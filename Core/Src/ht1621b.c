#include "gpio.h"
#include "ht1621b.h"
#include "stm32f1xx_hal.h"
#include "rbt_delay.h"
#include <stdio.h>

uint8_t HT1621B_NUMBERS[] = { 0XEB, 0X60, 0XC7, 0XE5, 0X6C, 0XAD, 0XAF, 0XE0, 0XEF, 0XED, 0X01};
char error[] = { 0xE5, 0x02, 0xE7 }; //n,-,A for n-A print
uint8_t addr;

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
