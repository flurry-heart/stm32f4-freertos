#ifndef __UART_H__
#define __UART_H__

#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define UART_RECEIVE_LENGTH 200

extern UART_HandleTypeDef g_usart1_handle;                                      // USART1句柄

extern uint8_t g_usart1_rx_buffer[UART_RECEIVE_LENGTH];                         // 接收数据缓冲区
extern uint16_t g_usart1_rx_status;                                             // 接收状态标记

void UART_Init(UART_HandleTypeDef *huart, USART_TypeDef *UARTx, uint32_t band);
void UART_Printf(UART_HandleTypeDef *huart, char *fmt, ...);

#endif // !__UART_H__