/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 BURAM register and bit field definitions
 ******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EFR32ZG23_BURAM_H
#define EFR32ZG23_BURAM_H
#define BURAM_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_BURAM BURAM
 * @{
 * @brief EFR32ZG23 BURAM Register Declaration.
 *****************************************************************************/

/** BURAM RET Register Group Declaration. */
typedef struct {
  __IOM uint32_t REG;                                /**< Retention Register                                 */
} BURAM_RET_TypeDef;

/** BURAM Register Declaration. */
typedef struct {
  BURAM_RET_TypeDef RET[32U];                   /**< RetentionReg                                       */
  uint32_t          RESERVED0[992U];            /**< Reserved for future use                            */
  BURAM_RET_TypeDef RET_SET[32U];               /**< RetentionReg                                       */
  uint32_t          RESERVED1[992U];            /**< Reserved for future use                            */
  BURAM_RET_TypeDef RET_CLR[32U];               /**< RetentionReg                                       */
  uint32_t          RESERVED2[992U];            /**< Reserved for future use                            */
  BURAM_RET_TypeDef RET_TGL[32U];               /**< RetentionReg                                       */
} BURAM_TypeDef;
/** @} End of group EFR32ZG23_BURAM */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_BURAM
 * @{
 * @defgroup EFR32ZG23_BURAM_BitFields BURAM Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for BURAM RET_REG */
#define _BURAM_RET_REG_RESETVALUE        0x00000000UL                                   /**< Default value for BURAM_RET_REG             */
#define _BURAM_RET_REG_MASK              0xFFFFFFFFUL                                   /**< Mask for BURAM_RET_REG                      */
#define _BURAM_RET_REG_RETREG_SHIFT      0                                              /**< Shift value for BURAM_RETREG                */
#define _BURAM_RET_REG_RETREG_MASK       0xFFFFFFFFUL                                   /**< Bit mask for BURAM_RETREG                   */
#define _BURAM_RET_REG_RETREG_DEFAULT    0x00000000UL                                   /**< Mode DEFAULT for BURAM_RET_REG              */
#define BURAM_RET_REG_RETREG_DEFAULT     (_BURAM_RET_REG_RETREG_DEFAULT << 0)           /**< Shifted mode DEFAULT for BURAM_RET_REG      */

/** @} End of group EFR32ZG23_BURAM_BitFields */
/** @} End of group EFR32ZG23_BURAM */
/** @} End of group Parts */

#endif /* EFR32ZG23_BURAM_H */
#ifdef __cplusplus
}
#endif

