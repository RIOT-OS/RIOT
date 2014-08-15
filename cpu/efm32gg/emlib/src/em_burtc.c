/***************************************************************************//**
 * @file em_burtc.c
 * @brief Backup Real Time Counter (BURTC) Peripheral API
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


#include "em_device.h"
#if defined(BURTC_PRESENT)
#include "em_burtc.h"

#include "em_assert.h"
#include "em_bitband.h"

/***************************************************************************//**
 * @addtogroup EM_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup BURTC
 * @brief Backup Real Time Counter (BURTC) Peripheral API
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief Convert dividend to prescaler logarithmic value. Only works for even
 *        numbers equal to 2^n
 * @param[in] div Unscaled dividend,
 * @return Base 2 logarithm of input, as used by fixed prescalers
 ******************************************************************************/
__STATIC_INLINE uint32_t BURTC_DivToLog2(uint32_t div)
{
  uint32_t log2;

  /* Prescaler accepts an argument of 128 or less, valid values being 2^n */
  EFM_ASSERT((div > 0) && (div <= 32768));

  /* Count leading zeroes and "reverse" result, Cortex-M3 intrinsic */
  log2 = (31 - __CLZ(div));

  return log2;
}


/***************************************************************************//**
 * @brief
 *   Wait for ongoing sync of register(s) to low frequency domain to complete.
 *
 * @param[in] mask
 *   Bitmask corresponding to SYNCBUSY register defined bits, indicating
 *   registers that must complete any ongoing synchronization.
 ******************************************************************************/
__STATIC_INLINE void BURTC_Sync(uint32_t mask)
{
  /* Avoid deadlock if modifying the same register twice when freeze mode is */
  /* activated. */
  if (BURTC->FREEZE & BURTC_FREEZE_REGFREEZE)
    return;

  /* Wait for any pending previous write operation to have been completed */
  /* in low frequency domain. This is only required for the Gecko Family */
  while (BURTC->SYNCBUSY & mask)
    ;
}


/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief Initialize BURTC
 *
 * @details
 *    Configures the BURTC peripheral.
 *
 * @note
 *   Before initialization, BURTC module must first be enabled by clearing the
 *   reset bit in the RMU, i.e.
 * @verbatim
 *   RMU_ResetControl(rmuResetBU, false);
 * @endverbatim
 *   Compare channel 0 must be configured outside this function, before
 *   initialization if enable is set to true. The counter will always be reset.
 *
 * @param[in] burtcInit
 *   Pointer to BURTC initialization structure
 ******************************************************************************/
void BURTC_Init(const BURTC_Init_TypeDef *burtcInit)
{
  uint32_t ctrl;
  uint32_t presc;

  /* Check initializer structure integrity */
  EFM_ASSERT(burtcInit != (BURTC_Init_TypeDef *) 0);
  /* Clock divider must be between 1 and 128, really on the form 2^n */
  EFM_ASSERT((burtcInit->clkDiv >= 1) && (burtcInit->clkDiv <= 128));
  /* Ignored compare bits during low power operation must be less than 7 */
  /* Note! Giant Gecko revision C errata, do NOT use LPCOMP=7 */
  EFM_ASSERT(burtcInit->lowPowerComp <= 6);
  /* You cannot enable the BURTC if mode is set to disabled */
  EFM_ASSERT((burtcInit->enable == false) ||
             ((burtcInit->enable == true) && (burtcInit->mode != burtcModeDisable)));
  /* Low power mode is only available with LFRCO or LFXO as clock source */
  EFM_ASSERT((burtcInit->clkSel != burtcClkSelULFRCO) ||
             ((burtcInit->clkSel == burtcClkSelULFRCO) && (burtcInit->lowPowerMode == burtcLPDisable)));

  /* Calculate prescaler value from clock divider input */
  /* Note! If clock select (clkSel) is ULFRCO, a clock divisor (clkDiv) of
     value 1 will select a 2kHz ULFRCO clock, while any other value will
     select a 1kHz ULFRCO clock source. */
  presc = BURTC_DivToLog2(burtcInit->clkDiv);

  /* Make sure all registers are updated simultaneously */
  if (burtcInit->enable)
  {
    BURTC_FreezeEnable(true);
  }

  /* Modification of LPMODE register requires sync with potential ongoing
   * register updates in LF domain. */
  BURTC_Sync(BURTC_SYNCBUSY_LPMODE);

  /* Configure low power mode */
  BURTC->LPMODE = (uint32_t) (burtcInit->lowPowerMode);

  /* New configuration */
  ctrl = ((BURTC_CTRL_RSTEN) |
          (burtcInit->mode) |
          (burtcInit->debugRun << _BURTC_CTRL_DEBUGRUN_SHIFT) |
          (burtcInit->compare0Top << _BURTC_CTRL_COMP0TOP_SHIFT) |
          (burtcInit->lowPowerComp << _BURTC_CTRL_LPCOMP_SHIFT) |
          (presc << _BURTC_CTRL_PRESC_SHIFT) |
          (burtcInit->clkSel) |
          (burtcInit->timeStamp << _BURTC_CTRL_BUMODETSEN_SHIFT));

  /* Clear interrupts */
  BURTC->IFC = 0xFFFFFFFF;

  /* Set new configuration */
  BURTC->CTRL = ctrl;

  /* Enable BURTC and counter */
  if (burtcInit->enable)
  {
    /* To enable BURTC counter, we need to disable reset */
    BURTC_Enable(true);

    /* Clear freeze */
    BURTC_FreezeEnable(false);
  }
}


