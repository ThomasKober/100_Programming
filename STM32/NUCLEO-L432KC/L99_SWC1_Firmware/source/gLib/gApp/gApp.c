
/**
 * @file    gApp.c
 * @brief   This library contains functions that are called to initialize before or after
 *          all other libraries and the user loop.
 *
 * @author  Nima Askari , Askari@gtec.at , Nima.Askari@gmail.com
 * @date    21/03/2025 09:27:53
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

#include "gApp.h"
#include "gCore.h"

/* USER CODE BEGIN header */
#include "gBuz.h"
#include "gVib.h"
#include "mp8859.h"
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

/* USER CODE BEGIN private function prototypes */

/* USER CODE END private function prototypes */

/**
 * ************************************************************************************************
 * global variable
 * ************************************************************************************************
 */

/* USER CODE BEGIN global variable */

const GBuzNoteSequence gbuz_beep_beep_[] =
{
  {GBUZ_NOTE_D5, 50},
  {GBUZ_NOTE_NONE, 100},
  {GBUZ_NOTE_D5, 50},
  {GBUZ_NOTE_NONE, 0}
};

/* USER CODE END global variable */

/**
 * ************************************************************************************************
 * public function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Pre-initializes application libraries
 * @param  None
 * @return None
 * @note   This function is called before all other initialization routines
 */
void gAppPreInit(void)
{
  /* USER CODE BEGIN App PreInitializes */

  /* USER CODE END App PreInitializes */
}

/**
 * ************************************************************************************************
 * @brief  Initializes application libraries
 * @param  None
 * @return None
 * @note   This function is called after all other initialization routines
 */
void gAppInit(void)
{
  /* USER CODE BEGIN App Initializes */
  gVibInit();
  gBuzPlay(gbuz_beep_beep_);
  /* USER CODE END App Initializes */
}

/**
 * ************************************************************************************************
 * @brief  De-initializes application libraries
 * @param  None
 * @return None
 * @note   This function releases resources or resets states as needed before application shutdown
 *         or in test routines.
 */
void gAppDeInit(void)
{
  /* USER CODE BEGIN App DeInitializes */
  gVibDeInit();
  /* USER CODE END App DeInitializes */
}

/**
 * ************************************************************************************************
 * @brief  Main application loop for processing events and handling logic
 * @param  None
 * @return None
 * @note   This function runs continuously, executing in core application tasks and responding to events
 */
void gAppLoop(void)
{
  /* USER CODE BEGIN core Loop */
  gVibLoop();
  mp8859Loop();
  /* USER CODE END core Loop */
}

/* USER CODE BEGIN public library functions */

/* USER CODE END public library functions */

/**
 * ************************************************************************************************
 * private function implementations
 * ************************************************************************************************
 */

/* USER CODE BEGIN private function implementation */

/**
 * ************************************************************************************************
 *
 */

/* USER CODE END private function implementation */
