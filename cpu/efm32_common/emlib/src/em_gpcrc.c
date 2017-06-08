/***************************************************************************//**
 * @file
 * @brief General Purpose Cyclic Redundancy Check (GPCRC) API.
 * @version 4.3.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
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

#include "em_gpcrc.h"
#include "em_assert.h"

#if defined(GPCRC_PRESENT) && (GPCRC_COUNT > 0)

/***************************************************************************//**
 * @addtogroup emlib
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup GPCRC
 * @brief General Purpose Cyclic Redundancy Check (GPCRC) API.
 * @details
 * The GPCRC API functions provide full support for the GPCRC peripheral.
 *
 * The GPCRC module is a peripheral that implements a Cyclic Redundancy Check
 * (CRC) function. It supports a fixed 32-bit polynomial and a user
 * configurable 16-bit polynomial. The fixed 32-bit polynomial is the commonly
 * used IEEE 802.3 polynomial 0x04C11DB7.
 *
 * When using a 16-bit polynomial it's up to the user to choose a polynomial
 * that fits the application. Commonly used 16-bit polynomial are 0x1021
 * (CCITT-16), 0x3D65 (IEC16-MBus), and 0x8005 (ZigBee, 802.15.4, and USB).
 * See this link for other polynomials:
 * https://en.wikipedia.org/wiki/Cyclic_redundancy_check
 *
 * Before a CRC calculation can begin it is important to call the
 * @ref GPCRC_Start function. This function will reset the CRC calculation
 * by copying the configured initialization value over to the CRC data register.
 *
 * There are two ways of sending input data to the GPCRC. You can either write
 * the input data into the input data register using the input functions
 * @ref GPCRC_InputWord, @ref GPCRC_InputHalf and @ref GPCRC_InputByte, or the
 * user can setup the DMA to transfer data to the GPCRC input data registers.
 *
 * <b> Examples of GPCRC usage: </b>

  A CRC-32 Calculation:
  @verbatim
  uint32_t checksum;

  // Initialize GPCRC, 32-bit fixed polynomial is default
  GPCRC_Init_TypeDef init = GPCRC_INIT_DEFAULT;
  init.initValue = 0xFFFFFFFF; // Standard CRC-32 init value
  GPCRC_Init(GPCRC, &init);

  GPCRC_Start(GPCRC);
  GPCRC_InputU8(GPCRC, 0xC5);
  // CRC-32 specify that end result should be inverted
  checksum = ~GPCRC_DataRead(GPCRC);
  @endverbatim

  @n A CRC-16 Calculation
  @verbatim
  uint16_t checksum;

  GPCRC_Init_TypeDef init = GPCRC_INIT_DEFAULT;
  init.crcPoly = 0x8005;
  GPCRC_Init(GPCRC, &init);

  GPCRC_Start(GPCRC);
  GPCRC_InputU8(GPCRC, 0xAB);
  checksum = (uint16_t) GPCRC_DataRead(GPCRC);
  @endverbatim
 * @{
 ******************************************************************************/

/*******************************************************************************
 ***************************   GLOBAL FUNCTIONS   ******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initialize the General Purpose Cyclic Redundancy Check (GPCRC) module.
 *
 * @details
 *   Use this function to configure the operational parameters of the GPCRC
 *   such as the polynomial to use and how the input should be preprocessed
 *   before entering the CRC calculation.
 *
 * @note
 *   This function will not copy the init value to the data register in order
 *   to prepare for a new CRC calculation. This must be done by a call
 *   to @ref GPCRC_Start before each calculation, or by using the
 *   autoInit functionality.
 *
 * @param[in] gpcrc
 *   Pointer to GPCRC peripheral register block.
 *
 * @param[in] init
 *   Pointer to initialization structure used to configure the GPCRC.
 ******************************************************************************/
void GPCRC_Init(GPCRC_TypeDef * gpcrc, const GPCRC_Init_TypeDef * init)
{
  uint32_t polySelect;

  if (init->crcPoly == 0x04C11DB7)
  {
    polySelect = GPCRC_CTRL_POLYSEL_CRC32;
  }
  else
  {
    // If not using the fixed CRC-32 polynomial then we must be using 16-bit
    EFM_ASSERT((init->crcPoly & 0xFFFF0000) == 0);
    polySelect = GPCRC_CTRL_POLYSEL_16;
  }

  gpcrc->CTRL = (((uint32_t)init->autoInit << _GPCRC_CTRL_AUTOINIT_SHIFT)
                | ((uint32_t)init->reverseByteOrder << _GPCRC_CTRL_BYTEREVERSE_SHIFT)
                | ((uint32_t)init->reverseBits << _GPCRC_CTRL_BITREVERSE_SHIFT)
                | ((uint32_t)init->enableByteMode << _GPCRC_CTRL_BYTEMODE_SHIFT)
                | polySelect
                | ((uint32_t)init->enable << _GPCRC_CTRL_EN_SHIFT));

  if (polySelect == GPCRC_CTRL_POLYSEL_16)
  {
    // Set CRC polynomial value
    uint32_t revPoly = __RBIT(init->crcPoly) >> 16;
    gpcrc->POLY = revPoly & _GPCRC_POLY_POLY_MASK;
  }

  // Load CRC initialization value to GPCRC_INIT
  gpcrc->INIT = init->initValue;
}

/***************************************************************************//**
 * @brief
 *   Reset GPCRC registers to the hardware reset state.
 *
 * @note
 *   The data registers are not reset by this function.
 *
 * @param[in] gpcrc
 *   Pointer to GPCRC peripheral register block.
 ******************************************************************************/
void GPCRC_Reset(GPCRC_TypeDef * gpcrc)
{
  gpcrc->CTRL = _GPCRC_CTRL_RESETVALUE;
  gpcrc->POLY = _GPCRC_POLY_RESETVALUE;
  gpcrc->INIT = _GPCRC_INIT_RESETVALUE;
}

/** @} (end addtogroup GPCRC) */
/** @} (end addtogroup emlib) */

#endif /* defined(GPCRC_COUNT) && (GPCRC_COUNT > 0) */
