/**
  ******************************************************************************
  * @file           : mq2.c
  * @brief          : MQ2烟雾传感器驱动实现
  * @author         : STM32智能安全帽项目组
  * @date           : 2025-12-01
  ******************************************************************************
  */

#include "mq2.h"
#include "adc.h"
#include <stdio.h>
#include <math.h>

/* ==================== 全局变量 ==================== */

static MQ2_Data_t mq2_data = {0};
static float R0 = MQ2_R0_CLEAN_AIR;  // 基准电阻

/* ==================== 函数实现 ==================== */

/**
 * @brief 初始化MQ2传感器
 */
void MQ2_Init(void)
{
    // ADC已在CubeMX中配置，此处仅启动ADC
    HAL_ADC_Start(&hadc1);

    printf("MQ2: Init Success, Preheating...\r\n");
    printf("MQ2: Please wait 3 minutes for preheating\r\n");
}

/**
 * @brief 校准MQ2传感器（在清洁空气中）
 * @retval R0值(kΩ)
 */
float MQ2_Calibrate(void)
{
    float sum_Rs = 0.0f;
    uint32_t adc_value;
    float voltage, Rs;

    printf("MQ2: Calibration started (50 samples)...\r\n");

    // 采集50个样本求平均
    for (int i = 0; i < 50; i++) {
        HAL_ADC_PollForConversion(&hadc1, 100);
        adc_value = HAL_ADC_GetValue(&hadc1);

        // 转换为电压 (3.3V参考)
        voltage = (float)adc_value * 3.3f / 4095.0f;

        // 计算传感器电阻 Rs (kΩ)
        // Rs = (Vc - Vout) / Vout * RL
        // Vc = 5V (MQ2供电电压)
        if (voltage > 0.1f) {  // 避免除零
            Rs = ((5.0f - voltage) / voltage) * MQ2_LOAD_RESISTANCE;
            sum_Rs += Rs;
        }

        HAL_Delay(100);
    }

    float avg_Rs = sum_Rs / 50.0f;

    // 清洁空气中 Rs/R0 = 9.9 (根据MQ2数据手册)
    R0 = avg_Rs / 9.9f;

    printf("MQ2: Calibration completed, R0=%.2f kΩ\r\n", R0);

    return R0;
}

/**
 * @brief 读取MQ2传感器数据
 * @param data: 数据结构指针
 */
void MQ2_Read_Data(MQ2_Data_t *data)
{
    // 读取ADC值
    HAL_ADC_PollForConversion(&hadc1, 100);
    data->adc_value = HAL_ADC_GetValue(&hadc1);

    // 转换为电压
    data->voltage = (float)data->adc_value * 3.3f / 4095.0f;

    // 计算传感器电阻 Rs
    if (data->voltage > 0.1f) {
        data->Rs = ((5.0f - data->voltage) / data->voltage) * MQ2_LOAD_RESISTANCE;

        // 计算Rs/R0比值
        data->ratio = data->Rs / R0;

        // 计算ppm浓度
        data->ppm = MQ2_Calculate_PPM(data->Rs, R0);

        // 检查报警
        data->alarm = (data->ppm > MQ2_ALARM_THRESHOLD);
    } else {
        data->Rs = 0;
        data->ratio = 0;
        data->ppm = 0;
        data->alarm = false;
    }
}

/**
 * @brief 计算烟雾浓度
 * @param Rs: 传感器电阻(kΩ)
 * @param R0: 清洁空气中的基准电阻(kΩ)
 * @retval 烟雾浓度(ppm)
 */
float MQ2_Calculate_PPM(float Rs, float R0)
{
    float ratio = Rs / R0;

    // MQ2的ppm计算公式（根据数据手册拟合）
    // ppm = (ratio / a) ^ (1/b)
    // 其中 a=11.5428, b=-1.5278 (经验值)

    float ppm = powf(ratio / 11.5428f, 1.0f / -1.5278f);

    // 限制范围
    if (ppm < 0) ppm = 0;
    if (ppm > 10000) ppm = 10000;

    return ppm;
}

/**
 * @brief 打印MQ2数据
 * @param data: 数据结构指针
 */
void MQ2_Print_Data(MQ2_Data_t *data)
{
    printf("MQ2: %.2f ppm, Alarm: %d\r\n",
           data->ppm,
           data->alarm);
}

/**
 * @brief MQ2任务函数（供调度器调用）
 */
void mq2_task(void)
{
    MQ2_Read_Data(&mq2_data);
    MQ2_Print_Data(&mq2_data);
}
