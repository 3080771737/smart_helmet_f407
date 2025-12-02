/**
  ******************************************************************************
  * @file           : asr_pro.c
  * @brief          : ASR-PRO语音模块驱动实现（简化版）
  * @author         : STM32智能安全帽项目组
  * @date           : 2025-12-01
  ******************************************************************************
  */

#include "asr_pro.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief 初始化语音模块
 */
void ASR_Init(void)
{
    printf("ASR-PRO: Init Success\r\n");
}

/**
 * @brief 语音播报
 * @param text: 播报内容
 */
void ASR_Speak(char *text)
{
    // 发送播报指令到UART1（简化版）
    HAL_UART_Transmit(&huart1, (uint8_t *)text, strlen(text), 1000);
}

/**
 * @brief ASR任务函数（供调度器调用）
 */
void asr_task(void)
{
    // 语音模块任务（简化版，实际需要解析接收到的指令）
}
