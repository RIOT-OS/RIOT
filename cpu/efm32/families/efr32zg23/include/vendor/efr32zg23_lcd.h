/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 LCD register and bit field definitions
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

#ifndef EFR32ZG23_LCD_H
#define EFR32ZG23_LCD_H
#define LCD_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_LCD LCD
 * @{
 * @brief EFR32ZG23 LCD Register Declaration.
 *****************************************************************************/

/** LCD Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IPVERSION                                          */
  __IOM uint32_t EN;                            /**< Enable                                             */
  __IOM uint32_t SWRST;                         /**< Software Reset                                     */
  __IOM uint32_t CTRL;                          /**< Control Register                                   */
  __IOM uint32_t CMD;                           /**< Command register                                   */
  __IOM uint32_t DISPCTRL;                      /**< Display Control Register                           */
  __IOM uint32_t BACFG;                         /**< Blink and Animation Config Register                */
  __IOM uint32_t BACTRL;                        /**< Blink and Animation Control Register               */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t AREGA;                         /**< Animation Register A                               */
  __IOM uint32_t AREGB;                         /**< Animation Register B                               */
  __IOM uint32_t IF;                            /**< Interrupt Enable Register                          */
  __IOM uint32_t IEN;                           /**< Interrupt Enable                                   */
  __IOM uint32_t BIASCTRL;                      /**< Analog BIAS Control                                */
  __IOM uint32_t DISPCTRLX;                     /**< Display Control Extended                           */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t SEGD0;                         /**< Segment Data Register 0                            */
  uint32_t       RESERVED1[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t SEGD1;                         /**< Segment Data Register 1                            */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t SEGD2;                         /**< Segment Data Register 2                            */
  uint32_t       RESERVED3[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t SEGD3;                         /**< Segment Data Register 3                            */
  uint32_t       RESERVED4[25U];                /**< Reserved for future use                            */
  __IOM uint32_t UPDATECTRL;                    /**< Update Control                                     */
  uint32_t       RESERVED5[11U];                /**< Reserved for future use                            */
  __IOM uint32_t FRAMERATE;                     /**< Frame Rate                                         */
  uint32_t       RESERVED6[963U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_SET;                        /**< Enable                                             */
  __IOM uint32_t SWRST_SET;                     /**< Software Reset                                     */
  __IOM uint32_t CTRL_SET;                      /**< Control Register                                   */
  __IOM uint32_t CMD_SET;                       /**< Command register                                   */
  __IOM uint32_t DISPCTRL_SET;                  /**< Display Control Register                           */
  __IOM uint32_t BACFG_SET;                     /**< Blink and Animation Config Register                */
  __IOM uint32_t BACTRL_SET;                    /**< Blink and Animation Control Register               */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t AREGA_SET;                     /**< Animation Register A                               */
  __IOM uint32_t AREGB_SET;                     /**< Animation Register B                               */
  __IOM uint32_t IF_SET;                        /**< Interrupt Enable Register                          */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable                                   */
  __IOM uint32_t BIASCTRL_SET;                  /**< Analog BIAS Control                                */
  __IOM uint32_t DISPCTRLX_SET;                 /**< Display Control Extended                           */
  uint32_t       RESERVED7[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t SEGD0_SET;                     /**< Segment Data Register 0                            */
  uint32_t       RESERVED8[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t SEGD1_SET;                     /**< Segment Data Register 1                            */
  uint32_t       RESERVED9[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t SEGD2_SET;                     /**< Segment Data Register 2                            */
  uint32_t       RESERVED10[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SEGD3_SET;                     /**< Segment Data Register 3                            */
  uint32_t       RESERVED11[25U];               /**< Reserved for future use                            */
  __IOM uint32_t UPDATECTRL_SET;                /**< Update Control                                     */
  uint32_t       RESERVED12[11U];               /**< Reserved for future use                            */
  __IOM uint32_t FRAMERATE_SET;                 /**< Frame Rate                                         */
  uint32_t       RESERVED13[963U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_CLR;                        /**< Enable                                             */
  __IOM uint32_t SWRST_CLR;                     /**< Software Reset                                     */
  __IOM uint32_t CTRL_CLR;                      /**< Control Register                                   */
  __IOM uint32_t CMD_CLR;                       /**< Command register                                   */
  __IOM uint32_t DISPCTRL_CLR;                  /**< Display Control Register                           */
  __IOM uint32_t BACFG_CLR;                     /**< Blink and Animation Config Register                */
  __IOM uint32_t BACTRL_CLR;                    /**< Blink and Animation Control Register               */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t AREGA_CLR;                     /**< Animation Register A                               */
  __IOM uint32_t AREGB_CLR;                     /**< Animation Register B                               */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Enable Register                          */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable                                   */
  __IOM uint32_t BIASCTRL_CLR;                  /**< Analog BIAS Control                                */
  __IOM uint32_t DISPCTRLX_CLR;                 /**< Display Control Extended                           */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SEGD0_CLR;                     /**< Segment Data Register 0                            */
  uint32_t       RESERVED15[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SEGD1_CLR;                     /**< Segment Data Register 1                            */
  uint32_t       RESERVED16[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SEGD2_CLR;                     /**< Segment Data Register 2                            */
  uint32_t       RESERVED17[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SEGD3_CLR;                     /**< Segment Data Register 3                            */
  uint32_t       RESERVED18[25U];               /**< Reserved for future use                            */
  __IOM uint32_t UPDATECTRL_CLR;                /**< Update Control                                     */
  uint32_t       RESERVED19[11U];               /**< Reserved for future use                            */
  __IOM uint32_t FRAMERATE_CLR;                 /**< Frame Rate                                         */
  uint32_t       RESERVED20[963U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_TGL;                        /**< Enable                                             */
  __IOM uint32_t SWRST_TGL;                     /**< Software Reset                                     */
  __IOM uint32_t CTRL_TGL;                      /**< Control Register                                   */
  __IOM uint32_t CMD_TGL;                       /**< Command register                                   */
  __IOM uint32_t DISPCTRL_TGL;                  /**< Display Control Register                           */
  __IOM uint32_t BACFG_TGL;                     /**< Blink and Animation Config Register                */
  __IOM uint32_t BACTRL_TGL;                    /**< Blink and Animation Control Register               */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t AREGA_TGL;                     /**< Animation Register A                               */
  __IOM uint32_t AREGB_TGL;                     /**< Animation Register B                               */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Enable Register                          */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable                                   */
  __IOM uint32_t BIASCTRL_TGL;                  /**< Analog BIAS Control                                */
  __IOM uint32_t DISPCTRLX_TGL;                 /**< Display Control Extended                           */
  uint32_t       RESERVED21[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SEGD0_TGL;                     /**< Segment Data Register 0                            */
  uint32_t       RESERVED22[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SEGD1_TGL;                     /**< Segment Data Register 1                            */
  uint32_t       RESERVED23[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SEGD2_TGL;                     /**< Segment Data Register 2                            */
  uint32_t       RESERVED24[1U];                /**< Reserved for future use                            */
  __IOM uint32_t SEGD3_TGL;                     /**< Segment Data Register 3                            */
  uint32_t       RESERVED25[25U];               /**< Reserved for future use                            */
  __IOM uint32_t UPDATECTRL_TGL;                /**< Update Control                                     */
  uint32_t       RESERVED26[11U];               /**< Reserved for future use                            */
  __IOM uint32_t FRAMERATE_TGL;                 /**< Frame Rate                                         */
} LCD_TypeDef;
/** @} End of group EFR32ZG23_LCD */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_LCD
 * @{
 * @defgroup EFR32ZG23_LCD_BitFields LCD Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for LCD IPVERSION */
#define _LCD_IPVERSION_RESETVALUE            0x00000001UL                               /**< Default value for LCD_IPVERSION             */
#define _LCD_IPVERSION_MASK                  0xFFFFFFFFUL                               /**< Mask for LCD_IPVERSION                      */
#define _LCD_IPVERSION_IPVERSION_SHIFT       0                                          /**< Shift value for LCD_IPVERSION               */
#define _LCD_IPVERSION_IPVERSION_MASK        0xFFFFFFFFUL                               /**< Bit mask for LCD_IPVERSION                  */
#define _LCD_IPVERSION_IPVERSION_DEFAULT     0x00000001UL                               /**< Mode DEFAULT for LCD_IPVERSION              */
#define LCD_IPVERSION_IPVERSION_DEFAULT      (_LCD_IPVERSION_IPVERSION_DEFAULT << 0)    /**< Shifted mode DEFAULT for LCD_IPVERSION      */

/* Bit fields for LCD EN */
#define _LCD_EN_RESETVALUE                   0x00000000UL                               /**< Default value for LCD_EN                    */
#define _LCD_EN_MASK                         0x00000003UL                               /**< Mask for LCD_EN                             */
#define LCD_EN_EN                            (0x1UL << 0)                               /**< Enable                                      */
#define _LCD_EN_EN_SHIFT                     0                                          /**< Shift value for LCD_EN                      */
#define _LCD_EN_EN_MASK                      0x1UL                                      /**< Bit mask for LCD_EN                         */
#define _LCD_EN_EN_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for LCD_EN                     */
#define _LCD_EN_EN_DISABLE                   0x00000000UL                               /**< Mode DISABLE for LCD_EN                     */
#define _LCD_EN_EN_ENABLE                    0x00000001UL                               /**< Mode ENABLE for LCD_EN                      */
#define LCD_EN_EN_DEFAULT                    (_LCD_EN_EN_DEFAULT << 0)                  /**< Shifted mode DEFAULT for LCD_EN             */
#define LCD_EN_EN_DISABLE                    (_LCD_EN_EN_DISABLE << 0)                  /**< Shifted mode DISABLE for LCD_EN             */
#define LCD_EN_EN_ENABLE                     (_LCD_EN_EN_ENABLE << 0)                   /**< Shifted mode ENABLE for LCD_EN              */
#define LCD_EN_DISABLING                     (0x1UL << 1)                               /**< Disablement busy status                     */
#define _LCD_EN_DISABLING_SHIFT              1                                          /**< Shift value for LCD_DISABLING               */
#define _LCD_EN_DISABLING_MASK               0x2UL                                      /**< Bit mask for LCD_DISABLING                  */
#define _LCD_EN_DISABLING_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LCD_EN                     */
#define LCD_EN_DISABLING_DEFAULT             (_LCD_EN_DISABLING_DEFAULT << 1)           /**< Shifted mode DEFAULT for LCD_EN             */

/* Bit fields for LCD SWRST */
#define _LCD_SWRST_RESETVALUE                0x00000000UL                               /**< Default value for LCD_SWRST                 */
#define _LCD_SWRST_MASK                      0x00000003UL                               /**< Mask for LCD_SWRST                          */
#define LCD_SWRST_SWRST                      (0x1UL << 0)                               /**< Software reset command                      */
#define _LCD_SWRST_SWRST_SHIFT               0                                          /**< Shift value for LCD_SWRST                   */
#define _LCD_SWRST_SWRST_MASK                0x1UL                                      /**< Bit mask for LCD_SWRST                      */
#define _LCD_SWRST_SWRST_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_SWRST                  */
#define LCD_SWRST_SWRST_DEFAULT              (_LCD_SWRST_SWRST_DEFAULT << 0)            /**< Shifted mode DEFAULT for LCD_SWRST          */
#define LCD_SWRST_RESETTING                  (0x1UL << 1)                               /**< Software reset busy status                  */
#define _LCD_SWRST_RESETTING_SHIFT           1                                          /**< Shift value for LCD_RESETTING               */
#define _LCD_SWRST_RESETTING_MASK            0x2UL                                      /**< Bit mask for LCD_RESETTING                  */
#define _LCD_SWRST_RESETTING_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for LCD_SWRST                  */
#define LCD_SWRST_RESETTING_DEFAULT          (_LCD_SWRST_RESETTING_DEFAULT << 1)        /**< Shifted mode DEFAULT for LCD_SWRST          */

/* Bit fields for LCD CTRL */
#define _LCD_CTRL_RESETVALUE                 0x00100000UL                               /**< Default value for LCD_CTRL                  */
#define _LCD_CTRL_MASK                       0x7F1D0006UL                               /**< Mask for LCD_CTRL                           */
#define _LCD_CTRL_UDCTRL_SHIFT               1                                          /**< Shift value for LCD_UDCTRL                  */
#define _LCD_CTRL_UDCTRL_MASK                0x6UL                                      /**< Bit mask for LCD_UDCTRL                     */
#define _LCD_CTRL_UDCTRL_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_CTRL                   */
#define _LCD_CTRL_UDCTRL_REGULAR             0x00000000UL                               /**< Mode REGULAR for LCD_CTRL                   */
#define _LCD_CTRL_UDCTRL_FRAMESTART          0x00000001UL                               /**< Mode FRAMESTART for LCD_CTRL                */
#define _LCD_CTRL_UDCTRL_FCEVENT             0x00000002UL                               /**< Mode FCEVENT for LCD_CTRL                   */
#define _LCD_CTRL_UDCTRL_DISPLAYEVENT        0x00000003UL                               /**< Mode DISPLAYEVENT for LCD_CTRL              */
#define LCD_CTRL_UDCTRL_DEFAULT              (_LCD_CTRL_UDCTRL_DEFAULT << 1)            /**< Shifted mode DEFAULT for LCD_CTRL           */
#define LCD_CTRL_UDCTRL_REGULAR              (_LCD_CTRL_UDCTRL_REGULAR << 1)            /**< Shifted mode REGULAR for LCD_CTRL           */
#define LCD_CTRL_UDCTRL_FRAMESTART           (_LCD_CTRL_UDCTRL_FRAMESTART << 1)         /**< Shifted mode FRAMESTART for LCD_CTRL        */
#define LCD_CTRL_UDCTRL_FCEVENT              (_LCD_CTRL_UDCTRL_FCEVENT << 1)            /**< Shifted mode FCEVENT for LCD_CTRL           */
#define LCD_CTRL_UDCTRL_DISPLAYEVENT         (_LCD_CTRL_UDCTRL_DISPLAYEVENT << 1)       /**< Shifted mode DISPLAYEVENT for LCD_CTRL      */
#define LCD_CTRL_DSC                         (0x1UL << 16)                              /**< Direct Segment Control                      */
#define _LCD_CTRL_DSC_SHIFT                  16                                         /**< Shift value for LCD_DSC                     */
#define _LCD_CTRL_DSC_MASK                   0x10000UL                                  /**< Bit mask for LCD_DSC                        */
#define _LCD_CTRL_DSC_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for LCD_CTRL                   */
#define _LCD_CTRL_DSC_DISABLE                0x00000000UL                               /**< Mode DISABLE for LCD_CTRL                   */
#define _LCD_CTRL_DSC_ENABLE                 0x00000001UL                               /**< Mode ENABLE for LCD_CTRL                    */
#define LCD_CTRL_DSC_DEFAULT                 (_LCD_CTRL_DSC_DEFAULT << 16)              /**< Shifted mode DEFAULT for LCD_CTRL           */
#define LCD_CTRL_DSC_DISABLE                 (_LCD_CTRL_DSC_DISABLE << 16)              /**< Shifted mode DISABLE for LCD_CTRL           */
#define LCD_CTRL_DSC_ENABLE                  (_LCD_CTRL_DSC_ENABLE << 16)               /**< Shifted mode ENABLE for LCD_CTRL            */
#define _LCD_CTRL_WARMUPDLY_SHIFT            18                                         /**< Shift value for LCD_WARMUPDLY               */
#define _LCD_CTRL_WARMUPDLY_MASK             0x1C0000UL                                 /**< Bit mask for LCD_WARMUPDLY                  */
#define _LCD_CTRL_WARMUPDLY_DEFAULT          0x00000004UL                               /**< Mode DEFAULT for LCD_CTRL                   */
#define _LCD_CTRL_WARMUPDLY_WARMUP1          0x00000000UL                               /**< Mode WARMUP1 for LCD_CTRL                   */
#define _LCD_CTRL_WARMUPDLY_WARMUP31         0x00000001UL                               /**< Mode WARMUP31 for LCD_CTRL                  */
#define _LCD_CTRL_WARMUPDLY_WARMUP63         0x00000002UL                               /**< Mode WARMUP63 for LCD_CTRL                  */
#define _LCD_CTRL_WARMUPDLY_WARMUP125        0x00000003UL                               /**< Mode WARMUP125 for LCD_CTRL                 */
#define _LCD_CTRL_WARMUPDLY_WARMUP250        0x00000004UL                               /**< Mode WARMUP250 for LCD_CTRL                 */
#define _LCD_CTRL_WARMUPDLY_WARMUP500        0x00000005UL                               /**< Mode WARMUP500 for LCD_CTRL                 */
#define _LCD_CTRL_WARMUPDLY_WARMUP1000       0x00000006UL                               /**< Mode WARMUP1000 for LCD_CTRL                */
#define _LCD_CTRL_WARMUPDLY_WARMUP2000       0x00000007UL                               /**< Mode WARMUP2000 for LCD_CTRL                */
#define LCD_CTRL_WARMUPDLY_DEFAULT           (_LCD_CTRL_WARMUPDLY_DEFAULT << 18)        /**< Shifted mode DEFAULT for LCD_CTRL           */
#define LCD_CTRL_WARMUPDLY_WARMUP1           (_LCD_CTRL_WARMUPDLY_WARMUP1 << 18)        /**< Shifted mode WARMUP1 for LCD_CTRL           */
#define LCD_CTRL_WARMUPDLY_WARMUP31          (_LCD_CTRL_WARMUPDLY_WARMUP31 << 18)       /**< Shifted mode WARMUP31 for LCD_CTRL          */
#define LCD_CTRL_WARMUPDLY_WARMUP63          (_LCD_CTRL_WARMUPDLY_WARMUP63 << 18)       /**< Shifted mode WARMUP63 for LCD_CTRL          */
#define LCD_CTRL_WARMUPDLY_WARMUP125         (_LCD_CTRL_WARMUPDLY_WARMUP125 << 18)      /**< Shifted mode WARMUP125 for LCD_CTRL         */
#define LCD_CTRL_WARMUPDLY_WARMUP250         (_LCD_CTRL_WARMUPDLY_WARMUP250 << 18)      /**< Shifted mode WARMUP250 for LCD_CTRL         */
#define LCD_CTRL_WARMUPDLY_WARMUP500         (_LCD_CTRL_WARMUPDLY_WARMUP500 << 18)      /**< Shifted mode WARMUP500 for LCD_CTRL         */
#define LCD_CTRL_WARMUPDLY_WARMUP1000        (_LCD_CTRL_WARMUPDLY_WARMUP1000 << 18)     /**< Shifted mode WARMUP1000 for LCD_CTRL        */
#define LCD_CTRL_WARMUPDLY_WARMUP2000        (_LCD_CTRL_WARMUPDLY_WARMUP2000 << 18)     /**< Shifted mode WARMUP2000 for LCD_CTRL        */
#define _LCD_CTRL_PRESCALE_SHIFT             24                                         /**< Shift value for LCD_PRESCALE                */
#define _LCD_CTRL_PRESCALE_MASK              0x7F000000UL                               /**< Bit mask for LCD_PRESCALE                   */
#define _LCD_CTRL_PRESCALE_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LCD_CTRL                   */
#define LCD_CTRL_PRESCALE_DEFAULT            (_LCD_CTRL_PRESCALE_DEFAULT << 24)         /**< Shifted mode DEFAULT for LCD_CTRL           */

/* Bit fields for LCD CMD */
#define _LCD_CMD_RESETVALUE                  0x00000000UL                               /**< Default value for LCD_CMD                   */
#define _LCD_CMD_MASK                        0x00000003UL                               /**< Mask for LCD_CMD                            */
#define LCD_CMD_LOAD                         (0x1UL << 0)                               /**< Load command                                */
#define _LCD_CMD_LOAD_SHIFT                  0                                          /**< Shift value for LCD_LOAD                    */
#define _LCD_CMD_LOAD_MASK                   0x1UL                                      /**< Bit mask for LCD_LOAD                       */
#define _LCD_CMD_LOAD_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for LCD_CMD                    */
#define LCD_CMD_LOAD_DEFAULT                 (_LCD_CMD_LOAD_DEFAULT << 0)               /**< Shifted mode DEFAULT for LCD_CMD            */
#define LCD_CMD_CLEAR                        (0x1UL << 1)                               /**< Clear command                               */
#define _LCD_CMD_CLEAR_SHIFT                 1                                          /**< Shift value for LCD_CLEAR                   */
#define _LCD_CMD_CLEAR_MASK                  0x2UL                                      /**< Bit mask for LCD_CLEAR                      */
#define _LCD_CMD_CLEAR_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for LCD_CMD                    */
#define LCD_CMD_CLEAR_DEFAULT                (_LCD_CMD_CLEAR_DEFAULT << 1)              /**< Shifted mode DEFAULT for LCD_CMD            */

/* Bit fields for LCD DISPCTRL */
#define _LCD_DISPCTRL_RESETVALUE             0x00100000UL                               /**< Default value for LCD_DISPCTRL              */
#define _LCD_DISPCTRL_MASK                   0x03700017UL                               /**< Mask for LCD_DISPCTRL                       */
#define _LCD_DISPCTRL_MUX_SHIFT              0                                          /**< Shift value for LCD_MUX                     */
#define _LCD_DISPCTRL_MUX_MASK               0x7UL                                      /**< Bit mask for LCD_MUX                        */
#define _LCD_DISPCTRL_MUX_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LCD_DISPCTRL               */
#define _LCD_DISPCTRL_MUX_STATIC             0x00000000UL                               /**< Mode STATIC for LCD_DISPCTRL                */
#define _LCD_DISPCTRL_MUX_DUPLEX             0x00000001UL                               /**< Mode DUPLEX for LCD_DISPCTRL                */
#define _LCD_DISPCTRL_MUX_TRIPLEX            0x00000002UL                               /**< Mode TRIPLEX for LCD_DISPCTRL               */
#define _LCD_DISPCTRL_MUX_QUADRUPLEX         0x00000003UL                               /**< Mode QUADRUPLEX for LCD_DISPCTRL            */
#define LCD_DISPCTRL_MUX_DEFAULT             (_LCD_DISPCTRL_MUX_DEFAULT << 0)           /**< Shifted mode DEFAULT for LCD_DISPCTRL       */
#define LCD_DISPCTRL_MUX_STATIC              (_LCD_DISPCTRL_MUX_STATIC << 0)            /**< Shifted mode STATIC for LCD_DISPCTRL        */
#define LCD_DISPCTRL_MUX_DUPLEX              (_LCD_DISPCTRL_MUX_DUPLEX << 0)            /**< Shifted mode DUPLEX for LCD_DISPCTRL        */
#define LCD_DISPCTRL_MUX_TRIPLEX             (_LCD_DISPCTRL_MUX_TRIPLEX << 0)           /**< Shifted mode TRIPLEX for LCD_DISPCTRL       */
#define LCD_DISPCTRL_MUX_QUADRUPLEX          (_LCD_DISPCTRL_MUX_QUADRUPLEX << 0)        /**< Shifted mode QUADRUPLEX for LCD_DISPCTRL    */
#define LCD_DISPCTRL_WAVE                    (0x1UL << 4)                               /**< Waveform Selection                          */
#define _LCD_DISPCTRL_WAVE_SHIFT             4                                          /**< Shift value for LCD_WAVE                    */
#define _LCD_DISPCTRL_WAVE_MASK              0x10UL                                     /**< Bit mask for LCD_WAVE                       */
#define _LCD_DISPCTRL_WAVE_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LCD_DISPCTRL               */
#define _LCD_DISPCTRL_WAVE_TYPEB             0x00000000UL                               /**< Mode TYPEB for LCD_DISPCTRL                 */
#define _LCD_DISPCTRL_WAVE_TYPEA             0x00000001UL                               /**< Mode TYPEA for LCD_DISPCTRL                 */
#define LCD_DISPCTRL_WAVE_DEFAULT            (_LCD_DISPCTRL_WAVE_DEFAULT << 4)          /**< Shifted mode DEFAULT for LCD_DISPCTRL       */
#define LCD_DISPCTRL_WAVE_TYPEB              (_LCD_DISPCTRL_WAVE_TYPEB << 4)            /**< Shifted mode TYPEB for LCD_DISPCTRL         */
#define LCD_DISPCTRL_WAVE_TYPEA              (_LCD_DISPCTRL_WAVE_TYPEA << 4)            /**< Shifted mode TYPEA for LCD_DISPCTRL         */
#define _LCD_DISPCTRL_CHGRDST_SHIFT          20                                         /**< Shift value for LCD_CHGRDST                 */
#define _LCD_DISPCTRL_CHGRDST_MASK           0x700000UL                                 /**< Bit mask for LCD_CHGRDST                    */
#define _LCD_DISPCTRL_CHGRDST_DEFAULT        0x00000001UL                               /**< Mode DEFAULT for LCD_DISPCTRL               */
#define _LCD_DISPCTRL_CHGRDST_DISABLE        0x00000000UL                               /**< Mode DISABLE for LCD_DISPCTRL               */
#define _LCD_DISPCTRL_CHGRDST_ONE            0x00000001UL                               /**< Mode ONE for LCD_DISPCTRL                   */
#define _LCD_DISPCTRL_CHGRDST_TWO            0x00000002UL                               /**< Mode TWO for LCD_DISPCTRL                   */
#define _LCD_DISPCTRL_CHGRDST_THREE          0x00000003UL                               /**< Mode THREE for LCD_DISPCTRL                 */
#define _LCD_DISPCTRL_CHGRDST_FOUR           0x00000004UL                               /**< Mode FOUR for LCD_DISPCTRL                  */
#define LCD_DISPCTRL_CHGRDST_DEFAULT         (_LCD_DISPCTRL_CHGRDST_DEFAULT << 20)      /**< Shifted mode DEFAULT for LCD_DISPCTRL       */
#define LCD_DISPCTRL_CHGRDST_DISABLE         (_LCD_DISPCTRL_CHGRDST_DISABLE << 20)      /**< Shifted mode DISABLE for LCD_DISPCTRL       */
#define LCD_DISPCTRL_CHGRDST_ONE             (_LCD_DISPCTRL_CHGRDST_ONE << 20)          /**< Shifted mode ONE for LCD_DISPCTRL           */
#define LCD_DISPCTRL_CHGRDST_TWO             (_LCD_DISPCTRL_CHGRDST_TWO << 20)          /**< Shifted mode TWO for LCD_DISPCTRL           */
#define LCD_DISPCTRL_CHGRDST_THREE           (_LCD_DISPCTRL_CHGRDST_THREE << 20)        /**< Shifted mode THREE for LCD_DISPCTRL         */
#define LCD_DISPCTRL_CHGRDST_FOUR            (_LCD_DISPCTRL_CHGRDST_FOUR << 20)         /**< Shifted mode FOUR for LCD_DISPCTRL          */
#define _LCD_DISPCTRL_BIAS_SHIFT             24                                         /**< Shift value for LCD_BIAS                    */
#define _LCD_DISPCTRL_BIAS_MASK              0x3000000UL                                /**< Bit mask for LCD_BIAS                       */
#define _LCD_DISPCTRL_BIAS_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LCD_DISPCTRL               */
#define _LCD_DISPCTRL_BIAS_STATIC            0x00000000UL                               /**< Mode STATIC for LCD_DISPCTRL                */
#define _LCD_DISPCTRL_BIAS_ONEHALF           0x00000001UL                               /**< Mode ONEHALF for LCD_DISPCTRL               */
#define _LCD_DISPCTRL_BIAS_ONETHIRD          0x00000002UL                               /**< Mode ONETHIRD for LCD_DISPCTRL              */
#define _LCD_DISPCTRL_BIAS_ONEFOURTH         0x00000003UL                               /**< Mode ONEFOURTH for LCD_DISPCTRL             */
#define LCD_DISPCTRL_BIAS_DEFAULT            (_LCD_DISPCTRL_BIAS_DEFAULT << 24)         /**< Shifted mode DEFAULT for LCD_DISPCTRL       */
#define LCD_DISPCTRL_BIAS_STATIC             (_LCD_DISPCTRL_BIAS_STATIC << 24)          /**< Shifted mode STATIC for LCD_DISPCTRL        */
#define LCD_DISPCTRL_BIAS_ONEHALF            (_LCD_DISPCTRL_BIAS_ONEHALF << 24)         /**< Shifted mode ONEHALF for LCD_DISPCTRL       */
#define LCD_DISPCTRL_BIAS_ONETHIRD           (_LCD_DISPCTRL_BIAS_ONETHIRD << 24)        /**< Shifted mode ONETHIRD for LCD_DISPCTRL      */
#define LCD_DISPCTRL_BIAS_ONEFOURTH          (_LCD_DISPCTRL_BIAS_ONEFOURTH << 24)       /**< Shifted mode ONEFOURTH for LCD_DISPCTRL     */

/* Bit fields for LCD BACFG */
#define _LCD_BACFG_RESETVALUE                0x00000007UL                               /**< Default value for LCD_BACFG                 */
#define _LCD_BACFG_MASK                      0x00FF0007UL                               /**< Mask for LCD_BACFG                          */
#define _LCD_BACFG_ASTATETOP_SHIFT           0                                          /**< Shift value for LCD_ASTATETOP               */
#define _LCD_BACFG_ASTATETOP_MASK            0x7UL                                      /**< Bit mask for LCD_ASTATETOP                  */
#define _LCD_BACFG_ASTATETOP_DEFAULT         0x00000007UL                               /**< Mode DEFAULT for LCD_BACFG                  */
#define LCD_BACFG_ASTATETOP_DEFAULT          (_LCD_BACFG_ASTATETOP_DEFAULT << 0)        /**< Shifted mode DEFAULT for LCD_BACFG          */
#define _LCD_BACFG_FCPRESC_SHIFT             16                                         /**< Shift value for LCD_FCPRESC                 */
#define _LCD_BACFG_FCPRESC_MASK              0x30000UL                                  /**< Bit mask for LCD_FCPRESC                    */
#define _LCD_BACFG_FCPRESC_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LCD_BACFG                  */
#define _LCD_BACFG_FCPRESC_DIV1              0x00000000UL                               /**< Mode DIV1 for LCD_BACFG                     */
#define _LCD_BACFG_FCPRESC_DIV2              0x00000001UL                               /**< Mode DIV2 for LCD_BACFG                     */
#define _LCD_BACFG_FCPRESC_DIV4              0x00000002UL                               /**< Mode DIV4 for LCD_BACFG                     */
#define _LCD_BACFG_FCPRESC_DIV8              0x00000003UL                               /**< Mode DIV8 for LCD_BACFG                     */
#define LCD_BACFG_FCPRESC_DEFAULT            (_LCD_BACFG_FCPRESC_DEFAULT << 16)         /**< Shifted mode DEFAULT for LCD_BACFG          */
#define LCD_BACFG_FCPRESC_DIV1               (_LCD_BACFG_FCPRESC_DIV1 << 16)            /**< Shifted mode DIV1 for LCD_BACFG             */
#define LCD_BACFG_FCPRESC_DIV2               (_LCD_BACFG_FCPRESC_DIV2 << 16)            /**< Shifted mode DIV2 for LCD_BACFG             */
#define LCD_BACFG_FCPRESC_DIV4               (_LCD_BACFG_FCPRESC_DIV4 << 16)            /**< Shifted mode DIV4 for LCD_BACFG             */
#define LCD_BACFG_FCPRESC_DIV8               (_LCD_BACFG_FCPRESC_DIV8 << 16)            /**< Shifted mode DIV8 for LCD_BACFG             */
#define _LCD_BACFG_FCTOP_SHIFT               18                                         /**< Shift value for LCD_FCTOP                   */
#define _LCD_BACFG_FCTOP_MASK                0xFC0000UL                                 /**< Bit mask for LCD_FCTOP                      */
#define _LCD_BACFG_FCTOP_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_BACFG                  */
#define LCD_BACFG_FCTOP_DEFAULT              (_LCD_BACFG_FCTOP_DEFAULT << 18)           /**< Shifted mode DEFAULT for LCD_BACFG          */

/* Bit fields for LCD BACTRL */
#define _LCD_BACTRL_RESETVALUE               0x00000000UL                               /**< Default value for LCD_BACTRL                */
#define _LCD_BACTRL_MASK                     0x100003FFUL                               /**< Mask for LCD_BACTRL                         */
#define LCD_BACTRL_BLINKEN                   (0x1UL << 0)                               /**< Blink Enable                                */
#define _LCD_BACTRL_BLINKEN_SHIFT            0                                          /**< Shift value for LCD_BLINKEN                 */
#define _LCD_BACTRL_BLINKEN_MASK             0x1UL                                      /**< Bit mask for LCD_BLINKEN                    */
#define _LCD_BACTRL_BLINKEN_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for LCD_BACTRL                 */
#define LCD_BACTRL_BLINKEN_DEFAULT           (_LCD_BACTRL_BLINKEN_DEFAULT << 0)         /**< Shifted mode DEFAULT for LCD_BACTRL         */
#define LCD_BACTRL_BLANK                     (0x1UL << 1)                               /**< Blank Display                               */
#define _LCD_BACTRL_BLANK_SHIFT              1                                          /**< Shift value for LCD_BLANK                   */
#define _LCD_BACTRL_BLANK_MASK               0x2UL                                      /**< Bit mask for LCD_BLANK                      */
#define _LCD_BACTRL_BLANK_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LCD_BACTRL                 */
#define _LCD_BACTRL_BLANK_DISABLE            0x00000000UL                               /**< Mode DISABLE for LCD_BACTRL                 */
#define _LCD_BACTRL_BLANK_ENABLE             0x00000001UL                               /**< Mode ENABLE for LCD_BACTRL                  */
#define LCD_BACTRL_BLANK_DEFAULT             (_LCD_BACTRL_BLANK_DEFAULT << 1)           /**< Shifted mode DEFAULT for LCD_BACTRL         */
#define LCD_BACTRL_BLANK_DISABLE             (_LCD_BACTRL_BLANK_DISABLE << 1)           /**< Shifted mode DISABLE for LCD_BACTRL         */
#define LCD_BACTRL_BLANK_ENABLE              (_LCD_BACTRL_BLANK_ENABLE << 1)            /**< Shifted mode ENABLE for LCD_BACTRL          */
#define LCD_BACTRL_AEN                       (0x1UL << 2)                               /**< Animation Enable                            */
#define _LCD_BACTRL_AEN_SHIFT                2                                          /**< Shift value for LCD_AEN                     */
#define _LCD_BACTRL_AEN_MASK                 0x4UL                                      /**< Bit mask for LCD_AEN                        */
#define _LCD_BACTRL_AEN_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for LCD_BACTRL                 */
#define LCD_BACTRL_AEN_DEFAULT               (_LCD_BACTRL_AEN_DEFAULT << 2)             /**< Shifted mode DEFAULT for LCD_BACTRL         */
#define _LCD_BACTRL_AREGASC_SHIFT            3                                          /**< Shift value for LCD_AREGASC                 */
#define _LCD_BACTRL_AREGASC_MASK             0x18UL                                     /**< Bit mask for LCD_AREGASC                    */
#define _LCD_BACTRL_AREGASC_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for LCD_BACTRL                 */
#define _LCD_BACTRL_AREGASC_NOSHIFT          0x00000000UL                               /**< Mode NOSHIFT for LCD_BACTRL                 */
#define _LCD_BACTRL_AREGASC_SHIFTLEFT        0x00000001UL                               /**< Mode SHIFTLEFT for LCD_BACTRL               */
#define _LCD_BACTRL_AREGASC_SHIFTRIGHT       0x00000002UL                               /**< Mode SHIFTRIGHT for LCD_BACTRL              */
#define LCD_BACTRL_AREGASC_DEFAULT           (_LCD_BACTRL_AREGASC_DEFAULT << 3)         /**< Shifted mode DEFAULT for LCD_BACTRL         */
#define LCD_BACTRL_AREGASC_NOSHIFT           (_LCD_BACTRL_AREGASC_NOSHIFT << 3)         /**< Shifted mode NOSHIFT for LCD_BACTRL         */
#define LCD_BACTRL_AREGASC_SHIFTLEFT         (_LCD_BACTRL_AREGASC_SHIFTLEFT << 3)       /**< Shifted mode SHIFTLEFT for LCD_BACTRL       */
#define LCD_BACTRL_AREGASC_SHIFTRIGHT        (_LCD_BACTRL_AREGASC_SHIFTRIGHT << 3)      /**< Shifted mode SHIFTRIGHT for LCD_BACTRL      */
#define _LCD_BACTRL_AREGBSC_SHIFT            5                                          /**< Shift value for LCD_AREGBSC                 */
#define _LCD_BACTRL_AREGBSC_MASK             0x60UL                                     /**< Bit mask for LCD_AREGBSC                    */
#define _LCD_BACTRL_AREGBSC_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for LCD_BACTRL                 */
#define _LCD_BACTRL_AREGBSC_NOSHIFT          0x00000000UL                               /**< Mode NOSHIFT for LCD_BACTRL                 */
#define _LCD_BACTRL_AREGBSC_SHIFTLEFT        0x00000001UL                               /**< Mode SHIFTLEFT for LCD_BACTRL               */
#define _LCD_BACTRL_AREGBSC_SHIFTRIGHT       0x00000002UL                               /**< Mode SHIFTRIGHT for LCD_BACTRL              */
#define LCD_BACTRL_AREGBSC_DEFAULT           (_LCD_BACTRL_AREGBSC_DEFAULT << 5)         /**< Shifted mode DEFAULT for LCD_BACTRL         */
#define LCD_BACTRL_AREGBSC_NOSHIFT           (_LCD_BACTRL_AREGBSC_NOSHIFT << 5)         /**< Shifted mode NOSHIFT for LCD_BACTRL         */
#define LCD_BACTRL_AREGBSC_SHIFTLEFT         (_LCD_BACTRL_AREGBSC_SHIFTLEFT << 5)       /**< Shifted mode SHIFTLEFT for LCD_BACTRL       */
#define LCD_BACTRL_AREGBSC_SHIFTRIGHT        (_LCD_BACTRL_AREGBSC_SHIFTRIGHT << 5)      /**< Shifted mode SHIFTRIGHT for LCD_BACTRL      */
#define LCD_BACTRL_ALOGSEL                   (0x1UL << 7)                               /**< Animate Logic Function Select               */
#define _LCD_BACTRL_ALOGSEL_SHIFT            7                                          /**< Shift value for LCD_ALOGSEL                 */
#define _LCD_BACTRL_ALOGSEL_MASK             0x80UL                                     /**< Bit mask for LCD_ALOGSEL                    */
#define _LCD_BACTRL_ALOGSEL_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for LCD_BACTRL                 */
#define _LCD_BACTRL_ALOGSEL_AND              0x00000000UL                               /**< Mode AND for LCD_BACTRL                     */
#define _LCD_BACTRL_ALOGSEL_OR               0x00000001UL                               /**< Mode OR for LCD_BACTRL                      */
#define LCD_BACTRL_ALOGSEL_DEFAULT           (_LCD_BACTRL_ALOGSEL_DEFAULT << 7)         /**< Shifted mode DEFAULT for LCD_BACTRL         */
#define LCD_BACTRL_ALOGSEL_AND               (_LCD_BACTRL_ALOGSEL_AND << 7)             /**< Shifted mode AND for LCD_BACTRL             */
#define LCD_BACTRL_ALOGSEL_OR                (_LCD_BACTRL_ALOGSEL_OR << 7)              /**< Shifted mode OR for LCD_BACTRL              */
#define LCD_BACTRL_FCEN                      (0x1UL << 8)                               /**< Frame Counter Enable                        */
#define _LCD_BACTRL_FCEN_SHIFT               8                                          /**< Shift value for LCD_FCEN                    */
#define _LCD_BACTRL_FCEN_MASK                0x100UL                                    /**< Bit mask for LCD_FCEN                       */
#define _LCD_BACTRL_FCEN_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_BACTRL                 */
#define LCD_BACTRL_FCEN_DEFAULT              (_LCD_BACTRL_FCEN_DEFAULT << 8)            /**< Shifted mode DEFAULT for LCD_BACTRL         */
#define LCD_BACTRL_DISPLAYCNTEN              (0x1UL << 9)                               /**< Display Counter Enable                      */
#define _LCD_BACTRL_DISPLAYCNTEN_SHIFT       9                                          /**< Shift value for LCD_DISPLAYCNTEN            */
#define _LCD_BACTRL_DISPLAYCNTEN_MASK        0x200UL                                    /**< Bit mask for LCD_DISPLAYCNTEN               */
#define _LCD_BACTRL_DISPLAYCNTEN_DEFAULT     0x00000000UL                               /**< Mode DEFAULT for LCD_BACTRL                 */
#define _LCD_BACTRL_DISPLAYCNTEN_DISABLE     0x00000000UL                               /**< Mode DISABLE for LCD_BACTRL                 */
#define _LCD_BACTRL_DISPLAYCNTEN_ENABLE      0x00000001UL                               /**< Mode ENABLE for LCD_BACTRL                  */
#define LCD_BACTRL_DISPLAYCNTEN_DEFAULT      (_LCD_BACTRL_DISPLAYCNTEN_DEFAULT << 9)    /**< Shifted mode DEFAULT for LCD_BACTRL         */
#define LCD_BACTRL_DISPLAYCNTEN_DISABLE      (_LCD_BACTRL_DISPLAYCNTEN_DISABLE << 9)    /**< Shifted mode DISABLE for LCD_BACTRL         */
#define LCD_BACTRL_DISPLAYCNTEN_ENABLE       (_LCD_BACTRL_DISPLAYCNTEN_ENABLE << 9)     /**< Shifted mode ENABLE for LCD_BACTRL          */
#define LCD_BACTRL_ALOC                      (0x1UL << 28)                              /**< Animation Location                          */
#define _LCD_BACTRL_ALOC_SHIFT               28                                         /**< Shift value for LCD_ALOC                    */
#define _LCD_BACTRL_ALOC_MASK                0x10000000UL                               /**< Bit mask for LCD_ALOC                       */
#define _LCD_BACTRL_ALOC_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_BACTRL                 */
#define _LCD_BACTRL_ALOC_SEG0TO7             0x00000000UL                               /**< Mode SEG0TO7 for LCD_BACTRL                 */
#define _LCD_BACTRL_ALOC_SEG8TO15            0x00000001UL                               /**< Mode SEG8TO15 for LCD_BACTRL                */
#define LCD_BACTRL_ALOC_DEFAULT              (_LCD_BACTRL_ALOC_DEFAULT << 28)           /**< Shifted mode DEFAULT for LCD_BACTRL         */
#define LCD_BACTRL_ALOC_SEG0TO7              (_LCD_BACTRL_ALOC_SEG0TO7 << 28)           /**< Shifted mode SEG0TO7 for LCD_BACTRL         */
#define LCD_BACTRL_ALOC_SEG8TO15             (_LCD_BACTRL_ALOC_SEG8TO15 << 28)          /**< Shifted mode SEG8TO15 for LCD_BACTRL        */

/* Bit fields for LCD STATUS */
#define _LCD_STATUS_RESETVALUE               0x00000000UL                               /**< Default value for LCD_STATUS                */
#define _LCD_STATUS_MASK                     0x0000090FUL                               /**< Mask for LCD_STATUS                         */
#define _LCD_STATUS_ASTATE_SHIFT             0                                          /**< Shift value for LCD_ASTATE                  */
#define _LCD_STATUS_ASTATE_MASK              0xFUL                                      /**< Bit mask for LCD_ASTATE                     */
#define _LCD_STATUS_ASTATE_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LCD_STATUS                 */
#define LCD_STATUS_ASTATE_DEFAULT            (_LCD_STATUS_ASTATE_DEFAULT << 0)          /**< Shifted mode DEFAULT for LCD_STATUS         */
#define LCD_STATUS_BLINK                     (0x1UL << 8)                               /**< Blink State                                 */
#define _LCD_STATUS_BLINK_SHIFT              8                                          /**< Shift value for LCD_BLINK                   */
#define _LCD_STATUS_BLINK_MASK               0x100UL                                    /**< Bit mask for LCD_BLINK                      */
#define _LCD_STATUS_BLINK_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for LCD_STATUS                 */
#define LCD_STATUS_BLINK_DEFAULT             (_LCD_STATUS_BLINK_DEFAULT << 8)           /**< Shifted mode DEFAULT for LCD_STATUS         */
#define LCD_STATUS_LOADBUSY                  (0x1UL << 11)                              /**< Load Synchronization is busy                */
#define _LCD_STATUS_LOADBUSY_SHIFT           11                                         /**< Shift value for LCD_LOADBUSY                */
#define _LCD_STATUS_LOADBUSY_MASK            0x800UL                                    /**< Bit mask for LCD_LOADBUSY                   */
#define _LCD_STATUS_LOADBUSY_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for LCD_STATUS                 */
#define LCD_STATUS_LOADBUSY_DEFAULT          (_LCD_STATUS_LOADBUSY_DEFAULT << 11)       /**< Shifted mode DEFAULT for LCD_STATUS         */

/* Bit fields for LCD AREGA */
#define _LCD_AREGA_RESETVALUE                0x00000000UL                               /**< Default value for LCD_AREGA                 */
#define _LCD_AREGA_MASK                      0x000000FFUL                               /**< Mask for LCD_AREGA                          */
#define _LCD_AREGA_AREGA_SHIFT               0                                          /**< Shift value for LCD_AREGA                   */
#define _LCD_AREGA_AREGA_MASK                0xFFUL                                     /**< Bit mask for LCD_AREGA                      */
#define _LCD_AREGA_AREGA_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_AREGA                  */
#define LCD_AREGA_AREGA_DEFAULT              (_LCD_AREGA_AREGA_DEFAULT << 0)            /**< Shifted mode DEFAULT for LCD_AREGA          */

/* Bit fields for LCD AREGB */
#define _LCD_AREGB_RESETVALUE                0x00000000UL                               /**< Default value for LCD_AREGB                 */
#define _LCD_AREGB_MASK                      0x000000FFUL                               /**< Mask for LCD_AREGB                          */
#define _LCD_AREGB_AREGB_SHIFT               0                                          /**< Shift value for LCD_AREGB                   */
#define _LCD_AREGB_AREGB_MASK                0xFFUL                                     /**< Bit mask for LCD_AREGB                      */
#define _LCD_AREGB_AREGB_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_AREGB                  */
#define LCD_AREGB_AREGB_DEFAULT              (_LCD_AREGB_AREGB_DEFAULT << 0)            /**< Shifted mode DEFAULT for LCD_AREGB          */

/* Bit fields for LCD IF */
#define _LCD_IF_RESETVALUE                   0x00000000UL                               /**< Default value for LCD_IF                    */
#define _LCD_IF_MASK                         0x00000007UL                               /**< Mask for LCD_IF                             */
#define LCD_IF_FC                            (0x1UL << 0)                               /**< Frame Counter                               */
#define _LCD_IF_FC_SHIFT                     0                                          /**< Shift value for LCD_FC                      */
#define _LCD_IF_FC_MASK                      0x1UL                                      /**< Bit mask for LCD_FC                         */
#define _LCD_IF_FC_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for LCD_IF                     */
#define LCD_IF_FC_DEFAULT                    (_LCD_IF_FC_DEFAULT << 0)                  /**< Shifted mode DEFAULT for LCD_IF             */
#define LCD_IF_DISPLAY                       (0x1UL << 1)                               /**< Display Update Event                        */
#define _LCD_IF_DISPLAY_SHIFT                1                                          /**< Shift value for LCD_DISPLAY                 */
#define _LCD_IF_DISPLAY_MASK                 0x2UL                                      /**< Bit mask for LCD_DISPLAY                    */
#define _LCD_IF_DISPLAY_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for LCD_IF                     */
#define LCD_IF_DISPLAY_DEFAULT               (_LCD_IF_DISPLAY_DEFAULT << 1)             /**< Shifted mode DEFAULT for LCD_IF             */
#define LCD_IF_SYNCBUSYDONE                  (0x1UL << 2)                               /**< Synchronization is Done                     */
#define _LCD_IF_SYNCBUSYDONE_SHIFT           2                                          /**< Shift value for LCD_SYNCBUSYDONE            */
#define _LCD_IF_SYNCBUSYDONE_MASK            0x4UL                                      /**< Bit mask for LCD_SYNCBUSYDONE               */
#define _LCD_IF_SYNCBUSYDONE_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for LCD_IF                     */
#define LCD_IF_SYNCBUSYDONE_DEFAULT          (_LCD_IF_SYNCBUSYDONE_DEFAULT << 2)        /**< Shifted mode DEFAULT for LCD_IF             */

/* Bit fields for LCD IEN */
#define _LCD_IEN_RESETVALUE                  0x00000000UL                               /**< Default value for LCD_IEN                   */
#define _LCD_IEN_MASK                        0x00000007UL                               /**< Mask for LCD_IEN                            */
#define LCD_IEN_FC                           (0x1UL << 0)                               /**< Frame Counter                               */
#define _LCD_IEN_FC_SHIFT                    0                                          /**< Shift value for LCD_FC                      */
#define _LCD_IEN_FC_MASK                     0x1UL                                      /**< Bit mask for LCD_FC                         */
#define _LCD_IEN_FC_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for LCD_IEN                    */
#define LCD_IEN_FC_DEFAULT                   (_LCD_IEN_FC_DEFAULT << 0)                 /**< Shifted mode DEFAULT for LCD_IEN            */
#define LCD_IEN_DISPLAY                      (0x1UL << 1)                               /**< Display Update Event                        */
#define _LCD_IEN_DISPLAY_SHIFT               1                                          /**< Shift value for LCD_DISPLAY                 */
#define _LCD_IEN_DISPLAY_MASK                0x2UL                                      /**< Bit mask for LCD_DISPLAY                    */
#define _LCD_IEN_DISPLAY_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_IEN                    */
#define LCD_IEN_DISPLAY_DEFAULT              (_LCD_IEN_DISPLAY_DEFAULT << 1)            /**< Shifted mode DEFAULT for LCD_IEN            */
#define LCD_IEN_SYNCBUSYDONE                 (0x1UL << 2)                               /**< Sync Busy Done                              */
#define _LCD_IEN_SYNCBUSYDONE_SHIFT          2                                          /**< Shift value for LCD_SYNCBUSYDONE            */
#define _LCD_IEN_SYNCBUSYDONE_MASK           0x4UL                                      /**< Bit mask for LCD_SYNCBUSYDONE               */
#define _LCD_IEN_SYNCBUSYDONE_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for LCD_IEN                    */
#define LCD_IEN_SYNCBUSYDONE_DEFAULT         (_LCD_IEN_SYNCBUSYDONE_DEFAULT << 2)       /**< Shifted mode DEFAULT for LCD_IEN            */

/* Bit fields for LCD BIASCTRL */
#define _LCD_BIASCTRL_RESETVALUE             0x001F0000UL                               /**< Default value for LCD_BIASCTRL              */
#define _LCD_BIASCTRL_MASK                   0xC45F137FUL                               /**< Mask for LCD_BIASCTRL                       */
#define _LCD_BIASCTRL_RESISTOR_SHIFT         0                                          /**< Shift value for LCD_RESISTOR                */
#define _LCD_BIASCTRL_RESISTOR_MASK          0xFUL                                      /**< Bit mask for LCD_RESISTOR                   */
#define _LCD_BIASCTRL_RESISTOR_DEFAULT       0x00000000UL                               /**< Mode DEFAULT for LCD_BIASCTRL               */
#define LCD_BIASCTRL_RESISTOR_DEFAULT        (_LCD_BIASCTRL_RESISTOR_DEFAULT << 0)      /**< Shifted mode DEFAULT for LCD_BIASCTRL       */
#define _LCD_BIASCTRL_BUFDRV_SHIFT           4                                          /**< Shift value for LCD_BUFDRV                  */
#define _LCD_BIASCTRL_BUFDRV_MASK            0x70UL                                     /**< Bit mask for LCD_BUFDRV                     */
#define _LCD_BIASCTRL_BUFDRV_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for LCD_BIASCTRL               */
#define LCD_BIASCTRL_BUFDRV_DEFAULT          (_LCD_BIASCTRL_BUFDRV_DEFAULT << 4)        /**< Shifted mode DEFAULT for LCD_BIASCTRL       */
#define _LCD_BIASCTRL_BUFBIAS_SHIFT          8                                          /**< Shift value for LCD_BUFBIAS                 */
#define _LCD_BIASCTRL_BUFBIAS_MASK           0x300UL                                    /**< Bit mask for LCD_BUFBIAS                    */
#define _LCD_BIASCTRL_BUFBIAS_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for LCD_BIASCTRL               */
#define LCD_BIASCTRL_BUFBIAS_DEFAULT         (_LCD_BIASCTRL_BUFBIAS_DEFAULT << 8)       /**< Shifted mode DEFAULT for LCD_BIASCTRL       */
#define LCD_BIASCTRL_MODE                    (0x1UL << 12)                              /**< Mode Setting                                */
#define _LCD_BIASCTRL_MODE_SHIFT             12                                         /**< Shift value for LCD_MODE                    */
#define _LCD_BIASCTRL_MODE_MASK              0x1000UL                                   /**< Bit mask for LCD_MODE                       */
#define _LCD_BIASCTRL_MODE_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for LCD_BIASCTRL               */
#define _LCD_BIASCTRL_MODE_STEPDOWN          0x00000000UL                               /**< Mode STEPDOWN for LCD_BIASCTRL              */
#define _LCD_BIASCTRL_MODE_CHARGEPUMP        0x00000001UL                               /**< Mode CHARGEPUMP for LCD_BIASCTRL            */
#define LCD_BIASCTRL_MODE_DEFAULT            (_LCD_BIASCTRL_MODE_DEFAULT << 12)         /**< Shifted mode DEFAULT for LCD_BIASCTRL       */
#define LCD_BIASCTRL_MODE_STEPDOWN           (_LCD_BIASCTRL_MODE_STEPDOWN << 12)        /**< Shifted mode STEPDOWN for LCD_BIASCTRL      */
#define LCD_BIASCTRL_MODE_CHARGEPUMP         (_LCD_BIASCTRL_MODE_CHARGEPUMP << 12)      /**< Shifted mode CHARGEPUMP for LCD_BIASCTRL    */
#define _LCD_BIASCTRL_VLCD_SHIFT             16                                         /**< Shift value for LCD_VLCD                    */
#define _LCD_BIASCTRL_VLCD_MASK              0x1F0000UL                                 /**< Bit mask for LCD_VLCD                       */
#define _LCD_BIASCTRL_VLCD_DEFAULT           0x0000001FUL                               /**< Mode DEFAULT for LCD_BIASCTRL               */
#define LCD_BIASCTRL_VLCD_DEFAULT            (_LCD_BIASCTRL_VLCD_DEFAULT << 16)         /**< Shifted mode DEFAULT for LCD_BIASCTRL       */
#define LCD_BIASCTRL_VDDXSEL                 (0x1UL << 22)                              /**< VDDX select                                 */
#define _LCD_BIASCTRL_VDDXSEL_SHIFT          22                                         /**< Shift value for LCD_VDDXSEL                 */
#define _LCD_BIASCTRL_VDDXSEL_MASK           0x400000UL                                 /**< Bit mask for LCD_VDDXSEL                    */
#define _LCD_BIASCTRL_VDDXSEL_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for LCD_BIASCTRL               */
#define _LCD_BIASCTRL_VDDXSEL_DVDD           0x00000000UL                               /**< Mode DVDD for LCD_BIASCTRL                  */
#define _LCD_BIASCTRL_VDDXSEL_AVDD           0x00000001UL                               /**< Mode AVDD for LCD_BIASCTRL                  */
#define LCD_BIASCTRL_VDDXSEL_DEFAULT         (_LCD_BIASCTRL_VDDXSEL_DEFAULT << 22)      /**< Shifted mode DEFAULT for LCD_BIASCTRL       */
#define LCD_BIASCTRL_VDDXSEL_DVDD            (_LCD_BIASCTRL_VDDXSEL_DVDD << 22)         /**< Shifted mode DVDD for LCD_BIASCTRL          */
#define LCD_BIASCTRL_VDDXSEL_AVDD            (_LCD_BIASCTRL_VDDXSEL_AVDD << 22)         /**< Shifted mode AVDD for LCD_BIASCTRL          */
#define LCD_BIASCTRL_LCDGATE                 (0x1UL << 26)                              /**< LCD Gate                                    */
#define _LCD_BIASCTRL_LCDGATE_SHIFT          26                                         /**< Shift value for LCD_LCDGATE                 */
#define _LCD_BIASCTRL_LCDGATE_MASK           0x4000000UL                                /**< Bit mask for LCD_LCDGATE                    */
#define _LCD_BIASCTRL_LCDGATE_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for LCD_BIASCTRL               */
#define _LCD_BIASCTRL_LCDGATE_UNGATE         0x00000000UL                               /**< Mode UNGATE for LCD_BIASCTRL                */
#define _LCD_BIASCTRL_LCDGATE_GATE           0x00000001UL                               /**< Mode GATE for LCD_BIASCTRL                  */
#define LCD_BIASCTRL_LCDGATE_DEFAULT         (_LCD_BIASCTRL_LCDGATE_DEFAULT << 26)      /**< Shifted mode DEFAULT for LCD_BIASCTRL       */
#define LCD_BIASCTRL_LCDGATE_UNGATE          (_LCD_BIASCTRL_LCDGATE_UNGATE << 26)       /**< Shifted mode UNGATE for LCD_BIASCTRL        */
#define LCD_BIASCTRL_LCDGATE_GATE            (_LCD_BIASCTRL_LCDGATE_GATE << 26)         /**< Shifted mode GATE for LCD_BIASCTRL          */
#define _LCD_BIASCTRL_DMAMODE_SHIFT          30                                         /**< Shift value for LCD_DMAMODE                 */
#define _LCD_BIASCTRL_DMAMODE_MASK           0xC0000000UL                               /**< Bit mask for LCD_DMAMODE                    */
#define _LCD_BIASCTRL_DMAMODE_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for LCD_BIASCTRL               */
#define _LCD_BIASCTRL_DMAMODE_DMADISABLE     0x00000000UL                               /**< Mode DMADISABLE for LCD_BIASCTRL            */
#define _LCD_BIASCTRL_DMAMODE_DMAFC          0x00000001UL                               /**< Mode DMAFC for LCD_BIASCTRL                 */
#define _LCD_BIASCTRL_DMAMODE_DMADISPLAY     0x00000002UL                               /**< Mode DMADISPLAY for LCD_BIASCTRL            */
#define LCD_BIASCTRL_DMAMODE_DEFAULT         (_LCD_BIASCTRL_DMAMODE_DEFAULT << 30)      /**< Shifted mode DEFAULT for LCD_BIASCTRL       */
#define LCD_BIASCTRL_DMAMODE_DMADISABLE      (_LCD_BIASCTRL_DMAMODE_DMADISABLE << 30)   /**< Shifted mode DMADISABLE for LCD_BIASCTRL    */
#define LCD_BIASCTRL_DMAMODE_DMAFC           (_LCD_BIASCTRL_DMAMODE_DMAFC << 30)        /**< Shifted mode DMAFC for LCD_BIASCTRL         */
#define LCD_BIASCTRL_DMAMODE_DMADISPLAY      (_LCD_BIASCTRL_DMAMODE_DMADISPLAY << 30)   /**< Shifted mode DMADISPLAY for LCD_BIASCTRL    */

/* Bit fields for LCD DISPCTRLX */
#define _LCD_DISPCTRLX_RESETVALUE            0x00000000UL                               /**< Default value for LCD_DISPCTRLX             */
#define _LCD_DISPCTRLX_MASK                  0x000003FFUL                               /**< Mask for LCD_DISPCTRLX                      */
#define _LCD_DISPCTRLX_DISPLAYDIV_SHIFT      0                                          /**< Shift value for LCD_DISPLAYDIV              */
#define _LCD_DISPCTRLX_DISPLAYDIV_MASK       0x3FFUL                                    /**< Bit mask for LCD_DISPLAYDIV                 */
#define _LCD_DISPCTRLX_DISPLAYDIV_DEFAULT    0x00000000UL                               /**< Mode DEFAULT for LCD_DISPCTRLX              */
#define LCD_DISPCTRLX_DISPLAYDIV_DEFAULT     (_LCD_DISPCTRLX_DISPLAYDIV_DEFAULT << 0)   /**< Shifted mode DEFAULT for LCD_DISPCTRLX      */

/* Bit fields for LCD SEGD0 */
#define _LCD_SEGD0_RESETVALUE                0x00000000UL                               /**< Default value for LCD_SEGD0                 */
#define _LCD_SEGD0_MASK                      0x000FFFFFUL                               /**< Mask for LCD_SEGD0                          */
#define _LCD_SEGD0_SEGD0_SHIFT               0                                          /**< Shift value for LCD_SEGD0                   */
#define _LCD_SEGD0_SEGD0_MASK                0xFFFFFUL                                  /**< Bit mask for LCD_SEGD0                      */
#define _LCD_SEGD0_SEGD0_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_SEGD0                  */
#define LCD_SEGD0_SEGD0_DEFAULT              (_LCD_SEGD0_SEGD0_DEFAULT << 0)            /**< Shifted mode DEFAULT for LCD_SEGD0          */

/* Bit fields for LCD SEGD1 */
#define _LCD_SEGD1_RESETVALUE                0x00000000UL                               /**< Default value for LCD_SEGD1                 */
#define _LCD_SEGD1_MASK                      0x000FFFFFUL                               /**< Mask for LCD_SEGD1                          */
#define _LCD_SEGD1_SEGD1_SHIFT               0                                          /**< Shift value for LCD_SEGD1                   */
#define _LCD_SEGD1_SEGD1_MASK                0xFFFFFUL                                  /**< Bit mask for LCD_SEGD1                      */
#define _LCD_SEGD1_SEGD1_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_SEGD1                  */
#define LCD_SEGD1_SEGD1_DEFAULT              (_LCD_SEGD1_SEGD1_DEFAULT << 0)            /**< Shifted mode DEFAULT for LCD_SEGD1          */

/* Bit fields for LCD SEGD2 */
#define _LCD_SEGD2_RESETVALUE                0x00000000UL                               /**< Default value for LCD_SEGD2                 */
#define _LCD_SEGD2_MASK                      0x000FFFFFUL                               /**< Mask for LCD_SEGD2                          */
#define _LCD_SEGD2_SEGD2_SHIFT               0                                          /**< Shift value for LCD_SEGD2                   */
#define _LCD_SEGD2_SEGD2_MASK                0xFFFFFUL                                  /**< Bit mask for LCD_SEGD2                      */
#define _LCD_SEGD2_SEGD2_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_SEGD2                  */
#define LCD_SEGD2_SEGD2_DEFAULT              (_LCD_SEGD2_SEGD2_DEFAULT << 0)            /**< Shifted mode DEFAULT for LCD_SEGD2          */

/* Bit fields for LCD SEGD3 */
#define _LCD_SEGD3_RESETVALUE                0x00000000UL                               /**< Default value for LCD_SEGD3                 */
#define _LCD_SEGD3_MASK                      0x000FFFFFUL                               /**< Mask for LCD_SEGD3                          */
#define _LCD_SEGD3_SEGD3_SHIFT               0                                          /**< Shift value for LCD_SEGD3                   */
#define _LCD_SEGD3_SEGD3_MASK                0xFFFFFUL                                  /**< Bit mask for LCD_SEGD3                      */
#define _LCD_SEGD3_SEGD3_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for LCD_SEGD3                  */
#define LCD_SEGD3_SEGD3_DEFAULT              (_LCD_SEGD3_SEGD3_DEFAULT << 0)            /**< Shifted mode DEFAULT for LCD_SEGD3          */

/* Bit fields for LCD UPDATECTRL */
#define _LCD_UPDATECTRL_RESETVALUE           0x00000000UL                               /**< Default value for LCD_UPDATECTRL            */
#define _LCD_UPDATECTRL_MASK                 0x0001E100UL                               /**< Mask for LCD_UPDATECTRL                     */
#define LCD_UPDATECTRL_AUTOLOAD              (0x1UL << 8)                               /**< Auto Load                                   */
#define _LCD_UPDATECTRL_AUTOLOAD_SHIFT       8                                          /**< Shift value for LCD_AUTOLOAD                */
#define _LCD_UPDATECTRL_AUTOLOAD_MASK        0x100UL                                    /**< Bit mask for LCD_AUTOLOAD                   */
#define _LCD_UPDATECTRL_AUTOLOAD_DEFAULT     0x00000000UL                               /**< Mode DEFAULT for LCD_UPDATECTRL             */
#define _LCD_UPDATECTRL_AUTOLOAD_MANUAL      0x00000000UL                               /**< Mode MANUAL for LCD_UPDATECTRL              */
#define _LCD_UPDATECTRL_AUTOLOAD_AUTO        0x00000001UL                               /**< Mode AUTO for LCD_UPDATECTRL                */
#define LCD_UPDATECTRL_AUTOLOAD_DEFAULT      (_LCD_UPDATECTRL_AUTOLOAD_DEFAULT << 8)    /**< Shifted mode DEFAULT for LCD_UPDATECTRL     */
#define LCD_UPDATECTRL_AUTOLOAD_MANUAL       (_LCD_UPDATECTRL_AUTOLOAD_MANUAL << 8)     /**< Shifted mode MANUAL for LCD_UPDATECTRL      */
#define LCD_UPDATECTRL_AUTOLOAD_AUTO         (_LCD_UPDATECTRL_AUTOLOAD_AUTO << 8)       /**< Shifted mode AUTO for LCD_UPDATECTRL        */
#define _LCD_UPDATECTRL_LOADADDR_SHIFT       13                                         /**< Shift value for LCD_LOADADDR                */
#define _LCD_UPDATECTRL_LOADADDR_MASK        0x1E000UL                                  /**< Bit mask for LCD_LOADADDR                   */
#define _LCD_UPDATECTRL_LOADADDR_DEFAULT     0x00000000UL                               /**< Mode DEFAULT for LCD_UPDATECTRL             */
#define _LCD_UPDATECTRL_LOADADDR_BACTRLWR    0x00000000UL                               /**< Mode BACTRLWR for LCD_UPDATECTRL            */
#define _LCD_UPDATECTRL_LOADADDR_AREGAWR     0x00000001UL                               /**< Mode AREGAWR for LCD_UPDATECTRL             */
#define _LCD_UPDATECTRL_LOADADDR_AREGBWR     0x00000002UL                               /**< Mode AREGBWR for LCD_UPDATECTRL             */
#define _LCD_UPDATECTRL_LOADADDR_SEGD0WR     0x00000003UL                               /**< Mode SEGD0WR for LCD_UPDATECTRL             */
#define _LCD_UPDATECTRL_LOADADDR_SEGD1WR     0x00000004UL                               /**< Mode SEGD1WR for LCD_UPDATECTRL             */
#define _LCD_UPDATECTRL_LOADADDR_SEGD2WR     0x00000005UL                               /**< Mode SEGD2WR for LCD_UPDATECTRL             */
#define _LCD_UPDATECTRL_LOADADDR_SEGD3WR     0x00000006UL                               /**< Mode SEGD3WR for LCD_UPDATECTRL             */
#define LCD_UPDATECTRL_LOADADDR_DEFAULT      (_LCD_UPDATECTRL_LOADADDR_DEFAULT << 13)   /**< Shifted mode DEFAULT for LCD_UPDATECTRL     */
#define LCD_UPDATECTRL_LOADADDR_BACTRLWR     (_LCD_UPDATECTRL_LOADADDR_BACTRLWR << 13)  /**< Shifted mode BACTRLWR for LCD_UPDATECTRL    */
#define LCD_UPDATECTRL_LOADADDR_AREGAWR      (_LCD_UPDATECTRL_LOADADDR_AREGAWR << 13)   /**< Shifted mode AREGAWR for LCD_UPDATECTRL     */
#define LCD_UPDATECTRL_LOADADDR_AREGBWR      (_LCD_UPDATECTRL_LOADADDR_AREGBWR << 13)   /**< Shifted mode AREGBWR for LCD_UPDATECTRL     */
#define LCD_UPDATECTRL_LOADADDR_SEGD0WR      (_LCD_UPDATECTRL_LOADADDR_SEGD0WR << 13)   /**< Shifted mode SEGD0WR for LCD_UPDATECTRL     */
#define LCD_UPDATECTRL_LOADADDR_SEGD1WR      (_LCD_UPDATECTRL_LOADADDR_SEGD1WR << 13)   /**< Shifted mode SEGD1WR for LCD_UPDATECTRL     */
#define LCD_UPDATECTRL_LOADADDR_SEGD2WR      (_LCD_UPDATECTRL_LOADADDR_SEGD2WR << 13)   /**< Shifted mode SEGD2WR for LCD_UPDATECTRL     */
#define LCD_UPDATECTRL_LOADADDR_SEGD3WR      (_LCD_UPDATECTRL_LOADADDR_SEGD3WR << 13)   /**< Shifted mode SEGD3WR for LCD_UPDATECTRL     */

/* Bit fields for LCD FRAMERATE */
#define _LCD_FRAMERATE_RESETVALUE            0x00000000UL                               /**< Default value for LCD_FRAMERATE             */
#define _LCD_FRAMERATE_MASK                  0x000001FFUL                               /**< Mask for LCD_FRAMERATE                      */
#define _LCD_FRAMERATE_FRDIV_SHIFT           0                                          /**< Shift value for LCD_FRDIV                   */
#define _LCD_FRAMERATE_FRDIV_MASK            0x1FFUL                                    /**< Bit mask for LCD_FRDIV                      */
#define _LCD_FRAMERATE_FRDIV_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for LCD_FRAMERATE              */
#define LCD_FRAMERATE_FRDIV_DEFAULT          (_LCD_FRAMERATE_FRDIV_DEFAULT << 0)        /**< Shifted mode DEFAULT for LCD_FRAMERATE      */

/** @} End of group EFR32ZG23_LCD_BitFields */
/** @} End of group EFR32ZG23_LCD */
/** @} End of group Parts */

#endif /* EFR32ZG23_LCD_H */
#ifdef __cplusplus
}
#endif

