
/**
 * @file    gBuf.c
 * @brief   This library contains functions for creating a ring buffer.
 *          Keep in mind that this library uses the malloc function,
 *          so you need to increase the system heap memory in the linker script.
 *
 * @author  Nima Askari , Askari@gtec.at , Nima.Askari@gmail.com
 * @date    12/11/2024 10:25:44
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

#include "gBuf.h"
#include <string.h>
#include <stdlib.h>

/**
 * ************************************************************************************************
 * private definitions
 * ************************************************************************************************
 */

// none

/**
 * ************************************************************************************************
 * private function prototypes
 * ************************************************************************************************
 */

// none

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */

// none

/**
 * ************************************************************************************************
 * public function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Initializes a ring buffer for storing items
 * @param  item_size [in] Size of each item in bytes
 * @param  capacity  [in] Maximum number of items the buffer can hold
 * @return Pointer to the initialized buffer handle, or NULL on failure
 * @note   This function allocates memory for the buffer and sets its size and capacity.
 */
GBufHandle* gBufInit(size_t item_size, uint32_t capacity)
{
  GBufHandle *gbuf = (GBufHandle*) malloc(sizeof(GBufHandle));
  do
  {
    if (gbuf == NULL)
    {
      break;
    }
    gbuf->item_size = item_size;
    gbuf->capacity = capacity;
    gbuf->ptr = malloc(item_size * capacity);
    if (gbuf->ptr == NULL)
    {
      free(gbuf);
      gbuf = NULL;
      break;
    }
    gbuf->head = 0;
    gbuf->tail = 0;
    gbuf->count = 0;

  } while (0);

  return gbuf;
}

/**
 * ************************************************************************************************
 * @brief  De-initializes and frees the resources associated with the buffer
 * @param  gbuf [in] Pointer to the buffer handle to be de-initialized
 * @return None
 * @note   This function releases the allocated memory for the buffer and resets its handle.
 */
void gBufDeInit(GBufHandle *gbuf)
{
  if (gbuf != NULL)
  {
    if (gbuf->ptr != NULL)
    {
      free(gbuf->ptr);
    }
    free(gbuf);
  }
}

/**
 * ************************************************************************************************
 * @brief  Checks if the buffer is full
 * @param  gbuf [in] Pointer to the buffer handle to be checked
 * @return true if the buffer is full, false otherwise
 * @note   This function evaluates the current state of the buffer and returns its fullness status.
 */
bool gBufIsFull(const GBufHandle *gbuf)
{
  return (gbuf->count == gbuf->capacity);
}

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
bool gBufIsEmpty(const GBufHandle *gbuf)
{
  return (gbuf->count == 0);
}

/**
 * ************************************************************************************************
 * @brief  Checks if the buffer is empty
 * @param  gbuf [in] Pointer to the buffer handle to be checked
 * @return true if the buffer is empty, false otherwise
 * @note   This function evaluates the current state of the buffer and returns its emptiness status.
 */
uint32_t gBufAvailable(const GBufHandle *gbuf)
{
  return (gbuf->capacity - gbuf->count);
}

/**
 * ************************************************************************************************
 * @brief  Returns the number of items currently stored in the buffer
 * @param  gbuf [in] Pointer to the buffer handle to be checked
 * @return Number of items currently in use in the buffer
 * @note   This function provides the count of items that have been stored to the buffer.
 */
uint32_t gBufUsed(const GBufHandle *gbuf)
{
  return gbuf->count;
}

/**
 * ************************************************************************************************
 * @brief  Formats the buffer, resetting it to its initial state
 * @param  gbuf [in] Pointer to the buffer handle to be formatted
 * @return None
 * @note   This function clears the buffer and prepares it for new data.
 */
void gBufFormat(GBufHandle *gbuf)
{
  gbuf->head = 0;
  gbuf->tail = 0;
  gbuf->count = 0;
  memset(gbuf->ptr, 0, gbuf->capacity * gbuf->item_size);
}

/**
 * ************************************************************************************************
 * @brief  Adds an item to the buffer
 * @param  gbuf [in] Pointer to the buffer handle where the item will be added
 * @param  data [in] Pointer to the data to be added to the buffer
 * @return true if the item was successfully added, false if the buffer is full
 * @note   This function attempts to push the specified data into the buffer and returns the success status.
 */
bool gBufPush(GBufHandle *gbuf, const void *data)
{
  bool answer = false;
  do
  {
    if (gBufIsFull(gbuf))
    {
      break;
    }
    uint8_t *dest = (uint8_t*) gbuf->ptr + gbuf->head * gbuf->item_size;
    memcpy(dest, data, gbuf->item_size);
    gbuf->head = (gbuf->head + 1) % gbuf->capacity;
    gbuf->count++;
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Adds multiple items to the buffer
 * @param  gbuf [in] Pointer to the buffer handle where the items will be added
 * @param  data [in] Pointer to the data to be added to the buffer
 * @param  num  [in] Number of items to be added from the data source
 * @return true if all items were successfully added, false if the buffer does not have enough space
 * @note   This function attempts to push the specified number of items into the buffer and returns the success status.
 */
bool gBufPushN(GBufHandle *gbuf, const void *data, uint32_t num)
{
  const uint8_t *src = (const uint8_t*) data;
  bool answer = false;
  do
  {
    if (num > gBufAvailable(gbuf))
    {
      break;
    }
    for (uint32_t i = 0; i < num; ++i)
    {
      uint8_t *dest = (uint8_t*) gbuf->ptr + gbuf->head * gbuf->item_size;
      memcpy(dest, src, gbuf->item_size);
      gbuf->head = (gbuf->head + 1) % gbuf->capacity;
      gbuf->count++;
      src += gbuf->item_size;
    }
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Reads an item from the buffer
 * @param  gbuf [in] Pointer to the buffer handle from which the item will be read
 * @param  data [out] Pointer to the location where the read item will be stored
 * @return true if an item was successfully read, false if the buffer is empty
 * @note   This function attempts to read the most recent item from the buffer and stores it in the provided location.
 */
 bool gBufPop(GBufHandle *gbuf, void *data)
{
  bool answer = false;
  do
  {
    if (gBufIsEmpty(gbuf))
    {
      break;
    }
    uint8_t *src = (uint8_t*) gbuf->ptr + gbuf->tail * gbuf->item_size;
    memcpy(data, src, gbuf->item_size);
    gbuf->tail = (gbuf->tail + 1) % gbuf->capacity;
    gbuf->count--;
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Reads multiple items from the buffer
 * @param  gbuf [in] Pointer to the buffer handle from which the items will be read
 * @param  data [out] Pointer to the location where the read items will be stored
 * @param  num  [in] Number of items to be read from the buffer
 * @return true if all items were successfully read, false if the buffer does not contain enough items
 * @note   This function attempts to read the specified number of items from the buffer and stores them in the provided location.
 */
 bool gBufPopN(GBufHandle *gbuf, void *data, uint32_t num)
{
  uint8_t *dest = (uint8_t*) data;
  bool answer = false;
  do
  {
    if (num > gBufUsed(gbuf))
    {
      break;
    }
    for (uint32_t i = 0; i < num; ++i)
    {
      uint8_t *src = (uint8_t*) gbuf->ptr + gbuf->tail * gbuf->item_size;
      memcpy(dest, src, gbuf->item_size);
      gbuf->tail = (gbuf->tail + 1) % gbuf->capacity;
      gbuf->count--;
      dest += gbuf->item_size;
    }
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * private function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief
 * @param
 * @return
 * @note
 */
