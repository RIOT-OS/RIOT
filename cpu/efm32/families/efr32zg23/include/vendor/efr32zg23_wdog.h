/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 WDOG register and bit field definitions
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

#ifndef EFR32ZG23_WDOG_H
#define EFR32ZG23_WDOG_H
#define WDOG_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_WDOG WDOG
 * @{
 * @brief EFR32ZG23 WDOG Register Declaration.
 *****************************************************************************/

/** WDOG Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP Version Register                                */
  __IOM uint32_t EN;                            /**< Enable Register                                    */
  __IOM uint32_t CFG;                           /**< Configuration Register                             */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  __IOM uint32_t LOCK;                          /**< Lock Register                                      */
  __IM uint32_t  SYNCBUSY;                      /**< Synchronization Busy Register                      */
  uint32_t       RESERVED1[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP Version Register                                */
  __IOM uint32_t EN_SET;                        /**< Enable Register                                    */
  __IOM uint32_t CFG_SET;                       /**< Configuration Register                             */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t LOCK_SET;                      /**< Lock Register                                      */
  __IM uint32_t  SYNCBUSY_SET;                  /**< Synchronization Busy Register                      */
  uint32_t       RESERVED3[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP Version Register                                */
  __IOM uint32_t EN_CLR;                        /**< Enable Register                                    */
  __IOM uint32_t CFG_CLR;                       /**< Configuration Register                             */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  uint32_t       RESERVED4[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t LOCK_CLR;                      /**< Lock Register                                      */
  __IM uint32_t  SYNCBUSY_CLR;                  /**< Synchronization Busy Register                      */
  uint32_t       RESERVED5[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP Version Register                                */
  __IOM uint32_t EN_TGL;                        /**< Enable Register                                    */
  __IOM uint32_t CFG_TGL;                       /**< Configuration Register                             */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t LOCK_TGL;                      /**< Lock Register                                      */
  __IM uint32_t  SYNCBUSY_TGL;                  /**< Synchronization Busy Register                      */
} WDOG_TypeDef;
/** @} End of group EFR32ZG23_WDOG */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_WDOG
 * @{
 * @defgroup EFR32ZG23_WDOG_BitFields WDOG Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for WDOG IPVERSION */
#define _WDOG_IPVERSION_RESETVALUE           0x00000001UL                               /**< Default value for WDOG_IPVERSION            */
#define _WDOG_IPVERSION_MASK                 0xFFFFFFFFUL                               /**< Mask for WDOG_IPVERSION                     */
#define _WDOG_IPVERSION_IPVERSION_SHIFT      0                                          /**< Shift value for WDOG_IPVERSION              */
#define _WDOG_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                               /**< Bit mask for WDOG_IPVERSION                 */
#define _WDOG_IPVERSION_IPVERSION_DEFAULT    0x00000001UL                               /**< Mode DEFAULT for WDOG_IPVERSION             */
#define WDOG_IPVERSION_IPVERSION_DEFAULT     (_WDOG_IPVERSION_IPVERSION_DEFAULT << 0)   /**< Shifted mode DEFAULT for WDOG_IPVERSION     */

/* Bit fields for WDOG EN */
#define _WDOG_EN_RESETVALUE                  0x00000000UL                               /**< Default value for WDOG_EN                   */
#define _WDOG_EN_MASK                        0x00000003UL                               /**< Mask for WDOG_EN                            */
#define WDOG_EN_EN                           (0x1UL << 0)                               /**< Module Enable                               */
#define _WDOG_EN_EN_SHIFT                    0                                          /**< Shift value for WDOG_EN                     */
#define _WDOG_EN_EN_MASK                     0x1UL                                      /**< Bit mask for WDOG_EN                        */
#define _WDOG_EN_EN_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for WDOG_EN                    */
#define WDOG_EN_EN_DEFAULT                   (_WDOG_EN_EN_DEFAULT << 0)                 /**< Shifted mode DEFAULT for WDOG_EN            */
#define WDOG_EN_DISABLING                    (0x1UL << 1)                               /**< Disabling busy status                       */
#define _WDOG_EN_DISABLING_SHIFT             1                                          /**< Shift value for WDOG_DISABLING              */
#define _WDOG_EN_DISABLING_MASK              0x2UL                                      /**< Bit mask for WDOG_DISABLING                 */
#define _WDOG_EN_DISABLING_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for WDOG_EN                    */
#define WDOG_EN_DISABLING_DEFAULT            (_WDOG_EN_DISABLING_DEFAULT << 1)          /**< Shifted mode DEFAULT for WDOG_EN            */

/* Bit fields for WDOG CFG */
#define _WDOG_CFG_RESETVALUE                 0x000F0000UL                               /**< Default value for WDOG_CFG                  */
#define _WDOG_CFG_MASK                       0x730F073FUL                               /**< Mask for WDOG_CFG                           */
#define WDOG_CFG_CLRSRC                      (0x1UL << 0)                               /**< WDOG Clear Source                           */
#define _WDOG_CFG_CLRSRC_SHIFT               0                                          /**< Shift value for WDOG_CLRSRC                 */
#define _WDOG_CFG_CLRSRC_MASK                0x1UL                                      /**< Bit mask for WDOG_CLRSRC                    */
#define _WDOG_CFG_CLRSRC_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define _WDOG_CFG_CLRSRC_SW                  0x00000000UL                               /**< Mode SW for WDOG_CFG                        */
#define _WDOG_CFG_CLRSRC_PRSSRC0             0x00000001UL                               /**< Mode PRSSRC0 for WDOG_CFG                   */
#define WDOG_CFG_CLRSRC_DEFAULT              (_WDOG_CFG_CLRSRC_DEFAULT << 0)            /**< Shifted mode DEFAULT for WDOG_CFG           */
#define WDOG_CFG_CLRSRC_SW                   (_WDOG_CFG_CLRSRC_SW << 0)                 /**< Shifted mode SW for WDOG_CFG                */
#define WDOG_CFG_CLRSRC_PRSSRC0              (_WDOG_CFG_CLRSRC_PRSSRC0 << 0)            /**< Shifted mode PRSSRC0 for WDOG_CFG           */
#define WDOG_CFG_EM1RUN                      (0x1UL << 1)                               /**< EM1 Run                                     */
#define _WDOG_CFG_EM1RUN_SHIFT               1                                          /**< Shift value for WDOG_EM1RUN                 */
#define _WDOG_CFG_EM1RUN_MASK                0x2UL                                      /**< Bit mask for WDOG_EM1RUN                    */
#define _WDOG_CFG_EM1RUN_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define _WDOG_CFG_EM1RUN_DISABLE             0x00000000UL                               /**< Mode DISABLE for WDOG_CFG                   */
#define _WDOG_CFG_EM1RUN_ENABLE              0x00000001UL                               /**< Mode ENABLE for WDOG_CFG                    */
#define WDOG_CFG_EM1RUN_DEFAULT              (_WDOG_CFG_EM1RUN_DEFAULT << 1)            /**< Shifted mode DEFAULT for WDOG_CFG           */
#define WDOG_CFG_EM1RUN_DISABLE              (_WDOG_CFG_EM1RUN_DISABLE << 1)            /**< Shifted mode DISABLE for WDOG_CFG           */
#define WDOG_CFG_EM1RUN_ENABLE               (_WDOG_CFG_EM1RUN_ENABLE << 1)             /**< Shifted mode ENABLE for WDOG_CFG            */
#define WDOG_CFG_EM2RUN                      (0x1UL << 2)                               /**< EM2 Run                                     */
#define _WDOG_CFG_EM2RUN_SHIFT               2                                          /**< Shift value for WDOG_EM2RUN                 */
#define _WDOG_CFG_EM2RUN_MASK                0x4UL                                      /**< Bit mask for WDOG_EM2RUN                    */
#define _WDOG_CFG_EM2RUN_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define _WDOG_CFG_EM2RUN_DISABLE             0x00000000UL                               /**< Mode DISABLE for WDOG_CFG                   */
#define _WDOG_CFG_EM2RUN_ENABLE              0x00000001UL                               /**< Mode ENABLE for WDOG_CFG                    */
#define WDOG_CFG_EM2RUN_DEFAULT              (_WDOG_CFG_EM2RUN_DEFAULT << 2)            /**< Shifted mode DEFAULT for WDOG_CFG           */
#define WDOG_CFG_EM2RUN_DISABLE              (_WDOG_CFG_EM2RUN_DISABLE << 2)            /**< Shifted mode DISABLE for WDOG_CFG           */
#define WDOG_CFG_EM2RUN_ENABLE               (_WDOG_CFG_EM2RUN_ENABLE << 2)             /**< Shifted mode ENABLE for WDOG_CFG            */
#define WDOG_CFG_EM3RUN                      (0x1UL << 3)                               /**< EM3 Run                                     */
#define _WDOG_CFG_EM3RUN_SHIFT               3                                          /**< Shift value for WDOG_EM3RUN                 */
#define _WDOG_CFG_EM3RUN_MASK                0x8UL                                      /**< Bit mask for WDOG_EM3RUN                    */
#define _WDOG_CFG_EM3RUN_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define _WDOG_CFG_EM3RUN_DISABLE             0x00000000UL                               /**< Mode DISABLE for WDOG_CFG                   */
#define _WDOG_CFG_EM3RUN_ENABLE              0x00000001UL                               /**< Mode ENABLE for WDOG_CFG                    */
#define WDOG_CFG_EM3RUN_DEFAULT              (_WDOG_CFG_EM3RUN_DEFAULT << 3)            /**< Shifted mode DEFAULT for WDOG_CFG           */
#define WDOG_CFG_EM3RUN_DISABLE              (_WDOG_CFG_EM3RUN_DISABLE << 3)            /**< Shifted mode DISABLE for WDOG_CFG           */
#define WDOG_CFG_EM3RUN_ENABLE               (_WDOG_CFG_EM3RUN_ENABLE << 3)             /**< Shifted mode ENABLE for WDOG_CFG            */
#define WDOG_CFG_EM4BLOCK                    (0x1UL << 4)                               /**< EM4 Block                                   */
#define _WDOG_CFG_EM4BLOCK_SHIFT             4                                          /**< Shift value for WDOG_EM4BLOCK               */
#define _WDOG_CFG_EM4BLOCK_MASK              0x10UL                                     /**< Bit mask for WDOG_EM4BLOCK                  */
#define _WDOG_CFG_EM4BLOCK_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define _WDOG_CFG_EM4BLOCK_DISABLE           0x00000000UL                               /**< Mode DISABLE for WDOG_CFG                   */
#define _WDOG_CFG_EM4BLOCK_ENABLE            0x00000001UL                               /**< Mode ENABLE for WDOG_CFG                    */
#define WDOG_CFG_EM4BLOCK_DEFAULT            (_WDOG_CFG_EM4BLOCK_DEFAULT << 4)          /**< Shifted mode DEFAULT for WDOG_CFG           */
#define WDOG_CFG_EM4BLOCK_DISABLE            (_WDOG_CFG_EM4BLOCK_DISABLE << 4)          /**< Shifted mode DISABLE for WDOG_CFG           */
#define WDOG_CFG_EM4BLOCK_ENABLE             (_WDOG_CFG_EM4BLOCK_ENABLE << 4)           /**< Shifted mode ENABLE for WDOG_CFG            */
#define WDOG_CFG_DEBUGRUN                    (0x1UL << 5)                               /**< Debug Mode Run                              */
#define _WDOG_CFG_DEBUGRUN_SHIFT             5                                          /**< Shift value for WDOG_DEBUGRUN               */
#define _WDOG_CFG_DEBUGRUN_MASK              0x20UL                                     /**< Bit mask for WDOG_DEBUGRUN                  */
#define _WDOG_CFG_DEBUGRUN_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define _WDOG_CFG_DEBUGRUN_DISABLE           0x00000000UL                               /**< Mode DISABLE for WDOG_CFG                   */
#define _WDOG_CFG_DEBUGRUN_ENABLE            0x00000001UL                               /**< Mode ENABLE for WDOG_CFG                    */
#define WDOG_CFG_DEBUGRUN_DEFAULT            (_WDOG_CFG_DEBUGRUN_DEFAULT << 5)          /**< Shifted mode DEFAULT for WDOG_CFG           */
#define WDOG_CFG_DEBUGRUN_DISABLE            (_WDOG_CFG_DEBUGRUN_DISABLE << 5)          /**< Shifted mode DISABLE for WDOG_CFG           */
#define WDOG_CFG_DEBUGRUN_ENABLE             (_WDOG_CFG_DEBUGRUN_ENABLE << 5)           /**< Shifted mode ENABLE for WDOG_CFG            */
#define WDOG_CFG_WDOGRSTDIS                  (0x1UL << 8)                               /**< WDOG Reset Disable                          */
#define _WDOG_CFG_WDOGRSTDIS_SHIFT           8                                          /**< Shift value for WDOG_WDOGRSTDIS             */
#define _WDOG_CFG_WDOGRSTDIS_MASK            0x100UL                                    /**< Bit mask for WDOG_WDOGRSTDIS                */
#define _WDOG_CFG_WDOGRSTDIS_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define _WDOG_CFG_WDOGRSTDIS_EN              0x00000000UL                               /**< Mode EN for WDOG_CFG                        */
#define _WDOG_CFG_WDOGRSTDIS_DIS             0x00000001UL                               /**< Mode DIS for WDOG_CFG                       */
#define WDOG_CFG_WDOGRSTDIS_DEFAULT          (_WDOG_CFG_WDOGRSTDIS_DEFAULT << 8)        /**< Shifted mode DEFAULT for WDOG_CFG           */
#define WDOG_CFG_WDOGRSTDIS_EN               (_WDOG_CFG_WDOGRSTDIS_EN << 8)             /**< Shifted mode EN for WDOG_CFG                */
#define WDOG_CFG_WDOGRSTDIS_DIS              (_WDOG_CFG_WDOGRSTDIS_DIS << 8)            /**< Shifted mode DIS for WDOG_CFG               */
#define WDOG_CFG_PRS0MISSRSTEN               (0x1UL << 9)                               /**< PRS Src0 Missing Event WDOG Reset           */
#define _WDOG_CFG_PRS0MISSRSTEN_SHIFT        9                                          /**< Shift value for WDOG_PRS0MISSRSTEN          */
#define _WDOG_CFG_PRS0MISSRSTEN_MASK         0x200UL                                    /**< Bit mask for WDOG_PRS0MISSRSTEN             */
#define _WDOG_CFG_PRS0MISSRSTEN_DEFAULT      0x00000000UL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define WDOG_CFG_PRS0MISSRSTEN_DEFAULT       (_WDOG_CFG_PRS0MISSRSTEN_DEFAULT << 9)     /**< Shifted mode DEFAULT for WDOG_CFG           */
#define WDOG_CFG_PRS1MISSRSTEN               (0x1UL << 10)                              /**< PRS Src1 Missing Event WDOG Reset           */
#define _WDOG_CFG_PRS1MISSRSTEN_SHIFT        10                                         /**< Shift value for WDOG_PRS1MISSRSTEN          */
#define _WDOG_CFG_PRS1MISSRSTEN_MASK         0x400UL                                    /**< Bit mask for WDOG_PRS1MISSRSTEN             */
#define _WDOG_CFG_PRS1MISSRSTEN_DEFAULT      0x00000000UL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define WDOG_CFG_PRS1MISSRSTEN_DEFAULT       (_WDOG_CFG_PRS1MISSRSTEN_DEFAULT << 10)    /**< Shifted mode DEFAULT for WDOG_CFG           */
#define _WDOG_CFG_PERSEL_SHIFT               16                                         /**< Shift value for WDOG_PERSEL                 */
#define _WDOG_CFG_PERSEL_MASK                0xF0000UL                                  /**< Bit mask for WDOG_PERSEL                    */
#define _WDOG_CFG_PERSEL_DEFAULT             0x0000000FUL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define _WDOG_CFG_PERSEL_SEL0                0x00000000UL                               /**< Mode SEL0 for WDOG_CFG                      */
#define _WDOG_CFG_PERSEL_SEL1                0x00000001UL                               /**< Mode SEL1 for WDOG_CFG                      */
#define _WDOG_CFG_PERSEL_SEL2                0x00000002UL                               /**< Mode SEL2 for WDOG_CFG                      */
#define _WDOG_CFG_PERSEL_SEL3                0x00000003UL                               /**< Mode SEL3 for WDOG_CFG                      */
#define _WDOG_CFG_PERSEL_SEL4                0x00000004UL                               /**< Mode SEL4 for WDOG_CFG                      */
#define _WDOG_CFG_PERSEL_SEL5                0x00000005UL                               /**< Mode SEL5 for WDOG_CFG                      */
#define _WDOG_CFG_PERSEL_SEL6                0x00000006UL                               /**< Mode SEL6 for WDOG_CFG                      */
#define _WDOG_CFG_PERSEL_SEL7                0x00000007UL                               /**< Mode SEL7 for WDOG_CFG                      */
#define _WDOG_CFG_PERSEL_SEL8                0x00000008UL                               /**< Mode SEL8 for WDOG_CFG                      */
#define _WDOG_CFG_PERSEL_SEL9                0x00000009UL                               /**< Mode SEL9 for WDOG_CFG                      */
#define _WDOG_CFG_PERSEL_SEL10               0x0000000AUL                               /**< Mode SEL10 for WDOG_CFG                     */
#define _WDOG_CFG_PERSEL_SEL11               0x0000000BUL                               /**< Mode SEL11 for WDOG_CFG                     */
#define _WDOG_CFG_PERSEL_SEL12               0x0000000CUL                               /**< Mode SEL12 for WDOG_CFG                     */
#define _WDOG_CFG_PERSEL_SEL13               0x0000000DUL                               /**< Mode SEL13 for WDOG_CFG                     */
#define _WDOG_CFG_PERSEL_SEL14               0x0000000EUL                               /**< Mode SEL14 for WDOG_CFG                     */
#define _WDOG_CFG_PERSEL_SEL15               0x0000000FUL                               /**< Mode SEL15 for WDOG_CFG                     */
#define WDOG_CFG_PERSEL_DEFAULT              (_WDOG_CFG_PERSEL_DEFAULT << 16)           /**< Shifted mode DEFAULT for WDOG_CFG           */
#define WDOG_CFG_PERSEL_SEL0                 (_WDOG_CFG_PERSEL_SEL0 << 16)              /**< Shifted mode SEL0 for WDOG_CFG              */
#define WDOG_CFG_PERSEL_SEL1                 (_WDOG_CFG_PERSEL_SEL1 << 16)              /**< Shifted mode SEL1 for WDOG_CFG              */
#define WDOG_CFG_PERSEL_SEL2                 (_WDOG_CFG_PERSEL_SEL2 << 16)              /**< Shifted mode SEL2 for WDOG_CFG              */
#define WDOG_CFG_PERSEL_SEL3                 (_WDOG_CFG_PERSEL_SEL3 << 16)              /**< Shifted mode SEL3 for WDOG_CFG              */
#define WDOG_CFG_PERSEL_SEL4                 (_WDOG_CFG_PERSEL_SEL4 << 16)              /**< Shifted mode SEL4 for WDOG_CFG              */
#define WDOG_CFG_PERSEL_SEL5                 (_WDOG_CFG_PERSEL_SEL5 << 16)              /**< Shifted mode SEL5 for WDOG_CFG              */
#define WDOG_CFG_PERSEL_SEL6                 (_WDOG_CFG_PERSEL_SEL6 << 16)              /**< Shifted mode SEL6 for WDOG_CFG              */
#define WDOG_CFG_PERSEL_SEL7                 (_WDOG_CFG_PERSEL_SEL7 << 16)              /**< Shifted mode SEL7 for WDOG_CFG              */
#define WDOG_CFG_PERSEL_SEL8                 (_WDOG_CFG_PERSEL_SEL8 << 16)              /**< Shifted mode SEL8 for WDOG_CFG              */
#define WDOG_CFG_PERSEL_SEL9                 (_WDOG_CFG_PERSEL_SEL9 << 16)              /**< Shifted mode SEL9 for WDOG_CFG              */
#define WDOG_CFG_PERSEL_SEL10                (_WDOG_CFG_PERSEL_SEL10 << 16)             /**< Shifted mode SEL10 for WDOG_CFG             */
#define WDOG_CFG_PERSEL_SEL11                (_WDOG_CFG_PERSEL_SEL11 << 16)             /**< Shifted mode SEL11 for WDOG_CFG             */
#define WDOG_CFG_PERSEL_SEL12                (_WDOG_CFG_PERSEL_SEL12 << 16)             /**< Shifted mode SEL12 for WDOG_CFG             */
#define WDOG_CFG_PERSEL_SEL13                (_WDOG_CFG_PERSEL_SEL13 << 16)             /**< Shifted mode SEL13 for WDOG_CFG             */
#define WDOG_CFG_PERSEL_SEL14                (_WDOG_CFG_PERSEL_SEL14 << 16)             /**< Shifted mode SEL14 for WDOG_CFG             */
#define WDOG_CFG_PERSEL_SEL15                (_WDOG_CFG_PERSEL_SEL15 << 16)             /**< Shifted mode SEL15 for WDOG_CFG             */
#define _WDOG_CFG_WARNSEL_SHIFT              24                                         /**< Shift value for WDOG_WARNSEL                */
#define _WDOG_CFG_WARNSEL_MASK               0x3000000UL                                /**< Bit mask for WDOG_WARNSEL                   */
#define _WDOG_CFG_WARNSEL_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define _WDOG_CFG_WARNSEL_DIS                0x00000000UL                               /**< Mode DIS for WDOG_CFG                       */
#define _WDOG_CFG_WARNSEL_SEL1               0x00000001UL                               /**< Mode SEL1 for WDOG_CFG                      */
#define _WDOG_CFG_WARNSEL_SEL2               0x00000002UL                               /**< Mode SEL2 for WDOG_CFG                      */
#define _WDOG_CFG_WARNSEL_SEL3               0x00000003UL                               /**< Mode SEL3 for WDOG_CFG                      */
#define WDOG_CFG_WARNSEL_DEFAULT             (_WDOG_CFG_WARNSEL_DEFAULT << 24)          /**< Shifted mode DEFAULT for WDOG_CFG           */
#define WDOG_CFG_WARNSEL_DIS                 (_WDOG_CFG_WARNSEL_DIS << 24)              /**< Shifted mode DIS for WDOG_CFG               */
#define WDOG_CFG_WARNSEL_SEL1                (_WDOG_CFG_WARNSEL_SEL1 << 24)             /**< Shifted mode SEL1 for WDOG_CFG              */
#define WDOG_CFG_WARNSEL_SEL2                (_WDOG_CFG_WARNSEL_SEL2 << 24)             /**< Shifted mode SEL2 for WDOG_CFG              */
#define WDOG_CFG_WARNSEL_SEL3                (_WDOG_CFG_WARNSEL_SEL3 << 24)             /**< Shifted mode SEL3 for WDOG_CFG              */
#define _WDOG_CFG_WINSEL_SHIFT               28                                         /**< Shift value for WDOG_WINSEL                 */
#define _WDOG_CFG_WINSEL_MASK                0x70000000UL                               /**< Bit mask for WDOG_WINSEL                    */
#define _WDOG_CFG_WINSEL_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for WDOG_CFG                   */
#define _WDOG_CFG_WINSEL_DIS                 0x00000000UL                               /**< Mode DIS for WDOG_CFG                       */
#define _WDOG_CFG_WINSEL_SEL1                0x00000001UL                               /**< Mode SEL1 for WDOG_CFG                      */
#define _WDOG_CFG_WINSEL_SEL2                0x00000002UL                               /**< Mode SEL2 for WDOG_CFG                      */
#define _WDOG_CFG_WINSEL_SEL3                0x00000003UL                               /**< Mode SEL3 for WDOG_CFG                      */
#define _WDOG_CFG_WINSEL_SEL4                0x00000004UL                               /**< Mode SEL4 for WDOG_CFG                      */
#define _WDOG_CFG_WINSEL_SEL5                0x00000005UL                               /**< Mode SEL5 for WDOG_CFG                      */
#define _WDOG_CFG_WINSEL_SEL6                0x00000006UL                               /**< Mode SEL6 for WDOG_CFG                      */
#define _WDOG_CFG_WINSEL_SEL7                0x00000007UL                               /**< Mode SEL7 for WDOG_CFG                      */
#define WDOG_CFG_WINSEL_DEFAULT              (_WDOG_CFG_WINSEL_DEFAULT << 28)           /**< Shifted mode DEFAULT for WDOG_CFG           */
#define WDOG_CFG_WINSEL_DIS                  (_WDOG_CFG_WINSEL_DIS << 28)               /**< Shifted mode DIS for WDOG_CFG               */
#define WDOG_CFG_WINSEL_SEL1                 (_WDOG_CFG_WINSEL_SEL1 << 28)              /**< Shifted mode SEL1 for WDOG_CFG              */
#define WDOG_CFG_WINSEL_SEL2                 (_WDOG_CFG_WINSEL_SEL2 << 28)              /**< Shifted mode SEL2 for WDOG_CFG              */
#define WDOG_CFG_WINSEL_SEL3                 (_WDOG_CFG_WINSEL_SEL3 << 28)              /**< Shifted mode SEL3 for WDOG_CFG              */
#define WDOG_CFG_WINSEL_SEL4                 (_WDOG_CFG_WINSEL_SEL4 << 28)              /**< Shifted mode SEL4 for WDOG_CFG              */
#define WDOG_CFG_WINSEL_SEL5                 (_WDOG_CFG_WINSEL_SEL5 << 28)              /**< Shifted mode SEL5 for WDOG_CFG              */
#define WDOG_CFG_WINSEL_SEL6                 (_WDOG_CFG_WINSEL_SEL6 << 28)              /**< Shifted mode SEL6 for WDOG_CFG              */
#define WDOG_CFG_WINSEL_SEL7                 (_WDOG_CFG_WINSEL_SEL7 << 28)              /**< Shifted mode SEL7 for WDOG_CFG              */

/* Bit fields for WDOG CMD */
#define _WDOG_CMD_RESETVALUE                 0x00000000UL                               /**< Default value for WDOG_CMD                  */
#define _WDOG_CMD_MASK                       0x00000001UL                               /**< Mask for WDOG_CMD                           */
#define WDOG_CMD_CLEAR                       (0x1UL << 0)                               /**< WDOG Timer Clear                            */
#define _WDOG_CMD_CLEAR_SHIFT                0                                          /**< Shift value for WDOG_CLEAR                  */
#define _WDOG_CMD_CLEAR_MASK                 0x1UL                                      /**< Bit mask for WDOG_CLEAR                     */
#define _WDOG_CMD_CLEAR_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for WDOG_CMD                   */
#define _WDOG_CMD_CLEAR_UNCHANGED            0x00000000UL                               /**< Mode UNCHANGED for WDOG_CMD                 */
#define _WDOG_CMD_CLEAR_CLEARED              0x00000001UL                               /**< Mode CLEARED for WDOG_CMD                   */
#define WDOG_CMD_CLEAR_DEFAULT               (_WDOG_CMD_CLEAR_DEFAULT << 0)             /**< Shifted mode DEFAULT for WDOG_CMD           */
#define WDOG_CMD_CLEAR_UNCHANGED             (_WDOG_CMD_CLEAR_UNCHANGED << 0)           /**< Shifted mode UNCHANGED for WDOG_CMD         */
#define WDOG_CMD_CLEAR_CLEARED               (_WDOG_CMD_CLEAR_CLEARED << 0)             /**< Shifted mode CLEARED for WDOG_CMD           */

/* Bit fields for WDOG STATUS */
#define _WDOG_STATUS_RESETVALUE              0x00000000UL                               /**< Default value for WDOG_STATUS               */
#define _WDOG_STATUS_MASK                    0x80000000UL                               /**< Mask for WDOG_STATUS                        */
#define WDOG_STATUS_LOCK                     (0x1UL << 31)                              /**< WDOG Configuration Lock Status              */
#define _WDOG_STATUS_LOCK_SHIFT              31                                         /**< Shift value for WDOG_LOCK                   */
#define _WDOG_STATUS_LOCK_MASK               0x80000000UL                               /**< Bit mask for WDOG_LOCK                      */
#define _WDOG_STATUS_LOCK_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for WDOG_STATUS                */
#define _WDOG_STATUS_LOCK_UNLOCKED           0x00000000UL                               /**< Mode UNLOCKED for WDOG_STATUS               */
#define _WDOG_STATUS_LOCK_LOCKED             0x00000001UL                               /**< Mode LOCKED for WDOG_STATUS                 */
#define WDOG_STATUS_LOCK_DEFAULT             (_WDOG_STATUS_LOCK_DEFAULT << 31)          /**< Shifted mode DEFAULT for WDOG_STATUS        */
#define WDOG_STATUS_LOCK_UNLOCKED            (_WDOG_STATUS_LOCK_UNLOCKED << 31)         /**< Shifted mode UNLOCKED for WDOG_STATUS       */
#define WDOG_STATUS_LOCK_LOCKED              (_WDOG_STATUS_LOCK_LOCKED << 31)           /**< Shifted mode LOCKED for WDOG_STATUS         */

/* Bit fields for WDOG IF */
#define _WDOG_IF_RESETVALUE                  0x00000000UL                               /**< Default value for WDOG_IF                   */
#define _WDOG_IF_MASK                        0x0000001FUL                               /**< Mask for WDOG_IF                            */
#define WDOG_IF_TOUT                         (0x1UL << 0)                               /**< WDOG Timeout Interrupt Flag                 */
#define _WDOG_IF_TOUT_SHIFT                  0                                          /**< Shift value for WDOG_TOUT                   */
#define _WDOG_IF_TOUT_MASK                   0x1UL                                      /**< Bit mask for WDOG_TOUT                      */
#define _WDOG_IF_TOUT_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for WDOG_IF                    */
#define WDOG_IF_TOUT_DEFAULT                 (_WDOG_IF_TOUT_DEFAULT << 0)               /**< Shifted mode DEFAULT for WDOG_IF            */
#define WDOG_IF_WARN                         (0x1UL << 1)                               /**< WDOG Warning Timeout Interrupt Flag         */
#define _WDOG_IF_WARN_SHIFT                  1                                          /**< Shift value for WDOG_WARN                   */
#define _WDOG_IF_WARN_MASK                   0x2UL                                      /**< Bit mask for WDOG_WARN                      */
#define _WDOG_IF_WARN_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for WDOG_IF                    */
#define WDOG_IF_WARN_DEFAULT                 (_WDOG_IF_WARN_DEFAULT << 1)               /**< Shifted mode DEFAULT for WDOG_IF            */
#define WDOG_IF_WIN                          (0x1UL << 2)                               /**< WDOG Window Interrupt Flag                  */
#define _WDOG_IF_WIN_SHIFT                   2                                          /**< Shift value for WDOG_WIN                    */
#define _WDOG_IF_WIN_MASK                    0x4UL                                      /**< Bit mask for WDOG_WIN                       */
#define _WDOG_IF_WIN_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for WDOG_IF                    */
#define WDOG_IF_WIN_DEFAULT                  (_WDOG_IF_WIN_DEFAULT << 2)                /**< Shifted mode DEFAULT for WDOG_IF            */
#define WDOG_IF_PEM0                         (0x1UL << 3)                               /**< PRS Src0 Event Missing Interrupt Flag       */
#define _WDOG_IF_PEM0_SHIFT                  3                                          /**< Shift value for WDOG_PEM0                   */
#define _WDOG_IF_PEM0_MASK                   0x8UL                                      /**< Bit mask for WDOG_PEM0                      */
#define _WDOG_IF_PEM0_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for WDOG_IF                    */
#define WDOG_IF_PEM0_DEFAULT                 (_WDOG_IF_PEM0_DEFAULT << 3)               /**< Shifted mode DEFAULT for WDOG_IF            */
#define WDOG_IF_PEM1                         (0x1UL << 4)                               /**< PRS Src1 Event Missing Interrupt Flag       */
#define _WDOG_IF_PEM1_SHIFT                  4                                          /**< Shift value for WDOG_PEM1                   */
#define _WDOG_IF_PEM1_MASK                   0x10UL                                     /**< Bit mask for WDOG_PEM1                      */
#define _WDOG_IF_PEM1_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for WDOG_IF                    */
#define WDOG_IF_PEM1_DEFAULT                 (_WDOG_IF_PEM1_DEFAULT << 4)               /**< Shifted mode DEFAULT for WDOG_IF            */

/* Bit fields for WDOG IEN */
#define _WDOG_IEN_RESETVALUE                 0x00000000UL                               /**< Default value for WDOG_IEN                  */
#define _WDOG_IEN_MASK                       0x0000001FUL                               /**< Mask for WDOG_IEN                           */
#define WDOG_IEN_TOUT                        (0x1UL << 0)                               /**< WDOG Timeout Interrupt Enable               */
#define _WDOG_IEN_TOUT_SHIFT                 0                                          /**< Shift value for WDOG_TOUT                   */
#define _WDOG_IEN_TOUT_MASK                  0x1UL                                      /**< Bit mask for WDOG_TOUT                      */
#define _WDOG_IEN_TOUT_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for WDOG_IEN                   */
#define WDOG_IEN_TOUT_DEFAULT                (_WDOG_IEN_TOUT_DEFAULT << 0)              /**< Shifted mode DEFAULT for WDOG_IEN           */
#define WDOG_IEN_WARN                        (0x1UL << 1)                               /**< WDOG Warning Timeout Interrupt Enable       */
#define _WDOG_IEN_WARN_SHIFT                 1                                          /**< Shift value for WDOG_WARN                   */
#define _WDOG_IEN_WARN_MASK                  0x2UL                                      /**< Bit mask for WDOG_WARN                      */
#define _WDOG_IEN_WARN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for WDOG_IEN                   */
#define WDOG_IEN_WARN_DEFAULT                (_WDOG_IEN_WARN_DEFAULT << 1)              /**< Shifted mode DEFAULT for WDOG_IEN           */
#define WDOG_IEN_WIN                         (0x1UL << 2)                               /**< WDOG Window Interrupt Enable                */
#define _WDOG_IEN_WIN_SHIFT                  2                                          /**< Shift value for WDOG_WIN                    */
#define _WDOG_IEN_WIN_MASK                   0x4UL                                      /**< Bit mask for WDOG_WIN                       */
#define _WDOG_IEN_WIN_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for WDOG_IEN                   */
#define WDOG_IEN_WIN_DEFAULT                 (_WDOG_IEN_WIN_DEFAULT << 2)               /**< Shifted mode DEFAULT for WDOG_IEN           */
#define WDOG_IEN_PEM0                        (0x1UL << 3)                               /**< PRS Src0 Event Missing Interrupt Enable     */
#define _WDOG_IEN_PEM0_SHIFT                 3                                          /**< Shift value for WDOG_PEM0                   */
#define _WDOG_IEN_PEM0_MASK                  0x8UL                                      /**< Bit mask for WDOG_PEM0                      */
#define _WDOG_IEN_PEM0_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for WDOG_IEN                   */
#define WDOG_IEN_PEM0_DEFAULT                (_WDOG_IEN_PEM0_DEFAULT << 3)              /**< Shifted mode DEFAULT for WDOG_IEN           */
#define WDOG_IEN_PEM1                        (0x1UL << 4)                               /**< PRS Src1 Event Missing Interrupt Enable     */
#define _WDOG_IEN_PEM1_SHIFT                 4                                          /**< Shift value for WDOG_PEM1                   */
#define _WDOG_IEN_PEM1_MASK                  0x10UL                                     /**< Bit mask for WDOG_PEM1                      */
#define _WDOG_IEN_PEM1_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for WDOG_IEN                   */
#define WDOG_IEN_PEM1_DEFAULT                (_WDOG_IEN_PEM1_DEFAULT << 4)              /**< Shifted mode DEFAULT for WDOG_IEN           */

/* Bit fields for WDOG LOCK */
#define _WDOG_LOCK_RESETVALUE                0x0000ABE8UL                               /**< Default value for WDOG_LOCK                 */
#define _WDOG_LOCK_MASK                      0x0000FFFFUL                               /**< Mask for WDOG_LOCK                          */
#define _WDOG_LOCK_LOCKKEY_SHIFT             0                                          /**< Shift value for WDOG_LOCKKEY                */
#define _WDOG_LOCK_LOCKKEY_MASK              0xFFFFUL                                   /**< Bit mask for WDOG_LOCKKEY                   */
#define _WDOG_LOCK_LOCKKEY_DEFAULT           0x0000ABE8UL                               /**< Mode DEFAULT for WDOG_LOCK                  */
#define _WDOG_LOCK_LOCKKEY_LOCK              0x00000000UL                               /**< Mode LOCK for WDOG_LOCK                     */
#define _WDOG_LOCK_LOCKKEY_UNLOCK            0x0000ABE8UL                               /**< Mode UNLOCK for WDOG_LOCK                   */
#define WDOG_LOCK_LOCKKEY_DEFAULT            (_WDOG_LOCK_LOCKKEY_DEFAULT << 0)          /**< Shifted mode DEFAULT for WDOG_LOCK          */
#define WDOG_LOCK_LOCKKEY_LOCK               (_WDOG_LOCK_LOCKKEY_LOCK << 0)             /**< Shifted mode LOCK for WDOG_LOCK             */
#define WDOG_LOCK_LOCKKEY_UNLOCK             (_WDOG_LOCK_LOCKKEY_UNLOCK << 0)           /**< Shifted mode UNLOCK for WDOG_LOCK           */

/* Bit fields for WDOG SYNCBUSY */
#define _WDOG_SYNCBUSY_RESETVALUE            0x00000000UL                               /**< Default value for WDOG_SYNCBUSY             */
#define _WDOG_SYNCBUSY_MASK                  0x00000001UL                               /**< Mask for WDOG_SYNCBUSY                      */
#define WDOG_SYNCBUSY_CMD                    (0x1UL << 0)                               /**< Sync Busy for Cmd Register                  */
#define _WDOG_SYNCBUSY_CMD_SHIFT             0                                          /**< Shift value for WDOG_CMD                    */
#define _WDOG_SYNCBUSY_CMD_MASK              0x1UL                                      /**< Bit mask for WDOG_CMD                       */
#define _WDOG_SYNCBUSY_CMD_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for WDOG_SYNCBUSY              */
#define WDOG_SYNCBUSY_CMD_DEFAULT            (_WDOG_SYNCBUSY_CMD_DEFAULT << 0)          /**< Shifted mode DEFAULT for WDOG_SYNCBUSY      */

/** @} End of group EFR32ZG23_WDOG_BitFields */
/** @} End of group EFR32ZG23_WDOG */
/** @} End of group Parts */

#endif /* EFR32ZG23_WDOG_H */
#ifdef __cplusplus
}
#endif

