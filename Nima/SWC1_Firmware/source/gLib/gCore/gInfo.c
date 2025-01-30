
/**
 * @file    gInfo.c
 * @brief   This library contains all header information, EEPROM data,
 *          and functions for reading and writing configurations.
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

#include "gInfo.h"
#include "gCore.h"
#include <string.h>
#include <stdio.h>

/* USER CODE BEGIN header */

/* USER CODE END header */

/**
 * ************************************************************************************************
 * private definitions
 * ************************************************************************************************
 */

#define GINFO_MEM_SIZE                FLASH_SECTOR_SIZE
#define GINFO_MEM_SECTOR              ((FLASH_SIZE / GINFO_MEM_SIZE / 2) - 1)
#define GINFO_MEM_ADDRESS             (FLASH_BASE + GINFO_MEM_SIZE * (GINFO_MEM_SECTOR * 2 + 1))

/**
 * ************************************************************************************************
 * private function prototypes
 * ************************************************************************************************
 */

bool          gInfoMemInit(void *ptr, uint32_t size);
void          gInfoMemDeInit(void);
uint32_t      gInfoMemCapacity(void);
bool          gInfoMemFormat(void);
bool          gInfoMemRead(void);
bool          gInfoMemWrite(void);

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */

GInfoStorageStruct  ginfo_storage_;
GInfoMemHandle      ginfo_mem_handle_;

__section(".gInfoHeader") const GInfoHeaderStruct ginfo_header_ =
{
  .magic_word = GINFO_MAGIC_WORD,
  .header_major = GINFO_HEADER_MAJOR,
  .header_minor = GINFO_HEADER_MINOR,
  .fw_major = GINFO_FW_MAJOR,
  .fw_minor = GINFO_FW_MINOR,
  .fw_patch = GINFO_FW_PATCH,
  .fw_dev_build = GINFO_FW_DEV_BUILD,
  .length = 0,
  .hash = {0},
  .reserved = {0},
};

const GInfoConfigStorageStruct ginfo_config_initialize_ =
{
  .check_do_not_change_in_code = GINFO_CONFIG_CHECK_WORD,

  /* USER CODE BEGIN gInfo configuration Initializer */

	.custom_config = {0}

  /* USER CODE END gInfo configuration Initializer */
};

/**
 * ************************************************************************************************
 * public function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Initializes the information storage structures and checks their integrity.
 * @param  None
 * @return None
 * @note   This function performs several critical operations:
 *         - Validates the size of the header and storage structures against predefined constants.
 *         - Initializes memory for the information storage structure.
 *         - Reads existing information from the storage medium.
 *         - Checks the validity of the stored information using a predefined check word.
 *         - If the stored information is invalid, it resets the structure with default values
 *           and saves them to the storage medium.
 *         - If any check fails, the core is halted to prevent further execution until the issue is resolved.
 *         - If GINFO_CONFIG_CHECK_SKIPPED is not defined, it checks the configuration for
 *           integrity and resets it if necessary.
 */
void gInfoInit(void)
{
  if (sizeof(GInfoHeaderStruct) != GINFO_HEADER_SIZE)
  {
    gCoreHalt();
  }
  if (sizeof(GInfoStorageStruct) != FLASH_SECTOR_SIZE)
  {
    gCoreHalt();
  }
  if (sizeof(GInfoConfigStorageStruct) != GINFO_CONFIG_SIZE)
  {
    gCoreHalt();
  }
  if (gInfoMemInit(&ginfo_storage_, sizeof(GInfoStorageStruct)) == false)
  {
    gCoreHalt();
  }
  if (gInfoMemRead() == false)
  {
    gCoreHalt();
  }
  if (ginfo_storage_.check != GINFO_CHECK_WORD)
  {
    memset(&ginfo_storage_, 0, sizeof(GInfoStorageStruct));
    memcpy(&ginfo_storage_.config, &ginfo_config_initialize_, GINFO_CONFIG_SIZE);
    ginfo_storage_.check = GINFO_CHECK_WORD;
    strcpy(ginfo_storage_.hw, GINFO_HW_DEFAULT);
    strcpy(ginfo_storage_.sn, GINFO_SN_DEFAULT);
    if (gInfoSave() == false)
    {
      gCoreHalt();
    }
  }
#if GINFO_CONFIG_CHECK_SKIPPED == 0
  if (ginfo_storage_.config.check_do_not_change_in_code != GINFO_CONFIG_CHECK_WORD)
  {
    memcpy(&ginfo_storage_.config, &ginfo_config_initialize_, GINFO_CONFIG_SIZE);
    if (gInfoSave() == false)
    {
      gCoreHalt();
    }
  }
#endif
}

