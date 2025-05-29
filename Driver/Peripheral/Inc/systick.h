#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "stm32f4xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

void Delay_Init(void);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);

extern void xPortSysTickHandler(void);

#endif // !__SYSTICK_H__