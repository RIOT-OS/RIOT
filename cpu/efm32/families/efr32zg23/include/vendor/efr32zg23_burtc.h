/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 BURTC register and bit field definitions
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

#ifndef EFR32ZG23_BURTC_H
#define EFR32ZG23_BURTC_H
#define BURTC_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_BURTC BURTC
 * @{
 * @brief EFR32ZG23 BURTC Register Declaration.
 *****************************************************************************/

/** BURTC Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  __IOM uint32_t EN;                            /**< Module Enable Register                             */
  __IOM uint32_t CFG;                           /**< Configuration Register                             */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  __IOM uint32_t PRECNT;                        /**< Pre-Counter Value Register                         */
  __IOM uint32_t CNT;                           /**< Counter Value Register                             */
  __IOM uint32_t EM4WUEN;                       /**< EM4 wakeup request Enable Register                 */
  __IM uint32_t  SYNCBUSY;                      /**< Synchronization Busy Register                      */
  __IOM uint32_t LOCK;                          /**< Configuration Lock Register                        */
  __IOM uint32_t COMP;                          /**< Compare Value Register                             */
  uint32_t       RESERVED0[1011U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  __IOM uint32_t EN_SET;                        /**< Module Enable Register                             */
  __IOM uint32_t CFG_SET;                       /**< Configuration Register                             */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t PRECNT_SET;                    /**< Pre-Counter Value Register                         */
  __IOM uint32_t CNT_SET;                       /**< Counter Value Register                             */
  __IOM uint32_t EM4WUEN_SET;                   /**< EM4 wakeup request Enable Register                 */
  __IM uint32_t  SYNCBUSY_SET;                  /**< Synchronization Busy Register                      */
  __IOM uint32_t LOCK_SET;                      /**< Configuration Lock Register                        */
  __IOM uint32_t COMP_SET;                      /**< Compare Value Register                             */
  uint32_t       RESERVED1[1011U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  __IOM uint32_t EN_CLR;                        /**< Module Enable Register                             */
  __IOM uint32_t CFG_CLR;                       /**< Configuration Register                             */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t PRECNT_CLR;                    /**< Pre-Counter Value Register                         */
  __IOM uint32_t CNT_CLR;                       /**< Counter Value Register                             */
  __IOM uint32_t EM4WUEN_CLR;                   /**< EM4 wakeup request Enable Register                 */
  __IM uint32_t  SYNCBUSY_CLR;                  /**< Synchronization Busy Register                      */
  __IOM uint32_t LOCK_CLR;                      /**< Configuration Lock Register                        */
  __IOM uint32_t COMP_CLR;                      /**< Compare Value Register                             */
  uint32_t       RESERVED2[1011U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  __IOM uint32_t EN_TGL;                        /**< Module Enable Register                             */
  __IOM uint32_t CFG_TGL;                       /**< Configuration Register                             */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t PRECNT_TGL;                    /**< Pre-Counter Value Register                         */
  __IOM uint32_t CNT_TGL;                       /**< Counter Value Register                             */
  __IOM uint32_t EM4WUEN_TGL;                   /**< EM4 wakeup request Enable Register                 */
  __IM uint32_t  SYNCBUSY_TGL;                  /**< Synchronization Busy Register                      */
  __IOM uint32_t LOCK_TGL;                      /**< Configuration Lock Register                        */
  __IOM uint32_t COMP_TGL;                      /**< Compare Value Register                             */
} BURTC_TypeDef;
/** @} End of group EFR32ZG23_BURTC */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_BURTC
 * @{
 * @defgroup EFR32ZG23_BURTC_BitFields BURTC Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for BURTC IPVERSION */
#define _BURTC_IPVERSION_RESETVALUE           0x00000001UL                              /**< Default value for BURTC_IPVERSION           */
#define _BURTC_IPVERSION_MASK                 0xFFFFFFFFUL                              /**< Mask for BURTC_IPVERSION                    */
#define _BURTC_IPVERSION_IPVERSION_SHIFT      0                                         /**< Shift value for BURTC_IPVERSION             */
#define _BURTC_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                              /**< Bit mask for BURTC_IPVERSION                */
#define _BURTC_IPVERSION_IPVERSION_DEFAULT    0x00000001UL                              /**< Mode DEFAULT for BURTC_IPVERSION            */
#define BURTC_IPVERSION_IPVERSION_DEFAULT     (_BURTC_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for BURTC_IPVERSION    */

/* Bit fields for BURTC EN */
#define _BURTC_EN_RESETVALUE                  0x00000000UL                              /**< Default value for BURTC_EN                  */
#define _BURTC_EN_MASK                        0x00000003UL                              /**< Mask for BURTC_EN                           */
#define BURTC_EN_EN                           (0x1UL << 0)                              /**< BURTC Enable                                */
#define _BURTC_EN_EN_SHIFT                    0                                         /**< Shift value for BURTC_EN                    */
#define _BURTC_EN_EN_MASK                     0x1UL                                     /**< Bit mask for BURTC_EN                       */
#define _BURTC_EN_EN_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for BURTC_EN                   */
#define BURTC_EN_EN_DEFAULT                   (_BURTC_EN_EN_DEFAULT << 0)               /**< Shifted mode DEFAULT for BURTC_EN           */
#define BURTC_EN_DISABLING                    (0x1UL << 1)                              /**< Disablement busy status                     */
#define _BURTC_EN_DISABLING_SHIFT             1                                         /**< Shift value for BURTC_DISABLING             */
#define _BURTC_EN_DISABLING_MASK              0x2UL                                     /**< Bit mask for BURTC_DISABLING                */
#define _BURTC_EN_DISABLING_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for BURTC_EN                   */
#define BURTC_EN_DISABLING_DEFAULT            (_BURTC_EN_DISABLING_DEFAULT << 1)        /**< Shifted mode DEFAULT for BURTC_EN           */

/* Bit fields for BURTC CFG */
#define _BURTC_CFG_RESETVALUE                 0x00000000UL                              /**< Default value for BURTC_CFG                 */
#define _BURTC_CFG_MASK                       0x000000F3UL                              /**< Mask for BURTC_CFG                          */
#define BURTC_CFG_DEBUGRUN                    (0x1UL << 0)                              /**< Debug Mode Run Enable                       */
#define _BURTC_CFG_DEBUGRUN_SHIFT             0                                         /**< Shift value for BURTC_DEBUGRUN              */
#define _BURTC_CFG_DEBUGRUN_MASK              0x1UL                                     /**< Bit mask for BURTC_DEBUGRUN                 */
#define _BURTC_CFG_DEBUGRUN_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for BURTC_CFG                  */
#define _BURTC_CFG_DEBUGRUN_X0                0x00000000UL                              /**< Mode X0 for BURTC_CFG                       */
#define _BURTC_CFG_DEBUGRUN_X1                0x00000001UL                              /**< Mode X1 for BURTC_CFG                       */
#define BURTC_CFG_DEBUGRUN_DEFAULT            (_BURTC_CFG_DEBUGRUN_DEFAULT << 0)        /**< Shifted mode DEFAULT for BURTC_CFG          */
#define BURTC_CFG_DEBUGRUN_X0                 (_BURTC_CFG_DEBUGRUN_X0 << 0)             /**< Shifted mode X0 for BURTC_CFG               */
#define BURTC_CFG_DEBUGRUN_X1                 (_BURTC_CFG_DEBUGRUN_X1 << 0)             /**< Shifted mode X1 for BURTC_CFG               */
#define BURTC_CFG_COMPTOP                     (0x1UL << 1)                              /**< Compare Channel is Top Value                */
#define _BURTC_CFG_COMPTOP_SHIFT              1                                         /**< Shift value for BURTC_COMPTOP               */
#define _BURTC_CFG_COMPTOP_MASK               0x2UL                                     /**< Bit mask for BURTC_COMPTOP                  */
#define _BURTC_CFG_COMPTOP_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for BURTC_CFG                  */
#define _BURTC_CFG_COMPTOP_DISABLE            0x00000000UL                              /**< Mode DISABLE for BURTC_CFG                  */
#define _BURTC_CFG_COMPTOP_ENABLE             0x00000001UL                              /**< Mode ENABLE for BURTC_CFG                   */
#define BURTC_CFG_COMPTOP_DEFAULT             (_BURTC_CFG_COMPTOP_DEFAULT << 1)         /**< Shifted mode DEFAULT for BURTC_CFG          */
#define BURTC_CFG_COMPTOP_DISABLE             (_BURTC_CFG_COMPTOP_DISABLE << 1)         /**< Shifted mode DISABLE for BURTC_CFG          */
#define BURTC_CFG_COMPTOP_ENABLE              (_BURTC_CFG_COMPTOP_ENABLE << 1)          /**< Shifted mode ENABLE for BURTC_CFG           */
#define _BURTC_CFG_CNTPRESC_SHIFT             4                                         /**< Shift value for BURTC_CNTPRESC              */
#define _BURTC_CFG_CNTPRESC_MASK              0xF0UL                                    /**< Bit mask for BURTC_CNTPRESC                 */
#define _BURTC_CFG_CNTPRESC_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for BURTC_CFG                  */
#define _BURTC_CFG_CNTPRESC_DIV1              0x00000000UL                              /**< Mode DIV1 for BURTC_CFG                     */
#define _BURTC_CFG_CNTPRESC_DIV2              0x00000001UL                              /**< Mode DIV2 for BURTC_CFG                     */
#define _BURTC_CFG_CNTPRESC_DIV4              0x00000002UL                              /**< Mode DIV4 for BURTC_CFG                     */
#define _BURTC_CFG_CNTPRESC_DIV8              0x00000003UL                              /**< Mode DIV8 for BURTC_CFG                     */
#define _BURTC_CFG_CNTPRESC_DIV16             0x00000004UL                              /**< Mode DIV16 for BURTC_CFG                    */
#define _BURTC_CFG_CNTPRESC_DIV32             0x00000005UL                              /**< Mode DIV32 for BURTC_CFG                    */
#define _BURTC_CFG_CNTPRESC_DIV64             0x00000006UL                              /**< Mode DIV64 for BURTC_CFG                    */
#define _BURTC_CFG_CNTPRESC_DIV128            0x00000007UL                              /**< Mode DIV128 for BURTC_CFG                   */
#define _BURTC_CFG_CNTPRESC_DIV256            0x00000008UL                              /**< Mode DIV256 for BURTC_CFG                   */
#define _BURTC_CFG_CNTPRESC_DIV512            0x00000009UL                              /**< Mode DIV512 for BURTC_CFG                   */
#define _BURTC_CFG_CNTPRESC_DIV1024           0x0000000AUL                              /**< Mode DIV1024 for BURTC_CFG                  */
#define _BURTC_CFG_CNTPRESC_DIV2048           0x0000000BUL                              /**< Mode DIV2048 for BURTC_CFG                  */
#define _BURTC_CFG_CNTPRESC_DIV4096           0x0000000CUL                              /**< Mode DIV4096 for BURTC_CFG                  */
#define _BURTC_CFG_CNTPRESC_DIV8192           0x0000000DUL                              /**< Mode DIV8192 for BURTC_CFG                  */
#define _BURTC_CFG_CNTPRESC_DIV16384          0x0000000EUL                              /**< Mode DIV16384 for BURTC_CFG                 */
#define _BURTC_CFG_CNTPRESC_DIV32768          0x0000000FUL                              /**< Mode DIV32768 for BURTC_CFG                 */
#define BURTC_CFG_CNTPRESC_DEFAULT            (_BURTC_CFG_CNTPRESC_DEFAULT << 4)        /**< Shifted mode DEFAULT for BURTC_CFG          */
#define BURTC_CFG_CNTPRESC_DIV1               (_BURTC_CFG_CNTPRESC_DIV1 << 4)           /**< Shifted mode DIV1 for BURTC_CFG             */
#define BURTC_CFG_CNTPRESC_DIV2               (_BURTC_CFG_CNTPRESC_DIV2 << 4)           /**< Shifted mode DIV2 for BURTC_CFG             */
#define BURTC_CFG_CNTPRESC_DIV4               (_BURTC_CFG_CNTPRESC_DIV4 << 4)           /**< Shifted mode DIV4 for BURTC_CFG             */
#define BURTC_CFG_CNTPRESC_DIV8               (_BURTC_CFG_CNTPRESC_DIV8 << 4)           /**< Shifted mode DIV8 for BURTC_CFG             */
#define BURTC_CFG_CNTPRESC_DIV16              (_BURTC_CFG_CNTPRESC_DIV16 << 4)          /**< Shifted mode DIV16 for BURTC_CFG            */
#define BURTC_CFG_CNTPRESC_DIV32              (_BURTC_CFG_CNTPRESC_DIV32 << 4)          /**< Shifted mode DIV32 for BURTC_CFG            */
#define BURTC_CFG_CNTPRESC_DIV64              (_BURTC_CFG_CNTPRESC_DIV64 << 4)          /**< Shifted mode DIV64 for BURTC_CFG            */
#define BURTC_CFG_CNTPRESC_DIV128             (_BURTC_CFG_CNTPRESC_DIV128 << 4)         /**< Shifted mode DIV128 for BURTC_CFG           */
#define BURTC_CFG_CNTPRESC_DIV256             (_BURTC_CFG_CNTPRESC_DIV256 << 4)         /**< Shifted mode DIV256 for BURTC_CFG           */
#define BURTC_CFG_CNTPRESC_DIV512             (_BURTC_CFG_CNTPRESC_DIV512 << 4)         /**< Shifted mode DIV512 for BURTC_CFG           */
#define BURTC_CFG_CNTPRESC_DIV1024            (_BURTC_CFG_CNTPRESC_DIV1024 << 4)        /**< Shifted mode DIV1024 for BURTC_CFG          */
#define BURTC_CFG_CNTPRESC_DIV2048            (_BURTC_CFG_CNTPRESC_DIV2048 << 4)        /**< Shifted mode DIV2048 for BURTC_CFG          */
#define BURTC_CFG_CNTPRESC_DIV4096            (_BURTC_CFG_CNTPRESC_DIV4096 << 4)        /**< Shifted mode DIV4096 for BURTC_CFG          */
#define BURTC_CFG_CNTPRESC_DIV8192            (_BURTC_CFG_CNTPRESC_DIV8192 << 4)        /**< Shifted mode DIV8192 for BURTC_CFG          */
#define BURTC_CFG_CNTPRESC_DIV16384           (_BURTC_CFG_CNTPRESC_DIV16384 << 4)       /**< Shifted mode DIV16384 for BURTC_CFG         */
#define BURTC_CFG_CNTPRESC_DIV32768           (_BURTC_CFG_CNTPRESC_DIV32768 << 4)       /**< Shifted mode DIV32768 for BURTC_CFG         */

/* Bit fields for BURTC CMD */
#define _BURTC_CMD_RESETVALUE                 0x00000000UL                              /**< Default value for BURTC_CMD                 */
#define _BURTC_CMD_MASK                       0x00000003UL                              /**< Mask for BURTC_CMD                          */
#define BURTC_CMD_START                       (0x1UL << 0)                              /**< Start BURTC counter                         */
#define _BURTC_CMD_START_SHIFT                0                                         /**< Shift value for BURTC_START                 */
#define _BURTC_CMD_START_MASK                 0x1UL                                     /**< Bit mask for BURTC_START                    */
#define _BURTC_CMD_START_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for BURTC_CMD                  */
#define BURTC_CMD_START_DEFAULT               (_BURTC_CMD_START_DEFAULT << 0)           /**< Shifted mode DEFAULT for BURTC_CMD          */
#define BURTC_CMD_STOP                        (0x1UL << 1)                              /**< Stop BURTC counter                          */
#define _BURTC_CMD_STOP_SHIFT                 1                                         /**< Shift value for BURTC_STOP                  */
#define _BURTC_CMD_STOP_MASK                  0x2UL                                     /**< Bit mask for BURTC_STOP                     */
#define _BURTC_CMD_STOP_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for BURTC_CMD                  */
#define BURTC_CMD_STOP_DEFAULT                (_BURTC_CMD_STOP_DEFAULT << 1)            /**< Shifted mode DEFAULT for BURTC_CMD          */

/* Bit fields for BURTC STATUS */
#define _BURTC_STATUS_RESETVALUE              0x00000000UL                              /**< Default value for BURTC_STATUS              */
#define _BURTC_STATUS_MASK                    0x00000003UL                              /**< Mask for BURTC_STATUS                       */
#define BURTC_STATUS_RUNNING                  (0x1UL << 0)                              /**< BURTC running status                        */
#define _BURTC_STATUS_RUNNING_SHIFT           0                                         /**< Shift value for BURTC_RUNNING               */
#define _BURTC_STATUS_RUNNING_MASK            0x1UL                                     /**< Bit mask for BURTC_RUNNING                  */
#define _BURTC_STATUS_RUNNING_DEFAULT         0x00000000UL                              /**< Mode DEFAULT for BURTC_STATUS               */
#define BURTC_STATUS_RUNNING_DEFAULT          (_BURTC_STATUS_RUNNING_DEFAULT << 0)      /**< Shifted mode DEFAULT for BURTC_STATUS       */
#define BURTC_STATUS_LOCK                     (0x1UL << 1)                              /**< Configuration Lock Status                   */
#define _BURTC_STATUS_LOCK_SHIFT              1                                         /**< Shift value for BURTC_LOCK                  */
#define _BURTC_STATUS_LOCK_MASK               0x2UL                                     /**< Bit mask for BURTC_LOCK                     */
#define _BURTC_STATUS_LOCK_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for BURTC_STATUS               */
#define _BURTC_STATUS_LOCK_UNLOCKED           0x00000000UL                              /**< Mode UNLOCKED for BURTC_STATUS              */
#define _BURTC_STATUS_LOCK_LOCKED             0x00000001UL                              /**< Mode LOCKED for BURTC_STATUS                */
#define BURTC_STATUS_LOCK_DEFAULT             (_BURTC_STATUS_LOCK_DEFAULT << 1)         /**< Shifted mode DEFAULT for BURTC_STATUS       */
#define BURTC_STATUS_LOCK_UNLOCKED            (_BURTC_STATUS_LOCK_UNLOCKED << 1)        /**< Shifted mode UNLOCKED for BURTC_STATUS      */
#define BURTC_STATUS_LOCK_LOCKED              (_BURTC_STATUS_LOCK_LOCKED << 1)          /**< Shifted mode LOCKED for BURTC_STATUS        */

/* Bit fields for BURTC IF */
#define _BURTC_IF_RESETVALUE                  0x00000000UL                              /**< Default value for BURTC_IF                  */
#define _BURTC_IF_MASK                        0x00000003UL                              /**< Mask for BURTC_IF                           */
#define BURTC_IF_OF                           (0x1UL << 0)                              /**< Overflow Interrupt Flag                     */
#define _BURTC_IF_OF_SHIFT                    0                                         /**< Shift value for BURTC_OF                    */
#define _BURTC_IF_OF_MASK                     0x1UL                                     /**< Bit mask for BURTC_OF                       */
#define _BURTC_IF_OF_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for BURTC_IF                   */
#define BURTC_IF_OF_DEFAULT                   (_BURTC_IF_OF_DEFAULT << 0)               /**< Shifted mode DEFAULT for BURTC_IF           */
#define BURTC_IF_COMP                         (0x1UL << 1)                              /**< Compare Match Interrupt Flag                */
#define _BURTC_IF_COMP_SHIFT                  1                                         /**< Shift value for BURTC_COMP                  */
#define _BURTC_IF_COMP_MASK                   0x2UL                                     /**< Bit mask for BURTC_COMP                     */
#define _BURTC_IF_COMP_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for BURTC_IF                   */
#define BURTC_IF_COMP_DEFAULT                 (_BURTC_IF_COMP_DEFAULT << 1)             /**< Shifted mode DEFAULT for BURTC_IF           */

/* Bit fields for BURTC IEN */
#define _BURTC_IEN_RESETVALUE                 0x00000000UL                              /**< Default value for BURTC_IEN                 */
#define _BURTC_IEN_MASK                       0x00000003UL                              /**< Mask for BURTC_IEN                          */
#define BURTC_IEN_OF                          (0x1UL << 0)                              /**< Overflow Interrupt Flag                     */
#define _BURTC_IEN_OF_SHIFT                   0                                         /**< Shift value for BURTC_OF                    */
#define _BURTC_IEN_OF_MASK                    0x1UL                                     /**< Bit mask for BURTC_OF                       */
#define _BURTC_IEN_OF_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for BURTC_IEN                  */
#define BURTC_IEN_OF_DEFAULT                  (_BURTC_IEN_OF_DEFAULT << 0)              /**< Shifted mode DEFAULT for BURTC_IEN          */
#define BURTC_IEN_COMP                        (0x1UL << 1)                              /**< Compare Match Interrupt Flag                */
#define _BURTC_IEN_COMP_SHIFT                 1                                         /**< Shift value for BURTC_COMP                  */
#define _BURTC_IEN_COMP_MASK                  0x2UL                                     /**< Bit mask for BURTC_COMP                     */
#define _BURTC_IEN_COMP_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for BURTC_IEN                  */
#define BURTC_IEN_COMP_DEFAULT                (_BURTC_IEN_COMP_DEFAULT << 1)            /**< Shifted mode DEFAULT for BURTC_IEN          */

/* Bit fields for BURTC PRECNT */
#define _BURTC_PRECNT_RESETVALUE              0x00000000UL                              /**< Default value for BURTC_PRECNT              */
#define _BURTC_PRECNT_MASK                    0x00007FFFUL                              /**< Mask for BURTC_PRECNT                       */
#define _BURTC_PRECNT_PRECNT_SHIFT            0                                         /**< Shift value for BURTC_PRECNT                */
#define _BURTC_PRECNT_PRECNT_MASK             0x7FFFUL                                  /**< Bit mask for BURTC_PRECNT                   */
#define _BURTC_PRECNT_PRECNT_DEFAULT          0x00000000UL                              /**< Mode DEFAULT for BURTC_PRECNT               */
#define BURTC_PRECNT_PRECNT_DEFAULT           (_BURTC_PRECNT_PRECNT_DEFAULT << 0)       /**< Shifted mode DEFAULT for BURTC_PRECNT       */

/* Bit fields for BURTC CNT */
#define _BURTC_CNT_RESETVALUE                 0x00000000UL                              /**< Default value for BURTC_CNT                 */
#define _BURTC_CNT_MASK                       0xFFFFFFFFUL                              /**< Mask for BURTC_CNT                          */
#define _BURTC_CNT_CNT_SHIFT                  0                                         /**< Shift value for BURTC_CNT                   */
#define _BURTC_CNT_CNT_MASK                   0xFFFFFFFFUL                              /**< Bit mask for BURTC_CNT                      */
#define _BURTC_CNT_CNT_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for BURTC_CNT                  */
#define BURTC_CNT_CNT_DEFAULT                 (_BURTC_CNT_CNT_DEFAULT << 0)             /**< Shifted mode DEFAULT for BURTC_CNT          */

/* Bit fields for BURTC EM4WUEN */
#define _BURTC_EM4WUEN_RESETVALUE             0x00000000UL                              /**< Default value for BURTC_EM4WUEN             */
#define _BURTC_EM4WUEN_MASK                   0x00000003UL                              /**< Mask for BURTC_EM4WUEN                      */
#define BURTC_EM4WUEN_OFEM4WUEN               (0x1UL << 0)                              /**< Overflow EM4 Wakeup Enable                  */
#define _BURTC_EM4WUEN_OFEM4WUEN_SHIFT        0                                         /**< Shift value for BURTC_OFEM4WUEN             */
#define _BURTC_EM4WUEN_OFEM4WUEN_MASK         0x1UL                                     /**< Bit mask for BURTC_OFEM4WUEN                */
#define _BURTC_EM4WUEN_OFEM4WUEN_DEFAULT      0x00000000UL                              /**< Mode DEFAULT for BURTC_EM4WUEN              */
#define BURTC_EM4WUEN_OFEM4WUEN_DEFAULT       (_BURTC_EM4WUEN_OFEM4WUEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for BURTC_EM4WUEN      */
#define BURTC_EM4WUEN_COMPEM4WUEN             (0x1UL << 1)                              /**< Compare Match EM4 Wakeup Enable             */
#define _BURTC_EM4WUEN_COMPEM4WUEN_SHIFT      1                                         /**< Shift value for BURTC_COMPEM4WUEN           */
#define _BURTC_EM4WUEN_COMPEM4WUEN_MASK       0x2UL                                     /**< Bit mask for BURTC_COMPEM4WUEN              */
#define _BURTC_EM4WUEN_COMPEM4WUEN_DEFAULT    0x00000000UL                              /**< Mode DEFAULT for BURTC_EM4WUEN              */
#define BURTC_EM4WUEN_COMPEM4WUEN_DEFAULT     (_BURTC_EM4WUEN_COMPEM4WUEN_DEFAULT << 1) /**< Shifted mode DEFAULT for BURTC_EM4WUEN      */

/* Bit fields for BURTC SYNCBUSY */
#define _BURTC_SYNCBUSY_RESETVALUE            0x00000000UL                              /**< Default value for BURTC_SYNCBUSY            */
#define _BURTC_SYNCBUSY_MASK                  0x0000001FUL                              /**< Mask for BURTC_SYNCBUSY                     */
#define BURTC_SYNCBUSY_START                  (0x1UL << 0)                              /**< Sync busy for START                         */
#define _BURTC_SYNCBUSY_START_SHIFT           0                                         /**< Shift value for BURTC_START                 */
#define _BURTC_SYNCBUSY_START_MASK            0x1UL                                     /**< Bit mask for BURTC_START                    */
#define _BURTC_SYNCBUSY_START_DEFAULT         0x00000000UL                              /**< Mode DEFAULT for BURTC_SYNCBUSY             */
#define BURTC_SYNCBUSY_START_DEFAULT          (_BURTC_SYNCBUSY_START_DEFAULT << 0)      /**< Shifted mode DEFAULT for BURTC_SYNCBUSY     */
#define BURTC_SYNCBUSY_STOP                   (0x1UL << 1)                              /**< Sync busy for STOP                          */
#define _BURTC_SYNCBUSY_STOP_SHIFT            1                                         /**< Shift value for BURTC_STOP                  */
#define _BURTC_SYNCBUSY_STOP_MASK             0x2UL                                     /**< Bit mask for BURTC_STOP                     */
#define _BURTC_SYNCBUSY_STOP_DEFAULT          0x00000000UL                              /**< Mode DEFAULT for BURTC_SYNCBUSY             */
#define BURTC_SYNCBUSY_STOP_DEFAULT           (_BURTC_SYNCBUSY_STOP_DEFAULT << 1)       /**< Shifted mode DEFAULT for BURTC_SYNCBUSY     */
#define BURTC_SYNCBUSY_PRECNT                 (0x1UL << 2)                              /**< Sync busy for PRECNT                        */
#define _BURTC_SYNCBUSY_PRECNT_SHIFT          2                                         /**< Shift value for BURTC_PRECNT                */
#define _BURTC_SYNCBUSY_PRECNT_MASK           0x4UL                                     /**< Bit mask for BURTC_PRECNT                   */
#define _BURTC_SYNCBUSY_PRECNT_DEFAULT        0x00000000UL                              /**< Mode DEFAULT for BURTC_SYNCBUSY             */
#define BURTC_SYNCBUSY_PRECNT_DEFAULT         (_BURTC_SYNCBUSY_PRECNT_DEFAULT << 2)     /**< Shifted mode DEFAULT for BURTC_SYNCBUSY     */
#define BURTC_SYNCBUSY_CNT                    (0x1UL << 3)                              /**< Sync busy for CNT                           */
#define _BURTC_SYNCBUSY_CNT_SHIFT             3                                         /**< Shift value for BURTC_CNT                   */
#define _BURTC_SYNCBUSY_CNT_MASK              0x8UL                                     /**< Bit mask for BURTC_CNT                      */
#define _BURTC_SYNCBUSY_CNT_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for BURTC_SYNCBUSY             */
#define BURTC_SYNCBUSY_CNT_DEFAULT            (_BURTC_SYNCBUSY_CNT_DEFAULT << 3)        /**< Shifted mode DEFAULT for BURTC_SYNCBUSY     */
#define BURTC_SYNCBUSY_COMP                   (0x1UL << 4)                              /**< Sync busy for COMP                          */
#define _BURTC_SYNCBUSY_COMP_SHIFT            4                                         /**< Shift value for BURTC_COMP                  */
#define _BURTC_SYNCBUSY_COMP_MASK             0x10UL                                    /**< Bit mask for BURTC_COMP                     */
#define _BURTC_SYNCBUSY_COMP_DEFAULT          0x00000000UL                              /**< Mode DEFAULT for BURTC_SYNCBUSY             */
#define BURTC_SYNCBUSY_COMP_DEFAULT           (_BURTC_SYNCBUSY_COMP_DEFAULT << 4)       /**< Shifted mode DEFAULT for BURTC_SYNCBUSY     */

/* Bit fields for BURTC LOCK */
#define _BURTC_LOCK_RESETVALUE                0x0000AEE8UL                              /**< Default value for BURTC_LOCK                */
#define _BURTC_LOCK_MASK                      0x0000FFFFUL                              /**< Mask for BURTC_LOCK                         */
#define _BURTC_LOCK_LOCKKEY_SHIFT             0                                         /**< Shift value for BURTC_LOCKKEY               */
#define _BURTC_LOCK_LOCKKEY_MASK              0xFFFFUL                                  /**< Bit mask for BURTC_LOCKKEY                  */
#define _BURTC_LOCK_LOCKKEY_DEFAULT           0x0000AEE8UL                              /**< Mode DEFAULT for BURTC_LOCK                 */
#define _BURTC_LOCK_LOCKKEY_UNLOCK            0x0000AEE8UL                              /**< Mode UNLOCK for BURTC_LOCK                  */
#define BURTC_LOCK_LOCKKEY_DEFAULT            (_BURTC_LOCK_LOCKKEY_DEFAULT << 0)        /**< Shifted mode DEFAULT for BURTC_LOCK         */
#define BURTC_LOCK_LOCKKEY_UNLOCK             (_BURTC_LOCK_LOCKKEY_UNLOCK << 0)         /**< Shifted mode UNLOCK for BURTC_LOCK          */

/* Bit fields for BURTC COMP */
#define _BURTC_COMP_RESETVALUE                0x00000000UL                              /**< Default value for BURTC_COMP                */
#define _BURTC_COMP_MASK                      0xFFFFFFFFUL                              /**< Mask for BURTC_COMP                         */
#define _BURTC_COMP_COMP_SHIFT                0                                         /**< Shift value for BURTC_COMP                  */
#define _BURTC_COMP_COMP_MASK                 0xFFFFFFFFUL                              /**< Bit mask for BURTC_COMP                     */
#define _BURTC_COMP_COMP_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for BURTC_COMP                 */
#define BURTC_COMP_COMP_DEFAULT               (_BURTC_COMP_COMP_DEFAULT << 0)           /**< Shifted mode DEFAULT for BURTC_COMP         */

/** @} End of group EFR32ZG23_BURTC_BitFields */
/** @} End of group EFR32ZG23_BURTC */
/** @} End of group Parts */

#endif /* EFR32ZG23_BURTC_H */
#ifdef __cplusplus
}
#endif