/***************************************************************************//**
 * @brief Set BURTC compare channel
 *
 * @param[in] comp Compare channel index, must be 0 for Giant / Leopard Gecko
 *
 * @param[in] value New compare value
 ******************************************************************************/
void BURTC_CompareSet(unsigned int comp, uint32_t value)
{
  (void) comp;  /* Unused parameter when EFM_ASSERT is undefined. */

  EFM_ASSERT(comp == 0);

  /* Modification of COMP0 register requires sync with potential ongoing
   * register updates in LF domain. */
  BURTC_Sync(BURTC_SYNCBUSY_COMP0);

  /* Configure compare channel 0 */
  BURTC->COMP0 = value;
}


/***************************************************************************//**
 * @brief Get BURTC compare value
 *
 * @param[in] comp Compare channel index value, must be 0 for Giant/Leopard.
 *
 * @return Currently configured value for this compare channel
 ******************************************************************************/
uint32_t BURTC_CompareGet(unsigned int comp)
{
  (void) comp;  /* Unused parameter when EFM_ASSERT is undefined. */

  EFM_ASSERT(comp == 0);

  return BURTC->COMP0;
}


/***************************************************************************//**
 * @brief Reset counter
 ******************************************************************************/
void BURTC_CounterReset(void)
{
  /* Set and clear reset bit */
  BITBAND_Peripheral(&BURTC->CTRL, _BURTC_CTRL_RSTEN_SHIFT, 1);
  BITBAND_Peripheral(&BURTC->CTRL, _BURTC_CTRL_RSTEN_SHIFT, 0);
}


/***************************************************************************//**
 * @brief
 *   Restore BURTC to reset state
 * @note
 *   Before accessing the BURTC, BURSTEN in RMU->CTRL must be cleared.
 *   LOCK will not be reset to default value, as this will disable access
 *   to core BURTC registers.
 ******************************************************************************/
void BURTC_Reset(void)
{
  /* Verify RMU BURSTEN is disabled */
  EFM_ASSERT((RMU->CTRL & RMU_CTRL_BURSTEN) == 0);

  /* Restore all essential BURTC registers to default config */
  BURTC->IEN      = _BURTC_IEN_RESETVALUE;
  /* Modification of LPMODE register requires sync with potential ongoing
   * register updates in LF domain. */
  BURTC_Sync(BURTC_SYNCBUSY_LPMODE);
  BURTC->LPMODE   = _BURTC_LPMODE_RESETVALUE;
  BURTC->LFXOFDET = _BURTC_LFXOFDET_RESETVALUE;
  /* Modification of COMP0 register requires sync with potential ongoing
   * register updates in LF domain. */
  BURTC_Sync(BURTC_SYNCBUSY_COMP0);
  BURTC->COMP0    = _BURTC_COMP0_RESETVALUE;
  BURTC->FREEZE   = _BURTC_FREEZE_RESETVALUE;
  /* We must wait for SYNCBUSY before resetting the CTRL register. */
  BURTC_Sync(BURTC_SYNCBUSY_LPMODE | BURTC_SYNCBUSY_COMP0);
  BURTC->CTRL     = _BURTC_CTRL_RESETVALUE;
}


/***************************************************************************//**
 * @brief
 *   Get clock frequency of the BURTC.
 *
 * @return
 *   The current frequency in Hz.
 ******************************************************************************/
uint32_t BURTC_ClockFreqGet(void)
{
  uint32_t clkSel;
  uint32_t clkDiv;
  uint32_t frequency;

  clkSel = BURTC->CTRL & _BURTC_CTRL_CLKSEL_MASK;
  clkDiv = (BURTC->CTRL & _BURTC_CTRL_PRESC_MASK) >> _BURTC_CTRL_PRESC_SHIFT;

  switch (clkSel)
  {
  /** Ultra low frequency (1 kHz) clock */
  case BURTC_CTRL_CLKSEL_ULFRCO:
    if (_BURTC_CTRL_PRESC_DIV1 == clkDiv)
    {
      frequency = 2000;     /* 2KHz when clock divisor is 1. */
    }
    else
    {
      frequency = SystemULFRCOClockGet();  /* 1KHz when divisor is different
                                              from 1. */
    }
    break;

  /** Low frequency RC oscillator */
  case BURTC_CTRL_CLKSEL_LFRCO:
    frequency = SystemLFRCOClockGet() / (1 << clkDiv); /* freq=32768/2^clkDiv */
    break;

  /** Low frequency crystal osciallator */
  case BURTC_CTRL_CLKSEL_LFXO:
    frequency = SystemLFXOClockGet() / (1 << clkDiv); /* freq=32768/2^clkDiv */
    break;

  default:
    /* No clock selected for BURTC. */
    frequency = 0;
  }

  return frequency;
}


/** @} (end addtogroup BURTC) */
/** @} (end addtogroup EM_Library) */

#endif /* BURTC_PRESENT */
