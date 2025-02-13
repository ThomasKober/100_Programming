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
 * Configuration UART
 * - Mode: 					asynchronously
 * - Baud Rate: 			100000
 * - Word Length: 			8 bits
 * - Stop Bits: 			1
 * - Parity: 				None
 * - Hardware Flow Control: None
 *
 * Receiving sBUS Data
 * - 25 bytes long frame (start byte and a checksum)
 * - start byte (0x0F)
 *
 * Help: (decoding sBUS)
 * - chatgpt: stm32 L4 logic to process the SBUS data
 * - cubeIDE stm32 dont jump into void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
 */

/*
 * sBUS Frame Description
 * [Byte 1] --> Start byte (0x0F)
 *  - 1 byte (fixed)
 *
 * [Byte 2-23] --> Channel data (16 channels)
 *  - 2 bytes for each channel, little-endian, range 0 to 1023 (10-bit data)
 *
 * [Byte 24] --> Flags byte (flags and failsafe)
 *  - 1 byte (status flags)
 * 	- (bit 0)......Failsafe status		:Indicates if the receiver is in failsafe mode (0 = normal, 1 = failsafe)
	- (bit 1)......Lost Frame			:Indicates whether a frame was lost (1 = lost, 0 = received)
	- (bits 2-5)...Ch 17 and 18 status  :These bits are often used for additional features like telemetry or other system status information (may not always be used)
	- (bits 6-7)...Reserved/Other flags	:Can be used for other future features or to store additional flags

 * [Byte 25] --> End byte (0x00)
 *  - 1 byte (fixed)
 *  - This byte is always 0x00, marks the end of the frame
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

#define SBUS_FRAME_LENGTH 25
#define SBUS_CHANNEL_COUNT 16

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

// Array for received sBUS Data
uint8_t sbusData[SBUS_FRAME_LENGTH];

// Array for sBUS channels
uint16_t sbusChannels[SBUS_CHANNEL_COUNT];

// Flags byte (Byte 24)
uint8_t flags;
uint8_t failsafe;		// Failsafe
uint8_t lost_frame;		// Lost Frame


// UART2 Transmit Variables
char txbuffer[14];
char txnewline[3];

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


// Checks received frame and extracts sBUS channels
void extractSbusChannels(uint16_t size, const uint8_t *sbusData, uint16_t *sbusChannels)
{
    if (sbusData == NULL || sbusChannels == NULL)
    {
        return;
    }

	// Checks Frame before extraction: [Frame Length], [Start Byte], [Flags], [End Byte]
	if ( (size == SBUS_FRAME_LENGTH) && (sbusData[0] == 0x0F) && (sbusData[23] == 0x00) && (sbusData[24] == 0x00))
	{
		// Extraction of sBUS channels
		sbusChannels[0]	= (sbusData[1] >> 0 | (sbusData[2] << 8)) & 0x07FF;
		sbusChannels[1] = (sbusData[2] >> 3 | (sbusData[3] << 5)) & 0x07FF;
		sbusChannels[2] = (sbusData[3] >> 6 | (sbusData[4] << 2) | sbusData[5] << 10) & 0x07FF;
		sbusChannels[3] = (sbusData[5] >> 1 | (sbusData[6] << 7)) & 0x07FF;
		sbusChannels[4] = (sbusData[6] >> 4 | (sbusData[7] << 4)) & 0x07FF;
		sbusChannels[5] = (sbusData[7] >> 7 | (sbusData[8] << 1) | sbusData[9] << 9) & 0x07FF;
		sbusChannels[6] = (sbusData[9] >> 2 | (sbusData[10] << 6)) & 0x07FF;
		sbusChannels[7] = (sbusData[10] >> 5 | (sbusData[11] << 3)) & 0x07FF;
		sbusChannels[8] = (sbusData[12] << 0 | (sbusData[13] << 8)) & 0x07FF;
		sbusChannels[9] = (sbusData[13] >> 3 | (sbusData[14] << 5)) & 0x07FF;
		sbusChannels[10] = (sbusData[14] >> 6 | (sbusData[15] << 2) | sbusData[16] << 10) & 0x07FF;
		sbusChannels[11] = (sbusData[16] >> 1 | (sbusData[17] << 7)) & 0x07FF;
		sbusChannels[12] = (sbusData[17] >> 4 | (sbusData[18] << 4)) & 0x07FF;
		sbusChannels[13] = (sbusData[18] >> 7 | (sbusData[19] << 1) | sbusData[20] << 9) & 0x07FF;
		sbusChannels[14] = (sbusData[20] >> 2 | (sbusData[21] << 6)) & 0x07FF;
		sbusChannels[15] = (sbusData[21] >> 5 | (sbusData[22] << 3)) & 0x07FF;
	}
	else
	{
		// Flags byte (byte 24)
		flags = sbusData[23];


		if (flags & 0x01)
		{
			// Failsafe triggered
			// Handle failsafe condition
			// For example, set a flag, log the event, or revert to a safe state
			// Count errors
		}
		if (flags & 0x08)
		{
			// Frame lost
			// Handle frame loss condition
			// You may want to log this or take action to recover
			// Count errors
		}
	}

	//
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if (huart->Instance == USART1)
    {
    	extractSbusChannels(size, sbusData, sbusChannels);

        // Restart DMA reception
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, sbusData, SBUS_FRAME_LENGTH);
    }
}


void DisplaySBUSChannels(void)
{
	//sprintf(txbuffer, "CH1: %04x | CH2: %04x | CH3: %04x | CH4: %04x | CH5: %04x | CH6: %04x | CH7: %04x | CH8: %04x | CH9: %04x | CH10: %04x | CH11: %04x | CH12: %04x | CH13: %04x | CH14: %04x | CH15: %04x | CH16: %04x \r\n", sbusChannels[0], sbusChannels[1], sbusChannels[2], sbusChannels[3], sbusChannels[4], sbusChannels[5], sbusChannels[6], sbusChannels[7], sbusChannels[8], sbusChannels[9], sbusChannels[10], sbusChannels[11], sbusChannels[12], sbusChannels[13], sbusChannels[14], sbusChannels[15]);
	for(int i = 0; i < 16; i++)
	{
		// Clear the string by setting all elements to '\0'
		memset(txbuffer, '\0', sizeof(txbuffer));

		sprintf(txbuffer, "CH%02d: %04x | ", i+1, sbusChannels[i]);
		HAL_UART_Transmit(&huart2, (uint8_t*) txbuffer, sizeof(txbuffer), HAL_MAX_DELAY);
	}

	sprintf(txnewline, "\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*) txnewline, sizeof(txnewline), HAL_MAX_DELAY);
}


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

  // Start DMA reception
   HAL_UARTEx_ReceiveToIdle_DMA(&huart1, sbusData, SBUS_FRAME_LENGTH);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  DisplaySBUSChannels();
	  HAL_Delay(100);

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
  huart1.Init.BaudRate = 100000;
  huart1.Init.WordLength = UART_WORDLENGTH_9B;
  huart1.Init.StopBits = UART_STOPBITS_2;
  huart1.Init.Parity = UART_PARITY_EVEN;
  huart1.Init.Mode = UART_MODE_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXINVERT_INIT;
  huart1.AdvancedInit.RxPinLevelInvert = UART_ADVFEATURE_RXINV_ENABLE;
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
  huart2.Init.BaudRate = 115200;
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
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

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

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//    if (htim->Instance == TIM1)
//    {
//  	  HAL_UART_Transmit(&huart2, data_sBusTest, sizeof(data_sBusTest), 100);
//  	  HAL_UART_Transmit(&huart2, (uint8_t *)crlf, 2, HAL_MAX_DELAY);
//  	  DisplaySBUSChannels();
//    }
//}

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
