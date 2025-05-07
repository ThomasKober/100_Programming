
/**
 * @file    gVib.c
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

#include "gVib.h"
#include "gCore.h"
#include "gApp.h"
#include "mp8859.h"
#include "gCom.h"
#include "gInfo.h"
#include "iwdg.h"
#include <string.h>

/**
 * ************************************************************************************************
 * private definitions
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * private function prototypes
 * ************************************************************************************************
 */

void          gVibResetSettings(void);
void          gVibSetPins(uint8_t mask);

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */

GVibHandle    gvib_;

/**
 * ************************************************************************************************
 * public function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Initializes vibration motor control by configuring GPIOs and trigger settings.
 * @note   Sets up vibration motor pins and default configuration settings. Attempts to initialize
 *         the `mp8859` component up to 500 times, with a 10 ms delay between each attempt.
 *         Halts the system if initialization fails.
 *         This function also refreshes the watchdog timer to prevent system reset during long delays.
 */
void gVibInit(void)
{
  const GVibGpioStruct gpio_pin_list[] = GVIB_GPIO_LIST;
  bool found = false;
  memset(&gvib_, 0, sizeof(GVibHandle));
  for (int i = 0; i < GVIB_STIMULATION_CHANNEL; i++)
  {
    gvib_.gpio[i] = gpio_pin_list[i].gpio;
    gvib_.pin[i] = gpio_pin_list[i].pin;
    HAL_GPIO_WritePin(gvib_.gpio[i], gvib_.pin[i], GPIO_PIN_RESET);
  }
  HAL_GPIO_WritePin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN, GPIO_PIN_RESET);
  gvib_.config.is_constant_trig = ginfo_storage_.config.is_constant_trig;
  gvib_.config.max_ch = ginfo_storage_.config.max_ch;
  gvib_.config.trig_length = ginfo_storage_.config.trig_length;
  gvib_.config.sequence_len = 1;
  gvib_.config.led_on_duration = ginfo_storage_.config.led_on_duration;
  gvib_.config.led_off_duration = ginfo_storage_.config.led_off_duration;
  for (int i = 0; i < 500; i++)
  {
    HAL_IWDG_Refresh(&hiwdg);
    HAL_Delay(10);
		if (mp8859Init())
		{
			found = true;
			break;
		}
  }
  if (found == false)
  {
    gCoreHalt();
  }
}

/**
 * ************************************************************************************************
 * @brief  Deinitializes vibration motor control by resetting GPIOs and clearing configuration data.
 * @note   Disables all vibration motor pins and resets trigger GPIO before clearing the
 *         `gvib_` structure to zero.
 */
void gVibDeInit(void)
{
  gVibSetPins(0x00);
  HAL_GPIO_WritePin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN, GPIO_PIN_RESET);
  memset(&gvib_, 0, sizeof(GVibHandle));
}

/**
 * ************************************************************************************************
 * @brief  Manages vibration and LED timing sequences for stimulation.
 * @note   Executes the active vibration sequence and LED control by checking each stage
 *         against elapsed time and adjusting the GPIO pins accordingly.
 *         If `gvib_.state` is set to `GVIB_STATE_STIMULATION`, it processes settings based on
 *         the specified lengths, triggering vibration channels and toggling LEDs as configured.
 *         When the sequence ends or if stimulation stops, all pins are reset.
 *         Requires gvib_ structure to be initialized, with each setting in `settings` configured.
 */
