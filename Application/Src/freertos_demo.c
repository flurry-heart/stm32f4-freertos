#include "freertos_demo.h"

/**
 * START_TASK 任务配置
 * 包括: 任务优先级 任务栈大小 任务句柄 任务函数
 */
#define START_TASK_PRIORITY     1
#define START_TASK_STACK_SIZE   128

TaskHandle_t start_task_handle;

void start_task(void *pvParameters );

/**
 * TASK1 任务配置
 * 包括: 任务优先级 任务栈大小 任务句柄 任务函数
 */
#define TASK1_STACK_SIZE   128
#define TASK1_PRIORITY     2

TaskHandle_t task1_handle;

void task1(void *pvParameters);

/**
 * TASK2 任务配置
 * 包括: 任务优先级 任务栈大小 任务句柄 任务控制块  任务栈 任务函数
 */
#define TASK2_PRIORITY     3
#define TASK2_STACK_SIZE   128

TaskHandle_t task2_handle;
StaticTask_t task2_tcb;
StackType_t task2_stack[TASK2_STACK_SIZE];

void task2(void *pvParameters);

/**
 * TASK3 任务配置
 * 包括: 任务优先级 任务栈大小 任务句柄 任务函数
 */
#define TASK3_PRIORITY     4
#define TASK3_STACK_SIZE   128

TaskHandle_t task3_handle;

void task3(void *pvParameters);

/**
 * @brief FreeRTOS的入口函数
 * 
 */
void freertos_demo(void)
{
    xTaskCreate((TaskFunction_t        ) start_task,                            // 任务函数
                (char *                ) "start_task",                          // 任务名
                (configSTACK_DEPTH_TYPE) START_TASK_STACK_SIZE,                 // 任务栈大小
                (void *                ) NULL,                                  // 入口参数
                (UBaseType_t           ) START_TASK_PRIORITY,                   // 任务优先级
                (TaskHandle_t *        ) start_task_handle);                    // 任务句柄

    vTaskStartScheduler();                                                      // 开启任务调度器
}

/**
 * @brief 开始任务的任务函数
 * 
 * @param pvParameters 任务函数的入口参数
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();                                                       // 进入临界区，关闭中断

    xTaskCreate((TaskFunction_t        ) task1,                                 // 任务函数
                (char *                ) "task1",                               // 任务名
                (configSTACK_DEPTH_TYPE) TASK1_STACK_SIZE,                      // 任务栈大小
                (void *                ) NULL,                                  // 入口参数
                (UBaseType_t           ) TASK1_PRIORITY,                        // 任务优先级
                (TaskHandle_t *        ) &task1_handle);                        // 任务句柄

    xTaskCreate((TaskFunction_t        ) task2,                                 // 任务函数
                (char *                ) "task2",                               // 任务名
                (configSTACK_DEPTH_TYPE) TASK2_STACK_SIZE,                      // 任务栈大小
                (void *                ) NULL,                                  // 入口参数
                (UBaseType_t           ) TASK2_PRIORITY,                        // 任务优先级
                (TaskHandle_t *        ) &task2_handle);                        // 任务句柄

    xTaskCreate((TaskFunction_t        ) task3,                                 // 任务函数
                (char *                ) "task3",                               // 任务名
                (configSTACK_DEPTH_TYPE) TASK3_STACK_SIZE,                      // 任务栈大小
                (void *                ) NULL,                                  // 入口参数
                (UBaseType_t           ) TASK3_PRIORITY,                        // 任务优先级
                (TaskHandle_t *        ) &task3_handle);                        // 任务句柄

    taskEXIT_CRITICAL();                                                        // 退出临界区，重新开启中断

    vTaskDelete(NULL);                                                          // 删除任务自身
}

/**
 * @brief 任务1的任务函数
 * 
 * @param pvParameters 任务函数的入口参数
 */
void task1(void *pvParameters)
{
    while (1)
    {
        printf("task1 正在运行\r\n");
        HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
        vTaskDelay(500);
    }
}

/**
 * @brief 任务2的任务函数
 * 
 * @param pvParameters 任务函数的入口参数
 */
void task2(void *pvParameters )
{
    while (1)
    {
        printf("task2 正在运行\r\n");
        HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
        vTaskDelay(500);
    }
}

/**
 * @brief 任务3的任务函数
 * 
 * @param pvParameters 任务函数的入口参数
 */
void task3(void *pvParameters )
{
    while (1)
    {
        switch (Key_Scan(0))
        {
        case KEY1_PRESS:
            printf("挂起 task1 任务\r\n");
            vTaskSuspend(task1_handle);
            break;

        case KEY2_PRESS:
        {
            printf("恢复 task1 任务\r\n");
            vTaskResume(task1_handle);
            break;
        }
        
        default:
            break;
        }

        vTaskDelay(10);
    }
}
