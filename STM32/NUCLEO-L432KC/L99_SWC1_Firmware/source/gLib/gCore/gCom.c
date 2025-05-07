
/**
 * @file    gCom.c
 * @brief   This library contains all communication protocols between the MCU and PC,
 *          as well as commands and callbacks.
 *
 * @author  Nima Askari , Askari@gtec.at , Nima.Askari@gmail.com
 * @date    21/03/2025 09:27:51
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

#include "gCore.h"
#include "gCom.h"
#include "gInfo.h"
#include "cmox_crypto.h"
#include "crc.h"
#include "usart.h"
#include <string.h>

/**
 * ************************************************************************************************
 * private definitions
 * ************************************************************************************************
 */

#if ((defined DEBUG) && (GCOM_DEBUG == 1))
#include <stdio.h>
#define dprintf(...)                  printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

/**
 * ************************************************************************************************
 * private function prototypes
 * ************************************************************************************************
 */

void          gComRxEventCallback(UART_HandleTypeDef *huart, uint16_t size);
void          gComTxDoneCallback(UART_HandleTypeDef *huart);
void          gComErrorCallback(UART_HandleTypeDef *huart);
bool          gComRxEnable(void);
bool          gComRespSend(void);
void          gComStateIdle(void);
void          gComStateGotData(void);
void          gComStateSemanticCheck(void);
void          gComStateSending(void);
void          gComStateError(void);
bool          gComRxEnable(void);

cmox_init_retval_t cmox_ll_init(void *pArg);

/**
 * ************************************************************************************************
 * global variable
 * ************************************************************************************************
 */

GComHandle    gcom_;
const uint8_t gcom_key_[] = GCOM_KEY;
const uint8_t gcom_iv_[] = GCOM_IV;

/**
 * ************************************************************************************************
 * public function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Initializes the communication system
 * @param  None
 * @return None
 * @note   This function sets up the communication handle, initializes the UART and CRC peripherals,
 *         and configures the necessary callbacks for communication events. It also checks for valid
 *         identifiers and handles errors appropriately by halting the core if any initialization step fails.
 */
void gComInit(void)
{
  uint8_t id = GCOM_SOF_ID0;

  /* USER CODE BEGIN read id */

  /* USER CODE END read id */

  if (id < GCOM_SOF_ID0)
  {
    gCoreHalt();
  }
  memset(&gcom_, 0, sizeof(GComHandle));
  MX_CRC_Init();
  MX_USART1_UART_Init();
  gcom_.id = id;
  gcom_.status_code = GCOM_STATUS_CODE_NO_ERROR;
  gcom_.instance.sync_time = HAL_GetTick();
#if GCOM_ENC_DEC == 1
  if (!gInfoReadIv(gcom_.iv))
  {
  	gCoreHalt();
  }
  if (!gInfoReadIv(gcom_.iv_tmp))
	{
		gCoreHalt();
	}
#endif
  if (HAL_CRCEx_Polynomial_Set(&hcrc, 0x1021, CRC_POLYLENGTH_16B) != HAL_OK)
  {
    gCoreHalt();
  }
  if (HAL_UART_RegisterRxEventCallback(&GCOM_USART, gComRxEventCallback) != HAL_OK)
  {
    gCoreHalt();
  }
  if (HAL_UART_RegisterCallback(&GCOM_USART, HAL_UART_TX_COMPLETE_CB_ID, gComTxDoneCallback)
      != HAL_OK)
  {
    gCoreHalt();
  }
  if (HAL_UART_RegisterCallback(&GCOM_USART, HAL_UART_ERROR_CB_ID, gComErrorCallback) != HAL_OK)
  {
    gCoreHalt();
  }
  if (gComRxEnable() == false)
  {
    gCoreHalt();
  }
}

/**
 * ************************************************************************************************
 * @brief  De-initializes the communication system
 * @param  None
 * @return None
 * @note   This function releases any resources allocated for the communication system,
 *         stops the UART peripherals, and clears the communication handle to ensure
 *         that the system is ready for re-initialization if needed.
 */
