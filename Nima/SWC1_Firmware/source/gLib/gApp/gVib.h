
/**
 * @file    gVib.h
 * @brief
 *
 * Detailed description explaining the purpose and usage of this file,
 * such as any context or dependencies it has. It can also include a description
 * of the related module or functionality.
 *
 * @author  Nima Askari , Askari@gtec.at , Nima.Askari@gmail.com
 *
 * @copyright
 *  Copyright (C) 2024 g.tec medical engineering GmbH.
 *  All rights reserved.
 *
 *  This software is proprietary to g.tec medical engineering GmbH. No part of this
 *  software may be reproduced, distributed, or transmitted in any form or by any means,
 *  without the prior written permission of g.tec medical engineering GmbH.
 */
 
#ifndef _GVIB_H_
#define _GVIB_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ************************************************************************************************
 * includes
 * ************************************************************************************************
 */

#include "main.h"
#include <stdbool.h>

/**
 * ************************************************************************************************
 * library configuration
 * ************************************************************************************************
 */

#define GVIB_STIMULATION_CHANNEL      7
#define GVIB_MAX_SETTINGS             0x10000

#define GVIB_MAX_CH_DEFAULT           3
#define GVIB_TRIG_LENGTH_DEFAULT      20

#define GVIB_GPIO_LIST                {\
                                        {VIB_01_GPIO_Port, VIB_01_Pin}, \
                                        {VIB_02_GPIO_Port, VIB_02_Pin}, \
                                        {VIB_03_GPIO_Port, VIB_03_Pin}, \
                                        {VIB_04_GPIO_Port, VIB_04_Pin}, \
                                        {VIB_05_GPIO_Port, VIB_05_Pin}, \
                                        {VIB_06_GPIO_Port, VIB_06_Pin}, \
                                        {VIB_07_GPIO_Port, VIB_07_Pin}, \
                                      }

#define GVIB_TRIG1_GPIO               TRIG_01_GPIO_Port
#define GVIB_TRIG1_PIN                TRIG_01_Pin

#define GVIB_LED_GPIO                 LED_STATE_GPIO_Port
#define GVIB_LED_PIN                  LED_STATE_Pin
#define GVIB_LED_ON_TIME_DEFAULT      400
#define GVIB_LED_OFF_TIME_DEFAULT     100

/**
 * ************************************************************************************************
 * struct/enum definitions
 * ************************************************************************************************
 */

typedef enum
{
  GVIB_STATE_STOP                     =  0,
  GVIB_STATE_STIMULATION,
  GVIB_STATE_PAUSE,

} GVibStateEnum;

typedef struct
{
  GPIO_TypeDef                        *gpio;
  uint16_t                            pin;

} GVibGpioStruct;

typedef struct
{
  bool                                is_constant_trig;
  uint8_t                             max_ch;
  uint16_t                            trig_length;
  uint16_t                            sequence_len;
  uint16_t                            led_on_duration;
  uint16_t                            led_off_duration;

} GVibConfigStruct;

typedef struct
{

  uint32_t                            time;
  uint32_t                            time_trig;
  uint32_t                            time_led;
  uint16_t                            sequence_index;
  uint16_t                            settings_index;
  uint16_t                            settings_len;

} GVibInstanceStruct;

typedef struct
{
  uint8_t                             ch;
  uint16_t                            length;

} GVibSettingsStruct;

typedef struct
{
  GPIO_TypeDef                        *gpio[GVIB_STIMULATION_CHANNEL];
  uint16_t                            pin[GVIB_STIMULATION_CHANNEL];
  GVibSettingsStruct                  settings[GVIB_MAX_SETTINGS];
  GVibConfigStruct                    config;
  GVibStateEnum                       state;
  GVibInstanceStruct                  instance;

} GVibHandle;

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * public library functions
 * ************************************************************************************************
 */

void          gVibInit(void);
void          gVibDeInit(void);
void          gVibLoop(void);

bool          gVibClearSettings(void);
GVibStateEnum gVibGetState(void);
bool          gVibSetState(GVibStateEnum state);
void          gVibGetConstantTrig(uint8_t *enabled, uint16_t *length);
void          gVibSetConstantTrig(bool enable, uint16_t length);
uint8_t       gVibGetMaxCh(void);
bool          gVibSetMaxCh(uint8_t max_ch);
uint16_t      gVibGetSeqLen(void);
bool          gVibSetSeqLen(uint16_t len);
bool          gVibGetSettings(uint16_t start_index, uint16_t cnt, GVibSettingsStruct *settings);
bool          gVibSetSettings(uint16_t cnt, GVibSettingsStruct *settings);
bool          gVibSingleSetting(uint8_t ch, uint16_t duration);
uint16_t      gVibGetSeqIndex(void);
uint16_t      gVibGetSettingsIndex(void);
uint16_t      gVibGetSettingsLen(void);
void          gVibGetLedDuration(uint16_t *on_duration, uint16_t *off_duration);
void          gVibSetLedDuration(uint16_t on_duration, uint16_t off_duration);

#ifdef __cplusplus
}
#endif
#endif
