/**
  ******************************************************************************
  * @file    aht20.h
  * @brief   AHT20 temperature and humidity sensor driver header
  * @author  Smart Helmet Project
  * @date    2025-11-29
  ******************************************************************************
  * @note    This driver replaces DHT11 for STM32F103VET6 board
  *          AHT20 is connected to I2C1 (PB6=SCL, PB7=SDA)
  *          I2C address: 0x38
  ******************************************************************************
  */

#ifndef __AHT20_H
#define __AHT20_H

#include "stm32f1xx_hal.h"

/* ==================== AHT20 I2C Address ==================== */
#define AHT20_ADDRESS           (0x38 << 1)  // 7-bit address 0x38, left shift for HAL

/* ==================== AHT20 Commands ==================== */
#define AHT20_CMD_INIT          0xBE    // Initialization command
#define AHT20_CMD_TRIGGER       0xAC    // Trigger measurement command
#define AHT20_CMD_SOFTRESET     0xBA    // Soft reset command

/* ==================== AHT20 Status Bits ==================== */
#define AHT20_STATUS_BUSY       0x80    // Busy bit (1 = busy)
#define AHT20_STATUS_CALIBRATED 0x08    // Calibration bit (1 = calibrated)

/* ==================== AHT20 Measurement Parameters ==================== */
#define AHT20_MEASUREMENT_DELAY 80      // Measurement time in ms
#define AHT20_POWERUP_DELAY     40      // Power-up time in ms
#define AHT20_RESET_DELAY       20      // Reset time in ms

/* ==================== AHT20 Specifications ==================== */
#define AHT20_TEMP_MIN          -40.0f  // Minimum temperature (°C)
#define AHT20_TEMP_MAX          85.0f   // Maximum temperature (°C)
#define AHT20_HUMIDITY_MIN      0.0f    // Minimum humidity (%)
#define AHT20_HUMIDITY_MAX      100.0f  // Maximum humidity (%)

#define AHT20_TEMP_ACCURACY     0.3f    // Temperature accuracy (±°C)
#define AHT20_HUMIDITY_ACCURACY 2.0f    // Humidity accuracy (±%)

/* ==================== Data Structures ==================== */

/**
 * @brief AHT20 raw data structure
 */
typedef struct {
    uint32_t humidity_raw;      // Raw humidity value (20-bit)
    uint32_t temperature_raw;   // Raw temperature value (20-bit)
    uint8_t status;             // Status byte
} AHT20_RawData_t;

/**
 * @brief AHT20 processed data structure
 */
typedef struct {
    float temperature;          // Temperature in °C
    float humidity;             // Relative humidity in %
    uint8_t is_valid;           // Data validity flag
} AHT20_Data_t;

/* ==================== Function Prototypes ==================== */

/**
 * @brief  Initialize AHT20 sensor
 * @param  hi2c: Pointer to I2C handle (e.g., &hi2c1)
 * @retval 0: Success, 1-5: Error codes
 */
uint8_t AHT20_Init(I2C_HandleTypeDef *hi2c);

/**
 * @brief  Soft reset AHT20 sensor
 * @param  hi2c: Pointer to I2C handle
 * @retval 0: Success, 1: Error
 */
uint8_t AHT20_SoftReset(I2C_HandleTypeDef *hi2c);

/**
 * @brief  Read status register
 * @param  hi2c: Pointer to I2C handle
 * @param  status: Pointer to store status byte
 * @retval 0: Success, 1: Error
 */
uint8_t AHT20_ReadStatus(I2C_HandleTypeDef *hi2c, uint8_t *status);

/**
 * @brief  Check if sensor is busy
 * @param  hi2c: Pointer to I2C handle
 * @retval 1: Busy, 0: Ready
 */
uint8_t AHT20_IsBusy(I2C_HandleTypeDef *hi2c);

/**
 * @brief  Check if sensor is calibrated
 * @param  hi2c: Pointer to I2C handle
 * @retval 1: Calibrated, 0: Not calibrated
 */
uint8_t AHT20_IsCalibrated(I2C_HandleTypeDef *hi2c);

/**
 * @brief  Trigger measurement
 * @param  hi2c: Pointer to I2C handle
 * @retval 0: Success, 1: Error
 */
uint8_t AHT20_TriggerMeasurement(I2C_HandleTypeDef *hi2c);

/**
 * @brief  Read raw data from AHT20
 * @param  hi2c: Pointer to I2C handle
 * @param  raw_data: Pointer to raw data structure
 * @retval 0: Success, 1-3: Error codes
 */
uint8_t AHT20_ReadRawData(I2C_HandleTypeDef *hi2c, AHT20_RawData_t *raw_data);

/**
 * @brief  Process raw data to physical units
 * @param  raw_data: Pointer to raw data
 * @param  data: Pointer to processed data structure
 * @retval None
 */
void AHT20_ProcessData(AHT20_RawData_t *raw_data, AHT20_Data_t *data);

/**
 * @brief  Read temperature and humidity (one-shot measurement)
 * @param  hi2c: Pointer to I2C handle
 * @param  data: Pointer to data structure
 * @retval 0: Success, 1-5: Error codes
 */
uint8_t AHT20_ReadData(I2C_HandleTypeDef *hi2c, AHT20_Data_t *data);

/**
 * @brief  Task function for scheduler (call every 1000ms)
 * @param  None
 * @retval None
 */
void aht20_task(void);

/* ==================== Global Variables (extern) ==================== */
extern I2C_HandleTypeDef hi2c1;         // I2C handle defined in main.c
extern AHT20_Data_t aht20_data;         // Global sensor data

#endif /* __AHT20_H */

/**
 * @brief  Usage Example:
 *
 * // In main.c
 * AHT20_Data_t aht20_data;
 *
 * int main(void) {
 *     // ... HAL Init, GPIO, I2C Init ...
 *
 *     // Initialize AHT20
 *     if(AHT20_Init(&hi2c1) != 0) {
 *         // Error handling
 *     }
 *
 *     // In scheduler (call every 1000ms)
 *     while(1) {
 *         aht20_task();
 *
 *         printf("Temperature: %.1f°C, Humidity: %.1f%%\r\n",
 *                aht20_data.temperature, aht20_data.humidity);
 *
 *         HAL_Delay(1000);
 *     }
 * }
 *
 * @note  AHT20 advantages over DHT11:
 *        - Higher accuracy: ±0.3°C vs ±2°C, ±2%RH vs ±5%RH
 *        - Wider range: -40~85°C vs 0~50°C, 0~100%RH vs 20~90%RH
 *        - I2C interface: More reliable than DHT11's single-wire protocol
 *        - Faster response: 80ms vs 2000ms
 *        - No need for precise microsecond timing
 */
