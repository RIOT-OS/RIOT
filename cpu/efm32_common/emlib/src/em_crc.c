/***************************************************************************//**
 * @file
 * @brief Cyclic Redundancy Check (CRC) API.
 * @version 4.2.1
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
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

#include "em_crc.h"
#include "em_assert.h"

#if defined(CRC_COUNT) && (CRC_COUNT > 0)

/***************************************************************************//**
 * @addtogroup EM_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup CRC
 * @{
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/


/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/


/*******************************************************************************
 ***************************   GLOBAL FUNCTIONS   ******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initialize the Cyclic Redundancy Check (CRC) module of EFR.
 *
 * @details
 *   Use this function to configure the main operational parameters of the CRC
 *   such as CRC bytes, number of valid input bits, input/output bit- and bit
 *   order reversing.
 *   Refer to EFR Reference Manual Chapter 14 and the configuration structure
 *   CRC_Init_TypeDef for more details.
 *
 * @note
 *   Internal notes:
 *   - Initialize the CRC in the Init() function or let users use the separate
 *   command function?
 *
 * @param[in] init
 *   Pointer to initialization structure used to configure the CRC.
 ******************************************************************************/
void CRC_Init(CRC_Init_TypeDef const *init)
{
  /* Sanity check of bitsPerWord. */
  EFM_ASSERT(init->bitsPerWord < 16U);

  /* Set CRC control configuration parameters such as CRC width, byte and bit
   * bit order, the number of bits per word, inverting input/output, etc. */
  CRC->CTRL = (uint32_t)init->crcWidth
              | (uint32_t)init->byteReverse
              | (uint32_t)init->inputBitOrder
              | (uint32_t)init->bitReverse
              | ((uint32_t)init->bitsPerWord >> _CRC_CTRL_BITSPERWORD_SHIFT)
              | ((uint32_t)init->inputPadding >> _CRC_CTRL_PADCRCINPUT_SHIFT)
              | ((uint32_t)init->invInput >> _CRC_CTRL_INPUTINV_SHIFT)
              | ((uint32_t)init->invOutput >> _CRC_CTRL_OUTPUTINV_SHIFT);

  /* Set CRC polynomial value. */
  CRC->POLY = init->crcPoly;

  /* Load CRC initialization value to CRC_INIT. Please note, that the
   * initialization is not performed here! */
  CRC->INIT = init->initValue;
}


/***************************************************************************//**
 * @brief
 *   Reset CRC registers to the hardware reset state.
 ******************************************************************************/
void CRC_Reset(void)
{
  /* Reset CRC registers to their default value. */
  CRC->CTRL = _CRC_CTRL_RESETVALUE;
  CRC->POLY = _CRC_POLY_RESETVALUE;
  CRC->INIT = _CRC_INIT_RESETVALUE;
}


/** @} (end addtogroup CRC) */
/** @} (end addtogroup EM_Library) */

#endif /* defined(CRC_COUNT) && (CRC_COUNT > 0) */
