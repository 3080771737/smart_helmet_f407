/**
  ******************************************************************************
  * @file    icm20608.h
  * @brief   ICM-20608-G 6-axis gyroscope and accelerometer driver header
  * @author  Smart Helmet Project
  * @date    2025-11-29
  ******************************************************************************
  * @note    This driver replaces MPU6050 for STM32F103VET6 board
  *          ICM-20608-G is connected to I2C1 (PB6=SCL, PB7=SDA)
  *          Interrupt pin: PA15
  ******************************************************************************
  */

#ifndef __ICM20608_H
#define __ICM20608_H

#include "stm32f1xx_hal.h"
#include <math.h>

/* ==================== ICM-20608-G Register Addresses ==================== */
// WHO_AM_I register
#define ICM20608_WHO_AM_I           0x75    // Device ID register (should return 0xAF)

// Power Management
#define ICM20608_PWR_MGMT_1         0x6B    // Power Management 1
#define ICM20608_PWR_MGMT_2         0x6C    // Power Management 2

// Configuration
#define ICM20608_CONFIG             0x1A    // Configuration
#define ICM20608_GYRO_CONFIG        0x1B    // Gyroscope Configuration
#define ICM20608_ACCEL_CONFIG       0x1C    // Accelerometer Configuration
#define ICM20608_ACCEL_CONFIG2      0x1D    // Accelerometer Configuration 2

// Interrupt
#define ICM20608_INT_PIN_CFG        0x37    // INT Pin Config
#define ICM20608_INT_ENABLE         0x38    // Interrupt Enable
#define ICM20608_INT_STATUS         0x3A    // Interrupt Status

// Accelerometer Measurements
#define ICM20608_ACCEL_XOUT_H       0x3B    // Accel X-axis High Byte
#define ICM20608_ACCEL_XOUT_L       0x3C    // Accel X-axis Low Byte
#define ICM20608_ACCEL_YOUT_H       0x3D    // Accel Y-axis High Byte
#define ICM20608_ACCEL_YOUT_L       0x3E    // Accel Y-axis Low Byte
#define ICM20608_ACCEL_ZOUT_H       0x3F    // Accel Z-axis High Byte
#define ICM20608_ACCEL_ZOUT_L       0x40    // Accel Z-axis Low Byte

// Temperature Measurement
#define ICM20608_TEMP_OUT_H         0x41    // Temperature High Byte
#define ICM20608_TEMP_OUT_L         0x42    // Temperature Low Byte

// Gyroscope Measurements
#define ICM20608_GYRO_XOUT_H        0x43    // Gyro X-axis High Byte
#define ICM20608_GYRO_XOUT_L        0x44    // Gyro X-axis Low Byte
#define ICM20608_GYRO_YOUT_H        0x45    // Gyro Y-axis High Byte
#define ICM20608_GYRO_YOUT_L        0x46    // Gyro Y-axis Low Byte
#define ICM20608_GYRO_ZOUT_H        0x47    // Gyro Z-axis High Byte
#define ICM20608_GYRO_ZOUT_L        0x48    // Gyro Z-axis Low Byte

// Signal Path Reset
#define ICM20608_SIGNAL_PATH_RESET  0x68    // Signal Path Reset

/* ==================== I2C Address ==================== */
// AD0 pin determines the I2C address
#define ICM20608_ADDRESS_AD0_LOW    0xD0    // AD0=0: I2C address 0x68 (7-bit) -> 0xD0 (8-bit)
#define ICM20608_ADDRESS_AD0_HIGH   0xD2    // AD0=1: I2C address 0x69 (7-bit) -> 0xD2 (8-bit)

// Default address (AD0=GND on board)
#define ICM20608_ADDRESS            ICM20608_ADDRESS_AD0_LOW

/* ==================== Configuration Values ==================== */
// Gyroscope Full Scale Range
#define ICM20608_GYRO_FS_250        0x00    // ±250 °/s
#define ICM20608_GYRO_FS_500        0x08    // ±500 °/s
#define ICM20608_GYRO_FS_1000       0x10    // ±1000 °/s
#define ICM20608_GYRO_FS_2000       0x18    // ±2000 °/s

// Accelerometer Full Scale Range
#define ICM20608_ACCEL_FS_2G        0x00    // ±2g
#define ICM20608_ACCEL_FS_4G        0x08    // ±4g
#define ICM20608_ACCEL_FS_8G        0x10    // ±8g
#define ICM20608_ACCEL_FS_16G       0x18    // ±16g

// Default full scale ranges
#define ICM20608_DEFAULT_GYRO_FS    ICM20608_GYRO_FS_2000    // ±2000 °/s
#define ICM20608_DEFAULT_ACCEL_FS   ICM20608_ACCEL_FS_16G    // ±16g

/* ==================== Sensitivity Scale Factors ==================== */
// Gyroscope LSB/(°/s)
#define ICM20608_GYRO_SENSITIVITY_250DPS    131.0f
#define ICM20608_GYRO_SENSITIVITY_500DPS    65.5f
#define ICM20608_GYRO_SENSITIVITY_1000DPS   32.8f
#define ICM20608_GYRO_SENSITIVITY_2000DPS   16.4f

