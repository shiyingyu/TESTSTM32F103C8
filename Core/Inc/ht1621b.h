#ifndef  HT1621B_H_
#define HT1621B_H_   //防止重复包含

#include "stm32f1xx_hal.h"

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

#define HT1621_CS_Pin GPIO_PIN_10
#define HT1621_WR_Pin GPIO_PIN_11
#define HT1621_DATA_Pin GPIO_PIN_12
#define HT1621_GPIO_PORT GPIOB

#define HT1621_CS1     HAL_GPIO_WritePin(HT1621_GPIO_PORT, HT1621_CS_Pin, GPIO_PIN_SET)
#define HT1621_CS0     HAL_GPIO_WritePin(HT1621_GPIO_PORT, HT1621_CS_Pin, GPIO_PIN_RESET)
#define HT1621_WR1     HAL_GPIO_WritePin(HT1621_GPIO_PORT, HT1621_WR_Pin, GPIO_PIN_SET)
#define HT1621_WR0     HAL_GPIO_WritePin(HT1621_GPIO_PORT, HT1621_WR_Pin, GPIO_PIN_RESET)
#define HT1621_DATA1   HAL_GPIO_WritePin(HT1621_GPIO_PORT, HT1621_DATA_Pin, GPIO_PIN_SET)
#define HT1621_DATA0   HAL_GPIO_WritePin(HT1621_GPIO_PORT, HT1621_DATA_Pin, GPIO_PIN_RESET)

#define  ComMode    0x52
#define  RCosc      0x30
#define  LCD_on     0x06
#define  LCD_off    0x04
#define  Sys_en     0x02
#define  CTRl_cmd   0x80
#define  Data_cmd   0xa0

extern uint8_t HT1621B_NUMBERS[];

// 基本函数
void HT1621B_Init(void);
void HT1621B_TurnOn_All(void);
void HT1621B_TurnOff_All(void);
void HT1621B_WriteRAM(uint8_t addr, uint8_t sdata);
void HT1621B_SendCmd(uint8_t command);
void HT1621B_SendBits(uint8_t sdata, uint8_t cnt);
void HT1621B_Scan(uint8_t StartAddress, uint8_t EndAddress);

// 特定液晶屏功能封装函数
void num_print(int num);
void Err_print(void);
void minus_print(void);
void battey_level(short unsigned int level);
void signal_method(short unsigned int method, short unsigned int strength);
void channel_count(int channel);
void units(short unsigned int unit); // temp=0, humi=1
void clear(int len); // length is 16
void symbol_HIGH(void);
void symbol_LOW(void);
void symbol_REFRESH(void);
void set_print(void);
void ON_print(void);
#endif
