
/**
 * @file    gCore.h
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

#ifndef _GCORE_H_
#define _GCORE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ************************************************************************************************
 * includes
 * ************************************************************************************************
 */

#include "main.h"

/* USER CODE BEGIN header */

/* USER CODE END header */

/**
 * ************************************************************************************************
 * library configuration
 * ************************************************************************************************
 */

/* USER CODE BEGIN library configuration */

#define GCORE_BLINKER_INTERVAL_MS			500UL

/* USER CODE END library configuration */

/**
 * ************************************************************************************************
 * struct/enum definitions
 * ************************************************************************************************
 */

/* USER CODE BEGIN struct/enum definitions */

/* USER CODE END struct/enum definitions */

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */

/* USER CODE BEGIN public variable */

/* USER CODE END public variable */

/**
 * ************************************************************************************************
 * public library functions
 * ************************************************************************************************
 */

void          gCorePreInit(void);
void          gCoreInit(void);
void          gCoreDeInit(void);
void          gCoreLoop(void);
void          gCoreHalt(void);

/* USER CODE BEGIN public library functions */

/* USER CODE END public library functions */

#ifdef __cplusplus
}
#endif
#endif
