/**
  ******************************************************************************
  * @file           : max30102.h
  * @brief          : MAX30102心率血氧传感器驱动头文件
  * @author         : STM32智能安全帽项目组
  * @date           : 2025-12-01
  ******************************************************************************
  * @attention
  *
  * MAX30102是基于PPG(光电容积脉搏波描记法)的心率血氧传感器
  * - 使用软件I2C通信（PB14-SDA, PB15-SCL）
  * - I2C地址：0xAE (7位地址0x57)
  * - 采样率：100Hz
  * - 分辨率：18位
  *
  * 核心算法：
  * - 三级滤波：硬件滤波 + 滑动平均 + 低通滤波
  * - 峰值检测：寻找心跳峰值
  * - 心率计算：60秒/峰值间隔
  * - 血氧计算：R值查表法
  *
  ******************************************************************************
  */

#ifndef __MAX30102_H
#define __MAX30102_H

#include "main.h"
#include <stdbool.h>

/* ==================== 寄存器地址定义 ==================== */

// 状态寄存器
#define MAX30102_INT_STATUS_1       0x00  // 中断状态1
#define MAX30102_INT_STATUS_2       0x01  // 中断状态2
#define MAX30102_INT_ENABLE_1       0x02  // 中断使能1
#define MAX30102_INT_ENABLE_2       0x03  // 中断使能2

// FIFO寄存器
#define MAX30102_FIFO_WR_PTR        0x04  // FIFO写指针
#define MAX30102_FIFO_OVF_CNT       0x05  // FIFO溢出计数
#define MAX30102_FIFO_RD_PTR        0x06  // FIFO读指针
#define MAX30102_FIFO_DATA          0x07  // FIFO数据寄存器

// 配置寄存器
#define MAX30102_FIFO_CONFIG        0x08  // FIFO配置
#define MAX30102_MODE_CONFIG        0x09  // 模式配置
#define MAX30102_SPO2_CONFIG        0x0A  // SpO2配置
#define MAX30102_LED1_PA            0x0C  // 红光LED电流
#define MAX30102_LED2_PA            0x0D  // 红外LED电流
#define MAX30102_PILOT_PA           0x10  // Pilot LED电流
#define MAX30102_MULTI_LED_CTRL1    0x11  // 多LED控制1
#define MAX30102_MULTI_LED_CTRL2    0x12  // 多LED控制2

// 温度寄存器
#define MAX30102_TEMP_INT           0x1F  // 温度整数部分
#define MAX30102_TEMP_FRAC          0x20  // 温度小数部分
#define MAX30102_TEMP_CONFIG        0x21  // 温度配置

// 器件ID
#define MAX30102_REV_ID             0xFE  // 修订版本ID
#define MAX30102_PART_ID            0xFF  // 器件ID (应为0x15)

/* ==================== 配置参数 ==================== */

// 模式配置
#define MAX30102_MODE_HR_ONLY       0x02  // 仅心率模式
#define MAX30102_MODE_SPO2          0x03  // 心率+血氧模式
#define MAX30102_MODE_MULTI_LED     0x07  // 多LED模式

// 采样率
#define MAX30102_SR_50HZ            0x00  // 50 Hz
#define MAX30102_SR_100HZ           0x01  // 100 Hz
#define MAX30102_SR_200HZ           0x02  // 200 Hz
#define MAX30102_SR_400HZ           0x03  // 400 Hz
#define MAX30102_SR_800HZ           0x04  // 800 Hz
#define MAX30102_SR_1000HZ          0x05  // 1000 Hz

// LED脉冲宽度（分辨率）
#define MAX30102_PW_15BIT_69US      0x00  // 69μs, 15位
#define MAX30102_PW_16BIT_118US     0x01  // 118μs, 16位
#define MAX30102_PW_17BIT_215US     0x02  // 215μs, 17位
#define MAX30102_PW_18BIT_411US     0x03  // 411μs, 18位

// ADC量程
#define MAX30102_ADC_RANGE_2048     0x00  // 2048 nA
#define MAX30102_ADC_RANGE_4096     0x01  // 4096 nA
#define MAX30102_ADC_RANGE_8192     0x02  // 8192 nA
#define MAX30102_ADC_RANGE_16384    0x03  // 16384 nA

/* ==================== 算法参数 ==================== */

#define MAX30102_BUFFER_SIZE        100   // FIFO缓冲区大小
#define MAX30102_MA_SIZE            20    // 滑动平均窗口大小
#define MAX30102_SAMPLE_RATE        100   // 采样率(Hz)

