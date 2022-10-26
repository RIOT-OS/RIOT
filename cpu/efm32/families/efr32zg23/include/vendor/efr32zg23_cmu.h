/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 CMU register and bit field definitions
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

#ifndef EFR32ZG23_CMU_H
#define EFR32ZG23_CMU_H
#define CMU_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_CMU CMU
 * @{
 * @brief EFR32ZG23 CMU Register Declaration.
 *****************************************************************************/

/** CMU Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  uint32_t       RESERVED1[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< Configuration Lock Register                        */
  __IOM uint32_t WDOGLOCK;                      /**< WDOG Configuration Lock Register                   */
  uint32_t       RESERVED2[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  uint32_t       RESERVED3[10U];                /**< Reserved for future use                            */
  __IOM uint32_t CALCMD;                        /**< Calibration Command Register                       */
  __IOM uint32_t CALCTRL;                       /**< Calibration Control Register                       */
  __IM uint32_t  CALCNT;                        /**< Calibration Result Counter Register                */
  uint32_t       RESERVED4[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CLKEN0;                        /**< Clock Enable Register 0                            */
  __IOM uint32_t CLKEN1;                        /**<  Clock Enable Register 1                           */
  uint32_t       RESERVED5[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t SYSCLKCTRL;                    /**< System Clock Control                               */
  uint32_t       RESERVED6[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t TRACECLKCTRL;                  /**< Debug Trace Clock Control                          */
  uint32_t       RESERVED7[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t EXPORTCLKCTRL;                 /**< Export Clock Control                               */
  uint32_t       RESERVED8[27U];                /**< Reserved for future use                            */
  __IOM uint32_t DPLLREFCLKCTRL;                /**< Digital PLL Reference Clock Control                */
  uint32_t       RESERVED9[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPACLKCTRL;               /**< EM01 Peripheral Group A Clock Control              */
  uint32_t       RESERVED10[1U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPCCLKCTRL;               /**< EM01 Peripheral Group C Clock Control              */
  uint32_t       RESERVED11[5U];                /**< Reserved for future use                            */
  __IOM uint32_t EM23GRPACLKCTRL;               /**< EM23 Peripheral Group A Clock Control              */
  uint32_t       RESERVED12[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM4GRPACLKCTRL;                /**< EM4 Peripheral Group A Clock Control               */
  uint32_t       RESERVED13[7U];                /**< Reserved for future use                            */
  __IOM uint32_t IADCCLKCTRL;                   /**< IADC Clock Control                                 */
  uint32_t       RESERVED14[31U];               /**< Reserved for future use                            */
  __IOM uint32_t WDOG0CLKCTRL;                  /**< Watchdog0 Clock Control                            */
  uint32_t       RESERVED15[1U];                /**< Reserved for future use                            */
  __IOM uint32_t WDOG1CLKCTRL;                  /**< Watchdog1 Clock Control                            */
  uint32_t       RESERVED16[5U];                /**< Reserved for future use                            */
  __IOM uint32_t EUSART0CLKCTRL;                /**< EUSART0 Clock Control                              */
  uint32_t       RESERVED17[7U];                /**< Reserved for future use                            */
  __IOM uint32_t SYSRTC0CLKCTRL;                /**< System RTC0 Clock Control                          */
  uint32_t       RESERVED18[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LCDCLKCTRL;                    /**< LCD Clock Control                                  */
  uint32_t       RESERVED19[3U];                /**< Reserved for future use                            */
  __IOM uint32_t VDAC0CLKCTRL;                  /**< VDAC0 Clock Control                                */
  uint32_t       RESERVED20[3U];                /**< Reserved for future use                            */
  __IOM uint32_t PCNT0CLKCTRL;                  /**< Pulse counter 0 Clock Control                      */
  uint32_t       RESERVED21[3U];                /**< Reserved for future use                            */
  __IOM uint32_t RADIOCLKCTRL;                  /**< Radio Clock Control                                */
  uint32_t       RESERVED22[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LESENSEHFCLKCTRL;              /**< LESENSE HF Clock Control                           */
  uint32_t       RESERVED23[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED24[858U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  uint32_t       RESERVED25[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  uint32_t       RESERVED26[1U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< Configuration Lock Register                        */
  __IOM uint32_t WDOGLOCK_SET;                  /**< WDOG Configuration Lock Register                   */
  uint32_t       RESERVED27[2U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED28[10U];               /**< Reserved for future use                            */
  __IOM uint32_t CALCMD_SET;                    /**< Calibration Command Register                       */
  __IOM uint32_t CALCTRL_SET;                   /**< Calibration Control Register                       */
  __IM uint32_t  CALCNT_SET;                    /**< Calibration Result Counter Register                */
  uint32_t       RESERVED29[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CLKEN0_SET;                    /**< Clock Enable Register 0                            */
  __IOM uint32_t CLKEN1_SET;                    /**<  Clock Enable Register 1                           */
  uint32_t       RESERVED30[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SYSCLKCTRL_SET;                /**< System Clock Control                               */
  uint32_t       RESERVED31[3U];                /**< Reserved for future use                            */
  __IOM uint32_t TRACECLKCTRL_SET;              /**< Debug Trace Clock Control                          */
  uint32_t       RESERVED32[3U];                /**< Reserved for future use                            */
  __IOM uint32_t EXPORTCLKCTRL_SET;             /**< Export Clock Control                               */
  uint32_t       RESERVED33[27U];               /**< Reserved for future use                            */
  __IOM uint32_t DPLLREFCLKCTRL_SET;            /**< Digital PLL Reference Clock Control                */
  uint32_t       RESERVED34[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPACLKCTRL_SET;           /**< EM01 Peripheral Group A Clock Control              */
  uint32_t       RESERVED35[1U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPCCLKCTRL_SET;           /**< EM01 Peripheral Group C Clock Control              */
  uint32_t       RESERVED36[5U];                /**< Reserved for future use                            */
  __IOM uint32_t EM23GRPACLKCTRL_SET;           /**< EM23 Peripheral Group A Clock Control              */
  uint32_t       RESERVED37[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM4GRPACLKCTRL_SET;            /**< EM4 Peripheral Group A Clock Control               */
  uint32_t       RESERVED38[7U];                /**< Reserved for future use                            */
  __IOM uint32_t IADCCLKCTRL_SET;               /**< IADC Clock Control                                 */
  uint32_t       RESERVED39[31U];               /**< Reserved for future use                            */
  __IOM uint32_t WDOG0CLKCTRL_SET;              /**< Watchdog0 Clock Control                            */
  uint32_t       RESERVED40[1U];                /**< Reserved for future use                            */
  __IOM uint32_t WDOG1CLKCTRL_SET;              /**< Watchdog1 Clock Control                            */
  uint32_t       RESERVED41[5U];                /**< Reserved for future use                            */
  __IOM uint32_t EUSART0CLKCTRL_SET;            /**< EUSART0 Clock Control                              */
  uint32_t       RESERVED42[7U];                /**< Reserved for future use                            */
  __IOM uint32_t SYSRTC0CLKCTRL_SET;            /**< System RTC0 Clock Control                          */
  uint32_t       RESERVED43[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LCDCLKCTRL_SET;                /**< LCD Clock Control                                  */
  uint32_t       RESERVED44[3U];                /**< Reserved for future use                            */
  __IOM uint32_t VDAC0CLKCTRL_SET;              /**< VDAC0 Clock Control                                */
  uint32_t       RESERVED45[3U];                /**< Reserved for future use                            */
  __IOM uint32_t PCNT0CLKCTRL_SET;              /**< Pulse counter 0 Clock Control                      */
  uint32_t       RESERVED46[3U];                /**< Reserved for future use                            */
  __IOM uint32_t RADIOCLKCTRL_SET;              /**< Radio Clock Control                                */
  uint32_t       RESERVED47[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LESENSEHFCLKCTRL_SET;          /**< LESENSE HF Clock Control                           */
  uint32_t       RESERVED48[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED49[858U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  uint32_t       RESERVED50[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  uint32_t       RESERVED51[1U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< Configuration Lock Register                        */
  __IOM uint32_t WDOGLOCK_CLR;                  /**< WDOG Configuration Lock Register                   */
  uint32_t       RESERVED52[2U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED53[10U];               /**< Reserved for future use                            */
  __IOM uint32_t CALCMD_CLR;                    /**< Calibration Command Register                       */
  __IOM uint32_t CALCTRL_CLR;                   /**< Calibration Control Register                       */
  __IM uint32_t  CALCNT_CLR;                    /**< Calibration Result Counter Register                */
  uint32_t       RESERVED54[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CLKEN0_CLR;                    /**< Clock Enable Register 0                            */
  __IOM uint32_t CLKEN1_CLR;                    /**<  Clock Enable Register 1                           */
  uint32_t       RESERVED55[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SYSCLKCTRL_CLR;                /**< System Clock Control                               */
  uint32_t       RESERVED56[3U];                /**< Reserved for future use                            */
  __IOM uint32_t TRACECLKCTRL_CLR;              /**< Debug Trace Clock Control                          */
  uint32_t       RESERVED57[3U];                /**< Reserved for future use                            */
  __IOM uint32_t EXPORTCLKCTRL_CLR;             /**< Export Clock Control                               */
  uint32_t       RESERVED58[27U];               /**< Reserved for future use                            */
  __IOM uint32_t DPLLREFCLKCTRL_CLR;            /**< Digital PLL Reference Clock Control                */
  uint32_t       RESERVED59[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPACLKCTRL_CLR;           /**< EM01 Peripheral Group A Clock Control              */
  uint32_t       RESERVED60[1U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPCCLKCTRL_CLR;           /**< EM01 Peripheral Group C Clock Control              */
  uint32_t       RESERVED61[5U];                /**< Reserved for future use                            */
  __IOM uint32_t EM23GRPACLKCTRL_CLR;           /**< EM23 Peripheral Group A Clock Control              */
  uint32_t       RESERVED62[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM4GRPACLKCTRL_CLR;            /**< EM4 Peripheral Group A Clock Control               */
  uint32_t       RESERVED63[7U];                /**< Reserved for future use                            */
  __IOM uint32_t IADCCLKCTRL_CLR;               /**< IADC Clock Control                                 */
  uint32_t       RESERVED64[31U];               /**< Reserved for future use                            */
  __IOM uint32_t WDOG0CLKCTRL_CLR;              /**< Watchdog0 Clock Control                            */
  uint32_t       RESERVED65[1U];                /**< Reserved for future use                            */
  __IOM uint32_t WDOG1CLKCTRL_CLR;              /**< Watchdog1 Clock Control                            */
  uint32_t       RESERVED66[5U];                /**< Reserved for future use                            */
  __IOM uint32_t EUSART0CLKCTRL_CLR;            /**< EUSART0 Clock Control                              */
  uint32_t       RESERVED67[7U];                /**< Reserved for future use                            */
  __IOM uint32_t SYSRTC0CLKCTRL_CLR;            /**< System RTC0 Clock Control                          */
  uint32_t       RESERVED68[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LCDCLKCTRL_CLR;                /**< LCD Clock Control                                  */
  uint32_t       RESERVED69[3U];                /**< Reserved for future use                            */
  __IOM uint32_t VDAC0CLKCTRL_CLR;              /**< VDAC0 Clock Control                                */
  uint32_t       RESERVED70[3U];                /**< Reserved for future use                            */
  __IOM uint32_t PCNT0CLKCTRL_CLR;              /**< Pulse counter 0 Clock Control                      */
  uint32_t       RESERVED71[3U];                /**< Reserved for future use                            */
  __IOM uint32_t RADIOCLKCTRL_CLR;              /**< Radio Clock Control                                */
  uint32_t       RESERVED72[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LESENSEHFCLKCTRL_CLR;          /**< LESENSE HF Clock Control                           */
  uint32_t       RESERVED73[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED74[858U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  uint32_t       RESERVED75[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  uint32_t       RESERVED76[1U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< Configuration Lock Register                        */
  __IOM uint32_t WDOGLOCK_TGL;                  /**< WDOG Configuration Lock Register                   */
  uint32_t       RESERVED77[2U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED78[10U];               /**< Reserved for future use                            */
  __IOM uint32_t CALCMD_TGL;                    /**< Calibration Command Register                       */
  __IOM uint32_t CALCTRL_TGL;                   /**< Calibration Control Register                       */
  __IM uint32_t  CALCNT_TGL;                    /**< Calibration Result Counter Register                */
  uint32_t       RESERVED79[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CLKEN0_TGL;                    /**< Clock Enable Register 0                            */
  __IOM uint32_t CLKEN1_TGL;                    /**<  Clock Enable Register 1                           */
  uint32_t       RESERVED80[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SYSCLKCTRL_TGL;                /**< System Clock Control                               */
  uint32_t       RESERVED81[3U];                /**< Reserved for future use                            */
  __IOM uint32_t TRACECLKCTRL_TGL;              /**< Debug Trace Clock Control                          */
  uint32_t       RESERVED82[3U];                /**< Reserved for future use                            */
  __IOM uint32_t EXPORTCLKCTRL_TGL;             /**< Export Clock Control                               */
  uint32_t       RESERVED83[27U];               /**< Reserved for future use                            */
  __IOM uint32_t DPLLREFCLKCTRL_TGL;            /**< Digital PLL Reference Clock Control                */
  uint32_t       RESERVED84[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPACLKCTRL_TGL;           /**< EM01 Peripheral Group A Clock Control              */
  uint32_t       RESERVED85[1U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPCCLKCTRL_TGL;           /**< EM01 Peripheral Group C Clock Control              */
  uint32_t       RESERVED86[5U];                /**< Reserved for future use                            */
  __IOM uint32_t EM23GRPACLKCTRL_TGL;           /**< EM23 Peripheral Group A Clock Control              */
  uint32_t       RESERVED87[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM4GRPACLKCTRL_TGL;            /**< EM4 Peripheral Group A Clock Control               */
  uint32_t       RESERVED88[7U];                /**< Reserved for future use                            */
  __IOM uint32_t IADCCLKCTRL_TGL;               /**< IADC Clock Control                                 */
  uint32_t       RESERVED89[31U];               /**< Reserved for future use                            */
  __IOM uint32_t WDOG0CLKCTRL_TGL;              /**< Watchdog0 Clock Control                            */
  uint32_t       RESERVED90[1U];                /**< Reserved for future use                            */
  __IOM uint32_t WDOG1CLKCTRL_TGL;              /**< Watchdog1 Clock Control                            */
  uint32_t       RESERVED91[5U];                /**< Reserved for future use                            */
  __IOM uint32_t EUSART0CLKCTRL_TGL;            /**< EUSART0 Clock Control                              */
  uint32_t       RESERVED92[7U];                /**< Reserved for future use                            */
  __IOM uint32_t SYSRTC0CLKCTRL_TGL;            /**< System RTC0 Clock Control                          */
  uint32_t       RESERVED93[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LCDCLKCTRL_TGL;                /**< LCD Clock Control                                  */
  uint32_t       RESERVED94[3U];                /**< Reserved for future use                            */
  __IOM uint32_t VDAC0CLKCTRL_TGL;              /**< VDAC0 Clock Control                                */
  uint32_t       RESERVED95[3U];                /**< Reserved for future use                            */
  __IOM uint32_t PCNT0CLKCTRL_TGL;              /**< Pulse counter 0 Clock Control                      */
  uint32_t       RESERVED96[3U];                /**< Reserved for future use                            */
  __IOM uint32_t RADIOCLKCTRL_TGL;              /**< Radio Clock Control                                */
  uint32_t       RESERVED97[3U];                /**< Reserved for future use                            */
  __IOM uint32_t LESENSEHFCLKCTRL_TGL;          /**< LESENSE HF Clock Control                           */
  uint32_t       RESERVED98[1U];                /**< Reserved for future use                            */
} CMU_TypeDef;
/** @} End of group EFR32ZG23_CMU */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_CMU
 * @{
 * @defgroup EFR32ZG23_CMU_BitFields CMU Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for CMU IPVERSION */
#define _CMU_IPVERSION_RESETVALUE                  0x00000002UL                            /**< Default value for CMU_IPVERSION             */
#define _CMU_IPVERSION_MASK                        0xFFFFFFFFUL                            /**< Mask for CMU_IPVERSION                      */
#define _CMU_IPVERSION_IPVERSION_SHIFT             0                                       /**< Shift value for CMU_IPVERSION               */
#define _CMU_IPVERSION_IPVERSION_MASK              0xFFFFFFFFUL                            /**< Bit mask for CMU_IPVERSION                  */
#define _CMU_IPVERSION_IPVERSION_DEFAULT           0x00000002UL                            /**< Mode DEFAULT for CMU_IPVERSION              */
#define CMU_IPVERSION_IPVERSION_DEFAULT            (_CMU_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_IPVERSION      */

/* Bit fields for CMU STATUS */
#define _CMU_STATUS_RESETVALUE                     0x00000000UL                          /**< Default value for CMU_STATUS                */
#define _CMU_STATUS_MASK                           0xC0038001UL                          /**< Mask for CMU_STATUS                         */
#define CMU_STATUS_CALRDY                          (0x1UL << 0)                          /**< Calibration Ready                           */
#define _CMU_STATUS_CALRDY_SHIFT                   0                                     /**< Shift value for CMU_CALRDY                  */
#define _CMU_STATUS_CALRDY_MASK                    0x1UL                                 /**< Bit mask for CMU_CALRDY                     */
#define _CMU_STATUS_CALRDY_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for CMU_STATUS                 */
#define CMU_STATUS_CALRDY_DEFAULT                  (_CMU_STATUS_CALRDY_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_STATUS         */
#define CMU_STATUS_WDOGLOCK                        (0x1UL << 30)                         /**< Configuration Lock Status for WDOG          */
#define _CMU_STATUS_WDOGLOCK_SHIFT                 30                                    /**< Shift value for CMU_WDOGLOCK                */
#define _CMU_STATUS_WDOGLOCK_MASK                  0x40000000UL                          /**< Bit mask for CMU_WDOGLOCK                   */
#define _CMU_STATUS_WDOGLOCK_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for CMU_STATUS                 */
#define _CMU_STATUS_WDOGLOCK_UNLOCKED              0x00000000UL                          /**< Mode UNLOCKED for CMU_STATUS                */
#define _CMU_STATUS_WDOGLOCK_LOCKED                0x00000001UL                          /**< Mode LOCKED for CMU_STATUS                  */
#define CMU_STATUS_WDOGLOCK_DEFAULT                (_CMU_STATUS_WDOGLOCK_DEFAULT << 30)  /**< Shifted mode DEFAULT for CMU_STATUS         */
#define CMU_STATUS_WDOGLOCK_UNLOCKED               (_CMU_STATUS_WDOGLOCK_UNLOCKED << 30) /**< Shifted mode UNLOCKED for CMU_STATUS        */
#define CMU_STATUS_WDOGLOCK_LOCKED                 (_CMU_STATUS_WDOGLOCK_LOCKED << 30)   /**< Shifted mode LOCKED for CMU_STATUS          */
#define CMU_STATUS_LOCK                            (0x1UL << 31)                         /**< Configuration Lock Status                   */
#define _CMU_STATUS_LOCK_SHIFT                     31                                    /**< Shift value for CMU_LOCK                    */
#define _CMU_STATUS_LOCK_MASK                      0x80000000UL                          /**< Bit mask for CMU_LOCK                       */
#define _CMU_STATUS_LOCK_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for CMU_STATUS                 */
#define _CMU_STATUS_LOCK_UNLOCKED                  0x00000000UL                          /**< Mode UNLOCKED for CMU_STATUS                */
#define _CMU_STATUS_LOCK_LOCKED                    0x00000001UL                          /**< Mode LOCKED for CMU_STATUS                  */
#define CMU_STATUS_LOCK_DEFAULT                    (_CMU_STATUS_LOCK_DEFAULT << 31)      /**< Shifted mode DEFAULT for CMU_STATUS         */
#define CMU_STATUS_LOCK_UNLOCKED                   (_CMU_STATUS_LOCK_UNLOCKED << 31)     /**< Shifted mode UNLOCKED for CMU_STATUS        */
#define CMU_STATUS_LOCK_LOCKED                     (_CMU_STATUS_LOCK_LOCKED << 31)       /**< Shifted mode LOCKED for CMU_STATUS          */

/* Bit fields for CMU LOCK */
#define _CMU_LOCK_RESETVALUE                       0x000093F7UL                         /**< Default value for CMU_LOCK                  */
#define _CMU_LOCK_MASK                             0x0000FFFFUL                         /**< Mask for CMU_LOCK                           */
#define _CMU_LOCK_LOCKKEY_SHIFT                    0                                    /**< Shift value for CMU_LOCKKEY                 */
#define _CMU_LOCK_LOCKKEY_MASK                     0xFFFFUL                             /**< Bit mask for CMU_LOCKKEY                    */
#define _CMU_LOCK_LOCKKEY_DEFAULT                  0x000093F7UL                         /**< Mode DEFAULT for CMU_LOCK                   */
#define _CMU_LOCK_LOCKKEY_UNLOCK                   0x000093F7UL                         /**< Mode UNLOCK for CMU_LOCK                    */
#define CMU_LOCK_LOCKKEY_DEFAULT                   (_CMU_LOCK_LOCKKEY_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_LOCK           */
#define CMU_LOCK_LOCKKEY_UNLOCK                    (_CMU_LOCK_LOCKKEY_UNLOCK << 0)      /**< Shifted mode UNLOCK for CMU_LOCK            */

/* Bit fields for CMU WDOGLOCK */
#define _CMU_WDOGLOCK_RESETVALUE                   0x00005257UL                         /**< Default value for CMU_WDOGLOCK              */
#define _CMU_WDOGLOCK_MASK                         0x0000FFFFUL                         /**< Mask for CMU_WDOGLOCK                       */
#define _CMU_WDOGLOCK_LOCKKEY_SHIFT                0                                    /**< Shift value for CMU_LOCKKEY                 */
#define _CMU_WDOGLOCK_LOCKKEY_MASK                 0xFFFFUL                             /**< Bit mask for CMU_LOCKKEY                    */
#define _CMU_WDOGLOCK_LOCKKEY_DEFAULT              0x00005257UL                         /**< Mode DEFAULT for CMU_WDOGLOCK               */
#define _CMU_WDOGLOCK_LOCKKEY_UNLOCK               0x000093F7UL                         /**< Mode UNLOCK for CMU_WDOGLOCK                */
#define CMU_WDOGLOCK_LOCKKEY_DEFAULT               (_CMU_WDOGLOCK_LOCKKEY_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_WDOGLOCK       */
#define CMU_WDOGLOCK_LOCKKEY_UNLOCK                (_CMU_WDOGLOCK_LOCKKEY_UNLOCK << 0)  /**< Shifted mode UNLOCK for CMU_WDOGLOCK        */

/* Bit fields for CMU IF */
#define _CMU_IF_RESETVALUE                         0x00000000UL                         /**< Default value for CMU_IF                    */
#define _CMU_IF_MASK                               0x00000003UL                         /**< Mask for CMU_IF                             */
#define CMU_IF_CALRDY                              (0x1UL << 0)                         /**< Calibration Ready Interrupt Flag            */
#define _CMU_IF_CALRDY_SHIFT                       0                                    /**< Shift value for CMU_CALRDY                  */
#define _CMU_IF_CALRDY_MASK                        0x1UL                                /**< Bit mask for CMU_CALRDY                     */
#define _CMU_IF_CALRDY_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for CMU_IF                     */
#define CMU_IF_CALRDY_DEFAULT                      (_CMU_IF_CALRDY_DEFAULT << 0)        /**< Shifted mode DEFAULT for CMU_IF             */
#define CMU_IF_CALOF                               (0x1UL << 1)                         /**< Calibration Overflow Interrupt Flag         */
#define _CMU_IF_CALOF_SHIFT                        1                                    /**< Shift value for CMU_CALOF                   */
#define _CMU_IF_CALOF_MASK                         0x2UL                                /**< Bit mask for CMU_CALOF                      */
#define _CMU_IF_CALOF_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for CMU_IF                     */
#define CMU_IF_CALOF_DEFAULT                       (_CMU_IF_CALOF_DEFAULT << 1)         /**< Shifted mode DEFAULT for CMU_IF             */

/* Bit fields for CMU IEN */
#define _CMU_IEN_RESETVALUE                        0x00000000UL                         /**< Default value for CMU_IEN                   */
#define _CMU_IEN_MASK                              0x00000003UL                         /**< Mask for CMU_IEN                            */
#define CMU_IEN_CALRDY                             (0x1UL << 0)                         /**< Calibration Ready Interrupt Enable          */
#define _CMU_IEN_CALRDY_SHIFT                      0                                    /**< Shift value for CMU_CALRDY                  */
#define _CMU_IEN_CALRDY_MASK                       0x1UL                                /**< Bit mask for CMU_CALRDY                     */
#define _CMU_IEN_CALRDY_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for CMU_IEN                    */
#define CMU_IEN_CALRDY_DEFAULT                     (_CMU_IEN_CALRDY_DEFAULT << 0)       /**< Shifted mode DEFAULT for CMU_IEN            */
#define CMU_IEN_CALOF                              (0x1UL << 1)                         /**< Calibration Overflow Interrupt Enable       */
#define _CMU_IEN_CALOF_SHIFT                       1                                    /**< Shift value for CMU_CALOF                   */
#define _CMU_IEN_CALOF_MASK                        0x2UL                                /**< Bit mask for CMU_CALOF                      */
#define _CMU_IEN_CALOF_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for CMU_IEN                    */
#define CMU_IEN_CALOF_DEFAULT                      (_CMU_IEN_CALOF_DEFAULT << 1)        /**< Shifted mode DEFAULT for CMU_IEN            */

/* Bit fields for CMU CALCMD */
#define _CMU_CALCMD_RESETVALUE                     0x00000000UL                         /**< Default value for CMU_CALCMD                */
#define _CMU_CALCMD_MASK                           0x00000003UL                         /**< Mask for CMU_CALCMD                         */
#define CMU_CALCMD_CALSTART                        (0x1UL << 0)                         /**< Calibration Start                           */
#define _CMU_CALCMD_CALSTART_SHIFT                 0                                    /**< Shift value for CMU_CALSTART                */
#define _CMU_CALCMD_CALSTART_MASK                  0x1UL                                /**< Bit mask for CMU_CALSTART                   */
#define _CMU_CALCMD_CALSTART_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for CMU_CALCMD                 */
#define CMU_CALCMD_CALSTART_DEFAULT                (_CMU_CALCMD_CALSTART_DEFAULT << 0)  /**< Shifted mode DEFAULT for CMU_CALCMD         */
#define CMU_CALCMD_CALSTOP                         (0x1UL << 1)                         /**< Calibration Stop                            */
#define _CMU_CALCMD_CALSTOP_SHIFT                  1                                    /**< Shift value for CMU_CALSTOP                 */
#define _CMU_CALCMD_CALSTOP_MASK                   0x2UL                                /**< Bit mask for CMU_CALSTOP                    */
#define _CMU_CALCMD_CALSTOP_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for CMU_CALCMD                 */
#define CMU_CALCMD_CALSTOP_DEFAULT                 (_CMU_CALCMD_CALSTOP_DEFAULT << 1)   /**< Shifted mode DEFAULT for CMU_CALCMD         */

/* Bit fields for CMU CALCTRL */
#define _CMU_CALCTRL_RESETVALUE                    0x00000000UL                           /**< Default value for CMU_CALCTRL               */
#define _CMU_CALCTRL_MASK                          0xFF8FFFFFUL                           /**< Mask for CMU_CALCTRL                        */
#define _CMU_CALCTRL_CALTOP_SHIFT                  0                                      /**< Shift value for CMU_CALTOP                  */
#define _CMU_CALCTRL_CALTOP_MASK                   0xFFFFFUL                              /**< Bit mask for CMU_CALTOP                     */
#define _CMU_CALCTRL_CALTOP_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for CMU_CALCTRL                */
#define CMU_CALCTRL_CALTOP_DEFAULT                 (_CMU_CALCTRL_CALTOP_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_CALCTRL        */
#define CMU_CALCTRL_CONT                           (0x1UL << 23)                          /**< Continuous Calibration                      */
#define _CMU_CALCTRL_CONT_SHIFT                    23                                     /**< Shift value for CMU_CONT                    */
#define _CMU_CALCTRL_CONT_MASK                     0x800000UL                             /**< Bit mask for CMU_CONT                       */
#define _CMU_CALCTRL_CONT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for CMU_CALCTRL                */
#define CMU_CALCTRL_CONT_DEFAULT                   (_CMU_CALCTRL_CONT_DEFAULT << 23)      /**< Shifted mode DEFAULT for CMU_CALCTRL        */
#define _CMU_CALCTRL_UPSEL_SHIFT                   24                                     /**< Shift value for CMU_UPSEL                   */
#define _CMU_CALCTRL_UPSEL_MASK                    0xF000000UL                            /**< Bit mask for CMU_UPSEL                      */
#define _CMU_CALCTRL_UPSEL_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for CMU_CALCTRL                */
#define _CMU_CALCTRL_UPSEL_DISABLED                0x00000000UL                           /**< Mode DISABLED for CMU_CALCTRL               */
#define _CMU_CALCTRL_UPSEL_PRS                     0x00000001UL                           /**< Mode PRS for CMU_CALCTRL                    */
#define _CMU_CALCTRL_UPSEL_HFXO                    0x00000002UL                           /**< Mode HFXO for CMU_CALCTRL                   */
#define _CMU_CALCTRL_UPSEL_LFXO                    0x00000003UL                           /**< Mode LFXO for CMU_CALCTRL                   */
#define _CMU_CALCTRL_UPSEL_HFRCODPLL               0x00000004UL                           /**< Mode HFRCODPLL for CMU_CALCTRL              */
#define _CMU_CALCTRL_UPSEL_HFRCOEM23               0x00000005UL                           /**< Mode HFRCOEM23 for CMU_CALCTRL              */
#define _CMU_CALCTRL_UPSEL_FSRCO                   0x00000008UL                           /**< Mode FSRCO for CMU_CALCTRL                  */
#define _CMU_CALCTRL_UPSEL_LFRCO                   0x00000009UL                           /**< Mode LFRCO for CMU_CALCTRL                  */
#define _CMU_CALCTRL_UPSEL_ULFRCO                  0x0000000AUL                           /**< Mode ULFRCO for CMU_CALCTRL                 */
#define CMU_CALCTRL_UPSEL_DEFAULT                  (_CMU_CALCTRL_UPSEL_DEFAULT << 24)     /**< Shifted mode DEFAULT for CMU_CALCTRL        */
#define CMU_CALCTRL_UPSEL_DISABLED                 (_CMU_CALCTRL_UPSEL_DISABLED << 24)    /**< Shifted mode DISABLED for CMU_CALCTRL       */
#define CMU_CALCTRL_UPSEL_PRS                      (_CMU_CALCTRL_UPSEL_PRS << 24)         /**< Shifted mode PRS for CMU_CALCTRL            */
#define CMU_CALCTRL_UPSEL_HFXO                     (_CMU_CALCTRL_UPSEL_HFXO << 24)        /**< Shifted mode HFXO for CMU_CALCTRL           */
#define CMU_CALCTRL_UPSEL_LFXO                     (_CMU_CALCTRL_UPSEL_LFXO << 24)        /**< Shifted mode LFXO for CMU_CALCTRL           */
#define CMU_CALCTRL_UPSEL_HFRCODPLL                (_CMU_CALCTRL_UPSEL_HFRCODPLL << 24)   /**< Shifted mode HFRCODPLL for CMU_CALCTRL      */
#define CMU_CALCTRL_UPSEL_HFRCOEM23                (_CMU_CALCTRL_UPSEL_HFRCOEM23 << 24)   /**< Shifted mode HFRCOEM23 for CMU_CALCTRL      */
#define CMU_CALCTRL_UPSEL_FSRCO                    (_CMU_CALCTRL_UPSEL_FSRCO << 24)       /**< Shifted mode FSRCO for CMU_CALCTRL          */
#define CMU_CALCTRL_UPSEL_LFRCO                    (_CMU_CALCTRL_UPSEL_LFRCO << 24)       /**< Shifted mode LFRCO for CMU_CALCTRL          */
#define CMU_CALCTRL_UPSEL_ULFRCO                   (_CMU_CALCTRL_UPSEL_ULFRCO << 24)      /**< Shifted mode ULFRCO for CMU_CALCTRL         */
#define _CMU_CALCTRL_DOWNSEL_SHIFT                 28                                     /**< Shift value for CMU_DOWNSEL                 */
#define _CMU_CALCTRL_DOWNSEL_MASK                  0xF0000000UL                           /**< Bit mask for CMU_DOWNSEL                    */
#define _CMU_CALCTRL_DOWNSEL_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for CMU_CALCTRL                */
#define _CMU_CALCTRL_DOWNSEL_DISABLED              0x00000000UL                           /**< Mode DISABLED for CMU_CALCTRL               */
#define _CMU_CALCTRL_DOWNSEL_HCLK                  0x00000001UL                           /**< Mode HCLK for CMU_CALCTRL                   */
#define _CMU_CALCTRL_DOWNSEL_PRS                   0x00000002UL                           /**< Mode PRS for CMU_CALCTRL                    */
#define _CMU_CALCTRL_DOWNSEL_HFXO                  0x00000003UL                           /**< Mode HFXO for CMU_CALCTRL                   */
#define _CMU_CALCTRL_DOWNSEL_LFXO                  0x00000004UL                           /**< Mode LFXO for CMU_CALCTRL                   */
#define _CMU_CALCTRL_DOWNSEL_HFRCODPLL             0x00000005UL                           /**< Mode HFRCODPLL for CMU_CALCTRL              */
#define _CMU_CALCTRL_DOWNSEL_HFRCOEM23             0x00000006UL                           /**< Mode HFRCOEM23 for CMU_CALCTRL              */
#define _CMU_CALCTRL_DOWNSEL_FSRCO                 0x00000009UL                           /**< Mode FSRCO for CMU_CALCTRL                  */
#define _CMU_CALCTRL_DOWNSEL_LFRCO                 0x0000000AUL                           /**< Mode LFRCO for CMU_CALCTRL                  */
#define _CMU_CALCTRL_DOWNSEL_ULFRCO                0x0000000BUL                           /**< Mode ULFRCO for CMU_CALCTRL                 */
#define CMU_CALCTRL_DOWNSEL_DEFAULT                (_CMU_CALCTRL_DOWNSEL_DEFAULT << 28)   /**< Shifted mode DEFAULT for CMU_CALCTRL        */
#define CMU_CALCTRL_DOWNSEL_DISABLED               (_CMU_CALCTRL_DOWNSEL_DISABLED << 28)  /**< Shifted mode DISABLED for CMU_CALCTRL       */
#define CMU_CALCTRL_DOWNSEL_HCLK                   (_CMU_CALCTRL_DOWNSEL_HCLK << 28)      /**< Shifted mode HCLK for CMU_CALCTRL           */
#define CMU_CALCTRL_DOWNSEL_PRS                    (_CMU_CALCTRL_DOWNSEL_PRS << 28)       /**< Shifted mode PRS for CMU_CALCTRL            */
#define CMU_CALCTRL_DOWNSEL_HFXO                   (_CMU_CALCTRL_DOWNSEL_HFXO << 28)      /**< Shifted mode HFXO for CMU_CALCTRL           */
#define CMU_CALCTRL_DOWNSEL_LFXO                   (_CMU_CALCTRL_DOWNSEL_LFXO << 28)      /**< Shifted mode LFXO for CMU_CALCTRL           */
#define CMU_CALCTRL_DOWNSEL_HFRCODPLL              (_CMU_CALCTRL_DOWNSEL_HFRCODPLL << 28) /**< Shifted mode HFRCODPLL for CMU_CALCTRL      */
#define CMU_CALCTRL_DOWNSEL_HFRCOEM23              (_CMU_CALCTRL_DOWNSEL_HFRCOEM23 << 28) /**< Shifted mode HFRCOEM23 for CMU_CALCTRL      */
#define CMU_CALCTRL_DOWNSEL_FSRCO                  (_CMU_CALCTRL_DOWNSEL_FSRCO << 28)     /**< Shifted mode FSRCO for CMU_CALCTRL          */
#define CMU_CALCTRL_DOWNSEL_LFRCO                  (_CMU_CALCTRL_DOWNSEL_LFRCO << 28)     /**< Shifted mode LFRCO for CMU_CALCTRL          */
#define CMU_CALCTRL_DOWNSEL_ULFRCO                 (_CMU_CALCTRL_DOWNSEL_ULFRCO << 28)    /**< Shifted mode ULFRCO for CMU_CALCTRL         */

/* Bit fields for CMU CALCNT */
#define _CMU_CALCNT_RESETVALUE                     0x00000000UL                         /**< Default value for CMU_CALCNT                */
#define _CMU_CALCNT_MASK                           0x000FFFFFUL                         /**< Mask for CMU_CALCNT                         */
#define _CMU_CALCNT_CALCNT_SHIFT                   0                                    /**< Shift value for CMU_CALCNT                  */
#define _CMU_CALCNT_CALCNT_MASK                    0xFFFFFUL                            /**< Bit mask for CMU_CALCNT                     */
#define _CMU_CALCNT_CALCNT_DEFAULT                 0x00000000UL                         /**< Mode DEFAULT for CMU_CALCNT                 */
#define CMU_CALCNT_CALCNT_DEFAULT                  (_CMU_CALCNT_CALCNT_DEFAULT << 0)    /**< Shifted mode DEFAULT for CMU_CALCNT         */

/* Bit fields for CMU CLKEN0 */
#define _CMU_CLKEN0_RESETVALUE                     0x00000000UL                          /**< Default value for CMU_CLKEN0                */
#define _CMU_CLKEN0_MASK                           0xFFFFFFFFUL                          /**< Mask for CMU_CLKEN0                         */
#define CMU_CLKEN0_LDMA                            (0x1UL << 0)                          /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_LDMA_SHIFT                     0                                     /**< Shift value for CMU_LDMA                    */
#define _CMU_CLKEN0_LDMA_MASK                      0x1UL                                 /**< Bit mask for CMU_LDMA                       */
#define _CMU_CLKEN0_LDMA_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_LDMA_DEFAULT                    (_CMU_CLKEN0_LDMA_DEFAULT << 0)       /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_LDMAXBAR                        (0x1UL << 1)                          /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_LDMAXBAR_SHIFT                 1                                     /**< Shift value for CMU_LDMAXBAR                */
#define _CMU_CLKEN0_LDMAXBAR_MASK                  0x2UL                                 /**< Bit mask for CMU_LDMAXBAR                   */
#define _CMU_CLKEN0_LDMAXBAR_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_LDMAXBAR_DEFAULT                (_CMU_CLKEN0_LDMAXBAR_DEFAULT << 1)   /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_RADIOAES                        (0x1UL << 2)                          /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_RADIOAES_SHIFT                 2                                     /**< Shift value for CMU_RADIOAES                */
#define _CMU_CLKEN0_RADIOAES_MASK                  0x4UL                                 /**< Bit mask for CMU_RADIOAES                   */
#define _CMU_CLKEN0_RADIOAES_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_RADIOAES_DEFAULT                (_CMU_CLKEN0_RADIOAES_DEFAULT << 2)   /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_GPCRC                           (0x1UL << 3)                          /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_GPCRC_SHIFT                    3                                     /**< Shift value for CMU_GPCRC                   */
#define _CMU_CLKEN0_GPCRC_MASK                     0x8UL                                 /**< Bit mask for CMU_GPCRC                      */
#define _CMU_CLKEN0_GPCRC_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_GPCRC_DEFAULT                   (_CMU_CLKEN0_GPCRC_DEFAULT << 3)      /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_TIMER0                          (0x1UL << 4)                          /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_TIMER0_SHIFT                   4                                     /**< Shift value for CMU_TIMER0                  */
#define _CMU_CLKEN0_TIMER0_MASK                    0x10UL                                /**< Bit mask for CMU_TIMER0                     */
#define _CMU_CLKEN0_TIMER0_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_TIMER0_DEFAULT                  (_CMU_CLKEN0_TIMER0_DEFAULT << 4)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_TIMER1                          (0x1UL << 5)                          /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_TIMER1_SHIFT                   5                                     /**< Shift value for CMU_TIMER1                  */
#define _CMU_CLKEN0_TIMER1_MASK                    0x20UL                                /**< Bit mask for CMU_TIMER1                     */
#define _CMU_CLKEN0_TIMER1_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_TIMER1_DEFAULT                  (_CMU_CLKEN0_TIMER1_DEFAULT << 5)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_TIMER2                          (0x1UL << 6)                          /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_TIMER2_SHIFT                   6                                     /**< Shift value for CMU_TIMER2                  */
#define _CMU_CLKEN0_TIMER2_MASK                    0x40UL                                /**< Bit mask for CMU_TIMER2                     */
#define _CMU_CLKEN0_TIMER2_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_TIMER2_DEFAULT                  (_CMU_CLKEN0_TIMER2_DEFAULT << 6)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_TIMER3                          (0x1UL << 7)                          /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_TIMER3_SHIFT                   7                                     /**< Shift value for CMU_TIMER3                  */
#define _CMU_CLKEN0_TIMER3_MASK                    0x80UL                                /**< Bit mask for CMU_TIMER3                     */
#define _CMU_CLKEN0_TIMER3_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_TIMER3_DEFAULT                  (_CMU_CLKEN0_TIMER3_DEFAULT << 7)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_TIMER4                          (0x1UL << 8)                          /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_TIMER4_SHIFT                   8                                     /**< Shift value for CMU_TIMER4                  */
#define _CMU_CLKEN0_TIMER4_MASK                    0x100UL                               /**< Bit mask for CMU_TIMER4                     */
#define _CMU_CLKEN0_TIMER4_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_TIMER4_DEFAULT                  (_CMU_CLKEN0_TIMER4_DEFAULT << 8)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_USART0                          (0x1UL << 9)                          /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_USART0_SHIFT                   9                                     /**< Shift value for CMU_USART0                  */
#define _CMU_CLKEN0_USART0_MASK                    0x200UL                               /**< Bit mask for CMU_USART0                     */
#define _CMU_CLKEN0_USART0_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_USART0_DEFAULT                  (_CMU_CLKEN0_USART0_DEFAULT << 9)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_IADC0                           (0x1UL << 10)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_IADC0_SHIFT                    10                                    /**< Shift value for CMU_IADC0                   */
#define _CMU_CLKEN0_IADC0_MASK                     0x400UL                               /**< Bit mask for CMU_IADC0                      */
#define _CMU_CLKEN0_IADC0_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_IADC0_DEFAULT                   (_CMU_CLKEN0_IADC0_DEFAULT << 10)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_AMUXCP0                         (0x1UL << 11)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_AMUXCP0_SHIFT                  11                                    /**< Shift value for CMU_AMUXCP0                 */
#define _CMU_CLKEN0_AMUXCP0_MASK                   0x800UL                               /**< Bit mask for CMU_AMUXCP0                    */
#define _CMU_CLKEN0_AMUXCP0_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_AMUXCP0_DEFAULT                 (_CMU_CLKEN0_AMUXCP0_DEFAULT << 11)   /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_LETIMER0                        (0x1UL << 12)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_LETIMER0_SHIFT                 12                                    /**< Shift value for CMU_LETIMER0                */
#define _CMU_CLKEN0_LETIMER0_MASK                  0x1000UL                              /**< Bit mask for CMU_LETIMER0                   */
#define _CMU_CLKEN0_LETIMER0_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_LETIMER0_DEFAULT                (_CMU_CLKEN0_LETIMER0_DEFAULT << 12)  /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_WDOG0                           (0x1UL << 13)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_WDOG0_SHIFT                    13                                    /**< Shift value for CMU_WDOG0                   */
#define _CMU_CLKEN0_WDOG0_MASK                     0x2000UL                              /**< Bit mask for CMU_WDOG0                      */
#define _CMU_CLKEN0_WDOG0_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_WDOG0_DEFAULT                   (_CMU_CLKEN0_WDOG0_DEFAULT << 13)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_I2C0                            (0x1UL << 14)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_I2C0_SHIFT                     14                                    /**< Shift value for CMU_I2C0                    */
#define _CMU_CLKEN0_I2C0_MASK                      0x4000UL                              /**< Bit mask for CMU_I2C0                       */
#define _CMU_CLKEN0_I2C0_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_I2C0_DEFAULT                    (_CMU_CLKEN0_I2C0_DEFAULT << 14)      /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_I2C1                            (0x1UL << 15)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_I2C1_SHIFT                     15                                    /**< Shift value for CMU_I2C1                    */
#define _CMU_CLKEN0_I2C1_MASK                      0x8000UL                              /**< Bit mask for CMU_I2C1                       */
#define _CMU_CLKEN0_I2C1_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_I2C1_DEFAULT                    (_CMU_CLKEN0_I2C1_DEFAULT << 15)      /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_SYSCFG                          (0x1UL << 16)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_SYSCFG_SHIFT                   16                                    /**< Shift value for CMU_SYSCFG                  */
#define _CMU_CLKEN0_SYSCFG_MASK                    0x10000UL                             /**< Bit mask for CMU_SYSCFG                     */
#define _CMU_CLKEN0_SYSCFG_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_SYSCFG_DEFAULT                  (_CMU_CLKEN0_SYSCFG_DEFAULT << 16)    /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_DPLL0                           (0x1UL << 17)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_DPLL0_SHIFT                    17                                    /**< Shift value for CMU_DPLL0                   */
#define _CMU_CLKEN0_DPLL0_MASK                     0x20000UL                             /**< Bit mask for CMU_DPLL0                      */
#define _CMU_CLKEN0_DPLL0_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_DPLL0_DEFAULT                   (_CMU_CLKEN0_DPLL0_DEFAULT << 17)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_HFRCO0                          (0x1UL << 18)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_HFRCO0_SHIFT                   18                                    /**< Shift value for CMU_HFRCO0                  */
#define _CMU_CLKEN0_HFRCO0_MASK                    0x40000UL                             /**< Bit mask for CMU_HFRCO0                     */
#define _CMU_CLKEN0_HFRCO0_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_HFRCO0_DEFAULT                  (_CMU_CLKEN0_HFRCO0_DEFAULT << 18)    /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_HFRCOEM23                       (0x1UL << 19)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_HFRCOEM23_SHIFT                19                                    /**< Shift value for CMU_HFRCOEM23               */
#define _CMU_CLKEN0_HFRCOEM23_MASK                 0x80000UL                             /**< Bit mask for CMU_HFRCOEM23                  */
#define _CMU_CLKEN0_HFRCOEM23_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_HFRCOEM23_DEFAULT               (_CMU_CLKEN0_HFRCOEM23_DEFAULT << 19) /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_HFXO0                           (0x1UL << 20)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_HFXO0_SHIFT                    20                                    /**< Shift value for CMU_HFXO0                   */
#define _CMU_CLKEN0_HFXO0_MASK                     0x100000UL                            /**< Bit mask for CMU_HFXO0                      */
#define _CMU_CLKEN0_HFXO0_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_HFXO0_DEFAULT                   (_CMU_CLKEN0_HFXO0_DEFAULT << 20)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_FSRCO                           (0x1UL << 21)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_FSRCO_SHIFT                    21                                    /**< Shift value for CMU_FSRCO                   */
#define _CMU_CLKEN0_FSRCO_MASK                     0x200000UL                            /**< Bit mask for CMU_FSRCO                      */
#define _CMU_CLKEN0_FSRCO_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_FSRCO_DEFAULT                   (_CMU_CLKEN0_FSRCO_DEFAULT << 21)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_LFRCO                           (0x1UL << 22)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_LFRCO_SHIFT                    22                                    /**< Shift value for CMU_LFRCO                   */
#define _CMU_CLKEN0_LFRCO_MASK                     0x400000UL                            /**< Bit mask for CMU_LFRCO                      */
#define _CMU_CLKEN0_LFRCO_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_LFRCO_DEFAULT                   (_CMU_CLKEN0_LFRCO_DEFAULT << 22)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_LFXO                            (0x1UL << 23)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_LFXO_SHIFT                     23                                    /**< Shift value for CMU_LFXO                    */
#define _CMU_CLKEN0_LFXO_MASK                      0x800000UL                            /**< Bit mask for CMU_LFXO                       */
#define _CMU_CLKEN0_LFXO_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_LFXO_DEFAULT                    (_CMU_CLKEN0_LFXO_DEFAULT << 23)      /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_ULFRCO                          (0x1UL << 24)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_ULFRCO_SHIFT                   24                                    /**< Shift value for CMU_ULFRCO                  */
#define _CMU_CLKEN0_ULFRCO_MASK                    0x1000000UL                           /**< Bit mask for CMU_ULFRCO                     */
#define _CMU_CLKEN0_ULFRCO_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_ULFRCO_DEFAULT                  (_CMU_CLKEN0_ULFRCO_DEFAULT << 24)    /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_LESENSE                         (0x1UL << 25)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_LESENSE_SHIFT                  25                                    /**< Shift value for CMU_LESENSE                 */
#define _CMU_CLKEN0_LESENSE_MASK                   0x2000000UL                           /**< Bit mask for CMU_LESENSE                    */
#define _CMU_CLKEN0_LESENSE_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_LESENSE_DEFAULT                 (_CMU_CLKEN0_LESENSE_DEFAULT << 25)   /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_GPIO                            (0x1UL << 26)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_GPIO_SHIFT                     26                                    /**< Shift value for CMU_GPIO                    */
#define _CMU_CLKEN0_GPIO_MASK                      0x4000000UL                           /**< Bit mask for CMU_GPIO                       */
#define _CMU_CLKEN0_GPIO_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_GPIO_DEFAULT                    (_CMU_CLKEN0_GPIO_DEFAULT << 26)      /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_PRS                             (0x1UL << 27)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_PRS_SHIFT                      27                                    /**< Shift value for CMU_PRS                     */
#define _CMU_CLKEN0_PRS_MASK                       0x8000000UL                           /**< Bit mask for CMU_PRS                        */
#define _CMU_CLKEN0_PRS_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_PRS_DEFAULT                     (_CMU_CLKEN0_PRS_DEFAULT << 27)       /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_BURAM                           (0x1UL << 28)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_BURAM_SHIFT                    28                                    /**< Shift value for CMU_BURAM                   */
#define _CMU_CLKEN0_BURAM_MASK                     0x10000000UL                          /**< Bit mask for CMU_BURAM                      */
#define _CMU_CLKEN0_BURAM_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_BURAM_DEFAULT                   (_CMU_CLKEN0_BURAM_DEFAULT << 28)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_BURTC                           (0x1UL << 29)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_BURTC_SHIFT                    29                                    /**< Shift value for CMU_BURTC                   */
#define _CMU_CLKEN0_BURTC_MASK                     0x20000000UL                          /**< Bit mask for CMU_BURTC                      */
#define _CMU_CLKEN0_BURTC_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_BURTC_DEFAULT                   (_CMU_CLKEN0_BURTC_DEFAULT << 29)     /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_SYSRTC0                         (0x1UL << 30)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_SYSRTC0_SHIFT                  30                                    /**< Shift value for CMU_SYSRTC0                 */
#define _CMU_CLKEN0_SYSRTC0_MASK                   0x40000000UL                          /**< Bit mask for CMU_SYSRTC0                    */
#define _CMU_CLKEN0_SYSRTC0_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_SYSRTC0_DEFAULT                 (_CMU_CLKEN0_SYSRTC0_DEFAULT << 30)   /**< Shifted mode DEFAULT for CMU_CLKEN0         */
#define CMU_CLKEN0_DCDC                            (0x1UL << 31)                         /**< Enable Bus Clock                            */
#define _CMU_CLKEN0_DCDC_SHIFT                     31                                    /**< Shift value for CMU_DCDC                    */
#define _CMU_CLKEN0_DCDC_MASK                      0x80000000UL                          /**< Bit mask for CMU_DCDC                       */
#define _CMU_CLKEN0_DCDC_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for CMU_CLKEN0                 */
#define CMU_CLKEN0_DCDC_DEFAULT                    (_CMU_CLKEN0_DCDC_DEFAULT << 31)      /**< Shifted mode DEFAULT for CMU_CLKEN0         */

/* Bit fields for CMU CLKEN1 */
#define _CMU_CLKEN1_RESETVALUE                     0x00000000UL                              /**< Default value for CMU_CLKEN1                */
#define _CMU_CLKEN1_MASK                           0x1FFFFFFFUL                              /**< Mask for CMU_CLKEN1                         */
#define CMU_CLKEN1_AGC                             (0x1UL << 0)                              /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_AGC_SHIFT                      0                                         /**< Shift value for CMU_AGC                     */
#define _CMU_CLKEN1_AGC_MASK                       0x1UL                                     /**< Bit mask for CMU_AGC                        */
#define _CMU_CLKEN1_AGC_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_AGC_DEFAULT                     (_CMU_CLKEN1_AGC_DEFAULT << 0)            /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_MODEM                           (0x1UL << 1)                              /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_MODEM_SHIFT                    1                                         /**< Shift value for CMU_MODEM                   */
#define _CMU_CLKEN1_MODEM_MASK                     0x2UL                                     /**< Bit mask for CMU_MODEM                      */
#define _CMU_CLKEN1_MODEM_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_MODEM_DEFAULT                   (_CMU_CLKEN1_MODEM_DEFAULT << 1)          /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_RFCRC                           (0x1UL << 2)                              /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_RFCRC_SHIFT                    2                                         /**< Shift value for CMU_RFCRC                   */
#define _CMU_CLKEN1_RFCRC_MASK                     0x4UL                                     /**< Bit mask for CMU_RFCRC                      */
#define _CMU_CLKEN1_RFCRC_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_RFCRC_DEFAULT                   (_CMU_CLKEN1_RFCRC_DEFAULT << 2)          /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_FRC                             (0x1UL << 3)                              /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_FRC_SHIFT                      3                                         /**< Shift value for CMU_FRC                     */
#define _CMU_CLKEN1_FRC_MASK                       0x8UL                                     /**< Bit mask for CMU_FRC                        */
#define _CMU_CLKEN1_FRC_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_FRC_DEFAULT                     (_CMU_CLKEN1_FRC_DEFAULT << 3)            /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_PROTIMER                        (0x1UL << 4)                              /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_PROTIMER_SHIFT                 4                                         /**< Shift value for CMU_PROTIMER                */
#define _CMU_CLKEN1_PROTIMER_MASK                  0x10UL                                    /**< Bit mask for CMU_PROTIMER                   */
#define _CMU_CLKEN1_PROTIMER_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_PROTIMER_DEFAULT                (_CMU_CLKEN1_PROTIMER_DEFAULT << 4)       /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_RAC                             (0x1UL << 5)                              /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_RAC_SHIFT                      5                                         /**< Shift value for CMU_RAC                     */
#define _CMU_CLKEN1_RAC_MASK                       0x20UL                                    /**< Bit mask for CMU_RAC                        */
#define _CMU_CLKEN1_RAC_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_RAC_DEFAULT                     (_CMU_CLKEN1_RAC_DEFAULT << 5)            /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_SYNTH                           (0x1UL << 6)                              /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_SYNTH_SHIFT                    6                                         /**< Shift value for CMU_SYNTH                   */
#define _CMU_CLKEN1_SYNTH_MASK                     0x40UL                                    /**< Bit mask for CMU_SYNTH                      */
#define _CMU_CLKEN1_SYNTH_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_SYNTH_DEFAULT                   (_CMU_CLKEN1_SYNTH_DEFAULT << 6)          /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_RFSCRATCHPAD                    (0x1UL << 7)                              /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_RFSCRATCHPAD_SHIFT             7                                         /**< Shift value for CMU_RFSCRATCHPAD            */
#define _CMU_CLKEN1_RFSCRATCHPAD_MASK              0x80UL                                    /**< Bit mask for CMU_RFSCRATCHPAD               */
#define _CMU_CLKEN1_RFSCRATCHPAD_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_RFSCRATCHPAD_DEFAULT            (_CMU_CLKEN1_RFSCRATCHPAD_DEFAULT << 7)   /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_HOSTMAILBOX                     (0x1UL << 8)                              /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_HOSTMAILBOX_SHIFT              8                                         /**< Shift value for CMU_HOSTMAILBOX             */
#define _CMU_CLKEN1_HOSTMAILBOX_MASK               0x100UL                                   /**< Bit mask for CMU_HOSTMAILBOX                */
#define _CMU_CLKEN1_HOSTMAILBOX_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_HOSTMAILBOX_DEFAULT             (_CMU_CLKEN1_HOSTMAILBOX_DEFAULT << 8)    /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_RFMAILBOX                       (0x1UL << 9)                              /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_RFMAILBOX_SHIFT                9                                         /**< Shift value for CMU_RFMAILBOX               */
#define _CMU_CLKEN1_RFMAILBOX_MASK                 0x200UL                                   /**< Bit mask for CMU_RFMAILBOX                  */
#define _CMU_CLKEN1_RFMAILBOX_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_RFMAILBOX_DEFAULT               (_CMU_CLKEN1_RFMAILBOX_DEFAULT << 9)      /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_SEMAILBOXHOST                   (0x1UL << 10)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_SEMAILBOXHOST_SHIFT            10                                        /**< Shift value for CMU_SEMAILBOXHOST           */
#define _CMU_CLKEN1_SEMAILBOXHOST_MASK             0x400UL                                   /**< Bit mask for CMU_SEMAILBOXHOST              */
#define _CMU_CLKEN1_SEMAILBOXHOST_DEFAULT          0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_SEMAILBOXHOST_DEFAULT           (_CMU_CLKEN1_SEMAILBOXHOST_DEFAULT << 10) /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_BUFC                            (0x1UL << 11)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_BUFC_SHIFT                     11                                        /**< Shift value for CMU_BUFC                    */
#define _CMU_CLKEN1_BUFC_MASK                      0x800UL                                   /**< Bit mask for CMU_BUFC                       */
#define _CMU_CLKEN1_BUFC_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_BUFC_DEFAULT                    (_CMU_CLKEN1_BUFC_DEFAULT << 11)          /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_LCD                             (0x1UL << 12)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_LCD_SHIFT                      12                                        /**< Shift value for CMU_LCD                     */
#define _CMU_CLKEN1_LCD_MASK                       0x1000UL                                  /**< Bit mask for CMU_LCD                        */
#define _CMU_CLKEN1_LCD_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_LCD_DEFAULT                     (_CMU_CLKEN1_LCD_DEFAULT << 12)           /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_KEYSCAN                         (0x1UL << 13)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_KEYSCAN_SHIFT                  13                                        /**< Shift value for CMU_KEYSCAN                 */
#define _CMU_CLKEN1_KEYSCAN_MASK                   0x2000UL                                  /**< Bit mask for CMU_KEYSCAN                    */
#define _CMU_CLKEN1_KEYSCAN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_KEYSCAN_DEFAULT                 (_CMU_CLKEN1_KEYSCAN_DEFAULT << 13)       /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_SMU                             (0x1UL << 14)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_SMU_SHIFT                      14                                        /**< Shift value for CMU_SMU                     */
#define _CMU_CLKEN1_SMU_MASK                       0x4000UL                                  /**< Bit mask for CMU_SMU                        */
#define _CMU_CLKEN1_SMU_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_SMU_DEFAULT                     (_CMU_CLKEN1_SMU_DEFAULT << 14)           /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_ICACHE0                         (0x1UL << 15)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_ICACHE0_SHIFT                  15                                        /**< Shift value for CMU_ICACHE0                 */
#define _CMU_CLKEN1_ICACHE0_MASK                   0x8000UL                                  /**< Bit mask for CMU_ICACHE0                    */
#define _CMU_CLKEN1_ICACHE0_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_ICACHE0_DEFAULT                 (_CMU_CLKEN1_ICACHE0_DEFAULT << 15)       /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_MSC                             (0x1UL << 16)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_MSC_SHIFT                      16                                        /**< Shift value for CMU_MSC                     */
#define _CMU_CLKEN1_MSC_MASK                       0x10000UL                                 /**< Bit mask for CMU_MSC                        */
#define _CMU_CLKEN1_MSC_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_MSC_DEFAULT                     (_CMU_CLKEN1_MSC_DEFAULT << 16)           /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_WDOG1                           (0x1UL << 17)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_WDOG1_SHIFT                    17                                        /**< Shift value for CMU_WDOG1                   */
#define _CMU_CLKEN1_WDOG1_MASK                     0x20000UL                                 /**< Bit mask for CMU_WDOG1                      */
#define _CMU_CLKEN1_WDOG1_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_WDOG1_DEFAULT                   (_CMU_CLKEN1_WDOG1_DEFAULT << 17)         /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_ACMP0                           (0x1UL << 18)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_ACMP0_SHIFT                    18                                        /**< Shift value for CMU_ACMP0                   */
#define _CMU_CLKEN1_ACMP0_MASK                     0x40000UL                                 /**< Bit mask for CMU_ACMP0                      */
#define _CMU_CLKEN1_ACMP0_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_ACMP0_DEFAULT                   (_CMU_CLKEN1_ACMP0_DEFAULT << 18)         /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_ACMP1                           (0x1UL << 19)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_ACMP1_SHIFT                    19                                        /**< Shift value for CMU_ACMP1                   */
#define _CMU_CLKEN1_ACMP1_MASK                     0x80000UL                                 /**< Bit mask for CMU_ACMP1                      */
#define _CMU_CLKEN1_ACMP1_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_ACMP1_DEFAULT                   (_CMU_CLKEN1_ACMP1_DEFAULT << 19)         /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_VDAC0                           (0x1UL << 20)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_VDAC0_SHIFT                    20                                        /**< Shift value for CMU_VDAC0                   */
#define _CMU_CLKEN1_VDAC0_MASK                     0x100000UL                                /**< Bit mask for CMU_VDAC0                      */
#define _CMU_CLKEN1_VDAC0_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_VDAC0_DEFAULT                   (_CMU_CLKEN1_VDAC0_DEFAULT << 20)         /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_PCNT0                           (0x1UL << 21)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_PCNT0_SHIFT                    21                                        /**< Shift value for CMU_PCNT0                   */
#define _CMU_CLKEN1_PCNT0_MASK                     0x200000UL                                /**< Bit mask for CMU_PCNT0                      */
#define _CMU_CLKEN1_PCNT0_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_PCNT0_DEFAULT                   (_CMU_CLKEN1_PCNT0_DEFAULT << 21)         /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_EUSART0                         (0x1UL << 22)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_EUSART0_SHIFT                  22                                        /**< Shift value for CMU_EUSART0                 */
#define _CMU_CLKEN1_EUSART0_MASK                   0x400000UL                                /**< Bit mask for CMU_EUSART0                    */
#define _CMU_CLKEN1_EUSART0_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_EUSART0_DEFAULT                 (_CMU_CLKEN1_EUSART0_DEFAULT << 22)       /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_EUSART1                         (0x1UL << 23)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_EUSART1_SHIFT                  23                                        /**< Shift value for CMU_EUSART1                 */
#define _CMU_CLKEN1_EUSART1_MASK                   0x800000UL                                /**< Bit mask for CMU_EUSART1                    */
#define _CMU_CLKEN1_EUSART1_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_EUSART1_DEFAULT                 (_CMU_CLKEN1_EUSART1_DEFAULT << 23)       /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_EUSART2                         (0x1UL << 24)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_EUSART2_SHIFT                  24                                        /**< Shift value for CMU_EUSART2                 */
#define _CMU_CLKEN1_EUSART2_MASK                   0x1000000UL                               /**< Bit mask for CMU_EUSART2                    */
#define _CMU_CLKEN1_EUSART2_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_EUSART2_DEFAULT                 (_CMU_CLKEN1_EUSART2_DEFAULT << 24)       /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_RFECA0                          (0x1UL << 25)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_RFECA0_SHIFT                   25                                        /**< Shift value for CMU_RFECA0                  */
#define _CMU_CLKEN1_RFECA0_MASK                    0x2000000UL                               /**< Bit mask for CMU_RFECA0                     */
#define _CMU_CLKEN1_RFECA0_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_RFECA0_DEFAULT                  (_CMU_CLKEN1_RFECA0_DEFAULT << 25)        /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_RFECA1                          (0x1UL << 26)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_RFECA1_SHIFT                   26                                        /**< Shift value for CMU_RFECA1                  */
#define _CMU_CLKEN1_RFECA1_MASK                    0x4000000UL                               /**< Bit mask for CMU_RFECA1                     */
#define _CMU_CLKEN1_RFECA1_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_RFECA1_DEFAULT                  (_CMU_CLKEN1_RFECA1_DEFAULT << 26)        /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_DMEM                            (0x1UL << 27)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_DMEM_SHIFT                     27                                        /**< Shift value for CMU_DMEM                    */
#define _CMU_CLKEN1_DMEM_MASK                      0x8000000UL                               /**< Bit mask for CMU_DMEM                       */
#define _CMU_CLKEN1_DMEM_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_DMEM_DEFAULT                    (_CMU_CLKEN1_DMEM_DEFAULT << 27)          /**< Shifted mode DEFAULT for CMU_CLKEN1         */
#define CMU_CLKEN1_ECAIFADC                        (0x1UL << 28)                             /**< Enable Bus Clock                            */
#define _CMU_CLKEN1_ECAIFADC_SHIFT                 28                                        /**< Shift value for CMU_ECAIFADC                */
#define _CMU_CLKEN1_ECAIFADC_MASK                  0x10000000UL                              /**< Bit mask for CMU_ECAIFADC                   */
#define _CMU_CLKEN1_ECAIFADC_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for CMU_CLKEN1                 */
#define CMU_CLKEN1_ECAIFADC_DEFAULT                (_CMU_CLKEN1_ECAIFADC_DEFAULT << 28)      /**< Shifted mode DEFAULT for CMU_CLKEN1         */

/* Bit fields for CMU SYSCLKCTRL */
#define _CMU_SYSCLKCTRL_RESETVALUE                 0x00000001UL                               /**< Default value for CMU_SYSCLKCTRL            */
#define _CMU_SYSCLKCTRL_MASK                       0x0001F507UL                               /**< Mask for CMU_SYSCLKCTRL                     */
#define _CMU_SYSCLKCTRL_CLKSEL_SHIFT               0                                          /**< Shift value for CMU_CLKSEL                  */
#define _CMU_SYSCLKCTRL_CLKSEL_MASK                0x7UL                                      /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_SYSCLKCTRL_CLKSEL_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for CMU_SYSCLKCTRL             */
#define _CMU_SYSCLKCTRL_CLKSEL_FSRCO               0x00000001UL                               /**< Mode FSRCO for CMU_SYSCLKCTRL               */
#define _CMU_SYSCLKCTRL_CLKSEL_HFRCODPLL           0x00000002UL                               /**< Mode HFRCODPLL for CMU_SYSCLKCTRL           */
#define _CMU_SYSCLKCTRL_CLKSEL_HFXO                0x00000003UL                               /**< Mode HFXO for CMU_SYSCLKCTRL                */
#define _CMU_SYSCLKCTRL_CLKSEL_CLKIN0              0x00000004UL                               /**< Mode CLKIN0 for CMU_SYSCLKCTRL              */
#define CMU_SYSCLKCTRL_CLKSEL_DEFAULT              (_CMU_SYSCLKCTRL_CLKSEL_DEFAULT << 0)      /**< Shifted mode DEFAULT for CMU_SYSCLKCTRL     */
#define CMU_SYSCLKCTRL_CLKSEL_FSRCO                (_CMU_SYSCLKCTRL_CLKSEL_FSRCO << 0)        /**< Shifted mode FSRCO for CMU_SYSCLKCTRL       */
#define CMU_SYSCLKCTRL_CLKSEL_HFRCODPLL            (_CMU_SYSCLKCTRL_CLKSEL_HFRCODPLL << 0)    /**< Shifted mode HFRCODPLL for CMU_SYSCLKCTRL   */
#define CMU_SYSCLKCTRL_CLKSEL_HFXO                 (_CMU_SYSCLKCTRL_CLKSEL_HFXO << 0)         /**< Shifted mode HFXO for CMU_SYSCLKCTRL        */
#define CMU_SYSCLKCTRL_CLKSEL_CLKIN0               (_CMU_SYSCLKCTRL_CLKSEL_CLKIN0 << 0)       /**< Shifted mode CLKIN0 for CMU_SYSCLKCTRL      */
#define CMU_SYSCLKCTRL_PCLKPRESC                   (0x1UL << 10)                              /**< PCLK Prescaler                              */
#define _CMU_SYSCLKCTRL_PCLKPRESC_SHIFT            10                                         /**< Shift value for CMU_PCLKPRESC               */
#define _CMU_SYSCLKCTRL_PCLKPRESC_MASK             0x400UL                                    /**< Bit mask for CMU_PCLKPRESC                  */
#define _CMU_SYSCLKCTRL_PCLKPRESC_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for CMU_SYSCLKCTRL             */
#define _CMU_SYSCLKCTRL_PCLKPRESC_DIV1             0x00000000UL                               /**< Mode DIV1 for CMU_SYSCLKCTRL                */
#define _CMU_SYSCLKCTRL_PCLKPRESC_DIV2             0x00000001UL                               /**< Mode DIV2 for CMU_SYSCLKCTRL                */
#define CMU_SYSCLKCTRL_PCLKPRESC_DEFAULT           (_CMU_SYSCLKCTRL_PCLKPRESC_DEFAULT << 10)  /**< Shifted mode DEFAULT for CMU_SYSCLKCTRL     */
#define CMU_SYSCLKCTRL_PCLKPRESC_DIV1              (_CMU_SYSCLKCTRL_PCLKPRESC_DIV1 << 10)     /**< Shifted mode DIV1 for CMU_SYSCLKCTRL        */
#define CMU_SYSCLKCTRL_PCLKPRESC_DIV2              (_CMU_SYSCLKCTRL_PCLKPRESC_DIV2 << 10)     /**< Shifted mode DIV2 for CMU_SYSCLKCTRL        */
#define _CMU_SYSCLKCTRL_HCLKPRESC_SHIFT            12                                         /**< Shift value for CMU_HCLKPRESC               */
#define _CMU_SYSCLKCTRL_HCLKPRESC_MASK             0xF000UL                                   /**< Bit mask for CMU_HCLKPRESC                  */
#define _CMU_SYSCLKCTRL_HCLKPRESC_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for CMU_SYSCLKCTRL             */
#define _CMU_SYSCLKCTRL_HCLKPRESC_DIV1             0x00000000UL                               /**< Mode DIV1 for CMU_SYSCLKCTRL                */
#define _CMU_SYSCLKCTRL_HCLKPRESC_DIV2             0x00000001UL                               /**< Mode DIV2 for CMU_SYSCLKCTRL                */
#define _CMU_SYSCLKCTRL_HCLKPRESC_DIV4             0x00000003UL                               /**< Mode DIV4 for CMU_SYSCLKCTRL                */
#define _CMU_SYSCLKCTRL_HCLKPRESC_DIV8             0x00000007UL                               /**< Mode DIV8 for CMU_SYSCLKCTRL                */
#define _CMU_SYSCLKCTRL_HCLKPRESC_DIV16            0x0000000FUL                               /**< Mode DIV16 for CMU_SYSCLKCTRL               */
#define CMU_SYSCLKCTRL_HCLKPRESC_DEFAULT           (_CMU_SYSCLKCTRL_HCLKPRESC_DEFAULT << 12)  /**< Shifted mode DEFAULT for CMU_SYSCLKCTRL     */
#define CMU_SYSCLKCTRL_HCLKPRESC_DIV1              (_CMU_SYSCLKCTRL_HCLKPRESC_DIV1 << 12)     /**< Shifted mode DIV1 for CMU_SYSCLKCTRL        */
#define CMU_SYSCLKCTRL_HCLKPRESC_DIV2              (_CMU_SYSCLKCTRL_HCLKPRESC_DIV2 << 12)     /**< Shifted mode DIV2 for CMU_SYSCLKCTRL        */
#define CMU_SYSCLKCTRL_HCLKPRESC_DIV4              (_CMU_SYSCLKCTRL_HCLKPRESC_DIV4 << 12)     /**< Shifted mode DIV4 for CMU_SYSCLKCTRL        */
#define CMU_SYSCLKCTRL_HCLKPRESC_DIV8              (_CMU_SYSCLKCTRL_HCLKPRESC_DIV8 << 12)     /**< Shifted mode DIV8 for CMU_SYSCLKCTRL        */
#define CMU_SYSCLKCTRL_HCLKPRESC_DIV16             (_CMU_SYSCLKCTRL_HCLKPRESC_DIV16 << 12)    /**< Shifted mode DIV16 for CMU_SYSCLKCTRL       */
#define CMU_SYSCLKCTRL_RHCLKPRESC                  (0x1UL << 16)                              /**< Radio HCLK Prescaler                        */
#define _CMU_SYSCLKCTRL_RHCLKPRESC_SHIFT           16                                         /**< Shift value for CMU_RHCLKPRESC              */
#define _CMU_SYSCLKCTRL_RHCLKPRESC_MASK            0x10000UL                                  /**< Bit mask for CMU_RHCLKPRESC                 */
#define _CMU_SYSCLKCTRL_RHCLKPRESC_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for CMU_SYSCLKCTRL             */
#define _CMU_SYSCLKCTRL_RHCLKPRESC_DIV1            0x00000000UL                               /**< Mode DIV1 for CMU_SYSCLKCTRL                */
#define _CMU_SYSCLKCTRL_RHCLKPRESC_DIV2            0x00000001UL                               /**< Mode DIV2 for CMU_SYSCLKCTRL                */
#define CMU_SYSCLKCTRL_RHCLKPRESC_DEFAULT          (_CMU_SYSCLKCTRL_RHCLKPRESC_DEFAULT << 16) /**< Shifted mode DEFAULT for CMU_SYSCLKCTRL     */
#define CMU_SYSCLKCTRL_RHCLKPRESC_DIV1             (_CMU_SYSCLKCTRL_RHCLKPRESC_DIV1 << 16)    /**< Shifted mode DIV1 for CMU_SYSCLKCTRL        */
#define CMU_SYSCLKCTRL_RHCLKPRESC_DIV2             (_CMU_SYSCLKCTRL_RHCLKPRESC_DIV2 << 16)    /**< Shifted mode DIV2 for CMU_SYSCLKCTRL        */

/* Bit fields for CMU TRACECLKCTRL */
#define _CMU_TRACECLKCTRL_RESETVALUE               0x00000000UL                           /**< Default value for CMU_TRACECLKCTRL          */
#define _CMU_TRACECLKCTRL_MASK                     0x00000030UL                           /**< Mask for CMU_TRACECLKCTRL                   */
#define _CMU_TRACECLKCTRL_PRESC_SHIFT              4                                      /**< Shift value for CMU_PRESC                   */
#define _CMU_TRACECLKCTRL_PRESC_MASK               0x30UL                                 /**< Bit mask for CMU_PRESC                      */
#define _CMU_TRACECLKCTRL_PRESC_DEFAULT            0x00000000UL                           /**< Mode DEFAULT for CMU_TRACECLKCTRL           */
#define _CMU_TRACECLKCTRL_PRESC_DIV1               0x00000000UL                           /**< Mode DIV1 for CMU_TRACECLKCTRL              */
#define _CMU_TRACECLKCTRL_PRESC_DIV2               0x00000001UL                           /**< Mode DIV2 for CMU_TRACECLKCTRL              */
#define _CMU_TRACECLKCTRL_PRESC_DIV4               0x00000003UL                           /**< Mode DIV4 for CMU_TRACECLKCTRL              */
#define CMU_TRACECLKCTRL_PRESC_DEFAULT             (_CMU_TRACECLKCTRL_PRESC_DEFAULT << 4) /**< Shifted mode DEFAULT for CMU_TRACECLKCTRL   */
#define CMU_TRACECLKCTRL_PRESC_DIV1                (_CMU_TRACECLKCTRL_PRESC_DIV1 << 4)    /**< Shifted mode DIV1 for CMU_TRACECLKCTRL      */
#define CMU_TRACECLKCTRL_PRESC_DIV2                (_CMU_TRACECLKCTRL_PRESC_DIV2 << 4)    /**< Shifted mode DIV2 for CMU_TRACECLKCTRL      */
#define CMU_TRACECLKCTRL_PRESC_DIV4                (_CMU_TRACECLKCTRL_PRESC_DIV4 << 4)    /**< Shifted mode DIV4 for CMU_TRACECLKCTRL      */

/* Bit fields for CMU EXPORTCLKCTRL */
#define _CMU_EXPORTCLKCTRL_RESETVALUE              0x00000000UL                                    /**< Default value for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_MASK                    0x1F0F0F0FUL                                    /**< Mask for CMU_EXPORTCLKCTRL                  */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_SHIFT        0                                               /**< Shift value for CMU_CLKOUTSEL0              */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_MASK         0xFUL                                           /**< Bit mask for CMU_CLKOUTSEL0                 */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_DEFAULT      0x00000000UL                                    /**< Mode DEFAULT for CMU_EXPORTCLKCTRL          */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_DISABLED     0x00000000UL                                    /**< Mode DISABLED for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_HCLK         0x00000001UL                                    /**< Mode HCLK for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFEXPCLK     0x00000002UL                                    /**< Mode HFEXPCLK for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_ULFRCO       0x00000003UL                                    /**< Mode ULFRCO for CMU_EXPORTCLKCTRL           */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFRCO        0x00000004UL                                    /**< Mode LFRCO for CMU_EXPORTCLKCTRL            */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFXO         0x00000005UL                                    /**< Mode LFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCODPLL    0x00000006UL                                    /**< Mode HFRCODPLL for CMU_EXPORTCLKCTRL        */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFXO         0x00000007UL                                    /**< Mode HFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_FSRCO        0x00000008UL                                    /**< Mode FSRCO for CMU_EXPORTCLKCTRL            */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCOEM23    0x00000009UL                                    /**< Mode HFRCOEM23 for CMU_EXPORTCLKCTRL        */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_DEFAULT       (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_DEFAULT << 0)    /**< Shifted mode DEFAULT for CMU_EXPORTCLKCTRL  */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_DISABLED      (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_DISABLED << 0)   /**< Shifted mode DISABLED for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_HCLK          (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_HCLK << 0)       /**< Shifted mode HCLK for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFEXPCLK      (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFEXPCLK << 0)   /**< Shifted mode HFEXPCLK for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_ULFRCO        (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_ULFRCO << 0)     /**< Shifted mode ULFRCO for CMU_EXPORTCLKCTRL   */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFRCO << 0)      /**< Shifted mode LFRCO for CMU_EXPORTCLKCTRL    */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFXO << 0)       /**< Shifted mode LFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCODPLL     (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCODPLL << 0)  /**< Shifted mode HFRCODPLL for CMU_EXPORTCLKCTRL*/
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFXO << 0)       /**< Shifted mode HFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_FSRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_FSRCO << 0)      /**< Shifted mode FSRCO for CMU_EXPORTCLKCTRL    */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCOEM23     (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCOEM23 << 0)  /**< Shifted mode HFRCOEM23 for CMU_EXPORTCLKCTRL*/
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_SHIFT        8                                               /**< Shift value for CMU_CLKOUTSEL1              */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_MASK         0xF00UL                                         /**< Bit mask for CMU_CLKOUTSEL1                 */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_DEFAULT      0x00000000UL                                    /**< Mode DEFAULT for CMU_EXPORTCLKCTRL          */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_DISABLED     0x00000000UL                                    /**< Mode DISABLED for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_HCLK         0x00000001UL                                    /**< Mode HCLK for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFEXPCLK     0x00000002UL                                    /**< Mode HFEXPCLK for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_ULFRCO       0x00000003UL                                    /**< Mode ULFRCO for CMU_EXPORTCLKCTRL           */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFRCO        0x00000004UL                                    /**< Mode LFRCO for CMU_EXPORTCLKCTRL            */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFXO         0x00000005UL                                    /**< Mode LFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCODPLL    0x00000006UL                                    /**< Mode HFRCODPLL for CMU_EXPORTCLKCTRL        */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFXO         0x00000007UL                                    /**< Mode HFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_FSRCO        0x00000008UL                                    /**< Mode FSRCO for CMU_EXPORTCLKCTRL            */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCOEM23    0x00000009UL                                    /**< Mode HFRCOEM23 for CMU_EXPORTCLKCTRL        */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_DEFAULT       (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_DEFAULT << 8)    /**< Shifted mode DEFAULT for CMU_EXPORTCLKCTRL  */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_DISABLED      (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_DISABLED << 8)   /**< Shifted mode DISABLED for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_HCLK          (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_HCLK << 8)       /**< Shifted mode HCLK for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFEXPCLK      (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFEXPCLK << 8)   /**< Shifted mode HFEXPCLK for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_ULFRCO        (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_ULFRCO << 8)     /**< Shifted mode ULFRCO for CMU_EXPORTCLKCTRL   */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFRCO << 8)      /**< Shifted mode LFRCO for CMU_EXPORTCLKCTRL    */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFXO << 8)       /**< Shifted mode LFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCODPLL     (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCODPLL << 8)  /**< Shifted mode HFRCODPLL for CMU_EXPORTCLKCTRL*/
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFXO << 8)       /**< Shifted mode HFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_FSRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_FSRCO << 8)      /**< Shifted mode FSRCO for CMU_EXPORTCLKCTRL    */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCOEM23     (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCOEM23 << 8)  /**< Shifted mode HFRCOEM23 for CMU_EXPORTCLKCTRL*/
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_SHIFT        16                                              /**< Shift value for CMU_CLKOUTSEL2              */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_MASK         0xF0000UL                                       /**< Bit mask for CMU_CLKOUTSEL2                 */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_DEFAULT      0x00000000UL                                    /**< Mode DEFAULT for CMU_EXPORTCLKCTRL          */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_DISABLED     0x00000000UL                                    /**< Mode DISABLED for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_HCLK         0x00000001UL                                    /**< Mode HCLK for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFEXPCLK     0x00000002UL                                    /**< Mode HFEXPCLK for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_ULFRCO       0x00000003UL                                    /**< Mode ULFRCO for CMU_EXPORTCLKCTRL           */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFRCO        0x00000004UL                                    /**< Mode LFRCO for CMU_EXPORTCLKCTRL            */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFXO         0x00000005UL                                    /**< Mode LFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCODPLL    0x00000006UL                                    /**< Mode HFRCODPLL for CMU_EXPORTCLKCTRL        */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFXO         0x00000007UL                                    /**< Mode HFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_FSRCO        0x00000008UL                                    /**< Mode FSRCO for CMU_EXPORTCLKCTRL            */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCOEM23    0x00000009UL                                    /**< Mode HFRCOEM23 for CMU_EXPORTCLKCTRL        */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_DEFAULT       (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_DEFAULT << 16)   /**< Shifted mode DEFAULT for CMU_EXPORTCLKCTRL  */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_DISABLED      (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_DISABLED << 16)  /**< Shifted mode DISABLED for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_HCLK          (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_HCLK << 16)      /**< Shifted mode HCLK for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFEXPCLK      (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFEXPCLK << 16)  /**< Shifted mode HFEXPCLK for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_ULFRCO        (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_ULFRCO << 16)    /**< Shifted mode ULFRCO for CMU_EXPORTCLKCTRL   */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFRCO << 16)     /**< Shifted mode LFRCO for CMU_EXPORTCLKCTRL    */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFXO << 16)      /**< Shifted mode LFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCODPLL     (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCODPLL << 16) /**< Shifted mode HFRCODPLL for CMU_EXPORTCLKCTRL*/
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFXO << 16)      /**< Shifted mode HFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_FSRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_FSRCO << 16)     /**< Shifted mode FSRCO for CMU_EXPORTCLKCTRL    */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCOEM23     (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCOEM23 << 16) /**< Shifted mode HFRCOEM23 for CMU_EXPORTCLKCTRL*/
#define _CMU_EXPORTCLKCTRL_PRESC_SHIFT             24                                              /**< Shift value for CMU_PRESC                   */
#define _CMU_EXPORTCLKCTRL_PRESC_MASK              0x1F000000UL                                    /**< Bit mask for CMU_PRESC                      */
#define _CMU_EXPORTCLKCTRL_PRESC_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for CMU_EXPORTCLKCTRL          */
#define CMU_EXPORTCLKCTRL_PRESC_DEFAULT            (_CMU_EXPORTCLKCTRL_PRESC_DEFAULT << 24)        /**< Shifted mode DEFAULT for CMU_EXPORTCLKCTRL  */

/* Bit fields for CMU DPLLREFCLKCTRL */
#define _CMU_DPLLREFCLKCTRL_RESETVALUE             0x00000000UL                               /**< Default value for CMU_DPLLREFCLKCTRL        */
#define _CMU_DPLLREFCLKCTRL_MASK                   0x00000003UL                               /**< Mask for CMU_DPLLREFCLKCTRL                 */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_SHIFT           0                                          /**< Shift value for CMU_CLKSEL                  */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_MASK            0x3UL                                      /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for CMU_DPLLREFCLKCTRL         */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_DISABLED        0x00000000UL                               /**< Mode DISABLED for CMU_DPLLREFCLKCTRL        */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_HFXO            0x00000001UL                               /**< Mode HFXO for CMU_DPLLREFCLKCTRL            */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_LFXO            0x00000002UL                               /**< Mode LFXO for CMU_DPLLREFCLKCTRL            */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_CLKIN0          0x00000003UL                               /**< Mode CLKIN0 for CMU_DPLLREFCLKCTRL          */
#define CMU_DPLLREFCLKCTRL_CLKSEL_DEFAULT          (_CMU_DPLLREFCLKCTRL_CLKSEL_DEFAULT << 0)  /**< Shifted mode DEFAULT for CMU_DPLLREFCLKCTRL */
#define CMU_DPLLREFCLKCTRL_CLKSEL_DISABLED         (_CMU_DPLLREFCLKCTRL_CLKSEL_DISABLED << 0) /**< Shifted mode DISABLED for CMU_DPLLREFCLKCTRL*/
#define CMU_DPLLREFCLKCTRL_CLKSEL_HFXO             (_CMU_DPLLREFCLKCTRL_CLKSEL_HFXO << 0)     /**< Shifted mode HFXO for CMU_DPLLREFCLKCTRL    */
#define CMU_DPLLREFCLKCTRL_CLKSEL_LFXO             (_CMU_DPLLREFCLKCTRL_CLKSEL_LFXO << 0)     /**< Shifted mode LFXO for CMU_DPLLREFCLKCTRL    */
#define CMU_DPLLREFCLKCTRL_CLKSEL_CLKIN0           (_CMU_DPLLREFCLKCTRL_CLKSEL_CLKIN0 << 0)   /**< Shifted mode CLKIN0 for CMU_DPLLREFCLKCTRL  */

/* Bit fields for CMU EM01GRPACLKCTRL */
#define _CMU_EM01GRPACLKCTRL_RESETVALUE            0x00000001UL                                   /**< Default value for CMU_EM01GRPACLKCTRL       */
#define _CMU_EM01GRPACLKCTRL_MASK                  0x00000007UL                                   /**< Mask for CMU_EM01GRPACLKCTRL                */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_SHIFT          0                                              /**< Shift value for CMU_CLKSEL                  */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_MASK           0x7UL                                          /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_DEFAULT        0x00000001UL                                   /**< Mode DEFAULT for CMU_EM01GRPACLKCTRL        */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_HFRCODPLL      0x00000001UL                                   /**< Mode HFRCODPLL for CMU_EM01GRPACLKCTRL      */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_HFXO           0x00000002UL                                   /**< Mode HFXO for CMU_EM01GRPACLKCTRL           */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_FSRCO          0x00000003UL                                   /**< Mode FSRCO for CMU_EM01GRPACLKCTRL          */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_HFRCOEM23      0x00000004UL                                   /**< Mode HFRCOEM23 for CMU_EM01GRPACLKCTRL      */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_HFRCODPLLRT    0x00000005UL                                   /**< Mode HFRCODPLLRT for CMU_EM01GRPACLKCTRL    */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_HFXORT         0x00000006UL                                   /**< Mode HFXORT for CMU_EM01GRPACLKCTRL         */
#define CMU_EM01GRPACLKCTRL_CLKSEL_DEFAULT         (_CMU_EM01GRPACLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_EM01GRPACLKCTRL*/
#define CMU_EM01GRPACLKCTRL_CLKSEL_HFRCODPLL       (_CMU_EM01GRPACLKCTRL_CLKSEL_HFRCODPLL << 0)   /**< Shifted mode HFRCODPLL for CMU_EM01GRPACLKCTRL*/
#define CMU_EM01GRPACLKCTRL_CLKSEL_HFXO            (_CMU_EM01GRPACLKCTRL_CLKSEL_HFXO << 0)        /**< Shifted mode HFXO for CMU_EM01GRPACLKCTRL   */
#define CMU_EM01GRPACLKCTRL_CLKSEL_FSRCO           (_CMU_EM01GRPACLKCTRL_CLKSEL_FSRCO << 0)       /**< Shifted mode FSRCO for CMU_EM01GRPACLKCTRL  */
#define CMU_EM01GRPACLKCTRL_CLKSEL_HFRCOEM23       (_CMU_EM01GRPACLKCTRL_CLKSEL_HFRCOEM23 << 0)   /**< Shifted mode HFRCOEM23 for CMU_EM01GRPACLKCTRL*/
#define CMU_EM01GRPACLKCTRL_CLKSEL_HFRCODPLLRT     (_CMU_EM01GRPACLKCTRL_CLKSEL_HFRCODPLLRT << 0) /**< Shifted mode HFRCODPLLRT for CMU_EM01GRPACLKCTRL*/
#define CMU_EM01GRPACLKCTRL_CLKSEL_HFXORT          (_CMU_EM01GRPACLKCTRL_CLKSEL_HFXORT << 0)      /**< Shifted mode HFXORT for CMU_EM01GRPACLKCTRL */

/* Bit fields for CMU EM01GRPCCLKCTRL */
#define _CMU_EM01GRPCCLKCTRL_RESETVALUE            0x00000001UL                                   /**< Default value for CMU_EM01GRPCCLKCTRL       */
#define _CMU_EM01GRPCCLKCTRL_MASK                  0x00000007UL                                   /**< Mask for CMU_EM01GRPCCLKCTRL                */
#define _CMU_EM01GRPCCLKCTRL_CLKSEL_SHIFT          0                                              /**< Shift value for CMU_CLKSEL                  */
#define _CMU_EM01GRPCCLKCTRL_CLKSEL_MASK           0x7UL                                          /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_EM01GRPCCLKCTRL_CLKSEL_DEFAULT        0x00000001UL                                   /**< Mode DEFAULT for CMU_EM01GRPCCLKCTRL        */
#define _CMU_EM01GRPCCLKCTRL_CLKSEL_HFRCODPLL      0x00000001UL                                   /**< Mode HFRCODPLL for CMU_EM01GRPCCLKCTRL      */
#define _CMU_EM01GRPCCLKCTRL_CLKSEL_HFXO           0x00000002UL                                   /**< Mode HFXO for CMU_EM01GRPCCLKCTRL           */
#define _CMU_EM01GRPCCLKCTRL_CLKSEL_FSRCO          0x00000003UL                                   /**< Mode FSRCO for CMU_EM01GRPCCLKCTRL          */
#define _CMU_EM01GRPCCLKCTRL_CLKSEL_HFRCOEM23      0x00000004UL                                   /**< Mode HFRCOEM23 for CMU_EM01GRPCCLKCTRL      */
#define _CMU_EM01GRPCCLKCTRL_CLKSEL_HFRCODPLLRT    0x00000005UL                                   /**< Mode HFRCODPLLRT for CMU_EM01GRPCCLKCTRL    */
#define _CMU_EM01GRPCCLKCTRL_CLKSEL_HFXORT         0x00000006UL                                   /**< Mode HFXORT for CMU_EM01GRPCCLKCTRL         */
#define CMU_EM01GRPCCLKCTRL_CLKSEL_DEFAULT         (_CMU_EM01GRPCCLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_EM01GRPCCLKCTRL*/
#define CMU_EM01GRPCCLKCTRL_CLKSEL_HFRCODPLL       (_CMU_EM01GRPCCLKCTRL_CLKSEL_HFRCODPLL << 0)   /**< Shifted mode HFRCODPLL for CMU_EM01GRPCCLKCTRL*/
#define CMU_EM01GRPCCLKCTRL_CLKSEL_HFXO            (_CMU_EM01GRPCCLKCTRL_CLKSEL_HFXO << 0)        /**< Shifted mode HFXO for CMU_EM01GRPCCLKCTRL   */
#define CMU_EM01GRPCCLKCTRL_CLKSEL_FSRCO           (_CMU_EM01GRPCCLKCTRL_CLKSEL_FSRCO << 0)       /**< Shifted mode FSRCO for CMU_EM01GRPCCLKCTRL  */
#define CMU_EM01GRPCCLKCTRL_CLKSEL_HFRCOEM23       (_CMU_EM01GRPCCLKCTRL_CLKSEL_HFRCOEM23 << 0)   /**< Shifted mode HFRCOEM23 for CMU_EM01GRPCCLKCTRL*/
#define CMU_EM01GRPCCLKCTRL_CLKSEL_HFRCODPLLRT     (_CMU_EM01GRPCCLKCTRL_CLKSEL_HFRCODPLLRT << 0) /**< Shifted mode HFRCODPLLRT for CMU_EM01GRPCCLKCTRL*/
#define CMU_EM01GRPCCLKCTRL_CLKSEL_HFXORT          (_CMU_EM01GRPCCLKCTRL_CLKSEL_HFXORT << 0)      /**< Shifted mode HFXORT for CMU_EM01GRPCCLKCTRL */

/* Bit fields for CMU EM23GRPACLKCTRL */
#define _CMU_EM23GRPACLKCTRL_RESETVALUE            0x00000001UL                               /**< Default value for CMU_EM23GRPACLKCTRL       */
#define _CMU_EM23GRPACLKCTRL_MASK                  0x00000003UL                               /**< Mask for CMU_EM23GRPACLKCTRL                */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_SHIFT          0                                          /**< Shift value for CMU_CLKSEL                  */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_MASK           0x3UL                                      /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_DEFAULT        0x00000001UL                               /**< Mode DEFAULT for CMU_EM23GRPACLKCTRL        */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_LFRCO          0x00000001UL                               /**< Mode LFRCO for CMU_EM23GRPACLKCTRL          */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_LFXO           0x00000002UL                               /**< Mode LFXO for CMU_EM23GRPACLKCTRL           */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_ULFRCO         0x00000003UL                               /**< Mode ULFRCO for CMU_EM23GRPACLKCTRL         */
#define CMU_EM23GRPACLKCTRL_CLKSEL_DEFAULT         (_CMU_EM23GRPACLKCTRL_CLKSEL_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_EM23GRPACLKCTRL*/
#define CMU_EM23GRPACLKCTRL_CLKSEL_LFRCO           (_CMU_EM23GRPACLKCTRL_CLKSEL_LFRCO << 0)   /**< Shifted mode LFRCO for CMU_EM23GRPACLKCTRL  */
#define CMU_EM23GRPACLKCTRL_CLKSEL_LFXO            (_CMU_EM23GRPACLKCTRL_CLKSEL_LFXO << 0)    /**< Shifted mode LFXO for CMU_EM23GRPACLKCTRL   */
#define CMU_EM23GRPACLKCTRL_CLKSEL_ULFRCO          (_CMU_EM23GRPACLKCTRL_CLKSEL_ULFRCO << 0)  /**< Shifted mode ULFRCO for CMU_EM23GRPACLKCTRL */

/* Bit fields for CMU EM4GRPACLKCTRL */
#define _CMU_EM4GRPACLKCTRL_RESETVALUE             0x00000001UL                              /**< Default value for CMU_EM4GRPACLKCTRL        */
#define _CMU_EM4GRPACLKCTRL_MASK                   0x00000003UL                              /**< Mask for CMU_EM4GRPACLKCTRL                 */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_SHIFT           0                                         /**< Shift value for CMU_CLKSEL                  */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_MASK            0x3UL                                     /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_DEFAULT         0x00000001UL                              /**< Mode DEFAULT for CMU_EM4GRPACLKCTRL         */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_LFRCO           0x00000001UL                              /**< Mode LFRCO for CMU_EM4GRPACLKCTRL           */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_LFXO            0x00000002UL                              /**< Mode LFXO for CMU_EM4GRPACLKCTRL            */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_ULFRCO          0x00000003UL                              /**< Mode ULFRCO for CMU_EM4GRPACLKCTRL          */
#define CMU_EM4GRPACLKCTRL_CLKSEL_DEFAULT          (_CMU_EM4GRPACLKCTRL_CLKSEL_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_EM4GRPACLKCTRL */
#define CMU_EM4GRPACLKCTRL_CLKSEL_LFRCO            (_CMU_EM4GRPACLKCTRL_CLKSEL_LFRCO << 0)   /**< Shifted mode LFRCO for CMU_EM4GRPACLKCTRL   */
#define CMU_EM4GRPACLKCTRL_CLKSEL_LFXO             (_CMU_EM4GRPACLKCTRL_CLKSEL_LFXO << 0)    /**< Shifted mode LFXO for CMU_EM4GRPACLKCTRL    */
#define CMU_EM4GRPACLKCTRL_CLKSEL_ULFRCO           (_CMU_EM4GRPACLKCTRL_CLKSEL_ULFRCO << 0)  /**< Shifted mode ULFRCO for CMU_EM4GRPACLKCTRL  */

/* Bit fields for CMU IADCCLKCTRL */
#define _CMU_IADCCLKCTRL_RESETVALUE                0x00000001UL                               /**< Default value for CMU_IADCCLKCTRL           */
#define _CMU_IADCCLKCTRL_MASK                      0x00000003UL                               /**< Mask for CMU_IADCCLKCTRL                    */
#define _CMU_IADCCLKCTRL_CLKSEL_SHIFT              0                                          /**< Shift value for CMU_CLKSEL                  */
#define _CMU_IADCCLKCTRL_CLKSEL_MASK               0x3UL                                      /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_IADCCLKCTRL_CLKSEL_DEFAULT            0x00000001UL                               /**< Mode DEFAULT for CMU_IADCCLKCTRL            */
#define _CMU_IADCCLKCTRL_CLKSEL_EM01GRPACLK        0x00000001UL                               /**< Mode EM01GRPACLK for CMU_IADCCLKCTRL        */
#define _CMU_IADCCLKCTRL_CLKSEL_FSRCO              0x00000002UL                               /**< Mode FSRCO for CMU_IADCCLKCTRL              */
#define _CMU_IADCCLKCTRL_CLKSEL_HFRCOEM23          0x00000003UL                               /**< Mode HFRCOEM23 for CMU_IADCCLKCTRL          */
#define CMU_IADCCLKCTRL_CLKSEL_DEFAULT             (_CMU_IADCCLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_IADCCLKCTRL    */
#define CMU_IADCCLKCTRL_CLKSEL_EM01GRPACLK         (_CMU_IADCCLKCTRL_CLKSEL_EM01GRPACLK << 0) /**< Shifted mode EM01GRPACLK for CMU_IADCCLKCTRL*/
#define CMU_IADCCLKCTRL_CLKSEL_FSRCO               (_CMU_IADCCLKCTRL_CLKSEL_FSRCO << 0)       /**< Shifted mode FSRCO for CMU_IADCCLKCTRL      */
#define CMU_IADCCLKCTRL_CLKSEL_HFRCOEM23           (_CMU_IADCCLKCTRL_CLKSEL_HFRCOEM23 << 0)   /**< Shifted mode HFRCOEM23 for CMU_IADCCLKCTRL  */

/* Bit fields for CMU WDOG0CLKCTRL */
#define _CMU_WDOG0CLKCTRL_RESETVALUE               0x00000001UL                                /**< Default value for CMU_WDOG0CLKCTRL          */
#define _CMU_WDOG0CLKCTRL_MASK                     0x00000007UL                                /**< Mask for CMU_WDOG0CLKCTRL                   */
#define _CMU_WDOG0CLKCTRL_CLKSEL_SHIFT             0                                           /**< Shift value for CMU_CLKSEL                  */
#define _CMU_WDOG0CLKCTRL_CLKSEL_MASK              0x7UL                                       /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_WDOG0CLKCTRL_CLKSEL_DEFAULT           0x00000001UL                                /**< Mode DEFAULT for CMU_WDOG0CLKCTRL           */
#define _CMU_WDOG0CLKCTRL_CLKSEL_LFRCO             0x00000001UL                                /**< Mode LFRCO for CMU_WDOG0CLKCTRL             */
#define _CMU_WDOG0CLKCTRL_CLKSEL_LFXO              0x00000002UL                                /**< Mode LFXO for CMU_WDOG0CLKCTRL              */
#define _CMU_WDOG0CLKCTRL_CLKSEL_ULFRCO            0x00000003UL                                /**< Mode ULFRCO for CMU_WDOG0CLKCTRL            */
#define _CMU_WDOG0CLKCTRL_CLKSEL_HCLKDIV1024       0x00000004UL                                /**< Mode HCLKDIV1024 for CMU_WDOG0CLKCTRL       */
#define CMU_WDOG0CLKCTRL_CLKSEL_DEFAULT            (_CMU_WDOG0CLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_WDOG0CLKCTRL   */
#define CMU_WDOG0CLKCTRL_CLKSEL_LFRCO              (_CMU_WDOG0CLKCTRL_CLKSEL_LFRCO << 0)       /**< Shifted mode LFRCO for CMU_WDOG0CLKCTRL     */
#define CMU_WDOG0CLKCTRL_CLKSEL_LFXO               (_CMU_WDOG0CLKCTRL_CLKSEL_LFXO << 0)        /**< Shifted mode LFXO for CMU_WDOG0CLKCTRL      */
#define CMU_WDOG0CLKCTRL_CLKSEL_ULFRCO             (_CMU_WDOG0CLKCTRL_CLKSEL_ULFRCO << 0)      /**< Shifted mode ULFRCO for CMU_WDOG0CLKCTRL    */
#define CMU_WDOG0CLKCTRL_CLKSEL_HCLKDIV1024        (_CMU_WDOG0CLKCTRL_CLKSEL_HCLKDIV1024 << 0) /**< Shifted mode HCLKDIV1024 for CMU_WDOG0CLKCTRL*/

/* Bit fields for CMU WDOG1CLKCTRL */
#define _CMU_WDOG1CLKCTRL_RESETVALUE               0x00000001UL                                /**< Default value for CMU_WDOG1CLKCTRL          */
#define _CMU_WDOG1CLKCTRL_MASK                     0x00000007UL                                /**< Mask for CMU_WDOG1CLKCTRL                   */
#define _CMU_WDOG1CLKCTRL_CLKSEL_SHIFT             0                                           /**< Shift value for CMU_CLKSEL                  */
#define _CMU_WDOG1CLKCTRL_CLKSEL_MASK              0x7UL                                       /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_WDOG1CLKCTRL_CLKSEL_DEFAULT           0x00000001UL                                /**< Mode DEFAULT for CMU_WDOG1CLKCTRL           */
#define _CMU_WDOG1CLKCTRL_CLKSEL_LFRCO             0x00000001UL                                /**< Mode LFRCO for CMU_WDOG1CLKCTRL             */
#define _CMU_WDOG1CLKCTRL_CLKSEL_LFXO              0x00000002UL                                /**< Mode LFXO for CMU_WDOG1CLKCTRL              */
#define _CMU_WDOG1CLKCTRL_CLKSEL_ULFRCO            0x00000003UL                                /**< Mode ULFRCO for CMU_WDOG1CLKCTRL            */
#define _CMU_WDOG1CLKCTRL_CLKSEL_HCLKDIV1024       0x00000004UL                                /**< Mode HCLKDIV1024 for CMU_WDOG1CLKCTRL       */
#define CMU_WDOG1CLKCTRL_CLKSEL_DEFAULT            (_CMU_WDOG1CLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_WDOG1CLKCTRL   */
#define CMU_WDOG1CLKCTRL_CLKSEL_LFRCO              (_CMU_WDOG1CLKCTRL_CLKSEL_LFRCO << 0)       /**< Shifted mode LFRCO for CMU_WDOG1CLKCTRL     */
#define CMU_WDOG1CLKCTRL_CLKSEL_LFXO               (_CMU_WDOG1CLKCTRL_CLKSEL_LFXO << 0)        /**< Shifted mode LFXO for CMU_WDOG1CLKCTRL      */
#define CMU_WDOG1CLKCTRL_CLKSEL_ULFRCO             (_CMU_WDOG1CLKCTRL_CLKSEL_ULFRCO << 0)      /**< Shifted mode ULFRCO for CMU_WDOG1CLKCTRL    */
#define CMU_WDOG1CLKCTRL_CLKSEL_HCLKDIV1024        (_CMU_WDOG1CLKCTRL_CLKSEL_HCLKDIV1024 << 0) /**< Shifted mode HCLKDIV1024 for CMU_WDOG1CLKCTRL*/

/* Bit fields for CMU EUSART0CLKCTRL */
#define _CMU_EUSART0CLKCTRL_RESETVALUE             0x00000001UL                                  /**< Default value for CMU_EUSART0CLKCTRL        */
#define _CMU_EUSART0CLKCTRL_MASK                   0x00000007UL                                  /**< Mask for CMU_EUSART0CLKCTRL                 */
#define _CMU_EUSART0CLKCTRL_CLKSEL_SHIFT           0                                             /**< Shift value for CMU_CLKSEL                  */
#define _CMU_EUSART0CLKCTRL_CLKSEL_MASK            0x7UL                                         /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_EUSART0CLKCTRL_CLKSEL_DEFAULT         0x00000001UL                                  /**< Mode DEFAULT for CMU_EUSART0CLKCTRL         */
#define _CMU_EUSART0CLKCTRL_CLKSEL_DISABLED        0x00000000UL                                  /**< Mode DISABLED for CMU_EUSART0CLKCTRL        */
#define _CMU_EUSART0CLKCTRL_CLKSEL_EM01GRPCCLK     0x00000001UL                                  /**< Mode EM01GRPCCLK for CMU_EUSART0CLKCTRL     */
#define _CMU_EUSART0CLKCTRL_CLKSEL_HFRCOEM23       0x00000002UL                                  /**< Mode HFRCOEM23 for CMU_EUSART0CLKCTRL       */
#define _CMU_EUSART0CLKCTRL_CLKSEL_LFRCO           0x00000003UL                                  /**< Mode LFRCO for CMU_EUSART0CLKCTRL           */
#define _CMU_EUSART0CLKCTRL_CLKSEL_LFXO            0x00000004UL                                  /**< Mode LFXO for CMU_EUSART0CLKCTRL            */
#define CMU_EUSART0CLKCTRL_CLKSEL_DEFAULT          (_CMU_EUSART0CLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_EUSART0CLKCTRL */
#define CMU_EUSART0CLKCTRL_CLKSEL_DISABLED         (_CMU_EUSART0CLKCTRL_CLKSEL_DISABLED << 0)    /**< Shifted mode DISABLED for CMU_EUSART0CLKCTRL*/
#define CMU_EUSART0CLKCTRL_CLKSEL_EM01GRPCCLK      (_CMU_EUSART0CLKCTRL_CLKSEL_EM01GRPCCLK << 0) /**< Shifted mode EM01GRPCCLK for CMU_EUSART0CLKCTRL*/
#define CMU_EUSART0CLKCTRL_CLKSEL_HFRCOEM23        (_CMU_EUSART0CLKCTRL_CLKSEL_HFRCOEM23 << 0)   /**< Shifted mode HFRCOEM23 for CMU_EUSART0CLKCTRL*/
#define CMU_EUSART0CLKCTRL_CLKSEL_LFRCO            (_CMU_EUSART0CLKCTRL_CLKSEL_LFRCO << 0)       /**< Shifted mode LFRCO for CMU_EUSART0CLKCTRL   */
#define CMU_EUSART0CLKCTRL_CLKSEL_LFXO             (_CMU_EUSART0CLKCTRL_CLKSEL_LFXO << 0)        /**< Shifted mode LFXO for CMU_EUSART0CLKCTRL    */

/* Bit fields for CMU SYSRTC0CLKCTRL */
#define _CMU_SYSRTC0CLKCTRL_RESETVALUE             0x00000001UL                              /**< Default value for CMU_SYSRTC0CLKCTRL        */
#define _CMU_SYSRTC0CLKCTRL_MASK                   0x00000003UL                              /**< Mask for CMU_SYSRTC0CLKCTRL                 */
#define _CMU_SYSRTC0CLKCTRL_CLKSEL_SHIFT           0                                         /**< Shift value for CMU_CLKSEL                  */
#define _CMU_SYSRTC0CLKCTRL_CLKSEL_MASK            0x3UL                                     /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_SYSRTC0CLKCTRL_CLKSEL_DEFAULT         0x00000001UL                              /**< Mode DEFAULT for CMU_SYSRTC0CLKCTRL         */
#define _CMU_SYSRTC0CLKCTRL_CLKSEL_LFRCO           0x00000001UL                              /**< Mode LFRCO for CMU_SYSRTC0CLKCTRL           */
#define _CMU_SYSRTC0CLKCTRL_CLKSEL_LFXO            0x00000002UL                              /**< Mode LFXO for CMU_SYSRTC0CLKCTRL            */
#define _CMU_SYSRTC0CLKCTRL_CLKSEL_ULFRCO          0x00000003UL                              /**< Mode ULFRCO for CMU_SYSRTC0CLKCTRL          */
#define CMU_SYSRTC0CLKCTRL_CLKSEL_DEFAULT          (_CMU_SYSRTC0CLKCTRL_CLKSEL_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_SYSRTC0CLKCTRL */
#define CMU_SYSRTC0CLKCTRL_CLKSEL_LFRCO            (_CMU_SYSRTC0CLKCTRL_CLKSEL_LFRCO << 0)   /**< Shifted mode LFRCO for CMU_SYSRTC0CLKCTRL   */
#define CMU_SYSRTC0CLKCTRL_CLKSEL_LFXO             (_CMU_SYSRTC0CLKCTRL_CLKSEL_LFXO << 0)    /**< Shifted mode LFXO for CMU_SYSRTC0CLKCTRL    */
#define CMU_SYSRTC0CLKCTRL_CLKSEL_ULFRCO           (_CMU_SYSRTC0CLKCTRL_CLKSEL_ULFRCO << 0)  /**< Shifted mode ULFRCO for CMU_SYSRTC0CLKCTRL  */

/* Bit fields for CMU LCDCLKCTRL */
#define _CMU_LCDCLKCTRL_RESETVALUE                 0x00000001UL                          /**< Default value for CMU_LCDCLKCTRL            */
#define _CMU_LCDCLKCTRL_MASK                       0x00000003UL                          /**< Mask for CMU_LCDCLKCTRL                     */
#define _CMU_LCDCLKCTRL_CLKSEL_SHIFT               0                                     /**< Shift value for CMU_CLKSEL                  */
#define _CMU_LCDCLKCTRL_CLKSEL_MASK                0x3UL                                 /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_LCDCLKCTRL_CLKSEL_DEFAULT             0x00000001UL                          /**< Mode DEFAULT for CMU_LCDCLKCTRL             */
#define _CMU_LCDCLKCTRL_CLKSEL_LFRCO               0x00000001UL                          /**< Mode LFRCO for CMU_LCDCLKCTRL               */
#define _CMU_LCDCLKCTRL_CLKSEL_LFXO                0x00000002UL                          /**< Mode LFXO for CMU_LCDCLKCTRL                */
#define _CMU_LCDCLKCTRL_CLKSEL_ULFRCO              0x00000003UL                          /**< Mode ULFRCO for CMU_LCDCLKCTRL              */
#define CMU_LCDCLKCTRL_CLKSEL_DEFAULT              (_CMU_LCDCLKCTRL_CLKSEL_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_LCDCLKCTRL     */
#define CMU_LCDCLKCTRL_CLKSEL_LFRCO                (_CMU_LCDCLKCTRL_CLKSEL_LFRCO << 0)   /**< Shifted mode LFRCO for CMU_LCDCLKCTRL       */
#define CMU_LCDCLKCTRL_CLKSEL_LFXO                 (_CMU_LCDCLKCTRL_CLKSEL_LFXO << 0)    /**< Shifted mode LFXO for CMU_LCDCLKCTRL        */
#define CMU_LCDCLKCTRL_CLKSEL_ULFRCO               (_CMU_LCDCLKCTRL_CLKSEL_ULFRCO << 0)  /**< Shifted mode ULFRCO for CMU_LCDCLKCTRL      */

/* Bit fields for CMU VDAC0CLKCTRL */
#define _CMU_VDAC0CLKCTRL_RESETVALUE               0x00000001UL                                /**< Default value for CMU_VDAC0CLKCTRL          */
#define _CMU_VDAC0CLKCTRL_MASK                     0x00000007UL                                /**< Mask for CMU_VDAC0CLKCTRL                   */
#define _CMU_VDAC0CLKCTRL_CLKSEL_SHIFT             0                                           /**< Shift value for CMU_CLKSEL                  */
#define _CMU_VDAC0CLKCTRL_CLKSEL_MASK              0x7UL                                       /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_VDAC0CLKCTRL_CLKSEL_DEFAULT           0x00000001UL                                /**< Mode DEFAULT for CMU_VDAC0CLKCTRL           */
#define _CMU_VDAC0CLKCTRL_CLKSEL_DISABLED          0x00000000UL                                /**< Mode DISABLED for CMU_VDAC0CLKCTRL          */
#define _CMU_VDAC0CLKCTRL_CLKSEL_EM01GRPACLK       0x00000001UL                                /**< Mode EM01GRPACLK for CMU_VDAC0CLKCTRL       */
#define _CMU_VDAC0CLKCTRL_CLKSEL_EM23GRPACLK       0x00000002UL                                /**< Mode EM23GRPACLK for CMU_VDAC0CLKCTRL       */
#define _CMU_VDAC0CLKCTRL_CLKSEL_FSRCO             0x00000003UL                                /**< Mode FSRCO for CMU_VDAC0CLKCTRL             */
#define _CMU_VDAC0CLKCTRL_CLKSEL_HFRCOEM23         0x00000004UL                                /**< Mode HFRCOEM23 for CMU_VDAC0CLKCTRL         */
#define CMU_VDAC0CLKCTRL_CLKSEL_DEFAULT            (_CMU_VDAC0CLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_VDAC0CLKCTRL   */
#define CMU_VDAC0CLKCTRL_CLKSEL_DISABLED           (_CMU_VDAC0CLKCTRL_CLKSEL_DISABLED << 0)    /**< Shifted mode DISABLED for CMU_VDAC0CLKCTRL  */
#define CMU_VDAC0CLKCTRL_CLKSEL_EM01GRPACLK        (_CMU_VDAC0CLKCTRL_CLKSEL_EM01GRPACLK << 0) /**< Shifted mode EM01GRPACLK for CMU_VDAC0CLKCTRL*/
#define CMU_VDAC0CLKCTRL_CLKSEL_EM23GRPACLK        (_CMU_VDAC0CLKCTRL_CLKSEL_EM23GRPACLK << 0) /**< Shifted mode EM23GRPACLK for CMU_VDAC0CLKCTRL*/
#define CMU_VDAC0CLKCTRL_CLKSEL_FSRCO              (_CMU_VDAC0CLKCTRL_CLKSEL_FSRCO << 0)       /**< Shifted mode FSRCO for CMU_VDAC0CLKCTRL     */
#define CMU_VDAC0CLKCTRL_CLKSEL_HFRCOEM23          (_CMU_VDAC0CLKCTRL_CLKSEL_HFRCOEM23 << 0)   /**< Shifted mode HFRCOEM23 for CMU_VDAC0CLKCTRL */

/* Bit fields for CMU PCNT0CLKCTRL */
#define _CMU_PCNT0CLKCTRL_RESETVALUE               0x00000001UL                                /**< Default value for CMU_PCNT0CLKCTRL          */
#define _CMU_PCNT0CLKCTRL_MASK                     0x00000003UL                                /**< Mask for CMU_PCNT0CLKCTRL                   */
#define _CMU_PCNT0CLKCTRL_CLKSEL_SHIFT             0                                           /**< Shift value for CMU_CLKSEL                  */
#define _CMU_PCNT0CLKCTRL_CLKSEL_MASK              0x3UL                                       /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_PCNT0CLKCTRL_CLKSEL_DEFAULT           0x00000001UL                                /**< Mode DEFAULT for CMU_PCNT0CLKCTRL           */
#define _CMU_PCNT0CLKCTRL_CLKSEL_DISABLED          0x00000000UL                                /**< Mode DISABLED for CMU_PCNT0CLKCTRL          */
#define _CMU_PCNT0CLKCTRL_CLKSEL_EM23GRPACLK       0x00000001UL                                /**< Mode EM23GRPACLK for CMU_PCNT0CLKCTRL       */
#define _CMU_PCNT0CLKCTRL_CLKSEL_PCNTS0            0x00000002UL                                /**< Mode PCNTS0 for CMU_PCNT0CLKCTRL            */
#define CMU_PCNT0CLKCTRL_CLKSEL_DEFAULT            (_CMU_PCNT0CLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_PCNT0CLKCTRL   */
#define CMU_PCNT0CLKCTRL_CLKSEL_DISABLED           (_CMU_PCNT0CLKCTRL_CLKSEL_DISABLED << 0)    /**< Shifted mode DISABLED for CMU_PCNT0CLKCTRL  */
#define CMU_PCNT0CLKCTRL_CLKSEL_EM23GRPACLK        (_CMU_PCNT0CLKCTRL_CLKSEL_EM23GRPACLK << 0) /**< Shifted mode EM23GRPACLK for CMU_PCNT0CLKCTRL*/
#define CMU_PCNT0CLKCTRL_CLKSEL_PCNTS0             (_CMU_PCNT0CLKCTRL_CLKSEL_PCNTS0 << 0)      /**< Shifted mode PCNTS0 for CMU_PCNT0CLKCTRL    */

/* Bit fields for CMU RADIOCLKCTRL */
#define _CMU_RADIOCLKCTRL_RESETVALUE               0x00000000UL                             /**< Default value for CMU_RADIOCLKCTRL          */
#define _CMU_RADIOCLKCTRL_MASK                     0x80000003UL                             /**< Mask for CMU_RADIOCLKCTRL                   */
#define CMU_RADIOCLKCTRL_EN                        (0x1UL << 0)                             /**< Enable                                      */
#define _CMU_RADIOCLKCTRL_EN_SHIFT                 0                                        /**< Shift value for CMU_EN                      */
#define _CMU_RADIOCLKCTRL_EN_MASK                  0x1UL                                    /**< Bit mask for CMU_EN                         */
#define _CMU_RADIOCLKCTRL_EN_DEFAULT               0x00000000UL                             /**< Mode DEFAULT for CMU_RADIOCLKCTRL           */
#define CMU_RADIOCLKCTRL_EN_DEFAULT                (_CMU_RADIOCLKCTRL_EN_DEFAULT << 0)      /**< Shifted mode DEFAULT for CMU_RADIOCLKCTRL   */
#define CMU_RADIOCLKCTRL_DBGCLK                    (0x1UL << 31)                            /**< Enable Clock for Debugger                   */
#define _CMU_RADIOCLKCTRL_DBGCLK_SHIFT             31                                       /**< Shift value for CMU_DBGCLK                  */
#define _CMU_RADIOCLKCTRL_DBGCLK_MASK              0x80000000UL                             /**< Bit mask for CMU_DBGCLK                     */
#define _CMU_RADIOCLKCTRL_DBGCLK_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for CMU_RADIOCLKCTRL           */
#define CMU_RADIOCLKCTRL_DBGCLK_DEFAULT            (_CMU_RADIOCLKCTRL_DBGCLK_DEFAULT << 31) /**< Shifted mode DEFAULT for CMU_RADIOCLKCTRL   */

/* Bit fields for CMU LESENSEHFCLKCTRL */
#define _CMU_LESENSEHFCLKCTRL_RESETVALUE           0x00000001UL                                  /**< Default value for CMU_LESENSEHFCLKCTRL      */
#define _CMU_LESENSEHFCLKCTRL_MASK                 0x00000003UL                                  /**< Mask for CMU_LESENSEHFCLKCTRL               */
#define _CMU_LESENSEHFCLKCTRL_CLKSEL_SHIFT         0                                             /**< Shift value for CMU_CLKSEL                  */
#define _CMU_LESENSEHFCLKCTRL_CLKSEL_MASK          0x3UL                                         /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_LESENSEHFCLKCTRL_CLKSEL_DEFAULT       0x00000001UL                                  /**< Mode DEFAULT for CMU_LESENSEHFCLKCTRL       */
#define _CMU_LESENSEHFCLKCTRL_CLKSEL_FSRCO         0x00000001UL                                  /**< Mode FSRCO for CMU_LESENSEHFCLKCTRL         */
#define _CMU_LESENSEHFCLKCTRL_CLKSEL_HFRCOEM23     0x00000002UL                                  /**< Mode HFRCOEM23 for CMU_LESENSEHFCLKCTRL     */
#define CMU_LESENSEHFCLKCTRL_CLKSEL_DEFAULT        (_CMU_LESENSEHFCLKCTRL_CLKSEL_DEFAULT << 0)   /**< Shifted mode DEFAULT for CMU_LESENSEHFCLKCTRL*/
#define CMU_LESENSEHFCLKCTRL_CLKSEL_FSRCO          (_CMU_LESENSEHFCLKCTRL_CLKSEL_FSRCO << 0)     /**< Shifted mode FSRCO for CMU_LESENSEHFCLKCTRL */
#define CMU_LESENSEHFCLKCTRL_CLKSEL_HFRCOEM23      (_CMU_LESENSEHFCLKCTRL_CLKSEL_HFRCOEM23 << 0) /**< Shifted mode HFRCOEM23 for CMU_LESENSEHFCLKCTRL*/

/** @} End of group EFR32ZG23_CMU_BitFields */
/** @} End of group EFR32ZG23_CMU */
/** @} End of group Parts */

#endif /* EFR32ZG23_CMU_H */
#ifdef __cplusplus
}
#endif

