
/**
 * @file    gBuf.h
 * @brief   This library contains functions for creating a ring buffer.
 *          Keep in mind that this library uses the malloc function,
 *          so you need to increase the system heap memory in the linker script.
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

#ifndef _GBUF_H_
#define _GBUF_H_

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

// none

/**
 * ************************************************************************************************
 * struct/enum definitions
 * ************************************************************************************************
 */

typedef struct
{
  size_t                              item_size;
  uint32_t                            capacity;
  uint32_t                            head;
  uint32_t                            tail;
  uint32_t                            count;
  void                                *ptr;

} GBufHandle;

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */

// none

/**
 * ************************************************************************************************
 * public library functions
 * ************************************************************************************************
 */

GBufHandle*   gBufInit(size_t item_size, uint32_t capacity);
void          gBufDeInit(GBufHandle *gbuf);
bool          gBufIsFull(const GBufHandle *gbuf);
bool          gBufIsEmpty(const GBufHandle *gbuf);
uint32_t      gBufAvailable(const GBufHandle *gbuf);
uint32_t      gBufUsed(const GBufHandle *gbuf);
void          gBufFormat(GBufHandle *gbuf);
bool          gBufPush(GBufHandle *gbuf, const void *data);
bool          gBufPushN(GBufHandle *gbuf, const void *data, uint32_t num);
bool          gBufPop(GBufHandle *gbuf, void *data);
bool          gBufPopN(GBufHandle *gbuf, void *data, uint32_t num);

#ifdef __cplusplus
}
#endif
#endif
