/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 HFRCO register and bit field definitions
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

#ifndef EFR32ZG23_HFRCO_H
#define EFR32ZG23_HFRCO_H
#define HFRCO_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_HFRCO HFRCO
 * @{
 * @brief EFR32ZG23 HFRCO Register Declaration.
 *****************************************************************************/

/** HFRCO Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP Version ID                                      */
  __IOM uint32_t CTRL;                          /**< Ctrl Register                                      */
  __IOM uint32_t CAL;                           /**< Calibration Register                               */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< Lock Register                                      */
  uint32_t       RESERVED1[1016U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP Version ID                                      */
  __IOM uint32_t CTRL_SET;                      /**< Ctrl Register                                      */
  __IOM uint32_t CAL_SET;                       /**< Calibration Register                               */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< Lock Register                                      */
  uint32_t       RESERVED3[1016U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP Version ID                                      */
  __IOM uint32_t CTRL_CLR;                      /**< Ctrl Register                                      */
  __IOM uint32_t CAL_CLR;                       /**< Calibration Register                               */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED4[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< Lock Register                                      */
  uint32_t       RESERVED5[1016U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP Version ID                                      */
  __IOM uint32_t CTRL_TGL;                      /**< Ctrl Register                                      */
  __IOM uint32_t CAL_TGL;                       /**< Calibration Register                               */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< Lock Register                                      */
} HFRCO_TypeDef;
/** @} End of group EFR32ZG23_HFRCO */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_HFRCO
 * @{
 * @defgroup EFR32ZG23_HFRCO_BitFields HFRCO Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for HFRCO IPVERSION */
#define _HFRCO_IPVERSION_RESETVALUE           0x00000002UL                              /**< Default value for HFRCO_IPVERSION           */
#define _HFRCO_IPVERSION_MASK                 0xFFFFFFFFUL                              /**< Mask for HFRCO_IPVERSION                    */
#define _HFRCO_IPVERSION_IPVERSION_SHIFT      0                                         /**< Shift value for HFRCO_IPVERSION             */
#define _HFRCO_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                              /**< Bit mask for HFRCO_IPVERSION                */
#define _HFRCO_IPVERSION_IPVERSION_DEFAULT    0x00000002UL                              /**< Mode DEFAULT for HFRCO_IPVERSION            */
#define HFRCO_IPVERSION_IPVERSION_DEFAULT     (_HFRCO_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for HFRCO_IPVERSION    */

/* Bit fields for HFRCO CTRL */
#define _HFRCO_CTRL_RESETVALUE                0x00000000UL                              /**< Default value for HFRCO_CTRL                */
#define _HFRCO_CTRL_MASK                      0x00000007UL                              /**< Mask for HFRCO_CTRL                         */
#define HFRCO_CTRL_FORCEEN                    (0x1UL << 0)                              /**< Force Enable                                */
#define _HFRCO_CTRL_FORCEEN_SHIFT             0                                         /**< Shift value for HFRCO_FORCEEN               */
#define _HFRCO_CTRL_FORCEEN_MASK              0x1UL                                     /**< Bit mask for HFRCO_FORCEEN                  */
#define _HFRCO_CTRL_FORCEEN_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for HFRCO_CTRL                 */
#define HFRCO_CTRL_FORCEEN_DEFAULT            (_HFRCO_CTRL_FORCEEN_DEFAULT << 0)        /**< Shifted mode DEFAULT for HFRCO_CTRL         */
#define HFRCO_CTRL_DISONDEMAND                (0x1UL << 1)                              /**< Disable On-demand                           */
#define _HFRCO_CTRL_DISONDEMAND_SHIFT         1                                         /**< Shift value for HFRCO_DISONDEMAND           */
#define _HFRCO_CTRL_DISONDEMAND_MASK          0x2UL                                     /**< Bit mask for HFRCO_DISONDEMAND              */
#define _HFRCO_CTRL_DISONDEMAND_DEFAULT       0x00000000UL                              /**< Mode DEFAULT for HFRCO_CTRL                 */
#define HFRCO_CTRL_DISONDEMAND_DEFAULT        (_HFRCO_CTRL_DISONDEMAND_DEFAULT << 1)    /**< Shifted mode DEFAULT for HFRCO_CTRL         */
#define HFRCO_CTRL_EM23ONDEMAND               (0x1UL << 2)                              /**< EM23 On-demand                              */
#define _HFRCO_CTRL_EM23ONDEMAND_SHIFT        2                                         /**< Shift value for HFRCO_EM23ONDEMAND          */
#define _HFRCO_CTRL_EM23ONDEMAND_MASK         0x4UL                                     /**< Bit mask for HFRCO_EM23ONDEMAND             */
#define _HFRCO_CTRL_EM23ONDEMAND_DEFAULT      0x00000000UL                              /**< Mode DEFAULT for HFRCO_CTRL                 */
#define HFRCO_CTRL_EM23ONDEMAND_DEFAULT       (_HFRCO_CTRL_EM23ONDEMAND_DEFAULT << 2)   /**< Shifted mode DEFAULT for HFRCO_CTRL         */

/* Bit fields for HFRCO CAL */
#define _HFRCO_CAL_RESETVALUE                 0xA8689F7FUL                              /**< Default value for HFRCO_CAL                 */
#define _HFRCO_CAL_MASK                       0xFFFFBF7FUL                              /**< Mask for HFRCO_CAL                          */
#define _HFRCO_CAL_TUNING_SHIFT               0                                         /**< Shift value for HFRCO_TUNING                */
#define _HFRCO_CAL_TUNING_MASK                0x7FUL                                    /**< Bit mask for HFRCO_TUNING                   */
#define _HFRCO_CAL_TUNING_DEFAULT             0x0000007FUL                              /**< Mode DEFAULT for HFRCO_CAL                  */
#define HFRCO_CAL_TUNING_DEFAULT              (_HFRCO_CAL_TUNING_DEFAULT << 0)          /**< Shifted mode DEFAULT for HFRCO_CAL          */
#define _HFRCO_CAL_FINETUNING_SHIFT           8                                         /**< Shift value for HFRCO_FINETUNING            */
#define _HFRCO_CAL_FINETUNING_MASK            0x3F00UL                                  /**< Bit mask for HFRCO_FINETUNING               */
#define _HFRCO_CAL_FINETUNING_DEFAULT         0x0000001FUL                              /**< Mode DEFAULT for HFRCO_CAL                  */
#define HFRCO_CAL_FINETUNING_DEFAULT          (_HFRCO_CAL_FINETUNING_DEFAULT << 8)      /**< Shifted mode DEFAULT for HFRCO_CAL          */
#define HFRCO_CAL_LDOHP                       (0x1UL << 15)                             /**< LDO High Power Mode                         */
#define _HFRCO_CAL_LDOHP_SHIFT                15                                        /**< Shift value for HFRCO_LDOHP                 */
#define _HFRCO_CAL_LDOHP_MASK                 0x8000UL                                  /**< Bit mask for HFRCO_LDOHP                    */
#define _HFRCO_CAL_LDOHP_DEFAULT              0x00000001UL                              /**< Mode DEFAULT for HFRCO_CAL                  */
#define HFRCO_CAL_LDOHP_DEFAULT               (_HFRCO_CAL_LDOHP_DEFAULT << 15)          /**< Shifted mode DEFAULT for HFRCO_CAL          */
#define _HFRCO_CAL_FREQRANGE_SHIFT            16                                        /**< Shift value for HFRCO_FREQRANGE             */
#define _HFRCO_CAL_FREQRANGE_MASK             0x1F0000UL                                /**< Bit mask for HFRCO_FREQRANGE                */
#define _HFRCO_CAL_FREQRANGE_DEFAULT          0x00000008UL                              /**< Mode DEFAULT for HFRCO_CAL                  */
#define HFRCO_CAL_FREQRANGE_DEFAULT           (_HFRCO_CAL_FREQRANGE_DEFAULT << 16)      /**< Shifted mode DEFAULT for HFRCO_CAL          */
#define _HFRCO_CAL_CMPBIAS_SHIFT              21                                        /**< Shift value for HFRCO_CMPBIAS               */
#define _HFRCO_CAL_CMPBIAS_MASK               0xE00000UL                                /**< Bit mask for HFRCO_CMPBIAS                  */
#define _HFRCO_CAL_CMPBIAS_DEFAULT            0x00000003UL                              /**< Mode DEFAULT for HFRCO_CAL                  */
#define HFRCO_CAL_CMPBIAS_DEFAULT             (_HFRCO_CAL_CMPBIAS_DEFAULT << 21)        /**< Shifted mode DEFAULT for HFRCO_CAL          */
#define _HFRCO_CAL_CLKDIV_SHIFT               24                                        /**< Shift value for HFRCO_CLKDIV                */
#define _HFRCO_CAL_CLKDIV_MASK                0x3000000UL                               /**< Bit mask for HFRCO_CLKDIV                   */
#define _HFRCO_CAL_CLKDIV_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for HFRCO_CAL                  */
#define _HFRCO_CAL_CLKDIV_DIV1                0x00000000UL                              /**< Mode DIV1 for HFRCO_CAL                     */
#define _HFRCO_CAL_CLKDIV_DIV2                0x00000001UL                              /**< Mode DIV2 for HFRCO_CAL                     */
#define _HFRCO_CAL_CLKDIV_DIV4                0x00000002UL                              /**< Mode DIV4 for HFRCO_CAL                     */
#define HFRCO_CAL_CLKDIV_DEFAULT              (_HFRCO_CAL_CLKDIV_DEFAULT << 24)         /**< Shifted mode DEFAULT for HFRCO_CAL          */
#define HFRCO_CAL_CLKDIV_DIV1                 (_HFRCO_CAL_CLKDIV_DIV1 << 24)            /**< Shifted mode DIV1 for HFRCO_CAL             */
#define HFRCO_CAL_CLKDIV_DIV2                 (_HFRCO_CAL_CLKDIV_DIV2 << 24)            /**< Shifted mode DIV2 for HFRCO_CAL             */
#define HFRCO_CAL_CLKDIV_DIV4                 (_HFRCO_CAL_CLKDIV_DIV4 << 24)            /**< Shifted mode DIV4 for HFRCO_CAL             */
#define _HFRCO_CAL_CMPSEL_SHIFT               26                                        /**< Shift value for HFRCO_CMPSEL                */
#define _HFRCO_CAL_CMPSEL_MASK                0xC000000UL                               /**< Bit mask for HFRCO_CMPSEL                   */
#define _HFRCO_CAL_CMPSEL_DEFAULT             0x00000002UL                              /**< Mode DEFAULT for HFRCO_CAL                  */
#define HFRCO_CAL_CMPSEL_DEFAULT              (_HFRCO_CAL_CMPSEL_DEFAULT << 26)         /**< Shifted mode DEFAULT for HFRCO_CAL          */
#define _HFRCO_CAL_IREFTC_SHIFT               28                                        /**< Shift value for HFRCO_IREFTC                */
#define _HFRCO_CAL_IREFTC_MASK                0xF0000000UL                              /**< Bit mask for HFRCO_IREFTC                   */
#define _HFRCO_CAL_IREFTC_DEFAULT             0x0000000AUL                              /**< Mode DEFAULT for HFRCO_CAL                  */
#define HFRCO_CAL_IREFTC_DEFAULT              (_HFRCO_CAL_IREFTC_DEFAULT << 28)         /**< Shifted mode DEFAULT for HFRCO_CAL          */

/* Bit fields for HFRCO STATUS */
#define _HFRCO_STATUS_RESETVALUE              0x00000000UL                              /**< Default value for HFRCO_STATUS              */
#define _HFRCO_STATUS_MASK                    0x80010007UL                              /**< Mask for HFRCO_STATUS                       */
#define HFRCO_STATUS_RDY                      (0x1UL << 0)                              /**< Ready                                       */
#define _HFRCO_STATUS_RDY_SHIFT               0                                         /**< Shift value for HFRCO_RDY                   */
#define _HFRCO_STATUS_RDY_MASK                0x1UL                                     /**< Bit mask for HFRCO_RDY                      */
#define _HFRCO_STATUS_RDY_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for HFRCO_STATUS               */
#define HFRCO_STATUS_RDY_DEFAULT              (_HFRCO_STATUS_RDY_DEFAULT << 0)          /**< Shifted mode DEFAULT for HFRCO_STATUS       */
#define HFRCO_STATUS_FREQBSY                  (0x1UL << 1)                              /**< Frequency Updating Busy                     */
#define _HFRCO_STATUS_FREQBSY_SHIFT           1                                         /**< Shift value for HFRCO_FREQBSY               */
#define _HFRCO_STATUS_FREQBSY_MASK            0x2UL                                     /**< Bit mask for HFRCO_FREQBSY                  */
#define _HFRCO_STATUS_FREQBSY_DEFAULT         0x00000000UL                              /**< Mode DEFAULT for HFRCO_STATUS               */
#define HFRCO_STATUS_FREQBSY_DEFAULT          (_HFRCO_STATUS_FREQBSY_DEFAULT << 1)      /**< Shifted mode DEFAULT for HFRCO_STATUS       */
#define HFRCO_STATUS_SYNCBUSY                 (0x1UL << 2)                              /**< Synchronization Busy                        */
#define _HFRCO_STATUS_SYNCBUSY_SHIFT          2                                         /**< Shift value for HFRCO_SYNCBUSY              */
#define _HFRCO_STATUS_SYNCBUSY_MASK           0x4UL                                     /**< Bit mask for HFRCO_SYNCBUSY                 */
#define _HFRCO_STATUS_SYNCBUSY_DEFAULT        0x00000000UL                              /**< Mode DEFAULT for HFRCO_STATUS               */
#define HFRCO_STATUS_SYNCBUSY_DEFAULT         (_HFRCO_STATUS_SYNCBUSY_DEFAULT << 2)     /**< Shifted mode DEFAULT for HFRCO_STATUS       */
#define HFRCO_STATUS_ENS                      (0x1UL << 16)                             /**< Enable Status                               */
#define _HFRCO_STATUS_ENS_SHIFT               16                                        /**< Shift value for HFRCO_ENS                   */
#define _HFRCO_STATUS_ENS_MASK                0x10000UL                                 /**< Bit mask for HFRCO_ENS                      */
#define _HFRCO_STATUS_ENS_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for HFRCO_STATUS               */
#define HFRCO_STATUS_ENS_DEFAULT              (_HFRCO_STATUS_ENS_DEFAULT << 16)         /**< Shifted mode DEFAULT for HFRCO_STATUS       */
#define HFRCO_STATUS_LOCK                     (0x1UL << 31)                             /**< Lock Status                                 */
#define _HFRCO_STATUS_LOCK_SHIFT              31                                        /**< Shift value for HFRCO_LOCK                  */
#define _HFRCO_STATUS_LOCK_MASK               0x80000000UL                              /**< Bit mask for HFRCO_LOCK                     */
#define _HFRCO_STATUS_LOCK_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for HFRCO_STATUS               */
#define _HFRCO_STATUS_LOCK_UNLOCKED           0x00000000UL                              /**< Mode UNLOCKED for HFRCO_STATUS              */
#define _HFRCO_STATUS_LOCK_LOCKED             0x00000001UL                              /**< Mode LOCKED for HFRCO_STATUS                */
#define HFRCO_STATUS_LOCK_DEFAULT             (_HFRCO_STATUS_LOCK_DEFAULT << 31)        /**< Shifted mode DEFAULT for HFRCO_STATUS       */
#define HFRCO_STATUS_LOCK_UNLOCKED            (_HFRCO_STATUS_LOCK_UNLOCKED << 31)       /**< Shifted mode UNLOCKED for HFRCO_STATUS      */
#define HFRCO_STATUS_LOCK_LOCKED              (_HFRCO_STATUS_LOCK_LOCKED << 31)         /**< Shifted mode LOCKED for HFRCO_STATUS        */

/* Bit fields for HFRCO IF */
#define _HFRCO_IF_RESETVALUE                  0x00000000UL                              /**< Default value for HFRCO_IF                  */
#define _HFRCO_IF_MASK                        0x00000001UL                              /**< Mask for HFRCO_IF                           */
#define HFRCO_IF_RDY                          (0x1UL << 0)                              /**< Ready Interrupt Flag                        */
#define _HFRCO_IF_RDY_SHIFT                   0                                         /**< Shift value for HFRCO_RDY                   */
#define _HFRCO_IF_RDY_MASK                    0x1UL                                     /**< Bit mask for HFRCO_RDY                      */
#define _HFRCO_IF_RDY_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for HFRCO_IF                   */
#define HFRCO_IF_RDY_DEFAULT                  (_HFRCO_IF_RDY_DEFAULT << 0)              /**< Shifted mode DEFAULT for HFRCO_IF           */

/* Bit fields for HFRCO IEN */
#define _HFRCO_IEN_RESETVALUE                 0x00000000UL                              /**< Default value for HFRCO_IEN                 */
#define _HFRCO_IEN_MASK                       0x00000001UL                              /**< Mask for HFRCO_IEN                          */
#define HFRCO_IEN_RDY                         (0x1UL << 0)                              /**< RDY Interrupt Enable                        */
#define _HFRCO_IEN_RDY_SHIFT                  0                                         /**< Shift value for HFRCO_RDY                   */
#define _HFRCO_IEN_RDY_MASK                   0x1UL                                     /**< Bit mask for HFRCO_RDY                      */
#define _HFRCO_IEN_RDY_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for HFRCO_IEN                  */
#define HFRCO_IEN_RDY_DEFAULT                 (_HFRCO_IEN_RDY_DEFAULT << 0)             /**< Shifted mode DEFAULT for HFRCO_IEN          */

/* Bit fields for HFRCO LOCK */
#define _HFRCO_LOCK_RESETVALUE                0x00008195UL                              /**< Default value for HFRCO_LOCK                */
#define _HFRCO_LOCK_MASK                      0x0000FFFFUL                              /**< Mask for HFRCO_LOCK                         */
#define _HFRCO_LOCK_LOCKKEY_SHIFT             0                                         /**< Shift value for HFRCO_LOCKKEY               */
#define _HFRCO_LOCK_LOCKKEY_MASK              0xFFFFUL                                  /**< Bit mask for HFRCO_LOCKKEY                  */
#define _HFRCO_LOCK_LOCKKEY_DEFAULT           0x00008195UL                              /**< Mode DEFAULT for HFRCO_LOCK                 */
#define _HFRCO_LOCK_LOCKKEY_UNLOCK            0x00008195UL                              /**< Mode UNLOCK for HFRCO_LOCK                  */
#define HFRCO_LOCK_LOCKKEY_DEFAULT            (_HFRCO_LOCK_LOCKKEY_DEFAULT << 0)        /**< Shifted mode DEFAULT for HFRCO_LOCK         */
#define HFRCO_LOCK_LOCKKEY_UNLOCK             (_HFRCO_LOCK_LOCKKEY_UNLOCK << 0)         /**< Shifted mode UNLOCK for HFRCO_LOCK          */

/** @} End of group EFR32ZG23_HFRCO_BitFields */
/** @} End of group EFR32ZG23_HFRCO */
/** @} End of group Parts */

#endif /* EFR32ZG23_HFRCO_H */
#ifdef __cplusplus
}
#endif

