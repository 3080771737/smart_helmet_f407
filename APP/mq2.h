/**
  ******************************************************************************
  * @file           : mq2.h
  * @brief          : MQ2烟雾传感器驱动头文件
  * @author         : STM32智能安全帽项目组
  * @date           : 2025-12-01
  ******************************************************************************
  * @attention
  *
  * MQ2是可燃气体和烟雾检测传感器
  * - 使用ADC1_IN0（PA0）采集模拟电压
  * - 检测范围：200-10000ppm
  * - 响应时间：<10秒
  * - 需要预热3分钟
  *
  ******************************************************************************
  */

#ifndef __MQ2_H
#define __MQ2_H

#include "main.h"
#include <stdbool.h>

/* ==================== 配置参数 ==================== */

#define MQ2_LOAD_RESISTANCE     4.7f    // 负载电阻(kΩ)
#define MQ2_R0_CLEAN_AIR        10.0f   // 清洁空气中的R0值(kΩ，需校准)
#define MQ2_ALARM_THRESHOLD     300.0f  // 报警阈值(ppm)

/* ==================== 数据结构 ==================== */

/**
 * @brief MQ2数据结构
 */
typedef struct {
    uint32_t adc_value;      // ADC原始值(0-4095)
    float voltage;           // 电压值(V)
    float Rs;                // 传感器电阻(kΩ)
    float ratio;             // Rs/R0比值
    float ppm;               // 烟雾浓度(ppm)
    bool alarm;              // 报警标志
} MQ2_Data_t;

/* ==================== 函数声明 ==================== */

/**
 * @brief 初始化MQ2传感器
 */
void MQ2_Init(void);

/**
 * @brief 校准MQ2传感器（在清洁空气中）
 * @retval R0值(kΩ)
 */
float MQ2_Calibrate(void);

/**
 * @brief 读取MQ2传感器数据
 * @param data: 数据结构指针
 */
void MQ2_Read_Data(MQ2_Data_t *data);

/**
 * @brief 计算烟雾浓度
 * @param Rs: 传感器电阻(kΩ)
 * @param R0: 清洁空气中的基准电阻(kΩ)
 * @retval 烟雾浓度(ppm)
 */
float MQ2_Calculate_PPM(float Rs, float R0);

/**
 * @brief MQ2任务函数（供调度器调用）
 */
void mq2_task(void);

/**
 * @brief 打印MQ2数据
 * @param data: 数据结构指针
 */
void MQ2_Print_Data(MQ2_Data_t *data);

#endif /* __MQ2_H */
