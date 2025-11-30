/**
  ******************************************************************************
  * @file           : scheduler.c
  * @brief          : 轻量级任务调度器实现
  * @author         : STM32智能安全帽项目组
  * @date           : 2025-11-30
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "scheduler.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static task_t task_list[MAX_TASKS];  // 任务列表
static uint8_t task_count = 0;        // 当前任务数量

/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

/**
  * @brief  调度器初始化
  * @retval None
  */
void scheduler_init(void)
{
    task_count = 0;
    
    // 清空任务列表
    for (uint8_t i = 0; i < MAX_TASKS; i++) {
        task_list[i].task_func = NULL;
        task_list[i].period_ms = 0;
        task_list[i].last_run = 0;
    }
}

/**
  * @brief  添加任务到调度器
  * @param  func: 任务函数指针
  * @param  period_ms: 执行周期（毫秒）
  * @retval None
  */
void scheduler_add_task(void (*func)(void), uint32_t period_ms)
{
    if (task_count < MAX_TASKS && func != NULL) {
        task_list[task_count].task_func = func;
        task_list[task_count].period_ms = period_ms;
        task_list[task_count].last_run = 0;
        task_count++;
    }
}

/**
  * @brief  调度器运行（主循环中调用）
  * @retval None
  */
void scheduler_run(void)
{
    uint32_t current_time = HAL_GetTick();
    
    for (uint8_t i = 0; i < task_count; i++) {
        // 检查是否到达执行时刻
        if (current_time - task_list[i].last_run >= task_list[i].period_ms) {
            task_list[i].last_run = current_time;
            
            // 执行任务
            if (task_list[i].task_func != NULL) {
                task_list[i].task_func();
            }
        }
    }
}

/************************ (C) COPYRIGHT STM32智能安全帽项目组 *****END OF FILE****/
