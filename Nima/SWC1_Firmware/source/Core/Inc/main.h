/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32h5xx_hal.h"

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
#define LED_STATE_Pin GPIO_PIN_0
#define LED_STATE_GPIO_Port GPIOC
#define TEMP_ADC_Pin GPIO_PIN_1
#define TEMP_ADC_GPIO_Port GPIOC
#define BUZZ_Pin GPIO_PIN_2
#define BUZZ_GPIO_Port GPIOC
#define DCDC_ALT_Pin GPIO_PIN_3
#define DCDC_ALT_GPIO_Port GPIOC
#define VIB_01_Pin GPIO_PIN_0
#define VIB_01_GPIO_Port GPIOA
#define VIB_02_Pin GPIO_PIN_1
#define VIB_02_GPIO_Port GPIOA
#define VIB_03_Pin GPIO_PIN_2
#define VIB_03_GPIO_Port GPIOA
#define VIB_04_Pin GPIO_PIN_3
#define VIB_04_GPIO_Port GPIOA
#define VIB_05_Pin GPIO_PIN_4
#define VIB_05_GPIO_Port GPIOA
#define VIB_06_Pin GPIO_PIN_5
#define VIB_06_GPIO_Port GPIOA
#define VIB_07_Pin GPIO_PIN_6
#define VIB_07_GPIO_Port GPIOA
#define TRIG_01_Pin GPIO_PIN_4
#define TRIG_01_GPIO_Port GPIOC
#define TRIG_02_Pin GPIO_PIN_5
#define TRIG_02_GPIO_Port GPIOC
#define DCDC_EN_Pin GPIO_PIN_6
#define DCDC_EN_GPIO_Port GPIOC
#define FTDI_RESET_Pin GPIO_PIN_11
#define FTDI_RESET_GPIO_Port GPIOA
#define FTDI_GPIO3_Pin GPIO_PIN_12
#define FTDI_GPIO3_GPIO_Port GPIOC
#define FTDI_GPIO2_Pin GPIO_PIN_2
#define FTDI_GPIO2_GPIO_Port GPIOD
#define LED_ERR_Pin GPIO_PIN_4
#define LED_ERR_GPIO_Port GPIOB
#define LED_SYS_Pin GPIO_PIN_5
#define LED_SYS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
