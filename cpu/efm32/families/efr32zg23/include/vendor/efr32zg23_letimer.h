/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 LETIMER register and bit field definitions
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

#ifndef EFR32ZG23_LETIMER_H
#define EFR32ZG23_LETIMER_H
#define LETIMER_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_LETIMER LETIMER
 * @{
 * @brief EFR32ZG23 LETIMER Register Declaration.
 *****************************************************************************/

/** LETIMER Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version                                         */
  __IOM uint32_t EN;                            /**< module en                                          */
  __IOM uint32_t SWRST;                         /**< Software Reset Register                            */
  __IOM uint32_t CTRL;                          /**< Control Register                                   */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t CNT;                           /**< Counter Value Register                             */
  __IOM uint32_t COMP0;                         /**< Compare Value Register 0                           */
  __IOM uint32_t COMP1;                         /**< Compare Value Register 1                           */
  __IOM uint32_t TOP;                           /**< Counter TOP Value Register                         */
  __IOM uint32_t TOPBUFF;                       /**< Buffered Counter TOP Value                         */
  __IOM uint32_t REP0;                          /**< Repeat Counter Register 0                          */
  __IOM uint32_t REP1;                          /**< Repeat Counter Register 1                          */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  __IOM uint32_t LOCK;                          /**< Configuration Lock Register                        */
  __IM uint32_t  SYNCBUSY;                      /**< Synchronization Busy Register                      */
  uint32_t       RESERVED0[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t PRSMODE;                       /**< PRS Input mode select Register                     */
  uint32_t       RESERVED1[1003U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version                                         */
  __IOM uint32_t EN_SET;                        /**< module en                                          */
  __IOM uint32_t SWRST_SET;                     /**< Software Reset Register                            */
  __IOM uint32_t CTRL_SET;                      /**< Control Register                                   */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t CNT_SET;                       /**< Counter Value Register                             */
  __IOM uint32_t COMP0_SET;                     /**< Compare Value Register 0                           */
  __IOM uint32_t COMP1_SET;                     /**< Compare Value Register 1                           */
  __IOM uint32_t TOP_SET;                       /**< Counter TOP Value Register                         */
  __IOM uint32_t TOPBUFF_SET;                   /**< Buffered Counter TOP Value                         */
  __IOM uint32_t REP0_SET;                      /**< Repeat Counter Register 0                          */
  __IOM uint32_t REP1_SET;                      /**< Repeat Counter Register 1                          */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t LOCK_SET;                      /**< Configuration Lock Register                        */
  __IM uint32_t  SYNCBUSY_SET;                  /**< Synchronization Busy Register                      */
  uint32_t       RESERVED2[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t PRSMODE_SET;                   /**< PRS Input mode select Register                     */
  uint32_t       RESERVED3[1003U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version                                         */
  __IOM uint32_t EN_CLR;                        /**< module en                                          */
  __IOM uint32_t SWRST_CLR;                     /**< Software Reset Register                            */
  __IOM uint32_t CTRL_CLR;                      /**< Control Register                                   */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t CNT_CLR;                       /**< Counter Value Register                             */
  __IOM uint32_t COMP0_CLR;                     /**< Compare Value Register 0                           */
  __IOM uint32_t COMP1_CLR;                     /**< Compare Value Register 1                           */
  __IOM uint32_t TOP_CLR;                       /**< Counter TOP Value Register                         */
  __IOM uint32_t TOPBUFF_CLR;                   /**< Buffered Counter TOP Value                         */
  __IOM uint32_t REP0_CLR;                      /**< Repeat Counter Register 0                          */
  __IOM uint32_t REP1_CLR;                      /**< Repeat Counter Register 1                          */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t LOCK_CLR;                      /**< Configuration Lock Register                        */
  __IM uint32_t  SYNCBUSY_CLR;                  /**< Synchronization Busy Register                      */
  uint32_t       RESERVED4[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t PRSMODE_CLR;                   /**< PRS Input mode select Register                     */
  uint32_t       RESERVED5[1003U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version                                         */
  __IOM uint32_t EN_TGL;                        /**< module en                                          */
  __IOM uint32_t SWRST_TGL;                     /**< Software Reset Register                            */
  __IOM uint32_t CTRL_TGL;                      /**< Control Register                                   */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t CNT_TGL;                       /**< Counter Value Register                             */
  __IOM uint32_t COMP0_TGL;                     /**< Compare Value Register 0                           */
  __IOM uint32_t COMP1_TGL;                     /**< Compare Value Register 1                           */
  __IOM uint32_t TOP_TGL;                       /**< Counter TOP Value Register                         */
  __IOM uint32_t TOPBUFF_TGL;                   /**< Buffered Counter TOP Value                         */
  __IOM uint32_t REP0_TGL;                      /**< Repeat Counter Register 0                          */
  __IOM uint32_t REP1_TGL;                      /**< Repeat Counter Register 1                          */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t LOCK_TGL;                      /**< Configuration Lock Register                        */
  __IM uint32_t  SYNCBUSY_TGL;                  /**< Synchronization Busy Register                      */
  uint32_t       RESERVED6[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t PRSMODE_TGL;                   /**< PRS Input mode select Register                     */
} LETIMER_TypeDef;
/** @} End of group EFR32ZG23_LETIMER */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_LETIMER
 * @{
 * @defgroup EFR32ZG23_LETIMER_BitFields LETIMER Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for LETIMER IPVERSION */
#define _LETIMER_IPVERSION_RESETVALUE                 0x00000001UL                                /**< Default value for LETIMER_IPVERSION         */
#define _LETIMER_IPVERSION_MASK                       0xFFFFFFFFUL                                /**< Mask for LETIMER_IPVERSION                  */
#define _LETIMER_IPVERSION_IPVERSION_SHIFT            0                                           /**< Shift value for LETIMER_IPVERSION           */
#define _LETIMER_IPVERSION_IPVERSION_MASK             0xFFFFFFFFUL                                /**< Bit mask for LETIMER_IPVERSION              */
#define _LETIMER_IPVERSION_IPVERSION_DEFAULT          0x00000001UL                                /**< Mode DEFAULT for LETIMER_IPVERSION          */
#define LETIMER_IPVERSION_IPVERSION_DEFAULT           (_LETIMER_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for LETIMER_IPVERSION  */

/* Bit fields for LETIMER EN */
#define _LETIMER_EN_RESETVALUE                        0x00000000UL                         /**< Default value for LETIMER_EN                */
#define _LETIMER_EN_MASK                              0x00000003UL                         /**< Mask for LETIMER_EN                         */
#define LETIMER_EN_EN                                 (0x1UL << 0)                         /**< module en                                   */
#define _LETIMER_EN_EN_SHIFT                          0                                    /**< Shift value for LETIMER_EN                  */
#define _LETIMER_EN_EN_MASK                           0x1UL                                /**< Bit mask for LETIMER_EN                     */
#define _LETIMER_EN_EN_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for LETIMER_EN                 */
#define LETIMER_EN_EN_DEFAULT                         (_LETIMER_EN_EN_DEFAULT << 0)        /**< Shifted mode DEFAULT for LETIMER_EN         */
#define LETIMER_EN_DISABLING                          (0x1UL << 1)                         /**< Disablement busy status                     */
#define _LETIMER_EN_DISABLING_SHIFT                   1                                    /**< Shift value for LETIMER_DISABLING           */
#define _LETIMER_EN_DISABLING_MASK                    0x2UL                                /**< Bit mask for LETIMER_DISABLING              */
#define _LETIMER_EN_DISABLING_DEFAULT                 0x00000000UL                         /**< Mode DEFAULT for LETIMER_EN                 */
#define LETIMER_EN_DISABLING_DEFAULT                  (_LETIMER_EN_DISABLING_DEFAULT << 1) /**< Shifted mode DEFAULT for LETIMER_EN         */

/* Bit fields for LETIMER SWRST */
#define _LETIMER_SWRST_RESETVALUE                     0x00000000UL                            /**< Default value for LETIMER_SWRST             */
#define _LETIMER_SWRST_MASK                           0x00000003UL                            /**< Mask for LETIMER_SWRST                      */
#define LETIMER_SWRST_SWRST                           (0x1UL << 0)                            /**< Software reset command                      */
#define _LETIMER_SWRST_SWRST_SHIFT                    0                                       /**< Shift value for LETIMER_SWRST               */
#define _LETIMER_SWRST_SWRST_MASK                     0x1UL                                   /**< Bit mask for LETIMER_SWRST                  */
#define _LETIMER_SWRST_SWRST_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for LETIMER_SWRST              */
#define LETIMER_SWRST_SWRST_DEFAULT                   (_LETIMER_SWRST_SWRST_DEFAULT << 0)     /**< Shifted mode DEFAULT for LETIMER_SWRST      */
#define LETIMER_SWRST_RESETTING                       (0x1UL << 1)                            /**< Software reset busy status                  */
#define _LETIMER_SWRST_RESETTING_SHIFT                1                                       /**< Shift value for LETIMER_RESETTING           */
#define _LETIMER_SWRST_RESETTING_MASK                 0x2UL                                   /**< Bit mask for LETIMER_RESETTING              */
#define _LETIMER_SWRST_RESETTING_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for LETIMER_SWRST              */
#define LETIMER_SWRST_RESETTING_DEFAULT               (_LETIMER_SWRST_RESETTING_DEFAULT << 1) /**< Shifted mode DEFAULT for LETIMER_SWRST      */

/* Bit fields for LETIMER CTRL */
#define _LETIMER_CTRL_RESETVALUE                      0x00000000UL                           /**< Default value for LETIMER_CTRL              */
#define _LETIMER_CTRL_MASK                            0x000F13FFUL                           /**< Mask for LETIMER_CTRL                       */
#define _LETIMER_CTRL_REPMODE_SHIFT                   0                                      /**< Shift value for LETIMER_REPMODE             */
#define _LETIMER_CTRL_REPMODE_MASK                    0x3UL                                  /**< Bit mask for LETIMER_REPMODE                */
#define _LETIMER_CTRL_REPMODE_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for LETIMER_CTRL               */
#define _LETIMER_CTRL_REPMODE_FREE                    0x00000000UL                           /**< Mode FREE for LETIMER_CTRL                  */
#define _LETIMER_CTRL_REPMODE_ONESHOT                 0x00000001UL                           /**< Mode ONESHOT for LETIMER_CTRL               */
#define _LETIMER_CTRL_REPMODE_BUFFERED                0x00000002UL                           /**< Mode BUFFERED for LETIMER_CTRL              */
#define _LETIMER_CTRL_REPMODE_DOUBLE                  0x00000003UL                           /**< Mode DOUBLE for LETIMER_CTRL                */
#define LETIMER_CTRL_REPMODE_DEFAULT                  (_LETIMER_CTRL_REPMODE_DEFAULT << 0)   /**< Shifted mode DEFAULT for LETIMER_CTRL       */
#define LETIMER_CTRL_REPMODE_FREE                     (_LETIMER_CTRL_REPMODE_FREE << 0)      /**< Shifted mode FREE for LETIMER_CTRL          */
#define LETIMER_CTRL_REPMODE_ONESHOT                  (_LETIMER_CTRL_REPMODE_ONESHOT << 0)   /**< Shifted mode ONESHOT for LETIMER_CTRL       */
#define LETIMER_CTRL_REPMODE_BUFFERED                 (_LETIMER_CTRL_REPMODE_BUFFERED << 0)  /**< Shifted mode BUFFERED for LETIMER_CTRL      */
#define LETIMER_CTRL_REPMODE_DOUBLE                   (_LETIMER_CTRL_REPMODE_DOUBLE << 0)    /**< Shifted mode DOUBLE for LETIMER_CTRL        */
#define _LETIMER_CTRL_UFOA0_SHIFT                     2                                      /**< Shift value for LETIMER_UFOA0               */
#define _LETIMER_CTRL_UFOA0_MASK                      0xCUL                                  /**< Bit mask for LETIMER_UFOA0                  */
#define _LETIMER_CTRL_UFOA0_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for LETIMER_CTRL               */
#define _LETIMER_CTRL_UFOA0_NONE                      0x00000000UL                           /**< Mode NONE for LETIMER_CTRL                  */
#define _LETIMER_CTRL_UFOA0_TOGGLE                    0x00000001UL                           /**< Mode TOGGLE for LETIMER_CTRL                */
#define _LETIMER_CTRL_UFOA0_PULSE                     0x00000002UL                           /**< Mode PULSE for LETIMER_CTRL                 */
#define _LETIMER_CTRL_UFOA0_PWM                       0x00000003UL                           /**< Mode PWM for LETIMER_CTRL                   */
#define LETIMER_CTRL_UFOA0_DEFAULT                    (_LETIMER_CTRL_UFOA0_DEFAULT << 2)     /**< Shifted mode DEFAULT for LETIMER_CTRL       */
#define LETIMER_CTRL_UFOA0_NONE                       (_LETIMER_CTRL_UFOA0_NONE << 2)        /**< Shifted mode NONE for LETIMER_CTRL          */
#define LETIMER_CTRL_UFOA0_TOGGLE                     (_LETIMER_CTRL_UFOA0_TOGGLE << 2)      /**< Shifted mode TOGGLE for LETIMER_CTRL        */
#define LETIMER_CTRL_UFOA0_PULSE                      (_LETIMER_CTRL_UFOA0_PULSE << 2)       /**< Shifted mode PULSE for LETIMER_CTRL         */
#define LETIMER_CTRL_UFOA0_PWM                        (_LETIMER_CTRL_UFOA0_PWM << 2)         /**< Shifted mode PWM for LETIMER_CTRL           */
#define _LETIMER_CTRL_UFOA1_SHIFT                     4                                      /**< Shift value for LETIMER_UFOA1               */
#define _LETIMER_CTRL_UFOA1_MASK                      0x30UL                                 /**< Bit mask for LETIMER_UFOA1                  */
#define _LETIMER_CTRL_UFOA1_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for LETIMER_CTRL               */
#define _LETIMER_CTRL_UFOA1_NONE                      0x00000000UL                           /**< Mode NONE for LETIMER_CTRL                  */
#define _LETIMER_CTRL_UFOA1_TOGGLE                    0x00000001UL                           /**< Mode TOGGLE for LETIMER_CTRL                */
#define _LETIMER_CTRL_UFOA1_PULSE                     0x00000002UL                           /**< Mode PULSE for LETIMER_CTRL                 */
#define _LETIMER_CTRL_UFOA1_PWM                       0x00000003UL                           /**< Mode PWM for LETIMER_CTRL                   */
#define LETIMER_CTRL_UFOA1_DEFAULT                    (_LETIMER_CTRL_UFOA1_DEFAULT << 4)     /**< Shifted mode DEFAULT for LETIMER_CTRL       */
#define LETIMER_CTRL_UFOA1_NONE                       (_LETIMER_CTRL_UFOA1_NONE << 4)        /**< Shifted mode NONE for LETIMER_CTRL          */
#define LETIMER_CTRL_UFOA1_TOGGLE                     (_LETIMER_CTRL_UFOA1_TOGGLE << 4)      /**< Shifted mode TOGGLE for LETIMER_CTRL        */
#define LETIMER_CTRL_UFOA1_PULSE                      (_LETIMER_CTRL_UFOA1_PULSE << 4)       /**< Shifted mode PULSE for LETIMER_CTRL         */
#define LETIMER_CTRL_UFOA1_PWM                        (_LETIMER_CTRL_UFOA1_PWM << 4)         /**< Shifted mode PWM for LETIMER_CTRL           */
#define LETIMER_CTRL_OPOL0                            (0x1UL << 6)                           /**< Output 0 Polarity                           */
#define _LETIMER_CTRL_OPOL0_SHIFT                     6                                      /**< Shift value for LETIMER_OPOL0               */
#define _LETIMER_CTRL_OPOL0_MASK                      0x40UL                                 /**< Bit mask for LETIMER_OPOL0                  */
#define _LETIMER_CTRL_OPOL0_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for LETIMER_CTRL               */
#define LETIMER_CTRL_OPOL0_DEFAULT                    (_LETIMER_CTRL_OPOL0_DEFAULT << 6)     /**< Shifted mode DEFAULT for LETIMER_CTRL       */
#define LETIMER_CTRL_OPOL1                            (0x1UL << 7)                           /**< Output 1 Polarity                           */
#define _LETIMER_CTRL_OPOL1_SHIFT                     7                                      /**< Shift value for LETIMER_OPOL1               */
#define _LETIMER_CTRL_OPOL1_MASK                      0x80UL                                 /**< Bit mask for LETIMER_OPOL1                  */
#define _LETIMER_CTRL_OPOL1_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for LETIMER_CTRL               */
#define LETIMER_CTRL_OPOL1_DEFAULT                    (_LETIMER_CTRL_OPOL1_DEFAULT << 7)     /**< Shifted mode DEFAULT for LETIMER_CTRL       */
#define LETIMER_CTRL_BUFTOP                           (0x1UL << 8)                           /**< Buffered Top                                */
#define _LETIMER_CTRL_BUFTOP_SHIFT                    8                                      /**< Shift value for LETIMER_BUFTOP              */
#define _LETIMER_CTRL_BUFTOP_MASK                     0x100UL                                /**< Bit mask for LETIMER_BUFTOP                 */
#define _LETIMER_CTRL_BUFTOP_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for LETIMER_CTRL               */
#define _LETIMER_CTRL_BUFTOP_DISABLE                  0x00000000UL                           /**< Mode DISABLE for LETIMER_CTRL               */
#define _LETIMER_CTRL_BUFTOP_ENABLE                   0x00000001UL                           /**< Mode ENABLE for LETIMER_CTRL                */
#define LETIMER_CTRL_BUFTOP_DEFAULT                   (_LETIMER_CTRL_BUFTOP_DEFAULT << 8)    /**< Shifted mode DEFAULT for LETIMER_CTRL       */
#define LETIMER_CTRL_BUFTOP_DISABLE                   (_LETIMER_CTRL_BUFTOP_DISABLE << 8)    /**< Shifted mode DISABLE for LETIMER_CTRL       */
#define LETIMER_CTRL_BUFTOP_ENABLE                    (_LETIMER_CTRL_BUFTOP_ENABLE << 8)     /**< Shifted mode ENABLE for LETIMER_CTRL        */
#define LETIMER_CTRL_CNTTOPEN                         (0x1UL << 9)                           /**< Compare Value 0 Is Top Value                */
#define _LETIMER_CTRL_CNTTOPEN_SHIFT                  9                                      /**< Shift value for LETIMER_CNTTOPEN            */
#define _LETIMER_CTRL_CNTTOPEN_MASK                   0x200UL                                /**< Bit mask for LETIMER_CNTTOPEN               */
#define _LETIMER_CTRL_CNTTOPEN_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for LETIMER_CTRL               */
#define _LETIMER_CTRL_CNTTOPEN_DISABLE                0x00000000UL                           /**< Mode DISABLE for LETIMER_CTRL               */
#define _LETIMER_CTRL_CNTTOPEN_ENABLE                 0x00000001UL                           /**< Mode ENABLE for LETIMER_CTRL                */
#define LETIMER_CTRL_CNTTOPEN_DEFAULT                 (_LETIMER_CTRL_CNTTOPEN_DEFAULT << 9)  /**< Shifted mode DEFAULT for LETIMER_CTRL       */
#define LETIMER_CTRL_CNTTOPEN_DISABLE                 (_LETIMER_CTRL_CNTTOPEN_DISABLE << 9)  /**< Shifted mode DISABLE for LETIMER_CTRL       */
#define LETIMER_CTRL_CNTTOPEN_ENABLE                  (_LETIMER_CTRL_CNTTOPEN_ENABLE << 9)   /**< Shifted mode ENABLE for LETIMER_CTRL        */
#define LETIMER_CTRL_DEBUGRUN                         (0x1UL << 12)                          /**< Debug Mode Run Enable                       */
#define _LETIMER_CTRL_DEBUGRUN_SHIFT                  12                                     /**< Shift value for LETIMER_DEBUGRUN            */
#define _LETIMER_CTRL_DEBUGRUN_MASK                   0x1000UL                               /**< Bit mask for LETIMER_DEBUGRUN               */
#define _LETIMER_CTRL_DEBUGRUN_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for LETIMER_CTRL               */
#define _LETIMER_CTRL_DEBUGRUN_DISABLE                0x00000000UL                           /**< Mode DISABLE for LETIMER_CTRL               */
#define _LETIMER_CTRL_DEBUGRUN_ENABLE                 0x00000001UL                           /**< Mode ENABLE for LETIMER_CTRL                */
#define LETIMER_CTRL_DEBUGRUN_DEFAULT                 (_LETIMER_CTRL_DEBUGRUN_DEFAULT << 12) /**< Shifted mode DEFAULT for LETIMER_CTRL       */
#define LETIMER_CTRL_DEBUGRUN_DISABLE                 (_LETIMER_CTRL_DEBUGRUN_DISABLE << 12) /**< Shifted mode DISABLE for LETIMER_CTRL       */
#define LETIMER_CTRL_DEBUGRUN_ENABLE                  (_LETIMER_CTRL_DEBUGRUN_ENABLE << 12)  /**< Shifted mode ENABLE for LETIMER_CTRL        */
#define _LETIMER_CTRL_CNTPRESC_SHIFT                  16                                     /**< Shift value for LETIMER_CNTPRESC            */
#define _LETIMER_CTRL_CNTPRESC_MASK                   0xF0000UL                              /**< Bit mask for LETIMER_CNTPRESC               */
#define _LETIMER_CTRL_CNTPRESC_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for LETIMER_CTRL               */
#define _LETIMER_CTRL_CNTPRESC_DIV1                   0x00000000UL                           /**< Mode DIV1 for LETIMER_CTRL                  */
#define _LETIMER_CTRL_CNTPRESC_DIV2                   0x00000001UL                           /**< Mode DIV2 for LETIMER_CTRL                  */
#define _LETIMER_CTRL_CNTPRESC_DIV4                   0x00000002UL                           /**< Mode DIV4 for LETIMER_CTRL                  */
#define _LETIMER_CTRL_CNTPRESC_DIV8                   0x00000003UL                           /**< Mode DIV8 for LETIMER_CTRL                  */
#define _LETIMER_CTRL_CNTPRESC_DIV16                  0x00000004UL                           /**< Mode DIV16 for LETIMER_CTRL                 */
#define _LETIMER_CTRL_CNTPRESC_DIV32                  0x00000005UL                           /**< Mode DIV32 for LETIMER_CTRL                 */
#define _LETIMER_CTRL_CNTPRESC_DIV64                  0x00000006UL                           /**< Mode DIV64 for LETIMER_CTRL                 */
#define _LETIMER_CTRL_CNTPRESC_DIV128                 0x00000007UL                           /**< Mode DIV128 for LETIMER_CTRL                */
#define _LETIMER_CTRL_CNTPRESC_DIV256                 0x00000008UL                           /**< Mode DIV256 for LETIMER_CTRL                */
#define LETIMER_CTRL_CNTPRESC_DEFAULT                 (_LETIMER_CTRL_CNTPRESC_DEFAULT << 16) /**< Shifted mode DEFAULT for LETIMER_CTRL       */
#define LETIMER_CTRL_CNTPRESC_DIV1                    (_LETIMER_CTRL_CNTPRESC_DIV1 << 16)    /**< Shifted mode DIV1 for LETIMER_CTRL          */
#define LETIMER_CTRL_CNTPRESC_DIV2                    (_LETIMER_CTRL_CNTPRESC_DIV2 << 16)    /**< Shifted mode DIV2 for LETIMER_CTRL          */
#define LETIMER_CTRL_CNTPRESC_DIV4                    (_LETIMER_CTRL_CNTPRESC_DIV4 << 16)    /**< Shifted mode DIV4 for LETIMER_CTRL          */
#define LETIMER_CTRL_CNTPRESC_DIV8                    (_LETIMER_CTRL_CNTPRESC_DIV8 << 16)    /**< Shifted mode DIV8 for LETIMER_CTRL          */
#define LETIMER_CTRL_CNTPRESC_DIV16                   (_LETIMER_CTRL_CNTPRESC_DIV16 << 16)   /**< Shifted mode DIV16 for LETIMER_CTRL         */
#define LETIMER_CTRL_CNTPRESC_DIV32                   (_LETIMER_CTRL_CNTPRESC_DIV32 << 16)   /**< Shifted mode DIV32 for LETIMER_CTRL         */
#define LETIMER_CTRL_CNTPRESC_DIV64                   (_LETIMER_CTRL_CNTPRESC_DIV64 << 16)   /**< Shifted mode DIV64 for LETIMER_CTRL         */
#define LETIMER_CTRL_CNTPRESC_DIV128                  (_LETIMER_CTRL_CNTPRESC_DIV128 << 16)  /**< Shifted mode DIV128 for LETIMER_CTRL        */
#define LETIMER_CTRL_CNTPRESC_DIV256                  (_LETIMER_CTRL_CNTPRESC_DIV256 << 16)  /**< Shifted mode DIV256 for LETIMER_CTRL        */

/* Bit fields for LETIMER CMD */
#define _LETIMER_CMD_RESETVALUE                       0x00000000UL                      /**< Default value for LETIMER_CMD               */
#define _LETIMER_CMD_MASK                             0x0000001FUL                      /**< Mask for LETIMER_CMD                        */
#define LETIMER_CMD_START                             (0x1UL << 0)                      /**< Start LETIMER                               */
#define _LETIMER_CMD_START_SHIFT                      0                                 /**< Shift value for LETIMER_START               */
#define _LETIMER_CMD_START_MASK                       0x1UL                             /**< Bit mask for LETIMER_START                  */
#define _LETIMER_CMD_START_DEFAULT                    0x00000000UL                      /**< Mode DEFAULT for LETIMER_CMD                */
#define LETIMER_CMD_START_DEFAULT                     (_LETIMER_CMD_START_DEFAULT << 0) /**< Shifted mode DEFAULT for LETIMER_CMD        */
#define LETIMER_CMD_STOP                              (0x1UL << 1)                      /**< Stop LETIMER                                */
#define _LETIMER_CMD_STOP_SHIFT                       1                                 /**< Shift value for LETIMER_STOP                */
#define _LETIMER_CMD_STOP_MASK                        0x2UL                             /**< Bit mask for LETIMER_STOP                   */
#define _LETIMER_CMD_STOP_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for LETIMER_CMD                */
#define LETIMER_CMD_STOP_DEFAULT                      (_LETIMER_CMD_STOP_DEFAULT << 1)  /**< Shifted mode DEFAULT for LETIMER_CMD        */
#define LETIMER_CMD_CLEAR                             (0x1UL << 2)                      /**< Clear LETIMER                               */
#define _LETIMER_CMD_CLEAR_SHIFT                      2                                 /**< Shift value for LETIMER_CLEAR               */
#define _LETIMER_CMD_CLEAR_MASK                       0x4UL                             /**< Bit mask for LETIMER_CLEAR                  */
#define _LETIMER_CMD_CLEAR_DEFAULT                    0x00000000UL                      /**< Mode DEFAULT for LETIMER_CMD                */
#define LETIMER_CMD_CLEAR_DEFAULT                     (_LETIMER_CMD_CLEAR_DEFAULT << 2) /**< Shifted mode DEFAULT for LETIMER_CMD        */
#define LETIMER_CMD_CTO0                              (0x1UL << 3)                      /**< Clear Toggle Output 0                       */
#define _LETIMER_CMD_CTO0_SHIFT                       3                                 /**< Shift value for LETIMER_CTO0                */
#define _LETIMER_CMD_CTO0_MASK                        0x8UL                             /**< Bit mask for LETIMER_CTO0                   */
#define _LETIMER_CMD_CTO0_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for LETIMER_CMD                */
#define LETIMER_CMD_CTO0_DEFAULT                      (_LETIMER_CMD_CTO0_DEFAULT << 3)  /**< Shifted mode DEFAULT for LETIMER_CMD        */
#define LETIMER_CMD_CTO1                              (0x1UL << 4)                      /**< Clear Toggle Output 1                       */
#define _LETIMER_CMD_CTO1_SHIFT                       4                                 /**< Shift value for LETIMER_CTO1                */
#define _LETIMER_CMD_CTO1_MASK                        0x10UL                            /**< Bit mask for LETIMER_CTO1                   */
#define _LETIMER_CMD_CTO1_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for LETIMER_CMD                */
#define LETIMER_CMD_CTO1_DEFAULT                      (_LETIMER_CMD_CTO1_DEFAULT << 4)  /**< Shifted mode DEFAULT for LETIMER_CMD        */

/* Bit fields for LETIMER STATUS */
#define _LETIMER_STATUS_RESETVALUE                    0x00000000UL                                      /**< Default value for LETIMER_STATUS            */
#define _LETIMER_STATUS_MASK                          0x00000003UL                                      /**< Mask for LETIMER_STATUS                     */
#define LETIMER_STATUS_RUNNING                        (0x1UL << 0)                                      /**< LETIMER Running                             */
#define _LETIMER_STATUS_RUNNING_SHIFT                 0                                                 /**< Shift value for LETIMER_RUNNING             */
#define _LETIMER_STATUS_RUNNING_MASK                  0x1UL                                             /**< Bit mask for LETIMER_RUNNING                */
#define _LETIMER_STATUS_RUNNING_DEFAULT               0x00000000UL                                      /**< Mode DEFAULT for LETIMER_STATUS             */
#define LETIMER_STATUS_RUNNING_DEFAULT                (_LETIMER_STATUS_RUNNING_DEFAULT << 0)            /**< Shifted mode DEFAULT for LETIMER_STATUS     */
#define LETIMER_STATUS_LETIMERLOCKSTATUS              (0x1UL << 1)                                      /**< LETIMER Lock Status                         */
#define _LETIMER_STATUS_LETIMERLOCKSTATUS_SHIFT       1                                                 /**< Shift value for LETIMER_LETIMERLOCKSTATUS   */
#define _LETIMER_STATUS_LETIMERLOCKSTATUS_MASK        0x2UL                                             /**< Bit mask for LETIMER_LETIMERLOCKSTATUS      */
#define _LETIMER_STATUS_LETIMERLOCKSTATUS_DEFAULT     0x00000000UL                                      /**< Mode DEFAULT for LETIMER_STATUS             */
#define _LETIMER_STATUS_LETIMERLOCKSTATUS_UNLOCKED    0x00000000UL                                      /**< Mode UNLOCKED for LETIMER_STATUS            */
#define _LETIMER_STATUS_LETIMERLOCKSTATUS_LOCKED      0x00000001UL                                      /**< Mode LOCKED for LETIMER_STATUS              */
#define LETIMER_STATUS_LETIMERLOCKSTATUS_DEFAULT      (_LETIMER_STATUS_LETIMERLOCKSTATUS_DEFAULT << 1)  /**< Shifted mode DEFAULT for LETIMER_STATUS     */
#define LETIMER_STATUS_LETIMERLOCKSTATUS_UNLOCKED     (_LETIMER_STATUS_LETIMERLOCKSTATUS_UNLOCKED << 1) /**< Shifted mode UNLOCKED for LETIMER_STATUS    */
#define LETIMER_STATUS_LETIMERLOCKSTATUS_LOCKED       (_LETIMER_STATUS_LETIMERLOCKSTATUS_LOCKED << 1)   /**< Shifted mode LOCKED for LETIMER_STATUS      */

/* Bit fields for LETIMER CNT */
#define _LETIMER_CNT_RESETVALUE                       0x00000000UL                      /**< Default value for LETIMER_CNT               */
#define _LETIMER_CNT_MASK                             0x00FFFFFFUL                      /**< Mask for LETIMER_CNT                        */
#define _LETIMER_CNT_CNT_SHIFT                        0                                 /**< Shift value for LETIMER_CNT                 */
#define _LETIMER_CNT_CNT_MASK                         0xFFFFFFUL                        /**< Bit mask for LETIMER_CNT                    */
#define _LETIMER_CNT_CNT_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for LETIMER_CNT                */
#define LETIMER_CNT_CNT_DEFAULT                       (_LETIMER_CNT_CNT_DEFAULT << 0)   /**< Shifted mode DEFAULT for LETIMER_CNT        */

/* Bit fields for LETIMER COMP0 */
#define _LETIMER_COMP0_RESETVALUE                     0x00000000UL                        /**< Default value for LETIMER_COMP0             */
#define _LETIMER_COMP0_MASK                           0x00FFFFFFUL                        /**< Mask for LETIMER_COMP0                      */
#define _LETIMER_COMP0_COMP0_SHIFT                    0                                   /**< Shift value for LETIMER_COMP0               */
#define _LETIMER_COMP0_COMP0_MASK                     0xFFFFFFUL                          /**< Bit mask for LETIMER_COMP0                  */
#define _LETIMER_COMP0_COMP0_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for LETIMER_COMP0              */
#define LETIMER_COMP0_COMP0_DEFAULT                   (_LETIMER_COMP0_COMP0_DEFAULT << 0) /**< Shifted mode DEFAULT for LETIMER_COMP0      */

/* Bit fields for LETIMER COMP1 */
#define _LETIMER_COMP1_RESETVALUE                     0x00000000UL                        /**< Default value for LETIMER_COMP1             */
#define _LETIMER_COMP1_MASK                           0x00FFFFFFUL                        /**< Mask for LETIMER_COMP1                      */
#define _LETIMER_COMP1_COMP1_SHIFT                    0                                   /**< Shift value for LETIMER_COMP1               */
#define _LETIMER_COMP1_COMP1_MASK                     0xFFFFFFUL                          /**< Bit mask for LETIMER_COMP1                  */
#define _LETIMER_COMP1_COMP1_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for LETIMER_COMP1              */
#define LETIMER_COMP1_COMP1_DEFAULT                   (_LETIMER_COMP1_COMP1_DEFAULT << 0) /**< Shifted mode DEFAULT for LETIMER_COMP1      */

/* Bit fields for LETIMER TOP */
#define _LETIMER_TOP_RESETVALUE                       0x00000000UL                      /**< Default value for LETIMER_TOP               */
#define _LETIMER_TOP_MASK                             0x00FFFFFFUL                      /**< Mask for LETIMER_TOP                        */
#define _LETIMER_TOP_TOP_SHIFT                        0                                 /**< Shift value for LETIMER_TOP                 */
#define _LETIMER_TOP_TOP_MASK                         0xFFFFFFUL                        /**< Bit mask for LETIMER_TOP                    */
#define _LETIMER_TOP_TOP_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for LETIMER_TOP                */
#define LETIMER_TOP_TOP_DEFAULT                       (_LETIMER_TOP_TOP_DEFAULT << 0)   /**< Shifted mode DEFAULT for LETIMER_TOP        */

/* Bit fields for LETIMER TOPBUFF */
#define _LETIMER_TOPBUFF_RESETVALUE                   0x00000000UL                            /**< Default value for LETIMER_TOPBUFF           */
#define _LETIMER_TOPBUFF_MASK                         0x00FFFFFFUL                            /**< Mask for LETIMER_TOPBUFF                    */
#define _LETIMER_TOPBUFF_TOPBUFF_SHIFT                0                                       /**< Shift value for LETIMER_TOPBUFF             */
#define _LETIMER_TOPBUFF_TOPBUFF_MASK                 0xFFFFFFUL                              /**< Bit mask for LETIMER_TOPBUFF                */
#define _LETIMER_TOPBUFF_TOPBUFF_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for LETIMER_TOPBUFF            */
#define LETIMER_TOPBUFF_TOPBUFF_DEFAULT               (_LETIMER_TOPBUFF_TOPBUFF_DEFAULT << 0) /**< Shifted mode DEFAULT for LETIMER_TOPBUFF    */

/* Bit fields for LETIMER REP0 */
#define _LETIMER_REP0_RESETVALUE                      0x00000000UL                      /**< Default value for LETIMER_REP0              */
#define _LETIMER_REP0_MASK                            0x000000FFUL                      /**< Mask for LETIMER_REP0                       */
#define _LETIMER_REP0_REP0_SHIFT                      0                                 /**< Shift value for LETIMER_REP0                */
#define _LETIMER_REP0_REP0_MASK                       0xFFUL                            /**< Bit mask for LETIMER_REP0                   */
#define _LETIMER_REP0_REP0_DEFAULT                    0x00000000UL                      /**< Mode DEFAULT for LETIMER_REP0               */
#define LETIMER_REP0_REP0_DEFAULT                     (_LETIMER_REP0_REP0_DEFAULT << 0) /**< Shifted mode DEFAULT for LETIMER_REP0       */

/* Bit fields for LETIMER REP1 */
#define _LETIMER_REP1_RESETVALUE                      0x00000000UL                      /**< Default value for LETIMER_REP1              */
#define _LETIMER_REP1_MASK                            0x000000FFUL                      /**< Mask for LETIMER_REP1                       */
#define _LETIMER_REP1_REP1_SHIFT                      0                                 /**< Shift value for LETIMER_REP1                */
#define _LETIMER_REP1_REP1_MASK                       0xFFUL                            /**< Bit mask for LETIMER_REP1                   */
#define _LETIMER_REP1_REP1_DEFAULT                    0x00000000UL                      /**< Mode DEFAULT for LETIMER_REP1               */
#define LETIMER_REP1_REP1_DEFAULT                     (_LETIMER_REP1_REP1_DEFAULT << 0) /**< Shifted mode DEFAULT for LETIMER_REP1       */

/* Bit fields for LETIMER IF */
#define _LETIMER_IF_RESETVALUE                        0x00000000UL                      /**< Default value for LETIMER_IF                */
#define _LETIMER_IF_MASK                              0x0000001FUL                      /**< Mask for LETIMER_IF                         */
#define LETIMER_IF_COMP0                              (0x1UL << 0)                      /**< Compare Match 0 Interrupt Flag              */
#define _LETIMER_IF_COMP0_SHIFT                       0                                 /**< Shift value for LETIMER_COMP0               */
#define _LETIMER_IF_COMP0_MASK                        0x1UL                             /**< Bit mask for LETIMER_COMP0                  */
#define _LETIMER_IF_COMP0_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for LETIMER_IF                 */
#define LETIMER_IF_COMP0_DEFAULT                      (_LETIMER_IF_COMP0_DEFAULT << 0)  /**< Shifted mode DEFAULT for LETIMER_IF         */
#define LETIMER_IF_COMP1                              (0x1UL << 1)                      /**< Compare Match 1 Interrupt Flag              */
#define _LETIMER_IF_COMP1_SHIFT                       1                                 /**< Shift value for LETIMER_COMP1               */
#define _LETIMER_IF_COMP1_MASK                        0x2UL                             /**< Bit mask for LETIMER_COMP1                  */
#define _LETIMER_IF_COMP1_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for LETIMER_IF                 */
#define LETIMER_IF_COMP1_DEFAULT                      (_LETIMER_IF_COMP1_DEFAULT << 1)  /**< Shifted mode DEFAULT for LETIMER_IF         */
#define LETIMER_IF_UF                                 (0x1UL << 2)                      /**< Underflow Interrupt Flag                    */
#define _LETIMER_IF_UF_SHIFT                          2                                 /**< Shift value for LETIMER_UF                  */
#define _LETIMER_IF_UF_MASK                           0x4UL                             /**< Bit mask for LETIMER_UF                     */
#define _LETIMER_IF_UF_DEFAULT                        0x00000000UL                      /**< Mode DEFAULT for LETIMER_IF                 */
#define LETIMER_IF_UF_DEFAULT                         (_LETIMER_IF_UF_DEFAULT << 2)     /**< Shifted mode DEFAULT for LETIMER_IF         */
#define LETIMER_IF_REP0                               (0x1UL << 3)                      /**< Repeat Counter 0 Interrupt Flag             */
#define _LETIMER_IF_REP0_SHIFT                        3                                 /**< Shift value for LETIMER_REP0                */
#define _LETIMER_IF_REP0_MASK                         0x8UL                             /**< Bit mask for LETIMER_REP0                   */
#define _LETIMER_IF_REP0_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for LETIMER_IF                 */
#define LETIMER_IF_REP0_DEFAULT                       (_LETIMER_IF_REP0_DEFAULT << 3)   /**< Shifted mode DEFAULT for LETIMER_IF         */
#define LETIMER_IF_REP1                               (0x1UL << 4)                      /**< Repeat Counter 1 Interrupt Flag             */
#define _LETIMER_IF_REP1_SHIFT                        4                                 /**< Shift value for LETIMER_REP1                */
#define _LETIMER_IF_REP1_MASK                         0x10UL                            /**< Bit mask for LETIMER_REP1                   */
#define _LETIMER_IF_REP1_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for LETIMER_IF                 */
#define LETIMER_IF_REP1_DEFAULT                       (_LETIMER_IF_REP1_DEFAULT << 4)   /**< Shifted mode DEFAULT for LETIMER_IF         */

/* Bit fields for LETIMER IEN */
#define _LETIMER_IEN_RESETVALUE                       0x00000000UL                      /**< Default value for LETIMER_IEN               */
#define _LETIMER_IEN_MASK                             0x0000001FUL                      /**< Mask for LETIMER_IEN                        */
#define LETIMER_IEN_COMP0                             (0x1UL << 0)                      /**< Compare Match 0 Interrupt Enable            */
#define _LETIMER_IEN_COMP0_SHIFT                      0                                 /**< Shift value for LETIMER_COMP0               */
#define _LETIMER_IEN_COMP0_MASK                       0x1UL                             /**< Bit mask for LETIMER_COMP0                  */
#define _LETIMER_IEN_COMP0_DEFAULT                    0x00000000UL                      /**< Mode DEFAULT for LETIMER_IEN                */
#define LETIMER_IEN_COMP0_DEFAULT                     (_LETIMER_IEN_COMP0_DEFAULT << 0) /**< Shifted mode DEFAULT for LETIMER_IEN        */
#define LETIMER_IEN_COMP1                             (0x1UL << 1)                      /**< Compare Match 1 Interrupt Enable            */
#define _LETIMER_IEN_COMP1_SHIFT                      1                                 /**< Shift value for LETIMER_COMP1               */
#define _LETIMER_IEN_COMP1_MASK                       0x2UL                             /**< Bit mask for LETIMER_COMP1                  */
#define _LETIMER_IEN_COMP1_DEFAULT                    0x00000000UL                      /**< Mode DEFAULT for LETIMER_IEN                */
#define LETIMER_IEN_COMP1_DEFAULT                     (_LETIMER_IEN_COMP1_DEFAULT << 1) /**< Shifted mode DEFAULT for LETIMER_IEN        */
#define LETIMER_IEN_UF                                (0x1UL << 2)                      /**< Underflow Interrupt Enable                  */
#define _LETIMER_IEN_UF_SHIFT                         2                                 /**< Shift value for LETIMER_UF                  */
#define _LETIMER_IEN_UF_MASK                          0x4UL                             /**< Bit mask for LETIMER_UF                     */
#define _LETIMER_IEN_UF_DEFAULT                       0x00000000UL                      /**< Mode DEFAULT for LETIMER_IEN                */
#define LETIMER_IEN_UF_DEFAULT                        (_LETIMER_IEN_UF_DEFAULT << 2)    /**< Shifted mode DEFAULT for LETIMER_IEN        */
#define LETIMER_IEN_REP0                              (0x1UL << 3)                      /**< Repeat Counter 0 Interrupt Enable           */
#define _LETIMER_IEN_REP0_SHIFT                       3                                 /**< Shift value for LETIMER_REP0                */
#define _LETIMER_IEN_REP0_MASK                        0x8UL                             /**< Bit mask for LETIMER_REP0                   */
#define _LETIMER_IEN_REP0_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for LETIMER_IEN                */
#define LETIMER_IEN_REP0_DEFAULT                      (_LETIMER_IEN_REP0_DEFAULT << 3)  /**< Shifted mode DEFAULT for LETIMER_IEN        */
#define LETIMER_IEN_REP1                              (0x1UL << 4)                      /**< Repeat Counter 1 Interrupt Enable           */
#define _LETIMER_IEN_REP1_SHIFT                       4                                 /**< Shift value for LETIMER_REP1                */
#define _LETIMER_IEN_REP1_MASK                        0x10UL                            /**< Bit mask for LETIMER_REP1                   */
#define _LETIMER_IEN_REP1_DEFAULT                     0x00000000UL                      /**< Mode DEFAULT for LETIMER_IEN                */
#define LETIMER_IEN_REP1_DEFAULT                      (_LETIMER_IEN_REP1_DEFAULT << 4)  /**< Shifted mode DEFAULT for LETIMER_IEN        */

/* Bit fields for LETIMER LOCK */
#define _LETIMER_LOCK_RESETVALUE                      0x00000000UL                                /**< Default value for LETIMER_LOCK              */
#define _LETIMER_LOCK_MASK                            0x0000FFFFUL                                /**< Mask for LETIMER_LOCK                       */
#define _LETIMER_LOCK_LETIMERLOCKKEY_SHIFT            0                                           /**< Shift value for LETIMER_LETIMERLOCKKEY      */
#define _LETIMER_LOCK_LETIMERLOCKKEY_MASK             0xFFFFUL                                    /**< Bit mask for LETIMER_LETIMERLOCKKEY         */
#define _LETIMER_LOCK_LETIMERLOCKKEY_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for LETIMER_LOCK               */
#define _LETIMER_LOCK_LETIMERLOCKKEY_UNLOCK           0x0000CCFCUL                                /**< Mode UNLOCK for LETIMER_LOCK                */
#define LETIMER_LOCK_LETIMERLOCKKEY_DEFAULT           (_LETIMER_LOCK_LETIMERLOCKKEY_DEFAULT << 0) /**< Shifted mode DEFAULT for LETIMER_LOCK       */
#define LETIMER_LOCK_LETIMERLOCKKEY_UNLOCK            (_LETIMER_LOCK_LETIMERLOCKKEY_UNLOCK << 0)  /**< Shifted mode UNLOCK for LETIMER_LOCK        */

/* Bit fields for LETIMER SYNCBUSY */
#define _LETIMER_SYNCBUSY_RESETVALUE                  0x00000000UL                           /**< Default value for LETIMER_SYNCBUSY          */
#define _LETIMER_SYNCBUSY_MASK                        0x000003FDUL                           /**< Mask for LETIMER_SYNCBUSY                   */
#define LETIMER_SYNCBUSY_CNT                          (0x1UL << 0)                           /**< Sync busy for CNT                           */
#define _LETIMER_SYNCBUSY_CNT_SHIFT                   0                                      /**< Shift value for LETIMER_CNT                 */
#define _LETIMER_SYNCBUSY_CNT_MASK                    0x1UL                                  /**< Bit mask for LETIMER_CNT                    */
#define _LETIMER_SYNCBUSY_CNT_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for LETIMER_SYNCBUSY           */
#define LETIMER_SYNCBUSY_CNT_DEFAULT                  (_LETIMER_SYNCBUSY_CNT_DEFAULT << 0)   /**< Shifted mode DEFAULT for LETIMER_SYNCBUSY   */
#define LETIMER_SYNCBUSY_TOP                          (0x1UL << 2)                           /**< Sync busy for TOP                           */
#define _LETIMER_SYNCBUSY_TOP_SHIFT                   2                                      /**< Shift value for LETIMER_TOP                 */
#define _LETIMER_SYNCBUSY_TOP_MASK                    0x4UL                                  /**< Bit mask for LETIMER_TOP                    */
#define _LETIMER_SYNCBUSY_TOP_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for LETIMER_SYNCBUSY           */
#define LETIMER_SYNCBUSY_TOP_DEFAULT                  (_LETIMER_SYNCBUSY_TOP_DEFAULT << 2)   /**< Shifted mode DEFAULT for LETIMER_SYNCBUSY   */
#define LETIMER_SYNCBUSY_REP0                         (0x1UL << 3)                           /**< Sync busy for REP0                          */
#define _LETIMER_SYNCBUSY_REP0_SHIFT                  3                                      /**< Shift value for LETIMER_REP0                */
#define _LETIMER_SYNCBUSY_REP0_MASK                   0x8UL                                  /**< Bit mask for LETIMER_REP0                   */
#define _LETIMER_SYNCBUSY_REP0_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for LETIMER_SYNCBUSY           */
#define LETIMER_SYNCBUSY_REP0_DEFAULT                 (_LETIMER_SYNCBUSY_REP0_DEFAULT << 3)  /**< Shifted mode DEFAULT for LETIMER_SYNCBUSY   */
#define LETIMER_SYNCBUSY_REP1                         (0x1UL << 4)                           /**< Sync busy for REP1                          */
#define _LETIMER_SYNCBUSY_REP1_SHIFT                  4                                      /**< Shift value for LETIMER_REP1                */
#define _LETIMER_SYNCBUSY_REP1_MASK                   0x10UL                                 /**< Bit mask for LETIMER_REP1                   */
#define _LETIMER_SYNCBUSY_REP1_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for LETIMER_SYNCBUSY           */
#define LETIMER_SYNCBUSY_REP1_DEFAULT                 (_LETIMER_SYNCBUSY_REP1_DEFAULT << 4)  /**< Shifted mode DEFAULT for LETIMER_SYNCBUSY   */
#define LETIMER_SYNCBUSY_START                        (0x1UL << 5)                           /**< Sync busy for START                         */
#define _LETIMER_SYNCBUSY_START_SHIFT                 5                                      /**< Shift value for LETIMER_START               */
#define _LETIMER_SYNCBUSY_START_MASK                  0x20UL                                 /**< Bit mask for LETIMER_START                  */
#define _LETIMER_SYNCBUSY_START_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for LETIMER_SYNCBUSY           */
#define LETIMER_SYNCBUSY_START_DEFAULT                (_LETIMER_SYNCBUSY_START_DEFAULT << 5) /**< Shifted mode DEFAULT for LETIMER_SYNCBUSY   */
#define LETIMER_SYNCBUSY_STOP                         (0x1UL << 6)                           /**< Sync busy for STOP                          */
#define _LETIMER_SYNCBUSY_STOP_SHIFT                  6                                      /**< Shift value for LETIMER_STOP                */
#define _LETIMER_SYNCBUSY_STOP_MASK                   0x40UL                                 /**< Bit mask for LETIMER_STOP                   */
#define _LETIMER_SYNCBUSY_STOP_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for LETIMER_SYNCBUSY           */
#define LETIMER_SYNCBUSY_STOP_DEFAULT                 (_LETIMER_SYNCBUSY_STOP_DEFAULT << 6)  /**< Shifted mode DEFAULT for LETIMER_SYNCBUSY   */
#define LETIMER_SYNCBUSY_CLEAR                        (0x1UL << 7)                           /**< Sync busy for CLEAR                         */
#define _LETIMER_SYNCBUSY_CLEAR_SHIFT                 7                                      /**< Shift value for LETIMER_CLEAR               */
#define _LETIMER_SYNCBUSY_CLEAR_MASK                  0x80UL                                 /**< Bit mask for LETIMER_CLEAR                  */
#define _LETIMER_SYNCBUSY_CLEAR_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for LETIMER_SYNCBUSY           */
#define LETIMER_SYNCBUSY_CLEAR_DEFAULT                (_LETIMER_SYNCBUSY_CLEAR_DEFAULT << 7) /**< Shifted mode DEFAULT for LETIMER_SYNCBUSY   */
#define LETIMER_SYNCBUSY_CTO0                         (0x1UL << 8)                           /**< Sync busy for CTO0                          */
#define _LETIMER_SYNCBUSY_CTO0_SHIFT                  8                                      /**< Shift value for LETIMER_CTO0                */
#define _LETIMER_SYNCBUSY_CTO0_MASK                   0x100UL                                /**< Bit mask for LETIMER_CTO0                   */
#define _LETIMER_SYNCBUSY_CTO0_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for LETIMER_SYNCBUSY           */
#define LETIMER_SYNCBUSY_CTO0_DEFAULT                 (_LETIMER_SYNCBUSY_CTO0_DEFAULT << 8)  /**< Shifted mode DEFAULT for LETIMER_SYNCBUSY   */
#define LETIMER_SYNCBUSY_CTO1                         (0x1UL << 9)                           /**< Sync busy for CTO1                          */
#define _LETIMER_SYNCBUSY_CTO1_SHIFT                  9                                      /**< Shift value for LETIMER_CTO1                */
#define _LETIMER_SYNCBUSY_CTO1_MASK                   0x200UL                                /**< Bit mask for LETIMER_CTO1                   */
#define _LETIMER_SYNCBUSY_CTO1_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for LETIMER_SYNCBUSY           */
#define LETIMER_SYNCBUSY_CTO1_DEFAULT                 (_LETIMER_SYNCBUSY_CTO1_DEFAULT << 9)  /**< Shifted mode DEFAULT for LETIMER_SYNCBUSY   */

/* Bit fields for LETIMER PRSMODE */
#define _LETIMER_PRSMODE_RESETVALUE                   0x00000000UL                                  /**< Default value for LETIMER_PRSMODE           */
#define _LETIMER_PRSMODE_MASK                         0x0CCC0000UL                                  /**< Mask for LETIMER_PRSMODE                    */
#define _LETIMER_PRSMODE_PRSSTARTMODE_SHIFT           18                                            /**< Shift value for LETIMER_PRSSTARTMODE        */
#define _LETIMER_PRSMODE_PRSSTARTMODE_MASK            0xC0000UL                                     /**< Bit mask for LETIMER_PRSSTARTMODE           */
#define _LETIMER_PRSMODE_PRSSTARTMODE_DEFAULT         0x00000000UL                                  /**< Mode DEFAULT for LETIMER_PRSMODE            */
#define _LETIMER_PRSMODE_PRSSTARTMODE_NONE            0x00000000UL                                  /**< Mode NONE for LETIMER_PRSMODE               */
#define _LETIMER_PRSMODE_PRSSTARTMODE_RISING          0x00000001UL                                  /**< Mode RISING for LETIMER_PRSMODE             */
#define _LETIMER_PRSMODE_PRSSTARTMODE_FALLING         0x00000002UL                                  /**< Mode FALLING for LETIMER_PRSMODE            */
#define _LETIMER_PRSMODE_PRSSTARTMODE_BOTH            0x00000003UL                                  /**< Mode BOTH for LETIMER_PRSMODE               */
#define LETIMER_PRSMODE_PRSSTARTMODE_DEFAULT          (_LETIMER_PRSMODE_PRSSTARTMODE_DEFAULT << 18) /**< Shifted mode DEFAULT for LETIMER_PRSMODE    */
#define LETIMER_PRSMODE_PRSSTARTMODE_NONE             (_LETIMER_PRSMODE_PRSSTARTMODE_NONE << 18)    /**< Shifted mode NONE for LETIMER_PRSMODE       */
#define LETIMER_PRSMODE_PRSSTARTMODE_RISING           (_LETIMER_PRSMODE_PRSSTARTMODE_RISING << 18)  /**< Shifted mode RISING for LETIMER_PRSMODE     */
#define LETIMER_PRSMODE_PRSSTARTMODE_FALLING          (_LETIMER_PRSMODE_PRSSTARTMODE_FALLING << 18) /**< Shifted mode FALLING for LETIMER_PRSMODE    */
#define LETIMER_PRSMODE_PRSSTARTMODE_BOTH             (_LETIMER_PRSMODE_PRSSTARTMODE_BOTH << 18)    /**< Shifted mode BOTH for LETIMER_PRSMODE       */
#define _LETIMER_PRSMODE_PRSSTOPMODE_SHIFT            22                                            /**< Shift value for LETIMER_PRSSTOPMODE         */
#define _LETIMER_PRSMODE_PRSSTOPMODE_MASK             0xC00000UL                                    /**< Bit mask for LETIMER_PRSSTOPMODE            */
#define _LETIMER_PRSMODE_PRSSTOPMODE_DEFAULT          0x00000000UL                                  /**< Mode DEFAULT for LETIMER_PRSMODE            */
#define _LETIMER_PRSMODE_PRSSTOPMODE_NONE             0x00000000UL                                  /**< Mode NONE for LETIMER_PRSMODE               */
#define _LETIMER_PRSMODE_PRSSTOPMODE_RISING           0x00000001UL                                  /**< Mode RISING for LETIMER_PRSMODE             */
#define _LETIMER_PRSMODE_PRSSTOPMODE_FALLING          0x00000002UL                                  /**< Mode FALLING for LETIMER_PRSMODE            */
#define _LETIMER_PRSMODE_PRSSTOPMODE_BOTH             0x00000003UL                                  /**< Mode BOTH for LETIMER_PRSMODE               */
#define LETIMER_PRSMODE_PRSSTOPMODE_DEFAULT           (_LETIMER_PRSMODE_PRSSTOPMODE_DEFAULT << 22)  /**< Shifted mode DEFAULT for LETIMER_PRSMODE    */
#define LETIMER_PRSMODE_PRSSTOPMODE_NONE              (_LETIMER_PRSMODE_PRSSTOPMODE_NONE << 22)     /**< Shifted mode NONE for LETIMER_PRSMODE       */
#define LETIMER_PRSMODE_PRSSTOPMODE_RISING            (_LETIMER_PRSMODE_PRSSTOPMODE_RISING << 22)   /**< Shifted mode RISING for LETIMER_PRSMODE     */
#define LETIMER_PRSMODE_PRSSTOPMODE_FALLING           (_LETIMER_PRSMODE_PRSSTOPMODE_FALLING << 22)  /**< Shifted mode FALLING for LETIMER_PRSMODE    */
#define LETIMER_PRSMODE_PRSSTOPMODE_BOTH              (_LETIMER_PRSMODE_PRSSTOPMODE_BOTH << 22)     /**< Shifted mode BOTH for LETIMER_PRSMODE       */
#define _LETIMER_PRSMODE_PRSCLEARMODE_SHIFT           26                                            /**< Shift value for LETIMER_PRSCLEARMODE        */
#define _LETIMER_PRSMODE_PRSCLEARMODE_MASK            0xC000000UL                                   /**< Bit mask for LETIMER_PRSCLEARMODE           */
#define _LETIMER_PRSMODE_PRSCLEARMODE_DEFAULT         0x00000000UL                                  /**< Mode DEFAULT for LETIMER_PRSMODE            */
#define _LETIMER_PRSMODE_PRSCLEARMODE_NONE            0x00000000UL                                  /**< Mode NONE for LETIMER_PRSMODE               */
#define _LETIMER_PRSMODE_PRSCLEARMODE_RISING          0x00000001UL                                  /**< Mode RISING for LETIMER_PRSMODE             */
#define _LETIMER_PRSMODE_PRSCLEARMODE_FALLING         0x00000002UL                                  /**< Mode FALLING for LETIMER_PRSMODE            */
#define _LETIMER_PRSMODE_PRSCLEARMODE_BOTH            0x00000003UL                                  /**< Mode BOTH for LETIMER_PRSMODE               */
#define LETIMER_PRSMODE_PRSCLEARMODE_DEFAULT          (_LETIMER_PRSMODE_PRSCLEARMODE_DEFAULT << 26) /**< Shifted mode DEFAULT for LETIMER_PRSMODE    */
#define LETIMER_PRSMODE_PRSCLEARMODE_NONE             (_LETIMER_PRSMODE_PRSCLEARMODE_NONE << 26)    /**< Shifted mode NONE for LETIMER_PRSMODE       */
#define LETIMER_PRSMODE_PRSCLEARMODE_RISING           (_LETIMER_PRSMODE_PRSCLEARMODE_RISING << 26)  /**< Shifted mode RISING for LETIMER_PRSMODE     */
#define LETIMER_PRSMODE_PRSCLEARMODE_FALLING          (_LETIMER_PRSMODE_PRSCLEARMODE_FALLING << 26) /**< Shifted mode FALLING for LETIMER_PRSMODE    */
#define LETIMER_PRSMODE_PRSCLEARMODE_BOTH             (_LETIMER_PRSMODE_PRSCLEARMODE_BOTH << 26)    /**< Shifted mode BOTH for LETIMER_PRSMODE       */

/** @} End of group EFR32ZG23_LETIMER_BitFields */
/** @} End of group EFR32ZG23_LETIMER */
/** @} End of group Parts */

#endif /* EFR32ZG23_LETIMER_H */
#ifdef __cplusplus
}
#endif

