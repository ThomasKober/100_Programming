
/**
 * @file    gComCallback.c
 * @brief   This library contains all communication protocols between the MCU and PC,
 *          as well as commands and callbacks.
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

#include "gCom.h"
#include "gInfo.h"
#include "gTemp.h"
#include "gBuz.h"
#include <string.h>

/* USER CODE BEGIN header */

#include "gVib.h"
#include "mp8859.h"
#include <stdlib.h>

/* USER CODE END header */

/**
 * ************************************************************************************************
 * private definitions
 * ************************************************************************************************
 */

/* USER CODE BEGIN private definitions */

/* USER CODE END private definitions */

/**
 * ************************************************************************************************
 * private function prototypes
 * ************************************************************************************************
 */

void          gComCallbackSync(const uint8_t *data, uint16_t length);
void          gComCallbackRestart(const uint8_t *data, uint16_t length);
void          gComCallbackGetHw(const uint8_t *data, uint16_t length);
void          gComCallbackSetHw(const uint8_t *data, uint16_t length);
void          gComCallbackGetFw(const uint8_t *data, uint16_t length);
void          gComCallbackGetSn(const uint8_t *data, uint16_t length);
void          gComCallbackSetSn(const uint8_t *data, uint16_t length);
void          gComCallbackGetTempThreshold(const uint8_t *data, uint16_t length);
void          gComCallbackSetTempThreshold(const uint8_t *data, uint16_t length);
void          gComCallbackSetIv(const uint8_t *data, uint16_t length);
void          gComCallbackOpenPort(const uint8_t *data, uint16_t length);
void          gComCallbackClosePort(const uint8_t *data, uint16_t length);

/* USER CODE BEGIN private function prototypes */

void 					gComCallbackVibSetState(const uint8_t *data, uint16_t length);
void 					gComCallbackVibGetTrigCfg(const uint8_t *data, uint16_t length);
void 					gComCallbackVibSetTrigCfg(const uint8_t *data, uint16_t length);
void 					gComCallbackVibGetMaxChCfg(const uint8_t *data, uint16_t length);
void 					gComCallbackVibSetMaxChCfg(const uint8_t *data, uint16_t length);
void 					gComCallbackVibGetSeqLen(const uint8_t *data, uint16_t length);
void 					gComCallbackVibSetSeqLen(const uint8_t *data, uint16_t length);
void 					gComCallbackVibGetSettings(const uint8_t *data, uint16_t length);
void 					gComCallbackVibSetSettings(const uint8_t *data, uint16_t length);
void 					gComCallbackVibGetDc(const uint8_t *data, uint16_t length);
void 					gComCallbackVibSetDc(const uint8_t *data, uint16_t length);
void 					gComCallbackVibClearSettings(const uint8_t *data, uint16_t length);
void 					gComCallbackVibSingleSettings(const uint8_t *data, uint16_t length);
void 					gComCallbackVibGetLedDuration(const uint8_t *data, uint16_t length);
void 					gComCallbackVibSetLedDuration(const uint8_t *data, uint16_t length);
void          gComCallbackVibIsOverCurrent(const uint8_t *data, uint16_t length);

/* USER CODE END private function prototypes */

/**
 * ************************************************************************************************
 * global variable
 * ************************************************************************************************
 */

/* USER CODE BEGIN global variable */

/* USER CODE END global variable */

