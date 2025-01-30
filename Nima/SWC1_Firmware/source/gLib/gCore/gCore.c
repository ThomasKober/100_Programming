
/**
 * @file    gCore.c
 * @brief   This library contains the entry point of the GLib,
 *          such as the main initialization and loop.
 *          All other functions and libraries are called from this library.
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

#include "gCore.h"
#include "gCom.h"
#include "gInfo.h"
#include "gTemp.h"
#include "gBuz.h"
#include "iwdg.h"
#include <stdio.h>

/* USER CODE BEGIN header */
#include "gApp.h"
/* USER CODE END header */

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

void          gCoreBlinker(void);
void          gCoreCheckResetSource(void);
int           __io_putchar(int ch);
void          HAL_RCC_CSSCallback(void);

/* USER CODE BEGIN private function prototypes */

/* USER CODE END private function prototypes */

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */

uint32_t      gcore_blinker_time_ = 0;

/* USER CODE BEGIN public variable */

/* USER CODE END public variable */

/**
 * ************************************************************************************************
 * public function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Configures the vector table location for the Cortex-M processor.
 * @param  None
 * @return None
 * @note   This function sets the vector table offset register (VTOR) to point
 *         to the beginning of the interrupt vector table. It is typically called
 *         during the system initialization phase to ensure that the processor
 *         correctly handles interrupts and exceptions by using the provided
 *         vector table located at the address of `g_pfnVectors`.
 *         This function is crucial for setting up the correct interrupt
 *         handling environment in embedded applications.
 */
void gCorePreInit(void)
{
  extern const uint32_t g_pfnVectors[];
  SCB->VTOR = (uint32_t)&g_pfnVectors[0];
}

/**
 * ************************************************************************************************
 * @brief  Initializes core system components and application modules.
 * @param  None
 * @return None
 * @note   This function is responsible for the initialization of various
 *         core components and application modules. It performs the following:
 *         - Calls the application pre-initialization function `gAppPreInit()`
 *           to set up any necessary application-level configurations before
 *           core initializations.
 *         - Initializes core components such as information structures,
 *           buzzer, temperature sensors, and communication interfaces.
 *         - Checks the reset source of the system to determine the reason
 *           for the last reset, which can help in diagnosing system behavior.
 *         - Finally, it calls the application initialization function
 *           `gAppInit()` to complete the setup.
 *         This function should be called during the system startup phase to
 *         ensure all components are properly initialized before normal
 *         operation begins.
 */
void gCoreInit(void)
{
  /* USER CODE BEGIN core PreInitializes */
	gAppPreInit();
  /* USER CODE END core PreInitializes */

  gInfoInit();
  gBuzInit();
  gTempInit();
  gComInit();
  gCoreCheckResetSource();

  /* USER CODE BEGIN core Initializes */
	gAppInit();
  /* USER CODE END core Initializes */
}

/**
 * ************************************************************************************************
 * @brief  Deinitializes core system components and application modules.
 * @param  None
 * @return None
 * @note   This function is responsible for safely deinitializing various core
 *         components and application modules. It performs the following actions:
 *         - Calls the deinitialization functions for the information structures,
 *           buzzer, temperature sensors, and communication interfaces to
 *           release any resources they are using and reset their states.
 *         - Calls the application deinitialization function `gAppDeInit()`
 *           to ensure that all application-level resources are properly cleaned
 *           up before the system shuts down or resets.
 *         This function should be invoked when the system is being powered down
 *         or when a reset is required to ensure that all components are properly
 *         deinitialized, preventing resource leaks and ensuring a clean state for
 *         future operations.
 */
void gCoreDeInit(void)
{
  gInfoDeInit();
  gBuzDeInit();
  gTempDeInit();
  gComDeInit();

  /* USER CODE BEGIN core DeInitializes */
	gAppDeInit();
  /* USER CODE END core DeInitializes */
}

/**
 * ************************************************************************************************
 * @brief  Executes the main loop for core system operations.
 * @param  None
 * @return None
 * @note   This function serves as the main loop for executing core system tasks.
 *         It continuously calls various loop functions for core components to
 *         perform their respective operations. The following functions are called:
 *         - `gCoreBlinker()`: Manages the blinking of any status indicators,
 *           typically used for visual feedback on the system's state and resets the Watchdog Timer.
 *         - `gBuzLoop()`: Handles buzzer operations, allowing for sound
 *           notifications or alerts as needed.
 *         - `gTempLoop()`: Manages temperature-related tasks, such as
 *           monitoring or responding to temperature changes.
 *         - `gComLoop()`: Processes communication tasks, handling incoming
 *           and outgoing data through the communication interface.
 *         - `gAppLoop()`: Calls the application-specific loop function, allowing
 *           the application to perform its tasks within the core loop.
 *         This function is intended to be called in a continuous manner,
 *         typically within a main application loop, to ensure that all core
 *         components operate smoothly and efficiently.
 */
void gCoreLoop(void)
{
  gCoreBlinker();
  gBuzLoop();
  gTempLoop();
  gComLoop();

  /* USER CODE BEGIN core Loop */
	gAppLoop();
  /* USER CODE END core Loop */
}

/**
 * ************************************************************************************************
 * @brief  Halts the core system operations and enters an infinite loop.
 * @param  None
 * @return None
 * @note   This function disables all interrupts and enters an infinite loop to halt
 *         system operations in case of a critical error or fault. The user can
 *         implement additional error handling, such as activating an error LED
 *         or logging the fault condition within the `USER CODE BEGIN halt` section.
 *         The infinite loop ensures that the system remains in a safe state,
 *         while the watchdog timer is refreshed to prevent system reset, allowing
 *         for potential debugging or recovery actions.
 *         - The GPIO pin for an error LED can be controlled by uncommenting the
 *           HAL_GPIO_WritePin() line, allowing visual indication of an error state.
 */
