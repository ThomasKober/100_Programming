
/**
 * @file    gApp.h
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

#ifndef _GAPP_H_
#define _GAPP_H_

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

void          gAppPreInit(void);
void          gAppInit(void);
void          gAppDeInit(void);
void          gAppLoop(void);

/* USER CODE BEGIN public library functions */

/* USER CODE END public library functions */

#ifdef __cplusplus
}
#endif
#endif
