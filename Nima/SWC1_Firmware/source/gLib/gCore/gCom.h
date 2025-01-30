
/**
 * @file    gCom.h
 * @brief   This library contains all communication protocols between the MCU and PC,
 *          as well as commands and callbacks.
 *
 * @author  Nima Askari , Askari@gtec.at , Nima.Askari@gmail.com
 * @date    12/11/2024 10:25:43
 * @version 0.9.18
 *
 * @copyright
 *  Copyright (C) 2024 g.tec medical engineering GmbH.
 *  All rights reserved.
 *
 *  This software is proprietary to g.tec medical engineering GmbH. No part of this
 *  software may be reproduced, distributed, or transmitted in any form or by any means,
 *  without the prior written permission of g.tec medical engineering GmbH.
 */

#ifndef _GCOM_H_
#define _GCOM_H_

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

/* USER CODE BEGIN header */

/* USER CODE END header */

/**
* ************************************************************************************************
* library configuration
* ************************************************************************************************
*/

/* USER CODE BEGIN library configuration */

#define GCOM_DEBUG                    1

#define	GCOM_USART                    huart1
#define	GCOM_BUFFER_SIZE      			  2048
#define	GCOM_TX_TIMEOUT			  				50
#define	GCOM_SYNC_TIMEOUT		  				1000

#define GCOM_ENC_DEC                  1

/* USER CODE END library configuration */

#define GCOM_KEY                      { 0x4b, 0x5c, 0x38, 0xe4, 0x85, 0x8d, 0xc1, 0xf1,\
                                        0xc0, 0x07, 0x20, 0x6a, 0x52, 0xcd, 0xa8, 0xe9,\
                                        0xd4, 0xf6, 0x9a, 0xe3, 0x25, 0xa7, 0xf9, 0x47,\
                                        0xd0, 0x95, 0x17, 0x3a, 0x98, 0xb1, 0xbc, 0x38 }

#define GCOM_IV                       { 0xb3, 0xd1, 0xb1, 0x67, 0x4d, 0x7d, 0x3f, 0x47,\
                                        0x1c, 0x8d, 0x44, 0xe5, 0x97, 0x2d, 0x1e, 0xeb }

#define GCOM_RES_MIN_LENGTH           9
#define GCOM_RES_DATA_OFFSET          6
#define GCOM_RES_DATA_LEN_MAX         (GCOM_BUFFER_SIZE - GCOM_RES_MIN_LENGTH)
#define GCOM_REQ_MIN_LENGTH           7
#define GCOM_REQ_CMD_OFFSET           3
#define GCOM_REQ_DATA_OFFSET          4
#if GCOM_ENC_DEC == 0
#define GCOM_DATA_LEN                 (gcom_.instance.rx_len - GCOM_REQ_MIN_LENGTH)
#define GCOM_DATA_PTR                 &gcom_.instance.rx[GCOM_REQ_DATA_OFFSET]
#define GCOM_DATA_START_POINT         gcom_.instance.rx[GCOM_REQ_DATA_OFFSET]
#else
#define GCOM_DATA_LEN                 (gcom_.instance.rx_len_decrypted - 2)
#define GCOM_DATA_PTR                 &gcom_.instance.rx_decrypted[2]
#define GCOM_DATA_START_POINT         gcom_.instance.rx_decrypted[0]
#endif
#define GCOM_REQ_DATA_LEN_MAX         (GCOM_BUFFER_SIZE - GCOM_REQ_MIN_LENGTH)
#define GCOM_SOF_ID0                  0xF0
#define GCOM_SOF_BROADCAST            0xEF
#define GCOM_EOF                      0xF1

/**
 * ************************************************************************************************
 * struct/enum definitions
 * ************************************************************************************************
 */

typedef enum
{
  GCOM_CMD_SYNC                       = 0x00,
  GCOM_CMD_RESP                       = 0x01,
  GCOM_CMD_RESTART                    = 0x02,
  GCOM_CMD_GET_HW                     = 0x10,
  GCOM_CMD_SET_HW                     = 0x11,
  GCOM_CMD_GET_FW                     = 0x12,
  GCOM_CMD_GET_SN                     = 0x13,
  GCOM_CMD_SET_SN                     = 0x14,
  GCOM_CMD_GET_TEMP                   = 0x20,
  GCOM_CMD_GET_TEMP_THRESHOLD         = 0x21,
  GCOM_CMD_SET_TEMP_THRESHOLD         = 0x22,
	GCOM_CMD_SET_IV                     = 0x2D,
  GCOM_CMD_OPEN_PORT                  = 0x2E,
  GCOM_CMD_CLOSE_PORT                 = 0x2F,

  /* USER CODE BEGIN GComCmd commands */

	GCOM_CMD_VIB_SET_STATE              = 0x60,
	GCOM_CMD_VIB_GET_TRIG_CFG           = 0x61,
	GCOM_CMD_VIB_SET_TRIG_CFG           = 0x62,
	GCOM_CMD_VIB_GET_MAX_CH_CFG         = 0x63,
	GCOM_CMD_VIB_SET_MAX_CH_CFG         = 0x64,
	GCOM_CMD_VIB_GET_SEQ_LEN            = 0x65,
	GCOM_CMD_VIB_SET_SEQ_LEN            = 0x66,
	GCOM_CMD_VIB_GET_SETTINGS           = 0x67,
	GCOM_CMD_VIB_SET_SETTINGS           = 0x68,
	GCOM_CMD_VIB_GET_DC                 = 0x69,
	GCOM_CMD_VIB_SET_DC                 = 0x6A,
	GCOM_CMD_VIB_CLEAR_SETTINGS         = 0x6B,
	GCOM_CMD_VIB_SINGLE_SETTING         = 0x6C,
	GCOM_CMD_VIB_GET_LED_DURATION       = 0x6D,
	GCOM_CMD_VIB_SET_LED_DURATION       = 0x6E,

  /* USER CODE END GComCmd commands */

} GComCmd;

