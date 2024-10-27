/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/*
 * ChatGPT:	- HAL_UART_ErrorCallback vs HAL_DMA_ErrorCallback
 *			- stm32 L4 HAL_UART_ErrorCallback error handling logic
 *
 *
 * Structure of HAL_DMA_ErrorCallback
 *
 * Identify the DMA Instance:
 * 		Use the hdma parameter to determine which DMA channel experienced the error.
 *
 * Log the Error:
 * 		Use a logging mechanism (e.g., printing to console, sending an error message,
 * 		etc.) to capture the error context.
 *
 * Clear the Error:
 * 		If applicable, you might want to clear the error flags associated with the DMA.
 *
 * Abort the DMA Transfer:
 * 		Call HAL_DMA_Abort() to stop the DMA transfer and clean up any resources.
 *
 * Optionally, Reinitialize the DMA:
 * 		Depending on the application, you might need to reconfigure or restart the DMA
 * 		after handling the error.
 *
 * Notify Other System Components:
 * 		If other parts of your system need to be aware of the error (e.g., an application layer),
 * 		send a notification or update a status variable.
 */

/*
 * Breakdown of Key Components
 *
 * Error Callback Function:
 * 		This function is called automatically when a UART error occurs. Ensure it's configured
 * 		properly in your UART initialization.
 *
 * dentify UART Instance:
 * 		The code checks if the error relates to UART2. This is crucial for applications with multiple UARTs.
 *
 * DMA Abort Handling:
 * 		Abort the DMA: Call HAL_DMA_Abort() to stop the ongoing DMA transfer. Check the return value
 * 		to ensure the abort was successful.
 *
 * 		Clear Error Flags: Use __HAL_DMA_CLEAR_FLAG() to clear relevant error flags, such as transfer
 * 		errors, FIFO errors, and direct memory errors.
 *
 * Stop UART DMA:
 * 		Use HAL_UART_DMAStop() to stop any ongoing DMA operations related to the UART. This prevents
 * 		further erroneous transmissions.
 *
 * Optional Reinitialization:
 * 		If necessary, you can reinitialize the UART peripheral by uncommenting the HAL_UART_Init() line.
 * 		This step may be required depending on your application's state management.
 *
 * Notify Other Components:
 * 		Consider implementing a mechanism to notify other parts of your application about the error, such
 * 		as setting an error state or invoking an error handler function.
 */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
uint8_t dataBufferTx[] = "Hello, UART with DMA!\r\n";		//data buffer tx
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


//------------------------------Error Logging------------------------------
// Error Log
void LogError(uint8_t errorCode)
{
	char message[50];

    // Implement your logging mechanism, e.g., send to UART, store in buffer

	// Example: send error code to a debug UART
    sprintf(message, "UART Error: 0x%08X\n", errorCode);
    HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}


//------------------------------UART Resets------------------------------
// UART Reset
void resetUART(UART_HandleTypeDef *huart)
{
	if (huart == NULL || huart->Instance != USART1 || huart->Instance != USART2)
	{
		return; // Error: Invalid handle
	}

	if (huart->Instance == USART1)
		{
			// Disable the UART1 peripheral
			__HAL_UART_DISABLE(huart);

			// Clear the RX buffer
			while (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE))
			{
				(void)huart->Instance->RDR; // Read the data register to clear RXNE
			}

			// Reset the UART1 peripheral
			__HAL_RCC_USART1_FORCE_RESET();
			__HAL_RCC_USART1_RELEASE_RESET();


			// Reinitialize the UART1 peripheral
			if (HAL_UART_Init(huart) == HAL_OK)		// Init UART1
			{
				// Reinitialize successful
			}else
			{
				// Reinitialize not successful
			}
		}

	if (huart->Instance == USART2)
	{
		// Disable the UART2 peripheral
		__HAL_UART_DISABLE(huart);

		// Clear the RX buffer
		while (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE))
		{
			(void)huart->Instance->RDR; // Read the data register to clear RXNE
		}

		// Reset the UART2 peripheral
		__HAL_RCC_USART2_FORCE_RESET();
		__HAL_RCC_USART2_RELEASE_RESET();


		// Reinitialize the UART2 peripheral
		if (HAL_UART_Init(huart) == HAL_OK)		// Init UART2
		{
			// Reinitialize successful
		}else
		{
			// Reinitialize not successful
		}
	}
}

// Debug over hardware
//void Error_Handler(void)
//{
//	// User can add their own implementation to report the HAL error return state
//    while (1)
//    {
//    	// Optionally toggle an LED or send an error message
//        HAL_Delay(100);
//        // Example: Toggle an LED connected to a GPIO pin
//        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin); // Adjust pin according to your setup
//    }
//}

/*
 * ----------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------
 */

// ------------------------------Error Handling Functions UART------------------------------

// UART Error Code Description
/*
 * ----------UART1----------
 * 0xA0....UART1 Error No
 * 0xA1....UART1 Error Parity error
 * 0xA2....UART1 Error Noise
 * 0xA3....UART1 Error Frame
 * 0xA4....UART1 Error Overrun
 * 0xA5....UART1 Error Receiver Timeout
 * 0xA6....UART1 Error Unknown
 */

