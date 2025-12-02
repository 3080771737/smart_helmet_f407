/**
  ******************************************************************************
  * @file           : esp01s.h
  * @brief          : ESP01S WiFi模块驱动头文件（简化版）
  * @author         : STM32智能安全帽项目组
  * @date           : 2025-12-01
  ******************************************************************************
  * @attention
  *
  * ESP01S是基于ESP8266的WiFi模块
  * - 使用UART3通信（PB10-TX, PB11-RX, 115200波特率）
  * - AT指令控制
  * - 支持MQTT协议
  *
  * ⚠️ 供电要求：
  * - 必须使用外部3.3V稳压模块（AMS1117-3.3）
  * - 峰值电流300mA，STM32引脚无法提供
  *
  ******************************************************************************
  */

#ifndef __ESP01S_H
#define __ESP01S_H

#include "main.h"
#include <stdbool.h>

/* ==================== 配置参数（需要用户修改）==================== */

// WiFi配置
#define WIFI_SSID           "YOUR_WIFI_SSID"      // WiFi名称
#define WIFI_PASSWORD       "YOUR_WIFI_PASSWORD"  // WiFi密码

// MQTT配置（华为云IoT）
#define MQTT_SERVER         "your-server.iot-mqtts.cn-north-4.myhuaweicloud.com"
#define MQTT_PORT           1883
#define MQTT_CLIENT_ID      "ProductID_NodeId_0_0_2025031607"
#define MQTT_USERNAME       "your_device_id"
#define MQTT_PASSWORD       "your_device_secret"
#define MQTT_TOPIC          "$oc/devices/your_device_id/sys/properties/report"

/* ==================== 数据结构 ==================== */

/**
 * @brief ESP01S状态
 */
typedef enum {
    ESP_IDLE = 0,
    ESP_WIFI_CONNECTING,
    ESP_WIFI_CONNECTED,
    ESP_MQTT_CONNECTING,
    ESP_MQTT_CONNECTED,
    ESP_ERROR
} ESP_Status_t;

/**
 * @brief ESP01S数据结构
 */
typedef struct {
    ESP_Status_t status;
    bool wifi_connected;
    bool mqtt_connected;
    uint32_t reconnect_count;
} ESP_Data_t;

/* ==================== 函数声明 ==================== */

/**
 * @brief 初始化ESP01S模块
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Init(void);

/**
 * @brief 连接WiFi
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Connect_WiFi(void);

/**
 * @brief 连接MQTT服务器
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Connect_MQTT(void);

/**
 * @brief 发送AT指令
 * @param cmd: AT指令字符串
 * @param timeout_ms: 超时时间（毫秒）
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Send_AT(char *cmd, uint32_t timeout_ms);

/**
 * @brief 发布MQTT消息
 * @param topic: 主题
 * @param payload: 消息内容（JSON格式）
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Publish_MQTT(char *topic, char *payload);

/**
 * @brief 上传传感器数据到云平台
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Upload_Data(void);

/**
 * @brief ESP01S任务函数（供调度器调用）
 */
void esp_task(void);

/**
 * @brief 检查连接状态
 */
void ESP_Check_Connection(void);

#endif /* __ESP01S_H */
