/**
  ******************************************************************************
  * @file           : atgm336h.c
  * @brief          : ATGM336H GPS模块驱动实现
  * @author         : STM32智能安全帽项目组
  * @date           : 2025-12-01
  ******************************************************************************
  */

#include "atgm336h.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ==================== 全局变量 ==================== */

static GPS_Data_t gps_data = {0};
static char gps_rx_buffer[GPS_UART_BUFFER_SIZE] = {0};
static char gps_sentence[GPS_SENTENCE_MAX_LEN] = {0};
static uint16_t gps_rx_index = 0;
static bool sentence_ready = false;

/* ==================== 函数实现 ==================== */

/**
 * @brief 初始化GPS模块
 */
void GPS_Init(void)
{
    // 启动UART2中断接收
    HAL_UART_Receive_IT(&huart2, (uint8_t *)gps_rx_buffer, 1);

    printf("GPS: Init Success\r\n");
    printf("GPS: Waiting for fix (may take 1-3 minutes outdoors)...\r\n");
}

/**
 * @brief NMEA坐标转换为十进制度数
 * @param nmea_coord: NMEA坐标（ddmm.mmmm或dddmm.mmmm格式）
 * @retval 十进制度数
 */
float GPS_NMEA_To_Decimal(float nmea_coord)
{
    int degrees;
    float minutes;

    // 提取度数（整数部分的前2或3位）
    if (nmea_coord >= 10000.0f) {
        // 经度（dddmm.mmmm）
        degrees = (int)(nmea_coord / 100.0f);
    } else {
        // 纬度（ddmm.mmmm）
        degrees = (int)(nmea_coord / 100.0f);
    }

    // 提取分钟
    minutes = nmea_coord - (degrees * 100.0f);

    // 转换为十进制度数
    return (float)degrees + (minutes / 60.0f);
}

/**
 * @brief 解析$GNRMC语句
 * @param sentence: NMEA语句字符串
 * @param data: GPS数据结构指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t GPS_Parse_GNRMC(char *sentence, GPS_Data_t *data)
{
    char *token;
    int field = 0;
    char time_str[16] = {0};
    char lat_str[16] = {0};
    char lon_str[16] = {0};
    char status_str[2] = {0};

    // $GNRMC,073040.00,A,3954.52200,N,11628.85100,E,0.012,,101125,,,A*7E
    // 字段：0=RMC, 1=时间, 2=状态, 3=纬度, 4=纬度方向, 5=经度, 6=经度方向...

    token = strtok(sentence, ",");

    while (token != NULL) {
        switch (field) {
            case 1:  // 时间 (hhmmss.ss)
                strncpy(time_str, token, sizeof(time_str) - 1);
                if (strlen(time_str) >= 6) {
                    data->hour = (time_str[0] - '0') * 10 + (time_str[1] - '0');
                    data->minute = (time_str[2] - '0') * 10 + (time_str[3] - '0');
                    data->second = (time_str[4] - '0') * 10 + (time_str[5] - '0');
                }
                break;

            case 2:  // 状态 (A=有效, V=无效)
                strncpy(status_str, token, sizeof(status_str) - 1);
                data->fix_status = status_str[0];
                data->fix_valid = (status_str[0] == 'A');
                break;

            case 3:  // 纬度 (ddmm.mmmm)
                if (strlen(token) > 0) {
                    strncpy(lat_str, token, sizeof(lat_str) - 1);
                    float nmea_lat = atof(lat_str);
                    data->latitude = GPS_NMEA_To_Decimal(nmea_lat);
                }
                break;

            case 4:  // 纬度方向 (N/S)
                data->lat_dir = token[0];
                break;

            case 5:  // 经度 (dddmm.mmmm)
                if (strlen(token) > 0) {
                    strncpy(lon_str, token, sizeof(lon_str) - 1);
                    float nmea_lon = atof(lon_str);
                    data->longitude = GPS_NMEA_To_Decimal(nmea_lon);
                }
                break;

            case 6:  // 经度方向 (E/W)
                data->lon_dir = token[0];
                break;
        }

        token = strtok(NULL, ",");
        field++;
    }

    return 0;
}

/**
 * @brief 解析$GNGGA语句
 * @param sentence: NMEA语句字符串
 * @param data: GPS数据结构指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t GPS_Parse_GNGGA(char *sentence, GPS_Data_t *data)
{
    char *token;
    int field = 0;

    // $GNGGA,073040.00,3954.52200,N,11628.85100,E,1,12,0.99,48.0,M,-5.0,M,,*7A
    // 字段：0=GGA, 1=时间, 2=纬度, 3=纬度方向, 4=经度, 5=经度方向,
    //       6=定位质量, 7=卫星数, 8=HDOP, 9=海拔...

    token = strtok(sentence, ",");

    while (token != NULL) {
        switch (field) {
            case 7:  // 卫星数
                data->satellites = (uint8_t)atoi(token);
                break;

            case 8:  // HDOP（水平精度因子）
                data->hdop = atof(token);
                break;

            case 9:  // 海拔高度（米）
                data->altitude = atof(token);
                break;
        }

        token = strtok(NULL, ",");
        field++;
    }

    return 0;
}

/**
 * @brief 解析NMEA语句
 * @param sentence: NMEA语句字符串
 * @param data: GPS数据结构指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t GPS_Parse_NMEA(char *sentence, GPS_Data_t *data)
{
    if (strstr(sentence, "$GNRMC") != NULL) {
        return GPS_Parse_GNRMC(sentence, data);
    } else if (strstr(sentence, "$GNGGA") != NULL) {
        return GPS_Parse_GNGGA(sentence, data);
    }

    return 1;  // 不支持的语句类型
}

/**
 * @brief 打印GPS数据
 * @param data: GPS数据结构指针
 */
void GPS_Print_Data(GPS_Data_t *data)
{
    if (data->fix_valid) {
        printf("GPS: Lat=%.6f%c, Lon=%.6f%c, Sats=%d\r\n",
               data->latitude, data->lat_dir,
               data->longitude, data->lon_dir,
               data->satellites);
    } else {
        printf("GPS: No Fix (Status: %c)\r\n", data->fix_status);
    }
}

/**
 * @brief GPS任务函数（供调度器调用）
 */
void gps_task(void)
{
    if (sentence_ready) {
        GPS_Parse_NMEA(gps_sentence, &gps_data);
        GPS_Print_Data(&gps_data);
        sentence_ready = false;
    }
}

/**
 * @brief UART接收回调函数
 * @param huart: UART句柄
 */
void GPS_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        char received = gps_rx_buffer[0];

        // 检测到$符号，开始新语句
        if (received == '$') {
            gps_rx_index = 0;
            gps_sentence[gps_rx_index++] = received;
        }
        // 检测到换行符，语句结束
        else if (received == '\n') {
            gps_sentence[gps_rx_index] = '\0';
            sentence_ready = true;
        }
        // 正常字符
        else if (gps_rx_index < GPS_SENTENCE_MAX_LEN - 1) {
            gps_sentence[gps_rx_index++] = received;
        }

        // 继续接收下一个字节
        HAL_UART_Receive_IT(&huart2, (uint8_t *)gps_rx_buffer, 1);
    }
}
