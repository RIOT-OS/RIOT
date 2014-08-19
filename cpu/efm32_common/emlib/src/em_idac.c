/***************************************************************************//**
 * @file em_idac.c
 * @brief Current Digital to Analog Converter (IDAC) peripheral API
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


#include "em_idac.h"
#if defined(IDAC_COUNT) && (IDAC_COUNT > 0)
#include "em_cmu.h"
#include "em_assert.h"
#include "em_bitband.h"

/***************************************************************************//**
 * @addtogroup EM_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup IDAC
 * @brief Current Digital to Analog Conversion (IDAC) Peripheral API
 * @{
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/***************************************************************************//**
 * @brief
 *   Initialize IDAC.
 *
 * @details
 *   Initializes IDAC according to the initialization structure parameter, and
 *   sets the default calibration value stored in the DEVINFO structure.
 *
 * @note
 *   This function will disable the IDAC prior to configuration.
 *
 * @param[in] idac
 *   Pointer to IDAC peripheral register block.
 *
 * @param[in] init
 *   Pointer to IDAC initialization structure.
 ******************************************************************************/
void IDAC_Init(IDAC_TypeDef *idac, const IDAC_Init_TypeDef *init)
{
  uint32_t tmp;

  EFM_ASSERT(IDAC_REF_VALID(idac));

  tmp = (uint32_t)(init->prsSel);

  tmp |= init->outMode;

  if (init->enable)
  {
    tmp |= IDAC_CTRL_EN;
  }
  if (init->prsEnable)
  {
    tmp |= IDAC_CTRL_OUTENPRS;
  }
  if (init->sinkEnable)
  {
    tmp |= IDAC_CTRL_CURSINK;
  }

  idac->CTRL = tmp;
}


/***************************************************************************//**
 * @brief
 *   Enable/disable IDAC.
 *
 * @param[in] idac
 *   Pointer to IDAC peripheral register block.
 *
 * @param[in] enable
 *   true to enable IDAC, false to disable.
 ******************************************************************************/
void IDAC_Enable(IDAC_TypeDef *idac, bool enable)
{
  volatile uint32_t *reg;

  EFM_ASSERT(IDAC_REF_VALID(idac));

  reg = &(idac->CTRL);

  BITBAND_Peripheral(reg, _IDAC_CTRL_EN_SHIFT, (unsigned int) enable);
}


/***************************************************************************//**
 * @brief
 *   Reset IDAC to same state as after a HW reset.
 *
 * @param[in] idac
 *   Pointer to IDAC peripheral register block.
 ******************************************************************************/
void IDAC_Reset(IDAC_TypeDef *idac)
{
  EFM_ASSERT(IDAC_REF_VALID(idac));

  idac->CTRL       = _IDAC_CTRL_RESETVALUE;
  idac->CURPROG    = _IDAC_CURPROG_RESETVALUE;
  idac->CAL        = _IDAC_CAL_RESETVALUE;
  idac->DUTYCONFIG = _IDAC_DUTYCONFIG_RESETVALUE;
}


/***************************************************************************//**
 * @brief
 *   Enable/disable Minimal Output Transition mode.
 *
 * @param[in] idac
 *   Pointer to IDAC peripheral register block.
 *
 * @param[in] enable
 *   true to enable Minimal Output Transition mode, false to disable.
 ******************************************************************************/
void IDAC_MinimalOutputTransitionMode(IDAC_TypeDef *idac, bool enable)
{
  volatile uint32_t *reg;

  EFM_ASSERT(IDAC_REF_VALID(idac));

  reg = &(idac->CTRL);

  BITBAND_Peripheral(reg, _IDAC_CTRL_MINOUTTRANS_SHIFT, (unsigned int) enable);
}


/***************************************************************************//**
 * @brief
 *   Set the current range of the IDAC output.
 *
 * @details
 *   This function sets the current range of the IDAC output. The function
 *   also updates the IDAC calibration register (IDAC_CAL) with the default
 *   calibration value (from DEVINFO, factory setting) corresponding to the
 *   specified range.
 *
 * @param[in] idac
 *   Pointer to IDAC peripheral register block.
 *
 * @param[in] range
 *   Current range value.
 ******************************************************************************/
void IDAC_RangeSet(IDAC_TypeDef *idac, const IDAC_Range_TypeDef range)
{
  uint32_t tmp;

  EFM_ASSERT(IDAC_REF_VALID(idac));
  EFM_ASSERT((range >> _IDAC_CURPROG_RANGESEL_SHIFT) <= (_IDAC_CURPROG_RANGESEL_MASK >> _IDAC_CURPROG_RANGESEL_SHIFT));

  /* Load proper calibration data depending on selected range */
  switch ((IDAC_Range_TypeDef) range)
  {
  case idacCurrentRange0:
    idac->CAL = (DEVINFO->IDAC0CAL0 & _DEVINFO_IDAC0CAL0_RANGE0_MASK) >> _DEVINFO_IDAC0CAL0_RANGE0_SHIFT;
    break;
  case idacCurrentRange1:
    idac->CAL = (DEVINFO->IDAC0CAL0 & _DEVINFO_IDAC0CAL0_RANGE1_MASK) >> _DEVINFO_IDAC0CAL0_RANGE1_SHIFT;
    break;
  case idacCurrentRange2:
    idac->CAL = (DEVINFO->IDAC0CAL0 & _DEVINFO_IDAC0CAL0_RANGE2_MASK) >> _DEVINFO_IDAC0CAL0_RANGE2_SHIFT;
    break;
  case idacCurrentRange3:
    idac->CAL = (DEVINFO->IDAC0CAL0 & _DEVINFO_IDAC0CAL0_RANGE3_MASK) >> _DEVINFO_IDAC0CAL0_RANGE3_SHIFT;
    break;
  }

  tmp  = idac->CURPROG & ~_IDAC_CURPROG_RANGESEL_MASK;
  tmp |= (uint32_t) range;

  idac->CURPROG = tmp;
}


/***************************************************************************//**
 * @brief
 *   Set the current step of the IDAC output.
 *
 * @param[in] idac
 *   Pointer to IDAC peripheral register block.
 *
 * @param[in] step
 *   Step value for IDAC output. Valid range is 0-31.
 ******************************************************************************/
void IDAC_StepSet(IDAC_TypeDef *idac, const uint32_t step)
{
  uint32_t tmp;

  EFM_ASSERT(IDAC_REF_VALID(idac));
  EFM_ASSERT(step <= (_IDAC_CURPROG_STEPSEL_MASK >> _IDAC_CURPROG_STEPSEL_SHIFT));

  tmp  = idac->CURPROG & ~_IDAC_CURPROG_STEPSEL_MASK;
  tmp |= step << _IDAC_CURPROG_STEPSEL_SHIFT;

  idac->CURPROG = tmp;
}


/***************************************************************************//**
 * @brief
 *   Enable/disable the IDAC OUT pin.
 *
 * @param[in] idac
 *   Pointer to IDAC peripheral register block.
 *
 * @param[in] enable
 *   true to enable the IDAC OUT pin, false to disable.
 ******************************************************************************/
void IDAC_OutEnable(IDAC_TypeDef *idac, bool enable)
{
  volatile uint32_t *reg;

  EFM_ASSERT(IDAC_REF_VALID(idac));

  reg = &(idac->CTRL);

  BITBAND_Peripheral(reg, _IDAC_CTRL_OUTEN_SHIFT, (unsigned int) enable);
}


/** @} (end addtogroup IDAC) */
/** @} (end addtogroup EM_Library) */

#endif /* defined(IDAC_COUNT) && (IDAC_COUNT > 0) */