void gComDeInit(void)
{
  HAL_UART_Abort(&GCOM_USART);
  HAL_UART_UnRegisterRxEventCallback(&GCOM_USART);
  HAL_UART_UnRegisterCallback(&GCOM_USART, HAL_UART_TX_COMPLETE_CB_ID);
  HAL_UART_UnRegisterCallback(&GCOM_USART, HAL_UART_ERROR_CB_ID);
  memset(&gcom_, 0, sizeof(GComHandle));
}

/**
 * ************************************************************************************************
 * @brief  Main loop for handling the communication state machine
 * @param  None
 * @return None
 * @note   This function processes the current state of the communication system and
 *         invokes the corresponding state handling functions. The state transitions and
 *         actions are based on the value of `gcom_.state`, ensuring that the system responds
 *         appropriately to incoming data and events.
 */
void gComLoop(void)
{
  switch (gcom_.state)
  {
    ///<------------------------------------------------------------------
  case GCOM_STATE_IDLE:
    gComStateIdle();
    break;
    ///<------------------------------------------------------------------
  case GCOM_STATE_GOT_DATA:
    gComStateGotData();
    break;
    ///<------------------------------------------------------------------
  case GCOM_STATE_SEMANTIC_CHECK:
    gComStateSemanticCheck();
    break;
    ///<------------------------------------------------------------------
  case GCOM_STATE_SENDING:
    gComStateSending();
    break;
    ///<------------------------------------------------------------------
  case GCOM_STATE_ERROR:
    gComStateError();
    break;
    ///<------------------------------------------------------------------
  }
}

/**
 * ************************************************************************************************
 * @brief  Constructs a communication response
 * @param  error [in] Error code to be included in the response
 * @param  data [in] Pointer to the data to be included in the response (can be NULL)
 * @param  length [in] Length of the data to be included in the response (can be 0)
 * @return true if the response was successfully constructed, false otherwise
 * @note   This function formats the response packet with the specified error code
 *         and data, preparing it for transmission over the communication channel.
 *         If the data pointer is NULL or the length is 0, the response will be created
 *         only for the error code without any additional data.
 */
