#include "systick.h"

uint16_t g_frequency_us = 0;                                                    // us延时倍乘数

/**
 * @brief 延迟初始化函数
 * 
 */
void Delay_Init(void)
{
    uint32_t reload = 0;

    SysTick->CTRL = 0;                                                          // SysTick控制寄存器清零

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);                        // 设置SysTick时钟源为HCLK
    g_frequency_us = SystemCoreClock / 1000000;                                 // 1us定时的计数频率

    // 设置溢出时间为1ms
    reload = SystemCoreClock / configTICK_RATE_HZ;

    SysTick->CTRL |= 1 << 1;                                                    // 开启SysTick中断
    SysTick->LOAD = reload;                                                     // 设置计数器的值
    SysTick->CTRL |= 1 << 0;                                                    // 开启计数器
}

/**
 * @brief 微秒级延迟函数
 * 
 * @param time 要延迟的微秒数
 */
void Delay_us(uint32_t time)
{
    uint32_t tick = 0;                                                         
    uint32_t t_old = 0, t_now = 0, t_count = 0;
    uint32_t reload = SysTick->LOAD;                                            // LOAD的值

    tick = time * g_frequency_us;                                               // 延迟时间加载
    t_old = SysTick->VAL;                                                       

    while (1)
    {
        t_now = SysTick->VAL;                                                   // tnow用于记录当前的SysTick->VAL 值
        if (t_now < t_old)                                                      
        {
            t_count += t_old - t_now;                                           // 在一轮内，t_count加等于t_old到t_now的差值
        }
        else                                                                    // SysTick是向下计数的定时器，当VAL值大于t_old时，表示已经到一轮了
        {
            t_count += reload - t_now + t_old;                                  // 超过一轮内，t_count加等于重装值减t_now到t_old的差值，即VAL-(t_now-t_old)
        }
        t_old = t_now;                                                          // t_old用于记录最近一次的SysTick->VAL值
        if (t_count >= tick)                                                    // 时间超过或等于要延迟的时间，则定时时间到，退出
        {
            break;
        }
    }
}

/**
 * @brief 毫秒级延迟函数
 * 
 * @param time 要延迟的毫秒数
 */
void Delay_ms(uint32_t time)
{
    for (uint32_t i = 0; i < time; i++)
    {
        Delay_us(1000);
    }
}

/**
 * @brief 重写HAL的延迟函数
 * 
 * @param time 要延迟的毫秒数
 */
void HAL_Delay(uint32_t time)
{
    Delay_ms(time);
}

/**
 * @brief Systick中断服务函数
 * 
 */
void SysTick_Handler(void)
{
    HAL_IncTick();

    // OS开始跑了，才执行正常的调度处理
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}