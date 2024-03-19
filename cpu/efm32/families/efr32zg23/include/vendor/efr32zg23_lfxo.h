/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 LFXO register and bit field definitions
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

#ifndef EFR32ZG23_LFXO_H
#define EFR32ZG23_LFXO_H
#define LFXO_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_LFXO LFXO
 * @{
 * @brief EFR32ZG23 LFXO Register Declaration.
 *****************************************************************************/

/** LFXO Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< LFXO IP version                                    */
  __IOM uint32_t CTRL;                          /**< LFXO Control Register                              */
  __IOM uint32_t CFG;                           /**< LFXO Configuration Register                        */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS;                        /**< LFXO Status Register                               */
  __IOM uint32_t CAL;                           /**< LFXO Calibration Register                          */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY;                      /**< LFXO Sync Busy Register                            */
  __IOM uint32_t LOCK;                          /**< Configuration Lock Register                        */
  uint32_t       RESERVED1[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< LFXO IP version                                    */
  __IOM uint32_t CTRL_SET;                      /**< LFXO Control Register                              */
  __IOM uint32_t CFG_SET;                       /**< LFXO Configuration Register                        */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS_SET;                    /**< LFXO Status Register                               */
  __IOM uint32_t CAL_SET;                       /**< LFXO Calibration Register                          */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY_SET;                  /**< LFXO Sync Busy Register                            */
  __IOM uint32_t LOCK_SET;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED3[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< LFXO IP version                                    */
  __IOM uint32_t CTRL_CLR;                      /**< LFXO Control Register                              */
  __IOM uint32_t CFG_CLR;                       /**< LFXO Configuration Register                        */
  uint32_t       RESERVED4[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS_CLR;                    /**< LFXO Status Register                               */
  __IOM uint32_t CAL_CLR;                       /**< LFXO Calibration Register                          */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY_CLR;                  /**< LFXO Sync Busy Register                            */
  __IOM uint32_t LOCK_CLR;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED5[1014U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< LFXO IP version                                    */
  __IOM uint32_t CTRL_TGL;                      /**< LFXO Control Register                              */
  __IOM uint32_t CFG_TGL;                       /**< LFXO Configuration Register                        */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS_TGL;                    /**< LFXO Status Register                               */
  __IOM uint32_t CAL_TGL;                       /**< LFXO Calibration Register                          */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  __IM uint32_t  SYNCBUSY_TGL;                  /**< LFXO Sync Busy Register                            */
  __IOM uint32_t LOCK_TGL;                      /**< Configuration Lock Register                        */
} LFXO_TypeDef;
/** @} End of group EFR32ZG23_LFXO */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_LFXO
 * @{
 * @defgroup EFR32ZG23_LFXO_BitFields LFXO Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for LFXO IPVERSION */
#define _LFXO_IPVERSION_RESETVALUE           0x00000000UL                               /**< Default value for LFXO_IPVERSION            */
#define _LFXO_IPVERSION_MASK                 0xFFFFFFFFUL                               /**< Mask for LFXO_IPVERSION                     */
#define _LFXO_IPVERSION_IPVERSION_SHIFT      0                                          /**< Shift value for LFXO_IPVERSION              */
#define _LFXO_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                               /**< Bit mask for LFXO_IPVERSION                 */
#define _LFXO_IPVERSION_IPVERSION_DEFAULT    0x00000000UL                               /**< Mode DEFAULT for LFXO_IPVERSION             */
#define LFXO_IPVERSION_IPVERSION_DEFAULT     (_LFXO_IPVERSION_IPVERSION_DEFAULT << 0)   /**< Shifted mode DEFAULT for LFXO_IPVERSION     */

/* Bit fields for LFXO CTRL */
#define _LFXO_CTRL_RESETVALUE                0x00000002UL                               /**< Default value for LFXO_CTRL                 */
#define _LFXO_CTRL_MASK                      0x00000033UL                               /**< Mask for LFXO_CTRL                          */
#define LFXO_CTRL_FORCEEN                    (0x1UL << 0)                               /**< LFXO Force Enable                           */
#define _LFXO_CTRL_FORCEEN_SHIFT             0                                          /**< Shift value for LFXO_FORCEEN                */
#define _LFXO_CTRL_FORCEEN_MASK              0x1UL                                      /**< Bit mask for LFXO_FORCEEN                   */
#define _LFXO_CTRL_FORCEEN_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LFXO_CTRL                  */
#define LFXO_CTRL_FORCEEN_DEFAULT            (_LFXO_CTRL_FORCEEN_DEFAULT << 0)          /**< Shifted mode DEFAULT for LFXO_CTRL          */
#define LFXO_CTRL_DISONDEMAND                (0x1UL << 1)                               /**< LFXO Disable On-demand requests             */
#define _LFXO_CTRL_DISONDEMAND_SHIFT         1                                          /**< Shift value for LFXO_DISONDEMAND            */
#define _LFXO_CTRL_DISONDEMAND_MASK          0x2UL                                      /**< Bit mask for LFXO_DISONDEMAND               */
#define _LFXO_CTRL_DISONDEMAND_DEFAULT       0x00000001UL                               /**< Mode DEFAULT for LFXO_CTRL                  */
#define LFXO_CTRL_DISONDEMAND_DEFAULT        (_LFXO_CTRL_DISONDEMAND_DEFAULT << 1)      /**< Shifted mode DEFAULT for LFXO_CTRL          */
#define LFXO_CTRL_FAILDETEN                  (0x1UL << 4)                               /**< LFXO Failure Detection Enable               */
#define _LFXO_CTRL_FAILDETEN_SHIFT           4                                          /**< Shift value for LFXO_FAILDETEN              */
#define _LFXO_CTRL_FAILDETEN_MASK            0x10UL                                     /**< Bit mask for LFXO_FAILDETEN                 */
#define _LFXO_CTRL_FAILDETEN_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for LFXO_CTRL                  */
#define LFXO_CTRL_FAILDETEN_DEFAULT          (_LFXO_CTRL_FAILDETEN_DEFAULT << 4)        /**< Shifted mode DEFAULT for LFXO_CTRL          */
#define LFXO_CTRL_FAILDETEM4WUEN             (0x1UL << 5)                               /**< LFXO Failure Detection EM4WU Enable         */
#define _LFXO_CTRL_FAILDETEM4WUEN_SHIFT      5                                          /**< Shift value for LFXO_FAILDETEM4WUEN         */
#define _LFXO_CTRL_FAILDETEM4WUEN_MASK       0x20UL                                     /**< Bit mask for LFXO_FAILDETEM4WUEN            */
#define _LFXO_CTRL_FAILDETEM4WUEN_DEFAULT    0x00000000UL                               /**< Mode DEFAULT for LFXO_CTRL                  */
#define LFXO_CTRL_FAILDETEM4WUEN_DEFAULT     (_LFXO_CTRL_FAILDETEM4WUEN_DEFAULT << 5)   /**< Shifted mode DEFAULT for LFXO_CTRL          */

/* Bit fields for LFXO CFG */
#define _LFXO_CFG_RESETVALUE                 0x00000701UL                               /**< Default value for LFXO_CFG                  */
#define _LFXO_CFG_MASK                       0x00000733UL                               /**< Mask for LFXO_CFG                           */
#define LFXO_CFG_AGC                         (0x1UL << 0)                               /**< LFXO AGC Enable                             */
#define _LFXO_CFG_AGC_SHIFT                  0                                          /**< Shift value for LFXO_AGC                    */
#define _LFXO_CFG_AGC_MASK                   0x1UL                                      /**< Bit mask for LFXO_AGC                       */
#define _LFXO_CFG_AGC_DEFAULT                0x00000001UL                               /**< Mode DEFAULT for LFXO_CFG                   */
#define LFXO_CFG_AGC_DEFAULT                 (_LFXO_CFG_AGC_DEFAULT << 0)               /**< Shifted mode DEFAULT for LFXO_CFG           */
#define LFXO_CFG_HIGHAMPL                    (0x1UL << 1)                               /**< LFXO High Amplitude Enable                  */
#define _LFXO_CFG_HIGHAMPL_SHIFT             1                                          /**< Shift value for LFXO_HIGHAMPL               */
#define _LFXO_CFG_HIGHAMPL_MASK              0x2UL                                      /**< Bit mask for LFXO_HIGHAMPL                  */
#define _LFXO_CFG_HIGHAMPL_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LFXO_CFG                   */
#define LFXO_CFG_HIGHAMPL_DEFAULT            (_LFXO_CFG_HIGHAMPL_DEFAULT << 1)          /**< Shifted mode DEFAULT for LFXO_CFG           */
#define _LFXO_CFG_MODE_SHIFT                 4                                          /**< Shift value for LFXO_MODE                   */
#define _LFXO_CFG_MODE_MASK                  0x30UL                                     /**< Bit mask for LFXO_MODE                      */
#define _LFXO_CFG_MODE_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for LFXO_CFG                   */
#define _LFXO_CFG_MODE_XTAL                  0x00000000UL                               /**< Mode XTAL for LFXO_CFG                      */
#define _LFXO_CFG_MODE_BUFEXTCLK             0x00000001UL                               /**< Mode BUFEXTCLK for LFXO_CFG                 */
#define _LFXO_CFG_MODE_DIGEXTCLK             0x00000002UL                               /**< Mode DIGEXTCLK for LFXO_CFG                 */
#define LFXO_CFG_MODE_DEFAULT                (_LFXO_CFG_MODE_DEFAULT << 4)              /**< Shifted mode DEFAULT for LFXO_CFG           */
#define LFXO_CFG_MODE_XTAL                   (_LFXO_CFG_MODE_XTAL << 4)                 /**< Shifted mode XTAL for LFXO_CFG              */
#define LFXO_CFG_MODE_BUFEXTCLK              (_LFXO_CFG_MODE_BUFEXTCLK << 4)            /**< Shifted mode BUFEXTCLK for LFXO_CFG         */
#define LFXO_CFG_MODE_DIGEXTCLK              (_LFXO_CFG_MODE_DIGEXTCLK << 4)            /**< Shifted mode DIGEXTCLK for LFXO_CFG         */
#define _LFXO_CFG_TIMEOUT_SHIFT              8                                          /**< Shift value for LFXO_TIMEOUT                */
#define _LFXO_CFG_TIMEOUT_MASK               0x700UL                                    /**< Bit mask for LFXO_TIMEOUT                   */
#define _LFXO_CFG_TIMEOUT_DEFAULT            0x00000007UL                               /**< Mode DEFAULT for LFXO_CFG                   */
#define _LFXO_CFG_TIMEOUT_CYCLES2            0x00000000UL                               /**< Mode CYCLES2 for LFXO_CFG                   */
#define _LFXO_CFG_TIMEOUT_CYCLES256          0x00000001UL                               /**< Mode CYCLES256 for LFXO_CFG                 */
#define _LFXO_CFG_TIMEOUT_CYCLES1K           0x00000002UL                               /**< Mode CYCLES1K for LFXO_CFG                  */
#define _LFXO_CFG_TIMEOUT_CYCLES2K           0x00000003UL                               /**< Mode CYCLES2K for LFXO_CFG                  */
#define _LFXO_CFG_TIMEOUT_CYCLES4K           0x00000004UL                               /**< Mode CYCLES4K for LFXO_CFG                  */
#define _LFXO_CFG_TIMEOUT_CYCLES8K           0x00000005UL                               /**< Mode CYCLES8K for LFXO_CFG                  */
#define _LFXO_CFG_TIMEOUT_CYCLES16K          0x00000006UL                               /**< Mode CYCLES16K for LFXO_CFG                 */
#define _LFXO_CFG_TIMEOUT_CYCLES32K          0x00000007UL                               /**< Mode CYCLES32K for LFXO_CFG                 */
#define LFXO_CFG_TIMEOUT_DEFAULT             (_LFXO_CFG_TIMEOUT_DEFAULT << 8)           /**< Shifted mode DEFAULT for LFXO_CFG           */
#define LFXO_CFG_TIMEOUT_CYCLES2             (_LFXO_CFG_TIMEOUT_CYCLES2 << 8)           /**< Shifted mode CYCLES2 for LFXO_CFG           */
#define LFXO_CFG_TIMEOUT_CYCLES256           (_LFXO_CFG_TIMEOUT_CYCLES256 << 8)         /**< Shifted mode CYCLES256 for LFXO_CFG         */
#define LFXO_CFG_TIMEOUT_CYCLES1K            (_LFXO_CFG_TIMEOUT_CYCLES1K << 8)          /**< Shifted mode CYCLES1K for LFXO_CFG          */
#define LFXO_CFG_TIMEOUT_CYCLES2K            (_LFXO_CFG_TIMEOUT_CYCLES2K << 8)          /**< Shifted mode CYCLES2K for LFXO_CFG          */
#define LFXO_CFG_TIMEOUT_CYCLES4K            (_LFXO_CFG_TIMEOUT_CYCLES4K << 8)          /**< Shifted mode CYCLES4K for LFXO_CFG          */
#define LFXO_CFG_TIMEOUT_CYCLES8K            (_LFXO_CFG_TIMEOUT_CYCLES8K << 8)          /**< Shifted mode CYCLES8K for LFXO_CFG          */
#define LFXO_CFG_TIMEOUT_CYCLES16K           (_LFXO_CFG_TIMEOUT_CYCLES16K << 8)         /**< Shifted mode CYCLES16K for LFXO_CFG         */
#define LFXO_CFG_TIMEOUT_CYCLES32K           (_LFXO_CFG_TIMEOUT_CYCLES32K << 8)         /**< Shifted mode CYCLES32K for LFXO_CFG         */

/* Bit fields for LFXO STATUS */
#define _LFXO_STATUS_RESETVALUE              0x00000000UL                               /**< Default value for LFXO_STATUS               */
#define _LFXO_STATUS_MASK                    0x80010001UL                               /**< Mask for LFXO_STATUS                        */
#define LFXO_STATUS_RDY                      (0x1UL << 0)                               /**< LFXO Ready Status                           */
#define _LFXO_STATUS_RDY_SHIFT               0                                          /**< Shift value for LFXO_RDY                    */
#define _LFXO_STATUS_RDY_MASK                0x1UL                                      /**< Bit mask for LFXO_RDY                       */
#define _LFXO_STATUS_RDY_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LFXO_STATUS                */
#define LFXO_STATUS_RDY_DEFAULT              (_LFXO_STATUS_RDY_DEFAULT << 0)            /**< Shifted mode DEFAULT for LFXO_STATUS        */
#define LFXO_STATUS_ENS                      (0x1UL << 16)                              /**< LFXO Enable Status                          */
#define _LFXO_STATUS_ENS_SHIFT               16                                         /**< Shift value for LFXO_ENS                    */
#define _LFXO_STATUS_ENS_MASK                0x10000UL                                  /**< Bit mask for LFXO_ENS                       */
#define _LFXO_STATUS_ENS_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LFXO_STATUS                */
#define LFXO_STATUS_ENS_DEFAULT              (_LFXO_STATUS_ENS_DEFAULT << 16)           /**< Shifted mode DEFAULT for LFXO_STATUS        */
#define LFXO_STATUS_LOCK                     (0x1UL << 31)                              /**< LFXO Locked Status                          */
#define _LFXO_STATUS_LOCK_SHIFT              31                                         /**< Shift value for LFXO_LOCK                   */
#define _LFXO_STATUS_LOCK_MASK               0x80000000UL                               /**< Bit mask for LFXO_LOCK                      */
#define _LFXO_STATUS_LOCK_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LFXO_STATUS                */
#define _LFXO_STATUS_LOCK_UNLOCKED           0x00000000UL                               /**< Mode UNLOCKED for LFXO_STATUS               */
#define _LFXO_STATUS_LOCK_LOCKED             0x00000001UL                               /**< Mode LOCKED for LFXO_STATUS                 */
#define LFXO_STATUS_LOCK_DEFAULT             (_LFXO_STATUS_LOCK_DEFAULT << 31)          /**< Shifted mode DEFAULT for LFXO_STATUS        */
#define LFXO_STATUS_LOCK_UNLOCKED            (_LFXO_STATUS_LOCK_UNLOCKED << 31)         /**< Shifted mode UNLOCKED for LFXO_STATUS       */
#define LFXO_STATUS_LOCK_LOCKED              (_LFXO_STATUS_LOCK_LOCKED << 31)           /**< Shifted mode LOCKED for LFXO_STATUS         */

/* Bit fields for LFXO CAL */
#define _LFXO_CAL_RESETVALUE                 0x00000200UL                               /**< Default value for LFXO_CAL                  */
#define _LFXO_CAL_MASK                       0x0000037FUL                               /**< Mask for LFXO_CAL                           */
#define _LFXO_CAL_CAPTUNE_SHIFT              0                                          /**< Shift value for LFXO_CAPTUNE                */
#define _LFXO_CAL_CAPTUNE_MASK               0x7FUL                                     /**< Bit mask for LFXO_CAPTUNE                   */
#define _LFXO_CAL_CAPTUNE_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LFXO_CAL                   */
#define LFXO_CAL_CAPTUNE_DEFAULT             (_LFXO_CAL_CAPTUNE_DEFAULT << 0)           /**< Shifted mode DEFAULT for LFXO_CAL           */
#define _LFXO_CAL_GAIN_SHIFT                 8                                          /**< Shift value for LFXO_GAIN                   */
#define _LFXO_CAL_GAIN_MASK                  0x300UL                                    /**< Bit mask for LFXO_GAIN                      */
#define _LFXO_CAL_GAIN_DEFAULT               0x00000002UL                               /**< Mode DEFAULT for LFXO_CAL                   */
#define LFXO_CAL_GAIN_DEFAULT                (_LFXO_CAL_GAIN_DEFAULT << 8)              /**< Shifted mode DEFAULT for LFXO_CAL           */

/* Bit fields for LFXO IF */
#define _LFXO_IF_RESETVALUE                  0x00000000UL                               /**< Default value for LFXO_IF                   */
#define _LFXO_IF_MASK                        0x0000000FUL                               /**< Mask for LFXO_IF                            */
#define LFXO_IF_RDY                          (0x1UL << 0)                               /**< LFXO Ready Interrupt Flag                   */
#define _LFXO_IF_RDY_SHIFT                   0                                          /**< Shift value for LFXO_RDY                    */
#define _LFXO_IF_RDY_MASK                    0x1UL                                      /**< Bit mask for LFXO_RDY                       */
#define _LFXO_IF_RDY_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for LFXO_IF                    */
#define LFXO_IF_RDY_DEFAULT                  (_LFXO_IF_RDY_DEFAULT << 0)                /**< Shifted mode DEFAULT for LFXO_IF            */
#define LFXO_IF_POSEDGE                      (0x1UL << 1)                               /**< Rising Edge Interrupt Flag                  */
#define _LFXO_IF_POSEDGE_SHIFT               1                                          /**< Shift value for LFXO_POSEDGE                */
#define _LFXO_IF_POSEDGE_MASK                0x2UL                                      /**< Bit mask for LFXO_POSEDGE                   */
#define _LFXO_IF_POSEDGE_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LFXO_IF                    */
#define LFXO_IF_POSEDGE_DEFAULT              (_LFXO_IF_POSEDGE_DEFAULT << 1)            /**< Shifted mode DEFAULT for LFXO_IF            */
#define LFXO_IF_NEGEDGE                      (0x1UL << 2)                               /**< Falling Edge Interrupt Flag                 */
#define _LFXO_IF_NEGEDGE_SHIFT               2                                          /**< Shift value for LFXO_NEGEDGE                */
#define _LFXO_IF_NEGEDGE_MASK                0x4UL                                      /**< Bit mask for LFXO_NEGEDGE                   */
#define _LFXO_IF_NEGEDGE_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LFXO_IF                    */
#define LFXO_IF_NEGEDGE_DEFAULT              (_LFXO_IF_NEGEDGE_DEFAULT << 2)            /**< Shifted mode DEFAULT for LFXO_IF            */
#define LFXO_IF_FAIL                         (0x1UL << 3)                               /**< LFXO Failure Interrupt Flag                 */
#define _LFXO_IF_FAIL_SHIFT                  3                                          /**< Shift value for LFXO_FAIL                   */
#define _LFXO_IF_FAIL_MASK                   0x8UL                                      /**< Bit mask for LFXO_FAIL                      */
#define _LFXO_IF_FAIL_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for LFXO_IF                    */
#define LFXO_IF_FAIL_DEFAULT                 (_LFXO_IF_FAIL_DEFAULT << 3)               /**< Shifted mode DEFAULT for LFXO_IF            */

/* Bit fields for LFXO IEN */
#define _LFXO_IEN_RESETVALUE                 0x00000000UL                               /**< Default value for LFXO_IEN                  */
#define _LFXO_IEN_MASK                       0x0000000FUL                               /**< Mask for LFXO_IEN                           */
#define LFXO_IEN_RDY                         (0x1UL << 0)                               /**< LFXO Ready Interrupt Enable                 */
#define _LFXO_IEN_RDY_SHIFT                  0                                          /**< Shift value for LFXO_RDY                    */
#define _LFXO_IEN_RDY_MASK                   0x1UL                                      /**< Bit mask for LFXO_RDY                       */
#define _LFXO_IEN_RDY_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for LFXO_IEN                   */
#define LFXO_IEN_RDY_DEFAULT                 (_LFXO_IEN_RDY_DEFAULT << 0)               /**< Shifted mode DEFAULT for LFXO_IEN           */
#define LFXO_IEN_POSEDGE                     (0x1UL << 1)                               /**< Rising Edge Interrupt Enable                */
#define _LFXO_IEN_POSEDGE_SHIFT              1                                          /**< Shift value for LFXO_POSEDGE                */
#define _LFXO_IEN_POSEDGE_MASK               0x2UL                                      /**< Bit mask for LFXO_POSEDGE                   */
#define _LFXO_IEN_POSEDGE_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LFXO_IEN                   */
#define LFXO_IEN_POSEDGE_DEFAULT             (_LFXO_IEN_POSEDGE_DEFAULT << 1)           /**< Shifted mode DEFAULT for LFXO_IEN           */
#define LFXO_IEN_NEGEDGE                     (0x1UL << 2)                               /**< Falling Edge Interrupt Enable               */
#define _LFXO_IEN_NEGEDGE_SHIFT              2                                          /**< Shift value for LFXO_NEGEDGE                */
#define _LFXO_IEN_NEGEDGE_MASK               0x4UL                                      /**< Bit mask for LFXO_NEGEDGE                   */
#define _LFXO_IEN_NEGEDGE_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LFXO_IEN                   */
#define LFXO_IEN_NEGEDGE_DEFAULT             (_LFXO_IEN_NEGEDGE_DEFAULT << 2)           /**< Shifted mode DEFAULT for LFXO_IEN           */
#define LFXO_IEN_FAIL                        (0x1UL << 3)                               /**< LFXO Failure Interrupt Enable               */
#define _LFXO_IEN_FAIL_SHIFT                 3                                          /**< Shift value for LFXO_FAIL                   */
#define _LFXO_IEN_FAIL_MASK                  0x8UL                                      /**< Bit mask for LFXO_FAIL                      */
#define _LFXO_IEN_FAIL_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for LFXO_IEN                   */
#define LFXO_IEN_FAIL_DEFAULT                (_LFXO_IEN_FAIL_DEFAULT << 3)              /**< Shifted mode DEFAULT for LFXO_IEN           */

/* Bit fields for LFXO SYNCBUSY */
#define _LFXO_SYNCBUSY_RESETVALUE            0x00000000UL                               /**< Default value for LFXO_SYNCBUSY             */
#define _LFXO_SYNCBUSY_MASK                  0x00000001UL                               /**< Mask for LFXO_SYNCBUSY                      */
#define LFXO_SYNCBUSY_CAL                    (0x1UL << 0)                               /**< LFXO Synchronization status                 */
#define _LFXO_SYNCBUSY_CAL_SHIFT             0                                          /**< Shift value for LFXO_CAL                    */
#define _LFXO_SYNCBUSY_CAL_MASK              0x1UL                                      /**< Bit mask for LFXO_CAL                       */
#define _LFXO_SYNCBUSY_CAL_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LFXO_SYNCBUSY              */
#define LFXO_SYNCBUSY_CAL_DEFAULT            (_LFXO_SYNCBUSY_CAL_DEFAULT << 0)          /**< Shifted mode DEFAULT for LFXO_SYNCBUSY      */

/* Bit fields for LFXO LOCK */
#define _LFXO_LOCK_RESETVALUE                0x00001A20UL                               /**< Default value for LFXO_LOCK                 */
#define _LFXO_LOCK_MASK                      0x0000FFFFUL                               /**< Mask for LFXO_LOCK                          */
#define _LFXO_LOCK_LOCKKEY_SHIFT             0                                          /**< Shift value for LFXO_LOCKKEY                */
#define _LFXO_LOCK_LOCKKEY_MASK              0xFFFFUL                                   /**< Bit mask for LFXO_LOCKKEY                   */
#define _LFXO_LOCK_LOCKKEY_DEFAULT           0x00001A20UL                               /**< Mode DEFAULT for LFXO_LOCK                  */
#define _LFXO_LOCK_LOCKKEY_UNLOCK            0x00001A20UL                               /**< Mode UNLOCK for LFXO_LOCK                   */
#define LFXO_LOCK_LOCKKEY_DEFAULT            (_LFXO_LOCK_LOCKKEY_DEFAULT << 0)          /**< Shifted mode DEFAULT for LFXO_LOCK          */
#define LFXO_LOCK_LOCKKEY_UNLOCK             (_LFXO_LOCK_LOCKKEY_UNLOCK << 0)           /**< Shifted mode UNLOCK for LFXO_LOCK           */

/** @} End of group EFR32ZG23_LFXO_BitFields */
/** @} End of group EFR32ZG23_LFXO */
/** @} End of group Parts */

#endif /* EFR32ZG23_LFXO_H */
#ifdef __cplusplus
}
#endif

