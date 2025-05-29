#ifndef  __CLOCK_H__
#define  __CLOCK_H__

#include "stm32f4xx_hal.h"

uint8_t System_Clock_Init(uint32_t pllm, uint32_t plln, uint32_t pllp, uint32_t pllq);

#endif // !__CLOCK_H__