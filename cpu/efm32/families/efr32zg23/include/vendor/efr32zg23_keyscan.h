/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 KEYSCAN register and bit field definitions
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

#ifndef EFR32ZG23_KEYSCAN_H
#define EFR32ZG23_KEYSCAN_H
#define KEYSCAN_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_KEYSCAN KEYSCAN
 * @{
 * @brief EFR32ZG23 KEYSCAN Register Declaration.
 *****************************************************************************/

/** KEYSCAN Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IPVERSION                                          */
  __IOM uint32_t EN;                            /**< Enable                                             */
  __IOM uint32_t SWRST;                         /**< Software Reset                                     */
  __IOM uint32_t CFG;                           /**< Config                                             */
  __IOM uint32_t CMD;                           /**< Command                                            */
  __IOM uint32_t DELAY;                         /**< Delay                                              */
  __IM uint32_t  STATUS;                        /**< Status                                             */
  __IOM uint32_t IF;                            /**< Interrupt Flags                                    */
  __IOM uint32_t IEN;                           /**< Interrupt Enables                                  */
  uint32_t       RESERVED0[1015U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_SET;                        /**< Enable                                             */
  __IOM uint32_t SWRST_SET;                     /**< Software Reset                                     */
  __IOM uint32_t CFG_SET;                       /**< Config                                             */
  __IOM uint32_t CMD_SET;                       /**< Command                                            */
  __IOM uint32_t DELAY_SET;                     /**< Delay                                              */
  __IM uint32_t  STATUS_SET;                    /**< Status                                             */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enables                                  */
  uint32_t       RESERVED1[1015U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_CLR;                        /**< Enable                                             */
  __IOM uint32_t SWRST_CLR;                     /**< Software Reset                                     */
  __IOM uint32_t CFG_CLR;                       /**< Config                                             */
  __IOM uint32_t CMD_CLR;                       /**< Command                                            */
  __IOM uint32_t DELAY_CLR;                     /**< Delay                                              */
  __IM uint32_t  STATUS_CLR;                    /**< Status                                             */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enables                                  */
  uint32_t       RESERVED2[1015U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_TGL;                        /**< Enable                                             */
  __IOM uint32_t SWRST_TGL;                     /**< Software Reset                                     */
  __IOM uint32_t CFG_TGL;                       /**< Config                                             */
  __IOM uint32_t CMD_TGL;                       /**< Command                                            */
  __IOM uint32_t DELAY_TGL;                     /**< Delay                                              */
  __IM uint32_t  STATUS_TGL;                    /**< Status                                             */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enables                                  */
} KEYSCAN_TypeDef;
/** @} End of group EFR32ZG23_KEYSCAN */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_KEYSCAN
 * @{
 * @defgroup EFR32ZG23_KEYSCAN_BitFields KEYSCAN Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for KEYSCAN IPVERSION */
#define _KEYSCAN_IPVERSION_RESETVALUE           0x00000001UL                                /**< Default value for KEYSCAN_IPVERSION         */
#define _KEYSCAN_IPVERSION_MASK                 0xFFFFFFFFUL                                /**< Mask for KEYSCAN_IPVERSION                  */
#define _KEYSCAN_IPVERSION_IPVERSION_SHIFT      0                                           /**< Shift value for KEYSCAN_IPVERSION           */
#define _KEYSCAN_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                                /**< Bit mask for KEYSCAN_IPVERSION              */
#define _KEYSCAN_IPVERSION_IPVERSION_DEFAULT    0x00000001UL                                /**< Mode DEFAULT for KEYSCAN_IPVERSION          */
#define KEYSCAN_IPVERSION_IPVERSION_DEFAULT     (_KEYSCAN_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for KEYSCAN_IPVERSION  */

/* Bit fields for KEYSCAN EN */
#define _KEYSCAN_EN_RESETVALUE                  0x00000000UL                            /**< Default value for KEYSCAN_EN                */
#define _KEYSCAN_EN_MASK                        0x00000003UL                            /**< Mask for KEYSCAN_EN                         */
#define KEYSCAN_EN_EN                           (0x1UL << 0)                            /**< Enable                                      */
#define _KEYSCAN_EN_EN_SHIFT                    0                                       /**< Shift value for KEYSCAN_EN                  */
#define _KEYSCAN_EN_EN_MASK                     0x1UL                                   /**< Bit mask for KEYSCAN_EN                     */
#define _KEYSCAN_EN_EN_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_EN                 */
#define _KEYSCAN_EN_EN_DISABLE                  0x00000000UL                            /**< Mode DISABLE for KEYSCAN_EN                 */
#define _KEYSCAN_EN_EN_ENABLE                   0x00000001UL                            /**< Mode ENABLE for KEYSCAN_EN                  */
#define KEYSCAN_EN_EN_DEFAULT                   (_KEYSCAN_EN_EN_DEFAULT << 0)           /**< Shifted mode DEFAULT for KEYSCAN_EN         */
#define KEYSCAN_EN_EN_DISABLE                   (_KEYSCAN_EN_EN_DISABLE << 0)           /**< Shifted mode DISABLE for KEYSCAN_EN         */
#define KEYSCAN_EN_EN_ENABLE                    (_KEYSCAN_EN_EN_ENABLE << 0)            /**< Shifted mode ENABLE for KEYSCAN_EN          */
#define KEYSCAN_EN_DISABLING                    (0x1UL << 1)                            /**< Disablement busy status                     */
#define _KEYSCAN_EN_DISABLING_SHIFT             1                                       /**< Shift value for KEYSCAN_DISABLING           */
#define _KEYSCAN_EN_DISABLING_MASK              0x2UL                                   /**< Bit mask for KEYSCAN_DISABLING              */
#define _KEYSCAN_EN_DISABLING_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_EN                 */
#define KEYSCAN_EN_DISABLING_DEFAULT            (_KEYSCAN_EN_DISABLING_DEFAULT << 1)    /**< Shifted mode DEFAULT for KEYSCAN_EN         */

/* Bit fields for KEYSCAN SWRST */
#define _KEYSCAN_SWRST_RESETVALUE               0x00000000UL                            /**< Default value for KEYSCAN_SWRST             */
#define _KEYSCAN_SWRST_MASK                     0x00000003UL                            /**< Mask for KEYSCAN_SWRST                      */
#define KEYSCAN_SWRST_SWRST                     (0x1UL << 0)                            /**< Software reset command                      */
#define _KEYSCAN_SWRST_SWRST_SHIFT              0                                       /**< Shift value for KEYSCAN_SWRST               */
#define _KEYSCAN_SWRST_SWRST_MASK               0x1UL                                   /**< Bit mask for KEYSCAN_SWRST                  */
#define _KEYSCAN_SWRST_SWRST_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_SWRST              */
#define KEYSCAN_SWRST_SWRST_DEFAULT             (_KEYSCAN_SWRST_SWRST_DEFAULT << 0)     /**< Shifted mode DEFAULT for KEYSCAN_SWRST      */
#define KEYSCAN_SWRST_RESETTING                 (0x1UL << 1)                            /**< Software reset busy status                  */
#define _KEYSCAN_SWRST_RESETTING_SHIFT          1                                       /**< Shift value for KEYSCAN_RESETTING           */
#define _KEYSCAN_SWRST_RESETTING_MASK           0x2UL                                   /**< Bit mask for KEYSCAN_RESETTING              */
#define _KEYSCAN_SWRST_RESETTING_DEFAULT        0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_SWRST              */
#define KEYSCAN_SWRST_RESETTING_DEFAULT         (_KEYSCAN_SWRST_RESETTING_DEFAULT << 1) /**< Shifted mode DEFAULT for KEYSCAN_SWRST      */

/* Bit fields for KEYSCAN CFG */
#define _KEYSCAN_CFG_RESETVALUE                 0x2501387FUL                                 /**< Default value for KEYSCAN_CFG               */
#define _KEYSCAN_CFG_MASK                       0x7753FFFFUL                                 /**< Mask for KEYSCAN_CFG                        */
#define _KEYSCAN_CFG_CLKDIV_SHIFT               0                                            /**< Shift value for KEYSCAN_CLKDIV              */
#define _KEYSCAN_CFG_CLKDIV_MASK                0x3FFFFUL                                    /**< Bit mask for KEYSCAN_CLKDIV                 */
#define _KEYSCAN_CFG_CLKDIV_DEFAULT             0x0001387FUL                                 /**< Mode DEFAULT for KEYSCAN_CFG                */
#define KEYSCAN_CFG_CLKDIV_DEFAULT              (_KEYSCAN_CFG_CLKDIV_DEFAULT << 0)           /**< Shifted mode DEFAULT for KEYSCAN_CFG        */
#define KEYSCAN_CFG_SINGLEPRESS                 (0x1UL << 20)                                /**< Single Press                                */
#define _KEYSCAN_CFG_SINGLEPRESS_SHIFT          20                                           /**< Shift value for KEYSCAN_SINGLEPRESS         */
#define _KEYSCAN_CFG_SINGLEPRESS_MASK           0x100000UL                                   /**< Bit mask for KEYSCAN_SINGLEPRESS            */
#define _KEYSCAN_CFG_SINGLEPRESS_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for KEYSCAN_CFG                */
#define _KEYSCAN_CFG_SINGLEPRESS_MULTIPRESS     0x00000000UL                                 /**< Mode MULTIPRESS for KEYSCAN_CFG             */
#define _KEYSCAN_CFG_SINGLEPRESS_SINGLEPRESS    0x00000001UL                                 /**< Mode SINGLEPRESS for KEYSCAN_CFG            */
#define KEYSCAN_CFG_SINGLEPRESS_DEFAULT         (_KEYSCAN_CFG_SINGLEPRESS_DEFAULT << 20)     /**< Shifted mode DEFAULT for KEYSCAN_CFG        */
#define KEYSCAN_CFG_SINGLEPRESS_MULTIPRESS      (_KEYSCAN_CFG_SINGLEPRESS_MULTIPRESS << 20)  /**< Shifted mode MULTIPRESS for KEYSCAN_CFG     */
#define KEYSCAN_CFG_SINGLEPRESS_SINGLEPRESS     (_KEYSCAN_CFG_SINGLEPRESS_SINGLEPRESS << 20) /**< Shifted mode SINGLEPRESS for KEYSCAN_CFG    */
#define KEYSCAN_CFG_AUTOSTART                   (0x1UL << 22)                                /**< Automatically Start                         */
#define _KEYSCAN_CFG_AUTOSTART_SHIFT            22                                           /**< Shift value for KEYSCAN_AUTOSTART           */
#define _KEYSCAN_CFG_AUTOSTART_MASK             0x400000UL                                   /**< Bit mask for KEYSCAN_AUTOSTART              */
#define _KEYSCAN_CFG_AUTOSTART_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for KEYSCAN_CFG                */
#define _KEYSCAN_CFG_AUTOSTART_AUTOSTARTDIS     0x00000000UL                                 /**< Mode AUTOSTARTDIS for KEYSCAN_CFG           */
#define _KEYSCAN_CFG_AUTOSTART_AUTOSTARTEN      0x00000001UL                                 /**< Mode AUTOSTARTEN for KEYSCAN_CFG            */
#define KEYSCAN_CFG_AUTOSTART_DEFAULT           (_KEYSCAN_CFG_AUTOSTART_DEFAULT << 22)       /**< Shifted mode DEFAULT for KEYSCAN_CFG        */
#define KEYSCAN_CFG_AUTOSTART_AUTOSTARTDIS      (_KEYSCAN_CFG_AUTOSTART_AUTOSTARTDIS << 22)  /**< Shifted mode AUTOSTARTDIS for KEYSCAN_CFG   */
#define KEYSCAN_CFG_AUTOSTART_AUTOSTARTEN       (_KEYSCAN_CFG_AUTOSTART_AUTOSTARTEN << 22)   /**< Shifted mode AUTOSTARTEN for KEYSCAN_CFG    */
#define _KEYSCAN_CFG_NUMROWS_SHIFT              24                                           /**< Shift value for KEYSCAN_NUMROWS             */
#define _KEYSCAN_CFG_NUMROWS_MASK               0x7000000UL                                  /**< Bit mask for KEYSCAN_NUMROWS                */
#define _KEYSCAN_CFG_NUMROWS_DEFAULT            0x00000005UL                                 /**< Mode DEFAULT for KEYSCAN_CFG                */
#define _KEYSCAN_CFG_NUMROWS_RSV1               0x00000000UL                                 /**< Mode RSV1 for KEYSCAN_CFG                   */
#define _KEYSCAN_CFG_NUMROWS_RSV2               0x00000001UL                                 /**< Mode RSV2 for KEYSCAN_CFG                   */
#define _KEYSCAN_CFG_NUMROWS_ROW3               0x00000002UL                                 /**< Mode ROW3 for KEYSCAN_CFG                   */
#define _KEYSCAN_CFG_NUMROWS_ROW4               0x00000003UL                                 /**< Mode ROW4 for KEYSCAN_CFG                   */
#define _KEYSCAN_CFG_NUMROWS_ROW5               0x00000004UL                                 /**< Mode ROW5 for KEYSCAN_CFG                   */
#define _KEYSCAN_CFG_NUMROWS_ROW6               0x00000005UL                                 /**< Mode ROW6 for KEYSCAN_CFG                   */
#define KEYSCAN_CFG_NUMROWS_DEFAULT             (_KEYSCAN_CFG_NUMROWS_DEFAULT << 24)         /**< Shifted mode DEFAULT for KEYSCAN_CFG        */
#define KEYSCAN_CFG_NUMROWS_RSV1                (_KEYSCAN_CFG_NUMROWS_RSV1 << 24)            /**< Shifted mode RSV1 for KEYSCAN_CFG           */
#define KEYSCAN_CFG_NUMROWS_RSV2                (_KEYSCAN_CFG_NUMROWS_RSV2 << 24)            /**< Shifted mode RSV2 for KEYSCAN_CFG           */
#define KEYSCAN_CFG_NUMROWS_ROW3                (_KEYSCAN_CFG_NUMROWS_ROW3 << 24)            /**< Shifted mode ROW3 for KEYSCAN_CFG           */
#define KEYSCAN_CFG_NUMROWS_ROW4                (_KEYSCAN_CFG_NUMROWS_ROW4 << 24)            /**< Shifted mode ROW4 for KEYSCAN_CFG           */
#define KEYSCAN_CFG_NUMROWS_ROW5                (_KEYSCAN_CFG_NUMROWS_ROW5 << 24)            /**< Shifted mode ROW5 for KEYSCAN_CFG           */
#define KEYSCAN_CFG_NUMROWS_ROW6                (_KEYSCAN_CFG_NUMROWS_ROW6 << 24)            /**< Shifted mode ROW6 for KEYSCAN_CFG           */
#define _KEYSCAN_CFG_NUMCOLS_SHIFT              28                                           /**< Shift value for KEYSCAN_NUMCOLS             */
#define _KEYSCAN_CFG_NUMCOLS_MASK               0x70000000UL                                 /**< Bit mask for KEYSCAN_NUMCOLS                */
#define _KEYSCAN_CFG_NUMCOLS_DEFAULT            0x00000002UL                                 /**< Mode DEFAULT for KEYSCAN_CFG                */
#define KEYSCAN_CFG_NUMCOLS_DEFAULT             (_KEYSCAN_CFG_NUMCOLS_DEFAULT << 28)         /**< Shifted mode DEFAULT for KEYSCAN_CFG        */

/* Bit fields for KEYSCAN CMD */
#define _KEYSCAN_CMD_RESETVALUE                 0x00000000UL                             /**< Default value for KEYSCAN_CMD               */
#define _KEYSCAN_CMD_MASK                       0x00000003UL                             /**< Mask for KEYSCAN_CMD                        */
#define KEYSCAN_CMD_KEYSCANSTART                (0x1UL << 0)                             /**< Keyscan Start                               */
#define _KEYSCAN_CMD_KEYSCANSTART_SHIFT         0                                        /**< Shift value for KEYSCAN_KEYSCANSTART        */
#define _KEYSCAN_CMD_KEYSCANSTART_MASK          0x1UL                                    /**< Bit mask for KEYSCAN_KEYSCANSTART           */
#define _KEYSCAN_CMD_KEYSCANSTART_DEFAULT       0x00000000UL                             /**< Mode DEFAULT for KEYSCAN_CMD                */
#define KEYSCAN_CMD_KEYSCANSTART_DEFAULT        (_KEYSCAN_CMD_KEYSCANSTART_DEFAULT << 0) /**< Shifted mode DEFAULT for KEYSCAN_CMD        */
#define KEYSCAN_CMD_KEYSCANSTOP                 (0x1UL << 1)                             /**< Keyscan Stop                                */
#define _KEYSCAN_CMD_KEYSCANSTOP_SHIFT          1                                        /**< Shift value for KEYSCAN_KEYSCANSTOP         */
#define _KEYSCAN_CMD_KEYSCANSTOP_MASK           0x2UL                                    /**< Bit mask for KEYSCAN_KEYSCANSTOP            */
#define _KEYSCAN_CMD_KEYSCANSTOP_DEFAULT        0x00000000UL                             /**< Mode DEFAULT for KEYSCAN_CMD                */
#define KEYSCAN_CMD_KEYSCANSTOP_DEFAULT         (_KEYSCAN_CMD_KEYSCANSTOP_DEFAULT << 1)  /**< Shifted mode DEFAULT for KEYSCAN_CMD        */

/* Bit fields for KEYSCAN DELAY */
#define _KEYSCAN_DELAY_RESETVALUE               0x00000000UL                             /**< Default value for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_MASK                     0x0F0F0F00UL                             /**< Mask for KEYSCAN_DELAY                      */
#define _KEYSCAN_DELAY_SCANDLY_SHIFT            8                                        /**< Shift value for KEYSCAN_SCANDLY             */
#define _KEYSCAN_DELAY_SCANDLY_MASK             0xF00UL                                  /**< Bit mask for KEYSCAN_SCANDLY                */
#define _KEYSCAN_DELAY_SCANDLY_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for KEYSCAN_DELAY              */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY2         0x00000000UL                             /**< Mode SCANDLY2 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY4         0x00000001UL                             /**< Mode SCANDLY4 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY6         0x00000002UL                             /**< Mode SCANDLY6 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY8         0x00000003UL                             /**< Mode SCANDLY8 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY10        0x00000004UL                             /**< Mode SCANDLY10 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY12        0x00000005UL                             /**< Mode SCANDLY12 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY14        0x00000006UL                             /**< Mode SCANDLY14 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY16        0x00000007UL                             /**< Mode SCANDLY16 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY18        0x00000008UL                             /**< Mode SCANDLY18 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY20        0x00000009UL                             /**< Mode SCANDLY20 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY22        0x0000000AUL                             /**< Mode SCANDLY22 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY24        0x0000000BUL                             /**< Mode SCANDLY24 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY26        0x0000000CUL                             /**< Mode SCANDLY26 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY28        0x0000000DUL                             /**< Mode SCANDLY28 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY30        0x0000000EUL                             /**< Mode SCANDLY30 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_SCANDLY_SCANDLY32        0x0000000FUL                             /**< Mode SCANDLY32 for KEYSCAN_DELAY            */
#define KEYSCAN_DELAY_SCANDLY_DEFAULT           (_KEYSCAN_DELAY_SCANDLY_DEFAULT << 8)    /**< Shifted mode DEFAULT for KEYSCAN_DELAY      */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY2          (_KEYSCAN_DELAY_SCANDLY_SCANDLY2 << 8)   /**< Shifted mode SCANDLY2 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY4          (_KEYSCAN_DELAY_SCANDLY_SCANDLY4 << 8)   /**< Shifted mode SCANDLY4 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY6          (_KEYSCAN_DELAY_SCANDLY_SCANDLY6 << 8)   /**< Shifted mode SCANDLY6 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY8          (_KEYSCAN_DELAY_SCANDLY_SCANDLY8 << 8)   /**< Shifted mode SCANDLY8 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY10         (_KEYSCAN_DELAY_SCANDLY_SCANDLY10 << 8)  /**< Shifted mode SCANDLY10 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY12         (_KEYSCAN_DELAY_SCANDLY_SCANDLY12 << 8)  /**< Shifted mode SCANDLY12 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY14         (_KEYSCAN_DELAY_SCANDLY_SCANDLY14 << 8)  /**< Shifted mode SCANDLY14 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY16         (_KEYSCAN_DELAY_SCANDLY_SCANDLY16 << 8)  /**< Shifted mode SCANDLY16 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY18         (_KEYSCAN_DELAY_SCANDLY_SCANDLY18 << 8)  /**< Shifted mode SCANDLY18 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY20         (_KEYSCAN_DELAY_SCANDLY_SCANDLY20 << 8)  /**< Shifted mode SCANDLY20 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY22         (_KEYSCAN_DELAY_SCANDLY_SCANDLY22 << 8)  /**< Shifted mode SCANDLY22 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY24         (_KEYSCAN_DELAY_SCANDLY_SCANDLY24 << 8)  /**< Shifted mode SCANDLY24 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY26         (_KEYSCAN_DELAY_SCANDLY_SCANDLY26 << 8)  /**< Shifted mode SCANDLY26 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY28         (_KEYSCAN_DELAY_SCANDLY_SCANDLY28 << 8)  /**< Shifted mode SCANDLY28 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY30         (_KEYSCAN_DELAY_SCANDLY_SCANDLY30 << 8)  /**< Shifted mode SCANDLY30 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_SCANDLY_SCANDLY32         (_KEYSCAN_DELAY_SCANDLY_SCANDLY32 << 8)  /**< Shifted mode SCANDLY32 for KEYSCAN_DELAY    */
#define _KEYSCAN_DELAY_DEBDLY_SHIFT             16                                       /**< Shift value for KEYSCAN_DEBDLY              */
#define _KEYSCAN_DELAY_DEBDLY_MASK              0xF0000UL                                /**< Bit mask for KEYSCAN_DEBDLY                 */
#define _KEYSCAN_DELAY_DEBDLY_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for KEYSCAN_DELAY              */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY2           0x00000000UL                             /**< Mode DEBDLY2 for KEYSCAN_DELAY              */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY4           0x00000001UL                             /**< Mode DEBDLY4 for KEYSCAN_DELAY              */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY6           0x00000002UL                             /**< Mode DEBDLY6 for KEYSCAN_DELAY              */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY8           0x00000003UL                             /**< Mode DEBDLY8 for KEYSCAN_DELAY              */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY10          0x00000004UL                             /**< Mode DEBDLY10 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY12          0x00000005UL                             /**< Mode DEBDLY12 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY14          0x00000006UL                             /**< Mode DEBDLY14 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY16          0x00000007UL                             /**< Mode DEBDLY16 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY18          0x00000008UL                             /**< Mode DEBDLY18 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY20          0x00000009UL                             /**< Mode DEBDLY20 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY22          0x0000000AUL                             /**< Mode DEBDLY22 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY24          0x0000000BUL                             /**< Mode DEBDLY24 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY26          0x0000000CUL                             /**< Mode DEBDLY26 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY28          0x0000000DUL                             /**< Mode DEBDLY28 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY30          0x0000000EUL                             /**< Mode DEBDLY30 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_DEBDLY_DEBDLY32          0x0000000FUL                             /**< Mode DEBDLY32 for KEYSCAN_DELAY             */
#define KEYSCAN_DELAY_DEBDLY_DEFAULT            (_KEYSCAN_DELAY_DEBDLY_DEFAULT << 16)    /**< Shifted mode DEFAULT for KEYSCAN_DELAY      */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY2            (_KEYSCAN_DELAY_DEBDLY_DEBDLY2 << 16)    /**< Shifted mode DEBDLY2 for KEYSCAN_DELAY      */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY4            (_KEYSCAN_DELAY_DEBDLY_DEBDLY4 << 16)    /**< Shifted mode DEBDLY4 for KEYSCAN_DELAY      */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY6            (_KEYSCAN_DELAY_DEBDLY_DEBDLY6 << 16)    /**< Shifted mode DEBDLY6 for KEYSCAN_DELAY      */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY8            (_KEYSCAN_DELAY_DEBDLY_DEBDLY8 << 16)    /**< Shifted mode DEBDLY8 for KEYSCAN_DELAY      */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY10           (_KEYSCAN_DELAY_DEBDLY_DEBDLY10 << 16)   /**< Shifted mode DEBDLY10 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY12           (_KEYSCAN_DELAY_DEBDLY_DEBDLY12 << 16)   /**< Shifted mode DEBDLY12 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY14           (_KEYSCAN_DELAY_DEBDLY_DEBDLY14 << 16)   /**< Shifted mode DEBDLY14 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY16           (_KEYSCAN_DELAY_DEBDLY_DEBDLY16 << 16)   /**< Shifted mode DEBDLY16 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY18           (_KEYSCAN_DELAY_DEBDLY_DEBDLY18 << 16)   /**< Shifted mode DEBDLY18 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY20           (_KEYSCAN_DELAY_DEBDLY_DEBDLY20 << 16)   /**< Shifted mode DEBDLY20 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY22           (_KEYSCAN_DELAY_DEBDLY_DEBDLY22 << 16)   /**< Shifted mode DEBDLY22 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY24           (_KEYSCAN_DELAY_DEBDLY_DEBDLY24 << 16)   /**< Shifted mode DEBDLY24 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY26           (_KEYSCAN_DELAY_DEBDLY_DEBDLY26 << 16)   /**< Shifted mode DEBDLY26 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY28           (_KEYSCAN_DELAY_DEBDLY_DEBDLY28 << 16)   /**< Shifted mode DEBDLY28 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY30           (_KEYSCAN_DELAY_DEBDLY_DEBDLY30 << 16)   /**< Shifted mode DEBDLY30 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_DEBDLY_DEBDLY32           (_KEYSCAN_DELAY_DEBDLY_DEBDLY32 << 16)   /**< Shifted mode DEBDLY32 for KEYSCAN_DELAY     */
#define _KEYSCAN_DELAY_STABDLY_SHIFT            24                                       /**< Shift value for KEYSCAN_STABDLY             */
#define _KEYSCAN_DELAY_STABDLY_MASK             0xF000000UL                              /**< Bit mask for KEYSCAN_STABDLY                */
#define _KEYSCAN_DELAY_STABDLY_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for KEYSCAN_DELAY              */
#define _KEYSCAN_DELAY_STABDLY_STABDLY2         0x00000000UL                             /**< Mode STABDLY2 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_STABDLY_STABDLY4         0x00000001UL                             /**< Mode STABDLY4 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_STABDLY_STABDLY6         0x00000002UL                             /**< Mode STABDLY6 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_STABDLY_STABDLY8         0x00000003UL                             /**< Mode STABDLY8 for KEYSCAN_DELAY             */
#define _KEYSCAN_DELAY_STABDLY_STABDLY10        0x00000004UL                             /**< Mode STABDLY10 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_STABDLY_STABDLY12        0x00000005UL                             /**< Mode STABDLY12 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_STABDLY_STABDLY14        0x00000006UL                             /**< Mode STABDLY14 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_STABDLY_STABDLY16        0x00000007UL                             /**< Mode STABDLY16 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_STABDLY_STABDLY18        0x00000008UL                             /**< Mode STABDLY18 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_STABDLY_STABDLY20        0x00000009UL                             /**< Mode STABDLY20 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_STABDLY_STABDLY22        0x0000000AUL                             /**< Mode STABDLY22 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_STABDLY_STABDLY24        0x0000000BUL                             /**< Mode STABDLY24 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_STABDLY_STABDLY26        0x0000000CUL                             /**< Mode STABDLY26 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_STABDLY_STABDLY28        0x0000000DUL                             /**< Mode STABDLY28 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_STABDLY_STABDLY30        0x0000000EUL                             /**< Mode STABDLY30 for KEYSCAN_DELAY            */
#define _KEYSCAN_DELAY_STABDLY_STABDLY32        0x0000000FUL                             /**< Mode STABDLY32 for KEYSCAN_DELAY            */
#define KEYSCAN_DELAY_STABDLY_DEFAULT           (_KEYSCAN_DELAY_STABDLY_DEFAULT << 24)   /**< Shifted mode DEFAULT for KEYSCAN_DELAY      */
#define KEYSCAN_DELAY_STABDLY_STABDLY2          (_KEYSCAN_DELAY_STABDLY_STABDLY2 << 24)  /**< Shifted mode STABDLY2 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_STABDLY_STABDLY4          (_KEYSCAN_DELAY_STABDLY_STABDLY4 << 24)  /**< Shifted mode STABDLY4 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_STABDLY_STABDLY6          (_KEYSCAN_DELAY_STABDLY_STABDLY6 << 24)  /**< Shifted mode STABDLY6 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_STABDLY_STABDLY8          (_KEYSCAN_DELAY_STABDLY_STABDLY8 << 24)  /**< Shifted mode STABDLY8 for KEYSCAN_DELAY     */
#define KEYSCAN_DELAY_STABDLY_STABDLY10         (_KEYSCAN_DELAY_STABDLY_STABDLY10 << 24) /**< Shifted mode STABDLY10 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_STABDLY_STABDLY12         (_KEYSCAN_DELAY_STABDLY_STABDLY12 << 24) /**< Shifted mode STABDLY12 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_STABDLY_STABDLY14         (_KEYSCAN_DELAY_STABDLY_STABDLY14 << 24) /**< Shifted mode STABDLY14 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_STABDLY_STABDLY16         (_KEYSCAN_DELAY_STABDLY_STABDLY16 << 24) /**< Shifted mode STABDLY16 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_STABDLY_STABDLY18         (_KEYSCAN_DELAY_STABDLY_STABDLY18 << 24) /**< Shifted mode STABDLY18 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_STABDLY_STABDLY20         (_KEYSCAN_DELAY_STABDLY_STABDLY20 << 24) /**< Shifted mode STABDLY20 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_STABDLY_STABDLY22         (_KEYSCAN_DELAY_STABDLY_STABDLY22 << 24) /**< Shifted mode STABDLY22 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_STABDLY_STABDLY24         (_KEYSCAN_DELAY_STABDLY_STABDLY24 << 24) /**< Shifted mode STABDLY24 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_STABDLY_STABDLY26         (_KEYSCAN_DELAY_STABDLY_STABDLY26 << 24) /**< Shifted mode STABDLY26 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_STABDLY_STABDLY28         (_KEYSCAN_DELAY_STABDLY_STABDLY28 << 24) /**< Shifted mode STABDLY28 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_STABDLY_STABDLY30         (_KEYSCAN_DELAY_STABDLY_STABDLY30 << 24) /**< Shifted mode STABDLY30 for KEYSCAN_DELAY    */
#define KEYSCAN_DELAY_STABDLY_STABDLY32         (_KEYSCAN_DELAY_STABDLY_STABDLY32 << 24) /**< Shifted mode STABDLY32 for KEYSCAN_DELAY    */

/* Bit fields for KEYSCAN STATUS */
#define _KEYSCAN_STATUS_RESETVALUE              0x40000000UL                             /**< Default value for KEYSCAN_STATUS            */
#define _KEYSCAN_STATUS_MASK                    0xC701003FUL                             /**< Mask for KEYSCAN_STATUS                     */
#define _KEYSCAN_STATUS_ROW_SHIFT               0                                        /**< Shift value for KEYSCAN_ROW                 */
#define _KEYSCAN_STATUS_ROW_MASK                0x3FUL                                   /**< Bit mask for KEYSCAN_ROW                    */
#define _KEYSCAN_STATUS_ROW_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for KEYSCAN_STATUS             */
#define KEYSCAN_STATUS_ROW_DEFAULT              (_KEYSCAN_STATUS_ROW_DEFAULT << 0)       /**< Shifted mode DEFAULT for KEYSCAN_STATUS     */
#define KEYSCAN_STATUS_RUNNING                  (0x1UL << 16)                            /**< Running                                     */
#define _KEYSCAN_STATUS_RUNNING_SHIFT           16                                       /**< Shift value for KEYSCAN_RUNNING             */
#define _KEYSCAN_STATUS_RUNNING_MASK            0x10000UL                                /**< Bit mask for KEYSCAN_RUNNING                */
#define _KEYSCAN_STATUS_RUNNING_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for KEYSCAN_STATUS             */
#define KEYSCAN_STATUS_RUNNING_DEFAULT          (_KEYSCAN_STATUS_RUNNING_DEFAULT << 16)  /**< Shifted mode DEFAULT for KEYSCAN_STATUS     */
#define _KEYSCAN_STATUS_COL_SHIFT               24                                       /**< Shift value for KEYSCAN_COL                 */
#define _KEYSCAN_STATUS_COL_MASK                0x7000000UL                              /**< Bit mask for KEYSCAN_COL                    */
#define _KEYSCAN_STATUS_COL_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for KEYSCAN_STATUS             */
#define KEYSCAN_STATUS_COL_DEFAULT              (_KEYSCAN_STATUS_COL_DEFAULT << 24)      /**< Shifted mode DEFAULT for KEYSCAN_STATUS     */
#define KEYSCAN_STATUS_NOKEY                    (0x1UL << 30)                            /**< No Key pressed status                       */
#define _KEYSCAN_STATUS_NOKEY_SHIFT             30                                       /**< Shift value for KEYSCAN_NOKEY               */
#define _KEYSCAN_STATUS_NOKEY_MASK              0x40000000UL                             /**< Bit mask for KEYSCAN_NOKEY                  */
#define _KEYSCAN_STATUS_NOKEY_DEFAULT           0x00000001UL                             /**< Mode DEFAULT for KEYSCAN_STATUS             */
#define KEYSCAN_STATUS_NOKEY_DEFAULT            (_KEYSCAN_STATUS_NOKEY_DEFAULT << 30)    /**< Shifted mode DEFAULT for KEYSCAN_STATUS     */
#define KEYSCAN_STATUS_SYNCBUSY                 (0x1UL << 31)                            /**< Sync Busy                                   */
#define _KEYSCAN_STATUS_SYNCBUSY_SHIFT          31                                       /**< Shift value for KEYSCAN_SYNCBUSY            */
#define _KEYSCAN_STATUS_SYNCBUSY_MASK           0x80000000UL                             /**< Bit mask for KEYSCAN_SYNCBUSY               */
#define _KEYSCAN_STATUS_SYNCBUSY_DEFAULT        0x00000000UL                             /**< Mode DEFAULT for KEYSCAN_STATUS             */
#define KEYSCAN_STATUS_SYNCBUSY_DEFAULT         (_KEYSCAN_STATUS_SYNCBUSY_DEFAULT << 31) /**< Shifted mode DEFAULT for KEYSCAN_STATUS     */

/* Bit fields for KEYSCAN IF */
#define _KEYSCAN_IF_RESETVALUE                  0x00000000UL                            /**< Default value for KEYSCAN_IF                */
#define _KEYSCAN_IF_MASK                        0x0000000FUL                            /**< Mask for KEYSCAN_IF                         */
#define KEYSCAN_IF_NOKEY                        (0x1UL << 0)                            /**< No key was pressed                          */
#define _KEYSCAN_IF_NOKEY_SHIFT                 0                                       /**< Shift value for KEYSCAN_NOKEY               */
#define _KEYSCAN_IF_NOKEY_MASK                  0x1UL                                   /**< Bit mask for KEYSCAN_NOKEY                  */
#define _KEYSCAN_IF_NOKEY_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_IF                 */
#define KEYSCAN_IF_NOKEY_DEFAULT                (_KEYSCAN_IF_NOKEY_DEFAULT << 0)        /**< Shifted mode DEFAULT for KEYSCAN_IF         */
#define KEYSCAN_IF_KEY                          (0x1UL << 1)                            /**< A key was pressed                           */
#define _KEYSCAN_IF_KEY_SHIFT                   1                                       /**< Shift value for KEYSCAN_KEY                 */
#define _KEYSCAN_IF_KEY_MASK                    0x2UL                                   /**< Bit mask for KEYSCAN_KEY                    */
#define _KEYSCAN_IF_KEY_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_IF                 */
#define KEYSCAN_IF_KEY_DEFAULT                  (_KEYSCAN_IF_KEY_DEFAULT << 1)          /**< Shifted mode DEFAULT for KEYSCAN_IF         */
#define KEYSCAN_IF_SCANNED                      (0x1UL << 2)                            /**< Completed scan                              */
#define _KEYSCAN_IF_SCANNED_SHIFT               2                                       /**< Shift value for KEYSCAN_SCANNED             */
#define _KEYSCAN_IF_SCANNED_MASK                0x4UL                                   /**< Bit mask for KEYSCAN_SCANNED                */
#define _KEYSCAN_IF_SCANNED_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_IF                 */
#define KEYSCAN_IF_SCANNED_DEFAULT              (_KEYSCAN_IF_SCANNED_DEFAULT << 2)      /**< Shifted mode DEFAULT for KEYSCAN_IF         */
#define KEYSCAN_IF_WAKEUP                       (0x1UL << 3)                            /**< Wake up                                     */
#define _KEYSCAN_IF_WAKEUP_SHIFT                3                                       /**< Shift value for KEYSCAN_WAKEUP              */
#define _KEYSCAN_IF_WAKEUP_MASK                 0x8UL                                   /**< Bit mask for KEYSCAN_WAKEUP                 */
#define _KEYSCAN_IF_WAKEUP_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_IF                 */
#define KEYSCAN_IF_WAKEUP_DEFAULT               (_KEYSCAN_IF_WAKEUP_DEFAULT << 3)       /**< Shifted mode DEFAULT for KEYSCAN_IF         */

/* Bit fields for KEYSCAN IEN */
#define _KEYSCAN_IEN_RESETVALUE                 0x00000000UL                            /**< Default value for KEYSCAN_IEN               */
#define _KEYSCAN_IEN_MASK                       0x0000000FUL                            /**< Mask for KEYSCAN_IEN                        */
#define KEYSCAN_IEN_NOKEY                       (0x1UL << 0)                            /**< No Key was pressed                          */
#define _KEYSCAN_IEN_NOKEY_SHIFT                0                                       /**< Shift value for KEYSCAN_NOKEY               */
#define _KEYSCAN_IEN_NOKEY_MASK                 0x1UL                                   /**< Bit mask for KEYSCAN_NOKEY                  */
#define _KEYSCAN_IEN_NOKEY_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_IEN                */
#define KEYSCAN_IEN_NOKEY_DEFAULT               (_KEYSCAN_IEN_NOKEY_DEFAULT << 0)       /**< Shifted mode DEFAULT for KEYSCAN_IEN        */
#define KEYSCAN_IEN_KEY                         (0x1UL << 1)                            /**< A Key was pressed                           */
#define _KEYSCAN_IEN_KEY_SHIFT                  1                                       /**< Shift value for KEYSCAN_KEY                 */
#define _KEYSCAN_IEN_KEY_MASK                   0x2UL                                   /**< Bit mask for KEYSCAN_KEY                    */
#define _KEYSCAN_IEN_KEY_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_IEN                */
#define KEYSCAN_IEN_KEY_DEFAULT                 (_KEYSCAN_IEN_KEY_DEFAULT << 1)         /**< Shifted mode DEFAULT for KEYSCAN_IEN        */
#define KEYSCAN_IEN_SCANNED                     (0x1UL << 2)                            /**< Completed Scanning                          */
#define _KEYSCAN_IEN_SCANNED_SHIFT              2                                       /**< Shift value for KEYSCAN_SCANNED             */
#define _KEYSCAN_IEN_SCANNED_MASK               0x4UL                                   /**< Bit mask for KEYSCAN_SCANNED                */
#define _KEYSCAN_IEN_SCANNED_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_IEN                */
#define KEYSCAN_IEN_SCANNED_DEFAULT             (_KEYSCAN_IEN_SCANNED_DEFAULT << 2)     /**< Shifted mode DEFAULT for KEYSCAN_IEN        */
#define KEYSCAN_IEN_WAKEUP                      (0x1UL << 3)                            /**< Wake up                                     */
#define _KEYSCAN_IEN_WAKEUP_SHIFT               3                                       /**< Shift value for KEYSCAN_WAKEUP              */
#define _KEYSCAN_IEN_WAKEUP_MASK                0x8UL                                   /**< Bit mask for KEYSCAN_WAKEUP                 */
#define _KEYSCAN_IEN_WAKEUP_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for KEYSCAN_IEN                */
#define KEYSCAN_IEN_WAKEUP_DEFAULT              (_KEYSCAN_IEN_WAKEUP_DEFAULT << 3)      /**< Shifted mode DEFAULT for KEYSCAN_IEN        */

/** @} End of group EFR32ZG23_KEYSCAN_BitFields */
/** @} End of group EFR32ZG23_KEYSCAN */
/** @} End of group Parts */

#endif /* EFR32ZG23_KEYSCAN_H */
#ifdef __cplusplus
}
#endif

