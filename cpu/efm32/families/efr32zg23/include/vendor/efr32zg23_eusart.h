/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 EUSART register and bit field definitions
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

#ifndef EFR32ZG23_EUSART_H
#define EFR32ZG23_EUSART_H
#define EUSART_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_EUSART EUSART
 * @{
 * @brief EFR32ZG23 EUSART Register Declaration.
 *****************************************************************************/

/** EUSART Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  __IOM uint32_t EN;                            /**< Enable Register                                    */
  __IOM uint32_t CFG0;                          /**< Configuration 0 Register                           */
  __IOM uint32_t CFG1;                          /**< Configuration 1 Register                           */
  __IOM uint32_t CFG2;                          /**< Configuration 2 Register                           */
  __IOM uint32_t FRAMECFG;                      /**< Frame Format Register                              */
  __IOM uint32_t DTXDATCFG;                     /**< Default TX DATA Register                           */
  __IOM uint32_t IRHFCFG;                       /**< HF IrDA Mod Config Register                        */
  __IOM uint32_t IRLFCFG;                       /**< LF IrDA Pulse Config Register                      */
  __IOM uint32_t TIMINGCFG;                     /**< Timing Register                                    */
  __IOM uint32_t STARTFRAMECFG;                 /**< Start Frame Register                               */
  __IOM uint32_t SIGFRAMECFG;                   /**< Signal Frame Register                              */
  __IOM uint32_t CLKDIV;                        /**< Clock Divider Register                             */
  __IOM uint32_t TRIGCTRL;                      /**< Trigger Control Register                           */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  __IM uint32_t  RXDATA;                        /**< RX Data Register                                   */
  __IM uint32_t  RXDATAP;                       /**< RX Data Peek Register                              */
  __IOM uint32_t TXDATA;                        /**< TX Data Register                                   */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY;                      /**< Synchronization Busy Register                      */
  uint32_t       RESERVED0[42U];                /**< Reserved for future use                            */
  uint32_t       RESERVED1[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED2[959U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  __IOM uint32_t EN_SET;                        /**< Enable Register                                    */
  __IOM uint32_t CFG0_SET;                      /**< Configuration 0 Register                           */
  __IOM uint32_t CFG1_SET;                      /**< Configuration 1 Register                           */
  __IOM uint32_t CFG2_SET;                      /**< Configuration 2 Register                           */
  __IOM uint32_t FRAMECFG_SET;                  /**< Frame Format Register                              */
  __IOM uint32_t DTXDATCFG_SET;                 /**< Default TX DATA Register                           */
  __IOM uint32_t IRHFCFG_SET;                   /**< HF IrDA Mod Config Register                        */
  __IOM uint32_t IRLFCFG_SET;                   /**< LF IrDA Pulse Config Register                      */
  __IOM uint32_t TIMINGCFG_SET;                 /**< Timing Register                                    */
  __IOM uint32_t STARTFRAMECFG_SET;             /**< Start Frame Register                               */
  __IOM uint32_t SIGFRAMECFG_SET;               /**< Signal Frame Register                              */
  __IOM uint32_t CLKDIV_SET;                    /**< Clock Divider Register                             */
  __IOM uint32_t TRIGCTRL_SET;                  /**< Trigger Control Register                           */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  __IM uint32_t  RXDATA_SET;                    /**< RX Data Register                                   */
  __IM uint32_t  RXDATAP_SET;                   /**< RX Data Peek Register                              */
  __IOM uint32_t TXDATA_SET;                    /**< TX Data Register                                   */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY_SET;                  /**< Synchronization Busy Register                      */
  uint32_t       RESERVED3[42U];                /**< Reserved for future use                            */
  uint32_t       RESERVED4[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED5[959U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  __IOM uint32_t EN_CLR;                        /**< Enable Register                                    */
  __IOM uint32_t CFG0_CLR;                      /**< Configuration 0 Register                           */
  __IOM uint32_t CFG1_CLR;                      /**< Configuration 1 Register                           */
  __IOM uint32_t CFG2_CLR;                      /**< Configuration 2 Register                           */
  __IOM uint32_t FRAMECFG_CLR;                  /**< Frame Format Register                              */
  __IOM uint32_t DTXDATCFG_CLR;                 /**< Default TX DATA Register                           */
  __IOM uint32_t IRHFCFG_CLR;                   /**< HF IrDA Mod Config Register                        */
  __IOM uint32_t IRLFCFG_CLR;                   /**< LF IrDA Pulse Config Register                      */
  __IOM uint32_t TIMINGCFG_CLR;                 /**< Timing Register                                    */
  __IOM uint32_t STARTFRAMECFG_CLR;             /**< Start Frame Register                               */
  __IOM uint32_t SIGFRAMECFG_CLR;               /**< Signal Frame Register                              */
  __IOM uint32_t CLKDIV_CLR;                    /**< Clock Divider Register                             */
  __IOM uint32_t TRIGCTRL_CLR;                  /**< Trigger Control Register                           */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  __IM uint32_t  RXDATA_CLR;                    /**< RX Data Register                                   */
  __IM uint32_t  RXDATAP_CLR;                   /**< RX Data Peek Register                              */
  __IOM uint32_t TXDATA_CLR;                    /**< TX Data Register                                   */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY_CLR;                  /**< Synchronization Busy Register                      */
  uint32_t       RESERVED6[42U];                /**< Reserved for future use                            */
  uint32_t       RESERVED7[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED8[959U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  __IOM uint32_t EN_TGL;                        /**< Enable Register                                    */
  __IOM uint32_t CFG0_TGL;                      /**< Configuration 0 Register                           */
  __IOM uint32_t CFG1_TGL;                      /**< Configuration 1 Register                           */
  __IOM uint32_t CFG2_TGL;                      /**< Configuration 2 Register                           */
  __IOM uint32_t FRAMECFG_TGL;                  /**< Frame Format Register                              */
  __IOM uint32_t DTXDATCFG_TGL;                 /**< Default TX DATA Register                           */
  __IOM uint32_t IRHFCFG_TGL;                   /**< HF IrDA Mod Config Register                        */
  __IOM uint32_t IRLFCFG_TGL;                   /**< LF IrDA Pulse Config Register                      */
  __IOM uint32_t TIMINGCFG_TGL;                 /**< Timing Register                                    */
  __IOM uint32_t STARTFRAMECFG_TGL;             /**< Start Frame Register                               */
  __IOM uint32_t SIGFRAMECFG_TGL;               /**< Signal Frame Register                              */
  __IOM uint32_t CLKDIV_TGL;                    /**< Clock Divider Register                             */
  __IOM uint32_t TRIGCTRL_TGL;                  /**< Trigger Control Register                           */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  __IM uint32_t  RXDATA_TGL;                    /**< RX Data Register                                   */
  __IM uint32_t  RXDATAP_TGL;                   /**< RX Data Peek Register                              */
  __IOM uint32_t TXDATA_TGL;                    /**< TX Data Register                                   */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY_TGL;                  /**< Synchronization Busy Register                      */
  uint32_t       RESERVED9[42U];                /**< Reserved for future use                            */
  uint32_t       RESERVED10[1U];                /**< Reserved for future use                            */
} EUSART_TypeDef;
/** @} End of group EFR32ZG23_EUSART */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_EUSART
 * @{
 * @defgroup EFR32ZG23_EUSART_BitFields EUSART Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for EUSART IPVERSION */
#define _EUSART_IPVERSION_RESETVALUE                0x00000001UL                               /**< Default value for EUSART_IPVERSION          */
#define _EUSART_IPVERSION_MASK                      0xFFFFFFFFUL                               /**< Mask for EUSART_IPVERSION                   */
#define _EUSART_IPVERSION_IPVERSION_SHIFT           0                                          /**< Shift value for EUSART_IPVERSION            */
#define _EUSART_IPVERSION_IPVERSION_MASK            0xFFFFFFFFUL                               /**< Bit mask for EUSART_IPVERSION               */
#define _EUSART_IPVERSION_IPVERSION_DEFAULT         0x00000001UL                               /**< Mode DEFAULT for EUSART_IPVERSION           */
#define EUSART_IPVERSION_IPVERSION_DEFAULT          (_EUSART_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for EUSART_IPVERSION   */

/* Bit fields for EUSART EN */
#define _EUSART_EN_RESETVALUE                       0x00000000UL                        /**< Default value for EUSART_EN                 */
#define _EUSART_EN_MASK                             0x00000003UL                        /**< Mask for EUSART_EN                          */
#define EUSART_EN_EN                                (0x1UL << 0)                        /**< Module enable                               */
#define _EUSART_EN_EN_SHIFT                         0                                   /**< Shift value for EUSART_EN                   */
#define _EUSART_EN_EN_MASK                          0x1UL                               /**< Bit mask for EUSART_EN                      */
#define _EUSART_EN_EN_DEFAULT                       0x00000000UL                        /**< Mode DEFAULT for EUSART_EN                  */
#define EUSART_EN_EN_DEFAULT                        (_EUSART_EN_EN_DEFAULT << 0)        /**< Shifted mode DEFAULT for EUSART_EN          */
#define EUSART_EN_DISABLING                         (0x1UL << 1)                        /**< Disablement busy status                     */
#define _EUSART_EN_DISABLING_SHIFT                  1                                   /**< Shift value for EUSART_DISABLING            */
#define _EUSART_EN_DISABLING_MASK                   0x2UL                               /**< Bit mask for EUSART_DISABLING               */
#define _EUSART_EN_DISABLING_DEFAULT                0x00000000UL                        /**< Mode DEFAULT for EUSART_EN                  */
#define EUSART_EN_DISABLING_DEFAULT                 (_EUSART_EN_DISABLING_DEFAULT << 1) /**< Shifted mode DEFAULT for EUSART_EN          */

/* Bit fields for EUSART CFG0 */
#define _EUSART_CFG0_RESETVALUE                     0x00000000UL                            /**< Default value for EUSART_CFG0               */
#define _EUSART_CFG0_MASK                           0xC1D264FFUL                            /**< Mask for EUSART_CFG0                        */
#define EUSART_CFG0_SYNC                            (0x1UL << 0)                            /**< Synchronous Mode                            */
#define _EUSART_CFG0_SYNC_SHIFT                     0                                       /**< Shift value for EUSART_SYNC                 */
#define _EUSART_CFG0_SYNC_MASK                      0x1UL                                   /**< Bit mask for EUSART_SYNC                    */
#define _EUSART_CFG0_SYNC_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_SYNC_ASYNC                     0x00000000UL                            /**< Mode ASYNC for EUSART_CFG0                  */
#define _EUSART_CFG0_SYNC_SYNC                      0x00000001UL                            /**< Mode SYNC for EUSART_CFG0                   */
#define EUSART_CFG0_SYNC_DEFAULT                    (_EUSART_CFG0_SYNC_DEFAULT << 0)        /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_SYNC_ASYNC                      (_EUSART_CFG0_SYNC_ASYNC << 0)          /**< Shifted mode ASYNC for EUSART_CFG0          */
#define EUSART_CFG0_SYNC_SYNC                       (_EUSART_CFG0_SYNC_SYNC << 0)           /**< Shifted mode SYNC for EUSART_CFG0           */
#define EUSART_CFG0_LOOPBK                          (0x1UL << 1)                            /**< Loopback Enable                             */
#define _EUSART_CFG0_LOOPBK_SHIFT                   1                                       /**< Shift value for EUSART_LOOPBK               */
#define _EUSART_CFG0_LOOPBK_MASK                    0x2UL                                   /**< Bit mask for EUSART_LOOPBK                  */
#define _EUSART_CFG0_LOOPBK_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_LOOPBK_DISABLE                 0x00000000UL                            /**< Mode DISABLE for EUSART_CFG0                */
#define _EUSART_CFG0_LOOPBK_ENABLE                  0x00000001UL                            /**< Mode ENABLE for EUSART_CFG0                 */
#define EUSART_CFG0_LOOPBK_DEFAULT                  (_EUSART_CFG0_LOOPBK_DEFAULT << 1)      /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_LOOPBK_DISABLE                  (_EUSART_CFG0_LOOPBK_DISABLE << 1)      /**< Shifted mode DISABLE for EUSART_CFG0        */
#define EUSART_CFG0_LOOPBK_ENABLE                   (_EUSART_CFG0_LOOPBK_ENABLE << 1)       /**< Shifted mode ENABLE for EUSART_CFG0         */
#define EUSART_CFG0_CCEN                            (0x1UL << 2)                            /**< Collision Check Enable                      */
#define _EUSART_CFG0_CCEN_SHIFT                     2                                       /**< Shift value for EUSART_CCEN                 */
#define _EUSART_CFG0_CCEN_MASK                      0x4UL                                   /**< Bit mask for EUSART_CCEN                    */
#define _EUSART_CFG0_CCEN_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_CCEN_DISABLE                   0x00000000UL                            /**< Mode DISABLE for EUSART_CFG0                */
#define _EUSART_CFG0_CCEN_ENABLE                    0x00000001UL                            /**< Mode ENABLE for EUSART_CFG0                 */
#define EUSART_CFG0_CCEN_DEFAULT                    (_EUSART_CFG0_CCEN_DEFAULT << 2)        /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_CCEN_DISABLE                    (_EUSART_CFG0_CCEN_DISABLE << 2)        /**< Shifted mode DISABLE for EUSART_CFG0        */
#define EUSART_CFG0_CCEN_ENABLE                     (_EUSART_CFG0_CCEN_ENABLE << 2)         /**< Shifted mode ENABLE for EUSART_CFG0         */
#define EUSART_CFG0_MPM                             (0x1UL << 3)                            /**< Multi-Processor Mode                        */
#define _EUSART_CFG0_MPM_SHIFT                      3                                       /**< Shift value for EUSART_MPM                  */
#define _EUSART_CFG0_MPM_MASK                       0x8UL                                   /**< Bit mask for EUSART_MPM                     */
#define _EUSART_CFG0_MPM_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_MPM_DISABLE                    0x00000000UL                            /**< Mode DISABLE for EUSART_CFG0                */
#define _EUSART_CFG0_MPM_ENABLE                     0x00000001UL                            /**< Mode ENABLE for EUSART_CFG0                 */
#define EUSART_CFG0_MPM_DEFAULT                     (_EUSART_CFG0_MPM_DEFAULT << 3)         /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_MPM_DISABLE                     (_EUSART_CFG0_MPM_DISABLE << 3)         /**< Shifted mode DISABLE for EUSART_CFG0        */
#define EUSART_CFG0_MPM_ENABLE                      (_EUSART_CFG0_MPM_ENABLE << 3)          /**< Shifted mode ENABLE for EUSART_CFG0         */
#define EUSART_CFG0_MPAB                            (0x1UL << 4)                            /**< Multi-Processor Address-Bit                 */
#define _EUSART_CFG0_MPAB_SHIFT                     4                                       /**< Shift value for EUSART_MPAB                 */
#define _EUSART_CFG0_MPAB_MASK                      0x10UL                                  /**< Bit mask for EUSART_MPAB                    */
#define _EUSART_CFG0_MPAB_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define EUSART_CFG0_MPAB_DEFAULT                    (_EUSART_CFG0_MPAB_DEFAULT << 4)        /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define _EUSART_CFG0_OVS_SHIFT                      5                                       /**< Shift value for EUSART_OVS                  */
#define _EUSART_CFG0_OVS_MASK                       0xE0UL                                  /**< Bit mask for EUSART_OVS                     */
#define _EUSART_CFG0_OVS_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_OVS_X16                        0x00000000UL                            /**< Mode X16 for EUSART_CFG0                    */
#define _EUSART_CFG0_OVS_X8                         0x00000001UL                            /**< Mode X8 for EUSART_CFG0                     */
#define _EUSART_CFG0_OVS_X6                         0x00000002UL                            /**< Mode X6 for EUSART_CFG0                     */
#define _EUSART_CFG0_OVS_X4                         0x00000003UL                            /**< Mode X4 for EUSART_CFG0                     */
#define _EUSART_CFG0_OVS_DISABLE                    0x00000004UL                            /**< Mode DISABLE for EUSART_CFG0                */
#define EUSART_CFG0_OVS_DEFAULT                     (_EUSART_CFG0_OVS_DEFAULT << 5)         /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_OVS_X16                         (_EUSART_CFG0_OVS_X16 << 5)             /**< Shifted mode X16 for EUSART_CFG0            */
#define EUSART_CFG0_OVS_X8                          (_EUSART_CFG0_OVS_X8 << 5)              /**< Shifted mode X8 for EUSART_CFG0             */
#define EUSART_CFG0_OVS_X6                          (_EUSART_CFG0_OVS_X6 << 5)              /**< Shifted mode X6 for EUSART_CFG0             */
#define EUSART_CFG0_OVS_X4                          (_EUSART_CFG0_OVS_X4 << 5)              /**< Shifted mode X4 for EUSART_CFG0             */
#define EUSART_CFG0_OVS_DISABLE                     (_EUSART_CFG0_OVS_DISABLE << 5)         /**< Shifted mode DISABLE for EUSART_CFG0        */
#define EUSART_CFG0_MSBF                            (0x1UL << 10)                           /**< Most Significant Bit First                  */
#define _EUSART_CFG0_MSBF_SHIFT                     10                                      /**< Shift value for EUSART_MSBF                 */
#define _EUSART_CFG0_MSBF_MASK                      0x400UL                                 /**< Bit mask for EUSART_MSBF                    */
#define _EUSART_CFG0_MSBF_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_MSBF_DISABLE                   0x00000000UL                            /**< Mode DISABLE for EUSART_CFG0                */
#define _EUSART_CFG0_MSBF_ENABLE                    0x00000001UL                            /**< Mode ENABLE for EUSART_CFG0                 */
#define EUSART_CFG0_MSBF_DEFAULT                    (_EUSART_CFG0_MSBF_DEFAULT << 10)       /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_MSBF_DISABLE                    (_EUSART_CFG0_MSBF_DISABLE << 10)       /**< Shifted mode DISABLE for EUSART_CFG0        */
#define EUSART_CFG0_MSBF_ENABLE                     (_EUSART_CFG0_MSBF_ENABLE << 10)        /**< Shifted mode ENABLE for EUSART_CFG0         */
#define EUSART_CFG0_RXINV                           (0x1UL << 13)                           /**< Receiver Input Invert                       */
#define _EUSART_CFG0_RXINV_SHIFT                    13                                      /**< Shift value for EUSART_RXINV                */
#define _EUSART_CFG0_RXINV_MASK                     0x2000UL                                /**< Bit mask for EUSART_RXINV                   */
#define _EUSART_CFG0_RXINV_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_RXINV_DISABLE                  0x00000000UL                            /**< Mode DISABLE for EUSART_CFG0                */
#define _EUSART_CFG0_RXINV_ENABLE                   0x00000001UL                            /**< Mode ENABLE for EUSART_CFG0                 */
#define EUSART_CFG0_RXINV_DEFAULT                   (_EUSART_CFG0_RXINV_DEFAULT << 13)      /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_RXINV_DISABLE                   (_EUSART_CFG0_RXINV_DISABLE << 13)      /**< Shifted mode DISABLE for EUSART_CFG0        */
#define EUSART_CFG0_RXINV_ENABLE                    (_EUSART_CFG0_RXINV_ENABLE << 13)       /**< Shifted mode ENABLE for EUSART_CFG0         */
#define EUSART_CFG0_TXINV                           (0x1UL << 14)                           /**< Transmitter output Invert                   */
#define _EUSART_CFG0_TXINV_SHIFT                    14                                      /**< Shift value for EUSART_TXINV                */
#define _EUSART_CFG0_TXINV_MASK                     0x4000UL                                /**< Bit mask for EUSART_TXINV                   */
#define _EUSART_CFG0_TXINV_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_TXINV_DISABLE                  0x00000000UL                            /**< Mode DISABLE for EUSART_CFG0                */
#define _EUSART_CFG0_TXINV_ENABLE                   0x00000001UL                            /**< Mode ENABLE for EUSART_CFG0                 */
#define EUSART_CFG0_TXINV_DEFAULT                   (_EUSART_CFG0_TXINV_DEFAULT << 14)      /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_TXINV_DISABLE                   (_EUSART_CFG0_TXINV_DISABLE << 14)      /**< Shifted mode DISABLE for EUSART_CFG0        */
#define EUSART_CFG0_TXINV_ENABLE                    (_EUSART_CFG0_TXINV_ENABLE << 14)       /**< Shifted mode ENABLE for EUSART_CFG0         */
#define EUSART_CFG0_AUTOTRI                         (0x1UL << 17)                           /**< Automatic TX Tristate                       */
#define _EUSART_CFG0_AUTOTRI_SHIFT                  17                                      /**< Shift value for EUSART_AUTOTRI              */
#define _EUSART_CFG0_AUTOTRI_MASK                   0x20000UL                               /**< Bit mask for EUSART_AUTOTRI                 */
#define _EUSART_CFG0_AUTOTRI_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_AUTOTRI_DISABLE                0x00000000UL                            /**< Mode DISABLE for EUSART_CFG0                */
#define _EUSART_CFG0_AUTOTRI_ENABLE                 0x00000001UL                            /**< Mode ENABLE for EUSART_CFG0                 */
#define EUSART_CFG0_AUTOTRI_DEFAULT                 (_EUSART_CFG0_AUTOTRI_DEFAULT << 17)    /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_AUTOTRI_DISABLE                 (_EUSART_CFG0_AUTOTRI_DISABLE << 17)    /**< Shifted mode DISABLE for EUSART_CFG0        */
#define EUSART_CFG0_AUTOTRI_ENABLE                  (_EUSART_CFG0_AUTOTRI_ENABLE << 17)     /**< Shifted mode ENABLE for EUSART_CFG0         */
#define EUSART_CFG0_SKIPPERRF                       (0x1UL << 20)                           /**< Skip Parity Error Frames                    */
#define _EUSART_CFG0_SKIPPERRF_SHIFT                20                                      /**< Shift value for EUSART_SKIPPERRF            */
#define _EUSART_CFG0_SKIPPERRF_MASK                 0x100000UL                              /**< Bit mask for EUSART_SKIPPERRF               */
#define _EUSART_CFG0_SKIPPERRF_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define EUSART_CFG0_SKIPPERRF_DEFAULT               (_EUSART_CFG0_SKIPPERRF_DEFAULT << 20)  /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_ERRSDMA                         (0x1UL << 22)                           /**< Halt DMA Read On Error                      */
#define _EUSART_CFG0_ERRSDMA_SHIFT                  22                                      /**< Shift value for EUSART_ERRSDMA              */
#define _EUSART_CFG0_ERRSDMA_MASK                   0x400000UL                              /**< Bit mask for EUSART_ERRSDMA                 */
#define _EUSART_CFG0_ERRSDMA_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_ERRSDMA_DISABLE                0x00000000UL                            /**< Mode DISABLE for EUSART_CFG0                */
#define _EUSART_CFG0_ERRSDMA_ENABLE                 0x00000001UL                            /**< Mode ENABLE for EUSART_CFG0                 */
#define EUSART_CFG0_ERRSDMA_DEFAULT                 (_EUSART_CFG0_ERRSDMA_DEFAULT << 22)    /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_ERRSDMA_DISABLE                 (_EUSART_CFG0_ERRSDMA_DISABLE << 22)    /**< Shifted mode DISABLE for EUSART_CFG0        */
#define EUSART_CFG0_ERRSDMA_ENABLE                  (_EUSART_CFG0_ERRSDMA_ENABLE << 22)     /**< Shifted mode ENABLE for EUSART_CFG0         */
#define EUSART_CFG0_ERRSRX                          (0x1UL << 23)                           /**< Disable RX On Error                         */
#define _EUSART_CFG0_ERRSRX_SHIFT                   23                                      /**< Shift value for EUSART_ERRSRX               */
#define _EUSART_CFG0_ERRSRX_MASK                    0x800000UL                              /**< Bit mask for EUSART_ERRSRX                  */
#define _EUSART_CFG0_ERRSRX_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_ERRSRX_DISABLE                 0x00000000UL                            /**< Mode DISABLE for EUSART_CFG0                */
#define _EUSART_CFG0_ERRSRX_ENABLE                  0x00000001UL                            /**< Mode ENABLE for EUSART_CFG0                 */
#define EUSART_CFG0_ERRSRX_DEFAULT                  (_EUSART_CFG0_ERRSRX_DEFAULT << 23)     /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_ERRSRX_DISABLE                  (_EUSART_CFG0_ERRSRX_DISABLE << 23)     /**< Shifted mode DISABLE for EUSART_CFG0        */
#define EUSART_CFG0_ERRSRX_ENABLE                   (_EUSART_CFG0_ERRSRX_ENABLE << 23)      /**< Shifted mode ENABLE for EUSART_CFG0         */
#define EUSART_CFG0_ERRSTX                          (0x1UL << 24)                           /**< Disable TX On Error                         */
#define _EUSART_CFG0_ERRSTX_SHIFT                   24                                      /**< Shift value for EUSART_ERRSTX               */
#define _EUSART_CFG0_ERRSTX_MASK                    0x1000000UL                             /**< Bit mask for EUSART_ERRSTX                  */
#define _EUSART_CFG0_ERRSTX_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define _EUSART_CFG0_ERRSTX_DISABLE                 0x00000000UL                            /**< Mode DISABLE for EUSART_CFG0                */
#define _EUSART_CFG0_ERRSTX_ENABLE                  0x00000001UL                            /**< Mode ENABLE for EUSART_CFG0                 */
#define EUSART_CFG0_ERRSTX_DEFAULT                  (_EUSART_CFG0_ERRSTX_DEFAULT << 24)     /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_ERRSTX_DISABLE                  (_EUSART_CFG0_ERRSTX_DISABLE << 24)     /**< Shifted mode DISABLE for EUSART_CFG0        */
#define EUSART_CFG0_ERRSTX_ENABLE                   (_EUSART_CFG0_ERRSTX_ENABLE << 24)      /**< Shifted mode ENABLE for EUSART_CFG0         */
#define EUSART_CFG0_MVDIS                           (0x1UL << 30)                           /**< Majority Vote Disable                       */
#define _EUSART_CFG0_MVDIS_SHIFT                    30                                      /**< Shift value for EUSART_MVDIS                */
#define _EUSART_CFG0_MVDIS_MASK                     0x40000000UL                            /**< Bit mask for EUSART_MVDIS                   */
#define _EUSART_CFG0_MVDIS_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define EUSART_CFG0_MVDIS_DEFAULT                   (_EUSART_CFG0_MVDIS_DEFAULT << 30)      /**< Shifted mode DEFAULT for EUSART_CFG0        */
#define EUSART_CFG0_AUTOBAUDEN                      (0x1UL << 31)                           /**< AUTOBAUD detection enable                   */
#define _EUSART_CFG0_AUTOBAUDEN_SHIFT               31                                      /**< Shift value for EUSART_AUTOBAUDEN           */
#define _EUSART_CFG0_AUTOBAUDEN_MASK                0x80000000UL                            /**< Bit mask for EUSART_AUTOBAUDEN              */
#define _EUSART_CFG0_AUTOBAUDEN_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for EUSART_CFG0                */
#define EUSART_CFG0_AUTOBAUDEN_DEFAULT              (_EUSART_CFG0_AUTOBAUDEN_DEFAULT << 31) /**< Shifted mode DEFAULT for EUSART_CFG0        */

/* Bit fields for EUSART CFG1 */
#define _EUSART_CFG1_RESETVALUE                     0x00000000UL                                /**< Default value for EUSART_CFG1               */
#define _EUSART_CFG1_MASK                           0x7BCF8E7FUL                                /**< Mask for EUSART_CFG1                        */
#define EUSART_CFG1_DBGHALT                         (0x1UL << 0)                                /**< Debug halt                                  */
#define _EUSART_CFG1_DBGHALT_SHIFT                  0                                           /**< Shift value for EUSART_DBGHALT              */
#define _EUSART_CFG1_DBGHALT_MASK                   0x1UL                                       /**< Bit mask for EUSART_DBGHALT                 */
#define _EUSART_CFG1_DBGHALT_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define _EUSART_CFG1_DBGHALT_DISABLE                0x00000000UL                                /**< Mode DISABLE for EUSART_CFG1                */
#define _EUSART_CFG1_DBGHALT_ENABLE                 0x00000001UL                                /**< Mode ENABLE for EUSART_CFG1                 */
#define EUSART_CFG1_DBGHALT_DEFAULT                 (_EUSART_CFG1_DBGHALT_DEFAULT << 0)         /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define EUSART_CFG1_DBGHALT_DISABLE                 (_EUSART_CFG1_DBGHALT_DISABLE << 0)         /**< Shifted mode DISABLE for EUSART_CFG1        */
#define EUSART_CFG1_DBGHALT_ENABLE                  (_EUSART_CFG1_DBGHALT_ENABLE << 0)          /**< Shifted mode ENABLE for EUSART_CFG1         */
#define EUSART_CFG1_CTSINV                          (0x1UL << 1)                                /**< Clear-to-send Invert Enable                 */
#define _EUSART_CFG1_CTSINV_SHIFT                   1                                           /**< Shift value for EUSART_CTSINV               */
#define _EUSART_CFG1_CTSINV_MASK                    0x2UL                                       /**< Bit mask for EUSART_CTSINV                  */
#define _EUSART_CFG1_CTSINV_DEFAULT                 0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define _EUSART_CFG1_CTSINV_DISABLE                 0x00000000UL                                /**< Mode DISABLE for EUSART_CFG1                */
#define _EUSART_CFG1_CTSINV_ENABLE                  0x00000001UL                                /**< Mode ENABLE for EUSART_CFG1                 */
#define EUSART_CFG1_CTSINV_DEFAULT                  (_EUSART_CFG1_CTSINV_DEFAULT << 1)          /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define EUSART_CFG1_CTSINV_DISABLE                  (_EUSART_CFG1_CTSINV_DISABLE << 1)          /**< Shifted mode DISABLE for EUSART_CFG1        */
#define EUSART_CFG1_CTSINV_ENABLE                   (_EUSART_CFG1_CTSINV_ENABLE << 1)           /**< Shifted mode ENABLE for EUSART_CFG1         */
#define EUSART_CFG1_CTSEN                           (0x1UL << 2)                                /**< Clear-to-send Enable                        */
#define _EUSART_CFG1_CTSEN_SHIFT                    2                                           /**< Shift value for EUSART_CTSEN                */
#define _EUSART_CFG1_CTSEN_MASK                     0x4UL                                       /**< Bit mask for EUSART_CTSEN                   */
#define _EUSART_CFG1_CTSEN_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define _EUSART_CFG1_CTSEN_DISABLE                  0x00000000UL                                /**< Mode DISABLE for EUSART_CFG1                */
#define _EUSART_CFG1_CTSEN_ENABLE                   0x00000001UL                                /**< Mode ENABLE for EUSART_CFG1                 */
#define EUSART_CFG1_CTSEN_DEFAULT                   (_EUSART_CFG1_CTSEN_DEFAULT << 2)           /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define EUSART_CFG1_CTSEN_DISABLE                   (_EUSART_CFG1_CTSEN_DISABLE << 2)           /**< Shifted mode DISABLE for EUSART_CFG1        */
#define EUSART_CFG1_CTSEN_ENABLE                    (_EUSART_CFG1_CTSEN_ENABLE << 2)            /**< Shifted mode ENABLE for EUSART_CFG1         */
#define EUSART_CFG1_RTSINV                          (0x1UL << 3)                                /**< Request-to-send Invert Enable               */
#define _EUSART_CFG1_RTSINV_SHIFT                   3                                           /**< Shift value for EUSART_RTSINV               */
#define _EUSART_CFG1_RTSINV_MASK                    0x8UL                                       /**< Bit mask for EUSART_RTSINV                  */
#define _EUSART_CFG1_RTSINV_DEFAULT                 0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define _EUSART_CFG1_RTSINV_DISABLE                 0x00000000UL                                /**< Mode DISABLE for EUSART_CFG1                */
#define _EUSART_CFG1_RTSINV_ENABLE                  0x00000001UL                                /**< Mode ENABLE for EUSART_CFG1                 */
#define EUSART_CFG1_RTSINV_DEFAULT                  (_EUSART_CFG1_RTSINV_DEFAULT << 3)          /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define EUSART_CFG1_RTSINV_DISABLE                  (_EUSART_CFG1_RTSINV_DISABLE << 3)          /**< Shifted mode DISABLE for EUSART_CFG1        */
#define EUSART_CFG1_RTSINV_ENABLE                   (_EUSART_CFG1_RTSINV_ENABLE << 3)           /**< Shifted mode ENABLE for EUSART_CFG1         */
#define _EUSART_CFG1_RXTIMEOUT_SHIFT                4                                           /**< Shift value for EUSART_RXTIMEOUT            */
#define _EUSART_CFG1_RXTIMEOUT_MASK                 0x70UL                                      /**< Bit mask for EUSART_RXTIMEOUT               */
#define _EUSART_CFG1_RXTIMEOUT_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define _EUSART_CFG1_RXTIMEOUT_DISABLED             0x00000000UL                                /**< Mode DISABLED for EUSART_CFG1               */
#define _EUSART_CFG1_RXTIMEOUT_ONEFRAME             0x00000001UL                                /**< Mode ONEFRAME for EUSART_CFG1               */
#define _EUSART_CFG1_RXTIMEOUT_TWOFRAMES            0x00000002UL                                /**< Mode TWOFRAMES for EUSART_CFG1              */
#define _EUSART_CFG1_RXTIMEOUT_THREEFRAMES          0x00000003UL                                /**< Mode THREEFRAMES for EUSART_CFG1            */
#define _EUSART_CFG1_RXTIMEOUT_FOURFRAMES           0x00000004UL                                /**< Mode FOURFRAMES for EUSART_CFG1             */
#define _EUSART_CFG1_RXTIMEOUT_FIVEFRAMES           0x00000005UL                                /**< Mode FIVEFRAMES for EUSART_CFG1             */
#define _EUSART_CFG1_RXTIMEOUT_SIXFRAMES            0x00000006UL                                /**< Mode SIXFRAMES for EUSART_CFG1              */
#define _EUSART_CFG1_RXTIMEOUT_SEVENFRAMES          0x00000007UL                                /**< Mode SEVENFRAMES for EUSART_CFG1            */
#define EUSART_CFG1_RXTIMEOUT_DEFAULT               (_EUSART_CFG1_RXTIMEOUT_DEFAULT << 4)       /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define EUSART_CFG1_RXTIMEOUT_DISABLED              (_EUSART_CFG1_RXTIMEOUT_DISABLED << 4)      /**< Shifted mode DISABLED for EUSART_CFG1       */
#define EUSART_CFG1_RXTIMEOUT_ONEFRAME              (_EUSART_CFG1_RXTIMEOUT_ONEFRAME << 4)      /**< Shifted mode ONEFRAME for EUSART_CFG1       */
#define EUSART_CFG1_RXTIMEOUT_TWOFRAMES             (_EUSART_CFG1_RXTIMEOUT_TWOFRAMES << 4)     /**< Shifted mode TWOFRAMES for EUSART_CFG1      */
#define EUSART_CFG1_RXTIMEOUT_THREEFRAMES           (_EUSART_CFG1_RXTIMEOUT_THREEFRAMES << 4)   /**< Shifted mode THREEFRAMES for EUSART_CFG1    */
#define EUSART_CFG1_RXTIMEOUT_FOURFRAMES            (_EUSART_CFG1_RXTIMEOUT_FOURFRAMES << 4)    /**< Shifted mode FOURFRAMES for EUSART_CFG1     */
#define EUSART_CFG1_RXTIMEOUT_FIVEFRAMES            (_EUSART_CFG1_RXTIMEOUT_FIVEFRAMES << 4)    /**< Shifted mode FIVEFRAMES for EUSART_CFG1     */
#define EUSART_CFG1_RXTIMEOUT_SIXFRAMES             (_EUSART_CFG1_RXTIMEOUT_SIXFRAMES << 4)     /**< Shifted mode SIXFRAMES for EUSART_CFG1      */
#define EUSART_CFG1_RXTIMEOUT_SEVENFRAMES           (_EUSART_CFG1_RXTIMEOUT_SEVENFRAMES << 4)   /**< Shifted mode SEVENFRAMES for EUSART_CFG1    */
#define EUSART_CFG1_TXDMAWU                         (0x1UL << 9)                                /**< Transmitter DMA Wakeup                      */
#define _EUSART_CFG1_TXDMAWU_SHIFT                  9                                           /**< Shift value for EUSART_TXDMAWU              */
#define _EUSART_CFG1_TXDMAWU_MASK                   0x200UL                                     /**< Bit mask for EUSART_TXDMAWU                 */
#define _EUSART_CFG1_TXDMAWU_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define EUSART_CFG1_TXDMAWU_DEFAULT                 (_EUSART_CFG1_TXDMAWU_DEFAULT << 9)         /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define EUSART_CFG1_RXDMAWU                         (0x1UL << 10)                               /**< Receiver DMA Wakeup                         */
#define _EUSART_CFG1_RXDMAWU_SHIFT                  10                                          /**< Shift value for EUSART_RXDMAWU              */
#define _EUSART_CFG1_RXDMAWU_MASK                   0x400UL                                     /**< Bit mask for EUSART_RXDMAWU                 */
#define _EUSART_CFG1_RXDMAWU_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define EUSART_CFG1_RXDMAWU_DEFAULT                 (_EUSART_CFG1_RXDMAWU_DEFAULT << 10)        /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define EUSART_CFG1_SFUBRX                          (0x1UL << 11)                               /**< Start Frame Unblock Receiver                */
#define _EUSART_CFG1_SFUBRX_SHIFT                   11                                          /**< Shift value for EUSART_SFUBRX               */
#define _EUSART_CFG1_SFUBRX_MASK                    0x800UL                                     /**< Bit mask for EUSART_SFUBRX                  */
#define _EUSART_CFG1_SFUBRX_DEFAULT                 0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define EUSART_CFG1_SFUBRX_DEFAULT                  (_EUSART_CFG1_SFUBRX_DEFAULT << 11)         /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define EUSART_CFG1_RXPRSEN                         (0x1UL << 15)                               /**< PRS RX Enable                               */
#define _EUSART_CFG1_RXPRSEN_SHIFT                  15                                          /**< Shift value for EUSART_RXPRSEN              */
#define _EUSART_CFG1_RXPRSEN_MASK                   0x8000UL                                    /**< Bit mask for EUSART_RXPRSEN                 */
#define _EUSART_CFG1_RXPRSEN_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define EUSART_CFG1_RXPRSEN_DEFAULT                 (_EUSART_CFG1_RXPRSEN_DEFAULT << 15)        /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define _EUSART_CFG1_TXFIW_SHIFT                    16                                          /**< Shift value for EUSART_TXFIW                */
#define _EUSART_CFG1_TXFIW_MASK                     0xF0000UL                                   /**< Bit mask for EUSART_TXFIW                   */
#define _EUSART_CFG1_TXFIW_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define _EUSART_CFG1_TXFIW_ONEFRAME                 0x00000000UL                                /**< Mode ONEFRAME for EUSART_CFG1               */
#define _EUSART_CFG1_TXFIW_TWOFRAMES                0x00000001UL                                /**< Mode TWOFRAMES for EUSART_CFG1              */
#define _EUSART_CFG1_TXFIW_THREEFRAMES              0x00000002UL                                /**< Mode THREEFRAMES for EUSART_CFG1            */
#define _EUSART_CFG1_TXFIW_FOURFRAMES               0x00000003UL                                /**< Mode FOURFRAMES for EUSART_CFG1             */
#define _EUSART_CFG1_TXFIW_FIVEFRAMES               0x00000004UL                                /**< Mode FIVEFRAMES for EUSART_CFG1             */
#define _EUSART_CFG1_TXFIW_SIXFRAMES                0x00000005UL                                /**< Mode SIXFRAMES for EUSART_CFG1              */
#define _EUSART_CFG1_TXFIW_SEVENFRAMES              0x00000006UL                                /**< Mode SEVENFRAMES for EUSART_CFG1            */
#define _EUSART_CFG1_TXFIW_EIGHTFRAMES              0x00000007UL                                /**< Mode EIGHTFRAMES for EUSART_CFG1            */
#define _EUSART_CFG1_TXFIW_NINEFRAMES               0x00000008UL                                /**< Mode NINEFRAMES for EUSART_CFG1             */
#define _EUSART_CFG1_TXFIW_TENFRAMES                0x00000009UL                                /**< Mode TENFRAMES for EUSART_CFG1              */
#define _EUSART_CFG1_TXFIW_ELEVENFRAMES             0x0000000AUL                                /**< Mode ELEVENFRAMES for EUSART_CFG1           */
#define _EUSART_CFG1_TXFIW_TWELVEFRAMES             0x0000000BUL                                /**< Mode TWELVEFRAMES for EUSART_CFG1           */
#define _EUSART_CFG1_TXFIW_THIRTEENFRAMES           0x0000000CUL                                /**< Mode THIRTEENFRAMES for EUSART_CFG1         */
#define _EUSART_CFG1_TXFIW_FOURTEENFRAMES           0x0000000DUL                                /**< Mode FOURTEENFRAMES for EUSART_CFG1         */
#define _EUSART_CFG1_TXFIW_FIFTEENFRAMES            0x0000000EUL                                /**< Mode FIFTEENFRAMES for EUSART_CFG1          */
#define _EUSART_CFG1_TXFIW_SIXTEENFRAMES            0x0000000FUL                                /**< Mode SIXTEENFRAMES for EUSART_CFG1          */
#define EUSART_CFG1_TXFIW_DEFAULT                   (_EUSART_CFG1_TXFIW_DEFAULT << 16)          /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define EUSART_CFG1_TXFIW_ONEFRAME                  (_EUSART_CFG1_TXFIW_ONEFRAME << 16)         /**< Shifted mode ONEFRAME for EUSART_CFG1       */
#define EUSART_CFG1_TXFIW_TWOFRAMES                 (_EUSART_CFG1_TXFIW_TWOFRAMES << 16)        /**< Shifted mode TWOFRAMES for EUSART_CFG1      */
#define EUSART_CFG1_TXFIW_THREEFRAMES               (_EUSART_CFG1_TXFIW_THREEFRAMES << 16)      /**< Shifted mode THREEFRAMES for EUSART_CFG1    */
#define EUSART_CFG1_TXFIW_FOURFRAMES                (_EUSART_CFG1_TXFIW_FOURFRAMES << 16)       /**< Shifted mode FOURFRAMES for EUSART_CFG1     */
#define EUSART_CFG1_TXFIW_FIVEFRAMES                (_EUSART_CFG1_TXFIW_FIVEFRAMES << 16)       /**< Shifted mode FIVEFRAMES for EUSART_CFG1     */
#define EUSART_CFG1_TXFIW_SIXFRAMES                 (_EUSART_CFG1_TXFIW_SIXFRAMES << 16)        /**< Shifted mode SIXFRAMES for EUSART_CFG1      */
#define EUSART_CFG1_TXFIW_SEVENFRAMES               (_EUSART_CFG1_TXFIW_SEVENFRAMES << 16)      /**< Shifted mode SEVENFRAMES for EUSART_CFG1    */
#define EUSART_CFG1_TXFIW_EIGHTFRAMES               (_EUSART_CFG1_TXFIW_EIGHTFRAMES << 16)      /**< Shifted mode EIGHTFRAMES for EUSART_CFG1    */
#define EUSART_CFG1_TXFIW_NINEFRAMES                (_EUSART_CFG1_TXFIW_NINEFRAMES << 16)       /**< Shifted mode NINEFRAMES for EUSART_CFG1     */
#define EUSART_CFG1_TXFIW_TENFRAMES                 (_EUSART_CFG1_TXFIW_TENFRAMES << 16)        /**< Shifted mode TENFRAMES for EUSART_CFG1      */
#define EUSART_CFG1_TXFIW_ELEVENFRAMES              (_EUSART_CFG1_TXFIW_ELEVENFRAMES << 16)     /**< Shifted mode ELEVENFRAMES for EUSART_CFG1   */
#define EUSART_CFG1_TXFIW_TWELVEFRAMES              (_EUSART_CFG1_TXFIW_TWELVEFRAMES << 16)     /**< Shifted mode TWELVEFRAMES for EUSART_CFG1   */
#define EUSART_CFG1_TXFIW_THIRTEENFRAMES            (_EUSART_CFG1_TXFIW_THIRTEENFRAMES << 16)   /**< Shifted mode THIRTEENFRAMES for EUSART_CFG1 */
#define EUSART_CFG1_TXFIW_FOURTEENFRAMES            (_EUSART_CFG1_TXFIW_FOURTEENFRAMES << 16)   /**< Shifted mode FOURTEENFRAMES for EUSART_CFG1 */
#define EUSART_CFG1_TXFIW_FIFTEENFRAMES             (_EUSART_CFG1_TXFIW_FIFTEENFRAMES << 16)    /**< Shifted mode FIFTEENFRAMES for EUSART_CFG1  */
#define EUSART_CFG1_TXFIW_SIXTEENFRAMES             (_EUSART_CFG1_TXFIW_SIXTEENFRAMES << 16)    /**< Shifted mode SIXTEENFRAMES for EUSART_CFG1  */
#define _EUSART_CFG1_RTSRXFW_SHIFT                  22                                          /**< Shift value for EUSART_RTSRXFW              */
#define _EUSART_CFG1_RTSRXFW_MASK                   0x3C00000UL                                 /**< Bit mask for EUSART_RTSRXFW                 */
#define _EUSART_CFG1_RTSRXFW_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define _EUSART_CFG1_RTSRXFW_ONEFRAME               0x00000000UL                                /**< Mode ONEFRAME for EUSART_CFG1               */
#define _EUSART_CFG1_RTSRXFW_TWOFRAMES              0x00000001UL                                /**< Mode TWOFRAMES for EUSART_CFG1              */
#define _EUSART_CFG1_RTSRXFW_THREEFRAMES            0x00000002UL                                /**< Mode THREEFRAMES for EUSART_CFG1            */
#define _EUSART_CFG1_RTSRXFW_FOURFRAMES             0x00000003UL                                /**< Mode FOURFRAMES for EUSART_CFG1             */
#define _EUSART_CFG1_RTSRXFW_FIVEFRAMES             0x00000004UL                                /**< Mode FIVEFRAMES for EUSART_CFG1             */
#define _EUSART_CFG1_RTSRXFW_SIXFRAMES              0x00000005UL                                /**< Mode SIXFRAMES for EUSART_CFG1              */
#define _EUSART_CFG1_RTSRXFW_SEVENFRAMES            0x00000006UL                                /**< Mode SEVENFRAMES for EUSART_CFG1            */
#define _EUSART_CFG1_RTSRXFW_EIGHTFRAMES            0x00000007UL                                /**< Mode EIGHTFRAMES for EUSART_CFG1            */
#define _EUSART_CFG1_RTSRXFW_NINEFRAMES             0x00000008UL                                /**< Mode NINEFRAMES for EUSART_CFG1             */
#define _EUSART_CFG1_RTSRXFW_TENFRAMES              0x00000009UL                                /**< Mode TENFRAMES for EUSART_CFG1              */
#define _EUSART_CFG1_RTSRXFW_ELEVENFRAMES           0x0000000AUL                                /**< Mode ELEVENFRAMES for EUSART_CFG1           */
#define _EUSART_CFG1_RTSRXFW_TWELVEFRAMES           0x0000000BUL                                /**< Mode TWELVEFRAMES for EUSART_CFG1           */
#define _EUSART_CFG1_RTSRXFW_THIRTEENFRAMES         0x0000000CUL                                /**< Mode THIRTEENFRAMES for EUSART_CFG1         */
#define _EUSART_CFG1_RTSRXFW_FOURTEENFRAMES         0x0000000DUL                                /**< Mode FOURTEENFRAMES for EUSART_CFG1         */
#define _EUSART_CFG1_RTSRXFW_FIFTEENFRAMES          0x0000000EUL                                /**< Mode FIFTEENFRAMES for EUSART_CFG1          */
#define _EUSART_CFG1_RTSRXFW_SIXTEENFRAMES          0x0000000FUL                                /**< Mode SIXTEENFRAMES for EUSART_CFG1          */
#define EUSART_CFG1_RTSRXFW_DEFAULT                 (_EUSART_CFG1_RTSRXFW_DEFAULT << 22)        /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define EUSART_CFG1_RTSRXFW_ONEFRAME                (_EUSART_CFG1_RTSRXFW_ONEFRAME << 22)       /**< Shifted mode ONEFRAME for EUSART_CFG1       */
#define EUSART_CFG1_RTSRXFW_TWOFRAMES               (_EUSART_CFG1_RTSRXFW_TWOFRAMES << 22)      /**< Shifted mode TWOFRAMES for EUSART_CFG1      */
#define EUSART_CFG1_RTSRXFW_THREEFRAMES             (_EUSART_CFG1_RTSRXFW_THREEFRAMES << 22)    /**< Shifted mode THREEFRAMES for EUSART_CFG1    */
#define EUSART_CFG1_RTSRXFW_FOURFRAMES              (_EUSART_CFG1_RTSRXFW_FOURFRAMES << 22)     /**< Shifted mode FOURFRAMES for EUSART_CFG1     */
#define EUSART_CFG1_RTSRXFW_FIVEFRAMES              (_EUSART_CFG1_RTSRXFW_FIVEFRAMES << 22)     /**< Shifted mode FIVEFRAMES for EUSART_CFG1     */
#define EUSART_CFG1_RTSRXFW_SIXFRAMES               (_EUSART_CFG1_RTSRXFW_SIXFRAMES << 22)      /**< Shifted mode SIXFRAMES for EUSART_CFG1      */
#define EUSART_CFG1_RTSRXFW_SEVENFRAMES             (_EUSART_CFG1_RTSRXFW_SEVENFRAMES << 22)    /**< Shifted mode SEVENFRAMES for EUSART_CFG1    */
#define EUSART_CFG1_RTSRXFW_EIGHTFRAMES             (_EUSART_CFG1_RTSRXFW_EIGHTFRAMES << 22)    /**< Shifted mode EIGHTFRAMES for EUSART_CFG1    */
#define EUSART_CFG1_RTSRXFW_NINEFRAMES              (_EUSART_CFG1_RTSRXFW_NINEFRAMES << 22)     /**< Shifted mode NINEFRAMES for EUSART_CFG1     */
#define EUSART_CFG1_RTSRXFW_TENFRAMES               (_EUSART_CFG1_RTSRXFW_TENFRAMES << 22)      /**< Shifted mode TENFRAMES for EUSART_CFG1      */
#define EUSART_CFG1_RTSRXFW_ELEVENFRAMES            (_EUSART_CFG1_RTSRXFW_ELEVENFRAMES << 22)   /**< Shifted mode ELEVENFRAMES for EUSART_CFG1   */
#define EUSART_CFG1_RTSRXFW_TWELVEFRAMES            (_EUSART_CFG1_RTSRXFW_TWELVEFRAMES << 22)   /**< Shifted mode TWELVEFRAMES for EUSART_CFG1   */
#define EUSART_CFG1_RTSRXFW_THIRTEENFRAMES          (_EUSART_CFG1_RTSRXFW_THIRTEENFRAMES << 22) /**< Shifted mode THIRTEENFRAMES for EUSART_CFG1 */
#define EUSART_CFG1_RTSRXFW_FOURTEENFRAMES          (_EUSART_CFG1_RTSRXFW_FOURTEENFRAMES << 22) /**< Shifted mode FOURTEENFRAMES for EUSART_CFG1 */
#define EUSART_CFG1_RTSRXFW_FIFTEENFRAMES           (_EUSART_CFG1_RTSRXFW_FIFTEENFRAMES << 22)  /**< Shifted mode FIFTEENFRAMES for EUSART_CFG1  */
#define EUSART_CFG1_RTSRXFW_SIXTEENFRAMES           (_EUSART_CFG1_RTSRXFW_SIXTEENFRAMES << 22)  /**< Shifted mode SIXTEENFRAMES for EUSART_CFG1  */
#define _EUSART_CFG1_RXFIW_SHIFT                    27                                          /**< Shift value for EUSART_RXFIW                */
#define _EUSART_CFG1_RXFIW_MASK                     0x78000000UL                                /**< Bit mask for EUSART_RXFIW                   */
#define _EUSART_CFG1_RXFIW_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for EUSART_CFG1                */
#define _EUSART_CFG1_RXFIW_ONEFRAME                 0x00000000UL                                /**< Mode ONEFRAME for EUSART_CFG1               */
#define _EUSART_CFG1_RXFIW_TWOFRAMES                0x00000001UL                                /**< Mode TWOFRAMES for EUSART_CFG1              */
#define _EUSART_CFG1_RXFIW_THREEFRAMES              0x00000002UL                                /**< Mode THREEFRAMES for EUSART_CFG1            */
#define _EUSART_CFG1_RXFIW_FOURFRAMES               0x00000003UL                                /**< Mode FOURFRAMES for EUSART_CFG1             */
#define _EUSART_CFG1_RXFIW_FIVEFRAMES               0x00000004UL                                /**< Mode FIVEFRAMES for EUSART_CFG1             */
#define _EUSART_CFG1_RXFIW_SIXFRAMES                0x00000005UL                                /**< Mode SIXFRAMES for EUSART_CFG1              */
#define _EUSART_CFG1_RXFIW_SEVENFRAMES              0x00000006UL                                /**< Mode SEVENFRAMES for EUSART_CFG1            */
#define _EUSART_CFG1_RXFIW_EIGHTFRAMES              0x00000007UL                                /**< Mode EIGHTFRAMES for EUSART_CFG1            */
#define _EUSART_CFG1_RXFIW_NINEFRAMES               0x00000008UL                                /**< Mode NINEFRAMES for EUSART_CFG1             */
#define _EUSART_CFG1_RXFIW_TENFRAMES                0x00000009UL                                /**< Mode TENFRAMES for EUSART_CFG1              */
#define _EUSART_CFG1_RXFIW_ELEVENFRAMES             0x0000000AUL                                /**< Mode ELEVENFRAMES for EUSART_CFG1           */
#define _EUSART_CFG1_RXFIW_TWELVEFRAMES             0x0000000BUL                                /**< Mode TWELVEFRAMES for EUSART_CFG1           */
#define _EUSART_CFG1_RXFIW_THIRTEENFRAMES           0x0000000CUL                                /**< Mode THIRTEENFRAMES for EUSART_CFG1         */
#define _EUSART_CFG1_RXFIW_FOURTEENFRAMES           0x0000000DUL                                /**< Mode FOURTEENFRAMES for EUSART_CFG1         */
#define _EUSART_CFG1_RXFIW_FIFTEENFRAMES            0x0000000EUL                                /**< Mode FIFTEENFRAMES for EUSART_CFG1          */
#define _EUSART_CFG1_RXFIW_SIXTEENFRAMES            0x0000000FUL                                /**< Mode SIXTEENFRAMES for EUSART_CFG1          */
#define EUSART_CFG1_RXFIW_DEFAULT                   (_EUSART_CFG1_RXFIW_DEFAULT << 27)          /**< Shifted mode DEFAULT for EUSART_CFG1        */
#define EUSART_CFG1_RXFIW_ONEFRAME                  (_EUSART_CFG1_RXFIW_ONEFRAME << 27)         /**< Shifted mode ONEFRAME for EUSART_CFG1       */
#define EUSART_CFG1_RXFIW_TWOFRAMES                 (_EUSART_CFG1_RXFIW_TWOFRAMES << 27)        /**< Shifted mode TWOFRAMES for EUSART_CFG1      */
#define EUSART_CFG1_RXFIW_THREEFRAMES               (_EUSART_CFG1_RXFIW_THREEFRAMES << 27)      /**< Shifted mode THREEFRAMES for EUSART_CFG1    */
#define EUSART_CFG1_RXFIW_FOURFRAMES                (_EUSART_CFG1_RXFIW_FOURFRAMES << 27)       /**< Shifted mode FOURFRAMES for EUSART_CFG1     */
#define EUSART_CFG1_RXFIW_FIVEFRAMES                (_EUSART_CFG1_RXFIW_FIVEFRAMES << 27)       /**< Shifted mode FIVEFRAMES for EUSART_CFG1     */
#define EUSART_CFG1_RXFIW_SIXFRAMES                 (_EUSART_CFG1_RXFIW_SIXFRAMES << 27)        /**< Shifted mode SIXFRAMES for EUSART_CFG1      */
#define EUSART_CFG1_RXFIW_SEVENFRAMES               (_EUSART_CFG1_RXFIW_SEVENFRAMES << 27)      /**< Shifted mode SEVENFRAMES for EUSART_CFG1    */
#define EUSART_CFG1_RXFIW_EIGHTFRAMES               (_EUSART_CFG1_RXFIW_EIGHTFRAMES << 27)      /**< Shifted mode EIGHTFRAMES for EUSART_CFG1    */
#define EUSART_CFG1_RXFIW_NINEFRAMES                (_EUSART_CFG1_RXFIW_NINEFRAMES << 27)       /**< Shifted mode NINEFRAMES for EUSART_CFG1     */
#define EUSART_CFG1_RXFIW_TENFRAMES                 (_EUSART_CFG1_RXFIW_TENFRAMES << 27)        /**< Shifted mode TENFRAMES for EUSART_CFG1      */
#define EUSART_CFG1_RXFIW_ELEVENFRAMES              (_EUSART_CFG1_RXFIW_ELEVENFRAMES << 27)     /**< Shifted mode ELEVENFRAMES for EUSART_CFG1   */
#define EUSART_CFG1_RXFIW_TWELVEFRAMES              (_EUSART_CFG1_RXFIW_TWELVEFRAMES << 27)     /**< Shifted mode TWELVEFRAMES for EUSART_CFG1   */
#define EUSART_CFG1_RXFIW_THIRTEENFRAMES            (_EUSART_CFG1_RXFIW_THIRTEENFRAMES << 27)   /**< Shifted mode THIRTEENFRAMES for EUSART_CFG1 */
#define EUSART_CFG1_RXFIW_FOURTEENFRAMES            (_EUSART_CFG1_RXFIW_FOURTEENFRAMES << 27)   /**< Shifted mode FOURTEENFRAMES for EUSART_CFG1 */
#define EUSART_CFG1_RXFIW_FIFTEENFRAMES             (_EUSART_CFG1_RXFIW_FIFTEENFRAMES << 27)    /**< Shifted mode FIFTEENFRAMES for EUSART_CFG1  */
#define EUSART_CFG1_RXFIW_SIXTEENFRAMES             (_EUSART_CFG1_RXFIW_SIXTEENFRAMES << 27)    /**< Shifted mode SIXTEENFRAMES for EUSART_CFG1  */

/* Bit fields for EUSART CFG2 */
#define _EUSART_CFG2_RESETVALUE                     0x00000020UL                              /**< Default value for EUSART_CFG2               */
#define _EUSART_CFG2_MASK                           0xFF0000FFUL                              /**< Mask for EUSART_CFG2                        */
#define EUSART_CFG2_MASTER                          (0x1UL << 0)                              /**< Main mode                                   */
#define _EUSART_CFG2_MASTER_SHIFT                   0                                         /**< Shift value for EUSART_MASTER               */
#define _EUSART_CFG2_MASTER_MASK                    0x1UL                                     /**< Bit mask for EUSART_MASTER                  */
#define _EUSART_CFG2_MASTER_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for EUSART_CFG2                */
#define _EUSART_CFG2_MASTER_SLAVE                   0x00000000UL                              /**< Mode SLAVE for EUSART_CFG2                  */
#define _EUSART_CFG2_MASTER_MASTER                  0x00000001UL                              /**< Mode MASTER for EUSART_CFG2                 */
#define EUSART_CFG2_MASTER_DEFAULT                  (_EUSART_CFG2_MASTER_DEFAULT << 0)        /**< Shifted mode DEFAULT for EUSART_CFG2        */
#define EUSART_CFG2_MASTER_SLAVE                    (_EUSART_CFG2_MASTER_SLAVE << 0)          /**< Shifted mode SLAVE for EUSART_CFG2          */
#define EUSART_CFG2_MASTER_MASTER                   (_EUSART_CFG2_MASTER_MASTER << 0)         /**< Shifted mode MASTER for EUSART_CFG2         */
#define EUSART_CFG2_CLKPOL                          (0x1UL << 1)                              /**< Clock Polarity                              */
#define _EUSART_CFG2_CLKPOL_SHIFT                   1                                         /**< Shift value for EUSART_CLKPOL               */
#define _EUSART_CFG2_CLKPOL_MASK                    0x2UL                                     /**< Bit mask for EUSART_CLKPOL                  */
#define _EUSART_CFG2_CLKPOL_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for EUSART_CFG2                */
#define _EUSART_CFG2_CLKPOL_IDLELOW                 0x00000000UL                              /**< Mode IDLELOW for EUSART_CFG2                */
#define _EUSART_CFG2_CLKPOL_IDLEHIGH                0x00000001UL                              /**< Mode IDLEHIGH for EUSART_CFG2               */
#define EUSART_CFG2_CLKPOL_DEFAULT                  (_EUSART_CFG2_CLKPOL_DEFAULT << 1)        /**< Shifted mode DEFAULT for EUSART_CFG2        */
#define EUSART_CFG2_CLKPOL_IDLELOW                  (_EUSART_CFG2_CLKPOL_IDLELOW << 1)        /**< Shifted mode IDLELOW for EUSART_CFG2        */
#define EUSART_CFG2_CLKPOL_IDLEHIGH                 (_EUSART_CFG2_CLKPOL_IDLEHIGH << 1)       /**< Shifted mode IDLEHIGH for EUSART_CFG2       */
#define EUSART_CFG2_CLKPHA                          (0x1UL << 2)                              /**< Clock Edge for Setup/Sample                 */
#define _EUSART_CFG2_CLKPHA_SHIFT                   2                                         /**< Shift value for EUSART_CLKPHA               */
#define _EUSART_CFG2_CLKPHA_MASK                    0x4UL                                     /**< Bit mask for EUSART_CLKPHA                  */
#define _EUSART_CFG2_CLKPHA_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for EUSART_CFG2                */
#define _EUSART_CFG2_CLKPHA_SAMPLELEADING           0x00000000UL                              /**< Mode SAMPLELEADING for EUSART_CFG2          */
#define _EUSART_CFG2_CLKPHA_SAMPLETRAILING          0x00000001UL                              /**< Mode SAMPLETRAILING for EUSART_CFG2         */
#define EUSART_CFG2_CLKPHA_DEFAULT                  (_EUSART_CFG2_CLKPHA_DEFAULT << 2)        /**< Shifted mode DEFAULT for EUSART_CFG2        */
#define EUSART_CFG2_CLKPHA_SAMPLELEADING            (_EUSART_CFG2_CLKPHA_SAMPLELEADING << 2)  /**< Shifted mode SAMPLELEADING for EUSART_CFG2  */
#define EUSART_CFG2_CLKPHA_SAMPLETRAILING           (_EUSART_CFG2_CLKPHA_SAMPLETRAILING << 2) /**< Shifted mode SAMPLETRAILING for EUSART_CFG2 */
#define EUSART_CFG2_CSINV                           (0x1UL << 3)                              /**< Chip Select Invert                          */
#define _EUSART_CFG2_CSINV_SHIFT                    3                                         /**< Shift value for EUSART_CSINV                */
#define _EUSART_CFG2_CSINV_MASK                     0x8UL                                     /**< Bit mask for EUSART_CSINV                   */
#define _EUSART_CFG2_CSINV_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for EUSART_CFG2                */
#define _EUSART_CFG2_CSINV_AL                       0x00000000UL                              /**< Mode AL for EUSART_CFG2                     */
#define _EUSART_CFG2_CSINV_AH                       0x00000001UL                              /**< Mode AH for EUSART_CFG2                     */
#define EUSART_CFG2_CSINV_DEFAULT                   (_EUSART_CFG2_CSINV_DEFAULT << 3)         /**< Shifted mode DEFAULT for EUSART_CFG2        */
#define EUSART_CFG2_CSINV_AL                        (_EUSART_CFG2_CSINV_AL << 3)              /**< Shifted mode AL for EUSART_CFG2             */
#define EUSART_CFG2_CSINV_AH                        (_EUSART_CFG2_CSINV_AH << 3)              /**< Shifted mode AH for EUSART_CFG2             */
#define EUSART_CFG2_AUTOTX                          (0x1UL << 4)                              /**< Always Transmit When RXFIFO Not Full        */
#define _EUSART_CFG2_AUTOTX_SHIFT                   4                                         /**< Shift value for EUSART_AUTOTX               */
#define _EUSART_CFG2_AUTOTX_MASK                    0x10UL                                    /**< Bit mask for EUSART_AUTOTX                  */
#define _EUSART_CFG2_AUTOTX_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for EUSART_CFG2                */
#define EUSART_CFG2_AUTOTX_DEFAULT                  (_EUSART_CFG2_AUTOTX_DEFAULT << 4)        /**< Shifted mode DEFAULT for EUSART_CFG2        */
#define EUSART_CFG2_AUTOCS                          (0x1UL << 5)                              /**< Automatic Chip Select                       */
#define _EUSART_CFG2_AUTOCS_SHIFT                   5                                         /**< Shift value for EUSART_AUTOCS               */
#define _EUSART_CFG2_AUTOCS_MASK                    0x20UL                                    /**< Bit mask for EUSART_AUTOCS                  */
#define _EUSART_CFG2_AUTOCS_DEFAULT                 0x00000001UL                              /**< Mode DEFAULT for EUSART_CFG2                */
#define EUSART_CFG2_AUTOCS_DEFAULT                  (_EUSART_CFG2_AUTOCS_DEFAULT << 5)        /**< Shifted mode DEFAULT for EUSART_CFG2        */
#define EUSART_CFG2_CLKPRSEN                        (0x1UL << 6)                              /**< PRS CLK Enable                              */
#define _EUSART_CFG2_CLKPRSEN_SHIFT                 6                                         /**< Shift value for EUSART_CLKPRSEN             */
#define _EUSART_CFG2_CLKPRSEN_MASK                  0x40UL                                    /**< Bit mask for EUSART_CLKPRSEN                */
#define _EUSART_CFG2_CLKPRSEN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for EUSART_CFG2                */
#define EUSART_CFG2_CLKPRSEN_DEFAULT                (_EUSART_CFG2_CLKPRSEN_DEFAULT << 6)      /**< Shifted mode DEFAULT for EUSART_CFG2        */
#define EUSART_CFG2_FORCELOAD                       (0x1UL << 7)                              /**< Force Load to Shift Register                */
#define _EUSART_CFG2_FORCELOAD_SHIFT                7                                         /**< Shift value for EUSART_FORCELOAD            */
#define _EUSART_CFG2_FORCELOAD_MASK                 0x80UL                                    /**< Bit mask for EUSART_FORCELOAD               */
#define _EUSART_CFG2_FORCELOAD_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for EUSART_CFG2                */
#define EUSART_CFG2_FORCELOAD_DEFAULT               (_EUSART_CFG2_FORCELOAD_DEFAULT << 7)     /**< Shifted mode DEFAULT for EUSART_CFG2        */
#define _EUSART_CFG2_SDIV_SHIFT                     24                                        /**< Shift value for EUSART_SDIV                 */
#define _EUSART_CFG2_SDIV_MASK                      0xFF000000UL                              /**< Bit mask for EUSART_SDIV                    */
#define _EUSART_CFG2_SDIV_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for EUSART_CFG2                */
#define EUSART_CFG2_SDIV_DEFAULT                    (_EUSART_CFG2_SDIV_DEFAULT << 24)         /**< Shifted mode DEFAULT for EUSART_CFG2        */

/* Bit fields for EUSART FRAMECFG */
#define _EUSART_FRAMECFG_RESETVALUE                 0x00001002UL                                  /**< Default value for EUSART_FRAMECFG           */
#define _EUSART_FRAMECFG_MASK                       0x0000330FUL                                  /**< Mask for EUSART_FRAMECFG                    */
#define _EUSART_FRAMECFG_DATABITS_SHIFT             0                                             /**< Shift value for EUSART_DATABITS             */
#define _EUSART_FRAMECFG_DATABITS_MASK              0xFUL                                         /**< Bit mask for EUSART_DATABITS                */
#define _EUSART_FRAMECFG_DATABITS_DEFAULT           0x00000002UL                                  /**< Mode DEFAULT for EUSART_FRAMECFG            */
#define _EUSART_FRAMECFG_DATABITS_SEVEN             0x00000001UL                                  /**< Mode SEVEN for EUSART_FRAMECFG              */
#define _EUSART_FRAMECFG_DATABITS_EIGHT             0x00000002UL                                  /**< Mode EIGHT for EUSART_FRAMECFG              */
#define _EUSART_FRAMECFG_DATABITS_NINE              0x00000003UL                                  /**< Mode NINE for EUSART_FRAMECFG               */
#define _EUSART_FRAMECFG_DATABITS_TEN               0x00000004UL                                  /**< Mode TEN for EUSART_FRAMECFG                */
#define _EUSART_FRAMECFG_DATABITS_ELEVEN            0x00000005UL                                  /**< Mode ELEVEN for EUSART_FRAMECFG             */
#define _EUSART_FRAMECFG_DATABITS_TWELVE            0x00000006UL                                  /**< Mode TWELVE for EUSART_FRAMECFG             */
#define _EUSART_FRAMECFG_DATABITS_THIRTEEN          0x00000007UL                                  /**< Mode THIRTEEN for EUSART_FRAMECFG           */
#define _EUSART_FRAMECFG_DATABITS_FOURTEEN          0x00000008UL                                  /**< Mode FOURTEEN for EUSART_FRAMECFG           */
#define _EUSART_FRAMECFG_DATABITS_FIFTEEN           0x00000009UL                                  /**< Mode FIFTEEN for EUSART_FRAMECFG            */
#define _EUSART_FRAMECFG_DATABITS_SIXTEEN           0x0000000AUL                                  /**< Mode SIXTEEN for EUSART_FRAMECFG            */
#define EUSART_FRAMECFG_DATABITS_DEFAULT            (_EUSART_FRAMECFG_DATABITS_DEFAULT << 0)      /**< Shifted mode DEFAULT for EUSART_FRAMECFG    */
#define EUSART_FRAMECFG_DATABITS_SEVEN              (_EUSART_FRAMECFG_DATABITS_SEVEN << 0)        /**< Shifted mode SEVEN for EUSART_FRAMECFG      */
#define EUSART_FRAMECFG_DATABITS_EIGHT              (_EUSART_FRAMECFG_DATABITS_EIGHT << 0)        /**< Shifted mode EIGHT for EUSART_FRAMECFG      */
#define EUSART_FRAMECFG_DATABITS_NINE               (_EUSART_FRAMECFG_DATABITS_NINE << 0)         /**< Shifted mode NINE for EUSART_FRAMECFG       */
#define EUSART_FRAMECFG_DATABITS_TEN                (_EUSART_FRAMECFG_DATABITS_TEN << 0)          /**< Shifted mode TEN for EUSART_FRAMECFG        */
#define EUSART_FRAMECFG_DATABITS_ELEVEN             (_EUSART_FRAMECFG_DATABITS_ELEVEN << 0)       /**< Shifted mode ELEVEN for EUSART_FRAMECFG     */
#define EUSART_FRAMECFG_DATABITS_TWELVE             (_EUSART_FRAMECFG_DATABITS_TWELVE << 0)       /**< Shifted mode TWELVE for EUSART_FRAMECFG     */
#define EUSART_FRAMECFG_DATABITS_THIRTEEN           (_EUSART_FRAMECFG_DATABITS_THIRTEEN << 0)     /**< Shifted mode THIRTEEN for EUSART_FRAMECFG   */
#define EUSART_FRAMECFG_DATABITS_FOURTEEN           (_EUSART_FRAMECFG_DATABITS_FOURTEEN << 0)     /**< Shifted mode FOURTEEN for EUSART_FRAMECFG   */
#define EUSART_FRAMECFG_DATABITS_FIFTEEN            (_EUSART_FRAMECFG_DATABITS_FIFTEEN << 0)      /**< Shifted mode FIFTEEN for EUSART_FRAMECFG    */
#define EUSART_FRAMECFG_DATABITS_SIXTEEN            (_EUSART_FRAMECFG_DATABITS_SIXTEEN << 0)      /**< Shifted mode SIXTEEN for EUSART_FRAMECFG    */
#define _EUSART_FRAMECFG_PARITY_SHIFT               8                                             /**< Shift value for EUSART_PARITY               */
#define _EUSART_FRAMECFG_PARITY_MASK                0x300UL                                       /**< Bit mask for EUSART_PARITY                  */
#define _EUSART_FRAMECFG_PARITY_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for EUSART_FRAMECFG            */
#define _EUSART_FRAMECFG_PARITY_NONE                0x00000000UL                                  /**< Mode NONE for EUSART_FRAMECFG               */
#define _EUSART_FRAMECFG_PARITY_EVEN                0x00000002UL                                  /**< Mode EVEN for EUSART_FRAMECFG               */
#define _EUSART_FRAMECFG_PARITY_ODD                 0x00000003UL                                  /**< Mode ODD for EUSART_FRAMECFG                */
#define EUSART_FRAMECFG_PARITY_DEFAULT              (_EUSART_FRAMECFG_PARITY_DEFAULT << 8)        /**< Shifted mode DEFAULT for EUSART_FRAMECFG    */
#define EUSART_FRAMECFG_PARITY_NONE                 (_EUSART_FRAMECFG_PARITY_NONE << 8)           /**< Shifted mode NONE for EUSART_FRAMECFG       */
#define EUSART_FRAMECFG_PARITY_EVEN                 (_EUSART_FRAMECFG_PARITY_EVEN << 8)           /**< Shifted mode EVEN for EUSART_FRAMECFG       */
#define EUSART_FRAMECFG_PARITY_ODD                  (_EUSART_FRAMECFG_PARITY_ODD << 8)            /**< Shifted mode ODD for EUSART_FRAMECFG        */
#define _EUSART_FRAMECFG_STOPBITS_SHIFT             12                                            /**< Shift value for EUSART_STOPBITS             */
#define _EUSART_FRAMECFG_STOPBITS_MASK              0x3000UL                                      /**< Bit mask for EUSART_STOPBITS                */
#define _EUSART_FRAMECFG_STOPBITS_DEFAULT           0x00000001UL                                  /**< Mode DEFAULT for EUSART_FRAMECFG            */
#define _EUSART_FRAMECFG_STOPBITS_HALF              0x00000000UL                                  /**< Mode HALF for EUSART_FRAMECFG               */
#define _EUSART_FRAMECFG_STOPBITS_ONE               0x00000001UL                                  /**< Mode ONE for EUSART_FRAMECFG                */
#define _EUSART_FRAMECFG_STOPBITS_ONEANDAHALF       0x00000002UL                                  /**< Mode ONEANDAHALF for EUSART_FRAMECFG        */
#define _EUSART_FRAMECFG_STOPBITS_TWO               0x00000003UL                                  /**< Mode TWO for EUSART_FRAMECFG                */
#define EUSART_FRAMECFG_STOPBITS_DEFAULT            (_EUSART_FRAMECFG_STOPBITS_DEFAULT << 12)     /**< Shifted mode DEFAULT for EUSART_FRAMECFG    */
#define EUSART_FRAMECFG_STOPBITS_HALF               (_EUSART_FRAMECFG_STOPBITS_HALF << 12)        /**< Shifted mode HALF for EUSART_FRAMECFG       */
#define EUSART_FRAMECFG_STOPBITS_ONE                (_EUSART_FRAMECFG_STOPBITS_ONE << 12)         /**< Shifted mode ONE for EUSART_FRAMECFG        */
#define EUSART_FRAMECFG_STOPBITS_ONEANDAHALF        (_EUSART_FRAMECFG_STOPBITS_ONEANDAHALF << 12) /**< Shifted mode ONEANDAHALF for EUSART_FRAMECFG*/
#define EUSART_FRAMECFG_STOPBITS_TWO                (_EUSART_FRAMECFG_STOPBITS_TWO << 12)         /**< Shifted mode TWO for EUSART_FRAMECFG        */

/* Bit fields for EUSART DTXDATCFG */
#define _EUSART_DTXDATCFG_RESETVALUE                0x00000000UL                            /**< Default value for EUSART_DTXDATCFG          */
#define _EUSART_DTXDATCFG_MASK                      0x0000FFFFUL                            /**< Mask for EUSART_DTXDATCFG                   */
#define _EUSART_DTXDATCFG_DTXDAT_SHIFT              0                                       /**< Shift value for EUSART_DTXDAT               */
#define _EUSART_DTXDATCFG_DTXDAT_MASK               0xFFFFUL                                /**< Bit mask for EUSART_DTXDAT                  */
#define _EUSART_DTXDATCFG_DTXDAT_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for EUSART_DTXDATCFG           */
#define EUSART_DTXDATCFG_DTXDAT_DEFAULT             (_EUSART_DTXDATCFG_DTXDAT_DEFAULT << 0) /**< Shifted mode DEFAULT for EUSART_DTXDATCFG   */

/* Bit fields for EUSART IRHFCFG */
#define _EUSART_IRHFCFG_RESETVALUE                  0x00000000UL                            /**< Default value for EUSART_IRHFCFG            */
#define _EUSART_IRHFCFG_MASK                        0x0000000FUL                            /**< Mask for EUSART_IRHFCFG                     */
#define EUSART_IRHFCFG_IRHFEN                       (0x1UL << 0)                            /**< Enable IrDA Module                          */
#define _EUSART_IRHFCFG_IRHFEN_SHIFT                0                                       /**< Shift value for EUSART_IRHFEN               */
#define _EUSART_IRHFCFG_IRHFEN_MASK                 0x1UL                                   /**< Bit mask for EUSART_IRHFEN                  */
#define _EUSART_IRHFCFG_IRHFEN_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EUSART_IRHFCFG             */
#define EUSART_IRHFCFG_IRHFEN_DEFAULT               (_EUSART_IRHFCFG_IRHFEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for EUSART_IRHFCFG     */
#define _EUSART_IRHFCFG_IRHFPW_SHIFT                1                                       /**< Shift value for EUSART_IRHFPW               */
#define _EUSART_IRHFCFG_IRHFPW_MASK                 0x6UL                                   /**< Bit mask for EUSART_IRHFPW                  */
#define _EUSART_IRHFCFG_IRHFPW_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EUSART_IRHFCFG             */
#define _EUSART_IRHFCFG_IRHFPW_ONE                  0x00000000UL                            /**< Mode ONE for EUSART_IRHFCFG                 */
#define _EUSART_IRHFCFG_IRHFPW_TWO                  0x00000001UL                            /**< Mode TWO for EUSART_IRHFCFG                 */
#define _EUSART_IRHFCFG_IRHFPW_THREE                0x00000002UL                            /**< Mode THREE for EUSART_IRHFCFG               */
#define _EUSART_IRHFCFG_IRHFPW_FOUR                 0x00000003UL                            /**< Mode FOUR for EUSART_IRHFCFG                */
#define EUSART_IRHFCFG_IRHFPW_DEFAULT               (_EUSART_IRHFCFG_IRHFPW_DEFAULT << 1)   /**< Shifted mode DEFAULT for EUSART_IRHFCFG     */
#define EUSART_IRHFCFG_IRHFPW_ONE                   (_EUSART_IRHFCFG_IRHFPW_ONE << 1)       /**< Shifted mode ONE for EUSART_IRHFCFG         */
#define EUSART_IRHFCFG_IRHFPW_TWO                   (_EUSART_IRHFCFG_IRHFPW_TWO << 1)       /**< Shifted mode TWO for EUSART_IRHFCFG         */
#define EUSART_IRHFCFG_IRHFPW_THREE                 (_EUSART_IRHFCFG_IRHFPW_THREE << 1)     /**< Shifted mode THREE for EUSART_IRHFCFG       */
#define EUSART_IRHFCFG_IRHFPW_FOUR                  (_EUSART_IRHFCFG_IRHFPW_FOUR << 1)      /**< Shifted mode FOUR for EUSART_IRHFCFG        */
#define EUSART_IRHFCFG_IRHFFILT                     (0x1UL << 3)                            /**< IrDA RX Filter                              */
#define _EUSART_IRHFCFG_IRHFFILT_SHIFT              3                                       /**< Shift value for EUSART_IRHFFILT             */
#define _EUSART_IRHFCFG_IRHFFILT_MASK               0x8UL                                   /**< Bit mask for EUSART_IRHFFILT                */
#define _EUSART_IRHFCFG_IRHFFILT_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for EUSART_IRHFCFG             */
#define _EUSART_IRHFCFG_IRHFFILT_DISABLE            0x00000000UL                            /**< Mode DISABLE for EUSART_IRHFCFG             */
#define _EUSART_IRHFCFG_IRHFFILT_ENABLE             0x00000001UL                            /**< Mode ENABLE for EUSART_IRHFCFG              */
#define EUSART_IRHFCFG_IRHFFILT_DEFAULT             (_EUSART_IRHFCFG_IRHFFILT_DEFAULT << 3) /**< Shifted mode DEFAULT for EUSART_IRHFCFG     */
#define EUSART_IRHFCFG_IRHFFILT_DISABLE             (_EUSART_IRHFCFG_IRHFFILT_DISABLE << 3) /**< Shifted mode DISABLE for EUSART_IRHFCFG     */
#define EUSART_IRHFCFG_IRHFFILT_ENABLE              (_EUSART_IRHFCFG_IRHFFILT_ENABLE << 3)  /**< Shifted mode ENABLE for EUSART_IRHFCFG      */

/* Bit fields for EUSART IRLFCFG */
#define _EUSART_IRLFCFG_RESETVALUE                  0x00000000UL                          /**< Default value for EUSART_IRLFCFG            */
#define _EUSART_IRLFCFG_MASK                        0x00000001UL                          /**< Mask for EUSART_IRLFCFG                     */
#define EUSART_IRLFCFG_IRLFEN                       (0x1UL << 0)                          /**< Pulse Generator/Extender Enable             */
#define _EUSART_IRLFCFG_IRLFEN_SHIFT                0                                     /**< Shift value for EUSART_IRLFEN               */
#define _EUSART_IRLFCFG_IRLFEN_MASK                 0x1UL                                 /**< Bit mask for EUSART_IRLFEN                  */
#define _EUSART_IRLFCFG_IRLFEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for EUSART_IRLFCFG             */
#define EUSART_IRLFCFG_IRLFEN_DEFAULT               (_EUSART_IRLFCFG_IRLFEN_DEFAULT << 0) /**< Shifted mode DEFAULT for EUSART_IRLFCFG     */

/* Bit fields for EUSART TIMINGCFG */
#define _EUSART_TIMINGCFG_RESETVALUE                0x00050000UL                                  /**< Default value for EUSART_TIMINGCFG          */
#define _EUSART_TIMINGCFG_MASK                      0x000F7773UL                                  /**< Mask for EUSART_TIMINGCFG                   */
#define _EUSART_TIMINGCFG_TXDELAY_SHIFT             0                                             /**< Shift value for EUSART_TXDELAY              */
#define _EUSART_TIMINGCFG_TXDELAY_MASK              0x3UL                                         /**< Bit mask for EUSART_TXDELAY                 */
#define _EUSART_TIMINGCFG_TXDELAY_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for EUSART_TIMINGCFG           */
#define _EUSART_TIMINGCFG_TXDELAY_NONE              0x00000000UL                                  /**< Mode NONE for EUSART_TIMINGCFG              */
#define _EUSART_TIMINGCFG_TXDELAY_SINGLE            0x00000001UL                                  /**< Mode SINGLE for EUSART_TIMINGCFG            */
#define _EUSART_TIMINGCFG_TXDELAY_DOUBLE            0x00000002UL                                  /**< Mode DOUBLE for EUSART_TIMINGCFG            */
#define _EUSART_TIMINGCFG_TXDELAY_TRIPPLE           0x00000003UL                                  /**< Mode TRIPPLE for EUSART_TIMINGCFG           */
#define EUSART_TIMINGCFG_TXDELAY_DEFAULT            (_EUSART_TIMINGCFG_TXDELAY_DEFAULT << 0)      /**< Shifted mode DEFAULT for EUSART_TIMINGCFG   */
#define EUSART_TIMINGCFG_TXDELAY_NONE               (_EUSART_TIMINGCFG_TXDELAY_NONE << 0)         /**< Shifted mode NONE for EUSART_TIMINGCFG      */
#define EUSART_TIMINGCFG_TXDELAY_SINGLE             (_EUSART_TIMINGCFG_TXDELAY_SINGLE << 0)       /**< Shifted mode SINGLE for EUSART_TIMINGCFG    */
#define EUSART_TIMINGCFG_TXDELAY_DOUBLE             (_EUSART_TIMINGCFG_TXDELAY_DOUBLE << 0)       /**< Shifted mode DOUBLE for EUSART_TIMINGCFG    */
#define EUSART_TIMINGCFG_TXDELAY_TRIPPLE            (_EUSART_TIMINGCFG_TXDELAY_TRIPPLE << 0)      /**< Shifted mode TRIPPLE for EUSART_TIMINGCFG   */
#define _EUSART_TIMINGCFG_CSSETUP_SHIFT             4                                             /**< Shift value for EUSART_CSSETUP              */
#define _EUSART_TIMINGCFG_CSSETUP_MASK              0x70UL                                        /**< Bit mask for EUSART_CSSETUP                 */
#define _EUSART_TIMINGCFG_CSSETUP_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for EUSART_TIMINGCFG           */
#define _EUSART_TIMINGCFG_CSSETUP_ZERO              0x00000000UL                                  /**< Mode ZERO for EUSART_TIMINGCFG              */
#define _EUSART_TIMINGCFG_CSSETUP_ONE               0x00000001UL                                  /**< Mode ONE for EUSART_TIMINGCFG               */
#define _EUSART_TIMINGCFG_CSSETUP_TWO               0x00000002UL                                  /**< Mode TWO for EUSART_TIMINGCFG               */
#define _EUSART_TIMINGCFG_CSSETUP_THREE             0x00000003UL                                  /**< Mode THREE for EUSART_TIMINGCFG             */
#define _EUSART_TIMINGCFG_CSSETUP_FOUR              0x00000004UL                                  /**< Mode FOUR for EUSART_TIMINGCFG              */
#define _EUSART_TIMINGCFG_CSSETUP_FIVE              0x00000005UL                                  /**< Mode FIVE for EUSART_TIMINGCFG              */
#define _EUSART_TIMINGCFG_CSSETUP_SIX               0x00000006UL                                  /**< Mode SIX for EUSART_TIMINGCFG               */
#define _EUSART_TIMINGCFG_CSSETUP_SEVEN             0x00000007UL                                  /**< Mode SEVEN for EUSART_TIMINGCFG             */
#define EUSART_TIMINGCFG_CSSETUP_DEFAULT            (_EUSART_TIMINGCFG_CSSETUP_DEFAULT << 4)      /**< Shifted mode DEFAULT for EUSART_TIMINGCFG   */
#define EUSART_TIMINGCFG_CSSETUP_ZERO               (_EUSART_TIMINGCFG_CSSETUP_ZERO << 4)         /**< Shifted mode ZERO for EUSART_TIMINGCFG      */
#define EUSART_TIMINGCFG_CSSETUP_ONE                (_EUSART_TIMINGCFG_CSSETUP_ONE << 4)          /**< Shifted mode ONE for EUSART_TIMINGCFG       */
#define EUSART_TIMINGCFG_CSSETUP_TWO                (_EUSART_TIMINGCFG_CSSETUP_TWO << 4)          /**< Shifted mode TWO for EUSART_TIMINGCFG       */
#define EUSART_TIMINGCFG_CSSETUP_THREE              (_EUSART_TIMINGCFG_CSSETUP_THREE << 4)        /**< Shifted mode THREE for EUSART_TIMINGCFG     */
#define EUSART_TIMINGCFG_CSSETUP_FOUR               (_EUSART_TIMINGCFG_CSSETUP_FOUR << 4)         /**< Shifted mode FOUR for EUSART_TIMINGCFG      */
#define EUSART_TIMINGCFG_CSSETUP_FIVE               (_EUSART_TIMINGCFG_CSSETUP_FIVE << 4)         /**< Shifted mode FIVE for EUSART_TIMINGCFG      */
#define EUSART_TIMINGCFG_CSSETUP_SIX                (_EUSART_TIMINGCFG_CSSETUP_SIX << 4)          /**< Shifted mode SIX for EUSART_TIMINGCFG       */
#define EUSART_TIMINGCFG_CSSETUP_SEVEN              (_EUSART_TIMINGCFG_CSSETUP_SEVEN << 4)        /**< Shifted mode SEVEN for EUSART_TIMINGCFG     */
#define _EUSART_TIMINGCFG_CSHOLD_SHIFT              8                                             /**< Shift value for EUSART_CSHOLD               */
#define _EUSART_TIMINGCFG_CSHOLD_MASK               0x700UL                                       /**< Bit mask for EUSART_CSHOLD                  */
#define _EUSART_TIMINGCFG_CSHOLD_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for EUSART_TIMINGCFG           */
#define _EUSART_TIMINGCFG_CSHOLD_ZERO               0x00000000UL                                  /**< Mode ZERO for EUSART_TIMINGCFG              */
#define _EUSART_TIMINGCFG_CSHOLD_ONE                0x00000001UL                                  /**< Mode ONE for EUSART_TIMINGCFG               */
#define _EUSART_TIMINGCFG_CSHOLD_TWO                0x00000002UL                                  /**< Mode TWO for EUSART_TIMINGCFG               */
#define _EUSART_TIMINGCFG_CSHOLD_THREE              0x00000003UL                                  /**< Mode THREE for EUSART_TIMINGCFG             */
#define _EUSART_TIMINGCFG_CSHOLD_FOUR               0x00000004UL                                  /**< Mode FOUR for EUSART_TIMINGCFG              */
#define _EUSART_TIMINGCFG_CSHOLD_FIVE               0x00000005UL                                  /**< Mode FIVE for EUSART_TIMINGCFG              */
#define _EUSART_TIMINGCFG_CSHOLD_SIX                0x00000006UL                                  /**< Mode SIX for EUSART_TIMINGCFG               */
#define _EUSART_TIMINGCFG_CSHOLD_SEVEN              0x00000007UL                                  /**< Mode SEVEN for EUSART_TIMINGCFG             */
#define EUSART_TIMINGCFG_CSHOLD_DEFAULT             (_EUSART_TIMINGCFG_CSHOLD_DEFAULT << 8)       /**< Shifted mode DEFAULT for EUSART_TIMINGCFG   */
#define EUSART_TIMINGCFG_CSHOLD_ZERO                (_EUSART_TIMINGCFG_CSHOLD_ZERO << 8)          /**< Shifted mode ZERO for EUSART_TIMINGCFG      */
#define EUSART_TIMINGCFG_CSHOLD_ONE                 (_EUSART_TIMINGCFG_CSHOLD_ONE << 8)           /**< Shifted mode ONE for EUSART_TIMINGCFG       */
#define EUSART_TIMINGCFG_CSHOLD_TWO                 (_EUSART_TIMINGCFG_CSHOLD_TWO << 8)           /**< Shifted mode TWO for EUSART_TIMINGCFG       */
#define EUSART_TIMINGCFG_CSHOLD_THREE               (_EUSART_TIMINGCFG_CSHOLD_THREE << 8)         /**< Shifted mode THREE for EUSART_TIMINGCFG     */
#define EUSART_TIMINGCFG_CSHOLD_FOUR                (_EUSART_TIMINGCFG_CSHOLD_FOUR << 8)          /**< Shifted mode FOUR for EUSART_TIMINGCFG      */
#define EUSART_TIMINGCFG_CSHOLD_FIVE                (_EUSART_TIMINGCFG_CSHOLD_FIVE << 8)          /**< Shifted mode FIVE for EUSART_TIMINGCFG      */
#define EUSART_TIMINGCFG_CSHOLD_SIX                 (_EUSART_TIMINGCFG_CSHOLD_SIX << 8)           /**< Shifted mode SIX for EUSART_TIMINGCFG       */
#define EUSART_TIMINGCFG_CSHOLD_SEVEN               (_EUSART_TIMINGCFG_CSHOLD_SEVEN << 8)         /**< Shifted mode SEVEN for EUSART_TIMINGCFG     */
#define _EUSART_TIMINGCFG_ICS_SHIFT                 12                                            /**< Shift value for EUSART_ICS                  */
#define _EUSART_TIMINGCFG_ICS_MASK                  0x7000UL                                      /**< Bit mask for EUSART_ICS                     */
#define _EUSART_TIMINGCFG_ICS_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for EUSART_TIMINGCFG           */
#define _EUSART_TIMINGCFG_ICS_ZERO                  0x00000000UL                                  /**< Mode ZERO for EUSART_TIMINGCFG              */
#define _EUSART_TIMINGCFG_ICS_ONE                   0x00000001UL                                  /**< Mode ONE for EUSART_TIMINGCFG               */
#define _EUSART_TIMINGCFG_ICS_TWO                   0x00000002UL                                  /**< Mode TWO for EUSART_TIMINGCFG               */
#define _EUSART_TIMINGCFG_ICS_THREE                 0x00000003UL                                  /**< Mode THREE for EUSART_TIMINGCFG             */
#define _EUSART_TIMINGCFG_ICS_FOUR                  0x00000004UL                                  /**< Mode FOUR for EUSART_TIMINGCFG              */
#define _EUSART_TIMINGCFG_ICS_FIVE                  0x00000005UL                                  /**< Mode FIVE for EUSART_TIMINGCFG              */
#define _EUSART_TIMINGCFG_ICS_SIX                   0x00000006UL                                  /**< Mode SIX for EUSART_TIMINGCFG               */
#define _EUSART_TIMINGCFG_ICS_SEVEN                 0x00000007UL                                  /**< Mode SEVEN for EUSART_TIMINGCFG             */
#define EUSART_TIMINGCFG_ICS_DEFAULT                (_EUSART_TIMINGCFG_ICS_DEFAULT << 12)         /**< Shifted mode DEFAULT for EUSART_TIMINGCFG   */
#define EUSART_TIMINGCFG_ICS_ZERO                   (_EUSART_TIMINGCFG_ICS_ZERO << 12)            /**< Shifted mode ZERO for EUSART_TIMINGCFG      */
#define EUSART_TIMINGCFG_ICS_ONE                    (_EUSART_TIMINGCFG_ICS_ONE << 12)             /**< Shifted mode ONE for EUSART_TIMINGCFG       */
#define EUSART_TIMINGCFG_ICS_TWO                    (_EUSART_TIMINGCFG_ICS_TWO << 12)             /**< Shifted mode TWO for EUSART_TIMINGCFG       */
#define EUSART_TIMINGCFG_ICS_THREE                  (_EUSART_TIMINGCFG_ICS_THREE << 12)           /**< Shifted mode THREE for EUSART_TIMINGCFG     */
#define EUSART_TIMINGCFG_ICS_FOUR                   (_EUSART_TIMINGCFG_ICS_FOUR << 12)            /**< Shifted mode FOUR for EUSART_TIMINGCFG      */
#define EUSART_TIMINGCFG_ICS_FIVE                   (_EUSART_TIMINGCFG_ICS_FIVE << 12)            /**< Shifted mode FIVE for EUSART_TIMINGCFG      */
#define EUSART_TIMINGCFG_ICS_SIX                    (_EUSART_TIMINGCFG_ICS_SIX << 12)             /**< Shifted mode SIX for EUSART_TIMINGCFG       */
#define EUSART_TIMINGCFG_ICS_SEVEN                  (_EUSART_TIMINGCFG_ICS_SEVEN << 12)           /**< Shifted mode SEVEN for EUSART_TIMINGCFG     */
#define _EUSART_TIMINGCFG_SETUPWINDOW_SHIFT         16                                            /**< Shift value for EUSART_SETUPWINDOW          */
#define _EUSART_TIMINGCFG_SETUPWINDOW_MASK          0xF0000UL                                     /**< Bit mask for EUSART_SETUPWINDOW             */
#define _EUSART_TIMINGCFG_SETUPWINDOW_DEFAULT       0x00000005UL                                  /**< Mode DEFAULT for EUSART_TIMINGCFG           */
#define EUSART_TIMINGCFG_SETUPWINDOW_DEFAULT        (_EUSART_TIMINGCFG_SETUPWINDOW_DEFAULT << 16) /**< Shifted mode DEFAULT for EUSART_TIMINGCFG   */

/* Bit fields for EUSART STARTFRAMECFG */
#define _EUSART_STARTFRAMECFG_RESETVALUE            0x00000000UL                                    /**< Default value for EUSART_STARTFRAMECFG      */
#define _EUSART_STARTFRAMECFG_MASK                  0x000001FFUL                                    /**< Mask for EUSART_STARTFRAMECFG               */
#define _EUSART_STARTFRAMECFG_STARTFRAME_SHIFT      0                                               /**< Shift value for EUSART_STARTFRAME           */
#define _EUSART_STARTFRAMECFG_STARTFRAME_MASK       0x1FFUL                                         /**< Bit mask for EUSART_STARTFRAME              */
#define _EUSART_STARTFRAMECFG_STARTFRAME_DEFAULT    0x00000000UL                                    /**< Mode DEFAULT for EUSART_STARTFRAMECFG       */
#define EUSART_STARTFRAMECFG_STARTFRAME_DEFAULT     (_EUSART_STARTFRAMECFG_STARTFRAME_DEFAULT << 0) /**< Shifted mode DEFAULT for EUSART_STARTFRAMECFG*/

/* Bit fields for EUSART SIGFRAMECFG */
#define _EUSART_SIGFRAMECFG_RESETVALUE              0x00000000UL                                /**< Default value for EUSART_SIGFRAMECFG        */
#define _EUSART_SIGFRAMECFG_MASK                    0x000001FFUL                                /**< Mask for EUSART_SIGFRAMECFG                 */
#define _EUSART_SIGFRAMECFG_SIGFRAME_SHIFT          0                                           /**< Shift value for EUSART_SIGFRAME             */
#define _EUSART_SIGFRAMECFG_SIGFRAME_MASK           0x1FFUL                                     /**< Bit mask for EUSART_SIGFRAME                */
#define _EUSART_SIGFRAMECFG_SIGFRAME_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for EUSART_SIGFRAMECFG         */
#define EUSART_SIGFRAMECFG_SIGFRAME_DEFAULT         (_EUSART_SIGFRAMECFG_SIGFRAME_DEFAULT << 0) /**< Shifted mode DEFAULT for EUSART_SIGFRAMECFG */

/* Bit fields for EUSART CLKDIV */
#define _EUSART_CLKDIV_RESETVALUE                   0x00000000UL                        /**< Default value for EUSART_CLKDIV             */
#define _EUSART_CLKDIV_MASK                         0x007FFFF8UL                        /**< Mask for EUSART_CLKDIV                      */
#define _EUSART_CLKDIV_DIV_SHIFT                    3                                   /**< Shift value for EUSART_DIV                  */
#define _EUSART_CLKDIV_DIV_MASK                     0x7FFFF8UL                          /**< Bit mask for EUSART_DIV                     */
#define _EUSART_CLKDIV_DIV_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for EUSART_CLKDIV              */
#define EUSART_CLKDIV_DIV_DEFAULT                   (_EUSART_CLKDIV_DIV_DEFAULT << 3)   /**< Shifted mode DEFAULT for EUSART_CLKDIV      */

/* Bit fields for EUSART TRIGCTRL */
#define _EUSART_TRIGCTRL_RESETVALUE                 0x00000000UL                              /**< Default value for EUSART_TRIGCTRL           */
#define _EUSART_TRIGCTRL_MASK                       0x00000007UL                              /**< Mask for EUSART_TRIGCTRL                    */
#define EUSART_TRIGCTRL_RXTEN                       (0x1UL << 0)                              /**< Receive Trigger Enable                      */
#define _EUSART_TRIGCTRL_RXTEN_SHIFT                0                                         /**< Shift value for EUSART_RXTEN                */
#define _EUSART_TRIGCTRL_RXTEN_MASK                 0x1UL                                     /**< Bit mask for EUSART_RXTEN                   */
#define _EUSART_TRIGCTRL_RXTEN_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for EUSART_TRIGCTRL            */
#define EUSART_TRIGCTRL_RXTEN_DEFAULT               (_EUSART_TRIGCTRL_RXTEN_DEFAULT << 0)     /**< Shifted mode DEFAULT for EUSART_TRIGCTRL    */
#define EUSART_TRIGCTRL_TXTEN                       (0x1UL << 1)                              /**< Transmit Trigger Enable                     */
#define _EUSART_TRIGCTRL_TXTEN_SHIFT                1                                         /**< Shift value for EUSART_TXTEN                */
#define _EUSART_TRIGCTRL_TXTEN_MASK                 0x2UL                                     /**< Bit mask for EUSART_TXTEN                   */
#define _EUSART_TRIGCTRL_TXTEN_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for EUSART_TRIGCTRL            */
#define EUSART_TRIGCTRL_TXTEN_DEFAULT               (_EUSART_TRIGCTRL_TXTEN_DEFAULT << 1)     /**< Shifted mode DEFAULT for EUSART_TRIGCTRL    */
#define EUSART_TRIGCTRL_AUTOTXTEN                   (0x1UL << 2)                              /**< AUTOTX Trigger Enable                       */
#define _EUSART_TRIGCTRL_AUTOTXTEN_SHIFT            2                                         /**< Shift value for EUSART_AUTOTXTEN            */
#define _EUSART_TRIGCTRL_AUTOTXTEN_MASK             0x4UL                                     /**< Bit mask for EUSART_AUTOTXTEN               */
#define _EUSART_TRIGCTRL_AUTOTXTEN_DEFAULT          0x00000000UL                              /**< Mode DEFAULT for EUSART_TRIGCTRL            */
#define EUSART_TRIGCTRL_AUTOTXTEN_DEFAULT           (_EUSART_TRIGCTRL_AUTOTXTEN_DEFAULT << 2) /**< Shifted mode DEFAULT for EUSART_TRIGCTRL    */

/* Bit fields for EUSART CMD */
#define _EUSART_CMD_RESETVALUE                      0x00000000UL                          /**< Default value for EUSART_CMD                */
#define _EUSART_CMD_MASK                            0x000001FFUL                          /**< Mask for EUSART_CMD                         */
#define EUSART_CMD_RXEN                             (0x1UL << 0)                          /**< Receiver Enable                             */
#define _EUSART_CMD_RXEN_SHIFT                      0                                     /**< Shift value for EUSART_RXEN                 */
#define _EUSART_CMD_RXEN_MASK                       0x1UL                                 /**< Bit mask for EUSART_RXEN                    */
#define _EUSART_CMD_RXEN_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for EUSART_CMD                 */
#define EUSART_CMD_RXEN_DEFAULT                     (_EUSART_CMD_RXEN_DEFAULT << 0)       /**< Shifted mode DEFAULT for EUSART_CMD         */
#define EUSART_CMD_RXDIS                            (0x1UL << 1)                          /**< Receiver Disable                            */
#define _EUSART_CMD_RXDIS_SHIFT                     1                                     /**< Shift value for EUSART_RXDIS                */
#define _EUSART_CMD_RXDIS_MASK                      0x2UL                                 /**< Bit mask for EUSART_RXDIS                   */
#define _EUSART_CMD_RXDIS_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for EUSART_CMD                 */
#define EUSART_CMD_RXDIS_DEFAULT                    (_EUSART_CMD_RXDIS_DEFAULT << 1)      /**< Shifted mode DEFAULT for EUSART_CMD         */
#define EUSART_CMD_TXEN                             (0x1UL << 2)                          /**< Transmitter Enable                          */
#define _EUSART_CMD_TXEN_SHIFT                      2                                     /**< Shift value for EUSART_TXEN                 */
#define _EUSART_CMD_TXEN_MASK                       0x4UL                                 /**< Bit mask for EUSART_TXEN                    */
#define _EUSART_CMD_TXEN_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for EUSART_CMD                 */
#define EUSART_CMD_TXEN_DEFAULT                     (_EUSART_CMD_TXEN_DEFAULT << 2)       /**< Shifted mode DEFAULT for EUSART_CMD         */
#define EUSART_CMD_TXDIS                            (0x1UL << 3)                          /**< Transmitter Disable                         */
#define _EUSART_CMD_TXDIS_SHIFT                     3                                     /**< Shift value for EUSART_TXDIS                */
#define _EUSART_CMD_TXDIS_MASK                      0x8UL                                 /**< Bit mask for EUSART_TXDIS                   */
#define _EUSART_CMD_TXDIS_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for EUSART_CMD                 */
#define EUSART_CMD_TXDIS_DEFAULT                    (_EUSART_CMD_TXDIS_DEFAULT << 3)      /**< Shifted mode DEFAULT for EUSART_CMD         */
#define EUSART_CMD_RXBLOCKEN                        (0x1UL << 4)                          /**< Receiver Block Enable                       */
#define _EUSART_CMD_RXBLOCKEN_SHIFT                 4                                     /**< Shift value for EUSART_RXBLOCKEN            */
#define _EUSART_CMD_RXBLOCKEN_MASK                  0x10UL                                /**< Bit mask for EUSART_RXBLOCKEN               */
#define _EUSART_CMD_RXBLOCKEN_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for EUSART_CMD                 */
#define EUSART_CMD_RXBLOCKEN_DEFAULT                (_EUSART_CMD_RXBLOCKEN_DEFAULT << 4)  /**< Shifted mode DEFAULT for EUSART_CMD         */
#define EUSART_CMD_RXBLOCKDIS                       (0x1UL << 5)                          /**< Receiver Block Disable                      */
#define _EUSART_CMD_RXBLOCKDIS_SHIFT                5                                     /**< Shift value for EUSART_RXBLOCKDIS           */
#define _EUSART_CMD_RXBLOCKDIS_MASK                 0x20UL                                /**< Bit mask for EUSART_RXBLOCKDIS              */
#define _EUSART_CMD_RXBLOCKDIS_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for EUSART_CMD                 */
#define EUSART_CMD_RXBLOCKDIS_DEFAULT               (_EUSART_CMD_RXBLOCKDIS_DEFAULT << 5) /**< Shifted mode DEFAULT for EUSART_CMD         */
#define EUSART_CMD_TXTRIEN                          (0x1UL << 6)                          /**< Transmitter Tristate Enable                 */
#define _EUSART_CMD_TXTRIEN_SHIFT                   6                                     /**< Shift value for EUSART_TXTRIEN              */
#define _EUSART_CMD_TXTRIEN_MASK                    0x40UL                                /**< Bit mask for EUSART_TXTRIEN                 */
#define _EUSART_CMD_TXTRIEN_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for EUSART_CMD                 */
#define EUSART_CMD_TXTRIEN_DEFAULT                  (_EUSART_CMD_TXTRIEN_DEFAULT << 6)    /**< Shifted mode DEFAULT for EUSART_CMD         */
#define EUSART_CMD_TXTRIDIS                         (0x1UL << 7)                          /**< Transmitter Tristate Disable                */
#define _EUSART_CMD_TXTRIDIS_SHIFT                  7                                     /**< Shift value for EUSART_TXTRIDIS             */
#define _EUSART_CMD_TXTRIDIS_MASK                   0x80UL                                /**< Bit mask for EUSART_TXTRIDIS                */
#define _EUSART_CMD_TXTRIDIS_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for EUSART_CMD                 */
#define EUSART_CMD_TXTRIDIS_DEFAULT                 (_EUSART_CMD_TXTRIDIS_DEFAULT << 7)   /**< Shifted mode DEFAULT for EUSART_CMD         */
#define EUSART_CMD_CLEARTX                          (0x1UL << 8)                          /**< Clear TX FIFO                               */
#define _EUSART_CMD_CLEARTX_SHIFT                   8                                     /**< Shift value for EUSART_CLEARTX              */
#define _EUSART_CMD_CLEARTX_MASK                    0x100UL                               /**< Bit mask for EUSART_CLEARTX                 */
#define _EUSART_CMD_CLEARTX_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for EUSART_CMD                 */
#define EUSART_CMD_CLEARTX_DEFAULT                  (_EUSART_CMD_CLEARTX_DEFAULT << 8)    /**< Shifted mode DEFAULT for EUSART_CMD         */

/* Bit fields for EUSART RXDATA */
#define _EUSART_RXDATA_RESETVALUE                   0x00000000UL                         /**< Default value for EUSART_RXDATA             */
#define _EUSART_RXDATA_MASK                         0x0000FFFFUL                         /**< Mask for EUSART_RXDATA                      */
#define _EUSART_RXDATA_RXDATA_SHIFT                 0                                    /**< Shift value for EUSART_RXDATA               */
#define _EUSART_RXDATA_RXDATA_MASK                  0xFFFFUL                             /**< Bit mask for EUSART_RXDATA                  */
#define _EUSART_RXDATA_RXDATA_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for EUSART_RXDATA              */
#define EUSART_RXDATA_RXDATA_DEFAULT                (_EUSART_RXDATA_RXDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for EUSART_RXDATA      */

/* Bit fields for EUSART RXDATAP */
#define _EUSART_RXDATAP_RESETVALUE                  0x00000000UL                           /**< Default value for EUSART_RXDATAP            */
#define _EUSART_RXDATAP_MASK                        0x0000FFFFUL                           /**< Mask for EUSART_RXDATAP                     */
#define _EUSART_RXDATAP_RXDATAP_SHIFT               0                                      /**< Shift value for EUSART_RXDATAP              */
#define _EUSART_RXDATAP_RXDATAP_MASK                0xFFFFUL                               /**< Bit mask for EUSART_RXDATAP                 */
#define _EUSART_RXDATAP_RXDATAP_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for EUSART_RXDATAP             */
#define EUSART_RXDATAP_RXDATAP_DEFAULT              (_EUSART_RXDATAP_RXDATAP_DEFAULT << 0) /**< Shifted mode DEFAULT for EUSART_RXDATAP     */

/* Bit fields for EUSART TXDATA */
#define _EUSART_TXDATA_RESETVALUE                   0x00000000UL                         /**< Default value for EUSART_TXDATA             */
#define _EUSART_TXDATA_MASK                         0x0000FFFFUL                         /**< Mask for EUSART_TXDATA                      */
#define _EUSART_TXDATA_TXDATA_SHIFT                 0                                    /**< Shift value for EUSART_TXDATA               */
#define _EUSART_TXDATA_TXDATA_MASK                  0xFFFFUL                             /**< Bit mask for EUSART_TXDATA                  */
#define _EUSART_TXDATA_TXDATA_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for EUSART_TXDATA              */
#define EUSART_TXDATA_TXDATA_DEFAULT                (_EUSART_TXDATA_TXDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for EUSART_TXDATA      */

/* Bit fields for EUSART STATUS */
#define _EUSART_STATUS_RESETVALUE                   0x00003040UL                                /**< Default value for EUSART_STATUS             */
#define _EUSART_STATUS_MASK                         0x031F31FBUL                                /**< Mask for EUSART_STATUS                      */
#define EUSART_STATUS_RXENS                         (0x1UL << 0)                                /**< Receiver Enable Status                      */
#define _EUSART_STATUS_RXENS_SHIFT                  0                                           /**< Shift value for EUSART_RXENS                */
#define _EUSART_STATUS_RXENS_MASK                   0x1UL                                       /**< Bit mask for EUSART_RXENS                   */
#define _EUSART_STATUS_RXENS_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_RXENS_DEFAULT                 (_EUSART_STATUS_RXENS_DEFAULT << 0)         /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define EUSART_STATUS_TXENS                         (0x1UL << 1)                                /**< Transmitter Enable Status                   */
#define _EUSART_STATUS_TXENS_SHIFT                  1                                           /**< Shift value for EUSART_TXENS                */
#define _EUSART_STATUS_TXENS_MASK                   0x2UL                                       /**< Bit mask for EUSART_TXENS                   */
#define _EUSART_STATUS_TXENS_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_TXENS_DEFAULT                 (_EUSART_STATUS_TXENS_DEFAULT << 1)         /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define EUSART_STATUS_RXBLOCK                       (0x1UL << 3)                                /**< Block Incoming Data                         */
#define _EUSART_STATUS_RXBLOCK_SHIFT                3                                           /**< Shift value for EUSART_RXBLOCK              */
#define _EUSART_STATUS_RXBLOCK_MASK                 0x8UL                                       /**< Bit mask for EUSART_RXBLOCK                 */
#define _EUSART_STATUS_RXBLOCK_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_RXBLOCK_DEFAULT               (_EUSART_STATUS_RXBLOCK_DEFAULT << 3)       /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define EUSART_STATUS_TXTRI                         (0x1UL << 4)                                /**< Transmitter Tristated                       */
#define _EUSART_STATUS_TXTRI_SHIFT                  4                                           /**< Shift value for EUSART_TXTRI                */
#define _EUSART_STATUS_TXTRI_MASK                   0x10UL                                      /**< Bit mask for EUSART_TXTRI                   */
#define _EUSART_STATUS_TXTRI_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_TXTRI_DEFAULT                 (_EUSART_STATUS_TXTRI_DEFAULT << 4)         /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define EUSART_STATUS_TXC                           (0x1UL << 5)                                /**< TX Complete                                 */
#define _EUSART_STATUS_TXC_SHIFT                    5                                           /**< Shift value for EUSART_TXC                  */
#define _EUSART_STATUS_TXC_MASK                     0x20UL                                      /**< Bit mask for EUSART_TXC                     */
#define _EUSART_STATUS_TXC_DEFAULT                  0x00000000UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_TXC_DEFAULT                   (_EUSART_STATUS_TXC_DEFAULT << 5)           /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define EUSART_STATUS_TXFL                          (0x1UL << 6)                                /**< TX FIFO Level                               */
#define _EUSART_STATUS_TXFL_SHIFT                   6                                           /**< Shift value for EUSART_TXFL                 */
#define _EUSART_STATUS_TXFL_MASK                    0x40UL                                      /**< Bit mask for EUSART_TXFL                    */
#define _EUSART_STATUS_TXFL_DEFAULT                 0x00000001UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_TXFL_DEFAULT                  (_EUSART_STATUS_TXFL_DEFAULT << 6)          /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define EUSART_STATUS_RXFL                          (0x1UL << 7)                                /**< RX FIFO Level                               */
#define _EUSART_STATUS_RXFL_SHIFT                   7                                           /**< Shift value for EUSART_RXFL                 */
#define _EUSART_STATUS_RXFL_MASK                    0x80UL                                      /**< Bit mask for EUSART_RXFL                    */
#define _EUSART_STATUS_RXFL_DEFAULT                 0x00000000UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_RXFL_DEFAULT                  (_EUSART_STATUS_RXFL_DEFAULT << 7)          /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define EUSART_STATUS_RXFULL                        (0x1UL << 8)                                /**< RX FIFO Full                                */
#define _EUSART_STATUS_RXFULL_SHIFT                 8                                           /**< Shift value for EUSART_RXFULL               */
#define _EUSART_STATUS_RXFULL_MASK                  0x100UL                                     /**< Bit mask for EUSART_RXFULL                  */
#define _EUSART_STATUS_RXFULL_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_RXFULL_DEFAULT                (_EUSART_STATUS_RXFULL_DEFAULT << 8)        /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define EUSART_STATUS_RXIDLE                        (0x1UL << 12)                               /**< RX Idle                                     */
#define _EUSART_STATUS_RXIDLE_SHIFT                 12                                          /**< Shift value for EUSART_RXIDLE               */
#define _EUSART_STATUS_RXIDLE_MASK                  0x1000UL                                    /**< Bit mask for EUSART_RXIDLE                  */
#define _EUSART_STATUS_RXIDLE_DEFAULT               0x00000001UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_RXIDLE_DEFAULT                (_EUSART_STATUS_RXIDLE_DEFAULT << 12)       /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define EUSART_STATUS_TXIDLE                        (0x1UL << 13)                               /**< TX Idle                                     */
#define _EUSART_STATUS_TXIDLE_SHIFT                 13                                          /**< Shift value for EUSART_TXIDLE               */
#define _EUSART_STATUS_TXIDLE_MASK                  0x2000UL                                    /**< Bit mask for EUSART_TXIDLE                  */
#define _EUSART_STATUS_TXIDLE_DEFAULT               0x00000001UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_TXIDLE_DEFAULT                (_EUSART_STATUS_TXIDLE_DEFAULT << 13)       /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define _EUSART_STATUS_TXFCNT_SHIFT                 16                                          /**< Shift value for EUSART_TXFCNT               */
#define _EUSART_STATUS_TXFCNT_MASK                  0x1F0000UL                                  /**< Bit mask for EUSART_TXFCNT                  */
#define _EUSART_STATUS_TXFCNT_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_TXFCNT_DEFAULT                (_EUSART_STATUS_TXFCNT_DEFAULT << 16)       /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define EUSART_STATUS_AUTOBAUDDONE                  (0x1UL << 24)                               /**< Auto Baud Rate Detection Completed          */
#define _EUSART_STATUS_AUTOBAUDDONE_SHIFT           24                                          /**< Shift value for EUSART_AUTOBAUDDONE         */
#define _EUSART_STATUS_AUTOBAUDDONE_MASK            0x1000000UL                                 /**< Bit mask for EUSART_AUTOBAUDDONE            */
#define _EUSART_STATUS_AUTOBAUDDONE_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_AUTOBAUDDONE_DEFAULT          (_EUSART_STATUS_AUTOBAUDDONE_DEFAULT << 24) /**< Shifted mode DEFAULT for EUSART_STATUS      */
#define EUSART_STATUS_CLEARTXBUSY                   (0x1UL << 25)                               /**< TX FIFO Clear Busy                          */
#define _EUSART_STATUS_CLEARTXBUSY_SHIFT            25                                          /**< Shift value for EUSART_CLEARTXBUSY          */
#define _EUSART_STATUS_CLEARTXBUSY_MASK             0x2000000UL                                 /**< Bit mask for EUSART_CLEARTXBUSY             */
#define _EUSART_STATUS_CLEARTXBUSY_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for EUSART_STATUS              */
#define EUSART_STATUS_CLEARTXBUSY_DEFAULT           (_EUSART_STATUS_CLEARTXBUSY_DEFAULT << 25)  /**< Shifted mode DEFAULT for EUSART_STATUS      */

/* Bit fields for EUSART IF */
#define _EUSART_IF_RESETVALUE                       0x00000000UL                            /**< Default value for EUSART_IF                 */
#define _EUSART_IF_MASK                             0x030D3FFFUL                            /**< Mask for EUSART_IF                          */
#define EUSART_IF_TXC                               (0x1UL << 0)                            /**< TX Complete Interrupt Flag                  */
#define _EUSART_IF_TXC_SHIFT                        0                                       /**< Shift value for EUSART_TXC                  */
#define _EUSART_IF_TXC_MASK                         0x1UL                                   /**< Bit mask for EUSART_TXC                     */
#define _EUSART_IF_TXC_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_TXC_DEFAULT                       (_EUSART_IF_TXC_DEFAULT << 0)           /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_TXFL                              (0x1UL << 1)                            /**< TX FIFO Level Interrupt Flag                */
#define _EUSART_IF_TXFL_SHIFT                       1                                       /**< Shift value for EUSART_TXFL                 */
#define _EUSART_IF_TXFL_MASK                        0x2UL                                   /**< Bit mask for EUSART_TXFL                    */
#define _EUSART_IF_TXFL_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_TXFL_DEFAULT                      (_EUSART_IF_TXFL_DEFAULT << 1)          /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_RXFL                              (0x1UL << 2)                            /**< RX FIFO Level Interrupt Flag                */
#define _EUSART_IF_RXFL_SHIFT                       2                                       /**< Shift value for EUSART_RXFL                 */
#define _EUSART_IF_RXFL_MASK                        0x4UL                                   /**< Bit mask for EUSART_RXFL                    */
#define _EUSART_IF_RXFL_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_RXFL_DEFAULT                      (_EUSART_IF_RXFL_DEFAULT << 2)          /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_RXFULL                            (0x1UL << 3)                            /**< RX FIFO Full Interrupt Flag                 */
#define _EUSART_IF_RXFULL_SHIFT                     3                                       /**< Shift value for EUSART_RXFULL               */
#define _EUSART_IF_RXFULL_MASK                      0x8UL                                   /**< Bit mask for EUSART_RXFULL                  */
#define _EUSART_IF_RXFULL_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_RXFULL_DEFAULT                    (_EUSART_IF_RXFULL_DEFAULT << 3)        /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_RXOF                              (0x1UL << 4)                            /**< RX FIFO Overflow Interrupt Flag             */
#define _EUSART_IF_RXOF_SHIFT                       4                                       /**< Shift value for EUSART_RXOF                 */
#define _EUSART_IF_RXOF_MASK                        0x10UL                                  /**< Bit mask for EUSART_RXOF                    */
#define _EUSART_IF_RXOF_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_RXOF_DEFAULT                      (_EUSART_IF_RXOF_DEFAULT << 4)          /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_RXUF                              (0x1UL << 5)                            /**< RX FIFO Underflow Interrupt Flag            */
#define _EUSART_IF_RXUF_SHIFT                       5                                       /**< Shift value for EUSART_RXUF                 */
#define _EUSART_IF_RXUF_MASK                        0x20UL                                  /**< Bit mask for EUSART_RXUF                    */
#define _EUSART_IF_RXUF_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_RXUF_DEFAULT                      (_EUSART_IF_RXUF_DEFAULT << 5)          /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_TXOF                              (0x1UL << 6)                            /**< TX FIFO Overflow Interrupt Flag             */
#define _EUSART_IF_TXOF_SHIFT                       6                                       /**< Shift value for EUSART_TXOF                 */
#define _EUSART_IF_TXOF_MASK                        0x40UL                                  /**< Bit mask for EUSART_TXOF                    */
#define _EUSART_IF_TXOF_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_TXOF_DEFAULT                      (_EUSART_IF_TXOF_DEFAULT << 6)          /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_TXUF                              (0x1UL << 7)                            /**< TX FIFO Underflow Interrupt Flag            */
#define _EUSART_IF_TXUF_SHIFT                       7                                       /**< Shift value for EUSART_TXUF                 */
#define _EUSART_IF_TXUF_MASK                        0x80UL                                  /**< Bit mask for EUSART_TXUF                    */
#define _EUSART_IF_TXUF_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_TXUF_DEFAULT                      (_EUSART_IF_TXUF_DEFAULT << 7)          /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_PERR                              (0x1UL << 8)                            /**< Parity Error Interrupt Flag                 */
#define _EUSART_IF_PERR_SHIFT                       8                                       /**< Shift value for EUSART_PERR                 */
#define _EUSART_IF_PERR_MASK                        0x100UL                                 /**< Bit mask for EUSART_PERR                    */
#define _EUSART_IF_PERR_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_PERR_DEFAULT                      (_EUSART_IF_PERR_DEFAULT << 8)          /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_FERR                              (0x1UL << 9)                            /**< Framing Error Interrupt Flag                */
#define _EUSART_IF_FERR_SHIFT                       9                                       /**< Shift value for EUSART_FERR                 */
#define _EUSART_IF_FERR_MASK                        0x200UL                                 /**< Bit mask for EUSART_FERR                    */
#define _EUSART_IF_FERR_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_FERR_DEFAULT                      (_EUSART_IF_FERR_DEFAULT << 9)          /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_MPAF                              (0x1UL << 10)                           /**< Multi-Processor Address Frame Interrupt     */
#define _EUSART_IF_MPAF_SHIFT                       10                                      /**< Shift value for EUSART_MPAF                 */
#define _EUSART_IF_MPAF_MASK                        0x400UL                                 /**< Bit mask for EUSART_MPAF                    */
#define _EUSART_IF_MPAF_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_MPAF_DEFAULT                      (_EUSART_IF_MPAF_DEFAULT << 10)         /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_LOADERR                           (0x1UL << 11)                           /**< Load Error Interrupt Flag                   */
#define _EUSART_IF_LOADERR_SHIFT                    11                                      /**< Shift value for EUSART_LOADERR              */
#define _EUSART_IF_LOADERR_MASK                     0x800UL                                 /**< Bit mask for EUSART_LOADERR                 */
#define _EUSART_IF_LOADERR_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_LOADERR_DEFAULT                   (_EUSART_IF_LOADERR_DEFAULT << 11)      /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_CCF                               (0x1UL << 12)                           /**< Collision Check Fail Interrupt Flag         */
#define _EUSART_IF_CCF_SHIFT                        12                                      /**< Shift value for EUSART_CCF                  */
#define _EUSART_IF_CCF_MASK                         0x1000UL                                /**< Bit mask for EUSART_CCF                     */
#define _EUSART_IF_CCF_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_CCF_DEFAULT                       (_EUSART_IF_CCF_DEFAULT << 12)          /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_TXIDLE                            (0x1UL << 13)                           /**< TX Idle Interrupt Flag                      */
#define _EUSART_IF_TXIDLE_SHIFT                     13                                      /**< Shift value for EUSART_TXIDLE               */
#define _EUSART_IF_TXIDLE_MASK                      0x2000UL                                /**< Bit mask for EUSART_TXIDLE                  */
#define _EUSART_IF_TXIDLE_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_TXIDLE_DEFAULT                    (_EUSART_IF_TXIDLE_DEFAULT << 13)       /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_CSWU                              (0x1UL << 16)                           /**< CS Wake-up Interrupt Flag                   */
#define _EUSART_IF_CSWU_SHIFT                       16                                      /**< Shift value for EUSART_CSWU                 */
#define _EUSART_IF_CSWU_MASK                        0x10000UL                               /**< Bit mask for EUSART_CSWU                    */
#define _EUSART_IF_CSWU_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_CSWU_DEFAULT                      (_EUSART_IF_CSWU_DEFAULT << 16)         /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_STARTF                            (0x1UL << 18)                           /**< Start Frame Interrupt Flag                  */
#define _EUSART_IF_STARTF_SHIFT                     18                                      /**< Shift value for EUSART_STARTF               */
#define _EUSART_IF_STARTF_MASK                      0x40000UL                               /**< Bit mask for EUSART_STARTF                  */
#define _EUSART_IF_STARTF_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_STARTF_DEFAULT                    (_EUSART_IF_STARTF_DEFAULT << 18)       /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_SIGF                              (0x1UL << 19)                           /**< Signal Frame Interrupt Flag                 */
#define _EUSART_IF_SIGF_SHIFT                       19                                      /**< Shift value for EUSART_SIGF                 */
#define _EUSART_IF_SIGF_MASK                        0x80000UL                               /**< Bit mask for EUSART_SIGF                    */
#define _EUSART_IF_SIGF_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_SIGF_DEFAULT                      (_EUSART_IF_SIGF_DEFAULT << 19)         /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_AUTOBAUDDONE                      (0x1UL << 24)                           /**< Auto Baud Complete Interrupt Flag           */
#define _EUSART_IF_AUTOBAUDDONE_SHIFT               24                                      /**< Shift value for EUSART_AUTOBAUDDONE         */
#define _EUSART_IF_AUTOBAUDDONE_MASK                0x1000000UL                             /**< Bit mask for EUSART_AUTOBAUDDONE            */
#define _EUSART_IF_AUTOBAUDDONE_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_AUTOBAUDDONE_DEFAULT              (_EUSART_IF_AUTOBAUDDONE_DEFAULT << 24) /**< Shifted mode DEFAULT for EUSART_IF          */
#define EUSART_IF_RXTO                              (0x1UL << 25)                           /**< RX Timeout Interrupt Flag                   */
#define _EUSART_IF_RXTO_SHIFT                       25                                      /**< Shift value for EUSART_RXTO                 */
#define _EUSART_IF_RXTO_MASK                        0x2000000UL                             /**< Bit mask for EUSART_RXTO                    */
#define _EUSART_IF_RXTO_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for EUSART_IF                  */
#define EUSART_IF_RXTO_DEFAULT                      (_EUSART_IF_RXTO_DEFAULT << 25)         /**< Shifted mode DEFAULT for EUSART_IF          */

/* Bit fields for EUSART IEN */
#define _EUSART_IEN_RESETVALUE                      0x00000000UL                             /**< Default value for EUSART_IEN                */
#define _EUSART_IEN_MASK                            0x030D3FFFUL                             /**< Mask for EUSART_IEN                         */
#define EUSART_IEN_TXC                              (0x1UL << 0)                             /**< TX Complete Enable                          */
#define _EUSART_IEN_TXC_SHIFT                       0                                        /**< Shift value for EUSART_TXC                  */
#define _EUSART_IEN_TXC_MASK                        0x1UL                                    /**< Bit mask for EUSART_TXC                     */
#define _EUSART_IEN_TXC_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_TXC_DEFAULT                      (_EUSART_IEN_TXC_DEFAULT << 0)           /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_TXFL                             (0x1UL << 1)                             /**< TX FIFO Level Enable                        */
#define _EUSART_IEN_TXFL_SHIFT                      1                                        /**< Shift value for EUSART_TXFL                 */
#define _EUSART_IEN_TXFL_MASK                       0x2UL                                    /**< Bit mask for EUSART_TXFL                    */
#define _EUSART_IEN_TXFL_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_TXFL_DEFAULT                     (_EUSART_IEN_TXFL_DEFAULT << 1)          /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_RXFL                             (0x1UL << 2)                             /**< RX FIFO Level Enable                        */
#define _EUSART_IEN_RXFL_SHIFT                      2                                        /**< Shift value for EUSART_RXFL                 */
#define _EUSART_IEN_RXFL_MASK                       0x4UL                                    /**< Bit mask for EUSART_RXFL                    */
#define _EUSART_IEN_RXFL_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_RXFL_DEFAULT                     (_EUSART_IEN_RXFL_DEFAULT << 2)          /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_RXFULL                           (0x1UL << 3)                             /**< RX FIFO Full Enable                         */
#define _EUSART_IEN_RXFULL_SHIFT                    3                                        /**< Shift value for EUSART_RXFULL               */
#define _EUSART_IEN_RXFULL_MASK                     0x8UL                                    /**< Bit mask for EUSART_RXFULL                  */
#define _EUSART_IEN_RXFULL_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_RXFULL_DEFAULT                   (_EUSART_IEN_RXFULL_DEFAULT << 3)        /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_RXOF                             (0x1UL << 4)                             /**< RX FIFO Overflow Enable                     */
#define _EUSART_IEN_RXOF_SHIFT                      4                                        /**< Shift value for EUSART_RXOF                 */
#define _EUSART_IEN_RXOF_MASK                       0x10UL                                   /**< Bit mask for EUSART_RXOF                    */
#define _EUSART_IEN_RXOF_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_RXOF_DEFAULT                     (_EUSART_IEN_RXOF_DEFAULT << 4)          /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_RXUF                             (0x1UL << 5)                             /**< RX FIFO Underflow Enable                    */
#define _EUSART_IEN_RXUF_SHIFT                      5                                        /**< Shift value for EUSART_RXUF                 */
#define _EUSART_IEN_RXUF_MASK                       0x20UL                                   /**< Bit mask for EUSART_RXUF                    */
#define _EUSART_IEN_RXUF_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_RXUF_DEFAULT                     (_EUSART_IEN_RXUF_DEFAULT << 5)          /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_TXOF                             (0x1UL << 6)                             /**< TX FIFO Overflow Enable                     */
#define _EUSART_IEN_TXOF_SHIFT                      6                                        /**< Shift value for EUSART_TXOF                 */
#define _EUSART_IEN_TXOF_MASK                       0x40UL                                   /**< Bit mask for EUSART_TXOF                    */
#define _EUSART_IEN_TXOF_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_TXOF_DEFAULT                     (_EUSART_IEN_TXOF_DEFAULT << 6)          /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_TXUF                             (0x1UL << 7)                             /**< TX FIFO Underflow Enable                    */
#define _EUSART_IEN_TXUF_SHIFT                      7                                        /**< Shift value for EUSART_TXUF                 */
#define _EUSART_IEN_TXUF_MASK                       0x80UL                                   /**< Bit mask for EUSART_TXUF                    */
#define _EUSART_IEN_TXUF_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_TXUF_DEFAULT                     (_EUSART_IEN_TXUF_DEFAULT << 7)          /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_PERR                             (0x1UL << 8)                             /**< Parity Error Enable                         */
#define _EUSART_IEN_PERR_SHIFT                      8                                        /**< Shift value for EUSART_PERR                 */
#define _EUSART_IEN_PERR_MASK                       0x100UL                                  /**< Bit mask for EUSART_PERR                    */
#define _EUSART_IEN_PERR_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_PERR_DEFAULT                     (_EUSART_IEN_PERR_DEFAULT << 8)          /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_FERR                             (0x1UL << 9)                             /**< Framing Error Enable                        */
#define _EUSART_IEN_FERR_SHIFT                      9                                        /**< Shift value for EUSART_FERR                 */
#define _EUSART_IEN_FERR_MASK                       0x200UL                                  /**< Bit mask for EUSART_FERR                    */
#define _EUSART_IEN_FERR_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_FERR_DEFAULT                     (_EUSART_IEN_FERR_DEFAULT << 9)          /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_MPAF                             (0x1UL << 10)                            /**< Multi-Processor Addr Frame Enable           */
#define _EUSART_IEN_MPAF_SHIFT                      10                                       /**< Shift value for EUSART_MPAF                 */
#define _EUSART_IEN_MPAF_MASK                       0x400UL                                  /**< Bit mask for EUSART_MPAF                    */
#define _EUSART_IEN_MPAF_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_MPAF_DEFAULT                     (_EUSART_IEN_MPAF_DEFAULT << 10)         /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_LOADERR                          (0x1UL << 11)                            /**< Load Error Enable                           */
#define _EUSART_IEN_LOADERR_SHIFT                   11                                       /**< Shift value for EUSART_LOADERR              */
#define _EUSART_IEN_LOADERR_MASK                    0x800UL                                  /**< Bit mask for EUSART_LOADERR                 */
#define _EUSART_IEN_LOADERR_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_LOADERR_DEFAULT                  (_EUSART_IEN_LOADERR_DEFAULT << 11)      /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_CCF                              (0x1UL << 12)                            /**< Collision Check Fail Enable                 */
#define _EUSART_IEN_CCF_SHIFT                       12                                       /**< Shift value for EUSART_CCF                  */
#define _EUSART_IEN_CCF_MASK                        0x1000UL                                 /**< Bit mask for EUSART_CCF                     */
#define _EUSART_IEN_CCF_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_CCF_DEFAULT                      (_EUSART_IEN_CCF_DEFAULT << 12)          /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_TXIDLE                           (0x1UL << 13)                            /**< TX IDLE Enable                              */
#define _EUSART_IEN_TXIDLE_SHIFT                    13                                       /**< Shift value for EUSART_TXIDLE               */
#define _EUSART_IEN_TXIDLE_MASK                     0x2000UL                                 /**< Bit mask for EUSART_TXIDLE                  */
#define _EUSART_IEN_TXIDLE_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_TXIDLE_DEFAULT                   (_EUSART_IEN_TXIDLE_DEFAULT << 13)       /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_CSWU                             (0x1UL << 16)                            /**< CS Wake-up Enable                           */
#define _EUSART_IEN_CSWU_SHIFT                      16                                       /**< Shift value for EUSART_CSWU                 */
#define _EUSART_IEN_CSWU_MASK                       0x10000UL                                /**< Bit mask for EUSART_CSWU                    */
#define _EUSART_IEN_CSWU_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_CSWU_DEFAULT                     (_EUSART_IEN_CSWU_DEFAULT << 16)         /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_STARTF                           (0x1UL << 18)                            /**< Start Frame Enable                          */
#define _EUSART_IEN_STARTF_SHIFT                    18                                       /**< Shift value for EUSART_STARTF               */
#define _EUSART_IEN_STARTF_MASK                     0x40000UL                                /**< Bit mask for EUSART_STARTF                  */
#define _EUSART_IEN_STARTF_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_STARTF_DEFAULT                   (_EUSART_IEN_STARTF_DEFAULT << 18)       /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_SIGF                             (0x1UL << 19)                            /**< Signal Frame Enable                         */
#define _EUSART_IEN_SIGF_SHIFT                      19                                       /**< Shift value for EUSART_SIGF                 */
#define _EUSART_IEN_SIGF_MASK                       0x80000UL                                /**< Bit mask for EUSART_SIGF                    */
#define _EUSART_IEN_SIGF_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_SIGF_DEFAULT                     (_EUSART_IEN_SIGF_DEFAULT << 19)         /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_AUTOBAUDDONE                     (0x1UL << 24)                            /**< Auto Baud Complete Enable                   */
#define _EUSART_IEN_AUTOBAUDDONE_SHIFT              24                                       /**< Shift value for EUSART_AUTOBAUDDONE         */
#define _EUSART_IEN_AUTOBAUDDONE_MASK               0x1000000UL                              /**< Bit mask for EUSART_AUTOBAUDDONE            */
#define _EUSART_IEN_AUTOBAUDDONE_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_AUTOBAUDDONE_DEFAULT             (_EUSART_IEN_AUTOBAUDDONE_DEFAULT << 24) /**< Shifted mode DEFAULT for EUSART_IEN         */
#define EUSART_IEN_RXTO                             (0x1UL << 25)                            /**< RX Timeout Enable                           */
#define _EUSART_IEN_RXTO_SHIFT                      25                                       /**< Shift value for EUSART_RXTO                 */
#define _EUSART_IEN_RXTO_MASK                       0x2000000UL                              /**< Bit mask for EUSART_RXTO                    */
#define _EUSART_IEN_RXTO_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for EUSART_IEN                 */
#define EUSART_IEN_RXTO_DEFAULT                     (_EUSART_IEN_RXTO_DEFAULT << 25)         /**< Shifted mode DEFAULT for EUSART_IEN         */

/* Bit fields for EUSART SYNCBUSY */
#define _EUSART_SYNCBUSY_RESETVALUE                 0x00000000UL                               /**< Default value for EUSART_SYNCBUSY           */
#define _EUSART_SYNCBUSY_MASK                       0x00000FFFUL                               /**< Mask for EUSART_SYNCBUSY                    */
#define EUSART_SYNCBUSY_DIV                         (0x1UL << 0)                               /**< SYNCBUSY for DIV in CLKDIV                  */
#define _EUSART_SYNCBUSY_DIV_SHIFT                  0                                          /**< Shift value for EUSART_DIV                  */
#define _EUSART_SYNCBUSY_DIV_MASK                   0x1UL                                      /**< Bit mask for EUSART_DIV                     */
#define _EUSART_SYNCBUSY_DIV_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_DIV_DEFAULT                 (_EUSART_SYNCBUSY_DIV_DEFAULT << 0)        /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */
#define EUSART_SYNCBUSY_RXTEN                       (0x1UL << 1)                               /**< SYNCBUSY for RXTEN in TRIGCTRL              */
#define _EUSART_SYNCBUSY_RXTEN_SHIFT                1                                          /**< Shift value for EUSART_RXTEN                */
#define _EUSART_SYNCBUSY_RXTEN_MASK                 0x2UL                                      /**< Bit mask for EUSART_RXTEN                   */
#define _EUSART_SYNCBUSY_RXTEN_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_RXTEN_DEFAULT               (_EUSART_SYNCBUSY_RXTEN_DEFAULT << 1)      /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */
#define EUSART_SYNCBUSY_TXTEN                       (0x1UL << 2)                               /**< SYNCBUSY for TXTEN in TRIGCTRL              */
#define _EUSART_SYNCBUSY_TXTEN_SHIFT                2                                          /**< Shift value for EUSART_TXTEN                */
#define _EUSART_SYNCBUSY_TXTEN_MASK                 0x4UL                                      /**< Bit mask for EUSART_TXTEN                   */
#define _EUSART_SYNCBUSY_TXTEN_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_TXTEN_DEFAULT               (_EUSART_SYNCBUSY_TXTEN_DEFAULT << 2)      /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */
#define EUSART_SYNCBUSY_RXEN                        (0x1UL << 3)                               /**< SYNCBUSY for RXEN in CMD                    */
#define _EUSART_SYNCBUSY_RXEN_SHIFT                 3                                          /**< Shift value for EUSART_RXEN                 */
#define _EUSART_SYNCBUSY_RXEN_MASK                  0x8UL                                      /**< Bit mask for EUSART_RXEN                    */
#define _EUSART_SYNCBUSY_RXEN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_RXEN_DEFAULT                (_EUSART_SYNCBUSY_RXEN_DEFAULT << 3)       /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */
#define EUSART_SYNCBUSY_RXDIS                       (0x1UL << 4)                               /**< SYNCBUSY for RXDIS in CMD                   */
#define _EUSART_SYNCBUSY_RXDIS_SHIFT                4                                          /**< Shift value for EUSART_RXDIS                */
#define _EUSART_SYNCBUSY_RXDIS_MASK                 0x10UL                                     /**< Bit mask for EUSART_RXDIS                   */
#define _EUSART_SYNCBUSY_RXDIS_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_RXDIS_DEFAULT               (_EUSART_SYNCBUSY_RXDIS_DEFAULT << 4)      /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */
#define EUSART_SYNCBUSY_TXEN                        (0x1UL << 5)                               /**< SYNCBUSY for TXEN in CMD                    */
#define _EUSART_SYNCBUSY_TXEN_SHIFT                 5                                          /**< Shift value for EUSART_TXEN                 */
#define _EUSART_SYNCBUSY_TXEN_MASK                  0x20UL                                     /**< Bit mask for EUSART_TXEN                    */
#define _EUSART_SYNCBUSY_TXEN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_TXEN_DEFAULT                (_EUSART_SYNCBUSY_TXEN_DEFAULT << 5)       /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */
#define EUSART_SYNCBUSY_TXDIS                       (0x1UL << 6)                               /**< SYNCBUSY for TXDIS in CMD                   */
#define _EUSART_SYNCBUSY_TXDIS_SHIFT                6                                          /**< Shift value for EUSART_TXDIS                */
#define _EUSART_SYNCBUSY_TXDIS_MASK                 0x40UL                                     /**< Bit mask for EUSART_TXDIS                   */
#define _EUSART_SYNCBUSY_TXDIS_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_TXDIS_DEFAULT               (_EUSART_SYNCBUSY_TXDIS_DEFAULT << 6)      /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */
#define EUSART_SYNCBUSY_RXBLOCKEN                   (0x1UL << 7)                               /**< SYNCBUSY for RXBLOCKEN in CMD               */
#define _EUSART_SYNCBUSY_RXBLOCKEN_SHIFT            7                                          /**< Shift value for EUSART_RXBLOCKEN            */
#define _EUSART_SYNCBUSY_RXBLOCKEN_MASK             0x80UL                                     /**< Bit mask for EUSART_RXBLOCKEN               */
#define _EUSART_SYNCBUSY_RXBLOCKEN_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_RXBLOCKEN_DEFAULT           (_EUSART_SYNCBUSY_RXBLOCKEN_DEFAULT << 7)  /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */
#define EUSART_SYNCBUSY_RXBLOCKDIS                  (0x1UL << 8)                               /**< SYNCBUSY for RXBLOCKDIS in CMD              */
#define _EUSART_SYNCBUSY_RXBLOCKDIS_SHIFT           8                                          /**< Shift value for EUSART_RXBLOCKDIS           */
#define _EUSART_SYNCBUSY_RXBLOCKDIS_MASK            0x100UL                                    /**< Bit mask for EUSART_RXBLOCKDIS              */
#define _EUSART_SYNCBUSY_RXBLOCKDIS_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_RXBLOCKDIS_DEFAULT          (_EUSART_SYNCBUSY_RXBLOCKDIS_DEFAULT << 8) /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */
#define EUSART_SYNCBUSY_TXTRIEN                     (0x1UL << 9)                               /**< SYNCBUSY for TXTRIEN in CMD                 */
#define _EUSART_SYNCBUSY_TXTRIEN_SHIFT              9                                          /**< Shift value for EUSART_TXTRIEN              */
#define _EUSART_SYNCBUSY_TXTRIEN_MASK               0x200UL                                    /**< Bit mask for EUSART_TXTRIEN                 */
#define _EUSART_SYNCBUSY_TXTRIEN_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_TXTRIEN_DEFAULT             (_EUSART_SYNCBUSY_TXTRIEN_DEFAULT << 9)    /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */
#define EUSART_SYNCBUSY_TXTRIDIS                    (0x1UL << 10)                              /**< SYNCBUSY in TXTRIDIS in CMD                 */
#define _EUSART_SYNCBUSY_TXTRIDIS_SHIFT             10                                         /**< Shift value for EUSART_TXTRIDIS             */
#define _EUSART_SYNCBUSY_TXTRIDIS_MASK              0x400UL                                    /**< Bit mask for EUSART_TXTRIDIS                */
#define _EUSART_SYNCBUSY_TXTRIDIS_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_TXTRIDIS_DEFAULT            (_EUSART_SYNCBUSY_TXTRIDIS_DEFAULT << 10)  /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */
#define EUSART_SYNCBUSY_AUTOTXTEN                   (0x1UL << 11)                              /**< SYNCBUSY for AUTOTXTEN in TRIGCTRL          */
#define _EUSART_SYNCBUSY_AUTOTXTEN_SHIFT            11                                         /**< Shift value for EUSART_AUTOTXTEN            */
#define _EUSART_SYNCBUSY_AUTOTXTEN_MASK             0x800UL                                    /**< Bit mask for EUSART_AUTOTXTEN               */
#define _EUSART_SYNCBUSY_AUTOTXTEN_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for EUSART_SYNCBUSY            */
#define EUSART_SYNCBUSY_AUTOTXTEN_DEFAULT           (_EUSART_SYNCBUSY_AUTOTXTEN_DEFAULT << 11) /**< Shifted mode DEFAULT for EUSART_SYNCBUSY    */

/** @} End of group EFR32ZG23_EUSART_BitFields */
/** @} End of group EFR32ZG23_EUSART */
/** @} End of group Parts */

#endif /* EFR32ZG23_EUSART_H */
#ifdef __cplusplus
}
#endif

