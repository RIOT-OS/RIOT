/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 LCDRF register and bit field definitions
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

#ifndef EFR32ZG23_LCDRF_H
#define EFR32ZG23_LCDRF_H
#define LCDRF_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_LCDRF LCDRF
 * @{
 * @brief EFR32ZG23 LCDRF Register Declaration.
 *****************************************************************************/

/** LCDRF Register Declaration. */
typedef struct {
  __IOM uint32_t RFIMLCDCTRL;                   /**< RF Interference Mitigation LCD Control             */
  uint32_t       RESERVED0[1023U];              /**< Reserved for future use                            */
  __IOM uint32_t RFIMLCDCTRL_SET;               /**< RF Interference Mitigation LCD Control             */
  uint32_t       RESERVED1[1023U];              /**< Reserved for future use                            */
  __IOM uint32_t RFIMLCDCTRL_CLR;               /**< RF Interference Mitigation LCD Control             */
  uint32_t       RESERVED2[1023U];              /**< Reserved for future use                            */
  __IOM uint32_t RFIMLCDCTRL_TGL;               /**< RF Interference Mitigation LCD Control             */
} LCDRF_TypeDef;
/** @} End of group EFR32ZG23_LCDRF */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_LCDRF
 * @{
 * @defgroup EFR32ZG23_LCDRF_BitFields LCDRF Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for LCDRF RFIMLCDCTRL */
#define _LCDRF_RFIMLCDCTRL_RESETVALUE                 0x00000000UL                                      /**< Default value for LCDRF_RFIMLCDCTRL         */
#define _LCDRF_RFIMLCDCTRL_MASK                       0x0000001FUL                                      /**< Mask for LCDRF_RFIMLCDCTRL                  */
#define LCDRF_RFIMLCDCTRL_LCDCPXOEN                   (0x1UL << 0)                                      /**< LCD Charge Pump XO Clock Enable             */
#define _LCDRF_RFIMLCDCTRL_LCDCPXOEN_SHIFT            0                                                 /**< Shift value for LCDRF_LCDCPXOEN             */
#define _LCDRF_RFIMLCDCTRL_LCDCPXOEN_MASK             0x1UL                                             /**< Bit mask for LCDRF_LCDCPXOEN                */
#define _LCDRF_RFIMLCDCTRL_LCDCPXOEN_DEFAULT          0x00000000UL                                      /**< Mode DEFAULT for LCDRF_RFIMLCDCTRL          */
#define LCDRF_RFIMLCDCTRL_LCDCPXOEN_DEFAULT           (_LCDRF_RFIMLCDCTRL_LCDCPXOEN_DEFAULT << 0)       /**< Shifted mode DEFAULT for LCDRF_RFIMLCDCTRL  */
#define LCDRF_RFIMLCDCTRL_LCDCPXOSEL                  (0x1UL << 1)                                      /**< LCD Charge Pump XO Select                   */
#define _LCDRF_RFIMLCDCTRL_LCDCPXOSEL_SHIFT           1                                                 /**< Shift value for LCDRF_LCDCPXOSEL            */
#define _LCDRF_RFIMLCDCTRL_LCDCPXOSEL_MASK            0x2UL                                             /**< Bit mask for LCDRF_LCDCPXOSEL               */
#define _LCDRF_RFIMLCDCTRL_LCDCPXOSEL_DEFAULT         0x00000000UL                                      /**< Mode DEFAULT for LCDRF_RFIMLCDCTRL          */
#define _LCDRF_RFIMLCDCTRL_LCDCPXOSEL_INTRCO          0x00000000UL                                      /**< Mode INTRCO for LCDRF_RFIMLCDCTRL           */
#define _LCDRF_RFIMLCDCTRL_LCDCPXOSEL_HFXODIV         0x00000001UL                                      /**< Mode HFXODIV for LCDRF_RFIMLCDCTRL          */
#define LCDRF_RFIMLCDCTRL_LCDCPXOSEL_DEFAULT          (_LCDRF_RFIMLCDCTRL_LCDCPXOSEL_DEFAULT << 1)      /**< Shifted mode DEFAULT for LCDRF_RFIMLCDCTRL  */
#define LCDRF_RFIMLCDCTRL_LCDCPXOSEL_INTRCO           (_LCDRF_RFIMLCDCTRL_LCDCPXOSEL_INTRCO << 1)       /**< Shifted mode INTRCO for LCDRF_RFIMLCDCTRL   */
#define LCDRF_RFIMLCDCTRL_LCDCPXOSEL_HFXODIV          (_LCDRF_RFIMLCDCTRL_LCDCPXOSEL_HFXODIV << 1)      /**< Shifted mode HFXODIV for LCDRF_RFIMLCDCTRL  */
#define LCDRF_RFIMLCDCTRL_LCDCPXORETIMEEN             (0x1UL << 2)                                      /**< LCD Charge Pump XO Retime Enable            */
#define _LCDRF_RFIMLCDCTRL_LCDCPXORETIMEEN_SHIFT      2                                                 /**< Shift value for LCDRF_LCDCPXORETIMEEN       */
#define _LCDRF_RFIMLCDCTRL_LCDCPXORETIMEEN_MASK       0x4UL                                             /**< Bit mask for LCDRF_LCDCPXORETIMEEN          */
#define _LCDRF_RFIMLCDCTRL_LCDCPXORETIMEEN_DEFAULT    0x00000000UL                                      /**< Mode DEFAULT for LCDRF_RFIMLCDCTRL          */
#define LCDRF_RFIMLCDCTRL_LCDCPXORETIMEEN_DEFAULT     (_LCDRF_RFIMLCDCTRL_LCDCPXORETIMEEN_DEFAULT << 2) /**< Shifted mode DEFAULT for LCDRF_RFIMLCDCTRL  */
#define LCDRF_RFIMLCDCTRL_LCDLOWNOISE                 (0x1UL << 3)                                      /**< LCD Low Noise                               */
#define _LCDRF_RFIMLCDCTRL_LCDLOWNOISE_SHIFT          3                                                 /**< Shift value for LCDRF_LCDLOWNOISE           */
#define _LCDRF_RFIMLCDCTRL_LCDLOWNOISE_MASK           0x8UL                                             /**< Bit mask for LCDRF_LCDLOWNOISE              */
#define _LCDRF_RFIMLCDCTRL_LCDLOWNOISE_DEFAULT        0x00000000UL                                      /**< Mode DEFAULT for LCDRF_RFIMLCDCTRL          */
#define _LCDRF_RFIMLCDCTRL_LCDLOWNOISE_NORMAL         0x00000000UL                                      /**< Mode NORMAL for LCDRF_RFIMLCDCTRL           */
#define _LCDRF_RFIMLCDCTRL_LCDLOWNOISE_SLOW           0x00000001UL                                      /**< Mode SLOW for LCDRF_RFIMLCDCTRL             */
#define LCDRF_RFIMLCDCTRL_LCDLOWNOISE_DEFAULT         (_LCDRF_RFIMLCDCTRL_LCDLOWNOISE_DEFAULT << 3)     /**< Shifted mode DEFAULT for LCDRF_RFIMLCDCTRL  */
#define LCDRF_RFIMLCDCTRL_LCDLOWNOISE_NORMAL          (_LCDRF_RFIMLCDCTRL_LCDLOWNOISE_NORMAL << 3)      /**< Shifted mode NORMAL for LCDRF_RFIMLCDCTRL   */
#define LCDRF_RFIMLCDCTRL_LCDLOWNOISE_SLOW            (_LCDRF_RFIMLCDCTRL_LCDLOWNOISE_SLOW << 3)        /**< Shifted mode SLOW for LCDRF_RFIMLCDCTRL     */
#define LCDRF_RFIMLCDCTRL_LCDCMPDOUT                  (0x1UL << 4)                                      /**< LCD Comparator Dout                         */
#define _LCDRF_RFIMLCDCTRL_LCDCMPDOUT_SHIFT           4                                                 /**< Shift value for LCDRF_LCDCMPDOUT            */
#define _LCDRF_RFIMLCDCTRL_LCDCMPDOUT_MASK            0x10UL                                            /**< Bit mask for LCDRF_LCDCMPDOUT               */
#define _LCDRF_RFIMLCDCTRL_LCDCMPDOUT_DEFAULT         0x00000000UL                                      /**< Mode DEFAULT for LCDRF_RFIMLCDCTRL          */
#define LCDRF_RFIMLCDCTRL_LCDCMPDOUT_DEFAULT          (_LCDRF_RFIMLCDCTRL_LCDCMPDOUT_DEFAULT << 4)      /**< Shifted mode DEFAULT for LCDRF_RFIMLCDCTRL  */

/** @} End of group EFR32ZG23_LCDRF_BitFields */
/** @} End of group EFR32ZG23_LCDRF */
/** @} End of group Parts */

#endif /* EFR32ZG23_LCDRF_H */
#ifdef __cplusplus
}
#endif

