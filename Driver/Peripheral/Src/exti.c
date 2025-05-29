#include "exti.h"

/**
 * @brief EXTI2初始化函数
 * 
 */
void EXTI2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOE_CLK_ENABLE();                                               // 使能GPIOE的时钟

    GPIO_InitStruct.Pin = GPIO_PIN_2;                                           // GPIO引脚
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;                                // 下降沿触发
    GPIO_InitStruct.Pull = GPIO_PULLUP;                                         // 使用上拉
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);                                     // GPIO初始化

    HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);                                     // 设置中断优先级
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);                                             // 使能中断
}

/**
 * @brief EXTI2中断服务函数
 * 
 */
void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);                                       // 调用HAL库的EXTI公共中断处理函数
}

/**
 * @brief 重写HAL库的EXTI回调函数
 * 
 * @param GPIO_Pin EXTI的引脚
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_2)
    {
        HAL_Delay(20);                                                          // 按键消抖
        if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) == GPIO_PIN_RESET)              // 读取GPIO引脚电平，再次判断按键是否按下
        {
            printf("在中断中恢复 task1 任务\r\n");
            BaseType_t xYieldRequired = xTaskResumeFromISR(task1_handle);       // 在中断中恢复函数
            if (xYieldRequired == pdTRUE)
            {
                portYIELD_FROM_ISR(xYieldRequired);                             // 任务切换
            }
        }  
    }
}