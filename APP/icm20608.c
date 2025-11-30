/**
  ******************************************************************************
  * @file    icm20608.c
  * @brief   ICM-20608-G 6-axis gyroscope and accelerometer driver implementation
  * @author  Smart Helmet Project
  * @date    2025-11-29
  ******************************************************************************
  */

#include "icm20608.h"
#include <stdio.h>

/* ==================== Global Variables ==================== */
ICM20608_Data_t icm_data;                // Global sensor data
ICM20608_Filter_t icm_filter = {
    .alpha = 0.98f,                      // Complementary filter coefficient
    .pitch = 0.0f,
    .roll = 0.0f,
    .last_update = 0
};
uint8_t fall_flag = 0;                   // Fall detection flag

/* ==================== Private Functions ==================== */

/**
 * @brief  Write one byte to ICM-20608-G register
 * @param  hi2c: Pointer to I2C handle
 * @param  reg: Register address
 * @param  data: Data to write
 * @retval HAL status
 */
static HAL_StatusTypeDef ICM20608_WriteReg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t data) {
    return HAL_I2C_Mem_Write(hi2c, ICM20608_ADDRESS, reg,
                             I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}

/**
 * @brief  Read one byte from ICM-20608-G register
 * @param  hi2c: Pointer to I2C handle
 * @param  reg: Register address
 * @param  data: Pointer to store read data
 * @retval HAL status
 */
static HAL_StatusTypeDef ICM20608_ReadReg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *data) {
    return HAL_I2C_Mem_Read(hi2c, ICM20608_ADDRESS, reg,
                            I2C_MEMADD_SIZE_8BIT, data, 1, 100);
}

/**
 * @brief  Read multiple bytes from ICM-20608-G
 * @param  hi2c: Pointer to I2C handle
 * @param  reg: Starting register address
 * @param  data: Pointer to data buffer
 * @param  len: Number of bytes to read
 * @retval HAL status
 */
static HAL_StatusTypeDef ICM20608_ReadRegs(I2C_HandleTypeDef *hi2c, uint8_t reg,
                                           uint8_t *data, uint16_t len) {
    return HAL_I2C_Mem_Read(hi2c, ICM20608_ADDRESS, reg,
                            I2C_MEMADD_SIZE_8BIT, data, len, 200);
}

/* ==================== Public Functions ==================== */

/**
 * @brief  Initialize ICM-20608-G sensor
 */
uint8_t ICM20608_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t who_am_i;
    uint8_t temp;

    // Step 1: Read WHO_AM_I register
    if(ICM20608_ReadReg(hi2c, ICM20608_WHO_AM_I, &who_am_i) != HAL_OK) {
        return 1;  // I2C communication error
    }

    if(who_am_i != 0xAF) {
        return 2;  // Device ID mismatch
    }

    // Step 2: Reset device
    temp = 0x80;  // PWR_MGMT_1: DEVICE_RESET bit
    if(ICM20608_WriteReg(hi2c, ICM20608_PWR_MGMT_1, temp) != HAL_OK) {
        return 3;
    }
    HAL_Delay(100);  // Wait for reset to complete

    // Step 3: Wake up device (disable SLEEP mode)
    temp = 0x00;  // PWR_MGMT_1: Clear SLEEP bit
    if(ICM20608_WriteReg(hi2c, ICM20608_PWR_MGMT_1, temp) != HAL_OK) {
        return 4;
    }
    HAL_Delay(10);

    // Step 4: Configure gyroscope full scale range (±2000°/s)
    temp = ICM20608_DEFAULT_GYRO_FS;
    if(ICM20608_WriteReg(hi2c, ICM20608_GYRO_CONFIG, temp) != HAL_OK) {
        return 5;
    }

    // Step 5: Configure accelerometer full scale range (±16g)
    temp = ICM20608_DEFAULT_ACCEL_FS;
    if(ICM20608_WriteReg(hi2c, ICM20608_ACCEL_CONFIG, temp) != HAL_OK) {
        return 6;
    }

    // Step 6: Set sample rate divider (optional, default 1kHz)
    // CONFIG register: DLPF_CFG = 0 (Accel: 460Hz, Gyro: 250Hz)
    temp = 0x00;
    if(ICM20608_WriteReg(hi2c, ICM20608_CONFIG, temp) != HAL_OK) {
        return 7;
    }

    // Step 7: Enable all axes
    temp = 0x00;  // PWR_MGMT_2: Enable all sensors
    if(ICM20608_WriteReg(hi2c, ICM20608_PWR_MGMT_2, temp) != HAL_OK) {
        return 8;
    }

    HAL_Delay(50);  // Wait for sensors to stabilize

    printf("ICM-20608-G initialized successfully (ID=0x%02X)\r\n", who_am_i);
    return 0;  // Success
}

