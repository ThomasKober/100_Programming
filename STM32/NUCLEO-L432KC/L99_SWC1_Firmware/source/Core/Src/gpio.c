/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
     PH0-OSC_IN(PH0)   ------> RCC_OSC_IN
     PH1-OSC_OUT(PH1)   ------> RCC_OSC_OUT
     PA13(JTMS/SWDIO)   ------> DEBUG_JTMS-SWDIO
     PA14(JTCK/SWCLK)   ------> DEBUG_JTCK-SWCLK
     PB3(JTDO/TRACESWO)   ------> DEBUG_JTDO-SWO
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, VIB_01_Pin|VIB_02_Pin|VIB_03_Pin|VIB_04_Pin
                          |VIB_05_Pin|VIB_06_Pin|VIB_07_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, TRIG_02_Pin|TRIG_01_Pin|DCDC_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FTDI_RESET_GPIO_Port, FTDI_RESET_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_ERR_Pin|LED_SYS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : LED_STATE_Pin TRIG_02_Pin TRIG_01_Pin DCDC_EN_Pin */
  GPIO_InitStruct.Pin = LED_STATE_Pin|TRIG_02_Pin|TRIG_01_Pin|DCDC_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : DCDC_ALT_Pin */
  GPIO_InitStruct.Pin = DCDC_ALT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DCDC_ALT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : VIB_01_Pin VIB_02_Pin VIB_03_Pin VIB_04_Pin
                           VIB_05_Pin VIB_06_Pin VIB_07_Pin */
  GPIO_InitStruct.Pin = VIB_01_Pin|VIB_02_Pin|VIB_03_Pin|VIB_04_Pin
                          |VIB_05_Pin|VIB_06_Pin|VIB_07_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : FTDI_RESET_Pin */
  GPIO_InitStruct.Pin = FTDI_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FTDI_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : FTDI_GPIO3_Pin */
  GPIO_InitStruct.Pin = FTDI_GPIO3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(FTDI_GPIO3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : FTDI_GPIO2_Pin */
  GPIO_InitStruct.Pin = FTDI_GPIO2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(FTDI_GPIO2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_ERR_Pin LED_SYS_Pin */
  GPIO_InitStruct.Pin = LED_ERR_Pin|LED_SYS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