void gCoreHalt(void)
{
  __disable_irq();

  /* USER CODE BEGIN halt */
	HAL_GPIO_WritePin(LED_ERR_GPIO_Port, LED_ERR_Pin, GPIO_PIN_RESET);
  /* USER CODE END halt */

  while (1)
  {
    HAL_IWDG_Refresh(&hiwdg);
  }
}

/* USER CODE BEGIN public library functions */

/* USER CODE END public library functions */

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
void gCoreBlinker(void)
{
  if (HAL_GetTick() - gcore_blinker_time_ >= GCORE_BLINKER_INTERVAL_MS)
  {
    gcore_blinker_time_ = HAL_GetTick();
    HAL_IWDG_Refresh(&hiwdg);

    /* USER CODE BEGIN blinker */
		HAL_GPIO_TogglePin(LED_SYS_GPIO_Port, LED_SYS_Pin);
    /* USER CODE END blinker */
  }
}

/**
 * ************************************************************************************************
 * @brief  Checks the source of the last reset event and updates the system status code accordingly.
 * @param  None
 * @return None
 * @note   This function reads the reset source flags from the RCC (Reset and Clock Control)
 *         register to determine the cause of the last reset. Based on the identified reset
 *         source, it sets the appropriate status code using `gComSetStatusCode()`.
 *         The following reset sources are checked:
 *         - BOR (Brown-Out Reset)
 *         - Software Reset
 *         - Independent Watchdog Reset
 *         - Window Watchdog Reset
 *         - Low-Power Reset
 *         - External Reset (Reset Pin)
 *
 *         After processing the reset source, the reset flags are cleared by writing to the
 *         RCC reset source register. This function should be called early in the system
 *         initialization process to ensure the correct status code is set before
 *         proceeding with other operations.
 */
void gCoreCheckResetSource(void)
{
  uint32_t rst_source = (RCC->RSR & RCC_RESET_FLAG_ALL);

  /* USER CODE BEGIN start gCoreCheckResetSource() */

  /* USER CODE END start gCoreCheckResetSource */

  if (rst_source & RCC_RSR_BORRSTF)
  {
    /* USER CODE BEGIN power on or BOR reset flag */
		gComSetStatusCode(GCOM_STATUS_CODE_POWER_ON);
    /* USER CODE END power on or BOR reset flag */
  }
  else if (rst_source & RCC_RSR_SFTRSTF)
  {
    /* USER CODE BEGIN Software Reset flag */
		gComSetStatusCode(GCOM_STATUS_CODE_FW);
    /* USER CODE END Software Reset flag */
  }
  else if (rst_source & RCC_RSR_IWDGRSTF)
  {
    /* USER CODE BEGIN iwdt */
		gComSetStatusCode(GCOM_STATUS_CODE_WDT);
    /* USER CODE END iwdt */
  }
  else if (rst_source & RCC_RSR_WWDGRSTF)
  {
    /* USER CODE BEGIN wwdt */
		gComSetStatusCode(GCOM_STATUS_CODE_WDT);
    /* USER CODE END wwdt */
  }
  else if (rst_source & RCC_RSR_LPWRRSTF)
  {
    /* USER CODE BEGIN Low-Power reset flag */
		gComSetStatusCode(GCOM_STATUS_CODE_BROWN_OUT);
    /* USER CODE END Low-Power reset flag */
  }
  else if (rst_source & RCC_RSR_PINRSTF)
  {
    /* USER CODE BEGIN normal power up or reset pin flag */
		gComSetStatusCode(GCOM_STATUS_CODE_EXT_RST);
    /* USER CODE END normal power up or reset pin flag */
  }

  /* USER CODE BEGIN finish gCoreCheckResetSource */
	RCC->RSR |= RCC_RSR_RMVF; 															///< erase reset flags
  /* USER CODE END finish gCoreCheckResetSource */
}

/**
 * ************************************************************************************************
 * @brief  Sends a character to the ITM (Instrumentation Trace Macrocell) for output.
 * @param  ch  The character to be sent.
 * @return Returns 1 to indicate that the character was successfully sent.
 * @note   This function implements the low-level output routine for the standard I/O library.
 *         It uses the ITM_SendChar function to transmit a character over the ITM interface,
 *         which is typically used for debugging purposes in embedded systems.
 *         The function assumes that the ITM is properly initialized and that the debugger
 *         is connected and capable of receiving ITM output.
 *         This function is called whenever a character needs to be printed to the output console.
 */
int __io_putchar(int ch)
{
  ITM_SendChar(ch);
  return 1;
}

/**
 * ************************************************************************************************
 * @brief  Callback function called when the Clock Security System (CSS) detects a clock failure.
 * @param  None
 * @return None
 * @note   This function is executed when the CSS detects a failure in the system clock,
 *         such as when the external clock source is no longer stable or operational.
 *         The default implementation halts the core, preventing further operation until
 *         the system is reset or reinitialized. This function should be defined as
 *         an interrupt handler for the CSS interrupt.
 *         The user can add additional error handling or recovery mechanisms within this function.
 */
void HAL_RCC_CSSCallback(void)
{
  /* USER CODE BEGIN HAL_RCC_CSSCallback() */
  gCoreHalt();
  /* USER CODE END HAL_RCC_CSSCallback() */
}

/* USER CODE BEGIN private function implementation */

/* USER CODE END private function implementation */
