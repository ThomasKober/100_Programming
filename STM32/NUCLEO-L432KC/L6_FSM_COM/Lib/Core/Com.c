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

void ComStateReceive(void);
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
	case COM_STATE_IDLE:
		ComStateIdle();
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
	// Check if the interrupt comes from UART2
	if(huart->Instance == USART2)
	{
		com.instance.rx_len = size;
	}
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
void ComStateIdle(void)
{
	if(com.instance.rx_len > 0)
	{
		com.state = COM_STATE_CHECK_FRAME;
	}

	// Reset device manually by PC

	// Sync. device
}
