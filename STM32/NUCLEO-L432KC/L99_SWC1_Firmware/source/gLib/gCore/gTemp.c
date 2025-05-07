
/**
 * @file    gTemp.c
 * @brief   This library is for reading an external temperature sensor and triggering an alarm
 *          if needed when the threshold is reached.
 *
 * @author  Nima Askari , Askari@gtec.at , Nima.Askari@gmail.com
 * @date    21/03/2025 09:27:52
 * @version 0.9.22
 *
 * @copyright
 *  Copyright (C) 2024 g.tec medical engineering GmbH.
 *  All rights reserved.
 *
 *  This software is proprietary to g.tec medical engineering GmbH. No part of this
 *  software may be reproduced, distributed, or transmitted in any form or by any means,
 *  without the prior written permission of g.tec medical engineering GmbH.
 */

#include "gTemp.h"
#include "gCore.h"
#include "gInfo.h"
#include <string.h>

#if GTEMP_ENABLE == 1

#include "adc.h"

/**
 * ************************************************************************************************
 * private definitions
 * ************************************************************************************************
 */

// none

/**
 * ************************************************************************************************
 * private function prototypes
 * ************************************************************************************************
 */

int16_t       gTempGet(void);

/**
 * ************************************************************************************************
 * global variable
 * ************************************************************************************************
 */

GTempHandle   gtemp_;

/**
 * ************************************************************************************************
 * public function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Initializes the temperature sensor
 * @param  None
 * @return None
 * @note   This function starts the ADC calibration for the temperature sensor and initializes
 *         the temperature handle structure. If calibration fails, the system halts.
 *         The default alarm value is set based on the predefined constant.
 */
void gTempInit(void)
{
  if (HAL_ADCEx_Calibration_Start(&GTEMP_ADC, ADC_SINGLE_ENDED) != HAL_OK)
  {
    gCoreHalt();
  }
  memset(&gtemp_, 0, sizeof(GTempHandle));
  gtemp_.alarm_val = ginfo_storage_.temp_threshold;
  gtemp_.temp = 250;
  for (int i = 0; i < 100; i++)
  {
    int16_t temp = gTempGet();
    if (temp != GTEMP_ERROR_VAL)
    {
      gtemp_.temp = (int16_t)((GTEMP_FILTER_COEF * temp) + ((1.0f - GTEMP_FILTER_COEF) * gtemp_.temp));
    }
  }
}

/**
 * ************************************************************************************************
 * @brief  Deinitializes the temperature sensor
 * @param  None
 * @return None
 * @note   This function stops the ADC for the temperature sensor and clears the temperature
 *         handle structure, resetting its values to zero.
 */
void gTempDeInit(void)
{
  HAL_ADC_Stop(&GTEMP_ADC);
  memset(&gtemp_, 0, sizeof(GTempHandle));
}

/**
 * ************************************************************************************************
 * @brief  Monitors and processes temperature readings
 * @param  None
 * @return None
 * @note   This function checks the temperature at defined intervals, updates the temperature
 *         value, and manages the state of the temperature sensor. If the temperature exceeds
 *         the alarm threshold, it transitions to the alarm state and calls the alarm callback.
 *         If the temperature falls below the hysteresis threshold, it returns to the normal state.
 */
void gTempLoop(void)
{
  if (HAL_GetTick() - gtemp_.time >= GTEMP_CHECK_INTERVAL_MS)
  {
  	int16_t temp;
    gtemp_.time = HAL_GetTick();
    temp = gTempGet();
    if (temp != GTEMP_ERROR_VAL)
    {
    	gtemp_.temp = (int16_t)((GTEMP_FILTER_COEF * temp) + ((1.0f - GTEMP_FILTER_COEF) * gtemp_.temp));
      if (gtemp_.state == GTEMP_STATE_NORMAL)
      {
        if (gtemp_.temp > gtemp_.alarm_val)
        {
          gtemp_.state = GTEMP_STATE_ALARM;
          gTempAlarmCallback();
        }
      }
      else
      {
        if (gtemp_.temp < (gtemp_.alarm_val - (GTEMP_HYSTERESIS_C * 10)))
        {
          gtemp_.state = GTEMP_STATE_NORMAL;
        }
      }
    }
  }
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the current temperature state
 * @param  None
 * @return Current temperature state
 * @note   This function returns the current state of the temperature sensor, which can be
 *         either normal or in an alarm condition.
 */
GTempState gTempGetState(void)
{
  return gtemp_.state;
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the current temperature in deci-Celsius
 * @param  None
 * @return Current temperature in deci-Celsius
 * @note   This function returns the temperature measured by the sensor in deci-Celsius.
 *         The temperature value is multiplied by 10 for precision.
 */
int16_t gTempGetDesiCelsius(void)
{
  return gtemp_.temp;
}

/**
 * ************************************************************************************************
 * @brief  Sets the temperature alarm threshold in deci-Celsius
 * @param  temp [in] Temperature threshold in deci-Celsius to set as the alarm value
 * @return None
 * @note   This function updates the alarm temperature threshold for the temperature monitoring.
 *         The temperature value is expected to be in deci-Celsius.
 */
void gTempSetAlarmDesiCelsius(int16_t temp)
{
  gtemp_.alarm_val = temp;
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the current temperature alarm threshold in deci-Celsius
 * @param  None
 * @return Current alarm temperature threshold in deci-Celsius
 * @note   This function returns the temperature value set as the alarm threshold for the temperature
 *         monitoring system. The returned value is in deci-Celsius.
 */
int16_t gTempGetAlarmDesiCelsius(void)
{
  return gtemp_.alarm_val;
}

/**
 * ************************************************************************************************
 * @brief  Entering Alarm Callback
 * @param  None
 * @return None
 * @note   This function Calls when current temperature reach to the alarm values.
 */
__weak void gTempAlarmCallback(void)
{

}

/**
 * ************************************************************************************************
 * private function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Read temperature from ADC in deci-Celsius
 * @param  None
 * @return Current temperature in deci-Celsius
 * @note   This function returns the current temperature
 */
int16_t gTempGet(void)
{
  int16_t answer = GTEMP_ERROR_VAL;
  do
  {
    if (HAL_ADCEx_Calibration_Start(&GTEMP_ADC, ADC_SINGLE_ENDED) != HAL_OK)
    {
      break;
    }
    if (HAL_ADC_Start(&GTEMP_ADC) != HAL_OK)
    {
      break;
    }
    if (HAL_ADC_PollForConversion(&GTEMP_ADC, 10) != HAL_OK)
    {
      break;
    }
    answer = (((int32_t) __HAL_ADC_CALC_DATA_TO_VOLTAGE(GTEMP_VREF_MV, HAL_ADC_GetValue(&GTEMP_ADC),
        ADC_RESOLUTION_12B) - GTEMP_V0) / (GTEMP_COEF / 10));
  }
  while (0);

  HAL_ADC_Stop(&GTEMP_ADC);
  return answer;
}

#else

/**
 * ************************************************************************************************
 * public function implementations if the library disabled
 * ************************************************************************************************
 */

void gTempInit(void)
{

}

void gTempDeInit(void)
{

}

void gTempLoop(void)
{

}

GTempState gTempGetState(void)
{
  return GTEMP_STATE_NORMAL;
}

int16_t gTempGetDesiCelsius(void)
{
  return GTEMP_ERROR_VAL;
}

void gTempSetAlarmDesiCelsius(int16_t temp)
{
  UNUSED(temp);
}

int16_t gTempGetAlarmDesiCelsius(void)
{
  return 0;
}

void gTempAlarmCallback(void)
{

}

#endif