void gVibLoop(void)
{
  if (gvib_.state == GVIB_STATE_STIMULATION)
  {
    if (gvib_.settings[gvib_.instance.settings_index].length != 0xFFFF)
    {
      if (HAL_GetTick() - gvib_.instance.time  >= gvib_.settings[gvib_.instance.settings_index].length)
      {
        gvib_.instance.time = HAL_GetTick();
        gvib_.instance.settings_index++;
        if (gvib_.instance.settings_index >= gvib_.instance.settings_len)
        {
          gvib_.instance.settings_index = 0;
          gvib_.instance.sequence_index++;
          if (gvib_.instance.sequence_index == gvib_.config.sequence_len)
          {
            gVibSetState(GVIB_STATE_STOP);
          }
          else
          {
          	gVibSetPins(gvib_.settings[gvib_.instance.settings_index].ch);
          }
        }
        else
        {
        	gVibSetPins(gvib_.settings[gvib_.instance.settings_index].ch);
        }
      }
    }

    if ((gvib_.config.is_constant_trig == true)  && (HAL_GPIO_ReadPin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN) == GPIO_PIN_SET))
    {
      if (HAL_GetTick() - gvib_.instance.time_trig >= (uint32_t) gvib_.config.trig_length)
      {
        HAL_GPIO_WritePin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN, GPIO_PIN_RESET);
      }
    }
    if (HAL_GPIO_ReadPin(GVIB_LED_GPIO, GVIB_LED_PIN) == GPIO_PIN_RESET)
    {
      if (HAL_GetTick() - gvib_.instance.time_led >= (uint32_t) gvib_.config.led_on_duration)
      {
        gvib_.instance.time_led = HAL_GetTick();
        HAL_GPIO_WritePin(GVIB_LED_GPIO, GVIB_LED_PIN, GPIO_PIN_SET);
      }
    }
    else
    {
      if (HAL_GetTick() - gvib_.instance.time_led >= (uint32_t) gvib_.config.led_off_duration)
      {
        gvib_.instance.time_led = HAL_GetTick();
        HAL_GPIO_WritePin(GVIB_LED_GPIO, GVIB_LED_PIN, GPIO_PIN_RESET);
      }
    }
  }
  else
  {
    HAL_GPIO_WritePin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GVIB_LED_GPIO, GVIB_LED_PIN, GPIO_PIN_RESET);
  }
}

/**
 * ************************************************************************************************
 * @brief  Clears the vibration settings when the system is in a stopped state.
 * @return `true` if settings were successfully cleared; otherwise, `false`
 * @note   This function checks if the vibration state is `GVIB_STATE_STOP`.
 *         If true, it resets all settings by calling `gVibResetSettings`.
 */
