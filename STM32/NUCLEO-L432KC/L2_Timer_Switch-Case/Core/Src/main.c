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


  /*--------------------------------------------------------------------------
   * Task:
   * - create light system for aircraft
   *	-> Beacon lights
   *	-> Position Lights
   *	-> Anti Collition Lights
   *	-> Landing Light
   *	-> Search light
   *
   * - Dev. Board: NUCLEO-L432KC
   * - User Manual: UM1884
   *   https://www.st.com/resource/en/user_manual/um1884-description-of-stm32l4l4-hal-and-lowlayer-drivers-stmicroelectronics.pdf
   * - STM32L4 Timers
   *   https://www.st.com/resource/en/product_training/STM32L4_WDG_TIMERS_GPTIM.pdf
   *--------------------------------------------------------------------------
   */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

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

/* USER CODE BEGIN PV */
uint16_t cnt = 0;
uint32_t i = 0;


/** Generated using Dr LUT - Free Lookup Table Generator
  * https://github.com/ppelikan/drlut
  **/
// Formula: sin(1*pi*t/T)
const int32_t data[1024] = {
     0,   101,   201,   302,   402,   503,   603,
   704,   804,   905,  1005,  1106,  1206,  1307,
  1407,  1507,  1608,  1708,  1809,  1909,  2009,
  2110,  2210,  2310,  2410,  2511,  2611,  2711,
  2811,  2911,  3012,  3112,  3212,  3312,  3412,
  3512,  3612,  3712,  3811,  3911,  4011,  4111,
  4210,  4310,  4410,  4509,  4609,  4708,  4808,
  4907,  5007,  5106,  5205,  5305,  5404,  5503,
  5602,  5701,  5800,  5899,  5998,  6096,  6195,
  6294,  6393,  6491,  6590,  6688,  6786,  6885,
  6983,  7081,  7179,  7277,  7375,  7473,  7571,
  7669,  7767,  7864,  7962,  8059,  8157,  8254,
  8351,  8448,  8545,  8642,  8739,  8836,  8933,
  9030,  9126,  9223,  9319,  9416,  9512,  9608,
  9704,  9800,  9896,  9992, 10087, 10183, 10278,
 10374, 10469, 10564, 10659, 10754, 10849, 10944,
 11039, 11133, 11228, 11322, 11417, 11511, 11605,
 11699, 11793, 11886, 11980, 12074, 12167, 12260,
 12353, 12446, 12539, 12632, 12725, 12817, 12910,
 13002, 13094, 13187, 13279, 13370, 13462, 13554,
 13645, 13736, 13828, 13919, 14010, 14101, 14191,
 14282, 14372, 14462, 14553, 14643, 14732, 14822,
 14912, 15001, 15090, 15180, 15269, 15358, 15446,
 15535, 15623, 15712, 15800, 15888, 15976, 16063,
 16151, 16238, 16325, 16413, 16499, 16586, 16673,
 16759, 16846, 16932, 17018, 17104, 17189, 17275,
 17360, 17445, 17530, 17615, 17700, 17784, 17869,
 17953, 18037, 18121, 18204, 18288, 18371, 18454,
 18537, 18620, 18703, 18785, 18868, 18950, 19032,
 19113, 19195, 19276, 19357, 19438, 19519, 19600,
 19680, 19761, 19841, 19921, 20000, 20080, 20159,
 20238, 20317, 20396, 20475, 20553, 20631, 20709,
 20787, 20865, 20942, 21019, 21096, 21173, 21250,
 21326, 21403, 21479, 21554, 21630, 21705, 21781,
 21856, 21930, 22005, 22079, 22154, 22227, 22301,
 22375, 22448, 22521, 22594, 22667, 22739, 22812,
 22884, 22956, 23027, 23099, 23170, 23241, 23311,
 23382, 23452, 23522, 23592, 23662, 23731, 23801,
 23870, 23938, 24007, 24075, 24143, 24211, 24279,
 24346, 24413, 24480, 24547, 24613, 24680, 24746,
 24811, 24877, 24942, 25007, 25072, 25137, 25201,
 25265, 25329, 25393, 25456, 25519, 25582, 25645,
 25708, 25770, 25832, 25893, 25955, 26016, 26077,
 26138, 26198, 26259, 26319, 26378, 26438, 26497,
 26556, 26615, 26674, 26732, 26790, 26848, 26905,
 26962, 27019, 27076, 27133, 27189, 27245, 27300,
 27356, 27411, 27466, 27521, 27575, 27629, 27683,
 27737, 27790, 27843, 27896, 27949, 28001, 28053,
 28105, 28157, 28208, 28259, 28310, 28360, 28411,
 28460, 28510, 28560, 28609, 28658, 28706, 28755,
 28803, 28850, 28898, 28945, 28992, 29039, 29085,
 29131, 29177, 29223, 29268, 29313, 29358, 29403,
 29447, 29491, 29534, 29578, 29621, 29664, 29706,
 29749, 29791, 29832, 29874, 29915, 29956, 29997,
 30037, 30077, 30117, 30156, 30195, 30234, 30273,
 30311, 30349, 30387, 30424, 30462, 30498, 30535,
 30571, 30607, 30643, 30679, 30714, 30749, 30783,
 30818, 30852, 30885, 30919, 30952, 30985, 31017,
 31050, 31082, 31113, 31145, 31176, 31206, 31237,
 31267, 31297, 31327, 31356, 31385, 31414, 31442,
 31470, 31498, 31526, 31553, 31580, 31607, 31633,
 31659, 31685, 31710, 31736, 31760, 31785, 31809,
 31833, 31857, 31880, 31903, 31926, 31949, 31971,
 31993, 32014, 32036, 32057, 32077, 32098, 32118,
 32137, 32157, 32176, 32195, 32213, 32232, 32250,
 32267, 32285, 32302, 32318, 32335, 32351, 32367,
 32382, 32397, 32412, 32427, 32441, 32455, 32469,
 32482, 32495, 32508, 32521, 32533, 32545, 32556,
 32567, 32578, 32589, 32599, 32609, 32619, 32628,
 32637, 32646, 32655, 32663, 32671, 32678, 32685,
 32692, 32699, 32705, 32711, 32717, 32722, 32728,
 32732, 32737, 32741, 32745, 32748, 32752, 32755,
 32757, 32759, 32761, 32763, 32765, 32766, 32766,
 32767, 32767, 32767, 32766, 32766, 32765, 32763,
 32761, 32759, 32757, 32755, 32752, 32748, 32745,
 32741, 32737, 32732, 32728, 32722, 32717, 32711,
 32705, 32699, 32692, 32685, 32678, 32671, 32663,
 32655, 32646, 32637, 32628, 32619, 32609, 32599,
 32589, 32578, 32567, 32556, 32545, 32533, 32521,
 32508, 32495, 32482, 32469, 32455, 32441, 32427,
 32412, 32397, 32382, 32367, 32351, 32335, 32318,
 32302, 32285, 32267, 32250, 32232, 32213, 32195,
 32176, 32157, 32137, 32118, 32098, 32077, 32057,
 32036, 32014, 31993, 31971, 31949, 31926, 31903,
 31880, 31857, 31833, 31809, 31785, 31760, 31736,
 31710, 31685, 31659, 31633, 31607, 31580, 31553,
 31526, 31498, 31470, 31442, 31414, 31385, 31356,
 31327, 31297, 31267, 31237, 31206, 31176, 31145,
 31113, 31082, 31050, 31017, 30985, 30952, 30919,
 30885, 30852, 30818, 30783, 30749, 30714, 30679,
 30643, 30607, 30571, 30535, 30498, 30462, 30424,
 30387, 30349, 30311, 30273, 30234, 30195, 30156,
 30117, 30077, 30037, 29997, 29956, 29915, 29874,
 29832, 29791, 29749, 29706, 29664, 29621, 29578,
 29534, 29491, 29447, 29403, 29358, 29313, 29268,
 29223, 29177, 29131, 29085, 29039, 28992, 28945,
 28898, 28850, 28803, 28755, 28706, 28658, 28609,
 28560, 28510, 28460, 28411, 28360, 28310, 28259,
 28208, 28157, 28105, 28053, 28001, 27949, 27896,
 27843, 27790, 27737, 27683, 27629, 27575, 27521,
 27466, 27411, 27356, 27300, 27245, 27189, 27133,
 27076, 27019, 26962, 26905, 26848, 26790, 26732,
 26674, 26615, 26556, 26497, 26438, 26378, 26319,
 26259, 26198, 26138, 26077, 26016, 25955, 25893,
 25832, 25770, 25708, 25645, 25582, 25519, 25456,
 25393, 25329, 25265, 25201, 25137, 25072, 25007,
 24942, 24877, 24811, 24746, 24680, 24613, 24547,
 24480, 24413, 24346, 24279, 24211, 24143, 24075,
 24007, 23938, 23870, 23801, 23731, 23662, 23592,
 23522, 23452, 23382, 23311, 23241, 23170, 23099,
 23027, 22956, 22884, 22812, 22739, 22667, 22594,
 22521, 22448, 22375, 22301, 22227, 22154, 22079,
 22005, 21930, 21856, 21781, 21705, 21630, 21554,
 21479, 21403, 21326, 21250, 21173, 21096, 21019,
 20942, 20865, 20787, 20709, 20631, 20553, 20475,
 20396, 20317, 20238, 20159, 20080, 20000, 19921,
 19841, 19761, 19680, 19600, 19519, 19438, 19357,
 19276, 19195, 19113, 19032, 18950, 18868, 18785,
 18703, 18620, 18537, 18454, 18371, 18288, 18204,
 18121, 18037, 17953, 17869, 17784, 17700, 17615,
 17530, 17445, 17360, 17275, 17189, 17104, 17018,
 16932, 16846, 16759, 16673, 16586, 16499, 16413,
 16325, 16238, 16151, 16063, 15976, 15888, 15800,
 15712, 15623, 15535, 15446, 15358, 15269, 15180,
 15090, 15001, 14912, 14822, 14732, 14643, 14553,
 14462, 14372, 14282, 14191, 14101, 14010, 13919,
 13828, 13736, 13645, 13554, 13462, 13370, 13279,
 13187, 13094, 13002, 12910, 12817, 12725, 12632,
 12539, 12446, 12353, 12260, 12167, 12074, 11980,
 11886, 11793, 11699, 11605, 11511, 11417, 11322,
 11228, 11133, 11039, 10944, 10849, 10754, 10659,
 10564, 10469, 10374, 10278, 10183, 10087,  9992,
  9896,  9800,  9704,  9608,  9512,  9416,  9319,
  9223,  9126,  9030,  8933,  8836,  8739,  8642,
  8545,  8448,  8351,  8254,  8157,  8059,  7962,
  7864,  7767,  7669,  7571,  7473,  7375,  7277,
  7179,  7081,  6983,  6885,  6786,  6688,  6590,
  6491,  6393,  6294,  6195,  6096,  5998,  5899,
  5800,  5701,  5602,  5503,  5404,  5305,  5205,
  5106,  5007,  4907,  4808,  4708,  4609,  4509,
  4410,  4310,  4210,  4111,  4011,  3911,  3811,
  3712,  3612,  3512,  3412,  3312,  3212,  3112,
  3012,  2911,  2811,  2711,  2611,  2511,  2410,
  2310,  2210,  2110,  2009,  1909,  1809,  1708,
  1608,  1507,  1407,  1307,  1206,  1106,  1005,
   905,   804,   704,   603,   503,   402,   302,
   201,   101 };



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/*
void lt_beacon(void)
{
		for(i=100; i<2000; i++)
		{
			__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, i);
		}
		HAL_Delay(1);

		for(i=2000; i>100; i--)
		{
			__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1, i);
		}
		HAL_Delay(1);

//	if( ((cnt>=0)&&(cnt<=40)) || ((cnt>=50)&&(cnt<=90)) )
//	{
//		HAL_GPIO_WritePin(lt_bea_GPIO_Port, lt_bea_Pin, GPIO_PIN_SET);
//	}
//	else
//	{
//		HAL_GPIO_WritePin(lt_bea_GPIO_Port, lt_bea_Pin, GPIO_PIN_RESET);
//	}

}
*/

