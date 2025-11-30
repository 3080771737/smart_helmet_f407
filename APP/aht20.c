/**
  ******************************************************************************
  * @file    aht20.c
  * @brief   AHT20 temperature and humidity sensor driver implementation
  * @author  Smart Helmet Project
  * @date    2025-11-29
  ******************************************************************************
  */

#include "aht20.h"
#include <stdio.h>

/* ==================== Global Variables ==================== */
AHT20_Data_t aht20_data = {
    .temperature = 0.0f,
    .humidity = 0.0f,
    .is_valid = 0
};

/* ==================== Public Functions ==================== */

/**
 * @brief  Initialize AHT20 sensor
 */
uint8_t AHT20_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t cmd[3];
    uint8_t status;

    // Wait for sensor to power up
    HAL_Delay(AHT20_POWERUP_DELAY);

    // Read status to check if calibrated
    if(AHT20_ReadStatus(hi2c, &status) != 0) {
        return 1;  // I2C communication error
    }

    // If not calibrated, send initialization command
    if(!(status & AHT20_STATUS_CALIBRATED)) {
        cmd[0] = AHT20_CMD_INIT;
        cmd[1] = 0x08;
        cmd[2] = 0x00;

        if(HAL_I2C_Master_Transmit(hi2c, AHT20_ADDRESS, cmd, 3, 100) != HAL_OK) {
            return 2;  // Initialization command failed
        }

        HAL_Delay(10);  // Wait for initialization

        // Verify calibration
        if(AHT20_ReadStatus(hi2c, &status) != 0) {
            return 3;
        }

        if(!(status & AHT20_STATUS_CALIBRATED)) {
            return 4;  // Calibration failed
        }
    }

    printf("AHT20 initialized successfully (Status=0x%02X)\r\n", status);
    return 0;  // Success
}

/**
 * @brief  Soft reset AHT20
 */
uint8_t AHT20_SoftReset(I2C_HandleTypeDef *hi2c) {
    uint8_t cmd = AHT20_CMD_SOFTRESET;

    if(HAL_I2C_Master_Transmit(hi2c, AHT20_ADDRESS, &cmd, 1, 100) != HAL_OK) {
        return 1;  // Reset command failed
    }

    HAL_Delay(AHT20_RESET_DELAY);
    return 0;  // Success
}

/**
 * @brief  Read status register
 */
uint8_t AHT20_ReadStatus(I2C_HandleTypeDef *hi2c, uint8_t *status) {
    if(HAL_I2C_Master_Receive(hi2c, AHT20_ADDRESS, status, 1, 100) != HAL_OK) {
        return 1;  // Read error
    }
    return 0;  // Success
}

/**
 * @brief  Check if sensor is busy
 */
uint8_t AHT20_IsBusy(I2C_HandleTypeDef *hi2c) {
    uint8_t status;

    if(AHT20_ReadStatus(hi2c, &status) != 0) {
        return 1;  // Assume busy if read fails
    }

    return (status & AHT20_STATUS_BUSY) ? 1 : 0;
}

/**
 * @brief  Check if sensor is calibrated
 */
uint8_t AHT20_IsCalibrated(I2C_HandleTypeDef *hi2c) {
    uint8_t status;

    if(AHT20_ReadStatus(hi2c, &status) != 0) {
        return 0;  // Assume not calibrated if read fails
    }

    return (status & AHT20_STATUS_CALIBRATED) ? 1 : 0;
}

/**
 * @brief  Trigger measurement
 */
uint8_t AHT20_TriggerMeasurement(I2C_HandleTypeDef *hi2c) {
    uint8_t cmd[3] = {AHT20_CMD_TRIGGER, 0x33, 0x00};

    if(HAL_I2C_Master_Transmit(hi2c, AHT20_ADDRESS, cmd, 3, 100) != HAL_OK) {
        return 1;  // Trigger command failed
    }

    return 0;  // Success
}

/**
 * @brief  Read raw data from AHT20
 */