/**
 * @brief  Read WHO_AM_I register
 */
uint8_t ICM20608_ReadID(I2C_HandleTypeDef *hi2c) {
    uint8_t id = 0;
    ICM20608_ReadReg(hi2c, ICM20608_WHO_AM_I, &id);
    return id;
}

/**
 * @brief  Read raw data from ICM-20608-G
 */
uint8_t ICM20608_ReadRawData(I2C_HandleTypeDef *hi2c, ICM20608_RawData_t *raw_data) {
    uint8_t buf[14];

    // Read 14 bytes starting from ACCEL_XOUT_H
    // Accel(6) + Temp(2) + Gyro(6) = 14 bytes
    if(ICM20608_ReadRegs(hi2c, ICM20608_ACCEL_XOUT_H, buf, 14) != HAL_OK) {
        return 1;  // Read error
    }

    // Parse accelerometer data (Big Endian)
    raw_data->accel_x_raw = (int16_t)((buf[0] << 8) | buf[1]);
    raw_data->accel_y_raw = (int16_t)((buf[2] << 8) | buf[3]);
    raw_data->accel_z_raw = (int16_t)((buf[4] << 8) | buf[5]);

    // Parse temperature data
    raw_data->temp_raw = (int16_t)((buf[6] << 8) | buf[7]);

    // Parse gyroscope data
    raw_data->gyro_x_raw = (int16_t)((buf[8] << 8) | buf[9]);
    raw_data->gyro_y_raw = (int16_t)((buf[10] << 8) | buf[11]);
    raw_data->gyro_z_raw = (int16_t)((buf[12] << 8) | buf[13]);

    return 0;  // Success
}

/**
 * @brief  Process raw data to physical units
 */
void ICM20608_ProcessData(ICM20608_RawData_t *raw_data, ICM20608_Data_t *data) {
    // Convert accelerometer (g)
    data->accel_x = (float)raw_data->accel_x_raw / ICM20608_DEFAULT_ACCEL_SENS;
    data->accel_y = (float)raw_data->accel_y_raw / ICM20608_DEFAULT_ACCEL_SENS;
    data->accel_z = (float)raw_data->accel_z_raw / ICM20608_DEFAULT_ACCEL_SENS;

    // Convert gyroscope (°/s)
    data->gyro_x = (float)raw_data->gyro_x_raw / ICM20608_DEFAULT_GYRO_SENS;
    data->gyro_y = (float)raw_data->gyro_y_raw / ICM20608_DEFAULT_GYRO_SENS;
    data->gyro_z = (float)raw_data->gyro_z_raw / ICM20608_DEFAULT_GYRO_SENS;

    // Convert temperature (°C)
    // Temperature in °C = (TEMP_OUT - RoomTemp_Offset)/Temp_Sensitivity + 21
    // For ICM-20608-G: Temp_Sensitivity = 326.8 LSB/°C
    data->temperature = ((float)raw_data->temp_raw / 326.8f) + 21.0f;
}

/**
 * @brief  Calculate attitude angles using complementary filter
 */
