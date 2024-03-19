/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 PFMXPPRF register and bit field definitions
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

#ifndef EFR32ZG23_PFMXPPRF_H
#define EFR32ZG23_PFMXPPRF_H
#define PFMXPPRF_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_PFMXPPRF PFMXPPRF
 * @{
 * @brief EFR32ZG23 PFMXPPRF Register Declaration.
 *****************************************************************************/

/** PFMXPPRF Register Declaration. */
typedef struct {
  __IOM uint32_t RFIMDCDCCTRL0;                 /**< New Register                                       */
  __IOM uint32_t RFIMDCDCCTRL1;                 /**< New Register                                       */
  __IOM uint32_t RFIMDCDCCTRL2;                 /**< New Register                                       */
  __IM uint32_t  RFIMDCDCSTATUS;                /**< New Register                                       */
  __IOM uint32_t RPURATD0;                      /**< Root Access Type Descriptor Register               */
  uint32_t       RESERVED0[1019U];              /**< Reserved for future use                            */
  __IOM uint32_t RFIMDCDCCTRL0_SET;             /**< New Register                                       */
  __IOM uint32_t RFIMDCDCCTRL1_SET;             /**< New Register                                       */
  __IOM uint32_t RFIMDCDCCTRL2_SET;             /**< New Register                                       */
  __IM uint32_t  RFIMDCDCSTATUS_SET;            /**< New Register                                       */
  __IOM uint32_t RPURATD0_SET;                  /**< Root Access Type Descriptor Register               */
  uint32_t       RESERVED1[1019U];              /**< Reserved for future use                            */
  __IOM uint32_t RFIMDCDCCTRL0_CLR;             /**< New Register                                       */
  __IOM uint32_t RFIMDCDCCTRL1_CLR;             /**< New Register                                       */
  __IOM uint32_t RFIMDCDCCTRL2_CLR;             /**< New Register                                       */
  __IM uint32_t  RFIMDCDCSTATUS_CLR;            /**< New Register                                       */
  __IOM uint32_t RPURATD0_CLR;                  /**< Root Access Type Descriptor Register               */
  uint32_t       RESERVED2[1019U];              /**< Reserved for future use                            */
  __IOM uint32_t RFIMDCDCCTRL0_TGL;             /**< New Register                                       */
  __IOM uint32_t RFIMDCDCCTRL1_TGL;             /**< New Register                                       */
  __IOM uint32_t RFIMDCDCCTRL2_TGL;             /**< New Register                                       */
  __IM uint32_t  RFIMDCDCSTATUS_TGL;            /**< New Register                                       */
  __IOM uint32_t RPURATD0_TGL;                  /**< Root Access Type Descriptor Register               */
} PFMXPPRF_TypeDef;
/** @} End of group EFR32ZG23_PFMXPPRF */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_PFMXPPRF
 * @{
 * @defgroup EFR32ZG23_PFMXPPRF_BitFields PFMXPPRF Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for PFMXPPRF RFIMDCDCCTRL0 */
#define _PFMXPPRF_RFIMDCDCCTRL0_RESETVALUE                 0x00000000UL                                    /**< Default value for PFMXPPRF_RFIMDCDCCTRL0    */
#define _PFMXPPRF_RFIMDCDCCTRL0_MASK                       0x80000003UL                                    /**< Mask for PFMXPPRF_RFIMDCDCCTRL0             */
#define PFMXPPRF_RFIMDCDCCTRL0_TXMAXREQ                    (0x1UL << 0)                                    /**< TX Max Req                                  */
#define _PFMXPPRF_RFIMDCDCCTRL0_TXMAXREQ_SHIFT             0                                               /**< Shift value for PFMXPPRF_TXMAXREQ           */
#define _PFMXPPRF_RFIMDCDCCTRL0_TXMAXREQ_MASK              0x1UL                                           /**< Bit mask for PFMXPPRF_TXMAXREQ              */
#define _PFMXPPRF_RFIMDCDCCTRL0_TXMAXREQ_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL0     */
#define PFMXPPRF_RFIMDCDCCTRL0_TXMAXREQ_DEFAULT            (_PFMXPPRF_RFIMDCDCCTRL0_TXMAXREQ_DEFAULT << 0) /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL0*/
#define PFMXPPRF_RFIMDCDCCTRL0_RXPPREQ                     (0x1UL << 1)                                    /**< RX PP Req                                   */
#define _PFMXPPRF_RFIMDCDCCTRL0_RXPPREQ_SHIFT              1                                               /**< Shift value for PFMXPPRF_RXPPREQ            */
#define _PFMXPPRF_RFIMDCDCCTRL0_RXPPREQ_MASK               0x2UL                                           /**< Bit mask for PFMXPPRF_RXPPREQ               */
#define _PFMXPPRF_RFIMDCDCCTRL0_RXPPREQ_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL0     */
#define PFMXPPRF_RFIMDCDCCTRL0_RXPPREQ_DEFAULT             (_PFMXPPRF_RFIMDCDCCTRL0_RXPPREQ_DEFAULT << 1)  /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL0*/

/* Bit fields for PFMXPPRF RFIMDCDCCTRL1 */
#define _PFMXPPRF_RFIMDCDCCTRL1_RESETVALUE                 0x00000014UL                                           /**< Default value for PFMXPPRF_RFIMDCDCCTRL1    */
#define _PFMXPPRF_RFIMDCDCCTRL1_MASK                       0x0000003FUL                                           /**< Mask for PFMXPPRF_RFIMDCDCCTRL1             */
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVEN                   (0x1UL << 0)                                           /**< DCDC DIV Enable                             */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVEN_SHIFT            0                                                      /**< Shift value for PFMXPPRF_DCDCDIVEN          */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVEN_MASK             0x1UL                                                  /**< Bit mask for PFMXPPRF_DCDCDIVEN             */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVEN_DEFAULT          0x00000000UL                                           /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL1     */
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVEN_DEFAULT           (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVEN_DEFAULT << 0)       /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVINVEN                (0x1UL << 1)                                           /**< DCDC DIV Inverter Enable                    */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVINVEN_SHIFT         1                                                      /**< Shift value for PFMXPPRF_DCDCDIVINVEN       */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVINVEN_MASK          0x2UL                                                  /**< Bit mask for PFMXPPRF_DCDCDIVINVEN          */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVINVEN_DEFAULT       0x00000000UL                                           /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL1     */
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVINVEN_DEFAULT        (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVINVEN_DEFAULT << 1)    /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL1*/
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_SHIFT         2                                                      /**< Shift value for PFMXPPRF_DCDCDIVRATIO       */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_MASK          0x3CUL                                                 /**< Bit mask for PFMXPPRF_DCDCDIVRATIO          */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DEFAULT       0x00000005UL                                           /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL1     */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO8     0x00000000UL                                           /**< Mode DIVRATIO8 for PFMXPPRF_RFIMDCDCCTRL1   */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO9     0x00000001UL                                           /**< Mode DIVRATIO9 for PFMXPPRF_RFIMDCDCCTRL1   */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO10    0x00000002UL                                           /**< Mode DIVRATIO10 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO11    0x00000003UL                                           /**< Mode DIVRATIO11 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO12    0x00000004UL                                           /**< Mode DIVRATIO12 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO13    0x00000005UL                                           /**< Mode DIVRATIO13 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO14    0x00000006UL                                           /**< Mode DIVRATIO14 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO15    0x00000007UL                                           /**< Mode DIVRATIO15 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO16    0x00000008UL                                           /**< Mode DIVRATIO16 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO17    0x00000009UL                                           /**< Mode DIVRATIO17 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO18    0x0000000AUL                                           /**< Mode DIVRATIO18 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO19    0x0000000BUL                                           /**< Mode DIVRATIO19 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO20    0x0000000CUL                                           /**< Mode DIVRATIO20 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO21    0x0000000DUL                                           /**< Mode DIVRATIO21 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO22    0x0000000EUL                                           /**< Mode DIVRATIO22 for PFMXPPRF_RFIMDCDCCTRL1  */
#define _PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO23    0x0000000FUL                                           /**< Mode DIVRATIO23 for PFMXPPRF_RFIMDCDCCTRL1  */
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DEFAULT        (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DEFAULT << 2)    /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO8      (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO8 << 2)  /**< Shifted mode DIVRATIO8 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO9      (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO9 << 2)  /**< Shifted mode DIVRATIO9 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO10     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO10 << 2) /**< Shifted mode DIVRATIO10 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO11     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO11 << 2) /**< Shifted mode DIVRATIO11 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO12     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO12 << 2) /**< Shifted mode DIVRATIO12 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO13     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO13 << 2) /**< Shifted mode DIVRATIO13 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO14     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO14 << 2) /**< Shifted mode DIVRATIO14 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO15     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO15 << 2) /**< Shifted mode DIVRATIO15 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO16     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO16 << 2) /**< Shifted mode DIVRATIO16 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO17     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO17 << 2) /**< Shifted mode DIVRATIO17 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO18     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO18 << 2) /**< Shifted mode DIVRATIO18 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO19     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO19 << 2) /**< Shifted mode DIVRATIO19 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO20     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO20 << 2) /**< Shifted mode DIVRATIO20 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO21     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO21 << 2) /**< Shifted mode DIVRATIO21 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO22     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO22 << 2) /**< Shifted mode DIVRATIO22 for PFMXPPRF_RFIMDCDCCTRL1*/
#define PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO23     (_PFMXPPRF_RFIMDCDCCTRL1_DCDCDIVRATIO_DIVRATIO23 << 2) /**< Shifted mode DIVRATIO23 for PFMXPPRF_RFIMDCDCCTRL1*/

/* Bit fields for PFMXPPRF RFIMDCDCCTRL2 */
#define _PFMXPPRF_RFIMDCDCCTRL2_RESETVALUE                 0x0AD0B4A0UL                                       /**< Default value for PFMXPPRF_RFIMDCDCCTRL2    */
#define _PFMXPPRF_RFIMDCDCCTRL2_MASK                       0x9FFFFFFFUL                                       /**< Mask for PFMXPPRF_RFIMDCDCCTRL2             */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPTMAX_SHIFT               0                                                  /**< Shift value for PFMXPPRF_PPTMAX             */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPTMAX_MASK                0x1FFUL                                            /**< Bit mask for PFMXPPRF_PPTMAX                */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPTMAX_DEFAULT             0x000000A0UL                                       /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL2     */
#define PFMXPPRF_RFIMDCDCCTRL2_PPTMAX_DEFAULT              (_PFMXPPRF_RFIMDCDCCTRL2_PPTMAX_DEFAULT << 0)      /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL2*/
#define _PFMXPPRF_RFIMDCDCCTRL2_PPTMIN_SHIFT               9                                                  /**< Shift value for PFMXPPRF_PPTMIN             */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPTMIN_MASK                0x3FE00UL                                          /**< Bit mask for PFMXPPRF_PPTMIN                */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPTMIN_DEFAULT             0x0000005AUL                                       /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL2     */
#define PFMXPPRF_RFIMDCDCCTRL2_PPTMIN_DEFAULT              (_PFMXPPRF_RFIMDCDCCTRL2_PPTMIN_DEFAULT << 9)      /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL2*/
#define _PFMXPPRF_RFIMDCDCCTRL2_PPND_SHIFT                 18                                                 /**< Shift value for PFMXPPRF_PPND               */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPND_MASK                  0x7FC0000UL                                        /**< Bit mask for PFMXPPRF_PPND                  */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPND_DEFAULT               0x000000B4UL                                       /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL2     */
#define PFMXPPRF_RFIMDCDCCTRL2_PPND_DEFAULT                (_PFMXPPRF_RFIMDCDCCTRL2_PPND_DEFAULT << 18)       /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL2*/
#define PFMXPPRF_RFIMDCDCCTRL2_PPCALEN                     (0x1UL << 27)                                      /**< Pulse Pairing Calibration Loop Enable       */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPCALEN_SHIFT              27                                                 /**< Shift value for PFMXPPRF_PPCALEN            */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPCALEN_MASK               0x8000000UL                                        /**< Bit mask for PFMXPPRF_PPCALEN               */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPCALEN_DEFAULT            0x00000001UL                                       /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL2     */
#define PFMXPPRF_RFIMDCDCCTRL2_PPCALEN_DEFAULT             (_PFMXPPRF_RFIMDCDCCTRL2_PPCALEN_DEFAULT << 27)    /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL2*/
#define PFMXPPRF_RFIMDCDCCTRL2_PPSYNCONLY                  (0x1UL << 28)                                      /**< Pulse Pairing Sync Only                     */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPSYNCONLY_SHIFT           28                                                 /**< Shift value for PFMXPPRF_PPSYNCONLY         */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPSYNCONLY_MASK            0x10000000UL                                       /**< Bit mask for PFMXPPRF_PPSYNCONLY            */
#define _PFMXPPRF_RFIMDCDCCTRL2_PPSYNCONLY_DEFAULT         0x00000000UL                                       /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL2     */
#define PFMXPPRF_RFIMDCDCCTRL2_PPSYNCONLY_DEFAULT          (_PFMXPPRF_RFIMDCDCCTRL2_PPSYNCONLY_DEFAULT << 28) /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCCTRL2*/

/* Bit fields for PFMXPPRF RFIMDCDCSTATUS */
#define _PFMXPPRF_RFIMDCDCSTATUS_RESETVALUE                0x00000000UL                                        /**< Default value for PFMXPPRF_RFIMDCDCSTATUS   */
#define _PFMXPPRF_RFIMDCDCSTATUS_MASK                      0x0001FF07UL                                        /**< Mask for PFMXPPRF_RFIMDCDCSTATUS            */
#define PFMXPPRF_RFIMDCDCSTATUS_DCDCEN                     (0x1UL << 0)                                        /**< DCDC Enable Status                          */
#define _PFMXPPRF_RFIMDCDCSTATUS_DCDCEN_SHIFT              0                                                   /**< Shift value for PFMXPPRF_DCDCEN             */
#define _PFMXPPRF_RFIMDCDCSTATUS_DCDCEN_MASK               0x1UL                                               /**< Bit mask for PFMXPPRF_DCDCEN                */
#define _PFMXPPRF_RFIMDCDCSTATUS_DCDCEN_DEFAULT            0x00000000UL                                        /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCSTATUS    */
#define PFMXPPRF_RFIMDCDCSTATUS_DCDCEN_DEFAULT             (_PFMXPPRF_RFIMDCDCSTATUS_DCDCEN_DEFAULT << 0)      /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCSTATUS*/
#define PFMXPPRF_RFIMDCDCSTATUS_TXMAXSTATUS                (0x1UL << 1)                                        /**< TX MAX Status                               */
#define _PFMXPPRF_RFIMDCDCSTATUS_TXMAXSTATUS_SHIFT         1                                                   /**< Shift value for PFMXPPRF_TXMAXSTATUS        */
#define _PFMXPPRF_RFIMDCDCSTATUS_TXMAXSTATUS_MASK          0x2UL                                               /**< Bit mask for PFMXPPRF_TXMAXSTATUS           */
#define _PFMXPPRF_RFIMDCDCSTATUS_TXMAXSTATUS_DEFAULT       0x00000000UL                                        /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCSTATUS    */
#define PFMXPPRF_RFIMDCDCSTATUS_TXMAXSTATUS_DEFAULT        (_PFMXPPRF_RFIMDCDCSTATUS_TXMAXSTATUS_DEFAULT << 1) /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCSTATUS*/
#define PFMXPPRF_RFIMDCDCSTATUS_RXPPSTATUS                 (0x1UL << 2)                                        /**< RX PP Status                                */
#define _PFMXPPRF_RFIMDCDCSTATUS_RXPPSTATUS_SHIFT          2                                                   /**< Shift value for PFMXPPRF_RXPPSTATUS         */
#define _PFMXPPRF_RFIMDCDCSTATUS_RXPPSTATUS_MASK           0x4UL                                               /**< Bit mask for PFMXPPRF_RXPPSTATUS            */
#define _PFMXPPRF_RFIMDCDCSTATUS_RXPPSTATUS_DEFAULT        0x00000000UL                                        /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCSTATUS    */
#define PFMXPPRF_RFIMDCDCSTATUS_RXPPSTATUS_DEFAULT         (_PFMXPPRF_RFIMDCDCSTATUS_RXPPSTATUS_DEFAULT << 2)  /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCSTATUS*/
#define _PFMXPPRF_RFIMDCDCSTATUS_WNO1_SHIFT                8                                                   /**< Shift value for PFMXPPRF_WNO1               */
#define _PFMXPPRF_RFIMDCDCSTATUS_WNO1_MASK                 0x1FF00UL                                           /**< Bit mask for PFMXPPRF_WNO1                  */
#define _PFMXPPRF_RFIMDCDCSTATUS_WNO1_DEFAULT              0x00000000UL                                        /**< Mode DEFAULT for PFMXPPRF_RFIMDCDCSTATUS    */
#define PFMXPPRF_RFIMDCDCSTATUS_WNO1_DEFAULT               (_PFMXPPRF_RFIMDCDCSTATUS_WNO1_DEFAULT << 8)        /**< Shifted mode DEFAULT for PFMXPPRF_RFIMDCDCSTATUS*/

/* Bit fields for PFMXPPRF RPURATD0 */
#define _PFMXPPRF_RPURATD0_RESETVALUE                      0x00000000UL                                        /**< Default value for PFMXPPRF_RPURATD0         */
#define _PFMXPPRF_RPURATD0_MASK                            0x00000007UL                                        /**< Mask for PFMXPPRF_RPURATD0                  */
#define PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL0                (0x1UL << 0)                                        /**< RFIMDCDCCTRL0 Protection Bit                */
#define _PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL0_SHIFT         0                                                   /**< Shift value for PFMXPPRF_RATDRFIMDCDCCTRL0  */
#define _PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL0_MASK          0x1UL                                               /**< Bit mask for PFMXPPRF_RATDRFIMDCDCCTRL0     */
#define _PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL0_DEFAULT       0x00000000UL                                        /**< Mode DEFAULT for PFMXPPRF_RPURATD0          */
#define PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL0_DEFAULT        (_PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL0_DEFAULT << 0) /**< Shifted mode DEFAULT for PFMXPPRF_RPURATD0  */
#define PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL1                (0x1UL << 1)                                        /**< RFIMDCDCCTRL1 Protection Bit                */
#define _PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL1_SHIFT         1                                                   /**< Shift value for PFMXPPRF_RATDRFIMDCDCCTRL1  */
#define _PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL1_MASK          0x2UL                                               /**< Bit mask for PFMXPPRF_RATDRFIMDCDCCTRL1     */
#define _PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL1_DEFAULT       0x00000000UL                                        /**< Mode DEFAULT for PFMXPPRF_RPURATD0          */
#define PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL1_DEFAULT        (_PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL1_DEFAULT << 1) /**< Shifted mode DEFAULT for PFMXPPRF_RPURATD0  */
#define PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL2                (0x1UL << 2)                                        /**< RFIMDCDCCTRL2 Protection Bit                */
#define _PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL2_SHIFT         2                                                   /**< Shift value for PFMXPPRF_RATDRFIMDCDCCTRL2  */
#define _PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL2_MASK          0x4UL                                               /**< Bit mask for PFMXPPRF_RATDRFIMDCDCCTRL2     */
#define _PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL2_DEFAULT       0x00000000UL                                        /**< Mode DEFAULT for PFMXPPRF_RPURATD0          */
#define PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL2_DEFAULT        (_PFMXPPRF_RPURATD0_RATDRFIMDCDCCTRL2_DEFAULT << 2) /**< Shifted mode DEFAULT for PFMXPPRF_RPURATD0  */

/** @} End of group EFR32ZG23_PFMXPPRF_BitFields */
/** @} End of group EFR32ZG23_PFMXPPRF */
/** @} End of group Parts */

#endif /* EFR32ZG23_PFMXPPRF_H */
#ifdef __cplusplus
}
#endif

