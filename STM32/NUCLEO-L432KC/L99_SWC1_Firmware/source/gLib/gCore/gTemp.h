
/**
 * @file    gTemp.h
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

#ifndef _GTEMP_H_
#define _GTEMP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ************************************************************************************************
 * includes
 * ************************************************************************************************
 */

#include "main.h"

/**
 * ************************************************************************************************
 * library configuration
 * ************************************************************************************************
 */

/* USER CODE BEGIN configuration */

#define GTEMP_ENABLE                  1
#define GTEMP_CHECK_INTERVAL_MS       500UL
#define GTEMP_ADC      								hadc2
#define GTEMP_VREF_MV                 3300
#define GTEMP_HYSTERESIS_C 						5

/* configuration default */
#define GTEMP_ALARM_DESI_DEFAULT      600

/* USER CODE END configuration */

#define GTEMP_V0                      500                           ///< voltage at 0 degree
#define GTEMP_COEF                    10                            ///< mV/Degree
#define GTEMP_ERROR_VAL               -1000
#define GTEMP_FILTER_COEF 						0.2f                          ///< Filter ADC value by this coef

/**
 * ************************************************************************************************
 * struct/enum definitions
 * ************************************************************************************************
 */

typedef enum
{
  GTEMP_STATE_NORMAL                  = 0,
  GTEMP_STATE_ALARM

} GTempState;

typedef struct
{
  int16_t                             alarm_val;                    ///< storing alarm in desi-degree
  int16_t                             temp;                         ///< storing latest sensor value in desi-degree
  uint32_t                            time;                         ///< for calculating interval
  GTempState                          state;                        ///< storing current state

} GTempHandle;

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */

// none

/**
 * ************************************************************************************************
 * public library functions
 * ************************************************************************************************
 */

void          gTempInit(void);
void          gTempDeInit(void);
void          gTempLoop(void);
GTempState    gTempGetState(void);
int16_t       gTempGetDesiCelsius(void);
void          gTempSetAlarmDesiCelsius(int16_t temp);
int16_t       gTempGetAlarmDesiCelsius(void);

void          gTempAlarmCallback(void);

/* USER CODE BEGIN public library functions */

/* USER CODE END public library functions */

#ifdef __cplusplus
}
#endif
#endif