/**
 * ************************************************************************************************
 * @brief  Deinitializes the information storage structure by resetting its contents.
 * @param  None
 * @return None
 * @note   This function clears the entire information storage structure by setting all its bytes
 *         to zero using `memset`. This effectively resets all stored data, which can be useful
 *         when reinitializing or cleaning up resources before the system enters a low-power state
 *         or when the system is reset.
 *         - Note that this does not affect any persistent data stored in external memory; it only
 *           affects the in-memory representation of the information structure.
 */
void gInfoDeInit(void)
{
  memset(&ginfo_storage_, 0, sizeof(GInfoStorageStruct));
}

/**
 * ************************************************************************************************
 * @brief  Loads information from memory into the information storage structure.
 * @param  None
 * @return true if the loading process is successful; false otherwise.
 * @note   This function attempts to read the information stored in memory using the
 *         `gInfoMemRead()` function. If the read operation is successful, it sets the
 *         return value to true. If the read operation fails, the function will return false.
 *         This is typically used during the initialization phase of the application to
 *         restore the state of the system based on previously stored information.
 */
bool gInfoLoad(void)
{
  bool answer = false;
  do
  {
    if (gInfoMemRead() == false)
    {
      break;
    }
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Saves the current information to memory.
 * @param  None
 * @return true if successful; false if failed.
 * @note   Calls `gInfoMemWrite()` to perform the save operation.
 */
bool gInfoSave(void)
{
  bool answer = false;
  do
  {
    if (gInfoMemWrite() == false)
    {
      break;
    }
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Reads the hardware information from the storage
 * @param  str [out] Pointer to the buffer where the hardware information will be stored
 * @return true if the hardware information was successfully read, false otherwise
 * @note   Retrieves the hardware information from `ginfo_storage_.hw` after reading from memory.
 */
bool gInfoReadHw(char *str)
{
  bool answer = false;
  do
  {
    if (str == NULL)
    {
      break;
    }
    if (gInfoMemRead() == false)
    {
      break;
    }
    strcpy(str, ginfo_storage_.hw);
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Writes the hardware information to the storage
 * @param  str [in] Pointer to the hardware information string to be written
 * @return true if the hardware information was successfully written, false otherwise
 * @note   Validates the string and its length before copying it to `ginfo_storage_.hw` and saving it.
 */
bool gInfoWriteHw(const char *str)
{
  bool answer = false;
  do
  {
    if (str == NULL)
    {
      break;
    }
    if (strlen(str) > sizeof(ginfo_storage_.hw) - 1)
    {
      break;
    }
    if (gInfoMemRead() == false)
    {
    	break;
    }
    if (memcmp(ginfo_storage_.hw, str, sizeof(ginfo_storage_.hw)) != 0)
    {
    	memset(ginfo_storage_.hw, 0, sizeof(ginfo_storage_.hw));
    	strncpy(ginfo_storage_.hw, str, sizeof(ginfo_storage_.hw) - 1);
    	if (gInfoMemWrite() == false)
    	{
    		break;
    	}
    }
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Reads the serial number from the storage
 * @param  str [out] Pointer to the buffer where the serial number will be stored
 * @return true if the serial number was successfully read, false otherwise
 * @note   Retrieves the serial number from `ginfo_storage_.sn` after reading from memory.
 */
bool gInfoReadSn(char *str)
{
  bool answer = false;
  do
  {
    if (str == NULL)
    {
      break;
    }
    if (gInfoMemRead() == false)
    {
      break;
    }
    strcpy(str, ginfo_storage_.sn);
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Writes the serial number to the storage
 * @param  str [in] Pointer to the serial number string to be written
 * @return true if the serial number was successfully written, false otherwise
 * @note   Stores the serial number in `ginfo_storage_.sn` and saves to memory.
 */
bool gInfoWriteSn(const char *str)
{
  bool answer = false;
  do
  {
    if (str == NULL)
    {
      break;
    }
    if (strlen(str) > sizeof(ginfo_storage_.sn) - 1)
    {
      break;
    }
    if (gInfoMemRead() == false)
    {
    	break;
    }
    if (memcmp(ginfo_storage_.sn, str, sizeof(ginfo_storage_.sn)) != 0)
    {
    	memset(ginfo_storage_.sn, 0, sizeof(ginfo_storage_.sn));
    	strncpy(ginfo_storage_.sn, str, sizeof(ginfo_storage_.sn) - 1);
    	if (gInfoMemWrite() == false)
    	{
    		break;
    	}
    }
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Reads the firmware version
 * @param  str [out] Pointer to a buffer where the firmware version will be stored
 * @return true if the firmware version was successfully read, false otherwise
 * @note   Formats the firmware version as "major.minor.patch-build" and stores it in the provided buffer.
 */
bool gInfoReadFw(char *str)
{
  bool answer = false;
  do
  {
    if (str == NULL)
    {
      break;
    }
    sprintf(str, "%d.%d.%d-%03d", ginfo_header_.fw_major, ginfo_header_.fw_minor,\
        ginfo_header_.fw_patch, ginfo_header_.fw_dev_build);
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Reads the iv
 * @param  iv [out] Pointer to a buffer where the iv will be stored
 * @return true if the iv was successfully read, false otherwise
 * @note   Retrieves the iv from `ginfo_storage_.iv` after reading from memory.
 */
bool gInfoReadIv(uint8_t *iv)
{
  bool answer = false;
  do
  {
    if (iv == NULL)
    {
      break;
    }
    if (gInfoMemRead() == false)
    {
      break;
    }
    memcpy(iv, ginfo_storage_.iv, sizeof(ginfo_storage_.iv));
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Writes the iv to the storage
 * @param  iv [in] Pointer to the iv to be written
 * @return true if the iv was successfully written, false otherwise
 * @note   Stores the iv in `ginfo_storage_.iv` and saves to memory.
 */
bool gInfoWriteIv(const uint8_t *iv)
{
  bool answer = false;
  do
  {
    if (iv == NULL)
    {
      break;
    }
    if (gInfoMemRead() == false)
		{
			break;
		}
    if (memcmp(ginfo_storage_.iv, iv, sizeof(ginfo_storage_.iv)) != 0)
    {
      memset(ginfo_storage_.iv, 0, sizeof(ginfo_storage_.iv));
      memcpy(ginfo_storage_.iv, iv, sizeof(ginfo_storage_.iv));
      if (gInfoMemWrite() == false)
      {
        break;
      }
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
 * @brief  Initializes memory for GInfo storage
 * @param  ptr [in] Pointer to the memory location to be initialized
 * @param  size [in] Size of the memory to be initialized
 * @return true if memory was successfully initialized, false otherwise
 * @note   This function sets up the memory handle for GInfo storage. It checks if the provided
 *         size is valid and if the pointer is not NULL. If successful, it assigns the size
 *         and data pointer to the memory handle.
 */
bool gInfoMemInit(void *ptr, uint32_t size)
{
  bool answer = false;
  do
  {
    if ((size > GINFO_MEM_SIZE) || (ptr == NULL) || (size == 0))
    {
      ginfo_mem_handle_.size = 0;
      ginfo_mem_handle_.data_pointer = NULL;
      break;
    }
    ginfo_mem_handle_.size = size;
    ginfo_mem_handle_.data_pointer = (uint8_t*)ptr;
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Deinitializes the memory for GInfo storage
 * @param  None
 * @return None
 * @note   This function clears the GInfo memory handle by setting its contents to zero,
 *         effectively resetting its size and data pointer.
 */
void gInfoMemDeInit(void)
{
  memset(&ginfo_mem_handle_, 0, sizeof(GInfoMemHandle));
}

/**
 * ************************************************************************************************
 * @brief  Returns the total capacity of the GInfo memory
 * @param  None
 * @return Total capacity of the GInfo memory in bytes
 * @note   This function provides the size of the GInfo memory, which can be used to check
 *         the maximum amount of data that can be stored.
 */
uint32_t gInfoMemCapacity(void)
{
  return GINFO_MEM_SIZE;
}

/**
 * ************************************************************************************************
 * @brief  Formats the GInfo memory by erasing its contents
 * @param  None
 * @return true if the memory was successfully formatted, false otherwise
 * @note   This function unlocks the flash memory, erases the specified sector, and then
 *         locks the flash memory again. It checks for errors during the erase operation
 *         and returns the appropriate status.
 */
bool gInfoMemFormat(void)
{
  bool answer = false;
  uint32_t error;
  FLASH_EraseInitTypeDef flash_erase;
  do
  {
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
      break;
    }
#ifdef HAL_ICACHE_MODULE_ENABLED
    if (HAL_ICACHE_Disable() != HAL_OK)
    {
      break;
    }
#endif
    flash_erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    flash_erase.Sector = GINFO_MEM_SECTOR;
    flash_erase.NbSectors = 1;
    flash_erase.Banks = FLASH_BANK_2;
    if (HAL_FLASHEx_Erase(&flash_erase, &error) != HAL_OK)
    {
      break;
    }
    if (error != 0xFFFFFFFF)
    {
      break;
    }
    answer = true;

  }
  while (0);

  HAL_FLASH_Lock();
#ifdef HAL_ICACHE_MODULE_ENABLED
    HAL_ICACHE_Enable();
#endif
  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Reads data from the GInfo memory into the specified memory buffer
 * @param  None
 * @return true if the data was successfully read, false otherwise
 * @note   This function reads data from the GInfo memory address into the buffer pointed to
 *         by `ginfo_mem_handle_.data_pointer`. It disables the instruction cache during the
 *         read operation (if enabled) to ensure the data is correctly read from memory.
 */
bool gInfoMemRead(void)
{
  bool answer = false;
  uint8_t *data = ginfo_mem_handle_.data_pointer;
  do
  {
#ifdef HAL_ICACHE_MODULE_ENABLED
    if (HAL_ICACHE_Disable() != HAL_OK)
    {
      break;
    }
#endif
    if ((data != NULL) && (ginfo_mem_handle_.size > 0))
    {
      for (uint32_t i = 0; i < ginfo_mem_handle_.size; i++)
      {
        *data = (*(__IO uint8_t*) (GINFO_MEM_ADDRESS + i));
        data++;
      }
      answer = true;
    }

  }
  while (0);

#ifdef HAL_ICACHE_MODULE_ENABLED
  HAL_ICACHE_Enable();
#endif
  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Writes data from the specified memory buffer to the GInfo memory
 * @param  None
 * @return true if the data was successfully written, false otherwise
 * @note   This function first formats the GInfo memory, then unlocks the flash memory
 *         for writing. It writes data from the buffer pointed to by `ginfo_mem_handle_.data_pointer`
 *         in quadword (16 bytes) increments. After writing, it verifies that the written data
 *         matches the data in the buffer.
 */
bool gInfoMemWrite(void)
{
  bool answer = true;
  uint8_t *data = ginfo_mem_handle_.data_pointer;
  do
  {
    if (data == NULL)
    {
      answer = false;
      break;
    }
    if (gInfoMemFormat() == false)
    {
      answer = false;
      break;
    }
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
      break;
    }
#ifdef HAL_ICACHE_MODULE_ENABLED
    if (HAL_ICACHE_Disable() != HAL_OK)
    {
      break;
    }
#endif

    for (uint32_t i = 0; i < ginfo_mem_handle_.size; i += 16)
    {
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, GINFO_MEM_ADDRESS + i, (uint32_t)data) != HAL_OK)
      {
        answer = false;
        break;
      }
      data += 16;
    }
    data = ginfo_mem_handle_.data_pointer;
    for (uint32_t i = 0; i < ginfo_mem_handle_.size; i++)
    {
      if (*data != (*(__IO uint8_t*) (GINFO_MEM_ADDRESS + i)))
      {
        answer = false;
        break;
      }
      data++;
    }

  } while (0);

  HAL_FLASH_Lock();
#ifdef HAL_ICACHE_MODULE_ENABLED
  HAL_ICACHE_Enable();
#endif
  return answer;
}
