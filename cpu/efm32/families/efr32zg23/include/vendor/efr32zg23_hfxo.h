/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 HFXO register and bit field definitions
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

#ifndef EFR32ZG23_HFXO_H
#define EFR32ZG23_HFXO_H
#define HFXO_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_HFXO HFXO
 * @{
 * @brief EFR32ZG23 HFXO Register Declaration.
 *****************************************************************************/

/** HFXO Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  uint32_t       RESERVED0[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t XTALCFG;                       /**< Crystal Configuration Register                     */
  uint32_t       RESERVED1[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t XTALCTRL;                      /**< Crystal Control Register                           */
  __IOM uint32_t XTALCTRL1;                     /**< BUFOUT Crystal Control Register                    */
  __IOM uint32_t CFG;                           /**< Configuration Register                             */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CTRL;                          /**< Control Register                                   */
  uint32_t       RESERVED3[5U];                 /**< Reserved for future use                            */
  __IOM uint32_t BUFOUTTRIM;                    /**< BUFOUT Trim Configuration Register                 */
  __IOM uint32_t BUFOUTCTRL;                    /**< BUFOUT Control Register                            */
  uint32_t       RESERVED4[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  uint32_t       RESERVED5[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  uint32_t       RESERVED6[5U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  uint32_t       RESERVED7[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< Configuration Lock Register                        */
  uint32_t       RESERVED8[991U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  uint32_t       RESERVED9[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t XTALCFG_SET;                   /**< Crystal Configuration Register                     */
  uint32_t       RESERVED10[1U];                /**< Reserved for future use                            */
  __IOM uint32_t XTALCTRL_SET;                  /**< Crystal Control Register                           */
  __IOM uint32_t XTALCTRL1_SET;                 /**< BUFOUT Crystal Control Register                    */
  __IOM uint32_t CFG_SET;                       /**< Configuration Register                             */
  uint32_t       RESERVED11[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL_SET;                      /**< Control Register                                   */
  uint32_t       RESERVED12[5U];                /**< Reserved for future use                            */
  __IOM uint32_t BUFOUTTRIM_SET;                /**< BUFOUT Trim Configuration Register                 */
  __IOM uint32_t BUFOUTCTRL_SET;                /**< BUFOUT Control Register                            */
  uint32_t       RESERVED13[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  uint32_t       RESERVED15[5U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED16[2U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED17[991U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  uint32_t       RESERVED18[3U];                /**< Reserved for future use                            */
  __IOM uint32_t XTALCFG_CLR;                   /**< Crystal Configuration Register                     */
  uint32_t       RESERVED19[1U];                /**< Reserved for future use                            */
  __IOM uint32_t XTALCTRL_CLR;                  /**< Crystal Control Register                           */
  __IOM uint32_t XTALCTRL1_CLR;                 /**< BUFOUT Crystal Control Register                    */
  __IOM uint32_t CFG_CLR;                       /**< Configuration Register                             */
  uint32_t       RESERVED20[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL_CLR;                      /**< Control Register                                   */
  uint32_t       RESERVED21[5U];                /**< Reserved for future use                            */
  __IOM uint32_t BUFOUTTRIM_CLR;                /**< BUFOUT Trim Configuration Register                 */
  __IOM uint32_t BUFOUTCTRL_CLR;                /**< BUFOUT Control Register                            */
  uint32_t       RESERVED22[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  uint32_t       RESERVED23[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  uint32_t       RESERVED24[5U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED25[2U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED26[991U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  uint32_t       RESERVED27[3U];                /**< Reserved for future use                            */
  __IOM uint32_t XTALCFG_TGL;                   /**< Crystal Configuration Register                     */
  uint32_t       RESERVED28[1U];                /**< Reserved for future use                            */
  __IOM uint32_t XTALCTRL_TGL;                  /**< Crystal Control Register                           */
  __IOM uint32_t XTALCTRL1_TGL;                 /**< BUFOUT Crystal Control Register                    */
  __IOM uint32_t CFG_TGL;                       /**< Configuration Register                             */
  uint32_t       RESERVED29[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL_TGL;                      /**< Control Register                                   */
  uint32_t       RESERVED30[5U];                /**< Reserved for future use                            */
  __IOM uint32_t BUFOUTTRIM_TGL;                /**< BUFOUT Trim Configuration Register                 */
  __IOM uint32_t BUFOUTCTRL_TGL;                /**< BUFOUT Control Register                            */
  uint32_t       RESERVED31[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  uint32_t       RESERVED32[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  uint32_t       RESERVED33[5U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED34[2U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< Configuration Lock Register                        */
} HFXO_TypeDef;
/** @} End of group EFR32ZG23_HFXO */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_HFXO
 * @{
 * @defgroup EFR32ZG23_HFXO_BitFields HFXO Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for HFXO IPVERSION */
#define _HFXO_IPVERSION_RESETVALUE                      0x00000003UL                             /**< Default value for HFXO_IPVERSION            */
#define _HFXO_IPVERSION_MASK                            0xFFFFFFFFUL                             /**< Mask for HFXO_IPVERSION                     */
#define _HFXO_IPVERSION_IPVERSION_SHIFT                 0                                        /**< Shift value for HFXO_IPVERSION              */
#define _HFXO_IPVERSION_IPVERSION_MASK                  0xFFFFFFFFUL                             /**< Bit mask for HFXO_IPVERSION                 */
#define _HFXO_IPVERSION_IPVERSION_DEFAULT               0x00000003UL                             /**< Mode DEFAULT for HFXO_IPVERSION             */
#define HFXO_IPVERSION_IPVERSION_DEFAULT                (_HFXO_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for HFXO_IPVERSION     */

/* Bit fields for HFXO XTALCFG */
#define _HFXO_XTALCFG_RESETVALUE                        0x0BB00820UL                                  /**< Default value for HFXO_XTALCFG              */
#define _HFXO_XTALCFG_MASK                              0x0FFFFFFFUL                                  /**< Mask for HFXO_XTALCFG                       */
#define _HFXO_XTALCFG_COREBIASSTARTUPI_SHIFT            0                                             /**< Shift value for HFXO_COREBIASSTARTUPI       */
#define _HFXO_XTALCFG_COREBIASSTARTUPI_MASK             0x3FUL                                        /**< Bit mask for HFXO_COREBIASSTARTUPI          */
#define _HFXO_XTALCFG_COREBIASSTARTUPI_DEFAULT          0x00000020UL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define HFXO_XTALCFG_COREBIASSTARTUPI_DEFAULT           (_HFXO_XTALCFG_COREBIASSTARTUPI_DEFAULT << 0) /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define _HFXO_XTALCFG_COREBIASSTARTUP_SHIFT             6                                             /**< Shift value for HFXO_COREBIASSTARTUP        */
#define _HFXO_XTALCFG_COREBIASSTARTUP_MASK              0xFC0UL                                       /**< Bit mask for HFXO_COREBIASSTARTUP           */
#define _HFXO_XTALCFG_COREBIASSTARTUP_DEFAULT           0x00000020UL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define HFXO_XTALCFG_COREBIASSTARTUP_DEFAULT            (_HFXO_XTALCFG_COREBIASSTARTUP_DEFAULT << 6)  /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define _HFXO_XTALCFG_CTUNEXISTARTUP_SHIFT              12                                            /**< Shift value for HFXO_CTUNEXISTARTUP         */
#define _HFXO_XTALCFG_CTUNEXISTARTUP_MASK               0xF000UL                                      /**< Bit mask for HFXO_CTUNEXISTARTUP            */
#define _HFXO_XTALCFG_CTUNEXISTARTUP_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define HFXO_XTALCFG_CTUNEXISTARTUP_DEFAULT             (_HFXO_XTALCFG_CTUNEXISTARTUP_DEFAULT << 12)  /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define _HFXO_XTALCFG_CTUNEXOSTARTUP_SHIFT              16                                            /**< Shift value for HFXO_CTUNEXOSTARTUP         */
#define _HFXO_XTALCFG_CTUNEXOSTARTUP_MASK               0xF0000UL                                     /**< Bit mask for HFXO_CTUNEXOSTARTUP            */
#define _HFXO_XTALCFG_CTUNEXOSTARTUP_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define HFXO_XTALCFG_CTUNEXOSTARTUP_DEFAULT             (_HFXO_XTALCFG_CTUNEXOSTARTUP_DEFAULT << 16)  /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_SHIFT               20                                            /**< Shift value for HFXO_TIMEOUTSTEADY          */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_MASK                0xF00000UL                                    /**< Bit mask for HFXO_TIMEOUTSTEADY             */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_DEFAULT             0x0000000BUL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T4US                0x00000000UL                                  /**< Mode T4US for HFXO_XTALCFG                  */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T16US               0x00000001UL                                  /**< Mode T16US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T41US               0x00000002UL                                  /**< Mode T41US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T83US               0x00000003UL                                  /**< Mode T83US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T125US              0x00000004UL                                  /**< Mode T125US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T166US              0x00000005UL                                  /**< Mode T166US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T208US              0x00000006UL                                  /**< Mode T208US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T250US              0x00000007UL                                  /**< Mode T250US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T333US              0x00000008UL                                  /**< Mode T333US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T416US              0x00000009UL                                  /**< Mode T416US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T500US              0x0000000AUL                                  /**< Mode T500US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T666US              0x0000000BUL                                  /**< Mode T666US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T833US              0x0000000CUL                                  /**< Mode T833US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T1666US             0x0000000DUL                                  /**< Mode T1666US for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T2500US             0x0000000EUL                                  /**< Mode T2500US for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T4166US             0x0000000FUL                                  /**< Mode T4166US for HFXO_XTALCFG               */
#define HFXO_XTALCFG_TIMEOUTSTEADY_DEFAULT              (_HFXO_XTALCFG_TIMEOUTSTEADY_DEFAULT << 20)   /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T4US                 (_HFXO_XTALCFG_TIMEOUTSTEADY_T4US << 20)      /**< Shifted mode T4US for HFXO_XTALCFG          */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T16US                (_HFXO_XTALCFG_TIMEOUTSTEADY_T16US << 20)     /**< Shifted mode T16US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T41US                (_HFXO_XTALCFG_TIMEOUTSTEADY_T41US << 20)     /**< Shifted mode T41US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T83US                (_HFXO_XTALCFG_TIMEOUTSTEADY_T83US << 20)     /**< Shifted mode T83US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T125US               (_HFXO_XTALCFG_TIMEOUTSTEADY_T125US << 20)    /**< Shifted mode T125US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T166US               (_HFXO_XTALCFG_TIMEOUTSTEADY_T166US << 20)    /**< Shifted mode T166US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T208US               (_HFXO_XTALCFG_TIMEOUTSTEADY_T208US << 20)    /**< Shifted mode T208US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T250US               (_HFXO_XTALCFG_TIMEOUTSTEADY_T250US << 20)    /**< Shifted mode T250US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T333US               (_HFXO_XTALCFG_TIMEOUTSTEADY_T333US << 20)    /**< Shifted mode T333US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T416US               (_HFXO_XTALCFG_TIMEOUTSTEADY_T416US << 20)    /**< Shifted mode T416US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T500US               (_HFXO_XTALCFG_TIMEOUTSTEADY_T500US << 20)    /**< Shifted mode T500US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T666US               (_HFXO_XTALCFG_TIMEOUTSTEADY_T666US << 20)    /**< Shifted mode T666US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T833US               (_HFXO_XTALCFG_TIMEOUTSTEADY_T833US << 20)    /**< Shifted mode T833US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T1666US              (_HFXO_XTALCFG_TIMEOUTSTEADY_T1666US << 20)   /**< Shifted mode T1666US for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T2500US              (_HFXO_XTALCFG_TIMEOUTSTEADY_T2500US << 20)   /**< Shifted mode T2500US for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T4166US              (_HFXO_XTALCFG_TIMEOUTSTEADY_T4166US << 20)   /**< Shifted mode T4166US for HFXO_XTALCFG       */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_SHIFT                24                                            /**< Shift value for HFXO_TIMEOUTCBLSB           */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_MASK                 0xF000000UL                                   /**< Bit mask for HFXO_TIMEOUTCBLSB              */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_DEFAULT              0x0000000BUL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T8US                 0x00000000UL                                  /**< Mode T8US for HFXO_XTALCFG                  */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T20US                0x00000001UL                                  /**< Mode T20US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T41US                0x00000002UL                                  /**< Mode T41US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T62US                0x00000003UL                                  /**< Mode T62US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T83US                0x00000004UL                                  /**< Mode T83US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T104US               0x00000005UL                                  /**< Mode T104US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T125US               0x00000006UL                                  /**< Mode T125US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T166US               0x00000007UL                                  /**< Mode T166US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T208US               0x00000008UL                                  /**< Mode T208US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T250US               0x00000009UL                                  /**< Mode T250US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T333US               0x0000000AUL                                  /**< Mode T333US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T416US               0x0000000BUL                                  /**< Mode T416US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T833US               0x0000000CUL                                  /**< Mode T833US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T1250US              0x0000000DUL                                  /**< Mode T1250US for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T2083US              0x0000000EUL                                  /**< Mode T2083US for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T3750US              0x0000000FUL                                  /**< Mode T3750US for HFXO_XTALCFG               */
#define HFXO_XTALCFG_TIMEOUTCBLSB_DEFAULT               (_HFXO_XTALCFG_TIMEOUTCBLSB_DEFAULT << 24)    /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T8US                  (_HFXO_XTALCFG_TIMEOUTCBLSB_T8US << 24)       /**< Shifted mode T8US for HFXO_XTALCFG          */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T20US                 (_HFXO_XTALCFG_TIMEOUTCBLSB_T20US << 24)      /**< Shifted mode T20US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T41US                 (_HFXO_XTALCFG_TIMEOUTCBLSB_T41US << 24)      /**< Shifted mode T41US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T62US                 (_HFXO_XTALCFG_TIMEOUTCBLSB_T62US << 24)      /**< Shifted mode T62US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T83US                 (_HFXO_XTALCFG_TIMEOUTCBLSB_T83US << 24)      /**< Shifted mode T83US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T104US                (_HFXO_XTALCFG_TIMEOUTCBLSB_T104US << 24)     /**< Shifted mode T104US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T125US                (_HFXO_XTALCFG_TIMEOUTCBLSB_T125US << 24)     /**< Shifted mode T125US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T166US                (_HFXO_XTALCFG_TIMEOUTCBLSB_T166US << 24)     /**< Shifted mode T166US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T208US                (_HFXO_XTALCFG_TIMEOUTCBLSB_T208US << 24)     /**< Shifted mode T208US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T250US                (_HFXO_XTALCFG_TIMEOUTCBLSB_T250US << 24)     /**< Shifted mode T250US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T333US                (_HFXO_XTALCFG_TIMEOUTCBLSB_T333US << 24)     /**< Shifted mode T333US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T416US                (_HFXO_XTALCFG_TIMEOUTCBLSB_T416US << 24)     /**< Shifted mode T416US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T833US                (_HFXO_XTALCFG_TIMEOUTCBLSB_T833US << 24)     /**< Shifted mode T833US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T1250US               (_HFXO_XTALCFG_TIMEOUTCBLSB_T1250US << 24)    /**< Shifted mode T1250US for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T2083US               (_HFXO_XTALCFG_TIMEOUTCBLSB_T2083US << 24)    /**< Shifted mode T2083US for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T3750US               (_HFXO_XTALCFG_TIMEOUTCBLSB_T3750US << 24)    /**< Shifted mode T3750US for HFXO_XTALCFG       */

/* Bit fields for HFXO XTALCTRL */
#define _HFXO_XTALCTRL_RESETVALUE                       0x033C3C3CUL                                   /**< Default value for HFXO_XTALCTRL             */
#define _HFXO_XTALCTRL_MASK                             0x8FFFFFFFUL                                   /**< Mask for HFXO_XTALCTRL                      */
#define _HFXO_XTALCTRL_COREBIASANA_SHIFT                0                                              /**< Shift value for HFXO_COREBIASANA            */
#define _HFXO_XTALCTRL_COREBIASANA_MASK                 0xFFUL                                         /**< Bit mask for HFXO_COREBIASANA               */
#define _HFXO_XTALCTRL_COREBIASANA_DEFAULT              0x0000003CUL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define HFXO_XTALCTRL_COREBIASANA_DEFAULT               (_HFXO_XTALCTRL_COREBIASANA_DEFAULT << 0)      /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */
#define _HFXO_XTALCTRL_CTUNEXIANA_SHIFT                 8                                              /**< Shift value for HFXO_CTUNEXIANA             */
#define _HFXO_XTALCTRL_CTUNEXIANA_MASK                  0xFF00UL                                       /**< Bit mask for HFXO_CTUNEXIANA                */
#define _HFXO_XTALCTRL_CTUNEXIANA_DEFAULT               0x0000003CUL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define HFXO_XTALCTRL_CTUNEXIANA_DEFAULT                (_HFXO_XTALCTRL_CTUNEXIANA_DEFAULT << 8)       /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */
#define _HFXO_XTALCTRL_CTUNEXOANA_SHIFT                 16                                             /**< Shift value for HFXO_CTUNEXOANA             */
#define _HFXO_XTALCTRL_CTUNEXOANA_MASK                  0xFF0000UL                                     /**< Bit mask for HFXO_CTUNEXOANA                */
#define _HFXO_XTALCTRL_CTUNEXOANA_DEFAULT               0x0000003CUL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define HFXO_XTALCTRL_CTUNEXOANA_DEFAULT                (_HFXO_XTALCTRL_CTUNEXOANA_DEFAULT << 16)      /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */
#define _HFXO_XTALCTRL_CTUNEFIXANA_SHIFT                24                                             /**< Shift value for HFXO_CTUNEFIXANA            */
#define _HFXO_XTALCTRL_CTUNEFIXANA_MASK                 0x3000000UL                                    /**< Bit mask for HFXO_CTUNEFIXANA               */
#define _HFXO_XTALCTRL_CTUNEFIXANA_DEFAULT              0x00000003UL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define _HFXO_XTALCTRL_CTUNEFIXANA_NONE                 0x00000000UL                                   /**< Mode NONE for HFXO_XTALCTRL                 */
#define _HFXO_XTALCTRL_CTUNEFIXANA_XI                   0x00000001UL                                   /**< Mode XI for HFXO_XTALCTRL                   */
#define _HFXO_XTALCTRL_CTUNEFIXANA_XO                   0x00000002UL                                   /**< Mode XO for HFXO_XTALCTRL                   */
#define _HFXO_XTALCTRL_CTUNEFIXANA_BOTH                 0x00000003UL                                   /**< Mode BOTH for HFXO_XTALCTRL                 */
#define HFXO_XTALCTRL_CTUNEFIXANA_DEFAULT               (_HFXO_XTALCTRL_CTUNEFIXANA_DEFAULT << 24)     /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */
#define HFXO_XTALCTRL_CTUNEFIXANA_NONE                  (_HFXO_XTALCTRL_CTUNEFIXANA_NONE << 24)        /**< Shifted mode NONE for HFXO_XTALCTRL         */
#define HFXO_XTALCTRL_CTUNEFIXANA_XI                    (_HFXO_XTALCTRL_CTUNEFIXANA_XI << 24)          /**< Shifted mode XI for HFXO_XTALCTRL           */
#define HFXO_XTALCTRL_CTUNEFIXANA_XO                    (_HFXO_XTALCTRL_CTUNEFIXANA_XO << 24)          /**< Shifted mode XO for HFXO_XTALCTRL           */
#define HFXO_XTALCTRL_CTUNEFIXANA_BOTH                  (_HFXO_XTALCTRL_CTUNEFIXANA_BOTH << 24)        /**< Shifted mode BOTH for HFXO_XTALCTRL         */
#define _HFXO_XTALCTRL_COREDGENANA_SHIFT                26                                             /**< Shift value for HFXO_COREDGENANA            */
#define _HFXO_XTALCTRL_COREDGENANA_MASK                 0xC000000UL                                    /**< Bit mask for HFXO_COREDGENANA               */
#define _HFXO_XTALCTRL_COREDGENANA_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define _HFXO_XTALCTRL_COREDGENANA_NONE                 0x00000000UL                                   /**< Mode NONE for HFXO_XTALCTRL                 */
#define _HFXO_XTALCTRL_COREDGENANA_DGEN33               0x00000001UL                                   /**< Mode DGEN33 for HFXO_XTALCTRL               */
#define _HFXO_XTALCTRL_COREDGENANA_DGEN50               0x00000002UL                                   /**< Mode DGEN50 for HFXO_XTALCTRL               */
#define _HFXO_XTALCTRL_COREDGENANA_DGEN100              0x00000003UL                                   /**< Mode DGEN100 for HFXO_XTALCTRL              */
#define HFXO_XTALCTRL_COREDGENANA_DEFAULT               (_HFXO_XTALCTRL_COREDGENANA_DEFAULT << 26)     /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */
#define HFXO_XTALCTRL_COREDGENANA_NONE                  (_HFXO_XTALCTRL_COREDGENANA_NONE << 26)        /**< Shifted mode NONE for HFXO_XTALCTRL         */
#define HFXO_XTALCTRL_COREDGENANA_DGEN33                (_HFXO_XTALCTRL_COREDGENANA_DGEN33 << 26)      /**< Shifted mode DGEN33 for HFXO_XTALCTRL       */
#define HFXO_XTALCTRL_COREDGENANA_DGEN50                (_HFXO_XTALCTRL_COREDGENANA_DGEN50 << 26)      /**< Shifted mode DGEN50 for HFXO_XTALCTRL       */
#define HFXO_XTALCTRL_COREDGENANA_DGEN100               (_HFXO_XTALCTRL_COREDGENANA_DGEN100 << 26)     /**< Shifted mode DGEN100 for HFXO_XTALCTRL      */
#define HFXO_XTALCTRL_SKIPCOREBIASOPT                   (0x1UL << 31)                                  /**< Skip Core Bias Optimization                 */
#define _HFXO_XTALCTRL_SKIPCOREBIASOPT_SHIFT            31                                             /**< Shift value for HFXO_SKIPCOREBIASOPT        */
#define _HFXO_XTALCTRL_SKIPCOREBIASOPT_MASK             0x80000000UL                                   /**< Bit mask for HFXO_SKIPCOREBIASOPT           */
#define _HFXO_XTALCTRL_SKIPCOREBIASOPT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define HFXO_XTALCTRL_SKIPCOREBIASOPT_DEFAULT           (_HFXO_XTALCTRL_SKIPCOREBIASOPT_DEFAULT << 31) /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */

/* Bit fields for HFXO XTALCTRL1 */
#define _HFXO_XTALCTRL1_RESETVALUE                      0x0000003CUL                                    /**< Default value for HFXO_XTALCTRL1            */
#define _HFXO_XTALCTRL1_MASK                            0x000000FFUL                                    /**< Mask for HFXO_XTALCTRL1                     */
#define _HFXO_XTALCTRL1_CTUNEXIBUFOUTANA_SHIFT          0                                               /**< Shift value for HFXO_CTUNEXIBUFOUTANA       */
#define _HFXO_XTALCTRL1_CTUNEXIBUFOUTANA_MASK           0xFFUL                                          /**< Bit mask for HFXO_CTUNEXIBUFOUTANA          */
#define _HFXO_XTALCTRL1_CTUNEXIBUFOUTANA_DEFAULT        0x0000003CUL                                    /**< Mode DEFAULT for HFXO_XTALCTRL1             */
#define HFXO_XTALCTRL1_CTUNEXIBUFOUTANA_DEFAULT         (_HFXO_XTALCTRL1_CTUNEXIBUFOUTANA_DEFAULT << 0) /**< Shifted mode DEFAULT for HFXO_XTALCTRL1     */

/* Bit fields for HFXO CFG */
#define _HFXO_CFG_RESETVALUE                            0x10000000UL                             /**< Default value for HFXO_CFG                  */
#define _HFXO_CFG_MASK                                  0xB000000FUL                             /**< Mask for HFXO_CFG                           */
#define _HFXO_CFG_MODE_SHIFT                            0                                        /**< Shift value for HFXO_MODE                   */
#define _HFXO_CFG_MODE_MASK                             0x3UL                                    /**< Bit mask for HFXO_MODE                      */
#define _HFXO_CFG_MODE_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for HFXO_CFG                   */
#define _HFXO_CFG_MODE_XTAL                             0x00000000UL                             /**< Mode XTAL for HFXO_CFG                      */
#define _HFXO_CFG_MODE_EXTCLK                           0x00000001UL                             /**< Mode EXTCLK for HFXO_CFG                    */
#define _HFXO_CFG_MODE_EXTCLKPKDET                      0x00000002UL                             /**< Mode EXTCLKPKDET for HFXO_CFG               */
#define HFXO_CFG_MODE_DEFAULT                           (_HFXO_CFG_MODE_DEFAULT << 0)            /**< Shifted mode DEFAULT for HFXO_CFG           */
#define HFXO_CFG_MODE_XTAL                              (_HFXO_CFG_MODE_XTAL << 0)               /**< Shifted mode XTAL for HFXO_CFG              */
#define HFXO_CFG_MODE_EXTCLK                            (_HFXO_CFG_MODE_EXTCLK << 0)             /**< Shifted mode EXTCLK for HFXO_CFG            */
#define HFXO_CFG_MODE_EXTCLKPKDET                       (_HFXO_CFG_MODE_EXTCLKPKDET << 0)        /**< Shifted mode EXTCLKPKDET for HFXO_CFG       */
#define HFXO_CFG_ENXIDCBIASANA                          (0x1UL << 2)                             /**< Enable XI Internal DC Bias                  */
#define _HFXO_CFG_ENXIDCBIASANA_SHIFT                   2                                        /**< Shift value for HFXO_ENXIDCBIASANA          */
#define _HFXO_CFG_ENXIDCBIASANA_MASK                    0x4UL                                    /**< Bit mask for HFXO_ENXIDCBIASANA             */
#define _HFXO_CFG_ENXIDCBIASANA_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for HFXO_CFG                   */
#define HFXO_CFG_ENXIDCBIASANA_DEFAULT                  (_HFXO_CFG_ENXIDCBIASANA_DEFAULT << 2)   /**< Shifted mode DEFAULT for HFXO_CFG           */
#define HFXO_CFG_SQBUFSCHTRGANA                         (0x1UL << 3)                             /**< Squaring Buffer Schmitt Trigger             */
#define _HFXO_CFG_SQBUFSCHTRGANA_SHIFT                  3                                        /**< Shift value for HFXO_SQBUFSCHTRGANA         */
#define _HFXO_CFG_SQBUFSCHTRGANA_MASK                   0x8UL                                    /**< Bit mask for HFXO_SQBUFSCHTRGANA            */
#define _HFXO_CFG_SQBUFSCHTRGANA_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for HFXO_CFG                   */
#define _HFXO_CFG_SQBUFSCHTRGANA_DISABLE                0x00000000UL                             /**< Mode DISABLE for HFXO_CFG                   */
#define _HFXO_CFG_SQBUFSCHTRGANA_ENABLE                 0x00000001UL                             /**< Mode ENABLE for HFXO_CFG                    */
#define HFXO_CFG_SQBUFSCHTRGANA_DEFAULT                 (_HFXO_CFG_SQBUFSCHTRGANA_DEFAULT << 3)  /**< Shifted mode DEFAULT for HFXO_CFG           */
#define HFXO_CFG_SQBUFSCHTRGANA_DISABLE                 (_HFXO_CFG_SQBUFSCHTRGANA_DISABLE << 3)  /**< Shifted mode DISABLE for HFXO_CFG           */
#define HFXO_CFG_SQBUFSCHTRGANA_ENABLE                  (_HFXO_CFG_SQBUFSCHTRGANA_ENABLE << 3)   /**< Shifted mode ENABLE for HFXO_CFG            */
#define HFXO_CFG_FORCELFTIMEOUT                         (0x1UL << 28)                            /**< Force Low Frequency Timeout                 */
#define _HFXO_CFG_FORCELFTIMEOUT_SHIFT                  28                                       /**< Shift value for HFXO_FORCELFTIMEOUT         */
#define _HFXO_CFG_FORCELFTIMEOUT_MASK                   0x10000000UL                             /**< Bit mask for HFXO_FORCELFTIMEOUT            */
#define _HFXO_CFG_FORCELFTIMEOUT_DEFAULT                0x00000001UL                             /**< Mode DEFAULT for HFXO_CFG                   */
#define HFXO_CFG_FORCELFTIMEOUT_DEFAULT                 (_HFXO_CFG_FORCELFTIMEOUT_DEFAULT << 28) /**< Shifted mode DEFAULT for HFXO_CFG           */

/* Bit fields for HFXO CTRL */
#define _HFXO_CTRL_RESETVALUE                           0x07000040UL                                    /**< Default value for HFXO_CTRL                 */
#define _HFXO_CTRL_MASK                                 0x8707FF7DUL                                    /**< Mask for HFXO_CTRL                          */
#define HFXO_CTRL_BUFOUTFREEZE                          (0x1UL << 0)                                    /**< Freeze BUFOUT Controls                      */
#define _HFXO_CTRL_BUFOUTFREEZE_SHIFT                   0                                               /**< Shift value for HFXO_BUFOUTFREEZE           */
#define _HFXO_CTRL_BUFOUTFREEZE_MASK                    0x1UL                                           /**< Bit mask for HFXO_BUFOUTFREEZE              */
#define _HFXO_CTRL_BUFOUTFREEZE_DEFAULT                 0x00000000UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_BUFOUTFREEZE_DEFAULT                  (_HFXO_CTRL_BUFOUTFREEZE_DEFAULT << 0)          /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_KEEPWARM                              (0x1UL << 2)                                    /**< Keep Warm                                   */
#define _HFXO_CTRL_KEEPWARM_SHIFT                       2                                               /**< Shift value for HFXO_KEEPWARM               */
#define _HFXO_CTRL_KEEPWARM_MASK                        0x4UL                                           /**< Bit mask for HFXO_KEEPWARM                  */
#define _HFXO_CTRL_KEEPWARM_DEFAULT                     0x00000000UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_KEEPWARM_DEFAULT                      (_HFXO_CTRL_KEEPWARM_DEFAULT << 2)              /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_EM23ONDEMAND                          (0x1UL << 3)                                    /**< On-demand During EM23                       */
#define _HFXO_CTRL_EM23ONDEMAND_SHIFT                   3                                               /**< Shift value for HFXO_EM23ONDEMAND           */
#define _HFXO_CTRL_EM23ONDEMAND_MASK                    0x8UL                                           /**< Bit mask for HFXO_EM23ONDEMAND              */
#define _HFXO_CTRL_EM23ONDEMAND_DEFAULT                 0x00000000UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_EM23ONDEMAND_DEFAULT                  (_HFXO_CTRL_EM23ONDEMAND_DEFAULT << 3)          /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_FORCEXI2GNDANA                        (0x1UL << 4)                                    /**< Force XI Pin to Ground                      */
#define _HFXO_CTRL_FORCEXI2GNDANA_SHIFT                 4                                               /**< Shift value for HFXO_FORCEXI2GNDANA         */
#define _HFXO_CTRL_FORCEXI2GNDANA_MASK                  0x10UL                                          /**< Bit mask for HFXO_FORCEXI2GNDANA            */
#define _HFXO_CTRL_FORCEXI2GNDANA_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define _HFXO_CTRL_FORCEXI2GNDANA_DISABLE               0x00000000UL                                    /**< Mode DISABLE for HFXO_CTRL                  */
#define _HFXO_CTRL_FORCEXI2GNDANA_ENABLE                0x00000001UL                                    /**< Mode ENABLE for HFXO_CTRL                   */
#define HFXO_CTRL_FORCEXI2GNDANA_DEFAULT                (_HFXO_CTRL_FORCEXI2GNDANA_DEFAULT << 4)        /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_FORCEXI2GNDANA_DISABLE                (_HFXO_CTRL_FORCEXI2GNDANA_DISABLE << 4)        /**< Shifted mode DISABLE for HFXO_CTRL          */
#define HFXO_CTRL_FORCEXI2GNDANA_ENABLE                 (_HFXO_CTRL_FORCEXI2GNDANA_ENABLE << 4)         /**< Shifted mode ENABLE for HFXO_CTRL           */
#define HFXO_CTRL_FORCEXO2GNDANA                        (0x1UL << 5)                                    /**< Force XO Pin to Ground                      */
#define _HFXO_CTRL_FORCEXO2GNDANA_SHIFT                 5                                               /**< Shift value for HFXO_FORCEXO2GNDANA         */
#define _HFXO_CTRL_FORCEXO2GNDANA_MASK                  0x20UL                                          /**< Bit mask for HFXO_FORCEXO2GNDANA            */
#define _HFXO_CTRL_FORCEXO2GNDANA_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define _HFXO_CTRL_FORCEXO2GNDANA_DISABLE               0x00000000UL                                    /**< Mode DISABLE for HFXO_CTRL                  */
#define _HFXO_CTRL_FORCEXO2GNDANA_ENABLE                0x00000001UL                                    /**< Mode ENABLE for HFXO_CTRL                   */
#define HFXO_CTRL_FORCEXO2GNDANA_DEFAULT                (_HFXO_CTRL_FORCEXO2GNDANA_DEFAULT << 5)        /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_FORCEXO2GNDANA_DISABLE                (_HFXO_CTRL_FORCEXO2GNDANA_DISABLE << 5)        /**< Shifted mode DISABLE for HFXO_CTRL          */
#define HFXO_CTRL_FORCEXO2GNDANA_ENABLE                 (_HFXO_CTRL_FORCEXO2GNDANA_ENABLE << 5)         /**< Shifted mode ENABLE for HFXO_CTRL           */
#define HFXO_CTRL_FORCECTUNEMAX                         (0x1UL << 6)                                    /**< Force Tuning Cap to Max Value               */
#define _HFXO_CTRL_FORCECTUNEMAX_SHIFT                  6                                               /**< Shift value for HFXO_FORCECTUNEMAX          */
#define _HFXO_CTRL_FORCECTUNEMAX_MASK                   0x40UL                                          /**< Bit mask for HFXO_FORCECTUNEMAX             */
#define _HFXO_CTRL_FORCECTUNEMAX_DEFAULT                0x00000001UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_FORCECTUNEMAX_DEFAULT                 (_HFXO_CTRL_FORCECTUNEMAX_DEFAULT << 6)         /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define _HFXO_CTRL_PRSSTATUSSEL0_SHIFT                  8                                               /**< Shift value for HFXO_PRSSTATUSSEL0          */
#define _HFXO_CTRL_PRSSTATUSSEL0_MASK                   0xF00UL                                         /**< Bit mask for HFXO_PRSSTATUSSEL0             */
#define _HFXO_CTRL_PRSSTATUSSEL0_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define _HFXO_CTRL_PRSSTATUSSEL0_DISABLED               0x00000000UL                                    /**< Mode DISABLED for HFXO_CTRL                 */
#define _HFXO_CTRL_PRSSTATUSSEL0_ENS                    0x00000001UL                                    /**< Mode ENS for HFXO_CTRL                      */
#define _HFXO_CTRL_PRSSTATUSSEL0_COREBIASOPTRDY         0x00000002UL                                    /**< Mode COREBIASOPTRDY for HFXO_CTRL           */
#define _HFXO_CTRL_PRSSTATUSSEL0_RDY                    0x00000003UL                                    /**< Mode RDY for HFXO_CTRL                      */
#define _HFXO_CTRL_PRSSTATUSSEL0_PRSRDY                 0x00000004UL                                    /**< Mode PRSRDY for HFXO_CTRL                   */
#define _HFXO_CTRL_PRSSTATUSSEL0_BUFOUTRDY              0x00000005UL                                    /**< Mode BUFOUTRDY for HFXO_CTRL                */
#define _HFXO_CTRL_PRSSTATUSSEL0_HWREQ                  0x00000008UL                                    /**< Mode HWREQ for HFXO_CTRL                    */
#define _HFXO_CTRL_PRSSTATUSSEL0_PRSHWREQ               0x00000009UL                                    /**< Mode PRSHWREQ for HFXO_CTRL                 */
#define _HFXO_CTRL_PRSSTATUSSEL0_BUFOUTHWREQ            0x0000000AUL                                    /**< Mode BUFOUTHWREQ for HFXO_CTRL              */
#define HFXO_CTRL_PRSSTATUSSEL0_DEFAULT                 (_HFXO_CTRL_PRSSTATUSSEL0_DEFAULT << 8)         /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_PRSSTATUSSEL0_DISABLED                (_HFXO_CTRL_PRSSTATUSSEL0_DISABLED << 8)        /**< Shifted mode DISABLED for HFXO_CTRL         */
#define HFXO_CTRL_PRSSTATUSSEL0_ENS                     (_HFXO_CTRL_PRSSTATUSSEL0_ENS << 8)             /**< Shifted mode ENS for HFXO_CTRL              */
#define HFXO_CTRL_PRSSTATUSSEL0_COREBIASOPTRDY          (_HFXO_CTRL_PRSSTATUSSEL0_COREBIASOPTRDY << 8)  /**< Shifted mode COREBIASOPTRDY for HFXO_CTRL   */
#define HFXO_CTRL_PRSSTATUSSEL0_RDY                     (_HFXO_CTRL_PRSSTATUSSEL0_RDY << 8)             /**< Shifted mode RDY for HFXO_CTRL              */
#define HFXO_CTRL_PRSSTATUSSEL0_PRSRDY                  (_HFXO_CTRL_PRSSTATUSSEL0_PRSRDY << 8)          /**< Shifted mode PRSRDY for HFXO_CTRL           */
#define HFXO_CTRL_PRSSTATUSSEL0_BUFOUTRDY               (_HFXO_CTRL_PRSSTATUSSEL0_BUFOUTRDY << 8)       /**< Shifted mode BUFOUTRDY for HFXO_CTRL        */
#define HFXO_CTRL_PRSSTATUSSEL0_HWREQ                   (_HFXO_CTRL_PRSSTATUSSEL0_HWREQ << 8)           /**< Shifted mode HWREQ for HFXO_CTRL            */
#define HFXO_CTRL_PRSSTATUSSEL0_PRSHWREQ                (_HFXO_CTRL_PRSSTATUSSEL0_PRSHWREQ << 8)        /**< Shifted mode PRSHWREQ for HFXO_CTRL         */
#define HFXO_CTRL_PRSSTATUSSEL0_BUFOUTHWREQ             (_HFXO_CTRL_PRSSTATUSSEL0_BUFOUTHWREQ << 8)     /**< Shifted mode BUFOUTHWREQ for HFXO_CTRL      */
#define _HFXO_CTRL_PRSSTATUSSEL1_SHIFT                  12                                              /**< Shift value for HFXO_PRSSTATUSSEL1          */
#define _HFXO_CTRL_PRSSTATUSSEL1_MASK                   0xF000UL                                        /**< Bit mask for HFXO_PRSSTATUSSEL1             */
#define _HFXO_CTRL_PRSSTATUSSEL1_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define _HFXO_CTRL_PRSSTATUSSEL1_DISABLED               0x00000000UL                                    /**< Mode DISABLED for HFXO_CTRL                 */
#define _HFXO_CTRL_PRSSTATUSSEL1_ENS                    0x00000001UL                                    /**< Mode ENS for HFXO_CTRL                      */
#define _HFXO_CTRL_PRSSTATUSSEL1_COREBIASOPTRDY         0x00000002UL                                    /**< Mode COREBIASOPTRDY for HFXO_CTRL           */
#define _HFXO_CTRL_PRSSTATUSSEL1_RDY                    0x00000003UL                                    /**< Mode RDY for HFXO_CTRL                      */
#define _HFXO_CTRL_PRSSTATUSSEL1_PRSRDY                 0x00000004UL                                    /**< Mode PRSRDY for HFXO_CTRL                   */
#define _HFXO_CTRL_PRSSTATUSSEL1_BUFOUTRDY              0x00000005UL                                    /**< Mode BUFOUTRDY for HFXO_CTRL                */
#define _HFXO_CTRL_PRSSTATUSSEL1_HWREQ                  0x00000008UL                                    /**< Mode HWREQ for HFXO_CTRL                    */
#define _HFXO_CTRL_PRSSTATUSSEL1_PRSHWREQ               0x00000009UL                                    /**< Mode PRSHWREQ for HFXO_CTRL                 */
#define _HFXO_CTRL_PRSSTATUSSEL1_BUFOUTHWREQ            0x0000000AUL                                    /**< Mode BUFOUTHWREQ for HFXO_CTRL              */
#define HFXO_CTRL_PRSSTATUSSEL1_DEFAULT                 (_HFXO_CTRL_PRSSTATUSSEL1_DEFAULT << 12)        /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_PRSSTATUSSEL1_DISABLED                (_HFXO_CTRL_PRSSTATUSSEL1_DISABLED << 12)       /**< Shifted mode DISABLED for HFXO_CTRL         */
#define HFXO_CTRL_PRSSTATUSSEL1_ENS                     (_HFXO_CTRL_PRSSTATUSSEL1_ENS << 12)            /**< Shifted mode ENS for HFXO_CTRL              */
#define HFXO_CTRL_PRSSTATUSSEL1_COREBIASOPTRDY          (_HFXO_CTRL_PRSSTATUSSEL1_COREBIASOPTRDY << 12) /**< Shifted mode COREBIASOPTRDY for HFXO_CTRL   */
#define HFXO_CTRL_PRSSTATUSSEL1_RDY                     (_HFXO_CTRL_PRSSTATUSSEL1_RDY << 12)            /**< Shifted mode RDY for HFXO_CTRL              */
#define HFXO_CTRL_PRSSTATUSSEL1_PRSRDY                  (_HFXO_CTRL_PRSSTATUSSEL1_PRSRDY << 12)         /**< Shifted mode PRSRDY for HFXO_CTRL           */
#define HFXO_CTRL_PRSSTATUSSEL1_BUFOUTRDY               (_HFXO_CTRL_PRSSTATUSSEL1_BUFOUTRDY << 12)      /**< Shifted mode BUFOUTRDY for HFXO_CTRL        */
#define HFXO_CTRL_PRSSTATUSSEL1_HWREQ                   (_HFXO_CTRL_PRSSTATUSSEL1_HWREQ << 12)          /**< Shifted mode HWREQ for HFXO_CTRL            */
#define HFXO_CTRL_PRSSTATUSSEL1_PRSHWREQ                (_HFXO_CTRL_PRSSTATUSSEL1_PRSHWREQ << 12)       /**< Shifted mode PRSHWREQ for HFXO_CTRL         */
#define HFXO_CTRL_PRSSTATUSSEL1_BUFOUTHWREQ             (_HFXO_CTRL_PRSSTATUSSEL1_BUFOUTHWREQ << 12)    /**< Shifted mode BUFOUTHWREQ for HFXO_CTRL      */
#define HFXO_CTRL_FORCEEN                               (0x1UL << 16)                                   /**< Force Digital Clock Request                 */
#define _HFXO_CTRL_FORCEEN_SHIFT                        16                                              /**< Shift value for HFXO_FORCEEN                */
#define _HFXO_CTRL_FORCEEN_MASK                         0x10000UL                                       /**< Bit mask for HFXO_FORCEEN                   */
#define _HFXO_CTRL_FORCEEN_DEFAULT                      0x00000000UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_FORCEEN_DEFAULT                       (_HFXO_CTRL_FORCEEN_DEFAULT << 16)              /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_FORCEENPRS                            (0x1UL << 17)                                   /**< Force PRS Oscillator Request                */
#define _HFXO_CTRL_FORCEENPRS_SHIFT                     17                                              /**< Shift value for HFXO_FORCEENPRS             */
#define _HFXO_CTRL_FORCEENPRS_MASK                      0x20000UL                                       /**< Bit mask for HFXO_FORCEENPRS                */
#define _HFXO_CTRL_FORCEENPRS_DEFAULT                   0x00000000UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_FORCEENPRS_DEFAULT                    (_HFXO_CTRL_FORCEENPRS_DEFAULT << 17)           /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_FORCEENBUFOUT                         (0x1UL << 18)                                   /**< Force BUFOUT Request                        */
#define _HFXO_CTRL_FORCEENBUFOUT_SHIFT                  18                                              /**< Shift value for HFXO_FORCEENBUFOUT          */
#define _HFXO_CTRL_FORCEENBUFOUT_MASK                   0x40000UL                                       /**< Bit mask for HFXO_FORCEENBUFOUT             */
#define _HFXO_CTRL_FORCEENBUFOUT_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_FORCEENBUFOUT_DEFAULT                 (_HFXO_CTRL_FORCEENBUFOUT_DEFAULT << 18)        /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_DISONDEMAND                           (0x1UL << 24)                                   /**< Disable On-demand For Digital Clock         */
#define _HFXO_CTRL_DISONDEMAND_SHIFT                    24                                              /**< Shift value for HFXO_DISONDEMAND            */
#define _HFXO_CTRL_DISONDEMAND_MASK                     0x1000000UL                                     /**< Bit mask for HFXO_DISONDEMAND               */
#define _HFXO_CTRL_DISONDEMAND_DEFAULT                  0x00000001UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_DISONDEMAND_DEFAULT                   (_HFXO_CTRL_DISONDEMAND_DEFAULT << 24)          /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_DISONDEMANDPRS                        (0x1UL << 25)                                   /**< Disable On-demand For PRS                   */
#define _HFXO_CTRL_DISONDEMANDPRS_SHIFT                 25                                              /**< Shift value for HFXO_DISONDEMANDPRS         */
#define _HFXO_CTRL_DISONDEMANDPRS_MASK                  0x2000000UL                                     /**< Bit mask for HFXO_DISONDEMANDPRS            */
#define _HFXO_CTRL_DISONDEMANDPRS_DEFAULT               0x00000001UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_DISONDEMANDPRS_DEFAULT                (_HFXO_CTRL_DISONDEMANDPRS_DEFAULT << 25)       /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_DISONDEMANDBUFOUT                     (0x1UL << 26)                                   /**< Disable On-demand For BUFOUT                */
#define _HFXO_CTRL_DISONDEMANDBUFOUT_SHIFT              26                                              /**< Shift value for HFXO_DISONDEMANDBUFOUT      */
#define _HFXO_CTRL_DISONDEMANDBUFOUT_MASK               0x4000000UL                                     /**< Bit mask for HFXO_DISONDEMANDBUFOUT         */
#define _HFXO_CTRL_DISONDEMANDBUFOUT_DEFAULT            0x00000001UL                                    /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_DISONDEMANDBUFOUT_DEFAULT             (_HFXO_CTRL_DISONDEMANDBUFOUT_DEFAULT << 26)    /**< Shifted mode DEFAULT for HFXO_CTRL          */

/* Bit fields for HFXO BUFOUTTRIM */
#define _HFXO_BUFOUTTRIM_RESETVALUE                     0x00000008UL                               /**< Default value for HFXO_BUFOUTTRIM           */
#define _HFXO_BUFOUTTRIM_MASK                           0x0000000FUL                               /**< Mask for HFXO_BUFOUTTRIM                    */
#define _HFXO_BUFOUTTRIM_VTRTRIMANA_SHIFT               0                                          /**< Shift value for HFXO_VTRTRIMANA             */
#define _HFXO_BUFOUTTRIM_VTRTRIMANA_MASK                0xFUL                                      /**< Bit mask for HFXO_VTRTRIMANA                */
#define _HFXO_BUFOUTTRIM_VTRTRIMANA_DEFAULT             0x00000008UL                               /**< Mode DEFAULT for HFXO_BUFOUTTRIM            */
#define HFXO_BUFOUTTRIM_VTRTRIMANA_DEFAULT              (_HFXO_BUFOUTTRIM_VTRTRIMANA_DEFAULT << 0) /**< Shifted mode DEFAULT for HFXO_BUFOUTTRIM    */

/* Bit fields for HFXO BUFOUTCTRL */
#define _HFXO_BUFOUTCTRL_RESETVALUE                     0x00643C15UL                                         /**< Default value for HFXO_BUFOUTCTRL           */
#define _HFXO_BUFOUTCTRL_MASK                           0xC0FFFFFFUL                                         /**< Mask for HFXO_BUFOUTCTRL                    */
#define _HFXO_BUFOUTCTRL_XOUTBIASANA_SHIFT              0                                                    /**< Shift value for HFXO_XOUTBIASANA            */
#define _HFXO_BUFOUTCTRL_XOUTBIASANA_MASK               0xFUL                                                /**< Bit mask for HFXO_XOUTBIASANA               */
#define _HFXO_BUFOUTCTRL_XOUTBIASANA_DEFAULT            0x00000005UL                                         /**< Mode DEFAULT for HFXO_BUFOUTCTRL            */
#define HFXO_BUFOUTCTRL_XOUTBIASANA_DEFAULT             (_HFXO_BUFOUTCTRL_XOUTBIASANA_DEFAULT << 0)          /**< Shifted mode DEFAULT for HFXO_BUFOUTCTRL    */
#define _HFXO_BUFOUTCTRL_XOUTCFANA_SHIFT                4                                                    /**< Shift value for HFXO_XOUTCFANA              */
#define _HFXO_BUFOUTCTRL_XOUTCFANA_MASK                 0xF0UL                                               /**< Bit mask for HFXO_XOUTCFANA                 */
#define _HFXO_BUFOUTCTRL_XOUTCFANA_DEFAULT              0x00000001UL                                         /**< Mode DEFAULT for HFXO_BUFOUTCTRL            */
#define HFXO_BUFOUTCTRL_XOUTCFANA_DEFAULT               (_HFXO_BUFOUTCTRL_XOUTCFANA_DEFAULT << 4)            /**< Shifted mode DEFAULT for HFXO_BUFOUTCTRL    */
#define _HFXO_BUFOUTCTRL_XOUTGMANA_SHIFT                8                                                    /**< Shift value for HFXO_XOUTGMANA              */
#define _HFXO_BUFOUTCTRL_XOUTGMANA_MASK                 0xF00UL                                              /**< Bit mask for HFXO_XOUTGMANA                 */
#define _HFXO_BUFOUTCTRL_XOUTGMANA_DEFAULT              0x0000000CUL                                         /**< Mode DEFAULT for HFXO_BUFOUTCTRL            */
#define HFXO_BUFOUTCTRL_XOUTGMANA_DEFAULT               (_HFXO_BUFOUTCTRL_XOUTGMANA_DEFAULT << 8)            /**< Shifted mode DEFAULT for HFXO_BUFOUTCTRL    */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_SHIFT          12                                                   /**< Shift value for HFXO_PEAKDETTHRESANA        */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_MASK           0xF000UL                                             /**< Bit mask for HFXO_PEAKDETTHRESANA           */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_DEFAULT        0x00000003UL                                         /**< Mode DEFAULT for HFXO_BUFOUTCTRL            */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V105MV         0x00000000UL                                         /**< Mode V105MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V132MV         0x00000001UL                                         /**< Mode V132MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V157MV         0x00000002UL                                         /**< Mode V157MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V184MV         0x00000003UL                                         /**< Mode V184MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V210MV         0x00000004UL                                         /**< Mode V210MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V236MV         0x00000005UL                                         /**< Mode V236MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V262MV         0x00000006UL                                         /**< Mode V262MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V289MV         0x00000007UL                                         /**< Mode V289MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V315MV         0x00000008UL                                         /**< Mode V315MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V341MV         0x00000009UL                                         /**< Mode V341MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V367MV         0x0000000AUL                                         /**< Mode V367MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V394MV         0x0000000BUL                                         /**< Mode V394MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V420MV         0x0000000CUL                                         /**< Mode V420MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V446MV         0x0000000DUL                                         /**< Mode V446MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V472MV         0x0000000EUL                                         /**< Mode V472MV for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V499MV         0x0000000FUL                                         /**< Mode V499MV for HFXO_BUFOUTCTRL             */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_DEFAULT         (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_DEFAULT << 12)     /**< Shifted mode DEFAULT for HFXO_BUFOUTCTRL    */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V105MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V105MV << 12)      /**< Shifted mode V105MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V132MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V132MV << 12)      /**< Shifted mode V132MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V157MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V157MV << 12)      /**< Shifted mode V157MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V184MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V184MV << 12)      /**< Shifted mode V184MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V210MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V210MV << 12)      /**< Shifted mode V210MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V236MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V236MV << 12)      /**< Shifted mode V236MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V262MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V262MV << 12)      /**< Shifted mode V262MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V289MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V289MV << 12)      /**< Shifted mode V289MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V315MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V315MV << 12)      /**< Shifted mode V315MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V341MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V341MV << 12)      /**< Shifted mode V341MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V367MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V367MV << 12)      /**< Shifted mode V367MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V394MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V394MV << 12)      /**< Shifted mode V394MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V420MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V420MV << 12)      /**< Shifted mode V420MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V446MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V446MV << 12)      /**< Shifted mode V446MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V472MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V472MV << 12)      /**< Shifted mode V472MV for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V499MV          (_HFXO_BUFOUTCTRL_PEAKDETTHRESANA_V499MV << 12)      /**< Shifted mode V499MV for HFXO_BUFOUTCTRL     */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_SHIFT             16                                                   /**< Shift value for HFXO_TIMEOUTCTUNE           */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_MASK              0xF0000UL                                            /**< Bit mask for HFXO_TIMEOUTCTUNE              */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_DEFAULT           0x00000004UL                                         /**< Mode DEFAULT for HFXO_BUFOUTCTRL            */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T2US              0x00000000UL                                         /**< Mode T2US for HFXO_BUFOUTCTRL               */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T5US              0x00000001UL                                         /**< Mode T5US for HFXO_BUFOUTCTRL               */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T10US             0x00000002UL                                         /**< Mode T10US for HFXO_BUFOUTCTRL              */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T16US             0x00000003UL                                         /**< Mode T16US for HFXO_BUFOUTCTRL              */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T21US             0x00000004UL                                         /**< Mode T21US for HFXO_BUFOUTCTRL              */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T26US             0x00000005UL                                         /**< Mode T26US for HFXO_BUFOUTCTRL              */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T31US             0x00000006UL                                         /**< Mode T31US for HFXO_BUFOUTCTRL              */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T42US             0x00000007UL                                         /**< Mode T42US for HFXO_BUFOUTCTRL              */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T52US             0x00000008UL                                         /**< Mode T52US for HFXO_BUFOUTCTRL              */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T63US             0x00000009UL                                         /**< Mode T63US for HFXO_BUFOUTCTRL              */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T83US             0x0000000AUL                                         /**< Mode T83US for HFXO_BUFOUTCTRL              */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T104US            0x0000000BUL                                         /**< Mode T104US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T208US            0x0000000CUL                                         /**< Mode T208US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T313US            0x0000000DUL                                         /**< Mode T313US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T521US            0x0000000EUL                                         /**< Mode T521US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T938US            0x0000000FUL                                         /**< Mode T938US for HFXO_BUFOUTCTRL             */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_DEFAULT            (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_DEFAULT << 16)        /**< Shifted mode DEFAULT for HFXO_BUFOUTCTRL    */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T2US               (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T2US << 16)           /**< Shifted mode T2US for HFXO_BUFOUTCTRL       */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T5US               (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T5US << 16)           /**< Shifted mode T5US for HFXO_BUFOUTCTRL       */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T10US              (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T10US << 16)          /**< Shifted mode T10US for HFXO_BUFOUTCTRL      */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T16US              (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T16US << 16)          /**< Shifted mode T16US for HFXO_BUFOUTCTRL      */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T21US              (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T21US << 16)          /**< Shifted mode T21US for HFXO_BUFOUTCTRL      */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T26US              (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T26US << 16)          /**< Shifted mode T26US for HFXO_BUFOUTCTRL      */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T31US              (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T31US << 16)          /**< Shifted mode T31US for HFXO_BUFOUTCTRL      */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T42US              (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T42US << 16)          /**< Shifted mode T42US for HFXO_BUFOUTCTRL      */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T52US              (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T52US << 16)          /**< Shifted mode T52US for HFXO_BUFOUTCTRL      */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T63US              (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T63US << 16)          /**< Shifted mode T63US for HFXO_BUFOUTCTRL      */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T83US              (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T83US << 16)          /**< Shifted mode T83US for HFXO_BUFOUTCTRL      */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T104US             (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T104US << 16)         /**< Shifted mode T104US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T208US             (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T208US << 16)         /**< Shifted mode T208US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T313US             (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T313US << 16)         /**< Shifted mode T313US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T521US             (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T521US << 16)         /**< Shifted mode T521US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T938US             (_HFXO_BUFOUTCTRL_TIMEOUTCTUNE_T938US << 16)         /**< Shifted mode T938US for HFXO_BUFOUTCTRL     */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_SHIFT           20                                                   /**< Shift value for HFXO_TIMEOUTSTARTUP         */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_MASK            0xF00000UL                                           /**< Bit mask for HFXO_TIMEOUTSTARTUP            */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_DEFAULT         0x00000006UL                                         /**< Mode DEFAULT for HFXO_BUFOUTCTRL            */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T42US           0x00000000UL                                         /**< Mode T42US for HFXO_BUFOUTCTRL              */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T83US           0x00000001UL                                         /**< Mode T83US for HFXO_BUFOUTCTRL              */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T108US          0x00000002UL                                         /**< Mode T108US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T133US          0x00000003UL                                         /**< Mode T133US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T158US          0x00000004UL                                         /**< Mode T158US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T183US          0x00000005UL                                         /**< Mode T183US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T208US          0x00000006UL                                         /**< Mode T208US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T233US          0x00000007UL                                         /**< Mode T233US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T258US          0x00000008UL                                         /**< Mode T258US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T283US          0x00000009UL                                         /**< Mode T283US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T333US          0x0000000AUL                                         /**< Mode T333US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T375US          0x0000000BUL                                         /**< Mode T375US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T417US          0x0000000CUL                                         /**< Mode T417US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T458US          0x0000000DUL                                         /**< Mode T458US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T500US          0x0000000EUL                                         /**< Mode T500US for HFXO_BUFOUTCTRL             */
#define _HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T667US          0x0000000FUL                                         /**< Mode T667US for HFXO_BUFOUTCTRL             */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_DEFAULT          (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_DEFAULT << 20)      /**< Shifted mode DEFAULT for HFXO_BUFOUTCTRL    */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T42US            (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T42US << 20)        /**< Shifted mode T42US for HFXO_BUFOUTCTRL      */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T83US            (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T83US << 20)        /**< Shifted mode T83US for HFXO_BUFOUTCTRL      */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T108US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T108US << 20)       /**< Shifted mode T108US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T133US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T133US << 20)       /**< Shifted mode T133US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T158US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T158US << 20)       /**< Shifted mode T158US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T183US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T183US << 20)       /**< Shifted mode T183US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T208US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T208US << 20)       /**< Shifted mode T208US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T233US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T233US << 20)       /**< Shifted mode T233US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T258US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T258US << 20)       /**< Shifted mode T258US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T283US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T283US << 20)       /**< Shifted mode T283US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T333US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T333US << 20)       /**< Shifted mode T333US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T375US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T375US << 20)       /**< Shifted mode T375US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T417US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T417US << 20)       /**< Shifted mode T417US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T458US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T458US << 20)       /**< Shifted mode T458US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T500US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T500US << 20)       /**< Shifted mode T500US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T667US           (_HFXO_BUFOUTCTRL_TIMEOUTSTARTUP_T667US << 20)       /**< Shifted mode T667US for HFXO_BUFOUTCTRL     */
#define HFXO_BUFOUTCTRL_MINIMUMSTARTUPDELAY             (0x1UL << 31)                                        /**< Minimum Startup Delay                       */
#define _HFXO_BUFOUTCTRL_MINIMUMSTARTUPDELAY_SHIFT      31                                                   /**< Shift value for HFXO_MINIMUMSTARTUPDELAY    */
#define _HFXO_BUFOUTCTRL_MINIMUMSTARTUPDELAY_MASK       0x80000000UL                                         /**< Bit mask for HFXO_MINIMUMSTARTUPDELAY       */
#define _HFXO_BUFOUTCTRL_MINIMUMSTARTUPDELAY_DEFAULT    0x00000000UL                                         /**< Mode DEFAULT for HFXO_BUFOUTCTRL            */
#define HFXO_BUFOUTCTRL_MINIMUMSTARTUPDELAY_DEFAULT     (_HFXO_BUFOUTCTRL_MINIMUMSTARTUPDELAY_DEFAULT << 31) /**< Shifted mode DEFAULT for HFXO_BUFOUTCTRL    */

/* Bit fields for HFXO CMD */
#define _HFXO_CMD_RESETVALUE                            0x00000000UL                         /**< Default value for HFXO_CMD                  */
#define _HFXO_CMD_MASK                                  0x00000001UL                         /**< Mask for HFXO_CMD                           */
#define HFXO_CMD_COREBIASOPT                            (0x1UL << 0)                         /**< Core Bias Optimizaton                       */
#define _HFXO_CMD_COREBIASOPT_SHIFT                     0                                    /**< Shift value for HFXO_COREBIASOPT            */
#define _HFXO_CMD_COREBIASOPT_MASK                      0x1UL                                /**< Bit mask for HFXO_COREBIASOPT               */
#define _HFXO_CMD_COREBIASOPT_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for HFXO_CMD                   */
#define HFXO_CMD_COREBIASOPT_DEFAULT                    (_HFXO_CMD_COREBIASOPT_DEFAULT << 0) /**< Shifted mode DEFAULT for HFXO_CMD           */

/* Bit fields for HFXO STATUS */
#define _HFXO_STATUS_RESETVALUE                         0x00000000UL                               /**< Default value for HFXO_STATUS               */
#define _HFXO_STATUS_MASK                               0xC03F800FUL                               /**< Mask for HFXO_STATUS                        */
#define HFXO_STATUS_RDY                                 (0x1UL << 0)                               /**< Ready Status                                */
#define _HFXO_STATUS_RDY_SHIFT                          0                                          /**< Shift value for HFXO_RDY                    */
#define _HFXO_STATUS_RDY_MASK                           0x1UL                                      /**< Bit mask for HFXO_RDY                       */
#define _HFXO_STATUS_RDY_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_RDY_DEFAULT                         (_HFXO_STATUS_RDY_DEFAULT << 0)            /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_COREBIASOPTRDY                      (0x1UL << 1)                               /**< Core Bias Optimization Ready                */
#define _HFXO_STATUS_COREBIASOPTRDY_SHIFT               1                                          /**< Shift value for HFXO_COREBIASOPTRDY         */
#define _HFXO_STATUS_COREBIASOPTRDY_MASK                0x2UL                                      /**< Bit mask for HFXO_COREBIASOPTRDY            */
#define _HFXO_STATUS_COREBIASOPTRDY_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_COREBIASOPTRDY_DEFAULT              (_HFXO_STATUS_COREBIASOPTRDY_DEFAULT << 1) /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_PRSRDY                              (0x1UL << 2)                               /**< PRS Ready Status                            */
#define _HFXO_STATUS_PRSRDY_SHIFT                       2                                          /**< Shift value for HFXO_PRSRDY                 */
#define _HFXO_STATUS_PRSRDY_MASK                        0x4UL                                      /**< Bit mask for HFXO_PRSRDY                    */
#define _HFXO_STATUS_PRSRDY_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_PRSRDY_DEFAULT                      (_HFXO_STATUS_PRSRDY_DEFAULT << 2)         /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_BUFOUTRDY                           (0x1UL << 3)                               /**< BUFOUT Ready Status                         */
#define _HFXO_STATUS_BUFOUTRDY_SHIFT                    3                                          /**< Shift value for HFXO_BUFOUTRDY              */
#define _HFXO_STATUS_BUFOUTRDY_MASK                     0x8UL                                      /**< Bit mask for HFXO_BUFOUTRDY                 */
#define _HFXO_STATUS_BUFOUTRDY_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_BUFOUTRDY_DEFAULT                   (_HFXO_STATUS_BUFOUTRDY_DEFAULT << 3)      /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_BUFOUTFROZEN                        (0x1UL << 15)                              /**< BUFOUT Frozen                               */
#define _HFXO_STATUS_BUFOUTFROZEN_SHIFT                 15                                         /**< Shift value for HFXO_BUFOUTFROZEN           */
#define _HFXO_STATUS_BUFOUTFROZEN_MASK                  0x8000UL                                   /**< Bit mask for HFXO_BUFOUTFROZEN              */
#define _HFXO_STATUS_BUFOUTFROZEN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_BUFOUTFROZEN_DEFAULT                (_HFXO_STATUS_BUFOUTFROZEN_DEFAULT << 15)  /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_ENS                                 (0x1UL << 16)                              /**< Enabled Status                              */
#define _HFXO_STATUS_ENS_SHIFT                          16                                         /**< Shift value for HFXO_ENS                    */
#define _HFXO_STATUS_ENS_MASK                           0x10000UL                                  /**< Bit mask for HFXO_ENS                       */
#define _HFXO_STATUS_ENS_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_ENS_DEFAULT                         (_HFXO_STATUS_ENS_DEFAULT << 16)           /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_HWREQ                               (0x1UL << 17)                              /**< Oscillator Requested by Digital Clock       */
#define _HFXO_STATUS_HWREQ_SHIFT                        17                                         /**< Shift value for HFXO_HWREQ                  */
#define _HFXO_STATUS_HWREQ_MASK                         0x20000UL                                  /**< Bit mask for HFXO_HWREQ                     */
#define _HFXO_STATUS_HWREQ_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_HWREQ_DEFAULT                       (_HFXO_STATUS_HWREQ_DEFAULT << 17)         /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_ISWARM                              (0x1UL << 19)                              /**< Oscillator Is Kept Warm                     */
#define _HFXO_STATUS_ISWARM_SHIFT                       19                                         /**< Shift value for HFXO_ISWARM                 */
#define _HFXO_STATUS_ISWARM_MASK                        0x80000UL                                  /**< Bit mask for HFXO_ISWARM                    */
#define _HFXO_STATUS_ISWARM_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_ISWARM_DEFAULT                      (_HFXO_STATUS_ISWARM_DEFAULT << 19)        /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_PRSHWREQ                            (0x1UL << 20)                              /**< Oscillator Requested by PRS Request         */
#define _HFXO_STATUS_PRSHWREQ_SHIFT                     20                                         /**< Shift value for HFXO_PRSHWREQ               */
#define _HFXO_STATUS_PRSHWREQ_MASK                      0x100000UL                                 /**< Bit mask for HFXO_PRSHWREQ                  */
#define _HFXO_STATUS_PRSHWREQ_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_PRSHWREQ_DEFAULT                    (_HFXO_STATUS_PRSHWREQ_DEFAULT << 20)      /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_BUFOUTHWREQ                         (0x1UL << 21)                              /**< Oscillator Requested by BUFOUT Request      */
#define _HFXO_STATUS_BUFOUTHWREQ_SHIFT                  21                                         /**< Shift value for HFXO_BUFOUTHWREQ            */
#define _HFXO_STATUS_BUFOUTHWREQ_MASK                   0x200000UL                                 /**< Bit mask for HFXO_BUFOUTHWREQ               */
#define _HFXO_STATUS_BUFOUTHWREQ_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_BUFOUTHWREQ_DEFAULT                 (_HFXO_STATUS_BUFOUTHWREQ_DEFAULT << 21)   /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_SYNCBUSY                            (0x1UL << 30)                              /**< Sync Busy                                   */
#define _HFXO_STATUS_SYNCBUSY_SHIFT                     30                                         /**< Shift value for HFXO_SYNCBUSY               */
#define _HFXO_STATUS_SYNCBUSY_MASK                      0x40000000UL                               /**< Bit mask for HFXO_SYNCBUSY                  */
#define _HFXO_STATUS_SYNCBUSY_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_SYNCBUSY_DEFAULT                    (_HFXO_STATUS_SYNCBUSY_DEFAULT << 30)      /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_LOCK                                (0x1UL << 31)                              /**< Configuration Lock Status                   */
#define _HFXO_STATUS_LOCK_SHIFT                         31                                         /**< Shift value for HFXO_LOCK                   */
#define _HFXO_STATUS_LOCK_MASK                          0x80000000UL                               /**< Bit mask for HFXO_LOCK                      */
#define _HFXO_STATUS_LOCK_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define _HFXO_STATUS_LOCK_UNLOCKED                      0x00000000UL                               /**< Mode UNLOCKED for HFXO_STATUS               */
#define _HFXO_STATUS_LOCK_LOCKED                        0x00000001UL                               /**< Mode LOCKED for HFXO_STATUS                 */
#define HFXO_STATUS_LOCK_DEFAULT                        (_HFXO_STATUS_LOCK_DEFAULT << 31)          /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_LOCK_UNLOCKED                       (_HFXO_STATUS_LOCK_UNLOCKED << 31)         /**< Shifted mode UNLOCKED for HFXO_STATUS       */
#define HFXO_STATUS_LOCK_LOCKED                         (_HFXO_STATUS_LOCK_LOCKED << 31)           /**< Shifted mode LOCKED for HFXO_STATUS         */

/* Bit fields for HFXO IF */
#define _HFXO_IF_RESETVALUE                             0x00000000UL                             /**< Default value for HFXO_IF                   */
#define _HFXO_IF_MASK                                   0xF830800FUL                             /**< Mask for HFXO_IF                            */
#define HFXO_IF_RDY                                     (0x1UL << 0)                             /**< Digital Clock Ready Interrupt               */
#define _HFXO_IF_RDY_SHIFT                              0                                        /**< Shift value for HFXO_RDY                    */
#define _HFXO_IF_RDY_MASK                               0x1UL                                    /**< Bit mask for HFXO_RDY                       */
#define _HFXO_IF_RDY_DEFAULT                            0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_RDY_DEFAULT                             (_HFXO_IF_RDY_DEFAULT << 0)              /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_COREBIASOPTRDY                          (0x1UL << 1)                             /**< Core Bias Optimization Ready Interrupt      */
#define _HFXO_IF_COREBIASOPTRDY_SHIFT                   1                                        /**< Shift value for HFXO_COREBIASOPTRDY         */
#define _HFXO_IF_COREBIASOPTRDY_MASK                    0x2UL                                    /**< Bit mask for HFXO_COREBIASOPTRDY            */
#define _HFXO_IF_COREBIASOPTRDY_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_COREBIASOPTRDY_DEFAULT                  (_HFXO_IF_COREBIASOPTRDY_DEFAULT << 1)   /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_PRSRDY                                  (0x1UL << 2)                             /**< PRS Ready Interrupt                         */
#define _HFXO_IF_PRSRDY_SHIFT                           2                                        /**< Shift value for HFXO_PRSRDY                 */
#define _HFXO_IF_PRSRDY_MASK                            0x4UL                                    /**< Bit mask for HFXO_PRSRDY                    */
#define _HFXO_IF_PRSRDY_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_PRSRDY_DEFAULT                          (_HFXO_IF_PRSRDY_DEFAULT << 2)           /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_BUFOUTRDY                               (0x1UL << 3)                             /**< BUFOUT Ready Interrupt                      */
#define _HFXO_IF_BUFOUTRDY_SHIFT                        3                                        /**< Shift value for HFXO_BUFOUTRDY              */
#define _HFXO_IF_BUFOUTRDY_MASK                         0x8UL                                    /**< Bit mask for HFXO_BUFOUTRDY                 */
#define _HFXO_IF_BUFOUTRDY_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_BUFOUTRDY_DEFAULT                       (_HFXO_IF_BUFOUTRDY_DEFAULT << 3)        /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_BUFOUTFROZEN                            (0x1UL << 15)                            /**< BUFOUT FROZEN Interrupt                     */
#define _HFXO_IF_BUFOUTFROZEN_SHIFT                     15                                       /**< Shift value for HFXO_BUFOUTFROZEN           */
#define _HFXO_IF_BUFOUTFROZEN_MASK                      0x8000UL                                 /**< Bit mask for HFXO_BUFOUTFROZEN              */
#define _HFXO_IF_BUFOUTFROZEN_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_BUFOUTFROZEN_DEFAULT                    (_HFXO_IF_BUFOUTFROZEN_DEFAULT << 15)    /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_PRSERR                                  (0x1UL << 20)                            /**< PRS Requset Error Interrupt                 */
#define _HFXO_IF_PRSERR_SHIFT                           20                                       /**< Shift value for HFXO_PRSERR                 */
#define _HFXO_IF_PRSERR_MASK                            0x100000UL                               /**< Bit mask for HFXO_PRSERR                    */
#define _HFXO_IF_PRSERR_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_PRSERR_DEFAULT                          (_HFXO_IF_PRSERR_DEFAULT << 20)          /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_BUFOUTERR                               (0x1UL << 21)                            /**< BUFOUT Request Error Interrupt              */
#define _HFXO_IF_BUFOUTERR_SHIFT                        21                                       /**< Shift value for HFXO_BUFOUTERR              */
#define _HFXO_IF_BUFOUTERR_MASK                         0x200000UL                               /**< Bit mask for HFXO_BUFOUTERR                 */
#define _HFXO_IF_BUFOUTERR_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_BUFOUTERR_DEFAULT                       (_HFXO_IF_BUFOUTERR_DEFAULT << 21)       /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_BUFOUTFREEZEERR                         (0x1UL << 27)                            /**< BUFOUT Freeze Error Interrupt               */
#define _HFXO_IF_BUFOUTFREEZEERR_SHIFT                  27                                       /**< Shift value for HFXO_BUFOUTFREEZEERR        */
#define _HFXO_IF_BUFOUTFREEZEERR_MASK                   0x8000000UL                              /**< Bit mask for HFXO_BUFOUTFREEZEERR           */
#define _HFXO_IF_BUFOUTFREEZEERR_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_BUFOUTFREEZEERR_DEFAULT                 (_HFXO_IF_BUFOUTFREEZEERR_DEFAULT << 27) /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_BUFOUTDNSERR                            (0x1UL << 28)                            /**< BUFOUT Did Not Start Error Interrupt        */
#define _HFXO_IF_BUFOUTDNSERR_SHIFT                     28                                       /**< Shift value for HFXO_BUFOUTDNSERR           */
#define _HFXO_IF_BUFOUTDNSERR_MASK                      0x10000000UL                             /**< Bit mask for HFXO_BUFOUTDNSERR              */
#define _HFXO_IF_BUFOUTDNSERR_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_BUFOUTDNSERR_DEFAULT                    (_HFXO_IF_BUFOUTDNSERR_DEFAULT << 28)    /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_DNSERR                                  (0x1UL << 29)                            /**< Did Not Start Error Interrupt               */
#define _HFXO_IF_DNSERR_SHIFT                           29                                       /**< Shift value for HFXO_DNSERR                 */
#define _HFXO_IF_DNSERR_MASK                            0x20000000UL                             /**< Bit mask for HFXO_DNSERR                    */
#define _HFXO_IF_DNSERR_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_DNSERR_DEFAULT                          (_HFXO_IF_DNSERR_DEFAULT << 29)          /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_LFTIMEOUTERR                            (0x1UL << 30)                            /**< Low Frequency Timeout Error Interrupt       */
#define _HFXO_IF_LFTIMEOUTERR_SHIFT                     30                                       /**< Shift value for HFXO_LFTIMEOUTERR           */
#define _HFXO_IF_LFTIMEOUTERR_MASK                      0x40000000UL                             /**< Bit mask for HFXO_LFTIMEOUTERR              */
#define _HFXO_IF_LFTIMEOUTERR_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_LFTIMEOUTERR_DEFAULT                    (_HFXO_IF_LFTIMEOUTERR_DEFAULT << 30)    /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_COREBIASOPTERR                          (0x1UL << 31)                            /**< Core Bias Optimization Error Interrupt      */
#define _HFXO_IF_COREBIASOPTERR_SHIFT                   31                                       /**< Shift value for HFXO_COREBIASOPTERR         */
#define _HFXO_IF_COREBIASOPTERR_MASK                    0x80000000UL                             /**< Bit mask for HFXO_COREBIASOPTERR            */
#define _HFXO_IF_COREBIASOPTERR_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_COREBIASOPTERR_DEFAULT                  (_HFXO_IF_COREBIASOPTERR_DEFAULT << 31)  /**< Shifted mode DEFAULT for HFXO_IF            */

/* Bit fields for HFXO IEN */
#define _HFXO_IEN_RESETVALUE                            0x00000000UL                              /**< Default value for HFXO_IEN                  */
#define _HFXO_IEN_MASK                                  0xF830800FUL                              /**< Mask for HFXO_IEN                           */
#define HFXO_IEN_RDY                                    (0x1UL << 0)                              /**< Digital Clock Ready Interrupt               */
#define _HFXO_IEN_RDY_SHIFT                             0                                         /**< Shift value for HFXO_RDY                    */
#define _HFXO_IEN_RDY_MASK                              0x1UL                                     /**< Bit mask for HFXO_RDY                       */
#define _HFXO_IEN_RDY_DEFAULT                           0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_RDY_DEFAULT                            (_HFXO_IEN_RDY_DEFAULT << 0)              /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_COREBIASOPTRDY                         (0x1UL << 1)                              /**< Core Bias Optimization Ready Interrupt      */
#define _HFXO_IEN_COREBIASOPTRDY_SHIFT                  1                                         /**< Shift value for HFXO_COREBIASOPTRDY         */
#define _HFXO_IEN_COREBIASOPTRDY_MASK                   0x2UL                                     /**< Bit mask for HFXO_COREBIASOPTRDY            */
#define _HFXO_IEN_COREBIASOPTRDY_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_COREBIASOPTRDY_DEFAULT                 (_HFXO_IEN_COREBIASOPTRDY_DEFAULT << 1)   /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_PRSRDY                                 (0x1UL << 2)                              /**< PRS Ready Interrupt                         */
#define _HFXO_IEN_PRSRDY_SHIFT                          2                                         /**< Shift value for HFXO_PRSRDY                 */
#define _HFXO_IEN_PRSRDY_MASK                           0x4UL                                     /**< Bit mask for HFXO_PRSRDY                    */
#define _HFXO_IEN_PRSRDY_DEFAULT                        0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_PRSRDY_DEFAULT                         (_HFXO_IEN_PRSRDY_DEFAULT << 2)           /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_BUFOUTRDY                              (0x1UL << 3)                              /**< BUFOUT Ready Interrupt                      */
#define _HFXO_IEN_BUFOUTRDY_SHIFT                       3                                         /**< Shift value for HFXO_BUFOUTRDY              */
#define _HFXO_IEN_BUFOUTRDY_MASK                        0x8UL                                     /**< Bit mask for HFXO_BUFOUTRDY                 */
#define _HFXO_IEN_BUFOUTRDY_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_BUFOUTRDY_DEFAULT                      (_HFXO_IEN_BUFOUTRDY_DEFAULT << 3)        /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_BUFOUTFROZEN                           (0x1UL << 15)                             /**< BUFOUT FROZEN Interrupt                     */
#define _HFXO_IEN_BUFOUTFROZEN_SHIFT                    15                                        /**< Shift value for HFXO_BUFOUTFROZEN           */
#define _HFXO_IEN_BUFOUTFROZEN_MASK                     0x8000UL                                  /**< Bit mask for HFXO_BUFOUTFROZEN              */
#define _HFXO_IEN_BUFOUTFROZEN_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_BUFOUTFROZEN_DEFAULT                   (_HFXO_IEN_BUFOUTFROZEN_DEFAULT << 15)    /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_PRSERR                                 (0x1UL << 20)                             /**< PRS Requset Error Interrupt                 */
#define _HFXO_IEN_PRSERR_SHIFT                          20                                        /**< Shift value for HFXO_PRSERR                 */
#define _HFXO_IEN_PRSERR_MASK                           0x100000UL                                /**< Bit mask for HFXO_PRSERR                    */
#define _HFXO_IEN_PRSERR_DEFAULT                        0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_PRSERR_DEFAULT                         (_HFXO_IEN_PRSERR_DEFAULT << 20)          /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_BUFOUTERR                              (0x1UL << 21)                             /**< BUFOUT Request Error Interrupt              */
#define _HFXO_IEN_BUFOUTERR_SHIFT                       21                                        /**< Shift value for HFXO_BUFOUTERR              */
#define _HFXO_IEN_BUFOUTERR_MASK                        0x200000UL                                /**< Bit mask for HFXO_BUFOUTERR                 */
#define _HFXO_IEN_BUFOUTERR_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_BUFOUTERR_DEFAULT                      (_HFXO_IEN_BUFOUTERR_DEFAULT << 21)       /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_BUFOUTFREEZEERR                        (0x1UL << 27)                             /**< BUFOUT Freeze Error Interrupt               */
#define _HFXO_IEN_BUFOUTFREEZEERR_SHIFT                 27                                        /**< Shift value for HFXO_BUFOUTFREEZEERR        */
#define _HFXO_IEN_BUFOUTFREEZEERR_MASK                  0x8000000UL                               /**< Bit mask for HFXO_BUFOUTFREEZEERR           */
#define _HFXO_IEN_BUFOUTFREEZEERR_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_BUFOUTFREEZEERR_DEFAULT                (_HFXO_IEN_BUFOUTFREEZEERR_DEFAULT << 27) /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_BUFOUTDNSERR                           (0x1UL << 28)                             /**< BUFOUT Did Not Start Error Interrupt        */
#define _HFXO_IEN_BUFOUTDNSERR_SHIFT                    28                                        /**< Shift value for HFXO_BUFOUTDNSERR           */
#define _HFXO_IEN_BUFOUTDNSERR_MASK                     0x10000000UL                              /**< Bit mask for HFXO_BUFOUTDNSERR              */
#define _HFXO_IEN_BUFOUTDNSERR_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_BUFOUTDNSERR_DEFAULT                   (_HFXO_IEN_BUFOUTDNSERR_DEFAULT << 28)    /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_DNSERR                                 (0x1UL << 29)                             /**< Did Not Start Error Interrupt               */
#define _HFXO_IEN_DNSERR_SHIFT                          29                                        /**< Shift value for HFXO_DNSERR                 */
#define _HFXO_IEN_DNSERR_MASK                           0x20000000UL                              /**< Bit mask for HFXO_DNSERR                    */
#define _HFXO_IEN_DNSERR_DEFAULT                        0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_DNSERR_DEFAULT                         (_HFXO_IEN_DNSERR_DEFAULT << 29)          /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_LFTIMEOUTERR                           (0x1UL << 30)                             /**< Low Frequency Timeout Error Interrupt       */
#define _HFXO_IEN_LFTIMEOUTERR_SHIFT                    30                                        /**< Shift value for HFXO_LFTIMEOUTERR           */
#define _HFXO_IEN_LFTIMEOUTERR_MASK                     0x40000000UL                              /**< Bit mask for HFXO_LFTIMEOUTERR              */
#define _HFXO_IEN_LFTIMEOUTERR_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_LFTIMEOUTERR_DEFAULT                   (_HFXO_IEN_LFTIMEOUTERR_DEFAULT << 30)    /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_COREBIASOPTERR                         (0x1UL << 31)                             /**< Core Bias Optimization Error Interrupt      */
#define _HFXO_IEN_COREBIASOPTERR_SHIFT                  31                                        /**< Shift value for HFXO_COREBIASOPTERR         */
#define _HFXO_IEN_COREBIASOPTERR_MASK                   0x80000000UL                              /**< Bit mask for HFXO_COREBIASOPTERR            */
#define _HFXO_IEN_COREBIASOPTERR_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_COREBIASOPTERR_DEFAULT                 (_HFXO_IEN_COREBIASOPTERR_DEFAULT << 31)  /**< Shifted mode DEFAULT for HFXO_IEN           */

/* Bit fields for HFXO LOCK */
#define _HFXO_LOCK_RESETVALUE                           0x0000580EUL                      /**< Default value for HFXO_LOCK                 */
#define _HFXO_LOCK_MASK                                 0x0000FFFFUL                      /**< Mask for HFXO_LOCK                          */
#define _HFXO_LOCK_LOCKKEY_SHIFT                        0                                 /**< Shift value for HFXO_LOCKKEY                */
#define _HFXO_LOCK_LOCKKEY_MASK                         0xFFFFUL                          /**< Bit mask for HFXO_LOCKKEY                   */
#define _HFXO_LOCK_LOCKKEY_DEFAULT                      0x0000580EUL                      /**< Mode DEFAULT for HFXO_LOCK                  */
#define _HFXO_LOCK_LOCKKEY_UNLOCK                       0x0000580EUL                      /**< Mode UNLOCK for HFXO_LOCK                   */
#define HFXO_LOCK_LOCKKEY_DEFAULT                       (_HFXO_LOCK_LOCKKEY_DEFAULT << 0) /**< Shifted mode DEFAULT for HFXO_LOCK          */
#define HFXO_LOCK_LOCKKEY_UNLOCK                        (_HFXO_LOCK_LOCKKEY_UNLOCK << 0)  /**< Shifted mode UNLOCK for HFXO_LOCK           */

/** @} End of group EFR32ZG23_HFXO_BitFields */
/** @} End of group EFR32ZG23_HFXO */
/** @} End of group Parts */

#endif /* EFR32ZG23_HFXO_H */
#ifdef __cplusplus
}
#endif

