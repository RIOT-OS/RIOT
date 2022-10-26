/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 PCNT register and bit field definitions
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

#ifndef EFR32ZG23_PCNT_H
#define EFR32ZG23_PCNT_H
#define PCNT_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_PCNT PCNT
 * @{
 * @brief EFR32ZG23 PCNT Register Declaration.
 *****************************************************************************/

/** PCNT Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  __IOM uint32_t EN;                            /**< Module Enable Register                             */
  __IOM uint32_t SWRST;                         /**< Software Reset Register                            */
  __IOM uint32_t CFG;                           /**< Configuration Register                             */
  __IOM uint32_t CTRL;                          /**< Control Register                                   */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  __IM uint32_t  CNT;                           /**< Counter Value Register                             */
  __IM uint32_t  AUXCNT;                        /**< Auxiliary Counter Value Register                   */
  __IOM uint32_t TOP;                           /**< Top Value Register                                 */
  __IOM uint32_t TOPB;                          /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t OVSCTRL;                       /**< Oversampling Control Register                      */
  __IM uint32_t  SYNCBUSY;                      /**< Synchronization Busy Register                      */
  __IOM uint32_t LOCK;                          /**< Configuration Lock Register                        */
  uint32_t       RESERVED0[1008U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  __IOM uint32_t EN_SET;                        /**< Module Enable Register                             */
  __IOM uint32_t SWRST_SET;                     /**< Software Reset Register                            */
  __IOM uint32_t CFG_SET;                       /**< Configuration Register                             */
  __IOM uint32_t CTRL_SET;                      /**< Control Register                                   */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  CNT_SET;                       /**< Counter Value Register                             */
  __IM uint32_t  AUXCNT_SET;                    /**< Auxiliary Counter Value Register                   */
  __IOM uint32_t TOP_SET;                       /**< Top Value Register                                 */
  __IOM uint32_t TOPB_SET;                      /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t OVSCTRL_SET;                   /**< Oversampling Control Register                      */
  __IM uint32_t  SYNCBUSY_SET;                  /**< Synchronization Busy Register                      */
  __IOM uint32_t LOCK_SET;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED1[1008U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  __IOM uint32_t EN_CLR;                        /**< Module Enable Register                             */
  __IOM uint32_t SWRST_CLR;                     /**< Software Reset Register                            */
  __IOM uint32_t CFG_CLR;                       /**< Configuration Register                             */
  __IOM uint32_t CTRL_CLR;                      /**< Control Register                                   */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  CNT_CLR;                       /**< Counter Value Register                             */
  __IM uint32_t  AUXCNT_CLR;                    /**< Auxiliary Counter Value Register                   */
  __IOM uint32_t TOP_CLR;                       /**< Top Value Register                                 */
  __IOM uint32_t TOPB_CLR;                      /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t OVSCTRL_CLR;                   /**< Oversampling Control Register                      */
  __IM uint32_t  SYNCBUSY_CLR;                  /**< Synchronization Busy Register                      */
  __IOM uint32_t LOCK_CLR;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED2[1008U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  __IOM uint32_t EN_TGL;                        /**< Module Enable Register                             */
  __IOM uint32_t SWRST_TGL;                     /**< Software Reset Register                            */
  __IOM uint32_t CFG_TGL;                       /**< Configuration Register                             */
  __IOM uint32_t CTRL_TGL;                      /**< Control Register                                   */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  CNT_TGL;                       /**< Counter Value Register                             */
  __IM uint32_t  AUXCNT_TGL;                    /**< Auxiliary Counter Value Register                   */
  __IOM uint32_t TOP_TGL;                       /**< Top Value Register                                 */
  __IOM uint32_t TOPB_TGL;                      /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t OVSCTRL_TGL;                   /**< Oversampling Control Register                      */
  __IM uint32_t  SYNCBUSY_TGL;                  /**< Synchronization Busy Register                      */
  __IOM uint32_t LOCK_TGL;                      /**< Configuration Lock Register                        */
} PCNT_TypeDef;
/** @} End of group EFR32ZG23_PCNT */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_PCNT
 * @{
 * @defgroup EFR32ZG23_PCNT_BitFields PCNT Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for PCNT IPVERSION */
#define _PCNT_IPVERSION_RESETVALUE              0x00000001UL                             /**< Default value for PCNT_IPVERSION            */
#define _PCNT_IPVERSION_MASK                    0xFFFFFFFFUL                             /**< Mask for PCNT_IPVERSION                     */
#define _PCNT_IPVERSION_IPVERSION_SHIFT         0                                        /**< Shift value for PCNT_IPVERSION              */
#define _PCNT_IPVERSION_IPVERSION_MASK          0xFFFFFFFFUL                             /**< Bit mask for PCNT_IPVERSION                 */
#define _PCNT_IPVERSION_IPVERSION_DEFAULT       0x00000001UL                             /**< Mode DEFAULT for PCNT_IPVERSION             */
#define PCNT_IPVERSION_IPVERSION_DEFAULT        (_PCNT_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for PCNT_IPVERSION     */

/* Bit fields for PCNT EN */
#define _PCNT_EN_RESETVALUE                     0x00000000UL                            /**< Default value for PCNT_EN                   */
#define _PCNT_EN_MASK                           0x00000003UL                            /**< Mask for PCNT_EN                            */
#define PCNT_EN_EN                              (0x1UL << 0)                            /**< PCNT Module Enable                          */
#define _PCNT_EN_EN_SHIFT                       0                                       /**< Shift value for PCNT_EN                     */
#define _PCNT_EN_EN_MASK                        0x1UL                                   /**< Bit mask for PCNT_EN                        */
#define _PCNT_EN_EN_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for PCNT_EN                    */
#define PCNT_EN_EN_DEFAULT                      (_PCNT_EN_EN_DEFAULT << 0)              /**< Shifted mode DEFAULT for PCNT_EN            */
#define PCNT_EN_DISABLING                       (0x1UL << 1)                            /**< Disablement busy status                     */
#define _PCNT_EN_DISABLING_SHIFT                1                                       /**< Shift value for PCNT_DISABLING              */
#define _PCNT_EN_DISABLING_MASK                 0x2UL                                   /**< Bit mask for PCNT_DISABLING                 */
#define _PCNT_EN_DISABLING_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for PCNT_EN                    */
#define PCNT_EN_DISABLING_DEFAULT               (_PCNT_EN_DISABLING_DEFAULT << 1)       /**< Shifted mode DEFAULT for PCNT_EN            */

/* Bit fields for PCNT SWRST */
#define _PCNT_SWRST_RESETVALUE                  0x00000000UL                            /**< Default value for PCNT_SWRST                */
#define _PCNT_SWRST_MASK                        0x00000003UL                            /**< Mask for PCNT_SWRST                         */
#define PCNT_SWRST_SWRST                        (0x1UL << 0)                            /**< Software reset command                      */
#define _PCNT_SWRST_SWRST_SHIFT                 0                                       /**< Shift value for PCNT_SWRST                  */
#define _PCNT_SWRST_SWRST_MASK                  0x1UL                                   /**< Bit mask for PCNT_SWRST                     */
#define _PCNT_SWRST_SWRST_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for PCNT_SWRST                 */
#define PCNT_SWRST_SWRST_DEFAULT                (_PCNT_SWRST_SWRST_DEFAULT << 0)        /**< Shifted mode DEFAULT for PCNT_SWRST         */
#define PCNT_SWRST_RESETTING                    (0x1UL << 1)                            /**< Software reset busy status                  */
#define _PCNT_SWRST_RESETTING_SHIFT             1                                       /**< Shift value for PCNT_RESETTING              */
#define _PCNT_SWRST_RESETTING_MASK              0x2UL                                   /**< Bit mask for PCNT_RESETTING                 */
#define _PCNT_SWRST_RESETTING_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for PCNT_SWRST                 */
#define PCNT_SWRST_RESETTING_DEFAULT            (_PCNT_SWRST_RESETTING_DEFAULT << 1)    /**< Shifted mode DEFAULT for PCNT_SWRST         */

/* Bit fields for PCNT CFG */
#define _PCNT_CFG_RESETVALUE                    0x00000000UL                            /**< Default value for PCNT_CFG                  */
#define _PCNT_CFG_MASK                          0x00000377UL                            /**< Mask for PCNT_CFG                           */
#define _PCNT_CFG_MODE_SHIFT                    0                                       /**< Shift value for PCNT_MODE                   */
#define _PCNT_CFG_MODE_MASK                     0x7UL                                   /**< Bit mask for PCNT_MODE                      */
#define _PCNT_CFG_MODE_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for PCNT_CFG                   */
#define _PCNT_CFG_MODE_OVSSINGLE                0x00000000UL                            /**< Mode OVSSINGLE for PCNT_CFG                 */
#define _PCNT_CFG_MODE_EXTCLKSINGLE             0x00000001UL                            /**< Mode EXTCLKSINGLE for PCNT_CFG              */
#define _PCNT_CFG_MODE_EXTCLKQUAD               0x00000002UL                            /**< Mode EXTCLKQUAD for PCNT_CFG                */
#define _PCNT_CFG_MODE_OVSQUAD1X                0x00000003UL                            /**< Mode OVSQUAD1X for PCNT_CFG                 */
#define _PCNT_CFG_MODE_OVSQUAD2X                0x00000004UL                            /**< Mode OVSQUAD2X for PCNT_CFG                 */
#define _PCNT_CFG_MODE_OVSQUAD4X                0x00000005UL                            /**< Mode OVSQUAD4X for PCNT_CFG                 */
#define PCNT_CFG_MODE_DEFAULT                   (_PCNT_CFG_MODE_DEFAULT << 0)           /**< Shifted mode DEFAULT for PCNT_CFG           */
#define PCNT_CFG_MODE_OVSSINGLE                 (_PCNT_CFG_MODE_OVSSINGLE << 0)         /**< Shifted mode OVSSINGLE for PCNT_CFG         */
#define PCNT_CFG_MODE_EXTCLKSINGLE              (_PCNT_CFG_MODE_EXTCLKSINGLE << 0)      /**< Shifted mode EXTCLKSINGLE for PCNT_CFG      */
#define PCNT_CFG_MODE_EXTCLKQUAD                (_PCNT_CFG_MODE_EXTCLKQUAD << 0)        /**< Shifted mode EXTCLKQUAD for PCNT_CFG        */
#define PCNT_CFG_MODE_OVSQUAD1X                 (_PCNT_CFG_MODE_OVSQUAD1X << 0)         /**< Shifted mode OVSQUAD1X for PCNT_CFG         */
#define PCNT_CFG_MODE_OVSQUAD2X                 (_PCNT_CFG_MODE_OVSQUAD2X << 0)         /**< Shifted mode OVSQUAD2X for PCNT_CFG         */
#define PCNT_CFG_MODE_OVSQUAD4X                 (_PCNT_CFG_MODE_OVSQUAD4X << 0)         /**< Shifted mode OVSQUAD4X for PCNT_CFG         */
#define PCNT_CFG_DEBUGHALT                      (0x1UL << 4)                            /**< Debug Mode Halt Enable                      */
#define _PCNT_CFG_DEBUGHALT_SHIFT               4                                       /**< Shift value for PCNT_DEBUGHALT              */
#define _PCNT_CFG_DEBUGHALT_MASK                0x10UL                                  /**< Bit mask for PCNT_DEBUGHALT                 */
#define _PCNT_CFG_DEBUGHALT_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for PCNT_CFG                   */
#define _PCNT_CFG_DEBUGHALT_DISABLE             0x00000000UL                            /**< Mode DISABLE for PCNT_CFG                   */
#define _PCNT_CFG_DEBUGHALT_ENABLE              0x00000001UL                            /**< Mode ENABLE for PCNT_CFG                    */
#define PCNT_CFG_DEBUGHALT_DEFAULT              (_PCNT_CFG_DEBUGHALT_DEFAULT << 4)      /**< Shifted mode DEFAULT for PCNT_CFG           */
#define PCNT_CFG_DEBUGHALT_DISABLE              (_PCNT_CFG_DEBUGHALT_DISABLE << 4)      /**< Shifted mode DISABLE for PCNT_CFG           */
#define PCNT_CFG_DEBUGHALT_ENABLE               (_PCNT_CFG_DEBUGHALT_ENABLE << 4)       /**< Shifted mode ENABLE for PCNT_CFG            */
#define PCNT_CFG_FILTEN                         (0x1UL << 5)                            /**< Enable Digital Pulse Width Filter           */
#define _PCNT_CFG_FILTEN_SHIFT                  5                                       /**< Shift value for PCNT_FILTEN                 */
#define _PCNT_CFG_FILTEN_MASK                   0x20UL                                  /**< Bit mask for PCNT_FILTEN                    */
#define _PCNT_CFG_FILTEN_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for PCNT_CFG                   */
#define PCNT_CFG_FILTEN_DEFAULT                 (_PCNT_CFG_FILTEN_DEFAULT << 5)         /**< Shifted mode DEFAULT for PCNT_CFG           */
#define PCNT_CFG_HYST                           (0x1UL << 6)                            /**< Enable Hysteresis                           */
#define _PCNT_CFG_HYST_SHIFT                    6                                       /**< Shift value for PCNT_HYST                   */
#define _PCNT_CFG_HYST_MASK                     0x40UL                                  /**< Bit mask for PCNT_HYST                      */
#define _PCNT_CFG_HYST_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for PCNT_CFG                   */
#define PCNT_CFG_HYST_DEFAULT                   (_PCNT_CFG_HYST_DEFAULT << 6)           /**< Shifted mode DEFAULT for PCNT_CFG           */
#define PCNT_CFG_S0PRSEN                        (0x1UL << 8)                            /**< S0IN PRS Enable                             */
#define _PCNT_CFG_S0PRSEN_SHIFT                 8                                       /**< Shift value for PCNT_S0PRSEN                */
#define _PCNT_CFG_S0PRSEN_MASK                  0x100UL                                 /**< Bit mask for PCNT_S0PRSEN                   */
#define _PCNT_CFG_S0PRSEN_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for PCNT_CFG                   */
#define PCNT_CFG_S0PRSEN_DEFAULT                (_PCNT_CFG_S0PRSEN_DEFAULT << 8)        /**< Shifted mode DEFAULT for PCNT_CFG           */
#define PCNT_CFG_S1PRSEN                        (0x1UL << 9)                            /**< S1IN PRS Enable                             */
#define _PCNT_CFG_S1PRSEN_SHIFT                 9                                       /**< Shift value for PCNT_S1PRSEN                */
#define _PCNT_CFG_S1PRSEN_MASK                  0x200UL                                 /**< Bit mask for PCNT_S1PRSEN                   */
#define _PCNT_CFG_S1PRSEN_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for PCNT_CFG                   */
#define PCNT_CFG_S1PRSEN_DEFAULT                (_PCNT_CFG_S1PRSEN_DEFAULT << 9)        /**< Shifted mode DEFAULT for PCNT_CFG           */

/* Bit fields for PCNT CTRL */
#define _PCNT_CTRL_RESETVALUE                   0x00000000UL                            /**< Default value for PCNT_CTRL                 */
#define _PCNT_CTRL_MASK                         0x000000F7UL                            /**< Mask for PCNT_CTRL                          */
#define PCNT_CTRL_S1CDIR                        (0x1UL << 0)                            /**< Count Direction Determined By S1            */
#define _PCNT_CTRL_S1CDIR_SHIFT                 0                                       /**< Shift value for PCNT_S1CDIR                 */
#define _PCNT_CTRL_S1CDIR_MASK                  0x1UL                                   /**< Bit mask for PCNT_S1CDIR                    */
#define _PCNT_CTRL_S1CDIR_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for PCNT_CTRL                  */
#define PCNT_CTRL_S1CDIR_DEFAULT                (_PCNT_CTRL_S1CDIR_DEFAULT << 0)        /**< Shifted mode DEFAULT for PCNT_CTRL          */
#define PCNT_CTRL_CNTDIR                        (0x1UL << 1)                            /**< Non-Quadrature Mode Counter Direction Co    */
#define _PCNT_CTRL_CNTDIR_SHIFT                 1                                       /**< Shift value for PCNT_CNTDIR                 */
#define _PCNT_CTRL_CNTDIR_MASK                  0x2UL                                   /**< Bit mask for PCNT_CNTDIR                    */
#define _PCNT_CTRL_CNTDIR_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for PCNT_CTRL                  */
#define _PCNT_CTRL_CNTDIR_UP                    0x00000000UL                            /**< Mode UP for PCNT_CTRL                       */
#define _PCNT_CTRL_CNTDIR_DOWN                  0x00000001UL                            /**< Mode DOWN for PCNT_CTRL                     */
#define PCNT_CTRL_CNTDIR_DEFAULT                (_PCNT_CTRL_CNTDIR_DEFAULT << 1)        /**< Shifted mode DEFAULT for PCNT_CTRL          */
#define PCNT_CTRL_CNTDIR_UP                     (_PCNT_CTRL_CNTDIR_UP << 1)             /**< Shifted mode UP for PCNT_CTRL               */
#define PCNT_CTRL_CNTDIR_DOWN                   (_PCNT_CTRL_CNTDIR_DOWN << 1)           /**< Shifted mode DOWN for PCNT_CTRL             */
#define PCNT_CTRL_EDGE                          (0x1UL << 2)                            /**< Edge Select                                 */
#define _PCNT_CTRL_EDGE_SHIFT                   2                                       /**< Shift value for PCNT_EDGE                   */
#define _PCNT_CTRL_EDGE_MASK                    0x4UL                                   /**< Bit mask for PCNT_EDGE                      */
#define _PCNT_CTRL_EDGE_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for PCNT_CTRL                  */
#define _PCNT_CTRL_EDGE_POS                     0x00000000UL                            /**< Mode POS for PCNT_CTRL                      */
#define _PCNT_CTRL_EDGE_NEG                     0x00000001UL                            /**< Mode NEG for PCNT_CTRL                      */
#define PCNT_CTRL_EDGE_DEFAULT                  (_PCNT_CTRL_EDGE_DEFAULT << 2)          /**< Shifted mode DEFAULT for PCNT_CTRL          */
#define PCNT_CTRL_EDGE_POS                      (_PCNT_CTRL_EDGE_POS << 2)              /**< Shifted mode POS for PCNT_CTRL              */
#define PCNT_CTRL_EDGE_NEG                      (_PCNT_CTRL_EDGE_NEG << 2)              /**< Shifted mode NEG for PCNT_CTRL              */
#define _PCNT_CTRL_CNTEV_SHIFT                  4                                       /**< Shift value for PCNT_CNTEV                  */
#define _PCNT_CTRL_CNTEV_MASK                   0x30UL                                  /**< Bit mask for PCNT_CNTEV                     */
#define _PCNT_CTRL_CNTEV_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for PCNT_CTRL                  */
#define _PCNT_CTRL_CNTEV_BOTH                   0x00000000UL                            /**< Mode BOTH for PCNT_CTRL                     */
#define _PCNT_CTRL_CNTEV_UP                     0x00000001UL                            /**< Mode UP for PCNT_CTRL                       */
#define _PCNT_CTRL_CNTEV_DOWN                   0x00000002UL                            /**< Mode DOWN for PCNT_CTRL                     */
#define PCNT_CTRL_CNTEV_DEFAULT                 (_PCNT_CTRL_CNTEV_DEFAULT << 4)         /**< Shifted mode DEFAULT for PCNT_CTRL          */
#define PCNT_CTRL_CNTEV_BOTH                    (_PCNT_CTRL_CNTEV_BOTH << 4)            /**< Shifted mode BOTH for PCNT_CTRL             */
#define PCNT_CTRL_CNTEV_UP                      (_PCNT_CTRL_CNTEV_UP << 4)              /**< Shifted mode UP for PCNT_CTRL               */
#define PCNT_CTRL_CNTEV_DOWN                    (_PCNT_CTRL_CNTEV_DOWN << 4)            /**< Shifted mode DOWN for PCNT_CTRL             */
#define _PCNT_CTRL_AUXCNTEV_SHIFT               6                                       /**< Shift value for PCNT_AUXCNTEV               */
#define _PCNT_CTRL_AUXCNTEV_MASK                0xC0UL                                  /**< Bit mask for PCNT_AUXCNTEV                  */
#define _PCNT_CTRL_AUXCNTEV_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for PCNT_CTRL                  */
#define _PCNT_CTRL_AUXCNTEV_BOTH                0x00000000UL                            /**< Mode BOTH for PCNT_CTRL                     */
#define _PCNT_CTRL_AUXCNTEV_UP                  0x00000001UL                            /**< Mode UP for PCNT_CTRL                       */
#define _PCNT_CTRL_AUXCNTEV_DOWN                0x00000002UL                            /**< Mode DOWN for PCNT_CTRL                     */
#define PCNT_CTRL_AUXCNTEV_DEFAULT              (_PCNT_CTRL_AUXCNTEV_DEFAULT << 6)      /**< Shifted mode DEFAULT for PCNT_CTRL          */
#define PCNT_CTRL_AUXCNTEV_BOTH                 (_PCNT_CTRL_AUXCNTEV_BOTH << 6)         /**< Shifted mode BOTH for PCNT_CTRL             */
#define PCNT_CTRL_AUXCNTEV_UP                   (_PCNT_CTRL_AUXCNTEV_UP << 6)           /**< Shifted mode UP for PCNT_CTRL               */
#define PCNT_CTRL_AUXCNTEV_DOWN                 (_PCNT_CTRL_AUXCNTEV_DOWN << 6)         /**< Shifted mode DOWN for PCNT_CTRL             */

/* Bit fields for PCNT CMD */
#define _PCNT_CMD_RESETVALUE                    0x00000000UL                            /**< Default value for PCNT_CMD                  */
#define _PCNT_CMD_MASK                          0x00000F17UL                            /**< Mask for PCNT_CMD                           */
#define PCNT_CMD_CORERST                        (0x1UL << 0)                            /**< PCNT Clock Domain Reset                     */
#define _PCNT_CMD_CORERST_SHIFT                 0                                       /**< Shift value for PCNT_CORERST                */
#define _PCNT_CMD_CORERST_MASK                  0x1UL                                   /**< Bit mask for PCNT_CORERST                   */
#define _PCNT_CMD_CORERST_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for PCNT_CMD                   */
#define PCNT_CMD_CORERST_DEFAULT                (_PCNT_CMD_CORERST_DEFAULT << 0)        /**< Shifted mode DEFAULT for PCNT_CMD           */
#define PCNT_CMD_CNTRST                         (0x1UL << 1)                            /**< CNT Reset                                   */
#define _PCNT_CMD_CNTRST_SHIFT                  1                                       /**< Shift value for PCNT_CNTRST                 */
#define _PCNT_CMD_CNTRST_MASK                   0x2UL                                   /**< Bit mask for PCNT_CNTRST                    */
#define _PCNT_CMD_CNTRST_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for PCNT_CMD                   */
#define PCNT_CMD_CNTRST_DEFAULT                 (_PCNT_CMD_CNTRST_DEFAULT << 1)         /**< Shifted mode DEFAULT for PCNT_CMD           */
#define PCNT_CMD_AUXCNTRST                      (0x1UL << 2)                            /**< AUXCNT Reset                                */
#define _PCNT_CMD_AUXCNTRST_SHIFT               2                                       /**< Shift value for PCNT_AUXCNTRST              */
#define _PCNT_CMD_AUXCNTRST_MASK                0x4UL                                   /**< Bit mask for PCNT_AUXCNTRST                 */
#define _PCNT_CMD_AUXCNTRST_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for PCNT_CMD                   */
#define PCNT_CMD_AUXCNTRST_DEFAULT              (_PCNT_CMD_AUXCNTRST_DEFAULT << 2)      /**< Shifted mode DEFAULT for PCNT_CMD           */
#define PCNT_CMD_LCNTIM                         (0x1UL << 4)                            /**< Load CNT Immediately                        */
#define _PCNT_CMD_LCNTIM_SHIFT                  4                                       /**< Shift value for PCNT_LCNTIM                 */
#define _PCNT_CMD_LCNTIM_MASK                   0x10UL                                  /**< Bit mask for PCNT_LCNTIM                    */
#define _PCNT_CMD_LCNTIM_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for PCNT_CMD                   */
#define PCNT_CMD_LCNTIM_DEFAULT                 (_PCNT_CMD_LCNTIM_DEFAULT << 4)         /**< Shifted mode DEFAULT for PCNT_CMD           */
#define PCNT_CMD_STARTCNT                       (0x1UL << 8)                            /**< Start Main Counter                          */
#define _PCNT_CMD_STARTCNT_SHIFT                8                                       /**< Shift value for PCNT_STARTCNT               */
#define _PCNT_CMD_STARTCNT_MASK                 0x100UL                                 /**< Bit mask for PCNT_STARTCNT                  */
#define _PCNT_CMD_STARTCNT_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for PCNT_CMD                   */
#define PCNT_CMD_STARTCNT_DEFAULT               (_PCNT_CMD_STARTCNT_DEFAULT << 8)       /**< Shifted mode DEFAULT for PCNT_CMD           */
#define PCNT_CMD_STARTAUXCNT                    (0x1UL << 9)                            /**< Start Aux Counter                           */
#define _PCNT_CMD_STARTAUXCNT_SHIFT             9                                       /**< Shift value for PCNT_STARTAUXCNT            */
#define _PCNT_CMD_STARTAUXCNT_MASK              0x200UL                                 /**< Bit mask for PCNT_STARTAUXCNT               */
#define _PCNT_CMD_STARTAUXCNT_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for PCNT_CMD                   */
#define PCNT_CMD_STARTAUXCNT_DEFAULT            (_PCNT_CMD_STARTAUXCNT_DEFAULT << 9)    /**< Shifted mode DEFAULT for PCNT_CMD           */
#define PCNT_CMD_STOPCNT                        (0x1UL << 10)                           /**< Stop Main Counter                           */
#define _PCNT_CMD_STOPCNT_SHIFT                 10                                      /**< Shift value for PCNT_STOPCNT                */
#define _PCNT_CMD_STOPCNT_MASK                  0x400UL                                 /**< Bit mask for PCNT_STOPCNT                   */
#define _PCNT_CMD_STOPCNT_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for PCNT_CMD                   */
#define PCNT_CMD_STOPCNT_DEFAULT                (_PCNT_CMD_STOPCNT_DEFAULT << 10)       /**< Shifted mode DEFAULT for PCNT_CMD           */
#define PCNT_CMD_STOPAUXCNT                     (0x1UL << 11)                           /**< Stop Aux Counter                            */
#define _PCNT_CMD_STOPAUXCNT_SHIFT              11                                      /**< Shift value for PCNT_STOPAUXCNT             */
#define _PCNT_CMD_STOPAUXCNT_MASK               0x800UL                                 /**< Bit mask for PCNT_STOPAUXCNT                */
#define _PCNT_CMD_STOPAUXCNT_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for PCNT_CMD                   */
#define PCNT_CMD_STOPAUXCNT_DEFAULT             (_PCNT_CMD_STOPAUXCNT_DEFAULT << 11)    /**< Shifted mode DEFAULT for PCNT_CMD           */

/* Bit fields for PCNT STATUS */
#define _PCNT_STATUS_RESETVALUE                 0x00000000UL                                /**< Default value for PCNT_STATUS               */
#define _PCNT_STATUS_MASK                       0x0000001FUL                                /**< Mask for PCNT_STATUS                        */
#define PCNT_STATUS_DIR                         (0x1UL << 0)                                /**< Current Counter Direction                   */
#define _PCNT_STATUS_DIR_SHIFT                  0                                           /**< Shift value for PCNT_DIR                    */
#define _PCNT_STATUS_DIR_MASK                   0x1UL                                       /**< Bit mask for PCNT_DIR                       */
#define _PCNT_STATUS_DIR_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for PCNT_STATUS                */
#define _PCNT_STATUS_DIR_UP                     0x00000000UL                                /**< Mode UP for PCNT_STATUS                     */
#define _PCNT_STATUS_DIR_DOWN                   0x00000001UL                                /**< Mode DOWN for PCNT_STATUS                   */
#define PCNT_STATUS_DIR_DEFAULT                 (_PCNT_STATUS_DIR_DEFAULT << 0)             /**< Shifted mode DEFAULT for PCNT_STATUS        */
#define PCNT_STATUS_DIR_UP                      (_PCNT_STATUS_DIR_UP << 0)                  /**< Shifted mode UP for PCNT_STATUS             */
#define PCNT_STATUS_DIR_DOWN                    (_PCNT_STATUS_DIR_DOWN << 0)                /**< Shifted mode DOWN for PCNT_STATUS           */
#define PCNT_STATUS_TOPBV                       (0x1UL << 1)                                /**< TOP Buffer Valid                            */
#define _PCNT_STATUS_TOPBV_SHIFT                1                                           /**< Shift value for PCNT_TOPBV                  */
#define _PCNT_STATUS_TOPBV_MASK                 0x2UL                                       /**< Bit mask for PCNT_TOPBV                     */
#define _PCNT_STATUS_TOPBV_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for PCNT_STATUS                */
#define PCNT_STATUS_TOPBV_DEFAULT               (_PCNT_STATUS_TOPBV_DEFAULT << 1)           /**< Shifted mode DEFAULT for PCNT_STATUS        */
#define PCNT_STATUS_PCNTLOCKSTATUS              (0x1UL << 2)                                /**< Lock Status                                 */
#define _PCNT_STATUS_PCNTLOCKSTATUS_SHIFT       2                                           /**< Shift value for PCNT_PCNTLOCKSTATUS         */
#define _PCNT_STATUS_PCNTLOCKSTATUS_MASK        0x4UL                                       /**< Bit mask for PCNT_PCNTLOCKSTATUS            */
#define _PCNT_STATUS_PCNTLOCKSTATUS_DEFAULT     0x00000000UL                                /**< Mode DEFAULT for PCNT_STATUS                */
#define _PCNT_STATUS_PCNTLOCKSTATUS_UNLOCKED    0x00000000UL                                /**< Mode UNLOCKED for PCNT_STATUS               */
#define _PCNT_STATUS_PCNTLOCKSTATUS_LOCKED      0x00000001UL                                /**< Mode LOCKED for PCNT_STATUS                 */
#define PCNT_STATUS_PCNTLOCKSTATUS_DEFAULT      (_PCNT_STATUS_PCNTLOCKSTATUS_DEFAULT << 2)  /**< Shifted mode DEFAULT for PCNT_STATUS        */
#define PCNT_STATUS_PCNTLOCKSTATUS_UNLOCKED     (_PCNT_STATUS_PCNTLOCKSTATUS_UNLOCKED << 2) /**< Shifted mode UNLOCKED for PCNT_STATUS       */
#define PCNT_STATUS_PCNTLOCKSTATUS_LOCKED       (_PCNT_STATUS_PCNTLOCKSTATUS_LOCKED << 2)   /**< Shifted mode LOCKED for PCNT_STATUS         */
#define PCNT_STATUS_CNTRUNNING                  (0x1UL << 3)                                /**< Main Counter running status                 */
#define _PCNT_STATUS_CNTRUNNING_SHIFT           3                                           /**< Shift value for PCNT_CNTRUNNING             */
#define _PCNT_STATUS_CNTRUNNING_MASK            0x8UL                                       /**< Bit mask for PCNT_CNTRUNNING                */
#define _PCNT_STATUS_CNTRUNNING_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for PCNT_STATUS                */
#define PCNT_STATUS_CNTRUNNING_DEFAULT          (_PCNT_STATUS_CNTRUNNING_DEFAULT << 3)      /**< Shifted mode DEFAULT for PCNT_STATUS        */
#define PCNT_STATUS_AUXCNTRUNNING               (0x1UL << 4)                                /**< Aux Counter running status                  */
#define _PCNT_STATUS_AUXCNTRUNNING_SHIFT        4                                           /**< Shift value for PCNT_AUXCNTRUNNING          */
#define _PCNT_STATUS_AUXCNTRUNNING_MASK         0x10UL                                      /**< Bit mask for PCNT_AUXCNTRUNNING             */
#define _PCNT_STATUS_AUXCNTRUNNING_DEFAULT      0x00000000UL                                /**< Mode DEFAULT for PCNT_STATUS                */
#define PCNT_STATUS_AUXCNTRUNNING_DEFAULT       (_PCNT_STATUS_AUXCNTRUNNING_DEFAULT << 4)   /**< Shifted mode DEFAULT for PCNT_STATUS        */

/* Bit fields for PCNT IF */
#define _PCNT_IF_RESETVALUE                     0x00000000UL                            /**< Default value for PCNT_IF                   */
#define _PCNT_IF_MASK                           0x0000001FUL                            /**< Mask for PCNT_IF                            */
#define PCNT_IF_UF                              (0x1UL << 0)                            /**< Underflow Interrupt Read Flag               */
#define _PCNT_IF_UF_SHIFT                       0                                       /**< Shift value for PCNT_UF                     */
#define _PCNT_IF_UF_MASK                        0x1UL                                   /**< Bit mask for PCNT_UF                        */
#define _PCNT_IF_UF_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for PCNT_IF                    */
#define PCNT_IF_UF_DEFAULT                      (_PCNT_IF_UF_DEFAULT << 0)              /**< Shifted mode DEFAULT for PCNT_IF            */
#define PCNT_IF_OF                              (0x1UL << 1)                            /**< Overflow Interrupt Read Flag                */
#define _PCNT_IF_OF_SHIFT                       1                                       /**< Shift value for PCNT_OF                     */
#define _PCNT_IF_OF_MASK                        0x2UL                                   /**< Bit mask for PCNT_OF                        */
#define _PCNT_IF_OF_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for PCNT_IF                    */
#define PCNT_IF_OF_DEFAULT                      (_PCNT_IF_OF_DEFAULT << 1)              /**< Shifted mode DEFAULT for PCNT_IF            */
#define PCNT_IF_DIRCNG                          (0x1UL << 2)                            /**< Direction Change Detect Interrupt Flag      */
#define _PCNT_IF_DIRCNG_SHIFT                   2                                       /**< Shift value for PCNT_DIRCNG                 */
#define _PCNT_IF_DIRCNG_MASK                    0x4UL                                   /**< Bit mask for PCNT_DIRCNG                    */
#define _PCNT_IF_DIRCNG_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for PCNT_IF                    */
#define PCNT_IF_DIRCNG_DEFAULT                  (_PCNT_IF_DIRCNG_DEFAULT << 2)          /**< Shifted mode DEFAULT for PCNT_IF            */
#define PCNT_IF_AUXOF                           (0x1UL << 3)                            /**< Auxiliary Overflow Interrupt Read Flag      */
#define _PCNT_IF_AUXOF_SHIFT                    3                                       /**< Shift value for PCNT_AUXOF                  */
#define _PCNT_IF_AUXOF_MASK                     0x8UL                                   /**< Bit mask for PCNT_AUXOF                     */
#define _PCNT_IF_AUXOF_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for PCNT_IF                    */
#define PCNT_IF_AUXOF_DEFAULT                   (_PCNT_IF_AUXOF_DEFAULT << 3)           /**< Shifted mode DEFAULT for PCNT_IF            */
#define PCNT_IF_OQSTERR                         (0x1UL << 4)                            /**< Oversampling Quad State Err Int Flag        */
#define _PCNT_IF_OQSTERR_SHIFT                  4                                       /**< Shift value for PCNT_OQSTERR                */
#define _PCNT_IF_OQSTERR_MASK                   0x10UL                                  /**< Bit mask for PCNT_OQSTERR                   */
#define _PCNT_IF_OQSTERR_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for PCNT_IF                    */
#define PCNT_IF_OQSTERR_DEFAULT                 (_PCNT_IF_OQSTERR_DEFAULT << 4)         /**< Shifted mode DEFAULT for PCNT_IF            */

/* Bit fields for PCNT IEN */
#define _PCNT_IEN_RESETVALUE                    0x00000000UL                            /**< Default value for PCNT_IEN                  */
#define _PCNT_IEN_MASK                          0x0000001FUL                            /**< Mask for PCNT_IEN                           */
#define PCNT_IEN_UF                             (0x1UL << 0)                            /**< Underflow Interrupt Read Flag               */
#define _PCNT_IEN_UF_SHIFT                      0                                       /**< Shift value for PCNT_UF                     */
#define _PCNT_IEN_UF_MASK                       0x1UL                                   /**< Bit mask for PCNT_UF                        */
#define _PCNT_IEN_UF_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for PCNT_IEN                   */
#define PCNT_IEN_UF_DEFAULT                     (_PCNT_IEN_UF_DEFAULT << 0)             /**< Shifted mode DEFAULT for PCNT_IEN           */
#define PCNT_IEN_OF                             (0x1UL << 1)                            /**< Overflow Interrupt Read Flag                */
#define _PCNT_IEN_OF_SHIFT                      1                                       /**< Shift value for PCNT_OF                     */
#define _PCNT_IEN_OF_MASK                       0x2UL                                   /**< Bit mask for PCNT_OF                        */
#define _PCNT_IEN_OF_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for PCNT_IEN                   */
#define PCNT_IEN_OF_DEFAULT                     (_PCNT_IEN_OF_DEFAULT << 1)             /**< Shifted mode DEFAULT for PCNT_IEN           */
#define PCNT_IEN_DIRCNG                         (0x1UL << 2)                            /**< Direction Change Detect Interrupt Flag      */
#define _PCNT_IEN_DIRCNG_SHIFT                  2                                       /**< Shift value for PCNT_DIRCNG                 */
#define _PCNT_IEN_DIRCNG_MASK                   0x4UL                                   /**< Bit mask for PCNT_DIRCNG                    */
#define _PCNT_IEN_DIRCNG_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for PCNT_IEN                   */
#define PCNT_IEN_DIRCNG_DEFAULT                 (_PCNT_IEN_DIRCNG_DEFAULT << 2)         /**< Shifted mode DEFAULT for PCNT_IEN           */
#define PCNT_IEN_AUXOF                          (0x1UL << 3)                            /**< Auxiliary Overflow Interrupt Read Flag      */
#define _PCNT_IEN_AUXOF_SHIFT                   3                                       /**< Shift value for PCNT_AUXOF                  */
#define _PCNT_IEN_AUXOF_MASK                    0x8UL                                   /**< Bit mask for PCNT_AUXOF                     */
#define _PCNT_IEN_AUXOF_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for PCNT_IEN                   */
#define PCNT_IEN_AUXOF_DEFAULT                  (_PCNT_IEN_AUXOF_DEFAULT << 3)          /**< Shifted mode DEFAULT for PCNT_IEN           */
#define PCNT_IEN_OQSTERR                        (0x1UL << 4)                            /**< Oversampling Quad State Err Int Flag        */
#define _PCNT_IEN_OQSTERR_SHIFT                 4                                       /**< Shift value for PCNT_OQSTERR                */
#define _PCNT_IEN_OQSTERR_MASK                  0x10UL                                  /**< Bit mask for PCNT_OQSTERR                   */
#define _PCNT_IEN_OQSTERR_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for PCNT_IEN                   */
#define PCNT_IEN_OQSTERR_DEFAULT                (_PCNT_IEN_OQSTERR_DEFAULT << 4)        /**< Shifted mode DEFAULT for PCNT_IEN           */

/* Bit fields for PCNT CNT */
#define _PCNT_CNT_RESETVALUE                    0x00000000UL                            /**< Default value for PCNT_CNT                  */
#define _PCNT_CNT_MASK                          0x0000FFFFUL                            /**< Mask for PCNT_CNT                           */
#define _PCNT_CNT_CNT_SHIFT                     0                                       /**< Shift value for PCNT_CNT                    */
#define _PCNT_CNT_CNT_MASK                      0xFFFFUL                                /**< Bit mask for PCNT_CNT                       */
#define _PCNT_CNT_CNT_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for PCNT_CNT                   */
#define PCNT_CNT_CNT_DEFAULT                    (_PCNT_CNT_CNT_DEFAULT << 0)            /**< Shifted mode DEFAULT for PCNT_CNT           */

/* Bit fields for PCNT AUXCNT */
#define _PCNT_AUXCNT_RESETVALUE                 0x00000000UL                            /**< Default value for PCNT_AUXCNT               */
#define _PCNT_AUXCNT_MASK                       0x0000FFFFUL                            /**< Mask for PCNT_AUXCNT                        */
#define _PCNT_AUXCNT_AUXCNT_SHIFT               0                                       /**< Shift value for PCNT_AUXCNT                 */
#define _PCNT_AUXCNT_AUXCNT_MASK                0xFFFFUL                                /**< Bit mask for PCNT_AUXCNT                    */
#define _PCNT_AUXCNT_AUXCNT_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for PCNT_AUXCNT                */
#define PCNT_AUXCNT_AUXCNT_DEFAULT              (_PCNT_AUXCNT_AUXCNT_DEFAULT << 0)      /**< Shifted mode DEFAULT for PCNT_AUXCNT        */

/* Bit fields for PCNT TOP */
#define _PCNT_TOP_RESETVALUE                    0x000000FFUL                            /**< Default value for PCNT_TOP                  */
#define _PCNT_TOP_MASK                          0x0000FFFFUL                            /**< Mask for PCNT_TOP                           */
#define _PCNT_TOP_TOP_SHIFT                     0                                       /**< Shift value for PCNT_TOP                    */
#define _PCNT_TOP_TOP_MASK                      0xFFFFUL                                /**< Bit mask for PCNT_TOP                       */
#define _PCNT_TOP_TOP_DEFAULT                   0x000000FFUL                            /**< Mode DEFAULT for PCNT_TOP                   */
#define PCNT_TOP_TOP_DEFAULT                    (_PCNT_TOP_TOP_DEFAULT << 0)            /**< Shifted mode DEFAULT for PCNT_TOP           */

/* Bit fields for PCNT TOPB */
#define _PCNT_TOPB_RESETVALUE                   0x000000FFUL                            /**< Default value for PCNT_TOPB                 */
#define _PCNT_TOPB_MASK                         0x0000FFFFUL                            /**< Mask for PCNT_TOPB                          */
#define _PCNT_TOPB_TOPB_SHIFT                   0                                       /**< Shift value for PCNT_TOPB                   */
#define _PCNT_TOPB_TOPB_MASK                    0xFFFFUL                                /**< Bit mask for PCNT_TOPB                      */
#define _PCNT_TOPB_TOPB_DEFAULT                 0x000000FFUL                            /**< Mode DEFAULT for PCNT_TOPB                  */
#define PCNT_TOPB_TOPB_DEFAULT                  (_PCNT_TOPB_TOPB_DEFAULT << 0)          /**< Shifted mode DEFAULT for PCNT_TOPB          */

/* Bit fields for PCNT OVSCTRL */
#define _PCNT_OVSCTRL_RESETVALUE                0x00000000UL                            /**< Default value for PCNT_OVSCTRL              */
#define _PCNT_OVSCTRL_MASK                      0x000010FFUL                            /**< Mask for PCNT_OVSCTRL                       */
#define _PCNT_OVSCTRL_FILTLEN_SHIFT             0                                       /**< Shift value for PCNT_FILTLEN                */
#define _PCNT_OVSCTRL_FILTLEN_MASK              0xFFUL                                  /**< Bit mask for PCNT_FILTLEN                   */
#define _PCNT_OVSCTRL_FILTLEN_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for PCNT_OVSCTRL               */
#define PCNT_OVSCTRL_FILTLEN_DEFAULT            (_PCNT_OVSCTRL_FILTLEN_DEFAULT << 0)    /**< Shifted mode DEFAULT for PCNT_OVSCTRL       */
#define PCNT_OVSCTRL_FLUTTERRM                  (0x1UL << 12)                           /**< Flutter Remove                              */
#define _PCNT_OVSCTRL_FLUTTERRM_SHIFT           12                                      /**< Shift value for PCNT_FLUTTERRM              */
#define _PCNT_OVSCTRL_FLUTTERRM_MASK            0x1000UL                                /**< Bit mask for PCNT_FLUTTERRM                 */
#define _PCNT_OVSCTRL_FLUTTERRM_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for PCNT_OVSCTRL               */
#define PCNT_OVSCTRL_FLUTTERRM_DEFAULT          (_PCNT_OVSCTRL_FLUTTERRM_DEFAULT << 12) /**< Shifted mode DEFAULT for PCNT_OVSCTRL       */

/* Bit fields for PCNT SYNCBUSY */
#define _PCNT_SYNCBUSY_RESETVALUE               0x00000000UL                            /**< Default value for PCNT_SYNCBUSY             */
#define _PCNT_SYNCBUSY_MASK                     0x0000001FUL                            /**< Mask for PCNT_SYNCBUSY                      */
#define PCNT_SYNCBUSY_CTRL                      (0x1UL << 0)                            /**< CTRL Register Busy                          */
#define _PCNT_SYNCBUSY_CTRL_SHIFT               0                                       /**< Shift value for PCNT_CTRL                   */
#define _PCNT_SYNCBUSY_CTRL_MASK                0x1UL                                   /**< Bit mask for PCNT_CTRL                      */
#define _PCNT_SYNCBUSY_CTRL_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for PCNT_SYNCBUSY              */
#define PCNT_SYNCBUSY_CTRL_DEFAULT              (_PCNT_SYNCBUSY_CTRL_DEFAULT << 0)      /**< Shifted mode DEFAULT for PCNT_SYNCBUSY      */
#define PCNT_SYNCBUSY_CMD                       (0x1UL << 1)                            /**< CMD Register Busy                           */
#define _PCNT_SYNCBUSY_CMD_SHIFT                1                                       /**< Shift value for PCNT_CMD                    */
#define _PCNT_SYNCBUSY_CMD_MASK                 0x2UL                                   /**< Bit mask for PCNT_CMD                       */
#define _PCNT_SYNCBUSY_CMD_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for PCNT_SYNCBUSY              */
#define PCNT_SYNCBUSY_CMD_DEFAULT               (_PCNT_SYNCBUSY_CMD_DEFAULT << 1)       /**< Shifted mode DEFAULT for PCNT_SYNCBUSY      */
#define PCNT_SYNCBUSY_TOP                       (0x1UL << 2)                            /**< TOP Register Busy                           */
#define _PCNT_SYNCBUSY_TOP_SHIFT                2                                       /**< Shift value for PCNT_TOP                    */
#define _PCNT_SYNCBUSY_TOP_MASK                 0x4UL                                   /**< Bit mask for PCNT_TOP                       */
#define _PCNT_SYNCBUSY_TOP_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for PCNT_SYNCBUSY              */
#define PCNT_SYNCBUSY_TOP_DEFAULT               (_PCNT_SYNCBUSY_TOP_DEFAULT << 2)       /**< Shifted mode DEFAULT for PCNT_SYNCBUSY      */
#define PCNT_SYNCBUSY_TOPB                      (0x1UL << 3)                            /**< TOPB Register Busy                          */
#define _PCNT_SYNCBUSY_TOPB_SHIFT               3                                       /**< Shift value for PCNT_TOPB                   */
#define _PCNT_SYNCBUSY_TOPB_MASK                0x8UL                                   /**< Bit mask for PCNT_TOPB                      */
#define _PCNT_SYNCBUSY_TOPB_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for PCNT_SYNCBUSY              */
#define PCNT_SYNCBUSY_TOPB_DEFAULT              (_PCNT_SYNCBUSY_TOPB_DEFAULT << 3)      /**< Shifted mode DEFAULT for PCNT_SYNCBUSY      */
#define PCNT_SYNCBUSY_OVSCTRL                   (0x1UL << 4)                            /**< OVSCTRL Register Busy                       */
#define _PCNT_SYNCBUSY_OVSCTRL_SHIFT            4                                       /**< Shift value for PCNT_OVSCTRL                */
#define _PCNT_SYNCBUSY_OVSCTRL_MASK             0x10UL                                  /**< Bit mask for PCNT_OVSCTRL                   */
#define _PCNT_SYNCBUSY_OVSCTRL_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for PCNT_SYNCBUSY              */
#define PCNT_SYNCBUSY_OVSCTRL_DEFAULT           (_PCNT_SYNCBUSY_OVSCTRL_DEFAULT << 4)   /**< Shifted mode DEFAULT for PCNT_SYNCBUSY      */

/* Bit fields for PCNT LOCK */
#define _PCNT_LOCK_RESETVALUE                   0x00000000UL                            /**< Default value for PCNT_LOCK                 */
#define _PCNT_LOCK_MASK                         0x0000FFFFUL                            /**< Mask for PCNT_LOCK                          */
#define _PCNT_LOCK_PCNTLOCKKEY_SHIFT            0                                       /**< Shift value for PCNT_PCNTLOCKKEY            */
#define _PCNT_LOCK_PCNTLOCKKEY_MASK             0xFFFFUL                                /**< Bit mask for PCNT_PCNTLOCKKEY               */
#define _PCNT_LOCK_PCNTLOCKKEY_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for PCNT_LOCK                  */
#define _PCNT_LOCK_PCNTLOCKKEY_UNLOCK           0x0000A7E0UL                            /**< Mode UNLOCK for PCNT_LOCK                   */
#define PCNT_LOCK_PCNTLOCKKEY_DEFAULT           (_PCNT_LOCK_PCNTLOCKKEY_DEFAULT << 0)   /**< Shifted mode DEFAULT for PCNT_LOCK          */
#define PCNT_LOCK_PCNTLOCKKEY_UNLOCK            (_PCNT_LOCK_PCNTLOCKKEY_UNLOCK << 0)    /**< Shifted mode UNLOCK for PCNT_LOCK           */

/** @} End of group EFR32ZG23_PCNT_BitFields */
/** @} End of group EFR32ZG23_PCNT */
/** @} End of group Parts */

#endif /* EFR32ZG23_PCNT_H */
#ifdef __cplusplus
}
#endif

