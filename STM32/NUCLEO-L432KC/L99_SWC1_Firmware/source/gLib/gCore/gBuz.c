
/**
 * @file    gBuz.c
 * @brief   This library contains all functions for controlling the buzzer,
 *          generating notes, and creating sequences
 *
 * @author  Nima Askari , Askari@gtec.at , Nima.Askari@gmail.com
 * @date    21/03/2025 09:27:51
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

#include "gBuz.h"
#include "gCore.h"
#include <string.h>

#if GBUZ_ENABLE == 1

#include "tim.h"

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

void          gBuzPlayNote(uint32_t note);

/**
 * ************************************************************************************************
 * global variable
 * ************************************************************************************************
 */

GBuzHandle    gbuz_;

/**
 * ************************************************************************************************
 * public function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Initializes the buzzer system
 * @param  None
 * @return None
 * @note   This function configures the necessary hardware and settings for the buzzer to operate.
 *         It initializes the buzzer handle, configures the timer for PWM output, and starts
 *         the PWM signal. If the timer fails to start, the core system is halted.
 */
void gBuzInit(void)
{
  memset(&gbuz_, 0, sizeof(GBuzHandle));
  __HAL_TIM_SET_PRESCALER(&GBUZ_TIMER, GBUZ_TIMER_FREQ_MHZ - 1);
  __HAL_TIM_SET_COMPARE(&GBUZ_TIMER, GBUZ_CHANNEL, 0);
  __HAL_TIM_SET_AUTORELOAD(&GBUZ_TIMER, 10);
  __HAL_TIM_SET_COUNTER(&GBUZ_TIMER, 0);
  if (HAL_TIM_PWM_Start(&GBUZ_TIMER, GBUZ_CHANNEL) != HAL_OK)
  {
    gCoreHalt();
  }
}

/**
 * ************************************************************************************************
 * @brief  De-initializes the buzzer system
 * @param  None
 * @return None
 * @note   This function stops the PWM signal, resets the timer settings,
 *         and clears the buzzer handle to prepare the system for a fresh initialization.
 */
void gBuzDeInit(void)
{
  HAL_TIM_PWM_Stop(&GBUZ_TIMER, GBUZ_CHANNEL);
  __HAL_TIM_SET_COMPARE(&GBUZ_TIMER, GBUZ_CHANNEL, 0);
  __HAL_TIM_SET_AUTORELOAD(&GBUZ_TIMER, 0);
  __HAL_TIM_SET_COUNTER(&GBUZ_TIMER, 0);
  memset(&gbuz_, 0, sizeof(GBuzHandle));
}

/**
 * ************************************************************************************************
 * @brief  Main loop for the buzzer operations
 * @param  None
 * @return None
 * @note   This function contains the logic for the continuous operation of the buzzer,
 *         such as handling buzzer patterns, timing, or state changes. It should be called
 *         regularly within the main application loop to ensure proper functionality.
 */
void gBuzLoop(void)
{
  if (gbuz_.playing == true)
  {
    if (gbuz_.sequence != NULL)
    {
      if (HAL_GetTick() - gbuz_.time >= gbuz_.sequence->duration)
      {
        gbuz_.sequence++;
        gBuzPlayNote(gbuz_.sequence->note);
        if ((gbuz_.sequence->duration == 0) && (gbuz_.sequence->note == GBUZ_NOTE_NONE))
        {
          gBuzStop();
        }
      }
    }
    else
    {
      gBuzStop();
    }
  }
}

/**
 * ************************************************************************************************
 * @brief  Plays a sequence of notes on the buzzer
 * @param  sequence [in] Pointer to the note sequence to be played
 * @return true if the note sequence is successfully initiated, false if there is an error
 * @note   This function starts playing the specified sequence of notes on the buzzer.
 *         It is responsible for managing the timing and execution of each note in the sequence.
 */
bool gBuzPlay(const GBuzNoteSequence *sequence)
{
  bool answer = false;
  do
  {
    if (sequence == NULL)
    {
      gCoreHalt();
    }
    if (gbuz_.playing == true)
    {
      break;
    }
    gbuz_.sequence = (GBuzNoteSequence*)sequence;
    gbuz_.playing = true;
    gBuzPlayNote(gbuz_.sequence->note);
    answer = true;

  } while (0);

  return answer;
}

/**
 * ************************************************************************************************
 * @brief  Stops the currently playing buzzer sequence
 * @param  None
 * @return None
 * @note   This function halts any active buzzer sounds and resets the buzzer state,
 *         ensuring that the buzzer is silent after it has been stopped.
 */
void gBuzStop(void)
{
  gBuzPlayNote(GBUZ_NOTE_NONE);
  gbuz_.playing = false;
  gbuz_.sequence = NULL;
}

/**
 * ************************************************************************************************
 * @brief  Checks if the buzzer is currently busy playing a sound
 * @param  None
 * @return true if the buzzer is busy, false if it is idle
 * @note   This function queries the state of the buzzer to determine if it is currently
 *         engaged in playing a sequence or producing a sound.
 */
bool gBuzIsBusy(void)
{
  return gbuz_.playing;
}

/**
 * ************************************************************************************************
 * private function implementations
 * ************************************************************************************************
 */

/**
 * ************************************************************************************************
 * @brief  Plays a specific note on the buzzer
 * @param  note [in] The note to be played, represented as a frequency or identifier
 * @return None
 * @note   This function generates the sound corresponding to the specified note on the buzzer.
 *         It can be called to play individual notes, and the behavior may depend on the
 *         buzzer's current state or settings.
 */
void gBuzPlayNote(uint32_t note)
{
  uint32_t tmp = 0;
  gbuz_.time = HAL_GetTick();
  if (note > 0)
  {
    tmp = (1000000UL / note) - 1;
    __HAL_TIM_SET_AUTORELOAD(&GBUZ_TIMER, tmp);
    __HAL_TIM_SET_COMPARE(&GBUZ_TIMER, GBUZ_CHANNEL, tmp / 16);
  }
  else
  {
    __HAL_TIM_SET_AUTORELOAD(&GBUZ_TIMER, 100);
    __HAL_TIM_SET_COMPARE(&GBUZ_TIMER, GBUZ_CHANNEL, 0);
  }
}

#else

/**
 * ************************************************************************************************
 * public function implementations if the library disabled
 * ************************************************************************************************
 */

void gBuzInit(void)
{

}

void gBuzDeInit(void)
{

}

void gBuzLoop(void)
{

}

bool gBuzPlay(const GBuzNoteSequence *sequence)
{
  UNUSED(sequence);
  return true;
}

void gBuzStop(void)
{

}

bool gBuzIsBusy(void)
{
  return false;
}

#endif