void lt_position(void)
{
	HAL_GPIO_WritePin(lt_pos_GPIO_Port, lt_pos_Pin, GPIO_PIN_SET);
}

void lt_anticollition(void)
{
	//ON | OFF | ON | OFF | ON | OFF (50 ms ON 100ms OFF)
	//if( ((cnt>=0)&&(cnt<=1)) || ((cnt>=3)&&(cnt<=4)) || ((cnt>=6)&&(cnt<=7)) || ((cnt>=50)&&(cnt<=51)) || ((cnt>=53)&&(cnt<=54)) || ((cnt>=56)&&(cnt<=57)))
	if( (cnt==0) || (cnt==3) || (cnt==6) || (cnt==50) || (cnt==53) || (cnt==56) )
	{
		HAL_GPIO_WritePin(lt_ac_GPIO_Port, lt_ac_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(lt_ac_GPIO_Port, lt_ac_Pin, GPIO_PIN_RESET);
	}
}

void lt_landing(void)
{
	HAL_GPIO_WritePin(lt_lan_GPIO_Port, lt_lan_Pin, GPIO_PIN_SET);
}


void lt_search(void)
{
	HAL_GPIO_WritePin(lt_sea_GPIO_Port, lt_sea_Pin, GPIO_PIN_SET);
}





/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  //HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, data, 1024);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //lt_beacon();
	  lt_position();
	  lt_anticollition();
	  lt_landing();
	  lt_search();

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

/* USER CODE BEGIN 4 */
HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1)
	{
		cnt++;
		if(cnt >= 100)
		{
			cnt = 0;
		}
	}
}
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
