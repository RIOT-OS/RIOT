/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 VDAC register and bit field definitions
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

#ifndef EFR32ZG23_VDAC_H
#define EFR32ZG23_VDAC_H
#define VDAC_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_VDAC VDAC
 * @{
 * @brief EFR32ZG23 VDAC Register Declaration.
 *****************************************************************************/

/** VDAC Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IPVERSION                                          */
  __IOM uint32_t EN;                            /**< Module Enable                                      */
  __IOM uint32_t SWRST;                         /**< Software Reset Register                            */
  __IOM uint32_t CFG;                           /**< Config Register                                    */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t CH0CFG;                        /**< Channel 0 Config Register                          */
  __IOM uint32_t CH1CFG;                        /**< Channel 1 Config Register                          */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  __IOM uint32_t CH0F;                          /**< Channel 0 Data Write Fifo                          */
  __IOM uint32_t CH1F;                          /**< Channel 1 Data Write Fifo                          */
  __IOM uint32_t OUTCTRL;                       /**< DAC Output Control                                 */
  __IOM uint32_t OUTTIMERCFG;                   /**< DAC Out Timer Config Register                      */
  uint32_t       RESERVED0[50U];                /**< Reserved for future use                            */
  uint32_t       RESERVED1[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED2[63U];                /**< Reserved for future use                            */
  uint32_t       RESERVED3[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED4[895U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_SET;                        /**< Module Enable                                      */
  __IOM uint32_t SWRST_SET;                     /**< Software Reset Register                            */
  __IOM uint32_t CFG_SET;                       /**< Config Register                                    */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t CH0CFG_SET;                    /**< Channel 0 Config Register                          */
  __IOM uint32_t CH1CFG_SET;                    /**< Channel 1 Config Register                          */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t CH0F_SET;                      /**< Channel 0 Data Write Fifo                          */
  __IOM uint32_t CH1F_SET;                      /**< Channel 1 Data Write Fifo                          */
  __IOM uint32_t OUTCTRL_SET;                   /**< DAC Output Control                                 */
  __IOM uint32_t OUTTIMERCFG_SET;               /**< DAC Out Timer Config Register                      */
  uint32_t       RESERVED5[50U];                /**< Reserved for future use                            */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED7[63U];                /**< Reserved for future use                            */
  uint32_t       RESERVED8[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED9[895U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_CLR;                        /**< Module Enable                                      */
  __IOM uint32_t SWRST_CLR;                     /**< Software Reset Register                            */
  __IOM uint32_t CFG_CLR;                       /**< Config Register                                    */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t CH0CFG_CLR;                    /**< Channel 0 Config Register                          */
  __IOM uint32_t CH1CFG_CLR;                    /**< Channel 1 Config Register                          */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t CH0F_CLR;                      /**< Channel 0 Data Write Fifo                          */
  __IOM uint32_t CH1F_CLR;                      /**< Channel 1 Data Write Fifo                          */
  __IOM uint32_t OUTCTRL_CLR;                   /**< DAC Output Control                                 */
  __IOM uint32_t OUTTIMERCFG_CLR;               /**< DAC Out Timer Config Register                      */
  uint32_t       RESERVED10[50U];               /**< Reserved for future use                            */
  uint32_t       RESERVED11[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED12[63U];               /**< Reserved for future use                            */
  uint32_t       RESERVED13[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED14[895U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_TGL;                        /**< Module Enable                                      */
  __IOM uint32_t SWRST_TGL;                     /**< Software Reset Register                            */
  __IOM uint32_t CFG_TGL;                       /**< Config Register                                    */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t CH0CFG_TGL;                    /**< Channel 0 Config Register                          */
  __IOM uint32_t CH1CFG_TGL;                    /**< Channel 1 Config Register                          */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  __IOM uint32_t CH0F_TGL;                      /**< Channel 0 Data Write Fifo                          */
  __IOM uint32_t CH1F_TGL;                      /**< Channel 1 Data Write Fifo                          */
  __IOM uint32_t OUTCTRL_TGL;                   /**< DAC Output Control                                 */
  __IOM uint32_t OUTTIMERCFG_TGL;               /**< DAC Out Timer Config Register                      */
  uint32_t       RESERVED15[50U];               /**< Reserved for future use                            */
  uint32_t       RESERVED16[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED17[63U];               /**< Reserved for future use                            */
  uint32_t       RESERVED18[1U];                /**< Reserved for future use                            */
} VDAC_TypeDef;
/** @} End of group EFR32ZG23_VDAC */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_VDAC
 * @{
 * @defgroup EFR32ZG23_VDAC_BitFields VDAC Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for VDAC IPVERSION */
#define _VDAC_IPVERSION_RESETVALUE                  0x00000001UL                             /**< Default value for VDAC_IPVERSION            */
#define _VDAC_IPVERSION_MASK                        0xFFFFFFFFUL                             /**< Mask for VDAC_IPVERSION                     */
#define _VDAC_IPVERSION_IPVERSION_SHIFT             0                                        /**< Shift value for VDAC_IPVERSION              */
#define _VDAC_IPVERSION_IPVERSION_MASK              0xFFFFFFFFUL                             /**< Bit mask for VDAC_IPVERSION                 */
#define _VDAC_IPVERSION_IPVERSION_DEFAULT           0x00000001UL                             /**< Mode DEFAULT for VDAC_IPVERSION             */
#define VDAC_IPVERSION_IPVERSION_DEFAULT            (_VDAC_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for VDAC_IPVERSION     */

/* Bit fields for VDAC EN */
#define _VDAC_EN_RESETVALUE                         0x00000000UL                        /**< Default value for VDAC_EN                   */
#define _VDAC_EN_MASK                               0x00000003UL                        /**< Mask for VDAC_EN                            */
#define VDAC_EN_EN                                  (0x1UL << 0)                        /**< VDAC Module Enable                          */
#define _VDAC_EN_EN_SHIFT                           0                                   /**< Shift value for VDAC_EN                     */
#define _VDAC_EN_EN_MASK                            0x1UL                               /**< Bit mask for VDAC_EN                        */
#define _VDAC_EN_EN_DEFAULT                         0x00000000UL                        /**< Mode DEFAULT for VDAC_EN                    */
#define _VDAC_EN_EN_DISABLE                         0x00000000UL                        /**< Mode DISABLE for VDAC_EN                    */
#define _VDAC_EN_EN_ENABLE                          0x00000001UL                        /**< Mode ENABLE for VDAC_EN                     */
#define VDAC_EN_EN_DEFAULT                          (_VDAC_EN_EN_DEFAULT << 0)          /**< Shifted mode DEFAULT for VDAC_EN            */
#define VDAC_EN_EN_DISABLE                          (_VDAC_EN_EN_DISABLE << 0)          /**< Shifted mode DISABLE for VDAC_EN            */
#define VDAC_EN_EN_ENABLE                           (_VDAC_EN_EN_ENABLE << 0)           /**< Shifted mode ENABLE for VDAC_EN             */
#define VDAC_EN_DISABLING                           (0x1UL << 1)                        /**< Disablement busy status                     */
#define _VDAC_EN_DISABLING_SHIFT                    1                                   /**< Shift value for VDAC_DISABLING              */
#define _VDAC_EN_DISABLING_MASK                     0x2UL                               /**< Bit mask for VDAC_DISABLING                 */
#define _VDAC_EN_DISABLING_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for VDAC_EN                    */
#define VDAC_EN_DISABLING_DEFAULT                   (_VDAC_EN_DISABLING_DEFAULT << 1)   /**< Shifted mode DEFAULT for VDAC_EN            */

/* Bit fields for VDAC SWRST */
#define _VDAC_SWRST_RESETVALUE                      0x00000000UL                         /**< Default value for VDAC_SWRST                */
#define _VDAC_SWRST_MASK                            0x00000003UL                         /**< Mask for VDAC_SWRST                         */
#define VDAC_SWRST_SWRST                            (0x1UL << 0)                         /**< Software reset command                      */
#define _VDAC_SWRST_SWRST_SHIFT                     0                                    /**< Shift value for VDAC_SWRST                  */
#define _VDAC_SWRST_SWRST_MASK                      0x1UL                                /**< Bit mask for VDAC_SWRST                     */
#define _VDAC_SWRST_SWRST_DEFAULT                   0x00000000UL                         /**< Mode DEFAULT for VDAC_SWRST                 */
#define VDAC_SWRST_SWRST_DEFAULT                    (_VDAC_SWRST_SWRST_DEFAULT << 0)     /**< Shifted mode DEFAULT for VDAC_SWRST         */
#define VDAC_SWRST_RESETTING                        (0x1UL << 1)                         /**< Software reset busy status                  */
#define _VDAC_SWRST_RESETTING_SHIFT                 1                                    /**< Shift value for VDAC_RESETTING              */
#define _VDAC_SWRST_RESETTING_MASK                  0x2UL                                /**< Bit mask for VDAC_RESETTING                 */
#define _VDAC_SWRST_RESETTING_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for VDAC_SWRST                 */
#define VDAC_SWRST_RESETTING_DEFAULT                (_VDAC_SWRST_RESETTING_DEFAULT << 1) /**< Shifted mode DEFAULT for VDAC_SWRST         */

/* Bit fields for VDAC CFG */
#define _VDAC_CFG_RESETVALUE                        0x20000000UL                                  /**< Default value for VDAC_CFG                  */
#define _VDAC_CFG_MASK                              0x7F773FBFUL                                  /**< Mask for VDAC_CFG                           */
#define VDAC_CFG_DIFF                               (0x1UL << 0)                                  /**< Differential Mode                           */
#define _VDAC_CFG_DIFF_SHIFT                        0                                             /**< Shift value for VDAC_DIFF                   */
#define _VDAC_CFG_DIFF_MASK                         0x1UL                                         /**< Bit mask for VDAC_DIFF                      */
#define _VDAC_CFG_DIFF_DEFAULT                      0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define _VDAC_CFG_DIFF_SINGLEENDED                  0x00000000UL                                  /**< Mode SINGLEENDED for VDAC_CFG               */
#define _VDAC_CFG_DIFF_DIFFERENTIAL                 0x00000001UL                                  /**< Mode DIFFERENTIAL for VDAC_CFG              */
#define VDAC_CFG_DIFF_DEFAULT                       (_VDAC_CFG_DIFF_DEFAULT << 0)                 /**< Shifted mode DEFAULT for VDAC_CFG           */
#define VDAC_CFG_DIFF_SINGLEENDED                   (_VDAC_CFG_DIFF_SINGLEENDED << 0)             /**< Shifted mode SINGLEENDED for VDAC_CFG       */
#define VDAC_CFG_DIFF_DIFFERENTIAL                  (_VDAC_CFG_DIFF_DIFFERENTIAL << 0)            /**< Shifted mode DIFFERENTIAL for VDAC_CFG      */
#define VDAC_CFG_SINEMODE                           (0x1UL << 1)                                  /**< Sine Mode                                   */
#define _VDAC_CFG_SINEMODE_SHIFT                    1                                             /**< Shift value for VDAC_SINEMODE               */
#define _VDAC_CFG_SINEMODE_MASK                     0x2UL                                         /**< Bit mask for VDAC_SINEMODE                  */
#define _VDAC_CFG_SINEMODE_DEFAULT                  0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define _VDAC_CFG_SINEMODE_DISSINEMODE              0x00000000UL                                  /**< Mode DISSINEMODE for VDAC_CFG               */
#define _VDAC_CFG_SINEMODE_ENSINEMODE               0x00000001UL                                  /**< Mode ENSINEMODE for VDAC_CFG                */
#define VDAC_CFG_SINEMODE_DEFAULT                   (_VDAC_CFG_SINEMODE_DEFAULT << 1)             /**< Shifted mode DEFAULT for VDAC_CFG           */
#define VDAC_CFG_SINEMODE_DISSINEMODE               (_VDAC_CFG_SINEMODE_DISSINEMODE << 1)         /**< Shifted mode DISSINEMODE for VDAC_CFG       */
#define VDAC_CFG_SINEMODE_ENSINEMODE                (_VDAC_CFG_SINEMODE_ENSINEMODE << 1)          /**< Shifted mode ENSINEMODE for VDAC_CFG        */
#define VDAC_CFG_SINERESET                          (0x1UL << 2)                                  /**< Sine Wave Reset When inactive               */
#define _VDAC_CFG_SINERESET_SHIFT                   2                                             /**< Shift value for VDAC_SINERESET              */
#define _VDAC_CFG_SINERESET_MASK                    0x4UL                                         /**< Bit mask for VDAC_SINERESET                 */
#define _VDAC_CFG_SINERESET_DEFAULT                 0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define VDAC_CFG_SINERESET_DEFAULT                  (_VDAC_CFG_SINERESET_DEFAULT << 2)            /**< Shifted mode DEFAULT for VDAC_CFG           */
#define VDAC_CFG_CH0PRESCRST                        (0x1UL << 3)                                  /**< Channel 0 Start Reset Prescaler             */
#define _VDAC_CFG_CH0PRESCRST_SHIFT                 3                                             /**< Shift value for VDAC_CH0PRESCRST            */
#define _VDAC_CFG_CH0PRESCRST_MASK                  0x8UL                                         /**< Bit mask for VDAC_CH0PRESCRST               */
#define _VDAC_CFG_CH0PRESCRST_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define _VDAC_CFG_CH0PRESCRST_NORESETPRESC          0x00000000UL                                  /**< Mode NORESETPRESC for VDAC_CFG              */
#define _VDAC_CFG_CH0PRESCRST_RESETPRESC            0x00000001UL                                  /**< Mode RESETPRESC for VDAC_CFG                */
#define VDAC_CFG_CH0PRESCRST_DEFAULT                (_VDAC_CFG_CH0PRESCRST_DEFAULT << 3)          /**< Shifted mode DEFAULT for VDAC_CFG           */
#define VDAC_CFG_CH0PRESCRST_NORESETPRESC           (_VDAC_CFG_CH0PRESCRST_NORESETPRESC << 3)     /**< Shifted mode NORESETPRESC for VDAC_CFG      */
#define VDAC_CFG_CH0PRESCRST_RESETPRESC             (_VDAC_CFG_CH0PRESCRST_RESETPRESC << 3)       /**< Shifted mode RESETPRESC for VDAC_CFG        */
#define _VDAC_CFG_REFRSEL_SHIFT                     4                                             /**< Shift value for VDAC_REFRSEL                */
#define _VDAC_CFG_REFRSEL_MASK                      0x30UL                                        /**< Bit mask for VDAC_REFRSEL                   */
#define _VDAC_CFG_REFRSEL_DEFAULT                   0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define _VDAC_CFG_REFRSEL_V125                      0x00000000UL                                  /**< Mode V125 for VDAC_CFG                      */
#define _VDAC_CFG_REFRSEL_V25                       0x00000001UL                                  /**< Mode V25 for VDAC_CFG                       */
#define _VDAC_CFG_REFRSEL_VDD                       0x00000002UL                                  /**< Mode VDD for VDAC_CFG                       */
#define _VDAC_CFG_REFRSEL_EXT                       0x00000003UL                                  /**< Mode EXT for VDAC_CFG                       */
#define VDAC_CFG_REFRSEL_DEFAULT                    (_VDAC_CFG_REFRSEL_DEFAULT << 4)              /**< Shifted mode DEFAULT for VDAC_CFG           */
#define VDAC_CFG_REFRSEL_V125                       (_VDAC_CFG_REFRSEL_V125 << 4)                 /**< Shifted mode V125 for VDAC_CFG              */
#define VDAC_CFG_REFRSEL_V25                        (_VDAC_CFG_REFRSEL_V25 << 4)                  /**< Shifted mode V25 for VDAC_CFG               */
#define VDAC_CFG_REFRSEL_VDD                        (_VDAC_CFG_REFRSEL_VDD << 4)                  /**< Shifted mode VDD for VDAC_CFG               */
#define VDAC_CFG_REFRSEL_EXT                        (_VDAC_CFG_REFRSEL_EXT << 4)                  /**< Shifted mode EXT for VDAC_CFG               */
#define _VDAC_CFG_PRESC_SHIFT                       7                                             /**< Shift value for VDAC_PRESC                  */
#define _VDAC_CFG_PRESC_MASK                        0x3F80UL                                      /**< Bit mask for VDAC_PRESC                     */
#define _VDAC_CFG_PRESC_DEFAULT                     0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define VDAC_CFG_PRESC_DEFAULT                      (_VDAC_CFG_PRESC_DEFAULT << 7)                /**< Shifted mode DEFAULT for VDAC_CFG           */
#define _VDAC_CFG_TIMEROVRFLOWPERIOD_SHIFT          16                                            /**< Shift value for VDAC_TIMEROVRFLOWPERIOD     */
#define _VDAC_CFG_TIMEROVRFLOWPERIOD_MASK           0x70000UL                                     /**< Bit mask for VDAC_TIMEROVRFLOWPERIOD        */
#define _VDAC_CFG_TIMEROVRFLOWPERIOD_DEFAULT        0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES2        0x00000000UL                                  /**< Mode CYCLES2 for VDAC_CFG                   */
#define _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES4        0x00000001UL                                  /**< Mode CYCLES4 for VDAC_CFG                   */
#define _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES8        0x00000002UL                                  /**< Mode CYCLES8 for VDAC_CFG                   */
#define _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES16       0x00000003UL                                  /**< Mode CYCLES16 for VDAC_CFG                  */
#define _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES32       0x00000004UL                                  /**< Mode CYCLES32 for VDAC_CFG                  */
#define _VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES64       0x00000005UL                                  /**< Mode CYCLES64 for VDAC_CFG                  */
#define VDAC_CFG_TIMEROVRFLOWPERIOD_DEFAULT         (_VDAC_CFG_TIMEROVRFLOWPERIOD_DEFAULT << 16)  /**< Shifted mode DEFAULT for VDAC_CFG           */
#define VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES2         (_VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES2 << 16)  /**< Shifted mode CYCLES2 for VDAC_CFG           */
#define VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES4         (_VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES4 << 16)  /**< Shifted mode CYCLES4 for VDAC_CFG           */
#define VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES8         (_VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES8 << 16)  /**< Shifted mode CYCLES8 for VDAC_CFG           */
#define VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES16        (_VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES16 << 16) /**< Shifted mode CYCLES16 for VDAC_CFG          */
#define VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES32        (_VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES32 << 16) /**< Shifted mode CYCLES32 for VDAC_CFG          */
#define VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES64        (_VDAC_CFG_TIMEROVRFLOWPERIOD_CYCLES64 << 16) /**< Shifted mode CYCLES64 for VDAC_CFG          */
#define _VDAC_CFG_REFRESHPERIOD_SHIFT               20                                            /**< Shift value for VDAC_REFRESHPERIOD          */
#define _VDAC_CFG_REFRESHPERIOD_MASK                0x700000UL                                    /**< Bit mask for VDAC_REFRESHPERIOD             */
#define _VDAC_CFG_REFRESHPERIOD_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define _VDAC_CFG_REFRESHPERIOD_CYCLES2             0x00000000UL                                  /**< Mode CYCLES2 for VDAC_CFG                   */
#define _VDAC_CFG_REFRESHPERIOD_CYCLES4             0x00000001UL                                  /**< Mode CYCLES4 for VDAC_CFG                   */
#define _VDAC_CFG_REFRESHPERIOD_CYCLES8             0x00000002UL                                  /**< Mode CYCLES8 for VDAC_CFG                   */
#define _VDAC_CFG_REFRESHPERIOD_CYCLES16            0x00000003UL                                  /**< Mode CYCLES16 for VDAC_CFG                  */
#define _VDAC_CFG_REFRESHPERIOD_CYCLES32            0x00000004UL                                  /**< Mode CYCLES32 for VDAC_CFG                  */
#define _VDAC_CFG_REFRESHPERIOD_CYCLES64            0x00000005UL                                  /**< Mode CYCLES64 for VDAC_CFG                  */
#define _VDAC_CFG_REFRESHPERIOD_CYCLES128           0x00000006UL                                  /**< Mode CYCLES128 for VDAC_CFG                 */
#define _VDAC_CFG_REFRESHPERIOD_CYCLES256           0x00000007UL                                  /**< Mode CYCLES256 for VDAC_CFG                 */
#define VDAC_CFG_REFRESHPERIOD_DEFAULT              (_VDAC_CFG_REFRESHPERIOD_DEFAULT << 20)       /**< Shifted mode DEFAULT for VDAC_CFG           */
#define VDAC_CFG_REFRESHPERIOD_CYCLES2              (_VDAC_CFG_REFRESHPERIOD_CYCLES2 << 20)       /**< Shifted mode CYCLES2 for VDAC_CFG           */
#define VDAC_CFG_REFRESHPERIOD_CYCLES4              (_VDAC_CFG_REFRESHPERIOD_CYCLES4 << 20)       /**< Shifted mode CYCLES4 for VDAC_CFG           */
#define VDAC_CFG_REFRESHPERIOD_CYCLES8              (_VDAC_CFG_REFRESHPERIOD_CYCLES8 << 20)       /**< Shifted mode CYCLES8 for VDAC_CFG           */
#define VDAC_CFG_REFRESHPERIOD_CYCLES16             (_VDAC_CFG_REFRESHPERIOD_CYCLES16 << 20)      /**< Shifted mode CYCLES16 for VDAC_CFG          */
#define VDAC_CFG_REFRESHPERIOD_CYCLES32             (_VDAC_CFG_REFRESHPERIOD_CYCLES32 << 20)      /**< Shifted mode CYCLES32 for VDAC_CFG          */
#define VDAC_CFG_REFRESHPERIOD_CYCLES64             (_VDAC_CFG_REFRESHPERIOD_CYCLES64 << 20)      /**< Shifted mode CYCLES64 for VDAC_CFG          */
#define VDAC_CFG_REFRESHPERIOD_CYCLES128            (_VDAC_CFG_REFRESHPERIOD_CYCLES128 << 20)     /**< Shifted mode CYCLES128 for VDAC_CFG         */
#define VDAC_CFG_REFRESHPERIOD_CYCLES256            (_VDAC_CFG_REFRESHPERIOD_CYCLES256 << 20)     /**< Shifted mode CYCLES256 for VDAC_CFG         */
#define VDAC_CFG_BIASKEEPWARM                       (0x1UL << 24)                                 /**< Bias Keepwarm Mode Enable                   */
#define _VDAC_CFG_BIASKEEPWARM_SHIFT                24                                            /**< Shift value for VDAC_BIASKEEPWARM           */
#define _VDAC_CFG_BIASKEEPWARM_MASK                 0x1000000UL                                   /**< Bit mask for VDAC_BIASKEEPWARM              */
#define _VDAC_CFG_BIASKEEPWARM_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define VDAC_CFG_BIASKEEPWARM_DEFAULT               (_VDAC_CFG_BIASKEEPWARM_DEFAULT << 24)        /**< Shifted mode DEFAULT for VDAC_CFG           */
#define VDAC_CFG_DMAWU                              (0x1UL << 25)                                 /**< VDAC DMA Wakeup                             */
#define _VDAC_CFG_DMAWU_SHIFT                       25                                            /**< Shift value for VDAC_DMAWU                  */
#define _VDAC_CFG_DMAWU_MASK                        0x2000000UL                                   /**< Bit mask for VDAC_DMAWU                     */
#define _VDAC_CFG_DMAWU_DEFAULT                     0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define VDAC_CFG_DMAWU_DEFAULT                      (_VDAC_CFG_DMAWU_DEFAULT << 25)               /**< Shifted mode DEFAULT for VDAC_CFG           */
#define VDAC_CFG_ONDEMANDCLK                        (0x1UL << 26)                                 /**< Always allow clk_dac                        */
#define _VDAC_CFG_ONDEMANDCLK_SHIFT                 26                                            /**< Shift value for VDAC_ONDEMANDCLK            */
#define _VDAC_CFG_ONDEMANDCLK_MASK                  0x4000000UL                                   /**< Bit mask for VDAC_ONDEMANDCLK               */
#define _VDAC_CFG_ONDEMANDCLK_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define VDAC_CFG_ONDEMANDCLK_DEFAULT                (_VDAC_CFG_ONDEMANDCLK_DEFAULT << 26)         /**< Shifted mode DEFAULT for VDAC_CFG           */
#define VDAC_CFG_DBGHALT                            (0x1UL << 27)                                 /**< Debug Halt                                  */
#define _VDAC_CFG_DBGHALT_SHIFT                     27                                            /**< Shift value for VDAC_DBGHALT                */
#define _VDAC_CFG_DBGHALT_MASK                      0x8000000UL                                   /**< Bit mask for VDAC_DBGHALT                   */
#define _VDAC_CFG_DBGHALT_DEFAULT                   0x00000000UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define _VDAC_CFG_DBGHALT_NORMAL                    0x00000000UL                                  /**< Mode NORMAL for VDAC_CFG                    */
#define _VDAC_CFG_DBGHALT_HALT                      0x00000001UL                                  /**< Mode HALT for VDAC_CFG                      */
#define VDAC_CFG_DBGHALT_DEFAULT                    (_VDAC_CFG_DBGHALT_DEFAULT << 27)             /**< Shifted mode DEFAULT for VDAC_CFG           */
#define VDAC_CFG_DBGHALT_NORMAL                     (_VDAC_CFG_DBGHALT_NORMAL << 27)              /**< Shifted mode NORMAL for VDAC_CFG            */
#define VDAC_CFG_DBGHALT_HALT                       (_VDAC_CFG_DBGHALT_HALT << 27)                /**< Shifted mode HALT for VDAC_CFG              */
#define _VDAC_CFG_WARMUPTIME_SHIFT                  28                                            /**< Shift value for VDAC_WARMUPTIME             */
#define _VDAC_CFG_WARMUPTIME_MASK                   0x70000000UL                                  /**< Bit mask for VDAC_WARMUPTIME                */
#define _VDAC_CFG_WARMUPTIME_DEFAULT                0x00000002UL                                  /**< Mode DEFAULT for VDAC_CFG                   */
#define VDAC_CFG_WARMUPTIME_DEFAULT                 (_VDAC_CFG_WARMUPTIME_DEFAULT << 28)          /**< Shifted mode DEFAULT for VDAC_CFG           */

/* Bit fields for VDAC STATUS */
#define _VDAC_STATUS_RESETVALUE                     0x00000000UL                                   /**< Default value for VDAC_STATUS               */
#define _VDAC_STATUS_MASK                           0xFCDBF333UL                                   /**< Mask for VDAC_STATUS                        */
#define VDAC_STATUS_CH0ENS                          (0x1UL << 0)                                   /**< Channel 0 Enabled Status                    */
#define _VDAC_STATUS_CH0ENS_SHIFT                   0                                              /**< Shift value for VDAC_CH0ENS                 */
#define _VDAC_STATUS_CH0ENS_MASK                    0x1UL                                          /**< Bit mask for VDAC_CH0ENS                    */
#define _VDAC_STATUS_CH0ENS_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH0ENS_DEFAULT                  (_VDAC_STATUS_CH0ENS_DEFAULT << 0)             /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_CH1ENS                          (0x1UL << 1)                                   /**< Channel 1 Enabled Status                    */
#define _VDAC_STATUS_CH1ENS_SHIFT                   1                                              /**< Shift value for VDAC_CH1ENS                 */
#define _VDAC_STATUS_CH1ENS_MASK                    0x2UL                                          /**< Bit mask for VDAC_CH1ENS                    */
#define _VDAC_STATUS_CH1ENS_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH1ENS_DEFAULT                  (_VDAC_STATUS_CH1ENS_DEFAULT << 1)             /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_CH0WARM                         (0x1UL << 4)                                   /**< Channel 0 Warmed Status                     */
#define _VDAC_STATUS_CH0WARM_SHIFT                  4                                              /**< Shift value for VDAC_CH0WARM                */
#define _VDAC_STATUS_CH0WARM_MASK                   0x10UL                                         /**< Bit mask for VDAC_CH0WARM                   */
#define _VDAC_STATUS_CH0WARM_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH0WARM_DEFAULT                 (_VDAC_STATUS_CH0WARM_DEFAULT << 4)            /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_CH1WARM                         (0x1UL << 5)                                   /**< Channel 1 Warmed Status                     */
#define _VDAC_STATUS_CH1WARM_SHIFT                  5                                              /**< Shift value for VDAC_CH1WARM                */
#define _VDAC_STATUS_CH1WARM_MASK                   0x20UL                                         /**< Bit mask for VDAC_CH1WARM                   */
#define _VDAC_STATUS_CH1WARM_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH1WARM_DEFAULT                 (_VDAC_STATUS_CH1WARM_DEFAULT << 5)            /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_CH0FIFOFULL                     (0x1UL << 8)                                   /**< Channel 0 FIFO Full Status                  */
#define _VDAC_STATUS_CH0FIFOFULL_SHIFT              8                                              /**< Shift value for VDAC_CH0FIFOFULL            */
#define _VDAC_STATUS_CH0FIFOFULL_MASK               0x100UL                                        /**< Bit mask for VDAC_CH0FIFOFULL               */
#define _VDAC_STATUS_CH0FIFOFULL_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH0FIFOFULL_DEFAULT             (_VDAC_STATUS_CH0FIFOFULL_DEFAULT << 8)        /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_CH1FIFOFULL                     (0x1UL << 9)                                   /**< Channel 1 FIFO Full Status                  */
#define _VDAC_STATUS_CH1FIFOFULL_SHIFT              9                                              /**< Shift value for VDAC_CH1FIFOFULL            */
#define _VDAC_STATUS_CH1FIFOFULL_MASK               0x200UL                                        /**< Bit mask for VDAC_CH1FIFOFULL               */
#define _VDAC_STATUS_CH1FIFOFULL_DEFAULT            0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH1FIFOFULL_DEFAULT             (_VDAC_STATUS_CH1FIFOFULL_DEFAULT << 9)        /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define _VDAC_STATUS_CH0FIFOCNT_SHIFT               12                                             /**< Shift value for VDAC_CH0FIFOCNT             */
#define _VDAC_STATUS_CH0FIFOCNT_MASK                0x7000UL                                       /**< Bit mask for VDAC_CH0FIFOCNT                */
#define _VDAC_STATUS_CH0FIFOCNT_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH0FIFOCNT_DEFAULT              (_VDAC_STATUS_CH0FIFOCNT_DEFAULT << 12)        /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define _VDAC_STATUS_CH1FIFOCNT_SHIFT               15                                             /**< Shift value for VDAC_CH1FIFOCNT             */
#define _VDAC_STATUS_CH1FIFOCNT_MASK                0x38000UL                                      /**< Bit mask for VDAC_CH1FIFOCNT                */
#define _VDAC_STATUS_CH1FIFOCNT_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH1FIFOCNT_DEFAULT              (_VDAC_STATUS_CH1FIFOCNT_DEFAULT << 15)        /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_CH0CURRENTSTATE                 (0x1UL << 19)                                  /**< Channel 0 Current Status                    */
#define _VDAC_STATUS_CH0CURRENTSTATE_SHIFT          19                                             /**< Shift value for VDAC_CH0CURRENTSTATE        */
#define _VDAC_STATUS_CH0CURRENTSTATE_MASK           0x80000UL                                      /**< Bit mask for VDAC_CH0CURRENTSTATE           */
#define _VDAC_STATUS_CH0CURRENTSTATE_DEFAULT        0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH0CURRENTSTATE_DEFAULT         (_VDAC_STATUS_CH0CURRENTSTATE_DEFAULT << 19)   /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_CH1CURRENTSTATE                 (0x1UL << 20)                                  /**< Channel 1 Current Status                    */
#define _VDAC_STATUS_CH1CURRENTSTATE_SHIFT          20                                             /**< Shift value for VDAC_CH1CURRENTSTATE        */
#define _VDAC_STATUS_CH1CURRENTSTATE_MASK           0x100000UL                                     /**< Bit mask for VDAC_CH1CURRENTSTATE           */
#define _VDAC_STATUS_CH1CURRENTSTATE_DEFAULT        0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH1CURRENTSTATE_DEFAULT         (_VDAC_STATUS_CH1CURRENTSTATE_DEFAULT << 20)   /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_CH0FIFOEMPTY                    (0x1UL << 22)                                  /**< Channel 0 FIFO Empty Status                 */
#define _VDAC_STATUS_CH0FIFOEMPTY_SHIFT             22                                             /**< Shift value for VDAC_CH0FIFOEMPTY           */
#define _VDAC_STATUS_CH0FIFOEMPTY_MASK              0x400000UL                                     /**< Bit mask for VDAC_CH0FIFOEMPTY              */
#define _VDAC_STATUS_CH0FIFOEMPTY_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH0FIFOEMPTY_DEFAULT            (_VDAC_STATUS_CH0FIFOEMPTY_DEFAULT << 22)      /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_CH1FIFOEMPTY                    (0x1UL << 23)                                  /**< Channel 1 FIFO Empty Status                 */
#define _VDAC_STATUS_CH1FIFOEMPTY_SHIFT             23                                             /**< Shift value for VDAC_CH1FIFOEMPTY           */
#define _VDAC_STATUS_CH1FIFOEMPTY_MASK              0x800000UL                                     /**< Bit mask for VDAC_CH1FIFOEMPTY              */
#define _VDAC_STATUS_CH1FIFOEMPTY_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH1FIFOEMPTY_DEFAULT            (_VDAC_STATUS_CH1FIFOEMPTY_DEFAULT << 23)      /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_CH0FIFOFLBUSY                   (0x1UL << 26)                                  /**< CH0 FIFO Flush Sync Busy                    */
#define _VDAC_STATUS_CH0FIFOFLBUSY_SHIFT            26                                             /**< Shift value for VDAC_CH0FIFOFLBUSY          */
#define _VDAC_STATUS_CH0FIFOFLBUSY_MASK             0x4000000UL                                    /**< Bit mask for VDAC_CH0FIFOFLBUSY             */
#define _VDAC_STATUS_CH0FIFOFLBUSY_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH0FIFOFLBUSY_DEFAULT           (_VDAC_STATUS_CH0FIFOFLBUSY_DEFAULT << 26)     /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_CH1FIFOFLBUSY                   (0x1UL << 27)                                  /**< CH1 FIFO Flush Sync Busy                    */
#define _VDAC_STATUS_CH1FIFOFLBUSY_SHIFT            27                                             /**< Shift value for VDAC_CH1FIFOFLBUSY          */
#define _VDAC_STATUS_CH1FIFOFLBUSY_MASK             0x8000000UL                                    /**< Bit mask for VDAC_CH1FIFOFLBUSY             */
#define _VDAC_STATUS_CH1FIFOFLBUSY_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_CH1FIFOFLBUSY_DEFAULT           (_VDAC_STATUS_CH1FIFOFLBUSY_DEFAULT << 27)     /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_ABUSINPUTCONFLICT               (0x1UL << 28)                                  /**< ABUS Input Conflict Status                  */
#define _VDAC_STATUS_ABUSINPUTCONFLICT_SHIFT        28                                             /**< Shift value for VDAC_ABUSINPUTCONFLICT      */
#define _VDAC_STATUS_ABUSINPUTCONFLICT_MASK         0x10000000UL                                   /**< Bit mask for VDAC_ABUSINPUTCONFLICT         */
#define _VDAC_STATUS_ABUSINPUTCONFLICT_DEFAULT      0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_ABUSINPUTCONFLICT_DEFAULT       (_VDAC_STATUS_ABUSINPUTCONFLICT_DEFAULT << 28) /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_SINEACTIVE                      (0x1UL << 29)                                  /**< Sine Wave Output Status on Channel          */
#define _VDAC_STATUS_SINEACTIVE_SHIFT               29                                             /**< Shift value for VDAC_SINEACTIVE             */
#define _VDAC_STATUS_SINEACTIVE_MASK                0x20000000UL                                   /**< Bit mask for VDAC_SINEACTIVE                */
#define _VDAC_STATUS_SINEACTIVE_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_SINEACTIVE_DEFAULT              (_VDAC_STATUS_SINEACTIVE_DEFAULT << 29)        /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_ABUSALLOCERR                    (0x1UL << 30)                                  /**< ABUS Allocation Error Status                */
#define _VDAC_STATUS_ABUSALLOCERR_SHIFT             30                                             /**< Shift value for VDAC_ABUSALLOCERR           */
#define _VDAC_STATUS_ABUSALLOCERR_MASK              0x40000000UL                                   /**< Bit mask for VDAC_ABUSALLOCERR              */
#define _VDAC_STATUS_ABUSALLOCERR_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_ABUSALLOCERR_DEFAULT            (_VDAC_STATUS_ABUSALLOCERR_DEFAULT << 30)      /**< Shifted mode DEFAULT for VDAC_STATUS        */
#define VDAC_STATUS_SYNCBUSY                        (0x1UL << 31)                                  /**< Sync Busy Combined                          */
#define _VDAC_STATUS_SYNCBUSY_SHIFT                 31                                             /**< Shift value for VDAC_SYNCBUSY               */
#define _VDAC_STATUS_SYNCBUSY_MASK                  0x80000000UL                                   /**< Bit mask for VDAC_SYNCBUSY                  */
#define _VDAC_STATUS_SYNCBUSY_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for VDAC_STATUS                */
#define VDAC_STATUS_SYNCBUSY_DEFAULT                (_VDAC_STATUS_SYNCBUSY_DEFAULT << 31)          /**< Shifted mode DEFAULT for VDAC_STATUS        */

/* Bit fields for VDAC CH0CFG */
#define _VDAC_CH0CFG_RESETVALUE                     0x00000010UL                                   /**< Default value for VDAC_CH0CFG               */
#define _VDAC_CH0CFG_MASK                           0x00015B75UL                                   /**< Mask for VDAC_CH0CFG                        */
#define VDAC_CH0CFG_CONVMODE                        (0x1UL << 0)                                   /**< Channel 0 Conversion Mode                   */
#define _VDAC_CH0CFG_CONVMODE_SHIFT                 0                                              /**< Shift value for VDAC_CONVMODE               */
#define _VDAC_CH0CFG_CONVMODE_MASK                  0x1UL                                          /**< Bit mask for VDAC_CONVMODE                  */
#define _VDAC_CH0CFG_CONVMODE_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH0CFG                */
#define _VDAC_CH0CFG_CONVMODE_CONTINUOUS            0x00000000UL                                   /**< Mode CONTINUOUS for VDAC_CH0CFG             */
#define _VDAC_CH0CFG_CONVMODE_SAMPLEOFF             0x00000001UL                                   /**< Mode SAMPLEOFF for VDAC_CH0CFG              */
#define VDAC_CH0CFG_CONVMODE_DEFAULT                (_VDAC_CH0CFG_CONVMODE_DEFAULT << 0)           /**< Shifted mode DEFAULT for VDAC_CH0CFG        */
#define VDAC_CH0CFG_CONVMODE_CONTINUOUS             (_VDAC_CH0CFG_CONVMODE_CONTINUOUS << 0)        /**< Shifted mode CONTINUOUS for VDAC_CH0CFG     */
#define VDAC_CH0CFG_CONVMODE_SAMPLEOFF              (_VDAC_CH0CFG_CONVMODE_SAMPLEOFF << 0)         /**< Shifted mode SAMPLEOFF for VDAC_CH0CFG      */
#define VDAC_CH0CFG_POWERMODE                       (0x1UL << 2)                                   /**< Channel 0 Power Mode                        */
#define _VDAC_CH0CFG_POWERMODE_SHIFT                2                                              /**< Shift value for VDAC_POWERMODE              */
#define _VDAC_CH0CFG_POWERMODE_MASK                 0x4UL                                          /**< Bit mask for VDAC_POWERMODE                 */
#define _VDAC_CH0CFG_POWERMODE_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH0CFG                */
#define _VDAC_CH0CFG_POWERMODE_HIGHPOWER            0x00000000UL                                   /**< Mode HIGHPOWER for VDAC_CH0CFG              */
#define _VDAC_CH0CFG_POWERMODE_LOWPOWER             0x00000001UL                                   /**< Mode LOWPOWER for VDAC_CH0CFG               */
#define VDAC_CH0CFG_POWERMODE_DEFAULT               (_VDAC_CH0CFG_POWERMODE_DEFAULT << 2)          /**< Shifted mode DEFAULT for VDAC_CH0CFG        */
#define VDAC_CH0CFG_POWERMODE_HIGHPOWER             (_VDAC_CH0CFG_POWERMODE_HIGHPOWER << 2)        /**< Shifted mode HIGHPOWER for VDAC_CH0CFG      */
#define VDAC_CH0CFG_POWERMODE_LOWPOWER              (_VDAC_CH0CFG_POWERMODE_LOWPOWER << 2)         /**< Shifted mode LOWPOWER for VDAC_CH0CFG       */
#define _VDAC_CH0CFG_TRIGMODE_SHIFT                 4                                              /**< Shift value for VDAC_TRIGMODE               */
#define _VDAC_CH0CFG_TRIGMODE_MASK                  0x70UL                                         /**< Bit mask for VDAC_TRIGMODE                  */
#define _VDAC_CH0CFG_TRIGMODE_DEFAULT               0x00000001UL                                   /**< Mode DEFAULT for VDAC_CH0CFG                */
#define _VDAC_CH0CFG_TRIGMODE_NONE                  0x00000000UL                                   /**< Mode NONE for VDAC_CH0CFG                   */
#define _VDAC_CH0CFG_TRIGMODE_SW                    0x00000001UL                                   /**< Mode SW for VDAC_CH0CFG                     */
#define _VDAC_CH0CFG_TRIGMODE_SYNCPRS               0x00000002UL                                   /**< Mode SYNCPRS for VDAC_CH0CFG                */
#define _VDAC_CH0CFG_TRIGMODE_LESENSE               0x00000003UL                                   /**< Mode LESENSE for VDAC_CH0CFG                */
#define _VDAC_CH0CFG_TRIGMODE_INTERNALTIMER         0x00000004UL                                   /**< Mode INTERNALTIMER for VDAC_CH0CFG          */
#define _VDAC_CH0CFG_TRIGMODE_ASYNCPRS              0x00000005UL                                   /**< Mode ASYNCPRS for VDAC_CH0CFG               */
#define VDAC_CH0CFG_TRIGMODE_DEFAULT                (_VDAC_CH0CFG_TRIGMODE_DEFAULT << 4)           /**< Shifted mode DEFAULT for VDAC_CH0CFG        */
#define VDAC_CH0CFG_TRIGMODE_NONE                   (_VDAC_CH0CFG_TRIGMODE_NONE << 4)              /**< Shifted mode NONE for VDAC_CH0CFG           */
#define VDAC_CH0CFG_TRIGMODE_SW                     (_VDAC_CH0CFG_TRIGMODE_SW << 4)                /**< Shifted mode SW for VDAC_CH0CFG             */
#define VDAC_CH0CFG_TRIGMODE_SYNCPRS                (_VDAC_CH0CFG_TRIGMODE_SYNCPRS << 4)           /**< Shifted mode SYNCPRS for VDAC_CH0CFG        */
#define VDAC_CH0CFG_TRIGMODE_LESENSE                (_VDAC_CH0CFG_TRIGMODE_LESENSE << 4)           /**< Shifted mode LESENSE for VDAC_CH0CFG        */
#define VDAC_CH0CFG_TRIGMODE_INTERNALTIMER          (_VDAC_CH0CFG_TRIGMODE_INTERNALTIMER << 4)     /**< Shifted mode INTERNALTIMER for VDAC_CH0CFG  */
#define VDAC_CH0CFG_TRIGMODE_ASYNCPRS               (_VDAC_CH0CFG_TRIGMODE_ASYNCPRS << 4)          /**< Shifted mode ASYNCPRS for VDAC_CH0CFG       */
#define _VDAC_CH0CFG_REFRESHSOURCE_SHIFT            8                                              /**< Shift value for VDAC_REFRESHSOURCE          */
#define _VDAC_CH0CFG_REFRESHSOURCE_MASK             0x300UL                                        /**< Bit mask for VDAC_REFRESHSOURCE             */
#define _VDAC_CH0CFG_REFRESHSOURCE_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH0CFG                */
#define _VDAC_CH0CFG_REFRESHSOURCE_NONE             0x00000000UL                                   /**< Mode NONE for VDAC_CH0CFG                   */
#define _VDAC_CH0CFG_REFRESHSOURCE_REFRESHTIMER     0x00000001UL                                   /**< Mode REFRESHTIMER for VDAC_CH0CFG           */
#define _VDAC_CH0CFG_REFRESHSOURCE_SYNCPRS          0x00000002UL                                   /**< Mode SYNCPRS for VDAC_CH0CFG                */
#define _VDAC_CH0CFG_REFRESHSOURCE_ASYNCPRS         0x00000003UL                                   /**< Mode ASYNCPRS for VDAC_CH0CFG               */
#define VDAC_CH0CFG_REFRESHSOURCE_DEFAULT           (_VDAC_CH0CFG_REFRESHSOURCE_DEFAULT << 8)      /**< Shifted mode DEFAULT for VDAC_CH0CFG        */
#define VDAC_CH0CFG_REFRESHSOURCE_NONE              (_VDAC_CH0CFG_REFRESHSOURCE_NONE << 8)         /**< Shifted mode NONE for VDAC_CH0CFG           */
#define VDAC_CH0CFG_REFRESHSOURCE_REFRESHTIMER      (_VDAC_CH0CFG_REFRESHSOURCE_REFRESHTIMER << 8) /**< Shifted mode REFRESHTIMER for VDAC_CH0CFG   */
#define VDAC_CH0CFG_REFRESHSOURCE_SYNCPRS           (_VDAC_CH0CFG_REFRESHSOURCE_SYNCPRS << 8)      /**< Shifted mode SYNCPRS for VDAC_CH0CFG        */
#define VDAC_CH0CFG_REFRESHSOURCE_ASYNCPRS          (_VDAC_CH0CFG_REFRESHSOURCE_ASYNCPRS << 8)     /**< Shifted mode ASYNCPRS for VDAC_CH0CFG       */
#define _VDAC_CH0CFG_FIFODVL_SHIFT                  11                                             /**< Shift value for VDAC_FIFODVL                */
#define _VDAC_CH0CFG_FIFODVL_MASK                   0x1800UL                                       /**< Bit mask for VDAC_FIFODVL                   */
#define _VDAC_CH0CFG_FIFODVL_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH0CFG                */
#define VDAC_CH0CFG_FIFODVL_DEFAULT                 (_VDAC_CH0CFG_FIFODVL_DEFAULT << 11)           /**< Shifted mode DEFAULT for VDAC_CH0CFG        */
#define VDAC_CH0CFG_HIGHCAPLOADEN                   (0x1UL << 14)                                  /**< Channel 0 High Cap Load Mode Enable         */
#define _VDAC_CH0CFG_HIGHCAPLOADEN_SHIFT            14                                             /**< Shift value for VDAC_HIGHCAPLOADEN          */
#define _VDAC_CH0CFG_HIGHCAPLOADEN_MASK             0x4000UL                                       /**< Bit mask for VDAC_HIGHCAPLOADEN             */
#define _VDAC_CH0CFG_HIGHCAPLOADEN_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH0CFG                */
#define VDAC_CH0CFG_HIGHCAPLOADEN_DEFAULT           (_VDAC_CH0CFG_HIGHCAPLOADEN_DEFAULT << 14)     /**< Shifted mode DEFAULT for VDAC_CH0CFG        */
#define VDAC_CH0CFG_KEEPWARM                        (0x1UL << 16)                                  /**< Channel 0 Keepwarm Mode Enable              */
#define _VDAC_CH0CFG_KEEPWARM_SHIFT                 16                                             /**< Shift value for VDAC_KEEPWARM               */
#define _VDAC_CH0CFG_KEEPWARM_MASK                  0x10000UL                                      /**< Bit mask for VDAC_KEEPWARM                  */
#define _VDAC_CH0CFG_KEEPWARM_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH0CFG                */
#define VDAC_CH0CFG_KEEPWARM_DEFAULT                (_VDAC_CH0CFG_KEEPWARM_DEFAULT << 16)          /**< Shifted mode DEFAULT for VDAC_CH0CFG        */

/* Bit fields for VDAC CH1CFG */
#define _VDAC_CH1CFG_RESETVALUE                     0x00000010UL                                   /**< Default value for VDAC_CH1CFG               */
#define _VDAC_CH1CFG_MASK                           0x00015B75UL                                   /**< Mask for VDAC_CH1CFG                        */
#define VDAC_CH1CFG_CONVMODE                        (0x1UL << 0)                                   /**< Channel 1 Conversion Mode                   */
#define _VDAC_CH1CFG_CONVMODE_SHIFT                 0                                              /**< Shift value for VDAC_CONVMODE               */
#define _VDAC_CH1CFG_CONVMODE_MASK                  0x1UL                                          /**< Bit mask for VDAC_CONVMODE                  */
#define _VDAC_CH1CFG_CONVMODE_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH1CFG                */
#define _VDAC_CH1CFG_CONVMODE_CONTINUOUS            0x00000000UL                                   /**< Mode CONTINUOUS for VDAC_CH1CFG             */
#define _VDAC_CH1CFG_CONVMODE_SAMPLEOFF             0x00000001UL                                   /**< Mode SAMPLEOFF for VDAC_CH1CFG              */
#define VDAC_CH1CFG_CONVMODE_DEFAULT                (_VDAC_CH1CFG_CONVMODE_DEFAULT << 0)           /**< Shifted mode DEFAULT for VDAC_CH1CFG        */
#define VDAC_CH1CFG_CONVMODE_CONTINUOUS             (_VDAC_CH1CFG_CONVMODE_CONTINUOUS << 0)        /**< Shifted mode CONTINUOUS for VDAC_CH1CFG     */
#define VDAC_CH1CFG_CONVMODE_SAMPLEOFF              (_VDAC_CH1CFG_CONVMODE_SAMPLEOFF << 0)         /**< Shifted mode SAMPLEOFF for VDAC_CH1CFG      */
#define VDAC_CH1CFG_POWERMODE                       (0x1UL << 2)                                   /**< Channel 1 Power Mode                        */
#define _VDAC_CH1CFG_POWERMODE_SHIFT                2                                              /**< Shift value for VDAC_POWERMODE              */
#define _VDAC_CH1CFG_POWERMODE_MASK                 0x4UL                                          /**< Bit mask for VDAC_POWERMODE                 */
#define _VDAC_CH1CFG_POWERMODE_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH1CFG                */
#define _VDAC_CH1CFG_POWERMODE_HIGHPOWER            0x00000000UL                                   /**< Mode HIGHPOWER for VDAC_CH1CFG              */
#define _VDAC_CH1CFG_POWERMODE_LOWPOWER             0x00000001UL                                   /**< Mode LOWPOWER for VDAC_CH1CFG               */
#define VDAC_CH1CFG_POWERMODE_DEFAULT               (_VDAC_CH1CFG_POWERMODE_DEFAULT << 2)          /**< Shifted mode DEFAULT for VDAC_CH1CFG        */
#define VDAC_CH1CFG_POWERMODE_HIGHPOWER             (_VDAC_CH1CFG_POWERMODE_HIGHPOWER << 2)        /**< Shifted mode HIGHPOWER for VDAC_CH1CFG      */
#define VDAC_CH1CFG_POWERMODE_LOWPOWER              (_VDAC_CH1CFG_POWERMODE_LOWPOWER << 2)         /**< Shifted mode LOWPOWER for VDAC_CH1CFG       */
#define _VDAC_CH1CFG_TRIGMODE_SHIFT                 4                                              /**< Shift value for VDAC_TRIGMODE               */
#define _VDAC_CH1CFG_TRIGMODE_MASK                  0x70UL                                         /**< Bit mask for VDAC_TRIGMODE                  */
#define _VDAC_CH1CFG_TRIGMODE_DEFAULT               0x00000001UL                                   /**< Mode DEFAULT for VDAC_CH1CFG                */
#define _VDAC_CH1CFG_TRIGMODE_NONE                  0x00000000UL                                   /**< Mode NONE for VDAC_CH1CFG                   */
#define _VDAC_CH1CFG_TRIGMODE_SW                    0x00000001UL                                   /**< Mode SW for VDAC_CH1CFG                     */
#define _VDAC_CH1CFG_TRIGMODE_SYNCPRS               0x00000002UL                                   /**< Mode SYNCPRS for VDAC_CH1CFG                */
#define _VDAC_CH1CFG_TRIGMODE_INTERNALTIMER         0x00000004UL                                   /**< Mode INTERNALTIMER for VDAC_CH1CFG          */
#define _VDAC_CH1CFG_TRIGMODE_ASYNCPRS              0x00000005UL                                   /**< Mode ASYNCPRS for VDAC_CH1CFG               */
#define VDAC_CH1CFG_TRIGMODE_DEFAULT                (_VDAC_CH1CFG_TRIGMODE_DEFAULT << 4)           /**< Shifted mode DEFAULT for VDAC_CH1CFG        */
#define VDAC_CH1CFG_TRIGMODE_NONE                   (_VDAC_CH1CFG_TRIGMODE_NONE << 4)              /**< Shifted mode NONE for VDAC_CH1CFG           */
#define VDAC_CH1CFG_TRIGMODE_SW                     (_VDAC_CH1CFG_TRIGMODE_SW << 4)                /**< Shifted mode SW for VDAC_CH1CFG             */
#define VDAC_CH1CFG_TRIGMODE_SYNCPRS                (_VDAC_CH1CFG_TRIGMODE_SYNCPRS << 4)           /**< Shifted mode SYNCPRS for VDAC_CH1CFG        */
#define VDAC_CH1CFG_TRIGMODE_INTERNALTIMER          (_VDAC_CH1CFG_TRIGMODE_INTERNALTIMER << 4)     /**< Shifted mode INTERNALTIMER for VDAC_CH1CFG  */
#define VDAC_CH1CFG_TRIGMODE_ASYNCPRS               (_VDAC_CH1CFG_TRIGMODE_ASYNCPRS << 4)          /**< Shifted mode ASYNCPRS for VDAC_CH1CFG       */
#define _VDAC_CH1CFG_REFRESHSOURCE_SHIFT            8                                              /**< Shift value for VDAC_REFRESHSOURCE          */
#define _VDAC_CH1CFG_REFRESHSOURCE_MASK             0x300UL                                        /**< Bit mask for VDAC_REFRESHSOURCE             */
#define _VDAC_CH1CFG_REFRESHSOURCE_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH1CFG                */
#define _VDAC_CH1CFG_REFRESHSOURCE_NONE             0x00000000UL                                   /**< Mode NONE for VDAC_CH1CFG                   */
#define _VDAC_CH1CFG_REFRESHSOURCE_REFRESHTIMER     0x00000001UL                                   /**< Mode REFRESHTIMER for VDAC_CH1CFG           */
#define _VDAC_CH1CFG_REFRESHSOURCE_SYNCPRS          0x00000002UL                                   /**< Mode SYNCPRS for VDAC_CH1CFG                */
#define _VDAC_CH1CFG_REFRESHSOURCE_ASYNCPRS         0x00000003UL                                   /**< Mode ASYNCPRS for VDAC_CH1CFG               */
#define VDAC_CH1CFG_REFRESHSOURCE_DEFAULT           (_VDAC_CH1CFG_REFRESHSOURCE_DEFAULT << 8)      /**< Shifted mode DEFAULT for VDAC_CH1CFG        */
#define VDAC_CH1CFG_REFRESHSOURCE_NONE              (_VDAC_CH1CFG_REFRESHSOURCE_NONE << 8)         /**< Shifted mode NONE for VDAC_CH1CFG           */
#define VDAC_CH1CFG_REFRESHSOURCE_REFRESHTIMER      (_VDAC_CH1CFG_REFRESHSOURCE_REFRESHTIMER << 8) /**< Shifted mode REFRESHTIMER for VDAC_CH1CFG   */
#define VDAC_CH1CFG_REFRESHSOURCE_SYNCPRS           (_VDAC_CH1CFG_REFRESHSOURCE_SYNCPRS << 8)      /**< Shifted mode SYNCPRS for VDAC_CH1CFG        */
#define VDAC_CH1CFG_REFRESHSOURCE_ASYNCPRS          (_VDAC_CH1CFG_REFRESHSOURCE_ASYNCPRS << 8)     /**< Shifted mode ASYNCPRS for VDAC_CH1CFG       */
#define _VDAC_CH1CFG_FIFODVL_SHIFT                  11                                             /**< Shift value for VDAC_FIFODVL                */
#define _VDAC_CH1CFG_FIFODVL_MASK                   0x1800UL                                       /**< Bit mask for VDAC_FIFODVL                   */
#define _VDAC_CH1CFG_FIFODVL_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH1CFG                */
#define VDAC_CH1CFG_FIFODVL_DEFAULT                 (_VDAC_CH1CFG_FIFODVL_DEFAULT << 11)           /**< Shifted mode DEFAULT for VDAC_CH1CFG        */
#define VDAC_CH1CFG_HIGHCAPLOADEN                   (0x1UL << 14)                                  /**< Channel 1 High Cap Load Mode Enable         */
#define _VDAC_CH1CFG_HIGHCAPLOADEN_SHIFT            14                                             /**< Shift value for VDAC_HIGHCAPLOADEN          */
#define _VDAC_CH1CFG_HIGHCAPLOADEN_MASK             0x4000UL                                       /**< Bit mask for VDAC_HIGHCAPLOADEN             */
#define _VDAC_CH1CFG_HIGHCAPLOADEN_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH1CFG                */
#define VDAC_CH1CFG_HIGHCAPLOADEN_DEFAULT           (_VDAC_CH1CFG_HIGHCAPLOADEN_DEFAULT << 14)     /**< Shifted mode DEFAULT for VDAC_CH1CFG        */
#define VDAC_CH1CFG_KEEPWARM                        (0x1UL << 16)                                  /**< Channel 1 Keepwarm Mode Enable              */
#define _VDAC_CH1CFG_KEEPWARM_SHIFT                 16                                             /**< Shift value for VDAC_KEEPWARM               */
#define _VDAC_CH1CFG_KEEPWARM_MASK                  0x10000UL                                      /**< Bit mask for VDAC_KEEPWARM                  */
#define _VDAC_CH1CFG_KEEPWARM_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for VDAC_CH1CFG                */
#define VDAC_CH1CFG_KEEPWARM_DEFAULT                (_VDAC_CH1CFG_KEEPWARM_DEFAULT << 16)          /**< Shifted mode DEFAULT for VDAC_CH1CFG        */

/* Bit fields for VDAC CMD */
#define _VDAC_CMD_RESETVALUE                        0x00000000UL                            /**< Default value for VDAC_CMD                  */
#define _VDAC_CMD_MASK                              0x00000F33UL                            /**< Mask for VDAC_CMD                           */
#define VDAC_CMD_CH0EN                              (0x1UL << 0)                            /**< DAC Channel 0 Enable                        */
#define _VDAC_CMD_CH0EN_SHIFT                       0                                       /**< Shift value for VDAC_CH0EN                  */
#define _VDAC_CMD_CH0EN_MASK                        0x1UL                                   /**< Bit mask for VDAC_CH0EN                     */
#define _VDAC_CMD_CH0EN_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for VDAC_CMD                   */
#define VDAC_CMD_CH0EN_DEFAULT                      (_VDAC_CMD_CH0EN_DEFAULT << 0)          /**< Shifted mode DEFAULT for VDAC_CMD           */
#define VDAC_CMD_CH0DIS                             (0x1UL << 1)                            /**< DAC Channel 0 Disable                       */
#define _VDAC_CMD_CH0DIS_SHIFT                      1                                       /**< Shift value for VDAC_CH0DIS                 */
#define _VDAC_CMD_CH0DIS_MASK                       0x2UL                                   /**< Bit mask for VDAC_CH0DIS                    */
#define _VDAC_CMD_CH0DIS_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for VDAC_CMD                   */
#define VDAC_CMD_CH0DIS_DEFAULT                     (_VDAC_CMD_CH0DIS_DEFAULT << 1)         /**< Shifted mode DEFAULT for VDAC_CMD           */
#define VDAC_CMD_CH1EN                              (0x1UL << 4)                            /**< DAC Channel 1 Enable                        */
#define _VDAC_CMD_CH1EN_SHIFT                       4                                       /**< Shift value for VDAC_CH1EN                  */
#define _VDAC_CMD_CH1EN_MASK                        0x10UL                                  /**< Bit mask for VDAC_CH1EN                     */
#define _VDAC_CMD_CH1EN_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for VDAC_CMD                   */
#define VDAC_CMD_CH1EN_DEFAULT                      (_VDAC_CMD_CH1EN_DEFAULT << 4)          /**< Shifted mode DEFAULT for VDAC_CMD           */
#define VDAC_CMD_CH1DIS                             (0x1UL << 5)                            /**< DAC Channel 1 Disable                       */
#define _VDAC_CMD_CH1DIS_SHIFT                      5                                       /**< Shift value for VDAC_CH1DIS                 */
#define _VDAC_CMD_CH1DIS_MASK                       0x20UL                                  /**< Bit mask for VDAC_CH1DIS                    */
#define _VDAC_CMD_CH1DIS_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for VDAC_CMD                   */
#define VDAC_CMD_CH1DIS_DEFAULT                     (_VDAC_CMD_CH1DIS_DEFAULT << 5)         /**< Shifted mode DEFAULT for VDAC_CMD           */
#define VDAC_CMD_CH0FIFOFLUSH                       (0x1UL << 8)                            /**< CH0 WFIFO Flush                             */
#define _VDAC_CMD_CH0FIFOFLUSH_SHIFT                8                                       /**< Shift value for VDAC_CH0FIFOFLUSH           */
#define _VDAC_CMD_CH0FIFOFLUSH_MASK                 0x100UL                                 /**< Bit mask for VDAC_CH0FIFOFLUSH              */
#define _VDAC_CMD_CH0FIFOFLUSH_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for VDAC_CMD                   */
#define VDAC_CMD_CH0FIFOFLUSH_DEFAULT               (_VDAC_CMD_CH0FIFOFLUSH_DEFAULT << 8)   /**< Shifted mode DEFAULT for VDAC_CMD           */
#define VDAC_CMD_CH1FIFOFLUSH                       (0x1UL << 9)                            /**< CH1 WFIFO Flush                             */
#define _VDAC_CMD_CH1FIFOFLUSH_SHIFT                9                                       /**< Shift value for VDAC_CH1FIFOFLUSH           */
#define _VDAC_CMD_CH1FIFOFLUSH_MASK                 0x200UL                                 /**< Bit mask for VDAC_CH1FIFOFLUSH              */
#define _VDAC_CMD_CH1FIFOFLUSH_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for VDAC_CMD                   */
#define VDAC_CMD_CH1FIFOFLUSH_DEFAULT               (_VDAC_CMD_CH1FIFOFLUSH_DEFAULT << 9)   /**< Shifted mode DEFAULT for VDAC_CMD           */
#define VDAC_CMD_SINEMODESTART                      (0x1UL << 10)                           /**< Start Sine Wave Generation                  */
#define _VDAC_CMD_SINEMODESTART_SHIFT               10                                      /**< Shift value for VDAC_SINEMODESTART          */
#define _VDAC_CMD_SINEMODESTART_MASK                0x400UL                                 /**< Bit mask for VDAC_SINEMODESTART             */
#define _VDAC_CMD_SINEMODESTART_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for VDAC_CMD                   */
#define VDAC_CMD_SINEMODESTART_DEFAULT              (_VDAC_CMD_SINEMODESTART_DEFAULT << 10) /**< Shifted mode DEFAULT for VDAC_CMD           */
#define VDAC_CMD_SINEMODESTOP                       (0x1UL << 11)                           /**< Stop Sine Wave Generation                   */
#define _VDAC_CMD_SINEMODESTOP_SHIFT                11                                      /**< Shift value for VDAC_SINEMODESTOP           */
#define _VDAC_CMD_SINEMODESTOP_MASK                 0x800UL                                 /**< Bit mask for VDAC_SINEMODESTOP              */
#define _VDAC_CMD_SINEMODESTOP_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for VDAC_CMD                   */
#define VDAC_CMD_SINEMODESTOP_DEFAULT               (_VDAC_CMD_SINEMODESTOP_DEFAULT << 11)  /**< Shifted mode DEFAULT for VDAC_CMD           */

/* Bit fields for VDAC IF */
#define _VDAC_IF_RESETVALUE                         0x00000000UL                               /**< Default value for VDAC_IF                   */
#define _VDAC_IF_MASK                               0x04340333UL                               /**< Mask for VDAC_IF                            */
#define VDAC_IF_CH0CD                               (0x1UL << 0)                               /**< CH0 Conversion Done Interrupt Flag          */
#define _VDAC_IF_CH0CD_SHIFT                        0                                          /**< Shift value for VDAC_CH0CD                  */
#define _VDAC_IF_CH0CD_MASK                         0x1UL                                      /**< Bit mask for VDAC_CH0CD                     */
#define _VDAC_IF_CH0CD_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for VDAC_IF                    */
#define VDAC_IF_CH0CD_DEFAULT                       (_VDAC_IF_CH0CD_DEFAULT << 0)              /**< Shifted mode DEFAULT for VDAC_IF            */
#define VDAC_IF_CH1CD                               (0x1UL << 1)                               /**< CH1 Conversion Done Interrupt Flag          */
#define _VDAC_IF_CH1CD_SHIFT                        1                                          /**< Shift value for VDAC_CH1CD                  */
#define _VDAC_IF_CH1CD_MASK                         0x2UL                                      /**< Bit mask for VDAC_CH1CD                     */
#define _VDAC_IF_CH1CD_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for VDAC_IF                    */
#define VDAC_IF_CH1CD_DEFAULT                       (_VDAC_IF_CH1CD_DEFAULT << 1)              /**< Shifted mode DEFAULT for VDAC_IF            */
#define VDAC_IF_CH0OF                               (0x1UL << 4)                               /**< CH0 Data Overflow Interrupt Flag            */
#define _VDAC_IF_CH0OF_SHIFT                        4                                          /**< Shift value for VDAC_CH0OF                  */
#define _VDAC_IF_CH0OF_MASK                         0x10UL                                     /**< Bit mask for VDAC_CH0OF                     */
#define _VDAC_IF_CH0OF_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for VDAC_IF                    */
#define VDAC_IF_CH0OF_DEFAULT                       (_VDAC_IF_CH0OF_DEFAULT << 4)              /**< Shifted mode DEFAULT for VDAC_IF            */
#define VDAC_IF_CH1OF                               (0x1UL << 5)                               /**< CH1 Data Overflow Interrupt Flag            */
#define _VDAC_IF_CH1OF_SHIFT                        5                                          /**< Shift value for VDAC_CH1OF                  */
#define _VDAC_IF_CH1OF_MASK                         0x20UL                                     /**< Bit mask for VDAC_CH1OF                     */
#define _VDAC_IF_CH1OF_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for VDAC_IF                    */
#define VDAC_IF_CH1OF_DEFAULT                       (_VDAC_IF_CH1OF_DEFAULT << 5)              /**< Shifted mode DEFAULT for VDAC_IF            */
#define VDAC_IF_CH0UF                               (0x1UL << 8)                               /**< CH0 Data Underflow Interrupt Flag           */
#define _VDAC_IF_CH0UF_SHIFT                        8                                          /**< Shift value for VDAC_CH0UF                  */
#define _VDAC_IF_CH0UF_MASK                         0x100UL                                    /**< Bit mask for VDAC_CH0UF                     */
#define _VDAC_IF_CH0UF_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for VDAC_IF                    */
#define VDAC_IF_CH0UF_DEFAULT                       (_VDAC_IF_CH0UF_DEFAULT << 8)              /**< Shifted mode DEFAULT for VDAC_IF            */
#define VDAC_IF_CH1UF                               (0x1UL << 9)                               /**< CH1 Data Underflow Interrupt Flag           */
#define _VDAC_IF_CH1UF_SHIFT                        9                                          /**< Shift value for VDAC_CH1UF                  */
#define _VDAC_IF_CH1UF_MASK                         0x200UL                                    /**< Bit mask for VDAC_CH1UF                     */
#define _VDAC_IF_CH1UF_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for VDAC_IF                    */
#define VDAC_IF_CH1UF_DEFAULT                       (_VDAC_IF_CH1UF_DEFAULT << 9)              /**< Shifted mode DEFAULT for VDAC_IF            */
#define VDAC_IF_ABUSALLOCERR                        (0x1UL << 18)                              /**< ABUS Port Allocation Error Flag             */
#define _VDAC_IF_ABUSALLOCERR_SHIFT                 18                                         /**< Shift value for VDAC_ABUSALLOCERR           */
#define _VDAC_IF_ABUSALLOCERR_MASK                  0x40000UL                                  /**< Bit mask for VDAC_ABUSALLOCERR              */
#define _VDAC_IF_ABUSALLOCERR_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for VDAC_IF                    */
#define VDAC_IF_ABUSALLOCERR_DEFAULT                (_VDAC_IF_ABUSALLOCERR_DEFAULT << 18)      /**< Shifted mode DEFAULT for VDAC_IF            */
#define VDAC_IF_CH0DVL                              (0x1UL << 20)                              /**< CH0 Data Valid Level Interrupt Flag         */
#define _VDAC_IF_CH0DVL_SHIFT                       20                                         /**< Shift value for VDAC_CH0DVL                 */
#define _VDAC_IF_CH0DVL_MASK                        0x100000UL                                 /**< Bit mask for VDAC_CH0DVL                    */
#define _VDAC_IF_CH0DVL_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for VDAC_IF                    */
#define VDAC_IF_CH0DVL_DEFAULT                      (_VDAC_IF_CH0DVL_DEFAULT << 20)            /**< Shifted mode DEFAULT for VDAC_IF            */
#define VDAC_IF_CH1DVL                              (0x1UL << 21)                              /**< CH1 Data Valid Level Interrupt Flag         */
#define _VDAC_IF_CH1DVL_SHIFT                       21                                         /**< Shift value for VDAC_CH1DVL                 */
#define _VDAC_IF_CH1DVL_MASK                        0x200000UL                                 /**< Bit mask for VDAC_CH1DVL                    */
#define _VDAC_IF_CH1DVL_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for VDAC_IF                    */
#define VDAC_IF_CH1DVL_DEFAULT                      (_VDAC_IF_CH1DVL_DEFAULT << 21)            /**< Shifted mode DEFAULT for VDAC_IF            */
#define VDAC_IF_ABUSINPUTCONFLICT                   (0x1UL << 26)                              /**< ABUS Input Conflict Error Flag              */
#define _VDAC_IF_ABUSINPUTCONFLICT_SHIFT            26                                         /**< Shift value for VDAC_ABUSINPUTCONFLICT      */
#define _VDAC_IF_ABUSINPUTCONFLICT_MASK             0x4000000UL                                /**< Bit mask for VDAC_ABUSINPUTCONFLICT         */
#define _VDAC_IF_ABUSINPUTCONFLICT_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for VDAC_IF                    */
#define VDAC_IF_ABUSINPUTCONFLICT_DEFAULT           (_VDAC_IF_ABUSINPUTCONFLICT_DEFAULT << 26) /**< Shifted mode DEFAULT for VDAC_IF            */

/* Bit fields for VDAC IEN */
#define _VDAC_IEN_RESETVALUE                        0x00000000UL                                /**< Default value for VDAC_IEN                  */
#define _VDAC_IEN_MASK                              0x04340333UL                                /**< Mask for VDAC_IEN                           */
#define VDAC_IEN_CH0CD                              (0x1UL << 0)                                /**< CH0 Conversion Done Interrupt Flag          */
#define _VDAC_IEN_CH0CD_SHIFT                       0                                           /**< Shift value for VDAC_CH0CD                  */
#define _VDAC_IEN_CH0CD_MASK                        0x1UL                                       /**< Bit mask for VDAC_CH0CD                     */
#define _VDAC_IEN_CH0CD_DEFAULT                     0x00000000UL                                /**< Mode DEFAULT for VDAC_IEN                   */
#define VDAC_IEN_CH0CD_DEFAULT                      (_VDAC_IEN_CH0CD_DEFAULT << 0)              /**< Shifted mode DEFAULT for VDAC_IEN           */
#define VDAC_IEN_CH1CD                              (0x1UL << 1)                                /**< CH1 Conversion Done Interrupt Flag          */
#define _VDAC_IEN_CH1CD_SHIFT                       1                                           /**< Shift value for VDAC_CH1CD                  */
#define _VDAC_IEN_CH1CD_MASK                        0x2UL                                       /**< Bit mask for VDAC_CH1CD                     */
#define _VDAC_IEN_CH1CD_DEFAULT                     0x00000000UL                                /**< Mode DEFAULT for VDAC_IEN                   */
#define VDAC_IEN_CH1CD_DEFAULT                      (_VDAC_IEN_CH1CD_DEFAULT << 1)              /**< Shifted mode DEFAULT for VDAC_IEN           */
#define VDAC_IEN_CH0OF                              (0x1UL << 4)                                /**< CH0 Data Overflow Interrupt Flag            */
#define _VDAC_IEN_CH0OF_SHIFT                       4                                           /**< Shift value for VDAC_CH0OF                  */
#define _VDAC_IEN_CH0OF_MASK                        0x10UL                                      /**< Bit mask for VDAC_CH0OF                     */
#define _VDAC_IEN_CH0OF_DEFAULT                     0x00000000UL                                /**< Mode DEFAULT for VDAC_IEN                   */
#define VDAC_IEN_CH0OF_DEFAULT                      (_VDAC_IEN_CH0OF_DEFAULT << 4)              /**< Shifted mode DEFAULT for VDAC_IEN           */
#define VDAC_IEN_CH1OF                              (0x1UL << 5)                                /**< CH1 Data Overflow Interrupt Flag            */
#define _VDAC_IEN_CH1OF_SHIFT                       5                                           /**< Shift value for VDAC_CH1OF                  */
#define _VDAC_IEN_CH1OF_MASK                        0x20UL                                      /**< Bit mask for VDAC_CH1OF                     */
#define _VDAC_IEN_CH1OF_DEFAULT                     0x00000000UL                                /**< Mode DEFAULT for VDAC_IEN                   */
#define VDAC_IEN_CH1OF_DEFAULT                      (_VDAC_IEN_CH1OF_DEFAULT << 5)              /**< Shifted mode DEFAULT for VDAC_IEN           */
#define VDAC_IEN_CH0UF                              (0x1UL << 8)                                /**< CH0 Data Underflow Interrupt Flag           */
#define _VDAC_IEN_CH0UF_SHIFT                       8                                           /**< Shift value for VDAC_CH0UF                  */
#define _VDAC_IEN_CH0UF_MASK                        0x100UL                                     /**< Bit mask for VDAC_CH0UF                     */
#define _VDAC_IEN_CH0UF_DEFAULT                     0x00000000UL                                /**< Mode DEFAULT for VDAC_IEN                   */
#define VDAC_IEN_CH0UF_DEFAULT                      (_VDAC_IEN_CH0UF_DEFAULT << 8)              /**< Shifted mode DEFAULT for VDAC_IEN           */
#define VDAC_IEN_CH1UF                              (0x1UL << 9)                                /**< CH1 Data Underflow Interrupt Flag           */
#define _VDAC_IEN_CH1UF_SHIFT                       9                                           /**< Shift value for VDAC_CH1UF                  */
#define _VDAC_IEN_CH1UF_MASK                        0x200UL                                     /**< Bit mask for VDAC_CH1UF                     */
#define _VDAC_IEN_CH1UF_DEFAULT                     0x00000000UL                                /**< Mode DEFAULT for VDAC_IEN                   */
#define VDAC_IEN_CH1UF_DEFAULT                      (_VDAC_IEN_CH1UF_DEFAULT << 9)              /**< Shifted mode DEFAULT for VDAC_IEN           */
#define VDAC_IEN_ABUSALLOCERR                       (0x1UL << 18)                               /**< ABUS Allocation Error Interrupt Flag        */
#define _VDAC_IEN_ABUSALLOCERR_SHIFT                18                                          /**< Shift value for VDAC_ABUSALLOCERR           */
#define _VDAC_IEN_ABUSALLOCERR_MASK                 0x40000UL                                   /**< Bit mask for VDAC_ABUSALLOCERR              */
#define _VDAC_IEN_ABUSALLOCERR_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for VDAC_IEN                   */
#define VDAC_IEN_ABUSALLOCERR_DEFAULT               (_VDAC_IEN_ABUSALLOCERR_DEFAULT << 18)      /**< Shifted mode DEFAULT for VDAC_IEN           */
#define VDAC_IEN_CH0DVL                             (0x1UL << 20)                               /**< CH0 Data Valid Level Interrupt Flag         */
#define _VDAC_IEN_CH0DVL_SHIFT                      20                                          /**< Shift value for VDAC_CH0DVL                 */
#define _VDAC_IEN_CH0DVL_MASK                       0x100000UL                                  /**< Bit mask for VDAC_CH0DVL                    */
#define _VDAC_IEN_CH0DVL_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for VDAC_IEN                   */
#define VDAC_IEN_CH0DVL_DEFAULT                     (_VDAC_IEN_CH0DVL_DEFAULT << 20)            /**< Shifted mode DEFAULT for VDAC_IEN           */
#define VDAC_IEN_CH1DVL                             (0x1UL << 21)                               /**< CH1 Data Valid Level Interrupt Flag         */
#define _VDAC_IEN_CH1DVL_SHIFT                      21                                          /**< Shift value for VDAC_CH1DVL                 */
#define _VDAC_IEN_CH1DVL_MASK                       0x200000UL                                  /**< Bit mask for VDAC_CH1DVL                    */
#define _VDAC_IEN_CH1DVL_DEFAULT                    0x00000000UL                                /**< Mode DEFAULT for VDAC_IEN                   */
#define VDAC_IEN_CH1DVL_DEFAULT                     (_VDAC_IEN_CH1DVL_DEFAULT << 21)            /**< Shifted mode DEFAULT for VDAC_IEN           */
#define VDAC_IEN_ABUSINPUTCONFLICT                  (0x1UL << 26)                               /**< ABUS Input Conflict Interrupt Flag          */
#define _VDAC_IEN_ABUSINPUTCONFLICT_SHIFT           26                                          /**< Shift value for VDAC_ABUSINPUTCONFLICT      */
#define _VDAC_IEN_ABUSINPUTCONFLICT_MASK            0x4000000UL                                 /**< Bit mask for VDAC_ABUSINPUTCONFLICT         */
#define _VDAC_IEN_ABUSINPUTCONFLICT_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for VDAC_IEN                   */
#define VDAC_IEN_ABUSINPUTCONFLICT_DEFAULT          (_VDAC_IEN_ABUSINPUTCONFLICT_DEFAULT << 26) /**< Shifted mode DEFAULT for VDAC_IEN           */

/* Bit fields for VDAC CH0F */
#define _VDAC_CH0F_RESETVALUE                       0x00000000UL                        /**< Default value for VDAC_CH0F                 */
#define _VDAC_CH0F_MASK                             0x00000FFFUL                        /**< Mask for VDAC_CH0F                          */
#define _VDAC_CH0F_DATA_SHIFT                       0                                   /**< Shift value for VDAC_DATA                   */
#define _VDAC_CH0F_DATA_MASK                        0xFFFUL                             /**< Bit mask for VDAC_DATA                      */
#define _VDAC_CH0F_DATA_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for VDAC_CH0F                  */
#define VDAC_CH0F_DATA_DEFAULT                      (_VDAC_CH0F_DATA_DEFAULT << 0)      /**< Shifted mode DEFAULT for VDAC_CH0F          */

/* Bit fields for VDAC CH1F */
#define _VDAC_CH1F_RESETVALUE                       0x00000000UL                        /**< Default value for VDAC_CH1F                 */
#define _VDAC_CH1F_MASK                             0x00000FFFUL                        /**< Mask for VDAC_CH1F                          */
#define _VDAC_CH1F_DATA_SHIFT                       0                                   /**< Shift value for VDAC_DATA                   */
#define _VDAC_CH1F_DATA_MASK                        0xFFFUL                             /**< Bit mask for VDAC_DATA                      */
#define _VDAC_CH1F_DATA_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for VDAC_CH1F                  */
#define VDAC_CH1F_DATA_DEFAULT                      (_VDAC_CH1F_DATA_DEFAULT << 0)      /**< Shifted mode DEFAULT for VDAC_CH1F          */

/* Bit fields for VDAC OUTCTRL */
#define _VDAC_OUTCTRL_RESETVALUE                    0x00000000UL                                 /**< Default value for VDAC_OUTCTRL              */
#define _VDAC_OUTCTRL_MASK                          0x7FDFF333UL                                 /**< Mask for VDAC_OUTCTRL                       */
#define VDAC_OUTCTRL_MAINOUTENCH0                   (0x1UL << 0)                                 /**< CH0 Main Output Enable                      */
#define _VDAC_OUTCTRL_MAINOUTENCH0_SHIFT            0                                            /**< Shift value for VDAC_MAINOUTENCH0           */
#define _VDAC_OUTCTRL_MAINOUTENCH0_MASK             0x1UL                                        /**< Bit mask for VDAC_MAINOUTENCH0              */
#define _VDAC_OUTCTRL_MAINOUTENCH0_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for VDAC_OUTCTRL               */
#define VDAC_OUTCTRL_MAINOUTENCH0_DEFAULT           (_VDAC_OUTCTRL_MAINOUTENCH0_DEFAULT << 0)    /**< Shifted mode DEFAULT for VDAC_OUTCTRL       */
#define VDAC_OUTCTRL_MAINOUTENCH1                   (0x1UL << 1)                                 /**< CH1 Main Output Enable                      */
#define _VDAC_OUTCTRL_MAINOUTENCH1_SHIFT            1                                            /**< Shift value for VDAC_MAINOUTENCH1           */
#define _VDAC_OUTCTRL_MAINOUTENCH1_MASK             0x2UL                                        /**< Bit mask for VDAC_MAINOUTENCH1              */
#define _VDAC_OUTCTRL_MAINOUTENCH1_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for VDAC_OUTCTRL               */
#define VDAC_OUTCTRL_MAINOUTENCH1_DEFAULT           (_VDAC_OUTCTRL_MAINOUTENCH1_DEFAULT << 1)    /**< Shifted mode DEFAULT for VDAC_OUTCTRL       */
#define VDAC_OUTCTRL_AUXOUTENCH0                    (0x1UL << 4)                                 /**< CH0 Alternative Output Enable               */
#define _VDAC_OUTCTRL_AUXOUTENCH0_SHIFT             4                                            /**< Shift value for VDAC_AUXOUTENCH0            */
#define _VDAC_OUTCTRL_AUXOUTENCH0_MASK              0x10UL                                       /**< Bit mask for VDAC_AUXOUTENCH0               */
#define _VDAC_OUTCTRL_AUXOUTENCH0_DEFAULT           0x00000000UL                                 /**< Mode DEFAULT for VDAC_OUTCTRL               */
#define VDAC_OUTCTRL_AUXOUTENCH0_DEFAULT            (_VDAC_OUTCTRL_AUXOUTENCH0_DEFAULT << 4)     /**< Shifted mode DEFAULT for VDAC_OUTCTRL       */
#define VDAC_OUTCTRL_AUXOUTENCH1                    (0x1UL << 5)                                 /**< CH1 Alternative Output Enable               */
#define _VDAC_OUTCTRL_AUXOUTENCH1_SHIFT             5                                            /**< Shift value for VDAC_AUXOUTENCH1            */
#define _VDAC_OUTCTRL_AUXOUTENCH1_MASK              0x20UL                                       /**< Bit mask for VDAC_AUXOUTENCH1               */
#define _VDAC_OUTCTRL_AUXOUTENCH1_DEFAULT           0x00000000UL                                 /**< Mode DEFAULT for VDAC_OUTCTRL               */
#define VDAC_OUTCTRL_AUXOUTENCH1_DEFAULT            (_VDAC_OUTCTRL_AUXOUTENCH1_DEFAULT << 5)     /**< Shifted mode DEFAULT for VDAC_OUTCTRL       */
#define VDAC_OUTCTRL_SHORTCH0                       (0x1UL << 8)                                 /**< CH1 Main and Alternative Output Short       */
#define _VDAC_OUTCTRL_SHORTCH0_SHIFT                8                                            /**< Shift value for VDAC_SHORTCH0               */
#define _VDAC_OUTCTRL_SHORTCH0_MASK                 0x100UL                                      /**< Bit mask for VDAC_SHORTCH0                  */
#define _VDAC_OUTCTRL_SHORTCH0_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for VDAC_OUTCTRL               */
#define VDAC_OUTCTRL_SHORTCH0_DEFAULT               (_VDAC_OUTCTRL_SHORTCH0_DEFAULT << 8)        /**< Shifted mode DEFAULT for VDAC_OUTCTRL       */
#define VDAC_OUTCTRL_SHORTCH1                       (0x1UL << 9)                                 /**< CH0 Main and Alternative Output Short       */
#define _VDAC_OUTCTRL_SHORTCH1_SHIFT                9                                            /**< Shift value for VDAC_SHORTCH1               */
#define _VDAC_OUTCTRL_SHORTCH1_MASK                 0x200UL                                      /**< Bit mask for VDAC_SHORTCH1                  */
#define _VDAC_OUTCTRL_SHORTCH1_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for VDAC_OUTCTRL               */
#define VDAC_OUTCTRL_SHORTCH1_DEFAULT               (_VDAC_OUTCTRL_SHORTCH1_DEFAULT << 9)        /**< Shifted mode DEFAULT for VDAC_OUTCTRL       */
#define _VDAC_OUTCTRL_ABUSPORTSELCH0_SHIFT          12                                           /**< Shift value for VDAC_ABUSPORTSELCH0         */
#define _VDAC_OUTCTRL_ABUSPORTSELCH0_MASK           0x7000UL                                     /**< Bit mask for VDAC_ABUSPORTSELCH0            */
#define _VDAC_OUTCTRL_ABUSPORTSELCH0_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for VDAC_OUTCTRL               */
#define _VDAC_OUTCTRL_ABUSPORTSELCH0_NONE           0x00000000UL                                 /**< Mode NONE for VDAC_OUTCTRL                  */
#define _VDAC_OUTCTRL_ABUSPORTSELCH0_PORTA          0x00000001UL                                 /**< Mode PORTA for VDAC_OUTCTRL                 */
#define _VDAC_OUTCTRL_ABUSPORTSELCH0_PORTB          0x00000002UL                                 /**< Mode PORTB for VDAC_OUTCTRL                 */
#define _VDAC_OUTCTRL_ABUSPORTSELCH0_PORTC          0x00000003UL                                 /**< Mode PORTC for VDAC_OUTCTRL                 */
#define _VDAC_OUTCTRL_ABUSPORTSELCH0_PORTD          0x00000004UL                                 /**< Mode PORTD for VDAC_OUTCTRL                 */
#define VDAC_OUTCTRL_ABUSPORTSELCH0_DEFAULT         (_VDAC_OUTCTRL_ABUSPORTSELCH0_DEFAULT << 12) /**< Shifted mode DEFAULT for VDAC_OUTCTRL       */
#define VDAC_OUTCTRL_ABUSPORTSELCH0_NONE            (_VDAC_OUTCTRL_ABUSPORTSELCH0_NONE << 12)    /**< Shifted mode NONE for VDAC_OUTCTRL          */
#define VDAC_OUTCTRL_ABUSPORTSELCH0_PORTA           (_VDAC_OUTCTRL_ABUSPORTSELCH0_PORTA << 12)   /**< Shifted mode PORTA for VDAC_OUTCTRL         */
#define VDAC_OUTCTRL_ABUSPORTSELCH0_PORTB           (_VDAC_OUTCTRL_ABUSPORTSELCH0_PORTB << 12)   /**< Shifted mode PORTB for VDAC_OUTCTRL         */
#define VDAC_OUTCTRL_ABUSPORTSELCH0_PORTC           (_VDAC_OUTCTRL_ABUSPORTSELCH0_PORTC << 12)   /**< Shifted mode PORTC for VDAC_OUTCTRL         */
#define VDAC_OUTCTRL_ABUSPORTSELCH0_PORTD           (_VDAC_OUTCTRL_ABUSPORTSELCH0_PORTD << 12)   /**< Shifted mode PORTD for VDAC_OUTCTRL         */
#define _VDAC_OUTCTRL_ABUSPINSELCH0_SHIFT           15                                           /**< Shift value for VDAC_ABUSPINSELCH0          */
#define _VDAC_OUTCTRL_ABUSPINSELCH0_MASK            0x1F8000UL                                   /**< Bit mask for VDAC_ABUSPINSELCH0             */
#define _VDAC_OUTCTRL_ABUSPINSELCH0_DEFAULT         0x00000000UL                                 /**< Mode DEFAULT for VDAC_OUTCTRL               */
#define VDAC_OUTCTRL_ABUSPINSELCH0_DEFAULT          (_VDAC_OUTCTRL_ABUSPINSELCH0_DEFAULT << 15)  /**< Shifted mode DEFAULT for VDAC_OUTCTRL       */
#define _VDAC_OUTCTRL_ABUSPORTSELCH1_SHIFT          22                                           /**< Shift value for VDAC_ABUSPORTSELCH1         */
#define _VDAC_OUTCTRL_ABUSPORTSELCH1_MASK           0x1C00000UL                                  /**< Bit mask for VDAC_ABUSPORTSELCH1            */
#define _VDAC_OUTCTRL_ABUSPORTSELCH1_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for VDAC_OUTCTRL               */
#define _VDAC_OUTCTRL_ABUSPORTSELCH1_NONE           0x00000000UL                                 /**< Mode NONE for VDAC_OUTCTRL                  */
#define _VDAC_OUTCTRL_ABUSPORTSELCH1_PORTA          0x00000001UL                                 /**< Mode PORTA for VDAC_OUTCTRL                 */
#define _VDAC_OUTCTRL_ABUSPORTSELCH1_PORTB          0x00000002UL                                 /**< Mode PORTB for VDAC_OUTCTRL                 */
#define _VDAC_OUTCTRL_ABUSPORTSELCH1_PORTC          0x00000003UL                                 /**< Mode PORTC for VDAC_OUTCTRL                 */
#define _VDAC_OUTCTRL_ABUSPORTSELCH1_PORTD          0x00000004UL                                 /**< Mode PORTD for VDAC_OUTCTRL                 */
#define VDAC_OUTCTRL_ABUSPORTSELCH1_DEFAULT         (_VDAC_OUTCTRL_ABUSPORTSELCH1_DEFAULT << 22) /**< Shifted mode DEFAULT for VDAC_OUTCTRL       */
#define VDAC_OUTCTRL_ABUSPORTSELCH1_NONE            (_VDAC_OUTCTRL_ABUSPORTSELCH1_NONE << 22)    /**< Shifted mode NONE for VDAC_OUTCTRL          */
#define VDAC_OUTCTRL_ABUSPORTSELCH1_PORTA           (_VDAC_OUTCTRL_ABUSPORTSELCH1_PORTA << 22)   /**< Shifted mode PORTA for VDAC_OUTCTRL         */
#define VDAC_OUTCTRL_ABUSPORTSELCH1_PORTB           (_VDAC_OUTCTRL_ABUSPORTSELCH1_PORTB << 22)   /**< Shifted mode PORTB for VDAC_OUTCTRL         */
#define VDAC_OUTCTRL_ABUSPORTSELCH1_PORTC           (_VDAC_OUTCTRL_ABUSPORTSELCH1_PORTC << 22)   /**< Shifted mode PORTC for VDAC_OUTCTRL         */
#define VDAC_OUTCTRL_ABUSPORTSELCH1_PORTD           (_VDAC_OUTCTRL_ABUSPORTSELCH1_PORTD << 22)   /**< Shifted mode PORTD for VDAC_OUTCTRL         */
#define _VDAC_OUTCTRL_ABUSPINSELCH1_SHIFT           25                                           /**< Shift value for VDAC_ABUSPINSELCH1          */
#define _VDAC_OUTCTRL_ABUSPINSELCH1_MASK            0x7E000000UL                                 /**< Bit mask for VDAC_ABUSPINSELCH1             */
#define _VDAC_OUTCTRL_ABUSPINSELCH1_DEFAULT         0x00000000UL                                 /**< Mode DEFAULT for VDAC_OUTCTRL               */
#define VDAC_OUTCTRL_ABUSPINSELCH1_DEFAULT          (_VDAC_OUTCTRL_ABUSPINSELCH1_DEFAULT << 25)  /**< Shifted mode DEFAULT for VDAC_OUTCTRL       */

/* Bit fields for VDAC OUTTIMERCFG */
#define _VDAC_OUTTIMERCFG_RESETVALUE                0x00000000UL                                     /**< Default value for VDAC_OUTTIMERCFG          */
#define _VDAC_OUTTIMERCFG_MASK                      0x01FF83FFUL                                     /**< Mask for VDAC_OUTTIMERCFG                   */
#define _VDAC_OUTTIMERCFG_CH0OUTHOLDTIME_SHIFT      0                                                /**< Shift value for VDAC_CH0OUTHOLDTIME         */
#define _VDAC_OUTTIMERCFG_CH0OUTHOLDTIME_MASK       0x3FFUL                                          /**< Bit mask for VDAC_CH0OUTHOLDTIME            */
#define _VDAC_OUTTIMERCFG_CH0OUTHOLDTIME_DEFAULT    0x00000000UL                                     /**< Mode DEFAULT for VDAC_OUTTIMERCFG           */
#define VDAC_OUTTIMERCFG_CH0OUTHOLDTIME_DEFAULT     (_VDAC_OUTTIMERCFG_CH0OUTHOLDTIME_DEFAULT << 0)  /**< Shifted mode DEFAULT for VDAC_OUTTIMERCFG   */
#define _VDAC_OUTTIMERCFG_CH1OUTHOLDTIME_SHIFT      15                                               /**< Shift value for VDAC_CH1OUTHOLDTIME         */
#define _VDAC_OUTTIMERCFG_CH1OUTHOLDTIME_MASK       0x1FF8000UL                                      /**< Bit mask for VDAC_CH1OUTHOLDTIME            */
#define _VDAC_OUTTIMERCFG_CH1OUTHOLDTIME_DEFAULT    0x00000000UL                                     /**< Mode DEFAULT for VDAC_OUTTIMERCFG           */
#define VDAC_OUTTIMERCFG_CH1OUTHOLDTIME_DEFAULT     (_VDAC_OUTTIMERCFG_CH1OUTHOLDTIME_DEFAULT << 15) /**< Shifted mode DEFAULT for VDAC_OUTTIMERCFG   */

/** @} End of group EFR32ZG23_VDAC_BitFields */
/** @} End of group EFR32ZG23_VDAC */
/** @} End of group Parts */

#endif /* EFR32ZG23_VDAC_H */
#ifdef __cplusplus
}
#endif