/**
 * ************************************************************************************************
 * callback function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackSyncTimeout(void)
{
  gComSetStatusCode(GCOM_STATUS_CODE_NO_PING);

  /* USER CODE BEGIN gComSyncTimeoutCallback */
	gVibSetState(GVIB_STATE_STOP);
  /* USER CODE END gComSyncTimeoutCallback */
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackCmd(GComCmd cmd, const uint8_t *data, uint16_t length)
{
  switch (cmd)
  {
  //------------------------------------------------------------------
  case GCOM_CMD_SYNC:
    gComCallbackSync(data, length);
    break;
  //------------------------------------------------------------------
  case GCOM_CMD_RESTART:
    gComCallbackRestart(data, length);
    break;
  //------------------------------------------------------------------
  case GCOM_CMD_GET_HW:
    gComCallbackGetHw(data, length);
    break;
  //------------------------------------------------------------------
  case GCOM_CMD_SET_HW:
    gComCallbackSetHw(data, length);
    break;
  //------------------------------------------------------------------
  case GCOM_CMD_GET_FW:
    gComCallbackGetFw(data, length);
    break;
  //------------------------------------------------------------------
  case GCOM_CMD_GET_SN:
    gComCallbackGetSn(data, length);
    break;
  //------------------------------------------------------------------
  case GCOM_CMD_SET_SN:
    gComCallbackSetSn(data, length);
    break;
  //------------------------------------------------------------------
  case GCOM_CMD_GET_TEMP_THRESHOLD:
    gComCallbackGetTempThreshold(data, length);
    break;
  //------------------------------------------------------------------
  case GCOM_CMD_SET_TEMP_THRESHOLD:
    gComCallbackSetTempThreshold(data, length);
    break;
	//------------------------------------------------------------------
	case GCOM_CMD_SET_IV:
		gComCallbackSetIv(data, length);
		break;
  //------------------------------------------------------------------
  case GCOM_CMD_OPEN_PORT:
    gComCallbackOpenPort(data, length);
    break;
  //------------------------------------------------------------------
  case GCOM_CMD_CLOSE_PORT:
    gComCallbackClosePort(data, length);
    break;
  //------------------------------------------------------------------
  /* USER CODE BEGIN gComCallbackCmd */

	//------------------------------------------------------------------
	case GCOM_CMD_VIB_SET_STATE:
		gComCallbackVibSetState(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_GET_TRIG_CFG:
		gComCallbackVibGetTrigCfg(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_SET_TRIG_CFG:
		gComCallbackVibSetTrigCfg(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_GET_MAX_CH_CFG:
		gComCallbackVibGetMaxChCfg(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_SET_MAX_CH_CFG:
		gComCallbackVibSetMaxChCfg(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_GET_SEQ_LEN:
		gComCallbackVibGetSeqLen(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_SET_SEQ_LEN:
		gComCallbackVibSetSeqLen(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_GET_SETTINGS:
		gComCallbackVibGetSettings(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_SET_SETTINGS:
		gComCallbackVibSetSettings(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_GET_DC:
		gComCallbackVibGetDc(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_SET_DC:
		gComCallbackVibSetDc(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_CLEAR_SETTINGS:
		gComCallbackVibClearSettings(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_SINGLE_SETTING:
		gComCallbackVibSingleSettings(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_GET_LED_DURATION:
		gComCallbackVibGetLedDuration(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_SET_LED_DURATION:
		gComCallbackVibSetLedDuration(data, length);
		break;
	//------------------------------------------------------------------
	case GCOM_CMD_VIB_IS_OVER_CURRENT:

	  break;
	//------------------------------------------------------------------
  /* USER CODE END gComCallbackCmd */
  //------------------------------------------------------------------
  default:

    break;
  }
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackSync(const uint8_t *data, uint16_t length)
{
  /* USER CODE BEGIN gComCallbackSync */
	uint8_t answer[11];
	if (length == 0)
	{
		answer[0] = gComGetStatusCode() >> 8;
		answer[1] = gComGetStatusCode() & 0x00FF;
		answer[2] = gVibGetState();
		answer[3] = gTempGetDesiCelsius() >> 8;
		answer[4] = gTempGetDesiCelsius() & 0x00FF;
		answer[5] = gVibGetSeqIndex() >> 8;
		answer[6] = gVibGetSeqIndex() & 0x00FF;
		answer[7] = gVibGetSettingsIndex() >> 8;
		answer[8] = gVibGetSettingsIndex() & 0x00FF;
		answer[9] = gVibGetSettingsLen() >> 8;
		answer[10] = gVibGetSettingsLen() & 0x00FF;
		gComRespMake(GCOM_ERR_NONE, answer, sizeof(answer));
	}
  /* USER CODE END gComCallbackSync */

  if (gComGetStatusCode() != GCOM_STATUS_CODE_NO_ERROR)
  {
    gcom_.status_code = GCOM_STATUS_CODE_NO_ERROR;
  }
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackRestart(const uint8_t *data, uint16_t length)
{
  if (length == 0)
  {
    gComRespMake(GCOM_ERR_NONE, NULL, 0);
    gcom_.restart_req = 1;
  }
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackGetHw(const uint8_t *data, uint16_t length)
{
  char str[32];
  memset(str, 0, 32);
  if (length == 0)
  {
    if (gInfoReadHw(str) == true)
    {
      gComRespMake(GCOM_ERR_NONE, (uint8_t*) str, strlen(str));
    }
    else
    {
      gComRespMake(GCOM_ERR_MEMORY, NULL, 0);
    }
  }
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackSetHw(const uint8_t *data, uint16_t length)
{
  if ((length < 32) && (length > 0))
  {
    char tmp[32];
    memset(tmp, 0, 32);
    memcpy(tmp, data, length);
    if (gInfoWriteHw(tmp))
    {
      gComRespMake(GCOM_ERR_NONE, NULL, 0);
    }
    else
    {
      gComRespMake(GCOM_ERR_MEMORY, NULL, 0);
    }
  }
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackGetFw(const uint8_t *data, uint16_t length)
{
  char str[32];
  memset(str, 0, 32);
  if (length == 0)
  {
    if (gInfoReadFw(str) == true)
    {
      gComRespMake(GCOM_ERR_NONE, (uint8_t*) str, strlen(str));
    }
    else
    {
      gComRespMake(GCOM_ERR_MEMORY, NULL, 0);
    }
  }
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackGetSn(const uint8_t *data, uint16_t length)
{
  char str[32];
  memset(str, 0, 32);
  if (length == 0)
  {
    if (gInfoReadSn(str) == true)
    {
      gComRespMake(GCOM_ERR_NONE, (uint8_t*) str, strlen(str));
    }
    else
    {
      gComRespMake(GCOM_ERR_MEMORY, NULL, 0);
    }
  }
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackSetSn(const uint8_t *data, uint16_t length)
{
  if ((length < 32) && (length > 0))
  {
    char tmp[32];
    memset(tmp, 0, 32);
    memcpy(tmp, data, length);
    if (gInfoWriteSn(tmp))
    {
      gComRespMake(GCOM_ERR_NONE, NULL, 0);
    }
    else
    {
      gComRespMake(GCOM_ERR_MEMORY, NULL, 0);
    }
  }
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackGetTempThreshold(const uint8_t *data, uint16_t length)
{
  if (length == 0)
  {
    uint8_t tmp[2];
    tmp[0] = gTempGetAlarmDesiCelsius() >> 8;
    tmp[1] = gTempGetAlarmDesiCelsius() & 0x00FF;
    gComRespMake(GCOM_ERR_NONE, tmp, 2);
  }
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackSetTempThreshold(const uint8_t *data, uint16_t length)
{
  if (length == 2)
  {
    ginfo_storage_.temp_threshold = (data[0] << 8) | data[1];
    if (gInfoSave())
    {
      gTempSetAlarmDesiCelsius(ginfo_storage_.temp_threshold);
      gComRespMake(GCOM_ERR_NONE, NULL, 0);
    }
    else
    {
      gComRespMake(GCOM_ERR_MEMORY, NULL, 0);
    }
  }
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackSetIv(const uint8_t *data, uint16_t length)
{
  if (length == 16)
  {
    if (gInfoWriteIv(data))
    {
      gComRespMake(GCOM_ERR_NONE, NULL, 0);
    }
    else
    {
      gComRespMake(GCOM_ERR_MEMORY, NULL, 0);
    }
  }
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackOpenPort(const uint8_t *data, uint16_t length)
{
#if GCOM_ENC_DEC == 1
  if (length == 16)
  {
    gComRespMake(GCOM_ERR_NONE, NULL, 0);
    gcom_.is_open = 1;
  }
#else
  if (length == 0)
  {
    gComRespMake(GCOM_ERR_NONE, NULL, 0);
    gcom_.is_open = 1;
  }
#endif
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
void gComCallbackClosePort(const uint8_t *data, uint16_t length)
{
  if (length == 0)
  {
    gComRespMake(GCOM_ERR_NONE, NULL, 0);
    gcom_.is_open = 0;
  }
}

/* USER CODE BEGIN callback function implementation */

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibSetState(const uint8_t *data, uint16_t length)
{
	if (length == 1)
	{
		if (gVibSetState(data[0]) == true)
		{
			gComRespMake(GCOM_ERR_NONE, NULL, 0);
		}
		else
		{
			gComRespMake(GCOM_ERR_VIB, NULL, 0);
		}
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibGetTrigCfg(const uint8_t *data, uint16_t length)
{
	if (length == 0)
	{
		uint16_t trig_len;
		uint8_t answer[3];
		gVibGetConstantTrig(&answer[0], &trig_len);
		answer[1] = trig_len >> 8;
		answer[2] = trig_len & 0x00FF;
		gComRespMake(GCOM_ERR_NONE, answer, sizeof(answer));
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibSetTrigCfg(const uint8_t *data, uint16_t length)
{
	if (length == 3)
	{
	  ginfo_storage_.config.is_constant_trig = (bool)data[0];
    ginfo_storage_.config.trig_length = (data[1] << 8) | data[2];
		gVibSetConstantTrig(ginfo_storage_.config.is_constant_trig, ginfo_storage_.config.trig_length);
		if (gInfoSave())
		{
		  gComRespMake(GCOM_ERR_NONE, NULL, 0);
		}
		else
		{
		  gComRespMake(GCOM_ERR_MEMORY, NULL, 0);
		}
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibGetMaxChCfg(const uint8_t *data, uint16_t length)
{
	if (length == 0)
	{
		uint8_t answer[1];
		answer[0] = gVibGetMaxCh();
		gComRespMake(GCOM_ERR_NONE, answer, sizeof(answer));
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibSetMaxChCfg(const uint8_t *data, uint16_t length)
{
	if (length == 1)
	{
	  ginfo_storage_.config.max_ch = data[0];
	  if (gInfoSave())
	  {
	    if (gVibSetMaxCh(data[0]))
      {
        gComRespMake(GCOM_ERR_NONE, NULL, 0);
      }
      else
      {
        gComRespMake(GCOM_ERR_VIB, NULL, 0);
      }
	  }
	  else
	  {
	    gComRespMake(GCOM_ERR_MEMORY, NULL, 0);
	  }
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibGetSeqLen(const uint8_t *data, uint16_t length)
{
	if (length == 0)
	{
		uint8_t answer[2];
		answer[0] = gVibGetSeqLen() >> 8;
		answer[1] = gVibGetSeqLen() & 0x00FF;
		gComRespMake(GCOM_ERR_NONE, answer, sizeof(answer));
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibSetSeqLen(const uint8_t *data, uint16_t length)
{
	if (length == 2)
	{
		if (gVibSetSeqLen((data[0] << 8) | data[1]))
		{
			gComRespMake(GCOM_ERR_NONE, NULL, 0);
		}
		else
		{
			gComRespMake(GCOM_ERR_VIB, NULL, 0);
		}
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibGetSettings(const uint8_t *data, uint16_t length)
{
	if (length == 4)
	{
		uint16_t start_index = (data[0] >> 8) | data[1];
		uint16_t cnt = (data[2] >> 8) | data[3];
		GVibSettingsStruct settings[(GCOM_BUFFER_SIZE - 8) / sizeof(GVibSettingsStruct)];
		if (gVibGetSettings(start_index, cnt, settings) == true)
		{
			gComRespMake(GCOM_ERR_NONE, (uint8_t*)settings, cnt * sizeof(GVibSettingsStruct));
		}
		else
		{
			gComRespMake(GCOM_ERR_VIB, NULL, 0);
		}
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibSetSettings(const uint8_t *data, uint16_t length)
{
	if (length % 3 == 0)
	{
		GVibSettingsStruct *settings = malloc((GCOM_BUFFER_SIZE - 8));
		if (settings != NULL)
		{
			for (uint16_t i = 0; i < length / 3; i++)
			{
				settings[i].ch = data[i * 3 + 0];
				settings[i].length = (data[i * 3 + 1] << 8) | (data[i * 3 + 2] & 0x00FF);
			}
			if (gVibSetSettings(length / 3, settings) == true)
			{
				gComRespMake(GCOM_ERR_NONE, NULL, 0);
			}
			else
			{
				gComRespMake(GCOM_ERR_VIB, NULL, 0);
			}
			free(settings);
		}
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibGetDc(const uint8_t *data, uint16_t length)
{
	if (length == 0)
	{
		bool error = true;
		uint8_t answer[4];
		uint32_t voltage, current;
		do
		{
			if (mp8859GetVol(&voltage) == false)
			{
				break;
			}
			if (mp8859GetCur(&current) == false)
			{
				break;
			}
			answer[0] = ((uint16_t)voltage >> 8);
			answer[1] = ((uint16_t)voltage & 0x00FF);
			answer[2] = ((uint16_t)current >> 8);
			answer[3] = ((uint16_t)current & 0x00FF);
			error = false;

		} while (0);

		if (!error)
		{
			gComRespMake(GCOM_ERR_NONE, answer, sizeof(answer));
		}
		else
		{
			gComRespMake(GCOM_ERR_VIB, NULL, 0);
		}
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibSetDc(const uint8_t *data, uint16_t length)
{
	if (length == 4)
	{
		uint32_t voltage = (data[0] << 8) | data[1];
		uint32_t current = (data[2] << 8) | data[3];
		do
		{
		  ginfo_storage_.config.voltage_mv = voltage;
		  ginfo_storage_.config.current_ma = current;
		  if (gInfoSave())
      {
        if (mp8859SetVol(voltage) == false)
        {
          gComRespMake(GCOM_ERR_VIB, NULL, 0);
          break;
        }
        if (mp8859SetCur(current) == false)
        {
          gComRespMake(GCOM_ERR_VIB, NULL, 0);
          break;
        }
        gComRespMake(GCOM_ERR_NONE, NULL, 0);
      }
		  else
		  {
		    gComRespMake(GCOM_ERR_MEMORY, NULL, 0);
		  }

		} while (0);
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibClearSettings(const uint8_t *data, uint16_t length)
{
	if (length == 0)
	{
		if (gVibClearSettings())
		{
			gComRespMake(GCOM_ERR_NONE, NULL, 0);
		}
		else
		{
			gComRespMake(GCOM_ERR_VIB, NULL, 0);
		}
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibSingleSettings(const uint8_t *data, uint16_t length)
{
	if (length == 3)
	{
		uint16_t duration = (data[1] << 8) | data[2];
		if (gVibSingleSetting(data[0], duration))
		{
			gComRespMake(GCOM_ERR_NONE, NULL, 0);
		}
		else
		{
			gComRespMake(GCOM_ERR_VIB, NULL, 0);
		}
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibGetLedDuration(const uint8_t *data, uint16_t length)
{
	if (length == 0)
	{
		uint16_t on_duration, off_duration;
		uint8_t answer[4];
		gVibGetLedDuration(&on_duration, &off_duration);
		answer[0] = on_duration >> 8;
		answer[1] = on_duration & 0x00FF;
		answer[2] = off_duration >> 8;
		answer[3] = off_duration & 0x00FF;
		gComRespMake(GCOM_ERR_NONE, answer, sizeof(answer));
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibSetLedDuration(const uint8_t *data, uint16_t length)
{
	if (length == 4)
	{
		uint16_t on_duration = (data[0] << 8) | data[1];
		uint16_t off_duration = (data[2] << 8) | data[3];
		ginfo_storage_.config.led_off_duration = off_duration;
		ginfo_storage_.config.led_on_duration = on_duration;
		if (gInfoSave())
		{
		  gVibSetLedDuration(on_duration, off_duration);
		  gComRespMake(GCOM_ERR_NONE, NULL, 0);
		}
		else
		{
		  gComRespMake(GCOM_ERR_MEMORY, NULL, 0);
		}
	}
}

/**
 * ************************************************************************************************
 *
 */
void gComCallbackVibIsOverCurrent(const uint8_t *data, uint16_t length)
{
  if (length == 0)
  {
    uint8_t answer[1];
    answer[0] = mp8859IsOverCurrent();
    gComRespMake(GCOM_ERR_NONE, answer, sizeof(answer));
  }
}

/* USER CODE END callback function implementation */
