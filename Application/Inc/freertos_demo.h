#ifndef __FREERTOS_DEMO_H__
#define __FREERTOS_DEMO_H__

#include <stdio.h>

#include "stm32f4xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

#include "key/key.h"

extern TaskHandle_t task1_handle;

void freertos_demo(void);

#endif // !__FREERTOS_DEMO_H__