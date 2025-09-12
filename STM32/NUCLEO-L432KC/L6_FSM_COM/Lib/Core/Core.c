/*
 * Core.c
 *
 *  Created on: Apr 23, 2025
 *      Author: g.tec
 */

#include "Core.h"
#include "Com.h"

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

void CoreBlinker(void);


/* USER CODE BEGIN private function prototypes */

/* USER CODE END private function prototypes */

/**
 * ************************************************************************************************
 * global variable
 * ************************************************************************************************
 */

uint32_t      core_blinker_time = 0;

/* USER CODE BEGIN global variable */

/* USER CODE END global variable */


/**
 * ************************************************************************************************
 * private function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Manages the blinking of an LED at a specified interval.
 * @param  None
 * @return None
 * @note   This function checks if the specified interval has elapsed since the last
 *         toggle of the LED. If the interval has passed, it refreshes the
 *         independent watchdog timer to prevent system resets and updates the
 *         blinker's timing.
 *         - The actual toggling of the LED can be implemented in the
 *           `USER CODE BEGIN blinker` section by uncommenting the
 *           HAL_GPIO_TogglePin() line, which provides a visual indication of
 *           the system's operational status.
 *         - The blinking interval is defined by the constant `GCORE_BLINKER_INTERVAL_MS`.
 *         - This function should be called regularly within the main loop to
 *           ensure timely execution of the blinking functionality.
 */
void CoreBlinker(void)
{
  if (HAL_GetTick() - core_blinker_time >= CORE_BLINKER_INTERVAL_MS)
  {
    core_blinker_time = HAL_GetTick();
    //HAL_IWDG_Refresh(&hiwdg);

    /* USER CODE BEGIN blinker */
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
    /* USER CODE END blinker */
  }
}

void CoreLoop(void)
{
	CoreBlinker();
	ComLoop();
}
