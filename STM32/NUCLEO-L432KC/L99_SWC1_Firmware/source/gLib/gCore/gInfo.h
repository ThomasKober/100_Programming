
/**
 * @file    gInfo.h
 * @brief   This library contains all header information, EEPROM data,
 *          and functions for reading and writing configurations.
 *
 * @author  Nima Askari , Askari@gtec.at , Nima.Askari@gmail.com
 * @date    21/03/2025 09:27:52
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

/* USER CODE BEGIN fw_version */
/**
 * @fw_version:  1.0.0-038
 * @fw_date:     21-03-2025
 */
/* USER CODE END fw_version */

#ifndef _GINFO_H_
#define _GINFO_H_

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

/* USER CODE BEGIN header */

/* USER CODE END header */

/**
 * ************************************************************************************************
 * library configuration
 * ************************************************************************************************
 */

/* USER CODE BEGIN configuration */

#define GINFO_FW_MAJOR                1
#define GINFO_FW_MINOR                0
#define GINFO_FW_PATCH                0
#define GINFO_FW_DEV_BUILD            38

/**
 * @def GINFO_CONFIG_CHECK_SKIPPED
 * @brief In boot loader, it would be 1
 * The configuration values does not check if enabled
 */
#define GINFO_CONFIG_CHECK_SKIPPED    0

/**
 * @def GINFO_CONFIG_CHECK_WORD
 * @brief After changing this value, during the boot process,
 * The configuration values are set to default
 */
#define GINFO_CONFIG_CHECK_WORD       0x0981CDA5

/* USER CODE END configuration */

#define GINFO_HEADER_SIZE             0x80                          ///< defined header size
#define GINFO_CONFIG_SIZE             7680                          ///< size of configuration structure
#define GINFO_HEADER_OFFSET           0x300                         ///< offset of starting header structure in flash
#define GINFO_MAGIC_WORD              0x85A15BCF                    ///< for checking a constant value at starting point of header
#define GINFO_CHECK_WORD              0x48831DD1                    ///< for checking eeprom memory for first initialization
#define GINFO_HEADER_MAJOR            1                             ///< defined header version
#define GINFO_HEADER_MINOR            0                             ///< defined header version

#define GINFO_HW_DEFAULT              "NOT_INITIALIZED"
#define GINFO_SN_DEFAULT              "NOT_INITIALIZED"

#define GINFO_CONFIG                  ginfo_storage_.config         ///< for easy accessing to config struct in program

/**
 * ************************************************************************************************
 * struct/enum definitions
 * ************************************************************************************************
 */

typedef struct
{
  uint32_t                            magic_word;
  uint32_t                            header_major;
  uint32_t                            header_minor;
  uint32_t                            length;                       ///< storing application length, fills by an external app
  uint8_t                             hash[32];                     ///< storing application hash, fills by an external app
  uint8_t                             fw_major;
  uint8_t                             fw_minor;
  uint8_t                             fw_patch;
  uint16_t                            fw_dev_build;
  uint8_t                             reserved[75];                 ///< reserved for future use

} __attribute__((packed)) GInfoHeaderStruct;                        ///< should be 128 (0x80) bytes

typedef struct
{
  uint32_t                            check_do_not_change_in_code;

  /* It should be Fix to 7676 bytes */
  /* USER CODE BEGIN configuration storage */

  uint8_t                             is_constant_trig;
  uint16_t                            trig_length;
  uint8_t                             max_ch;
  uint16_t                            voltage_mv;
  uint16_t                            current_ma;
  uint16_t                            led_on_duration;
  uint16_t                            led_off_duration;

	uint8_t															custom_config[7664];

  /* USER CODE END configuration storage */

} __attribute__((packed)) GInfoConfigStorageStruct;                 ///< should be 7680 bytes

typedef struct
{
  uint32_t                            check;
  char                                hw[32];
  char                                sn[32];
  uint8_t                             iv[16];
  int16_t                             temp_threshold;
  uint8_t                             reserved[426];                ///< reserved for future use
  GInfoConfigStorageStruct            config;

} __attribute__((packed)) GInfoStorageStruct;                       ///< should be 8192 bytes

typedef struct
{
  uint8_t                             *data_pointer;
  uint32_t                            size;

} GInfoMemHandle;                                                   ///< lowest layer of eeprom emulation

/**
 * ************************************************************************************************
 * public variable
 * ************************************************************************************************
 */

extern GInfoStorageStruct  ginfo_storage_;

/**
 * ************************************************************************************************
 * public library functions
 * ************************************************************************************************
 */

void          gInfoInit(void);
void          gInfoDeInit(void);
bool          gInfoLoad(void);
bool          gInfoSave(void);
bool          gInfoReadHw(char *str);
bool          gInfoWriteHw(const char *str);
bool          gInfoReadSn(char *str);
bool          gInfoWriteSn(const char *str);
bool          gInfoReadFw(char *str);
bool          gInfoReadIv(uint8_t *iv);
bool          gInfoWriteIv(const uint8_t *iv);

/* USER CODE BEGIN functions */

/* USER CODE END functions */

#ifdef __cplusplus
}
#endif
#endif