// 心率范围
#define MAX30102_HR_MIN             40    // 最小心率(bpm)
#define MAX30102_HR_MAX             200   // 最大心率(bpm)

// 血氧范围
#define MAX30102_SPO2_MIN           70    // 最小血氧(%)
#define MAX30102_SPO2_MAX           100   // 最大血氧(%)

// 报警阈值
#define MAX30102_HR_ALARM_LOW       50    // 心率过低报警
#define MAX30102_HR_ALARM_HIGH      120   // 心率过高报警
#define MAX30102_SPO2_ALARM_LOW     90    // 血氧过低报警

/* ==================== 数据结构 ==================== */

/**
 * @brief MAX30102数据结构
 */
typedef struct {
    uint32_t red_buffer[MAX30102_BUFFER_SIZE];   // 红光数据缓冲区
    uint32_t ir_buffer[MAX30102_BUFFER_SIZE];    // 红外数据缓冲区

    int32_t heart_rate;                          // 心率值(bpm)
    int32_t spo2;                                // 血氧饱和度(%)

    bool hr_valid;                               // 心率数据有效标志
    bool spo2_valid;                             // 血氧数据有效标志

    bool hr_alarm;                               // 心率报警标志
    bool spo2_alarm;                             // 血氧报警标志

    uint32_t buffer_index;                       // 缓冲区索引
    uint32_t sample_count;                       // 采样计数

} MAX30102_Data_t;

/* ==================== 函数声明 ==================== */

/**
 * @brief 初始化MAX30102传感器
 * @retval 0: 成功, 1: 失败
 */
uint8_t MAX30102_Init(void);

/**
 * @brief 复位MAX30102
 * @retval 0: 成功, 1: 失败
 */
uint8_t MAX30102_Reset(void);

/**
 * @brief 读取FIFO数据
 * @param red_led: 红光LED数据指针
 * @param ir_led: 红外LED数据指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t MAX30102_Read_FIFO(uint32_t *red_led, uint32_t *ir_led);

/**
 * @brief 读取多个FIFO样本
 * @param data: 数据结构指针
 * @param num_samples: 读取样本数
 * @retval 读取的实际样本数
 */
uint32_t MAX30102_Read_FIFO_Multi(MAX30102_Data_t *data, uint32_t num_samples);

/**
 * @brief 计算心率和血氧
 * @param data: 数据结构指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t MAX30102_Calculate(MAX30102_Data_t *data);

/**
 * @brief 获取传感器数据
 * @param data: 数据结构指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t MAX30102_Get_Data(MAX30102_Data_t *data);

/**
 * @brief MAX30102任务函数（供调度器调用）
 */
void max30102_task(void);

/**
 * @brief 检查心率血氧报警
 * @param data: 数据结构指针
 */
void MAX30102_Check_Alarm(MAX30102_Data_t *data);

/**
 * @brief 打印MAX30102数据
 * @param data: 数据结构指针
 */
void MAX30102_Print_Data(MAX30102_Data_t *data);

/* ==================== 底层I2C函数 ==================== */

/**
 * @brief 写MAX30102寄存器
 * @param reg_addr: 寄存器地址
 * @param data: 写入的数据
 * @retval 0: 成功, 1: 失败
 */
uint8_t MAX30102_Write_Reg(uint8_t reg_addr, uint8_t data);

/**
 * @brief 读MAX30102寄存器
 * @param reg_addr: 寄存器地址
 * @param data: 读取的数据指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t MAX30102_Read_Reg(uint8_t reg_addr, uint8_t *data);

/**
 * @brief 软件I2C初始化
 */
void MAX30102_I2C_Init(void);

/**
 * @brief 软件I2C起始信号
 */
void MAX30102_I2C_Start(void);

/**
 * @brief 软件I2C停止信号
 */
void MAX30102_I2C_Stop(void);

/**
 * @brief 软件I2C发送字节
 * @param byte: 要发送的字节
 * @retval 应答位 (0: ACK, 1: NACK)
 */
uint8_t MAX30102_I2C_Send_Byte(uint8_t byte);

/**
 * @brief 软件I2C接收字节
 * @param ack: 应答位 (0: ACK, 1: NACK)
 * @retval 接收到的字节
 */
uint8_t MAX30102_I2C_Recv_Byte(uint8_t ack);

#endif /* __MAX30102_H */
