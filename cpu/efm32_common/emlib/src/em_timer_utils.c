/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "em_timer_utils.h"
#if defined(TIMER_COUNT) && (TIMER_COUNT > 0)

#include "em_timer.h"
#include "em_assert.h"

/***************************************************************************//**
 * @brief
 *   Convert a channel index number to the route bit field.
 *
 * @note
 *   The channel index number must exist.
 *
 * @param[in] ch
 *   The channel index number.
 *
 * @return
 *   Route bit field corresponding to channel index number.
 ******************************************************************************/
__INLINE uint32_t TIMER_Channel2Route(uint8_t ch)
{
  EFM_ASSERT(TIMER_CH_VALID(ch));

#ifdef _SILICON_LABS_32B_PLATFORM_1
  return (TIMER_ROUTE_CC0PEN << ch);
#else
  return (TIMER_ROUTEPEN_CC0PEN << ch);
#endif
}

/***************************************************************************//**
 * @brief
 *   Convert a timer prescaler into a divisor.
 *
 * @note
 *   The prescaler must be a valid prescaler (timerPrescale1 to
 *   timerPrescale1024).
 *
 * @param[in] prescaler
 *   The timer prescaler.
 *
 * @return
 *   Integer divisor in the range 1 to 1024, corresponding to the prescaler.
 ******************************************************************************/
__INLINE uint32_t TIMER_Prescaler2Div(TIMER_Prescale_TypeDef prescaler)
{
  EFM_ASSERT(prescaler <= timerPrescale1024);

  return (1 << ((uint8_t) prescaler));
}

/***************************************************************************//**
 * @brief
 *   Calculate the timer prescaler for a given frequency and timer frequency.
 *
 * @note
 *   The resulting prescaler will result a frequency that is equal or higher
 *   than the desired frequency. If no prescaler is found, it will exceed
 *   timerPrescale1024.
 *
 * @param[in] freq
 *   The desired frequency (in Hz).
 *
 * @param[in] freq_timer
 *   The reference frequency of the timer (in Hz).
 *
 * @return
 *   Timer prescaler (timerPrescale1 to timerPrescale1024).
 ******************************************************************************/
TIMER_Prescale_TypeDef TIMER_PrescalerCalc(uint32_t freq, uint32_t freq_timer)
{
  uint8_t prescaler = 0;

  /* keep dividing the timer frequency until it is lower than the desired
     frequency */
  while (freq_timer && freq_timer > freq) {
    freq_timer = freq_timer / 2;
    prescaler = prescaler + 1;
  }

  /* the return type starts at 0 for the first prescaler (division by 1), so if
     no prescaler was found, prescaler - 1 will wrap around, which means only
     one comparison is needed to check for validity */
  return (TIMER_Prescale_TypeDef) ((uint8_t) prescaler - 1);
}

#endif /* defined(TIMER_COUNT) && (TIMER_COUNT > 0) */
