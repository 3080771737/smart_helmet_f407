/**
  ******************************************************************************
  * @file           : atgm336h.h
  * @brief          : ATGM336H GPS模块驱动头文件
  * @author         : STM32智能安全帽项目组
  * @date           : 2025-12-01
  ******************************************************************************
  * @attention
  *
  * ATGM336H是GPS+北斗双模定位模块
  * - 使用UART2通信（PA2-TX, PA3-RX, 9600波特率）
  * - 输出NMEA-0183协议数据
  * - 定位精度：3-5米
  * - 冷启动时间：约35秒
  *
  ******************************************************************************
  */

#ifndef __ATGM336H_H
#define __ATGM336H_H

#include "main.h"
#include <stdbool.h>

/* ==================== 配置参数 ==================== */

#define GPS_UART_BUFFER_SIZE    256     // UART接收缓冲区大小
#define GPS_SENTENCE_MAX_LEN    128     // NMEA语句最大长度

/* ==================== 数据结构 ==================== */

/**
 * @brief GPS数据结构
 */
typedef struct {
    float latitude;      // 纬度（度）
    char lat_dir;        // 纬度方向（N/S）
    float longitude;     // 经度（度）
    char lon_dir;        // 经度方向（E/W）

    uint8_t hour;        // 时（UTC）
    uint8_t minute;      // 分
    uint8_t second;      // 秒

    uint8_t satellites;  // 可见卫星数
    float hdop;          // 水平精度因子
    float altitude;      // 海拔高度（米）

    bool fix_valid;      // 定位有效标志
    char fix_status;     // 定位状态（A=有效, V=无效）

} GPS_Data_t;

/* ==================== 函数声明 ==================== */

/**
 * @brief 初始化GPS模块
 */
void GPS_Init(void);

/**
 * @brief 解析NMEA语句
 * @param sentence: NMEA语句字符串
 * @param data: GPS数据结构指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t GPS_Parse_NMEA(char *sentence, GPS_Data_t *data);

/**
 * @brief 解析$GNRMC语句
 * @param sentence: NMEA语句字符串
 * @param data: GPS数据结构指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t GPS_Parse_GNRMC(char *sentence, GPS_Data_t *data);

/**
 * @brief 解析$GNGGA语句
 * @param sentence: NMEA语句字符串
 * @param data: GPS数据结构指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t GPS_Parse_GNGGA(char *sentence, GPS_Data_t *data);

/**
 * @brief NMEA坐标转换为十进制度数
 * @param nmea_coord: NMEA坐标（ddmm.mmmm格式）
 * @retval 十进制度数
 */
float GPS_NMEA_To_Decimal(float nmea_coord);

/**
 * @brief GPS任务函数（供调度器调用）
 */
void gps_task(void);

/**
 * @brief 打印GPS数据
 * @param data: GPS数据结构指针
 */
void GPS_Print_Data(GPS_Data_t *data);

/**
 * @brief UART接收回调函数
 * @param huart: UART句柄
 */
void GPS_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* __ATGM336H_H */
