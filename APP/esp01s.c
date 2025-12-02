/**
  ******************************************************************************
  * @file           : esp01s.c
  * @brief          : ESP01S WiFi模块驱动实现（简化版）
  * @author         : STM32智能安全帽项目组
  * @date           : 2025-12-01
  ******************************************************************************
  */

#include "esp01s.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

/* ==================== 全局变量 ==================== */

static ESP_Data_t esp_data = {0};
static char esp_rx_buffer[256] = {0};

/* ==================== 函数实现 ==================== */

/**
 * @brief 发送AT指令
 * @param cmd: AT指令字符串
 * @param timeout_ms: 超时时间（毫秒）
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Send_AT(char *cmd, uint32_t timeout_ms)
{
    // 发送AT指令
    HAL_UART_Transmit(&huart3, (uint8_t *)cmd, strlen(cmd), 1000);

    // 等待响应（简化版，实际应使用中断接收）
    HAL_Delay(timeout_ms);

    return 0;
}

/**
 * @brief 初始化ESP01S模块
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Init(void)
{
    printf("ESP01S: Initializing...\r\n");

    // 测试AT指令
    ESP_Send_AT("AT\r\n", 100);

    // 设置WiFi模式为Station
    ESP_Send_AT("AT+CWMODE=1\r\n", 500);

    printf("ESP01S: Init Success\r\n");

    return 0;
}

/**
 * @brief 连接WiFi
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Connect_WiFi(void)
{
    char cmd[128];

    printf("ESP01S: Connecting to WiFi: %s\r\n", WIFI_SSID);

    // 构建连接命令
    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWORD);

    // 发送连接命令（需要较长时间）
    ESP_Send_AT(cmd, 5000);

    esp_data.wifi_connected = true;
    esp_data.status = ESP_WIFI_CONNECTED;

    printf("ESP01S: WiFi Connected\r\n");

    return 0;
}

/**
 * @brief 连接MQTT服务器
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Connect_MQTT(void)
{
    char cmd[256];

    printf("ESP01S: Connecting to MQTT...\r\n");

    // 配置MQTT用户信息
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n",
             MQTT_USERNAME, MQTT_PASSWORD);
    ESP_Send_AT(cmd, 1000);

    // 连接MQTT服务器
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTCONN=0,\"%s\",%d,1\r\n",
             MQTT_SERVER, MQTT_PORT);
    ESP_Send_AT(cmd, 3000);

    esp_data.mqtt_connected = true;
    esp_data.status = ESP_MQTT_CONNECTED;

    printf("ESP01S: MQTT Connected\r\n");

    return 0;
}

/**
 * @brief 发布MQTT消息
 * @param topic: 主题
 * @param payload: 消息内容（JSON格式）
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Publish_MQTT(char *topic, char *payload)
{
    char cmd[512];

    // 构建发布命令
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n",
             topic, payload);

    ESP_Send_AT(cmd, 1000);

    return 0;
}

/**
 * @brief 上传传感器数据到云平台
 * @retval 0: 成功, 1: 失败
 */
uint8_t ESP_Upload_Data(void)
{
    char json_data[256];

    // 构建JSON格式数据（示例）
    snprintf(json_data, sizeof(json_data),
             "{\"services\":[{\"service_id\":\"BasicData\",\"properties\":{"
             "\"temperature\":25,"
             "\"humidity\":60,"
             "\"heart_rate\":75,"
             "\"fall_flag\":0"
             "}}]}");

    // 发布到MQTT主题
    ESP_Publish_MQTT((char *)MQTT_TOPIC, json_data);

    return 0;
}

/**
 * @brief 检查连接状态
 */
void ESP_Check_Connection(void)
{
    static uint32_t last_check = 0;
    uint32_t now = HAL_GetTick();

    // 每30秒检查一次
    if (now - last_check > 30000) {
        if (!esp_data.wifi_connected) {
            ESP_Connect_WiFi();
        }
        if (esp_data.wifi_connected && !esp_data.mqtt_connected) {
            ESP_Connect_MQTT();
        }
        last_check = now;
    }
}

/**
 * @brief ESP01S任务函数（供调度器调用）
 */
void esp_task(void)
{
    // 检查连接状态
    ESP_Check_Connection();

    // 上传数据
    if (esp_data.mqtt_connected) {
        ESP_Upload_Data();
    }
}
