#include "stm32f4xx_hal.h"

#include "clock.h"
#include "systick.h"

#include "freertos_demo.h"

#include "uart.h"
#include "exti.h"

#include "led/led.h"
#include "key/key.h"

int main(void)
{
    HAL_Init();
    System_Clock_Init(8, 336, 2, 7);
    Delay_Init();

    UART_Init(&g_usart1_handle, USART1, 115200);

    LED_Init();
    Key_Init();

    EXTI2_Init();

    while (1)
    {
        freertos_demo();
    }
    
    return 0;
}