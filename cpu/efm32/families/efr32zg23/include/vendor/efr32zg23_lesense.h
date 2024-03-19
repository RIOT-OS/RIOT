/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 LESENSE register and bit field definitions
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

#ifndef EFR32ZG23_LESENSE_H
#define EFR32ZG23_LESENSE_H
#define LESENSE_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_LESENSE LESENSE
 * @{
 * @brief EFR32ZG23 LESENSE Register Declaration.
 *****************************************************************************/

/** LESENSE CH Register Group Declaration. */
typedef struct {
  __IOM uint32_t TIMING;                             /**< Scan configuration                                 */
  __IOM uint32_t INTERACT;                           /**< Scan configuration                                 */
  __IOM uint32_t EVALCFG;                            /**< Scan configuration                                 */
  __IOM uint32_t EVALTHRES;                          /**< Scan confguration                                  */
} LESENSE_CH_TypeDef;

/** LESENSE ST Register Group Declaration. */
typedef struct {
  __IOM uint32_t ARC;                                /**< State transition Arc                               */
} LESENSE_ST_TypeDef;

/** LESENSE Register Declaration. */
typedef struct {
  __IM uint32_t      IPVERSION;                 /**< IPVERSION                                          */
  __IOM uint32_t     EN;                        /**< Enable                                             */
  __IOM uint32_t     SWRST;                     /**< Software Reset Register                            */
  __IOM uint32_t     CFG;                       /**< Configuration                                      */
  __IOM uint32_t     TIMCTRL;                   /**< Timing Control                                     */
  __IOM uint32_t     PERCTRL;                   /**< Peripheral Control                                 */
  __IOM uint32_t     DECCTRL;                   /**< Decoder control                                    */
  __IOM uint32_t     EVALCTRL;                  /**< LESENSE evaluation                                 */
  __IOM uint32_t     PRSCTRL;                   /**< PRS control                                        */
  __IOM uint32_t     CMD;                       /**< Command                                            */
  __IOM uint32_t     CHEN;                      /**< Channel enable                                     */
  __IM uint32_t      SCANRES;                   /**< Scan result                                        */
  __IM uint32_t      STATUS;                    /**< Status                                             */
  __IM uint32_t      RESCOUNT;                  /**< Result FIFO Count                                  */
  __IM uint32_t      RESFIFO;                   /**< Result Fifo                                        */
  __IM uint32_t      CURCH;                     /**< Current channel index                              */
  __IM uint32_t      DECSTATE;                  /**< Current decoder state                              */
  __IM uint32_t      SENSORSTATE;               /**< Sensor State                                       */
  __IOM uint32_t     IDLECONF;                  /**< IDLE Configuration                                 */
  uint32_t           RESERVED0[1U];             /**< Reserved for future use                            */
  __IM uint32_t      SYNCBUSY;                  /**< Synchronization                                    */
  uint32_t           RESERVED1[3U];             /**< Reserved for future use                            */
  __IOM uint32_t     IF;                        /**< Interrupt Flags                                    */
  __IOM uint32_t     IEN;                       /**< Interrupt Enables                                  */
  uint32_t           RESERVED2[38U];            /**< Reserved for future use                            */
  LESENSE_CH_TypeDef CH[16U];                   /**< Channels                                           */
  LESENSE_ST_TypeDef ST[64U];                   /**< Decoding FSM Arcs                                  */
  uint32_t           RESERVED3[832U];           /**< Reserved for future use                            */
  __IM uint32_t      IPVERSION_SET;             /**< IPVERSION                                          */
  __IOM uint32_t     EN_SET;                    /**< Enable                                             */
  __IOM uint32_t     SWRST_SET;                 /**< Software Reset Register                            */
  __IOM uint32_t     CFG_SET;                   /**< Configuration                                      */
  __IOM uint32_t     TIMCTRL_SET;               /**< Timing Control                                     */
  __IOM uint32_t     PERCTRL_SET;               /**< Peripheral Control                                 */
  __IOM uint32_t     DECCTRL_SET;               /**< Decoder control                                    */
  __IOM uint32_t     EVALCTRL_SET;              /**< LESENSE evaluation                                 */
  __IOM uint32_t     PRSCTRL_SET;               /**< PRS control                                        */
  __IOM uint32_t     CMD_SET;                   /**< Command                                            */
  __IOM uint32_t     CHEN_SET;                  /**< Channel enable                                     */
  __IM uint32_t      SCANRES_SET;               /**< Scan result                                        */
  __IM uint32_t      STATUS_SET;                /**< Status                                             */
  __IM uint32_t      RESCOUNT_SET;              /**< Result FIFO Count                                  */
  __IM uint32_t      RESFIFO_SET;               /**< Result Fifo                                        */
  __IM uint32_t      CURCH_SET;                 /**< Current channel index                              */
  __IM uint32_t      DECSTATE_SET;              /**< Current decoder state                              */
  __IM uint32_t      SENSORSTATE_SET;           /**< Sensor State                                       */
  __IOM uint32_t     IDLECONF_SET;              /**< IDLE Configuration                                 */
  uint32_t           RESERVED4[1U];             /**< Reserved for future use                            */
  __IM uint32_t      SYNCBUSY_SET;              /**< Synchronization                                    */
  uint32_t           RESERVED5[3U];             /**< Reserved for future use                            */
  __IOM uint32_t     IF_SET;                    /**< Interrupt Flags                                    */
  __IOM uint32_t     IEN_SET;                   /**< Interrupt Enables                                  */
  uint32_t           RESERVED6[38U];            /**< Reserved for future use                            */
  LESENSE_CH_TypeDef CH_SET[16U];               /**< Channels                                           */
  LESENSE_ST_TypeDef ST_SET[64U];               /**< Decoding FSM Arcs                                  */
  uint32_t           RESERVED7[832U];           /**< Reserved for future use                            */
  __IM uint32_t      IPVERSION_CLR;             /**< IPVERSION                                          */
  __IOM uint32_t     EN_CLR;                    /**< Enable                                             */
  __IOM uint32_t     SWRST_CLR;                 /**< Software Reset Register                            */
  __IOM uint32_t     CFG_CLR;                   /**< Configuration                                      */
  __IOM uint32_t     TIMCTRL_CLR;               /**< Timing Control                                     */
  __IOM uint32_t     PERCTRL_CLR;               /**< Peripheral Control                                 */
  __IOM uint32_t     DECCTRL_CLR;               /**< Decoder control                                    */
  __IOM uint32_t     EVALCTRL_CLR;              /**< LESENSE evaluation                                 */
  __IOM uint32_t     PRSCTRL_CLR;               /**< PRS control                                        */
  __IOM uint32_t     CMD_CLR;                   /**< Command                                            */
  __IOM uint32_t     CHEN_CLR;                  /**< Channel enable                                     */
  __IM uint32_t      SCANRES_CLR;               /**< Scan result                                        */
  __IM uint32_t      STATUS_CLR;                /**< Status                                             */
  __IM uint32_t      RESCOUNT_CLR;              /**< Result FIFO Count                                  */
  __IM uint32_t      RESFIFO_CLR;               /**< Result Fifo                                        */
  __IM uint32_t      CURCH_CLR;                 /**< Current channel index                              */
  __IM uint32_t      DECSTATE_CLR;              /**< Current decoder state                              */
  __IM uint32_t      SENSORSTATE_CLR;           /**< Sensor State                                       */
  __IOM uint32_t     IDLECONF_CLR;              /**< IDLE Configuration                                 */
  uint32_t           RESERVED8[1U];             /**< Reserved for future use                            */
  __IM uint32_t      SYNCBUSY_CLR;              /**< Synchronization                                    */
  uint32_t           RESERVED9[3U];             /**< Reserved for future use                            */
  __IOM uint32_t     IF_CLR;                    /**< Interrupt Flags                                    */
  __IOM uint32_t     IEN_CLR;                   /**< Interrupt Enables                                  */
  uint32_t           RESERVED10[38U];           /**< Reserved for future use                            */
  LESENSE_CH_TypeDef CH_CLR[16U];               /**< Channels                                           */
  LESENSE_ST_TypeDef ST_CLR[64U];               /**< Decoding FSM Arcs                                  */
  uint32_t           RESERVED11[832U];          /**< Reserved for future use                            */
  __IM uint32_t      IPVERSION_TGL;             /**< IPVERSION                                          */
  __IOM uint32_t     EN_TGL;                    /**< Enable                                             */
  __IOM uint32_t     SWRST_TGL;                 /**< Software Reset Register                            */
  __IOM uint32_t     CFG_TGL;                   /**< Configuration                                      */
  __IOM uint32_t     TIMCTRL_TGL;               /**< Timing Control                                     */
  __IOM uint32_t     PERCTRL_TGL;               /**< Peripheral Control                                 */
  __IOM uint32_t     DECCTRL_TGL;               /**< Decoder control                                    */
  __IOM uint32_t     EVALCTRL_TGL;              /**< LESENSE evaluation                                 */
  __IOM uint32_t     PRSCTRL_TGL;               /**< PRS control                                        */
  __IOM uint32_t     CMD_TGL;                   /**< Command                                            */
  __IOM uint32_t     CHEN_TGL;                  /**< Channel enable                                     */
  __IM uint32_t      SCANRES_TGL;               /**< Scan result                                        */
  __IM uint32_t      STATUS_TGL;                /**< Status                                             */
  __IM uint32_t      RESCOUNT_TGL;              /**< Result FIFO Count                                  */
  __IM uint32_t      RESFIFO_TGL;               /**< Result Fifo                                        */
  __IM uint32_t      CURCH_TGL;                 /**< Current channel index                              */
  __IM uint32_t      DECSTATE_TGL;              /**< Current decoder state                              */
  __IM uint32_t      SENSORSTATE_TGL;           /**< Sensor State                                       */
  __IOM uint32_t     IDLECONF_TGL;              /**< IDLE Configuration                                 */
  uint32_t           RESERVED12[1U];            /**< Reserved for future use                            */
  __IM uint32_t      SYNCBUSY_TGL;              /**< Synchronization                                    */
  uint32_t           RESERVED13[3U];            /**< Reserved for future use                            */
  __IOM uint32_t     IF_TGL;                    /**< Interrupt Flags                                    */
  __IOM uint32_t     IEN_TGL;                   /**< Interrupt Enables                                  */
  uint32_t           RESERVED14[38U];           /**< Reserved for future use                            */
  LESENSE_CH_TypeDef CH_TGL[16U];               /**< Channels                                           */
  LESENSE_ST_TypeDef ST_TGL[64U];               /**< Decoding FSM Arcs                                  */
} LESENSE_TypeDef;
/** @} End of group EFR32ZG23_LESENSE */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_LESENSE
 * @{
 * @defgroup EFR32ZG23_LESENSE_BitFields LESENSE Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for LESENSE IPVERSION */
#define _LESENSE_IPVERSION_RESETVALUE                0x00000001UL                                /**< Default value for LESENSE_IPVERSION         */
#define _LESENSE_IPVERSION_MASK                      0xFFFFFFFFUL                                /**< Mask for LESENSE_IPVERSION                  */
#define _LESENSE_IPVERSION_IPVERSION_SHIFT           0                                           /**< Shift value for LESENSE_IPVERSION           */
#define _LESENSE_IPVERSION_IPVERSION_MASK            0xFFFFFFFFUL                                /**< Bit mask for LESENSE_IPVERSION              */
#define _LESENSE_IPVERSION_IPVERSION_DEFAULT         0x00000001UL                                /**< Mode DEFAULT for LESENSE_IPVERSION          */
#define LESENSE_IPVERSION_IPVERSION_DEFAULT          (_LESENSE_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for LESENSE_IPVERSION  */

/* Bit fields for LESENSE EN */
#define _LESENSE_EN_RESETVALUE                       0x00000000UL                         /**< Default value for LESENSE_EN                */
#define _LESENSE_EN_MASK                             0x00000003UL                         /**< Mask for LESENSE_EN                         */
#define LESENSE_EN_EN                                (0x1UL << 0)                         /**< Enable                                      */
#define _LESENSE_EN_EN_SHIFT                         0                                    /**< Shift value for LESENSE_EN                  */
#define _LESENSE_EN_EN_MASK                          0x1UL                                /**< Bit mask for LESENSE_EN                     */
#define _LESENSE_EN_EN_DEFAULT                       0x00000000UL                         /**< Mode DEFAULT for LESENSE_EN                 */
#define _LESENSE_EN_EN_DISABLE                       0x00000000UL                         /**< Mode DISABLE for LESENSE_EN                 */
#define _LESENSE_EN_EN_ENABLE                        0x00000001UL                         /**< Mode ENABLE for LESENSE_EN                  */
#define LESENSE_EN_EN_DEFAULT                        (_LESENSE_EN_EN_DEFAULT << 0)        /**< Shifted mode DEFAULT for LESENSE_EN         */
#define LESENSE_EN_EN_DISABLE                        (_LESENSE_EN_EN_DISABLE << 0)        /**< Shifted mode DISABLE for LESENSE_EN         */
#define LESENSE_EN_EN_ENABLE                         (_LESENSE_EN_EN_ENABLE << 0)         /**< Shifted mode ENABLE for LESENSE_EN          */
#define LESENSE_EN_DISABLING                         (0x1UL << 1)                         /**< Disabling                                   */
#define _LESENSE_EN_DISABLING_SHIFT                  1                                    /**< Shift value for LESENSE_DISABLING           */
#define _LESENSE_EN_DISABLING_MASK                   0x2UL                                /**< Bit mask for LESENSE_DISABLING              */
#define _LESENSE_EN_DISABLING_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for LESENSE_EN                 */
#define LESENSE_EN_DISABLING_DEFAULT                 (_LESENSE_EN_DISABLING_DEFAULT << 1) /**< Shifted mode DEFAULT for LESENSE_EN         */

/* Bit fields for LESENSE SWRST */
#define _LESENSE_SWRST_RESETVALUE                    0x00000000UL                            /**< Default value for LESENSE_SWRST             */
#define _LESENSE_SWRST_MASK                          0x00000003UL                            /**< Mask for LESENSE_SWRST                      */
#define LESENSE_SWRST_SWRST                          (0x1UL << 0)                            /**< Software reset command                      */
#define _LESENSE_SWRST_SWRST_SHIFT                   0                                       /**< Shift value for LESENSE_SWRST               */
#define _LESENSE_SWRST_SWRST_MASK                    0x1UL                                   /**< Bit mask for LESENSE_SWRST                  */
#define _LESENSE_SWRST_SWRST_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for LESENSE_SWRST              */
#define LESENSE_SWRST_SWRST_DEFAULT                  (_LESENSE_SWRST_SWRST_DEFAULT << 0)     /**< Shifted mode DEFAULT for LESENSE_SWRST      */
#define LESENSE_SWRST_RESETTING                      (0x1UL << 1)                            /**< Software reset busy status                  */
#define _LESENSE_SWRST_RESETTING_SHIFT               1                                       /**< Shift value for LESENSE_RESETTING           */
#define _LESENSE_SWRST_RESETTING_MASK                0x2UL                                   /**< Bit mask for LESENSE_RESETTING              */
#define _LESENSE_SWRST_RESETTING_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for LESENSE_SWRST              */
#define LESENSE_SWRST_RESETTING_DEFAULT              (_LESENSE_SWRST_RESETTING_DEFAULT << 1) /**< Shifted mode DEFAULT for LESENSE_SWRST      */

/* Bit fields for LESENSE CFG */
#define _LESENSE_CFG_RESETVALUE                      0x00000000UL                           /**< Default value for LESENSE_CFG               */
#define _LESENSE_CFG_MASK                            0x00020FEFUL                           /**< Mask for LESENSE_CFG                        */
#define _LESENSE_CFG_SCANMODE_SHIFT                  0                                      /**< Shift value for LESENSE_SCANMODE            */
#define _LESENSE_CFG_SCANMODE_MASK                   0x3UL                                  /**< Bit mask for LESENSE_SCANMODE               */
#define _LESENSE_CFG_SCANMODE_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for LESENSE_CFG                */
#define _LESENSE_CFG_SCANMODE_PERIODIC               0x00000000UL                           /**< Mode PERIODIC for LESENSE_CFG               */
#define _LESENSE_CFG_SCANMODE_ONESHOT                0x00000001UL                           /**< Mode ONESHOT for LESENSE_CFG                */
#define _LESENSE_CFG_SCANMODE_PRS                    0x00000002UL                           /**< Mode PRS for LESENSE_CFG                    */
#define LESENSE_CFG_SCANMODE_DEFAULT                 (_LESENSE_CFG_SCANMODE_DEFAULT << 0)   /**< Shifted mode DEFAULT for LESENSE_CFG        */
#define LESENSE_CFG_SCANMODE_PERIODIC                (_LESENSE_CFG_SCANMODE_PERIODIC << 0)  /**< Shifted mode PERIODIC for LESENSE_CFG       */
#define LESENSE_CFG_SCANMODE_ONESHOT                 (_LESENSE_CFG_SCANMODE_ONESHOT << 0)   /**< Shifted mode ONESHOT for LESENSE_CFG        */
#define LESENSE_CFG_SCANMODE_PRS                     (_LESENSE_CFG_SCANMODE_PRS << 0)       /**< Shifted mode PRS for LESENSE_CFG            */
#define _LESENSE_CFG_SCANCONF_SHIFT                  2                                      /**< Shift value for LESENSE_SCANCONF            */
#define _LESENSE_CFG_SCANCONF_MASK                   0xCUL                                  /**< Bit mask for LESENSE_SCANCONF               */
#define _LESENSE_CFG_SCANCONF_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for LESENSE_CFG                */
#define _LESENSE_CFG_SCANCONF_DIRMAP                 0x00000000UL                           /**< Mode DIRMAP for LESENSE_CFG                 */
#define _LESENSE_CFG_SCANCONF_INVMAP                 0x00000001UL                           /**< Mode INVMAP for LESENSE_CFG                 */
#define _LESENSE_CFG_SCANCONF_TOGGLE                 0x00000002UL                           /**< Mode TOGGLE for LESENSE_CFG                 */
#define _LESENSE_CFG_SCANCONF_DECDEF                 0x00000003UL                           /**< Mode DECDEF for LESENSE_CFG                 */
#define LESENSE_CFG_SCANCONF_DEFAULT                 (_LESENSE_CFG_SCANCONF_DEFAULT << 2)   /**< Shifted mode DEFAULT for LESENSE_CFG        */
#define LESENSE_CFG_SCANCONF_DIRMAP                  (_LESENSE_CFG_SCANCONF_DIRMAP << 2)    /**< Shifted mode DIRMAP for LESENSE_CFG         */
#define LESENSE_CFG_SCANCONF_INVMAP                  (_LESENSE_CFG_SCANCONF_INVMAP << 2)    /**< Shifted mode INVMAP for LESENSE_CFG         */
#define LESENSE_CFG_SCANCONF_TOGGLE                  (_LESENSE_CFG_SCANCONF_TOGGLE << 2)    /**< Shifted mode TOGGLE for LESENSE_CFG         */
#define LESENSE_CFG_SCANCONF_DECDEF                  (_LESENSE_CFG_SCANCONF_DECDEF << 2)    /**< Shifted mode DECDEF for LESENSE_CFG         */
#define LESENSE_CFG_DUALSAMPLE                       (0x1UL << 5)                           /**< Enable dual sample mode                     */
#define _LESENSE_CFG_DUALSAMPLE_SHIFT                5                                      /**< Shift value for LESENSE_DUALSAMPLE          */
#define _LESENSE_CFG_DUALSAMPLE_MASK                 0x20UL                                 /**< Bit mask for LESENSE_DUALSAMPLE             */
#define _LESENSE_CFG_DUALSAMPLE_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for LESENSE_CFG                */
#define LESENSE_CFG_DUALSAMPLE_DEFAULT               (_LESENSE_CFG_DUALSAMPLE_DEFAULT << 5) /**< Shifted mode DEFAULT for LESENSE_CFG        */
#define LESENSE_CFG_STRSCANRES                       (0x1UL << 6)                           /**< Enable storing of SCANRES                   */
#define _LESENSE_CFG_STRSCANRES_SHIFT                6                                      /**< Shift value for LESENSE_STRSCANRES          */
#define _LESENSE_CFG_STRSCANRES_MASK                 0x40UL                                 /**< Bit mask for LESENSE_STRSCANRES             */
#define _LESENSE_CFG_STRSCANRES_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for LESENSE_CFG                */
#define LESENSE_CFG_STRSCANRES_DEFAULT               (_LESENSE_CFG_STRSCANRES_DEFAULT << 6) /**< Shifted mode DEFAULT for LESENSE_CFG        */
#define LESENSE_CFG_DMAWU                            (0x1UL << 7)                           /**< DMA wake-up from EM2                        */
#define _LESENSE_CFG_DMAWU_SHIFT                     7                                      /**< Shift value for LESENSE_DMAWU               */
#define _LESENSE_CFG_DMAWU_MASK                      0x80UL                                 /**< Bit mask for LESENSE_DMAWU                  */
#define _LESENSE_CFG_DMAWU_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for LESENSE_CFG                */
#define _LESENSE_CFG_DMAWU_DISABLE                   0x00000000UL                           /**< Mode DISABLE for LESENSE_CFG                */
#define _LESENSE_CFG_DMAWU_ENABLE                    0x00000001UL                           /**< Mode ENABLE for LESENSE_CFG                 */
#define LESENSE_CFG_DMAWU_DEFAULT                    (_LESENSE_CFG_DMAWU_DEFAULT << 7)      /**< Shifted mode DEFAULT for LESENSE_CFG        */
#define LESENSE_CFG_DMAWU_DISABLE                    (_LESENSE_CFG_DMAWU_DISABLE << 7)      /**< Shifted mode DISABLE for LESENSE_CFG        */
#define LESENSE_CFG_DMAWU_ENABLE                     (_LESENSE_CFG_DMAWU_ENABLE << 7)       /**< Shifted mode ENABLE for LESENSE_CFG         */
#define _LESENSE_CFG_RESFIDL_SHIFT                   8                                      /**< Shift value for LESENSE_RESFIDL             */
#define _LESENSE_CFG_RESFIDL_MASK                    0xF00UL                                /**< Bit mask for LESENSE_RESFIDL                */
#define _LESENSE_CFG_RESFIDL_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for LESENSE_CFG                */
#define LESENSE_CFG_RESFIDL_DEFAULT                  (_LESENSE_CFG_RESFIDL_DEFAULT << 8)    /**< Shifted mode DEFAULT for LESENSE_CFG        */
#define LESENSE_CFG_DEBUGRUN                         (0x1UL << 17)                          /**< Debug Mode Run Enable                       */
#define _LESENSE_CFG_DEBUGRUN_SHIFT                  17                                     /**< Shift value for LESENSE_DEBUGRUN            */
#define _LESENSE_CFG_DEBUGRUN_MASK                   0x20000UL                              /**< Bit mask for LESENSE_DEBUGRUN               */
#define _LESENSE_CFG_DEBUGRUN_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for LESENSE_CFG                */
#define _LESENSE_CFG_DEBUGRUN_X0                     0x00000000UL                           /**< Mode X0 for LESENSE_CFG                     */
#define _LESENSE_CFG_DEBUGRUN_X1                     0x00000001UL                           /**< Mode X1 for LESENSE_CFG                     */
#define LESENSE_CFG_DEBUGRUN_DEFAULT                 (_LESENSE_CFG_DEBUGRUN_DEFAULT << 17)  /**< Shifted mode DEFAULT for LESENSE_CFG        */
#define LESENSE_CFG_DEBUGRUN_X0                      (_LESENSE_CFG_DEBUGRUN_X0 << 17)       /**< Shifted mode X0 for LESENSE_CFG             */
#define LESENSE_CFG_DEBUGRUN_X1                      (_LESENSE_CFG_DEBUGRUN_X1 << 17)       /**< Shifted mode X1 for LESENSE_CFG             */

/* Bit fields for LESENSE TIMCTRL */
#define _LESENSE_TIMCTRL_RESETVALUE                  0x00000000UL                                  /**< Default value for LESENSE_TIMCTRL           */
#define _LESENSE_TIMCTRL_MASK                        0x10CFF773UL                                  /**< Mask for LESENSE_TIMCTRL                    */
#define _LESENSE_TIMCTRL_AUXPRESC_SHIFT              0                                             /**< Shift value for LESENSE_AUXPRESC            */
#define _LESENSE_TIMCTRL_AUXPRESC_MASK               0x3UL                                         /**< Bit mask for LESENSE_AUXPRESC               */
#define _LESENSE_TIMCTRL_AUXPRESC_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for LESENSE_TIMCTRL            */
#define _LESENSE_TIMCTRL_AUXPRESC_DIV1               0x00000000UL                                  /**< Mode DIV1 for LESENSE_TIMCTRL               */
#define _LESENSE_TIMCTRL_AUXPRESC_DIV2               0x00000001UL                                  /**< Mode DIV2 for LESENSE_TIMCTRL               */
#define _LESENSE_TIMCTRL_AUXPRESC_DIV4               0x00000002UL                                  /**< Mode DIV4 for LESENSE_TIMCTRL               */
#define _LESENSE_TIMCTRL_AUXPRESC_DIV8               0x00000003UL                                  /**< Mode DIV8 for LESENSE_TIMCTRL               */
#define LESENSE_TIMCTRL_AUXPRESC_DEFAULT             (_LESENSE_TIMCTRL_AUXPRESC_DEFAULT << 0)      /**< Shifted mode DEFAULT for LESENSE_TIMCTRL    */
#define LESENSE_TIMCTRL_AUXPRESC_DIV1                (_LESENSE_TIMCTRL_AUXPRESC_DIV1 << 0)         /**< Shifted mode DIV1 for LESENSE_TIMCTRL       */
#define LESENSE_TIMCTRL_AUXPRESC_DIV2                (_LESENSE_TIMCTRL_AUXPRESC_DIV2 << 0)         /**< Shifted mode DIV2 for LESENSE_TIMCTRL       */
#define LESENSE_TIMCTRL_AUXPRESC_DIV4                (_LESENSE_TIMCTRL_AUXPRESC_DIV4 << 0)         /**< Shifted mode DIV4 for LESENSE_TIMCTRL       */
#define LESENSE_TIMCTRL_AUXPRESC_DIV8                (_LESENSE_TIMCTRL_AUXPRESC_DIV8 << 0)         /**< Shifted mode DIV8 for LESENSE_TIMCTRL       */
#define _LESENSE_TIMCTRL_LFPRESC_SHIFT               4                                             /**< Shift value for LESENSE_LFPRESC             */
#define _LESENSE_TIMCTRL_LFPRESC_MASK                0x70UL                                        /**< Bit mask for LESENSE_LFPRESC                */
#define _LESENSE_TIMCTRL_LFPRESC_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for LESENSE_TIMCTRL            */
#define _LESENSE_TIMCTRL_LFPRESC_DIV1                0x00000000UL                                  /**< Mode DIV1 for LESENSE_TIMCTRL               */
#define _LESENSE_TIMCTRL_LFPRESC_DIV2                0x00000001UL                                  /**< Mode DIV2 for LESENSE_TIMCTRL               */
#define _LESENSE_TIMCTRL_LFPRESC_DIV4                0x00000002UL                                  /**< Mode DIV4 for LESENSE_TIMCTRL               */
#define _LESENSE_TIMCTRL_LFPRESC_DIV8                0x00000003UL                                  /**< Mode DIV8 for LESENSE_TIMCTRL               */
#define _LESENSE_TIMCTRL_LFPRESC_DIV16               0x00000004UL                                  /**< Mode DIV16 for LESENSE_TIMCTRL              */
#define _LESENSE_TIMCTRL_LFPRESC_DIV32               0x00000005UL                                  /**< Mode DIV32 for LESENSE_TIMCTRL              */
#define _LESENSE_TIMCTRL_LFPRESC_DIV64               0x00000006UL                                  /**< Mode DIV64 for LESENSE_TIMCTRL              */
#define _LESENSE_TIMCTRL_LFPRESC_DIV128              0x00000007UL                                  /**< Mode DIV128 for LESENSE_TIMCTRL             */
#define LESENSE_TIMCTRL_LFPRESC_DEFAULT              (_LESENSE_TIMCTRL_LFPRESC_DEFAULT << 4)       /**< Shifted mode DEFAULT for LESENSE_TIMCTRL    */
#define LESENSE_TIMCTRL_LFPRESC_DIV1                 (_LESENSE_TIMCTRL_LFPRESC_DIV1 << 4)          /**< Shifted mode DIV1 for LESENSE_TIMCTRL       */
#define LESENSE_TIMCTRL_LFPRESC_DIV2                 (_LESENSE_TIMCTRL_LFPRESC_DIV2 << 4)          /**< Shifted mode DIV2 for LESENSE_TIMCTRL       */
#define LESENSE_TIMCTRL_LFPRESC_DIV4                 (_LESENSE_TIMCTRL_LFPRESC_DIV4 << 4)          /**< Shifted mode DIV4 for LESENSE_TIMCTRL       */
#define LESENSE_TIMCTRL_LFPRESC_DIV8                 (_LESENSE_TIMCTRL_LFPRESC_DIV8 << 4)          /**< Shifted mode DIV8 for LESENSE_TIMCTRL       */
#define LESENSE_TIMCTRL_LFPRESC_DIV16                (_LESENSE_TIMCTRL_LFPRESC_DIV16 << 4)         /**< Shifted mode DIV16 for LESENSE_TIMCTRL      */
#define LESENSE_TIMCTRL_LFPRESC_DIV32                (_LESENSE_TIMCTRL_LFPRESC_DIV32 << 4)         /**< Shifted mode DIV32 for LESENSE_TIMCTRL      */
#define LESENSE_TIMCTRL_LFPRESC_DIV64                (_LESENSE_TIMCTRL_LFPRESC_DIV64 << 4)         /**< Shifted mode DIV64 for LESENSE_TIMCTRL      */
#define LESENSE_TIMCTRL_LFPRESC_DIV128               (_LESENSE_TIMCTRL_LFPRESC_DIV128 << 4)        /**< Shifted mode DIV128 for LESENSE_TIMCTRL     */
#define _LESENSE_TIMCTRL_PCPRESC_SHIFT               8                                             /**< Shift value for LESENSE_PCPRESC             */
#define _LESENSE_TIMCTRL_PCPRESC_MASK                0x700UL                                       /**< Bit mask for LESENSE_PCPRESC                */
#define _LESENSE_TIMCTRL_PCPRESC_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for LESENSE_TIMCTRL            */
#define _LESENSE_TIMCTRL_PCPRESC_DIV1                0x00000000UL                                  /**< Mode DIV1 for LESENSE_TIMCTRL               */
#define _LESENSE_TIMCTRL_PCPRESC_DIV2                0x00000001UL                                  /**< Mode DIV2 for LESENSE_TIMCTRL               */
#define _LESENSE_TIMCTRL_PCPRESC_DIV4                0x00000002UL                                  /**< Mode DIV4 for LESENSE_TIMCTRL               */
#define _LESENSE_TIMCTRL_PCPRESC_DIV8                0x00000003UL                                  /**< Mode DIV8 for LESENSE_TIMCTRL               */
#define _LESENSE_TIMCTRL_PCPRESC_DIV16               0x00000004UL                                  /**< Mode DIV16 for LESENSE_TIMCTRL              */
#define _LESENSE_TIMCTRL_PCPRESC_DIV32               0x00000005UL                                  /**< Mode DIV32 for LESENSE_TIMCTRL              */
#define _LESENSE_TIMCTRL_PCPRESC_DIV64               0x00000006UL                                  /**< Mode DIV64 for LESENSE_TIMCTRL              */
#define _LESENSE_TIMCTRL_PCPRESC_DIV128              0x00000007UL                                  /**< Mode DIV128 for LESENSE_TIMCTRL             */
#define LESENSE_TIMCTRL_PCPRESC_DEFAULT              (_LESENSE_TIMCTRL_PCPRESC_DEFAULT << 8)       /**< Shifted mode DEFAULT for LESENSE_TIMCTRL    */
#define LESENSE_TIMCTRL_PCPRESC_DIV1                 (_LESENSE_TIMCTRL_PCPRESC_DIV1 << 8)          /**< Shifted mode DIV1 for LESENSE_TIMCTRL       */
#define LESENSE_TIMCTRL_PCPRESC_DIV2                 (_LESENSE_TIMCTRL_PCPRESC_DIV2 << 8)          /**< Shifted mode DIV2 for LESENSE_TIMCTRL       */
#define LESENSE_TIMCTRL_PCPRESC_DIV4                 (_LESENSE_TIMCTRL_PCPRESC_DIV4 << 8)          /**< Shifted mode DIV4 for LESENSE_TIMCTRL       */
#define LESENSE_TIMCTRL_PCPRESC_DIV8                 (_LESENSE_TIMCTRL_PCPRESC_DIV8 << 8)          /**< Shifted mode DIV8 for LESENSE_TIMCTRL       */
#define LESENSE_TIMCTRL_PCPRESC_DIV16                (_LESENSE_TIMCTRL_PCPRESC_DIV16 << 8)         /**< Shifted mode DIV16 for LESENSE_TIMCTRL      */
#define LESENSE_TIMCTRL_PCPRESC_DIV32                (_LESENSE_TIMCTRL_PCPRESC_DIV32 << 8)         /**< Shifted mode DIV32 for LESENSE_TIMCTRL      */
#define LESENSE_TIMCTRL_PCPRESC_DIV64                (_LESENSE_TIMCTRL_PCPRESC_DIV64 << 8)         /**< Shifted mode DIV64 for LESENSE_TIMCTRL      */
#define LESENSE_TIMCTRL_PCPRESC_DIV128               (_LESENSE_TIMCTRL_PCPRESC_DIV128 << 8)        /**< Shifted mode DIV128 for LESENSE_TIMCTRL     */
#define _LESENSE_TIMCTRL_PCTOP_SHIFT                 12                                            /**< Shift value for LESENSE_PCTOP               */
#define _LESENSE_TIMCTRL_PCTOP_MASK                  0xFF000UL                                     /**< Bit mask for LESENSE_PCTOP                  */
#define _LESENSE_TIMCTRL_PCTOP_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for LESENSE_TIMCTRL            */
#define LESENSE_TIMCTRL_PCTOP_DEFAULT                (_LESENSE_TIMCTRL_PCTOP_DEFAULT << 12)        /**< Shifted mode DEFAULT for LESENSE_TIMCTRL    */
#define _LESENSE_TIMCTRL_STARTDLY_SHIFT              22                                            /**< Shift value for LESENSE_STARTDLY            */
#define _LESENSE_TIMCTRL_STARTDLY_MASK               0xC00000UL                                    /**< Bit mask for LESENSE_STARTDLY               */
#define _LESENSE_TIMCTRL_STARTDLY_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for LESENSE_TIMCTRL            */
#define LESENSE_TIMCTRL_STARTDLY_DEFAULT             (_LESENSE_TIMCTRL_STARTDLY_DEFAULT << 22)     /**< Shifted mode DEFAULT for LESENSE_TIMCTRL    */
#define LESENSE_TIMCTRL_AUXSTARTUP                   (0x1UL << 28)                                 /**< AUX startup config                          */
#define _LESENSE_TIMCTRL_AUXSTARTUP_SHIFT            28                                            /**< Shift value for LESENSE_AUXSTARTUP          */
#define _LESENSE_TIMCTRL_AUXSTARTUP_MASK             0x10000000UL                                  /**< Bit mask for LESENSE_AUXSTARTUP             */
#define _LESENSE_TIMCTRL_AUXSTARTUP_DEFAULT          0x00000000UL                                  /**< Mode DEFAULT for LESENSE_TIMCTRL            */
#define _LESENSE_TIMCTRL_AUXSTARTUP_PREDEMAND        0x00000000UL                                  /**< Mode PREDEMAND for LESENSE_TIMCTRL          */
#define _LESENSE_TIMCTRL_AUXSTARTUP_ONDEMAND         0x00000001UL                                  /**< Mode ONDEMAND for LESENSE_TIMCTRL           */
#define LESENSE_TIMCTRL_AUXSTARTUP_DEFAULT           (_LESENSE_TIMCTRL_AUXSTARTUP_DEFAULT << 28)   /**< Shifted mode DEFAULT for LESENSE_TIMCTRL    */
#define LESENSE_TIMCTRL_AUXSTARTUP_PREDEMAND         (_LESENSE_TIMCTRL_AUXSTARTUP_PREDEMAND << 28) /**< Shifted mode PREDEMAND for LESENSE_TIMCTRL  */
#define LESENSE_TIMCTRL_AUXSTARTUP_ONDEMAND          (_LESENSE_TIMCTRL_AUXSTARTUP_ONDEMAND << 28)  /**< Shifted mode ONDEMAND for LESENSE_TIMCTRL   */

/* Bit fields for LESENSE PERCTRL */
#define _LESENSE_PERCTRL_RESETVALUE                  0x00000000UL                                     /**< Default value for LESENSE_PERCTRL           */
#define _LESENSE_PERCTRL_MASK                        0x03500144UL                                     /**< Mask for LESENSE_PERCTRL                    */
#define LESENSE_PERCTRL_DACCH0DATA                   (0x1UL << 2)                                     /**< DAC CH0 data selection.                     */
#define _LESENSE_PERCTRL_DACCH0DATA_SHIFT            2                                                /**< Shift value for LESENSE_DACCH0DATA          */
#define _LESENSE_PERCTRL_DACCH0DATA_MASK             0x4UL                                            /**< Bit mask for LESENSE_DACCH0DATA             */
#define _LESENSE_PERCTRL_DACCH0DATA_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for LESENSE_PERCTRL            */
#define _LESENSE_PERCTRL_DACCH0DATA_DACDATA          0x00000000UL                                     /**< Mode DACDATA for LESENSE_PERCTRL            */
#define _LESENSE_PERCTRL_DACCH0DATA_THRES            0x00000001UL                                     /**< Mode THRES for LESENSE_PERCTRL              */
#define LESENSE_PERCTRL_DACCH0DATA_DEFAULT           (_LESENSE_PERCTRL_DACCH0DATA_DEFAULT << 2)       /**< Shifted mode DEFAULT for LESENSE_PERCTRL    */
#define LESENSE_PERCTRL_DACCH0DATA_DACDATA           (_LESENSE_PERCTRL_DACCH0DATA_DACDATA << 2)       /**< Shifted mode DACDATA for LESENSE_PERCTRL    */
#define LESENSE_PERCTRL_DACCH0DATA_THRES             (_LESENSE_PERCTRL_DACCH0DATA_THRES << 2)         /**< Shifted mode THRES for LESENSE_PERCTRL      */
#define LESENSE_PERCTRL_DACSTARTUP                   (0x1UL << 6)                                     /**< DAC startup configuration                   */
#define _LESENSE_PERCTRL_DACSTARTUP_SHIFT            6                                                /**< Shift value for LESENSE_DACSTARTUP          */
#define _LESENSE_PERCTRL_DACSTARTUP_MASK             0x40UL                                           /**< Bit mask for LESENSE_DACSTARTUP             */
#define _LESENSE_PERCTRL_DACSTARTUP_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for LESENSE_PERCTRL            */
#define _LESENSE_PERCTRL_DACSTARTUP_FULLCYCLE        0x00000000UL                                     /**< Mode FULLCYCLE for LESENSE_PERCTRL          */
#define _LESENSE_PERCTRL_DACSTARTUP_HALFCYCLE        0x00000001UL                                     /**< Mode HALFCYCLE for LESENSE_PERCTRL          */
#define LESENSE_PERCTRL_DACSTARTUP_DEFAULT           (_LESENSE_PERCTRL_DACSTARTUP_DEFAULT << 6)       /**< Shifted mode DEFAULT for LESENSE_PERCTRL    */
#define LESENSE_PERCTRL_DACSTARTUP_FULLCYCLE         (_LESENSE_PERCTRL_DACSTARTUP_FULLCYCLE << 6)     /**< Shifted mode FULLCYCLE for LESENSE_PERCTRL  */
#define LESENSE_PERCTRL_DACSTARTUP_HALFCYCLE         (_LESENSE_PERCTRL_DACSTARTUP_HALFCYCLE << 6)     /**< Shifted mode HALFCYCLE for LESENSE_PERCTRL  */
#define LESENSE_PERCTRL_DACCONVTRIG                  (0x1UL << 8)                                     /**< DAC conversion trigger configuration        */
#define _LESENSE_PERCTRL_DACCONVTRIG_SHIFT           8                                                /**< Shift value for LESENSE_DACCONVTRIG         */
#define _LESENSE_PERCTRL_DACCONVTRIG_MASK            0x100UL                                          /**< Bit mask for LESENSE_DACCONVTRIG            */
#define _LESENSE_PERCTRL_DACCONVTRIG_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for LESENSE_PERCTRL            */
#define _LESENSE_PERCTRL_DACCONVTRIG_CHANNELSTART    0x00000000UL                                     /**< Mode CHANNELSTART for LESENSE_PERCTRL       */
#define _LESENSE_PERCTRL_DACCONVTRIG_SCANSTART       0x00000001UL                                     /**< Mode SCANSTART for LESENSE_PERCTRL          */
#define LESENSE_PERCTRL_DACCONVTRIG_DEFAULT          (_LESENSE_PERCTRL_DACCONVTRIG_DEFAULT << 8)      /**< Shifted mode DEFAULT for LESENSE_PERCTRL    */
#define LESENSE_PERCTRL_DACCONVTRIG_CHANNELSTART     (_LESENSE_PERCTRL_DACCONVTRIG_CHANNELSTART << 8) /**< Shifted mode CHANNELSTART for LESENSE_PERCTRL*/
#define LESENSE_PERCTRL_DACCONVTRIG_SCANSTART        (_LESENSE_PERCTRL_DACCONVTRIG_SCANSTART << 8)    /**< Shifted mode SCANSTART for LESENSE_PERCTRL  */
#define LESENSE_PERCTRL_ACMP0MODE                    (0x1UL << 20)                                    /**< ACMP0 mode                                  */
#define _LESENSE_PERCTRL_ACMP0MODE_SHIFT             20                                               /**< Shift value for LESENSE_ACMP0MODE           */
#define _LESENSE_PERCTRL_ACMP0MODE_MASK              0x100000UL                                       /**< Bit mask for LESENSE_ACMP0MODE              */
#define _LESENSE_PERCTRL_ACMP0MODE_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for LESENSE_PERCTRL            */
#define _LESENSE_PERCTRL_ACMP0MODE_MUX               0x00000000UL                                     /**< Mode MUX for LESENSE_PERCTRL                */
#define _LESENSE_PERCTRL_ACMP0MODE_MUXTHRES          0x00000001UL                                     /**< Mode MUXTHRES for LESENSE_PERCTRL           */
#define LESENSE_PERCTRL_ACMP0MODE_DEFAULT            (_LESENSE_PERCTRL_ACMP0MODE_DEFAULT << 20)       /**< Shifted mode DEFAULT for LESENSE_PERCTRL    */
#define LESENSE_PERCTRL_ACMP0MODE_MUX                (_LESENSE_PERCTRL_ACMP0MODE_MUX << 20)           /**< Shifted mode MUX for LESENSE_PERCTRL        */
#define LESENSE_PERCTRL_ACMP0MODE_MUXTHRES           (_LESENSE_PERCTRL_ACMP0MODE_MUXTHRES << 20)      /**< Shifted mode MUXTHRES for LESENSE_PERCTRL   */
#define LESENSE_PERCTRL_ACMP1MODE                    (0x1UL << 22)                                    /**< ACMP1 mode                                  */
#define _LESENSE_PERCTRL_ACMP1MODE_SHIFT             22                                               /**< Shift value for LESENSE_ACMP1MODE           */
#define _LESENSE_PERCTRL_ACMP1MODE_MASK              0x400000UL                                       /**< Bit mask for LESENSE_ACMP1MODE              */
#define _LESENSE_PERCTRL_ACMP1MODE_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for LESENSE_PERCTRL            */
#define _LESENSE_PERCTRL_ACMP1MODE_MUX               0x00000000UL                                     /**< Mode MUX for LESENSE_PERCTRL                */
#define _LESENSE_PERCTRL_ACMP1MODE_MUXTHRES          0x00000001UL                                     /**< Mode MUXTHRES for LESENSE_PERCTRL           */
#define LESENSE_PERCTRL_ACMP1MODE_DEFAULT            (_LESENSE_PERCTRL_ACMP1MODE_DEFAULT << 22)       /**< Shifted mode DEFAULT for LESENSE_PERCTRL    */
#define LESENSE_PERCTRL_ACMP1MODE_MUX                (_LESENSE_PERCTRL_ACMP1MODE_MUX << 22)           /**< Shifted mode MUX for LESENSE_PERCTRL        */
#define LESENSE_PERCTRL_ACMP1MODE_MUXTHRES           (_LESENSE_PERCTRL_ACMP1MODE_MUXTHRES << 22)      /**< Shifted mode MUXTHRES for LESENSE_PERCTRL   */
#define LESENSE_PERCTRL_ACMP0INV                     (0x1UL << 24)                                    /**< Invert analog comparator 0 output           */
#define _LESENSE_PERCTRL_ACMP0INV_SHIFT              24                                               /**< Shift value for LESENSE_ACMP0INV            */
#define _LESENSE_PERCTRL_ACMP0INV_MASK               0x1000000UL                                      /**< Bit mask for LESENSE_ACMP0INV               */
#define _LESENSE_PERCTRL_ACMP0INV_DEFAULT            0x00000000UL                                     /**< Mode DEFAULT for LESENSE_PERCTRL            */
#define LESENSE_PERCTRL_ACMP0INV_DEFAULT             (_LESENSE_PERCTRL_ACMP0INV_DEFAULT << 24)        /**< Shifted mode DEFAULT for LESENSE_PERCTRL    */
#define LESENSE_PERCTRL_ACMP1INV                     (0x1UL << 25)                                    /**< Invert analog comparator 1 output           */
#define _LESENSE_PERCTRL_ACMP1INV_SHIFT              25                                               /**< Shift value for LESENSE_ACMP1INV            */
#define _LESENSE_PERCTRL_ACMP1INV_MASK               0x2000000UL                                      /**< Bit mask for LESENSE_ACMP1INV               */
#define _LESENSE_PERCTRL_ACMP1INV_DEFAULT            0x00000000UL                                     /**< Mode DEFAULT for LESENSE_PERCTRL            */
#define LESENSE_PERCTRL_ACMP1INV_DEFAULT             (_LESENSE_PERCTRL_ACMP1INV_DEFAULT << 25)        /**< Shifted mode DEFAULT for LESENSE_PERCTRL    */

/* Bit fields for LESENSE DECCTRL */
#define _LESENSE_DECCTRL_RESETVALUE                  0x00000000UL                             /**< Default value for LESENSE_DECCTRL           */
#define _LESENSE_DECCTRL_MASK                        0x000000FDUL                             /**< Mask for LESENSE_DECCTRL                    */
#define LESENSE_DECCTRL_DECDIS                       (0x1UL << 0)                             /**< Disable the decoder                         */
#define _LESENSE_DECCTRL_DECDIS_SHIFT                0                                        /**< Shift value for LESENSE_DECDIS              */
#define _LESENSE_DECCTRL_DECDIS_MASK                 0x1UL                                    /**< Bit mask for LESENSE_DECDIS                 */
#define _LESENSE_DECCTRL_DECDIS_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for LESENSE_DECCTRL            */
#define LESENSE_DECCTRL_DECDIS_DEFAULT               (_LESENSE_DECCTRL_DECDIS_DEFAULT << 0)   /**< Shifted mode DEFAULT for LESENSE_DECCTRL    */
#define LESENSE_DECCTRL_INTMAP                       (0x1UL << 2)                             /**< Enable decoder to channel interrupt map     */
#define _LESENSE_DECCTRL_INTMAP_SHIFT                2                                        /**< Shift value for LESENSE_INTMAP              */
#define _LESENSE_DECCTRL_INTMAP_MASK                 0x4UL                                    /**< Bit mask for LESENSE_INTMAP                 */
#define _LESENSE_DECCTRL_INTMAP_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for LESENSE_DECCTRL            */
#define LESENSE_DECCTRL_INTMAP_DEFAULT               (_LESENSE_DECCTRL_INTMAP_DEFAULT << 2)   /**< Shifted mode DEFAULT for LESENSE_DECCTRL    */
#define LESENSE_DECCTRL_HYSTPRS0                     (0x1UL << 3)                             /**< Enable decoder hysteresis on PRS0 output    */
#define _LESENSE_DECCTRL_HYSTPRS0_SHIFT              3                                        /**< Shift value for LESENSE_HYSTPRS0            */
#define _LESENSE_DECCTRL_HYSTPRS0_MASK               0x8UL                                    /**< Bit mask for LESENSE_HYSTPRS0               */
#define _LESENSE_DECCTRL_HYSTPRS0_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for LESENSE_DECCTRL            */
#define LESENSE_DECCTRL_HYSTPRS0_DEFAULT             (_LESENSE_DECCTRL_HYSTPRS0_DEFAULT << 3) /**< Shifted mode DEFAULT for LESENSE_DECCTRL    */
#define LESENSE_DECCTRL_HYSTPRS1                     (0x1UL << 4)                             /**< Enable decoder hysteresis on PRS1 output    */
#define _LESENSE_DECCTRL_HYSTPRS1_SHIFT              4                                        /**< Shift value for LESENSE_HYSTPRS1            */
#define _LESENSE_DECCTRL_HYSTPRS1_MASK               0x10UL                                   /**< Bit mask for LESENSE_HYSTPRS1               */
#define _LESENSE_DECCTRL_HYSTPRS1_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for LESENSE_DECCTRL            */
#define LESENSE_DECCTRL_HYSTPRS1_DEFAULT             (_LESENSE_DECCTRL_HYSTPRS1_DEFAULT << 4) /**< Shifted mode DEFAULT for LESENSE_DECCTRL    */
#define LESENSE_DECCTRL_HYSTPRS2                     (0x1UL << 5)                             /**< Enable decoder hysteresis on PRS2 output    */
#define _LESENSE_DECCTRL_HYSTPRS2_SHIFT              5                                        /**< Shift value for LESENSE_HYSTPRS2            */
#define _LESENSE_DECCTRL_HYSTPRS2_MASK               0x20UL                                   /**< Bit mask for LESENSE_HYSTPRS2               */
#define _LESENSE_DECCTRL_HYSTPRS2_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for LESENSE_DECCTRL            */
#define LESENSE_DECCTRL_HYSTPRS2_DEFAULT             (_LESENSE_DECCTRL_HYSTPRS2_DEFAULT << 5) /**< Shifted mode DEFAULT for LESENSE_DECCTRL    */
#define LESENSE_DECCTRL_HYSTIRQ                      (0x1UL << 6)                             /**< Enable decoder hysteresis on interrupt r    */
#define _LESENSE_DECCTRL_HYSTIRQ_SHIFT               6                                        /**< Shift value for LESENSE_HYSTIRQ             */
#define _LESENSE_DECCTRL_HYSTIRQ_MASK                0x40UL                                   /**< Bit mask for LESENSE_HYSTIRQ                */
#define _LESENSE_DECCTRL_HYSTIRQ_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for LESENSE_DECCTRL            */
#define LESENSE_DECCTRL_HYSTIRQ_DEFAULT              (_LESENSE_DECCTRL_HYSTIRQ_DEFAULT << 6)  /**< Shifted mode DEFAULT for LESENSE_DECCTRL    */
#define LESENSE_DECCTRL_PRSCNT                       (0x1UL << 7)                             /**< Enable count mode on decoder PRS channel    */
#define _LESENSE_DECCTRL_PRSCNT_SHIFT                7                                        /**< Shift value for LESENSE_PRSCNT              */
#define _LESENSE_DECCTRL_PRSCNT_MASK                 0x80UL                                   /**< Bit mask for LESENSE_PRSCNT                 */
#define _LESENSE_DECCTRL_PRSCNT_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for LESENSE_DECCTRL            */
#define LESENSE_DECCTRL_PRSCNT_DEFAULT               (_LESENSE_DECCTRL_PRSCNT_DEFAULT << 7)   /**< Shifted mode DEFAULT for LESENSE_DECCTRL    */

/* Bit fields for LESENSE EVALCTRL */
#define _LESENSE_EVALCTRL_RESETVALUE                 0x00000000UL                             /**< Default value for LESENSE_EVALCTRL          */
#define _LESENSE_EVALCTRL_MASK                       0x0000FFFFUL                             /**< Mask for LESENSE_EVALCTRL                   */
#define _LESENSE_EVALCTRL_WINSIZE_SHIFT              0                                        /**< Shift value for LESENSE_WINSIZE             */
#define _LESENSE_EVALCTRL_WINSIZE_MASK               0xFFFFUL                                 /**< Bit mask for LESENSE_WINSIZE                */
#define _LESENSE_EVALCTRL_WINSIZE_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for LESENSE_EVALCTRL           */
#define LESENSE_EVALCTRL_WINSIZE_DEFAULT             (_LESENSE_EVALCTRL_WINSIZE_DEFAULT << 0) /**< Shifted mode DEFAULT for LESENSE_EVALCTRL   */

/* Bit fields for LESENSE PRSCTRL */
#define _LESENSE_PRSCTRL_RESETVALUE                  0x00000000UL                               /**< Default value for LESENSE_PRSCTRL           */
#define _LESENSE_PRSCTRL_MASK                        0x00011F1FUL                               /**< Mask for LESENSE_PRSCTRL                    */
#define _LESENSE_PRSCTRL_DECCMPVAL_SHIFT             0                                          /**< Shift value for LESENSE_DECCMPVAL           */
#define _LESENSE_PRSCTRL_DECCMPVAL_MASK              0x1FUL                                     /**< Bit mask for LESENSE_DECCMPVAL              */
#define _LESENSE_PRSCTRL_DECCMPVAL_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LESENSE_PRSCTRL            */
#define LESENSE_PRSCTRL_DECCMPVAL_DEFAULT            (_LESENSE_PRSCTRL_DECCMPVAL_DEFAULT << 0)  /**< Shifted mode DEFAULT for LESENSE_PRSCTRL    */
#define _LESENSE_PRSCTRL_DECCMPMASK_SHIFT            8                                          /**< Shift value for LESENSE_DECCMPMASK          */
#define _LESENSE_PRSCTRL_DECCMPMASK_MASK             0x1F00UL                                   /**< Bit mask for LESENSE_DECCMPMASK             */
#define _LESENSE_PRSCTRL_DECCMPMASK_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for LESENSE_PRSCTRL            */
#define LESENSE_PRSCTRL_DECCMPMASK_DEFAULT           (_LESENSE_PRSCTRL_DECCMPMASK_DEFAULT << 8) /**< Shifted mode DEFAULT for LESENSE_PRSCTRL    */
#define LESENSE_PRSCTRL_DECCMPEN                     (0x1UL << 16)                              /**< Enable PRS output DECCMP                    */
#define _LESENSE_PRSCTRL_DECCMPEN_SHIFT              16                                         /**< Shift value for LESENSE_DECCMPEN            */
#define _LESENSE_PRSCTRL_DECCMPEN_MASK               0x10000UL                                  /**< Bit mask for LESENSE_DECCMPEN               */
#define _LESENSE_PRSCTRL_DECCMPEN_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_PRSCTRL            */
#define LESENSE_PRSCTRL_DECCMPEN_DEFAULT             (_LESENSE_PRSCTRL_DECCMPEN_DEFAULT << 16)  /**< Shifted mode DEFAULT for LESENSE_PRSCTRL    */

/* Bit fields for LESENSE CMD */
#define _LESENSE_CMD_RESETVALUE                      0x00000000UL                         /**< Default value for LESENSE_CMD               */
#define _LESENSE_CMD_MASK                            0x0000000FUL                         /**< Mask for LESENSE_CMD                        */
#define LESENSE_CMD_START                            (0x1UL << 0)                         /**< Start scanning of sensors.                  */
#define _LESENSE_CMD_START_SHIFT                     0                                    /**< Shift value for LESENSE_START               */
#define _LESENSE_CMD_START_MASK                      0x1UL                                /**< Bit mask for LESENSE_START                  */
#define _LESENSE_CMD_START_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for LESENSE_CMD                */
#define LESENSE_CMD_START_DEFAULT                    (_LESENSE_CMD_START_DEFAULT << 0)    /**< Shifted mode DEFAULT for LESENSE_CMD        */
#define LESENSE_CMD_STOP                             (0x1UL << 1)                         /**< Stop scanning of sensors                    */
#define _LESENSE_CMD_STOP_SHIFT                      1                                    /**< Shift value for LESENSE_STOP                */
#define _LESENSE_CMD_STOP_MASK                       0x2UL                                /**< Bit mask for LESENSE_STOP                   */
#define _LESENSE_CMD_STOP_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for LESENSE_CMD                */
#define LESENSE_CMD_STOP_DEFAULT                     (_LESENSE_CMD_STOP_DEFAULT << 1)     /**< Shifted mode DEFAULT for LESENSE_CMD        */
#define LESENSE_CMD_DECODE                           (0x1UL << 2)                         /**< Start decoder                               */
#define _LESENSE_CMD_DECODE_SHIFT                    2                                    /**< Shift value for LESENSE_DECODE              */
#define _LESENSE_CMD_DECODE_MASK                     0x4UL                                /**< Bit mask for LESENSE_DECODE                 */
#define _LESENSE_CMD_DECODE_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for LESENSE_CMD                */
#define LESENSE_CMD_DECODE_DEFAULT                   (_LESENSE_CMD_DECODE_DEFAULT << 2)   /**< Shifted mode DEFAULT for LESENSE_CMD        */
#define LESENSE_CMD_CLEARBUF                         (0x1UL << 3)                         /**< Clear result buffer                         */
#define _LESENSE_CMD_CLEARBUF_SHIFT                  3                                    /**< Shift value for LESENSE_CLEARBUF            */
#define _LESENSE_CMD_CLEARBUF_MASK                   0x8UL                                /**< Bit mask for LESENSE_CLEARBUF               */
#define _LESENSE_CMD_CLEARBUF_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for LESENSE_CMD                */
#define LESENSE_CMD_CLEARBUF_DEFAULT                 (_LESENSE_CMD_CLEARBUF_DEFAULT << 3) /**< Shifted mode DEFAULT for LESENSE_CMD        */

/* Bit fields for LESENSE CHEN */
#define _LESENSE_CHEN_RESETVALUE                     0x00000000UL                       /**< Default value for LESENSE_CHEN              */
#define _LESENSE_CHEN_MASK                           0x0000FFFFUL                       /**< Mask for LESENSE_CHEN                       */
#define _LESENSE_CHEN_CHEN_SHIFT                     0                                  /**< Shift value for LESENSE_CHEN                */
#define _LESENSE_CHEN_CHEN_MASK                      0xFFFFUL                           /**< Bit mask for LESENSE_CHEN                   */
#define _LESENSE_CHEN_CHEN_DEFAULT                   0x00000000UL                       /**< Mode DEFAULT for LESENSE_CHEN               */
#define LESENSE_CHEN_CHEN_DEFAULT                    (_LESENSE_CHEN_CHEN_DEFAULT << 0)  /**< Shifted mode DEFAULT for LESENSE_CHEN       */

/* Bit fields for LESENSE SCANRES */
#define _LESENSE_SCANRES_RESETVALUE                  0x00000000UL                             /**< Default value for LESENSE_SCANRES           */
#define _LESENSE_SCANRES_MASK                        0xFFFFFFFFUL                             /**< Mask for LESENSE_SCANRES                    */
#define _LESENSE_SCANRES_SCANRES_SHIFT               0                                        /**< Shift value for LESENSE_SCANRES             */
#define _LESENSE_SCANRES_SCANRES_MASK                0xFFFFUL                                 /**< Bit mask for LESENSE_SCANRES                */
#define _LESENSE_SCANRES_SCANRES_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for LESENSE_SCANRES            */
#define LESENSE_SCANRES_SCANRES_DEFAULT              (_LESENSE_SCANRES_SCANRES_DEFAULT << 0)  /**< Shifted mode DEFAULT for LESENSE_SCANRES    */
#define _LESENSE_SCANRES_STEPDIR_SHIFT               16                                       /**< Shift value for LESENSE_STEPDIR             */
#define _LESENSE_SCANRES_STEPDIR_MASK                0xFFFF0000UL                             /**< Bit mask for LESENSE_STEPDIR                */
#define _LESENSE_SCANRES_STEPDIR_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for LESENSE_SCANRES            */
#define LESENSE_SCANRES_STEPDIR_DEFAULT              (_LESENSE_SCANRES_STEPDIR_DEFAULT << 16) /**< Shifted mode DEFAULT for LESENSE_SCANRES    */

/* Bit fields for LESENSE STATUS */
#define _LESENSE_STATUS_RESETVALUE                   0x00000000UL                               /**< Default value for LESENSE_STATUS            */
#define _LESENSE_STATUS_MASK                         0x0000007BUL                               /**< Mask for LESENSE_STATUS                     */
#define LESENSE_STATUS_RESFIFOV                      (0x1UL << 0)                               /**< Result fifo valid                           */
#define _LESENSE_STATUS_RESFIFOV_SHIFT               0                                          /**< Shift value for LESENSE_RESFIFOV            */
#define _LESENSE_STATUS_RESFIFOV_MASK                0x1UL                                      /**< Bit mask for LESENSE_RESFIFOV               */
#define _LESENSE_STATUS_RESFIFOV_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LESENSE_STATUS             */
#define LESENSE_STATUS_RESFIFOV_DEFAULT              (_LESENSE_STATUS_RESFIFOV_DEFAULT << 0)    /**< Shifted mode DEFAULT for LESENSE_STATUS     */
#define LESENSE_STATUS_RESFIFOFULL                   (0x1UL << 1)                               /**< Result fifo full                            */
#define _LESENSE_STATUS_RESFIFOFULL_SHIFT            1                                          /**< Shift value for LESENSE_RESFIFOFULL         */
#define _LESENSE_STATUS_RESFIFOFULL_MASK             0x2UL                                      /**< Bit mask for LESENSE_RESFIFOFULL            */
#define _LESENSE_STATUS_RESFIFOFULL_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for LESENSE_STATUS             */
#define LESENSE_STATUS_RESFIFOFULL_DEFAULT           (_LESENSE_STATUS_RESFIFOFULL_DEFAULT << 1) /**< Shifted mode DEFAULT for LESENSE_STATUS     */
#define LESENSE_STATUS_SCANACTIVE                    (0x1UL << 3)                               /**< LESENSE scan active                         */
#define _LESENSE_STATUS_SCANACTIVE_SHIFT             3                                          /**< Shift value for LESENSE_SCANACTIVE          */
#define _LESENSE_STATUS_SCANACTIVE_MASK              0x8UL                                      /**< Bit mask for LESENSE_SCANACTIVE             */
#define _LESENSE_STATUS_SCANACTIVE_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LESENSE_STATUS             */
#define LESENSE_STATUS_SCANACTIVE_DEFAULT            (_LESENSE_STATUS_SCANACTIVE_DEFAULT << 3)  /**< Shifted mode DEFAULT for LESENSE_STATUS     */
#define LESENSE_STATUS_RUNNING                       (0x1UL << 4)                               /**< LESENSE periodic counter running            */
#define _LESENSE_STATUS_RUNNING_SHIFT                4                                          /**< Shift value for LESENSE_RUNNING             */
#define _LESENSE_STATUS_RUNNING_MASK                 0x10UL                                     /**< Bit mask for LESENSE_RUNNING                */
#define _LESENSE_STATUS_RUNNING_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for LESENSE_STATUS             */
#define LESENSE_STATUS_RUNNING_DEFAULT               (_LESENSE_STATUS_RUNNING_DEFAULT << 4)     /**< Shifted mode DEFAULT for LESENSE_STATUS     */
#define LESENSE_STATUS_READBUSY                      (0x1UL << 5)                               /**< FIFO Read Busy                              */
#define _LESENSE_STATUS_READBUSY_SHIFT               5                                          /**< Shift value for LESENSE_READBUSY            */
#define _LESENSE_STATUS_READBUSY_MASK                0x20UL                                     /**< Bit mask for LESENSE_READBUSY               */
#define _LESENSE_STATUS_READBUSY_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LESENSE_STATUS             */
#define LESENSE_STATUS_READBUSY_DEFAULT              (_LESENSE_STATUS_READBUSY_DEFAULT << 5)    /**< Shifted mode DEFAULT for LESENSE_STATUS     */
#define LESENSE_STATUS_FLUSHING                      (0x1UL << 6)                               /**< FIFO Flushing                               */
#define _LESENSE_STATUS_FLUSHING_SHIFT               6                                          /**< Shift value for LESENSE_FLUSHING            */
#define _LESENSE_STATUS_FLUSHING_MASK                0x40UL                                     /**< Bit mask for LESENSE_FLUSHING               */
#define _LESENSE_STATUS_FLUSHING_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LESENSE_STATUS             */
#define LESENSE_STATUS_FLUSHING_DEFAULT              (_LESENSE_STATUS_FLUSHING_DEFAULT << 6)    /**< Shifted mode DEFAULT for LESENSE_STATUS     */

/* Bit fields for LESENSE RESCOUNT */
#define _LESENSE_RESCOUNT_RESETVALUE                 0x00000000UL                           /**< Default value for LESENSE_RESCOUNT          */
#define _LESENSE_RESCOUNT_MASK                       0x0000001FUL                           /**< Mask for LESENSE_RESCOUNT                   */
#define _LESENSE_RESCOUNT_COUNT_SHIFT                0                                      /**< Shift value for LESENSE_COUNT               */
#define _LESENSE_RESCOUNT_COUNT_MASK                 0x1FUL                                 /**< Bit mask for LESENSE_COUNT                  */
#define _LESENSE_RESCOUNT_COUNT_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for LESENSE_RESCOUNT           */
#define LESENSE_RESCOUNT_COUNT_DEFAULT               (_LESENSE_RESCOUNT_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for LESENSE_RESCOUNT   */

/* Bit fields for LESENSE RESFIFO */
#define _LESENSE_RESFIFO_RESETVALUE                  0x00000000UL                               /**< Default value for LESENSE_RESFIFO           */
#define _LESENSE_RESFIFO_MASK                        0x000FFFFFUL                               /**< Mask for LESENSE_RESFIFO                    */
#define _LESENSE_RESFIFO_BUFDATASRC_SHIFT            0                                          /**< Shift value for LESENSE_BUFDATASRC          */
#define _LESENSE_RESFIFO_BUFDATASRC_MASK             0xFFFFFUL                                  /**< Bit mask for LESENSE_BUFDATASRC             */
#define _LESENSE_RESFIFO_BUFDATASRC_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for LESENSE_RESFIFO            */
#define LESENSE_RESFIFO_BUFDATASRC_DEFAULT           (_LESENSE_RESFIFO_BUFDATASRC_DEFAULT << 0) /**< Shifted mode DEFAULT for LESENSE_RESFIFO    */

/* Bit fields for LESENSE CURCH */
#define _LESENSE_CURCH_RESETVALUE                    0x00000000UL                        /**< Default value for LESENSE_CURCH             */
#define _LESENSE_CURCH_MASK                          0x0000000FUL                        /**< Mask for LESENSE_CURCH                      */
#define _LESENSE_CURCH_CURCH_SHIFT                   0                                   /**< Shift value for LESENSE_CURCH               */
#define _LESENSE_CURCH_CURCH_MASK                    0xFUL                               /**< Bit mask for LESENSE_CURCH                  */
#define _LESENSE_CURCH_CURCH_DEFAULT                 0x00000000UL                        /**< Mode DEFAULT for LESENSE_CURCH              */
#define LESENSE_CURCH_CURCH_DEFAULT                  (_LESENSE_CURCH_CURCH_DEFAULT << 0) /**< Shifted mode DEFAULT for LESENSE_CURCH      */

/* Bit fields for LESENSE DECSTATE */
#define _LESENSE_DECSTATE_RESETVALUE                 0x00000000UL                              /**< Default value for LESENSE_DECSTATE          */
#define _LESENSE_DECSTATE_MASK                       0x0000001FUL                              /**< Mask for LESENSE_DECSTATE                   */
#define _LESENSE_DECSTATE_DECSTATE_SHIFT             0                                         /**< Shift value for LESENSE_DECSTATE            */
#define _LESENSE_DECSTATE_DECSTATE_MASK              0x1FUL                                    /**< Bit mask for LESENSE_DECSTATE               */
#define _LESENSE_DECSTATE_DECSTATE_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for LESENSE_DECSTATE           */
#define LESENSE_DECSTATE_DECSTATE_DEFAULT            (_LESENSE_DECSTATE_DECSTATE_DEFAULT << 0) /**< Shifted mode DEFAULT for LESENSE_DECSTATE   */

/* Bit fields for LESENSE SENSORSTATE */
#define _LESENSE_SENSORSTATE_RESETVALUE              0x00000000UL                                    /**< Default value for LESENSE_SENSORSTATE       */
#define _LESENSE_SENSORSTATE_MASK                    0x0000000FUL                                    /**< Mask for LESENSE_SENSORSTATE                */
#define _LESENSE_SENSORSTATE_SENSORSTATE_SHIFT       0                                               /**< Shift value for LESENSE_SENSORSTATE         */
#define _LESENSE_SENSORSTATE_SENSORSTATE_MASK        0xFUL                                           /**< Bit mask for LESENSE_SENSORSTATE            */
#define _LESENSE_SENSORSTATE_SENSORSTATE_DEFAULT     0x00000000UL                                    /**< Mode DEFAULT for LESENSE_SENSORSTATE        */
#define LESENSE_SENSORSTATE_SENSORSTATE_DEFAULT      (_LESENSE_SENSORSTATE_SENSORSTATE_DEFAULT << 0) /**< Shifted mode DEFAULT for LESENSE_SENSORSTATE*/

/* Bit fields for LESENSE IDLECONF */
#define _LESENSE_IDLECONF_RESETVALUE                 0x00000000UL                               /**< Default value for LESENSE_IDLECONF          */
#define _LESENSE_IDLECONF_MASK                       0xFFFFFFFFUL                               /**< Mask for LESENSE_IDLECONF                   */
#define _LESENSE_IDLECONF_CHIDLE0_SHIFT              0                                          /**< Shift value for LESENSE_CHIDLE0             */
#define _LESENSE_IDLECONF_CHIDLE0_MASK               0x3UL                                      /**< Bit mask for LESENSE_CHIDLE0                */
#define _LESENSE_IDLECONF_CHIDLE0_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE0_DISABLE            0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE0_HIGH               0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE0_LOW                0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE0_DAC                0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE0_DEFAULT             (_LESENSE_IDLECONF_CHIDLE0_DEFAULT << 0)   /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE0_DISABLE             (_LESENSE_IDLECONF_CHIDLE0_DISABLE << 0)   /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE0_HIGH                (_LESENSE_IDLECONF_CHIDLE0_HIGH << 0)      /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE0_LOW                 (_LESENSE_IDLECONF_CHIDLE0_LOW << 0)       /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE0_DAC                 (_LESENSE_IDLECONF_CHIDLE0_DAC << 0)       /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE1_SHIFT              2                                          /**< Shift value for LESENSE_CHIDLE1             */
#define _LESENSE_IDLECONF_CHIDLE1_MASK               0xCUL                                      /**< Bit mask for LESENSE_CHIDLE1                */
#define _LESENSE_IDLECONF_CHIDLE1_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE1_DISABLE            0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE1_HIGH               0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE1_LOW                0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE1_DAC                0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE1_DEFAULT             (_LESENSE_IDLECONF_CHIDLE1_DEFAULT << 2)   /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE1_DISABLE             (_LESENSE_IDLECONF_CHIDLE1_DISABLE << 2)   /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE1_HIGH                (_LESENSE_IDLECONF_CHIDLE1_HIGH << 2)      /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE1_LOW                 (_LESENSE_IDLECONF_CHIDLE1_LOW << 2)       /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE1_DAC                 (_LESENSE_IDLECONF_CHIDLE1_DAC << 2)       /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE2_SHIFT              4                                          /**< Shift value for LESENSE_CHIDLE2             */
#define _LESENSE_IDLECONF_CHIDLE2_MASK               0x30UL                                     /**< Bit mask for LESENSE_CHIDLE2                */
#define _LESENSE_IDLECONF_CHIDLE2_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE2_DISABLE            0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE2_HIGH               0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE2_LOW                0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE2_DAC                0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE2_DEFAULT             (_LESENSE_IDLECONF_CHIDLE2_DEFAULT << 4)   /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE2_DISABLE             (_LESENSE_IDLECONF_CHIDLE2_DISABLE << 4)   /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE2_HIGH                (_LESENSE_IDLECONF_CHIDLE2_HIGH << 4)      /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE2_LOW                 (_LESENSE_IDLECONF_CHIDLE2_LOW << 4)       /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE2_DAC                 (_LESENSE_IDLECONF_CHIDLE2_DAC << 4)       /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE3_SHIFT              6                                          /**< Shift value for LESENSE_CHIDLE3             */
#define _LESENSE_IDLECONF_CHIDLE3_MASK               0xC0UL                                     /**< Bit mask for LESENSE_CHIDLE3                */
#define _LESENSE_IDLECONF_CHIDLE3_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE3_DISABLE            0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE3_HIGH               0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE3_LOW                0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE3_DAC                0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE3_DEFAULT             (_LESENSE_IDLECONF_CHIDLE3_DEFAULT << 6)   /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE3_DISABLE             (_LESENSE_IDLECONF_CHIDLE3_DISABLE << 6)   /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE3_HIGH                (_LESENSE_IDLECONF_CHIDLE3_HIGH << 6)      /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE3_LOW                 (_LESENSE_IDLECONF_CHIDLE3_LOW << 6)       /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE3_DAC                 (_LESENSE_IDLECONF_CHIDLE3_DAC << 6)       /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE4_SHIFT              8                                          /**< Shift value for LESENSE_CHIDLE4             */
#define _LESENSE_IDLECONF_CHIDLE4_MASK               0x300UL                                    /**< Bit mask for LESENSE_CHIDLE4                */
#define _LESENSE_IDLECONF_CHIDLE4_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE4_DISABLE            0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE4_HIGH               0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE4_LOW                0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE4_DAC                0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE4_DEFAULT             (_LESENSE_IDLECONF_CHIDLE4_DEFAULT << 8)   /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE4_DISABLE             (_LESENSE_IDLECONF_CHIDLE4_DISABLE << 8)   /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE4_HIGH                (_LESENSE_IDLECONF_CHIDLE4_HIGH << 8)      /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE4_LOW                 (_LESENSE_IDLECONF_CHIDLE4_LOW << 8)       /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE4_DAC                 (_LESENSE_IDLECONF_CHIDLE4_DAC << 8)       /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE5_SHIFT              10                                         /**< Shift value for LESENSE_CHIDLE5             */
#define _LESENSE_IDLECONF_CHIDLE5_MASK               0xC00UL                                    /**< Bit mask for LESENSE_CHIDLE5                */
#define _LESENSE_IDLECONF_CHIDLE5_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE5_DISABLE            0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE5_HIGH               0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE5_LOW                0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE5_DAC                0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE5_DEFAULT             (_LESENSE_IDLECONF_CHIDLE5_DEFAULT << 10)  /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE5_DISABLE             (_LESENSE_IDLECONF_CHIDLE5_DISABLE << 10)  /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE5_HIGH                (_LESENSE_IDLECONF_CHIDLE5_HIGH << 10)     /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE5_LOW                 (_LESENSE_IDLECONF_CHIDLE5_LOW << 10)      /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE5_DAC                 (_LESENSE_IDLECONF_CHIDLE5_DAC << 10)      /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE6_SHIFT              12                                         /**< Shift value for LESENSE_CHIDLE6             */
#define _LESENSE_IDLECONF_CHIDLE6_MASK               0x3000UL                                   /**< Bit mask for LESENSE_CHIDLE6                */
#define _LESENSE_IDLECONF_CHIDLE6_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE6_DISABLE            0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE6_HIGH               0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE6_LOW                0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE6_DAC                0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE6_DEFAULT             (_LESENSE_IDLECONF_CHIDLE6_DEFAULT << 12)  /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE6_DISABLE             (_LESENSE_IDLECONF_CHIDLE6_DISABLE << 12)  /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE6_HIGH                (_LESENSE_IDLECONF_CHIDLE6_HIGH << 12)     /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE6_LOW                 (_LESENSE_IDLECONF_CHIDLE6_LOW << 12)      /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE6_DAC                 (_LESENSE_IDLECONF_CHIDLE6_DAC << 12)      /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE7_SHIFT              14                                         /**< Shift value for LESENSE_CHIDLE7             */
#define _LESENSE_IDLECONF_CHIDLE7_MASK               0xC000UL                                   /**< Bit mask for LESENSE_CHIDLE7                */
#define _LESENSE_IDLECONF_CHIDLE7_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE7_DISABLE            0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE7_HIGH               0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE7_LOW                0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE7_DAC                0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE7_DEFAULT             (_LESENSE_IDLECONF_CHIDLE7_DEFAULT << 14)  /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE7_DISABLE             (_LESENSE_IDLECONF_CHIDLE7_DISABLE << 14)  /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE7_HIGH                (_LESENSE_IDLECONF_CHIDLE7_HIGH << 14)     /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE7_LOW                 (_LESENSE_IDLECONF_CHIDLE7_LOW << 14)      /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE7_DAC                 (_LESENSE_IDLECONF_CHIDLE7_DAC << 14)      /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE8_SHIFT              16                                         /**< Shift value for LESENSE_CHIDLE8             */
#define _LESENSE_IDLECONF_CHIDLE8_MASK               0x30000UL                                  /**< Bit mask for LESENSE_CHIDLE8                */
#define _LESENSE_IDLECONF_CHIDLE8_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE8_DISABLE            0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE8_HIGH               0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE8_LOW                0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE8_DAC                0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE8_DEFAULT             (_LESENSE_IDLECONF_CHIDLE8_DEFAULT << 16)  /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE8_DISABLE             (_LESENSE_IDLECONF_CHIDLE8_DISABLE << 16)  /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE8_HIGH                (_LESENSE_IDLECONF_CHIDLE8_HIGH << 16)     /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE8_LOW                 (_LESENSE_IDLECONF_CHIDLE8_LOW << 16)      /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE8_DAC                 (_LESENSE_IDLECONF_CHIDLE8_DAC << 16)      /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE9_SHIFT              18                                         /**< Shift value for LESENSE_CHIDLE9             */
#define _LESENSE_IDLECONF_CHIDLE9_MASK               0xC0000UL                                  /**< Bit mask for LESENSE_CHIDLE9                */
#define _LESENSE_IDLECONF_CHIDLE9_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE9_DISABLE            0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE9_HIGH               0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE9_LOW                0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE9_DAC                0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE9_DEFAULT             (_LESENSE_IDLECONF_CHIDLE9_DEFAULT << 18)  /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE9_DISABLE             (_LESENSE_IDLECONF_CHIDLE9_DISABLE << 18)  /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE9_HIGH                (_LESENSE_IDLECONF_CHIDLE9_HIGH << 18)     /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE9_LOW                 (_LESENSE_IDLECONF_CHIDLE9_LOW << 18)      /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE9_DAC                 (_LESENSE_IDLECONF_CHIDLE9_DAC << 18)      /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE10_SHIFT             20                                         /**< Shift value for LESENSE_CHIDLE10            */
#define _LESENSE_IDLECONF_CHIDLE10_MASK              0x300000UL                                 /**< Bit mask for LESENSE_CHIDLE10               */
#define _LESENSE_IDLECONF_CHIDLE10_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE10_DISABLE           0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE10_HIGH              0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE10_LOW               0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE10_DAC               0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE10_DEFAULT            (_LESENSE_IDLECONF_CHIDLE10_DEFAULT << 20) /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE10_DISABLE            (_LESENSE_IDLECONF_CHIDLE10_DISABLE << 20) /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE10_HIGH               (_LESENSE_IDLECONF_CHIDLE10_HIGH << 20)    /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE10_LOW                (_LESENSE_IDLECONF_CHIDLE10_LOW << 20)     /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE10_DAC                (_LESENSE_IDLECONF_CHIDLE10_DAC << 20)     /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE11_SHIFT             22                                         /**< Shift value for LESENSE_CHIDLE11            */
#define _LESENSE_IDLECONF_CHIDLE11_MASK              0xC00000UL                                 /**< Bit mask for LESENSE_CHIDLE11               */
#define _LESENSE_IDLECONF_CHIDLE11_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE11_DISABLE           0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE11_HIGH              0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE11_LOW               0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE11_DAC               0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE11_DEFAULT            (_LESENSE_IDLECONF_CHIDLE11_DEFAULT << 22) /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE11_DISABLE            (_LESENSE_IDLECONF_CHIDLE11_DISABLE << 22) /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE11_HIGH               (_LESENSE_IDLECONF_CHIDLE11_HIGH << 22)    /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE11_LOW                (_LESENSE_IDLECONF_CHIDLE11_LOW << 22)     /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE11_DAC                (_LESENSE_IDLECONF_CHIDLE11_DAC << 22)     /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE12_SHIFT             24                                         /**< Shift value for LESENSE_CHIDLE12            */
#define _LESENSE_IDLECONF_CHIDLE12_MASK              0x3000000UL                                /**< Bit mask for LESENSE_CHIDLE12               */
#define _LESENSE_IDLECONF_CHIDLE12_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE12_DISABLE           0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE12_HIGH              0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE12_LOW               0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE12_DAC               0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE12_DEFAULT            (_LESENSE_IDLECONF_CHIDLE12_DEFAULT << 24) /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE12_DISABLE            (_LESENSE_IDLECONF_CHIDLE12_DISABLE << 24) /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE12_HIGH               (_LESENSE_IDLECONF_CHIDLE12_HIGH << 24)    /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE12_LOW                (_LESENSE_IDLECONF_CHIDLE12_LOW << 24)     /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE12_DAC                (_LESENSE_IDLECONF_CHIDLE12_DAC << 24)     /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE13_SHIFT             26                                         /**< Shift value for LESENSE_CHIDLE13            */
#define _LESENSE_IDLECONF_CHIDLE13_MASK              0xC000000UL                                /**< Bit mask for LESENSE_CHIDLE13               */
#define _LESENSE_IDLECONF_CHIDLE13_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE13_DISABLE           0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE13_HIGH              0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE13_LOW               0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE13_DAC               0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE13_DEFAULT            (_LESENSE_IDLECONF_CHIDLE13_DEFAULT << 26) /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE13_DISABLE            (_LESENSE_IDLECONF_CHIDLE13_DISABLE << 26) /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE13_HIGH               (_LESENSE_IDLECONF_CHIDLE13_HIGH << 26)    /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE13_LOW                (_LESENSE_IDLECONF_CHIDLE13_LOW << 26)     /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE13_DAC                (_LESENSE_IDLECONF_CHIDLE13_DAC << 26)     /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE14_SHIFT             28                                         /**< Shift value for LESENSE_CHIDLE14            */
#define _LESENSE_IDLECONF_CHIDLE14_MASK              0x30000000UL                               /**< Bit mask for LESENSE_CHIDLE14               */
#define _LESENSE_IDLECONF_CHIDLE14_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE14_DISABLE           0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE14_HIGH              0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE14_LOW               0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE14_DAC               0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE14_DEFAULT            (_LESENSE_IDLECONF_CHIDLE14_DEFAULT << 28) /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE14_DISABLE            (_LESENSE_IDLECONF_CHIDLE14_DISABLE << 28) /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE14_HIGH               (_LESENSE_IDLECONF_CHIDLE14_HIGH << 28)    /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE14_LOW                (_LESENSE_IDLECONF_CHIDLE14_LOW << 28)     /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE14_DAC                (_LESENSE_IDLECONF_CHIDLE14_DAC << 28)     /**< Shifted mode DAC for LESENSE_IDLECONF       */
#define _LESENSE_IDLECONF_CHIDLE15_SHIFT             30                                         /**< Shift value for LESENSE_CHIDLE15            */
#define _LESENSE_IDLECONF_CHIDLE15_MASK              0xC0000000UL                               /**< Bit mask for LESENSE_CHIDLE15               */
#define _LESENSE_IDLECONF_CHIDLE15_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE15_DISABLE           0x00000000UL                               /**< Mode DISABLE for LESENSE_IDLECONF           */
#define _LESENSE_IDLECONF_CHIDLE15_HIGH              0x00000001UL                               /**< Mode HIGH for LESENSE_IDLECONF              */
#define _LESENSE_IDLECONF_CHIDLE15_LOW               0x00000002UL                               /**< Mode LOW for LESENSE_IDLECONF               */
#define _LESENSE_IDLECONF_CHIDLE15_DAC               0x00000003UL                               /**< Mode DAC for LESENSE_IDLECONF               */
#define LESENSE_IDLECONF_CHIDLE15_DEFAULT            (_LESENSE_IDLECONF_CHIDLE15_DEFAULT << 30) /**< Shifted mode DEFAULT for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE15_DISABLE            (_LESENSE_IDLECONF_CHIDLE15_DISABLE << 30) /**< Shifted mode DISABLE for LESENSE_IDLECONF   */
#define LESENSE_IDLECONF_CHIDLE15_HIGH               (_LESENSE_IDLECONF_CHIDLE15_HIGH << 30)    /**< Shifted mode HIGH for LESENSE_IDLECONF      */
#define LESENSE_IDLECONF_CHIDLE15_LOW                (_LESENSE_IDLECONF_CHIDLE15_LOW << 30)     /**< Shifted mode LOW for LESENSE_IDLECONF       */
#define LESENSE_IDLECONF_CHIDLE15_DAC                (_LESENSE_IDLECONF_CHIDLE15_DAC << 30)     /**< Shifted mode DAC for LESENSE_IDLECONF       */

/* Bit fields for LESENSE SYNCBUSY */
#define _LESENSE_SYNCBUSY_RESETVALUE                 0x00000000UL                         /**< Default value for LESENSE_SYNCBUSY          */
#define _LESENSE_SYNCBUSY_MASK                       0x00000001UL                         /**< Mask for LESENSE_SYNCBUSY                   */
#define LESENSE_SYNCBUSY_CMD                         (0x1UL << 0)                         /**< Command                                     */
#define _LESENSE_SYNCBUSY_CMD_SHIFT                  0                                    /**< Shift value for LESENSE_CMD                 */
#define _LESENSE_SYNCBUSY_CMD_MASK                   0x1UL                                /**< Bit mask for LESENSE_CMD                    */
#define _LESENSE_SYNCBUSY_CMD_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for LESENSE_SYNCBUSY           */
#define LESENSE_SYNCBUSY_CMD_DEFAULT                 (_LESENSE_SYNCBUSY_CMD_DEFAULT << 0) /**< Shifted mode DEFAULT for LESENSE_SYNCBUSY   */

/* Bit fields for LESENSE IF */
#define _LESENSE_IF_RESETVALUE                       0x00000000UL                         /**< Default value for LESENSE_IF                */
#define _LESENSE_IF_MASK                             0x003FFFFFUL                         /**< Mask for LESENSE_IF                         */
#define LESENSE_IF_CH0                               (0x1UL << 0)                         /**< Channel                                     */
#define _LESENSE_IF_CH0_SHIFT                        0                                    /**< Shift value for LESENSE_CH0                 */
#define _LESENSE_IF_CH0_MASK                         0x1UL                                /**< Bit mask for LESENSE_CH0                    */
#define _LESENSE_IF_CH0_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH0_DEFAULT                       (_LESENSE_IF_CH0_DEFAULT << 0)       /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH1                               (0x1UL << 1)                         /**< Channel                                     */
#define _LESENSE_IF_CH1_SHIFT                        1                                    /**< Shift value for LESENSE_CH1                 */
#define _LESENSE_IF_CH1_MASK                         0x2UL                                /**< Bit mask for LESENSE_CH1                    */
#define _LESENSE_IF_CH1_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH1_DEFAULT                       (_LESENSE_IF_CH1_DEFAULT << 1)       /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH2                               (0x1UL << 2)                         /**< Channel                                     */
#define _LESENSE_IF_CH2_SHIFT                        2                                    /**< Shift value for LESENSE_CH2                 */
#define _LESENSE_IF_CH2_MASK                         0x4UL                                /**< Bit mask for LESENSE_CH2                    */
#define _LESENSE_IF_CH2_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH2_DEFAULT                       (_LESENSE_IF_CH2_DEFAULT << 2)       /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH3                               (0x1UL << 3)                         /**< Channel                                     */
#define _LESENSE_IF_CH3_SHIFT                        3                                    /**< Shift value for LESENSE_CH3                 */
#define _LESENSE_IF_CH3_MASK                         0x8UL                                /**< Bit mask for LESENSE_CH3                    */
#define _LESENSE_IF_CH3_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH3_DEFAULT                       (_LESENSE_IF_CH3_DEFAULT << 3)       /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH4                               (0x1UL << 4)                         /**< Channel                                     */
#define _LESENSE_IF_CH4_SHIFT                        4                                    /**< Shift value for LESENSE_CH4                 */
#define _LESENSE_IF_CH4_MASK                         0x10UL                               /**< Bit mask for LESENSE_CH4                    */
#define _LESENSE_IF_CH4_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH4_DEFAULT                       (_LESENSE_IF_CH4_DEFAULT << 4)       /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH5                               (0x1UL << 5)                         /**< Channel                                     */
#define _LESENSE_IF_CH5_SHIFT                        5                                    /**< Shift value for LESENSE_CH5                 */
#define _LESENSE_IF_CH5_MASK                         0x20UL                               /**< Bit mask for LESENSE_CH5                    */
#define _LESENSE_IF_CH5_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH5_DEFAULT                       (_LESENSE_IF_CH5_DEFAULT << 5)       /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH6                               (0x1UL << 6)                         /**< Channel                                     */
#define _LESENSE_IF_CH6_SHIFT                        6                                    /**< Shift value for LESENSE_CH6                 */
#define _LESENSE_IF_CH6_MASK                         0x40UL                               /**< Bit mask for LESENSE_CH6                    */
#define _LESENSE_IF_CH6_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH6_DEFAULT                       (_LESENSE_IF_CH6_DEFAULT << 6)       /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH7                               (0x1UL << 7)                         /**< Channel                                     */
#define _LESENSE_IF_CH7_SHIFT                        7                                    /**< Shift value for LESENSE_CH7                 */
#define _LESENSE_IF_CH7_MASK                         0x80UL                               /**< Bit mask for LESENSE_CH7                    */
#define _LESENSE_IF_CH7_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH7_DEFAULT                       (_LESENSE_IF_CH7_DEFAULT << 7)       /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH8                               (0x1UL << 8)                         /**< Channel                                     */
#define _LESENSE_IF_CH8_SHIFT                        8                                    /**< Shift value for LESENSE_CH8                 */
#define _LESENSE_IF_CH8_MASK                         0x100UL                              /**< Bit mask for LESENSE_CH8                    */
#define _LESENSE_IF_CH8_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH8_DEFAULT                       (_LESENSE_IF_CH8_DEFAULT << 8)       /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH9                               (0x1UL << 9)                         /**< Channel                                     */
#define _LESENSE_IF_CH9_SHIFT                        9                                    /**< Shift value for LESENSE_CH9                 */
#define _LESENSE_IF_CH9_MASK                         0x200UL                              /**< Bit mask for LESENSE_CH9                    */
#define _LESENSE_IF_CH9_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH9_DEFAULT                       (_LESENSE_IF_CH9_DEFAULT << 9)       /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH10                              (0x1UL << 10)                        /**< Channel                                     */
#define _LESENSE_IF_CH10_SHIFT                       10                                   /**< Shift value for LESENSE_CH10                */
#define _LESENSE_IF_CH10_MASK                        0x400UL                              /**< Bit mask for LESENSE_CH10                   */
#define _LESENSE_IF_CH10_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH10_DEFAULT                      (_LESENSE_IF_CH10_DEFAULT << 10)     /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH11                              (0x1UL << 11)                        /**< Channel                                     */
#define _LESENSE_IF_CH11_SHIFT                       11                                   /**< Shift value for LESENSE_CH11                */
#define _LESENSE_IF_CH11_MASK                        0x800UL                              /**< Bit mask for LESENSE_CH11                   */
#define _LESENSE_IF_CH11_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH11_DEFAULT                      (_LESENSE_IF_CH11_DEFAULT << 11)     /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH12                              (0x1UL << 12)                        /**< Channel                                     */
#define _LESENSE_IF_CH12_SHIFT                       12                                   /**< Shift value for LESENSE_CH12                */
#define _LESENSE_IF_CH12_MASK                        0x1000UL                             /**< Bit mask for LESENSE_CH12                   */
#define _LESENSE_IF_CH12_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH12_DEFAULT                      (_LESENSE_IF_CH12_DEFAULT << 12)     /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH13                              (0x1UL << 13)                        /**< Channel                                     */
#define _LESENSE_IF_CH13_SHIFT                       13                                   /**< Shift value for LESENSE_CH13                */
#define _LESENSE_IF_CH13_MASK                        0x2000UL                             /**< Bit mask for LESENSE_CH13                   */
#define _LESENSE_IF_CH13_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH13_DEFAULT                      (_LESENSE_IF_CH13_DEFAULT << 13)     /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH14                              (0x1UL << 14)                        /**< Channel                                     */
#define _LESENSE_IF_CH14_SHIFT                       14                                   /**< Shift value for LESENSE_CH14                */
#define _LESENSE_IF_CH14_MASK                        0x4000UL                             /**< Bit mask for LESENSE_CH14                   */
#define _LESENSE_IF_CH14_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH14_DEFAULT                      (_LESENSE_IF_CH14_DEFAULT << 14)     /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CH15                              (0x1UL << 15)                        /**< Channel                                     */
#define _LESENSE_IF_CH15_SHIFT                       15                                   /**< Shift value for LESENSE_CH15                */
#define _LESENSE_IF_CH15_MASK                        0x8000UL                             /**< Bit mask for LESENSE_CH15                   */
#define _LESENSE_IF_CH15_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CH15_DEFAULT                      (_LESENSE_IF_CH15_DEFAULT << 15)     /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_SCANDONE                          (0x1UL << 16)                        /**< Scan Done                                   */
#define _LESENSE_IF_SCANDONE_SHIFT                   16                                   /**< Shift value for LESENSE_SCANDONE            */
#define _LESENSE_IF_SCANDONE_MASK                    0x10000UL                            /**< Bit mask for LESENSE_SCANDONE               */
#define _LESENSE_IF_SCANDONE_DEFAULT                 0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_SCANDONE_DEFAULT                  (_LESENSE_IF_SCANDONE_DEFAULT << 16) /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_DEC                               (0x1UL << 17)                        /**< Decoder                                     */
#define _LESENSE_IF_DEC_SHIFT                        17                                   /**< Shift value for LESENSE_DEC                 */
#define _LESENSE_IF_DEC_MASK                         0x20000UL                            /**< Bit mask for LESENSE_DEC                    */
#define _LESENSE_IF_DEC_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_DEC_DEFAULT                       (_LESENSE_IF_DEC_DEFAULT << 17)      /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_RESWL                             (0x1UL << 18)                        /**< Result Watermark Level                      */
#define _LESENSE_IF_RESWL_SHIFT                      18                                   /**< Shift value for LESENSE_RESWL               */
#define _LESENSE_IF_RESWL_MASK                       0x40000UL                            /**< Bit mask for LESENSE_RESWL                  */
#define _LESENSE_IF_RESWL_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_RESWL_DEFAULT                     (_LESENSE_IF_RESWL_DEFAULT << 18)    /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_RESOF                             (0x1UL << 19)                        /**< Result Overflow                             */
#define _LESENSE_IF_RESOF_SHIFT                      19                                   /**< Shift value for LESENSE_RESOF               */
#define _LESENSE_IF_RESOF_MASK                       0x80000UL                            /**< Bit mask for LESENSE_RESOF                  */
#define _LESENSE_IF_RESOF_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_RESOF_DEFAULT                     (_LESENSE_IF_RESOF_DEFAULT << 19)    /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_CNTOF                             (0x1UL << 20)                        /**< Counter Overflow                            */
#define _LESENSE_IF_CNTOF_SHIFT                      20                                   /**< Shift value for LESENSE_CNTOF               */
#define _LESENSE_IF_CNTOF_MASK                       0x100000UL                           /**< Bit mask for LESENSE_CNTOF                  */
#define _LESENSE_IF_CNTOF_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_CNTOF_DEFAULT                     (_LESENSE_IF_CNTOF_DEFAULT << 20)    /**< Shifted mode DEFAULT for LESENSE_IF         */
#define LESENSE_IF_RESUF                             (0x1UL << 21)                        /**< Result Underflow                            */
#define _LESENSE_IF_RESUF_SHIFT                      21                                   /**< Shift value for LESENSE_RESUF               */
#define _LESENSE_IF_RESUF_MASK                       0x200000UL                           /**< Bit mask for LESENSE_RESUF                  */
#define _LESENSE_IF_RESUF_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for LESENSE_IF                 */
#define LESENSE_IF_RESUF_DEFAULT                     (_LESENSE_IF_RESUF_DEFAULT << 21)    /**< Shifted mode DEFAULT for LESENSE_IF         */

/* Bit fields for LESENSE IEN */
#define _LESENSE_IEN_RESETVALUE                      0x00000000UL                          /**< Default value for LESENSE_IEN               */
#define _LESENSE_IEN_MASK                            0x003FFFFFUL                          /**< Mask for LESENSE_IEN                        */
#define LESENSE_IEN_CH0                              (0x1UL << 0)                          /**< Channel                                     */
#define _LESENSE_IEN_CH0_SHIFT                       0                                     /**< Shift value for LESENSE_CH0                 */
#define _LESENSE_IEN_CH0_MASK                        0x1UL                                 /**< Bit mask for LESENSE_CH0                    */
#define _LESENSE_IEN_CH0_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH0_DEFAULT                      (_LESENSE_IEN_CH0_DEFAULT << 0)       /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH1                              (0x1UL << 1)                          /**< Channel                                     */
#define _LESENSE_IEN_CH1_SHIFT                       1                                     /**< Shift value for LESENSE_CH1                 */
#define _LESENSE_IEN_CH1_MASK                        0x2UL                                 /**< Bit mask for LESENSE_CH1                    */
#define _LESENSE_IEN_CH1_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH1_DEFAULT                      (_LESENSE_IEN_CH1_DEFAULT << 1)       /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH2                              (0x1UL << 2)                          /**< Channel                                     */
#define _LESENSE_IEN_CH2_SHIFT                       2                                     /**< Shift value for LESENSE_CH2                 */
#define _LESENSE_IEN_CH2_MASK                        0x4UL                                 /**< Bit mask for LESENSE_CH2                    */
#define _LESENSE_IEN_CH2_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH2_DEFAULT                      (_LESENSE_IEN_CH2_DEFAULT << 2)       /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH3                              (0x1UL << 3)                          /**< Channel                                     */
#define _LESENSE_IEN_CH3_SHIFT                       3                                     /**< Shift value for LESENSE_CH3                 */
#define _LESENSE_IEN_CH3_MASK                        0x8UL                                 /**< Bit mask for LESENSE_CH3                    */
#define _LESENSE_IEN_CH3_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH3_DEFAULT                      (_LESENSE_IEN_CH3_DEFAULT << 3)       /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH4                              (0x1UL << 4)                          /**< Channel                                     */
#define _LESENSE_IEN_CH4_SHIFT                       4                                     /**< Shift value for LESENSE_CH4                 */
#define _LESENSE_IEN_CH4_MASK                        0x10UL                                /**< Bit mask for LESENSE_CH4                    */
#define _LESENSE_IEN_CH4_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH4_DEFAULT                      (_LESENSE_IEN_CH4_DEFAULT << 4)       /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH5                              (0x1UL << 5)                          /**< Channel                                     */
#define _LESENSE_IEN_CH5_SHIFT                       5                                     /**< Shift value for LESENSE_CH5                 */
#define _LESENSE_IEN_CH5_MASK                        0x20UL                                /**< Bit mask for LESENSE_CH5                    */
#define _LESENSE_IEN_CH5_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH5_DEFAULT                      (_LESENSE_IEN_CH5_DEFAULT << 5)       /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH6                              (0x1UL << 6)                          /**< Channel                                     */
#define _LESENSE_IEN_CH6_SHIFT                       6                                     /**< Shift value for LESENSE_CH6                 */
#define _LESENSE_IEN_CH6_MASK                        0x40UL                                /**< Bit mask for LESENSE_CH6                    */
#define _LESENSE_IEN_CH6_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH6_DEFAULT                      (_LESENSE_IEN_CH6_DEFAULT << 6)       /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH7                              (0x1UL << 7)                          /**< Channel                                     */
#define _LESENSE_IEN_CH7_SHIFT                       7                                     /**< Shift value for LESENSE_CH7                 */
#define _LESENSE_IEN_CH7_MASK                        0x80UL                                /**< Bit mask for LESENSE_CH7                    */
#define _LESENSE_IEN_CH7_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH7_DEFAULT                      (_LESENSE_IEN_CH7_DEFAULT << 7)       /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH8                              (0x1UL << 8)                          /**< Channel                                     */
#define _LESENSE_IEN_CH8_SHIFT                       8                                     /**< Shift value for LESENSE_CH8                 */
#define _LESENSE_IEN_CH8_MASK                        0x100UL                               /**< Bit mask for LESENSE_CH8                    */
#define _LESENSE_IEN_CH8_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH8_DEFAULT                      (_LESENSE_IEN_CH8_DEFAULT << 8)       /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH9                              (0x1UL << 9)                          /**< Channel                                     */
#define _LESENSE_IEN_CH9_SHIFT                       9                                     /**< Shift value for LESENSE_CH9                 */
#define _LESENSE_IEN_CH9_MASK                        0x200UL                               /**< Bit mask for LESENSE_CH9                    */
#define _LESENSE_IEN_CH9_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH9_DEFAULT                      (_LESENSE_IEN_CH9_DEFAULT << 9)       /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH10                             (0x1UL << 10)                         /**< Channel                                     */
#define _LESENSE_IEN_CH10_SHIFT                      10                                    /**< Shift value for LESENSE_CH10                */
#define _LESENSE_IEN_CH10_MASK                       0x400UL                               /**< Bit mask for LESENSE_CH10                   */
#define _LESENSE_IEN_CH10_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH10_DEFAULT                     (_LESENSE_IEN_CH10_DEFAULT << 10)     /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH11                             (0x1UL << 11)                         /**< Channel                                     */
#define _LESENSE_IEN_CH11_SHIFT                      11                                    /**< Shift value for LESENSE_CH11                */
#define _LESENSE_IEN_CH11_MASK                       0x800UL                               /**< Bit mask for LESENSE_CH11                   */
#define _LESENSE_IEN_CH11_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH11_DEFAULT                     (_LESENSE_IEN_CH11_DEFAULT << 11)     /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH12                             (0x1UL << 12)                         /**< Channel                                     */
#define _LESENSE_IEN_CH12_SHIFT                      12                                    /**< Shift value for LESENSE_CH12                */
#define _LESENSE_IEN_CH12_MASK                       0x1000UL                              /**< Bit mask for LESENSE_CH12                   */
#define _LESENSE_IEN_CH12_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH12_DEFAULT                     (_LESENSE_IEN_CH12_DEFAULT << 12)     /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH13                             (0x1UL << 13)                         /**< Channel                                     */
#define _LESENSE_IEN_CH13_SHIFT                      13                                    /**< Shift value for LESENSE_CH13                */
#define _LESENSE_IEN_CH13_MASK                       0x2000UL                              /**< Bit mask for LESENSE_CH13                   */
#define _LESENSE_IEN_CH13_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH13_DEFAULT                     (_LESENSE_IEN_CH13_DEFAULT << 13)     /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH14                             (0x1UL << 14)                         /**< Channel                                     */
#define _LESENSE_IEN_CH14_SHIFT                      14                                    /**< Shift value for LESENSE_CH14                */
#define _LESENSE_IEN_CH14_MASK                       0x4000UL                              /**< Bit mask for LESENSE_CH14                   */
#define _LESENSE_IEN_CH14_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH14_DEFAULT                     (_LESENSE_IEN_CH14_DEFAULT << 14)     /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CH15                             (0x1UL << 15)                         /**< Channel                                     */
#define _LESENSE_IEN_CH15_SHIFT                      15                                    /**< Shift value for LESENSE_CH15                */
#define _LESENSE_IEN_CH15_MASK                       0x8000UL                              /**< Bit mask for LESENSE_CH15                   */
#define _LESENSE_IEN_CH15_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CH15_DEFAULT                     (_LESENSE_IEN_CH15_DEFAULT << 15)     /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_SCANDONE                         (0x1UL << 16)                         /**< Scan Complete                               */
#define _LESENSE_IEN_SCANDONE_SHIFT                  16                                    /**< Shift value for LESENSE_SCANDONE            */
#define _LESENSE_IEN_SCANDONE_MASK                   0x10000UL                             /**< Bit mask for LESENSE_SCANDONE               */
#define _LESENSE_IEN_SCANDONE_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_SCANDONE_DEFAULT                 (_LESENSE_IEN_SCANDONE_DEFAULT << 16) /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_DEC                              (0x1UL << 17)                         /**< Decoder                                     */
#define _LESENSE_IEN_DEC_SHIFT                       17                                    /**< Shift value for LESENSE_DEC                 */
#define _LESENSE_IEN_DEC_MASK                        0x20000UL                             /**< Bit mask for LESENSE_DEC                    */
#define _LESENSE_IEN_DEC_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_DEC_DEFAULT                      (_LESENSE_IEN_DEC_DEFAULT << 17)      /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_RESWL                            (0x1UL << 18)                         /**< Result Watermark Level                      */
#define _LESENSE_IEN_RESWL_SHIFT                     18                                    /**< Shift value for LESENSE_RESWL               */
#define _LESENSE_IEN_RESWL_MASK                      0x40000UL                             /**< Bit mask for LESENSE_RESWL                  */
#define _LESENSE_IEN_RESWL_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_RESWL_DEFAULT                    (_LESENSE_IEN_RESWL_DEFAULT << 18)    /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_RESOF                            (0x1UL << 19)                         /**< Result Overflow                             */
#define _LESENSE_IEN_RESOF_SHIFT                     19                                    /**< Shift value for LESENSE_RESOF               */
#define _LESENSE_IEN_RESOF_MASK                      0x80000UL                             /**< Bit mask for LESENSE_RESOF                  */
#define _LESENSE_IEN_RESOF_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_RESOF_DEFAULT                    (_LESENSE_IEN_RESOF_DEFAULT << 19)    /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_CNTOF                            (0x1UL << 20)                         /**< Counter Overflow                            */
#define _LESENSE_IEN_CNTOF_SHIFT                     20                                    /**< Shift value for LESENSE_CNTOF               */
#define _LESENSE_IEN_CNTOF_MASK                      0x100000UL                            /**< Bit mask for LESENSE_CNTOF                  */
#define _LESENSE_IEN_CNTOF_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_CNTOF_DEFAULT                    (_LESENSE_IEN_CNTOF_DEFAULT << 20)    /**< Shifted mode DEFAULT for LESENSE_IEN        */
#define LESENSE_IEN_RESUF                            (0x1UL << 21)                         /**< Result Underflow                            */
#define _LESENSE_IEN_RESUF_SHIFT                     21                                    /**< Shift value for LESENSE_RESUF               */
#define _LESENSE_IEN_RESUF_MASK                      0x200000UL                            /**< Bit mask for LESENSE_RESUF                  */
#define _LESENSE_IEN_RESUF_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for LESENSE_IEN                */
#define LESENSE_IEN_RESUF_DEFAULT                    (_LESENSE_IEN_RESUF_DEFAULT << 21)    /**< Shifted mode DEFAULT for LESENSE_IEN        */

/* Bit fields for LESENSE CH_TIMING */
#define _LESENSE_CH_TIMING_RESETVALUE                0x00000000UL                                  /**< Default value for LESENSE_CH_TIMING         */
#define _LESENSE_CH_TIMING_MASK                      0x00FFFFFFUL                                  /**< Mask for LESENSE_CH_TIMING                  */
#define _LESENSE_CH_TIMING_EXTIME_SHIFT              0                                             /**< Shift value for LESENSE_EXTIME              */
#define _LESENSE_CH_TIMING_EXTIME_MASK               0x3FUL                                        /**< Bit mask for LESENSE_EXTIME                 */
#define _LESENSE_CH_TIMING_EXTIME_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for LESENSE_CH_TIMING          */
#define LESENSE_CH_TIMING_EXTIME_DEFAULT             (_LESENSE_CH_TIMING_EXTIME_DEFAULT << 0)      /**< Shifted mode DEFAULT for LESENSE_CH_TIMING  */
#define _LESENSE_CH_TIMING_SAMPLEDLY_SHIFT           6                                             /**< Shift value for LESENSE_SAMPLEDLY           */
#define _LESENSE_CH_TIMING_SAMPLEDLY_MASK            0x3FC0UL                                      /**< Bit mask for LESENSE_SAMPLEDLY              */
#define _LESENSE_CH_TIMING_SAMPLEDLY_DEFAULT         0x00000000UL                                  /**< Mode DEFAULT for LESENSE_CH_TIMING          */
#define LESENSE_CH_TIMING_SAMPLEDLY_DEFAULT          (_LESENSE_CH_TIMING_SAMPLEDLY_DEFAULT << 6)   /**< Shifted mode DEFAULT for LESENSE_CH_TIMING  */
#define _LESENSE_CH_TIMING_MEASUREDLY_SHIFT          14                                            /**< Shift value for LESENSE_MEASUREDLY          */
#define _LESENSE_CH_TIMING_MEASUREDLY_MASK           0xFFC000UL                                    /**< Bit mask for LESENSE_MEASUREDLY             */
#define _LESENSE_CH_TIMING_MEASUREDLY_DEFAULT        0x00000000UL                                  /**< Mode DEFAULT for LESENSE_CH_TIMING          */
#define LESENSE_CH_TIMING_MEASUREDLY_DEFAULT         (_LESENSE_CH_TIMING_MEASUREDLY_DEFAULT << 14) /**< Shifted mode DEFAULT for LESENSE_CH_TIMING  */

/* Bit fields for LESENSE CH_INTERACT */
#define _LESENSE_CH_INTERACT_RESETVALUE              0x00000000UL                                    /**< Default value for LESENSE_CH_INTERACT       */
#define _LESENSE_CH_INTERACT_MASK                    0x3FFF0FFFUL                                    /**< Mask for LESENSE_CH_INTERACT                */
#define _LESENSE_CH_INTERACT_THRES_SHIFT             0                                               /**< Shift value for LESENSE_THRES               */
#define _LESENSE_CH_INTERACT_THRES_MASK              0xFFFUL                                         /**< Bit mask for LESENSE_THRES                  */
#define _LESENSE_CH_INTERACT_THRES_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for LESENSE_CH_INTERACT        */
#define LESENSE_CH_INTERACT_THRES_DEFAULT            (_LESENSE_CH_INTERACT_THRES_DEFAULT << 0)       /**< Shifted mode DEFAULT for LESENSE_CH_INTERACT*/
#define _LESENSE_CH_INTERACT_EXMODE_SHIFT            16                                              /**< Shift value for LESENSE_EXMODE              */
#define _LESENSE_CH_INTERACT_EXMODE_MASK             0x30000UL                                       /**< Bit mask for LESENSE_EXMODE                 */
#define _LESENSE_CH_INTERACT_EXMODE_DEFAULT          0x00000000UL                                    /**< Mode DEFAULT for LESENSE_CH_INTERACT        */
#define _LESENSE_CH_INTERACT_EXMODE_DISABLE          0x00000000UL                                    /**< Mode DISABLE for LESENSE_CH_INTERACT        */
#define _LESENSE_CH_INTERACT_EXMODE_HIGH             0x00000001UL                                    /**< Mode HIGH for LESENSE_CH_INTERACT           */
#define _LESENSE_CH_INTERACT_EXMODE_LOW              0x00000002UL                                    /**< Mode LOW for LESENSE_CH_INTERACT            */
#define _LESENSE_CH_INTERACT_EXMODE_DACOUT           0x00000003UL                                    /**< Mode DACOUT for LESENSE_CH_INTERACT         */
#define LESENSE_CH_INTERACT_EXMODE_DEFAULT           (_LESENSE_CH_INTERACT_EXMODE_DEFAULT << 16)     /**< Shifted mode DEFAULT for LESENSE_CH_INTERACT*/
#define LESENSE_CH_INTERACT_EXMODE_DISABLE           (_LESENSE_CH_INTERACT_EXMODE_DISABLE << 16)     /**< Shifted mode DISABLE for LESENSE_CH_INTERACT*/
#define LESENSE_CH_INTERACT_EXMODE_HIGH              (_LESENSE_CH_INTERACT_EXMODE_HIGH << 16)        /**< Shifted mode HIGH for LESENSE_CH_INTERACT   */
#define LESENSE_CH_INTERACT_EXMODE_LOW               (_LESENSE_CH_INTERACT_EXMODE_LOW << 16)         /**< Shifted mode LOW for LESENSE_CH_INTERACT    */
#define LESENSE_CH_INTERACT_EXMODE_DACOUT            (_LESENSE_CH_INTERACT_EXMODE_DACOUT << 16)      /**< Shifted mode DACOUT for LESENSE_CH_INTERACT */
#define LESENSE_CH_INTERACT_ALTEX                    (0x1UL << 18)                                   /**< Use alternative excite pin                  */
#define _LESENSE_CH_INTERACT_ALTEX_SHIFT             18                                              /**< Shift value for LESENSE_ALTEX               */
#define _LESENSE_CH_INTERACT_ALTEX_MASK              0x40000UL                                       /**< Bit mask for LESENSE_ALTEX                  */
#define _LESENSE_CH_INTERACT_ALTEX_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for LESENSE_CH_INTERACT        */
#define LESENSE_CH_INTERACT_ALTEX_DEFAULT            (_LESENSE_CH_INTERACT_ALTEX_DEFAULT << 18)      /**< Shifted mode DEFAULT for LESENSE_CH_INTERACT*/
#define LESENSE_CH_INTERACT_SAMPLECLK                (0x1UL << 19)                                   /**< Select clock used for timing of sample d    */
#define _LESENSE_CH_INTERACT_SAMPLECLK_SHIFT         19                                              /**< Shift value for LESENSE_SAMPLECLK           */
#define _LESENSE_CH_INTERACT_SAMPLECLK_MASK          0x80000UL                                       /**< Bit mask for LESENSE_SAMPLECLK              */
#define _LESENSE_CH_INTERACT_SAMPLECLK_DEFAULT       0x00000000UL                                    /**< Mode DEFAULT for LESENSE_CH_INTERACT        */
#define _LESENSE_CH_INTERACT_SAMPLECLK_LFACLK        0x00000000UL                                    /**< Mode LFACLK for LESENSE_CH_INTERACT         */
#define _LESENSE_CH_INTERACT_SAMPLECLK_AUXHFRCO      0x00000001UL                                    /**< Mode AUXHFRCO for LESENSE_CH_INTERACT       */
#define LESENSE_CH_INTERACT_SAMPLECLK_DEFAULT        (_LESENSE_CH_INTERACT_SAMPLECLK_DEFAULT << 19)  /**< Shifted mode DEFAULT for LESENSE_CH_INTERACT*/
#define LESENSE_CH_INTERACT_SAMPLECLK_LFACLK         (_LESENSE_CH_INTERACT_SAMPLECLK_LFACLK << 19)   /**< Shifted mode LFACLK for LESENSE_CH_INTERACT */
#define LESENSE_CH_INTERACT_SAMPLECLK_AUXHFRCO       (_LESENSE_CH_INTERACT_SAMPLECLK_AUXHFRCO << 19) /**< Shifted mode AUXHFRCO for LESENSE_CH_INTERACT*/
#define LESENSE_CH_INTERACT_EXCLK                    (0x1UL << 20)                                   /**< Select clock used for excitation timing     */
#define _LESENSE_CH_INTERACT_EXCLK_SHIFT             20                                              /**< Shift value for LESENSE_EXCLK               */
#define _LESENSE_CH_INTERACT_EXCLK_MASK              0x100000UL                                      /**< Bit mask for LESENSE_EXCLK                  */
#define _LESENSE_CH_INTERACT_EXCLK_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for LESENSE_CH_INTERACT        */
#define _LESENSE_CH_INTERACT_EXCLK_LFACLK            0x00000000UL                                    /**< Mode LFACLK for LESENSE_CH_INTERACT         */
#define _LESENSE_CH_INTERACT_EXCLK_AUXHFRCO          0x00000001UL                                    /**< Mode AUXHFRCO for LESENSE_CH_INTERACT       */
#define LESENSE_CH_INTERACT_EXCLK_DEFAULT            (_LESENSE_CH_INTERACT_EXCLK_DEFAULT << 20)      /**< Shifted mode DEFAULT for LESENSE_CH_INTERACT*/
#define LESENSE_CH_INTERACT_EXCLK_LFACLK             (_LESENSE_CH_INTERACT_EXCLK_LFACLK << 20)       /**< Shifted mode LFACLK for LESENSE_CH_INTERACT */
#define LESENSE_CH_INTERACT_EXCLK_AUXHFRCO           (_LESENSE_CH_INTERACT_EXCLK_AUXHFRCO << 20)     /**< Shifted mode AUXHFRCO for LESENSE_CH_INTERACT*/
#define _LESENSE_CH_INTERACT_SETIF_SHIFT             21                                              /**< Shift value for LESENSE_SETIF               */
#define _LESENSE_CH_INTERACT_SETIF_MASK              0xE00000UL                                      /**< Bit mask for LESENSE_SETIF                  */
#define _LESENSE_CH_INTERACT_SETIF_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for LESENSE_CH_INTERACT        */
#define _LESENSE_CH_INTERACT_SETIF_NONE              0x00000000UL                                    /**< Mode NONE for LESENSE_CH_INTERACT           */
#define _LESENSE_CH_INTERACT_SETIF_LEVEL             0x00000001UL                                    /**< Mode LEVEL for LESENSE_CH_INTERACT          */
#define _LESENSE_CH_INTERACT_SETIF_POSEDGE           0x00000002UL                                    /**< Mode POSEDGE for LESENSE_CH_INTERACT        */
#define _LESENSE_CH_INTERACT_SETIF_NEGEDGE           0x00000003UL                                    /**< Mode NEGEDGE for LESENSE_CH_INTERACT        */
#define _LESENSE_CH_INTERACT_SETIF_BOTHEDGES         0x00000004UL                                    /**< Mode BOTHEDGES for LESENSE_CH_INTERACT      */
#define LESENSE_CH_INTERACT_SETIF_DEFAULT            (_LESENSE_CH_INTERACT_SETIF_DEFAULT << 21)      /**< Shifted mode DEFAULT for LESENSE_CH_INTERACT*/
#define LESENSE_CH_INTERACT_SETIF_NONE               (_LESENSE_CH_INTERACT_SETIF_NONE << 21)         /**< Shifted mode NONE for LESENSE_CH_INTERACT   */
#define LESENSE_CH_INTERACT_SETIF_LEVEL              (_LESENSE_CH_INTERACT_SETIF_LEVEL << 21)        /**< Shifted mode LEVEL for LESENSE_CH_INTERACT  */
#define LESENSE_CH_INTERACT_SETIF_POSEDGE            (_LESENSE_CH_INTERACT_SETIF_POSEDGE << 21)      /**< Shifted mode POSEDGE for LESENSE_CH_INTERACT*/
#define LESENSE_CH_INTERACT_SETIF_NEGEDGE            (_LESENSE_CH_INTERACT_SETIF_NEGEDGE << 21)      /**< Shifted mode NEGEDGE for LESENSE_CH_INTERACT*/
#define LESENSE_CH_INTERACT_SETIF_BOTHEDGES          (_LESENSE_CH_INTERACT_SETIF_BOTHEDGES << 21)    /**< Shifted mode BOTHEDGES for LESENSE_CH_INTERACT*/
#define _LESENSE_CH_INTERACT_OFFSET_SHIFT            24                                              /**< Shift value for LESENSE_OFFSET              */
#define _LESENSE_CH_INTERACT_OFFSET_MASK             0xF000000UL                                     /**< Bit mask for LESENSE_OFFSET                 */
#define _LESENSE_CH_INTERACT_OFFSET_DEFAULT          0x00000000UL                                    /**< Mode DEFAULT for LESENSE_CH_INTERACT        */
#define LESENSE_CH_INTERACT_OFFSET_DEFAULT           (_LESENSE_CH_INTERACT_OFFSET_DEFAULT << 24)     /**< Shifted mode DEFAULT for LESENSE_CH_INTERACT*/
#define _LESENSE_CH_INTERACT_SAMPLE_SHIFT            28                                              /**< Shift value for LESENSE_SAMPLE              */
#define _LESENSE_CH_INTERACT_SAMPLE_MASK             0x30000000UL                                    /**< Bit mask for LESENSE_SAMPLE                 */
#define _LESENSE_CH_INTERACT_SAMPLE_DEFAULT          0x00000000UL                                    /**< Mode DEFAULT for LESENSE_CH_INTERACT        */
#define _LESENSE_CH_INTERACT_SAMPLE_ACMPCOUNT        0x00000000UL                                    /**< Mode ACMPCOUNT for LESENSE_CH_INTERACT      */
#define _LESENSE_CH_INTERACT_SAMPLE_ACMP             0x00000001UL                                    /**< Mode ACMP for LESENSE_CH_INTERACT           */
#define _LESENSE_CH_INTERACT_SAMPLE_ADC              0x00000002UL                                    /**< Mode ADC for LESENSE_CH_INTERACT            */
#define _LESENSE_CH_INTERACT_SAMPLE_ADCDIFF          0x00000003UL                                    /**< Mode ADCDIFF for LESENSE_CH_INTERACT        */
#define LESENSE_CH_INTERACT_SAMPLE_DEFAULT           (_LESENSE_CH_INTERACT_SAMPLE_DEFAULT << 28)     /**< Shifted mode DEFAULT for LESENSE_CH_INTERACT*/
#define LESENSE_CH_INTERACT_SAMPLE_ACMPCOUNT         (_LESENSE_CH_INTERACT_SAMPLE_ACMPCOUNT << 28)   /**< Shifted mode ACMPCOUNT for LESENSE_CH_INTERACT*/
#define LESENSE_CH_INTERACT_SAMPLE_ACMP              (_LESENSE_CH_INTERACT_SAMPLE_ACMP << 28)        /**< Shifted mode ACMP for LESENSE_CH_INTERACT   */
#define LESENSE_CH_INTERACT_SAMPLE_ADC               (_LESENSE_CH_INTERACT_SAMPLE_ADC << 28)         /**< Shifted mode ADC for LESENSE_CH_INTERACT    */
#define LESENSE_CH_INTERACT_SAMPLE_ADCDIFF           (_LESENSE_CH_INTERACT_SAMPLE_ADCDIFF << 28)     /**< Shifted mode ADCDIFF for LESENSE_CH_INTERACT*/

/* Bit fields for LESENSE CH_EVALCFG */
#define _LESENSE_CH_EVALCFG_RESETVALUE               0x00000000UL                                  /**< Default value for LESENSE_CH_EVALCFG        */
#define _LESENSE_CH_EVALCFG_MASK                     0x0000037CUL                                  /**< Mask for LESENSE_CH_EVALCFG                 */
#define LESENSE_CH_EVALCFG_DECODE                    (0x1UL << 2)                                  /**< Send result to decoder                      */
#define _LESENSE_CH_EVALCFG_DECODE_SHIFT             2                                             /**< Shift value for LESENSE_DECODE              */
#define _LESENSE_CH_EVALCFG_DECODE_MASK              0x4UL                                         /**< Bit mask for LESENSE_DECODE                 */
#define _LESENSE_CH_EVALCFG_DECODE_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for LESENSE_CH_EVALCFG         */
#define LESENSE_CH_EVALCFG_DECODE_DEFAULT            (_LESENSE_CH_EVALCFG_DECODE_DEFAULT << 2)     /**< Shifted mode DEFAULT for LESENSE_CH_EVALCFG */
#define LESENSE_CH_EVALCFG_COMP                      (0x1UL << 3)                                  /**< Select mode for threshold comparison        */
#define _LESENSE_CH_EVALCFG_COMP_SHIFT               3                                             /**< Shift value for LESENSE_COMP                */
#define _LESENSE_CH_EVALCFG_COMP_MASK                0x8UL                                         /**< Bit mask for LESENSE_COMP                   */
#define _LESENSE_CH_EVALCFG_COMP_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for LESENSE_CH_EVALCFG         */
#define _LESENSE_CH_EVALCFG_COMP_LESS                0x00000000UL                                  /**< Mode LESS for LESENSE_CH_EVALCFG            */
#define _LESENSE_CH_EVALCFG_COMP_GE                  0x00000001UL                                  /**< Mode GE for LESENSE_CH_EVALCFG              */
#define LESENSE_CH_EVALCFG_COMP_DEFAULT              (_LESENSE_CH_EVALCFG_COMP_DEFAULT << 3)       /**< Shifted mode DEFAULT for LESENSE_CH_EVALCFG */
#define LESENSE_CH_EVALCFG_COMP_LESS                 (_LESENSE_CH_EVALCFG_COMP_LESS << 3)          /**< Shifted mode LESS for LESENSE_CH_EVALCFG    */
#define LESENSE_CH_EVALCFG_COMP_GE                   (_LESENSE_CH_EVALCFG_COMP_GE << 3)            /**< Shifted mode GE for LESENSE_CH_EVALCFG      */
#define _LESENSE_CH_EVALCFG_STRSAMPLE_SHIFT          4                                             /**< Shift value for LESENSE_STRSAMPLE           */
#define _LESENSE_CH_EVALCFG_STRSAMPLE_MASK           0x30UL                                        /**< Bit mask for LESENSE_STRSAMPLE              */
#define _LESENSE_CH_EVALCFG_STRSAMPLE_DEFAULT        0x00000000UL                                  /**< Mode DEFAULT for LESENSE_CH_EVALCFG         */
#define _LESENSE_CH_EVALCFG_STRSAMPLE_DISABLE        0x00000000UL                                  /**< Mode DISABLE for LESENSE_CH_EVALCFG         */
#define _LESENSE_CH_EVALCFG_STRSAMPLE_DATA           0x00000001UL                                  /**< Mode DATA for LESENSE_CH_EVALCFG            */
#define _LESENSE_CH_EVALCFG_STRSAMPLE_DATASRC        0x00000002UL                                  /**< Mode DATASRC for LESENSE_CH_EVALCFG         */
#define LESENSE_CH_EVALCFG_STRSAMPLE_DEFAULT         (_LESENSE_CH_EVALCFG_STRSAMPLE_DEFAULT << 4)  /**< Shifted mode DEFAULT for LESENSE_CH_EVALCFG */
#define LESENSE_CH_EVALCFG_STRSAMPLE_DISABLE         (_LESENSE_CH_EVALCFG_STRSAMPLE_DISABLE << 4)  /**< Shifted mode DISABLE for LESENSE_CH_EVALCFG */
#define LESENSE_CH_EVALCFG_STRSAMPLE_DATA            (_LESENSE_CH_EVALCFG_STRSAMPLE_DATA << 4)     /**< Shifted mode DATA for LESENSE_CH_EVALCFG    */
#define LESENSE_CH_EVALCFG_STRSAMPLE_DATASRC         (_LESENSE_CH_EVALCFG_STRSAMPLE_DATASRC << 4)  /**< Shifted mode DATASRC for LESENSE_CH_EVALCFG */
#define LESENSE_CH_EVALCFG_SCANRESINV                (0x1UL << 6)                                  /**< Enable inversion of result                  */
#define _LESENSE_CH_EVALCFG_SCANRESINV_SHIFT         6                                             /**< Shift value for LESENSE_SCANRESINV          */
#define _LESENSE_CH_EVALCFG_SCANRESINV_MASK          0x40UL                                        /**< Bit mask for LESENSE_SCANRESINV             */
#define _LESENSE_CH_EVALCFG_SCANRESINV_DEFAULT       0x00000000UL                                  /**< Mode DEFAULT for LESENSE_CH_EVALCFG         */
#define LESENSE_CH_EVALCFG_SCANRESINV_DEFAULT        (_LESENSE_CH_EVALCFG_SCANRESINV_DEFAULT << 6) /**< Shifted mode DEFAULT for LESENSE_CH_EVALCFG */
#define _LESENSE_CH_EVALCFG_MODE_SHIFT               8                                             /**< Shift value for LESENSE_MODE                */
#define _LESENSE_CH_EVALCFG_MODE_MASK                0x300UL                                       /**< Bit mask for LESENSE_MODE                   */
#define _LESENSE_CH_EVALCFG_MODE_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for LESENSE_CH_EVALCFG         */
#define _LESENSE_CH_EVALCFG_MODE_THRES               0x00000000UL                                  /**< Mode THRES for LESENSE_CH_EVALCFG           */
#define _LESENSE_CH_EVALCFG_MODE_SLIDINGWIN          0x00000001UL                                  /**< Mode SLIDINGWIN for LESENSE_CH_EVALCFG      */
#define _LESENSE_CH_EVALCFG_MODE_STEPDET             0x00000002UL                                  /**< Mode STEPDET for LESENSE_CH_EVALCFG         */
#define LESENSE_CH_EVALCFG_MODE_DEFAULT              (_LESENSE_CH_EVALCFG_MODE_DEFAULT << 8)       /**< Shifted mode DEFAULT for LESENSE_CH_EVALCFG */
#define LESENSE_CH_EVALCFG_MODE_THRES                (_LESENSE_CH_EVALCFG_MODE_THRES << 8)         /**< Shifted mode THRES for LESENSE_CH_EVALCFG   */
#define LESENSE_CH_EVALCFG_MODE_SLIDINGWIN           (_LESENSE_CH_EVALCFG_MODE_SLIDINGWIN << 8)    /**< Shifted mode SLIDINGWIN for LESENSE_CH_EVALCFG*/
#define LESENSE_CH_EVALCFG_MODE_STEPDET              (_LESENSE_CH_EVALCFG_MODE_STEPDET << 8)       /**< Shifted mode STEPDET for LESENSE_CH_EVALCFG */

/* Bit fields for LESENSE CH_EVALTHRES */
#define _LESENSE_CH_EVALTHRES_RESETVALUE             0x00000000UL                                   /**< Default value for LESENSE_CH_EVALTHRES      */
#define _LESENSE_CH_EVALTHRES_MASK                   0x0000FFFFUL                                   /**< Mask for LESENSE_CH_EVALTHRES               */
#define _LESENSE_CH_EVALTHRES_EVALTHRES_SHIFT        0                                              /**< Shift value for LESENSE_EVALTHRES           */
#define _LESENSE_CH_EVALTHRES_EVALTHRES_MASK         0xFFFFUL                                       /**< Bit mask for LESENSE_EVALTHRES              */
#define _LESENSE_CH_EVALTHRES_EVALTHRES_DEFAULT      0x00000000UL                                   /**< Mode DEFAULT for LESENSE_CH_EVALTHRES       */
#define LESENSE_CH_EVALTHRES_EVALTHRES_DEFAULT       (_LESENSE_CH_EVALTHRES_EVALTHRES_DEFAULT << 0) /**< Shifted mode DEFAULT for LESENSE_CH_EVALTHRES*/

/* Bit fields for LESENSE ST_ARC */
#define _LESENSE_ST_ARC_RESETVALUE                   0x00000000UL                               /**< Default value for LESENSE_ST_ARC            */
#define _LESENSE_ST_ARC_MASK                         0x003FFFFFUL                               /**< Mask for LESENSE_ST_ARC                     */
#define _LESENSE_ST_ARC_SCOMP_SHIFT                  0                                          /**< Shift value for LESENSE_SCOMP               */
#define _LESENSE_ST_ARC_SCOMP_MASK                   0xFUL                                      /**< Bit mask for LESENSE_SCOMP                  */
#define _LESENSE_ST_ARC_SCOMP_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for LESENSE_ST_ARC             */
#define LESENSE_ST_ARC_SCOMP_DEFAULT                 (_LESENSE_ST_ARC_SCOMP_DEFAULT << 0)       /**< Shifted mode DEFAULT for LESENSE_ST_ARC     */
#define _LESENSE_ST_ARC_SMASK_SHIFT                  4                                          /**< Shift value for LESENSE_SMASK               */
#define _LESENSE_ST_ARC_SMASK_MASK                   0xF0UL                                     /**< Bit mask for LESENSE_SMASK                  */
#define _LESENSE_ST_ARC_SMASK_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for LESENSE_ST_ARC             */
#define LESENSE_ST_ARC_SMASK_DEFAULT                 (_LESENSE_ST_ARC_SMASK_DEFAULT << 4)       /**< Shifted mode DEFAULT for LESENSE_ST_ARC     */
#define _LESENSE_ST_ARC_CURSTATE_SHIFT               8                                          /**< Shift value for LESENSE_CURSTATE            */
#define _LESENSE_ST_ARC_CURSTATE_MASK                0x1F00UL                                   /**< Bit mask for LESENSE_CURSTATE               */
#define _LESENSE_ST_ARC_CURSTATE_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LESENSE_ST_ARC             */
#define LESENSE_ST_ARC_CURSTATE_DEFAULT              (_LESENSE_ST_ARC_CURSTATE_DEFAULT << 8)    /**< Shifted mode DEFAULT for LESENSE_ST_ARC     */
#define _LESENSE_ST_ARC_PRSACT_SHIFT                 13                                         /**< Shift value for LESENSE_PRSACT              */
#define _LESENSE_ST_ARC_PRSACT_MASK                  0xE000UL                                   /**< Bit mask for LESENSE_PRSACT                 */
#define _LESENSE_ST_ARC_PRSACT_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for LESENSE_ST_ARC             */
#define _LESENSE_ST_ARC_PRSACT_NONE                  0x00000000UL                               /**< Mode NONE for LESENSE_ST_ARC                */
#define _LESENSE_ST_ARC_PRSACT_PRS0                  0x00000001UL                               /**< Mode PRS0 for LESENSE_ST_ARC                */
#define _LESENSE_ST_ARC_PRSACT_UP                    0x00000001UL                               /**< Mode UP for LESENSE_ST_ARC                  */
#define _LESENSE_ST_ARC_PRSACT_PRS1                  0x00000002UL                               /**< Mode PRS1 for LESENSE_ST_ARC                */
#define _LESENSE_ST_ARC_PRSACT_DOWN                  0x00000002UL                               /**< Mode DOWN for LESENSE_ST_ARC                */
#define _LESENSE_ST_ARC_PRSACT_PRS01                 0x00000003UL                               /**< Mode PRS01 for LESENSE_ST_ARC               */
#define _LESENSE_ST_ARC_PRSACT_PRS2                  0x00000004UL                               /**< Mode PRS2 for LESENSE_ST_ARC                */
#define _LESENSE_ST_ARC_PRSACT_PRS02                 0x00000005UL                               /**< Mode PRS02 for LESENSE_ST_ARC               */
#define _LESENSE_ST_ARC_PRSACT_UPANDPRS2             0x00000005UL                               /**< Mode UPANDPRS2 for LESENSE_ST_ARC           */
#define _LESENSE_ST_ARC_PRSACT_PRS12                 0x00000006UL                               /**< Mode PRS12 for LESENSE_ST_ARC               */
#define _LESENSE_ST_ARC_PRSACT_DOWNANDPRS2           0x00000006UL                               /**< Mode DOWNANDPRS2 for LESENSE_ST_ARC         */
#define _LESENSE_ST_ARC_PRSACT_PRS012                0x00000007UL                               /**< Mode PRS012 for LESENSE_ST_ARC              */
#define LESENSE_ST_ARC_PRSACT_DEFAULT                (_LESENSE_ST_ARC_PRSACT_DEFAULT << 13)     /**< Shifted mode DEFAULT for LESENSE_ST_ARC     */
#define LESENSE_ST_ARC_PRSACT_NONE                   (_LESENSE_ST_ARC_PRSACT_NONE << 13)        /**< Shifted mode NONE for LESENSE_ST_ARC        */
#define LESENSE_ST_ARC_PRSACT_PRS0                   (_LESENSE_ST_ARC_PRSACT_PRS0 << 13)        /**< Shifted mode PRS0 for LESENSE_ST_ARC        */
#define LESENSE_ST_ARC_PRSACT_UP                     (_LESENSE_ST_ARC_PRSACT_UP << 13)          /**< Shifted mode UP for LESENSE_ST_ARC          */
#define LESENSE_ST_ARC_PRSACT_PRS1                   (_LESENSE_ST_ARC_PRSACT_PRS1 << 13)        /**< Shifted mode PRS1 for LESENSE_ST_ARC        */
#define LESENSE_ST_ARC_PRSACT_DOWN                   (_LESENSE_ST_ARC_PRSACT_DOWN << 13)        /**< Shifted mode DOWN for LESENSE_ST_ARC        */
#define LESENSE_ST_ARC_PRSACT_PRS01                  (_LESENSE_ST_ARC_PRSACT_PRS01 << 13)       /**< Shifted mode PRS01 for LESENSE_ST_ARC       */
#define LESENSE_ST_ARC_PRSACT_PRS2                   (_LESENSE_ST_ARC_PRSACT_PRS2 << 13)        /**< Shifted mode PRS2 for LESENSE_ST_ARC        */
#define LESENSE_ST_ARC_PRSACT_PRS02                  (_LESENSE_ST_ARC_PRSACT_PRS02 << 13)       /**< Shifted mode PRS02 for LESENSE_ST_ARC       */
#define LESENSE_ST_ARC_PRSACT_UPANDPRS2              (_LESENSE_ST_ARC_PRSACT_UPANDPRS2 << 13)   /**< Shifted mode UPANDPRS2 for LESENSE_ST_ARC   */
#define LESENSE_ST_ARC_PRSACT_PRS12                  (_LESENSE_ST_ARC_PRSACT_PRS12 << 13)       /**< Shifted mode PRS12 for LESENSE_ST_ARC       */
#define LESENSE_ST_ARC_PRSACT_DOWNANDPRS2            (_LESENSE_ST_ARC_PRSACT_DOWNANDPRS2 << 13) /**< Shifted mode DOWNANDPRS2 for LESENSE_ST_ARC */
#define LESENSE_ST_ARC_PRSACT_PRS012                 (_LESENSE_ST_ARC_PRSACT_PRS012 << 13)      /**< Shifted mode PRS012 for LESENSE_ST_ARC      */
#define _LESENSE_ST_ARC_NEXTSTATE_SHIFT              16                                         /**< Shift value for LESENSE_NEXTSTATE           */
#define _LESENSE_ST_ARC_NEXTSTATE_MASK               0x1F0000UL                                 /**< Bit mask for LESENSE_NEXTSTATE              */
#define _LESENSE_ST_ARC_NEXTSTATE_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LESENSE_ST_ARC             */
#define LESENSE_ST_ARC_NEXTSTATE_DEFAULT             (_LESENSE_ST_ARC_NEXTSTATE_DEFAULT << 16)  /**< Shifted mode DEFAULT for LESENSE_ST_ARC     */
#define LESENSE_ST_ARC_SETIF                         (0x1UL << 21)                              /**< Set interrupt flag                          */
#define _LESENSE_ST_ARC_SETIF_SHIFT                  21                                         /**< Shift value for LESENSE_SETIF               */
#define _LESENSE_ST_ARC_SETIF_MASK                   0x200000UL                                 /**< Bit mask for LESENSE_SETIF                  */
#define _LESENSE_ST_ARC_SETIF_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for LESENSE_ST_ARC             */
#define LESENSE_ST_ARC_SETIF_DEFAULT                 (_LESENSE_ST_ARC_SETIF_DEFAULT << 21)      /**< Shifted mode DEFAULT for LESENSE_ST_ARC     */

/** @} End of group EFR32ZG23_LESENSE_BitFields */
/** @} End of group EFR32ZG23_LESENSE */
/** @} End of group Parts */

#endif /* EFR32ZG23_LESENSE_H */
#ifdef __cplusplus
}
#endif

