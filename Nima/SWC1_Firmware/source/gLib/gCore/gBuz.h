
/**
 * @file    gBuz.h
 * @brief   This library contains all functions for controlling the buzzer,
 *          generating notes, and creating sequences
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

#ifndef _GBUZ_H_
#define _GBUZ_H_

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

/* USER CODE BEGIN configuration */

#define GBUZ_ENABLE										1
#define GBUZ_TIMER                    htim4
#define GBUZ_CHANNEL                  TIM_CHANNEL_4
#define GBUZ_TIMER_FREQ_MHZ           250

/* USER CODE END configuration */

/**
 * ************************************************************************************************
 * struct/enum definitions
 * ************************************************************************************************
 */

typedef enum
{
  GBUZ_NOTE_NONE                      = 0,

  GBUZ_NOTE_C0                        = 16,
  GBUZ_NOTE_CS0                       = 17,
  GBUZ_NOTE_D0                        = 18,
  GBUZ_NOTE_DS0                       = 19,
  GBUZ_NOTE_E0                        = 21,
  GBUZ_NOTE_F0                        = 22,
  GBUZ_NOTE_FS0                       = 23,
  GBUZ_NOTE_G0                        = 25,
  GBUZ_NOTE_GS0                       = 26,
  GBUZ_NOTE_A0                        = 28,
  GBUZ_NOTE_AS0                       = 29,
  GBUZ_NOTE_B0                        = 31,

  GBUZ_NOTE_C1                        = 33,
  GBUZ_NOTE_CS1                       = 35,
  GBUZ_NOTE_D1                        = 37,
  GBUZ_NOTE_DS1                       = 39,
  GBUZ_NOTE_E1                        = 41,
  GBUZ_NOTE_F1                        = 44,
  GBUZ_NOTE_FS1                       = 46,
  GBUZ_NOTE_G1                        = 49,
  GBUZ_NOTE_GS1                       = 52,
  GBUZ_NOTE_A1                        = 55,
  GBUZ_NOTE_AS1                       = 58,
  GBUZ_NOTE_B1                        = 62,

  GBUZ_NOTE_C2                        = 65,
  GBUZ_NOTE_CS2                       = 69,
  GBUZ_NOTE_D2                        = 73,
  GBUZ_NOTE_DS2                       = 78,
  GBUZ_NOTE_E2                        = 82,
  GBUZ_NOTE_F2                        = 87,
  GBUZ_NOTE_FS2                       = 93,
  GBUZ_NOTE_G2                        = 98,
  GBUZ_NOTE_GS2                       = 104,
  GBUZ_NOTE_A2                        = 110,
  GBUZ_NOTE_AS2                       = 117,
  GBUZ_NOTE_B2                        = 123,

  GBUZ_NOTE_C3                        = 131,
  GBUZ_NOTE_CS3                       = 139,
  GBUZ_NOTE_D3                        = 147,
  GBUZ_NOTE_DS3                       = 156,
  GBUZ_NOTE_E3                        = 165,
  GBUZ_NOTE_F3                        = 175,
  GBUZ_NOTE_FS3                       = 185,
  GBUZ_NOTE_G3                        = 196,
  GBUZ_NOTE_GS3                       = 208,
  GBUZ_NOTE_A3                        = 220,
  GBUZ_NOTE_AS3                       = 233,
  GBUZ_NOTE_B3                        = 247,

  GBUZ_NOTE_C4                        = 262,
  GBUZ_NOTE_CS4                       = 277,
  GBUZ_NOTE_D4                        = 294,
  GBUZ_NOTE_DS4                       = 311,
  GBUZ_NOTE_E4                        = 330,
  GBUZ_NOTE_F4                        = 349,
  GBUZ_NOTE_FS4                       = 370,
  GBUZ_NOTE_G4                        = 392,
  GBUZ_NOTE_GS4                       = 415,
  GBUZ_NOTE_A4                        = 440,
  GBUZ_NOTE_AS4                       = 466,
  GBUZ_NOTE_B4                        = 494,

  GBUZ_NOTE_C5                        = 523,
  GBUZ_NOTE_CS5                       = 554,
  GBUZ_NOTE_D5                        = 587,
  GBUZ_NOTE_DS5                       = 622,
  GBUZ_NOTE_E5                        = 659,
  GBUZ_NOTE_F5                        = 698,
  GBUZ_NOTE_FS5                       = 740,
  GBUZ_NOTE_G5                        = 784,
  GBUZ_NOTE_GS5                       = 831,
  GBUZ_NOTE_A5                        = 880,
  GBUZ_NOTE_AS5                       = 932,
  GBUZ_NOTE_B5                        = 988,

  GBUZ_NOTE_C6                        = 1047,
  GBUZ_NOTE_CS6                       = 1109,
  GBUZ_NOTE_D6                        = 1175,
  GBUZ_NOTE_DS6                       = 1245,
  GBUZ_NOTE_E6                        = 1319,
  GBUZ_NOTE_F6                        = 1397,
  GBUZ_NOTE_FS6                       = 1480,
  GBUZ_NOTE_G6                        = 1568,
  GBUZ_NOTE_GS6                       = 1661,
  GBUZ_NOTE_A6                        = 1760,
  GBUZ_NOTE_AS6                       = 1865,
  GBUZ_NOTE_B6                        = 1976,

  GBUZ_NOTE_C7                        = 2093,
  GBUZ_NOTE_CS7                       = 2217,
  GBUZ_NOTE_D7                        = 2349,
  GBUZ_NOTE_DS7                       = 2489,
  GBUZ_NOTE_E7                        = 2637,
  GBUZ_NOTE_F7                        = 2794,
  GBUZ_NOTE_FS7                       = 2960,
  GBUZ_NOTE_G7                        = 3136,
  GBUZ_NOTE_GS7                       = 3322,
  GBUZ_NOTE_A7                        = 3520,
  GBUZ_NOTE_AS7                       = 3729,
  GBUZ_NOTE_B7                        = 3951,

  GBUZ_NOTE_C8                        = 4186,
  GBUZ_NOTE_CS8                       = 4435,
  GBUZ_NOTE_D8                        = 4699,
  GBUZ_NOTE_DS8                       = 4978

} GBuzNote;

typedef struct
{
  GBuzNote                            note;
  uint32_t                            duration;

} GBuzNoteSequence;

typedef struct
{
  bool                                playing;
  uint32_t                            time;
  GBuzNoteSequence                    *sequence;

} GBuzHandle;

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

void          gBuzInit(void);
void          gBuzDeInit(void);
void          gBuzLoop(void);
bool          gBuzPlay(const GBuzNoteSequence *sequence);
void          gBuzStop(void);
bool          gBuzIsBusy(void);

#ifdef __cplusplus
}
#endif
#endif
