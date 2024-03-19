/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 TIMER register and bit field definitions
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

#ifndef EFR32ZG23_TIMER_H
#define EFR32ZG23_TIMER_H
#define TIMER_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_TIMER TIMER
 * @{
 * @brief EFR32ZG23 TIMER Register Declaration.
 *****************************************************************************/

/** TIMER CC Register Group Declaration. */
typedef struct {
  __IOM uint32_t CFG;                                /**< CC Channel Configuration Register                  */
  __IOM uint32_t CTRL;                               /**< CC Channel Control Register                        */
  __IOM uint32_t OC;                                 /**< OC Channel Value Register                          */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t OCB;                                /**< OC Channel Value Buffer Register                   */
  __IM uint32_t  ICF;                                /**< IC Channel Value Register                          */
  __IM uint32_t  ICOF;                               /**< IC Channel Value Overflow Register                 */
  uint32_t       RESERVED1[1U];                      /**< Reserved for future use                            */
} TIMER_CC_TypeDef;

/** TIMER Register Declaration. */
typedef struct {
  __IM uint32_t    IPVERSION;                   /**< IP version ID                                      */
  __IOM uint32_t   CFG;                         /**< Configuration Register                             */
  __IOM uint32_t   CTRL;                        /**< Control Register                                   */
  __IOM uint32_t   CMD;                         /**< Command Register                                   */
  __IM uint32_t    STATUS;                      /**< Status Register                                    */
  __IOM uint32_t   IF;                          /**< Interrupt Flag Register                            */
  __IOM uint32_t   IEN;                         /**< Interrupt Enable Register                          */
  __IOM uint32_t   TOP;                         /**< Counter Top Value Register                         */
  __IOM uint32_t   TOPB;                        /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t   CNT;                         /**< Counter Value Register                             */
  uint32_t         RESERVED0[1U];               /**< Reserved for future use                            */
  __IOM uint32_t   LOCK;                        /**< TIMER Configuration Lock Register                  */
  __IOM uint32_t   EN;                          /**< module en                                          */
  uint32_t         RESERVED1[11U];              /**< Reserved for future use                            */
  TIMER_CC_TypeDef CC[3U];                      /**< Compare/Capture Channel                            */
  uint32_t         RESERVED2[8U];               /**< Reserved for future use                            */
  __IOM uint32_t   DTCFG;                       /**< DTI Configuration Register                         */
  __IOM uint32_t   DTTIMECFG;                   /**< DTI Time Configuration Register                    */
  __IOM uint32_t   DTFCFG;                      /**< DTI Fault Configuration Register                   */
  __IOM uint32_t   DTCTRL;                      /**< DTI Control Register                               */
  __IOM uint32_t   DTOGEN;                      /**< DTI Output Generation Enable Register              */
  __IM uint32_t    DTFAULT;                     /**< DTI Fault Register                                 */
  __IOM uint32_t   DTFAULTC;                    /**< DTI Fault Clear Register                           */
  __IOM uint32_t   DTLOCK;                      /**< DTI Configuration Lock Register                    */
  uint32_t         RESERVED3[960U];             /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_SET;               /**< IP version ID                                      */
  __IOM uint32_t   CFG_SET;                     /**< Configuration Register                             */
  __IOM uint32_t   CTRL_SET;                    /**< Control Register                                   */
  __IOM uint32_t   CMD_SET;                     /**< Command Register                                   */
  __IM uint32_t    STATUS_SET;                  /**< Status Register                                    */
  __IOM uint32_t   IF_SET;                      /**< Interrupt Flag Register                            */
  __IOM uint32_t   IEN_SET;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   TOP_SET;                     /**< Counter Top Value Register                         */
  __IOM uint32_t   TOPB_SET;                    /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t   CNT_SET;                     /**< Counter Value Register                             */
  uint32_t         RESERVED4[1U];               /**< Reserved for future use                            */
  __IOM uint32_t   LOCK_SET;                    /**< TIMER Configuration Lock Register                  */
  __IOM uint32_t   EN_SET;                      /**< module en                                          */
  uint32_t         RESERVED5[11U];              /**< Reserved for future use                            */
  TIMER_CC_TypeDef CC_SET[3U];                  /**< Compare/Capture Channel                            */
  uint32_t         RESERVED6[8U];               /**< Reserved for future use                            */
  __IOM uint32_t   DTCFG_SET;                   /**< DTI Configuration Register                         */
  __IOM uint32_t   DTTIMECFG_SET;               /**< DTI Time Configuration Register                    */
  __IOM uint32_t   DTFCFG_SET;                  /**< DTI Fault Configuration Register                   */
  __IOM uint32_t   DTCTRL_SET;                  /**< DTI Control Register                               */
  __IOM uint32_t   DTOGEN_SET;                  /**< DTI Output Generation Enable Register              */
  __IM uint32_t    DTFAULT_SET;                 /**< DTI Fault Register                                 */
  __IOM uint32_t   DTFAULTC_SET;                /**< DTI Fault Clear Register                           */
  __IOM uint32_t   DTLOCK_SET;                  /**< DTI Configuration Lock Register                    */
  uint32_t         RESERVED7[960U];             /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_CLR;               /**< IP version ID                                      */
  __IOM uint32_t   CFG_CLR;                     /**< Configuration Register                             */
  __IOM uint32_t   CTRL_CLR;                    /**< Control Register                                   */
  __IOM uint32_t   CMD_CLR;                     /**< Command Register                                   */
  __IM uint32_t    STATUS_CLR;                  /**< Status Register                                    */
  __IOM uint32_t   IF_CLR;                      /**< Interrupt Flag Register                            */
  __IOM uint32_t   IEN_CLR;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   TOP_CLR;                     /**< Counter Top Value Register                         */
  __IOM uint32_t   TOPB_CLR;                    /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t   CNT_CLR;                     /**< Counter Value Register                             */
  uint32_t         RESERVED8[1U];               /**< Reserved for future use                            */
  __IOM uint32_t   LOCK_CLR;                    /**< TIMER Configuration Lock Register                  */
  __IOM uint32_t   EN_CLR;                      /**< module en                                          */
  uint32_t         RESERVED9[11U];              /**< Reserved for future use                            */
  TIMER_CC_TypeDef CC_CLR[3U];                  /**< Compare/Capture Channel                            */
  uint32_t         RESERVED10[8U];              /**< Reserved for future use                            */
  __IOM uint32_t   DTCFG_CLR;                   /**< DTI Configuration Register                         */
  __IOM uint32_t   DTTIMECFG_CLR;               /**< DTI Time Configuration Register                    */
  __IOM uint32_t   DTFCFG_CLR;                  /**< DTI Fault Configuration Register                   */
  __IOM uint32_t   DTCTRL_CLR;                  /**< DTI Control Register                               */
  __IOM uint32_t   DTOGEN_CLR;                  /**< DTI Output Generation Enable Register              */
  __IM uint32_t    DTFAULT_CLR;                 /**< DTI Fault Register                                 */
  __IOM uint32_t   DTFAULTC_CLR;                /**< DTI Fault Clear Register                           */
  __IOM uint32_t   DTLOCK_CLR;                  /**< DTI Configuration Lock Register                    */
  uint32_t         RESERVED11[960U];            /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_TGL;               /**< IP version ID                                      */
  __IOM uint32_t   CFG_TGL;                     /**< Configuration Register                             */
  __IOM uint32_t   CTRL_TGL;                    /**< Control Register                                   */
  __IOM uint32_t   CMD_TGL;                     /**< Command Register                                   */
  __IM uint32_t    STATUS_TGL;                  /**< Status Register                                    */
  __IOM uint32_t   IF_TGL;                      /**< Interrupt Flag Register                            */
  __IOM uint32_t   IEN_TGL;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   TOP_TGL;                     /**< Counter Top Value Register                         */
  __IOM uint32_t   TOPB_TGL;                    /**< Counter Top Value Buffer Register                  */
  __IOM uint32_t   CNT_TGL;                     /**< Counter Value Register                             */
  uint32_t         RESERVED12[1U];              /**< Reserved for future use                            */
  __IOM uint32_t   LOCK_TGL;                    /**< TIMER Configuration Lock Register                  */
  __IOM uint32_t   EN_TGL;                      /**< module en                                          */
  uint32_t         RESERVED13[11U];             /**< Reserved for future use                            */
  TIMER_CC_TypeDef CC_TGL[3U];                  /**< Compare/Capture Channel                            */
  uint32_t         RESERVED14[8U];              /**< Reserved for future use                            */
  __IOM uint32_t   DTCFG_TGL;                   /**< DTI Configuration Register                         */
  __IOM uint32_t   DTTIMECFG_TGL;               /**< DTI Time Configuration Register                    */
  __IOM uint32_t   DTFCFG_TGL;                  /**< DTI Fault Configuration Register                   */
  __IOM uint32_t   DTCTRL_TGL;                  /**< DTI Control Register                               */
  __IOM uint32_t   DTOGEN_TGL;                  /**< DTI Output Generation Enable Register              */
  __IM uint32_t    DTFAULT_TGL;                 /**< DTI Fault Register                                 */
  __IOM uint32_t   DTFAULTC_TGL;                /**< DTI Fault Clear Register                           */
  __IOM uint32_t   DTLOCK_TGL;                  /**< DTI Configuration Lock Register                    */
} TIMER_TypeDef;
/** @} End of group EFR32ZG23_TIMER */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_TIMER
 * @{
 * @defgroup EFR32ZG23_TIMER_BitFields TIMER Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for TIMER IPVERSION */
#define _TIMER_IPVERSION_RESETVALUE                0x00000001UL                              /**< Default value for TIMER_IPVERSION           */
#define _TIMER_IPVERSION_MASK                      0xFFFFFFFFUL                              /**< Mask for TIMER_IPVERSION                    */
#define _TIMER_IPVERSION_IPVERSION_SHIFT           0                                         /**< Shift value for TIMER_IPVERSION             */
#define _TIMER_IPVERSION_IPVERSION_MASK            0xFFFFFFFFUL                              /**< Bit mask for TIMER_IPVERSION                */
#define _TIMER_IPVERSION_IPVERSION_DEFAULT         0x00000001UL                              /**< Mode DEFAULT for TIMER_IPVERSION            */
#define TIMER_IPVERSION_IPVERSION_DEFAULT          (_TIMER_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for TIMER_IPVERSION    */

/* Bit fields for TIMER CFG */
#define _TIMER_CFG_RESETVALUE                      0x00000000UL                              /**< Default value for TIMER_CFG                 */
#define _TIMER_CFG_MASK                            0x0FFF1FFBUL                              /**< Mask for TIMER_CFG                          */
#define _TIMER_CFG_MODE_SHIFT                      0                                         /**< Shift value for TIMER_MODE                  */
#define _TIMER_CFG_MODE_MASK                       0x3UL                                     /**< Bit mask for TIMER_MODE                     */
#define _TIMER_CFG_MODE_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define _TIMER_CFG_MODE_UP                         0x00000000UL                              /**< Mode UP for TIMER_CFG                       */
#define _TIMER_CFG_MODE_DOWN                       0x00000001UL                              /**< Mode DOWN for TIMER_CFG                     */
#define _TIMER_CFG_MODE_UPDOWN                     0x00000002UL                              /**< Mode UPDOWN for TIMER_CFG                   */
#define _TIMER_CFG_MODE_QDEC                       0x00000003UL                              /**< Mode QDEC for TIMER_CFG                     */
#define TIMER_CFG_MODE_DEFAULT                     (_TIMER_CFG_MODE_DEFAULT << 0)            /**< Shifted mode DEFAULT for TIMER_CFG          */
#define TIMER_CFG_MODE_UP                          (_TIMER_CFG_MODE_UP << 0)                 /**< Shifted mode UP for TIMER_CFG               */
#define TIMER_CFG_MODE_DOWN                        (_TIMER_CFG_MODE_DOWN << 0)               /**< Shifted mode DOWN for TIMER_CFG             */
#define TIMER_CFG_MODE_UPDOWN                      (_TIMER_CFG_MODE_UPDOWN << 0)             /**< Shifted mode UPDOWN for TIMER_CFG           */
#define TIMER_CFG_MODE_QDEC                        (_TIMER_CFG_MODE_QDEC << 0)               /**< Shifted mode QDEC for TIMER_CFG             */
#define TIMER_CFG_SYNC                             (0x1UL << 3)                              /**< Timer Start/Stop/Reload Synchronization     */
#define _TIMER_CFG_SYNC_SHIFT                      3                                         /**< Shift value for TIMER_SYNC                  */
#define _TIMER_CFG_SYNC_MASK                       0x8UL                                     /**< Bit mask for TIMER_SYNC                     */
#define _TIMER_CFG_SYNC_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define _TIMER_CFG_SYNC_DISABLE                    0x00000000UL                              /**< Mode DISABLE for TIMER_CFG                  */
#define _TIMER_CFG_SYNC_ENABLE                     0x00000001UL                              /**< Mode ENABLE for TIMER_CFG                   */
#define TIMER_CFG_SYNC_DEFAULT                     (_TIMER_CFG_SYNC_DEFAULT << 3)            /**< Shifted mode DEFAULT for TIMER_CFG          */
#define TIMER_CFG_SYNC_DISABLE                     (_TIMER_CFG_SYNC_DISABLE << 3)            /**< Shifted mode DISABLE for TIMER_CFG          */
#define TIMER_CFG_SYNC_ENABLE                      (_TIMER_CFG_SYNC_ENABLE << 3)             /**< Shifted mode ENABLE for TIMER_CFG           */
#define TIMER_CFG_OSMEN                            (0x1UL << 4)                              /**< One-shot Mode Enable                        */
#define _TIMER_CFG_OSMEN_SHIFT                     4                                         /**< Shift value for TIMER_OSMEN                 */
#define _TIMER_CFG_OSMEN_MASK                      0x10UL                                    /**< Bit mask for TIMER_OSMEN                    */
#define _TIMER_CFG_OSMEN_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define TIMER_CFG_OSMEN_DEFAULT                    (_TIMER_CFG_OSMEN_DEFAULT << 4)           /**< Shifted mode DEFAULT for TIMER_CFG          */
#define TIMER_CFG_QDM                              (0x1UL << 5)                              /**< Quadrature Decoder Mode Selection           */
#define _TIMER_CFG_QDM_SHIFT                       5                                         /**< Shift value for TIMER_QDM                   */
#define _TIMER_CFG_QDM_MASK                        0x20UL                                    /**< Bit mask for TIMER_QDM                      */
#define _TIMER_CFG_QDM_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define _TIMER_CFG_QDM_X2                          0x00000000UL                              /**< Mode X2 for TIMER_CFG                       */
#define _TIMER_CFG_QDM_X4                          0x00000001UL                              /**< Mode X4 for TIMER_CFG                       */
#define TIMER_CFG_QDM_DEFAULT                      (_TIMER_CFG_QDM_DEFAULT << 5)             /**< Shifted mode DEFAULT for TIMER_CFG          */
#define TIMER_CFG_QDM_X2                           (_TIMER_CFG_QDM_X2 << 5)                  /**< Shifted mode X2 for TIMER_CFG               */
#define TIMER_CFG_QDM_X4                           (_TIMER_CFG_QDM_X4 << 5)                  /**< Shifted mode X4 for TIMER_CFG               */
#define TIMER_CFG_DEBUGRUN                         (0x1UL << 6)                              /**< Debug Mode Run Enable                       */
#define _TIMER_CFG_DEBUGRUN_SHIFT                  6                                         /**< Shift value for TIMER_DEBUGRUN              */
#define _TIMER_CFG_DEBUGRUN_MASK                   0x40UL                                    /**< Bit mask for TIMER_DEBUGRUN                 */
#define _TIMER_CFG_DEBUGRUN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define _TIMER_CFG_DEBUGRUN_HALT                   0x00000000UL                              /**< Mode HALT for TIMER_CFG                     */
#define _TIMER_CFG_DEBUGRUN_RUN                    0x00000001UL                              /**< Mode RUN for TIMER_CFG                      */
#define TIMER_CFG_DEBUGRUN_DEFAULT                 (_TIMER_CFG_DEBUGRUN_DEFAULT << 6)        /**< Shifted mode DEFAULT for TIMER_CFG          */
#define TIMER_CFG_DEBUGRUN_HALT                    (_TIMER_CFG_DEBUGRUN_HALT << 6)           /**< Shifted mode HALT for TIMER_CFG             */
#define TIMER_CFG_DEBUGRUN_RUN                     (_TIMER_CFG_DEBUGRUN_RUN << 6)            /**< Shifted mode RUN for TIMER_CFG              */
#define TIMER_CFG_DMACLRACT                        (0x1UL << 7)                              /**< DMA Request Clear on Active                 */
#define _TIMER_CFG_DMACLRACT_SHIFT                 7                                         /**< Shift value for TIMER_DMACLRACT             */
#define _TIMER_CFG_DMACLRACT_MASK                  0x80UL                                    /**< Bit mask for TIMER_DMACLRACT                */
#define _TIMER_CFG_DMACLRACT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define TIMER_CFG_DMACLRACT_DEFAULT                (_TIMER_CFG_DMACLRACT_DEFAULT << 7)       /**< Shifted mode DEFAULT for TIMER_CFG          */
#define _TIMER_CFG_CLKSEL_SHIFT                    8                                         /**< Shift value for TIMER_CLKSEL                */
#define _TIMER_CFG_CLKSEL_MASK                     0x300UL                                   /**< Bit mask for TIMER_CLKSEL                   */
#define _TIMER_CFG_CLKSEL_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define _TIMER_CFG_CLKSEL_PRESCEM01GRPACLK         0x00000000UL                              /**< Mode PRESCEM01GRPACLK for TIMER_CFG         */
#define _TIMER_CFG_CLKSEL_CC1                      0x00000001UL                              /**< Mode CC1 for TIMER_CFG                      */
#define _TIMER_CFG_CLKSEL_TIMEROUF                 0x00000002UL                              /**< Mode TIMEROUF for TIMER_CFG                 */
#define TIMER_CFG_CLKSEL_DEFAULT                   (_TIMER_CFG_CLKSEL_DEFAULT << 8)          /**< Shifted mode DEFAULT for TIMER_CFG          */
#define TIMER_CFG_CLKSEL_PRESCEM01GRPACLK          (_TIMER_CFG_CLKSEL_PRESCEM01GRPACLK << 8) /**< Shifted mode PRESCEM01GRPACLK for TIMER_CFG */
#define TIMER_CFG_CLKSEL_CC1                       (_TIMER_CFG_CLKSEL_CC1 << 8)              /**< Shifted mode CC1 for TIMER_CFG              */
#define TIMER_CFG_CLKSEL_TIMEROUF                  (_TIMER_CFG_CLKSEL_TIMEROUF << 8)         /**< Shifted mode TIMEROUF for TIMER_CFG         */
#define TIMER_CFG_RETIMEEN                         (0x1UL << 10)                             /**< PWM output retimed enable                   */
#define _TIMER_CFG_RETIMEEN_SHIFT                  10                                        /**< Shift value for TIMER_RETIMEEN              */
#define _TIMER_CFG_RETIMEEN_MASK                   0x400UL                                   /**< Bit mask for TIMER_RETIMEEN                 */
#define _TIMER_CFG_RETIMEEN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define _TIMER_CFG_RETIMEEN_DISABLE                0x00000000UL                              /**< Mode DISABLE for TIMER_CFG                  */
#define _TIMER_CFG_RETIMEEN_ENABLE                 0x00000001UL                              /**< Mode ENABLE for TIMER_CFG                   */
#define TIMER_CFG_RETIMEEN_DEFAULT                 (_TIMER_CFG_RETIMEEN_DEFAULT << 10)       /**< Shifted mode DEFAULT for TIMER_CFG          */
#define TIMER_CFG_RETIMEEN_DISABLE                 (_TIMER_CFG_RETIMEEN_DISABLE << 10)       /**< Shifted mode DISABLE for TIMER_CFG          */
#define TIMER_CFG_RETIMEEN_ENABLE                  (_TIMER_CFG_RETIMEEN_ENABLE << 10)        /**< Shifted mode ENABLE for TIMER_CFG           */
#define TIMER_CFG_DISSYNCOUT                       (0x1UL << 11)                             /**< Disable Timer Start/Stop/Reload output      */
#define _TIMER_CFG_DISSYNCOUT_SHIFT                11                                        /**< Shift value for TIMER_DISSYNCOUT            */
#define _TIMER_CFG_DISSYNCOUT_MASK                 0x800UL                                   /**< Bit mask for TIMER_DISSYNCOUT               */
#define _TIMER_CFG_DISSYNCOUT_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define _TIMER_CFG_DISSYNCOUT_EN                   0x00000000UL                              /**< Mode EN for TIMER_CFG                       */
#define _TIMER_CFG_DISSYNCOUT_DIS                  0x00000001UL                              /**< Mode DIS for TIMER_CFG                      */
#define TIMER_CFG_DISSYNCOUT_DEFAULT               (_TIMER_CFG_DISSYNCOUT_DEFAULT << 11)     /**< Shifted mode DEFAULT for TIMER_CFG          */
#define TIMER_CFG_DISSYNCOUT_EN                    (_TIMER_CFG_DISSYNCOUT_EN << 11)          /**< Shifted mode EN for TIMER_CFG               */
#define TIMER_CFG_DISSYNCOUT_DIS                   (_TIMER_CFG_DISSYNCOUT_DIS << 11)         /**< Shifted mode DIS for TIMER_CFG              */
#define TIMER_CFG_RETIMESEL                        (0x1UL << 12)                             /**< PWM output retime select                    */
#define _TIMER_CFG_RETIMESEL_SHIFT                 12                                        /**< Shift value for TIMER_RETIMESEL             */
#define _TIMER_CFG_RETIMESEL_MASK                  0x1000UL                                  /**< Bit mask for TIMER_RETIMESEL                */
#define _TIMER_CFG_RETIMESEL_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define TIMER_CFG_RETIMESEL_DEFAULT                (_TIMER_CFG_RETIMESEL_DEFAULT << 12)      /**< Shifted mode DEFAULT for TIMER_CFG          */
#define TIMER_CFG_ATI                              (0x1UL << 16)                             /**< Always Track Inputs                         */
#define _TIMER_CFG_ATI_SHIFT                       16                                        /**< Shift value for TIMER_ATI                   */
#define _TIMER_CFG_ATI_MASK                        0x10000UL                                 /**< Bit mask for TIMER_ATI                      */
#define _TIMER_CFG_ATI_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define TIMER_CFG_ATI_DEFAULT                      (_TIMER_CFG_ATI_DEFAULT << 16)            /**< Shifted mode DEFAULT for TIMER_CFG          */
#define TIMER_CFG_RSSCOIST                         (0x1UL << 17)                             /**< Reload-Start Sets COIST                     */
#define _TIMER_CFG_RSSCOIST_SHIFT                  17                                        /**< Shift value for TIMER_RSSCOIST              */
#define _TIMER_CFG_RSSCOIST_MASK                   0x20000UL                                 /**< Bit mask for TIMER_RSSCOIST                 */
#define _TIMER_CFG_RSSCOIST_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define TIMER_CFG_RSSCOIST_DEFAULT                 (_TIMER_CFG_RSSCOIST_DEFAULT << 17)       /**< Shifted mode DEFAULT for TIMER_CFG          */
#define _TIMER_CFG_PRESC_SHIFT                     18                                        /**< Shift value for TIMER_PRESC                 */
#define _TIMER_CFG_PRESC_MASK                      0xFFC0000UL                               /**< Bit mask for TIMER_PRESC                    */
#define _TIMER_CFG_PRESC_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for TIMER_CFG                  */
#define _TIMER_CFG_PRESC_DIV1                      0x00000000UL                              /**< Mode DIV1 for TIMER_CFG                     */
#define _TIMER_CFG_PRESC_DIV2                      0x00000001UL                              /**< Mode DIV2 for TIMER_CFG                     */
#define _TIMER_CFG_PRESC_DIV4                      0x00000003UL                              /**< Mode DIV4 for TIMER_CFG                     */
#define _TIMER_CFG_PRESC_DIV8                      0x00000007UL                              /**< Mode DIV8 for TIMER_CFG                     */
#define _TIMER_CFG_PRESC_DIV16                     0x0000000FUL                              /**< Mode DIV16 for TIMER_CFG                    */
#define _TIMER_CFG_PRESC_DIV32                     0x0000001FUL                              /**< Mode DIV32 for TIMER_CFG                    */
#define _TIMER_CFG_PRESC_DIV64                     0x0000003FUL                              /**< Mode DIV64 for TIMER_CFG                    */
#define _TIMER_CFG_PRESC_DIV128                    0x0000007FUL                              /**< Mode DIV128 for TIMER_CFG                   */
#define _TIMER_CFG_PRESC_DIV256                    0x000000FFUL                              /**< Mode DIV256 for TIMER_CFG                   */
#define _TIMER_CFG_PRESC_DIV512                    0x000001FFUL                              /**< Mode DIV512 for TIMER_CFG                   */
#define _TIMER_CFG_PRESC_DIV1024                   0x000003FFUL                              /**< Mode DIV1024 for TIMER_CFG                  */
#define TIMER_CFG_PRESC_DEFAULT                    (_TIMER_CFG_PRESC_DEFAULT << 18)          /**< Shifted mode DEFAULT for TIMER_CFG          */
#define TIMER_CFG_PRESC_DIV1                       (_TIMER_CFG_PRESC_DIV1 << 18)             /**< Shifted mode DIV1 for TIMER_CFG             */
#define TIMER_CFG_PRESC_DIV2                       (_TIMER_CFG_PRESC_DIV2 << 18)             /**< Shifted mode DIV2 for TIMER_CFG             */
#define TIMER_CFG_PRESC_DIV4                       (_TIMER_CFG_PRESC_DIV4 << 18)             /**< Shifted mode DIV4 for TIMER_CFG             */
#define TIMER_CFG_PRESC_DIV8                       (_TIMER_CFG_PRESC_DIV8 << 18)             /**< Shifted mode DIV8 for TIMER_CFG             */
#define TIMER_CFG_PRESC_DIV16                      (_TIMER_CFG_PRESC_DIV16 << 18)            /**< Shifted mode DIV16 for TIMER_CFG            */
#define TIMER_CFG_PRESC_DIV32                      (_TIMER_CFG_PRESC_DIV32 << 18)            /**< Shifted mode DIV32 for TIMER_CFG            */
#define TIMER_CFG_PRESC_DIV64                      (_TIMER_CFG_PRESC_DIV64 << 18)            /**< Shifted mode DIV64 for TIMER_CFG            */
#define TIMER_CFG_PRESC_DIV128                     (_TIMER_CFG_PRESC_DIV128 << 18)           /**< Shifted mode DIV128 for TIMER_CFG           */
#define TIMER_CFG_PRESC_DIV256                     (_TIMER_CFG_PRESC_DIV256 << 18)           /**< Shifted mode DIV256 for TIMER_CFG           */
#define TIMER_CFG_PRESC_DIV512                     (_TIMER_CFG_PRESC_DIV512 << 18)           /**< Shifted mode DIV512 for TIMER_CFG           */
#define TIMER_CFG_PRESC_DIV1024                    (_TIMER_CFG_PRESC_DIV1024 << 18)          /**< Shifted mode DIV1024 for TIMER_CFG          */

/* Bit fields for TIMER CTRL */
#define _TIMER_CTRL_RESETVALUE                     0x00000000UL                         /**< Default value for TIMER_CTRL                */
#define _TIMER_CTRL_MASK                           0x0000001FUL                         /**< Mask for TIMER_CTRL                         */
#define _TIMER_CTRL_RISEA_SHIFT                    0                                    /**< Shift value for TIMER_RISEA                 */
#define _TIMER_CTRL_RISEA_MASK                     0x3UL                                /**< Bit mask for TIMER_RISEA                    */
#define _TIMER_CTRL_RISEA_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for TIMER_CTRL                 */
#define _TIMER_CTRL_RISEA_NONE                     0x00000000UL                         /**< Mode NONE for TIMER_CTRL                    */
#define _TIMER_CTRL_RISEA_START                    0x00000001UL                         /**< Mode START for TIMER_CTRL                   */
#define _TIMER_CTRL_RISEA_STOP                     0x00000002UL                         /**< Mode STOP for TIMER_CTRL                    */
#define _TIMER_CTRL_RISEA_RELOADSTART              0x00000003UL                         /**< Mode RELOADSTART for TIMER_CTRL             */
#define TIMER_CTRL_RISEA_DEFAULT                   (_TIMER_CTRL_RISEA_DEFAULT << 0)     /**< Shifted mode DEFAULT for TIMER_CTRL         */
#define TIMER_CTRL_RISEA_NONE                      (_TIMER_CTRL_RISEA_NONE << 0)        /**< Shifted mode NONE for TIMER_CTRL            */
#define TIMER_CTRL_RISEA_START                     (_TIMER_CTRL_RISEA_START << 0)       /**< Shifted mode START for TIMER_CTRL           */
#define TIMER_CTRL_RISEA_STOP                      (_TIMER_CTRL_RISEA_STOP << 0)        /**< Shifted mode STOP for TIMER_CTRL            */
#define TIMER_CTRL_RISEA_RELOADSTART               (_TIMER_CTRL_RISEA_RELOADSTART << 0) /**< Shifted mode RELOADSTART for TIMER_CTRL     */
#define _TIMER_CTRL_FALLA_SHIFT                    2                                    /**< Shift value for TIMER_FALLA                 */
#define _TIMER_CTRL_FALLA_MASK                     0xCUL                                /**< Bit mask for TIMER_FALLA                    */
#define _TIMER_CTRL_FALLA_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for TIMER_CTRL                 */
#define _TIMER_CTRL_FALLA_NONE                     0x00000000UL                         /**< Mode NONE for TIMER_CTRL                    */
#define _TIMER_CTRL_FALLA_START                    0x00000001UL                         /**< Mode START for TIMER_CTRL                   */
#define _TIMER_CTRL_FALLA_STOP                     0x00000002UL                         /**< Mode STOP for TIMER_CTRL                    */
#define _TIMER_CTRL_FALLA_RELOADSTART              0x00000003UL                         /**< Mode RELOADSTART for TIMER_CTRL             */
#define TIMER_CTRL_FALLA_DEFAULT                   (_TIMER_CTRL_FALLA_DEFAULT << 2)     /**< Shifted mode DEFAULT for TIMER_CTRL         */
#define TIMER_CTRL_FALLA_NONE                      (_TIMER_CTRL_FALLA_NONE << 2)        /**< Shifted mode NONE for TIMER_CTRL            */
#define TIMER_CTRL_FALLA_START                     (_TIMER_CTRL_FALLA_START << 2)       /**< Shifted mode START for TIMER_CTRL           */
#define TIMER_CTRL_FALLA_STOP                      (_TIMER_CTRL_FALLA_STOP << 2)        /**< Shifted mode STOP for TIMER_CTRL            */
#define TIMER_CTRL_FALLA_RELOADSTART               (_TIMER_CTRL_FALLA_RELOADSTART << 2) /**< Shifted mode RELOADSTART for TIMER_CTRL     */
#define TIMER_CTRL_X2CNT                           (0x1UL << 4)                         /**< 2x Count Mode                               */
#define _TIMER_CTRL_X2CNT_SHIFT                    4                                    /**< Shift value for TIMER_X2CNT                 */
#define _TIMER_CTRL_X2CNT_MASK                     0x10UL                               /**< Bit mask for TIMER_X2CNT                    */
#define _TIMER_CTRL_X2CNT_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for TIMER_CTRL                 */
#define TIMER_CTRL_X2CNT_DEFAULT                   (_TIMER_CTRL_X2CNT_DEFAULT << 4)     /**< Shifted mode DEFAULT for TIMER_CTRL         */

/* Bit fields for TIMER CMD */
#define _TIMER_CMD_RESETVALUE                      0x00000000UL                         /**< Default value for TIMER_CMD                 */
#define _TIMER_CMD_MASK                            0x00000003UL                         /**< Mask for TIMER_CMD                          */
#define TIMER_CMD_START                            (0x1UL << 0)                         /**< Start Timer                                 */
#define _TIMER_CMD_START_SHIFT                     0                                    /**< Shift value for TIMER_START                 */
#define _TIMER_CMD_START_MASK                      0x1UL                                /**< Bit mask for TIMER_START                    */
#define _TIMER_CMD_START_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for TIMER_CMD                  */
#define TIMER_CMD_START_DEFAULT                    (_TIMER_CMD_START_DEFAULT << 0)      /**< Shifted mode DEFAULT for TIMER_CMD          */
#define TIMER_CMD_STOP                             (0x1UL << 1)                         /**< Stop Timer                                  */
#define _TIMER_CMD_STOP_SHIFT                      1                                    /**< Shift value for TIMER_STOP                  */
#define _TIMER_CMD_STOP_MASK                       0x2UL                                /**< Bit mask for TIMER_STOP                     */
#define _TIMER_CMD_STOP_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for TIMER_CMD                  */
#define TIMER_CMD_STOP_DEFAULT                     (_TIMER_CMD_STOP_DEFAULT << 1)       /**< Shifted mode DEFAULT for TIMER_CMD          */

/* Bit fields for TIMER STATUS */
#define _TIMER_STATUS_RESETVALUE                   0x00000000UL                                  /**< Default value for TIMER_STATUS              */
#define _TIMER_STATUS_MASK                         0x07070777UL                                  /**< Mask for TIMER_STATUS                       */
#define TIMER_STATUS_RUNNING                       (0x1UL << 0)                                  /**< Running                                     */
#define _TIMER_STATUS_RUNNING_SHIFT                0                                             /**< Shift value for TIMER_RUNNING               */
#define _TIMER_STATUS_RUNNING_MASK                 0x1UL                                         /**< Bit mask for TIMER_RUNNING                  */
#define _TIMER_STATUS_RUNNING_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define TIMER_STATUS_RUNNING_DEFAULT               (_TIMER_STATUS_RUNNING_DEFAULT << 0)          /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_DIR                           (0x1UL << 1)                                  /**< Direction                                   */
#define _TIMER_STATUS_DIR_SHIFT                    1                                             /**< Shift value for TIMER_DIR                   */
#define _TIMER_STATUS_DIR_MASK                     0x2UL                                         /**< Bit mask for TIMER_DIR                      */
#define _TIMER_STATUS_DIR_DEFAULT                  0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define _TIMER_STATUS_DIR_UP                       0x00000000UL                                  /**< Mode UP for TIMER_STATUS                    */
#define _TIMER_STATUS_DIR_DOWN                     0x00000001UL                                  /**< Mode DOWN for TIMER_STATUS                  */
#define TIMER_STATUS_DIR_DEFAULT                   (_TIMER_STATUS_DIR_DEFAULT << 1)              /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_DIR_UP                        (_TIMER_STATUS_DIR_UP << 1)                   /**< Shifted mode UP for TIMER_STATUS            */
#define TIMER_STATUS_DIR_DOWN                      (_TIMER_STATUS_DIR_DOWN << 1)                 /**< Shifted mode DOWN for TIMER_STATUS          */
#define TIMER_STATUS_TOPBV                         (0x1UL << 2)                                  /**< TOP Buffer Valid                            */
#define _TIMER_STATUS_TOPBV_SHIFT                  2                                             /**< Shift value for TIMER_TOPBV                 */
#define _TIMER_STATUS_TOPBV_MASK                   0x4UL                                         /**< Bit mask for TIMER_TOPBV                    */
#define _TIMER_STATUS_TOPBV_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define TIMER_STATUS_TOPBV_DEFAULT                 (_TIMER_STATUS_TOPBV_DEFAULT << 2)            /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_TIMERLOCKSTATUS               (0x1UL << 4)                                  /**< Timer lock status                           */
#define _TIMER_STATUS_TIMERLOCKSTATUS_SHIFT        4                                             /**< Shift value for TIMER_TIMERLOCKSTATUS       */
#define _TIMER_STATUS_TIMERLOCKSTATUS_MASK         0x10UL                                        /**< Bit mask for TIMER_TIMERLOCKSTATUS          */
#define _TIMER_STATUS_TIMERLOCKSTATUS_DEFAULT      0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define _TIMER_STATUS_TIMERLOCKSTATUS_UNLOCKED     0x00000000UL                                  /**< Mode UNLOCKED for TIMER_STATUS              */
#define _TIMER_STATUS_TIMERLOCKSTATUS_LOCKED       0x00000001UL                                  /**< Mode LOCKED for TIMER_STATUS                */
#define TIMER_STATUS_TIMERLOCKSTATUS_DEFAULT       (_TIMER_STATUS_TIMERLOCKSTATUS_DEFAULT << 4)  /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_TIMERLOCKSTATUS_UNLOCKED      (_TIMER_STATUS_TIMERLOCKSTATUS_UNLOCKED << 4) /**< Shifted mode UNLOCKED for TIMER_STATUS      */
#define TIMER_STATUS_TIMERLOCKSTATUS_LOCKED        (_TIMER_STATUS_TIMERLOCKSTATUS_LOCKED << 4)   /**< Shifted mode LOCKED for TIMER_STATUS        */
#define TIMER_STATUS_DTILOCKSTATUS                 (0x1UL << 5)                                  /**< DTI lock status                             */
#define _TIMER_STATUS_DTILOCKSTATUS_SHIFT          5                                             /**< Shift value for TIMER_DTILOCKSTATUS         */
#define _TIMER_STATUS_DTILOCKSTATUS_MASK           0x20UL                                        /**< Bit mask for TIMER_DTILOCKSTATUS            */
#define _TIMER_STATUS_DTILOCKSTATUS_DEFAULT        0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define _TIMER_STATUS_DTILOCKSTATUS_UNLOCKED       0x00000000UL                                  /**< Mode UNLOCKED for TIMER_STATUS              */
#define _TIMER_STATUS_DTILOCKSTATUS_LOCKED         0x00000001UL                                  /**< Mode LOCKED for TIMER_STATUS                */
#define TIMER_STATUS_DTILOCKSTATUS_DEFAULT         (_TIMER_STATUS_DTILOCKSTATUS_DEFAULT << 5)    /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_DTILOCKSTATUS_UNLOCKED        (_TIMER_STATUS_DTILOCKSTATUS_UNLOCKED << 5)   /**< Shifted mode UNLOCKED for TIMER_STATUS      */
#define TIMER_STATUS_DTILOCKSTATUS_LOCKED          (_TIMER_STATUS_DTILOCKSTATUS_LOCKED << 5)     /**< Shifted mode LOCKED for TIMER_STATUS        */
#define TIMER_STATUS_SYNCBUSY                      (0x1UL << 6)                                  /**< Sync Busy                                   */
#define _TIMER_STATUS_SYNCBUSY_SHIFT               6                                             /**< Shift value for TIMER_SYNCBUSY              */
#define _TIMER_STATUS_SYNCBUSY_MASK                0x40UL                                        /**< Bit mask for TIMER_SYNCBUSY                 */
#define _TIMER_STATUS_SYNCBUSY_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define TIMER_STATUS_SYNCBUSY_DEFAULT              (_TIMER_STATUS_SYNCBUSY_DEFAULT << 6)         /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_OCBV0                         (0x1UL << 8)                                  /**< Output Compare Buffer Valid                 */
#define _TIMER_STATUS_OCBV0_SHIFT                  8                                             /**< Shift value for TIMER_OCBV0                 */
#define _TIMER_STATUS_OCBV0_MASK                   0x100UL                                       /**< Bit mask for TIMER_OCBV0                    */
#define _TIMER_STATUS_OCBV0_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define TIMER_STATUS_OCBV0_DEFAULT                 (_TIMER_STATUS_OCBV0_DEFAULT << 8)            /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_OCBV1                         (0x1UL << 9)                                  /**< Output Compare Buffer Valid                 */
#define _TIMER_STATUS_OCBV1_SHIFT                  9                                             /**< Shift value for TIMER_OCBV1                 */
#define _TIMER_STATUS_OCBV1_MASK                   0x200UL                                       /**< Bit mask for TIMER_OCBV1                    */
#define _TIMER_STATUS_OCBV1_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define TIMER_STATUS_OCBV1_DEFAULT                 (_TIMER_STATUS_OCBV1_DEFAULT << 9)            /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_OCBV2                         (0x1UL << 10)                                 /**< Output Compare Buffer Valid                 */
#define _TIMER_STATUS_OCBV2_SHIFT                  10                                            /**< Shift value for TIMER_OCBV2                 */
#define _TIMER_STATUS_OCBV2_MASK                   0x400UL                                       /**< Bit mask for TIMER_OCBV2                    */
#define _TIMER_STATUS_OCBV2_DEFAULT                0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define TIMER_STATUS_OCBV2_DEFAULT                 (_TIMER_STATUS_OCBV2_DEFAULT << 10)           /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_ICFEMPTY0                     (0x1UL << 16)                                 /**< Input capture fifo empty                    */
#define _TIMER_STATUS_ICFEMPTY0_SHIFT              16                                            /**< Shift value for TIMER_ICFEMPTY0             */
#define _TIMER_STATUS_ICFEMPTY0_MASK               0x10000UL                                     /**< Bit mask for TIMER_ICFEMPTY0                */
#define _TIMER_STATUS_ICFEMPTY0_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define TIMER_STATUS_ICFEMPTY0_DEFAULT             (_TIMER_STATUS_ICFEMPTY0_DEFAULT << 16)       /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_ICFEMPTY1                     (0x1UL << 17)                                 /**< Input capture fifo empty                    */
#define _TIMER_STATUS_ICFEMPTY1_SHIFT              17                                            /**< Shift value for TIMER_ICFEMPTY1             */
#define _TIMER_STATUS_ICFEMPTY1_MASK               0x20000UL                                     /**< Bit mask for TIMER_ICFEMPTY1                */
#define _TIMER_STATUS_ICFEMPTY1_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define TIMER_STATUS_ICFEMPTY1_DEFAULT             (_TIMER_STATUS_ICFEMPTY1_DEFAULT << 17)       /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_ICFEMPTY2                     (0x1UL << 18)                                 /**< Input capture fifo empty                    */
#define _TIMER_STATUS_ICFEMPTY2_SHIFT              18                                            /**< Shift value for TIMER_ICFEMPTY2             */
#define _TIMER_STATUS_ICFEMPTY2_MASK               0x40000UL                                     /**< Bit mask for TIMER_ICFEMPTY2                */
#define _TIMER_STATUS_ICFEMPTY2_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define TIMER_STATUS_ICFEMPTY2_DEFAULT             (_TIMER_STATUS_ICFEMPTY2_DEFAULT << 18)       /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_CCPOL0                        (0x1UL << 24)                                 /**< Compare/Capture Polarity                    */
#define _TIMER_STATUS_CCPOL0_SHIFT                 24                                            /**< Shift value for TIMER_CCPOL0                */
#define _TIMER_STATUS_CCPOL0_MASK                  0x1000000UL                                   /**< Bit mask for TIMER_CCPOL0                   */
#define _TIMER_STATUS_CCPOL0_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define _TIMER_STATUS_CCPOL0_LOWRISE               0x00000000UL                                  /**< Mode LOWRISE for TIMER_STATUS               */
#define _TIMER_STATUS_CCPOL0_HIGHFALL              0x00000001UL                                  /**< Mode HIGHFALL for TIMER_STATUS              */
#define TIMER_STATUS_CCPOL0_DEFAULT                (_TIMER_STATUS_CCPOL0_DEFAULT << 24)          /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_CCPOL0_LOWRISE                (_TIMER_STATUS_CCPOL0_LOWRISE << 24)          /**< Shifted mode LOWRISE for TIMER_STATUS       */
#define TIMER_STATUS_CCPOL0_HIGHFALL               (_TIMER_STATUS_CCPOL0_HIGHFALL << 24)         /**< Shifted mode HIGHFALL for TIMER_STATUS      */
#define TIMER_STATUS_CCPOL1                        (0x1UL << 25)                                 /**< Compare/Capture Polarity                    */
#define _TIMER_STATUS_CCPOL1_SHIFT                 25                                            /**< Shift value for TIMER_CCPOL1                */
#define _TIMER_STATUS_CCPOL1_MASK                  0x2000000UL                                   /**< Bit mask for TIMER_CCPOL1                   */
#define _TIMER_STATUS_CCPOL1_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define _TIMER_STATUS_CCPOL1_LOWRISE               0x00000000UL                                  /**< Mode LOWRISE for TIMER_STATUS               */
#define _TIMER_STATUS_CCPOL1_HIGHFALL              0x00000001UL                                  /**< Mode HIGHFALL for TIMER_STATUS              */
#define TIMER_STATUS_CCPOL1_DEFAULT                (_TIMER_STATUS_CCPOL1_DEFAULT << 25)          /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_CCPOL1_LOWRISE                (_TIMER_STATUS_CCPOL1_LOWRISE << 25)          /**< Shifted mode LOWRISE for TIMER_STATUS       */
#define TIMER_STATUS_CCPOL1_HIGHFALL               (_TIMER_STATUS_CCPOL1_HIGHFALL << 25)         /**< Shifted mode HIGHFALL for TIMER_STATUS      */
#define TIMER_STATUS_CCPOL2                        (0x1UL << 26)                                 /**< Compare/Capture Polarity                    */
#define _TIMER_STATUS_CCPOL2_SHIFT                 26                                            /**< Shift value for TIMER_CCPOL2                */
#define _TIMER_STATUS_CCPOL2_MASK                  0x4000000UL                                   /**< Bit mask for TIMER_CCPOL2                   */
#define _TIMER_STATUS_CCPOL2_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for TIMER_STATUS               */
#define _TIMER_STATUS_CCPOL2_LOWRISE               0x00000000UL                                  /**< Mode LOWRISE for TIMER_STATUS               */
#define _TIMER_STATUS_CCPOL2_HIGHFALL              0x00000001UL                                  /**< Mode HIGHFALL for TIMER_STATUS              */
#define TIMER_STATUS_CCPOL2_DEFAULT                (_TIMER_STATUS_CCPOL2_DEFAULT << 26)          /**< Shifted mode DEFAULT for TIMER_STATUS       */
#define TIMER_STATUS_CCPOL2_LOWRISE                (_TIMER_STATUS_CCPOL2_LOWRISE << 26)          /**< Shifted mode LOWRISE for TIMER_STATUS       */
#define TIMER_STATUS_CCPOL2_HIGHFALL               (_TIMER_STATUS_CCPOL2_HIGHFALL << 26)         /**< Shifted mode HIGHFALL for TIMER_STATUS      */

/* Bit fields for TIMER IF */
#define _TIMER_IF_RESETVALUE                       0x00000000UL                         /**< Default value for TIMER_IF                  */
#define _TIMER_IF_MASK                             0x07770077UL                         /**< Mask for TIMER_IF                           */
#define TIMER_IF_OF                                (0x1UL << 0)                         /**< Overflow Interrupt Flag                     */
#define _TIMER_IF_OF_SHIFT                         0                                    /**< Shift value for TIMER_OF                    */
#define _TIMER_IF_OF_MASK                          0x1UL                                /**< Bit mask for TIMER_OF                       */
#define _TIMER_IF_OF_DEFAULT                       0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_OF_DEFAULT                        (_TIMER_IF_OF_DEFAULT << 0)          /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_UF                                (0x1UL << 1)                         /**< Underflow Interrupt Flag                    */
#define _TIMER_IF_UF_SHIFT                         1                                    /**< Shift value for TIMER_UF                    */
#define _TIMER_IF_UF_MASK                          0x2UL                                /**< Bit mask for TIMER_UF                       */
#define _TIMER_IF_UF_DEFAULT                       0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_UF_DEFAULT                        (_TIMER_IF_UF_DEFAULT << 1)          /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_DIRCHG                            (0x1UL << 2)                         /**< Direction Change Detect Interrupt Flag      */
#define _TIMER_IF_DIRCHG_SHIFT                     2                                    /**< Shift value for TIMER_DIRCHG                */
#define _TIMER_IF_DIRCHG_MASK                      0x4UL                                /**< Bit mask for TIMER_DIRCHG                   */
#define _TIMER_IF_DIRCHG_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_DIRCHG_DEFAULT                    (_TIMER_IF_DIRCHG_DEFAULT << 2)      /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_CC0                               (0x1UL << 4)                         /**< Capture Compare Channel 0 Interrupt Flag    */
#define _TIMER_IF_CC0_SHIFT                        4                                    /**< Shift value for TIMER_CC0                   */
#define _TIMER_IF_CC0_MASK                         0x10UL                               /**< Bit mask for TIMER_CC0                      */
#define _TIMER_IF_CC0_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_CC0_DEFAULT                       (_TIMER_IF_CC0_DEFAULT << 4)         /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_CC1                               (0x1UL << 5)                         /**< Capture Compare Channel 1 Interrupt Flag    */
#define _TIMER_IF_CC1_SHIFT                        5                                    /**< Shift value for TIMER_CC1                   */
#define _TIMER_IF_CC1_MASK                         0x20UL                               /**< Bit mask for TIMER_CC1                      */
#define _TIMER_IF_CC1_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_CC1_DEFAULT                       (_TIMER_IF_CC1_DEFAULT << 5)         /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_CC2                               (0x1UL << 6)                         /**< Capture Compare Channel 2 Interrupt Flag    */
#define _TIMER_IF_CC2_SHIFT                        6                                    /**< Shift value for TIMER_CC2                   */
#define _TIMER_IF_CC2_MASK                         0x40UL                               /**< Bit mask for TIMER_CC2                      */
#define _TIMER_IF_CC2_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_CC2_DEFAULT                       (_TIMER_IF_CC2_DEFAULT << 6)         /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_ICFWLFULL0                        (0x1UL << 16)                        /**< Input Capture Watermark Level Full          */
#define _TIMER_IF_ICFWLFULL0_SHIFT                 16                                   /**< Shift value for TIMER_ICFWLFULL0            */
#define _TIMER_IF_ICFWLFULL0_MASK                  0x10000UL                            /**< Bit mask for TIMER_ICFWLFULL0               */
#define _TIMER_IF_ICFWLFULL0_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_ICFWLFULL0_DEFAULT                (_TIMER_IF_ICFWLFULL0_DEFAULT << 16) /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_ICFWLFULL1                        (0x1UL << 17)                        /**< Input Capture Watermark Level Full          */
#define _TIMER_IF_ICFWLFULL1_SHIFT                 17                                   /**< Shift value for TIMER_ICFWLFULL1            */
#define _TIMER_IF_ICFWLFULL1_MASK                  0x20000UL                            /**< Bit mask for TIMER_ICFWLFULL1               */
#define _TIMER_IF_ICFWLFULL1_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_ICFWLFULL1_DEFAULT                (_TIMER_IF_ICFWLFULL1_DEFAULT << 17) /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_ICFWLFULL2                        (0x1UL << 18)                        /**< Input Capture Watermark Level Full          */
#define _TIMER_IF_ICFWLFULL2_SHIFT                 18                                   /**< Shift value for TIMER_ICFWLFULL2            */
#define _TIMER_IF_ICFWLFULL2_MASK                  0x40000UL                            /**< Bit mask for TIMER_ICFWLFULL2               */
#define _TIMER_IF_ICFWLFULL2_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_ICFWLFULL2_DEFAULT                (_TIMER_IF_ICFWLFULL2_DEFAULT << 18) /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_ICFOF0                            (0x1UL << 20)                        /**< Input Capture FIFO overflow                 */
#define _TIMER_IF_ICFOF0_SHIFT                     20                                   /**< Shift value for TIMER_ICFOF0                */
#define _TIMER_IF_ICFOF0_MASK                      0x100000UL                           /**< Bit mask for TIMER_ICFOF0                   */
#define _TIMER_IF_ICFOF0_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_ICFOF0_DEFAULT                    (_TIMER_IF_ICFOF0_DEFAULT << 20)     /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_ICFOF1                            (0x1UL << 21)                        /**< Input Capture FIFO overflow                 */
#define _TIMER_IF_ICFOF1_SHIFT                     21                                   /**< Shift value for TIMER_ICFOF1                */
#define _TIMER_IF_ICFOF1_MASK                      0x200000UL                           /**< Bit mask for TIMER_ICFOF1                   */
#define _TIMER_IF_ICFOF1_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_ICFOF1_DEFAULT                    (_TIMER_IF_ICFOF1_DEFAULT << 21)     /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_ICFOF2                            (0x1UL << 22)                        /**< Input Capture FIFO overflow                 */
#define _TIMER_IF_ICFOF2_SHIFT                     22                                   /**< Shift value for TIMER_ICFOF2                */
#define _TIMER_IF_ICFOF2_MASK                      0x400000UL                           /**< Bit mask for TIMER_ICFOF2                   */
#define _TIMER_IF_ICFOF2_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_ICFOF2_DEFAULT                    (_TIMER_IF_ICFOF2_DEFAULT << 22)     /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_ICFUF0                            (0x1UL << 24)                        /**< Input capture FIFO underflow                */
#define _TIMER_IF_ICFUF0_SHIFT                     24                                   /**< Shift value for TIMER_ICFUF0                */
#define _TIMER_IF_ICFUF0_MASK                      0x1000000UL                          /**< Bit mask for TIMER_ICFUF0                   */
#define _TIMER_IF_ICFUF0_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_ICFUF0_DEFAULT                    (_TIMER_IF_ICFUF0_DEFAULT << 24)     /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_ICFUF1                            (0x1UL << 25)                        /**< Input capture FIFO underflow                */
#define _TIMER_IF_ICFUF1_SHIFT                     25                                   /**< Shift value for TIMER_ICFUF1                */
#define _TIMER_IF_ICFUF1_MASK                      0x2000000UL                          /**< Bit mask for TIMER_ICFUF1                   */
#define _TIMER_IF_ICFUF1_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_ICFUF1_DEFAULT                    (_TIMER_IF_ICFUF1_DEFAULT << 25)     /**< Shifted mode DEFAULT for TIMER_IF           */
#define TIMER_IF_ICFUF2                            (0x1UL << 26)                        /**< Input capture FIFO underflow                */
#define _TIMER_IF_ICFUF2_SHIFT                     26                                   /**< Shift value for TIMER_ICFUF2                */
#define _TIMER_IF_ICFUF2_MASK                      0x4000000UL                          /**< Bit mask for TIMER_ICFUF2                   */
#define _TIMER_IF_ICFUF2_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for TIMER_IF                   */
#define TIMER_IF_ICFUF2_DEFAULT                    (_TIMER_IF_ICFUF2_DEFAULT << 26)     /**< Shifted mode DEFAULT for TIMER_IF           */

/* Bit fields for TIMER IEN */
#define _TIMER_IEN_RESETVALUE                      0x00000000UL                          /**< Default value for TIMER_IEN                 */
#define _TIMER_IEN_MASK                            0x07770077UL                          /**< Mask for TIMER_IEN                          */
#define TIMER_IEN_OF                               (0x1UL << 0)                          /**< Overflow Interrupt Enable                   */
#define _TIMER_IEN_OF_SHIFT                        0                                     /**< Shift value for TIMER_OF                    */
#define _TIMER_IEN_OF_MASK                         0x1UL                                 /**< Bit mask for TIMER_OF                       */
#define _TIMER_IEN_OF_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_OF_DEFAULT                       (_TIMER_IEN_OF_DEFAULT << 0)          /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_UF                               (0x1UL << 1)                          /**< Underflow Interrupt Enable                  */
#define _TIMER_IEN_UF_SHIFT                        1                                     /**< Shift value for TIMER_UF                    */
#define _TIMER_IEN_UF_MASK                         0x2UL                                 /**< Bit mask for TIMER_UF                       */
#define _TIMER_IEN_UF_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_UF_DEFAULT                       (_TIMER_IEN_UF_DEFAULT << 1)          /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_DIRCHG                           (0x1UL << 2)                          /**< Direction Change Detect Interrupt Enable    */
#define _TIMER_IEN_DIRCHG_SHIFT                    2                                     /**< Shift value for TIMER_DIRCHG                */
#define _TIMER_IEN_DIRCHG_MASK                     0x4UL                                 /**< Bit mask for TIMER_DIRCHG                   */
#define _TIMER_IEN_DIRCHG_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_DIRCHG_DEFAULT                   (_TIMER_IEN_DIRCHG_DEFAULT << 2)      /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_CC0                              (0x1UL << 4)                          /**< CC0 Interrupt Enable                        */
#define _TIMER_IEN_CC0_SHIFT                       4                                     /**< Shift value for TIMER_CC0                   */
#define _TIMER_IEN_CC0_MASK                        0x10UL                                /**< Bit mask for TIMER_CC0                      */
#define _TIMER_IEN_CC0_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_CC0_DEFAULT                      (_TIMER_IEN_CC0_DEFAULT << 4)         /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_CC1                              (0x1UL << 5)                          /**< CC1 Interrupt Enable                        */
#define _TIMER_IEN_CC1_SHIFT                       5                                     /**< Shift value for TIMER_CC1                   */
#define _TIMER_IEN_CC1_MASK                        0x20UL                                /**< Bit mask for TIMER_CC1                      */
#define _TIMER_IEN_CC1_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_CC1_DEFAULT                      (_TIMER_IEN_CC1_DEFAULT << 5)         /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_CC2                              (0x1UL << 6)                          /**< CC2 Interrupt Enable                        */
#define _TIMER_IEN_CC2_SHIFT                       6                                     /**< Shift value for TIMER_CC2                   */
#define _TIMER_IEN_CC2_MASK                        0x40UL                                /**< Bit mask for TIMER_CC2                      */
#define _TIMER_IEN_CC2_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_CC2_DEFAULT                      (_TIMER_IEN_CC2_DEFAULT << 6)         /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_ICFWLFULL0                       (0x1UL << 16)                         /**< ICFWLFULL0 Interrupt Enable                 */
#define _TIMER_IEN_ICFWLFULL0_SHIFT                16                                    /**< Shift value for TIMER_ICFWLFULL0            */
#define _TIMER_IEN_ICFWLFULL0_MASK                 0x10000UL                             /**< Bit mask for TIMER_ICFWLFULL0               */
#define _TIMER_IEN_ICFWLFULL0_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_ICFWLFULL0_DEFAULT               (_TIMER_IEN_ICFWLFULL0_DEFAULT << 16) /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_ICFWLFULL1                       (0x1UL << 17)                         /**< ICFWLFULL1 Interrupt Enable                 */
#define _TIMER_IEN_ICFWLFULL1_SHIFT                17                                    /**< Shift value for TIMER_ICFWLFULL1            */
#define _TIMER_IEN_ICFWLFULL1_MASK                 0x20000UL                             /**< Bit mask for TIMER_ICFWLFULL1               */
#define _TIMER_IEN_ICFWLFULL1_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_ICFWLFULL1_DEFAULT               (_TIMER_IEN_ICFWLFULL1_DEFAULT << 17) /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_ICFWLFULL2                       (0x1UL << 18)                         /**< ICFWLFULL2 Interrupt Enable                 */
#define _TIMER_IEN_ICFWLFULL2_SHIFT                18                                    /**< Shift value for TIMER_ICFWLFULL2            */
#define _TIMER_IEN_ICFWLFULL2_MASK                 0x40000UL                             /**< Bit mask for TIMER_ICFWLFULL2               */
#define _TIMER_IEN_ICFWLFULL2_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_ICFWLFULL2_DEFAULT               (_TIMER_IEN_ICFWLFULL2_DEFAULT << 18) /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_ICFOF0                           (0x1UL << 20)                         /**< ICFOF0 Interrupt Enable                     */
#define _TIMER_IEN_ICFOF0_SHIFT                    20                                    /**< Shift value for TIMER_ICFOF0                */
#define _TIMER_IEN_ICFOF0_MASK                     0x100000UL                            /**< Bit mask for TIMER_ICFOF0                   */
#define _TIMER_IEN_ICFOF0_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_ICFOF0_DEFAULT                   (_TIMER_IEN_ICFOF0_DEFAULT << 20)     /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_ICFOF1                           (0x1UL << 21)                         /**< ICFOF1 Interrupt Enable                     */
#define _TIMER_IEN_ICFOF1_SHIFT                    21                                    /**< Shift value for TIMER_ICFOF1                */
#define _TIMER_IEN_ICFOF1_MASK                     0x200000UL                            /**< Bit mask for TIMER_ICFOF1                   */
#define _TIMER_IEN_ICFOF1_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_ICFOF1_DEFAULT                   (_TIMER_IEN_ICFOF1_DEFAULT << 21)     /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_ICFOF2                           (0x1UL << 22)                         /**< ICFOF2 Interrupt Enable                     */
#define _TIMER_IEN_ICFOF2_SHIFT                    22                                    /**< Shift value for TIMER_ICFOF2                */
#define _TIMER_IEN_ICFOF2_MASK                     0x400000UL                            /**< Bit mask for TIMER_ICFOF2                   */
#define _TIMER_IEN_ICFOF2_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_ICFOF2_DEFAULT                   (_TIMER_IEN_ICFOF2_DEFAULT << 22)     /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_ICFUF0                           (0x1UL << 24)                         /**< ICFUF0 Interrupt Enable                     */
#define _TIMER_IEN_ICFUF0_SHIFT                    24                                    /**< Shift value for TIMER_ICFUF0                */
#define _TIMER_IEN_ICFUF0_MASK                     0x1000000UL                           /**< Bit mask for TIMER_ICFUF0                   */
#define _TIMER_IEN_ICFUF0_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_ICFUF0_DEFAULT                   (_TIMER_IEN_ICFUF0_DEFAULT << 24)     /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_ICFUF1                           (0x1UL << 25)                         /**< ICFUF1 Interrupt Enable                     */
#define _TIMER_IEN_ICFUF1_SHIFT                    25                                    /**< Shift value for TIMER_ICFUF1                */
#define _TIMER_IEN_ICFUF1_MASK                     0x2000000UL                           /**< Bit mask for TIMER_ICFUF1                   */
#define _TIMER_IEN_ICFUF1_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_ICFUF1_DEFAULT                   (_TIMER_IEN_ICFUF1_DEFAULT << 25)     /**< Shifted mode DEFAULT for TIMER_IEN          */
#define TIMER_IEN_ICFUF2                           (0x1UL << 26)                         /**< ICFUF2 Interrupt Enable                     */
#define _TIMER_IEN_ICFUF2_SHIFT                    26                                    /**< Shift value for TIMER_ICFUF2                */
#define _TIMER_IEN_ICFUF2_MASK                     0x4000000UL                           /**< Bit mask for TIMER_ICFUF2                   */
#define _TIMER_IEN_ICFUF2_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for TIMER_IEN                  */
#define TIMER_IEN_ICFUF2_DEFAULT                   (_TIMER_IEN_ICFUF2_DEFAULT << 26)     /**< Shifted mode DEFAULT for TIMER_IEN          */

/* Bit fields for TIMER TOP */
#define _TIMER_TOP_RESETVALUE                      0x0000FFFFUL                         /**< Default value for TIMER_TOP                 */
#define _TIMER_TOP_MASK                            0xFFFFFFFFUL                         /**< Mask for TIMER_TOP                          */
#define _TIMER_TOP_TOP_SHIFT                       0                                    /**< Shift value for TIMER_TOP                   */
#define _TIMER_TOP_TOP_MASK                        0xFFFFFFFFUL                         /**< Bit mask for TIMER_TOP                      */
#define _TIMER_TOP_TOP_DEFAULT                     0x0000FFFFUL                         /**< Mode DEFAULT for TIMER_TOP                  */
#define TIMER_TOP_TOP_DEFAULT                      (_TIMER_TOP_TOP_DEFAULT << 0)        /**< Shifted mode DEFAULT for TIMER_TOP          */

/* Bit fields for TIMER TOPB */
#define _TIMER_TOPB_RESETVALUE                     0x00000000UL                         /**< Default value for TIMER_TOPB                */
#define _TIMER_TOPB_MASK                           0xFFFFFFFFUL                         /**< Mask for TIMER_TOPB                         */
#define _TIMER_TOPB_TOPB_SHIFT                     0                                    /**< Shift value for TIMER_TOPB                  */
#define _TIMER_TOPB_TOPB_MASK                      0xFFFFFFFFUL                         /**< Bit mask for TIMER_TOPB                     */
#define _TIMER_TOPB_TOPB_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for TIMER_TOPB                 */
#define TIMER_TOPB_TOPB_DEFAULT                    (_TIMER_TOPB_TOPB_DEFAULT << 0)      /**< Shifted mode DEFAULT for TIMER_TOPB         */

/* Bit fields for TIMER CNT */
#define _TIMER_CNT_RESETVALUE                      0x00000000UL                         /**< Default value for TIMER_CNT                 */
#define _TIMER_CNT_MASK                            0xFFFFFFFFUL                         /**< Mask for TIMER_CNT                          */
#define _TIMER_CNT_CNT_SHIFT                       0                                    /**< Shift value for TIMER_CNT                   */
#define _TIMER_CNT_CNT_MASK                        0xFFFFFFFFUL                         /**< Bit mask for TIMER_CNT                      */
#define _TIMER_CNT_CNT_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for TIMER_CNT                  */
#define TIMER_CNT_CNT_DEFAULT                      (_TIMER_CNT_CNT_DEFAULT << 0)        /**< Shifted mode DEFAULT for TIMER_CNT          */

/* Bit fields for TIMER LOCK */
#define _TIMER_LOCK_RESETVALUE                     0x00000000UL                         /**< Default value for TIMER_LOCK                */
#define _TIMER_LOCK_MASK                           0x0000FFFFUL                         /**< Mask for TIMER_LOCK                         */
#define _TIMER_LOCK_LOCKKEY_SHIFT                  0                                    /**< Shift value for TIMER_LOCKKEY               */
#define _TIMER_LOCK_LOCKKEY_MASK                   0xFFFFUL                             /**< Bit mask for TIMER_LOCKKEY                  */
#define _TIMER_LOCK_LOCKKEY_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for TIMER_LOCK                 */
#define _TIMER_LOCK_LOCKKEY_UNLOCK                 0x0000CE80UL                         /**< Mode UNLOCK for TIMER_LOCK                  */
#define TIMER_LOCK_LOCKKEY_DEFAULT                 (_TIMER_LOCK_LOCKKEY_DEFAULT << 0)   /**< Shifted mode DEFAULT for TIMER_LOCK         */
#define TIMER_LOCK_LOCKKEY_UNLOCK                  (_TIMER_LOCK_LOCKKEY_UNLOCK << 0)    /**< Shifted mode UNLOCK for TIMER_LOCK          */

/* Bit fields for TIMER EN */
#define _TIMER_EN_RESETVALUE                       0x00000000UL                         /**< Default value for TIMER_EN                  */
#define _TIMER_EN_MASK                             0x00000003UL                         /**< Mask for TIMER_EN                           */
#define TIMER_EN_EN                                (0x1UL << 0)                         /**< Timer Module Enable                         */
#define _TIMER_EN_EN_SHIFT                         0                                    /**< Shift value for TIMER_EN                    */
#define _TIMER_EN_EN_MASK                          0x1UL                                /**< Bit mask for TIMER_EN                       */
#define _TIMER_EN_EN_DEFAULT                       0x00000000UL                         /**< Mode DEFAULT for TIMER_EN                   */
#define TIMER_EN_EN_DEFAULT                        (_TIMER_EN_EN_DEFAULT << 0)          /**< Shifted mode DEFAULT for TIMER_EN           */
#define TIMER_EN_DISABLING                         (0x1UL << 1)                         /**< Disablement busy status                     */
#define _TIMER_EN_DISABLING_SHIFT                  1                                    /**< Shift value for TIMER_DISABLING             */
#define _TIMER_EN_DISABLING_MASK                   0x2UL                                /**< Bit mask for TIMER_DISABLING                */
#define _TIMER_EN_DISABLING_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for TIMER_EN                   */
#define TIMER_EN_DISABLING_DEFAULT                 (_TIMER_EN_DISABLING_DEFAULT << 1)   /**< Shifted mode DEFAULT for TIMER_EN           */

/* Bit fields for TIMER CC_CFG */
#define _TIMER_CC_CFG_RESETVALUE                   0x00000000UL                              /**< Default value for TIMER_CC_CFG              */
#define _TIMER_CC_CFG_MASK                         0x003E0013UL                              /**< Mask for TIMER_CC_CFG                       */
#define _TIMER_CC_CFG_MODE_SHIFT                   0                                         /**< Shift value for TIMER_MODE                  */
#define _TIMER_CC_CFG_MODE_MASK                    0x3UL                                     /**< Bit mask for TIMER_MODE                     */
#define _TIMER_CC_CFG_MODE_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for TIMER_CC_CFG               */
#define _TIMER_CC_CFG_MODE_OFF                     0x00000000UL                              /**< Mode OFF for TIMER_CC_CFG                   */
#define _TIMER_CC_CFG_MODE_INPUTCAPTURE            0x00000001UL                              /**< Mode INPUTCAPTURE for TIMER_CC_CFG          */
#define _TIMER_CC_CFG_MODE_OUTPUTCOMPARE           0x00000002UL                              /**< Mode OUTPUTCOMPARE for TIMER_CC_CFG         */
#define _TIMER_CC_CFG_MODE_PWM                     0x00000003UL                              /**< Mode PWM for TIMER_CC_CFG                   */
#define TIMER_CC_CFG_MODE_DEFAULT                  (_TIMER_CC_CFG_MODE_DEFAULT << 0)         /**< Shifted mode DEFAULT for TIMER_CC_CFG       */
#define TIMER_CC_CFG_MODE_OFF                      (_TIMER_CC_CFG_MODE_OFF << 0)             /**< Shifted mode OFF for TIMER_CC_CFG           */
#define TIMER_CC_CFG_MODE_INPUTCAPTURE             (_TIMER_CC_CFG_MODE_INPUTCAPTURE << 0)    /**< Shifted mode INPUTCAPTURE for TIMER_CC_CFG  */
#define TIMER_CC_CFG_MODE_OUTPUTCOMPARE            (_TIMER_CC_CFG_MODE_OUTPUTCOMPARE << 0)   /**< Shifted mode OUTPUTCOMPARE for TIMER_CC_CFG */
#define TIMER_CC_CFG_MODE_PWM                      (_TIMER_CC_CFG_MODE_PWM << 0)             /**< Shifted mode PWM for TIMER_CC_CFG           */
#define TIMER_CC_CFG_COIST                         (0x1UL << 4)                              /**< Compare Output Initial State                */
#define _TIMER_CC_CFG_COIST_SHIFT                  4                                         /**< Shift value for TIMER_COIST                 */
#define _TIMER_CC_CFG_COIST_MASK                   0x10UL                                    /**< Bit mask for TIMER_COIST                    */
#define _TIMER_CC_CFG_COIST_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for TIMER_CC_CFG               */
#define TIMER_CC_CFG_COIST_DEFAULT                 (_TIMER_CC_CFG_COIST_DEFAULT << 4)        /**< Shifted mode DEFAULT for TIMER_CC_CFG       */
#define _TIMER_CC_CFG_INSEL_SHIFT                  17                                        /**< Shift value for TIMER_INSEL                 */
#define _TIMER_CC_CFG_INSEL_MASK                   0x60000UL                                 /**< Bit mask for TIMER_INSEL                    */
#define _TIMER_CC_CFG_INSEL_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for TIMER_CC_CFG               */
#define _TIMER_CC_CFG_INSEL_PIN                    0x00000000UL                              /**< Mode PIN for TIMER_CC_CFG                   */
#define _TIMER_CC_CFG_INSEL_PRSSYNC                0x00000001UL                              /**< Mode PRSSYNC for TIMER_CC_CFG               */
#define _TIMER_CC_CFG_INSEL_PRSASYNCLEVEL          0x00000002UL                              /**< Mode PRSASYNCLEVEL for TIMER_CC_CFG         */
#define _TIMER_CC_CFG_INSEL_PRSASYNCPULSE          0x00000003UL                              /**< Mode PRSASYNCPULSE for TIMER_CC_CFG         */
#define TIMER_CC_CFG_INSEL_DEFAULT                 (_TIMER_CC_CFG_INSEL_DEFAULT << 17)       /**< Shifted mode DEFAULT for TIMER_CC_CFG       */
#define TIMER_CC_CFG_INSEL_PIN                     (_TIMER_CC_CFG_INSEL_PIN << 17)           /**< Shifted mode PIN for TIMER_CC_CFG           */
#define TIMER_CC_CFG_INSEL_PRSSYNC                 (_TIMER_CC_CFG_INSEL_PRSSYNC << 17)       /**< Shifted mode PRSSYNC for TIMER_CC_CFG       */
#define TIMER_CC_CFG_INSEL_PRSASYNCLEVEL           (_TIMER_CC_CFG_INSEL_PRSASYNCLEVEL << 17) /**< Shifted mode PRSASYNCLEVEL for TIMER_CC_CFG */
#define TIMER_CC_CFG_INSEL_PRSASYNCPULSE           (_TIMER_CC_CFG_INSEL_PRSASYNCPULSE << 17) /**< Shifted mode PRSASYNCPULSE for TIMER_CC_CFG */
#define TIMER_CC_CFG_PRSCONF                       (0x1UL << 19)                             /**< PRS Configuration                           */
#define _TIMER_CC_CFG_PRSCONF_SHIFT                19                                        /**< Shift value for TIMER_PRSCONF               */
#define _TIMER_CC_CFG_PRSCONF_MASK                 0x80000UL                                 /**< Bit mask for TIMER_PRSCONF                  */
#define _TIMER_CC_CFG_PRSCONF_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for TIMER_CC_CFG               */
#define _TIMER_CC_CFG_PRSCONF_PULSE                0x00000000UL                              /**< Mode PULSE for TIMER_CC_CFG                 */
#define _TIMER_CC_CFG_PRSCONF_LEVEL                0x00000001UL                              /**< Mode LEVEL for TIMER_CC_CFG                 */
#define TIMER_CC_CFG_PRSCONF_DEFAULT               (_TIMER_CC_CFG_PRSCONF_DEFAULT << 19)     /**< Shifted mode DEFAULT for TIMER_CC_CFG       */
#define TIMER_CC_CFG_PRSCONF_PULSE                 (_TIMER_CC_CFG_PRSCONF_PULSE << 19)       /**< Shifted mode PULSE for TIMER_CC_CFG         */
#define TIMER_CC_CFG_PRSCONF_LEVEL                 (_TIMER_CC_CFG_PRSCONF_LEVEL << 19)       /**< Shifted mode LEVEL for TIMER_CC_CFG         */
#define TIMER_CC_CFG_FILT                          (0x1UL << 20)                             /**< Digital Filter                              */
#define _TIMER_CC_CFG_FILT_SHIFT                   20                                        /**< Shift value for TIMER_FILT                  */
#define _TIMER_CC_CFG_FILT_MASK                    0x100000UL                                /**< Bit mask for TIMER_FILT                     */
#define _TIMER_CC_CFG_FILT_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for TIMER_CC_CFG               */
#define _TIMER_CC_CFG_FILT_DISABLE                 0x00000000UL                              /**< Mode DISABLE for TIMER_CC_CFG               */
#define _TIMER_CC_CFG_FILT_ENABLE                  0x00000001UL                              /**< Mode ENABLE for TIMER_CC_CFG                */
#define TIMER_CC_CFG_FILT_DEFAULT                  (_TIMER_CC_CFG_FILT_DEFAULT << 20)        /**< Shifted mode DEFAULT for TIMER_CC_CFG       */
#define TIMER_CC_CFG_FILT_DISABLE                  (_TIMER_CC_CFG_FILT_DISABLE << 20)        /**< Shifted mode DISABLE for TIMER_CC_CFG       */
#define TIMER_CC_CFG_FILT_ENABLE                   (_TIMER_CC_CFG_FILT_ENABLE << 20)         /**< Shifted mode ENABLE for TIMER_CC_CFG        */
#define TIMER_CC_CFG_ICFWL                         (0x1UL << 21)                             /**< Input Capture FIFO watermark level          */
#define _TIMER_CC_CFG_ICFWL_SHIFT                  21                                        /**< Shift value for TIMER_ICFWL                 */
#define _TIMER_CC_CFG_ICFWL_MASK                   0x200000UL                                /**< Bit mask for TIMER_ICFWL                    */
#define _TIMER_CC_CFG_ICFWL_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for TIMER_CC_CFG               */
#define TIMER_CC_CFG_ICFWL_DEFAULT                 (_TIMER_CC_CFG_ICFWL_DEFAULT << 21)       /**< Shifted mode DEFAULT for TIMER_CC_CFG       */

/* Bit fields for TIMER CC_CTRL */
#define _TIMER_CC_CTRL_RESETVALUE                  0x00000000UL                                    /**< Default value for TIMER_CC_CTRL             */
#define _TIMER_CC_CTRL_MASK                        0x0F003F04UL                                    /**< Mask for TIMER_CC_CTRL                      */
#define TIMER_CC_CTRL_OUTINV                       (0x1UL << 2)                                    /**< Output Invert                               */
#define _TIMER_CC_CTRL_OUTINV_SHIFT                2                                               /**< Shift value for TIMER_OUTINV                */
#define _TIMER_CC_CTRL_OUTINV_MASK                 0x4UL                                           /**< Bit mask for TIMER_OUTINV                   */
#define _TIMER_CC_CTRL_OUTINV_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL              */
#define TIMER_CC_CTRL_OUTINV_DEFAULT               (_TIMER_CC_CTRL_OUTINV_DEFAULT << 2)            /**< Shifted mode DEFAULT for TIMER_CC_CTRL      */
#define _TIMER_CC_CTRL_CMOA_SHIFT                  8                                               /**< Shift value for TIMER_CMOA                  */
#define _TIMER_CC_CTRL_CMOA_MASK                   0x300UL                                         /**< Bit mask for TIMER_CMOA                     */
#define _TIMER_CC_CTRL_CMOA_DEFAULT                0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL              */
#define _TIMER_CC_CTRL_CMOA_NONE                   0x00000000UL                                    /**< Mode NONE for TIMER_CC_CTRL                 */
#define _TIMER_CC_CTRL_CMOA_TOGGLE                 0x00000001UL                                    /**< Mode TOGGLE for TIMER_CC_CTRL               */
#define _TIMER_CC_CTRL_CMOA_CLEAR                  0x00000002UL                                    /**< Mode CLEAR for TIMER_CC_CTRL                */
#define _TIMER_CC_CTRL_CMOA_SET                    0x00000003UL                                    /**< Mode SET for TIMER_CC_CTRL                  */
#define TIMER_CC_CTRL_CMOA_DEFAULT                 (_TIMER_CC_CTRL_CMOA_DEFAULT << 8)              /**< Shifted mode DEFAULT for TIMER_CC_CTRL      */
#define TIMER_CC_CTRL_CMOA_NONE                    (_TIMER_CC_CTRL_CMOA_NONE << 8)                 /**< Shifted mode NONE for TIMER_CC_CTRL         */
#define TIMER_CC_CTRL_CMOA_TOGGLE                  (_TIMER_CC_CTRL_CMOA_TOGGLE << 8)               /**< Shifted mode TOGGLE for TIMER_CC_CTRL       */
#define TIMER_CC_CTRL_CMOA_CLEAR                   (_TIMER_CC_CTRL_CMOA_CLEAR << 8)                /**< Shifted mode CLEAR for TIMER_CC_CTRL        */
#define TIMER_CC_CTRL_CMOA_SET                     (_TIMER_CC_CTRL_CMOA_SET << 8)                  /**< Shifted mode SET for TIMER_CC_CTRL          */
#define _TIMER_CC_CTRL_COFOA_SHIFT                 10                                              /**< Shift value for TIMER_COFOA                 */
#define _TIMER_CC_CTRL_COFOA_MASK                  0xC00UL                                         /**< Bit mask for TIMER_COFOA                    */
#define _TIMER_CC_CTRL_COFOA_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL              */
#define _TIMER_CC_CTRL_COFOA_NONE                  0x00000000UL                                    /**< Mode NONE for TIMER_CC_CTRL                 */
#define _TIMER_CC_CTRL_COFOA_TOGGLE                0x00000001UL                                    /**< Mode TOGGLE for TIMER_CC_CTRL               */
#define _TIMER_CC_CTRL_COFOA_CLEAR                 0x00000002UL                                    /**< Mode CLEAR for TIMER_CC_CTRL                */
#define _TIMER_CC_CTRL_COFOA_SET                   0x00000003UL                                    /**< Mode SET for TIMER_CC_CTRL                  */
#define TIMER_CC_CTRL_COFOA_DEFAULT                (_TIMER_CC_CTRL_COFOA_DEFAULT << 10)            /**< Shifted mode DEFAULT for TIMER_CC_CTRL      */
#define TIMER_CC_CTRL_COFOA_NONE                   (_TIMER_CC_CTRL_COFOA_NONE << 10)               /**< Shifted mode NONE for TIMER_CC_CTRL         */
#define TIMER_CC_CTRL_COFOA_TOGGLE                 (_TIMER_CC_CTRL_COFOA_TOGGLE << 10)             /**< Shifted mode TOGGLE for TIMER_CC_CTRL       */
#define TIMER_CC_CTRL_COFOA_CLEAR                  (_TIMER_CC_CTRL_COFOA_CLEAR << 10)              /**< Shifted mode CLEAR for TIMER_CC_CTRL        */
#define TIMER_CC_CTRL_COFOA_SET                    (_TIMER_CC_CTRL_COFOA_SET << 10)                /**< Shifted mode SET for TIMER_CC_CTRL          */
#define _TIMER_CC_CTRL_CUFOA_SHIFT                 12                                              /**< Shift value for TIMER_CUFOA                 */
#define _TIMER_CC_CTRL_CUFOA_MASK                  0x3000UL                                        /**< Bit mask for TIMER_CUFOA                    */
#define _TIMER_CC_CTRL_CUFOA_DEFAULT               0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL              */
#define _TIMER_CC_CTRL_CUFOA_NONE                  0x00000000UL                                    /**< Mode NONE for TIMER_CC_CTRL                 */
#define _TIMER_CC_CTRL_CUFOA_TOGGLE                0x00000001UL                                    /**< Mode TOGGLE for TIMER_CC_CTRL               */
#define _TIMER_CC_CTRL_CUFOA_CLEAR                 0x00000002UL                                    /**< Mode CLEAR for TIMER_CC_CTRL                */
#define _TIMER_CC_CTRL_CUFOA_SET                   0x00000003UL                                    /**< Mode SET for TIMER_CC_CTRL                  */
#define TIMER_CC_CTRL_CUFOA_DEFAULT                (_TIMER_CC_CTRL_CUFOA_DEFAULT << 12)            /**< Shifted mode DEFAULT for TIMER_CC_CTRL      */
#define TIMER_CC_CTRL_CUFOA_NONE                   (_TIMER_CC_CTRL_CUFOA_NONE << 12)               /**< Shifted mode NONE for TIMER_CC_CTRL         */
#define TIMER_CC_CTRL_CUFOA_TOGGLE                 (_TIMER_CC_CTRL_CUFOA_TOGGLE << 12)             /**< Shifted mode TOGGLE for TIMER_CC_CTRL       */
#define TIMER_CC_CTRL_CUFOA_CLEAR                  (_TIMER_CC_CTRL_CUFOA_CLEAR << 12)              /**< Shifted mode CLEAR for TIMER_CC_CTRL        */
#define TIMER_CC_CTRL_CUFOA_SET                    (_TIMER_CC_CTRL_CUFOA_SET << 12)                /**< Shifted mode SET for TIMER_CC_CTRL          */
#define _TIMER_CC_CTRL_ICEDGE_SHIFT                24                                              /**< Shift value for TIMER_ICEDGE                */
#define _TIMER_CC_CTRL_ICEDGE_MASK                 0x3000000UL                                     /**< Bit mask for TIMER_ICEDGE                   */
#define _TIMER_CC_CTRL_ICEDGE_DEFAULT              0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL              */
#define _TIMER_CC_CTRL_ICEDGE_RISING               0x00000000UL                                    /**< Mode RISING for TIMER_CC_CTRL               */
#define _TIMER_CC_CTRL_ICEDGE_FALLING              0x00000001UL                                    /**< Mode FALLING for TIMER_CC_CTRL              */
#define _TIMER_CC_CTRL_ICEDGE_BOTH                 0x00000002UL                                    /**< Mode BOTH for TIMER_CC_CTRL                 */
#define _TIMER_CC_CTRL_ICEDGE_NONE                 0x00000003UL                                    /**< Mode NONE for TIMER_CC_CTRL                 */
#define TIMER_CC_CTRL_ICEDGE_DEFAULT               (_TIMER_CC_CTRL_ICEDGE_DEFAULT << 24)           /**< Shifted mode DEFAULT for TIMER_CC_CTRL      */
#define TIMER_CC_CTRL_ICEDGE_RISING                (_TIMER_CC_CTRL_ICEDGE_RISING << 24)            /**< Shifted mode RISING for TIMER_CC_CTRL       */
#define TIMER_CC_CTRL_ICEDGE_FALLING               (_TIMER_CC_CTRL_ICEDGE_FALLING << 24)           /**< Shifted mode FALLING for TIMER_CC_CTRL      */
#define TIMER_CC_CTRL_ICEDGE_BOTH                  (_TIMER_CC_CTRL_ICEDGE_BOTH << 24)              /**< Shifted mode BOTH for TIMER_CC_CTRL         */
#define TIMER_CC_CTRL_ICEDGE_NONE                  (_TIMER_CC_CTRL_ICEDGE_NONE << 24)              /**< Shifted mode NONE for TIMER_CC_CTRL         */
#define _TIMER_CC_CTRL_ICEVCTRL_SHIFT              26                                              /**< Shift value for TIMER_ICEVCTRL              */
#define _TIMER_CC_CTRL_ICEVCTRL_MASK               0xC000000UL                                     /**< Bit mask for TIMER_ICEVCTRL                 */
#define _TIMER_CC_CTRL_ICEVCTRL_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for TIMER_CC_CTRL              */
#define _TIMER_CC_CTRL_ICEVCTRL_EVERYEDGE          0x00000000UL                                    /**< Mode EVERYEDGE for TIMER_CC_CTRL            */
#define _TIMER_CC_CTRL_ICEVCTRL_EVERYSECONDEDGE    0x00000001UL                                    /**< Mode EVERYSECONDEDGE for TIMER_CC_CTRL      */
#define _TIMER_CC_CTRL_ICEVCTRL_RISING             0x00000002UL                                    /**< Mode RISING for TIMER_CC_CTRL               */
#define _TIMER_CC_CTRL_ICEVCTRL_FALLING            0x00000003UL                                    /**< Mode FALLING for TIMER_CC_CTRL              */
#define TIMER_CC_CTRL_ICEVCTRL_DEFAULT             (_TIMER_CC_CTRL_ICEVCTRL_DEFAULT << 26)         /**< Shifted mode DEFAULT for TIMER_CC_CTRL      */
#define TIMER_CC_CTRL_ICEVCTRL_EVERYEDGE           (_TIMER_CC_CTRL_ICEVCTRL_EVERYEDGE << 26)       /**< Shifted mode EVERYEDGE for TIMER_CC_CTRL    */
#define TIMER_CC_CTRL_ICEVCTRL_EVERYSECONDEDGE     (_TIMER_CC_CTRL_ICEVCTRL_EVERYSECONDEDGE << 26) /**< Shifted mode EVERYSECONDEDGE for TIMER_CC_CTRL*/
#define TIMER_CC_CTRL_ICEVCTRL_RISING              (_TIMER_CC_CTRL_ICEVCTRL_RISING << 26)          /**< Shifted mode RISING for TIMER_CC_CTRL       */
#define TIMER_CC_CTRL_ICEVCTRL_FALLING             (_TIMER_CC_CTRL_ICEVCTRL_FALLING << 26)         /**< Shifted mode FALLING for TIMER_CC_CTRL      */

/* Bit fields for TIMER CC_OC */
#define _TIMER_CC_OC_RESETVALUE                    0x00000000UL                         /**< Default value for TIMER_CC_OC               */
#define _TIMER_CC_OC_MASK                          0xFFFFFFFFUL                         /**< Mask for TIMER_CC_OC                        */
#define _TIMER_CC_OC_OC_SHIFT                      0                                    /**< Shift value for TIMER_OC                    */
#define _TIMER_CC_OC_OC_MASK                       0xFFFFFFFFUL                         /**< Bit mask for TIMER_OC                       */
#define _TIMER_CC_OC_OC_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for TIMER_CC_OC                */
#define TIMER_CC_OC_OC_DEFAULT                     (_TIMER_CC_OC_OC_DEFAULT << 0)       /**< Shifted mode DEFAULT for TIMER_CC_OC        */

/* Bit fields for TIMER CC_OCB */
#define _TIMER_CC_OCB_RESETVALUE                   0x00000000UL                         /**< Default value for TIMER_CC_OCB              */
#define _TIMER_CC_OCB_MASK                         0xFFFFFFFFUL                         /**< Mask for TIMER_CC_OCB                       */
#define _TIMER_CC_OCB_OCB_SHIFT                    0                                    /**< Shift value for TIMER_OCB                   */
#define _TIMER_CC_OCB_OCB_MASK                     0xFFFFFFFFUL                         /**< Bit mask for TIMER_OCB                      */
#define _TIMER_CC_OCB_OCB_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for TIMER_CC_OCB               */
#define TIMER_CC_OCB_OCB_DEFAULT                   (_TIMER_CC_OCB_OCB_DEFAULT << 0)     /**< Shifted mode DEFAULT for TIMER_CC_OCB       */

/* Bit fields for TIMER CC_ICF */
#define _TIMER_CC_ICF_RESETVALUE                   0x00000000UL                         /**< Default value for TIMER_CC_ICF              */
#define _TIMER_CC_ICF_MASK                         0xFFFFFFFFUL                         /**< Mask for TIMER_CC_ICF                       */
#define _TIMER_CC_ICF_ICF_SHIFT                    0                                    /**< Shift value for TIMER_ICF                   */
#define _TIMER_CC_ICF_ICF_MASK                     0xFFFFFFFFUL                         /**< Bit mask for TIMER_ICF                      */
#define _TIMER_CC_ICF_ICF_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for TIMER_CC_ICF               */
#define TIMER_CC_ICF_ICF_DEFAULT                   (_TIMER_CC_ICF_ICF_DEFAULT << 0)     /**< Shifted mode DEFAULT for TIMER_CC_ICF       */

/* Bit fields for TIMER CC_ICOF */
#define _TIMER_CC_ICOF_RESETVALUE                  0x00000000UL                         /**< Default value for TIMER_CC_ICOF             */
#define _TIMER_CC_ICOF_MASK                        0xFFFFFFFFUL                         /**< Mask for TIMER_CC_ICOF                      */
#define _TIMER_CC_ICOF_ICOF_SHIFT                  0                                    /**< Shift value for TIMER_ICOF                  */
#define _TIMER_CC_ICOF_ICOF_MASK                   0xFFFFFFFFUL                         /**< Bit mask for TIMER_ICOF                     */
#define _TIMER_CC_ICOF_ICOF_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for TIMER_CC_ICOF              */
#define TIMER_CC_ICOF_ICOF_DEFAULT                 (_TIMER_CC_ICOF_ICOF_DEFAULT << 0)   /**< Shifted mode DEFAULT for TIMER_CC_ICOF      */

/* Bit fields for TIMER DTCFG */
#define _TIMER_DTCFG_RESETVALUE                    0x00000000UL                         /**< Default value for TIMER_DTCFG               */
#define _TIMER_DTCFG_MASK                          0x00000E03UL                         /**< Mask for TIMER_DTCFG                        */
#define TIMER_DTCFG_DTEN                           (0x1UL << 0)                         /**< DTI Enable                                  */
#define _TIMER_DTCFG_DTEN_SHIFT                    0                                    /**< Shift value for TIMER_DTEN                  */
#define _TIMER_DTCFG_DTEN_MASK                     0x1UL                                /**< Bit mask for TIMER_DTEN                     */
#define _TIMER_DTCFG_DTEN_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for TIMER_DTCFG                */
#define TIMER_DTCFG_DTEN_DEFAULT                   (_TIMER_DTCFG_DTEN_DEFAULT << 0)     /**< Shifted mode DEFAULT for TIMER_DTCFG        */
#define TIMER_DTCFG_DTDAS                          (0x1UL << 1)                         /**< DTI Automatic Start-up Functionality        */
#define _TIMER_DTCFG_DTDAS_SHIFT                   1                                    /**< Shift value for TIMER_DTDAS                 */
#define _TIMER_DTCFG_DTDAS_MASK                    0x2UL                                /**< Bit mask for TIMER_DTDAS                    */
#define _TIMER_DTCFG_DTDAS_DEFAULT                 0x00000000UL                         /**< Mode DEFAULT for TIMER_DTCFG                */
#define _TIMER_DTCFG_DTDAS_NORESTART               0x00000000UL                         /**< Mode NORESTART for TIMER_DTCFG              */
#define _TIMER_DTCFG_DTDAS_RESTART                 0x00000001UL                         /**< Mode RESTART for TIMER_DTCFG                */
#define TIMER_DTCFG_DTDAS_DEFAULT                  (_TIMER_DTCFG_DTDAS_DEFAULT << 1)    /**< Shifted mode DEFAULT for TIMER_DTCFG        */
#define TIMER_DTCFG_DTDAS_NORESTART                (_TIMER_DTCFG_DTDAS_NORESTART << 1)  /**< Shifted mode NORESTART for TIMER_DTCFG      */
#define TIMER_DTCFG_DTDAS_RESTART                  (_TIMER_DTCFG_DTDAS_RESTART << 1)    /**< Shifted mode RESTART for TIMER_DTCFG        */
#define TIMER_DTCFG_DTAR                           (0x1UL << 9)                         /**< DTI Always Run                              */
#define _TIMER_DTCFG_DTAR_SHIFT                    9                                    /**< Shift value for TIMER_DTAR                  */
#define _TIMER_DTCFG_DTAR_MASK                     0x200UL                              /**< Bit mask for TIMER_DTAR                     */
#define _TIMER_DTCFG_DTAR_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for TIMER_DTCFG                */
#define TIMER_DTCFG_DTAR_DEFAULT                   (_TIMER_DTCFG_DTAR_DEFAULT << 9)     /**< Shifted mode DEFAULT for TIMER_DTCFG        */
#define TIMER_DTCFG_DTFATS                         (0x1UL << 10)                        /**< DTI Fault Action on Timer Stop              */
#define _TIMER_DTCFG_DTFATS_SHIFT                  10                                   /**< Shift value for TIMER_DTFATS                */
#define _TIMER_DTCFG_DTFATS_MASK                   0x400UL                              /**< Bit mask for TIMER_DTFATS                   */
#define _TIMER_DTCFG_DTFATS_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for TIMER_DTCFG                */
#define TIMER_DTCFG_DTFATS_DEFAULT                 (_TIMER_DTCFG_DTFATS_DEFAULT << 10)  /**< Shifted mode DEFAULT for TIMER_DTCFG        */
#define TIMER_DTCFG_DTPRSEN                        (0x1UL << 11)                        /**< DTI PRS Source Enable                       */
#define _TIMER_DTCFG_DTPRSEN_SHIFT                 11                                   /**< Shift value for TIMER_DTPRSEN               */
#define _TIMER_DTCFG_DTPRSEN_MASK                  0x800UL                              /**< Bit mask for TIMER_DTPRSEN                  */
#define _TIMER_DTCFG_DTPRSEN_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for TIMER_DTCFG                */
#define TIMER_DTCFG_DTPRSEN_DEFAULT                (_TIMER_DTCFG_DTPRSEN_DEFAULT << 11) /**< Shifted mode DEFAULT for TIMER_DTCFG        */

/* Bit fields for TIMER DTTIMECFG */
#define _TIMER_DTTIMECFG_RESETVALUE                0x00000000UL                             /**< Default value for TIMER_DTTIMECFG           */
#define _TIMER_DTTIMECFG_MASK                      0x003FFFFFUL                             /**< Mask for TIMER_DTTIMECFG                    */
#define _TIMER_DTTIMECFG_DTPRESC_SHIFT             0                                        /**< Shift value for TIMER_DTPRESC               */
#define _TIMER_DTTIMECFG_DTPRESC_MASK              0x3FFUL                                  /**< Bit mask for TIMER_DTPRESC                  */
#define _TIMER_DTTIMECFG_DTPRESC_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for TIMER_DTTIMECFG            */
#define TIMER_DTTIMECFG_DTPRESC_DEFAULT            (_TIMER_DTTIMECFG_DTPRESC_DEFAULT << 0)  /**< Shifted mode DEFAULT for TIMER_DTTIMECFG    */
#define _TIMER_DTTIMECFG_DTRISET_SHIFT             10                                       /**< Shift value for TIMER_DTRISET               */
#define _TIMER_DTTIMECFG_DTRISET_MASK              0xFC00UL                                 /**< Bit mask for TIMER_DTRISET                  */
#define _TIMER_DTTIMECFG_DTRISET_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for TIMER_DTTIMECFG            */
#define TIMER_DTTIMECFG_DTRISET_DEFAULT            (_TIMER_DTTIMECFG_DTRISET_DEFAULT << 10) /**< Shifted mode DEFAULT for TIMER_DTTIMECFG    */
#define _TIMER_DTTIMECFG_DTFALLT_SHIFT             16                                       /**< Shift value for TIMER_DTFALLT               */
#define _TIMER_DTTIMECFG_DTFALLT_MASK              0x3F0000UL                               /**< Bit mask for TIMER_DTFALLT                  */
#define _TIMER_DTTIMECFG_DTFALLT_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for TIMER_DTTIMECFG            */
#define TIMER_DTTIMECFG_DTFALLT_DEFAULT            (_TIMER_DTTIMECFG_DTFALLT_DEFAULT << 16) /**< Shifted mode DEFAULT for TIMER_DTTIMECFG    */

/* Bit fields for TIMER DTFCFG */
#define _TIMER_DTFCFG_RESETVALUE                   0x00000000UL                              /**< Default value for TIMER_DTFCFG              */
#define _TIMER_DTFCFG_MASK                         0x1F030000UL                              /**< Mask for TIMER_DTFCFG                       */
#define _TIMER_DTFCFG_DTFA_SHIFT                   16                                        /**< Shift value for TIMER_DTFA                  */
#define _TIMER_DTFCFG_DTFA_MASK                    0x30000UL                                 /**< Bit mask for TIMER_DTFA                     */
#define _TIMER_DTFCFG_DTFA_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for TIMER_DTFCFG               */
#define _TIMER_DTFCFG_DTFA_NONE                    0x00000000UL                              /**< Mode NONE for TIMER_DTFCFG                  */
#define _TIMER_DTFCFG_DTFA_INACTIVE                0x00000001UL                              /**< Mode INACTIVE for TIMER_DTFCFG              */
#define _TIMER_DTFCFG_DTFA_CLEAR                   0x00000002UL                              /**< Mode CLEAR for TIMER_DTFCFG                 */
#define _TIMER_DTFCFG_DTFA_TRISTATE                0x00000003UL                              /**< Mode TRISTATE for TIMER_DTFCFG              */
#define TIMER_DTFCFG_DTFA_DEFAULT                  (_TIMER_DTFCFG_DTFA_DEFAULT << 16)        /**< Shifted mode DEFAULT for TIMER_DTFCFG       */
#define TIMER_DTFCFG_DTFA_NONE                     (_TIMER_DTFCFG_DTFA_NONE << 16)           /**< Shifted mode NONE for TIMER_DTFCFG          */
#define TIMER_DTFCFG_DTFA_INACTIVE                 (_TIMER_DTFCFG_DTFA_INACTIVE << 16)       /**< Shifted mode INACTIVE for TIMER_DTFCFG      */
#define TIMER_DTFCFG_DTFA_CLEAR                    (_TIMER_DTFCFG_DTFA_CLEAR << 16)          /**< Shifted mode CLEAR for TIMER_DTFCFG         */
#define TIMER_DTFCFG_DTFA_TRISTATE                 (_TIMER_DTFCFG_DTFA_TRISTATE << 16)       /**< Shifted mode TRISTATE for TIMER_DTFCFG      */
#define TIMER_DTFCFG_DTPRS0FEN                     (0x1UL << 24)                             /**< DTI PRS 0 Fault Enable                      */
#define _TIMER_DTFCFG_DTPRS0FEN_SHIFT              24                                        /**< Shift value for TIMER_DTPRS0FEN             */
#define _TIMER_DTFCFG_DTPRS0FEN_MASK               0x1000000UL                               /**< Bit mask for TIMER_DTPRS0FEN                */
#define _TIMER_DTFCFG_DTPRS0FEN_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for TIMER_DTFCFG               */
#define TIMER_DTFCFG_DTPRS0FEN_DEFAULT             (_TIMER_DTFCFG_DTPRS0FEN_DEFAULT << 24)   /**< Shifted mode DEFAULT for TIMER_DTFCFG       */
#define TIMER_DTFCFG_DTPRS1FEN                     (0x1UL << 25)                             /**< DTI PRS 1 Fault Enable                      */
#define _TIMER_DTFCFG_DTPRS1FEN_SHIFT              25                                        /**< Shift value for TIMER_DTPRS1FEN             */
#define _TIMER_DTFCFG_DTPRS1FEN_MASK               0x2000000UL                               /**< Bit mask for TIMER_DTPRS1FEN                */
#define _TIMER_DTFCFG_DTPRS1FEN_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for TIMER_DTFCFG               */
#define TIMER_DTFCFG_DTPRS1FEN_DEFAULT             (_TIMER_DTFCFG_DTPRS1FEN_DEFAULT << 25)   /**< Shifted mode DEFAULT for TIMER_DTFCFG       */
#define TIMER_DTFCFG_DTDBGFEN                      (0x1UL << 26)                             /**< DTI Debugger Fault Enable                   */
#define _TIMER_DTFCFG_DTDBGFEN_SHIFT               26                                        /**< Shift value for TIMER_DTDBGFEN              */
#define _TIMER_DTFCFG_DTDBGFEN_MASK                0x4000000UL                               /**< Bit mask for TIMER_DTDBGFEN                 */
#define _TIMER_DTFCFG_DTDBGFEN_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for TIMER_DTFCFG               */
#define TIMER_DTFCFG_DTDBGFEN_DEFAULT              (_TIMER_DTFCFG_DTDBGFEN_DEFAULT << 26)    /**< Shifted mode DEFAULT for TIMER_DTFCFG       */
#define TIMER_DTFCFG_DTLOCKUPFEN                   (0x1UL << 27)                             /**< DTI Lockup Fault Enable                     */
#define _TIMER_DTFCFG_DTLOCKUPFEN_SHIFT            27                                        /**< Shift value for TIMER_DTLOCKUPFEN           */
#define _TIMER_DTFCFG_DTLOCKUPFEN_MASK             0x8000000UL                               /**< Bit mask for TIMER_DTLOCKUPFEN              */
#define _TIMER_DTFCFG_DTLOCKUPFEN_DEFAULT          0x00000000UL                              /**< Mode DEFAULT for TIMER_DTFCFG               */
#define TIMER_DTFCFG_DTLOCKUPFEN_DEFAULT           (_TIMER_DTFCFG_DTLOCKUPFEN_DEFAULT << 27) /**< Shifted mode DEFAULT for TIMER_DTFCFG       */
#define TIMER_DTFCFG_DTEM23FEN                     (0x1UL << 28)                             /**< DTI EM23 Fault Enable                       */
#define _TIMER_DTFCFG_DTEM23FEN_SHIFT              28                                        /**< Shift value for TIMER_DTEM23FEN             */
#define _TIMER_DTFCFG_DTEM23FEN_MASK               0x10000000UL                              /**< Bit mask for TIMER_DTEM23FEN                */
#define _TIMER_DTFCFG_DTEM23FEN_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for TIMER_DTFCFG               */
#define TIMER_DTFCFG_DTEM23FEN_DEFAULT             (_TIMER_DTFCFG_DTEM23FEN_DEFAULT << 28)   /**< Shifted mode DEFAULT for TIMER_DTFCFG       */

/* Bit fields for TIMER DTCTRL */
#define _TIMER_DTCTRL_RESETVALUE                   0x00000000UL                         /**< Default value for TIMER_DTCTRL              */
#define _TIMER_DTCTRL_MASK                         0x00000003UL                         /**< Mask for TIMER_DTCTRL                       */
#define TIMER_DTCTRL_DTCINV                        (0x1UL << 0)                         /**< DTI Complementary Output Invert.            */
#define _TIMER_DTCTRL_DTCINV_SHIFT                 0                                    /**< Shift value for TIMER_DTCINV                */
#define _TIMER_DTCTRL_DTCINV_MASK                  0x1UL                                /**< Bit mask for TIMER_DTCINV                   */
#define _TIMER_DTCTRL_DTCINV_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for TIMER_DTCTRL               */
#define TIMER_DTCTRL_DTCINV_DEFAULT                (_TIMER_DTCTRL_DTCINV_DEFAULT << 0)  /**< Shifted mode DEFAULT for TIMER_DTCTRL       */
#define TIMER_DTCTRL_DTIPOL                        (0x1UL << 1)                         /**< DTI Inactive Polarity                       */
#define _TIMER_DTCTRL_DTIPOL_SHIFT                 1                                    /**< Shift value for TIMER_DTIPOL                */
#define _TIMER_DTCTRL_DTIPOL_MASK                  0x2UL                                /**< Bit mask for TIMER_DTIPOL                   */
#define _TIMER_DTCTRL_DTIPOL_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for TIMER_DTCTRL               */
#define TIMER_DTCTRL_DTIPOL_DEFAULT                (_TIMER_DTCTRL_DTIPOL_DEFAULT << 1)  /**< Shifted mode DEFAULT for TIMER_DTCTRL       */

/* Bit fields for TIMER DTOGEN */
#define _TIMER_DTOGEN_RESETVALUE                   0x00000000UL                             /**< Default value for TIMER_DTOGEN              */
#define _TIMER_DTOGEN_MASK                         0x0000003FUL                             /**< Mask for TIMER_DTOGEN                       */
#define TIMER_DTOGEN_DTOGCC0EN                     (0x1UL << 0)                             /**< DTI CCn Output Generation Enable            */
#define _TIMER_DTOGEN_DTOGCC0EN_SHIFT              0                                        /**< Shift value for TIMER_DTOGCC0EN             */
#define _TIMER_DTOGEN_DTOGCC0EN_MASK               0x1UL                                    /**< Bit mask for TIMER_DTOGCC0EN                */
#define _TIMER_DTOGEN_DTOGCC0EN_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for TIMER_DTOGEN               */
#define TIMER_DTOGEN_DTOGCC0EN_DEFAULT             (_TIMER_DTOGEN_DTOGCC0EN_DEFAULT << 0)   /**< Shifted mode DEFAULT for TIMER_DTOGEN       */
#define TIMER_DTOGEN_DTOGCC1EN                     (0x1UL << 1)                             /**< DTI CCn Output Generation Enable            */
#define _TIMER_DTOGEN_DTOGCC1EN_SHIFT              1                                        /**< Shift value for TIMER_DTOGCC1EN             */
#define _TIMER_DTOGEN_DTOGCC1EN_MASK               0x2UL                                    /**< Bit mask for TIMER_DTOGCC1EN                */
#define _TIMER_DTOGEN_DTOGCC1EN_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for TIMER_DTOGEN               */
#define TIMER_DTOGEN_DTOGCC1EN_DEFAULT             (_TIMER_DTOGEN_DTOGCC1EN_DEFAULT << 1)   /**< Shifted mode DEFAULT for TIMER_DTOGEN       */
#define TIMER_DTOGEN_DTOGCC2EN                     (0x1UL << 2)                             /**< DTI CCn Output Generation Enable            */
#define _TIMER_DTOGEN_DTOGCC2EN_SHIFT              2                                        /**< Shift value for TIMER_DTOGCC2EN             */
#define _TIMER_DTOGEN_DTOGCC2EN_MASK               0x4UL                                    /**< Bit mask for TIMER_DTOGCC2EN                */
#define _TIMER_DTOGEN_DTOGCC2EN_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for TIMER_DTOGEN               */
#define TIMER_DTOGEN_DTOGCC2EN_DEFAULT             (_TIMER_DTOGEN_DTOGCC2EN_DEFAULT << 2)   /**< Shifted mode DEFAULT for TIMER_DTOGEN       */
#define TIMER_DTOGEN_DTOGCDTI0EN                   (0x1UL << 3)                             /**< DTI CDTIn Output Generation Enable          */
#define _TIMER_DTOGEN_DTOGCDTI0EN_SHIFT            3                                        /**< Shift value for TIMER_DTOGCDTI0EN           */
#define _TIMER_DTOGEN_DTOGCDTI0EN_MASK             0x8UL                                    /**< Bit mask for TIMER_DTOGCDTI0EN              */
#define _TIMER_DTOGEN_DTOGCDTI0EN_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for TIMER_DTOGEN               */
#define TIMER_DTOGEN_DTOGCDTI0EN_DEFAULT           (_TIMER_DTOGEN_DTOGCDTI0EN_DEFAULT << 3) /**< Shifted mode DEFAULT for TIMER_DTOGEN       */
#define TIMER_DTOGEN_DTOGCDTI1EN                   (0x1UL << 4)                             /**< DTI CDTIn Output Generation Enable          */
#define _TIMER_DTOGEN_DTOGCDTI1EN_SHIFT            4                                        /**< Shift value for TIMER_DTOGCDTI1EN           */
#define _TIMER_DTOGEN_DTOGCDTI1EN_MASK             0x10UL                                   /**< Bit mask for TIMER_DTOGCDTI1EN              */
#define _TIMER_DTOGEN_DTOGCDTI1EN_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for TIMER_DTOGEN               */
#define TIMER_DTOGEN_DTOGCDTI1EN_DEFAULT           (_TIMER_DTOGEN_DTOGCDTI1EN_DEFAULT << 4) /**< Shifted mode DEFAULT for TIMER_DTOGEN       */
#define TIMER_DTOGEN_DTOGCDTI2EN                   (0x1UL << 5)                             /**< DTI CDTIn Output Generation Enable          */
#define _TIMER_DTOGEN_DTOGCDTI2EN_SHIFT            5                                        /**< Shift value for TIMER_DTOGCDTI2EN           */
#define _TIMER_DTOGEN_DTOGCDTI2EN_MASK             0x20UL                                   /**< Bit mask for TIMER_DTOGCDTI2EN              */
#define _TIMER_DTOGEN_DTOGCDTI2EN_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for TIMER_DTOGEN               */
#define TIMER_DTOGEN_DTOGCDTI2EN_DEFAULT           (_TIMER_DTOGEN_DTOGCDTI2EN_DEFAULT << 5) /**< Shifted mode DEFAULT for TIMER_DTOGEN       */

/* Bit fields for TIMER DTFAULT */
#define _TIMER_DTFAULT_RESETVALUE                  0x00000000UL                            /**< Default value for TIMER_DTFAULT             */
#define _TIMER_DTFAULT_MASK                        0x0000001FUL                            /**< Mask for TIMER_DTFAULT                      */
#define TIMER_DTFAULT_DTPRS0F                      (0x1UL << 0)                            /**< DTI PRS 0 Fault                             */
#define _TIMER_DTFAULT_DTPRS0F_SHIFT               0                                       /**< Shift value for TIMER_DTPRS0F               */
#define _TIMER_DTFAULT_DTPRS0F_MASK                0x1UL                                   /**< Bit mask for TIMER_DTPRS0F                  */
#define _TIMER_DTFAULT_DTPRS0F_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for TIMER_DTFAULT              */
#define TIMER_DTFAULT_DTPRS0F_DEFAULT              (_TIMER_DTFAULT_DTPRS0F_DEFAULT << 0)   /**< Shifted mode DEFAULT for TIMER_DTFAULT      */
#define TIMER_DTFAULT_DTPRS1F                      (0x1UL << 1)                            /**< DTI PRS 1 Fault                             */
#define _TIMER_DTFAULT_DTPRS1F_SHIFT               1                                       /**< Shift value for TIMER_DTPRS1F               */
#define _TIMER_DTFAULT_DTPRS1F_MASK                0x2UL                                   /**< Bit mask for TIMER_DTPRS1F                  */
#define _TIMER_DTFAULT_DTPRS1F_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for TIMER_DTFAULT              */
#define TIMER_DTFAULT_DTPRS1F_DEFAULT              (_TIMER_DTFAULT_DTPRS1F_DEFAULT << 1)   /**< Shifted mode DEFAULT for TIMER_DTFAULT      */
#define TIMER_DTFAULT_DTDBGF                       (0x1UL << 2)                            /**< DTI Debugger Fault                          */
#define _TIMER_DTFAULT_DTDBGF_SHIFT                2                                       /**< Shift value for TIMER_DTDBGF                */
#define _TIMER_DTFAULT_DTDBGF_MASK                 0x4UL                                   /**< Bit mask for TIMER_DTDBGF                   */
#define _TIMER_DTFAULT_DTDBGF_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for TIMER_DTFAULT              */
#define TIMER_DTFAULT_DTDBGF_DEFAULT               (_TIMER_DTFAULT_DTDBGF_DEFAULT << 2)    /**< Shifted mode DEFAULT for TIMER_DTFAULT      */
#define TIMER_DTFAULT_DTLOCKUPF                    (0x1UL << 3)                            /**< DTI Lockup Fault                            */
#define _TIMER_DTFAULT_DTLOCKUPF_SHIFT             3                                       /**< Shift value for TIMER_DTLOCKUPF             */
#define _TIMER_DTFAULT_DTLOCKUPF_MASK              0x8UL                                   /**< Bit mask for TIMER_DTLOCKUPF                */
#define _TIMER_DTFAULT_DTLOCKUPF_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for TIMER_DTFAULT              */
#define TIMER_DTFAULT_DTLOCKUPF_DEFAULT            (_TIMER_DTFAULT_DTLOCKUPF_DEFAULT << 3) /**< Shifted mode DEFAULT for TIMER_DTFAULT      */
#define TIMER_DTFAULT_DTEM23F                      (0x1UL << 4)                            /**< DTI EM23 Entry Fault                        */
#define _TIMER_DTFAULT_DTEM23F_SHIFT               4                                       /**< Shift value for TIMER_DTEM23F               */
#define _TIMER_DTFAULT_DTEM23F_MASK                0x10UL                                  /**< Bit mask for TIMER_DTEM23F                  */
#define _TIMER_DTFAULT_DTEM23F_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for TIMER_DTFAULT              */
#define TIMER_DTFAULT_DTEM23F_DEFAULT              (_TIMER_DTFAULT_DTEM23F_DEFAULT << 4)   /**< Shifted mode DEFAULT for TIMER_DTFAULT      */

/* Bit fields for TIMER DTFAULTC */
#define _TIMER_DTFAULTC_RESETVALUE                 0x00000000UL                              /**< Default value for TIMER_DTFAULTC            */
#define _TIMER_DTFAULTC_MASK                       0x0000001FUL                              /**< Mask for TIMER_DTFAULTC                     */
#define TIMER_DTFAULTC_DTPRS0FC                    (0x1UL << 0)                              /**< DTI PRS0 Fault Clear                        */
#define _TIMER_DTFAULTC_DTPRS0FC_SHIFT             0                                         /**< Shift value for TIMER_DTPRS0FC              */
#define _TIMER_DTFAULTC_DTPRS0FC_MASK              0x1UL                                     /**< Bit mask for TIMER_DTPRS0FC                 */
#define _TIMER_DTFAULTC_DTPRS0FC_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for TIMER_DTFAULTC             */
#define TIMER_DTFAULTC_DTPRS0FC_DEFAULT            (_TIMER_DTFAULTC_DTPRS0FC_DEFAULT << 0)   /**< Shifted mode DEFAULT for TIMER_DTFAULTC     */
#define TIMER_DTFAULTC_DTPRS1FC                    (0x1UL << 1)                              /**< DTI PRS1 Fault Clear                        */
#define _TIMER_DTFAULTC_DTPRS1FC_SHIFT             1                                         /**< Shift value for TIMER_DTPRS1FC              */
#define _TIMER_DTFAULTC_DTPRS1FC_MASK              0x2UL                                     /**< Bit mask for TIMER_DTPRS1FC                 */
#define _TIMER_DTFAULTC_DTPRS1FC_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for TIMER_DTFAULTC             */
#define TIMER_DTFAULTC_DTPRS1FC_DEFAULT            (_TIMER_DTFAULTC_DTPRS1FC_DEFAULT << 1)   /**< Shifted mode DEFAULT for TIMER_DTFAULTC     */
#define TIMER_DTFAULTC_DTDBGFC                     (0x1UL << 2)                              /**< DTI Debugger Fault Clear                    */
#define _TIMER_DTFAULTC_DTDBGFC_SHIFT              2                                         /**< Shift value for TIMER_DTDBGFC               */
#define _TIMER_DTFAULTC_DTDBGFC_MASK               0x4UL                                     /**< Bit mask for TIMER_DTDBGFC                  */
#define _TIMER_DTFAULTC_DTDBGFC_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for TIMER_DTFAULTC             */
#define TIMER_DTFAULTC_DTDBGFC_DEFAULT             (_TIMER_DTFAULTC_DTDBGFC_DEFAULT << 2)    /**< Shifted mode DEFAULT for TIMER_DTFAULTC     */
#define TIMER_DTFAULTC_DTLOCKUPFC                  (0x1UL << 3)                              /**< DTI Lockup Fault Clear                      */
#define _TIMER_DTFAULTC_DTLOCKUPFC_SHIFT           3                                         /**< Shift value for TIMER_DTLOCKUPFC            */
#define _TIMER_DTFAULTC_DTLOCKUPFC_MASK            0x8UL                                     /**< Bit mask for TIMER_DTLOCKUPFC               */
#define _TIMER_DTFAULTC_DTLOCKUPFC_DEFAULT         0x00000000UL                              /**< Mode DEFAULT for TIMER_DTFAULTC             */
#define TIMER_DTFAULTC_DTLOCKUPFC_DEFAULT          (_TIMER_DTFAULTC_DTLOCKUPFC_DEFAULT << 3) /**< Shifted mode DEFAULT for TIMER_DTFAULTC     */
#define TIMER_DTFAULTC_DTEM23FC                    (0x1UL << 4)                              /**< DTI EM23 Fault Clear                        */
#define _TIMER_DTFAULTC_DTEM23FC_SHIFT             4                                         /**< Shift value for TIMER_DTEM23FC              */
#define _TIMER_DTFAULTC_DTEM23FC_MASK              0x10UL                                    /**< Bit mask for TIMER_DTEM23FC                 */
#define _TIMER_DTFAULTC_DTEM23FC_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for TIMER_DTFAULTC             */
#define TIMER_DTFAULTC_DTEM23FC_DEFAULT            (_TIMER_DTFAULTC_DTEM23FC_DEFAULT << 4)   /**< Shifted mode DEFAULT for TIMER_DTFAULTC     */

/* Bit fields for TIMER DTLOCK */
#define _TIMER_DTLOCK_RESETVALUE                   0x00000000UL                            /**< Default value for TIMER_DTLOCK              */
#define _TIMER_DTLOCK_MASK                         0x0000FFFFUL                            /**< Mask for TIMER_DTLOCK                       */
#define _TIMER_DTLOCK_DTILOCKKEY_SHIFT             0                                       /**< Shift value for TIMER_DTILOCKKEY            */
#define _TIMER_DTLOCK_DTILOCKKEY_MASK              0xFFFFUL                                /**< Bit mask for TIMER_DTILOCKKEY               */
#define _TIMER_DTLOCK_DTILOCKKEY_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for TIMER_DTLOCK               */
#define _TIMER_DTLOCK_DTILOCKKEY_UNLOCK            0x0000CE80UL                            /**< Mode UNLOCK for TIMER_DTLOCK                */
#define TIMER_DTLOCK_DTILOCKKEY_DEFAULT            (_TIMER_DTLOCK_DTILOCKKEY_DEFAULT << 0) /**< Shifted mode DEFAULT for TIMER_DTLOCK       */
#define TIMER_DTLOCK_DTILOCKKEY_UNLOCK             (_TIMER_DTLOCK_DTILOCKKEY_UNLOCK << 0)  /**< Shifted mode UNLOCK for TIMER_DTLOCK        */

/** @} End of group EFR32ZG23_TIMER_BitFields */
/** @} End of group EFR32ZG23_TIMER */
/** @} End of group Parts */

#endif /* EFR32ZG23_TIMER_H */
#ifdef __cplusplus
}
#endif

