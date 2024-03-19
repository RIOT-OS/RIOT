/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 SMU register and bit field definitions
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

#ifndef EFR32ZG23_SMU_H
#define EFR32ZG23_SMU_H
#define SMU_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_SMU SMU
 * @{
 * @brief EFR32ZG23 SMU Register Declaration.
 *****************************************************************************/

/** SMU Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP Version                                         */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t LOCK;                          /**< Lock Register                                      */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  uint32_t       RESERVED0[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t M33CTRL;                       /**< M33 Control Settings                               */
  uint32_t       RESERVED1[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t PPUPATD0;                      /**< Privileged Access                                  */
  __IOM uint32_t PPUPATD1;                      /**< Privileged Access                                  */
  uint32_t       RESERVED2[6U];                 /**< Reserved for future use                            */
  __IOM uint32_t PPUSATD0;                      /**< Secure Access                                      */
  __IOM uint32_t PPUSATD1;                      /**< Secure Access                                      */
  uint32_t       RESERVED3[54U];                /**< Reserved for future use                            */
  __IM uint32_t  PPUFS;                         /**< Fault Status                                       */
  uint32_t       RESERVED4[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t BMPUPATD0;                     /**< Privileged Attribute                               */
  uint32_t       RESERVED5[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t BMPUSATD0;                     /**< Secure Attribute                                   */
  uint32_t       RESERVED6[55U];                /**< Reserved for future use                            */
  __IM uint32_t  BMPUFS;                        /**< Fault Status                                       */
  __IM uint32_t  BMPUFSADDR;                    /**< Fault Status Address                               */
  uint32_t       RESERVED7[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t ESAURTYPES0;                   /**< Region Types 0                                     */
  __IOM uint32_t ESAURTYPES1;                   /**< Region Types 1                                     */
  uint32_t       RESERVED8[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t ESAUMRB01;                     /**< Movable Region Boundary                            */
  __IOM uint32_t ESAUMRB12;                     /**< Movable Region Boundary                            */
  uint32_t       RESERVED9[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t ESAUMRB45;                     /**< Movable Region Boundary                            */
  __IOM uint32_t ESAUMRB56;                     /**< Movable Region Boundary                            */
  uint32_t       RESERVED10[862U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP Version                                         */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t LOCK_SET;                      /**< Lock Register                                      */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED11[3U];                /**< Reserved for future use                            */
  __IOM uint32_t M33CTRL_SET;                   /**< M33 Control Settings                               */
  uint32_t       RESERVED12[7U];                /**< Reserved for future use                            */
  __IOM uint32_t PPUPATD0_SET;                  /**< Privileged Access                                  */
  __IOM uint32_t PPUPATD1_SET;                  /**< Privileged Access                                  */
  uint32_t       RESERVED13[6U];                /**< Reserved for future use                            */
  __IOM uint32_t PPUSATD0_SET;                  /**< Secure Access                                      */
  __IOM uint32_t PPUSATD1_SET;                  /**< Secure Access                                      */
  uint32_t       RESERVED14[54U];               /**< Reserved for future use                            */
  __IM uint32_t  PPUFS_SET;                     /**< Fault Status                                       */
  uint32_t       RESERVED15[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BMPUPATD0_SET;                 /**< Privileged Attribute                               */
  uint32_t       RESERVED16[7U];                /**< Reserved for future use                            */
  __IOM uint32_t BMPUSATD0_SET;                 /**< Secure Attribute                                   */
  uint32_t       RESERVED17[55U];               /**< Reserved for future use                            */
  __IM uint32_t  BMPUFS_SET;                    /**< Fault Status                                       */
  __IM uint32_t  BMPUFSADDR_SET;                /**< Fault Status Address                               */
  uint32_t       RESERVED18[2U];                /**< Reserved for future use                            */
  __IOM uint32_t ESAURTYPES0_SET;               /**< Region Types 0                                     */
  __IOM uint32_t ESAURTYPES1_SET;               /**< Region Types 1                                     */
  uint32_t       RESERVED19[2U];                /**< Reserved for future use                            */
  __IOM uint32_t ESAUMRB01_SET;                 /**< Movable Region Boundary                            */
  __IOM uint32_t ESAUMRB12_SET;                 /**< Movable Region Boundary                            */
  uint32_t       RESERVED20[2U];                /**< Reserved for future use                            */
  __IOM uint32_t ESAUMRB45_SET;                 /**< Movable Region Boundary                            */
  __IOM uint32_t ESAUMRB56_SET;                 /**< Movable Region Boundary                            */
  uint32_t       RESERVED21[862U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP Version                                         */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t LOCK_CLR;                      /**< Lock Register                                      */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED22[3U];                /**< Reserved for future use                            */
  __IOM uint32_t M33CTRL_CLR;                   /**< M33 Control Settings                               */
  uint32_t       RESERVED23[7U];                /**< Reserved for future use                            */
  __IOM uint32_t PPUPATD0_CLR;                  /**< Privileged Access                                  */
  __IOM uint32_t PPUPATD1_CLR;                  /**< Privileged Access                                  */
  uint32_t       RESERVED24[6U];                /**< Reserved for future use                            */
  __IOM uint32_t PPUSATD0_CLR;                  /**< Secure Access                                      */
  __IOM uint32_t PPUSATD1_CLR;                  /**< Secure Access                                      */
  uint32_t       RESERVED25[54U];               /**< Reserved for future use                            */
  __IM uint32_t  PPUFS_CLR;                     /**< Fault Status                                       */
  uint32_t       RESERVED26[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BMPUPATD0_CLR;                 /**< Privileged Attribute                               */
  uint32_t       RESERVED27[7U];                /**< Reserved for future use                            */
  __IOM uint32_t BMPUSATD0_CLR;                 /**< Secure Attribute                                   */
  uint32_t       RESERVED28[55U];               /**< Reserved for future use                            */
  __IM uint32_t  BMPUFS_CLR;                    /**< Fault Status                                       */
  __IM uint32_t  BMPUFSADDR_CLR;                /**< Fault Status Address                               */
  uint32_t       RESERVED29[2U];                /**< Reserved for future use                            */
  __IOM uint32_t ESAURTYPES0_CLR;               /**< Region Types 0                                     */
  __IOM uint32_t ESAURTYPES1_CLR;               /**< Region Types 1                                     */
  uint32_t       RESERVED30[2U];                /**< Reserved for future use                            */
  __IOM uint32_t ESAUMRB01_CLR;                 /**< Movable Region Boundary                            */
  __IOM uint32_t ESAUMRB12_CLR;                 /**< Movable Region Boundary                            */
  uint32_t       RESERVED31[2U];                /**< Reserved for future use                            */
  __IOM uint32_t ESAUMRB45_CLR;                 /**< Movable Region Boundary                            */
  __IOM uint32_t ESAUMRB56_CLR;                 /**< Movable Region Boundary                            */
  uint32_t       RESERVED32[862U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP Version                                         */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t LOCK_TGL;                      /**< Lock Register                                      */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED33[3U];                /**< Reserved for future use                            */
  __IOM uint32_t M33CTRL_TGL;                   /**< M33 Control Settings                               */
  uint32_t       RESERVED34[7U];                /**< Reserved for future use                            */
  __IOM uint32_t PPUPATD0_TGL;                  /**< Privileged Access                                  */
  __IOM uint32_t PPUPATD1_TGL;                  /**< Privileged Access                                  */
  uint32_t       RESERVED35[6U];                /**< Reserved for future use                            */
  __IOM uint32_t PPUSATD0_TGL;                  /**< Secure Access                                      */
  __IOM uint32_t PPUSATD1_TGL;                  /**< Secure Access                                      */
  uint32_t       RESERVED36[54U];               /**< Reserved for future use                            */
  __IM uint32_t  PPUFS_TGL;                     /**< Fault Status                                       */
  uint32_t       RESERVED37[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BMPUPATD0_TGL;                 /**< Privileged Attribute                               */
  uint32_t       RESERVED38[7U];                /**< Reserved for future use                            */
  __IOM uint32_t BMPUSATD0_TGL;                 /**< Secure Attribute                                   */
  uint32_t       RESERVED39[55U];               /**< Reserved for future use                            */
  __IM uint32_t  BMPUFS_TGL;                    /**< Fault Status                                       */
  __IM uint32_t  BMPUFSADDR_TGL;                /**< Fault Status Address                               */
  uint32_t       RESERVED40[2U];                /**< Reserved for future use                            */
  __IOM uint32_t ESAURTYPES0_TGL;               /**< Region Types 0                                     */
  __IOM uint32_t ESAURTYPES1_TGL;               /**< Region Types 1                                     */
  uint32_t       RESERVED41[2U];                /**< Reserved for future use                            */
  __IOM uint32_t ESAUMRB01_TGL;                 /**< Movable Region Boundary                            */
  __IOM uint32_t ESAUMRB12_TGL;                 /**< Movable Region Boundary                            */
  uint32_t       RESERVED42[2U];                /**< Reserved for future use                            */
  __IOM uint32_t ESAUMRB45_TGL;                 /**< Movable Region Boundary                            */
  __IOM uint32_t ESAUMRB56_TGL;                 /**< Movable Region Boundary                            */
} SMU_TypeDef;
/** @} End of group EFR32ZG23_SMU */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_SMU
 * @{
 * @defgroup EFR32ZG23_SMU_BitFields SMU Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SMU IPVERSION */
#define _SMU_IPVERSION_RESETVALUE                0x00000002UL                            /**< Default value for SMU_IPVERSION             */
#define _SMU_IPVERSION_MASK                      0xFFFFFFFFUL                            /**< Mask for SMU_IPVERSION                      */
#define _SMU_IPVERSION_IPVERSION_SHIFT           0                                       /**< Shift value for SMU_IPVERSION               */
#define _SMU_IPVERSION_IPVERSION_MASK            0xFFFFFFFFUL                            /**< Bit mask for SMU_IPVERSION                  */
#define _SMU_IPVERSION_IPVERSION_DEFAULT         0x00000002UL                            /**< Mode DEFAULT for SMU_IPVERSION              */
#define SMU_IPVERSION_IPVERSION_DEFAULT          (_SMU_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_IPVERSION      */

/* Bit fields for SMU STATUS */
#define _SMU_STATUS_RESETVALUE                   0x00000000UL                           /**< Default value for SMU_STATUS                */
#define _SMU_STATUS_MASK                         0x00000003UL                           /**< Mask for SMU_STATUS                         */
#define SMU_STATUS_SMULOCK                       (0x1UL << 0)                           /**< SMU Lock                                    */
#define _SMU_STATUS_SMULOCK_SHIFT                0                                      /**< Shift value for SMU_SMULOCK                 */
#define _SMU_STATUS_SMULOCK_MASK                 0x1UL                                  /**< Bit mask for SMU_SMULOCK                    */
#define _SMU_STATUS_SMULOCK_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for SMU_STATUS                 */
#define _SMU_STATUS_SMULOCK_UNLOCKED             0x00000000UL                           /**< Mode UNLOCKED for SMU_STATUS                */
#define _SMU_STATUS_SMULOCK_LOCKED               0x00000001UL                           /**< Mode LOCKED for SMU_STATUS                  */
#define SMU_STATUS_SMULOCK_DEFAULT               (_SMU_STATUS_SMULOCK_DEFAULT << 0)     /**< Shifted mode DEFAULT for SMU_STATUS         */
#define SMU_STATUS_SMULOCK_UNLOCKED              (_SMU_STATUS_SMULOCK_UNLOCKED << 0)    /**< Shifted mode UNLOCKED for SMU_STATUS        */
#define SMU_STATUS_SMULOCK_LOCKED                (_SMU_STATUS_SMULOCK_LOCKED << 0)      /**< Shifted mode LOCKED for SMU_STATUS          */
#define SMU_STATUS_SMUPRGERR                     (0x1UL << 1)                           /**< SMU Programming Error                       */
#define _SMU_STATUS_SMUPRGERR_SHIFT              1                                      /**< Shift value for SMU_SMUPRGERR               */
#define _SMU_STATUS_SMUPRGERR_MASK               0x2UL                                  /**< Bit mask for SMU_SMUPRGERR                  */
#define _SMU_STATUS_SMUPRGERR_DEFAULT            0x00000000UL                           /**< Mode DEFAULT for SMU_STATUS                 */
#define SMU_STATUS_SMUPRGERR_DEFAULT             (_SMU_STATUS_SMUPRGERR_DEFAULT << 1)   /**< Shifted mode DEFAULT for SMU_STATUS         */

/* Bit fields for SMU LOCK */
#define _SMU_LOCK_RESETVALUE                     0x00000000UL                           /**< Default value for SMU_LOCK                  */
#define _SMU_LOCK_MASK                           0x00FFFFFFUL                           /**< Mask for SMU_LOCK                           */
#define _SMU_LOCK_SMULOCKKEY_SHIFT               0                                      /**< Shift value for SMU_SMULOCKKEY              */
#define _SMU_LOCK_SMULOCKKEY_MASK                0xFFFFFFUL                             /**< Bit mask for SMU_SMULOCKKEY                 */
#define _SMU_LOCK_SMULOCKKEY_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for SMU_LOCK                   */
#define _SMU_LOCK_SMULOCKKEY_UNLOCK              0x00ACCE55UL                           /**< Mode UNLOCK for SMU_LOCK                    */
#define SMU_LOCK_SMULOCKKEY_DEFAULT              (_SMU_LOCK_SMULOCKKEY_DEFAULT << 0)    /**< Shifted mode DEFAULT for SMU_LOCK           */
#define SMU_LOCK_SMULOCKKEY_UNLOCK               (_SMU_LOCK_SMULOCKKEY_UNLOCK << 0)     /**< Shifted mode UNLOCK for SMU_LOCK            */

/* Bit fields for SMU IF */
#define _SMU_IF_RESETVALUE                       0x00000000UL                           /**< Default value for SMU_IF                    */
#define _SMU_IF_MASK                             0x00030005UL                           /**< Mask for SMU_IF                             */
#define SMU_IF_PPUPRIV                           (0x1UL << 0)                           /**< PPU Privilege Interrupt Flag                */
#define _SMU_IF_PPUPRIV_SHIFT                    0                                      /**< Shift value for SMU_PPUPRIV                 */
#define _SMU_IF_PPUPRIV_MASK                     0x1UL                                  /**< Bit mask for SMU_PPUPRIV                    */
#define _SMU_IF_PPUPRIV_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for SMU_IF                     */
#define SMU_IF_PPUPRIV_DEFAULT                   (_SMU_IF_PPUPRIV_DEFAULT << 0)         /**< Shifted mode DEFAULT for SMU_IF             */
#define SMU_IF_PPUINST                           (0x1UL << 2)                           /**< PPU Instruction Interrupt Flag              */
#define _SMU_IF_PPUINST_SHIFT                    2                                      /**< Shift value for SMU_PPUINST                 */
#define _SMU_IF_PPUINST_MASK                     0x4UL                                  /**< Bit mask for SMU_PPUINST                    */
#define _SMU_IF_PPUINST_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for SMU_IF                     */
#define SMU_IF_PPUINST_DEFAULT                   (_SMU_IF_PPUINST_DEFAULT << 2)         /**< Shifted mode DEFAULT for SMU_IF             */
#define SMU_IF_PPUSEC                            (0x1UL << 16)                          /**< PPU Security Interrupt Flag                 */
#define _SMU_IF_PPUSEC_SHIFT                     16                                     /**< Shift value for SMU_PPUSEC                  */
#define _SMU_IF_PPUSEC_MASK                      0x10000UL                              /**< Bit mask for SMU_PPUSEC                     */
#define _SMU_IF_PPUSEC_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for SMU_IF                     */
#define SMU_IF_PPUSEC_DEFAULT                    (_SMU_IF_PPUSEC_DEFAULT << 16)         /**< Shifted mode DEFAULT for SMU_IF             */
#define SMU_IF_BMPUSEC                           (0x1UL << 17)                          /**< BMPU Security Interrupt Flag                */
#define _SMU_IF_BMPUSEC_SHIFT                    17                                     /**< Shift value for SMU_BMPUSEC                 */
#define _SMU_IF_BMPUSEC_MASK                     0x20000UL                              /**< Bit mask for SMU_BMPUSEC                    */
#define _SMU_IF_BMPUSEC_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for SMU_IF                     */
#define SMU_IF_BMPUSEC_DEFAULT                   (_SMU_IF_BMPUSEC_DEFAULT << 17)        /**< Shifted mode DEFAULT for SMU_IF             */

/* Bit fields for SMU IEN */
#define _SMU_IEN_RESETVALUE                      0x00000000UL                           /**< Default value for SMU_IEN                   */
#define _SMU_IEN_MASK                            0x00030005UL                           /**< Mask for SMU_IEN                            */
#define SMU_IEN_PPUPRIV                          (0x1UL << 0)                           /**< PPU Privilege Interrupt Enable              */
#define _SMU_IEN_PPUPRIV_SHIFT                   0                                      /**< Shift value for SMU_PPUPRIV                 */
#define _SMU_IEN_PPUPRIV_MASK                    0x1UL                                  /**< Bit mask for SMU_PPUPRIV                    */
#define _SMU_IEN_PPUPRIV_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SMU_IEN                    */
#define SMU_IEN_PPUPRIV_DEFAULT                  (_SMU_IEN_PPUPRIV_DEFAULT << 0)        /**< Shifted mode DEFAULT for SMU_IEN            */
#define SMU_IEN_PPUINST                          (0x1UL << 2)                           /**< PPU Instruction Interrupt Enable            */
#define _SMU_IEN_PPUINST_SHIFT                   2                                      /**< Shift value for SMU_PPUINST                 */
#define _SMU_IEN_PPUINST_MASK                    0x4UL                                  /**< Bit mask for SMU_PPUINST                    */
#define _SMU_IEN_PPUINST_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SMU_IEN                    */
#define SMU_IEN_PPUINST_DEFAULT                  (_SMU_IEN_PPUINST_DEFAULT << 2)        /**< Shifted mode DEFAULT for SMU_IEN            */
#define SMU_IEN_PPUSEC                           (0x1UL << 16)                          /**< PPU Security Interrupt Enable               */
#define _SMU_IEN_PPUSEC_SHIFT                    16                                     /**< Shift value for SMU_PPUSEC                  */
#define _SMU_IEN_PPUSEC_MASK                     0x10000UL                              /**< Bit mask for SMU_PPUSEC                     */
#define _SMU_IEN_PPUSEC_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for SMU_IEN                    */
#define SMU_IEN_PPUSEC_DEFAULT                   (_SMU_IEN_PPUSEC_DEFAULT << 16)        /**< Shifted mode DEFAULT for SMU_IEN            */
#define SMU_IEN_BMPUSEC                          (0x1UL << 17)                          /**< BMPU Security Interrupt Enable              */
#define _SMU_IEN_BMPUSEC_SHIFT                   17                                     /**< Shift value for SMU_BMPUSEC                 */
#define _SMU_IEN_BMPUSEC_MASK                    0x20000UL                              /**< Bit mask for SMU_BMPUSEC                    */
#define _SMU_IEN_BMPUSEC_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for SMU_IEN                    */
#define SMU_IEN_BMPUSEC_DEFAULT                  (_SMU_IEN_BMPUSEC_DEFAULT << 17)       /**< Shifted mode DEFAULT for SMU_IEN            */

/* Bit fields for SMU M33CTRL */
#define _SMU_M33CTRL_RESETVALUE                  0x00000000UL                             /**< Default value for SMU_M33CTRL               */
#define _SMU_M33CTRL_MASK                        0x0000001FUL                             /**< Mask for SMU_M33CTRL                        */
#define SMU_M33CTRL_LOCKSVTAIRCR                 (0x1UL << 0)                             /**< New BitField                                */
#define _SMU_M33CTRL_LOCKSVTAIRCR_SHIFT          0                                        /**< Shift value for SMU_LOCKSVTAIRCR            */
#define _SMU_M33CTRL_LOCKSVTAIRCR_MASK           0x1UL                                    /**< Bit mask for SMU_LOCKSVTAIRCR               */
#define _SMU_M33CTRL_LOCKSVTAIRCR_DEFAULT        0x00000000UL                             /**< Mode DEFAULT for SMU_M33CTRL                */
#define SMU_M33CTRL_LOCKSVTAIRCR_DEFAULT         (_SMU_M33CTRL_LOCKSVTAIRCR_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_M33CTRL        */
#define SMU_M33CTRL_LOCKNSVTOR                   (0x1UL << 1)                             /**< New BitField                                */
#define _SMU_M33CTRL_LOCKNSVTOR_SHIFT            1                                        /**< Shift value for SMU_LOCKNSVTOR              */
#define _SMU_M33CTRL_LOCKNSVTOR_MASK             0x2UL                                    /**< Bit mask for SMU_LOCKNSVTOR                 */
#define _SMU_M33CTRL_LOCKNSVTOR_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for SMU_M33CTRL                */
#define SMU_M33CTRL_LOCKNSVTOR_DEFAULT           (_SMU_M33CTRL_LOCKNSVTOR_DEFAULT << 1)   /**< Shifted mode DEFAULT for SMU_M33CTRL        */
#define SMU_M33CTRL_LOCKSMPU                     (0x1UL << 2)                             /**< New BitField                                */
#define _SMU_M33CTRL_LOCKSMPU_SHIFT              2                                        /**< Shift value for SMU_LOCKSMPU                */
#define _SMU_M33CTRL_LOCKSMPU_MASK               0x4UL                                    /**< Bit mask for SMU_LOCKSMPU                   */
#define _SMU_M33CTRL_LOCKSMPU_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for SMU_M33CTRL                */
#define SMU_M33CTRL_LOCKSMPU_DEFAULT             (_SMU_M33CTRL_LOCKSMPU_DEFAULT << 2)     /**< Shifted mode DEFAULT for SMU_M33CTRL        */
#define SMU_M33CTRL_LOCKNSMPU                    (0x1UL << 3)                             /**< New BitField                                */
#define _SMU_M33CTRL_LOCKNSMPU_SHIFT             3                                        /**< Shift value for SMU_LOCKNSMPU               */
#define _SMU_M33CTRL_LOCKNSMPU_MASK              0x8UL                                    /**< Bit mask for SMU_LOCKNSMPU                  */
#define _SMU_M33CTRL_LOCKNSMPU_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for SMU_M33CTRL                */
#define SMU_M33CTRL_LOCKNSMPU_DEFAULT            (_SMU_M33CTRL_LOCKNSMPU_DEFAULT << 3)    /**< Shifted mode DEFAULT for SMU_M33CTRL        */
#define SMU_M33CTRL_LOCKSAU                      (0x1UL << 4)                             /**< New BitField                                */
#define _SMU_M33CTRL_LOCKSAU_SHIFT               4                                        /**< Shift value for SMU_LOCKSAU                 */
#define _SMU_M33CTRL_LOCKSAU_MASK                0x10UL                                   /**< Bit mask for SMU_LOCKSAU                    */
#define _SMU_M33CTRL_LOCKSAU_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for SMU_M33CTRL                */
#define SMU_M33CTRL_LOCKSAU_DEFAULT              (_SMU_M33CTRL_LOCKSAU_DEFAULT << 4)      /**< Shifted mode DEFAULT for SMU_M33CTRL        */

/* Bit fields for SMU PPUPATD0 */
#define _SMU_PPUPATD0_RESETVALUE                 0xFFFFFFFFUL                               /**< Default value for SMU_PPUPATD0              */
#define _SMU_PPUPATD0_MASK                       0xFFFFFFFFUL                               /**< Mask for SMU_PPUPATD0                       */
#define SMU_PPUPATD0_EMU                         (0x1UL << 1)                               /**< EMU Privileged Access                       */
#define _SMU_PPUPATD0_EMU_SHIFT                  1                                          /**< Shift value for SMU_EMU                     */
#define _SMU_PPUPATD0_EMU_MASK                   0x2UL                                      /**< Bit mask for SMU_EMU                        */
#define _SMU_PPUPATD0_EMU_DEFAULT                0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_EMU_DEFAULT                 (_SMU_PPUPATD0_EMU_DEFAULT << 1)           /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_CMU                         (0x1UL << 2)                               /**< CMU Privileged Access                       */
#define _SMU_PPUPATD0_CMU_SHIFT                  2                                          /**< Shift value for SMU_CMU                     */
#define _SMU_PPUPATD0_CMU_MASK                   0x4UL                                      /**< Bit mask for SMU_CMU                        */
#define _SMU_PPUPATD0_CMU_DEFAULT                0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_CMU_DEFAULT                 (_SMU_PPUPATD0_CMU_DEFAULT << 2)           /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_HFRCO0                      (0x1UL << 3)                               /**< HFRCO0 Privileged Access                    */
#define _SMU_PPUPATD0_HFRCO0_SHIFT               3                                          /**< Shift value for SMU_HFRCO0                  */
#define _SMU_PPUPATD0_HFRCO0_MASK                0x8UL                                      /**< Bit mask for SMU_HFRCO0                     */
#define _SMU_PPUPATD0_HFRCO0_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_HFRCO0_DEFAULT              (_SMU_PPUPATD0_HFRCO0_DEFAULT << 3)        /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_FSRCO                       (0x1UL << 4)                               /**< FSRCO Privileged Access                     */
#define _SMU_PPUPATD0_FSRCO_SHIFT                4                                          /**< Shift value for SMU_FSRCO                   */
#define _SMU_PPUPATD0_FSRCO_MASK                 0x10UL                                     /**< Bit mask for SMU_FSRCO                      */
#define _SMU_PPUPATD0_FSRCO_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_FSRCO_DEFAULT               (_SMU_PPUPATD0_FSRCO_DEFAULT << 4)         /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_DPLL0                       (0x1UL << 5)                               /**< DPLL0 Privileged Access                     */
#define _SMU_PPUPATD0_DPLL0_SHIFT                5                                          /**< Shift value for SMU_DPLL0                   */
#define _SMU_PPUPATD0_DPLL0_MASK                 0x20UL                                     /**< Bit mask for SMU_DPLL0                      */
#define _SMU_PPUPATD0_DPLL0_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_DPLL0_DEFAULT               (_SMU_PPUPATD0_DPLL0_DEFAULT << 5)         /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_LFXO                        (0x1UL << 6)                               /**< LFXO Privileged Access                      */
#define _SMU_PPUPATD0_LFXO_SHIFT                 6                                          /**< Shift value for SMU_LFXO                    */
#define _SMU_PPUPATD0_LFXO_MASK                  0x40UL                                     /**< Bit mask for SMU_LFXO                       */
#define _SMU_PPUPATD0_LFXO_DEFAULT               0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_LFXO_DEFAULT                (_SMU_PPUPATD0_LFXO_DEFAULT << 6)          /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_LFRCO                       (0x1UL << 7)                               /**< LFRCO Privileged Access                     */
#define _SMU_PPUPATD0_LFRCO_SHIFT                7                                          /**< Shift value for SMU_LFRCO                   */
#define _SMU_PPUPATD0_LFRCO_MASK                 0x80UL                                     /**< Bit mask for SMU_LFRCO                      */
#define _SMU_PPUPATD0_LFRCO_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_LFRCO_DEFAULT               (_SMU_PPUPATD0_LFRCO_DEFAULT << 7)         /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_ULFRCO                      (0x1UL << 8)                               /**< ULFRCO Privileged Access                    */
#define _SMU_PPUPATD0_ULFRCO_SHIFT               8                                          /**< Shift value for SMU_ULFRCO                  */
#define _SMU_PPUPATD0_ULFRCO_MASK                0x100UL                                    /**< Bit mask for SMU_ULFRCO                     */
#define _SMU_PPUPATD0_ULFRCO_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_ULFRCO_DEFAULT              (_SMU_PPUPATD0_ULFRCO_DEFAULT << 8)        /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_MSC                         (0x1UL << 9)                               /**< MSC Privileged Access                       */
#define _SMU_PPUPATD0_MSC_SHIFT                  9                                          /**< Shift value for SMU_MSC                     */
#define _SMU_PPUPATD0_MSC_MASK                   0x200UL                                    /**< Bit mask for SMU_MSC                        */
#define _SMU_PPUPATD0_MSC_DEFAULT                0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_MSC_DEFAULT                 (_SMU_PPUPATD0_MSC_DEFAULT << 9)           /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_ICACHE0                     (0x1UL << 10)                              /**< ICACHE0 Privileged Access                   */
#define _SMU_PPUPATD0_ICACHE0_SHIFT              10                                         /**< Shift value for SMU_ICACHE0                 */
#define _SMU_PPUPATD0_ICACHE0_MASK               0x400UL                                    /**< Bit mask for SMU_ICACHE0                    */
#define _SMU_PPUPATD0_ICACHE0_DEFAULT            0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_ICACHE0_DEFAULT             (_SMU_PPUPATD0_ICACHE0_DEFAULT << 10)      /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_PRS                         (0x1UL << 11)                              /**< PRS Privileged Access                       */
#define _SMU_PPUPATD0_PRS_SHIFT                  11                                         /**< Shift value for SMU_PRS                     */
#define _SMU_PPUPATD0_PRS_MASK                   0x800UL                                    /**< Bit mask for SMU_PRS                        */
#define _SMU_PPUPATD0_PRS_DEFAULT                0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_PRS_DEFAULT                 (_SMU_PPUPATD0_PRS_DEFAULT << 11)          /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_GPIO                        (0x1UL << 12)                              /**< GPIO Privileged Access                      */
#define _SMU_PPUPATD0_GPIO_SHIFT                 12                                         /**< Shift value for SMU_GPIO                    */
#define _SMU_PPUPATD0_GPIO_MASK                  0x1000UL                                   /**< Bit mask for SMU_GPIO                       */
#define _SMU_PPUPATD0_GPIO_DEFAULT               0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_GPIO_DEFAULT                (_SMU_PPUPATD0_GPIO_DEFAULT << 12)         /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_LDMA                        (0x1UL << 13)                              /**< LDMA Privileged Access                      */
#define _SMU_PPUPATD0_LDMA_SHIFT                 13                                         /**< Shift value for SMU_LDMA                    */
#define _SMU_PPUPATD0_LDMA_MASK                  0x2000UL                                   /**< Bit mask for SMU_LDMA                       */
#define _SMU_PPUPATD0_LDMA_DEFAULT               0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_LDMA_DEFAULT                (_SMU_PPUPATD0_LDMA_DEFAULT << 13)         /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_LDMAXBAR                    (0x1UL << 14)                              /**< LDMAXBAR Privileged Access                  */
#define _SMU_PPUPATD0_LDMAXBAR_SHIFT             14                                         /**< Shift value for SMU_LDMAXBAR                */
#define _SMU_PPUPATD0_LDMAXBAR_MASK              0x4000UL                                   /**< Bit mask for SMU_LDMAXBAR                   */
#define _SMU_PPUPATD0_LDMAXBAR_DEFAULT           0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_LDMAXBAR_DEFAULT            (_SMU_PPUPATD0_LDMAXBAR_DEFAULT << 14)     /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_TIMER0                      (0x1UL << 15)                              /**< TIMER0 Privileged Access                    */
#define _SMU_PPUPATD0_TIMER0_SHIFT               15                                         /**< Shift value for SMU_TIMER0                  */
#define _SMU_PPUPATD0_TIMER0_MASK                0x8000UL                                   /**< Bit mask for SMU_TIMER0                     */
#define _SMU_PPUPATD0_TIMER0_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_TIMER0_DEFAULT              (_SMU_PPUPATD0_TIMER0_DEFAULT << 15)       /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_TIMER1                      (0x1UL << 16)                              /**< TIMER1 Privileged Access                    */
#define _SMU_PPUPATD0_TIMER1_SHIFT               16                                         /**< Shift value for SMU_TIMER1                  */
#define _SMU_PPUPATD0_TIMER1_MASK                0x10000UL                                  /**< Bit mask for SMU_TIMER1                     */
#define _SMU_PPUPATD0_TIMER1_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_TIMER1_DEFAULT              (_SMU_PPUPATD0_TIMER1_DEFAULT << 16)       /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_TIMER2                      (0x1UL << 17)                              /**< TIMER2 Privileged Access                    */
#define _SMU_PPUPATD0_TIMER2_SHIFT               17                                         /**< Shift value for SMU_TIMER2                  */
#define _SMU_PPUPATD0_TIMER2_MASK                0x20000UL                                  /**< Bit mask for SMU_TIMER2                     */
#define _SMU_PPUPATD0_TIMER2_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_TIMER2_DEFAULT              (_SMU_PPUPATD0_TIMER2_DEFAULT << 17)       /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_TIMER3                      (0x1UL << 18)                              /**< TIMER3 Privileged Access                    */
#define _SMU_PPUPATD0_TIMER3_SHIFT               18                                         /**< Shift value for SMU_TIMER3                  */
#define _SMU_PPUPATD0_TIMER3_MASK                0x40000UL                                  /**< Bit mask for SMU_TIMER3                     */
#define _SMU_PPUPATD0_TIMER3_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_TIMER3_DEFAULT              (_SMU_PPUPATD0_TIMER3_DEFAULT << 18)       /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_TIMER4                      (0x1UL << 19)                              /**< TIMER4 Privileged Access                    */
#define _SMU_PPUPATD0_TIMER4_SHIFT               19                                         /**< Shift value for SMU_TIMER4                  */
#define _SMU_PPUPATD0_TIMER4_MASK                0x80000UL                                  /**< Bit mask for SMU_TIMER4                     */
#define _SMU_PPUPATD0_TIMER4_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_TIMER4_DEFAULT              (_SMU_PPUPATD0_TIMER4_DEFAULT << 19)       /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_USART0                      (0x1UL << 20)                              /**< USART0 Privileged Access                    */
#define _SMU_PPUPATD0_USART0_SHIFT               20                                         /**< Shift value for SMU_USART0                  */
#define _SMU_PPUPATD0_USART0_MASK                0x100000UL                                 /**< Bit mask for SMU_USART0                     */
#define _SMU_PPUPATD0_USART0_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_USART0_DEFAULT              (_SMU_PPUPATD0_USART0_DEFAULT << 20)       /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_BURTC                       (0x1UL << 21)                              /**< BURTC Privileged Access                     */
#define _SMU_PPUPATD0_BURTC_SHIFT                21                                         /**< Shift value for SMU_BURTC                   */
#define _SMU_PPUPATD0_BURTC_MASK                 0x200000UL                                 /**< Bit mask for SMU_BURTC                      */
#define _SMU_PPUPATD0_BURTC_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_BURTC_DEFAULT               (_SMU_PPUPATD0_BURTC_DEFAULT << 21)        /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_I2C1                        (0x1UL << 22)                              /**< I2C1 Privileged Access                      */
#define _SMU_PPUPATD0_I2C1_SHIFT                 22                                         /**< Shift value for SMU_I2C1                    */
#define _SMU_PPUPATD0_I2C1_MASK                  0x400000UL                                 /**< Bit mask for SMU_I2C1                       */
#define _SMU_PPUPATD0_I2C1_DEFAULT               0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_I2C1_DEFAULT                (_SMU_PPUPATD0_I2C1_DEFAULT << 22)         /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_CHIPTESTCTRL                (0x1UL << 23)                              /**< CHIPTESTCTRL Privileged Access              */
#define _SMU_PPUPATD0_CHIPTESTCTRL_SHIFT         23                                         /**< Shift value for SMU_CHIPTESTCTRL            */
#define _SMU_PPUPATD0_CHIPTESTCTRL_MASK          0x800000UL                                 /**< Bit mask for SMU_CHIPTESTCTRL               */
#define _SMU_PPUPATD0_CHIPTESTCTRL_DEFAULT       0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_CHIPTESTCTRL_DEFAULT        (_SMU_PPUPATD0_CHIPTESTCTRL_DEFAULT << 23) /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_SYSCFGCFGNS                 (0x1UL << 24)                              /**< SYSCFGCFGNS Privileged Access               */
#define _SMU_PPUPATD0_SYSCFGCFGNS_SHIFT          24                                         /**< Shift value for SMU_SYSCFGCFGNS             */
#define _SMU_PPUPATD0_SYSCFGCFGNS_MASK           0x1000000UL                                /**< Bit mask for SMU_SYSCFGCFGNS                */
#define _SMU_PPUPATD0_SYSCFGCFGNS_DEFAULT        0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_SYSCFGCFGNS_DEFAULT         (_SMU_PPUPATD0_SYSCFGCFGNS_DEFAULT << 24)  /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_SYSCFG                      (0x1UL << 25)                              /**< SYSCFG Privileged Access                    */
#define _SMU_PPUPATD0_SYSCFG_SHIFT               25                                         /**< Shift value for SMU_SYSCFG                  */
#define _SMU_PPUPATD0_SYSCFG_MASK                0x2000000UL                                /**< Bit mask for SMU_SYSCFG                     */
#define _SMU_PPUPATD0_SYSCFG_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_SYSCFG_DEFAULT              (_SMU_PPUPATD0_SYSCFG_DEFAULT << 25)       /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_BURAM                       (0x1UL << 26)                              /**< BURAM Privileged Access                     */
#define _SMU_PPUPATD0_BURAM_SHIFT                26                                         /**< Shift value for SMU_BURAM                   */
#define _SMU_PPUPATD0_BURAM_MASK                 0x4000000UL                                /**< Bit mask for SMU_BURAM                      */
#define _SMU_PPUPATD0_BURAM_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_BURAM_DEFAULT               (_SMU_PPUPATD0_BURAM_DEFAULT << 26)        /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_GPCRC                       (0x1UL << 27)                              /**< GPCRC Privileged Access                     */
#define _SMU_PPUPATD0_GPCRC_SHIFT                27                                         /**< Shift value for SMU_GPCRC                   */
#define _SMU_PPUPATD0_GPCRC_MASK                 0x8000000UL                                /**< Bit mask for SMU_GPCRC                      */
#define _SMU_PPUPATD0_GPCRC_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_GPCRC_DEFAULT               (_SMU_PPUPATD0_GPCRC_DEFAULT << 27)        /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_DCDC                        (0x1UL << 28)                              /**< DCDC Privileged Access                      */
#define _SMU_PPUPATD0_DCDC_SHIFT                 28                                         /**< Shift value for SMU_DCDC                    */
#define _SMU_PPUPATD0_DCDC_MASK                  0x10000000UL                               /**< Bit mask for SMU_DCDC                       */
#define _SMU_PPUPATD0_DCDC_DEFAULT               0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_DCDC_DEFAULT                (_SMU_PPUPATD0_DCDC_DEFAULT << 28)         /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_HOSTMAILBOX                 (0x1UL << 29)                              /**< HOSTMAILBOX Privileged Access               */
#define _SMU_PPUPATD0_HOSTMAILBOX_SHIFT          29                                         /**< Shift value for SMU_HOSTMAILBOX             */
#define _SMU_PPUPATD0_HOSTMAILBOX_MASK           0x20000000UL                               /**< Bit mask for SMU_HOSTMAILBOX                */
#define _SMU_PPUPATD0_HOSTMAILBOX_DEFAULT        0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_HOSTMAILBOX_DEFAULT         (_SMU_PPUPATD0_HOSTMAILBOX_DEFAULT << 29)  /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_EUSART1                     (0x1UL << 30)                              /**< EUSART1 Privileged Access                   */
#define _SMU_PPUPATD0_EUSART1_SHIFT              30                                         /**< Shift value for SMU_EUSART1                 */
#define _SMU_PPUPATD0_EUSART1_MASK               0x40000000UL                               /**< Bit mask for SMU_EUSART1                    */
#define _SMU_PPUPATD0_EUSART1_DEFAULT            0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_EUSART1_DEFAULT             (_SMU_PPUPATD0_EUSART1_DEFAULT << 30)      /**< Shifted mode DEFAULT for SMU_PPUPATD0       */
#define SMU_PPUPATD0_EUSART2                     (0x1UL << 31)                              /**< EUSART2 Privileged Access                   */
#define _SMU_PPUPATD0_EUSART2_SHIFT              31                                         /**< Shift value for SMU_EUSART2                 */
#define _SMU_PPUPATD0_EUSART2_MASK               0x80000000UL                               /**< Bit mask for SMU_EUSART2                    */
#define _SMU_PPUPATD0_EUSART2_DEFAULT            0x00000001UL                               /**< Mode DEFAULT for SMU_PPUPATD0               */
#define SMU_PPUPATD0_EUSART2_DEFAULT             (_SMU_PPUPATD0_EUSART2_DEFAULT << 31)      /**< Shifted mode DEFAULT for SMU_PPUPATD0       */

/* Bit fields for SMU PPUPATD1 */
#define _SMU_PPUPATD1_RESETVALUE                 0x01FFFFFFUL                            /**< Default value for SMU_PPUPATD1              */
#define _SMU_PPUPATD1_MASK                       0x01FFFFFFUL                            /**< Mask for SMU_PPUPATD1                       */
#define SMU_PPUPATD1_SYSRTC                      (0x1UL << 0)                            /**< SYSRTC Privileged Access                    */
#define _SMU_PPUPATD1_SYSRTC_SHIFT               0                                       /**< Shift value for SMU_SYSRTC                  */
#define _SMU_PPUPATD1_SYSRTC_MASK                0x1UL                                   /**< Bit mask for SMU_SYSRTC                     */
#define _SMU_PPUPATD1_SYSRTC_DEFAULT             0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_SYSRTC_DEFAULT              (_SMU_PPUPATD1_SYSRTC_DEFAULT << 0)     /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_LCD                         (0x1UL << 1)                            /**< LCD Privileged Access                       */
#define _SMU_PPUPATD1_LCD_SHIFT                  1                                       /**< Shift value for SMU_LCD                     */
#define _SMU_PPUPATD1_LCD_MASK                   0x2UL                                   /**< Bit mask for SMU_LCD                        */
#define _SMU_PPUPATD1_LCD_DEFAULT                0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_LCD_DEFAULT                 (_SMU_PPUPATD1_LCD_DEFAULT << 1)        /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_KEYSCAN                     (0x1UL << 2)                            /**< KEYSCAN Privileged Access                   */
#define _SMU_PPUPATD1_KEYSCAN_SHIFT              2                                       /**< Shift value for SMU_KEYSCAN                 */
#define _SMU_PPUPATD1_KEYSCAN_MASK               0x4UL                                   /**< Bit mask for SMU_KEYSCAN                    */
#define _SMU_PPUPATD1_KEYSCAN_DEFAULT            0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_KEYSCAN_DEFAULT             (_SMU_PPUPATD1_KEYSCAN_DEFAULT << 2)    /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_DMEM                        (0x1UL << 3)                            /**< DMEM Privileged Access                      */
#define _SMU_PPUPATD1_DMEM_SHIFT                 3                                       /**< Shift value for SMU_DMEM                    */
#define _SMU_PPUPATD1_DMEM_MASK                  0x8UL                                   /**< Bit mask for SMU_DMEM                       */
#define _SMU_PPUPATD1_DMEM_DEFAULT               0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_DMEM_DEFAULT                (_SMU_PPUPATD1_DMEM_DEFAULT << 3)       /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_LCDRF                       (0x1UL << 4)                            /**< LCDRF Privileged Access                     */
#define _SMU_PPUPATD1_LCDRF_SHIFT                4                                       /**< Shift value for SMU_LCDRF                   */
#define _SMU_PPUPATD1_LCDRF_MASK                 0x10UL                                  /**< Bit mask for SMU_LCDRF                      */
#define _SMU_PPUPATD1_LCDRF_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_LCDRF_DEFAULT               (_SMU_PPUPATD1_LCDRF_DEFAULT << 4)      /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_PFMXPPRF                    (0x1UL << 5)                            /**< PFMXPPRF Privileged Access                  */
#define _SMU_PPUPATD1_PFMXPPRF_SHIFT             5                                       /**< Shift value for SMU_PFMXPPRF                */
#define _SMU_PPUPATD1_PFMXPPRF_MASK              0x20UL                                  /**< Bit mask for SMU_PFMXPPRF                   */
#define _SMU_PPUPATD1_PFMXPPRF_DEFAULT           0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_PFMXPPRF_DEFAULT            (_SMU_PPUPATD1_PFMXPPRF_DEFAULT << 5)   /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_RADIOAES                    (0x1UL << 6)                            /**< RADIOAES Privileged Access                  */
#define _SMU_PPUPATD1_RADIOAES_SHIFT             6                                       /**< Shift value for SMU_RADIOAES                */
#define _SMU_PPUPATD1_RADIOAES_MASK              0x40UL                                  /**< Bit mask for SMU_RADIOAES                   */
#define _SMU_PPUPATD1_RADIOAES_DEFAULT           0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_RADIOAES_DEFAULT            (_SMU_PPUPATD1_RADIOAES_DEFAULT << 6)   /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_SMU                         (0x1UL << 7)                            /**< SMU Privileged Access                       */
#define _SMU_PPUPATD1_SMU_SHIFT                  7                                       /**< Shift value for SMU_SMU                     */
#define _SMU_PPUPATD1_SMU_MASK                   0x80UL                                  /**< Bit mask for SMU_SMU                        */
#define _SMU_PPUPATD1_SMU_DEFAULT                0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_SMU_DEFAULT                 (_SMU_PPUPATD1_SMU_DEFAULT << 7)        /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_SMUCFGNS                    (0x1UL << 8)                            /**< SMUCFGNS Privileged Access                  */
#define _SMU_PPUPATD1_SMUCFGNS_SHIFT             8                                       /**< Shift value for SMU_SMUCFGNS                */
#define _SMU_PPUPATD1_SMUCFGNS_MASK              0x100UL                                 /**< Bit mask for SMU_SMUCFGNS                   */
#define _SMU_PPUPATD1_SMUCFGNS_DEFAULT           0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_SMUCFGNS_DEFAULT            (_SMU_PPUPATD1_SMUCFGNS_DEFAULT << 8)   /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_LETIMER0                    (0x1UL << 9)                            /**< LETIMER0 Privileged Access                  */
#define _SMU_PPUPATD1_LETIMER0_SHIFT             9                                       /**< Shift value for SMU_LETIMER0                */
#define _SMU_PPUPATD1_LETIMER0_MASK              0x200UL                                 /**< Bit mask for SMU_LETIMER0                   */
#define _SMU_PPUPATD1_LETIMER0_DEFAULT           0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_LETIMER0_DEFAULT            (_SMU_PPUPATD1_LETIMER0_DEFAULT << 9)   /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_IADC0                       (0x1UL << 10)                           /**< IADC0 Privileged Access                     */
#define _SMU_PPUPATD1_IADC0_SHIFT                10                                      /**< Shift value for SMU_IADC0                   */
#define _SMU_PPUPATD1_IADC0_MASK                 0x400UL                                 /**< Bit mask for SMU_IADC0                      */
#define _SMU_PPUPATD1_IADC0_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_IADC0_DEFAULT               (_SMU_PPUPATD1_IADC0_DEFAULT << 10)     /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_ACMP0                       (0x1UL << 11)                           /**< ACMP0 Privileged Access                     */
#define _SMU_PPUPATD1_ACMP0_SHIFT                11                                      /**< Shift value for SMU_ACMP0                   */
#define _SMU_PPUPATD1_ACMP0_MASK                 0x800UL                                 /**< Bit mask for SMU_ACMP0                      */
#define _SMU_PPUPATD1_ACMP0_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_ACMP0_DEFAULT               (_SMU_PPUPATD1_ACMP0_DEFAULT << 11)     /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_ACMP1                       (0x1UL << 12)                           /**< ACMP1 Privileged Access                     */
#define _SMU_PPUPATD1_ACMP1_SHIFT                12                                      /**< Shift value for SMU_ACMP1                   */
#define _SMU_PPUPATD1_ACMP1_MASK                 0x1000UL                                /**< Bit mask for SMU_ACMP1                      */
#define _SMU_PPUPATD1_ACMP1_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_ACMP1_DEFAULT               (_SMU_PPUPATD1_ACMP1_DEFAULT << 12)     /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_AMUXCP0                     (0x1UL << 13)                           /**< AMUXCP0 Privileged Access                   */
#define _SMU_PPUPATD1_AMUXCP0_SHIFT              13                                      /**< Shift value for SMU_AMUXCP0                 */
#define _SMU_PPUPATD1_AMUXCP0_MASK               0x2000UL                                /**< Bit mask for SMU_AMUXCP0                    */
#define _SMU_PPUPATD1_AMUXCP0_DEFAULT            0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_AMUXCP0_DEFAULT             (_SMU_PPUPATD1_AMUXCP0_DEFAULT << 13)   /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_VDAC0                       (0x1UL << 14)                           /**< VDAC0 Privileged Access                     */
#define _SMU_PPUPATD1_VDAC0_SHIFT                14                                      /**< Shift value for SMU_VDAC0                   */
#define _SMU_PPUPATD1_VDAC0_MASK                 0x4000UL                                /**< Bit mask for SMU_VDAC0                      */
#define _SMU_PPUPATD1_VDAC0_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_VDAC0_DEFAULT               (_SMU_PPUPATD1_VDAC0_DEFAULT << 14)     /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_PCNT                        (0x1UL << 15)                           /**< PCNT Privileged Access                      */
#define _SMU_PPUPATD1_PCNT_SHIFT                 15                                      /**< Shift value for SMU_PCNT                    */
#define _SMU_PPUPATD1_PCNT_MASK                  0x8000UL                                /**< Bit mask for SMU_PCNT                       */
#define _SMU_PPUPATD1_PCNT_DEFAULT               0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_PCNT_DEFAULT                (_SMU_PPUPATD1_PCNT_DEFAULT << 15)      /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_LESENSE                     (0x1UL << 16)                           /**< LESENSE Privileged Access                   */
#define _SMU_PPUPATD1_LESENSE_SHIFT              16                                      /**< Shift value for SMU_LESENSE                 */
#define _SMU_PPUPATD1_LESENSE_MASK               0x10000UL                               /**< Bit mask for SMU_LESENSE                    */
#define _SMU_PPUPATD1_LESENSE_DEFAULT            0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_LESENSE_DEFAULT             (_SMU_PPUPATD1_LESENSE_DEFAULT << 16)   /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_HFRCO1                      (0x1UL << 17)                           /**< HFRCO1 Privileged Access                    */
#define _SMU_PPUPATD1_HFRCO1_SHIFT               17                                      /**< Shift value for SMU_HFRCO1                  */
#define _SMU_PPUPATD1_HFRCO1_MASK                0x20000UL                               /**< Bit mask for SMU_HFRCO1                     */
#define _SMU_PPUPATD1_HFRCO1_DEFAULT             0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_HFRCO1_DEFAULT              (_SMU_PPUPATD1_HFRCO1_DEFAULT << 17)    /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_HFXO0                       (0x1UL << 18)                           /**< HFXO0 Privileged Access                     */
#define _SMU_PPUPATD1_HFXO0_SHIFT                18                                      /**< Shift value for SMU_HFXO0                   */
#define _SMU_PPUPATD1_HFXO0_MASK                 0x40000UL                               /**< Bit mask for SMU_HFXO0                      */
#define _SMU_PPUPATD1_HFXO0_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_HFXO0_DEFAULT               (_SMU_PPUPATD1_HFXO0_DEFAULT << 18)     /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_I2C0                        (0x1UL << 19)                           /**< I2C0 Privileged Access                      */
#define _SMU_PPUPATD1_I2C0_SHIFT                 19                                      /**< Shift value for SMU_I2C0                    */
#define _SMU_PPUPATD1_I2C0_MASK                  0x80000UL                               /**< Bit mask for SMU_I2C0                       */
#define _SMU_PPUPATD1_I2C0_DEFAULT               0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_I2C0_DEFAULT                (_SMU_PPUPATD1_I2C0_DEFAULT << 19)      /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_WDOG0                       (0x1UL << 20)                           /**< WDOG0 Privileged Access                     */
#define _SMU_PPUPATD1_WDOG0_SHIFT                20                                      /**< Shift value for SMU_WDOG0                   */
#define _SMU_PPUPATD1_WDOG0_MASK                 0x100000UL                              /**< Bit mask for SMU_WDOG0                      */
#define _SMU_PPUPATD1_WDOG0_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_WDOG0_DEFAULT               (_SMU_PPUPATD1_WDOG0_DEFAULT << 20)     /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_WDOG1                       (0x1UL << 21)                           /**< WDOG1 Privileged Access                     */
#define _SMU_PPUPATD1_WDOG1_SHIFT                21                                      /**< Shift value for SMU_WDOG1                   */
#define _SMU_PPUPATD1_WDOG1_MASK                 0x200000UL                              /**< Bit mask for SMU_WDOG1                      */
#define _SMU_PPUPATD1_WDOG1_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_WDOG1_DEFAULT               (_SMU_PPUPATD1_WDOG1_DEFAULT << 21)     /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_EUSART0                     (0x1UL << 22)                           /**< EUSART0 Privileged Access                   */
#define _SMU_PPUPATD1_EUSART0_SHIFT              22                                      /**< Shift value for SMU_EUSART0                 */
#define _SMU_PPUPATD1_EUSART0_MASK               0x400000UL                              /**< Bit mask for SMU_EUSART0                    */
#define _SMU_PPUPATD1_EUSART0_DEFAULT            0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_EUSART0_DEFAULT             (_SMU_PPUPATD1_EUSART0_DEFAULT << 22)   /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_SEMAILBOX                   (0x1UL << 23)                           /**< SEMAILBOX Privileged Access                 */
#define _SMU_PPUPATD1_SEMAILBOX_SHIFT            23                                      /**< Shift value for SMU_SEMAILBOX               */
#define _SMU_PPUPATD1_SEMAILBOX_MASK             0x800000UL                              /**< Bit mask for SMU_SEMAILBOX                  */
#define _SMU_PPUPATD1_SEMAILBOX_DEFAULT          0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_SEMAILBOX_DEFAULT           (_SMU_PPUPATD1_SEMAILBOX_DEFAULT << 23) /**< Shifted mode DEFAULT for SMU_PPUPATD1       */
#define SMU_PPUPATD1_AHBRADIO                    (0x1UL << 24)                           /**< AHBRADIO Privileged Access                  */
#define _SMU_PPUPATD1_AHBRADIO_SHIFT             24                                      /**< Shift value for SMU_AHBRADIO                */
#define _SMU_PPUPATD1_AHBRADIO_MASK              0x1000000UL                             /**< Bit mask for SMU_AHBRADIO                   */
#define _SMU_PPUPATD1_AHBRADIO_DEFAULT           0x00000001UL                            /**< Mode DEFAULT for SMU_PPUPATD1               */
#define SMU_PPUPATD1_AHBRADIO_DEFAULT            (_SMU_PPUPATD1_AHBRADIO_DEFAULT << 24)  /**< Shifted mode DEFAULT for SMU_PPUPATD1       */

/* Bit fields for SMU PPUSATD0 */
#define _SMU_PPUSATD0_RESETVALUE                 0xFFFFFFFFUL                               /**< Default value for SMU_PPUSATD0              */
#define _SMU_PPUSATD0_MASK                       0xFFFFFFFFUL                               /**< Mask for SMU_PPUSATD0                       */
#define SMU_PPUSATD0_EMU                         (0x1UL << 1)                               /**< EMU Secure Access                           */
#define _SMU_PPUSATD0_EMU_SHIFT                  1                                          /**< Shift value for SMU_EMU                     */
#define _SMU_PPUSATD0_EMU_MASK                   0x2UL                                      /**< Bit mask for SMU_EMU                        */
#define _SMU_PPUSATD0_EMU_DEFAULT                0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_EMU_DEFAULT                 (_SMU_PPUSATD0_EMU_DEFAULT << 1)           /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_CMU                         (0x1UL << 2)                               /**< CMU Secure Access                           */
#define _SMU_PPUSATD0_CMU_SHIFT                  2                                          /**< Shift value for SMU_CMU                     */
#define _SMU_PPUSATD0_CMU_MASK                   0x4UL                                      /**< Bit mask for SMU_CMU                        */
#define _SMU_PPUSATD0_CMU_DEFAULT                0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_CMU_DEFAULT                 (_SMU_PPUSATD0_CMU_DEFAULT << 2)           /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_HFRCO0                      (0x1UL << 3)                               /**< HFRCO0 Secure Access                        */
#define _SMU_PPUSATD0_HFRCO0_SHIFT               3                                          /**< Shift value for SMU_HFRCO0                  */
#define _SMU_PPUSATD0_HFRCO0_MASK                0x8UL                                      /**< Bit mask for SMU_HFRCO0                     */
#define _SMU_PPUSATD0_HFRCO0_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_HFRCO0_DEFAULT              (_SMU_PPUSATD0_HFRCO0_DEFAULT << 3)        /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_FSRCO                       (0x1UL << 4)                               /**< FSRCO Secure Access                         */
#define _SMU_PPUSATD0_FSRCO_SHIFT                4                                          /**< Shift value for SMU_FSRCO                   */
#define _SMU_PPUSATD0_FSRCO_MASK                 0x10UL                                     /**< Bit mask for SMU_FSRCO                      */
#define _SMU_PPUSATD0_FSRCO_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_FSRCO_DEFAULT               (_SMU_PPUSATD0_FSRCO_DEFAULT << 4)         /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_DPLL0                       (0x1UL << 5)                               /**< DPLL0 Secure Access                         */
#define _SMU_PPUSATD0_DPLL0_SHIFT                5                                          /**< Shift value for SMU_DPLL0                   */
#define _SMU_PPUSATD0_DPLL0_MASK                 0x20UL                                     /**< Bit mask for SMU_DPLL0                      */
#define _SMU_PPUSATD0_DPLL0_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_DPLL0_DEFAULT               (_SMU_PPUSATD0_DPLL0_DEFAULT << 5)         /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_LFXO                        (0x1UL << 6)                               /**< LFXO Secure Access                          */
#define _SMU_PPUSATD0_LFXO_SHIFT                 6                                          /**< Shift value for SMU_LFXO                    */
#define _SMU_PPUSATD0_LFXO_MASK                  0x40UL                                     /**< Bit mask for SMU_LFXO                       */
#define _SMU_PPUSATD0_LFXO_DEFAULT               0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_LFXO_DEFAULT                (_SMU_PPUSATD0_LFXO_DEFAULT << 6)          /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_LFRCO                       (0x1UL << 7)                               /**< LFRCO Secure Access                         */
#define _SMU_PPUSATD0_LFRCO_SHIFT                7                                          /**< Shift value for SMU_LFRCO                   */
#define _SMU_PPUSATD0_LFRCO_MASK                 0x80UL                                     /**< Bit mask for SMU_LFRCO                      */
#define _SMU_PPUSATD0_LFRCO_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_LFRCO_DEFAULT               (_SMU_PPUSATD0_LFRCO_DEFAULT << 7)         /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_ULFRCO                      (0x1UL << 8)                               /**< ULFRCO Secure Access                        */
#define _SMU_PPUSATD0_ULFRCO_SHIFT               8                                          /**< Shift value for SMU_ULFRCO                  */
#define _SMU_PPUSATD0_ULFRCO_MASK                0x100UL                                    /**< Bit mask for SMU_ULFRCO                     */
#define _SMU_PPUSATD0_ULFRCO_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_ULFRCO_DEFAULT              (_SMU_PPUSATD0_ULFRCO_DEFAULT << 8)        /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_MSC                         (0x1UL << 9)                               /**< MSC Secure Access                           */
#define _SMU_PPUSATD0_MSC_SHIFT                  9                                          /**< Shift value for SMU_MSC                     */
#define _SMU_PPUSATD0_MSC_MASK                   0x200UL                                    /**< Bit mask for SMU_MSC                        */
#define _SMU_PPUSATD0_MSC_DEFAULT                0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_MSC_DEFAULT                 (_SMU_PPUSATD0_MSC_DEFAULT << 9)           /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_ICACHE0                     (0x1UL << 10)                              /**< ICACHE0 Secure Access                       */
#define _SMU_PPUSATD0_ICACHE0_SHIFT              10                                         /**< Shift value for SMU_ICACHE0                 */
#define _SMU_PPUSATD0_ICACHE0_MASK               0x400UL                                    /**< Bit mask for SMU_ICACHE0                    */
#define _SMU_PPUSATD0_ICACHE0_DEFAULT            0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_ICACHE0_DEFAULT             (_SMU_PPUSATD0_ICACHE0_DEFAULT << 10)      /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_PRS                         (0x1UL << 11)                              /**< PRS Secure Access                           */
#define _SMU_PPUSATD0_PRS_SHIFT                  11                                         /**< Shift value for SMU_PRS                     */
#define _SMU_PPUSATD0_PRS_MASK                   0x800UL                                    /**< Bit mask for SMU_PRS                        */
#define _SMU_PPUSATD0_PRS_DEFAULT                0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_PRS_DEFAULT                 (_SMU_PPUSATD0_PRS_DEFAULT << 11)          /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_GPIO                        (0x1UL << 12)                              /**< GPIO Secure Access                          */
#define _SMU_PPUSATD0_GPIO_SHIFT                 12                                         /**< Shift value for SMU_GPIO                    */
#define _SMU_PPUSATD0_GPIO_MASK                  0x1000UL                                   /**< Bit mask for SMU_GPIO                       */
#define _SMU_PPUSATD0_GPIO_DEFAULT               0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_GPIO_DEFAULT                (_SMU_PPUSATD0_GPIO_DEFAULT << 12)         /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_LDMA                        (0x1UL << 13)                              /**< LDMA Secure Access                          */
#define _SMU_PPUSATD0_LDMA_SHIFT                 13                                         /**< Shift value for SMU_LDMA                    */
#define _SMU_PPUSATD0_LDMA_MASK                  0x2000UL                                   /**< Bit mask for SMU_LDMA                       */
#define _SMU_PPUSATD0_LDMA_DEFAULT               0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_LDMA_DEFAULT                (_SMU_PPUSATD0_LDMA_DEFAULT << 13)         /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_LDMAXBAR                    (0x1UL << 14)                              /**< LDMAXBAR Secure Access                      */
#define _SMU_PPUSATD0_LDMAXBAR_SHIFT             14                                         /**< Shift value for SMU_LDMAXBAR                */
#define _SMU_PPUSATD0_LDMAXBAR_MASK              0x4000UL                                   /**< Bit mask for SMU_LDMAXBAR                   */
#define _SMU_PPUSATD0_LDMAXBAR_DEFAULT           0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_LDMAXBAR_DEFAULT            (_SMU_PPUSATD0_LDMAXBAR_DEFAULT << 14)     /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_TIMER0                      (0x1UL << 15)                              /**< TIMER0 Secure Access                        */
#define _SMU_PPUSATD0_TIMER0_SHIFT               15                                         /**< Shift value for SMU_TIMER0                  */
#define _SMU_PPUSATD0_TIMER0_MASK                0x8000UL                                   /**< Bit mask for SMU_TIMER0                     */
#define _SMU_PPUSATD0_TIMER0_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_TIMER0_DEFAULT              (_SMU_PPUSATD0_TIMER0_DEFAULT << 15)       /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_TIMER1                      (0x1UL << 16)                              /**< TIMER1 Secure Access                        */
#define _SMU_PPUSATD0_TIMER1_SHIFT               16                                         /**< Shift value for SMU_TIMER1                  */
#define _SMU_PPUSATD0_TIMER1_MASK                0x10000UL                                  /**< Bit mask for SMU_TIMER1                     */
#define _SMU_PPUSATD0_TIMER1_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_TIMER1_DEFAULT              (_SMU_PPUSATD0_TIMER1_DEFAULT << 16)       /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_TIMER2                      (0x1UL << 17)                              /**< TIMER2 Secure Access                        */
#define _SMU_PPUSATD0_TIMER2_SHIFT               17                                         /**< Shift value for SMU_TIMER2                  */
#define _SMU_PPUSATD0_TIMER2_MASK                0x20000UL                                  /**< Bit mask for SMU_TIMER2                     */
#define _SMU_PPUSATD0_TIMER2_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_TIMER2_DEFAULT              (_SMU_PPUSATD0_TIMER2_DEFAULT << 17)       /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_TIMER3                      (0x1UL << 18)                              /**< TIMER3 Secure Access                        */
#define _SMU_PPUSATD0_TIMER3_SHIFT               18                                         /**< Shift value for SMU_TIMER3                  */
#define _SMU_PPUSATD0_TIMER3_MASK                0x40000UL                                  /**< Bit mask for SMU_TIMER3                     */
#define _SMU_PPUSATD0_TIMER3_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_TIMER3_DEFAULT              (_SMU_PPUSATD0_TIMER3_DEFAULT << 18)       /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_TIMER4                      (0x1UL << 19)                              /**< TIMER4 Secure Access                        */
#define _SMU_PPUSATD0_TIMER4_SHIFT               19                                         /**< Shift value for SMU_TIMER4                  */
#define _SMU_PPUSATD0_TIMER4_MASK                0x80000UL                                  /**< Bit mask for SMU_TIMER4                     */
#define _SMU_PPUSATD0_TIMER4_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_TIMER4_DEFAULT              (_SMU_PPUSATD0_TIMER4_DEFAULT << 19)       /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_USART0                      (0x1UL << 20)                              /**< USART0 Secure Access                        */
#define _SMU_PPUSATD0_USART0_SHIFT               20                                         /**< Shift value for SMU_USART0                  */
#define _SMU_PPUSATD0_USART0_MASK                0x100000UL                                 /**< Bit mask for SMU_USART0                     */
#define _SMU_PPUSATD0_USART0_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_USART0_DEFAULT              (_SMU_PPUSATD0_USART0_DEFAULT << 20)       /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_BURTC                       (0x1UL << 21)                              /**< BURTC Secure Access                         */
#define _SMU_PPUSATD0_BURTC_SHIFT                21                                         /**< Shift value for SMU_BURTC                   */
#define _SMU_PPUSATD0_BURTC_MASK                 0x200000UL                                 /**< Bit mask for SMU_BURTC                      */
#define _SMU_PPUSATD0_BURTC_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_BURTC_DEFAULT               (_SMU_PPUSATD0_BURTC_DEFAULT << 21)        /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_I2C1                        (0x1UL << 22)                              /**< I2C1 Secure Access                          */
#define _SMU_PPUSATD0_I2C1_SHIFT                 22                                         /**< Shift value for SMU_I2C1                    */
#define _SMU_PPUSATD0_I2C1_MASK                  0x400000UL                                 /**< Bit mask for SMU_I2C1                       */
#define _SMU_PPUSATD0_I2C1_DEFAULT               0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_I2C1_DEFAULT                (_SMU_PPUSATD0_I2C1_DEFAULT << 22)         /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_CHIPTESTCTRL                (0x1UL << 23)                              /**< CHIPTESTCTRL Secure Access                  */
#define _SMU_PPUSATD0_CHIPTESTCTRL_SHIFT         23                                         /**< Shift value for SMU_CHIPTESTCTRL            */
#define _SMU_PPUSATD0_CHIPTESTCTRL_MASK          0x800000UL                                 /**< Bit mask for SMU_CHIPTESTCTRL               */
#define _SMU_PPUSATD0_CHIPTESTCTRL_DEFAULT       0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_CHIPTESTCTRL_DEFAULT        (_SMU_PPUSATD0_CHIPTESTCTRL_DEFAULT << 23) /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_SYSCFGCFGNS                 (0x1UL << 24)                              /**< SYSCFGCFGNS Secure Access                   */
#define _SMU_PPUSATD0_SYSCFGCFGNS_SHIFT          24                                         /**< Shift value for SMU_SYSCFGCFGNS             */
#define _SMU_PPUSATD0_SYSCFGCFGNS_MASK           0x1000000UL                                /**< Bit mask for SMU_SYSCFGCFGNS                */
#define _SMU_PPUSATD0_SYSCFGCFGNS_DEFAULT        0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_SYSCFGCFGNS_DEFAULT         (_SMU_PPUSATD0_SYSCFGCFGNS_DEFAULT << 24)  /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_SYSCFG                      (0x1UL << 25)                              /**< SYSCFG Secure Access                        */
#define _SMU_PPUSATD0_SYSCFG_SHIFT               25                                         /**< Shift value for SMU_SYSCFG                  */
#define _SMU_PPUSATD0_SYSCFG_MASK                0x2000000UL                                /**< Bit mask for SMU_SYSCFG                     */
#define _SMU_PPUSATD0_SYSCFG_DEFAULT             0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_SYSCFG_DEFAULT              (_SMU_PPUSATD0_SYSCFG_DEFAULT << 25)       /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_BURAM                       (0x1UL << 26)                              /**< BURAM Secure Access                         */
#define _SMU_PPUSATD0_BURAM_SHIFT                26                                         /**< Shift value for SMU_BURAM                   */
#define _SMU_PPUSATD0_BURAM_MASK                 0x4000000UL                                /**< Bit mask for SMU_BURAM                      */
#define _SMU_PPUSATD0_BURAM_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_BURAM_DEFAULT               (_SMU_PPUSATD0_BURAM_DEFAULT << 26)        /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_GPCRC                       (0x1UL << 27)                              /**< GPCRC Secure Access                         */
#define _SMU_PPUSATD0_GPCRC_SHIFT                27                                         /**< Shift value for SMU_GPCRC                   */
#define _SMU_PPUSATD0_GPCRC_MASK                 0x8000000UL                                /**< Bit mask for SMU_GPCRC                      */
#define _SMU_PPUSATD0_GPCRC_DEFAULT              0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_GPCRC_DEFAULT               (_SMU_PPUSATD0_GPCRC_DEFAULT << 27)        /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_DCDC                        (0x1UL << 28)                              /**< DCDC Secure Access                          */
#define _SMU_PPUSATD0_DCDC_SHIFT                 28                                         /**< Shift value for SMU_DCDC                    */
#define _SMU_PPUSATD0_DCDC_MASK                  0x10000000UL                               /**< Bit mask for SMU_DCDC                       */
#define _SMU_PPUSATD0_DCDC_DEFAULT               0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_DCDC_DEFAULT                (_SMU_PPUSATD0_DCDC_DEFAULT << 28)         /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_HOSTMAILBOX                 (0x1UL << 29)                              /**< HOSTMAILBOX Secure Access                   */
#define _SMU_PPUSATD0_HOSTMAILBOX_SHIFT          29                                         /**< Shift value for SMU_HOSTMAILBOX             */
#define _SMU_PPUSATD0_HOSTMAILBOX_MASK           0x20000000UL                               /**< Bit mask for SMU_HOSTMAILBOX                */
#define _SMU_PPUSATD0_HOSTMAILBOX_DEFAULT        0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_HOSTMAILBOX_DEFAULT         (_SMU_PPUSATD0_HOSTMAILBOX_DEFAULT << 29)  /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_EUSART1                     (0x1UL << 30)                              /**< EUSART1 Secure Access                       */
#define _SMU_PPUSATD0_EUSART1_SHIFT              30                                         /**< Shift value for SMU_EUSART1                 */
#define _SMU_PPUSATD0_EUSART1_MASK               0x40000000UL                               /**< Bit mask for SMU_EUSART1                    */
#define _SMU_PPUSATD0_EUSART1_DEFAULT            0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_EUSART1_DEFAULT             (_SMU_PPUSATD0_EUSART1_DEFAULT << 30)      /**< Shifted mode DEFAULT for SMU_PPUSATD0       */
#define SMU_PPUSATD0_EUSART2                     (0x1UL << 31)                              /**< EUSART2 Secure Access                       */
#define _SMU_PPUSATD0_EUSART2_SHIFT              31                                         /**< Shift value for SMU_EUSART2                 */
#define _SMU_PPUSATD0_EUSART2_MASK               0x80000000UL                               /**< Bit mask for SMU_EUSART2                    */
#define _SMU_PPUSATD0_EUSART2_DEFAULT            0x00000001UL                               /**< Mode DEFAULT for SMU_PPUSATD0               */
#define SMU_PPUSATD0_EUSART2_DEFAULT             (_SMU_PPUSATD0_EUSART2_DEFAULT << 31)      /**< Shifted mode DEFAULT for SMU_PPUSATD0       */

/* Bit fields for SMU PPUSATD1 */
#define _SMU_PPUSATD1_RESETVALUE                 0x01FFFFFFUL                            /**< Default value for SMU_PPUSATD1              */
#define _SMU_PPUSATD1_MASK                       0x01FFFFFFUL                            /**< Mask for SMU_PPUSATD1                       */
#define SMU_PPUSATD1_SYSRTC                      (0x1UL << 0)                            /**< SYSRTC Secure Access                        */
#define _SMU_PPUSATD1_SYSRTC_SHIFT               0                                       /**< Shift value for SMU_SYSRTC                  */
#define _SMU_PPUSATD1_SYSRTC_MASK                0x1UL                                   /**< Bit mask for SMU_SYSRTC                     */
#define _SMU_PPUSATD1_SYSRTC_DEFAULT             0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_SYSRTC_DEFAULT              (_SMU_PPUSATD1_SYSRTC_DEFAULT << 0)     /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_LCD                         (0x1UL << 1)                            /**< LCD Secure Access                           */
#define _SMU_PPUSATD1_LCD_SHIFT                  1                                       /**< Shift value for SMU_LCD                     */
#define _SMU_PPUSATD1_LCD_MASK                   0x2UL                                   /**< Bit mask for SMU_LCD                        */
#define _SMU_PPUSATD1_LCD_DEFAULT                0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_LCD_DEFAULT                 (_SMU_PPUSATD1_LCD_DEFAULT << 1)        /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_KEYSCAN                     (0x1UL << 2)                            /**< KEYSCAN Secure Access                       */
#define _SMU_PPUSATD1_KEYSCAN_SHIFT              2                                       /**< Shift value for SMU_KEYSCAN                 */
#define _SMU_PPUSATD1_KEYSCAN_MASK               0x4UL                                   /**< Bit mask for SMU_KEYSCAN                    */
#define _SMU_PPUSATD1_KEYSCAN_DEFAULT            0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_KEYSCAN_DEFAULT             (_SMU_PPUSATD1_KEYSCAN_DEFAULT << 2)    /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_DMEM                        (0x1UL << 3)                            /**< DMEM Secure Access                          */
#define _SMU_PPUSATD1_DMEM_SHIFT                 3                                       /**< Shift value for SMU_DMEM                    */
#define _SMU_PPUSATD1_DMEM_MASK                  0x8UL                                   /**< Bit mask for SMU_DMEM                       */
#define _SMU_PPUSATD1_DMEM_DEFAULT               0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_DMEM_DEFAULT                (_SMU_PPUSATD1_DMEM_DEFAULT << 3)       /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_LCDRF                       (0x1UL << 4)                            /**< LCDRF Secure Access                         */
#define _SMU_PPUSATD1_LCDRF_SHIFT                4                                       /**< Shift value for SMU_LCDRF                   */
#define _SMU_PPUSATD1_LCDRF_MASK                 0x10UL                                  /**< Bit mask for SMU_LCDRF                      */
#define _SMU_PPUSATD1_LCDRF_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_LCDRF_DEFAULT               (_SMU_PPUSATD1_LCDRF_DEFAULT << 4)      /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_PFMXPPRF                    (0x1UL << 5)                            /**< PFMXPPRF Secure Access                      */
#define _SMU_PPUSATD1_PFMXPPRF_SHIFT             5                                       /**< Shift value for SMU_PFMXPPRF                */
#define _SMU_PPUSATD1_PFMXPPRF_MASK              0x20UL                                  /**< Bit mask for SMU_PFMXPPRF                   */
#define _SMU_PPUSATD1_PFMXPPRF_DEFAULT           0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_PFMXPPRF_DEFAULT            (_SMU_PPUSATD1_PFMXPPRF_DEFAULT << 5)   /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_RADIOAES                    (0x1UL << 6)                            /**< RADIOAES Secure Access                      */
#define _SMU_PPUSATD1_RADIOAES_SHIFT             6                                       /**< Shift value for SMU_RADIOAES                */
#define _SMU_PPUSATD1_RADIOAES_MASK              0x40UL                                  /**< Bit mask for SMU_RADIOAES                   */
#define _SMU_PPUSATD1_RADIOAES_DEFAULT           0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_RADIOAES_DEFAULT            (_SMU_PPUSATD1_RADIOAES_DEFAULT << 6)   /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_SMU                         (0x1UL << 7)                            /**< SMU Secure Access                           */
#define _SMU_PPUSATD1_SMU_SHIFT                  7                                       /**< Shift value for SMU_SMU                     */
#define _SMU_PPUSATD1_SMU_MASK                   0x80UL                                  /**< Bit mask for SMU_SMU                        */
#define _SMU_PPUSATD1_SMU_DEFAULT                0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_SMU_DEFAULT                 (_SMU_PPUSATD1_SMU_DEFAULT << 7)        /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_SMUCFGNS                    (0x1UL << 8)                            /**< SMUCFGNS Secure Access                      */
#define _SMU_PPUSATD1_SMUCFGNS_SHIFT             8                                       /**< Shift value for SMU_SMUCFGNS                */
#define _SMU_PPUSATD1_SMUCFGNS_MASK              0x100UL                                 /**< Bit mask for SMU_SMUCFGNS                   */
#define _SMU_PPUSATD1_SMUCFGNS_DEFAULT           0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_SMUCFGNS_DEFAULT            (_SMU_PPUSATD1_SMUCFGNS_DEFAULT << 8)   /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_LETIMER0                    (0x1UL << 9)                            /**< LETIMER0 Secure Access                      */
#define _SMU_PPUSATD1_LETIMER0_SHIFT             9                                       /**< Shift value for SMU_LETIMER0                */
#define _SMU_PPUSATD1_LETIMER0_MASK              0x200UL                                 /**< Bit mask for SMU_LETIMER0                   */
#define _SMU_PPUSATD1_LETIMER0_DEFAULT           0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_LETIMER0_DEFAULT            (_SMU_PPUSATD1_LETIMER0_DEFAULT << 9)   /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_IADC0                       (0x1UL << 10)                           /**< IADC0 Secure Access                         */
#define _SMU_PPUSATD1_IADC0_SHIFT                10                                      /**< Shift value for SMU_IADC0                   */
#define _SMU_PPUSATD1_IADC0_MASK                 0x400UL                                 /**< Bit mask for SMU_IADC0                      */
#define _SMU_PPUSATD1_IADC0_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_IADC0_DEFAULT               (_SMU_PPUSATD1_IADC0_DEFAULT << 10)     /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_ACMP0                       (0x1UL << 11)                           /**< ACMP0 Secure Access                         */
#define _SMU_PPUSATD1_ACMP0_SHIFT                11                                      /**< Shift value for SMU_ACMP0                   */
#define _SMU_PPUSATD1_ACMP0_MASK                 0x800UL                                 /**< Bit mask for SMU_ACMP0                      */
#define _SMU_PPUSATD1_ACMP0_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_ACMP0_DEFAULT               (_SMU_PPUSATD1_ACMP0_DEFAULT << 11)     /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_ACMP1                       (0x1UL << 12)                           /**< ACMP1 Secure Access                         */
#define _SMU_PPUSATD1_ACMP1_SHIFT                12                                      /**< Shift value for SMU_ACMP1                   */
#define _SMU_PPUSATD1_ACMP1_MASK                 0x1000UL                                /**< Bit mask for SMU_ACMP1                      */
#define _SMU_PPUSATD1_ACMP1_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_ACMP1_DEFAULT               (_SMU_PPUSATD1_ACMP1_DEFAULT << 12)     /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_AMUXCP0                     (0x1UL << 13)                           /**< AMUXCP0 Secure Access                       */
#define _SMU_PPUSATD1_AMUXCP0_SHIFT              13                                      /**< Shift value for SMU_AMUXCP0                 */
#define _SMU_PPUSATD1_AMUXCP0_MASK               0x2000UL                                /**< Bit mask for SMU_AMUXCP0                    */
#define _SMU_PPUSATD1_AMUXCP0_DEFAULT            0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_AMUXCP0_DEFAULT             (_SMU_PPUSATD1_AMUXCP0_DEFAULT << 13)   /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_VDAC0                       (0x1UL << 14)                           /**< VDAC0 Secure Access                         */
#define _SMU_PPUSATD1_VDAC0_SHIFT                14                                      /**< Shift value for SMU_VDAC0                   */
#define _SMU_PPUSATD1_VDAC0_MASK                 0x4000UL                                /**< Bit mask for SMU_VDAC0                      */
#define _SMU_PPUSATD1_VDAC0_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_VDAC0_DEFAULT               (_SMU_PPUSATD1_VDAC0_DEFAULT << 14)     /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_PCNT                        (0x1UL << 15)                           /**< PCNT Secure Access                          */
#define _SMU_PPUSATD1_PCNT_SHIFT                 15                                      /**< Shift value for SMU_PCNT                    */
#define _SMU_PPUSATD1_PCNT_MASK                  0x8000UL                                /**< Bit mask for SMU_PCNT                       */
#define _SMU_PPUSATD1_PCNT_DEFAULT               0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_PCNT_DEFAULT                (_SMU_PPUSATD1_PCNT_DEFAULT << 15)      /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_LESENSE                     (0x1UL << 16)                           /**< LESENSE Secure Access                       */
#define _SMU_PPUSATD1_LESENSE_SHIFT              16                                      /**< Shift value for SMU_LESENSE                 */
#define _SMU_PPUSATD1_LESENSE_MASK               0x10000UL                               /**< Bit mask for SMU_LESENSE                    */
#define _SMU_PPUSATD1_LESENSE_DEFAULT            0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_LESENSE_DEFAULT             (_SMU_PPUSATD1_LESENSE_DEFAULT << 16)   /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_HFRCO1                      (0x1UL << 17)                           /**< HFRCO1 Secure Access                        */
#define _SMU_PPUSATD1_HFRCO1_SHIFT               17                                      /**< Shift value for SMU_HFRCO1                  */
#define _SMU_PPUSATD1_HFRCO1_MASK                0x20000UL                               /**< Bit mask for SMU_HFRCO1                     */
#define _SMU_PPUSATD1_HFRCO1_DEFAULT             0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_HFRCO1_DEFAULT              (_SMU_PPUSATD1_HFRCO1_DEFAULT << 17)    /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_HFXO0                       (0x1UL << 18)                           /**< HFXO0 Secure Access                         */
#define _SMU_PPUSATD1_HFXO0_SHIFT                18                                      /**< Shift value for SMU_HFXO0                   */
#define _SMU_PPUSATD1_HFXO0_MASK                 0x40000UL                               /**< Bit mask for SMU_HFXO0                      */
#define _SMU_PPUSATD1_HFXO0_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_HFXO0_DEFAULT               (_SMU_PPUSATD1_HFXO0_DEFAULT << 18)     /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_I2C0                        (0x1UL << 19)                           /**< I2C0 Secure Access                          */
#define _SMU_PPUSATD1_I2C0_SHIFT                 19                                      /**< Shift value for SMU_I2C0                    */
#define _SMU_PPUSATD1_I2C0_MASK                  0x80000UL                               /**< Bit mask for SMU_I2C0                       */
#define _SMU_PPUSATD1_I2C0_DEFAULT               0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_I2C0_DEFAULT                (_SMU_PPUSATD1_I2C0_DEFAULT << 19)      /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_WDOG0                       (0x1UL << 20)                           /**< WDOG0 Secure Access                         */
#define _SMU_PPUSATD1_WDOG0_SHIFT                20                                      /**< Shift value for SMU_WDOG0                   */
#define _SMU_PPUSATD1_WDOG0_MASK                 0x100000UL                              /**< Bit mask for SMU_WDOG0                      */
#define _SMU_PPUSATD1_WDOG0_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_WDOG0_DEFAULT               (_SMU_PPUSATD1_WDOG0_DEFAULT << 20)     /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_WDOG1                       (0x1UL << 21)                           /**< WDOG1 Secure Access                         */
#define _SMU_PPUSATD1_WDOG1_SHIFT                21                                      /**< Shift value for SMU_WDOG1                   */
#define _SMU_PPUSATD1_WDOG1_MASK                 0x200000UL                              /**< Bit mask for SMU_WDOG1                      */
#define _SMU_PPUSATD1_WDOG1_DEFAULT              0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_WDOG1_DEFAULT               (_SMU_PPUSATD1_WDOG1_DEFAULT << 21)     /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_EUSART0                     (0x1UL << 22)                           /**< EUSART0 Secure Access                       */
#define _SMU_PPUSATD1_EUSART0_SHIFT              22                                      /**< Shift value for SMU_EUSART0                 */
#define _SMU_PPUSATD1_EUSART0_MASK               0x400000UL                              /**< Bit mask for SMU_EUSART0                    */
#define _SMU_PPUSATD1_EUSART0_DEFAULT            0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_EUSART0_DEFAULT             (_SMU_PPUSATD1_EUSART0_DEFAULT << 22)   /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_SEMAILBOX                   (0x1UL << 23)                           /**< SEMAILBOX Secure Access                     */
#define _SMU_PPUSATD1_SEMAILBOX_SHIFT            23                                      /**< Shift value for SMU_SEMAILBOX               */
#define _SMU_PPUSATD1_SEMAILBOX_MASK             0x800000UL                              /**< Bit mask for SMU_SEMAILBOX                  */
#define _SMU_PPUSATD1_SEMAILBOX_DEFAULT          0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_SEMAILBOX_DEFAULT           (_SMU_PPUSATD1_SEMAILBOX_DEFAULT << 23) /**< Shifted mode DEFAULT for SMU_PPUSATD1       */
#define SMU_PPUSATD1_AHBRADIO                    (0x1UL << 24)                           /**< AHBRADIO Secure Access                      */
#define _SMU_PPUSATD1_AHBRADIO_SHIFT             24                                      /**< Shift value for SMU_AHBRADIO                */
#define _SMU_PPUSATD1_AHBRADIO_MASK              0x1000000UL                             /**< Bit mask for SMU_AHBRADIO                   */
#define _SMU_PPUSATD1_AHBRADIO_DEFAULT           0x00000001UL                            /**< Mode DEFAULT for SMU_PPUSATD1               */
#define SMU_PPUSATD1_AHBRADIO_DEFAULT            (_SMU_PPUSATD1_AHBRADIO_DEFAULT << 24)  /**< Shifted mode DEFAULT for SMU_PPUSATD1       */

/* Bit fields for SMU PPUFS */
#define _SMU_PPUFS_RESETVALUE                    0x00000000UL                            /**< Default value for SMU_PPUFS                 */
#define _SMU_PPUFS_MASK                          0x000000FFUL                            /**< Mask for SMU_PPUFS                          */
#define _SMU_PPUFS_PPUFSPERIPHID_SHIFT           0                                       /**< Shift value for SMU_PPUFSPERIPHID           */
#define _SMU_PPUFS_PPUFSPERIPHID_MASK            0xFFUL                                  /**< Bit mask for SMU_PPUFSPERIPHID              */
#define _SMU_PPUFS_PPUFSPERIPHID_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for SMU_PPUFS                  */
#define SMU_PPUFS_PPUFSPERIPHID_DEFAULT          (_SMU_PPUFS_PPUFSPERIPHID_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_PPUFS          */

/* Bit fields for SMU BMPUPATD0 */
#define _SMU_BMPUPATD0_RESETVALUE                0x0000003FUL                                 /**< Default value for SMU_BMPUPATD0             */
#define _SMU_BMPUPATD0_MASK                      0x0000003FUL                                 /**< Mask for SMU_BMPUPATD0                      */
#define SMU_BMPUPATD0_RADIOAES                   (0x1UL << 0)                                 /**< RADIO AES DMA privileged mode               */
#define _SMU_BMPUPATD0_RADIOAES_SHIFT            0                                            /**< Shift value for SMU_RADIOAES                */
#define _SMU_BMPUPATD0_RADIOAES_MASK             0x1UL                                        /**< Bit mask for SMU_RADIOAES                   */
#define _SMU_BMPUPATD0_RADIOAES_DEFAULT          0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUPATD0              */
#define SMU_BMPUPATD0_RADIOAES_DEFAULT           (_SMU_BMPUPATD0_RADIOAES_DEFAULT << 0)       /**< Shifted mode DEFAULT for SMU_BMPUPATD0      */
#define SMU_BMPUPATD0_RADIOSUBSYSTEM             (0x1UL << 1)                                 /**< RADIO subsystem manager privileged mode     */
#define _SMU_BMPUPATD0_RADIOSUBSYSTEM_SHIFT      1                                            /**< Shift value for SMU_RADIOSUBSYSTEM          */
#define _SMU_BMPUPATD0_RADIOSUBSYSTEM_MASK       0x2UL                                        /**< Bit mask for SMU_RADIOSUBSYSTEM             */
#define _SMU_BMPUPATD0_RADIOSUBSYSTEM_DEFAULT    0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUPATD0              */
#define SMU_BMPUPATD0_RADIOSUBSYSTEM_DEFAULT     (_SMU_BMPUPATD0_RADIOSUBSYSTEM_DEFAULT << 1) /**< Shifted mode DEFAULT for SMU_BMPUPATD0      */
#define SMU_BMPUPATD0_LDMA                       (0x1UL << 2)                                 /**< MCU LDMA privileged mode                    */
#define _SMU_BMPUPATD0_LDMA_SHIFT                2                                            /**< Shift value for SMU_LDMA                    */
#define _SMU_BMPUPATD0_LDMA_MASK                 0x4UL                                        /**< Bit mask for SMU_LDMA                       */
#define _SMU_BMPUPATD0_LDMA_DEFAULT              0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUPATD0              */
#define SMU_BMPUPATD0_LDMA_DEFAULT               (_SMU_BMPUPATD0_LDMA_DEFAULT << 2)           /**< Shifted mode DEFAULT for SMU_BMPUPATD0      */
#define SMU_BMPUPATD0_RFECA0                     (0x1UL << 3)                                 /**< RFECA0 privileged mode                      */
#define _SMU_BMPUPATD0_RFECA0_SHIFT              3                                            /**< Shift value for SMU_RFECA0                  */
#define _SMU_BMPUPATD0_RFECA0_MASK               0x8UL                                        /**< Bit mask for SMU_RFECA0                     */
#define _SMU_BMPUPATD0_RFECA0_DEFAULT            0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUPATD0              */
#define SMU_BMPUPATD0_RFECA0_DEFAULT             (_SMU_BMPUPATD0_RFECA0_DEFAULT << 3)         /**< Shifted mode DEFAULT for SMU_BMPUPATD0      */
#define SMU_BMPUPATD0_RFECA1                     (0x1UL << 4)                                 /**< RFECA1 privileged mode                      */
#define _SMU_BMPUPATD0_RFECA1_SHIFT              4                                            /**< Shift value for SMU_RFECA1                  */
#define _SMU_BMPUPATD0_RFECA1_MASK               0x10UL                                       /**< Bit mask for SMU_RFECA1                     */
#define _SMU_BMPUPATD0_RFECA1_DEFAULT            0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUPATD0              */
#define SMU_BMPUPATD0_RFECA1_DEFAULT             (_SMU_BMPUPATD0_RFECA1_DEFAULT << 4)         /**< Shifted mode DEFAULT for SMU_BMPUPATD0      */
#define SMU_BMPUPATD0_SEEXTDMA                   (0x1UL << 5)                                 /**< SEEXTDMA privileged mode                    */
#define _SMU_BMPUPATD0_SEEXTDMA_SHIFT            5                                            /**< Shift value for SMU_SEEXTDMA                */
#define _SMU_BMPUPATD0_SEEXTDMA_MASK             0x20UL                                       /**< Bit mask for SMU_SEEXTDMA                   */
#define _SMU_BMPUPATD0_SEEXTDMA_DEFAULT          0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUPATD0              */
#define SMU_BMPUPATD0_SEEXTDMA_DEFAULT           (_SMU_BMPUPATD0_SEEXTDMA_DEFAULT << 5)       /**< Shifted mode DEFAULT for SMU_BMPUPATD0      */

/* Bit fields for SMU BMPUSATD0 */
#define _SMU_BMPUSATD0_RESETVALUE                0x0000003FUL                                 /**< Default value for SMU_BMPUSATD0             */
#define _SMU_BMPUSATD0_MASK                      0x0000003FUL                                 /**< Mask for SMU_BMPUSATD0                      */
#define SMU_BMPUSATD0_RADIOAES                   (0x1UL << 0)                                 /**< RADIOAES DMA secure mode                    */
#define _SMU_BMPUSATD0_RADIOAES_SHIFT            0                                            /**< Shift value for SMU_RADIOAES                */
#define _SMU_BMPUSATD0_RADIOAES_MASK             0x1UL                                        /**< Bit mask for SMU_RADIOAES                   */
#define _SMU_BMPUSATD0_RADIOAES_DEFAULT          0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUSATD0              */
#define SMU_BMPUSATD0_RADIOAES_DEFAULT           (_SMU_BMPUSATD0_RADIOAES_DEFAULT << 0)       /**< Shifted mode DEFAULT for SMU_BMPUSATD0      */
#define SMU_BMPUSATD0_RADIOSUBSYSTEM             (0x1UL << 1)                                 /**< RADIO subsystem manager secure mode         */
#define _SMU_BMPUSATD0_RADIOSUBSYSTEM_SHIFT      1                                            /**< Shift value for SMU_RADIOSUBSYSTEM          */
#define _SMU_BMPUSATD0_RADIOSUBSYSTEM_MASK       0x2UL                                        /**< Bit mask for SMU_RADIOSUBSYSTEM             */
#define _SMU_BMPUSATD0_RADIOSUBSYSTEM_DEFAULT    0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUSATD0              */
#define SMU_BMPUSATD0_RADIOSUBSYSTEM_DEFAULT     (_SMU_BMPUSATD0_RADIOSUBSYSTEM_DEFAULT << 1) /**< Shifted mode DEFAULT for SMU_BMPUSATD0      */
#define SMU_BMPUSATD0_LDMA                       (0x1UL << 2)                                 /**< MCU LDMA secure mode                        */
#define _SMU_BMPUSATD0_LDMA_SHIFT                2                                            /**< Shift value for SMU_LDMA                    */
#define _SMU_BMPUSATD0_LDMA_MASK                 0x4UL                                        /**< Bit mask for SMU_LDMA                       */
#define _SMU_BMPUSATD0_LDMA_DEFAULT              0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUSATD0              */
#define SMU_BMPUSATD0_LDMA_DEFAULT               (_SMU_BMPUSATD0_LDMA_DEFAULT << 2)           /**< Shifted mode DEFAULT for SMU_BMPUSATD0      */
#define SMU_BMPUSATD0_RFECA0                     (0x1UL << 3)                                 /**< RFECA0 secure mode                          */
#define _SMU_BMPUSATD0_RFECA0_SHIFT              3                                            /**< Shift value for SMU_RFECA0                  */
#define _SMU_BMPUSATD0_RFECA0_MASK               0x8UL                                        /**< Bit mask for SMU_RFECA0                     */
#define _SMU_BMPUSATD0_RFECA0_DEFAULT            0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUSATD0              */
#define SMU_BMPUSATD0_RFECA0_DEFAULT             (_SMU_BMPUSATD0_RFECA0_DEFAULT << 3)         /**< Shifted mode DEFAULT for SMU_BMPUSATD0      */
#define SMU_BMPUSATD0_RFECA1                     (0x1UL << 4)                                 /**< RFECA1 secure mode                          */
#define _SMU_BMPUSATD0_RFECA1_SHIFT              4                                            /**< Shift value for SMU_RFECA1                  */
#define _SMU_BMPUSATD0_RFECA1_MASK               0x10UL                                       /**< Bit mask for SMU_RFECA1                     */
#define _SMU_BMPUSATD0_RFECA1_DEFAULT            0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUSATD0              */
#define SMU_BMPUSATD0_RFECA1_DEFAULT             (_SMU_BMPUSATD0_RFECA1_DEFAULT << 4)         /**< Shifted mode DEFAULT for SMU_BMPUSATD0      */
#define SMU_BMPUSATD0_SEEXTDMA                   (0x1UL << 5)                                 /**< SEEXTDMA secure mode                        */
#define _SMU_BMPUSATD0_SEEXTDMA_SHIFT            5                                            /**< Shift value for SMU_SEEXTDMA                */
#define _SMU_BMPUSATD0_SEEXTDMA_MASK             0x20UL                                       /**< Bit mask for SMU_SEEXTDMA                   */
#define _SMU_BMPUSATD0_SEEXTDMA_DEFAULT          0x00000001UL                                 /**< Mode DEFAULT for SMU_BMPUSATD0              */
#define SMU_BMPUSATD0_SEEXTDMA_DEFAULT           (_SMU_BMPUSATD0_SEEXTDMA_DEFAULT << 5)       /**< Shifted mode DEFAULT for SMU_BMPUSATD0      */

/* Bit fields for SMU BMPUFS */
#define _SMU_BMPUFS_RESETVALUE                   0x00000000UL                              /**< Default value for SMU_BMPUFS                */
#define _SMU_BMPUFS_MASK                         0x000000FFUL                              /**< Mask for SMU_BMPUFS                         */
#define _SMU_BMPUFS_BMPUFSMASTERID_SHIFT         0                                         /**< Shift value for SMU_BMPUFSMASTERID          */
#define _SMU_BMPUFS_BMPUFSMASTERID_MASK          0xFFUL                                    /**< Bit mask for SMU_BMPUFSMASTERID             */
#define _SMU_BMPUFS_BMPUFSMASTERID_DEFAULT       0x00000000UL                              /**< Mode DEFAULT for SMU_BMPUFS                 */
#define SMU_BMPUFS_BMPUFSMASTERID_DEFAULT        (_SMU_BMPUFS_BMPUFSMASTERID_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_BMPUFS         */

/* Bit fields for SMU BMPUFSADDR */
#define _SMU_BMPUFSADDR_RESETVALUE               0x00000000UL                              /**< Default value for SMU_BMPUFSADDR            */
#define _SMU_BMPUFSADDR_MASK                     0xFFFFFFFFUL                              /**< Mask for SMU_BMPUFSADDR                     */
#define _SMU_BMPUFSADDR_BMPUFSADDR_SHIFT         0                                         /**< Shift value for SMU_BMPUFSADDR              */
#define _SMU_BMPUFSADDR_BMPUFSADDR_MASK          0xFFFFFFFFUL                              /**< Bit mask for SMU_BMPUFSADDR                 */
#define _SMU_BMPUFSADDR_BMPUFSADDR_DEFAULT       0x00000000UL                              /**< Mode DEFAULT for SMU_BMPUFSADDR             */
#define SMU_BMPUFSADDR_BMPUFSADDR_DEFAULT        (_SMU_BMPUFSADDR_BMPUFSADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_BMPUFSADDR     */

/* Bit fields for SMU ESAURTYPES0 */
#define _SMU_ESAURTYPES0_RESETVALUE              0x00000000UL                              /**< Default value for SMU_ESAURTYPES0           */
#define _SMU_ESAURTYPES0_MASK                    0x00001000UL                              /**< Mask for SMU_ESAURTYPES0                    */
#define SMU_ESAURTYPES0_ESAUR3NS                 (0x1UL << 12)                             /**< Region 3 Non-Secure                         */
#define _SMU_ESAURTYPES0_ESAUR3NS_SHIFT          12                                        /**< Shift value for SMU_ESAUR3NS                */
#define _SMU_ESAURTYPES0_ESAUR3NS_MASK           0x1000UL                                  /**< Bit mask for SMU_ESAUR3NS                   */
#define _SMU_ESAURTYPES0_ESAUR3NS_DEFAULT        0x00000000UL                              /**< Mode DEFAULT for SMU_ESAURTYPES0            */
#define SMU_ESAURTYPES0_ESAUR3NS_DEFAULT         (_SMU_ESAURTYPES0_ESAUR3NS_DEFAULT << 12) /**< Shifted mode DEFAULT for SMU_ESAURTYPES0    */

/* Bit fields for SMU ESAURTYPES1 */
#define _SMU_ESAURTYPES1_RESETVALUE              0x00000000UL                               /**< Default value for SMU_ESAURTYPES1           */
#define _SMU_ESAURTYPES1_MASK                    0x00001000UL                               /**< Mask for SMU_ESAURTYPES1                    */
#define SMU_ESAURTYPES1_ESAUR11NS                (0x1UL << 12)                              /**< Region 11 Non-Secure                        */
#define _SMU_ESAURTYPES1_ESAUR11NS_SHIFT         12                                         /**< Shift value for SMU_ESAUR11NS               */
#define _SMU_ESAURTYPES1_ESAUR11NS_MASK          0x1000UL                                   /**< Bit mask for SMU_ESAUR11NS                  */
#define _SMU_ESAURTYPES1_ESAUR11NS_DEFAULT       0x00000000UL                               /**< Mode DEFAULT for SMU_ESAURTYPES1            */
#define SMU_ESAURTYPES1_ESAUR11NS_DEFAULT        (_SMU_ESAURTYPES1_ESAUR11NS_DEFAULT << 12) /**< Shifted mode DEFAULT for SMU_ESAURTYPES1    */

/* Bit fields for SMU ESAUMRB01 */
#define _SMU_ESAUMRB01_RESETVALUE                0x0A000000UL                             /**< Default value for SMU_ESAUMRB01             */
#define _SMU_ESAUMRB01_MASK                      0x0FFFF000UL                             /**< Mask for SMU_ESAUMRB01                      */
#define _SMU_ESAUMRB01_ESAUMRB01_SHIFT           12                                       /**< Shift value for SMU_ESAUMRB01               */
#define _SMU_ESAUMRB01_ESAUMRB01_MASK            0xFFFF000UL                              /**< Bit mask for SMU_ESAUMRB01                  */
#define _SMU_ESAUMRB01_ESAUMRB01_DEFAULT         0x0000A000UL                             /**< Mode DEFAULT for SMU_ESAUMRB01              */
#define SMU_ESAUMRB01_ESAUMRB01_DEFAULT          (_SMU_ESAUMRB01_ESAUMRB01_DEFAULT << 12) /**< Shifted mode DEFAULT for SMU_ESAUMRB01      */

/* Bit fields for SMU ESAUMRB12 */
#define _SMU_ESAUMRB12_RESETVALUE                0x0C000000UL                             /**< Default value for SMU_ESAUMRB12             */
#define _SMU_ESAUMRB12_MASK                      0x0FFFF000UL                             /**< Mask for SMU_ESAUMRB12                      */
#define _SMU_ESAUMRB12_ESAUMRB12_SHIFT           12                                       /**< Shift value for SMU_ESAUMRB12               */
#define _SMU_ESAUMRB12_ESAUMRB12_MASK            0xFFFF000UL                              /**< Bit mask for SMU_ESAUMRB12                  */
#define _SMU_ESAUMRB12_ESAUMRB12_DEFAULT         0x0000C000UL                             /**< Mode DEFAULT for SMU_ESAUMRB12              */
#define SMU_ESAUMRB12_ESAUMRB12_DEFAULT          (_SMU_ESAUMRB12_ESAUMRB12_DEFAULT << 12) /**< Shifted mode DEFAULT for SMU_ESAUMRB12      */

/* Bit fields for SMU ESAUMRB45 */
#define _SMU_ESAUMRB45_RESETVALUE                0x02000000UL                             /**< Default value for SMU_ESAUMRB45             */
#define _SMU_ESAUMRB45_MASK                      0x0FFFF000UL                             /**< Mask for SMU_ESAUMRB45                      */
#define _SMU_ESAUMRB45_ESAUMRB45_SHIFT           12                                       /**< Shift value for SMU_ESAUMRB45               */
#define _SMU_ESAUMRB45_ESAUMRB45_MASK            0xFFFF000UL                              /**< Bit mask for SMU_ESAUMRB45                  */
#define _SMU_ESAUMRB45_ESAUMRB45_DEFAULT         0x00002000UL                             /**< Mode DEFAULT for SMU_ESAUMRB45              */
#define SMU_ESAUMRB45_ESAUMRB45_DEFAULT          (_SMU_ESAUMRB45_ESAUMRB45_DEFAULT << 12) /**< Shifted mode DEFAULT for SMU_ESAUMRB45      */

/* Bit fields for SMU ESAUMRB56 */
#define _SMU_ESAUMRB56_RESETVALUE                0x04000000UL                             /**< Default value for SMU_ESAUMRB56             */
#define _SMU_ESAUMRB56_MASK                      0x0FFFF000UL                             /**< Mask for SMU_ESAUMRB56                      */
#define _SMU_ESAUMRB56_ESAUMRB56_SHIFT           12                                       /**< Shift value for SMU_ESAUMRB56               */
#define _SMU_ESAUMRB56_ESAUMRB56_MASK            0xFFFF000UL                              /**< Bit mask for SMU_ESAUMRB56                  */
#define _SMU_ESAUMRB56_ESAUMRB56_DEFAULT         0x00004000UL                             /**< Mode DEFAULT for SMU_ESAUMRB56              */
#define SMU_ESAUMRB56_ESAUMRB56_DEFAULT          (_SMU_ESAUMRB56_ESAUMRB56_DEFAULT << 12) /**< Shifted mode DEFAULT for SMU_ESAUMRB56      */

/** @} End of group EFR32ZG23_SMU_BitFields */
/** @} End of group EFR32ZG23_SMU */
/**************************************************************************//**
 * @defgroup EFR32ZG23_SMU_CFGNS SMU_CFGNS
 * @{
 * @brief EFR32ZG23 SMU_CFGNS Register Declaration.
 *****************************************************************************/

/** SMU_CFGNS Register Declaration. */
typedef struct {
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  NSSTATUS;                      /**< Status Register                                    */
  __IOM uint32_t NSLOCK;                        /**< Lock Register                                      */
  __IOM uint32_t NSIF;                          /**< Interrupt Flag Register                            */
  __IOM uint32_t NSIEN;                         /**< Interrupt Enable Register                          */
  uint32_t       RESERVED1[3U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED2[8U];                 /**< Reserved for future use                            */
  __IOM uint32_t PPUNSPATD0;                    /**< Privileged Access                                  */
  __IOM uint32_t PPUNSPATD1;                    /**< Privileged Access                                  */
  uint32_t       RESERVED3[62U];                /**< Reserved for future use                            */
  __IM uint32_t  PPUNSFS;                       /**< Fault Status                                       */
  uint32_t       RESERVED4[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t BMPUNSPATD0;                   /**< Privileged Attribute                               */
  uint32_t       RESERVED5[63U];                /**< Reserved for future use                            */
  uint32_t       RESERVED6[876U];               /**< Reserved for future use                            */
  uint32_t       RESERVED7[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  NSSTATUS_SET;                  /**< Status Register                                    */
  __IOM uint32_t NSLOCK_SET;                    /**< Lock Register                                      */
  __IOM uint32_t NSIF_SET;                      /**< Interrupt Flag Register                            */
  __IOM uint32_t NSIEN_SET;                     /**< Interrupt Enable Register                          */
  uint32_t       RESERVED8[3U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED9[8U];                 /**< Reserved for future use                            */
  __IOM uint32_t PPUNSPATD0_SET;                /**< Privileged Access                                  */
  __IOM uint32_t PPUNSPATD1_SET;                /**< Privileged Access                                  */
  uint32_t       RESERVED10[62U];               /**< Reserved for future use                            */
  __IM uint32_t  PPUNSFS_SET;                   /**< Fault Status                                       */
  uint32_t       RESERVED11[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BMPUNSPATD0_SET;               /**< Privileged Attribute                               */
  uint32_t       RESERVED12[63U];               /**< Reserved for future use                            */
  uint32_t       RESERVED13[876U];              /**< Reserved for future use                            */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
  __IM uint32_t  NSSTATUS_CLR;                  /**< Status Register                                    */
  __IOM uint32_t NSLOCK_CLR;                    /**< Lock Register                                      */
  __IOM uint32_t NSIF_CLR;                      /**< Interrupt Flag Register                            */
  __IOM uint32_t NSIEN_CLR;                     /**< Interrupt Enable Register                          */
  uint32_t       RESERVED15[3U];                /**< Reserved for future use                            */
  uint32_t       RESERVED16[8U];                /**< Reserved for future use                            */
  __IOM uint32_t PPUNSPATD0_CLR;                /**< Privileged Access                                  */
  __IOM uint32_t PPUNSPATD1_CLR;                /**< Privileged Access                                  */
  uint32_t       RESERVED17[62U];               /**< Reserved for future use                            */
  __IM uint32_t  PPUNSFS_CLR;                   /**< Fault Status                                       */
  uint32_t       RESERVED18[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BMPUNSPATD0_CLR;               /**< Privileged Attribute                               */
  uint32_t       RESERVED19[63U];               /**< Reserved for future use                            */
  uint32_t       RESERVED20[876U];              /**< Reserved for future use                            */
  uint32_t       RESERVED21[1U];                /**< Reserved for future use                            */
  __IM uint32_t  NSSTATUS_TGL;                  /**< Status Register                                    */
  __IOM uint32_t NSLOCK_TGL;                    /**< Lock Register                                      */
  __IOM uint32_t NSIF_TGL;                      /**< Interrupt Flag Register                            */
  __IOM uint32_t NSIEN_TGL;                     /**< Interrupt Enable Register                          */
  uint32_t       RESERVED22[3U];                /**< Reserved for future use                            */
  uint32_t       RESERVED23[8U];                /**< Reserved for future use                            */
  __IOM uint32_t PPUNSPATD0_TGL;                /**< Privileged Access                                  */
  __IOM uint32_t PPUNSPATD1_TGL;                /**< Privileged Access                                  */
  uint32_t       RESERVED24[62U];               /**< Reserved for future use                            */
  __IM uint32_t  PPUNSFS_TGL;                   /**< Fault Status                                       */
  uint32_t       RESERVED25[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BMPUNSPATD0_TGL;               /**< Privileged Attribute                               */
  uint32_t       RESERVED26[63U];               /**< Reserved for future use                            */
} SMU_CFGNS_TypeDef;
/** @} End of group EFR32ZG23_SMU_CFGNS */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_SMU_CFGNS
 * @{
 * @defgroup EFR32ZG23_SMU_CFGNS_BitFields SMU_CFGNS Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SMU NSSTATUS */
#define _SMU_NSSTATUS_RESETVALUE                   0x00000000UL                            /**< Default value for SMU_NSSTATUS              */
#define _SMU_NSSTATUS_MASK                         0x00000001UL                            /**< Mask for SMU_NSSTATUS                       */
#define SMU_NSSTATUS_SMUNSLOCK                     (0x1UL << 0)                            /**< SMUNS Lock                                  */
#define _SMU_NSSTATUS_SMUNSLOCK_SHIFT              0                                       /**< Shift value for SMU_SMUNSLOCK               */
#define _SMU_NSSTATUS_SMUNSLOCK_MASK               0x1UL                                   /**< Bit mask for SMU_SMUNSLOCK                  */
#define _SMU_NSSTATUS_SMUNSLOCK_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for SMU_NSSTATUS               */
#define _SMU_NSSTATUS_SMUNSLOCK_UNLOCKED           0x00000000UL                            /**< Mode UNLOCKED for SMU_NSSTATUS              */
#define _SMU_NSSTATUS_SMUNSLOCK_LOCKED             0x00000001UL                            /**< Mode LOCKED for SMU_NSSTATUS                */
#define SMU_NSSTATUS_SMUNSLOCK_DEFAULT             (_SMU_NSSTATUS_SMUNSLOCK_DEFAULT << 0)  /**< Shifted mode DEFAULT for SMU_NSSTATUS       */
#define SMU_NSSTATUS_SMUNSLOCK_UNLOCKED            (_SMU_NSSTATUS_SMUNSLOCK_UNLOCKED << 0) /**< Shifted mode UNLOCKED for SMU_NSSTATUS      */
#define SMU_NSSTATUS_SMUNSLOCK_LOCKED              (_SMU_NSSTATUS_SMUNSLOCK_LOCKED << 0)   /**< Shifted mode LOCKED for SMU_NSSTATUS        */

/* Bit fields for SMU NSLOCK */
#define _SMU_NSLOCK_RESETVALUE                     0x00000000UL                            /**< Default value for SMU_NSLOCK                */
#define _SMU_NSLOCK_MASK                           0x00FFFFFFUL                            /**< Mask for SMU_NSLOCK                         */
#define _SMU_NSLOCK_SMUNSLOCKKEY_SHIFT             0                                       /**< Shift value for SMU_SMUNSLOCKKEY            */
#define _SMU_NSLOCK_SMUNSLOCKKEY_MASK              0xFFFFFFUL                              /**< Bit mask for SMU_SMUNSLOCKKEY               */
#define _SMU_NSLOCK_SMUNSLOCKKEY_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for SMU_NSLOCK                 */
#define _SMU_NSLOCK_SMUNSLOCKKEY_UNLOCK            0x00ACCE55UL                            /**< Mode UNLOCK for SMU_NSLOCK                  */
#define SMU_NSLOCK_SMUNSLOCKKEY_DEFAULT            (_SMU_NSLOCK_SMUNSLOCKKEY_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_NSLOCK         */
#define SMU_NSLOCK_SMUNSLOCKKEY_UNLOCK             (_SMU_NSLOCK_SMUNSLOCKKEY_UNLOCK << 0)  /**< Shifted mode UNLOCK for SMU_NSLOCK          */

/* Bit fields for SMU NSIF */
#define _SMU_NSIF_RESETVALUE                       0x00000000UL                         /**< Default value for SMU_NSIF                  */
#define _SMU_NSIF_MASK                             0x00000005UL                         /**< Mask for SMU_NSIF                           */
#define SMU_NSIF_PPUNSPRIV                         (0x1UL << 0)                         /**< PPUNS Privilege Interrupt Flag              */
#define _SMU_NSIF_PPUNSPRIV_SHIFT                  0                                    /**< Shift value for SMU_PPUNSPRIV               */
#define _SMU_NSIF_PPUNSPRIV_MASK                   0x1UL                                /**< Bit mask for SMU_PPUNSPRIV                  */
#define _SMU_NSIF_PPUNSPRIV_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for SMU_NSIF                   */
#define SMU_NSIF_PPUNSPRIV_DEFAULT                 (_SMU_NSIF_PPUNSPRIV_DEFAULT << 0)   /**< Shifted mode DEFAULT for SMU_NSIF           */
#define SMU_NSIF_PPUNSINST                         (0x1UL << 2)                         /**< PPUNS Instruction Interrupt Flag            */
#define _SMU_NSIF_PPUNSINST_SHIFT                  2                                    /**< Shift value for SMU_PPUNSINST               */
#define _SMU_NSIF_PPUNSINST_MASK                   0x4UL                                /**< Bit mask for SMU_PPUNSINST                  */
#define _SMU_NSIF_PPUNSINST_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for SMU_NSIF                   */
#define SMU_NSIF_PPUNSINST_DEFAULT                 (_SMU_NSIF_PPUNSINST_DEFAULT << 2)   /**< Shifted mode DEFAULT for SMU_NSIF           */

/* Bit fields for SMU NSIEN */
#define _SMU_NSIEN_RESETVALUE                      0x00000000UL                         /**< Default value for SMU_NSIEN                 */
#define _SMU_NSIEN_MASK                            0x00000005UL                         /**< Mask for SMU_NSIEN                          */
#define SMU_NSIEN_PPUNSPRIV                        (0x1UL << 0)                         /**< PPUNS Privilege Interrupt Enable            */
#define _SMU_NSIEN_PPUNSPRIV_SHIFT                 0                                    /**< Shift value for SMU_PPUNSPRIV               */
#define _SMU_NSIEN_PPUNSPRIV_MASK                  0x1UL                                /**< Bit mask for SMU_PPUNSPRIV                  */
#define _SMU_NSIEN_PPUNSPRIV_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for SMU_NSIEN                  */
#define SMU_NSIEN_PPUNSPRIV_DEFAULT                (_SMU_NSIEN_PPUNSPRIV_DEFAULT << 0)  /**< Shifted mode DEFAULT for SMU_NSIEN          */
#define SMU_NSIEN_PPUNSINST                        (0x1UL << 2)                         /**< PPUNS Instruction Interrupt Enable          */
#define _SMU_NSIEN_PPUNSINST_SHIFT                 2                                    /**< Shift value for SMU_PPUNSINST               */
#define _SMU_NSIEN_PPUNSINST_MASK                  0x4UL                                /**< Bit mask for SMU_PPUNSINST                  */
#define _SMU_NSIEN_PPUNSINST_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for SMU_NSIEN                  */
#define SMU_NSIEN_PPUNSINST_DEFAULT                (_SMU_NSIEN_PPUNSINST_DEFAULT << 2)  /**< Shifted mode DEFAULT for SMU_NSIEN          */

/* Bit fields for SMU PPUNSPATD0 */
#define _SMU_PPUNSPATD0_RESETVALUE                 0x00000000UL                                 /**< Default value for SMU_PPUNSPATD0            */
#define _SMU_PPUNSPATD0_MASK                       0xFFFFFFFFUL                                 /**< Mask for SMU_PPUNSPATD0                     */
#define SMU_PPUNSPATD0_SCRATCHPAD                  (0x1UL << 0)                                 /**< SCRATCHPAD Privileged Access                */
#define _SMU_PPUNSPATD0_SCRATCHPAD_SHIFT           0                                            /**< Shift value for SMU_SCRATCHPAD              */
#define _SMU_PPUNSPATD0_SCRATCHPAD_MASK            0x1UL                                        /**< Bit mask for SMU_SCRATCHPAD                 */
#define _SMU_PPUNSPATD0_SCRATCHPAD_DEFAULT         0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_SCRATCHPAD_DEFAULT          (_SMU_PPUNSPATD0_SCRATCHPAD_DEFAULT << 0)    /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_EMU                         (0x1UL << 1)                                 /**< EMU Privileged Access                       */
#define _SMU_PPUNSPATD0_EMU_SHIFT                  1                                            /**< Shift value for SMU_EMU                     */
#define _SMU_PPUNSPATD0_EMU_MASK                   0x2UL                                        /**< Bit mask for SMU_EMU                        */
#define _SMU_PPUNSPATD0_EMU_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_EMU_DEFAULT                 (_SMU_PPUNSPATD0_EMU_DEFAULT << 1)           /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_CMU                         (0x1UL << 2)                                 /**< CMU Privileged Access                       */
#define _SMU_PPUNSPATD0_CMU_SHIFT                  2                                            /**< Shift value for SMU_CMU                     */
#define _SMU_PPUNSPATD0_CMU_MASK                   0x4UL                                        /**< Bit mask for SMU_CMU                        */
#define _SMU_PPUNSPATD0_CMU_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_CMU_DEFAULT                 (_SMU_PPUNSPATD0_CMU_DEFAULT << 2)           /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_HFRCO0                      (0x1UL << 3)                                 /**< HFRCO0 Privileged Access                    */
#define _SMU_PPUNSPATD0_HFRCO0_SHIFT               3                                            /**< Shift value for SMU_HFRCO0                  */
#define _SMU_PPUNSPATD0_HFRCO0_MASK                0x8UL                                        /**< Bit mask for SMU_HFRCO0                     */
#define _SMU_PPUNSPATD0_HFRCO0_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_HFRCO0_DEFAULT              (_SMU_PPUNSPATD0_HFRCO0_DEFAULT << 3)        /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_FSRCO                       (0x1UL << 4)                                 /**< FSRCO Privileged Access                     */
#define _SMU_PPUNSPATD0_FSRCO_SHIFT                4                                            /**< Shift value for SMU_FSRCO                   */
#define _SMU_PPUNSPATD0_FSRCO_MASK                 0x10UL                                       /**< Bit mask for SMU_FSRCO                      */
#define _SMU_PPUNSPATD0_FSRCO_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_FSRCO_DEFAULT               (_SMU_PPUNSPATD0_FSRCO_DEFAULT << 4)         /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_DPLL0                       (0x1UL << 5)                                 /**< DPLL0 Privileged Access                     */
#define _SMU_PPUNSPATD0_DPLL0_SHIFT                5                                            /**< Shift value for SMU_DPLL0                   */
#define _SMU_PPUNSPATD0_DPLL0_MASK                 0x20UL                                       /**< Bit mask for SMU_DPLL0                      */
#define _SMU_PPUNSPATD0_DPLL0_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_DPLL0_DEFAULT               (_SMU_PPUNSPATD0_DPLL0_DEFAULT << 5)         /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_LFXO                        (0x1UL << 6)                                 /**< LFXO Privileged Access                      */
#define _SMU_PPUNSPATD0_LFXO_SHIFT                 6                                            /**< Shift value for SMU_LFXO                    */
#define _SMU_PPUNSPATD0_LFXO_MASK                  0x40UL                                       /**< Bit mask for SMU_LFXO                       */
#define _SMU_PPUNSPATD0_LFXO_DEFAULT               0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_LFXO_DEFAULT                (_SMU_PPUNSPATD0_LFXO_DEFAULT << 6)          /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_LFRCO                       (0x1UL << 7)                                 /**< LFRCO Privileged Access                     */
#define _SMU_PPUNSPATD0_LFRCO_SHIFT                7                                            /**< Shift value for SMU_LFRCO                   */
#define _SMU_PPUNSPATD0_LFRCO_MASK                 0x80UL                                       /**< Bit mask for SMU_LFRCO                      */
#define _SMU_PPUNSPATD0_LFRCO_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_LFRCO_DEFAULT               (_SMU_PPUNSPATD0_LFRCO_DEFAULT << 7)         /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_ULFRCO                      (0x1UL << 8)                                 /**< ULFRCO Privileged Access                    */
#define _SMU_PPUNSPATD0_ULFRCO_SHIFT               8                                            /**< Shift value for SMU_ULFRCO                  */
#define _SMU_PPUNSPATD0_ULFRCO_MASK                0x100UL                                      /**< Bit mask for SMU_ULFRCO                     */
#define _SMU_PPUNSPATD0_ULFRCO_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_ULFRCO_DEFAULT              (_SMU_PPUNSPATD0_ULFRCO_DEFAULT << 8)        /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_MSC                         (0x1UL << 9)                                 /**< MSC Privileged Access                       */
#define _SMU_PPUNSPATD0_MSC_SHIFT                  9                                            /**< Shift value for SMU_MSC                     */
#define _SMU_PPUNSPATD0_MSC_MASK                   0x200UL                                      /**< Bit mask for SMU_MSC                        */
#define _SMU_PPUNSPATD0_MSC_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_MSC_DEFAULT                 (_SMU_PPUNSPATD0_MSC_DEFAULT << 9)           /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_ICACHE0                     (0x1UL << 10)                                /**< ICACHE0 Privileged Access                   */
#define _SMU_PPUNSPATD0_ICACHE0_SHIFT              10                                           /**< Shift value for SMU_ICACHE0                 */
#define _SMU_PPUNSPATD0_ICACHE0_MASK               0x400UL                                      /**< Bit mask for SMU_ICACHE0                    */
#define _SMU_PPUNSPATD0_ICACHE0_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_ICACHE0_DEFAULT             (_SMU_PPUNSPATD0_ICACHE0_DEFAULT << 10)      /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_PRS                         (0x1UL << 11)                                /**< PRS Privileged Access                       */
#define _SMU_PPUNSPATD0_PRS_SHIFT                  11                                           /**< Shift value for SMU_PRS                     */
#define _SMU_PPUNSPATD0_PRS_MASK                   0x800UL                                      /**< Bit mask for SMU_PRS                        */
#define _SMU_PPUNSPATD0_PRS_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_PRS_DEFAULT                 (_SMU_PPUNSPATD0_PRS_DEFAULT << 11)          /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_GPIO                        (0x1UL << 12)                                /**< GPIO Privileged Access                      */
#define _SMU_PPUNSPATD0_GPIO_SHIFT                 12                                           /**< Shift value for SMU_GPIO                    */
#define _SMU_PPUNSPATD0_GPIO_MASK                  0x1000UL                                     /**< Bit mask for SMU_GPIO                       */
#define _SMU_PPUNSPATD0_GPIO_DEFAULT               0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_GPIO_DEFAULT                (_SMU_PPUNSPATD0_GPIO_DEFAULT << 12)         /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_LDMA                        (0x1UL << 13)                                /**< LDMA Privileged Access                      */
#define _SMU_PPUNSPATD0_LDMA_SHIFT                 13                                           /**< Shift value for SMU_LDMA                    */
#define _SMU_PPUNSPATD0_LDMA_MASK                  0x2000UL                                     /**< Bit mask for SMU_LDMA                       */
#define _SMU_PPUNSPATD0_LDMA_DEFAULT               0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_LDMA_DEFAULT                (_SMU_PPUNSPATD0_LDMA_DEFAULT << 13)         /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_LDMAXBAR                    (0x1UL << 14)                                /**< LDMAXBAR Privileged Access                  */
#define _SMU_PPUNSPATD0_LDMAXBAR_SHIFT             14                                           /**< Shift value for SMU_LDMAXBAR                */
#define _SMU_PPUNSPATD0_LDMAXBAR_MASK              0x4000UL                                     /**< Bit mask for SMU_LDMAXBAR                   */
#define _SMU_PPUNSPATD0_LDMAXBAR_DEFAULT           0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_LDMAXBAR_DEFAULT            (_SMU_PPUNSPATD0_LDMAXBAR_DEFAULT << 14)     /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_TIMER0                      (0x1UL << 15)                                /**< TIMER0 Privileged Access                    */
#define _SMU_PPUNSPATD0_TIMER0_SHIFT               15                                           /**< Shift value for SMU_TIMER0                  */
#define _SMU_PPUNSPATD0_TIMER0_MASK                0x8000UL                                     /**< Bit mask for SMU_TIMER0                     */
#define _SMU_PPUNSPATD0_TIMER0_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_TIMER0_DEFAULT              (_SMU_PPUNSPATD0_TIMER0_DEFAULT << 15)       /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_TIMER1                      (0x1UL << 16)                                /**< TIMER1 Privileged Access                    */
#define _SMU_PPUNSPATD0_TIMER1_SHIFT               16                                           /**< Shift value for SMU_TIMER1                  */
#define _SMU_PPUNSPATD0_TIMER1_MASK                0x10000UL                                    /**< Bit mask for SMU_TIMER1                     */
#define _SMU_PPUNSPATD0_TIMER1_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_TIMER1_DEFAULT              (_SMU_PPUNSPATD0_TIMER1_DEFAULT << 16)       /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_TIMER2                      (0x1UL << 17)                                /**< TIMER2 Privileged Access                    */
#define _SMU_PPUNSPATD0_TIMER2_SHIFT               17                                           /**< Shift value for SMU_TIMER2                  */
#define _SMU_PPUNSPATD0_TIMER2_MASK                0x20000UL                                    /**< Bit mask for SMU_TIMER2                     */
#define _SMU_PPUNSPATD0_TIMER2_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_TIMER2_DEFAULT              (_SMU_PPUNSPATD0_TIMER2_DEFAULT << 17)       /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_TIMER3                      (0x1UL << 18)                                /**< TIMER3 Privileged Access                    */
#define _SMU_PPUNSPATD0_TIMER3_SHIFT               18                                           /**< Shift value for SMU_TIMER3                  */
#define _SMU_PPUNSPATD0_TIMER3_MASK                0x40000UL                                    /**< Bit mask for SMU_TIMER3                     */
#define _SMU_PPUNSPATD0_TIMER3_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_TIMER3_DEFAULT              (_SMU_PPUNSPATD0_TIMER3_DEFAULT << 18)       /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_TIMER4                      (0x1UL << 19)                                /**< TIMER4 Privileged Access                    */
#define _SMU_PPUNSPATD0_TIMER4_SHIFT               19                                           /**< Shift value for SMU_TIMER4                  */
#define _SMU_PPUNSPATD0_TIMER4_MASK                0x80000UL                                    /**< Bit mask for SMU_TIMER4                     */
#define _SMU_PPUNSPATD0_TIMER4_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_TIMER4_DEFAULT              (_SMU_PPUNSPATD0_TIMER4_DEFAULT << 19)       /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_USART0                      (0x1UL << 20)                                /**< USART0 Privileged Access                    */
#define _SMU_PPUNSPATD0_USART0_SHIFT               20                                           /**< Shift value for SMU_USART0                  */
#define _SMU_PPUNSPATD0_USART0_MASK                0x100000UL                                   /**< Bit mask for SMU_USART0                     */
#define _SMU_PPUNSPATD0_USART0_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_USART0_DEFAULT              (_SMU_PPUNSPATD0_USART0_DEFAULT << 20)       /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_BURTC                       (0x1UL << 21)                                /**< BURTC Privileged Access                     */
#define _SMU_PPUNSPATD0_BURTC_SHIFT                21                                           /**< Shift value for SMU_BURTC                   */
#define _SMU_PPUNSPATD0_BURTC_MASK                 0x200000UL                                   /**< Bit mask for SMU_BURTC                      */
#define _SMU_PPUNSPATD0_BURTC_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_BURTC_DEFAULT               (_SMU_PPUNSPATD0_BURTC_DEFAULT << 21)        /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_I2C1                        (0x1UL << 22)                                /**< I2C1 Privileged Access                      */
#define _SMU_PPUNSPATD0_I2C1_SHIFT                 22                                           /**< Shift value for SMU_I2C1                    */
#define _SMU_PPUNSPATD0_I2C1_MASK                  0x400000UL                                   /**< Bit mask for SMU_I2C1                       */
#define _SMU_PPUNSPATD0_I2C1_DEFAULT               0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_I2C1_DEFAULT                (_SMU_PPUNSPATD0_I2C1_DEFAULT << 22)         /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_CHIPTESTCTRL                (0x1UL << 23)                                /**< CHIPTESTCTRL Privileged Access              */
#define _SMU_PPUNSPATD0_CHIPTESTCTRL_SHIFT         23                                           /**< Shift value for SMU_CHIPTESTCTRL            */
#define _SMU_PPUNSPATD0_CHIPTESTCTRL_MASK          0x800000UL                                   /**< Bit mask for SMU_CHIPTESTCTRL               */
#define _SMU_PPUNSPATD0_CHIPTESTCTRL_DEFAULT       0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_CHIPTESTCTRL_DEFAULT        (_SMU_PPUNSPATD0_CHIPTESTCTRL_DEFAULT << 23) /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_SYSCFGCFGNS                 (0x1UL << 24)                                /**< SYSCFGCFGNS Privileged Access               */
#define _SMU_PPUNSPATD0_SYSCFGCFGNS_SHIFT          24                                           /**< Shift value for SMU_SYSCFGCFGNS             */
#define _SMU_PPUNSPATD0_SYSCFGCFGNS_MASK           0x1000000UL                                  /**< Bit mask for SMU_SYSCFGCFGNS                */
#define _SMU_PPUNSPATD0_SYSCFGCFGNS_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_SYSCFGCFGNS_DEFAULT         (_SMU_PPUNSPATD0_SYSCFGCFGNS_DEFAULT << 24)  /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_SYSCFG                      (0x1UL << 25)                                /**< SYSCFG Privileged Access                    */
#define _SMU_PPUNSPATD0_SYSCFG_SHIFT               25                                           /**< Shift value for SMU_SYSCFG                  */
#define _SMU_PPUNSPATD0_SYSCFG_MASK                0x2000000UL                                  /**< Bit mask for SMU_SYSCFG                     */
#define _SMU_PPUNSPATD0_SYSCFG_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_SYSCFG_DEFAULT              (_SMU_PPUNSPATD0_SYSCFG_DEFAULT << 25)       /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_BURAM                       (0x1UL << 26)                                /**< BURAM Privileged Access                     */
#define _SMU_PPUNSPATD0_BURAM_SHIFT                26                                           /**< Shift value for SMU_BURAM                   */
#define _SMU_PPUNSPATD0_BURAM_MASK                 0x4000000UL                                  /**< Bit mask for SMU_BURAM                      */
#define _SMU_PPUNSPATD0_BURAM_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_BURAM_DEFAULT               (_SMU_PPUNSPATD0_BURAM_DEFAULT << 26)        /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_GPCRC                       (0x1UL << 27)                                /**< GPCRC Privileged Access                     */
#define _SMU_PPUNSPATD0_GPCRC_SHIFT                27                                           /**< Shift value for SMU_GPCRC                   */
#define _SMU_PPUNSPATD0_GPCRC_MASK                 0x8000000UL                                  /**< Bit mask for SMU_GPCRC                      */
#define _SMU_PPUNSPATD0_GPCRC_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_GPCRC_DEFAULT               (_SMU_PPUNSPATD0_GPCRC_DEFAULT << 27)        /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_DCDC                        (0x1UL << 28)                                /**< DCDC Privileged Access                      */
#define _SMU_PPUNSPATD0_DCDC_SHIFT                 28                                           /**< Shift value for SMU_DCDC                    */
#define _SMU_PPUNSPATD0_DCDC_MASK                  0x10000000UL                                 /**< Bit mask for SMU_DCDC                       */
#define _SMU_PPUNSPATD0_DCDC_DEFAULT               0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_DCDC_DEFAULT                (_SMU_PPUNSPATD0_DCDC_DEFAULT << 28)         /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_HOSTMAILBOX                 (0x1UL << 29)                                /**< HOSTMAILBOX Privileged Access               */
#define _SMU_PPUNSPATD0_HOSTMAILBOX_SHIFT          29                                           /**< Shift value for SMU_HOSTMAILBOX             */
#define _SMU_PPUNSPATD0_HOSTMAILBOX_MASK           0x20000000UL                                 /**< Bit mask for SMU_HOSTMAILBOX                */
#define _SMU_PPUNSPATD0_HOSTMAILBOX_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_HOSTMAILBOX_DEFAULT         (_SMU_PPUNSPATD0_HOSTMAILBOX_DEFAULT << 29)  /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_EUSART1                     (0x1UL << 30)                                /**< EUSART1 Privileged Access                   */
#define _SMU_PPUNSPATD0_EUSART1_SHIFT              30                                           /**< Shift value for SMU_EUSART1                 */
#define _SMU_PPUNSPATD0_EUSART1_MASK               0x40000000UL                                 /**< Bit mask for SMU_EUSART1                    */
#define _SMU_PPUNSPATD0_EUSART1_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_EUSART1_DEFAULT             (_SMU_PPUNSPATD0_EUSART1_DEFAULT << 30)      /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */
#define SMU_PPUNSPATD0_EUSART2                     (0x1UL << 31)                                /**< EUSART2 Privileged Access                   */
#define _SMU_PPUNSPATD0_EUSART2_SHIFT              31                                           /**< Shift value for SMU_EUSART2                 */
#define _SMU_PPUNSPATD0_EUSART2_MASK               0x80000000UL                                 /**< Bit mask for SMU_EUSART2                    */
#define _SMU_PPUNSPATD0_EUSART2_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for SMU_PPUNSPATD0             */
#define SMU_PPUNSPATD0_EUSART2_DEFAULT             (_SMU_PPUNSPATD0_EUSART2_DEFAULT << 31)      /**< Shifted mode DEFAULT for SMU_PPUNSPATD0     */

/* Bit fields for SMU PPUNSPATD1 */
#define _SMU_PPUNSPATD1_RESETVALUE                 0x00000000UL                              /**< Default value for SMU_PPUNSPATD1            */
#define _SMU_PPUNSPATD1_MASK                       0x01FFFFFFUL                              /**< Mask for SMU_PPUNSPATD1                     */
#define SMU_PPUNSPATD1_SYSRTC                      (0x1UL << 0)                              /**< SYSRTC Privileged Access                    */
#define _SMU_PPUNSPATD1_SYSRTC_SHIFT               0                                         /**< Shift value for SMU_SYSRTC                  */
#define _SMU_PPUNSPATD1_SYSRTC_MASK                0x1UL                                     /**< Bit mask for SMU_SYSRTC                     */
#define _SMU_PPUNSPATD1_SYSRTC_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_SYSRTC_DEFAULT              (_SMU_PPUNSPATD1_SYSRTC_DEFAULT << 0)     /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_LCD                         (0x1UL << 1)                              /**< LCD Privileged Access                       */
#define _SMU_PPUNSPATD1_LCD_SHIFT                  1                                         /**< Shift value for SMU_LCD                     */
#define _SMU_PPUNSPATD1_LCD_MASK                   0x2UL                                     /**< Bit mask for SMU_LCD                        */
#define _SMU_PPUNSPATD1_LCD_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_LCD_DEFAULT                 (_SMU_PPUNSPATD1_LCD_DEFAULT << 1)        /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_KEYSCAN                     (0x1UL << 2)                              /**< KEYSCAN Privileged Access                   */
#define _SMU_PPUNSPATD1_KEYSCAN_SHIFT              2                                         /**< Shift value for SMU_KEYSCAN                 */
#define _SMU_PPUNSPATD1_KEYSCAN_MASK               0x4UL                                     /**< Bit mask for SMU_KEYSCAN                    */
#define _SMU_PPUNSPATD1_KEYSCAN_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_KEYSCAN_DEFAULT             (_SMU_PPUNSPATD1_KEYSCAN_DEFAULT << 2)    /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_DMEM                        (0x1UL << 3)                              /**< DMEM Privileged Access                      */
#define _SMU_PPUNSPATD1_DMEM_SHIFT                 3                                         /**< Shift value for SMU_DMEM                    */
#define _SMU_PPUNSPATD1_DMEM_MASK                  0x8UL                                     /**< Bit mask for SMU_DMEM                       */
#define _SMU_PPUNSPATD1_DMEM_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_DMEM_DEFAULT                (_SMU_PPUNSPATD1_DMEM_DEFAULT << 3)       /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_LCDRF                       (0x1UL << 4)                              /**< LCDRF Privileged Access                     */
#define _SMU_PPUNSPATD1_LCDRF_SHIFT                4                                         /**< Shift value for SMU_LCDRF                   */
#define _SMU_PPUNSPATD1_LCDRF_MASK                 0x10UL                                    /**< Bit mask for SMU_LCDRF                      */
#define _SMU_PPUNSPATD1_LCDRF_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_LCDRF_DEFAULT               (_SMU_PPUNSPATD1_LCDRF_DEFAULT << 4)      /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_PFMXPPRF                    (0x1UL << 5)                              /**< PFMXPPRF Privileged Access                  */
#define _SMU_PPUNSPATD1_PFMXPPRF_SHIFT             5                                         /**< Shift value for SMU_PFMXPPRF                */
#define _SMU_PPUNSPATD1_PFMXPPRF_MASK              0x20UL                                    /**< Bit mask for SMU_PFMXPPRF                   */
#define _SMU_PPUNSPATD1_PFMXPPRF_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_PFMXPPRF_DEFAULT            (_SMU_PPUNSPATD1_PFMXPPRF_DEFAULT << 5)   /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_RADIOAES                    (0x1UL << 6)                              /**< RADIOAES Privileged Access                  */
#define _SMU_PPUNSPATD1_RADIOAES_SHIFT             6                                         /**< Shift value for SMU_RADIOAES                */
#define _SMU_PPUNSPATD1_RADIOAES_MASK              0x40UL                                    /**< Bit mask for SMU_RADIOAES                   */
#define _SMU_PPUNSPATD1_RADIOAES_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_RADIOAES_DEFAULT            (_SMU_PPUNSPATD1_RADIOAES_DEFAULT << 6)   /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_SMU                         (0x1UL << 7)                              /**< SMU Privileged Access                       */
#define _SMU_PPUNSPATD1_SMU_SHIFT                  7                                         /**< Shift value for SMU_SMU                     */
#define _SMU_PPUNSPATD1_SMU_MASK                   0x80UL                                    /**< Bit mask for SMU_SMU                        */
#define _SMU_PPUNSPATD1_SMU_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_SMU_DEFAULT                 (_SMU_PPUNSPATD1_SMU_DEFAULT << 7)        /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_SMUCFGNS                    (0x1UL << 8)                              /**< SMUCFGNS Privileged Access                  */
#define _SMU_PPUNSPATD1_SMUCFGNS_SHIFT             8                                         /**< Shift value for SMU_SMUCFGNS                */
#define _SMU_PPUNSPATD1_SMUCFGNS_MASK              0x100UL                                   /**< Bit mask for SMU_SMUCFGNS                   */
#define _SMU_PPUNSPATD1_SMUCFGNS_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_SMUCFGNS_DEFAULT            (_SMU_PPUNSPATD1_SMUCFGNS_DEFAULT << 8)   /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_LETIMER0                    (0x1UL << 9)                              /**< LETIMER0 Privileged Access                  */
#define _SMU_PPUNSPATD1_LETIMER0_SHIFT             9                                         /**< Shift value for SMU_LETIMER0                */
#define _SMU_PPUNSPATD1_LETIMER0_MASK              0x200UL                                   /**< Bit mask for SMU_LETIMER0                   */
#define _SMU_PPUNSPATD1_LETIMER0_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_LETIMER0_DEFAULT            (_SMU_PPUNSPATD1_LETIMER0_DEFAULT << 9)   /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_IADC0                       (0x1UL << 10)                             /**< IADC0 Privileged Access                     */
#define _SMU_PPUNSPATD1_IADC0_SHIFT                10                                        /**< Shift value for SMU_IADC0                   */
#define _SMU_PPUNSPATD1_IADC0_MASK                 0x400UL                                   /**< Bit mask for SMU_IADC0                      */
#define _SMU_PPUNSPATD1_IADC0_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_IADC0_DEFAULT               (_SMU_PPUNSPATD1_IADC0_DEFAULT << 10)     /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_ACMP0                       (0x1UL << 11)                             /**< ACMP0 Privileged Access                     */
#define _SMU_PPUNSPATD1_ACMP0_SHIFT                11                                        /**< Shift value for SMU_ACMP0                   */
#define _SMU_PPUNSPATD1_ACMP0_MASK                 0x800UL                                   /**< Bit mask for SMU_ACMP0                      */
#define _SMU_PPUNSPATD1_ACMP0_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_ACMP0_DEFAULT               (_SMU_PPUNSPATD1_ACMP0_DEFAULT << 11)     /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_ACMP1                       (0x1UL << 12)                             /**< ACMP1 Privileged Access                     */
#define _SMU_PPUNSPATD1_ACMP1_SHIFT                12                                        /**< Shift value for SMU_ACMP1                   */
#define _SMU_PPUNSPATD1_ACMP1_MASK                 0x1000UL                                  /**< Bit mask for SMU_ACMP1                      */
#define _SMU_PPUNSPATD1_ACMP1_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_ACMP1_DEFAULT               (_SMU_PPUNSPATD1_ACMP1_DEFAULT << 12)     /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_AMUXCP0                     (0x1UL << 13)                             /**< AMUXCP0 Privileged Access                   */
#define _SMU_PPUNSPATD1_AMUXCP0_SHIFT              13                                        /**< Shift value for SMU_AMUXCP0                 */
#define _SMU_PPUNSPATD1_AMUXCP0_MASK               0x2000UL                                  /**< Bit mask for SMU_AMUXCP0                    */
#define _SMU_PPUNSPATD1_AMUXCP0_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_AMUXCP0_DEFAULT             (_SMU_PPUNSPATD1_AMUXCP0_DEFAULT << 13)   /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_VDAC0                       (0x1UL << 14)                             /**< VDAC0 Privileged Access                     */
#define _SMU_PPUNSPATD1_VDAC0_SHIFT                14                                        /**< Shift value for SMU_VDAC0                   */
#define _SMU_PPUNSPATD1_VDAC0_MASK                 0x4000UL                                  /**< Bit mask for SMU_VDAC0                      */
#define _SMU_PPUNSPATD1_VDAC0_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_VDAC0_DEFAULT               (_SMU_PPUNSPATD1_VDAC0_DEFAULT << 14)     /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_PCNT                        (0x1UL << 15)                             /**< PCNT Privileged Access                      */
#define _SMU_PPUNSPATD1_PCNT_SHIFT                 15                                        /**< Shift value for SMU_PCNT                    */
#define _SMU_PPUNSPATD1_PCNT_MASK                  0x8000UL                                  /**< Bit mask for SMU_PCNT                       */
#define _SMU_PPUNSPATD1_PCNT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_PCNT_DEFAULT                (_SMU_PPUNSPATD1_PCNT_DEFAULT << 15)      /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_LESENSE                     (0x1UL << 16)                             /**< LESENSE Privileged Access                   */
#define _SMU_PPUNSPATD1_LESENSE_SHIFT              16                                        /**< Shift value for SMU_LESENSE                 */
#define _SMU_PPUNSPATD1_LESENSE_MASK               0x10000UL                                 /**< Bit mask for SMU_LESENSE                    */
#define _SMU_PPUNSPATD1_LESENSE_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_LESENSE_DEFAULT             (_SMU_PPUNSPATD1_LESENSE_DEFAULT << 16)   /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_HFRCO1                      (0x1UL << 17)                             /**< HFRCO1 Privileged Access                    */
#define _SMU_PPUNSPATD1_HFRCO1_SHIFT               17                                        /**< Shift value for SMU_HFRCO1                  */
#define _SMU_PPUNSPATD1_HFRCO1_MASK                0x20000UL                                 /**< Bit mask for SMU_HFRCO1                     */
#define _SMU_PPUNSPATD1_HFRCO1_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_HFRCO1_DEFAULT              (_SMU_PPUNSPATD1_HFRCO1_DEFAULT << 17)    /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_HFXO0                       (0x1UL << 18)                             /**< HFXO0 Privileged Access                     */
#define _SMU_PPUNSPATD1_HFXO0_SHIFT                18                                        /**< Shift value for SMU_HFXO0                   */
#define _SMU_PPUNSPATD1_HFXO0_MASK                 0x40000UL                                 /**< Bit mask for SMU_HFXO0                      */
#define _SMU_PPUNSPATD1_HFXO0_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_HFXO0_DEFAULT               (_SMU_PPUNSPATD1_HFXO0_DEFAULT << 18)     /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_I2C0                        (0x1UL << 19)                             /**< I2C0 Privileged Access                      */
#define _SMU_PPUNSPATD1_I2C0_SHIFT                 19                                        /**< Shift value for SMU_I2C0                    */
#define _SMU_PPUNSPATD1_I2C0_MASK                  0x80000UL                                 /**< Bit mask for SMU_I2C0                       */
#define _SMU_PPUNSPATD1_I2C0_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_I2C0_DEFAULT                (_SMU_PPUNSPATD1_I2C0_DEFAULT << 19)      /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_WDOG0                       (0x1UL << 20)                             /**< WDOG0 Privileged Access                     */
#define _SMU_PPUNSPATD1_WDOG0_SHIFT                20                                        /**< Shift value for SMU_WDOG0                   */
#define _SMU_PPUNSPATD1_WDOG0_MASK                 0x100000UL                                /**< Bit mask for SMU_WDOG0                      */
#define _SMU_PPUNSPATD1_WDOG0_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_WDOG0_DEFAULT               (_SMU_PPUNSPATD1_WDOG0_DEFAULT << 20)     /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_WDOG1                       (0x1UL << 21)                             /**< WDOG1 Privileged Access                     */
#define _SMU_PPUNSPATD1_WDOG1_SHIFT                21                                        /**< Shift value for SMU_WDOG1                   */
#define _SMU_PPUNSPATD1_WDOG1_MASK                 0x200000UL                                /**< Bit mask for SMU_WDOG1                      */
#define _SMU_PPUNSPATD1_WDOG1_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_WDOG1_DEFAULT               (_SMU_PPUNSPATD1_WDOG1_DEFAULT << 21)     /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_EUSART0                     (0x1UL << 22)                             /**< EUSART0 Privileged Access                   */
#define _SMU_PPUNSPATD1_EUSART0_SHIFT              22                                        /**< Shift value for SMU_EUSART0                 */
#define _SMU_PPUNSPATD1_EUSART0_MASK               0x400000UL                                /**< Bit mask for SMU_EUSART0                    */
#define _SMU_PPUNSPATD1_EUSART0_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_EUSART0_DEFAULT             (_SMU_PPUNSPATD1_EUSART0_DEFAULT << 22)   /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_SEMAILBOX                   (0x1UL << 23)                             /**< SEMAILBOX Privileged Access                 */
#define _SMU_PPUNSPATD1_SEMAILBOX_SHIFT            23                                        /**< Shift value for SMU_SEMAILBOX               */
#define _SMU_PPUNSPATD1_SEMAILBOX_MASK             0x800000UL                                /**< Bit mask for SMU_SEMAILBOX                  */
#define _SMU_PPUNSPATD1_SEMAILBOX_DEFAULT          0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_SEMAILBOX_DEFAULT           (_SMU_PPUNSPATD1_SEMAILBOX_DEFAULT << 23) /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */
#define SMU_PPUNSPATD1_AHBRADIO                    (0x1UL << 24)                             /**< AHBRADIO Privileged Access                  */
#define _SMU_PPUNSPATD1_AHBRADIO_SHIFT             24                                        /**< Shift value for SMU_AHBRADIO                */
#define _SMU_PPUNSPATD1_AHBRADIO_MASK              0x1000000UL                               /**< Bit mask for SMU_AHBRADIO                   */
#define _SMU_PPUNSPATD1_AHBRADIO_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSPATD1             */
#define SMU_PPUNSPATD1_AHBRADIO_DEFAULT            (_SMU_PPUNSPATD1_AHBRADIO_DEFAULT << 24)  /**< Shifted mode DEFAULT for SMU_PPUNSPATD1     */

/* Bit fields for SMU PPUNSFS */
#define _SMU_PPUNSFS_RESETVALUE                    0x00000000UL                              /**< Default value for SMU_PPUNSFS               */
#define _SMU_PPUNSFS_MASK                          0x000000FFUL                              /**< Mask for SMU_PPUNSFS                        */
#define _SMU_PPUNSFS_PPUFSPERIPHID_SHIFT           0                                         /**< Shift value for SMU_PPUFSPERIPHID           */
#define _SMU_PPUNSFS_PPUFSPERIPHID_MASK            0xFFUL                                    /**< Bit mask for SMU_PPUFSPERIPHID              */
#define _SMU_PPUNSFS_PPUFSPERIPHID_DEFAULT         0x00000000UL                              /**< Mode DEFAULT for SMU_PPUNSFS                */
#define SMU_PPUNSFS_PPUFSPERIPHID_DEFAULT          (_SMU_PPUNSFS_PPUFSPERIPHID_DEFAULT << 0) /**< Shifted mode DEFAULT for SMU_PPUNSFS        */

/* Bit fields for SMU BMPUNSPATD0 */
#define _SMU_BMPUNSPATD0_RESETVALUE                0x00000000UL                                   /**< Default value for SMU_BMPUNSPATD0           */
#define _SMU_BMPUNSPATD0_MASK                      0x0000003FUL                                   /**< Mask for SMU_BMPUNSPATD0                    */
#define SMU_BMPUNSPATD0_RADIOAES                   (0x1UL << 0)                                   /**< RADIO AES DMA privileged mode               */
#define _SMU_BMPUNSPATD0_RADIOAES_SHIFT            0                                              /**< Shift value for SMU_RADIOAES                */
#define _SMU_BMPUNSPATD0_RADIOAES_MASK             0x1UL                                          /**< Bit mask for SMU_RADIOAES                   */
#define _SMU_BMPUNSPATD0_RADIOAES_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for SMU_BMPUNSPATD0            */
#define SMU_BMPUNSPATD0_RADIOAES_DEFAULT           (_SMU_BMPUNSPATD0_RADIOAES_DEFAULT << 0)       /**< Shifted mode DEFAULT for SMU_BMPUNSPATD0    */
#define SMU_BMPUNSPATD0_RADIOSUBSYSTEM             (0x1UL << 1)                                   /**< RADIO subsystem manager privileged mode     */
#define _SMU_BMPUNSPATD0_RADIOSUBSYSTEM_SHIFT      1                                              /**< Shift value for SMU_RADIOSUBSYSTEM          */
#define _SMU_BMPUNSPATD0_RADIOSUBSYSTEM_MASK       0x2UL                                          /**< Bit mask for SMU_RADIOSUBSYSTEM             */
#define _SMU_BMPUNSPATD0_RADIOSUBSYSTEM_DEFAULT    0x00000000UL                                   /**< Mode DEFAULT for SMU_BMPUNSPATD0            */
#define SMU_BMPUNSPATD0_RADIOSUBSYSTEM_DEFAULT     (_SMU_BMPUNSPATD0_RADIOSUBSYSTEM_DEFAULT << 1) /**< Shifted mode DEFAULT for SMU_BMPUNSPATD0    */
#define SMU_BMPUNSPATD0_LDMA                       (0x1UL << 2)                                   /**< MCU LDMA privileged mode                    */
#define _SMU_BMPUNSPATD0_LDMA_SHIFT                2                                              /**< Shift value for SMU_LDMA                    */
#define _SMU_BMPUNSPATD0_LDMA_MASK                 0x4UL                                          /**< Bit mask for SMU_LDMA                       */
#define _SMU_BMPUNSPATD0_LDMA_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for SMU_BMPUNSPATD0            */
#define SMU_BMPUNSPATD0_LDMA_DEFAULT               (_SMU_BMPUNSPATD0_LDMA_DEFAULT << 2)           /**< Shifted mode DEFAULT for SMU_BMPUNSPATD0    */
#define SMU_BMPUNSPATD0_RFECA0                     (0x1UL << 3)                                   /**< RFECA0 privileged mode                      */
#define _SMU_BMPUNSPATD0_RFECA0_SHIFT              3                                              /**< Shift value for SMU_RFECA0                  */
#define _SMU_BMPUNSPATD0_RFECA0_MASK               0x8UL                                          /**< Bit mask for SMU_RFECA0                     */
#define _SMU_BMPUNSPATD0_RFECA0_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for SMU_BMPUNSPATD0            */
#define SMU_BMPUNSPATD0_RFECA0_DEFAULT             (_SMU_BMPUNSPATD0_RFECA0_DEFAULT << 3)         /**< Shifted mode DEFAULT for SMU_BMPUNSPATD0    */
#define SMU_BMPUNSPATD0_RFECA1                     (0x1UL << 4)                                   /**< RFECA1 privileged mode                      */
#define _SMU_BMPUNSPATD0_RFECA1_SHIFT              4                                              /**< Shift value for SMU_RFECA1                  */
#define _SMU_BMPUNSPATD0_RFECA1_MASK               0x10UL                                         /**< Bit mask for SMU_RFECA1                     */
#define _SMU_BMPUNSPATD0_RFECA1_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for SMU_BMPUNSPATD0            */
#define SMU_BMPUNSPATD0_RFECA1_DEFAULT             (_SMU_BMPUNSPATD0_RFECA1_DEFAULT << 4)         /**< Shifted mode DEFAULT for SMU_BMPUNSPATD0    */
#define SMU_BMPUNSPATD0_SEEXTDMA                   (0x1UL << 5)                                   /**< SEEXTDMA privileged mode                    */
#define _SMU_BMPUNSPATD0_SEEXTDMA_SHIFT            5                                              /**< Shift value for SMU_SEEXTDMA                */
#define _SMU_BMPUNSPATD0_SEEXTDMA_MASK             0x20UL                                         /**< Bit mask for SMU_SEEXTDMA                   */
#define _SMU_BMPUNSPATD0_SEEXTDMA_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for SMU_BMPUNSPATD0            */
#define SMU_BMPUNSPATD0_SEEXTDMA_DEFAULT           (_SMU_BMPUNSPATD0_SEEXTDMA_DEFAULT << 5)       /**< Shifted mode DEFAULT for SMU_BMPUNSPATD0    */

/** @} End of group EFR32ZG23_SMU_CFGNS_BitFields */
/** @} End of group EFR32ZG23_SMU_CFGNS */
/** @} End of group Parts */

#endif /* EFR32ZG23_SMU_H */
#ifdef __cplusplus
}
#endif

