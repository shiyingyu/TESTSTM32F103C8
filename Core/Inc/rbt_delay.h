#ifndef __RBT_DELAY_H
#define __RBT_DELAY_H 			   
#include "stm32f1xx_hal.h"

void RBT_Delay_Init(void);
void RBT_Delay_MS(uint16_t nMilliseconds);
void RBT_Delay_US(uint32_t nMicroseconds);
void RBT_Delay_Second(uint8_t nSeconds);

#endif
