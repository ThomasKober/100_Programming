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
#include <stdio.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
UART_HandleTypeDef huart2;
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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/*
 * Error Code description UART1
 * 0xA0....UART1 Unknown Error
 * 0xA1....UART1 Overrun Error
 * 0xA2....UART1 Noise Error
 * 0xA3....UART1 Framing Error
 * 0xA4....UART1 Parity Error
 */

/*
 * Error Code description UART2
 * 0xB0....UART2 Unknown Error
 * 0xB1....UART2 Overrun Error
 * 0xB2....UART2 Noise Error
 * 0xB3....UART2 Framing Error
 * 0xB4....UART2 Parity Error
 */


void LogError(uint8_t errorCode)
{
	char message[50];

    // Implement your logging mechanism, e.g., send to UART, store in buffer

	// Example: send error code to a debug UART
    sprintf(message, "UART Error: 0x%08X\n", errorCode);
    HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}


void resetUART(UART_HandleTypeDef *huart)
{
	if (huart == NULL || huart->Instance != USART1 || huart->Instance != USART2)
	{
		return; // Error: Invalid handle
	}

//	if (huart->Instance == USART1)
//	{
//		// Disable the UART1 peripheral
//		__HAL_UART_DISABLE(&huart1);
//
//		// Clear the RX buffer
//		while (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE))
//		{
//			(void)huart->Instance->RDR; // Read the data register to clear RXNE
//		}
//
//		// Reset the UART1 peripheral
//		__HAL_RCC_USART1_FORCE_RESET();
//		__HAL_RCC_USART1_RELEASE_RESET();
//
//		// Reinitialize the UART1 peripheral
//		if (HAL_UART_Init(&huart1) == HAL_OK)		// Init UART1
//		{
//			// Reinitialize successful
//		}else
//		{
//			// Reinitialize not successful
//		}
//	}

	if (huart->Instance == USART2)
	{
		// Disable the UART2 peripheral
		__HAL_UART_DISABLE(&huart2);

		// Clear the RX buffer
		while (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE))
		{
			(void)huart->Instance->RDR; // Read the data register to clear RXNE
		}

		// Reset the UART2 peripheral
		__HAL_RCC_USART2_FORCE_RESET();
		__HAL_RCC_USART2_RELEASE_RESET();

		// Reinitialize the UART2 peripheral
		if (HAL_UART_Init(&huart2) == HAL_OK)		// Init UART2
		{
			// Reinitialize successful
		}else
		{
			// Reinitialize not successful
		}
	}
}


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


////------------------------------Error Handling UART1------------------------------
//// Handle overrun error
//void HandleOverrunErrorUART1(void)
//{
//	LogError(0xB1);						// log the error
//	__HAL_UART_CLEAR_OREFLAG(&huart1);	// clear error flag
//	//resetUART1(&huart1);						// reset UART1
//}
//
//// Handle noise error
//void HandleNoiseErrorUART1(void)
//{
//	LogError(0xB2);						// log the error
//	__HAL_UART_CLEAR_NEFLAG(&huart1);	// clear error flag
//}
//
//// Handle framing error
//void HandleFramingErrorUART1(void)
//{
//	LogError(0xB3);						// log the error
//	__HAL_UART_CLEAR_FEFLAG(&huart1);	// clear error flag
//}
//
//// Handle parity error
//void HandleParityErrorUART1(void)
//{
//	LogError(0xB4);						// log the error
//	__HAL_UART_CLEAR_PEFLAG(&huart1);	// clear error flag
//}
//
//// Handle unknown errors
//void HandleUnknownErrorUART1(void)
//{
//	LogError(0xB0);						// log the error
//	//resetUART1();						// reset UART1
//}


//------------------------------Error Handling UART2------------------------------
// Handle overrun error
void HandleOverrunErrorUART2(void)
{
	LogError(0xB1);						// log the error
	__HAL_UART_CLEAR_OREFLAG(&huart2);	// clear error flag
	resetUART(&huart2);					// reset UART2
}

// Handle noise error
void HandleNoiseErrorUART2(void)
{
	LogError(0xB2);						// log the error
	__HAL_UART_CLEAR_NEFLAG(&huart2);	// clear error flag
}

// Handle framing error
void HandleFramingErrorUART2(void)
{
	LogError(0xB3);						// log the error
	__HAL_UART_CLEAR_FEFLAG(&huart2);	// clear error flag
}

// Handle parity error
void HandleParityErrorUART2(void)
{
	LogError(0xB4);						// log the error
	__HAL_UART_CLEAR_PEFLAG(&huart2);	// clear error flag
}

// Handle unknown errors
void HandleUnknownErrorUART2(void)
{
	LogError(0xB0);						// log the error
	//resetUART2();						// reset UART2
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart == NULL || huart->Instance != USART1 || huart->Instance != USART2)
	{
		return; // Error: Invalid handle
	}

//	//Error Handling UART1
//    if (huart->Instance == USART1)
//    {
//        switch (huart->ErrorCode)
//        {
//            case HAL_UART_ERROR_ORE:
//                // Handle overrun error
//                HandleOverrunErrorUART1();
//                break;
//            case HAL_UART_ERROR_NE:
//                // Handle noise error
//                HandleNoiseErrorUART1();
//                break;
//            case HAL_UART_ERROR_FE:
//                // Handle framing error
//                HandleFramingErrorUART1();
//                break;
//            case HAL_UART_ERROR_PE:
//                // Handle parity error
//                HandleParityErrorUART1();
//                break;
//            default:
//                // Handle unknown errors
//                HandleUnknownErrorUART1();
//                break;
//        }
//    }

    //Error Handling UART2
    if (huart->Instance == USART2)
    {
        switch (huart->ErrorCode)
        {
            case HAL_UART_ERROR_ORE:
                // Handle overrun error
                HandleOverrunErrorUART2();
                break;
            case HAL_UART_ERROR_NE:
                // Handle noise error
                HandleNoiseErrorUART2();
                break;
            case HAL_UART_ERROR_FE:
                // Handle framing error
                HandleFramingErrorUART2();
                break;
            case HAL_UART_ERROR_PE:
                // Handle parity error
                HandleParityErrorUART2();
                break;
            default:
                // Handle unknown errors
                HandleUnknownErrorUART2();
                break;
        }
    }
}




// Callback for DMA transmission complete
//void HAL_DMA_ErrorCallback(DMA_HandleTypeDef *hdma)
//{
//    // Step 1: Identify the DMA instance
//    if (hdma->Instance == DMA1_Channel6)		//UART2, TX
//    {
//        // Step 2: Log the error for DMA1_Channel6
//        //printf("Error occurred on DMA1_Channel6.\n");
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
//}





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
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_Delay(1000);

	  if (HAL_UART_Transmit_DMA(&huart2, dataBufferTx, sizeof(dataBufferTx)) != HAL_OK)
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
