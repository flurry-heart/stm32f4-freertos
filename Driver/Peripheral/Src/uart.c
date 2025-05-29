#include "uart.h"

UART_HandleTypeDef g_usart1_handle;                                             // USART1句柄

uint8_t g_uart_rx_buffer[1];                                                    // HAL库使用的串口接收数据缓冲区
uint8_t g_usart1_rx_buffer[UART_RECEIVE_LENGTH];                                // 接收数据缓冲区
uint16_t g_usart1_rx_status = 0;                                                // 接收状态标记

/**
 * @brief 串口初始化函数
 * 
 * @param huart 串口句柄
 * @param UARTx 串口寄存器基地址
 * @param band 波特率
 */
void UART_Init(UART_HandleTypeDef *huart, USART_TypeDef *UARTx, uint32_t band)
{
    huart->Instance = UARTx;                                                    // 寄存器基地址
    huart->Init.BaudRate = band;                                                // 波特率
    huart->Init.WordLength = UART_WORDLENGTH_8B;                                // 数据位
    huart->Init.StopBits = UART_STOPBITS_1;                                     // 停止位
    huart->Init.Parity = UART_PARITY_NONE;                                      // 奇偶校验位
    huart->Init.Mode = UART_MODE_TX_RX;                                         // 收发模式
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;                                // 硬件流控制
    huart->Init.OverSampling = UART_OVERSAMPLING_16;                            // 过采样
    HAL_UART_Init(huart);

    HAL_UART_Receive_IT(huart, (uint8_t *)g_uart_rx_buffer, 1);                 // 开启接收中断
}

/**
 * @brief 串口底层初始化函数
 * 
 * @param huart 串口句柄
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (huart->Instance == USART1)                                              // 初始化的串口是否是USART1
    {
        __HAL_RCC_USART1_CLK_ENABLE();                                          // 使能USART1时钟
        __HAL_RCC_GPIOA_CLK_ENABLE();                                           // 使能对应GPIO的时钟

        // PA9 -> USART TXD
        GPIO_InitStruct.Pin = GPIO_PIN_9;                                       // USART1 TXD的引脚
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                                 // 推挽式复用
        GPIO_InitStruct.Pull = GPIO_NOPULL;                                     // 不使用上下拉
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;                           // 输出速度
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;                            // 复用功能
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // PA10 -> USART RXD
        GPIO_InitStruct.Pin = GPIO_PIN_10;                                      // USART1 RXD的引脚
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_EnableIRQ(USART1_IRQn);                                        // 使能USART1中断
        HAL_NVIC_SetPriority(USART1_IRQn, 2, 0);                                // 设置中断优先级
    }
}

/**
 * @brief USART1中断服务函数
 * 
 */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&g_usart1_handle);                                      // 调用HAL库公共处理函数
    HAL_UART_Receive_IT(&g_usart1_handle, (uint8_t *)g_uart_rx_buffer, 1);      // 再次开启接收中断
}

/**
 * @brief USART接收中断回调函数
 * 
 * @param huart 串口句柄
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if ((g_usart1_rx_status & 0x8000) ==  0)                                // 接收未完成
        {
            if (g_usart1_rx_status & 0x4000)                                    // 接收到了0x0D，即回车键
            {
                if (g_uart_rx_buffer[0] != 0x0A)                                // 接收到的不是0x0A，即不是换行符
                {
                    g_usart1_rx_status = 0;                                     // 接收错误，重新开始
                }
                else                                                            // 接收到的是0x0A，即换行符
                {
                    g_usart1_rx_status |= 0x8000;                               // 接收完成  
                }
            }
            else                                                                // 还没接收到0x0D，即还没接收到回车键
            {
                if (g_uart_rx_buffer[0] == 0x0D)                                // 接收到的是0x0D，即回车键
                {
                    g_usart1_rx_status |= 0x4000;
                }
                else                                                            // 如果没有接收到回车
                {
                    // 接收到的数据存入接收缓冲区
                    g_usart1_rx_buffer[g_usart1_rx_status & 0x3FFF] = g_uart_rx_buffer[0];
                    g_usart1_rx_status++;
                    if (g_usart1_rx_status > (UART_RECEIVE_LENGTH - 1))         // 接收到数据大于接收缓冲区大小
                    {
                        g_usart1_rx_status = 0;                                 // 接收数据错误，重新开始接收
                    }
                }
            }
        }
    }
}

/**
 * @brief 重写_write使用printf()函数
 * 
 * @param fd 一个非负整数，代表要写入数据的文件或设备的标识
 * @param ptr 一个指向字符数据的指针，即要写入的数据的起始位置
 * @param length 一个整数，表示要写入的数据的字节数
 * @return int 数据的字节数
 */
int _write(int fd, char *ptr, int length)
{
    HAL_UART_Transmit(&g_usart1_handle, (uint8_t *)ptr, length, 0xFFFF);        // g_usart1_handle是对应串口
    return length;
}

/**
 * @brief 多串口使用printf()函数
 * 
 * @param huart 串口句柄
 * @param fmt 格式化字符串
 * @param ... 格式化参数
 */
void UART_Printf(UART_HandleTypeDef *huart, char *fmt, ...)
{
    char buffer[UART_RECEIVE_LENGTH+1];                                           // 用来存放转换后的数据
    uint16_t i = 0;

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, UART_RECEIVE_LENGTH+1, fmt, args);                        // 将格式化字符串转换为字符数组

    i = (strlen(buffer) > UART_RECEIVE_LENGTH) ? UART_RECEIVE_LENGTH : strlen(buffer);

    HAL_UART_Transmit(huart, (uint8_t *)buffer, i, 1000);                         // 串口发送数据

    va_end(args);
}