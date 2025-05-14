/*
 * Com_UART.c
 *
 *  Created on: Apr 23, 2025
 *      Author: g.tec
 */

#include "Com.h"

/**
 * ************************************************************************************************
 * private function prototypes
 * ************************************************************************************************
 */

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size);
/**
 * ************************************************************************************************
 * global variable
 * ************************************************************************************************
 */

ComHandle com;

/**
 * ************************************************************************************************
 * public function implementations
 * ************************************************************************************************
 */

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


void ComLoop(void)
{
	switch (com.state)
	{
	case COM_STATE_RECEIVE:
		//
		break;

	case COM_STATE_CHECK_FRAME:
		//
		break;

	case COM_STATE_CHECK_DATA:
		//
		break;

	case COM_STATE_SEND_DATA:
		//
		break;

	case COM_STATE_ERROR:
		//
		break;
	}
}



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
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
	com.instance.rx_len = size;
}