// Accelerometer LSB/g
#define ICM20608_ACCEL_SENSITIVITY_2G       16384.0f
#define ICM20608_ACCEL_SENSITIVITY_4G       8192.0f
#define ICM20608_ACCEL_SENSITIVITY_8G       4096.0f
#define ICM20608_ACCEL_SENSITIVITY_16G      2048.0f

// Default sensitivities
#define ICM20608_DEFAULT_GYRO_SENS  ICM20608_GYRO_SENSITIVITY_2000DPS
#define ICM20608_DEFAULT_ACCEL_SENS ICM20608_ACCEL_SENSITIVITY_16G

/* ==================== Data Structures ==================== */
/**
 * @brief ICM-20608-G raw data structure
 */
typedef struct {
    int16_t accel_x_raw;    // Raw accelerometer X-axis
    int16_t accel_y_raw;    // Raw accelerometer Y-axis
    int16_t accel_z_raw;    // Raw accelerometer Z-axis
    int16_t temp_raw;       // Raw temperature
    int16_t gyro_x_raw;     // Raw gyroscope X-axis
    int16_t gyro_y_raw;     // Raw gyroscope Y-axis
    int16_t gyro_z_raw;     // Raw gyroscope Z-axis
} ICM20608_RawData_t;

/**
 * @brief ICM-20608-G processed data structure
 */
typedef struct {
    float accel_x;          // Accelerometer X-axis (g)
    float accel_y;          // Accelerometer Y-axis (g)
    float accel_z;          // Accelerometer Z-axis (g)
    float temperature;      // Temperature (°C)
    float gyro_x;           // Gyroscope X-axis (°/s)
    float gyro_y;           // Gyroscope Y-axis (°/s)
    float gyro_z;           // Gyroscope Z-axis (°/s)
    float pitch;            // Pitch angle (°)
    float roll;             // Roll angle (°)
    float yaw;              // Yaw angle (°) - approximate
} ICM20608_Data_t;

/**
 * @brief Complementary filter parameters
 */
typedef struct {
    float alpha;            // Filter coefficient (0.0 - 1.0)
    float pitch;            // Filtered pitch angle
    float roll;             // Filtered roll angle
    uint32_t last_update;   // Last update timestamp (ms)
} ICM20608_Filter_t;

/* ==================== Function Prototypes ==================== */

/**
 * @brief  Initialize ICM-20608-G sensor
 * @param  hi2c: Pointer to I2C handle (e.g., &hi2c1)
 * @retval 0: Success, 1-10: Error codes
 */
uint8_t ICM20608_Init(I2C_HandleTypeDef *hi2c);

/**
 * @brief  Read WHO_AM_I register to verify device
 * @param  hi2c: Pointer to I2C handle
 * @retval Device ID (should be 0xAF)
 */
uint8_t ICM20608_ReadID(I2C_HandleTypeDef *hi2c);

/**
 * @brief  Read raw data from ICM-20608-G
 * @param  hi2c: Pointer to I2C handle
 * @param  raw_data: Pointer to raw data structure
 * @retval 0: Success, 1: Error
 */
uint8_t ICM20608_ReadRawData(I2C_HandleTypeDef *hi2c, ICM20608_RawData_t *raw_data);

/**
 * @brief  Process raw data to physical units
 * @param  raw_data: Pointer to raw data
 * @param  data: Pointer to processed data structure
 * @retval None
 */
void ICM20608_ProcessData(ICM20608_RawData_t *raw_data, ICM20608_Data_t *data);

/**
 * @brief  Calculate attitude angles using complementary filter
 * @param  data: Pointer to processed data
 * @param  filter: Pointer to filter structure
 * @retval None
 */
void ICM20608_CalculateAttitude(ICM20608_Data_t *data, ICM20608_Filter_t *filter);

/**
 * @brief  Detect fall event based on pitch and roll angles
 * @param  data: Pointer to processed data
 * @param  threshold: Angle threshold in degrees (e.g., 60°)
 * @retval 1: Fall detected, 0: Normal
 */
uint8_t ICM20608_DetectFall(ICM20608_Data_t *data, float threshold);

/**
 * @brief  Enable data ready interrupt
 * @param  hi2c: Pointer to I2C handle
 * @retval 0: Success, 1: Error
 */
uint8_t ICM20608_EnableInterrupt(I2C_HandleTypeDef *hi2c);

/**
 * @brief  Task function for scheduler (call every 100ms)
 * @param  None
 * @retval None
 */
void icm20608_task(void);

/* ==================== Global Variables (extern) ==================== */
extern I2C_HandleTypeDef hi2c1;             // I2C handle defined in main.c
extern ICM20608_Data_t icm_data;            // Global sensor data
extern ICM20608_Filter_t icm_filter;        // Global filter structure
extern uint8_t fall_flag;                   // Fall detection flag

#endif /* __ICM20608_H */

/**
 * @brief  Usage Example:
 *
 * // In main.c
 * ICM20608_Data_t icm_data;
 * ICM20608_Filter_t icm_filter = {.alpha = 0.98f};
 *
 * int main(void) {
 *     // ... HAL Init, GPIO, I2C Init ...
 *
 *     // Initialize ICM-20608-G
 *     if(ICM20608_Init(&hi2c1) != 0) {
 *         // Error handling
 *     }
 *
 *     // In scheduler
 *     while(1) {
 *         icm20608_task();  // Call every 100ms
 *
 *         if(fall_flag) {
 *             // Handle fall event
 *         }
 *     }
 * }
 */
