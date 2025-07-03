/*
 * Com_UART.h
 *
 *  Created on: Apr 23, 2025
 *      Author: g.tec
 */

#ifndef CORE_COM_H_
#define CORE_COM_H_

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

/* USER CODE BEGIN library configuration */

#define	COM_BUFFER_SIZE		2048

/**
 * ************************************************************************************************
 * struct/enum definitions
 * ************************************************************************************************
 */
typedef enum
{
	COM_STATE_IDLE = 0,
	COM_STATE_CHECK_FRAME,
	COM_STATE_CHECK_DATA,
	COM_STATE_SEND_DATA,
	COM_STATE_ERROR,
} ComState;

typedef struct
{
	uint8_t		rx[COM_BUFFER_SIZE];
	uint8_t     tx[COM_BUFFER_SIZE];
	uint32_t    rx_len;
	uint32_t    tx_len;
} ComInstance;

typedef struct
{
	ComInstance	instance;
	ComState 	state;
} ComHandle;

/**
 * ************************************************************************************************
 * public library functions
 * ************************************************************************************************
 */

void gComLoop(void);

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */

extern ComHandle gcom;

#endif /* CORE_COM_H_ */
