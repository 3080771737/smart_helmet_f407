/**
  ******************************************************************************
  * @file           : scheduler.h
  * @brief          : 轻量级任务调度器头文件
  * @author         : STM32智能安全帽项目组
  * @date           : 2025-11-30
  ******************************************************************************
  */

#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef struct {
    void (*task_func)(void);  // 任务函数指针
    uint32_t period_ms;       // 执行周期（毫秒）
    uint32_t last_run;        // 上次运行时刻（毫秒）
} task_t;

/* Exported constants --------------------------------------------------------*/
#define MAX_TASKS 16  // 最大任务数量

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void scheduler_init(void);
void scheduler_run(void);
void scheduler_add_task(void (*func)(void), uint32_t period_ms);

#ifdef __cplusplus
}
#endif

#endif /* __SCHEDULER_H */
