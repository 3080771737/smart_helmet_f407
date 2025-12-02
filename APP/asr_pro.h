/**
  ******************************************************************************
  * @file           : asr_pro.h
  * @brief          : ASR-PRO语音模块驱动头文件（简化版）
  * @author         : STM32智能安全帽项目组
  * @date           : 2025-12-01
  ******************************************************************************
  */

#ifndef __ASR_PRO_H
#define __ASR_PRO_H

#include "main.h"

/* ==================== 语音指令定义 ==================== */

#define ASR_CMD_OPEN    0x01    // 打开设备
#define ASR_CMD_CLOSE   0x02    // 关闭设备
#define ASR_CMD_ALARM   0x03    // 报警

/* ==================== 函数声明 ==================== */

/**
 * @brief 初始化语音模块
 */
void ASR_Init(void);

/**
 * @brief 语音播报
 * @param text: 播报内容
 */
void ASR_Speak(char *text);

/**
 * @brief ASR任务函数（供调度器调用）
 */
void asr_task(void);

#endif /* __ASR_PRO_H */
