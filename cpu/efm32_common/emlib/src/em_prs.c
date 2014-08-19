/***************************************************************************//**
 * @file em_prs.c
 * @brief Peripheral Reflex System (PRS) Peripheral API
 * @version 3.20.6
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/


#include "em_prs.h"
#if defined(PRS_COUNT) && (PRS_COUNT > 0)

#include "em_assert.h"
#include "em_bitband.h"

/***************************************************************************//**
 * @addtogroup EM_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup PRS
 * @brief Peripheral Reflex System (PRS) Peripheral API
 * @{
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Set source and signal to be used for a channel.
 *
 * @param[in] ch
 *   Channel to define signal and source for.
 *
 * @param[in] source
 *   Source to select for channel. Use one of PRS_CH_CTRL_SOURCESEL_x defines.
 *
 * @param[in] signal
 *   Signal (for selected @p source) to use. Use one of PRS_CH_CTRL_SIGSEL_x
 *   defines.
 *
 * @param[in] edge
 *   Edge (for selected source/signal) to generate pulse for.
 ******************************************************************************/
void PRS_SourceSignalSet(unsigned int ch,
                         uint32_t source,
                         uint32_t signal,
                         PRS_Edge_TypeDef edge)
{
  EFM_ASSERT(ch < PRS_CHAN_COUNT);

  PRS->CH[ch].CTRL = (source & _PRS_CH_CTRL_SOURCESEL_MASK) |
                     (signal & _PRS_CH_CTRL_SIGSEL_MASK) |
                     (uint32_t)edge;
}

#if defined( PRS_CH_CTRL_ASYNC )
/***************************************************************************//**
 * @brief
 *   Set source and asynchronous signal to be used for a channel.
 *
 * @details
 *   Asynchronous reflexes are not clocked on HFPERCLK, and can be used even in
 *   EM2/EM3.
 *   There is a limitation to reflexes operating in asynchronous mode: they can
 *   only be used by a subset of the reflex consumers. Please refer to PRS
 *   chapter in the reference manual for the complete list of supported
 *   asynchronous signals and consumers.
 *
 * @note
 *   This function is supported on the following device families:
 *   @li Giant Gecko (EFM32GGxxxFxxx)
 *   @li Leopard Gecko (EFM32LGxxxFxxx)
 *   @li Tiny Gecko (EFM32TGxxxFxxx)
 *   @li Wonder Gecko (EFM32WGxxxFxxx)
 *   @li Zero Gecko (EFM32ZGxxxFxxx)
 *   In asynchronous mode, the edge detector only works in EM0, hence it shall
 *   not be used. The EDSEL parameter in PRS_CHx_CTRL register is set to 0 (OFF)
 *   by default.
 *
 * @param[in] ch
 *   Channel to define source and asynchronous signal for.
 *
 * @param[in] source
 *   Source to select for channel. Use one of PRS_CH_CTRL_SOURCESEL_x defines.
 *
 * @param[in] signal
 *   Asynchronous signal (for selected @p source) to use. Use one of the
 *   PRS_CH_CTRL_SIGSEL_x defines that support asynchronous operation.
 ******************************************************************************/
void PRS_SourceAsyncSignalSet(unsigned int ch,
                              uint32_t source,
                              uint32_t signal)
{
  EFM_ASSERT(ch < PRS_CHAN_COUNT);

  PRS->CH[ch].CTRL = PRS_CH_CTRL_ASYNC |
                     (source & _PRS_CH_CTRL_SOURCESEL_MASK) |
                     (signal & _PRS_CH_CTRL_SIGSEL_MASK) |
                     PRS_CH_CTRL_EDSEL_OFF;
}
#endif

/** @} (end addtogroup PRS) */
/** @} (end addtogroup EM_Library) */
#endif /* defined(PRS_COUNT) && (PRS_COUNT > 0) */