typedef enum
{
  GCOM_ERR_NONE                       = 0x0000,
  GCOM_ERR_BYTE_TIMEOUT               = 0x0001,
  GCOM_ERR_FRAME_TIMEOUT              = 0x0002,
  GCOM_ERR_CRC                        = 0x0003,
  GCOM_ERR_EOF                        = 0x0004,
  GCOM_ERR_SEMANTIC                   = 0x0005,
  GCOM_ERR_GENERAL                    = 0x0006,                     ///< only API
  GCOM_ERR_DISCONNECTED               = 0x0007,                     ///< only API
  GCOM_ERR_DONGLE_MISSING             = 0x0008,                     ///< only API
  GCOM_ERR_INVALID_DEVICE_INDEX       = 0x0009,                     ///< only API
  GCOM_ERR_MEMORY                     = 0x000A,
  GCOM_ERR_FRAME_MISSED               = 0x000B,
  GCOM_ERR_UNKNOWN                    = 0xFFFF,

  /* USER CODE BEGIN GComError error code */

	GCOM_ERR_VIB                				= 0x1060,

  /* USER CODE END GComError error code */

} GComError;

typedef enum
{
  GCOM_STATUS_CODE_NO_PING            = 0x1005,
  GCOM_STATUS_CODE_WDT                = 0x1006,
  GCOM_STATUS_CODE_EXT_RST            = 0x1009,
  GCOM_STATUS_CODE_FW                 = 0x100A,
  GCOM_STATUS_CODE_BROWN_OUT          = 0x100B,
  GCOM_STATUS_CODE_POWER_ON           = 0x100C,
  GCOM_STATUS_CODE_NO_ERROR           = 0xFFFF,

  /* USER CODE BEGIN GComStatusCode status code */

  /* USER CODE END GComStatusCode status code */

} GComStatusCode;

typedef enum
{
  GCOM_STATE_IDLE                     = 0,
  GCOM_STATE_GOT_DATA,
  GCOM_STATE_SEMANTIC_CHECK,
  GCOM_STATE_SENDING,
  GCOM_STATE_ERROR,

} GComState;

typedef struct
{
  uint8_t                             rx[GCOM_BUFFER_SIZE];
  uint8_t                             tx[GCOM_BUFFER_SIZE];
#if GCOM_ENC_DEC == 1
  uint8_t                             rx_decrypted[GCOM_BUFFER_SIZE];
  uint8_t                             tx_encrypted[GCOM_BUFFER_SIZE];
  uint32_t                            rx_len_decrypted;
  uint32_t                            tx_len_encrypted;
  uint32_t                            crypto_counter;
#endif
  uint32_t                            rx_len;
  uint32_t                            tx_len;
  uint32_t                            tx_done;
  uint32_t                            tx_time;
  uint32_t                            sync_time;

} GComInstance;

typedef struct
{
  GComInstance                        instance;
  GComState                           state;
  GComError                           error;
  uint8_t                             restart_req;
  uint8_t                             is_broadcast;
  uint8_t                             is_open;
  uint8_t                             id;
  uint8_t                             sync_timeout_called;
  uint16_t                            status_code;
#if GCOM_ENC_DEC == 1
  uint8_t                             iv[16];
  uint8_t                             iv_tmp[16];
#endif

} GComHandle;

/**
* ************************************************************************************************
* public variable
* ************************************************************************************************
*/

extern GComHandle  gcom_;

/* USER CODE BEGIN public variable */

/* USER CODE END public variable */

/**
* ************************************************************************************************
* public library functions
* ************************************************************************************************
*/

void          gComInit(void);
void          gComDeInit(void);
void          gComLoop(void);
bool          gComRespMake(uint16_t error, const uint8_t *data, uint16_t length);
void          gComSetStatusCode(uint16_t status_code);
uint16_t      gComGetStatusCode(void);

void          gComCallbackCmd(GComCmd cmd, const uint8_t *data, uint16_t length);
void          gComCallbackSyncTimeout(void);

/* USER CODE BEGIN functions */

/* USER CODE END functions */

#ifdef __cplusplus
}
#endif
#endif
