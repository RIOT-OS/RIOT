/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 SYSCFG register and bit field definitions
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

#ifndef EFR32ZG23_SYSCFG_H
#define EFR32ZG23_SYSCFG_H
#define SYSCFG_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_SYSCFG SYSCFG
 * @{
 * @brief EFR32ZG23 SYSCFG Register Declaration.
 *****************************************************************************/

/** SYSCFG Register Declaration. */
typedef struct {
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  __IOM uint32_t IF;                            /**< Interrupt Flag                                     */
  __IOM uint32_t IEN;                           /**< Interrupt Enable                                   */
  uint32_t       RESERVED1[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW;                     /**< Chip Revision, Hard-wired                          */
  __IOM uint32_t CHIPREV;                       /**< Part Family and Revision Values                    */
  uint32_t       RESERVED2[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFGSYSTIC;                     /**< SysTick clock source                               */
  uint32_t       RESERVED3[54U];                /**< Reserved for future use                            */
  uint32_t       RESERVED4[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED5[63U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL;                          /**< Control                                            */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL;                 /**< DMEM0 Retention Control                            */
  uint32_t       RESERVED7[64U];                /**< Reserved for future use                            */
  __IOM uint32_t RAMBIASCONF;                   /**< RAM Bias Configuration                             */
  uint32_t       RESERVED8[60U];                /**< Reserved for future use                            */
  __IOM uint32_t RADIORAMRETNCTRL;              /**< RADIO RAM Retention Control Register               */
  uint32_t       RESERVED9[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t RADIOECCCTRL;                  /**< RADIO RAM ECC Control Register                     */
  uint32_t       RESERVED10[1U];                /**< Reserved for future use                            */
  __IM uint32_t  SEQRAMECCADDR;                 /**< SEQRAM ECC Address                                 */
  __IM uint32_t  FRCRAMECCADDR;                 /**< FRCRAM ECC Address                                 */
  __IOM uint32_t ICACHERAMRETNCTRL;             /**< HOST ICACHERAM Retention Control                   */
  __IOM uint32_t DMEM0PORTMAPSEL;               /**< DMEM0 port remap selection                         */
  uint32_t       RESERVED11[120U];              /**< Reserved for future use                            */
  __IOM uint32_t ROOTDATA0;                     /**< Data Register 0                                    */
  __IOM uint32_t ROOTDATA1;                     /**< Data Register 1                                    */
  __IM uint32_t  ROOTLOCKSTATUS;                /**< Lock Status                                        */
  __IOM uint32_t ROOTSESWVERSION;               /**< SE SW Version                                      */
  uint32_t       RESERVED12[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED13[635U];              /**< Reserved for future use                            */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable                                   */
  uint32_t       RESERVED15[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW_SET;                 /**< Chip Revision, Hard-wired                          */
  __IOM uint32_t CHIPREV_SET;                   /**< Part Family and Revision Values                    */
  uint32_t       RESERVED16[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CFGSYSTIC_SET;                 /**< SysTick clock source                               */
  uint32_t       RESERVED17[54U];               /**< Reserved for future use                            */
  uint32_t       RESERVED18[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED19[63U];               /**< Reserved for future use                            */
  __IOM uint32_t CTRL_SET;                      /**< Control                                            */
  uint32_t       RESERVED20[1U];                /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL_SET;             /**< DMEM0 Retention Control                            */
  uint32_t       RESERVED21[64U];               /**< Reserved for future use                            */
  __IOM uint32_t RAMBIASCONF_SET;               /**< RAM Bias Configuration                             */
  uint32_t       RESERVED22[60U];               /**< Reserved for future use                            */
  __IOM uint32_t RADIORAMRETNCTRL_SET;          /**< RADIO RAM Retention Control Register               */
  uint32_t       RESERVED23[1U];                /**< Reserved for future use                            */
  __IOM uint32_t RADIOECCCTRL_SET;              /**< RADIO RAM ECC Control Register                     */
  uint32_t       RESERVED24[1U];                /**< Reserved for future use                            */
  __IM uint32_t  SEQRAMECCADDR_SET;             /**< SEQRAM ECC Address                                 */
  __IM uint32_t  FRCRAMECCADDR_SET;             /**< FRCRAM ECC Address                                 */
  __IOM uint32_t ICACHERAMRETNCTRL_SET;         /**< HOST ICACHERAM Retention Control                   */
  __IOM uint32_t DMEM0PORTMAPSEL_SET;           /**< DMEM0 port remap selection                         */
  uint32_t       RESERVED25[120U];              /**< Reserved for future use                            */
  __IOM uint32_t ROOTDATA0_SET;                 /**< Data Register 0                                    */
  __IOM uint32_t ROOTDATA1_SET;                 /**< Data Register 1                                    */
  __IM uint32_t  ROOTLOCKSTATUS_SET;            /**< Lock Status                                        */
  __IOM uint32_t ROOTSESWVERSION_SET;           /**< SE SW Version                                      */
  uint32_t       RESERVED26[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED27[635U];              /**< Reserved for future use                            */
  uint32_t       RESERVED28[1U];                /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable                                   */
  uint32_t       RESERVED29[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW_CLR;                 /**< Chip Revision, Hard-wired                          */
  __IOM uint32_t CHIPREV_CLR;                   /**< Part Family and Revision Values                    */
  uint32_t       RESERVED30[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CFGSYSTIC_CLR;                 /**< SysTick clock source                               */
  uint32_t       RESERVED31[54U];               /**< Reserved for future use                            */
  uint32_t       RESERVED32[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED33[63U];               /**< Reserved for future use                            */
  __IOM uint32_t CTRL_CLR;                      /**< Control                                            */
  uint32_t       RESERVED34[1U];                /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL_CLR;             /**< DMEM0 Retention Control                            */
  uint32_t       RESERVED35[64U];               /**< Reserved for future use                            */
  __IOM uint32_t RAMBIASCONF_CLR;               /**< RAM Bias Configuration                             */
  uint32_t       RESERVED36[60U];               /**< Reserved for future use                            */
  __IOM uint32_t RADIORAMRETNCTRL_CLR;          /**< RADIO RAM Retention Control Register               */
  uint32_t       RESERVED37[1U];                /**< Reserved for future use                            */
  __IOM uint32_t RADIOECCCTRL_CLR;              /**< RADIO RAM ECC Control Register                     */
  uint32_t       RESERVED38[1U];                /**< Reserved for future use                            */
  __IM uint32_t  SEQRAMECCADDR_CLR;             /**< SEQRAM ECC Address                                 */
  __IM uint32_t  FRCRAMECCADDR_CLR;             /**< FRCRAM ECC Address                                 */
  __IOM uint32_t ICACHERAMRETNCTRL_CLR;         /**< HOST ICACHERAM Retention Control                   */
  __IOM uint32_t DMEM0PORTMAPSEL_CLR;           /**< DMEM0 port remap selection                         */
  uint32_t       RESERVED39[120U];              /**< Reserved for future use                            */
  __IOM uint32_t ROOTDATA0_CLR;                 /**< Data Register 0                                    */
  __IOM uint32_t ROOTDATA1_CLR;                 /**< Data Register 1                                    */
  __IM uint32_t  ROOTLOCKSTATUS_CLR;            /**< Lock Status                                        */
  __IOM uint32_t ROOTSESWVERSION_CLR;           /**< SE SW Version                                      */
  uint32_t       RESERVED40[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED41[635U];              /**< Reserved for future use                            */
  uint32_t       RESERVED42[1U];                /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag                                     */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable                                   */
  uint32_t       RESERVED43[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW_TGL;                 /**< Chip Revision, Hard-wired                          */
  __IOM uint32_t CHIPREV_TGL;                   /**< Part Family and Revision Values                    */
  uint32_t       RESERVED44[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CFGSYSTIC_TGL;                 /**< SysTick clock source                               */
  uint32_t       RESERVED45[54U];               /**< Reserved for future use                            */
  uint32_t       RESERVED46[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED47[63U];               /**< Reserved for future use                            */
  __IOM uint32_t CTRL_TGL;                      /**< Control                                            */
  uint32_t       RESERVED48[1U];                /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL_TGL;             /**< DMEM0 Retention Control                            */
  uint32_t       RESERVED49[64U];               /**< Reserved for future use                            */
  __IOM uint32_t RAMBIASCONF_TGL;               /**< RAM Bias Configuration                             */
  uint32_t       RESERVED50[60U];               /**< Reserved for future use                            */
  __IOM uint32_t RADIORAMRETNCTRL_TGL;          /**< RADIO RAM Retention Control Register               */
  uint32_t       RESERVED51[1U];                /**< Reserved for future use                            */
  __IOM uint32_t RADIOECCCTRL_TGL;              /**< RADIO RAM ECC Control Register                     */
  uint32_t       RESERVED52[1U];                /**< Reserved for future use                            */
  __IM uint32_t  SEQRAMECCADDR_TGL;             /**< SEQRAM ECC Address                                 */
  __IM uint32_t  FRCRAMECCADDR_TGL;             /**< FRCRAM ECC Address                                 */
  __IOM uint32_t ICACHERAMRETNCTRL_TGL;         /**< HOST ICACHERAM Retention Control                   */
  __IOM uint32_t DMEM0PORTMAPSEL_TGL;           /**< DMEM0 port remap selection                         */
  uint32_t       RESERVED53[120U];              /**< Reserved for future use                            */
  __IOM uint32_t ROOTDATA0_TGL;                 /**< Data Register 0                                    */
  __IOM uint32_t ROOTDATA1_TGL;                 /**< Data Register 1                                    */
  __IM uint32_t  ROOTLOCKSTATUS_TGL;            /**< Lock Status                                        */
  __IOM uint32_t ROOTSESWVERSION_TGL;           /**< SE SW Version                                      */
  uint32_t       RESERVED54[1U];                /**< Reserved for future use                            */
} SYSCFG_TypeDef;
/** @} End of group EFR32ZG23_SYSCFG */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_SYSCFG
 * @{
 * @defgroup EFR32ZG23_SYSCFG_BitFields SYSCFG Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SYSCFG IPVERSION */
#define _SYSCFG_IPVERSION_RESETVALUE                       0x00000002UL                               /**< Default value for SYSCFG_IPVERSION          */
#define _SYSCFG_IPVERSION_MASK                             0xFFFFFFFFUL                               /**< Mask for SYSCFG_IPVERSION                   */
#define _SYSCFG_IPVERSION_IPVERSION_SHIFT                  0                                          /**< Shift value for SYSCFG_IPVERSION            */
#define _SYSCFG_IPVERSION_IPVERSION_MASK                   0xFFFFFFFFUL                               /**< Bit mask for SYSCFG_IPVERSION               */
#define _SYSCFG_IPVERSION_IPVERSION_DEFAULT                0x00000002UL                               /**< Mode DEFAULT for SYSCFG_IPVERSION           */
#define SYSCFG_IPVERSION_IPVERSION_DEFAULT                 (_SYSCFG_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_IPVERSION   */

/* Bit fields for SYSCFG IF */
#define _SYSCFG_IF_RESETVALUE                              0x00000000UL                           /**< Default value for SYSCFG_IF                 */
#define _SYSCFG_IF_MASK                                    0x33003F0FUL                           /**< Mask for SYSCFG_IF                          */
#define SYSCFG_IF_SW0                                      (0x1UL << 0)                           /**< Software Interrupt Flag                     */
#define _SYSCFG_IF_SW0_SHIFT                               0                                      /**< Shift value for SYSCFG_SW0                  */
#define _SYSCFG_IF_SW0_MASK                                0x1UL                                  /**< Bit mask for SYSCFG_SW0                     */
#define _SYSCFG_IF_SW0_DEFAULT                             0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW0_DEFAULT                              (_SYSCFG_IF_SW0_DEFAULT << 0)          /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SW1                                      (0x1UL << 1)                           /**< Software Interrupt Flag                     */
#define _SYSCFG_IF_SW1_SHIFT                               1                                      /**< Shift value for SYSCFG_SW1                  */
#define _SYSCFG_IF_SW1_MASK                                0x2UL                                  /**< Bit mask for SYSCFG_SW1                     */
#define _SYSCFG_IF_SW1_DEFAULT                             0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW1_DEFAULT                              (_SYSCFG_IF_SW1_DEFAULT << 1)          /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SW2                                      (0x1UL << 2)                           /**< Software Interrupt Flag                     */
#define _SYSCFG_IF_SW2_SHIFT                               2                                      /**< Shift value for SYSCFG_SW2                  */
#define _SYSCFG_IF_SW2_MASK                                0x4UL                                  /**< Bit mask for SYSCFG_SW2                     */
#define _SYSCFG_IF_SW2_DEFAULT                             0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW2_DEFAULT                              (_SYSCFG_IF_SW2_DEFAULT << 2)          /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SW3                                      (0x1UL << 3)                           /**< Software Interrupt Flag                     */
#define _SYSCFG_IF_SW3_SHIFT                               3                                      /**< Shift value for SYSCFG_SW3                  */
#define _SYSCFG_IF_SW3_MASK                                0x8UL                                  /**< Bit mask for SYSCFG_SW3                     */
#define _SYSCFG_IF_SW3_DEFAULT                             0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW3_DEFAULT                              (_SYSCFG_IF_SW3_DEFAULT << 3)          /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_FPIOC                                    (0x1UL << 8)                           /**< FPU Invalid Operation interrupt flag        */
#define _SYSCFG_IF_FPIOC_SHIFT                             8                                      /**< Shift value for SYSCFG_FPIOC                */
#define _SYSCFG_IF_FPIOC_MASK                              0x100UL                                /**< Bit mask for SYSCFG_FPIOC                   */
#define _SYSCFG_IF_FPIOC_DEFAULT                           0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_FPIOC_DEFAULT                            (_SYSCFG_IF_FPIOC_DEFAULT << 8)        /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_FPDZC                                    (0x1UL << 9)                           /**< FPU Divide by zero interrupt flag           */
#define _SYSCFG_IF_FPDZC_SHIFT                             9                                      /**< Shift value for SYSCFG_FPDZC                */
#define _SYSCFG_IF_FPDZC_MASK                              0x200UL                                /**< Bit mask for SYSCFG_FPDZC                   */
#define _SYSCFG_IF_FPDZC_DEFAULT                           0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_FPDZC_DEFAULT                            (_SYSCFG_IF_FPDZC_DEFAULT << 9)        /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_FPUFC                                    (0x1UL << 10)                          /**< FPU Underflow interrupt flag                */
#define _SYSCFG_IF_FPUFC_SHIFT                             10                                     /**< Shift value for SYSCFG_FPUFC                */
#define _SYSCFG_IF_FPUFC_MASK                              0x400UL                                /**< Bit mask for SYSCFG_FPUFC                   */
#define _SYSCFG_IF_FPUFC_DEFAULT                           0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_FPUFC_DEFAULT                            (_SYSCFG_IF_FPUFC_DEFAULT << 10)       /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_FPOFC                                    (0x1UL << 11)                          /**< FPU Overflow interrupt flag                 */
#define _SYSCFG_IF_FPOFC_SHIFT                             11                                     /**< Shift value for SYSCFG_FPOFC                */
#define _SYSCFG_IF_FPOFC_MASK                              0x800UL                                /**< Bit mask for SYSCFG_FPOFC                   */
#define _SYSCFG_IF_FPOFC_DEFAULT                           0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_FPOFC_DEFAULT                            (_SYSCFG_IF_FPOFC_DEFAULT << 11)       /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_FPIDC                                    (0x1UL << 12)                          /**< FPU Input denormal interrupt flag           */
#define _SYSCFG_IF_FPIDC_SHIFT                             12                                     /**< Shift value for SYSCFG_FPIDC                */
#define _SYSCFG_IF_FPIDC_MASK                              0x1000UL                               /**< Bit mask for SYSCFG_FPIDC                   */
#define _SYSCFG_IF_FPIDC_DEFAULT                           0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_FPIDC_DEFAULT                            (_SYSCFG_IF_FPIDC_DEFAULT << 12)       /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_FPIXC                                    (0x1UL << 13)                          /**< FPU Inexact interrupt flag                  */
#define _SYSCFG_IF_FPIXC_SHIFT                             13                                     /**< Shift value for SYSCFG_FPIXC                */
#define _SYSCFG_IF_FPIXC_MASK                              0x2000UL                               /**< Bit mask for SYSCFG_FPIXC                   */
#define _SYSCFG_IF_FPIXC_DEFAULT                           0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_FPIXC_DEFAULT                            (_SYSCFG_IF_FPIXC_DEFAULT << 13)       /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SEQRAMERR1B                              (0x1UL << 24)                          /**< SEQRAM Error 1-bit Interrupt Flag           */
#define _SYSCFG_IF_SEQRAMERR1B_SHIFT                       24                                     /**< Shift value for SYSCFG_SEQRAMERR1B          */
#define _SYSCFG_IF_SEQRAMERR1B_MASK                        0x1000000UL                            /**< Bit mask for SYSCFG_SEQRAMERR1B             */
#define _SYSCFG_IF_SEQRAMERR1B_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SEQRAMERR1B_DEFAULT                      (_SYSCFG_IF_SEQRAMERR1B_DEFAULT << 24) /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SEQRAMERR2B                              (0x1UL << 25)                          /**< SEQRAM Error 2-bit Interrupt Flag           */
#define _SYSCFG_IF_SEQRAMERR2B_SHIFT                       25                                     /**< Shift value for SYSCFG_SEQRAMERR2B          */
#define _SYSCFG_IF_SEQRAMERR2B_MASK                        0x2000000UL                            /**< Bit mask for SYSCFG_SEQRAMERR2B             */
#define _SYSCFG_IF_SEQRAMERR2B_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SEQRAMERR2B_DEFAULT                      (_SYSCFG_IF_SEQRAMERR2B_DEFAULT << 25) /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_FRCRAMERR1B                              (0x1UL << 28)                          /**< FRCRAM Error 1-bit Interrupt Flag           */
#define _SYSCFG_IF_FRCRAMERR1B_SHIFT                       28                                     /**< Shift value for SYSCFG_FRCRAMERR1B          */
#define _SYSCFG_IF_FRCRAMERR1B_MASK                        0x10000000UL                           /**< Bit mask for SYSCFG_FRCRAMERR1B             */
#define _SYSCFG_IF_FRCRAMERR1B_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_FRCRAMERR1B_DEFAULT                      (_SYSCFG_IF_FRCRAMERR1B_DEFAULT << 28) /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_FRCRAMERR2B                              (0x1UL << 29)                          /**< FRCRAM Error 2-bit Interrupt Flag           */
#define _SYSCFG_IF_FRCRAMERR2B_SHIFT                       29                                     /**< Shift value for SYSCFG_FRCRAMERR2B          */
#define _SYSCFG_IF_FRCRAMERR2B_MASK                        0x20000000UL                           /**< Bit mask for SYSCFG_FRCRAMERR2B             */
#define _SYSCFG_IF_FRCRAMERR2B_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_FRCRAMERR2B_DEFAULT                      (_SYSCFG_IF_FRCRAMERR2B_DEFAULT << 29) /**< Shifted mode DEFAULT for SYSCFG_IF          */

/* Bit fields for SYSCFG IEN */
#define _SYSCFG_IEN_RESETVALUE                             0x00000000UL                            /**< Default value for SYSCFG_IEN                */
#define _SYSCFG_IEN_MASK                                   0x33003F0FUL                            /**< Mask for SYSCFG_IEN                         */
#define SYSCFG_IEN_SW0                                     (0x1UL << 0)                            /**< Software Interrupt Enable                   */
#define _SYSCFG_IEN_SW0_SHIFT                              0                                       /**< Shift value for SYSCFG_SW0                  */
#define _SYSCFG_IEN_SW0_MASK                               0x1UL                                   /**< Bit mask for SYSCFG_SW0                     */
#define _SYSCFG_IEN_SW0_DEFAULT                            0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW0_DEFAULT                             (_SYSCFG_IEN_SW0_DEFAULT << 0)          /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SW1                                     (0x1UL << 1)                            /**< Software Interrupt Enable                   */
#define _SYSCFG_IEN_SW1_SHIFT                              1                                       /**< Shift value for SYSCFG_SW1                  */
#define _SYSCFG_IEN_SW1_MASK                               0x2UL                                   /**< Bit mask for SYSCFG_SW1                     */
#define _SYSCFG_IEN_SW1_DEFAULT                            0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW1_DEFAULT                             (_SYSCFG_IEN_SW1_DEFAULT << 1)          /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SW2                                     (0x1UL << 2)                            /**< Software Interrupt Enable                   */
#define _SYSCFG_IEN_SW2_SHIFT                              2                                       /**< Shift value for SYSCFG_SW2                  */
#define _SYSCFG_IEN_SW2_MASK                               0x4UL                                   /**< Bit mask for SYSCFG_SW2                     */
#define _SYSCFG_IEN_SW2_DEFAULT                            0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW2_DEFAULT                             (_SYSCFG_IEN_SW2_DEFAULT << 2)          /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SW3                                     (0x1UL << 3)                            /**< Software Interrupt Enable                   */
#define _SYSCFG_IEN_SW3_SHIFT                              3                                       /**< Shift value for SYSCFG_SW3                  */
#define _SYSCFG_IEN_SW3_MASK                               0x8UL                                   /**< Bit mask for SYSCFG_SW3                     */
#define _SYSCFG_IEN_SW3_DEFAULT                            0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW3_DEFAULT                             (_SYSCFG_IEN_SW3_DEFAULT << 3)          /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_FPIOC                                   (0x1UL << 8)                            /**< FPU Invalid Operation Interrupt Enable      */
#define _SYSCFG_IEN_FPIOC_SHIFT                            8                                       /**< Shift value for SYSCFG_FPIOC                */
#define _SYSCFG_IEN_FPIOC_MASK                             0x100UL                                 /**< Bit mask for SYSCFG_FPIOC                   */
#define _SYSCFG_IEN_FPIOC_DEFAULT                          0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_FPIOC_DEFAULT                           (_SYSCFG_IEN_FPIOC_DEFAULT << 8)        /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_FPDZC                                   (0x1UL << 9)                            /**< FPU Divide by zero Interrupt Enable         */
#define _SYSCFG_IEN_FPDZC_SHIFT                            9                                       /**< Shift value for SYSCFG_FPDZC                */
#define _SYSCFG_IEN_FPDZC_MASK                             0x200UL                                 /**< Bit mask for SYSCFG_FPDZC                   */
#define _SYSCFG_IEN_FPDZC_DEFAULT                          0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_FPDZC_DEFAULT                           (_SYSCFG_IEN_FPDZC_DEFAULT << 9)        /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_FPUFC                                   (0x1UL << 10)                           /**< FPU Underflow Interrupt Enable              */
#define _SYSCFG_IEN_FPUFC_SHIFT                            10                                      /**< Shift value for SYSCFG_FPUFC                */
#define _SYSCFG_IEN_FPUFC_MASK                             0x400UL                                 /**< Bit mask for SYSCFG_FPUFC                   */
#define _SYSCFG_IEN_FPUFC_DEFAULT                          0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_FPUFC_DEFAULT                           (_SYSCFG_IEN_FPUFC_DEFAULT << 10)       /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_FPOFC                                   (0x1UL << 11)                           /**< FPU Overflow Interrupt Enable               */
#define _SYSCFG_IEN_FPOFC_SHIFT                            11                                      /**< Shift value for SYSCFG_FPOFC                */
#define _SYSCFG_IEN_FPOFC_MASK                             0x800UL                                 /**< Bit mask for SYSCFG_FPOFC                   */
#define _SYSCFG_IEN_FPOFC_DEFAULT                          0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_FPOFC_DEFAULT                           (_SYSCFG_IEN_FPOFC_DEFAULT << 11)       /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_FPIDC                                   (0x1UL << 12)                           /**< FPU Input denormal Interrupt Enable         */
#define _SYSCFG_IEN_FPIDC_SHIFT                            12                                      /**< Shift value for SYSCFG_FPIDC                */
#define _SYSCFG_IEN_FPIDC_MASK                             0x1000UL                                /**< Bit mask for SYSCFG_FPIDC                   */
#define _SYSCFG_IEN_FPIDC_DEFAULT                          0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_FPIDC_DEFAULT                           (_SYSCFG_IEN_FPIDC_DEFAULT << 12)       /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_FPIXC                                   (0x1UL << 13)                           /**< FPU Inexact Interrupt Enable                */
#define _SYSCFG_IEN_FPIXC_SHIFT                            13                                      /**< Shift value for SYSCFG_FPIXC                */
#define _SYSCFG_IEN_FPIXC_MASK                             0x2000UL                                /**< Bit mask for SYSCFG_FPIXC                   */
#define _SYSCFG_IEN_FPIXC_DEFAULT                          0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_FPIXC_DEFAULT                           (_SYSCFG_IEN_FPIXC_DEFAULT << 13)       /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SEQRAMERR1B                             (0x1UL << 24)                           /**< SEQRAM Error 1-bit Interrupt Enable         */
#define _SYSCFG_IEN_SEQRAMERR1B_SHIFT                      24                                      /**< Shift value for SYSCFG_SEQRAMERR1B          */
#define _SYSCFG_IEN_SEQRAMERR1B_MASK                       0x1000000UL                             /**< Bit mask for SYSCFG_SEQRAMERR1B             */
#define _SYSCFG_IEN_SEQRAMERR1B_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SEQRAMERR1B_DEFAULT                     (_SYSCFG_IEN_SEQRAMERR1B_DEFAULT << 24) /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SEQRAMERR2B                             (0x1UL << 25)                           /**< SEQRAM Error 2-bit Interrupt Enable         */
#define _SYSCFG_IEN_SEQRAMERR2B_SHIFT                      25                                      /**< Shift value for SYSCFG_SEQRAMERR2B          */
#define _SYSCFG_IEN_SEQRAMERR2B_MASK                       0x2000000UL                             /**< Bit mask for SYSCFG_SEQRAMERR2B             */
#define _SYSCFG_IEN_SEQRAMERR2B_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SEQRAMERR2B_DEFAULT                     (_SYSCFG_IEN_SEQRAMERR2B_DEFAULT << 25) /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_FRCRAMERR1B                             (0x1UL << 28)                           /**< FRCRAM Error 1-bit Interrupt Enable         */
#define _SYSCFG_IEN_FRCRAMERR1B_SHIFT                      28                                      /**< Shift value for SYSCFG_FRCRAMERR1B          */
#define _SYSCFG_IEN_FRCRAMERR1B_MASK                       0x10000000UL                            /**< Bit mask for SYSCFG_FRCRAMERR1B             */
#define _SYSCFG_IEN_FRCRAMERR1B_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_FRCRAMERR1B_DEFAULT                     (_SYSCFG_IEN_FRCRAMERR1B_DEFAULT << 28) /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_FRCRAMERR2B                             (0x1UL << 29)                           /**< FRCRAM Error 2-bit Interrupt Enable         */
#define _SYSCFG_IEN_FRCRAMERR2B_SHIFT                      29                                      /**< Shift value for SYSCFG_FRCRAMERR2B          */
#define _SYSCFG_IEN_FRCRAMERR2B_MASK                       0x20000000UL                            /**< Bit mask for SYSCFG_FRCRAMERR2B             */
#define _SYSCFG_IEN_FRCRAMERR2B_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_FRCRAMERR2B_DEFAULT                     (_SYSCFG_IEN_FRCRAMERR2B_DEFAULT << 29) /**< Shifted mode DEFAULT for SYSCFG_IEN         */

/* Bit fields for SYSCFG CHIPREVHW */
#define _SYSCFG_CHIPREVHW_RESETVALUE                       0x00000E01UL                            /**< Default value for SYSCFG_CHIPREVHW          */
#define _SYSCFG_CHIPREVHW_MASK                             0xFF0FFFFFUL                            /**< Mask for SYSCFG_CHIPREVHW                   */
#define _SYSCFG_CHIPREVHW_MAJOR_SHIFT                      0                                       /**< Shift value for SYSCFG_MAJOR                */
#define _SYSCFG_CHIPREVHW_MAJOR_MASK                       0x3FUL                                  /**< Bit mask for SYSCFG_MAJOR                   */
#define _SYSCFG_CHIPREVHW_MAJOR_DEFAULT                    0x00000001UL                            /**< Mode DEFAULT for SYSCFG_CHIPREVHW           */
#define SYSCFG_CHIPREVHW_MAJOR_DEFAULT                     (_SYSCFG_CHIPREVHW_MAJOR_DEFAULT << 0)  /**< Shifted mode DEFAULT for SYSCFG_CHIPREVHW   */
#define _SYSCFG_CHIPREVHW_FAMILY_SHIFT                     6                                       /**< Shift value for SYSCFG_FAMILY               */
#define _SYSCFG_CHIPREVHW_FAMILY_MASK                      0xFC0UL                                 /**< Bit mask for SYSCFG_FAMILY                  */
#define _SYSCFG_CHIPREVHW_FAMILY_DEFAULT                   0x00000038UL                            /**< Mode DEFAULT for SYSCFG_CHIPREVHW           */
#define SYSCFG_CHIPREVHW_FAMILY_DEFAULT                    (_SYSCFG_CHIPREVHW_FAMILY_DEFAULT << 6) /**< Shifted mode DEFAULT for SYSCFG_CHIPREVHW   */
#define _SYSCFG_CHIPREVHW_MINOR_SHIFT                      12                                      /**< Shift value for SYSCFG_MINOR                */
#define _SYSCFG_CHIPREVHW_MINOR_MASK                       0xFF000UL                               /**< Bit mask for SYSCFG_MINOR                   */
#define _SYSCFG_CHIPREVHW_MINOR_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for SYSCFG_CHIPREVHW           */
#define SYSCFG_CHIPREVHW_MINOR_DEFAULT                     (_SYSCFG_CHIPREVHW_MINOR_DEFAULT << 12) /**< Shifted mode DEFAULT for SYSCFG_CHIPREVHW   */

/* Bit fields for SYSCFG CHIPREV */
#define _SYSCFG_CHIPREV_RESETVALUE                         0x00000000UL                          /**< Default value for SYSCFG_CHIPREV            */
#define _SYSCFG_CHIPREV_MASK                               0x000FFFFFUL                          /**< Mask for SYSCFG_CHIPREV                     */
#define _SYSCFG_CHIPREV_MAJOR_SHIFT                        0                                     /**< Shift value for SYSCFG_MAJOR                */
#define _SYSCFG_CHIPREV_MAJOR_MASK                         0x3FUL                                /**< Bit mask for SYSCFG_MAJOR                   */
#define _SYSCFG_CHIPREV_MAJOR_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for SYSCFG_CHIPREV             */
#define SYSCFG_CHIPREV_MAJOR_DEFAULT                       (_SYSCFG_CHIPREV_MAJOR_DEFAULT << 0)  /**< Shifted mode DEFAULT for SYSCFG_CHIPREV     */
#define _SYSCFG_CHIPREV_FAMILY_SHIFT                       6                                     /**< Shift value for SYSCFG_FAMILY               */
#define _SYSCFG_CHIPREV_FAMILY_MASK                        0xFC0UL                               /**< Bit mask for SYSCFG_FAMILY                  */
#define _SYSCFG_CHIPREV_FAMILY_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for SYSCFG_CHIPREV             */
#define SYSCFG_CHIPREV_FAMILY_DEFAULT                      (_SYSCFG_CHIPREV_FAMILY_DEFAULT << 6) /**< Shifted mode DEFAULT for SYSCFG_CHIPREV     */
#define _SYSCFG_CHIPREV_MINOR_SHIFT                        12                                    /**< Shift value for SYSCFG_MINOR                */
#define _SYSCFG_CHIPREV_MINOR_MASK                         0xFF000UL                             /**< Bit mask for SYSCFG_MINOR                   */
#define _SYSCFG_CHIPREV_MINOR_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for SYSCFG_CHIPREV             */
#define SYSCFG_CHIPREV_MINOR_DEFAULT                       (_SYSCFG_CHIPREV_MINOR_DEFAULT << 12) /**< Shifted mode DEFAULT for SYSCFG_CHIPREV     */

/* Bit fields for SYSCFG CFGSYSTIC */
#define _SYSCFG_CFGSYSTIC_RESETVALUE                       0x00000000UL                                    /**< Default value for SYSCFG_CFGSYSTIC          */
#define _SYSCFG_CFGSYSTIC_MASK                             0x00000001UL                                    /**< Mask for SYSCFG_CFGSYSTIC                   */
#define SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN                    (0x1UL << 0)                                    /**< SysTick External Clock Enable               */
#define _SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN_SHIFT             0                                               /**< Shift value for SYSCFG_SYSTICEXTCLKEN       */
#define _SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN_MASK              0x1UL                                           /**< Bit mask for SYSCFG_SYSTICEXTCLKEN          */
#define _SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for SYSCFG_CFGSYSTIC           */
#define SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN_DEFAULT            (_SYSCFG_CFGSYSTIC_SYSTICEXTCLKEN_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_CFGSYSTIC   */

/* Bit fields for SYSCFG CTRL */
#define _SYSCFG_CTRL_RESETVALUE                            0x00000023UL                                 /**< Default value for SYSCFG_CTRL               */
#define _SYSCFG_CTRL_MASK                                  0x00000023UL                                 /**< Mask for SYSCFG_CTRL                        */
#define SYSCFG_CTRL_ADDRFAULTEN                            (0x1UL << 0)                                 /**< Invalid Address Bus Fault Response Enabl    */
#define _SYSCFG_CTRL_ADDRFAULTEN_SHIFT                     0                                            /**< Shift value for SYSCFG_ADDRFAULTEN          */
#define _SYSCFG_CTRL_ADDRFAULTEN_MASK                      0x1UL                                        /**< Bit mask for SYSCFG_ADDRFAULTEN             */
#define _SYSCFG_CTRL_ADDRFAULTEN_DEFAULT                   0x00000001UL                                 /**< Mode DEFAULT for SYSCFG_CTRL                */
#define SYSCFG_CTRL_ADDRFAULTEN_DEFAULT                    (_SYSCFG_CTRL_ADDRFAULTEN_DEFAULT << 0)      /**< Shifted mode DEFAULT for SYSCFG_CTRL        */
#define SYSCFG_CTRL_CLKDISFAULTEN                          (0x1UL << 1)                                 /**< Disabled Clkbus Bus Fault Enable            */
#define _SYSCFG_CTRL_CLKDISFAULTEN_SHIFT                   1                                            /**< Shift value for SYSCFG_CLKDISFAULTEN        */
#define _SYSCFG_CTRL_CLKDISFAULTEN_MASK                    0x2UL                                        /**< Bit mask for SYSCFG_CLKDISFAULTEN           */
#define _SYSCFG_CTRL_CLKDISFAULTEN_DEFAULT                 0x00000001UL                                 /**< Mode DEFAULT for SYSCFG_CTRL                */
#define SYSCFG_CTRL_CLKDISFAULTEN_DEFAULT                  (_SYSCFG_CTRL_CLKDISFAULTEN_DEFAULT << 1)    /**< Shifted mode DEFAULT for SYSCFG_CTRL        */
#define SYSCFG_CTRL_RAMECCERRFAULTEN                       (0x1UL << 5)                                 /**< Two bit ECC error bus fault response ena    */
#define _SYSCFG_CTRL_RAMECCERRFAULTEN_SHIFT                5                                            /**< Shift value for SYSCFG_RAMECCERRFAULTEN     */
#define _SYSCFG_CTRL_RAMECCERRFAULTEN_MASK                 0x20UL                                       /**< Bit mask for SYSCFG_RAMECCERRFAULTEN        */
#define _SYSCFG_CTRL_RAMECCERRFAULTEN_DEFAULT              0x00000001UL                                 /**< Mode DEFAULT for SYSCFG_CTRL                */
#define SYSCFG_CTRL_RAMECCERRFAULTEN_DEFAULT               (_SYSCFG_CTRL_RAMECCERRFAULTEN_DEFAULT << 5) /**< Shifted mode DEFAULT for SYSCFG_CTRL        */

/* Bit fields for SYSCFG DMEM0RETNCTRL */
#define _SYSCFG_DMEM0RETNCTRL_RESETVALUE                   0x00000000UL                                     /**< Default value for SYSCFG_DMEM0RETNCTRL      */
#define _SYSCFG_DMEM0RETNCTRL_MASK                         0x00000007UL                                     /**< Mask for SYSCFG_DMEM0RETNCTRL               */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_SHIFT            0                                                /**< Shift value for SYSCFG_RAMRETNCTRL          */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_MASK             0x7UL                                            /**< Bit mask for SYSCFG_RAMRETNCTRL             */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for SYSCFG_DMEM0RETNCTRL       */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_ALLON            0x00000000UL                                     /**< Mode ALLON for SYSCFG_DMEM0RETNCTRL         */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK3             0x00000004UL                                     /**< Mode BLK3 for SYSCFG_DMEM0RETNCTRL          */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK2TO3          0x00000006UL                                     /**< Mode BLK2TO3 for SYSCFG_DMEM0RETNCTRL       */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK1TO3          0x00000007UL                                     /**< Mode BLK1TO3 for SYSCFG_DMEM0RETNCTRL       */
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_DEFAULT           (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_DMEM0RETNCTRL*/
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_ALLON             (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_ALLON << 0)   /**< Shifted mode ALLON for SYSCFG_DMEM0RETNCTRL */
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK3              (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK3 << 0)    /**< Shifted mode BLK3 for SYSCFG_DMEM0RETNCTRL  */
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK2TO3           (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK2TO3 << 0) /**< Shifted mode BLK2TO3 for SYSCFG_DMEM0RETNCTRL*/
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK1TO3           (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK1TO3 << 0) /**< Shifted mode BLK1TO3 for SYSCFG_DMEM0RETNCTRL*/

/* Bit fields for SYSCFG RAMBIASCONF */
#define _SYSCFG_RAMBIASCONF_RESETVALUE                     0x00000002UL                                   /**< Default value for SYSCFG_RAMBIASCONF        */
#define _SYSCFG_RAMBIASCONF_MASK                           0x0000000FUL                                   /**< Mask for SYSCFG_RAMBIASCONF                 */
#define _SYSCFG_RAMBIASCONF_RAMBIASCTRL_SHIFT              0                                              /**< Shift value for SYSCFG_RAMBIASCTRL          */
#define _SYSCFG_RAMBIASCONF_RAMBIASCTRL_MASK               0xFUL                                          /**< Bit mask for SYSCFG_RAMBIASCTRL             */
#define _SYSCFG_RAMBIASCONF_RAMBIASCTRL_DEFAULT            0x00000002UL                                   /**< Mode DEFAULT for SYSCFG_RAMBIASCONF         */
#define _SYSCFG_RAMBIASCONF_RAMBIASCTRL_No                 0x00000000UL                                   /**< Mode No for SYSCFG_RAMBIASCONF              */
#define _SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB100             0x00000001UL                                   /**< Mode VSB100 for SYSCFG_RAMBIASCONF          */
#define _SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB200             0x00000002UL                                   /**< Mode VSB200 for SYSCFG_RAMBIASCONF          */
#define _SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB300             0x00000004UL                                   /**< Mode VSB300 for SYSCFG_RAMBIASCONF          */
#define _SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB400             0x00000008UL                                   /**< Mode VSB400 for SYSCFG_RAMBIASCONF          */
#define SYSCFG_RAMBIASCONF_RAMBIASCTRL_DEFAULT             (_SYSCFG_RAMBIASCONF_RAMBIASCTRL_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_RAMBIASCONF */
#define SYSCFG_RAMBIASCONF_RAMBIASCTRL_No                  (_SYSCFG_RAMBIASCONF_RAMBIASCTRL_No << 0)      /**< Shifted mode No for SYSCFG_RAMBIASCONF      */
#define SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB100              (_SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB100 << 0)  /**< Shifted mode VSB100 for SYSCFG_RAMBIASCONF  */
#define SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB200              (_SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB200 << 0)  /**< Shifted mode VSB200 for SYSCFG_RAMBIASCONF  */
#define SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB300              (_SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB300 << 0)  /**< Shifted mode VSB300 for SYSCFG_RAMBIASCONF  */
#define SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB400              (_SYSCFG_RAMBIASCONF_RAMBIASCTRL_VSB400 << 0)  /**< Shifted mode VSB400 for SYSCFG_RAMBIASCONF  */

/* Bit fields for SYSCFG RADIORAMRETNCTRL */
#define _SYSCFG_RADIORAMRETNCTRL_RESETVALUE                0x00000000UL                                           /**< Default value for SYSCFG_RADIORAMRETNCTRL   */
#define _SYSCFG_RADIORAMRETNCTRL_MASK                      0x00000103UL                                           /**< Mask for SYSCFG_RADIORAMRETNCTRL            */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_SHIFT      0                                                      /**< Shift value for SYSCFG_SEQRAMRETNCTRL       */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_MASK       0x3UL                                                  /**< Bit mask for SYSCFG_SEQRAMRETNCTRL          */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_DEFAULT    0x00000000UL                                           /**< Mode DEFAULT for SYSCFG_RADIORAMRETNCTRL    */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_ALLON      0x00000000UL                                           /**< Mode ALLON for SYSCFG_RADIORAMRETNCTRL      */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_BLK0       0x00000001UL                                           /**< Mode BLK0 for SYSCFG_RADIORAMRETNCTRL       */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_BLK1       0x00000002UL                                           /**< Mode BLK1 for SYSCFG_RADIORAMRETNCTRL       */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_ALLOFF     0x00000003UL                                           /**< Mode ALLOFF for SYSCFG_RADIORAMRETNCTRL     */
#define SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_DEFAULT     (_SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_ALLON       (_SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_ALLON << 0)   /**< Shifted mode ALLON for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_BLK0        (_SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_BLK0 << 0)    /**< Shifted mode BLK0 for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_BLK1        (_SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_BLK1 << 0)    /**< Shifted mode BLK1 for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_ALLOFF      (_SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_ALLOFF << 0)  /**< Shifted mode ALLOFF for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL             (0x1UL << 8)                                           /**< FRCRAM Retention Control                    */
#define _SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_SHIFT      8                                                      /**< Shift value for SYSCFG_FRCRAMRETNCTRL       */
#define _SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_MASK       0x100UL                                                /**< Bit mask for SYSCFG_FRCRAMRETNCTRL          */
#define _SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_DEFAULT    0x00000000UL                                           /**< Mode DEFAULT for SYSCFG_RADIORAMRETNCTRL    */
#define _SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_ALLON      0x00000000UL                                           /**< Mode ALLON for SYSCFG_RADIORAMRETNCTRL      */
#define _SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_ALLOFF     0x00000001UL                                           /**< Mode ALLOFF for SYSCFG_RADIORAMRETNCTRL     */
#define SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_DEFAULT     (_SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_DEFAULT << 8) /**< Shifted mode DEFAULT for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_ALLON       (_SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_ALLON << 8)   /**< Shifted mode ALLON for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_ALLOFF      (_SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_ALLOFF << 8)  /**< Shifted mode ALLOFF for SYSCFG_RADIORAMRETNCTRL*/

/* Bit fields for SYSCFG RADIOECCCTRL */
#define _SYSCFG_RADIOECCCTRL_RESETVALUE                    0x00000000UL                                      /**< Default value for SYSCFG_RADIOECCCTRL       */
#define _SYSCFG_RADIOECCCTRL_MASK                          0x00000303UL                                      /**< Mask for SYSCFG_RADIOECCCTRL                */
#define SYSCFG_RADIOECCCTRL_SEQRAMECCEN                    (0x1UL << 0)                                      /**< SEQRAM ECC Enable                           */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCEN_SHIFT             0                                                 /**< Shift value for SYSCFG_SEQRAMECCEN          */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCEN_MASK              0x1UL                                             /**< Bit mask for SYSCFG_SEQRAMECCEN             */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCEN_DEFAULT           0x00000000UL                                      /**< Mode DEFAULT for SYSCFG_RADIOECCCTRL        */
#define SYSCFG_RADIOECCCTRL_SEQRAMECCEN_DEFAULT            (_SYSCFG_RADIOECCCTRL_SEQRAMECCEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for SYSCFG_RADIOECCCTRL*/
#define SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN                  (0x1UL << 1)                                      /**< SEQRAM ECC Error Writeback Enable           */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN_SHIFT           1                                                 /**< Shift value for SYSCFG_SEQRAMECCEWEN        */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN_MASK            0x2UL                                             /**< Bit mask for SYSCFG_SEQRAMECCEWEN           */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN_DEFAULT         0x00000000UL                                      /**< Mode DEFAULT for SYSCFG_RADIOECCCTRL        */
#define SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN_DEFAULT          (_SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN_DEFAULT << 1) /**< Shifted mode DEFAULT for SYSCFG_RADIOECCCTRL*/
#define SYSCFG_RADIOECCCTRL_FRCRAMECCEN                    (0x1UL << 8)                                      /**< FRCRAM ECC Enable                           */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCEN_SHIFT             8                                                 /**< Shift value for SYSCFG_FRCRAMECCEN          */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCEN_MASK              0x100UL                                           /**< Bit mask for SYSCFG_FRCRAMECCEN             */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCEN_DEFAULT           0x00000000UL                                      /**< Mode DEFAULT for SYSCFG_RADIOECCCTRL        */
#define SYSCFG_RADIOECCCTRL_FRCRAMECCEN_DEFAULT            (_SYSCFG_RADIOECCCTRL_FRCRAMECCEN_DEFAULT << 8)   /**< Shifted mode DEFAULT for SYSCFG_RADIOECCCTRL*/
#define SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN                  (0x1UL << 9)                                      /**< FRCRAM ECC Error Writeback Enable           */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN_SHIFT           9                                                 /**< Shift value for SYSCFG_FRCRAMECCEWEN        */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN_MASK            0x200UL                                           /**< Bit mask for SYSCFG_FRCRAMECCEWEN           */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN_DEFAULT         0x00000000UL                                      /**< Mode DEFAULT for SYSCFG_RADIOECCCTRL        */
#define SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN_DEFAULT          (_SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN_DEFAULT << 9) /**< Shifted mode DEFAULT for SYSCFG_RADIOECCCTRL*/

/* Bit fields for SYSCFG SEQRAMECCADDR */
#define _SYSCFG_SEQRAMECCADDR_RESETVALUE                   0x00000000UL                                       /**< Default value for SYSCFG_SEQRAMECCADDR      */
#define _SYSCFG_SEQRAMECCADDR_MASK                         0xFFFFFFFFUL                                       /**< Mask for SYSCFG_SEQRAMECCADDR               */
#define _SYSCFG_SEQRAMECCADDR_SEQRAMECCADDR_SHIFT          0                                                  /**< Shift value for SYSCFG_SEQRAMECCADDR        */
#define _SYSCFG_SEQRAMECCADDR_SEQRAMECCADDR_MASK           0xFFFFFFFFUL                                       /**< Bit mask for SYSCFG_SEQRAMECCADDR           */
#define _SYSCFG_SEQRAMECCADDR_SEQRAMECCADDR_DEFAULT        0x00000000UL                                       /**< Mode DEFAULT for SYSCFG_SEQRAMECCADDR       */
#define SYSCFG_SEQRAMECCADDR_SEQRAMECCADDR_DEFAULT         (_SYSCFG_SEQRAMECCADDR_SEQRAMECCADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_SEQRAMECCADDR*/

/* Bit fields for SYSCFG FRCRAMECCADDR */
#define _SYSCFG_FRCRAMECCADDR_RESETVALUE                   0x00000000UL                                       /**< Default value for SYSCFG_FRCRAMECCADDR      */
#define _SYSCFG_FRCRAMECCADDR_MASK                         0xFFFFFFFFUL                                       /**< Mask for SYSCFG_FRCRAMECCADDR               */
#define _SYSCFG_FRCRAMECCADDR_FRCRAMECCADDR_SHIFT          0                                                  /**< Shift value for SYSCFG_FRCRAMECCADDR        */
#define _SYSCFG_FRCRAMECCADDR_FRCRAMECCADDR_MASK           0xFFFFFFFFUL                                       /**< Bit mask for SYSCFG_FRCRAMECCADDR           */
#define _SYSCFG_FRCRAMECCADDR_FRCRAMECCADDR_DEFAULT        0x00000000UL                                       /**< Mode DEFAULT for SYSCFG_FRCRAMECCADDR       */
#define SYSCFG_FRCRAMECCADDR_FRCRAMECCADDR_DEFAULT         (_SYSCFG_FRCRAMECCADDR_FRCRAMECCADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_FRCRAMECCADDR*/

/* Bit fields for SYSCFG ICACHERAMRETNCTRL */
#define _SYSCFG_ICACHERAMRETNCTRL_RESETVALUE               0x00000000UL                                         /**< Default value for SYSCFG_ICACHERAMRETNCTRL  */
#define _SYSCFG_ICACHERAMRETNCTRL_MASK                     0x00000001UL                                         /**< Mask for SYSCFG_ICACHERAMRETNCTRL           */
#define SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL               (0x1UL << 0)                                         /**< ICACHERAM Retention control                 */
#define _SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL_SHIFT        0                                                    /**< Shift value for SYSCFG_RAMRETNCTRL          */
#define _SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL_MASK         0x1UL                                                /**< Bit mask for SYSCFG_RAMRETNCTRL             */
#define _SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL_DEFAULT      0x00000000UL                                         /**< Mode DEFAULT for SYSCFG_ICACHERAMRETNCTRL   */
#define _SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL_ALLON        0x00000000UL                                         /**< Mode ALLON for SYSCFG_ICACHERAMRETNCTRL     */
#define _SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL_ALLOFF       0x00000001UL                                         /**< Mode ALLOFF for SYSCFG_ICACHERAMRETNCTRL    */
#define SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL_DEFAULT       (_SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_ICACHERAMRETNCTRL*/
#define SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL_ALLON         (_SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL_ALLON << 0)   /**< Shifted mode ALLON for SYSCFG_ICACHERAMRETNCTRL*/
#define SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL_ALLOFF        (_SYSCFG_ICACHERAMRETNCTRL_RAMRETNCTRL_ALLOFF << 0)  /**< Shifted mode ALLOFF for SYSCFG_ICACHERAMRETNCTRL*/

/* Bit fields for SYSCFG DMEM0PORTMAPSEL */
#define _SYSCFG_DMEM0PORTMAPSEL_RESETVALUE                 0x00000013UL                                          /**< Default value for SYSCFG_DMEM0PORTMAPSEL    */
#define _SYSCFG_DMEM0PORTMAPSEL_MASK                       0x0000001FUL                                          /**< Mask for SYSCFG_DMEM0PORTMAPSEL             */
#define SYSCFG_DMEM0PORTMAPSEL_LDMAPORTSEL                 (0x1UL << 0)                                          /**< LDMA portmap selection                      */
#define _SYSCFG_DMEM0PORTMAPSEL_LDMAPORTSEL_SHIFT          0                                                     /**< Shift value for SYSCFG_LDMAPORTSEL          */
#define _SYSCFG_DMEM0PORTMAPSEL_LDMAPORTSEL_MASK           0x1UL                                                 /**< Bit mask for SYSCFG_LDMAPORTSEL             */
#define _SYSCFG_DMEM0PORTMAPSEL_LDMAPORTSEL_DEFAULT        0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_DMEM0PORTMAPSEL     */
#define SYSCFG_DMEM0PORTMAPSEL_LDMAPORTSEL_DEFAULT         (_SYSCFG_DMEM0PORTMAPSEL_LDMAPORTSEL_DEFAULT << 0)    /**< Shifted mode DEFAULT for SYSCFG_DMEM0PORTMAPSEL*/
#define SYSCFG_DMEM0PORTMAPSEL_SRWAESPORTSEL               (0x1UL << 1)                                          /**< SRWAES portmap selection                    */
#define _SYSCFG_DMEM0PORTMAPSEL_SRWAESPORTSEL_SHIFT        1                                                     /**< Shift value for SYSCFG_SRWAESPORTSEL        */
#define _SYSCFG_DMEM0PORTMAPSEL_SRWAESPORTSEL_MASK         0x2UL                                                 /**< Bit mask for SYSCFG_SRWAESPORTSEL           */
#define _SYSCFG_DMEM0PORTMAPSEL_SRWAESPORTSEL_DEFAULT      0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_DMEM0PORTMAPSEL     */
#define SYSCFG_DMEM0PORTMAPSEL_SRWAESPORTSEL_DEFAULT       (_SYSCFG_DMEM0PORTMAPSEL_SRWAESPORTSEL_DEFAULT << 1)  /**< Shifted mode DEFAULT for SYSCFG_DMEM0PORTMAPSEL*/
#define SYSCFG_DMEM0PORTMAPSEL_AHBSRWPORTSEL               (0x1UL << 2)                                          /**< AHBSRW portmap selection                    */
#define _SYSCFG_DMEM0PORTMAPSEL_AHBSRWPORTSEL_SHIFT        2                                                     /**< Shift value for SYSCFG_AHBSRWPORTSEL        */
#define _SYSCFG_DMEM0PORTMAPSEL_AHBSRWPORTSEL_MASK         0x4UL                                                 /**< Bit mask for SYSCFG_AHBSRWPORTSEL           */
#define _SYSCFG_DMEM0PORTMAPSEL_AHBSRWPORTSEL_DEFAULT      0x00000000UL                                          /**< Mode DEFAULT for SYSCFG_DMEM0PORTMAPSEL     */
#define SYSCFG_DMEM0PORTMAPSEL_AHBSRWPORTSEL_DEFAULT       (_SYSCFG_DMEM0PORTMAPSEL_AHBSRWPORTSEL_DEFAULT << 2)  /**< Shifted mode DEFAULT for SYSCFG_DMEM0PORTMAPSEL*/
#define SYSCFG_DMEM0PORTMAPSEL_SRWECA0PORTSEL              (0x1UL << 3)                                          /**< SRWECA0 portmap selection                   */
#define _SYSCFG_DMEM0PORTMAPSEL_SRWECA0PORTSEL_SHIFT       3                                                     /**< Shift value for SYSCFG_SRWECA0PORTSEL       */
#define _SYSCFG_DMEM0PORTMAPSEL_SRWECA0PORTSEL_MASK        0x8UL                                                 /**< Bit mask for SYSCFG_SRWECA0PORTSEL          */
#define _SYSCFG_DMEM0PORTMAPSEL_SRWECA0PORTSEL_DEFAULT     0x00000000UL                                          /**< Mode DEFAULT for SYSCFG_DMEM0PORTMAPSEL     */
#define SYSCFG_DMEM0PORTMAPSEL_SRWECA0PORTSEL_DEFAULT      (_SYSCFG_DMEM0PORTMAPSEL_SRWECA0PORTSEL_DEFAULT << 3) /**< Shifted mode DEFAULT for SYSCFG_DMEM0PORTMAPSEL*/
#define SYSCFG_DMEM0PORTMAPSEL_SRWECA1PORTSEL              (0x1UL << 4)                                          /**< SRWECA1 portmap selection                   */
#define _SYSCFG_DMEM0PORTMAPSEL_SRWECA1PORTSEL_SHIFT       4                                                     /**< Shift value for SYSCFG_SRWECA1PORTSEL       */
#define _SYSCFG_DMEM0PORTMAPSEL_SRWECA1PORTSEL_MASK        0x10UL                                                /**< Bit mask for SYSCFG_SRWECA1PORTSEL          */
#define _SYSCFG_DMEM0PORTMAPSEL_SRWECA1PORTSEL_DEFAULT     0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_DMEM0PORTMAPSEL     */
#define SYSCFG_DMEM0PORTMAPSEL_SRWECA1PORTSEL_DEFAULT      (_SYSCFG_DMEM0PORTMAPSEL_SRWECA1PORTSEL_DEFAULT << 4) /**< Shifted mode DEFAULT for SYSCFG_DMEM0PORTMAPSEL*/

/* Bit fields for SYSCFG ROOTDATA0 */
#define _SYSCFG_ROOTDATA0_RESETVALUE                       0x00000000UL                          /**< Default value for SYSCFG_ROOTDATA0          */
#define _SYSCFG_ROOTDATA0_MASK                             0xFFFFFFFFUL                          /**< Mask for SYSCFG_ROOTDATA0                   */
#define _SYSCFG_ROOTDATA0_DATA_SHIFT                       0                                     /**< Shift value for SYSCFG_DATA                 */
#define _SYSCFG_ROOTDATA0_DATA_MASK                        0xFFFFFFFFUL                          /**< Bit mask for SYSCFG_DATA                    */
#define _SYSCFG_ROOTDATA0_DATA_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for SYSCFG_ROOTDATA0           */
#define SYSCFG_ROOTDATA0_DATA_DEFAULT                      (_SYSCFG_ROOTDATA0_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_ROOTDATA0   */

/* Bit fields for SYSCFG ROOTDATA1 */
#define _SYSCFG_ROOTDATA1_RESETVALUE                       0x00000000UL                          /**< Default value for SYSCFG_ROOTDATA1          */
#define _SYSCFG_ROOTDATA1_MASK                             0xFFFFFFFFUL                          /**< Mask for SYSCFG_ROOTDATA1                   */
#define _SYSCFG_ROOTDATA1_DATA_SHIFT                       0                                     /**< Shift value for SYSCFG_DATA                 */
#define _SYSCFG_ROOTDATA1_DATA_MASK                        0xFFFFFFFFUL                          /**< Bit mask for SYSCFG_DATA                    */
#define _SYSCFG_ROOTDATA1_DATA_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for SYSCFG_ROOTDATA1           */
#define SYSCFG_ROOTDATA1_DATA_DEFAULT                      (_SYSCFG_ROOTDATA1_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_ROOTDATA1   */

/* Bit fields for SYSCFG ROOTLOCKSTATUS */
#define _SYSCFG_ROOTLOCKSTATUS_RESETVALUE                  0x007F0107UL                                          /**< Default value for SYSCFG_ROOTLOCKSTATUS     */
#define _SYSCFG_ROOTLOCKSTATUS_MASK                        0x807F0107UL                                          /**< Mask for SYSCFG_ROOTLOCKSTATUS              */
#define SYSCFG_ROOTLOCKSTATUS_BUSLOCK                      (0x1UL << 0)                                          /**< Bus Lock                                    */
#define _SYSCFG_ROOTLOCKSTATUS_BUSLOCK_SHIFT               0                                                     /**< Shift value for SYSCFG_BUSLOCK              */
#define _SYSCFG_ROOTLOCKSTATUS_BUSLOCK_MASK                0x1UL                                                 /**< Bit mask for SYSCFG_BUSLOCK                 */
#define _SYSCFG_ROOTLOCKSTATUS_BUSLOCK_DEFAULT             0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_BUSLOCK_DEFAULT              (_SYSCFG_ROOTLOCKSTATUS_BUSLOCK_DEFAULT << 0)         /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_REGLOCK                      (0x1UL << 1)                                          /**< Register Lock                               */
#define _SYSCFG_ROOTLOCKSTATUS_REGLOCK_SHIFT               1                                                     /**< Shift value for SYSCFG_REGLOCK              */
#define _SYSCFG_ROOTLOCKSTATUS_REGLOCK_MASK                0x2UL                                                 /**< Bit mask for SYSCFG_REGLOCK                 */
#define _SYSCFG_ROOTLOCKSTATUS_REGLOCK_DEFAULT             0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_REGLOCK_DEFAULT              (_SYSCFG_ROOTLOCKSTATUS_REGLOCK_DEFAULT << 1)         /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_MFRLOCK                      (0x1UL << 2)                                          /**< Manufacture Lock                            */
#define _SYSCFG_ROOTLOCKSTATUS_MFRLOCK_SHIFT               2                                                     /**< Shift value for SYSCFG_MFRLOCK              */
#define _SYSCFG_ROOTLOCKSTATUS_MFRLOCK_MASK                0x4UL                                                 /**< Bit mask for SYSCFG_MFRLOCK                 */
#define _SYSCFG_ROOTLOCKSTATUS_MFRLOCK_DEFAULT             0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_MFRLOCK_DEFAULT              (_SYSCFG_ROOTLOCKSTATUS_MFRLOCK_DEFAULT << 2)         /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK                  (0x1UL << 8)                                          /**< Root Debug Lock                             */
#define _SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK_SHIFT           8                                                     /**< Shift value for SYSCFG_ROOTDBGLOCK          */
#define _SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK_MASK            0x100UL                                               /**< Bit mask for SYSCFG_ROOTDBGLOCK             */
#define _SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK_DEFAULT         0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK_DEFAULT          (_SYSCFG_ROOTLOCKSTATUS_ROOTDBGLOCK_DEFAULT << 8)     /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK                (0x1UL << 16)                                         /**< User Debug Access Port Lock                 */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK_SHIFT         16                                                    /**< Shift value for SYSCFG_USERDBGAPLOCK        */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK_MASK          0x10000UL                                             /**< Bit mask for SYSCFG_USERDBGAPLOCK           */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK_DEFAULT       0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK_DEFAULT        (_SYSCFG_ROOTLOCKSTATUS_USERDBGAPLOCK_DEFAULT << 16)  /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK                  (0x1UL << 17)                                         /**< User Invasive Debug Lock                    */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK_SHIFT           17                                                    /**< Shift value for SYSCFG_USERDBGLOCK          */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK_MASK            0x20000UL                                             /**< Bit mask for SYSCFG_USERDBGLOCK             */
#define _SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK_DEFAULT         0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK_DEFAULT          (_SYSCFG_ROOTLOCKSTATUS_USERDBGLOCK_DEFAULT << 17)    /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK                  (0x1UL << 18)                                         /**< User Non-invasive Debug Lock                */
#define _SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK_SHIFT           18                                                    /**< Shift value for SYSCFG_USERNIDLOCK          */
#define _SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK_MASK            0x40000UL                                             /**< Bit mask for SYSCFG_USERNIDLOCK             */
#define _SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK_DEFAULT         0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK_DEFAULT          (_SYSCFG_ROOTLOCKSTATUS_USERNIDLOCK_DEFAULT << 18)    /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK                 (0x1UL << 19)                                         /**< User Secure Invasive Debug Lock             */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK_SHIFT          19                                                    /**< Shift value for SYSCFG_USERSPIDLOCK         */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK_MASK           0x80000UL                                             /**< Bit mask for SYSCFG_USERSPIDLOCK            */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK_DEFAULT        0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK_DEFAULT         (_SYSCFG_ROOTLOCKSTATUS_USERSPIDLOCK_DEFAULT << 19)   /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK                (0x1UL << 20)                                         /**< User Secure Non-invasive Debug Lock         */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK_SHIFT         20                                                    /**< Shift value for SYSCFG_USERSPNIDLOCK        */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK_MASK          0x100000UL                                            /**< Bit mask for SYSCFG_USERSPNIDLOCK           */
#define _SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK_DEFAULT       0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK_DEFAULT        (_SYSCFG_ROOTLOCKSTATUS_USERSPNIDLOCK_DEFAULT << 20)  /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_RADIOIDBGLOCK                (0x1UL << 21)                                         /**< Radio Invasive Debug Lock                   */
#define _SYSCFG_ROOTLOCKSTATUS_RADIOIDBGLOCK_SHIFT         21                                                    /**< Shift value for SYSCFG_RADIOIDBGLOCK        */
#define _SYSCFG_ROOTLOCKSTATUS_RADIOIDBGLOCK_MASK          0x200000UL                                            /**< Bit mask for SYSCFG_RADIOIDBGLOCK           */
#define _SYSCFG_ROOTLOCKSTATUS_RADIOIDBGLOCK_DEFAULT       0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_RADIOIDBGLOCK_DEFAULT        (_SYSCFG_ROOTLOCKSTATUS_RADIOIDBGLOCK_DEFAULT << 21)  /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_RADIONIDBGLOCK               (0x1UL << 22)                                         /**< Radio Non-invasive Debug Lock               */
#define _SYSCFG_ROOTLOCKSTATUS_RADIONIDBGLOCK_SHIFT        22                                                    /**< Shift value for SYSCFG_RADIONIDBGLOCK       */
#define _SYSCFG_ROOTLOCKSTATUS_RADIONIDBGLOCK_MASK         0x400000UL                                            /**< Bit mask for SYSCFG_RADIONIDBGLOCK          */
#define _SYSCFG_ROOTLOCKSTATUS_RADIONIDBGLOCK_DEFAULT      0x00000001UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_RADIONIDBGLOCK_DEFAULT       (_SYSCFG_ROOTLOCKSTATUS_RADIONIDBGLOCK_DEFAULT << 22) /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/
#define SYSCFG_ROOTLOCKSTATUS_EFUSEUNLOCKED                (0x1UL << 31)                                         /**< E-Fuse Unlocked                             */
#define _SYSCFG_ROOTLOCKSTATUS_EFUSEUNLOCKED_SHIFT         31                                                    /**< Shift value for SYSCFG_EFUSEUNLOCKED        */
#define _SYSCFG_ROOTLOCKSTATUS_EFUSEUNLOCKED_MASK          0x80000000UL                                          /**< Bit mask for SYSCFG_EFUSEUNLOCKED           */
#define _SYSCFG_ROOTLOCKSTATUS_EFUSEUNLOCKED_DEFAULT       0x00000000UL                                          /**< Mode DEFAULT for SYSCFG_ROOTLOCKSTATUS      */
#define SYSCFG_ROOTLOCKSTATUS_EFUSEUNLOCKED_DEFAULT        (_SYSCFG_ROOTLOCKSTATUS_EFUSEUNLOCKED_DEFAULT << 31)  /**< Shifted mode DEFAULT for SYSCFG_ROOTLOCKSTATUS*/

/* Bit fields for SYSCFG ROOTSESWVERSION */
#define _SYSCFG_ROOTSESWVERSION_RESETVALUE                 0x00000000UL                                     /**< Default value for SYSCFG_ROOTSESWVERSION    */
#define _SYSCFG_ROOTSESWVERSION_MASK                       0xFFFFFFFFUL                                     /**< Mask for SYSCFG_ROOTSESWVERSION             */
#define _SYSCFG_ROOTSESWVERSION_SWVERSION_SHIFT            0                                                /**< Shift value for SYSCFG_SWVERSION            */
#define _SYSCFG_ROOTSESWVERSION_SWVERSION_MASK             0xFFFFFFFFUL                                     /**< Bit mask for SYSCFG_SWVERSION               */
#define _SYSCFG_ROOTSESWVERSION_SWVERSION_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for SYSCFG_ROOTSESWVERSION     */
#define SYSCFG_ROOTSESWVERSION_SWVERSION_DEFAULT           (_SYSCFG_ROOTSESWVERSION_SWVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_ROOTSESWVERSION*/

/** @} End of group EFR32ZG23_SYSCFG_BitFields */
/** @} End of group EFR32ZG23_SYSCFG */
/**************************************************************************//**
 * @defgroup EFR32ZG23_SYSCFG_CFGNS SYSCFG_CFGNS
 * @{
 * @brief EFR32ZG23 SYSCFG_CFGNS Register Declaration.
 *****************************************************************************/

/** SYSCFG_CFGNS Register Declaration. */
typedef struct {
  uint32_t       RESERVED0[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFGNSTCALIB;                   /**< Configure Non-Secure Sys-Tick cal.                 */
  uint32_t       RESERVED1[376U];               /**< Reserved for future use                            */
  __IOM uint32_t ROOTNSDATA0;                   /**< Data Register 0                                    */
  __IOM uint32_t ROOTNSDATA1;                   /**< Data Register 1                                    */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED3[637U];               /**< Reserved for future use                            */
  uint32_t       RESERVED4[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFGNSTCALIB_SET;               /**< Configure Non-Secure Sys-Tick cal.                 */
  uint32_t       RESERVED5[376U];               /**< Reserved for future use                            */
  __IOM uint32_t ROOTNSDATA0_SET;               /**< Data Register 0                                    */
  __IOM uint32_t ROOTNSDATA1_SET;               /**< Data Register 1                                    */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED7[637U];               /**< Reserved for future use                            */
  uint32_t       RESERVED8[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFGNSTCALIB_CLR;               /**< Configure Non-Secure Sys-Tick cal.                 */
  uint32_t       RESERVED9[376U];               /**< Reserved for future use                            */
  __IOM uint32_t ROOTNSDATA0_CLR;               /**< Data Register 0                                    */
  __IOM uint32_t ROOTNSDATA1_CLR;               /**< Data Register 1                                    */
  uint32_t       RESERVED10[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED11[637U];              /**< Reserved for future use                            */
  uint32_t       RESERVED12[7U];                /**< Reserved for future use                            */
  __IOM uint32_t CFGNSTCALIB_TGL;               /**< Configure Non-Secure Sys-Tick cal.                 */
  uint32_t       RESERVED13[376U];              /**< Reserved for future use                            */
  __IOM uint32_t ROOTNSDATA0_TGL;               /**< Data Register 0                                    */
  __IOM uint32_t ROOTNSDATA1_TGL;               /**< Data Register 1                                    */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
} SYSCFG_CFGNS_TypeDef;
/** @} End of group EFR32ZG23_SYSCFG_CFGNS */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_SYSCFG_CFGNS
 * @{
 * @defgroup EFR32ZG23_SYSCFG_CFGNS_BitFields SYSCFG_CFGNS Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SYSCFG CFGNSTCALIB */
#define _SYSCFG_CFGNSTCALIB_RESETVALUE       0x01004A37UL                               /**< Default value for SYSCFG_CFGNSTCALIB        */
#define _SYSCFG_CFGNSTCALIB_MASK             0x03FFFFFFUL                               /**< Mask for SYSCFG_CFGNSTCALIB                 */
#define _SYSCFG_CFGNSTCALIB_TENMS_SHIFT      0                                          /**< Shift value for SYSCFG_TENMS                */
#define _SYSCFG_CFGNSTCALIB_TENMS_MASK       0xFFFFFFUL                                 /**< Bit mask for SYSCFG_TENMS                   */
#define _SYSCFG_CFGNSTCALIB_TENMS_DEFAULT    0x00004A37UL                               /**< Mode DEFAULT for SYSCFG_CFGNSTCALIB         */
#define SYSCFG_CFGNSTCALIB_TENMS_DEFAULT     (_SYSCFG_CFGNSTCALIB_TENMS_DEFAULT << 0)   /**< Shifted mode DEFAULT for SYSCFG_CFGNSTCALIB */
#define SYSCFG_CFGNSTCALIB_SKEW              (0x1UL << 24)                              /**< Skew                                        */
#define _SYSCFG_CFGNSTCALIB_SKEW_SHIFT       24                                         /**< Shift value for SYSCFG_SKEW                 */
#define _SYSCFG_CFGNSTCALIB_SKEW_MASK        0x1000000UL                                /**< Bit mask for SYSCFG_SKEW                    */
#define _SYSCFG_CFGNSTCALIB_SKEW_DEFAULT     0x00000001UL                               /**< Mode DEFAULT for SYSCFG_CFGNSTCALIB         */
#define SYSCFG_CFGNSTCALIB_SKEW_DEFAULT      (_SYSCFG_CFGNSTCALIB_SKEW_DEFAULT << 24)   /**< Shifted mode DEFAULT for SYSCFG_CFGNSTCALIB */
#define SYSCFG_CFGNSTCALIB_NOREF             (0x1UL << 25)                              /**< No Reference                                */
#define _SYSCFG_CFGNSTCALIB_NOREF_SHIFT      25                                         /**< Shift value for SYSCFG_NOREF                */
#define _SYSCFG_CFGNSTCALIB_NOREF_MASK       0x2000000UL                                /**< Bit mask for SYSCFG_NOREF                   */
#define _SYSCFG_CFGNSTCALIB_NOREF_DEFAULT    0x00000000UL                               /**< Mode DEFAULT for SYSCFG_CFGNSTCALIB         */
#define _SYSCFG_CFGNSTCALIB_NOREF_REF        0x00000000UL                               /**< Mode REF for SYSCFG_CFGNSTCALIB             */
#define _SYSCFG_CFGNSTCALIB_NOREF_NOREF      0x00000001UL                               /**< Mode NOREF for SYSCFG_CFGNSTCALIB           */
#define SYSCFG_CFGNSTCALIB_NOREF_DEFAULT     (_SYSCFG_CFGNSTCALIB_NOREF_DEFAULT << 25)  /**< Shifted mode DEFAULT for SYSCFG_CFGNSTCALIB */
#define SYSCFG_CFGNSTCALIB_NOREF_REF         (_SYSCFG_CFGNSTCALIB_NOREF_REF << 25)      /**< Shifted mode REF for SYSCFG_CFGNSTCALIB     */
#define SYSCFG_CFGNSTCALIB_NOREF_NOREF       (_SYSCFG_CFGNSTCALIB_NOREF_NOREF << 25)    /**< Shifted mode NOREF for SYSCFG_CFGNSTCALIB   */

/* Bit fields for SYSCFG ROOTNSDATA0 */
#define _SYSCFG_ROOTNSDATA0_RESETVALUE       0x00000000UL                               /**< Default value for SYSCFG_ROOTNSDATA0        */
#define _SYSCFG_ROOTNSDATA0_MASK             0xFFFFFFFFUL                               /**< Mask for SYSCFG_ROOTNSDATA0                 */
#define _SYSCFG_ROOTNSDATA0_DATA_SHIFT       0                                          /**< Shift value for SYSCFG_DATA                 */
#define _SYSCFG_ROOTNSDATA0_DATA_MASK        0xFFFFFFFFUL                               /**< Bit mask for SYSCFG_DATA                    */
#define _SYSCFG_ROOTNSDATA0_DATA_DEFAULT     0x00000000UL                               /**< Mode DEFAULT for SYSCFG_ROOTNSDATA0         */
#define SYSCFG_ROOTNSDATA0_DATA_DEFAULT      (_SYSCFG_ROOTNSDATA0_DATA_DEFAULT << 0)    /**< Shifted mode DEFAULT for SYSCFG_ROOTNSDATA0 */

/* Bit fields for SYSCFG ROOTNSDATA1 */
#define _SYSCFG_ROOTNSDATA1_RESETVALUE       0x00000000UL                               /**< Default value for SYSCFG_ROOTNSDATA1        */
#define _SYSCFG_ROOTNSDATA1_MASK             0xFFFFFFFFUL                               /**< Mask for SYSCFG_ROOTNSDATA1                 */
#define _SYSCFG_ROOTNSDATA1_DATA_SHIFT       0                                          /**< Shift value for SYSCFG_DATA                 */
#define _SYSCFG_ROOTNSDATA1_DATA_MASK        0xFFFFFFFFUL                               /**< Bit mask for SYSCFG_DATA                    */
#define _SYSCFG_ROOTNSDATA1_DATA_DEFAULT     0x00000000UL                               /**< Mode DEFAULT for SYSCFG_ROOTNSDATA1         */
#define SYSCFG_ROOTNSDATA1_DATA_DEFAULT      (_SYSCFG_ROOTNSDATA1_DATA_DEFAULT << 0)    /**< Shifted mode DEFAULT for SYSCFG_ROOTNSDATA1 */

/** @} End of group EFR32ZG23_SYSCFG_CFGNS_BitFields */
/** @} End of group EFR32ZG23_SYSCFG_CFGNS */
/** @} End of group Parts */

#endif /* EFR32ZG23_SYSCFG_H */
#ifdef __cplusplus
}
#endif

