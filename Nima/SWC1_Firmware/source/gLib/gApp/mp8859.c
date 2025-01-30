
/**
 * @file    mp8859.c
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

#include "mp8859.h"
#include "i2c.h"

/**
 * ************************************************************************************************
 * private definitions
 * ************************************************************************************************
 */

bool          mp8859WriteReg(Mp8859Reg reg, uint8_t data);
bool          mp8859ReadReg(Mp8859Reg reg, uint8_t *data);

/**
 * ************************************************************************************************
 * private function prototypes
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * public function implementations
 * ************************************************************************************************
 */
bool mp8859Init(void)
{
  bool answer = false;
  do
  {
  	mp8859Enable(false);
    mp8859Enable(true);
    if (HAL_I2C_IsDeviceReady(&MP8859_I2C, MP8859_I2C_ADDRESS, 1, 10) != HAL_OK)
    {
      break;
    }
    if (mp8859SetVol(MP8859_DEFAULT_VOLTAGE) == false)
    {
      break;
    }
    if (mp8859SetCur(MP8859_DEFAULT_CURRENT) == false)
    {
      break;
    }
    mp8859Enable(true);
    answer = true;

  }
  while (0);

  return answer;
}

/**
 * ************************************************************************************************
 *
 */
void mp8859DeInit(void)
{
  mp8859Enable(false);
}


/**
 * ************************************************************************************************
 *
 */
void mp8859Loop(void)
{
  static uint32_t last_time = 0;
  static uint8_t power_good = 1;
  if (HAL_GetTick() - last_time >= MP8859_CHECK_TIME_MS)
  {
    uint8_t data = 0;
    if (mp8859ReadReg(MP8859_REG_STATUS, &data) == true)
    {
      if (((data & (1 << MP8859_STATUS_PG)) == 0) && (power_good == 1))
      {
        power_good = 0;
        mp8859CallbackPowerFail();
      }
      if (((data & (1 << MP8859_STATUS_PG)) != 0) && (power_good == 0))
      {
        power_good = 1;
      }
    }
  }
}

/**
 * ************************************************************************************************
 *
 */
bool mp8859SetVol(uint32_t voltage_mv)
{
  bool answer = false;
  do
  {
    if ((voltage_mv < MP8859_MIN_VOLTAGE) || (voltage_mv > MP8859_MAX_VOLTAGE))
    {
      break;
    }
    if (mp8859WriteReg(MP8859_REG_VOUT_L, (voltage_mv / 10) & 0x0003) == false)
    {
      break;
    }
    if (mp8859WriteReg(MP8859_REG_VOUT_H, (voltage_mv / 10) >> 3) == false)
    {
      break;
    }
    if (mp8859WriteReg(MP8859_REG_VOUT_GO, 0x01) == false)
    {
      break;
    }
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 *
 */
bool mp8859GetVol(uint32_t *voltage_mv)
{
  bool answer = false;
  uint8_t data[2];
  do
  {
    if (voltage_mv == NULL)
    {
      break;
    }
    if (mp8859ReadReg(MP8859_REG_VOUT_L, &data[0]) == false)
    {
      break;
    }
    if (mp8859ReadReg(MP8859_REG_VOUT_H, &data[1]) == false)
    {
      break;
    }
    *voltage_mv = ((uint16_t)data[1] << 3) | ((uint16_t)(data[0] & 0x03));
    *voltage_mv *= 10;
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 *
 */
bool mp8859SetCur(uint32_t current_ma)
{
  bool answer = false;
  do
  {
    if (current_ma > MP8859_MAX_CURRENT)
    {
      break;
    }
    if (mp8859WriteReg(MP8859_REG_IOUT_LIM, current_ma / 50) == false)
    {
      break;
    }
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 *
 */
bool mp8859GetCur(uint32_t *current_ma)
{
  bool answer = false;
  uint8_t data = 0;
  do
  {
    if (current_ma == NULL)
    {
      break;
    }
    if (mp8859ReadReg(MP8859_REG_IOUT_LIM, &data) == false)
    {
      break;
    }
    *current_ma = data * 50;
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 *
 */
void mp8859Enable(bool enable_disable)
{
  HAL_GPIO_WritePin(MP8859_EN_GPIO, MP8859_EN_PIN, (GPIO_PinState)enable_disable);
  HAL_Delay(1);
}

/**
 * ************************************************************************************************
 *
 */
void mp8859CallbackPowerFail(void)
{

}

/**
 * ************************************************************************************************
 * private function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 *
 */
bool mp8859WriteReg(Mp8859Reg reg, uint8_t data)
{
  bool answer = false;
  do
  {
    if (HAL_I2C_Mem_Write(&MP8859_I2C, MP8859_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 10) != HAL_OK)
    {
      break;
    }
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 *
 */
bool mp8859ReadReg(Mp8859Reg reg, uint8_t *data)
{
  bool answer = false;
  do
  {
    if (data == NULL)
    {
      break;
    }
    if (HAL_I2C_Mem_Read(&MP8859_I2C, MP8859_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, 1, 10) != HAL_OK)
    {
      break;
    }
    answer = true;

  } while (0);

  return answer;
}
