#include "gpio.h"
#include "ht1621b.h"
#include "stm32f1xx_hal.h"
#include "rbt_delay.h"
#include <stdio.h>

/**
 * ���ֱ���
 * 0-9 . -
 */
uint8_t QYH04418_NUMBERS[] = { 0XEB, 0X60, 0XC7, 0XE5, 0X6C, 0XAD, 0XAF, 0XE0, 0XEF, 0XED, 0X10, 0X04};

/**
 * @brief ��ʼ��HT1621B������GPIO�˿ںͷ��ͳ�ʼ������
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
	
	// ��ʼ������
	HT1621B_SendCmd(Sys_en);
	HT1621B_SendCmd(RCosc);
	HT1621B_SendCmd(ComMode);
	HT1621B_SendCmd(LCD_on);
}

/**
 * @brief ��HT1621оƬ����һ���������ص�����
 * @param sdata ����Ҫ���͵�����
 * @param cnt ���͵ı�����
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
 * @brief ��HT1621B����һ������
 * @param command ����
 * @return None
 */
void HT1621B_SendCmd(uint8_t command) {
	HT1621_CS0;
	HT1621B_SendBits(0x80, 4);
	HT1621B_SendBits(command, 8);
	HT1621_CS1;
}

/**
 * @brief ��HT1621Bд�ڴ�
 * @param addr Ҫд��ĵ�ַ
 * @param sdata Ҫд�������
 */
void HT1621B_WriteRAM(uint8_t addr, uint8_t sdata) {
	addr <<= 2;
	HT1621_CS0;
	// ����ID��10100000 = 0xA0
	HT1621B_SendBits(0xa0, 3);
	// 6λ��ַ
	HT1621B_SendBits(addr, 6);
	// 4λData
	HT1621B_SendBits(sdata, 4);
	HT1621_CS1;
}

/**
 * @brief ��HT1621B�ڴ�
 * @param addr Ҫ��ȡ�ĵ�ַ
 * @return ��ȡ������
 */
uint8_t HT1621B_ReadRAM(uint8_t addr) {
	addr <<= 2;
	HT1621_CS0;
	// ����ID��10100000 = 0xA0
	HT1621B_SendBits(0xa0, 3);
	// 6λ��ַ
	HT1621B_SendBits(addr, 6);
	// 4λData
	HT1621B_SendBits(0, 4);
	HT1621_CS1;
	return 0;
}

/**
 * @brief �ر����еĶ��루�������ڴ�λ��Ϊ0
 * @param num �ڴ��ַ����ÿ������ַΪ4λ��
 */
void HT1621B_TurnOff_All() {
	uint8_t i;
	for (i = 0; i < 32; i++) {
		HT1621B_WriteRAM(i, 0x00);
	}
}

/**
 * @brief �����еĶ��루�������ڴ�λ��Ϊ1
 * @param num �ڴ��ַ����ÿ������ַΪ4λ��
 */
void HT1621B_TurnOn_All() {
	uint8_t i;
	for (i = 0; i < 32; i++) {
		HT1621B_WriteRAM(i, 0xff);
	}
}

/**
 * @brief ɨ����루��δ򿪣����ڽ������ձ�
 * @param StartAddress ɨ�迪ʼ��ַ��0~31��
 * @param EndAddress ɨ�������ַ��0~31��
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
		// һ����ַ������ʱ����˸����
		
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
 * @brief ��ʾһ����󲻳���5λ�����֣��ɺ����ź͵�ţ�
 * @note �����ʾ5λ������5λֻ��ʾ���5λ�����һλ�����ǵ�ţ�����������϶ȷ��ų�ͻ
 * @param Number Ҫ��ʾ������
 * @param Length �ַ�������
 * @return None
 */
void QYH04418_Number(volatile char *Number, uint8_t Length)
{
	volatile int8_t i, addr = 9, idx;
	volatile uint8_t	value;
	volatile uint8_t dot_flag = 0;
	
	// �������5λ��ֻ��ʾ����5λ�������ţ�С���㲻ռλ����
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
		// ����ǰһλ�Ƿ�λ'.'�����ǣ��ڱ�λ�ϼ��ϵ��
		if (dot_flag) {
			value |= QYH04418_NUMBERS[10];
			dot_flag = 0;
		}
		HT1621B_WriteRAM(addr --, value);
		HT1621B_WriteRAM(addr --, value << 4);
		// д��5λ��ͺ��Ժ����
		if (addr == 0xff) break;
	}
	
	// ��ʣ���λ�����
	for (i = addr; i >= 0; i --) {
		HT1621B_WriteRAM(i, 0);
	}
}

/**
 * @brief ��ʾ���϶ȣ������ţ�
 * @param Value Ҫ��ʾ�����϶�
 * @param Dot_Position С�����λ�ã������� 0��ʾû��С���㣩
 * @note С��λ����С�������෴�ģ����С��λ��Ϊ1����Dot_Positionȡ4����
 *       С��λ��Ϊ4����Dot_Positionȡ1
 */
void QYH04418_Celsius(char *Number, uint8_t Length)
{
	// ����ٷֱȵ�λ
	HT1621B_WriteRAM(9, 0);
	
	QYH04418_Number(Number, Length);
	// ������10��ֻ��һ�����϶ȷ��ţ�����ȫ����1����
	HT1621B_WriteRAM(10, 0xFF);
}


void QYH04418_Percent(char *Number, uint8_t Length)
{
	uint8_t num;
	// ������϶ȵ�λ
	HT1621B_WriteRAM(10, 0);
	
	QYH04418_Number(Number, Length);
	
	// �����һλ���ֵ��ڴ���ͨ��λ����Ѱٷֱȷ��ż���
	// ������β����'.'��
	while (!((Number[Length - 1] >= '0' && Number[Length - 1] <= '9') || (Number[Length - 1] == '-'))) Length --;
	num = Number[Length - 1];
	if (num >= '0' && num <= '9') num -= '0';
	else if (num == '-') num = 11;
	
	uint8_t value = QYH04418_NUMBERS[num];
	value = value | ( 1 << 4);
	HT1621B_WriteRAM(9, value);
}

/**
 * @brief ��ʾ�źŵȼ�
 * @param Level ȡֵ��ΧΪ0~3
 */
void QYH04418_Signal(uint8_t Level)
{
	// �������Ĭ����ʾ
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