/*
 * ----------UART2----------
 * 0xB0....UART2 Error No
 * 0xB1....UART2 Error Parity error
 * 0xB2....UART2 Error Noise
 * 0xB3....UART2 Error Frame
 * 0xB4....UART2 Error Overrun
 * 0xB5....UART2 Error Receiver Timeout
 * 0xB6....UART1 Error Unknown
 */


// Handle No Error
void HandleNoErrorUART(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		LogError(0xA0);	// Log Error
	}
	else if (huart->Instance == USART2)
	{
		LogError(0xB0);	// Log Error
	}
}

// Handle Parity Error
void HandleParityErrorUART(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		LogError(0xA1);			// Log Error
	}
	else if (huart->Instance == USART2)
	{
		LogError(0xB1);			// Log Error
	}

	__HAL_UART_CLEAR_PEFLAG(huart);	// Clear Error Flag
}

// Handle Noise Error
void HandleNoiseErrorUART(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)		// log the error
	{
		LogError(0xA2);
	}
	else if (huart->Instance == USART2)
	{
		LogError(0xB2);				// log the error
	}

	__HAL_UART_CLEAR_NEFLAG(huart);		// clear error flag
}

// Handle Framing Error
void HandleFramErrorUART(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)		// Log Error
	{
		LogError(0xA3);
	}
	else if (huart->Instance == USART2)
	{
		LogError(0xB3);				// Log Error
	}

	__HAL_UART_CLEAR_FEFLAG(huart);		// Clear Error Flag
}

// Handle Overrun Error
void HandleOverrunErrorUART(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		LogError(0xA4);				// Log the Error
	}
	else if (huart->Instance == USART2)
	{
		LogError(0xB4);				// Log the Error
	}

	__HAL_UART_CLEAR_OREFLAG(huart);	// Clear Error Flag

	resetUART(huart);					// Reset UART
}

// Receiver Timeout Error
void HandleReceiverTimeoutErrorUART(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)						// Log Error
	{
		LogError(0xA5);
	}else if (huart->Instance == USART2)
	{
		LogError(0xB5);								// Log Error
	}
}

// Handle Unknown Errors
void HandleUnknownErrorUART(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)		// Log Error
	{
		LogError(0xA6);
	}else if (huart->Instance == USART2)
	{
		LogError(0xB6);				// Log Error
	}

	resetUART(huart);					// Reset UART
}


// ------------------------------UART Error Callback------------------------------
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart == NULL || huart->Instance != USART1 || huart->Instance != USART2)
	{
		return; // Error: Invalid handle
	}

    //Error Handling UART
	switch (huart->ErrorCode)
	{
		case HAL_UART_ERROR_NONE:
			// Handle No Error
			HandleNoErrorUART(huart);
			break;
		case HAL_UART_ERROR_PE:
			// Handle Parity Error
			HandleParityErrorUART(huart);
			break;
		case HAL_UART_ERROR_NE:
			// Handle Noise Error
			HandleNoiseErrorUART(huart);
			break;
		case HAL_UART_ERROR_FE:
			// Handle Frame Error
			HandleFramErrorUART(huart);
			break;
		case HAL_UART_ERROR_ORE:
			// Handle Overrun error
			HandleOverrunErrorUART(huart);
			break;
		case HAL_UART_ERROR_RTO:
			// Handle Receiver Timeout Error
			HandleReceiverTimeoutErrorUART(huart);
			break;
		default:
			// Handle Unknown Errors
			HandleUnknownErrorUART(huart);
			break;
	}
}

/*
 * ----------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------
 */

//------------------------------Error Handling Functions DMA------------------------------

// UART Error Code Description
/*
 * ----------DMA UART1----------
 * 0xC0....UART1 Error No
 * 0xC1....UART1 Error Transfer
 * 0xC2....UART1 Error Abort requested with no Xfer ongoing
 * 0xC3....UART1 Error Not supported mode
 * 0xC4....UART1 Error DMAMUX sync overrun  error
 * 0xC5....UART1 Error DMAMUX request generator overrun  error
 * 0xC6....UART1 Error Unknown
 */

/*
 * ----------DMA UART2----------
 * 0xC0....UART2 Error No
 * 0xC1....UART2 Error Transfer
 * 0xC2....UART2 Error Abort requested with no Xfer ongoing
 * 0xC3....UART2 Error Timeout
 * 0xC4....UART2 Error Not supported mode
 * 0xC5....UART2 Error DMAMUX sync overrun
 * 0xC6....UART2 Error DMAMUX request generator overrun
 * 0xC7....UART2 Error Unknown
 */