uint8_t AHT20_ReadRawData(I2C_HandleTypeDef *hi2c, AHT20_RawData_t *raw_data) {
    uint8_t buf[7];

    // Read 7 bytes: Status(1) + Humidity(3) + Temperature(3)
    if(HAL_I2C_Master_Receive(hi2c, AHT20_ADDRESS, buf, 7, 100) != HAL_OK) {
        return 1;  // Read error
    }

    // Save status
    raw_data->status = buf[0];

    // Check busy flag
    if(raw_data->status & AHT20_STATUS_BUSY) {
        return 2;  // Sensor is busy
    }

    // Parse humidity (20-bit data)
    // Humidity = buf[1][7:0] + buf[2][7:0] + buf[3][7:4]
    raw_data->humidity_raw = ((uint32_t)buf[1] << 12) |
                             ((uint32_t)buf[2] << 4) |
                             ((uint32_t)buf[3] >> 4);

    // Parse temperature (20-bit data)
    // Temperature = buf[3][3:0] + buf[4][7:0] + buf[5][7:0]
    raw_data->temperature_raw = (((uint32_t)buf[3] & 0x0F) << 16) |
                                ((uint32_t)buf[4] << 8) |
                                (uint32_t)buf[5];

    return 0;  // Success
}

/**
 * @brief  Process raw data to physical units
 */
void AHT20_ProcessData(AHT20_RawData_t *raw_data, AHT20_Data_t *data) {
    // Convert humidity
    // Formula: RH% = (raw_value / 2^20) * 100
    data->humidity = ((float)raw_data->humidity_raw / 1048576.0f) * 100.0f;

    // Convert temperature
    // Formula: T(°C) = (raw_value / 2^20) * 200 - 50
    data->temperature = ((float)raw_data->temperature_raw / 1048576.0f) * 200.0f - 50.0f;

    // Validate ranges
    if(data->temperature < AHT20_TEMP_MIN || data->temperature > AHT20_TEMP_MAX) {
        data->is_valid = 0;
    } else if(data->humidity < AHT20_HUMIDITY_MIN || data->humidity > AHT20_HUMIDITY_MAX) {
        data->is_valid = 0;
    } else {
        data->is_valid = 1;
    }
}

/**
 * @brief  Read temperature and humidity (one-shot measurement)
 */
uint8_t AHT20_ReadData(I2C_HandleTypeDef *hi2c, AHT20_Data_t *data) {
    AHT20_RawData_t raw_data;

    // Step 1: Trigger measurement
    if(AHT20_TriggerMeasurement(hi2c) != 0) {
        return 1;  // Trigger failed
    }

    // Step 2: Wait for measurement to complete (max 80ms)
    HAL_Delay(AHT20_MEASUREMENT_DELAY);

    // Optional: Poll busy flag instead of fixed delay
    uint8_t retries = 10;
    while(AHT20_IsBusy(hi2c) && retries > 0) {
        HAL_Delay(10);
        retries--;
    }

    if(retries == 0) {
        return 2;  // Timeout waiting for measurement
    }

    // Step 3: Read raw data
    uint8_t result = AHT20_ReadRawData(hi2c, &raw_data);
    if(result != 0) {
        return result + 2;  // Read error (3 or 4)
    }

    // Step 4: Process data
    AHT20_ProcessData(&raw_data, data);

    return 0;  // Success
}

/**
 * @brief  Task function for scheduler
 */
void aht20_task(void) {
    uint8_t result = AHT20_ReadData(&hi2c1, &aht20_data);

    if(result != 0) {
        // printf("AHT20: Read error (code=%d)\r\n", result);
        aht20_data.is_valid = 0;
        return;
    }

    if(!aht20_data.is_valid) {
        // printf("AHT20: Data out of range\r\n");
        return;
    }

    // Optional: Print debug information
    // printf("AHT20: Temperature=%.1f°C, Humidity=%.1f%%\r\n",
    //        aht20_data.temperature, aht20_data.humidity);
}

/**
 * @brief  Comparison with DHT11
 *
 * | Feature          | DHT11           | AHT20              |
 * |------------------|-----------------|-------------------|
 * | Interface        | Single-wire     | I2C               |
 * | Temperature      | 0~50°C          | -40~85°C          |
 * | Temp Accuracy    | ±2°C            | ±0.3°C            |
 * | Humidity         | 20~90%RH        | 0~100%RH          |
 * | Humidity Accuracy| ±5%RH           | ±2%RH             |
 * | Response Time    | 2000ms          | 80ms              |
 * | Timing Critical  | Yes (us level)  | No                |
 * | Power            | 0.5~2.5mA       | 0.2~1.5mA         |
 * | I2C Shareable    | No              | Yes (shared bus)  |
 *
 * AHT20 is a superior replacement for DHT11 in every aspect!
 */
