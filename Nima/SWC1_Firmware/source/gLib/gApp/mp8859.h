
/**
 * @file    mp8859.h
 * @brief
 *
 * Detailed description explaining the purpose and usage of this file,
 * such as any context or dependencies it has. It can also include a description
 * of the related module or functionality.
 *
 * @author  Nima Askari , Askari@gtec.at , Nima.Askari@gmail.com
 *
 * @copyright
 *  Copyright (C) 2024 g.tec medical engineering GmbH.
 *  All rights reserved.
 *
 *  This software is proprietary to g.tec medical engineering GmbH. No part of this
 *  software may be reproduced, distributed, or transmitted in any form or by any means,
 *  without the prior written permission of g.tec medical engineering GmbH.
 */

#ifndef _MP8859_H_
#define _MP8859_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ************************************************************************************************
 * includes
 * ************************************************************************************************
 */

#include "main.h"
#include <stdbool.h>

/**
 * ************************************************************************************************
 * library configuration
 * ************************************************************************************************
 */

#define MP8859_I2C                    hi2c3
#define MP8859_I2C_ADDRESS            (0x60 << 1)
#define MP8859_CHECK_TIME_MS          10
#define MP8859_DEFAULT_VOLTAGE        4000
#define MP8859_DEFAULT_CURRENT        1000
#define MP8859_EN_GPIO                DCDC_EN_GPIO_Port
#define MP8859_EN_PIN                 DCDC_EN_Pin
#define MP8859_ALT_GPIO               DCDC_ALT_GPIO_Port
#define MP8859_ALT_PIN                DCDC_ALT_Pin

#define MP8859_MIN_VOLTAGE            2000
#define MP8859_MAX_VOLTAGE            5000
#define MP8859_MAX_CURRENT            2000

#define MP8859_CTRL1_EN               7
#define MP8859_CTRL1_HICCUP_OCP_OVP   6
#define MP8859_CTRL1_DISCHG_EN        5
#define MP8859_CTRL1_MODE             4

#define MP8859_CTRL2_LINE_DROP_COMP_1 7
#define MP8859_CTRL2_LINE_DROP_COMP_0 6
#define MP8859_CTRL2_SS_1             5
#define MP8859_CTRL2_SS_0             4

#define MP8859_STATUS_PG              7
#define MP8859_STATUS_OPT             6
#define MP8859_STATUS_OTW             5
#define MP8859_STATUS_CC_CV           4

/**
 * ************************************************************************************************
 * struct/enum definitions
 * ************************************************************************************************
 */

typedef enum
{
  MP8859_REG_VOUT_L                   = 0x00,
  MP8859_REG_VOUT_H                   = 0x01,
  MP8859_REG_VOUT_GO                  = 0x02,
  MP8859_REG_IOUT_LIM                 = 0x03,
  MP8859_REG_CTRL1                    = 0x04,
  MP8859_REG_CTRL2                    = 0x05,
  MP8859_REG_STATUS                   = 0x09,
  MP8859_REG_IRQ                      = 0x0A,
  MP8859_REG_MASK                     = 0x0B,

} Mp8859Reg;

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */


/**
 * ************************************************************************************************
 * public library functions
 * ************************************************************************************************
 */

bool          mp8859Init(void);
void          mp8859DeInit(void);
void          mp8859Loop(void);
bool          mp8859SetVol(uint32_t voltage_mv);
bool          mp8859GetVol(uint32_t *voltage_mv);
bool          mp8859SetCur(uint32_t current_ma);
bool          mp8859GetCur(uint32_t *current_ma);
void          mp8859Enable(bool enable_disable);

void          mp8859CallbackPowerFail(void);

#ifdef __cplusplus
}
#endif
#endif
