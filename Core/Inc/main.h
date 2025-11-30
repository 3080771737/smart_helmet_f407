/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IO_PE2_Pin GPIO_PIN_2
#define IO_PE2_GPIO_Port GPIOE
#define IO_PE3_Pin GPIO_PIN_3
#define IO_PE3_GPIO_Port GPIOE
#define IO_PE4_Pin GPIO_PIN_4
#define IO_PE4_GPIO_Port GPIOE
#define IO_PE5_Pin GPIO_PIN_5
#define IO_PE5_GPIO_Port GPIOE
#define IO_PE6_Pin GPIO_PIN_6
#define IO_PE6_GPIO_Port GPIOE
#define LED_BUILTIN_Pin GPIO_PIN_13
#define LED_BUILTIN_GPIO_Port GPIOC
#define ADC_IN1_Pin GPIO_PIN_1
#define ADC_IN1_GPIO_Port GPIOA
#define IO_PC4_Pin GPIO_PIN_4
#define IO_PC4_GPIO_Port GPIOC
#define IO_PC5_Pin GPIO_PIN_5
#define IO_PC5_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_0
#define KEY1_GPIO_Port GPIOB
#define KEY2_Pin GPIO_PIN_1
#define KEY2_GPIO_Port GPIOB
#define IO_PB10_Pin GPIO_PIN_10
#define IO_PB10_GPIO_Port GPIOB
#define IO_PB11_Pin GPIO_PIN_11
#define IO_PB11_GPIO_Port GPIOB
#define IO_PB12_Pin GPIO_PIN_12
#define IO_PB12_GPIO_Port GPIOB
#define IO_PB13_Pin GPIO_PIN_13
#define IO_PB13_GPIO_Port GPIOB
#define IO_PB14_Pin GPIO_PIN_14
#define IO_PB14_GPIO_Port GPIOB
#define IO_PB15_Pin GPIO_PIN_15
#define IO_PB15_GPIO_Port GPIOB
#define IO_PD8_Pin GPIO_PIN_8
#define IO_PD8_GPIO_Port GPIOD
#define IO_PD9_Pin GPIO_PIN_9
#define IO_PD9_GPIO_Port GPIOD
#define IO_PD10_Pin GPIO_PIN_10
#define IO_PD10_GPIO_Port GPIOD
#define IO_PD11_Pin GPIO_PIN_11
#define IO_PD11_GPIO_Port GPIOD
#define IO_PD12_Pin GPIO_PIN_12
#define IO_PD12_GPIO_Port GPIOD
#define IO_PD13_Pin GPIO_PIN_13
#define IO_PD13_GPIO_Port GPIOD
#define LED_PWR_Pin GPIO_PIN_14
#define LED_PWR_GPIO_Port GPIOD
#define LED_STA_Pin GPIO_PIN_15
#define LED_STA_GPIO_Port GPIOD
#define IO_PC6_Pin GPIO_PIN_6
#define IO_PC6_GPIO_Port GPIOC
#define IO_PC7_Pin GPIO_PIN_7
#define IO_PC7_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_8
#define LED_R_GPIO_Port GPIOA
#define ICM_INT_Pin GPIO_PIN_15
#define ICM_INT_GPIO_Port GPIOA
#define ICM_INT_EXTI_IRQn EXTI15_10_IRQn
#define IO_PD0_Pin GPIO_PIN_0
#define IO_PD0_GPIO_Port GPIOD
#define IO_PD1_Pin GPIO_PIN_1
#define IO_PD1_GPIO_Port GPIOD
#define LED_G_Pin GPIO_PIN_3
#define LED_G_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_4
#define LED_B_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_5
#define BOOT1_GPIO_Port GPIOB
#define IO_PB8_Pin GPIO_PIN_8
#define IO_PB8_GPIO_Port GPIOB
#define IO_PB9_Pin GPIO_PIN_9
#define IO_PB9_GPIO_Port GPIOB
#define IO_PE0_Pin GPIO_PIN_0
#define IO_PE0_GPIO_Port GPIOE
#define IO_PE1_Pin GPIO_PIN_1
#define IO_PE1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