void ICM20608_CalculateAttitude(ICM20608_Data_t *data, ICM20608_Filter_t *filter) {
    uint32_t current_time = HAL_GetTick();
    float dt;

    // Calculate time delta (seconds)
    if(filter->last_update == 0) {
        dt = 0.1f;  // Default 100ms for first call
    } else {
        dt = (current_time - filter->last_update) / 1000.0f;
    }
    filter->last_update = current_time;

    // Calculate pitch and roll from accelerometer (degrees)
    float accel_pitch = atan2(-data->accel_x,
                              sqrt(data->accel_y * data->accel_y +
                                   data->accel_z * data->accel_z)) * 57.3f;
    float accel_roll = atan2(data->accel_y, data->accel_z) * 57.3f;

    // Integrate gyroscope to get angle change (degrees)
    float gyro_pitch_delta = data->gyro_y * dt;
    float gyro_roll_delta = data->gyro_x * dt;

    // Complementary filter: alpha * (gyro integration) + (1-alpha) * (accel)
    filter->pitch = filter->alpha * (filter->pitch + gyro_pitch_delta) +
                    (1.0f - filter->alpha) * accel_pitch;
    filter->roll = filter->alpha * (filter->roll + gyro_roll_delta) +
                   (1.0f - filter->alpha) * accel_roll;

    // Update data structure
    data->pitch = filter->pitch;
    data->roll = filter->roll;

    // Yaw cannot be calculated accurately without magnetometer
    // Approximate yaw from gyroscope integration (will drift over time)
    static float yaw_estimate = 0.0f;
    yaw_estimate += data->gyro_z * dt;
    data->yaw = yaw_estimate;
}

/**
 * @brief  Detect fall event
 */
uint8_t ICM20608_DetectFall(ICM20608_Data_t *data, float threshold) {
    // Fall detected if pitch or roll exceeds threshold (e.g., 60°)
    if(fabs(data->pitch) > threshold || fabs(data->roll) > threshold) {
        return 1;  // Fall detected
    }
    return 0;  // Normal
}

/**
 * @brief  Enable data ready interrupt
 */
uint8_t ICM20608_EnableInterrupt(I2C_HandleTypeDef *hi2c) {
    uint8_t temp;

    // Configure INT pin (active low, push-pull, latched)
    temp = 0xA0;  // INT_PIN_CFG: LATCH_INT_EN | INT_RD_CLEAR
    if(ICM20608_WriteReg(hi2c, ICM20608_INT_PIN_CFG, temp) != HAL_OK) {
        return 1;
    }

    // Enable data ready interrupt
    temp = 0x01;  // INT_ENABLE: DATA_RDY_EN
    if(ICM20608_WriteReg(hi2c, ICM20608_INT_ENABLE, temp) != HAL_OK) {
        return 2;
    }

    return 0;  // Success
}

/**
 * @brief  Task function for scheduler
 */
void icm20608_task(void) {
    ICM20608_RawData_t raw_data;

    // Read raw sensor data
    if(ICM20608_ReadRawData(&hi2c1, &raw_data) != 0) {
        // printf("ICM20608: Read error\r\n");
        return;
    }

    // Process data to physical units
    ICM20608_ProcessData(&raw_data, &icm_data);

    // Calculate attitude angles using complementary filter
    ICM20608_CalculateAttitude(&icm_data, &icm_filter);

    // Detect fall event (threshold: 60 degrees)
    fall_flag = ICM20608_DetectFall(&icm_data, 60.0f);

    // Optional: Print debug information
    // printf("Pitch: %.1f°, Roll: %.1f°, Fall: %d\r\n",
    //        icm_data.pitch, icm_data.roll, fall_flag);
}

/**
 * @brief  EXTI callback for PA15 interrupt (data ready)
 *         Add this to stm32f1xx_it.c: HAL_GPIO_EXTI_Callback()
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == GPIO_PIN_15) {  // PA15: ICM-20608-G INT
        // Interrupt triggered, data is ready
        // Set a flag or call icm20608_task() directly
        icm20608_task();
    }
}