bool gComRespMake(uint16_t error, const uint8_t *data, uint16_t length)
{
  bool answer = false;
  do
  {
    uint16_t crc = 0;
    gcom_.instance.tx_done = 0;
    gcom_.instance.tx_len = 0;
    if (data == NULL)
    {
      length = 0;
    }
    if (length >= GCOM_BUFFER_SIZE - GCOM_RES_MIN_LENGTH)
    {
      break;
    }
    gcom_.instance.tx[0] = gcom_.id;
    gcom_.instance.tx[1] = (length + 6) >> 8;
    gcom_.instance.tx[2] = (length + 6) & 0x00FF;
    gcom_.instance.tx[3] = GCOM_CMD_RESP;
    gcom_.instance.tx[4] = error >> 8;
    gcom_.instance.tx[5] = error & 0x00FF;

#if GCOM_ENC_DEC == 1
    cmox_cipher_retval_t cipher_result;
    if (length != 0)
    {
      for (uint16_t i = 0; i < length; i++)
      {
        gcom_.instance.tx_encrypted[i + 6] = data[i];
      }
      cmox_initialize(NULL);
			if (HAL_CRCEx_Polynomial_Set(&hcrc, 0x4c11db7, CRC_POLYLENGTH_32B) != HAL_OK)
			{
				break;
			}
			cipher_result = cmox_cipher_encrypt(CMOX_AES_CTR_ENC_ALGO,
                                          &gcom_.instance.tx_encrypted[6], length,
																					gcom_key_, sizeof(gcom_key_),
																					gcom_.iv, sizeof(gcom_.iv),
																					&gcom_.instance.tx[6], (size_t*) &gcom_.instance.tx_len_encrypted);

			if (cipher_result != CMOX_CIPHER_SUCCESS)
			{
				break;
			}
			if (gcom_.instance.tx_len_encrypted != (length))
			{
				break;
			}

    }
#else
    if (length != 0)
    {
      for (uint16_t i = 0; i < length; i++)
      {
        gcom_.instance.tx[i + 6] = data[i];
      }
    }
#endif
    __HAL_CRC_DR_RESET(&hcrc);
    if (HAL_CRCEx_Polynomial_Set(&hcrc, 0x1021, CRC_POLYLENGTH_16B) != HAL_OK)
    {
      break;
    }
    crc = HAL_CRC_Calculate(&hcrc, (uint32_t*) &gcom_.instance.tx[1], 5 + length);
    gcom_.instance.tx[6 + length] = crc >> 8;
    gcom_.instance.tx[7 + length] = crc & 0x00FF;
    gcom_.instance.tx[8 + length] = GCOM_EOF;
    gcom_.instance.tx_len = GCOM_RES_MIN_LENGTH + length;
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Sets the status code for the communication system
 * @param  status_code [in] The status code to be set, represented as a 16-bit unsigned integer
 * @return None
 * @note   This function updates the internal status code used in the communication
 *         process. The status code can be used to convey the result of operations
 *         or the current state of the communication system. After sending the status
 *         code once, it resets to `0xFFFF`. The status code is always sent after the
 *         SYNC command.
 */
void gComSetStatusCode(uint16_t status_code)
{
  if (gcom_.status_code == GCOM_STATUS_CODE_NO_ERROR)
  {
    gcom_.status_code = status_code;
  }
}

/**
 * ************************************************************************************************
 * @brief  Retrieves the current status code of the communication system
 * @param  None
 * @return The current status code as a 16-bit unsigned integer
 * @note   This function returns the internal status code that indicates the result
 *         of the last operation or the current state of the communication system.
 */
uint16_t gComGetStatusCode(void)
{
  return gcom_.status_code;
}

/**
 * ************************************************************************************************
 * private function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Callback function for UART receive events
 * @param  huart [in] Pointer to the UART_HandleTypeDef structure that contains
 *                    the configuration information for the specified UART module
 * @param  size [in] The size of the received data in bytes
 * @return None
 * @note   This function is called when data is received via UART. It processes
 *         the incoming data based on the specified size and may trigger further
 *         actions or state transitions within the communication system.
 */
void gComRxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
  gcom_.instance.rx_len = size;
}

/**
 * ************************************************************************************************
 * @brief  Callback function for handling UART transmission completion events
 * @param  huart [in] Pointer to the UART handle that triggered the event
 * @return None
 * @note   This function is called when a UART transmission is completed successfully.
 *         It can be used to perform any post-transmission tasks, such as updating
 *         the communication state or triggering further actions based on the transmitted data.
 *         This function should be registered with the UART peripheral to handle transmission
 *         complete events correctly.
 */
void gComTxDoneCallback(UART_HandleTypeDef *huart)
{
  gcom_.instance.tx_done = 1;
}

/**
 * ************************************************************************************************
 * @brief  Callback function for handling UART error events
 * @param  huart [in] Pointer to the UART handle that triggered the error event
 * @return None
 * @note   This function is called when a UART error occurs, such as framing errors,
 *         overrun errors, or noise errors. It is responsible for handling the error
 *         condition and may include recovery actions. In this implementation, it
 *         re-enables the UART receive functionality to ensure that subsequent data can be processed.
 *         This function should be registered with the UART peripheral to handle error events correctly.
 */
void gComErrorCallback(UART_HandleTypeDef *huart)
{
  gComRxEnable();
}

/**
 * ************************************************************************************************
 * @brief  Enables the UART receive functionality
 * @param  None
 * @return true if reception is successfully enabled, false otherwise
 * @note   This function initializes the necessary state variables for the UART receive process,
 *         clears any existing UART flags, and sets up the UART to receive data in DMA mode.
 *         If an error occurs during the setup, the communication state is set to an error state.
 *         The function returns true if the reception is successfully enabled, and false if
 *         an error occurs during the process.
 */
bool gComRxEnable(void)
{
  bool answer = false;
  do
  {
    gcom_.error = GCOM_ERR_NONE;
    gcom_.instance.rx_len = 0;
    gcom_.instance.tx_done = 0;
    gcom_.instance.tx_len = 0;
    __HAL_UART_CLEAR_FLAG(&GCOM_USART, 0xFFFFFFFF);
    if (HAL_UARTEx_ReceiveToIdle_DMA(&GCOM_USART, gcom_.instance.rx, GCOM_BUFFER_SIZE) != HAL_OK)
    {
      gcom_.state = GCOM_STATE_ERROR;
      break;
    }
    __HAL_DMA_DISABLE_IT(GCOM_USART.hdmarx, DMA_IT_HT);
    gcom_.state = GCOM_STATE_IDLE;
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Sends a response packet over UART if the received packet is not a broadcast
 * @param  None
 * @return true if the response is successfully initiated, false otherwise
 * @note   This function checks if the received packet was a broadcast. If it was not,
 *         it prepares to send the response data using DMA. The current transmission time
 *         is recorded, and any existing UART flags are cleared. If an error occurs during
 *         the transmission, the function returns false. If the received packet was a
 *         broadcast, no data is sent; the function sets the communication state to IDLE
 *         and enables UART reception mode.
 *         The function returns true if the response is initiated successfully.
 */
bool gComRespSend(void)
{
  bool answer = false;
  do
  {
    if (gcom_.is_broadcast == 0)
    {
#if ((defined DEBUG) && (GCOM_DEBUG == 1))
			dprintf("gCom TX [ ");
			for (int i = 0; i < gcom_.instance.tx_len; i++)
			{
				dprintf("%02X ", gcom_.instance.tx[i]);
			}
			dprintf("]\r\n\r\n");
#endif
      gcom_.instance.tx_time = HAL_GetTick();
      __HAL_UART_CLEAR_FLAG(&GCOM_USART, 0xFFFFFFFF);
      if (HAL_UART_Transmit_DMA(&GCOM_USART, gcom_.instance.tx, gcom_.instance.tx_len) != HAL_OK)
      {
        break;
      }
      __HAL_DMA_DISABLE_IT(GCOM_USART.hdmatx, DMA_IT_HT);
      gcom_.state = GCOM_STATE_SENDING;
    }
    else
    {
      gComRxEnable();
    }
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Handles the idle state of the communication protocol
 * @param  None
 * @return None
 * @note   This function checks the current state of the communication protocol while in
 *         idle mode. It updates the state to `GCOM_STATE_GOT_DATA` if any data has
 *         been received. If a restart is requested, the system is reset using
 *         `NVIC_SystemReset()`. Additionally, if the synchronization timeout is exceeded,
 *         it resets the synchronization time and calls the synchronization timeout callback
 *         function. This allows for proper handling of timeouts in the communication state.
 */
void gComStateIdle(void)
{
  if (gcom_.instance.rx_len > 0)
  {
#if ((defined DEBUG) && (GCOM_DEBUG == 1))
  	dprintf("gCom RX [ ");
  	for (int i = 0; i < gcom_.instance.rx_len; i++)
  	{
  		dprintf("%02X ", gcom_.instance.rx[i]);
  	}
		dprintf("]\r\n");
#endif
    gcom_.state = GCOM_STATE_GOT_DATA;
  }
  if (gcom_.restart_req == 1)
  {
    NVIC_SystemReset();
  }
  if (gcom_.sync_timeout_called == 0)
  {
    if (HAL_GetTick() - gcom_.instance.sync_time >= GCOM_SYNC_TIMEOUT)
    {
      gcom_.instance.sync_time = HAL_GetTick();
      gcom_.sync_timeout_called = 1;
      gComCallbackSyncTimeout();
    }
  }
}

/**
 * ************************************************************************************************
 * @brief  Processes received data in the communication protocol
 * @param  None
 * @return None
 * @note   This function evaluates the received data in the communication protocol. It performs
 *         various checks to ensure the integrity and correctness of the received message:
 *         - Checks if the received length is at least the minimum required length.
 *         - Verifies if the first byte corresponds to the expected ID or is a broadcast identifier.
 *         - Confirms that the last byte is the end-of-frame (EOF) marker.
 *         - Validates the length field against the actual received length.
 *         - Calculates the CRC of the received data and compares it with the received CRC.
 *
 *         If any of these checks fail, it sets the appropriate error state and transitions to
 *         `GCOM_STATE_SENDING` to respond with an error. If the data is valid, it sets the
 *         communication state to `GCOM_STATE_SEMANTIC_CHECK`. It also determines whether the
 *         received message is a broadcast or not and enables RX mode if transitioning to idle.
 */
void gComStateGotData(void)
{
  GComState next_state = GCOM_STATE_IDLE;
  do
  {
    uint16_t crc = 0;
    if (gcom_.instance.rx_len < GCOM_REQ_MIN_LENGTH)
    {
      break;
    }
    if (!((gcom_.instance.rx[0] == gcom_.id) || (gcom_.instance.rx[0] == GCOM_SOF_BROADCAST)))
    {
      break;
    }
    if (gcom_.instance.rx[gcom_.instance.rx_len - 1] != GCOM_EOF)
    {
      gcom_.error = GCOM_ERR_EOF;
      next_state = GCOM_STATE_SENDING;
      break;
    }
    if ((((uint16_t)gcom_.instance.rx[1] * 256) + (uint16_t)gcom_.instance.rx[2]) != gcom_.instance.rx_len - 3)
    {
      gcom_.error = GCOM_ERR_EOF;                                   // it does not exist correct error in list
      next_state = GCOM_STATE_SENDING;
      break;
    }
    __HAL_CRC_DR_RESET(&hcrc);
    if (HAL_CRCEx_Polynomial_Set(&hcrc, 0x1021, CRC_POLYLENGTH_16B) != HAL_OK)
    {
      break;
    }
    crc = HAL_CRC_Calculate(&hcrc, (uint32_t*)&gcom_.instance.rx[1], gcom_.instance.rx_len - 4);
    if (crc != (gcom_.instance.rx[gcom_.instance.rx_len - 3] << 8 | gcom_.instance.rx[gcom_.instance.rx_len - 2]))
    {
      gcom_.error = GCOM_ERR_CRC;
      next_state = GCOM_STATE_SENDING;
      break;
    }
    if (gcom_.instance.rx[0] == GCOM_SOF_BROADCAST)
    {
      gcom_.is_broadcast = 1;
    }
    else
    {
      gcom_.is_broadcast = 0;
    }
    next_state = GCOM_STATE_SEMANTIC_CHECK;

  } while (0);

  if (next_state == GCOM_STATE_IDLE)
  {
    gComRxEnable();
  }
  else if (next_state == GCOM_STATE_SENDING)
  {
    gComRespMake(gcom_.error, NULL, 0);
    gComRespSend();
  }
  else if (next_state == GCOM_STATE_SEMANTIC_CHECK)
  {
    gcom_.state = GCOM_STATE_SEMANTIC_CHECK;
  }
}

/**
 * ************************************************************************************************
 * @brief  Performs semantic validation on the received command and processes it accordingly.
 * @param  None
 * @return None
 * @note   This function checks the semantic correctness of the received command after
 *         decryption (if applicable) and handles the command execution:
 *         - If encryption and decryption are enabled, it attempts to decrypt the received
 *           data. If the decryption fails or the expected sequence number is incorrect,
 *           it generates an error response and returns to idle mode.
 *         - If the command is valid, it calls the appropriate command callback function.
 *         - If no valid response is generated during the command execution, it prepares a
 *           semantic error response.
 *         - The sync time is updated if a valid response is generated.
 */
void gComStateSemanticCheck()
{
  do
  {
#if GCOM_ENC_DEC == 1
    bool skip_check_counter = false;
    cmox_cipher_retval_t cipher_result;
    if ((gcom_.instance.rx[GCOM_REQ_CMD_OFFSET] == GCOM_CMD_SET_IV) ||
    		(gcom_.instance.rx[GCOM_REQ_CMD_OFFSET] == GCOM_CMD_OPEN_PORT) ||
				(gcom_.instance.rx[GCOM_REQ_CMD_OFFSET] == GCOM_CMD_CLOSE_PORT))
    {
      skip_check_counter = true;
      gcom_.instance.crypto_counter = 1;
      for (int i = 0; i < 16; i++)
      {
        gcom_.iv[i] = gcom_iv_[i];
      }
    }
    else
    {
    	for (int i = 0; i < 16; i++)
			{
				gcom_.iv[i] = gcom_.iv_tmp[i];
			}
    }
    cmox_initialize(NULL);
    if (HAL_CRCEx_Polynomial_Set(&hcrc, 0x4c11db7, CRC_POLYLENGTH_32B) != HAL_OK)
    {
      break;
    }
    cipher_result = cmox_cipher_decrypt(CMOX_AES_CTR_DEC_ALGO,
            &gcom_.instance.rx[GCOM_REQ_DATA_OFFSET],
            gcom_.instance.rx_len - GCOM_REQ_MIN_LENGTH,
            gcom_key_, sizeof(gcom_key_),
            gcom_.iv, sizeof(gcom_.iv),
            gcom_.instance.rx_decrypted,
            (size_t*)&gcom_.instance.rx_len_decrypted);

    if (cipher_result != CMOX_CIPHER_SUCCESS)
    {
      break;
    }
    if (!skip_check_counter)
    {
      if ((((uint16_t)((gcom_.instance.rx_decrypted[0] << 8) | gcom_.instance.rx_decrypted[1]) - (uint16_t)gcom_.instance.crypto_counter) == 1) ||
    		 ((uint16_t)((gcom_.instance.rx_decrypted[0] << 8) | gcom_.instance.rx_decrypted[1]) == 1))
      {
        gcom_.instance.crypto_counter = (gcom_.instance.rx_decrypted[0] << 8) | gcom_.instance.rx_decrypted[1];
      }
      else
      {
        if (gComRespMake(GCOM_ERR_FRAME_MISSED, NULL, 0) == false)
        {
          gComRxEnable();
        }
        break;
      }
    }
#endif
    gComCallbackCmd(gcom_.instance.rx[GCOM_REQ_CMD_OFFSET], GCOM_DATA_PTR, GCOM_DATA_LEN);

  } while (0);

  if (gcom_.instance.tx_len == 0)
  {
    if (gComRespMake(GCOM_ERR_SEMANTIC, NULL, 0) == false)
    {
      gComRxEnable();
    }
  }
  else
  {
    gcom_.instance.sync_time = HAL_GetTick();
    gcom_.sync_timeout_called = 0;
  }
  gComRespSend();
}

/**
 * ************************************************************************************************
 * @brief  Manages the state of the communication during data transmission.
 * @param  None
 * @return None
 * @note   This function checks if the transmission of data is complete or if a timeout has
 *         occurred during the transmission. If the transmission is complete, it re-enables
 *         the reception of new data. If the transmission exceeds the predefined timeout,
 *         it aborts the transmission and also re-enables the reception of new data.
 */
void gComStateSending(void)
{
  if (gcom_.instance.tx_done)
  {
    gComRxEnable();
  }
  else if (HAL_GetTick() - gcom_.instance.tx_time >= GCOM_TX_TIMEOUT)
  {
    HAL_UART_AbortTransmit(&GCOM_USART);
    gComRxEnable();
  }
}

/**
 * ************************************************************************************************
 * @brief  Handles the error state of the communication module.
 * @param  None
 * @return None
 * @note   This function is called when an error occurs in the communication process. It
 *         de-initializes the current communication setup, re-initializes it to restore
 *         normal operation, and re-enables reception of new data. This allows the system
 *         to recover from errors and continue functioning.
 */
void gComStateError(void)
{
  gComDeInit();
  gComInit();
  gComRxEnable();
}

/**
 * ************************************************************************************************
 * @brief  Initializes the low-level cryptographic library interface.
 * @param  pArg  Pointer to an optional argument for initialization (can be NULL).
 * @return CMOX_INIT_SUCCESS if initialization is successful, otherwise an error code.
 * @note   This function performs the following actions:
 *         - Releases any reset on the CRC peripheral.
 *         - Enables the clock for the CRC peripheral.
 *         - Resets the CRC data register to ensure a clean state.
 *         This prepares the cryptographic library for use, ensuring that the
 *         hardware resources are correctly initialized and ready for operations.
 */
cmox_init_retval_t cmox_ll_init(void *pArg)
{
  (void)pArg;
  __HAL_RCC_CRC_RELEASE_RESET();
  __HAL_RCC_CRC_CLK_ENABLE();
  __HAL_CRC_DR_RESET(&hcrc);
  return CMOX_INIT_SUCCESS;
}