bool gVibClearSettings(void)
{
  bool answer = false;
  do
  {
    if (gvib_.state == GVIB_STATE_STOP)
    {
      gVibResetSettings();
      answer = true;
    }

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the current state of the vibration system.
 * @return Current vibration state of type `GVibStateEnum`.
 * @note   This function returns the state from the global vibration handle.
 */
GVibStateEnum gVibGetState(void)
{
  return gvib_.state;
}

/**
 * ************************************************************************************************
 * @brief  Sets the state of the vibration system.
 * @param  state [in] Desired state of type `GVibStateEnum` to set.
 * @return True if the state was successfully set, false otherwise.
 * @note   This function transitions the vibration system between different states:
 *         - `GVIB_STATE_STOP`: Stops the vibration and resets settings.
 *         - `GVIB_STATE_STIMULATION`: Starts stimulation if in a valid state.
 *         - `GVIB_STATE_PAUSE`: Pauses stimulation if currently active.
 */
bool gVibSetState(GVibStateEnum state)
{
  bool answer = false;
  do
  {
    switch (state)
    {
    ///----------------------------------------------------
    case GVIB_STATE_STOP:
      HAL_GPIO_WritePin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN, GPIO_PIN_RESET);
      gVibSetPins(0x00);
      gVibResetSettings();
      mp8859Enable(true);
      gvib_.state = GVIB_STATE_STOP;
      answer = true;
      break;
    ///----------------------------------------------------
    case GVIB_STATE_STIMULATION:
      if ((gvib_.state == GVIB_STATE_STOP) && (gvib_.instance.settings_len > 0))
      {
        gvib_.instance.time = HAL_GetTick();
        gVibSetPins(gvib_.settings[0].ch);
        gvib_.state = GVIB_STATE_STIMULATION;
        answer = true;
      }
      else if (gvib_.state == GVIB_STATE_PAUSE)
      {
        gvib_.instance.time = HAL_GetTick();
        gVibSetPins(gvib_.settings[gvib_.instance.settings_index].ch);
        gvib_.state = GVIB_STATE_STIMULATION;
        answer = true;
      }
      break;
    ///----------------------------------------------------
    case GVIB_STATE_PAUSE:
      if (gvib_.state == GVIB_STATE_STIMULATION)
      {
        HAL_GPIO_WritePin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN, GPIO_PIN_RESET);
        gVibSetPins(0x00);
        gvib_.state = GVIB_STATE_PAUSE;
        answer = true;
      }
      break;
    ///----------------------------------------------------
    default:
      break;
    }

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the constant triggering configuration and its length.
 * @param  enabled [out] Pointer to a variable where the constant trigger is enable or not.
 * @param  length [out] Pointer to a variable where the trigger length will be stored.
 * @return none.
 * @note   If the `length` parameter is not NULL, the function updates it with the current
 *         trigger length. If constant triggering is disabled, the length value remains unchanged.
 */

void gVibGetConstantTrig(uint8_t *enabled, uint16_t *length)
{
  if (length != NULL)
  {
    *length = gvib_.config.trig_length;
  }
  if (enabled != NULL)
  {
    *enabled = gvib_.config.is_constant_trig;
  }
}

/**
 * ************************************************************************************************
 * @brief  Configures the constant triggering setting.
 * @param  enable [in] Boolean value to enable or disable constant triggering.
 * @param  length [in] The length of the trigger in milliseconds.
 * @return None
 * @note   This function updates the configuration to enable or disable constant triggering
 *         and sets the duration for which it should be active.
 */

void gVibSetConstantTrig(bool enable, uint16_t length)
{
  gvib_.config.is_constant_trig = enable;
  gvib_.config.trig_length = length;
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the maximum number of stimulation channels configured.
 * @return The maximum number of channels.
 * @note   This function returns the value of the maximum number of stimulation channels
 *         set in the configuration.
 */

uint8_t gVibGetMaxCh(void)
{
  return gvib_.config.max_ch;
}

/**
 * ************************************************************************************************
 * @brief  Sets the maximum number of stimulation channels.
 * @param  max_ch [in] The maximum number of stimulation channels to be set.
 * @return true if the maximum channels were successfully set; false otherwise.
 * @note   This function validates the input value and ensures it is within the acceptable range
 *         (1 to `GVIB_STIMULATION_CHANNEL`). It does not apply the value if it is out of range.
 */

bool gVibSetMaxCh(uint8_t max_ch)
{
  bool answer = false;
  do
  {
    if ((max_ch == 0) || (max_ch > GVIB_STIMULATION_CHANNEL))
    {
      break;
    }
    gvib_.config.max_ch = max_ch;
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the length of the vibration sequence.
 * @return The length of the vibration sequence.
 * @note   This function returns the current length of the vibration sequence configured in the
 *         `gvib_` structure.
 */

uint16_t gVibGetSeqLen(void)
{
  return gvib_.config.sequence_len;
}

/**
 * ************************************************************************************************
 * @brief  Sets the length of the vibration sequence.
 * @param  len [in] The desired length of the vibration sequence (must be greater than 0).
 * @return true if the sequence length was set successfully, false otherwise.
 * @note   This function updates the length of the vibration sequence in the `gvib_` structure.
 *         A length of 0 is not allowed.
 */
bool gVibSetSeqLen(uint16_t len)
{
  bool answer = false;
  do
  {
    if (len == 0)
    {
      break;
    }
    gvib_.config.sequence_len = len;
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Retrieves vibration settings from the settings array.
 * @param  start_index [in] The starting index from which to retrieve settings.
 * @param  cnt [in] The number of settings to retrieve.
 * @param  settings [out] Pointer to an array where the retrieved settings will be stored.
 * @return true if the settings were retrieved successfully, false otherwise.
 * @note   This function checks if the input parameters are valid and retrieves the specified number
 *         of settings starting from the given index into the provided settings array.
 */

bool gVibGetSettings(uint16_t start_index, uint16_t cnt, GVibSettingsStruct *settings)
{
  bool answer = false;
  GVibSettingsStruct *settings_tmp = settings;
  do
  {
    if (settings == NULL)
    {
      break;
    }
    if (start_index + cnt > GVIB_MAX_SETTINGS)
    {
      break;
    }
    for (uint16_t i = 0; i < cnt; i++)
    {
      settings_tmp->ch = gvib_.settings[start_index + i].ch;
      settings_tmp->length = gvib_.settings[start_index + i].length;
      settings_tmp++;
    }
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Sets vibration settings in the settings array.
 * @param  cnt [in] The number of settings to set.
 * @param  settings [in] Pointer to an array containing the settings to be set.
 * @return true if the settings were set successfully, false otherwise.
 * @note   This function checks if the number of settings to be added exceeds the maximum allowed
 *         settings. It also validates the number of channels specified in each setting against
 *         the maximum channel configuration. If any setting fails validation, no settings are added.
 */

bool gVibSetSettings(uint16_t cnt, GVibSettingsStruct *settings)
{
  bool answer = false;
  bool error_max_ch = false;
  GVibSettingsStruct *settings_tmp = settings;
  do
  {
    if (cnt > (GVIB_MAX_SETTINGS - gvib_.instance.settings_len))
    {
      break;
    }
    for (uint16_t i = 0; i < cnt; i++)
    {
      uint8_t sum = 0;
      for (uint16_t found_idx = 0; found_idx < GVIB_STIMULATION_CHANNEL; found_idx++)
      {
        if ((settings_tmp->ch & (1 << found_idx)) != 0)
        {
          sum++;
        }
      }
      if (sum > gvib_.config.max_ch)
      {
        error_max_ch = true;
        break;
      }
      gvib_.settings[gvib_.instance.settings_len].ch = settings_tmp->ch;
      gvib_.settings[gvib_.instance.settings_len].length = settings_tmp->length;
      gvib_.instance.settings_len++;
      settings_tmp++;
    }
    if (!error_max_ch)
    {
      answer = true;
    }

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Configures a single vibration setting with a specified channel and duration.
 * @param  ch [in] The channel mask indicating which channels to activate (bitmask).
 * @param  duration [in] The duration for which the vibration should be active (in milliseconds).
 * @return true if the setting was applied successfully, false otherwise.
 * @note   This function checks if the number of channels specified does not exceed the maximum
 *         allowed channels. If the channel mask is zero or the duration is zero, it stops
 *         the vibration and resets the settings.
 */

bool gVibSingleSetting(uint8_t ch, uint16_t duration)
{
  bool answer = false;
  do
  {
    uint8_t sum = 0;
    for (uint16_t found_idx = 0; found_idx < GVIB_STIMULATION_CHANNEL; found_idx++)
    {
      if ((ch & (1 << found_idx)) != 0)
      {
        sum++;
      }
    }
    if (sum <= gvib_.config.max_ch)
    {
      gVibResetSettings();
      gvib_.settings[0].ch = ch;
      gvib_.settings[0].length = duration;
      gvib_.instance.settings_len = 1;
      if ((sum == 0) || (duration == 0))
      {
        gvib_.state = GVIB_STATE_STOP;
        gVibSetPins(0);
      }
      else
      {
        gvib_.state = GVIB_STATE_STIMULATION;
        gvib_.instance.time = HAL_GetTick();
        gVibSetPins(gvib_.settings[0].ch);
      }
      answer = true;
    }

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the current sequence index of the vibration settings.
 * @return The current sequence index.
 * @note   This function returns the index of the currently active sequence for the vibration
 *         settings, which can be used to track the progress of the vibration stimulation.
 */

uint16_t gVibGetSeqIndex(void)
{
  return gvib_.instance.sequence_index;
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the current settings index for the vibration settings.
 * @return The current settings index.
 * @note   This function returns the index of the currently active settings within the vibration
 *         configuration, which can be useful for tracking which settings are being applied during
 *         vibration stimulation.
 */

uint16_t gVibGetSettingsIndex(void)
{
  return gvib_.instance.settings_index;
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the LED on and off durations for the vibration system.
 * @param  on_duration [out] Pointer to store the LED on duration (ms).
 * @param  off_duration [out] Pointer to store the LED off duration (ms).
 * @return None
 * @note   This function populates the provided pointers with the current on and off durations
 *         for the LED, which are used to control the LED behavior during vibration stimulation.
 */
void gVibGetLedDuration(uint16_t *on_duration, uint16_t *off_duration)
{
  if (on_duration != NULL)
  {
    *on_duration = gvib_.config.led_on_duration;
  }
  if (off_duration != NULL)
  {
    *off_duration = gvib_.config.led_off_duration;
  }
}

/**
 * ************************************************************************************************
 * @brief  Sets the LED on and off durations for the vibration system.
 * @param  on_duration [in] The duration for which the LED should remain on (ms).
 * @param  off_duration [in] The duration for which the LED should remain off (ms).
 * @return None
 * @note   This function updates the configuration for the LED durations used during vibration
 *         stimulation, allowing for customizable LED behavior.
 */
void gVibSetLedDuration(uint16_t on_duration, uint16_t off_duration)
{
  gvib_.config.led_on_duration = on_duration;
  gvib_.config.led_off_duration = off_duration;
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the number of currently set vibration settings.
 * @return The length of the current settings in the vibration system.
 * @note   This function returns the count of settings that have been configured for the
 *         vibration system, which can be useful for managing or iterating through the settings.
 */
uint16_t gVibGetSettingsLen(void)
{
  return gvib_.instance.settings_len;
}

/**
 * ************************************************************************************************
 *
 */

/**
 * ************************************************************************************************
 * private function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Resets all the vibration settings to their default values.
 * @return None
 * @note   This function clears the settings array and resets the instance state,
 *         ensuring that all previously configured vibration settings are removed.
 *         It can be used to initialize the system before setting new configurations.
 */
void gVibResetSettings(void)
{
  memset(gvib_.settings, 0, sizeof(GVibSettingsStruct) * GVIB_MAX_SETTINGS);
  memset(&gvib_.instance, 0, sizeof(GVibInstanceStruct));
}

/**
 * ************************************************************************************************
 * @brief  Sets the GPIO pins for vibration stimulation based on the given mask.
 * @param  mask [in] A bitmask representing the channels to be activated.
 *                 Each bit corresponds to a stimulation channel; a value of 1 activates the channel,
 *                 while a value of 0 deactivates it.
 * @return None
 * @note   This function iterates through the configured GPIO pins and sets them high or low based
 *         on the provided mask. If no channels are activated, it handles the trigger pin accordingly,
 *         based on the constant trigger configuration.
 *         If a channel is active and constant triggering is enabled, it sets the trigger pin high
 *         and records the current time for potential timing purposes.
 */
void gVibSetPins(uint8_t mask)
{
  uint8_t count_ch = 0;
  for (int i = 0; i < GVIB_STIMULATION_CHANNEL; i++)
  {
    if (gvib_.gpio[i] != NULL)
    {
      if ((mask & (1 << i)) == 0)
      {
        HAL_GPIO_WritePin(gvib_.gpio[i], gvib_.pin[i], GPIO_PIN_RESET);
      }
      else
      {
        HAL_GPIO_WritePin(gvib_.gpio[i], gvib_.pin[i], GPIO_PIN_SET);
        count_ch++;
      }
    }
  }
  if (count_ch == 0)
  {
    if (gvib_.config.is_constant_trig == false)
    {
      if ((mask & (1 << GVIB_STIMULATION_CHANNEL)) == 0)
      {
        HAL_GPIO_WritePin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN, GPIO_PIN_RESET);
      }
      else
      {
        HAL_GPIO_WritePin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN, GPIO_PIN_SET);
      }
    }
  }
  else
  {
    if (gvib_.config.is_constant_trig == true)
    {
      gvib_.instance.time_trig = HAL_GetTick();
      HAL_GPIO_WritePin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN, GPIO_PIN_SET);
    }
    else
    {
      if ((mask & (1 << GVIB_STIMULATION_CHANNEL)) == 0)
      {
        HAL_GPIO_WritePin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN, GPIO_PIN_RESET);
      }
      else
      {
        HAL_GPIO_WritePin(GVIB_TRIG_GPIO, GVIB_TRIG_PIN, GPIO_PIN_SET);
      }
    }
  }
}

/**
 * ************************************************************************************************
 * @brief  OverCurrent callback
 * @return None
 * @note   This function is a weak function in mp8859 library. it calls after over current
 */
void mp8859CallbackOverCurrent(void)
{
  gVibSetState(GVIB_STATE_STOP);
  gComSetStatusCode(GCOM_STATUS_CODE_OVER_CURRENT);
}

