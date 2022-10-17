/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 IADC register and bit field definitions
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

#ifndef EFR32ZG23_IADC_H
#define EFR32ZG23_IADC_H
#define IADC_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_IADC IADC
 * @{
 * @brief EFR32ZG23 IADC Register Declaration.
 *****************************************************************************/

/** IADC CFG Register Group Declaration. */
typedef struct {
  __IOM uint32_t CFG;                                /**< Configuration                                      */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t SCALE;                              /**< Scaling                                            */
  __IOM uint32_t SCHED;                              /**< Scheduling                                         */
} IADC_CFG_TypeDef;

/** IADC SCANTABLE Register Group Declaration. */
typedef struct {
  __IOM uint32_t SCAN;                               /**< SCAN Entry                                         */
} IADC_SCANTABLE_TypeDef;

/** IADC Register Declaration. */
typedef struct {
  __IM uint32_t          IPVERSION;             /**< IPVERSION                                          */
  __IOM uint32_t         EN;                    /**< Enable                                             */
  __IOM uint32_t         CTRL;                  /**< Control                                            */
  __IOM uint32_t         CMD;                   /**< Command                                            */
  __IOM uint32_t         TIMER;                 /**< Timer                                              */
  __IM uint32_t          STATUS;                /**< Status                                             */
  __IOM uint32_t         MASKREQ;               /**< Mask Request                                       */
  __IM uint32_t          STMASK;                /**< Scan Table Mask                                    */
  __IOM uint32_t         CMPTHR;                /**< Digital Window Comparator Threshold                */
  __IOM uint32_t         IF;                    /**< Interrupt Flags                                    */
  __IOM uint32_t         IEN;                   /**< Interrupt Enable                                   */
  __IOM uint32_t         TRIGGER;               /**< Trigger                                            */
  uint32_t               RESERVED0[1U];         /**< Reserved for future use                            */
  uint32_t               RESERVED1[5U];         /**< Reserved for future use                            */
  IADC_CFG_TypeDef       CFG[2U];               /**< CFG                                                */
  uint32_t               RESERVED2[2U];         /**< Reserved for future use                            */
  __IOM uint32_t         SINGLEFIFOCFG;         /**< Single FIFO Configuration                          */
  __IM uint32_t          SINGLEFIFODATA;        /**< Single FIFO DATA                                   */
  __IM uint32_t          SINGLEFIFOSTAT;        /**< Single FIFO Status                                 */
  __IM uint32_t          SINGLEDATA;            /**< Single Data                                        */
  __IOM uint32_t         SCANFIFOCFG;           /**< Scan FIFO Configuration                            */
  __IM uint32_t          SCANFIFODATA;          /**< Scan FIFO Read Data                                */
  __IM uint32_t          SCANFIFOSTAT;          /**< Scan FIFO Status                                   */
  __IM uint32_t          SCANDATA;              /**< Scan Data                                          */
  uint32_t               RESERVED3[1U];         /**< Reserved for future use                            */
  uint32_t               RESERVED4[1U];         /**< Reserved for future use                            */
  __IOM uint32_t         SINGLE;                /**< Single Queue Port Selection                        */
  uint32_t               RESERVED5[1U];         /**< Reserved for future use                            */
  IADC_SCANTABLE_TypeDef SCANTABLE[16U];        /**< SCANTABLE                                          */
  uint32_t               RESERVED6[4U];         /**< Reserved for future use                            */
  uint32_t               RESERVED7[1U];         /**< Reserved for future use                            */
  uint32_t               RESERVED8[963U];       /**< Reserved for future use                            */
  __IM uint32_t          IPVERSION_SET;         /**< IPVERSION                                          */
  __IOM uint32_t         EN_SET;                /**< Enable                                             */
  __IOM uint32_t         CTRL_SET;              /**< Control                                            */
  __IOM uint32_t         CMD_SET;               /**< Command                                            */
  __IOM uint32_t         TIMER_SET;             /**< Timer                                              */
  __IM uint32_t          STATUS_SET;            /**< Status                                             */
  __IOM uint32_t         MASKREQ_SET;           /**< Mask Request                                       */
  __IM uint32_t          STMASK_SET;            /**< Scan Table Mask                                    */
  __IOM uint32_t         CMPTHR_SET;            /**< Digital Window Comparator Threshold                */
  __IOM uint32_t         IF_SET;                /**< Interrupt Flags                                    */
  __IOM uint32_t         IEN_SET;               /**< Interrupt Enable                                   */
  __IOM uint32_t         TRIGGER_SET;           /**< Trigger                                            */
  uint32_t               RESERVED9[1U];         /**< Reserved for future use                            */
  uint32_t               RESERVED10[5U];        /**< Reserved for future use                            */
  IADC_CFG_TypeDef       CFG_SET[2U];           /**< CFG                                                */
  uint32_t               RESERVED11[2U];        /**< Reserved for future use                            */
  __IOM uint32_t         SINGLEFIFOCFG_SET;     /**< Single FIFO Configuration                          */
  __IM uint32_t          SINGLEFIFODATA_SET;    /**< Single FIFO DATA                                   */
  __IM uint32_t          SINGLEFIFOSTAT_SET;    /**< Single FIFO Status                                 */
  __IM uint32_t          SINGLEDATA_SET;        /**< Single Data                                        */
  __IOM uint32_t         SCANFIFOCFG_SET;       /**< Scan FIFO Configuration                            */
  __IM uint32_t          SCANFIFODATA_SET;      /**< Scan FIFO Read Data                                */
  __IM uint32_t          SCANFIFOSTAT_SET;      /**< Scan FIFO Status                                   */
  __IM uint32_t          SCANDATA_SET;          /**< Scan Data                                          */
  uint32_t               RESERVED12[1U];        /**< Reserved for future use                            */
  uint32_t               RESERVED13[1U];        /**< Reserved for future use                            */
  __IOM uint32_t         SINGLE_SET;            /**< Single Queue Port Selection                        */
  uint32_t               RESERVED14[1U];        /**< Reserved for future use                            */
  IADC_SCANTABLE_TypeDef SCANTABLE_SET[16U];    /**< SCANTABLE                                          */
  uint32_t               RESERVED15[4U];        /**< Reserved for future use                            */
  uint32_t               RESERVED16[1U];        /**< Reserved for future use                            */
  uint32_t               RESERVED17[963U];      /**< Reserved for future use                            */
  __IM uint32_t          IPVERSION_CLR;         /**< IPVERSION                                          */
  __IOM uint32_t         EN_CLR;                /**< Enable                                             */
  __IOM uint32_t         CTRL_CLR;              /**< Control                                            */
  __IOM uint32_t         CMD_CLR;               /**< Command                                            */
  __IOM uint32_t         TIMER_CLR;             /**< Timer                                              */
  __IM uint32_t          STATUS_CLR;            /**< Status                                             */
  __IOM uint32_t         MASKREQ_CLR;           /**< Mask Request                                       */
  __IM uint32_t          STMASK_CLR;            /**< Scan Table Mask                                    */
  __IOM uint32_t         CMPTHR_CLR;            /**< Digital Window Comparator Threshold                */
  __IOM uint32_t         IF_CLR;                /**< Interrupt Flags                                    */
  __IOM uint32_t         IEN_CLR;               /**< Interrupt Enable                                   */
  __IOM uint32_t         TRIGGER_CLR;           /**< Trigger                                            */
  uint32_t               RESERVED18[1U];        /**< Reserved for future use                            */
  uint32_t               RESERVED19[5U];        /**< Reserved for future use                            */
  IADC_CFG_TypeDef       CFG_CLR[2U];           /**< CFG                                                */
  uint32_t               RESERVED20[2U];        /**< Reserved for future use                            */
  __IOM uint32_t         SINGLEFIFOCFG_CLR;     /**< Single FIFO Configuration                          */
  __IM uint32_t          SINGLEFIFODATA_CLR;    /**< Single FIFO DATA                                   */
  __IM uint32_t          SINGLEFIFOSTAT_CLR;    /**< Single FIFO Status                                 */
  __IM uint32_t          SINGLEDATA_CLR;        /**< Single Data                                        */
  __IOM uint32_t         SCANFIFOCFG_CLR;       /**< Scan FIFO Configuration                            */
  __IM uint32_t          SCANFIFODATA_CLR;      /**< Scan FIFO Read Data                                */
  __IM uint32_t          SCANFIFOSTAT_CLR;      /**< Scan FIFO Status                                   */
  __IM uint32_t          SCANDATA_CLR;          /**< Scan Data                                          */
  uint32_t               RESERVED21[1U];        /**< Reserved for future use                            */
  uint32_t               RESERVED22[1U];        /**< Reserved for future use                            */
  __IOM uint32_t         SINGLE_CLR;            /**< Single Queue Port Selection                        */
  uint32_t               RESERVED23[1U];        /**< Reserved for future use                            */
  IADC_SCANTABLE_TypeDef SCANTABLE_CLR[16U];    /**< SCANTABLE                                          */
  uint32_t               RESERVED24[4U];        /**< Reserved for future use                            */
  uint32_t               RESERVED25[1U];        /**< Reserved for future use                            */
  uint32_t               RESERVED26[963U];      /**< Reserved for future use                            */
  __IM uint32_t          IPVERSION_TGL;         /**< IPVERSION                                          */
  __IOM uint32_t         EN_TGL;                /**< Enable                                             */
  __IOM uint32_t         CTRL_TGL;              /**< Control                                            */
  __IOM uint32_t         CMD_TGL;               /**< Command                                            */
  __IOM uint32_t         TIMER_TGL;             /**< Timer                                              */
  __IM uint32_t          STATUS_TGL;            /**< Status                                             */
  __IOM uint32_t         MASKREQ_TGL;           /**< Mask Request                                       */
  __IM uint32_t          STMASK_TGL;            /**< Scan Table Mask                                    */
  __IOM uint32_t         CMPTHR_TGL;            /**< Digital Window Comparator Threshold                */
  __IOM uint32_t         IF_TGL;                /**< Interrupt Flags                                    */
  __IOM uint32_t         IEN_TGL;               /**< Interrupt Enable                                   */
  __IOM uint32_t         TRIGGER_TGL;           /**< Trigger                                            */
  uint32_t               RESERVED27[1U];        /**< Reserved for future use                            */
  uint32_t               RESERVED28[5U];        /**< Reserved for future use                            */
  IADC_CFG_TypeDef       CFG_TGL[2U];           /**< CFG                                                */
  uint32_t               RESERVED29[2U];        /**< Reserved for future use                            */
  __IOM uint32_t         SINGLEFIFOCFG_TGL;     /**< Single FIFO Configuration                          */
  __IM uint32_t          SINGLEFIFODATA_TGL;    /**< Single FIFO DATA                                   */
  __IM uint32_t          SINGLEFIFOSTAT_TGL;    /**< Single FIFO Status                                 */
  __IM uint32_t          SINGLEDATA_TGL;        /**< Single Data                                        */
  __IOM uint32_t         SCANFIFOCFG_TGL;       /**< Scan FIFO Configuration                            */
  __IM uint32_t          SCANFIFODATA_TGL;      /**< Scan FIFO Read Data                                */
  __IM uint32_t          SCANFIFOSTAT_TGL;      /**< Scan FIFO Status                                   */
  __IM uint32_t          SCANDATA_TGL;          /**< Scan Data                                          */
  uint32_t               RESERVED30[1U];        /**< Reserved for future use                            */
  uint32_t               RESERVED31[1U];        /**< Reserved for future use                            */
  __IOM uint32_t         SINGLE_TGL;            /**< Single Queue Port Selection                        */
  uint32_t               RESERVED32[1U];        /**< Reserved for future use                            */
  IADC_SCANTABLE_TypeDef SCANTABLE_TGL[16U];    /**< SCANTABLE                                          */
  uint32_t               RESERVED33[4U];        /**< Reserved for future use                            */
  uint32_t               RESERVED34[1U];        /**< Reserved for future use                            */
} IADC_TypeDef;
/** @} End of group EFR32ZG23_IADC */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_IADC
 * @{
 * @defgroup EFR32ZG23_IADC_BitFields IADC Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for IADC IPVERSION */
#define _IADC_IPVERSION_RESETVALUE                      0x00000002UL                             /**< Default value for IADC_IPVERSION            */
#define _IADC_IPVERSION_MASK                            0xFFFFFFFFUL                             /**< Mask for IADC_IPVERSION                     */
#define _IADC_IPVERSION_IPVERSION_SHIFT                 0                                        /**< Shift value for IADC_IPVERSION              */
#define _IADC_IPVERSION_IPVERSION_MASK                  0xFFFFFFFFUL                             /**< Bit mask for IADC_IPVERSION                 */
#define _IADC_IPVERSION_IPVERSION_DEFAULT               0x00000002UL                             /**< Mode DEFAULT for IADC_IPVERSION             */
#define IADC_IPVERSION_IPVERSION_DEFAULT                (_IADC_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for IADC_IPVERSION     */

/* Bit fields for IADC EN */
#define _IADC_EN_RESETVALUE                             0x00000000UL                      /**< Default value for IADC_EN                   */
#define _IADC_EN_MASK                                   0x00000003UL                      /**< Mask for IADC_EN                            */
#define IADC_EN_EN                                      (0x1UL << 0)                      /**< Enable IADC Module                          */
#define _IADC_EN_EN_SHIFT                               0                                 /**< Shift value for IADC_EN                     */
#define _IADC_EN_EN_MASK                                0x1UL                             /**< Bit mask for IADC_EN                        */
#define _IADC_EN_EN_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for IADC_EN                    */
#define _IADC_EN_EN_DISABLE                             0x00000000UL                      /**< Mode DISABLE for IADC_EN                    */
#define _IADC_EN_EN_ENABLE                              0x00000001UL                      /**< Mode ENABLE for IADC_EN                     */
#define IADC_EN_EN_DEFAULT                              (_IADC_EN_EN_DEFAULT << 0)        /**< Shifted mode DEFAULT for IADC_EN            */
#define IADC_EN_EN_DISABLE                              (_IADC_EN_EN_DISABLE << 0)        /**< Shifted mode DISABLE for IADC_EN            */
#define IADC_EN_EN_ENABLE                               (_IADC_EN_EN_ENABLE << 0)         /**< Shifted mode ENABLE for IADC_EN             */
#define IADC_EN_DISABLING                               (0x1UL << 1)                      /**< Disablement busy status                     */
#define _IADC_EN_DISABLING_SHIFT                        1                                 /**< Shift value for IADC_DISABLING              */
#define _IADC_EN_DISABLING_MASK                         0x2UL                             /**< Bit mask for IADC_DISABLING                 */
#define _IADC_EN_DISABLING_DEFAULT                      0x00000000UL                      /**< Mode DEFAULT for IADC_EN                    */
#define IADC_EN_DISABLING_DEFAULT                       (_IADC_EN_DISABLING_DEFAULT << 1) /**< Shifted mode DEFAULT for IADC_EN            */

/* Bit fields for IADC CTRL */
#define _IADC_CTRL_RESETVALUE                           0x00000000UL                               /**< Default value for IADC_CTRL                 */
#define _IADC_CTRL_MASK                                 0x707F003FUL                               /**< Mask for IADC_CTRL                          */
#define IADC_CTRL_EM23WUCONVERT                         (0x1UL << 0)                               /**< EM23 Wakeup on Conversion                   */
#define _IADC_CTRL_EM23WUCONVERT_SHIFT                  0                                          /**< Shift value for IADC_EM23WUCONVERT          */
#define _IADC_CTRL_EM23WUCONVERT_MASK                   0x1UL                                      /**< Bit mask for IADC_EM23WUCONVERT             */
#define _IADC_CTRL_EM23WUCONVERT_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for IADC_CTRL                  */
#define _IADC_CTRL_EM23WUCONVERT_WUDVL                  0x00000000UL                               /**< Mode WUDVL for IADC_CTRL                    */
#define _IADC_CTRL_EM23WUCONVERT_WUCONVERT              0x00000001UL                               /**< Mode WUCONVERT for IADC_CTRL                */
#define IADC_CTRL_EM23WUCONVERT_DEFAULT                 (_IADC_CTRL_EM23WUCONVERT_DEFAULT << 0)    /**< Shifted mode DEFAULT for IADC_CTRL          */
#define IADC_CTRL_EM23WUCONVERT_WUDVL                   (_IADC_CTRL_EM23WUCONVERT_WUDVL << 0)      /**< Shifted mode WUDVL for IADC_CTRL            */
#define IADC_CTRL_EM23WUCONVERT_WUCONVERT               (_IADC_CTRL_EM23WUCONVERT_WUCONVERT << 0)  /**< Shifted mode WUCONVERT for IADC_CTRL        */
#define IADC_CTRL_ADCCLKSUSPEND0                        (0x1UL << 1)                               /**< ADC_CLK Suspend - PRS0                      */
#define _IADC_CTRL_ADCCLKSUSPEND0_SHIFT                 1                                          /**< Shift value for IADC_ADCCLKSUSPEND0         */
#define _IADC_CTRL_ADCCLKSUSPEND0_MASK                  0x2UL                                      /**< Bit mask for IADC_ADCCLKSUSPEND0            */
#define _IADC_CTRL_ADCCLKSUSPEND0_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for IADC_CTRL                  */
#define _IADC_CTRL_ADCCLKSUSPEND0_PRSWUDIS              0x00000000UL                               /**< Mode PRSWUDIS for IADC_CTRL                 */
#define _IADC_CTRL_ADCCLKSUSPEND0_PRSWUEN               0x00000001UL                               /**< Mode PRSWUEN for IADC_CTRL                  */
#define IADC_CTRL_ADCCLKSUSPEND0_DEFAULT                (_IADC_CTRL_ADCCLKSUSPEND0_DEFAULT << 1)   /**< Shifted mode DEFAULT for IADC_CTRL          */
#define IADC_CTRL_ADCCLKSUSPEND0_PRSWUDIS               (_IADC_CTRL_ADCCLKSUSPEND0_PRSWUDIS << 1)  /**< Shifted mode PRSWUDIS for IADC_CTRL         */
#define IADC_CTRL_ADCCLKSUSPEND0_PRSWUEN                (_IADC_CTRL_ADCCLKSUSPEND0_PRSWUEN << 1)   /**< Shifted mode PRSWUEN for IADC_CTRL          */
#define IADC_CTRL_ADCCLKSUSPEND1                        (0x1UL << 2)                               /**< ADC_CLK Suspend - PRS1                      */
#define _IADC_CTRL_ADCCLKSUSPEND1_SHIFT                 2                                          /**< Shift value for IADC_ADCCLKSUSPEND1         */
#define _IADC_CTRL_ADCCLKSUSPEND1_MASK                  0x4UL                                      /**< Bit mask for IADC_ADCCLKSUSPEND1            */
#define _IADC_CTRL_ADCCLKSUSPEND1_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for IADC_CTRL                  */
#define _IADC_CTRL_ADCCLKSUSPEND1_PRSWUDIS              0x00000000UL                               /**< Mode PRSWUDIS for IADC_CTRL                 */
#define _IADC_CTRL_ADCCLKSUSPEND1_PRSWUEN               0x00000001UL                               /**< Mode PRSWUEN for IADC_CTRL                  */
#define IADC_CTRL_ADCCLKSUSPEND1_DEFAULT                (_IADC_CTRL_ADCCLKSUSPEND1_DEFAULT << 2)   /**< Shifted mode DEFAULT for IADC_CTRL          */
#define IADC_CTRL_ADCCLKSUSPEND1_PRSWUDIS               (_IADC_CTRL_ADCCLKSUSPEND1_PRSWUDIS << 2)  /**< Shifted mode PRSWUDIS for IADC_CTRL         */
#define IADC_CTRL_ADCCLKSUSPEND1_PRSWUEN                (_IADC_CTRL_ADCCLKSUSPEND1_PRSWUEN << 2)   /**< Shifted mode PRSWUEN for IADC_CTRL          */
#define IADC_CTRL_DBGHALT                               (0x1UL << 3)                               /**< Debug Halt                                  */
#define _IADC_CTRL_DBGHALT_SHIFT                        3                                          /**< Shift value for IADC_DBGHALT                */
#define _IADC_CTRL_DBGHALT_MASK                         0x8UL                                      /**< Bit mask for IADC_DBGHALT                   */
#define _IADC_CTRL_DBGHALT_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for IADC_CTRL                  */
#define _IADC_CTRL_DBGHALT_NORMAL                       0x00000000UL                               /**< Mode NORMAL for IADC_CTRL                   */
#define _IADC_CTRL_DBGHALT_HALT                         0x00000001UL                               /**< Mode HALT for IADC_CTRL                     */
#define IADC_CTRL_DBGHALT_DEFAULT                       (_IADC_CTRL_DBGHALT_DEFAULT << 3)          /**< Shifted mode DEFAULT for IADC_CTRL          */
#define IADC_CTRL_DBGHALT_NORMAL                        (_IADC_CTRL_DBGHALT_NORMAL << 3)           /**< Shifted mode NORMAL for IADC_CTRL           */
#define IADC_CTRL_DBGHALT_HALT                          (_IADC_CTRL_DBGHALT_HALT << 3)             /**< Shifted mode HALT for IADC_CTRL             */
#define _IADC_CTRL_WARMUPMODE_SHIFT                     4                                          /**< Shift value for IADC_WARMUPMODE             */
#define _IADC_CTRL_WARMUPMODE_MASK                      0x30UL                                     /**< Bit mask for IADC_WARMUPMODE                */
#define _IADC_CTRL_WARMUPMODE_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for IADC_CTRL                  */
#define _IADC_CTRL_WARMUPMODE_NORMAL                    0x00000000UL                               /**< Mode NORMAL for IADC_CTRL                   */
#define _IADC_CTRL_WARMUPMODE_KEEPINSTANDBY             0x00000001UL                               /**< Mode KEEPINSTANDBY for IADC_CTRL            */
#define _IADC_CTRL_WARMUPMODE_KEEPWARM                  0x00000002UL                               /**< Mode KEEPWARM for IADC_CTRL                 */
#define IADC_CTRL_WARMUPMODE_DEFAULT                    (_IADC_CTRL_WARMUPMODE_DEFAULT << 4)       /**< Shifted mode DEFAULT for IADC_CTRL          */
#define IADC_CTRL_WARMUPMODE_NORMAL                     (_IADC_CTRL_WARMUPMODE_NORMAL << 4)        /**< Shifted mode NORMAL for IADC_CTRL           */
#define IADC_CTRL_WARMUPMODE_KEEPINSTANDBY              (_IADC_CTRL_WARMUPMODE_KEEPINSTANDBY << 4) /**< Shifted mode KEEPINSTANDBY for IADC_CTRL    */
#define IADC_CTRL_WARMUPMODE_KEEPWARM                   (_IADC_CTRL_WARMUPMODE_KEEPWARM << 4)      /**< Shifted mode KEEPWARM for IADC_CTRL         */
#define _IADC_CTRL_TIMEBASE_SHIFT                       16                                         /**< Shift value for IADC_TIMEBASE               */
#define _IADC_CTRL_TIMEBASE_MASK                        0x7F0000UL                                 /**< Bit mask for IADC_TIMEBASE                  */
#define _IADC_CTRL_TIMEBASE_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for IADC_CTRL                  */
#define IADC_CTRL_TIMEBASE_DEFAULT                      (_IADC_CTRL_TIMEBASE_DEFAULT << 16)        /**< Shifted mode DEFAULT for IADC_CTRL          */
#define _IADC_CTRL_HSCLKRATE_SHIFT                      28                                         /**< Shift value for IADC_HSCLKRATE              */
#define _IADC_CTRL_HSCLKRATE_MASK                       0x70000000UL                               /**< Bit mask for IADC_HSCLKRATE                 */
#define _IADC_CTRL_HSCLKRATE_DEFAULT                    0x00000000UL                               /**< Mode DEFAULT for IADC_CTRL                  */
#define _IADC_CTRL_HSCLKRATE_DIV1                       0x00000000UL                               /**< Mode DIV1 for IADC_CTRL                     */
#define _IADC_CTRL_HSCLKRATE_DIV2                       0x00000001UL                               /**< Mode DIV2 for IADC_CTRL                     */
#define _IADC_CTRL_HSCLKRATE_DIV3                       0x00000002UL                               /**< Mode DIV3 for IADC_CTRL                     */
#define _IADC_CTRL_HSCLKRATE_DIV4                       0x00000003UL                               /**< Mode DIV4 for IADC_CTRL                     */
#define IADC_CTRL_HSCLKRATE_DEFAULT                     (_IADC_CTRL_HSCLKRATE_DEFAULT << 28)       /**< Shifted mode DEFAULT for IADC_CTRL          */
#define IADC_CTRL_HSCLKRATE_DIV1                        (_IADC_CTRL_HSCLKRATE_DIV1 << 28)          /**< Shifted mode DIV1 for IADC_CTRL             */
#define IADC_CTRL_HSCLKRATE_DIV2                        (_IADC_CTRL_HSCLKRATE_DIV2 << 28)          /**< Shifted mode DIV2 for IADC_CTRL             */
#define IADC_CTRL_HSCLKRATE_DIV3                        (_IADC_CTRL_HSCLKRATE_DIV3 << 28)          /**< Shifted mode DIV3 for IADC_CTRL             */
#define IADC_CTRL_HSCLKRATE_DIV4                        (_IADC_CTRL_HSCLKRATE_DIV4 << 28)          /**< Shifted mode DIV4 for IADC_CTRL             */

/* Bit fields for IADC CMD */
#define _IADC_CMD_RESETVALUE                            0x00000000UL                              /**< Default value for IADC_CMD                  */
#define _IADC_CMD_MASK                                  0x0303001BUL                              /**< Mask for IADC_CMD                           */
#define IADC_CMD_SINGLESTART                            (0x1UL << 0)                              /**< Single Queue Start                          */
#define _IADC_CMD_SINGLESTART_SHIFT                     0                                         /**< Shift value for IADC_SINGLESTART            */
#define _IADC_CMD_SINGLESTART_MASK                      0x1UL                                     /**< Bit mask for IADC_SINGLESTART               */
#define _IADC_CMD_SINGLESTART_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for IADC_CMD                   */
#define IADC_CMD_SINGLESTART_DEFAULT                    (_IADC_CMD_SINGLESTART_DEFAULT << 0)      /**< Shifted mode DEFAULT for IADC_CMD           */
#define IADC_CMD_SINGLESTOP                             (0x1UL << 1)                              /**< Single Queue Stop                           */
#define _IADC_CMD_SINGLESTOP_SHIFT                      1                                         /**< Shift value for IADC_SINGLESTOP             */
#define _IADC_CMD_SINGLESTOP_MASK                       0x2UL                                     /**< Bit mask for IADC_SINGLESTOP                */
#define _IADC_CMD_SINGLESTOP_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for IADC_CMD                   */
#define IADC_CMD_SINGLESTOP_DEFAULT                     (_IADC_CMD_SINGLESTOP_DEFAULT << 1)       /**< Shifted mode DEFAULT for IADC_CMD           */
#define IADC_CMD_SCANSTART                              (0x1UL << 3)                              /**< Scan Queue Start                            */
#define _IADC_CMD_SCANSTART_SHIFT                       3                                         /**< Shift value for IADC_SCANSTART              */
#define _IADC_CMD_SCANSTART_MASK                        0x8UL                                     /**< Bit mask for IADC_SCANSTART                 */
#define _IADC_CMD_SCANSTART_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for IADC_CMD                   */
#define IADC_CMD_SCANSTART_DEFAULT                      (_IADC_CMD_SCANSTART_DEFAULT << 3)        /**< Shifted mode DEFAULT for IADC_CMD           */
#define IADC_CMD_SCANSTOP                               (0x1UL << 4)                              /**< Scan Queue Stop                             */
#define _IADC_CMD_SCANSTOP_SHIFT                        4                                         /**< Shift value for IADC_SCANSTOP               */
#define _IADC_CMD_SCANSTOP_MASK                         0x10UL                                    /**< Bit mask for IADC_SCANSTOP                  */
#define _IADC_CMD_SCANSTOP_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for IADC_CMD                   */
#define IADC_CMD_SCANSTOP_DEFAULT                       (_IADC_CMD_SCANSTOP_DEFAULT << 4)         /**< Shifted mode DEFAULT for IADC_CMD           */
#define IADC_CMD_TIMEREN                                (0x1UL << 16)                             /**< Timer Enable                                */
#define _IADC_CMD_TIMEREN_SHIFT                         16                                        /**< Shift value for IADC_TIMEREN                */
#define _IADC_CMD_TIMEREN_MASK                          0x10000UL                                 /**< Bit mask for IADC_TIMEREN                   */
#define _IADC_CMD_TIMEREN_DEFAULT                       0x00000000UL                              /**< Mode DEFAULT for IADC_CMD                   */
#define IADC_CMD_TIMEREN_DEFAULT                        (_IADC_CMD_TIMEREN_DEFAULT << 16)         /**< Shifted mode DEFAULT for IADC_CMD           */
#define IADC_CMD_TIMERDIS                               (0x1UL << 17)                             /**< Timer Disable                               */
#define _IADC_CMD_TIMERDIS_SHIFT                        17                                        /**< Shift value for IADC_TIMERDIS               */
#define _IADC_CMD_TIMERDIS_MASK                         0x20000UL                                 /**< Bit mask for IADC_TIMERDIS                  */
#define _IADC_CMD_TIMERDIS_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for IADC_CMD                   */
#define IADC_CMD_TIMERDIS_DEFAULT                       (_IADC_CMD_TIMERDIS_DEFAULT << 17)        /**< Shifted mode DEFAULT for IADC_CMD           */
#define IADC_CMD_SINGLEFIFOFLUSH                        (0x1UL << 24)                             /**< Flush the Single FIFO                       */
#define _IADC_CMD_SINGLEFIFOFLUSH_SHIFT                 24                                        /**< Shift value for IADC_SINGLEFIFOFLUSH        */
#define _IADC_CMD_SINGLEFIFOFLUSH_MASK                  0x1000000UL                               /**< Bit mask for IADC_SINGLEFIFOFLUSH           */
#define _IADC_CMD_SINGLEFIFOFLUSH_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for IADC_CMD                   */
#define IADC_CMD_SINGLEFIFOFLUSH_DEFAULT                (_IADC_CMD_SINGLEFIFOFLUSH_DEFAULT << 24) /**< Shifted mode DEFAULT for IADC_CMD           */
#define IADC_CMD_SCANFIFOFLUSH                          (0x1UL << 25)                             /**< Flush the Scan FIFO                         */
#define _IADC_CMD_SCANFIFOFLUSH_SHIFT                   25                                        /**< Shift value for IADC_SCANFIFOFLUSH          */
#define _IADC_CMD_SCANFIFOFLUSH_MASK                    0x2000000UL                               /**< Bit mask for IADC_SCANFIFOFLUSH             */
#define _IADC_CMD_SCANFIFOFLUSH_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for IADC_CMD                   */
#define IADC_CMD_SCANFIFOFLUSH_DEFAULT                  (_IADC_CMD_SCANFIFOFLUSH_DEFAULT << 25)   /**< Shifted mode DEFAULT for IADC_CMD           */

/* Bit fields for IADC TIMER */
#define _IADC_TIMER_RESETVALUE                          0x00000000UL                     /**< Default value for IADC_TIMER                */
#define _IADC_TIMER_MASK                                0x0000FFFFUL                     /**< Mask for IADC_TIMER                         */
#define _IADC_TIMER_TIMER_SHIFT                         0                                /**< Shift value for IADC_TIMER                  */
#define _IADC_TIMER_TIMER_MASK                          0xFFFFUL                         /**< Bit mask for IADC_TIMER                     */
#define _IADC_TIMER_TIMER_DEFAULT                       0x00000000UL                     /**< Mode DEFAULT for IADC_TIMER                 */
#define IADC_TIMER_TIMER_DEFAULT                        (_IADC_TIMER_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for IADC_TIMER         */

/* Bit fields for IADC STATUS */
#define _IADC_STATUS_RESETVALUE                         0x00000000UL                                     /**< Default value for IADC_STATUS               */
#define _IADC_STATUS_MASK                               0x4131CF5BUL                                     /**< Mask for IADC_STATUS                        */
#define IADC_STATUS_SINGLEQEN                           (0x1UL << 0)                                     /**< Single Queue Enabled                        */
#define _IADC_STATUS_SINGLEQEN_SHIFT                    0                                                /**< Shift value for IADC_SINGLEQEN              */
#define _IADC_STATUS_SINGLEQEN_MASK                     0x1UL                                            /**< Bit mask for IADC_SINGLEQEN                 */
#define _IADC_STATUS_SINGLEQEN_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_SINGLEQEN_DEFAULT                   (_IADC_STATUS_SINGLEQEN_DEFAULT << 0)            /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_SINGLEQUEUEPENDING                  (0x1UL << 1)                                     /**< Single Queue Pending                        */
#define _IADC_STATUS_SINGLEQUEUEPENDING_SHIFT           1                                                /**< Shift value for IADC_SINGLEQUEUEPENDING     */
#define _IADC_STATUS_SINGLEQUEUEPENDING_MASK            0x2UL                                            /**< Bit mask for IADC_SINGLEQUEUEPENDING        */
#define _IADC_STATUS_SINGLEQUEUEPENDING_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_SINGLEQUEUEPENDING_DEFAULT          (_IADC_STATUS_SINGLEQUEUEPENDING_DEFAULT << 1)   /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_SCANQEN                             (0x1UL << 3)                                     /**< Scan Queued Enabled                         */
#define _IADC_STATUS_SCANQEN_SHIFT                      3                                                /**< Shift value for IADC_SCANQEN                */
#define _IADC_STATUS_SCANQEN_MASK                       0x8UL                                            /**< Bit mask for IADC_SCANQEN                   */
#define _IADC_STATUS_SCANQEN_DEFAULT                    0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_SCANQEN_DEFAULT                     (_IADC_STATUS_SCANQEN_DEFAULT << 3)              /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_SCANQUEUEPENDING                    (0x1UL << 4)                                     /**< Scan Queue Pending                          */
#define _IADC_STATUS_SCANQUEUEPENDING_SHIFT             4                                                /**< Shift value for IADC_SCANQUEUEPENDING       */
#define _IADC_STATUS_SCANQUEUEPENDING_MASK              0x10UL                                           /**< Bit mask for IADC_SCANQUEUEPENDING          */
#define _IADC_STATUS_SCANQUEUEPENDING_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_SCANQUEUEPENDING_DEFAULT            (_IADC_STATUS_SCANQUEUEPENDING_DEFAULT << 4)     /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_CONVERTING                          (0x1UL << 6)                                     /**< Converting                                  */
#define _IADC_STATUS_CONVERTING_SHIFT                   6                                                /**< Shift value for IADC_CONVERTING             */
#define _IADC_STATUS_CONVERTING_MASK                    0x40UL                                           /**< Bit mask for IADC_CONVERTING                */
#define _IADC_STATUS_CONVERTING_DEFAULT                 0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_CONVERTING_DEFAULT                  (_IADC_STATUS_CONVERTING_DEFAULT << 6)           /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_SINGLEFIFODV                        (0x1UL << 8)                                     /**< SINGLEFIFO Data Valid                       */
#define _IADC_STATUS_SINGLEFIFODV_SHIFT                 8                                                /**< Shift value for IADC_SINGLEFIFODV           */
#define _IADC_STATUS_SINGLEFIFODV_MASK                  0x100UL                                          /**< Bit mask for IADC_SINGLEFIFODV              */
#define _IADC_STATUS_SINGLEFIFODV_DEFAULT               0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_SINGLEFIFODV_DEFAULT                (_IADC_STATUS_SINGLEFIFODV_DEFAULT << 8)         /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_SCANFIFODV                          (0x1UL << 9)                                     /**< SCANFIFO Data Valid                         */
#define _IADC_STATUS_SCANFIFODV_SHIFT                   9                                                /**< Shift value for IADC_SCANFIFODV             */
#define _IADC_STATUS_SCANFIFODV_MASK                    0x200UL                                          /**< Bit mask for IADC_SCANFIFODV                */
#define _IADC_STATUS_SCANFIFODV_DEFAULT                 0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_SCANFIFODV_DEFAULT                  (_IADC_STATUS_SCANFIFODV_DEFAULT << 9)           /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_SINGLEFIFOFLUSHING                  (0x1UL << 14)                                    /**< The Single FIFO is flushing                 */
#define _IADC_STATUS_SINGLEFIFOFLUSHING_SHIFT           14                                               /**< Shift value for IADC_SINGLEFIFOFLUSHING     */
#define _IADC_STATUS_SINGLEFIFOFLUSHING_MASK            0x4000UL                                         /**< Bit mask for IADC_SINGLEFIFOFLUSHING        */
#define _IADC_STATUS_SINGLEFIFOFLUSHING_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_SINGLEFIFOFLUSHING_DEFAULT          (_IADC_STATUS_SINGLEFIFOFLUSHING_DEFAULT << 14)  /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_SCANFIFOFLUSHING                    (0x1UL << 15)                                    /**< The Scan FIFO is flushing                   */
#define _IADC_STATUS_SCANFIFOFLUSHING_SHIFT             15                                               /**< Shift value for IADC_SCANFIFOFLUSHING       */
#define _IADC_STATUS_SCANFIFOFLUSHING_MASK              0x8000UL                                         /**< Bit mask for IADC_SCANFIFOFLUSHING          */
#define _IADC_STATUS_SCANFIFOFLUSHING_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_SCANFIFOFLUSHING_DEFAULT            (_IADC_STATUS_SCANFIFOFLUSHING_DEFAULT << 15)    /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_TIMERACTIVE                         (0x1UL << 16)                                    /**< Timer Active                                */
#define _IADC_STATUS_TIMERACTIVE_SHIFT                  16                                               /**< Shift value for IADC_TIMERACTIVE            */
#define _IADC_STATUS_TIMERACTIVE_MASK                   0x10000UL                                        /**< Bit mask for IADC_TIMERACTIVE               */
#define _IADC_STATUS_TIMERACTIVE_DEFAULT                0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_TIMERACTIVE_DEFAULT                 (_IADC_STATUS_TIMERACTIVE_DEFAULT << 16)         /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_SINGLEWRITEPENDING                  (0x1UL << 20)                                    /**< SINGLE write pending                        */
#define _IADC_STATUS_SINGLEWRITEPENDING_SHIFT           20                                               /**< Shift value for IADC_SINGLEWRITEPENDING     */
#define _IADC_STATUS_SINGLEWRITEPENDING_MASK            0x100000UL                                       /**< Bit mask for IADC_SINGLEWRITEPENDING        */
#define _IADC_STATUS_SINGLEWRITEPENDING_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_SINGLEWRITEPENDING_DEFAULT          (_IADC_STATUS_SINGLEWRITEPENDING_DEFAULT << 20)  /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_MASKREQWRITEPENDING                 (0x1UL << 21)                                    /**< MASKREQ write pending                       */
#define _IADC_STATUS_MASKREQWRITEPENDING_SHIFT          21                                               /**< Shift value for IADC_MASKREQWRITEPENDING    */
#define _IADC_STATUS_MASKREQWRITEPENDING_MASK           0x200000UL                                       /**< Bit mask for IADC_MASKREQWRITEPENDING       */
#define _IADC_STATUS_MASKREQWRITEPENDING_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_MASKREQWRITEPENDING_DEFAULT         (_IADC_STATUS_MASKREQWRITEPENDING_DEFAULT << 21) /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_SYNCBUSY                            (0x1UL << 24)                                    /**< SYNCBUSY                                    */
#define _IADC_STATUS_SYNCBUSY_SHIFT                     24                                               /**< Shift value for IADC_SYNCBUSY               */
#define _IADC_STATUS_SYNCBUSY_MASK                      0x1000000UL                                      /**< Bit mask for IADC_SYNCBUSY                  */
#define _IADC_STATUS_SYNCBUSY_DEFAULT                   0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_SYNCBUSY_DEFAULT                    (_IADC_STATUS_SYNCBUSY_DEFAULT << 24)            /**< Shifted mode DEFAULT for IADC_STATUS        */
#define IADC_STATUS_ADCWARM                             (0x1UL << 30)                                    /**< ADCWARM                                     */
#define _IADC_STATUS_ADCWARM_SHIFT                      30                                               /**< Shift value for IADC_ADCWARM                */
#define _IADC_STATUS_ADCWARM_MASK                       0x40000000UL                                     /**< Bit mask for IADC_ADCWARM                   */
#define _IADC_STATUS_ADCWARM_DEFAULT                    0x00000000UL                                     /**< Mode DEFAULT for IADC_STATUS                */
#define IADC_STATUS_ADCWARM_DEFAULT                     (_IADC_STATUS_ADCWARM_DEFAULT << 30)             /**< Shifted mode DEFAULT for IADC_STATUS        */

/* Bit fields for IADC MASKREQ */
#define _IADC_MASKREQ_RESETVALUE                        0x00000000UL                         /**< Default value for IADC_MASKREQ              */
#define _IADC_MASKREQ_MASK                              0x0000FFFFUL                         /**< Mask for IADC_MASKREQ                       */
#define _IADC_MASKREQ_MASKREQ_SHIFT                     0                                    /**< Shift value for IADC_MASKREQ                */
#define _IADC_MASKREQ_MASKREQ_MASK                      0xFFFFUL                             /**< Bit mask for IADC_MASKREQ                   */
#define _IADC_MASKREQ_MASKREQ_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for IADC_MASKREQ               */
#define IADC_MASKREQ_MASKREQ_DEFAULT                    (_IADC_MASKREQ_MASKREQ_DEFAULT << 0) /**< Shifted mode DEFAULT for IADC_MASKREQ       */

/* Bit fields for IADC STMASK */
#define _IADC_STMASK_RESETVALUE                         0x00000000UL                       /**< Default value for IADC_STMASK               */
#define _IADC_STMASK_MASK                               0x0000FFFFUL                       /**< Mask for IADC_STMASK                        */
#define _IADC_STMASK_STMASK_SHIFT                       0                                  /**< Shift value for IADC_STMASK                 */
#define _IADC_STMASK_STMASK_MASK                        0xFFFFUL                           /**< Bit mask for IADC_STMASK                    */
#define _IADC_STMASK_STMASK_DEFAULT                     0x00000000UL                       /**< Mode DEFAULT for IADC_STMASK                */
#define IADC_STMASK_STMASK_DEFAULT                      (_IADC_STMASK_STMASK_DEFAULT << 0) /**< Shifted mode DEFAULT for IADC_STMASK        */

/* Bit fields for IADC CMPTHR */
#define _IADC_CMPTHR_RESETVALUE                         0x00000000UL                      /**< Default value for IADC_CMPTHR               */
#define _IADC_CMPTHR_MASK                               0xFFFFFFFFUL                      /**< Mask for IADC_CMPTHR                        */
#define _IADC_CMPTHR_ADLT_SHIFT                         0                                 /**< Shift value for IADC_ADLT                   */
#define _IADC_CMPTHR_ADLT_MASK                          0xFFFFUL                          /**< Bit mask for IADC_ADLT                      */
#define _IADC_CMPTHR_ADLT_DEFAULT                       0x00000000UL                      /**< Mode DEFAULT for IADC_CMPTHR                */
#define IADC_CMPTHR_ADLT_DEFAULT                        (_IADC_CMPTHR_ADLT_DEFAULT << 0)  /**< Shifted mode DEFAULT for IADC_CMPTHR        */
#define _IADC_CMPTHR_ADGT_SHIFT                         16                                /**< Shift value for IADC_ADGT                   */
#define _IADC_CMPTHR_ADGT_MASK                          0xFFFF0000UL                      /**< Bit mask for IADC_ADGT                      */
#define _IADC_CMPTHR_ADGT_DEFAULT                       0x00000000UL                      /**< Mode DEFAULT for IADC_CMPTHR                */
#define IADC_CMPTHR_ADGT_DEFAULT                        (_IADC_CMPTHR_ADGT_DEFAULT << 16) /**< Shifted mode DEFAULT for IADC_CMPTHR        */

/* Bit fields for IADC IF */
#define _IADC_IF_RESETVALUE                             0x00000000UL                            /**< Default value for IADC_IF                   */
#define _IADC_IF_MASK                                   0x800F338FUL                            /**< Mask for IADC_IF                            */
#define IADC_IF_SINGLEFIFODVL                           (0x1UL << 0)                            /**< Single FIFO Data Valid Level                */
#define _IADC_IF_SINGLEFIFODVL_SHIFT                    0                                       /**< Shift value for IADC_SINGLEFIFODVL          */
#define _IADC_IF_SINGLEFIFODVL_MASK                     0x1UL                                   /**< Bit mask for IADC_SINGLEFIFODVL             */
#define _IADC_IF_SINGLEFIFODVL_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_SINGLEFIFODVL_DEFAULT                   (_IADC_IF_SINGLEFIFODVL_DEFAULT << 0)   /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_SCANFIFODVL                             (0x1UL << 1)                            /**< Scan FIFO Data Valid Level                  */
#define _IADC_IF_SCANFIFODVL_SHIFT                      1                                       /**< Shift value for IADC_SCANFIFODVL            */
#define _IADC_IF_SCANFIFODVL_MASK                       0x2UL                                   /**< Bit mask for IADC_SCANFIFODVL               */
#define _IADC_IF_SCANFIFODVL_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_SCANFIFODVL_DEFAULT                     (_IADC_IF_SCANFIFODVL_DEFAULT << 1)     /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_SINGLECMP                               (0x1UL << 2)                            /**< Single Result Window Compare                */
#define _IADC_IF_SINGLECMP_SHIFT                        2                                       /**< Shift value for IADC_SINGLECMP              */
#define _IADC_IF_SINGLECMP_MASK                         0x4UL                                   /**< Bit mask for IADC_SINGLECMP                 */
#define _IADC_IF_SINGLECMP_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_SINGLECMP_DEFAULT                       (_IADC_IF_SINGLECMP_DEFAULT << 2)       /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_SCANCMP                                 (0x1UL << 3)                            /**< Scan Result Window Compare                  */
#define _IADC_IF_SCANCMP_SHIFT                          3                                       /**< Shift value for IADC_SCANCMP                */
#define _IADC_IF_SCANCMP_MASK                           0x8UL                                   /**< Bit mask for IADC_SCANCMP                   */
#define _IADC_IF_SCANCMP_DEFAULT                        0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_SCANCMP_DEFAULT                         (_IADC_IF_SCANCMP_DEFAULT << 3)         /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_SCANENTRYDONE                           (0x1UL << 7)                            /**< Scan Entry Done                             */
#define _IADC_IF_SCANENTRYDONE_SHIFT                    7                                       /**< Shift value for IADC_SCANENTRYDONE          */
#define _IADC_IF_SCANENTRYDONE_MASK                     0x80UL                                  /**< Bit mask for IADC_SCANENTRYDONE             */
#define _IADC_IF_SCANENTRYDONE_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_SCANENTRYDONE_DEFAULT                   (_IADC_IF_SCANENTRYDONE_DEFAULT << 7)   /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_SCANTABLEDONE                           (0x1UL << 8)                            /**< Scan Table Done                             */
#define _IADC_IF_SCANTABLEDONE_SHIFT                    8                                       /**< Shift value for IADC_SCANTABLEDONE          */
#define _IADC_IF_SCANTABLEDONE_MASK                     0x100UL                                 /**< Bit mask for IADC_SCANTABLEDONE             */
#define _IADC_IF_SCANTABLEDONE_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_SCANTABLEDONE_DEFAULT                   (_IADC_IF_SCANTABLEDONE_DEFAULT << 8)   /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_SINGLEDONE                              (0x1UL << 9)                            /**< Single Conversion Done                      */
#define _IADC_IF_SINGLEDONE_SHIFT                       9                                       /**< Shift value for IADC_SINGLEDONE             */
#define _IADC_IF_SINGLEDONE_MASK                        0x200UL                                 /**< Bit mask for IADC_SINGLEDONE                */
#define _IADC_IF_SINGLEDONE_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_SINGLEDONE_DEFAULT                      (_IADC_IF_SINGLEDONE_DEFAULT << 9)      /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_POLARITYERR                             (0x1UL << 12)                           /**< Polarity Error                              */
#define _IADC_IF_POLARITYERR_SHIFT                      12                                      /**< Shift value for IADC_POLARITYERR            */
#define _IADC_IF_POLARITYERR_MASK                       0x1000UL                                /**< Bit mask for IADC_POLARITYERR               */
#define _IADC_IF_POLARITYERR_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_POLARITYERR_DEFAULT                     (_IADC_IF_POLARITYERR_DEFAULT << 12)    /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_PORTALLOCERR                            (0x1UL << 13)                           /**< Port Allocation Error                       */
#define _IADC_IF_PORTALLOCERR_SHIFT                     13                                      /**< Shift value for IADC_PORTALLOCERR           */
#define _IADC_IF_PORTALLOCERR_MASK                      0x2000UL                                /**< Bit mask for IADC_PORTALLOCERR              */
#define _IADC_IF_PORTALLOCERR_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_PORTALLOCERR_DEFAULT                    (_IADC_IF_PORTALLOCERR_DEFAULT << 13)   /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_SINGLEFIFOOF                            (0x1UL << 16)                           /**< Single FIFO Overflow                        */
#define _IADC_IF_SINGLEFIFOOF_SHIFT                     16                                      /**< Shift value for IADC_SINGLEFIFOOF           */
#define _IADC_IF_SINGLEFIFOOF_MASK                      0x10000UL                               /**< Bit mask for IADC_SINGLEFIFOOF              */
#define _IADC_IF_SINGLEFIFOOF_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_SINGLEFIFOOF_DEFAULT                    (_IADC_IF_SINGLEFIFOOF_DEFAULT << 16)   /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_SCANFIFOOF                              (0x1UL << 17)                           /**< Scan FIFO Overflow                          */
#define _IADC_IF_SCANFIFOOF_SHIFT                       17                                      /**< Shift value for IADC_SCANFIFOOF             */
#define _IADC_IF_SCANFIFOOF_MASK                        0x20000UL                               /**< Bit mask for IADC_SCANFIFOOF                */
#define _IADC_IF_SCANFIFOOF_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_SCANFIFOOF_DEFAULT                      (_IADC_IF_SCANFIFOOF_DEFAULT << 17)     /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_SINGLEFIFOUF                            (0x1UL << 18)                           /**< Single FIFO Underflow                       */
#define _IADC_IF_SINGLEFIFOUF_SHIFT                     18                                      /**< Shift value for IADC_SINGLEFIFOUF           */
#define _IADC_IF_SINGLEFIFOUF_MASK                      0x40000UL                               /**< Bit mask for IADC_SINGLEFIFOUF              */
#define _IADC_IF_SINGLEFIFOUF_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_SINGLEFIFOUF_DEFAULT                    (_IADC_IF_SINGLEFIFOUF_DEFAULT << 18)   /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_SCANFIFOUF                              (0x1UL << 19)                           /**< Scan FIFO Underflow                         */
#define _IADC_IF_SCANFIFOUF_SHIFT                       19                                      /**< Shift value for IADC_SCANFIFOUF             */
#define _IADC_IF_SCANFIFOUF_MASK                        0x80000UL                               /**< Bit mask for IADC_SCANFIFOUF                */
#define _IADC_IF_SCANFIFOUF_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_SCANFIFOUF_DEFAULT                      (_IADC_IF_SCANFIFOUF_DEFAULT << 19)     /**< Shifted mode DEFAULT for IADC_IF            */
#define IADC_IF_EM23ABORTERROR                          (0x1UL << 31)                           /**< EM2/3 Abort Error                           */
#define _IADC_IF_EM23ABORTERROR_SHIFT                   31                                      /**< Shift value for IADC_EM23ABORTERROR         */
#define _IADC_IF_EM23ABORTERROR_MASK                    0x80000000UL                            /**< Bit mask for IADC_EM23ABORTERROR            */
#define _IADC_IF_EM23ABORTERROR_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for IADC_IF                    */
#define IADC_IF_EM23ABORTERROR_DEFAULT                  (_IADC_IF_EM23ABORTERROR_DEFAULT << 31) /**< Shifted mode DEFAULT for IADC_IF            */

/* Bit fields for IADC IEN */
#define _IADC_IEN_RESETVALUE                            0x00000000UL                             /**< Default value for IADC_IEN                  */
#define _IADC_IEN_MASK                                  0x800F338FUL                             /**< Mask for IADC_IEN                           */
#define IADC_IEN_SINGLEFIFODVL                          (0x1UL << 0)                             /**< Single FIFO Data Valid Level Enable         */
#define _IADC_IEN_SINGLEFIFODVL_SHIFT                   0                                        /**< Shift value for IADC_SINGLEFIFODVL          */
#define _IADC_IEN_SINGLEFIFODVL_MASK                    0x1UL                                    /**< Bit mask for IADC_SINGLEFIFODVL             */
#define _IADC_IEN_SINGLEFIFODVL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_SINGLEFIFODVL_DEFAULT                  (_IADC_IEN_SINGLEFIFODVL_DEFAULT << 0)   /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_SCANFIFODVL                            (0x1UL << 1)                             /**< Scan FIFO Data Valid Level Enable           */
#define _IADC_IEN_SCANFIFODVL_SHIFT                     1                                        /**< Shift value for IADC_SCANFIFODVL            */
#define _IADC_IEN_SCANFIFODVL_MASK                      0x2UL                                    /**< Bit mask for IADC_SCANFIFODVL               */
#define _IADC_IEN_SCANFIFODVL_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_SCANFIFODVL_DEFAULT                    (_IADC_IEN_SCANFIFODVL_DEFAULT << 1)     /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_SINGLECMP                              (0x1UL << 2)                             /**< Single Result Window Compare Enable         */
#define _IADC_IEN_SINGLECMP_SHIFT                       2                                        /**< Shift value for IADC_SINGLECMP              */
#define _IADC_IEN_SINGLECMP_MASK                        0x4UL                                    /**< Bit mask for IADC_SINGLECMP                 */
#define _IADC_IEN_SINGLECMP_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_SINGLECMP_DEFAULT                      (_IADC_IEN_SINGLECMP_DEFAULT << 2)       /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_SCANCMP                                (0x1UL << 3)                             /**< Scan Result Window Compare Enable           */
#define _IADC_IEN_SCANCMP_SHIFT                         3                                        /**< Shift value for IADC_SCANCMP                */
#define _IADC_IEN_SCANCMP_MASK                          0x8UL                                    /**< Bit mask for IADC_SCANCMP                   */
#define _IADC_IEN_SCANCMP_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_SCANCMP_DEFAULT                        (_IADC_IEN_SCANCMP_DEFAULT << 3)         /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_SCANENTRYDONE                          (0x1UL << 7)                             /**< Scan Entry Done Enable                      */
#define _IADC_IEN_SCANENTRYDONE_SHIFT                   7                                        /**< Shift value for IADC_SCANENTRYDONE          */
#define _IADC_IEN_SCANENTRYDONE_MASK                    0x80UL                                   /**< Bit mask for IADC_SCANENTRYDONE             */
#define _IADC_IEN_SCANENTRYDONE_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_SCANENTRYDONE_DEFAULT                  (_IADC_IEN_SCANENTRYDONE_DEFAULT << 7)   /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_SCANTABLEDONE                          (0x1UL << 8)                             /**< Scan Table Done Enable                      */
#define _IADC_IEN_SCANTABLEDONE_SHIFT                   8                                        /**< Shift value for IADC_SCANTABLEDONE          */
#define _IADC_IEN_SCANTABLEDONE_MASK                    0x100UL                                  /**< Bit mask for IADC_SCANTABLEDONE             */
#define _IADC_IEN_SCANTABLEDONE_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_SCANTABLEDONE_DEFAULT                  (_IADC_IEN_SCANTABLEDONE_DEFAULT << 8)   /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_SINGLEDONE                             (0x1UL << 9)                             /**< Single Conversion Done Enable               */
#define _IADC_IEN_SINGLEDONE_SHIFT                      9                                        /**< Shift value for IADC_SINGLEDONE             */
#define _IADC_IEN_SINGLEDONE_MASK                       0x200UL                                  /**< Bit mask for IADC_SINGLEDONE                */
#define _IADC_IEN_SINGLEDONE_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_SINGLEDONE_DEFAULT                     (_IADC_IEN_SINGLEDONE_DEFAULT << 9)      /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_POLARITYERR                            (0x1UL << 12)                            /**< Polarity Error Enable                       */
#define _IADC_IEN_POLARITYERR_SHIFT                     12                                       /**< Shift value for IADC_POLARITYERR            */
#define _IADC_IEN_POLARITYERR_MASK                      0x1000UL                                 /**< Bit mask for IADC_POLARITYERR               */
#define _IADC_IEN_POLARITYERR_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_POLARITYERR_DEFAULT                    (_IADC_IEN_POLARITYERR_DEFAULT << 12)    /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_PORTALLOCERR                           (0x1UL << 13)                            /**< Port Allocation Error Enable                */
#define _IADC_IEN_PORTALLOCERR_SHIFT                    13                                       /**< Shift value for IADC_PORTALLOCERR           */
#define _IADC_IEN_PORTALLOCERR_MASK                     0x2000UL                                 /**< Bit mask for IADC_PORTALLOCERR              */
#define _IADC_IEN_PORTALLOCERR_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_PORTALLOCERR_DEFAULT                   (_IADC_IEN_PORTALLOCERR_DEFAULT << 13)   /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_SINGLEFIFOOF                           (0x1UL << 16)                            /**< Single FIFO Overflow Enable                 */
#define _IADC_IEN_SINGLEFIFOOF_SHIFT                    16                                       /**< Shift value for IADC_SINGLEFIFOOF           */
#define _IADC_IEN_SINGLEFIFOOF_MASK                     0x10000UL                                /**< Bit mask for IADC_SINGLEFIFOOF              */
#define _IADC_IEN_SINGLEFIFOOF_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_SINGLEFIFOOF_DEFAULT                   (_IADC_IEN_SINGLEFIFOOF_DEFAULT << 16)   /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_SCANFIFOOF                             (0x1UL << 17)                            /**< Scan FIFO Overflow Enable                   */
#define _IADC_IEN_SCANFIFOOF_SHIFT                      17                                       /**< Shift value for IADC_SCANFIFOOF             */
#define _IADC_IEN_SCANFIFOOF_MASK                       0x20000UL                                /**< Bit mask for IADC_SCANFIFOOF                */
#define _IADC_IEN_SCANFIFOOF_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_SCANFIFOOF_DEFAULT                     (_IADC_IEN_SCANFIFOOF_DEFAULT << 17)     /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_SINGLEFIFOUF                           (0x1UL << 18)                            /**< Single FIFO Underflow Enable                */
#define _IADC_IEN_SINGLEFIFOUF_SHIFT                    18                                       /**< Shift value for IADC_SINGLEFIFOUF           */
#define _IADC_IEN_SINGLEFIFOUF_MASK                     0x40000UL                                /**< Bit mask for IADC_SINGLEFIFOUF              */
#define _IADC_IEN_SINGLEFIFOUF_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_SINGLEFIFOUF_DEFAULT                   (_IADC_IEN_SINGLEFIFOUF_DEFAULT << 18)   /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_SCANFIFOUF                             (0x1UL << 19)                            /**< Scan FIFO Underflow Enable                  */
#define _IADC_IEN_SCANFIFOUF_SHIFT                      19                                       /**< Shift value for IADC_SCANFIFOUF             */
#define _IADC_IEN_SCANFIFOUF_MASK                       0x80000UL                                /**< Bit mask for IADC_SCANFIFOUF                */
#define _IADC_IEN_SCANFIFOUF_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_SCANFIFOUF_DEFAULT                     (_IADC_IEN_SCANFIFOUF_DEFAULT << 19)     /**< Shifted mode DEFAULT for IADC_IEN           */
#define IADC_IEN_EM23ABORTERROR                         (0x1UL << 31)                            /**< EM2/3 Abort Error Enable                    */
#define _IADC_IEN_EM23ABORTERROR_SHIFT                  31                                       /**< Shift value for IADC_EM23ABORTERROR         */
#define _IADC_IEN_EM23ABORTERROR_MASK                   0x80000000UL                             /**< Bit mask for IADC_EM23ABORTERROR            */
#define _IADC_IEN_EM23ABORTERROR_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for IADC_IEN                   */
#define IADC_IEN_EM23ABORTERROR_DEFAULT                 (_IADC_IEN_EM23ABORTERROR_DEFAULT << 31) /**< Shifted mode DEFAULT for IADC_IEN           */

/* Bit fields for IADC TRIGGER */
#define _IADC_TRIGGER_RESETVALUE                        0x00000000UL                                      /**< Default value for IADC_TRIGGER              */
#define _IADC_TRIGGER_MASK                              0x00011717UL                                      /**< Mask for IADC_TRIGGER                       */
#define _IADC_TRIGGER_SCANTRIGSEL_SHIFT                 0                                                 /**< Shift value for IADC_SCANTRIGSEL            */
#define _IADC_TRIGGER_SCANTRIGSEL_MASK                  0x7UL                                             /**< Bit mask for IADC_SCANTRIGSEL               */
#define _IADC_TRIGGER_SCANTRIGSEL_DEFAULT               0x00000000UL                                      /**< Mode DEFAULT for IADC_TRIGGER               */
#define _IADC_TRIGGER_SCANTRIGSEL_IMMEDIATE             0x00000000UL                                      /**< Mode IMMEDIATE for IADC_TRIGGER             */
#define _IADC_TRIGGER_SCANTRIGSEL_TIMER                 0x00000001UL                                      /**< Mode TIMER for IADC_TRIGGER                 */
#define _IADC_TRIGGER_SCANTRIGSEL_PRSCLKGRP             0x00000002UL                                      /**< Mode PRSCLKGRP for IADC_TRIGGER             */
#define _IADC_TRIGGER_SCANTRIGSEL_PRSPOS                0x00000003UL                                      /**< Mode PRSPOS for IADC_TRIGGER                */
#define _IADC_TRIGGER_SCANTRIGSEL_PRSNEG                0x00000004UL                                      /**< Mode PRSNEG for IADC_TRIGGER                */
#define _IADC_TRIGGER_SCANTRIGSEL_LESENSE               0x00000005UL                                      /**< Mode LESENSE for IADC_TRIGGER               */
#define IADC_TRIGGER_SCANTRIGSEL_DEFAULT                (_IADC_TRIGGER_SCANTRIGSEL_DEFAULT << 0)          /**< Shifted mode DEFAULT for IADC_TRIGGER       */
#define IADC_TRIGGER_SCANTRIGSEL_IMMEDIATE              (_IADC_TRIGGER_SCANTRIGSEL_IMMEDIATE << 0)        /**< Shifted mode IMMEDIATE for IADC_TRIGGER     */
#define IADC_TRIGGER_SCANTRIGSEL_TIMER                  (_IADC_TRIGGER_SCANTRIGSEL_TIMER << 0)            /**< Shifted mode TIMER for IADC_TRIGGER         */
#define IADC_TRIGGER_SCANTRIGSEL_PRSCLKGRP              (_IADC_TRIGGER_SCANTRIGSEL_PRSCLKGRP << 0)        /**< Shifted mode PRSCLKGRP for IADC_TRIGGER     */
#define IADC_TRIGGER_SCANTRIGSEL_PRSPOS                 (_IADC_TRIGGER_SCANTRIGSEL_PRSPOS << 0)           /**< Shifted mode PRSPOS for IADC_TRIGGER        */
#define IADC_TRIGGER_SCANTRIGSEL_PRSNEG                 (_IADC_TRIGGER_SCANTRIGSEL_PRSNEG << 0)           /**< Shifted mode PRSNEG for IADC_TRIGGER        */
#define IADC_TRIGGER_SCANTRIGSEL_LESENSE                (_IADC_TRIGGER_SCANTRIGSEL_LESENSE << 0)          /**< Shifted mode LESENSE for IADC_TRIGGER       */
#define IADC_TRIGGER_SCANTRIGACTION                     (0x1UL << 4)                                      /**< Scan Trigger Action                         */
#define _IADC_TRIGGER_SCANTRIGACTION_SHIFT              4                                                 /**< Shift value for IADC_SCANTRIGACTION         */
#define _IADC_TRIGGER_SCANTRIGACTION_MASK               0x10UL                                            /**< Bit mask for IADC_SCANTRIGACTION            */
#define _IADC_TRIGGER_SCANTRIGACTION_DEFAULT            0x00000000UL                                      /**< Mode DEFAULT for IADC_TRIGGER               */
#define _IADC_TRIGGER_SCANTRIGACTION_ONCE               0x00000000UL                                      /**< Mode ONCE for IADC_TRIGGER                  */
#define _IADC_TRIGGER_SCANTRIGACTION_CONTINUOUS         0x00000001UL                                      /**< Mode CONTINUOUS for IADC_TRIGGER            */
#define IADC_TRIGGER_SCANTRIGACTION_DEFAULT             (_IADC_TRIGGER_SCANTRIGACTION_DEFAULT << 4)       /**< Shifted mode DEFAULT for IADC_TRIGGER       */
#define IADC_TRIGGER_SCANTRIGACTION_ONCE                (_IADC_TRIGGER_SCANTRIGACTION_ONCE << 4)          /**< Shifted mode ONCE for IADC_TRIGGER          */
#define IADC_TRIGGER_SCANTRIGACTION_CONTINUOUS          (_IADC_TRIGGER_SCANTRIGACTION_CONTINUOUS << 4)    /**< Shifted mode CONTINUOUS for IADC_TRIGGER    */
#define _IADC_TRIGGER_SINGLETRIGSEL_SHIFT               8                                                 /**< Shift value for IADC_SINGLETRIGSEL          */
#define _IADC_TRIGGER_SINGLETRIGSEL_MASK                0x700UL                                           /**< Bit mask for IADC_SINGLETRIGSEL             */
#define _IADC_TRIGGER_SINGLETRIGSEL_DEFAULT             0x00000000UL                                      /**< Mode DEFAULT for IADC_TRIGGER               */
#define _IADC_TRIGGER_SINGLETRIGSEL_IMMEDIATE           0x00000000UL                                      /**< Mode IMMEDIATE for IADC_TRIGGER             */
#define _IADC_TRIGGER_SINGLETRIGSEL_TIMER               0x00000001UL                                      /**< Mode TIMER for IADC_TRIGGER                 */
#define _IADC_TRIGGER_SINGLETRIGSEL_PRSCLKGRP           0x00000002UL                                      /**< Mode PRSCLKGRP for IADC_TRIGGER             */
#define _IADC_TRIGGER_SINGLETRIGSEL_PRSPOS              0x00000003UL                                      /**< Mode PRSPOS for IADC_TRIGGER                */
#define _IADC_TRIGGER_SINGLETRIGSEL_PRSNEG              0x00000004UL                                      /**< Mode PRSNEG for IADC_TRIGGER                */
#define IADC_TRIGGER_SINGLETRIGSEL_DEFAULT              (_IADC_TRIGGER_SINGLETRIGSEL_DEFAULT << 8)        /**< Shifted mode DEFAULT for IADC_TRIGGER       */
#define IADC_TRIGGER_SINGLETRIGSEL_IMMEDIATE            (_IADC_TRIGGER_SINGLETRIGSEL_IMMEDIATE << 8)      /**< Shifted mode IMMEDIATE for IADC_TRIGGER     */
#define IADC_TRIGGER_SINGLETRIGSEL_TIMER                (_IADC_TRIGGER_SINGLETRIGSEL_TIMER << 8)          /**< Shifted mode TIMER for IADC_TRIGGER         */
#define IADC_TRIGGER_SINGLETRIGSEL_PRSCLKGRP            (_IADC_TRIGGER_SINGLETRIGSEL_PRSCLKGRP << 8)      /**< Shifted mode PRSCLKGRP for IADC_TRIGGER     */
#define IADC_TRIGGER_SINGLETRIGSEL_PRSPOS               (_IADC_TRIGGER_SINGLETRIGSEL_PRSPOS << 8)         /**< Shifted mode PRSPOS for IADC_TRIGGER        */
#define IADC_TRIGGER_SINGLETRIGSEL_PRSNEG               (_IADC_TRIGGER_SINGLETRIGSEL_PRSNEG << 8)         /**< Shifted mode PRSNEG for IADC_TRIGGER        */
#define IADC_TRIGGER_SINGLETRIGACTION                   (0x1UL << 12)                                     /**< Single Trigger Action                       */
#define _IADC_TRIGGER_SINGLETRIGACTION_SHIFT            12                                                /**< Shift value for IADC_SINGLETRIGACTION       */
#define _IADC_TRIGGER_SINGLETRIGACTION_MASK             0x1000UL                                          /**< Bit mask for IADC_SINGLETRIGACTION          */
#define _IADC_TRIGGER_SINGLETRIGACTION_DEFAULT          0x00000000UL                                      /**< Mode DEFAULT for IADC_TRIGGER               */
#define _IADC_TRIGGER_SINGLETRIGACTION_ONCE             0x00000000UL                                      /**< Mode ONCE for IADC_TRIGGER                  */
#define _IADC_TRIGGER_SINGLETRIGACTION_CONTINUOUS       0x00000001UL                                      /**< Mode CONTINUOUS for IADC_TRIGGER            */
#define IADC_TRIGGER_SINGLETRIGACTION_DEFAULT           (_IADC_TRIGGER_SINGLETRIGACTION_DEFAULT << 12)    /**< Shifted mode DEFAULT for IADC_TRIGGER       */
#define IADC_TRIGGER_SINGLETRIGACTION_ONCE              (_IADC_TRIGGER_SINGLETRIGACTION_ONCE << 12)       /**< Shifted mode ONCE for IADC_TRIGGER          */
#define IADC_TRIGGER_SINGLETRIGACTION_CONTINUOUS        (_IADC_TRIGGER_SINGLETRIGACTION_CONTINUOUS << 12) /**< Shifted mode CONTINUOUS for IADC_TRIGGER    */
#define IADC_TRIGGER_SINGLETAILGATE                     (0x1UL << 16)                                     /**< Single Tailgate Enable                      */
#define _IADC_TRIGGER_SINGLETAILGATE_SHIFT              16                                                /**< Shift value for IADC_SINGLETAILGATE         */
#define _IADC_TRIGGER_SINGLETAILGATE_MASK               0x10000UL                                         /**< Bit mask for IADC_SINGLETAILGATE            */
#define _IADC_TRIGGER_SINGLETAILGATE_DEFAULT            0x00000000UL                                      /**< Mode DEFAULT for IADC_TRIGGER               */
#define _IADC_TRIGGER_SINGLETAILGATE_TAILGATEOFF        0x00000000UL                                      /**< Mode TAILGATEOFF for IADC_TRIGGER           */
#define _IADC_TRIGGER_SINGLETAILGATE_TAILGATEON         0x00000001UL                                      /**< Mode TAILGATEON for IADC_TRIGGER            */
#define IADC_TRIGGER_SINGLETAILGATE_DEFAULT             (_IADC_TRIGGER_SINGLETAILGATE_DEFAULT << 16)      /**< Shifted mode DEFAULT for IADC_TRIGGER       */
#define IADC_TRIGGER_SINGLETAILGATE_TAILGATEOFF         (_IADC_TRIGGER_SINGLETAILGATE_TAILGATEOFF << 16)  /**< Shifted mode TAILGATEOFF for IADC_TRIGGER   */
#define IADC_TRIGGER_SINGLETAILGATE_TAILGATEON          (_IADC_TRIGGER_SINGLETAILGATE_TAILGATEON << 16)   /**< Shifted mode TAILGATEON for IADC_TRIGGER    */

/* Bit fields for IADC CFG */
#define _IADC_CFG_RESETVALUE                            0x00002060UL                              /**< Default value for IADC_CFG                  */
#define _IADC_CFG_MASK                                  0x30E770FFUL                              /**< Mask for IADC_CFG                           */
#define _IADC_CFG_ADCMODE_SHIFT                         0                                         /**< Shift value for IADC_ADCMODE                */
#define _IADC_CFG_ADCMODE_MASK                          0x3UL                                     /**< Bit mask for IADC_ADCMODE                   */
#define _IADC_CFG_ADCMODE_DEFAULT                       0x00000000UL                              /**< Mode DEFAULT for IADC_CFG                   */
#define _IADC_CFG_ADCMODE_NORMAL                        0x00000000UL                              /**< Mode NORMAL for IADC_CFG                    */
#define IADC_CFG_ADCMODE_DEFAULT                        (_IADC_CFG_ADCMODE_DEFAULT << 0)          /**< Shifted mode DEFAULT for IADC_CFG           */
#define IADC_CFG_ADCMODE_NORMAL                         (_IADC_CFG_ADCMODE_NORMAL << 0)           /**< Shifted mode NORMAL for IADC_CFG            */
#define _IADC_CFG_OSRHS_SHIFT                           2                                         /**< Shift value for IADC_OSRHS                  */
#define _IADC_CFG_OSRHS_MASK                            0x1CUL                                    /**< Bit mask for IADC_OSRHS                     */
#define _IADC_CFG_OSRHS_DEFAULT                         0x00000000UL                              /**< Mode DEFAULT for IADC_CFG                   */
#define _IADC_CFG_OSRHS_HISPD2                          0x00000000UL                              /**< Mode HISPD2 for IADC_CFG                    */
#define _IADC_CFG_OSRHS_HISPD4                          0x00000001UL                              /**< Mode HISPD4 for IADC_CFG                    */
#define _IADC_CFG_OSRHS_HISPD8                          0x00000002UL                              /**< Mode HISPD8 for IADC_CFG                    */
#define _IADC_CFG_OSRHS_HISPD16                         0x00000003UL                              /**< Mode HISPD16 for IADC_CFG                   */
#define _IADC_CFG_OSRHS_HISPD32                         0x00000004UL                              /**< Mode HISPD32 for IADC_CFG                   */
#define _IADC_CFG_OSRHS_HISPD64                         0x00000005UL                              /**< Mode HISPD64 for IADC_CFG                   */
#define IADC_CFG_OSRHS_DEFAULT                          (_IADC_CFG_OSRHS_DEFAULT << 2)            /**< Shifted mode DEFAULT for IADC_CFG           */
#define IADC_CFG_OSRHS_HISPD2                           (_IADC_CFG_OSRHS_HISPD2 << 2)             /**< Shifted mode HISPD2 for IADC_CFG            */
#define IADC_CFG_OSRHS_HISPD4                           (_IADC_CFG_OSRHS_HISPD4 << 2)             /**< Shifted mode HISPD4 for IADC_CFG            */
#define IADC_CFG_OSRHS_HISPD8                           (_IADC_CFG_OSRHS_HISPD8 << 2)             /**< Shifted mode HISPD8 for IADC_CFG            */
#define IADC_CFG_OSRHS_HISPD16                          (_IADC_CFG_OSRHS_HISPD16 << 2)            /**< Shifted mode HISPD16 for IADC_CFG           */
#define IADC_CFG_OSRHS_HISPD32                          (_IADC_CFG_OSRHS_HISPD32 << 2)            /**< Shifted mode HISPD32 for IADC_CFG           */
#define IADC_CFG_OSRHS_HISPD64                          (_IADC_CFG_OSRHS_HISPD64 << 2)            /**< Shifted mode HISPD64 for IADC_CFG           */
#define _IADC_CFG_ANALOGGAIN_SHIFT                      12                                        /**< Shift value for IADC_ANALOGGAIN             */
#define _IADC_CFG_ANALOGGAIN_MASK                       0x7000UL                                  /**< Bit mask for IADC_ANALOGGAIN                */
#define _IADC_CFG_ANALOGGAIN_DEFAULT                    0x00000002UL                              /**< Mode DEFAULT for IADC_CFG                   */
#define _IADC_CFG_ANALOGGAIN_ANAGAIN0P5                 0x00000001UL                              /**< Mode ANAGAIN0P5 for IADC_CFG                */
#define _IADC_CFG_ANALOGGAIN_ANAGAIN1                   0x00000002UL                              /**< Mode ANAGAIN1 for IADC_CFG                  */
#define _IADC_CFG_ANALOGGAIN_ANAGAIN2                   0x00000003UL                              /**< Mode ANAGAIN2 for IADC_CFG                  */
#define _IADC_CFG_ANALOGGAIN_ANAGAIN3                   0x00000004UL                              /**< Mode ANAGAIN3 for IADC_CFG                  */
#define _IADC_CFG_ANALOGGAIN_ANAGAIN4                   0x00000005UL                              /**< Mode ANAGAIN4 for IADC_CFG                  */
#define IADC_CFG_ANALOGGAIN_DEFAULT                     (_IADC_CFG_ANALOGGAIN_DEFAULT << 12)      /**< Shifted mode DEFAULT for IADC_CFG           */
#define IADC_CFG_ANALOGGAIN_ANAGAIN0P5                  (_IADC_CFG_ANALOGGAIN_ANAGAIN0P5 << 12)   /**< Shifted mode ANAGAIN0P5 for IADC_CFG        */
#define IADC_CFG_ANALOGGAIN_ANAGAIN1                    (_IADC_CFG_ANALOGGAIN_ANAGAIN1 << 12)     /**< Shifted mode ANAGAIN1 for IADC_CFG          */
#define IADC_CFG_ANALOGGAIN_ANAGAIN2                    (_IADC_CFG_ANALOGGAIN_ANAGAIN2 << 12)     /**< Shifted mode ANAGAIN2 for IADC_CFG          */
#define IADC_CFG_ANALOGGAIN_ANAGAIN3                    (_IADC_CFG_ANALOGGAIN_ANAGAIN3 << 12)     /**< Shifted mode ANAGAIN3 for IADC_CFG          */
#define IADC_CFG_ANALOGGAIN_ANAGAIN4                    (_IADC_CFG_ANALOGGAIN_ANAGAIN4 << 12)     /**< Shifted mode ANAGAIN4 for IADC_CFG          */
#define _IADC_CFG_REFSEL_SHIFT                          16                                        /**< Shift value for IADC_REFSEL                 */
#define _IADC_CFG_REFSEL_MASK                           0x70000UL                                 /**< Bit mask for IADC_REFSEL                    */
#define _IADC_CFG_REFSEL_DEFAULT                        0x00000000UL                              /**< Mode DEFAULT for IADC_CFG                   */
#define _IADC_CFG_REFSEL_VBGR                           0x00000000UL                              /**< Mode VBGR for IADC_CFG                      */
#define _IADC_CFG_REFSEL_VREF                           0x00000001UL                              /**< Mode VREF for IADC_CFG                      */
#define _IADC_CFG_REFSEL_VDDX                           0x00000003UL                              /**< Mode VDDX for IADC_CFG                      */
#define _IADC_CFG_REFSEL_VDDX0P8BUF                     0x00000004UL                              /**< Mode VDDX0P8BUF for IADC_CFG                */
#define IADC_CFG_REFSEL_DEFAULT                         (_IADC_CFG_REFSEL_DEFAULT << 16)          /**< Shifted mode DEFAULT for IADC_CFG           */
#define IADC_CFG_REFSEL_VBGR                            (_IADC_CFG_REFSEL_VBGR << 16)             /**< Shifted mode VBGR for IADC_CFG              */
#define IADC_CFG_REFSEL_VREF                            (_IADC_CFG_REFSEL_VREF << 16)             /**< Shifted mode VREF for IADC_CFG              */
#define IADC_CFG_REFSEL_VDDX                            (_IADC_CFG_REFSEL_VDDX << 16)             /**< Shifted mode VDDX for IADC_CFG              */
#define IADC_CFG_REFSEL_VDDX0P8BUF                      (_IADC_CFG_REFSEL_VDDX0P8BUF << 16)       /**< Shifted mode VDDX0P8BUF for IADC_CFG        */
#define _IADC_CFG_DIGAVG_SHIFT                          21                                        /**< Shift value for IADC_DIGAVG                 */
#define _IADC_CFG_DIGAVG_MASK                           0xE00000UL                                /**< Bit mask for IADC_DIGAVG                    */
#define _IADC_CFG_DIGAVG_DEFAULT                        0x00000000UL                              /**< Mode DEFAULT for IADC_CFG                   */
#define _IADC_CFG_DIGAVG_AVG1                           0x00000000UL                              /**< Mode AVG1 for IADC_CFG                      */
#define _IADC_CFG_DIGAVG_AVG2                           0x00000001UL                              /**< Mode AVG2 for IADC_CFG                      */
#define _IADC_CFG_DIGAVG_AVG4                           0x00000002UL                              /**< Mode AVG4 for IADC_CFG                      */
#define _IADC_CFG_DIGAVG_AVG8                           0x00000003UL                              /**< Mode AVG8 for IADC_CFG                      */
#define _IADC_CFG_DIGAVG_AVG16                          0x00000004UL                              /**< Mode AVG16 for IADC_CFG                     */
#define IADC_CFG_DIGAVG_DEFAULT                         (_IADC_CFG_DIGAVG_DEFAULT << 21)          /**< Shifted mode DEFAULT for IADC_CFG           */
#define IADC_CFG_DIGAVG_AVG1                            (_IADC_CFG_DIGAVG_AVG1 << 21)             /**< Shifted mode AVG1 for IADC_CFG              */
#define IADC_CFG_DIGAVG_AVG2                            (_IADC_CFG_DIGAVG_AVG2 << 21)             /**< Shifted mode AVG2 for IADC_CFG              */
#define IADC_CFG_DIGAVG_AVG4                            (_IADC_CFG_DIGAVG_AVG4 << 21)             /**< Shifted mode AVG4 for IADC_CFG              */
#define IADC_CFG_DIGAVG_AVG8                            (_IADC_CFG_DIGAVG_AVG8 << 21)             /**< Shifted mode AVG8 for IADC_CFG              */
#define IADC_CFG_DIGAVG_AVG16                           (_IADC_CFG_DIGAVG_AVG16 << 21)            /**< Shifted mode AVG16 for IADC_CFG             */
#define _IADC_CFG_TWOSCOMPL_SHIFT                       28                                        /**< Shift value for IADC_TWOSCOMPL              */
#define _IADC_CFG_TWOSCOMPL_MASK                        0x30000000UL                              /**< Bit mask for IADC_TWOSCOMPL                 */
#define _IADC_CFG_TWOSCOMPL_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for IADC_CFG                   */
#define _IADC_CFG_TWOSCOMPL_AUTO                        0x00000000UL                              /**< Mode AUTO for IADC_CFG                      */
#define _IADC_CFG_TWOSCOMPL_FORCEUNIPOLAR               0x00000001UL                              /**< Mode FORCEUNIPOLAR for IADC_CFG             */
#define _IADC_CFG_TWOSCOMPL_FORCEBIPOLAR                0x00000002UL                              /**< Mode FORCEBIPOLAR for IADC_CFG              */
#define IADC_CFG_TWOSCOMPL_DEFAULT                      (_IADC_CFG_TWOSCOMPL_DEFAULT << 28)       /**< Shifted mode DEFAULT for IADC_CFG           */
#define IADC_CFG_TWOSCOMPL_AUTO                         (_IADC_CFG_TWOSCOMPL_AUTO << 28)          /**< Shifted mode AUTO for IADC_CFG              */
#define IADC_CFG_TWOSCOMPL_FORCEUNIPOLAR                (_IADC_CFG_TWOSCOMPL_FORCEUNIPOLAR << 28) /**< Shifted mode FORCEUNIPOLAR for IADC_CFG     */
#define IADC_CFG_TWOSCOMPL_FORCEBIPOLAR                 (_IADC_CFG_TWOSCOMPL_FORCEBIPOLAR << 28)  /**< Shifted mode FORCEBIPOLAR for IADC_CFG      */

/* Bit fields for IADC SCALE */
#define _IADC_SCALE_RESETVALUE                          0x8002C000UL                          /**< Default value for IADC_SCALE                */
#define _IADC_SCALE_MASK                                0xFFFFFFFFUL                          /**< Mask for IADC_SCALE                         */
#define _IADC_SCALE_OFFSET_SHIFT                        0                                     /**< Shift value for IADC_OFFSET                 */
#define _IADC_SCALE_OFFSET_MASK                         0x3FFFFUL                             /**< Bit mask for IADC_OFFSET                    */
#define _IADC_SCALE_OFFSET_DEFAULT                      0x0002C000UL                          /**< Mode DEFAULT for IADC_SCALE                 */
#define IADC_SCALE_OFFSET_DEFAULT                       (_IADC_SCALE_OFFSET_DEFAULT << 0)     /**< Shifted mode DEFAULT for IADC_SCALE         */
#define _IADC_SCALE_GAIN13LSB_SHIFT                     18                                    /**< Shift value for IADC_GAIN13LSB              */
#define _IADC_SCALE_GAIN13LSB_MASK                      0x7FFC0000UL                          /**< Bit mask for IADC_GAIN13LSB                 */
#define _IADC_SCALE_GAIN13LSB_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for IADC_SCALE                 */
#define IADC_SCALE_GAIN13LSB_DEFAULT                    (_IADC_SCALE_GAIN13LSB_DEFAULT << 18) /**< Shifted mode DEFAULT for IADC_SCALE         */
#define IADC_SCALE_GAIN3MSB                             (0x1UL << 31)                         /**< Gain 3 MSBs                                 */
#define _IADC_SCALE_GAIN3MSB_SHIFT                      31                                    /**< Shift value for IADC_GAIN3MSB               */
#define _IADC_SCALE_GAIN3MSB_MASK                       0x80000000UL                          /**< Bit mask for IADC_GAIN3MSB                  */
#define _IADC_SCALE_GAIN3MSB_DEFAULT                    0x00000001UL                          /**< Mode DEFAULT for IADC_SCALE                 */
#define _IADC_SCALE_GAIN3MSB_GAIN011                    0x00000000UL                          /**< Mode GAIN011 for IADC_SCALE                 */
#define _IADC_SCALE_GAIN3MSB_GAIN100                    0x00000001UL                          /**< Mode GAIN100 for IADC_SCALE                 */
#define IADC_SCALE_GAIN3MSB_DEFAULT                     (_IADC_SCALE_GAIN3MSB_DEFAULT << 31)  /**< Shifted mode DEFAULT for IADC_SCALE         */
#define IADC_SCALE_GAIN3MSB_GAIN011                     (_IADC_SCALE_GAIN3MSB_GAIN011 << 31)  /**< Shifted mode GAIN011 for IADC_SCALE         */
#define IADC_SCALE_GAIN3MSB_GAIN100                     (_IADC_SCALE_GAIN3MSB_GAIN100 << 31)  /**< Shifted mode GAIN100 for IADC_SCALE         */

/* Bit fields for IADC SCHED */
#define _IADC_SCHED_RESETVALUE                          0x00000000UL                        /**< Default value for IADC_SCHED                */
#define _IADC_SCHED_MASK                                0x000003FFUL                        /**< Mask for IADC_SCHED                         */
#define _IADC_SCHED_PRESCALE_SHIFT                      0                                   /**< Shift value for IADC_PRESCALE               */
#define _IADC_SCHED_PRESCALE_MASK                       0x3FFUL                             /**< Bit mask for IADC_PRESCALE                  */
#define _IADC_SCHED_PRESCALE_DEFAULT                    0x00000000UL                        /**< Mode DEFAULT for IADC_SCHED                 */
#define IADC_SCHED_PRESCALE_DEFAULT                     (_IADC_SCHED_PRESCALE_DEFAULT << 0) /**< Shifted mode DEFAULT for IADC_SCHED         */

/* Bit fields for IADC SINGLEFIFOCFG */
#define _IADC_SINGLEFIFOCFG_RESETVALUE                  0x00000030UL                                        /**< Default value for IADC_SINGLEFIFOCFG        */
#define _IADC_SINGLEFIFOCFG_MASK                        0x0000017FUL                                        /**< Mask for IADC_SINGLEFIFOCFG                 */
#define _IADC_SINGLEFIFOCFG_ALIGNMENT_SHIFT             0                                                   /**< Shift value for IADC_ALIGNMENT              */
#define _IADC_SINGLEFIFOCFG_ALIGNMENT_MASK              0x7UL                                               /**< Bit mask for IADC_ALIGNMENT                 */
#define _IADC_SINGLEFIFOCFG_ALIGNMENT_DEFAULT           0x00000000UL                                        /**< Mode DEFAULT for IADC_SINGLEFIFOCFG         */
#define _IADC_SINGLEFIFOCFG_ALIGNMENT_RIGHT12           0x00000000UL                                        /**< Mode RIGHT12 for IADC_SINGLEFIFOCFG         */
#define _IADC_SINGLEFIFOCFG_ALIGNMENT_RIGHT16           0x00000001UL                                        /**< Mode RIGHT16 for IADC_SINGLEFIFOCFG         */
#define _IADC_SINGLEFIFOCFG_ALIGNMENT_RIGHT20           0x00000002UL                                        /**< Mode RIGHT20 for IADC_SINGLEFIFOCFG         */
#define _IADC_SINGLEFIFOCFG_ALIGNMENT_LEFT12            0x00000003UL                                        /**< Mode LEFT12 for IADC_SINGLEFIFOCFG          */
#define _IADC_SINGLEFIFOCFG_ALIGNMENT_LEFT16            0x00000004UL                                        /**< Mode LEFT16 for IADC_SINGLEFIFOCFG          */
#define _IADC_SINGLEFIFOCFG_ALIGNMENT_LEFT20            0x00000005UL                                        /**< Mode LEFT20 for IADC_SINGLEFIFOCFG          */
#define IADC_SINGLEFIFOCFG_ALIGNMENT_DEFAULT            (_IADC_SINGLEFIFOCFG_ALIGNMENT_DEFAULT << 0)        /**< Shifted mode DEFAULT for IADC_SINGLEFIFOCFG */
#define IADC_SINGLEFIFOCFG_ALIGNMENT_RIGHT12            (_IADC_SINGLEFIFOCFG_ALIGNMENT_RIGHT12 << 0)        /**< Shifted mode RIGHT12 for IADC_SINGLEFIFOCFG */
#define IADC_SINGLEFIFOCFG_ALIGNMENT_RIGHT16            (_IADC_SINGLEFIFOCFG_ALIGNMENT_RIGHT16 << 0)        /**< Shifted mode RIGHT16 for IADC_SINGLEFIFOCFG */
#define IADC_SINGLEFIFOCFG_ALIGNMENT_RIGHT20            (_IADC_SINGLEFIFOCFG_ALIGNMENT_RIGHT20 << 0)        /**< Shifted mode RIGHT20 for IADC_SINGLEFIFOCFG */
#define IADC_SINGLEFIFOCFG_ALIGNMENT_LEFT12             (_IADC_SINGLEFIFOCFG_ALIGNMENT_LEFT12 << 0)         /**< Shifted mode LEFT12 for IADC_SINGLEFIFOCFG  */
#define IADC_SINGLEFIFOCFG_ALIGNMENT_LEFT16             (_IADC_SINGLEFIFOCFG_ALIGNMENT_LEFT16 << 0)         /**< Shifted mode LEFT16 for IADC_SINGLEFIFOCFG  */
#define IADC_SINGLEFIFOCFG_ALIGNMENT_LEFT20             (_IADC_SINGLEFIFOCFG_ALIGNMENT_LEFT20 << 0)         /**< Shifted mode LEFT20 for IADC_SINGLEFIFOCFG  */
#define IADC_SINGLEFIFOCFG_SHOWID                       (0x1UL << 3)                                        /**< Show ID                                     */
#define _IADC_SINGLEFIFOCFG_SHOWID_SHIFT                3                                                   /**< Shift value for IADC_SHOWID                 */
#define _IADC_SINGLEFIFOCFG_SHOWID_MASK                 0x8UL                                               /**< Bit mask for IADC_SHOWID                    */
#define _IADC_SINGLEFIFOCFG_SHOWID_DEFAULT              0x00000000UL                                        /**< Mode DEFAULT for IADC_SINGLEFIFOCFG         */
#define IADC_SINGLEFIFOCFG_SHOWID_DEFAULT               (_IADC_SINGLEFIFOCFG_SHOWID_DEFAULT << 3)           /**< Shifted mode DEFAULT for IADC_SINGLEFIFOCFG */
#define _IADC_SINGLEFIFOCFG_DVL_SHIFT                   4                                                   /**< Shift value for IADC_DVL                    */
#define _IADC_SINGLEFIFOCFG_DVL_MASK                    0x70UL                                              /**< Bit mask for IADC_DVL                       */
#define _IADC_SINGLEFIFOCFG_DVL_DEFAULT                 0x00000003UL                                        /**< Mode DEFAULT for IADC_SINGLEFIFOCFG         */
#define _IADC_SINGLEFIFOCFG_DVL_VALID1                  0x00000000UL                                        /**< Mode VALID1 for IADC_SINGLEFIFOCFG          */
#define _IADC_SINGLEFIFOCFG_DVL_VALID2                  0x00000001UL                                        /**< Mode VALID2 for IADC_SINGLEFIFOCFG          */
#define _IADC_SINGLEFIFOCFG_DVL_VALID3                  0x00000002UL                                        /**< Mode VALID3 for IADC_SINGLEFIFOCFG          */
#define _IADC_SINGLEFIFOCFG_DVL_VALID4                  0x00000003UL                                        /**< Mode VALID4 for IADC_SINGLEFIFOCFG          */
#define _IADC_SINGLEFIFOCFG_DVL_VALID5                  0x00000004UL                                        /**< Mode VALID5 for IADC_SINGLEFIFOCFG          */
#define _IADC_SINGLEFIFOCFG_DVL_VALID6                  0x00000005UL                                        /**< Mode VALID6 for IADC_SINGLEFIFOCFG          */
#define _IADC_SINGLEFIFOCFG_DVL_VALID7                  0x00000006UL                                        /**< Mode VALID7 for IADC_SINGLEFIFOCFG          */
#define _IADC_SINGLEFIFOCFG_DVL_VALID8                  0x00000007UL                                        /**< Mode VALID8 for IADC_SINGLEFIFOCFG          */
#define IADC_SINGLEFIFOCFG_DVL_DEFAULT                  (_IADC_SINGLEFIFOCFG_DVL_DEFAULT << 4)              /**< Shifted mode DEFAULT for IADC_SINGLEFIFOCFG */
#define IADC_SINGLEFIFOCFG_DVL_VALID1                   (_IADC_SINGLEFIFOCFG_DVL_VALID1 << 4)               /**< Shifted mode VALID1 for IADC_SINGLEFIFOCFG  */
#define IADC_SINGLEFIFOCFG_DVL_VALID2                   (_IADC_SINGLEFIFOCFG_DVL_VALID2 << 4)               /**< Shifted mode VALID2 for IADC_SINGLEFIFOCFG  */
#define IADC_SINGLEFIFOCFG_DVL_VALID3                   (_IADC_SINGLEFIFOCFG_DVL_VALID3 << 4)               /**< Shifted mode VALID3 for IADC_SINGLEFIFOCFG  */
#define IADC_SINGLEFIFOCFG_DVL_VALID4                   (_IADC_SINGLEFIFOCFG_DVL_VALID4 << 4)               /**< Shifted mode VALID4 for IADC_SINGLEFIFOCFG  */
#define IADC_SINGLEFIFOCFG_DVL_VALID5                   (_IADC_SINGLEFIFOCFG_DVL_VALID5 << 4)               /**< Shifted mode VALID5 for IADC_SINGLEFIFOCFG  */
#define IADC_SINGLEFIFOCFG_DVL_VALID6                   (_IADC_SINGLEFIFOCFG_DVL_VALID6 << 4)               /**< Shifted mode VALID6 for IADC_SINGLEFIFOCFG  */
#define IADC_SINGLEFIFOCFG_DVL_VALID7                   (_IADC_SINGLEFIFOCFG_DVL_VALID7 << 4)               /**< Shifted mode VALID7 for IADC_SINGLEFIFOCFG  */
#define IADC_SINGLEFIFOCFG_DVL_VALID8                   (_IADC_SINGLEFIFOCFG_DVL_VALID8 << 4)               /**< Shifted mode VALID8 for IADC_SINGLEFIFOCFG  */
#define IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE              (0x1UL << 8)                                        /**< Single FIFO DMA wakeup.                     */
#define _IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE_SHIFT       8                                                   /**< Shift value for IADC_DMAWUFIFOSINGLE        */
#define _IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE_MASK        0x100UL                                             /**< Bit mask for IADC_DMAWUFIFOSINGLE           */
#define _IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE_DEFAULT     0x00000000UL                                        /**< Mode DEFAULT for IADC_SINGLEFIFOCFG         */
#define _IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE_DISABLED    0x00000000UL                                        /**< Mode DISABLED for IADC_SINGLEFIFOCFG        */
#define _IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE_ENABLED     0x00000001UL                                        /**< Mode ENABLED for IADC_SINGLEFIFOCFG         */
#define IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE_DEFAULT      (_IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE_DEFAULT << 8)  /**< Shifted mode DEFAULT for IADC_SINGLEFIFOCFG */
#define IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE_DISABLED     (_IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE_DISABLED << 8) /**< Shifted mode DISABLED for IADC_SINGLEFIFOCFG*/
#define IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE_ENABLED      (_IADC_SINGLEFIFOCFG_DMAWUFIFOSINGLE_ENABLED << 8)  /**< Shifted mode ENABLED for IADC_SINGLEFIFOCFG */

/* Bit fields for IADC SINGLEFIFODATA */
#define _IADC_SINGLEFIFODATA_RESETVALUE                 0x00000000UL                             /**< Default value for IADC_SINGLEFIFODATA       */
#define _IADC_SINGLEFIFODATA_MASK                       0xFFFFFFFFUL                             /**< Mask for IADC_SINGLEFIFODATA                */
#define _IADC_SINGLEFIFODATA_DATA_SHIFT                 0                                        /**< Shift value for IADC_DATA                   */
#define _IADC_SINGLEFIFODATA_DATA_MASK                  0xFFFFFFFFUL                             /**< Bit mask for IADC_DATA                      */
#define _IADC_SINGLEFIFODATA_DATA_DEFAULT               0x00000000UL                             /**< Mode DEFAULT for IADC_SINGLEFIFODATA        */
#define IADC_SINGLEFIFODATA_DATA_DEFAULT                (_IADC_SINGLEFIFODATA_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for IADC_SINGLEFIFODATA*/

/* Bit fields for IADC SINGLEFIFOSTAT */
#define _IADC_SINGLEFIFOSTAT_RESETVALUE                 0x00000000UL                                    /**< Default value for IADC_SINGLEFIFOSTAT       */
#define _IADC_SINGLEFIFOSTAT_MASK                       0x0000000FUL                                    /**< Mask for IADC_SINGLEFIFOSTAT                */
#define _IADC_SINGLEFIFOSTAT_FIFOREADCNT_SHIFT          0                                               /**< Shift value for IADC_FIFOREADCNT            */
#define _IADC_SINGLEFIFOSTAT_FIFOREADCNT_MASK           0xFUL                                           /**< Bit mask for IADC_FIFOREADCNT               */
#define _IADC_SINGLEFIFOSTAT_FIFOREADCNT_DEFAULT        0x00000000UL                                    /**< Mode DEFAULT for IADC_SINGLEFIFOSTAT        */
#define IADC_SINGLEFIFOSTAT_FIFOREADCNT_DEFAULT         (_IADC_SINGLEFIFOSTAT_FIFOREADCNT_DEFAULT << 0) /**< Shifted mode DEFAULT for IADC_SINGLEFIFOSTAT*/

/* Bit fields for IADC SINGLEDATA */
#define _IADC_SINGLEDATA_RESETVALUE                     0x00000000UL                         /**< Default value for IADC_SINGLEDATA           */
#define _IADC_SINGLEDATA_MASK                           0xFFFFFFFFUL                         /**< Mask for IADC_SINGLEDATA                    */
#define _IADC_SINGLEDATA_DATA_SHIFT                     0                                    /**< Shift value for IADC_DATA                   */
#define _IADC_SINGLEDATA_DATA_MASK                      0xFFFFFFFFUL                         /**< Bit mask for IADC_DATA                      */
#define _IADC_SINGLEDATA_DATA_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for IADC_SINGLEDATA            */
#define IADC_SINGLEDATA_DATA_DEFAULT                    (_IADC_SINGLEDATA_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for IADC_SINGLEDATA    */

/* Bit fields for IADC SCANFIFOCFG */
#define _IADC_SCANFIFOCFG_RESETVALUE                    0x00000030UL                                    /**< Default value for IADC_SCANFIFOCFG          */
#define _IADC_SCANFIFOCFG_MASK                          0x0000017FUL                                    /**< Mask for IADC_SCANFIFOCFG                   */
#define _IADC_SCANFIFOCFG_ALIGNMENT_SHIFT               0                                               /**< Shift value for IADC_ALIGNMENT              */
#define _IADC_SCANFIFOCFG_ALIGNMENT_MASK                0x7UL                                           /**< Bit mask for IADC_ALIGNMENT                 */
#define _IADC_SCANFIFOCFG_ALIGNMENT_DEFAULT             0x00000000UL                                    /**< Mode DEFAULT for IADC_SCANFIFOCFG           */
#define _IADC_SCANFIFOCFG_ALIGNMENT_RIGHT12             0x00000000UL                                    /**< Mode RIGHT12 for IADC_SCANFIFOCFG           */
#define _IADC_SCANFIFOCFG_ALIGNMENT_RIGHT16             0x00000001UL                                    /**< Mode RIGHT16 for IADC_SCANFIFOCFG           */
#define _IADC_SCANFIFOCFG_ALIGNMENT_RIGHT20             0x00000002UL                                    /**< Mode RIGHT20 for IADC_SCANFIFOCFG           */
#define _IADC_SCANFIFOCFG_ALIGNMENT_LEFT12              0x00000003UL                                    /**< Mode LEFT12 for IADC_SCANFIFOCFG            */
#define _IADC_SCANFIFOCFG_ALIGNMENT_LEFT16              0x00000004UL                                    /**< Mode LEFT16 for IADC_SCANFIFOCFG            */
#define _IADC_SCANFIFOCFG_ALIGNMENT_LEFT20              0x00000005UL                                    /**< Mode LEFT20 for IADC_SCANFIFOCFG            */
#define IADC_SCANFIFOCFG_ALIGNMENT_DEFAULT              (_IADC_SCANFIFOCFG_ALIGNMENT_DEFAULT << 0)      /**< Shifted mode DEFAULT for IADC_SCANFIFOCFG   */
#define IADC_SCANFIFOCFG_ALIGNMENT_RIGHT12              (_IADC_SCANFIFOCFG_ALIGNMENT_RIGHT12 << 0)      /**< Shifted mode RIGHT12 for IADC_SCANFIFOCFG   */
#define IADC_SCANFIFOCFG_ALIGNMENT_RIGHT16              (_IADC_SCANFIFOCFG_ALIGNMENT_RIGHT16 << 0)      /**< Shifted mode RIGHT16 for IADC_SCANFIFOCFG   */
#define IADC_SCANFIFOCFG_ALIGNMENT_RIGHT20              (_IADC_SCANFIFOCFG_ALIGNMENT_RIGHT20 << 0)      /**< Shifted mode RIGHT20 for IADC_SCANFIFOCFG   */
#define IADC_SCANFIFOCFG_ALIGNMENT_LEFT12               (_IADC_SCANFIFOCFG_ALIGNMENT_LEFT12 << 0)       /**< Shifted mode LEFT12 for IADC_SCANFIFOCFG    */
#define IADC_SCANFIFOCFG_ALIGNMENT_LEFT16               (_IADC_SCANFIFOCFG_ALIGNMENT_LEFT16 << 0)       /**< Shifted mode LEFT16 for IADC_SCANFIFOCFG    */
#define IADC_SCANFIFOCFG_ALIGNMENT_LEFT20               (_IADC_SCANFIFOCFG_ALIGNMENT_LEFT20 << 0)       /**< Shifted mode LEFT20 for IADC_SCANFIFOCFG    */
#define IADC_SCANFIFOCFG_SHOWID                         (0x1UL << 3)                                    /**< Show ID                                     */
#define _IADC_SCANFIFOCFG_SHOWID_SHIFT                  3                                               /**< Shift value for IADC_SHOWID                 */
#define _IADC_SCANFIFOCFG_SHOWID_MASK                   0x8UL                                           /**< Bit mask for IADC_SHOWID                    */
#define _IADC_SCANFIFOCFG_SHOWID_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for IADC_SCANFIFOCFG           */
#define IADC_SCANFIFOCFG_SHOWID_DEFAULT                 (_IADC_SCANFIFOCFG_SHOWID_DEFAULT << 3)         /**< Shifted mode DEFAULT for IADC_SCANFIFOCFG   */
#define _IADC_SCANFIFOCFG_DVL_SHIFT                     4                                               /**< Shift value for IADC_DVL                    */
#define _IADC_SCANFIFOCFG_DVL_MASK                      0x70UL                                          /**< Bit mask for IADC_DVL                       */
#define _IADC_SCANFIFOCFG_DVL_DEFAULT                   0x00000003UL                                    /**< Mode DEFAULT for IADC_SCANFIFOCFG           */
#define _IADC_SCANFIFOCFG_DVL_VALID1                    0x00000000UL                                    /**< Mode VALID1 for IADC_SCANFIFOCFG            */
#define _IADC_SCANFIFOCFG_DVL_VALID2                    0x00000001UL                                    /**< Mode VALID2 for IADC_SCANFIFOCFG            */
#define _IADC_SCANFIFOCFG_DVL_VALID3                    0x00000002UL                                    /**< Mode VALID3 for IADC_SCANFIFOCFG            */
#define _IADC_SCANFIFOCFG_DVL_VALID4                    0x00000003UL                                    /**< Mode VALID4 for IADC_SCANFIFOCFG            */
#define _IADC_SCANFIFOCFG_DVL_VALID5                    0x00000004UL                                    /**< Mode VALID5 for IADC_SCANFIFOCFG            */
#define _IADC_SCANFIFOCFG_DVL_VALID6                    0x00000005UL                                    /**< Mode VALID6 for IADC_SCANFIFOCFG            */
#define _IADC_SCANFIFOCFG_DVL_VALID7                    0x00000006UL                                    /**< Mode VALID7 for IADC_SCANFIFOCFG            */
#define _IADC_SCANFIFOCFG_DVL_VALID8                    0x00000007UL                                    /**< Mode VALID8 for IADC_SCANFIFOCFG            */
#define IADC_SCANFIFOCFG_DVL_DEFAULT                    (_IADC_SCANFIFOCFG_DVL_DEFAULT << 4)            /**< Shifted mode DEFAULT for IADC_SCANFIFOCFG   */
#define IADC_SCANFIFOCFG_DVL_VALID1                     (_IADC_SCANFIFOCFG_DVL_VALID1 << 4)             /**< Shifted mode VALID1 for IADC_SCANFIFOCFG    */
#define IADC_SCANFIFOCFG_DVL_VALID2                     (_IADC_SCANFIFOCFG_DVL_VALID2 << 4)             /**< Shifted mode VALID2 for IADC_SCANFIFOCFG    */
#define IADC_SCANFIFOCFG_DVL_VALID3                     (_IADC_SCANFIFOCFG_DVL_VALID3 << 4)             /**< Shifted mode VALID3 for IADC_SCANFIFOCFG    */
#define IADC_SCANFIFOCFG_DVL_VALID4                     (_IADC_SCANFIFOCFG_DVL_VALID4 << 4)             /**< Shifted mode VALID4 for IADC_SCANFIFOCFG    */
#define IADC_SCANFIFOCFG_DVL_VALID5                     (_IADC_SCANFIFOCFG_DVL_VALID5 << 4)             /**< Shifted mode VALID5 for IADC_SCANFIFOCFG    */
#define IADC_SCANFIFOCFG_DVL_VALID6                     (_IADC_SCANFIFOCFG_DVL_VALID6 << 4)             /**< Shifted mode VALID6 for IADC_SCANFIFOCFG    */
#define IADC_SCANFIFOCFG_DVL_VALID7                     (_IADC_SCANFIFOCFG_DVL_VALID7 << 4)             /**< Shifted mode VALID7 for IADC_SCANFIFOCFG    */
#define IADC_SCANFIFOCFG_DVL_VALID8                     (_IADC_SCANFIFOCFG_DVL_VALID8 << 4)             /**< Shifted mode VALID8 for IADC_SCANFIFOCFG    */
#define IADC_SCANFIFOCFG_DMAWUFIFOSCAN                  (0x1UL << 8)                                    /**< Scan FIFO DMA Wakeup                        */
#define _IADC_SCANFIFOCFG_DMAWUFIFOSCAN_SHIFT           8                                               /**< Shift value for IADC_DMAWUFIFOSCAN          */
#define _IADC_SCANFIFOCFG_DMAWUFIFOSCAN_MASK            0x100UL                                         /**< Bit mask for IADC_DMAWUFIFOSCAN             */
#define _IADC_SCANFIFOCFG_DMAWUFIFOSCAN_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for IADC_SCANFIFOCFG           */
#define _IADC_SCANFIFOCFG_DMAWUFIFOSCAN_DISABLED        0x00000000UL                                    /**< Mode DISABLED for IADC_SCANFIFOCFG          */
#define _IADC_SCANFIFOCFG_DMAWUFIFOSCAN_ENABLED         0x00000001UL                                    /**< Mode ENABLED for IADC_SCANFIFOCFG           */
#define IADC_SCANFIFOCFG_DMAWUFIFOSCAN_DEFAULT          (_IADC_SCANFIFOCFG_DMAWUFIFOSCAN_DEFAULT << 8)  /**< Shifted mode DEFAULT for IADC_SCANFIFOCFG   */
#define IADC_SCANFIFOCFG_DMAWUFIFOSCAN_DISABLED         (_IADC_SCANFIFOCFG_DMAWUFIFOSCAN_DISABLED << 8) /**< Shifted mode DISABLED for IADC_SCANFIFOCFG  */
#define IADC_SCANFIFOCFG_DMAWUFIFOSCAN_ENABLED          (_IADC_SCANFIFOCFG_DMAWUFIFOSCAN_ENABLED << 8)  /**< Shifted mode ENABLED for IADC_SCANFIFOCFG   */

/* Bit fields for IADC SCANFIFODATA */
#define _IADC_SCANFIFODATA_RESETVALUE                   0x00000000UL                           /**< Default value for IADC_SCANFIFODATA         */
#define _IADC_SCANFIFODATA_MASK                         0xFFFFFFFFUL                           /**< Mask for IADC_SCANFIFODATA                  */
#define _IADC_SCANFIFODATA_DATA_SHIFT                   0                                      /**< Shift value for IADC_DATA                   */
#define _IADC_SCANFIFODATA_DATA_MASK                    0xFFFFFFFFUL                           /**< Bit mask for IADC_DATA                      */
#define _IADC_SCANFIFODATA_DATA_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for IADC_SCANFIFODATA          */
#define IADC_SCANFIFODATA_DATA_DEFAULT                  (_IADC_SCANFIFODATA_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for IADC_SCANFIFODATA  */

/* Bit fields for IADC SCANFIFOSTAT */
#define _IADC_SCANFIFOSTAT_RESETVALUE                   0x00000000UL                                  /**< Default value for IADC_SCANFIFOSTAT         */
#define _IADC_SCANFIFOSTAT_MASK                         0x0000000FUL                                  /**< Mask for IADC_SCANFIFOSTAT                  */
#define _IADC_SCANFIFOSTAT_FIFOREADCNT_SHIFT            0                                             /**< Shift value for IADC_FIFOREADCNT            */
#define _IADC_SCANFIFOSTAT_FIFOREADCNT_MASK             0xFUL                                         /**< Bit mask for IADC_FIFOREADCNT               */
#define _IADC_SCANFIFOSTAT_FIFOREADCNT_DEFAULT          0x00000000UL                                  /**< Mode DEFAULT for IADC_SCANFIFOSTAT          */
#define IADC_SCANFIFOSTAT_FIFOREADCNT_DEFAULT           (_IADC_SCANFIFOSTAT_FIFOREADCNT_DEFAULT << 0) /**< Shifted mode DEFAULT for IADC_SCANFIFOSTAT  */

/* Bit fields for IADC SCANDATA */
#define _IADC_SCANDATA_RESETVALUE                       0x00000000UL                       /**< Default value for IADC_SCANDATA             */
#define _IADC_SCANDATA_MASK                             0xFFFFFFFFUL                       /**< Mask for IADC_SCANDATA                      */
#define _IADC_SCANDATA_DATA_SHIFT                       0                                  /**< Shift value for IADC_DATA                   */
#define _IADC_SCANDATA_DATA_MASK                        0xFFFFFFFFUL                       /**< Bit mask for IADC_DATA                      */
#define _IADC_SCANDATA_DATA_DEFAULT                     0x00000000UL                       /**< Mode DEFAULT for IADC_SCANDATA              */
#define IADC_SCANDATA_DATA_DEFAULT                      (_IADC_SCANDATA_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for IADC_SCANDATA      */

/* Bit fields for IADC SINGLE */
#define _IADC_SINGLE_RESETVALUE                         0x00000000UL                         /**< Default value for IADC_SINGLE               */
#define _IADC_SINGLE_MASK                               0x0003FFFFUL                         /**< Mask for IADC_SINGLE                        */
#define _IADC_SINGLE_PINNEG_SHIFT                       0                                    /**< Shift value for IADC_PINNEG                 */
#define _IADC_SINGLE_PINNEG_MASK                        0xFUL                                /**< Bit mask for IADC_PINNEG                    */
#define _IADC_SINGLE_PINNEG_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for IADC_SINGLE                */
#define IADC_SINGLE_PINNEG_DEFAULT                      (_IADC_SINGLE_PINNEG_DEFAULT << 0)   /**< Shifted mode DEFAULT for IADC_SINGLE        */
#define _IADC_SINGLE_PORTNEG_SHIFT                      4                                    /**< Shift value for IADC_PORTNEG                */
#define _IADC_SINGLE_PORTNEG_MASK                       0xF0UL                               /**< Bit mask for IADC_PORTNEG                   */
#define _IADC_SINGLE_PORTNEG_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for IADC_SINGLE                */
#define _IADC_SINGLE_PORTNEG_GND                        0x00000000UL                         /**< Mode GND for IADC_SINGLE                    */
#define _IADC_SINGLE_PORTNEG_DAC1                       0x00000002UL                         /**< Mode DAC1 for IADC_SINGLE                   */
#define _IADC_SINGLE_PORTNEG_PORTA                      0x00000008UL                         /**< Mode PORTA for IADC_SINGLE                  */
#define _IADC_SINGLE_PORTNEG_PORTB                      0x00000009UL                         /**< Mode PORTB for IADC_SINGLE                  */
#define _IADC_SINGLE_PORTNEG_PORTC                      0x0000000AUL                         /**< Mode PORTC for IADC_SINGLE                  */
#define _IADC_SINGLE_PORTNEG_PORTD                      0x0000000BUL                         /**< Mode PORTD for IADC_SINGLE                  */
#define IADC_SINGLE_PORTNEG_DEFAULT                     (_IADC_SINGLE_PORTNEG_DEFAULT << 4)  /**< Shifted mode DEFAULT for IADC_SINGLE        */
#define IADC_SINGLE_PORTNEG_GND                         (_IADC_SINGLE_PORTNEG_GND << 4)      /**< Shifted mode GND for IADC_SINGLE            */
#define IADC_SINGLE_PORTNEG_DAC1                        (_IADC_SINGLE_PORTNEG_DAC1 << 4)     /**< Shifted mode DAC1 for IADC_SINGLE           */
#define IADC_SINGLE_PORTNEG_PORTA                       (_IADC_SINGLE_PORTNEG_PORTA << 4)    /**< Shifted mode PORTA for IADC_SINGLE          */
#define IADC_SINGLE_PORTNEG_PORTB                       (_IADC_SINGLE_PORTNEG_PORTB << 4)    /**< Shifted mode PORTB for IADC_SINGLE          */
#define IADC_SINGLE_PORTNEG_PORTC                       (_IADC_SINGLE_PORTNEG_PORTC << 4)    /**< Shifted mode PORTC for IADC_SINGLE          */
#define IADC_SINGLE_PORTNEG_PORTD                       (_IADC_SINGLE_PORTNEG_PORTD << 4)    /**< Shifted mode PORTD for IADC_SINGLE          */
#define _IADC_SINGLE_PINPOS_SHIFT                       8                                    /**< Shift value for IADC_PINPOS                 */
#define _IADC_SINGLE_PINPOS_MASK                        0xF00UL                              /**< Bit mask for IADC_PINPOS                    */
#define _IADC_SINGLE_PINPOS_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for IADC_SINGLE                */
#define IADC_SINGLE_PINPOS_DEFAULT                      (_IADC_SINGLE_PINPOS_DEFAULT << 8)   /**< Shifted mode DEFAULT for IADC_SINGLE        */
#define _IADC_SINGLE_PORTPOS_SHIFT                      12                                   /**< Shift value for IADC_PORTPOS                */
#define _IADC_SINGLE_PORTPOS_MASK                       0xF000UL                             /**< Bit mask for IADC_PORTPOS                   */
#define _IADC_SINGLE_PORTPOS_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for IADC_SINGLE                */
#define _IADC_SINGLE_PORTPOS_GND                        0x00000000UL                         /**< Mode GND for IADC_SINGLE                    */
#define _IADC_SINGLE_PORTPOS_SUPPLY                     0x00000001UL                         /**< Mode SUPPLY for IADC_SINGLE                 */
#define _IADC_SINGLE_PORTPOS_DAC0                       0x00000002UL                         /**< Mode DAC0 for IADC_SINGLE                   */
#define _IADC_SINGLE_PORTPOS_PORTA                      0x00000008UL                         /**< Mode PORTA for IADC_SINGLE                  */
#define _IADC_SINGLE_PORTPOS_PORTB                      0x00000009UL                         /**< Mode PORTB for IADC_SINGLE                  */
#define _IADC_SINGLE_PORTPOS_PORTC                      0x0000000AUL                         /**< Mode PORTC for IADC_SINGLE                  */
#define _IADC_SINGLE_PORTPOS_PORTD                      0x0000000BUL                         /**< Mode PORTD for IADC_SINGLE                  */
#define IADC_SINGLE_PORTPOS_DEFAULT                     (_IADC_SINGLE_PORTPOS_DEFAULT << 12) /**< Shifted mode DEFAULT for IADC_SINGLE        */
#define IADC_SINGLE_PORTPOS_GND                         (_IADC_SINGLE_PORTPOS_GND << 12)     /**< Shifted mode GND for IADC_SINGLE            */
#define IADC_SINGLE_PORTPOS_SUPPLY                      (_IADC_SINGLE_PORTPOS_SUPPLY << 12)  /**< Shifted mode SUPPLY for IADC_SINGLE         */
#define IADC_SINGLE_PORTPOS_DAC0                        (_IADC_SINGLE_PORTPOS_DAC0 << 12)    /**< Shifted mode DAC0 for IADC_SINGLE           */
#define IADC_SINGLE_PORTPOS_PORTA                       (_IADC_SINGLE_PORTPOS_PORTA << 12)   /**< Shifted mode PORTA for IADC_SINGLE          */
#define IADC_SINGLE_PORTPOS_PORTB                       (_IADC_SINGLE_PORTPOS_PORTB << 12)   /**< Shifted mode PORTB for IADC_SINGLE          */
#define IADC_SINGLE_PORTPOS_PORTC                       (_IADC_SINGLE_PORTPOS_PORTC << 12)   /**< Shifted mode PORTC for IADC_SINGLE          */
#define IADC_SINGLE_PORTPOS_PORTD                       (_IADC_SINGLE_PORTPOS_PORTD << 12)   /**< Shifted mode PORTD for IADC_SINGLE          */
#define IADC_SINGLE_CFG                                 (0x1UL << 16)                        /**< Configuration Group Select                  */
#define _IADC_SINGLE_CFG_SHIFT                          16                                   /**< Shift value for IADC_CFG                    */
#define _IADC_SINGLE_CFG_MASK                           0x10000UL                            /**< Bit mask for IADC_CFG                       */
#define _IADC_SINGLE_CFG_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for IADC_SINGLE                */
#define _IADC_SINGLE_CFG_CONFIG0                        0x00000000UL                         /**< Mode CONFIG0 for IADC_SINGLE                */
#define _IADC_SINGLE_CFG_CONFIG1                        0x00000001UL                         /**< Mode CONFIG1 for IADC_SINGLE                */
#define IADC_SINGLE_CFG_DEFAULT                         (_IADC_SINGLE_CFG_DEFAULT << 16)     /**< Shifted mode DEFAULT for IADC_SINGLE        */
#define IADC_SINGLE_CFG_CONFIG0                         (_IADC_SINGLE_CFG_CONFIG0 << 16)     /**< Shifted mode CONFIG0 for IADC_SINGLE        */
#define IADC_SINGLE_CFG_CONFIG1                         (_IADC_SINGLE_CFG_CONFIG1 << 16)     /**< Shifted mode CONFIG1 for IADC_SINGLE        */
#define IADC_SINGLE_CMP                                 (0x1UL << 17)                        /**< Comparison Enable                           */
#define _IADC_SINGLE_CMP_SHIFT                          17                                   /**< Shift value for IADC_CMP                    */
#define _IADC_SINGLE_CMP_MASK                           0x20000UL                            /**< Bit mask for IADC_CMP                       */
#define _IADC_SINGLE_CMP_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for IADC_SINGLE                */
#define IADC_SINGLE_CMP_DEFAULT                         (_IADC_SINGLE_CMP_DEFAULT << 17)     /**< Shifted mode DEFAULT for IADC_SINGLE        */

/* Bit fields for IADC SCAN */
#define _IADC_SCAN_RESETVALUE                           0x00000000UL                       /**< Default value for IADC_SCAN                 */
#define _IADC_SCAN_MASK                                 0x0003FFFFUL                       /**< Mask for IADC_SCAN                          */
#define _IADC_SCAN_PINNEG_SHIFT                         0                                  /**< Shift value for IADC_PINNEG                 */
#define _IADC_SCAN_PINNEG_MASK                          0xFUL                              /**< Bit mask for IADC_PINNEG                    */
#define _IADC_SCAN_PINNEG_DEFAULT                       0x00000000UL                       /**< Mode DEFAULT for IADC_SCAN                  */
#define IADC_SCAN_PINNEG_DEFAULT                        (_IADC_SCAN_PINNEG_DEFAULT << 0)   /**< Shifted mode DEFAULT for IADC_SCAN          */
#define _IADC_SCAN_PORTNEG_SHIFT                        4                                  /**< Shift value for IADC_PORTNEG                */
#define _IADC_SCAN_PORTNEG_MASK                         0xF0UL                             /**< Bit mask for IADC_PORTNEG                   */
#define _IADC_SCAN_PORTNEG_DEFAULT                      0x00000000UL                       /**< Mode DEFAULT for IADC_SCAN                  */
#define _IADC_SCAN_PORTNEG_GND                          0x00000000UL                       /**< Mode GND for IADC_SCAN                      */
#define _IADC_SCAN_PORTNEG_DAC1                         0x00000002UL                       /**< Mode DAC1 for IADC_SCAN                     */
#define _IADC_SCAN_PORTNEG_PORTA                        0x00000008UL                       /**< Mode PORTA for IADC_SCAN                    */
#define _IADC_SCAN_PORTNEG_PORTB                        0x00000009UL                       /**< Mode PORTB for IADC_SCAN                    */
#define _IADC_SCAN_PORTNEG_PORTC                        0x0000000AUL                       /**< Mode PORTC for IADC_SCAN                    */
#define _IADC_SCAN_PORTNEG_PORTD                        0x0000000BUL                       /**< Mode PORTD for IADC_SCAN                    */
#define IADC_SCAN_PORTNEG_DEFAULT                       (_IADC_SCAN_PORTNEG_DEFAULT << 4)  /**< Shifted mode DEFAULT for IADC_SCAN          */
#define IADC_SCAN_PORTNEG_GND                           (_IADC_SCAN_PORTNEG_GND << 4)      /**< Shifted mode GND for IADC_SCAN              */
#define IADC_SCAN_PORTNEG_DAC1                          (_IADC_SCAN_PORTNEG_DAC1 << 4)     /**< Shifted mode DAC1 for IADC_SCAN             */
#define IADC_SCAN_PORTNEG_PORTA                         (_IADC_SCAN_PORTNEG_PORTA << 4)    /**< Shifted mode PORTA for IADC_SCAN            */
#define IADC_SCAN_PORTNEG_PORTB                         (_IADC_SCAN_PORTNEG_PORTB << 4)    /**< Shifted mode PORTB for IADC_SCAN            */
#define IADC_SCAN_PORTNEG_PORTC                         (_IADC_SCAN_PORTNEG_PORTC << 4)    /**< Shifted mode PORTC for IADC_SCAN            */
#define IADC_SCAN_PORTNEG_PORTD                         (_IADC_SCAN_PORTNEG_PORTD << 4)    /**< Shifted mode PORTD for IADC_SCAN            */
#define _IADC_SCAN_PINPOS_SHIFT                         8                                  /**< Shift value for IADC_PINPOS                 */
#define _IADC_SCAN_PINPOS_MASK                          0xF00UL                            /**< Bit mask for IADC_PINPOS                    */
#define _IADC_SCAN_PINPOS_DEFAULT                       0x00000000UL                       /**< Mode DEFAULT for IADC_SCAN                  */
#define IADC_SCAN_PINPOS_DEFAULT                        (_IADC_SCAN_PINPOS_DEFAULT << 8)   /**< Shifted mode DEFAULT for IADC_SCAN          */
#define _IADC_SCAN_PORTPOS_SHIFT                        12                                 /**< Shift value for IADC_PORTPOS                */
#define _IADC_SCAN_PORTPOS_MASK                         0xF000UL                           /**< Bit mask for IADC_PORTPOS                   */
#define _IADC_SCAN_PORTPOS_DEFAULT                      0x00000000UL                       /**< Mode DEFAULT for IADC_SCAN                  */
#define _IADC_SCAN_PORTPOS_GND                          0x00000000UL                       /**< Mode GND for IADC_SCAN                      */
#define _IADC_SCAN_PORTPOS_SUPPLY                       0x00000001UL                       /**< Mode SUPPLY for IADC_SCAN                   */
#define _IADC_SCAN_PORTPOS_DAC0                         0x00000002UL                       /**< Mode DAC0 for IADC_SCAN                     */
#define _IADC_SCAN_PORTPOS_PORTA                        0x00000008UL                       /**< Mode PORTA for IADC_SCAN                    */
#define _IADC_SCAN_PORTPOS_PORTB                        0x00000009UL                       /**< Mode PORTB for IADC_SCAN                    */
#define _IADC_SCAN_PORTPOS_PORTC                        0x0000000AUL                       /**< Mode PORTC for IADC_SCAN                    */
#define _IADC_SCAN_PORTPOS_PORTD                        0x0000000BUL                       /**< Mode PORTD for IADC_SCAN                    */
#define IADC_SCAN_PORTPOS_DEFAULT                       (_IADC_SCAN_PORTPOS_DEFAULT << 12) /**< Shifted mode DEFAULT for IADC_SCAN          */
#define IADC_SCAN_PORTPOS_GND                           (_IADC_SCAN_PORTPOS_GND << 12)     /**< Shifted mode GND for IADC_SCAN              */
#define IADC_SCAN_PORTPOS_SUPPLY                        (_IADC_SCAN_PORTPOS_SUPPLY << 12)  /**< Shifted mode SUPPLY for IADC_SCAN           */
#define IADC_SCAN_PORTPOS_DAC0                          (_IADC_SCAN_PORTPOS_DAC0 << 12)    /**< Shifted mode DAC0 for IADC_SCAN             */
#define IADC_SCAN_PORTPOS_PORTA                         (_IADC_SCAN_PORTPOS_PORTA << 12)   /**< Shifted mode PORTA for IADC_SCAN            */
#define IADC_SCAN_PORTPOS_PORTB                         (_IADC_SCAN_PORTPOS_PORTB << 12)   /**< Shifted mode PORTB for IADC_SCAN            */
#define IADC_SCAN_PORTPOS_PORTC                         (_IADC_SCAN_PORTPOS_PORTC << 12)   /**< Shifted mode PORTC for IADC_SCAN            */
#define IADC_SCAN_PORTPOS_PORTD                         (_IADC_SCAN_PORTPOS_PORTD << 12)   /**< Shifted mode PORTD for IADC_SCAN            */
#define IADC_SCAN_CFG                                   (0x1UL << 16)                      /**< Configuration Group Select                  */
#define _IADC_SCAN_CFG_SHIFT                            16                                 /**< Shift value for IADC_CFG                    */
#define _IADC_SCAN_CFG_MASK                             0x10000UL                          /**< Bit mask for IADC_CFG                       */
#define _IADC_SCAN_CFG_DEFAULT                          0x00000000UL                       /**< Mode DEFAULT for IADC_SCAN                  */
#define _IADC_SCAN_CFG_CONFIG0                          0x00000000UL                       /**< Mode CONFIG0 for IADC_SCAN                  */
#define _IADC_SCAN_CFG_CONFIG1                          0x00000001UL                       /**< Mode CONFIG1 for IADC_SCAN                  */
#define IADC_SCAN_CFG_DEFAULT                           (_IADC_SCAN_CFG_DEFAULT << 16)     /**< Shifted mode DEFAULT for IADC_SCAN          */
#define IADC_SCAN_CFG_CONFIG0                           (_IADC_SCAN_CFG_CONFIG0 << 16)     /**< Shifted mode CONFIG0 for IADC_SCAN          */
#define IADC_SCAN_CFG_CONFIG1                           (_IADC_SCAN_CFG_CONFIG1 << 16)     /**< Shifted mode CONFIG1 for IADC_SCAN          */
#define IADC_SCAN_CMP                                   (0x1UL << 17)                      /**< Comparison Enable                           */
#define _IADC_SCAN_CMP_SHIFT                            17                                 /**< Shift value for IADC_CMP                    */
#define _IADC_SCAN_CMP_MASK                             0x20000UL                          /**< Bit mask for IADC_CMP                       */
#define _IADC_SCAN_CMP_DEFAULT                          0x00000000UL                       /**< Mode DEFAULT for IADC_SCAN                  */
#define IADC_SCAN_CMP_DEFAULT                           (_IADC_SCAN_CMP_DEFAULT << 17)     /**< Shifted mode DEFAULT for IADC_SCAN          */

/** @} End of group EFR32ZG23_IADC_BitFields */
/** @} End of group EFR32ZG23_IADC */
/** @} End of group Parts */

#endif /* EFR32ZG23_IADC_H */
#ifdef __cplusplus
}
#endif