void HandleNoErrorDMA(DMA_HandleTypeDef *hdma)
{
	// Behandlung des Transferfehlers
	// Zum Beispiel: DMA-Kanal zurücksetzen und neu starten
	HAL_DMA_Abort(hdma);
	// Optional: Fehlermeldung oder Wiederherstellungsmaßnahmen
	// z.B. Fehlermeldung an eine Logging-Funktion senden
	// LogError("DMA Transfer Error");
}

void HandleTransferErrorDMA(DMA_HandleTypeDef *hdma)
{
//	if (huart->Instance == USART1)
//	{
//		LogError(0xA05);								// Log the Error
//	}
//	else if (huart->Instance == USART2)
//	{
//		LogError(0xB05);								// Log the Error
//	}
//
//	__HAL_DMA_CLEAR_FLAG(&hdma_uart_rx, DMA_FLAG_TE);	// Clear Error Flag
}

void HandleNoXferErrorDMA(DMA_HandleTypeDef *hdma)
{

}

void HandleTimeOutErrorDMA(DMA_HandleTypeDef *hdma)
{
	// Behandlung des Timeout-Fehlers
	// Zum Beispiel: DMA-Kanal zurücksetzen und neu starten
	HAL_DMA_Abort(hdma);
	// Optional: Fehlermeldung oder Wiederherstellungsmaßnahmen
	// z.B. Fehlermeldung an eine Logging-Funktion senden
	// LogError("DMA Timeout Error");
}

void HandleNoSupportedErrorDMA(DMA_HandleTypeDef *hdma)
{

}

void HandleSyncErrorDMA(DMA_HandleTypeDef *hdma)
{

}

void HandleRequestErrorDMA(DMA_HandleTypeDef *hdma)
{

}

void HandleUnknownErrorDMA(DMA_HandleTypeDef *hdma)
{
	// Unbekannter Fehler
	// Behandlung des unbekannten Fehlers
	// Zum Beispiel: DMA-Kanal zurücksetzen und neu starten
	HAL_DMA_Abort(hdma);
	// Optional: Fehlermeldung oder Wiederherstellungsmaßnahmen
	// z.B. Fehlermeldung an eine Logging-Funktion senden
	// LogError("Unknown DMA Error");
}


//------------------------------DMA Error Callback------------------------------
void HAL_DMA_ErrorCallback(DMA_HandleTypeDef *hdma)
{
	if (hdma == NULL)
	{
		return; // Error: Invalid handle
	}

	//Error Handling DMA1 UART1 and UART2

	//switch (huart->ErrorCode)
	switch (hdma->ErrorCode)
	{
		case HAL_DMA_ERROR_NONE:
			// Handle No Error
			HandleNoErrorDMA(hdma);
			break;
		case HAL_DMA_ERROR_TE:
			// Handle Transfer Error
			HandleTransferErrorDMA(hdma);
			break;
		case HAL_DMA_ERROR_NO_XFER:
			// Handle Abort Requested With No Xfer Ongoing Error
			HandleNoXferErrorDMA(hdma);
			break;
		case HAL_DMA_ERROR_TIMEOUT:
			// Handle Timeout Error
			HandleTimeOutErrorDMA(hdma);
			break;
		case HAL_DMA_ERROR_NOT_SUPPORTED:
			// Handle Not Supported Mode Error
			HandleNoSupportedErrorDMA(hdma);
			break;
		case HAL_DMA_ERROR_SYNC:
			// Handle DMAMUX Sync Overrun Error
			HandleSyncErrorDMA(hdma);
			break;
		case HAL_DMA_ERROR_REQGEN:
			// Handle MAMUX Request Generator Overrun Error
			HandleRequestErrorDMA(hdma);
			break;
		default:
			// Handle Unknown Error
			HandleUnknownErrorDMA(hdma);
			break;
	}

//    // Step 1: Identify the DMA instance
//    if (hdma->Instance == DMA1_Channel6)		//UART2, TX
//    {
//        // Step 2: Log the error for DMA1_Channel6
//    	LogError(0xA0);
//
//        // Step 3: Clear any error flags
//        __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_TE6);
//    }
//    else if (hdma->Instance == DMA1_Channel7)	//UART2, RX
//    {
//        // Step 2: Log the error for DMA1_Channel7
//    	//printf("Error occurred on DMA1_Channel7.\n");
//
//        // Step 3: Clear any error flags
//        __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_TE7);
//    }
//    // Add additional channels as needed
//
//    // Step 4: Abort the DMA transfer
//    if (HAL_DMA_Abort(hdma) != HAL_OK)
//    {
//        // Handle abort error if necessary
//    	//printf("Failed to abort DMA transfer.\n");
//    }
//
//    // Step 5: Optional - Reinitialize the DMA if needed
//    // HAL_DMA_Init(hdma); // Uncomment if you wish to reinitialize
//
//    // Step 6: Notify other parts of the application if needed
//    // e.g., set an error flag, send a message, etc.
}

/*
 * ----------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------
 */



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_Delay(1000);

	  if (HAL_UART_Transmit_DMA(&huart2, dataBufferTx, sizeof(dataBufferTx)))
	  {
		  // Transmission error handling
		  Error_Handler();
	  }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
