/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 AES register and bit field definitions
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

#ifndef EFR32ZG23_AES_H
#define EFR32ZG23_AES_H

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_AES AES
 * @{
 * @brief EFR32ZG23 AES Register Declaration.
 *****************************************************************************/

/** AES Register Declaration. */
typedef struct {
  __IOM uint32_t FETCHADDR;                     /**< Fetcher Address                                    */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t FETCHLEN;                      /**< Fetcher Length                                     */
  __IOM uint32_t FETCHTAG;                      /**< Fetcher Tag                                        */
  __IOM uint32_t PUSHADDR;                      /**< Pusher Address                                     */
  uint32_t       RESERVED1[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t PUSHLEN;                       /**< Pusher Length                                      */
  __IOM uint32_t IEN;                           /**< Interrupt Enable                                   */
  uint32_t       RESERVED2[2U];                 /**< Reserved for future use                            */
  __IM uint32_t  IF;                            /**< Interrupt Flags                                    */
  uint32_t       RESERVED3[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt status clear                             */
  __IOM uint32_t CTRL;                          /**< Control register                                   */
  __IOM uint32_t CMD;                           /**< Command register                                   */
  __IM uint32_t  STATUS;                        /**< Status register                                    */
  uint32_t       RESERVED4[240U];               /**< Reserved for future use                            */
  __IM uint32_t  INCL_IPS_HW_CFG;               /**< INCL_IPS_HW_CFG                                    */
  __IM uint32_t  BA411E_HW_CFG_1;               /**< BA411E_HW_CFG_1                                    */
  __IM uint32_t  BA411E_HW_CFG_2;               /**< BA411E_HW_CFG_2                                    */
  __IM uint32_t  BA413_HW_CFG;                  /**< BA413_HW_CFG                                       */
  __IM uint32_t  BA418_HW_CFG;                  /**< BA418_HW_CFG                                       */
  __IM uint32_t  BA419_HW_CFG;                  /**< BA419_HW_CFG                                       */
} AES_TypeDef;
/** @} End of group EFR32ZG23_AES */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_AES
 * @{
 * @defgroup EFR32ZG23_AES_BitFields AES Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for AES FETCHADDR */
#define _AES_FETCHADDR_RESETVALUE                           0x00000000UL                       /**< Default value for AES_FETCHADDR             */
#define _AES_FETCHADDR_MASK                                 0xFFFFFFFFUL                       /**< Mask for AES_FETCHADDR                      */
#define _AES_FETCHADDR_ADDR_SHIFT                           0                                  /**< Shift value for AES_ADDR                    */
#define _AES_FETCHADDR_ADDR_MASK                            0xFFFFFFFFUL                       /**< Bit mask for AES_ADDR                       */
#define _AES_FETCHADDR_ADDR_DEFAULT                         0x00000000UL                       /**< Mode DEFAULT for AES_FETCHADDR              */
#define AES_FETCHADDR_ADDR_DEFAULT                          (_AES_FETCHADDR_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_FETCHADDR      */

/* Bit fields for AES FETCHLEN */
#define _AES_FETCHLEN_RESETVALUE                            0x00000000UL                            /**< Default value for AES_FETCHLEN              */
#define _AES_FETCHLEN_MASK                                  0x3FFFFFFFUL                            /**< Mask for AES_FETCHLEN                       */
#define _AES_FETCHLEN_LENGTH_SHIFT                          0                                       /**< Shift value for AES_LENGTH                  */
#define _AES_FETCHLEN_LENGTH_MASK                           0xFFFFFFFUL                             /**< Bit mask for AES_LENGTH                     */
#define _AES_FETCHLEN_LENGTH_DEFAULT                        0x00000000UL                            /**< Mode DEFAULT for AES_FETCHLEN               */
#define AES_FETCHLEN_LENGTH_DEFAULT                         (_AES_FETCHLEN_LENGTH_DEFAULT << 0)     /**< Shifted mode DEFAULT for AES_FETCHLEN       */
#define AES_FETCHLEN_CONSTADDR                              (0x1UL << 28)                           /**< Constant address                            */
#define _AES_FETCHLEN_CONSTADDR_SHIFT                       28                                      /**< Shift value for AES_CONSTADDR               */
#define _AES_FETCHLEN_CONSTADDR_MASK                        0x10000000UL                            /**< Bit mask for AES_CONSTADDR                  */
#define _AES_FETCHLEN_CONSTADDR_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for AES_FETCHLEN               */
#define AES_FETCHLEN_CONSTADDR_DEFAULT                      (_AES_FETCHLEN_CONSTADDR_DEFAULT << 28) /**< Shifted mode DEFAULT for AES_FETCHLEN       */
#define AES_FETCHLEN_REALIGN                                (0x1UL << 29)                           /**< Realign lengh                               */
#define _AES_FETCHLEN_REALIGN_SHIFT                         29                                      /**< Shift value for AES_REALIGN                 */
#define _AES_FETCHLEN_REALIGN_MASK                          0x20000000UL                            /**< Bit mask for AES_REALIGN                    */
#define _AES_FETCHLEN_REALIGN_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for AES_FETCHLEN               */
#define AES_FETCHLEN_REALIGN_DEFAULT                        (_AES_FETCHLEN_REALIGN_DEFAULT << 29)   /**< Shifted mode DEFAULT for AES_FETCHLEN       */

/* Bit fields for AES FETCHTAG */
#define _AES_FETCHTAG_RESETVALUE                            0x00000000UL                     /**< Default value for AES_FETCHTAG              */
#define _AES_FETCHTAG_MASK                                  0xFFFFFFFFUL                     /**< Mask for AES_FETCHTAG                       */
#define _AES_FETCHTAG_TAG_SHIFT                             0                                /**< Shift value for AES_TAG                     */
#define _AES_FETCHTAG_TAG_MASK                              0xFFFFFFFFUL                     /**< Bit mask for AES_TAG                        */
#define _AES_FETCHTAG_TAG_DEFAULT                           0x00000000UL                     /**< Mode DEFAULT for AES_FETCHTAG               */
#define AES_FETCHTAG_TAG_DEFAULT                            (_AES_FETCHTAG_TAG_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_FETCHTAG       */

/* Bit fields for AES PUSHADDR */
#define _AES_PUSHADDR_RESETVALUE                            0x00000000UL                      /**< Default value for AES_PUSHADDR              */
#define _AES_PUSHADDR_MASK                                  0xFFFFFFFFUL                      /**< Mask for AES_PUSHADDR                       */
#define _AES_PUSHADDR_ADDR_SHIFT                            0                                 /**< Shift value for AES_ADDR                    */
#define _AES_PUSHADDR_ADDR_MASK                             0xFFFFFFFFUL                      /**< Bit mask for AES_ADDR                       */
#define _AES_PUSHADDR_ADDR_DEFAULT                          0x00000000UL                      /**< Mode DEFAULT for AES_PUSHADDR               */
#define AES_PUSHADDR_ADDR_DEFAULT                           (_AES_PUSHADDR_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_PUSHADDR       */

/* Bit fields for AES PUSHLEN */
#define _AES_PUSHLEN_RESETVALUE                             0x00000000UL                           /**< Default value for AES_PUSHLEN               */
#define _AES_PUSHLEN_MASK                                   0x7FFFFFFFUL                           /**< Mask for AES_PUSHLEN                        */
#define _AES_PUSHLEN_LENGTH_SHIFT                           0                                      /**< Shift value for AES_LENGTH                  */
#define _AES_PUSHLEN_LENGTH_MASK                            0xFFFFFFFUL                            /**< Bit mask for AES_LENGTH                     */
#define _AES_PUSHLEN_LENGTH_DEFAULT                         0x00000000UL                           /**< Mode DEFAULT for AES_PUSHLEN                */
#define AES_PUSHLEN_LENGTH_DEFAULT                          (_AES_PUSHLEN_LENGTH_DEFAULT << 0)     /**< Shifted mode DEFAULT for AES_PUSHLEN        */
#define AES_PUSHLEN_CONSTADDR                               (0x1UL << 28)                          /**< Constant address                            */
#define _AES_PUSHLEN_CONSTADDR_SHIFT                        28                                     /**< Shift value for AES_CONSTADDR               */
#define _AES_PUSHLEN_CONSTADDR_MASK                         0x10000000UL                           /**< Bit mask for AES_CONSTADDR                  */
#define _AES_PUSHLEN_CONSTADDR_DEFAULT                      0x00000000UL                           /**< Mode DEFAULT for AES_PUSHLEN                */
#define AES_PUSHLEN_CONSTADDR_DEFAULT                       (_AES_PUSHLEN_CONSTADDR_DEFAULT << 28) /**< Shifted mode DEFAULT for AES_PUSHLEN        */
#define AES_PUSHLEN_REALIGN                                 (0x1UL << 29)                          /**< Realign length                              */
#define _AES_PUSHLEN_REALIGN_SHIFT                          29                                     /**< Shift value for AES_REALIGN                 */
#define _AES_PUSHLEN_REALIGN_MASK                           0x20000000UL                           /**< Bit mask for AES_REALIGN                    */
#define _AES_PUSHLEN_REALIGN_DEFAULT                        0x00000000UL                           /**< Mode DEFAULT for AES_PUSHLEN                */
#define AES_PUSHLEN_REALIGN_DEFAULT                         (_AES_PUSHLEN_REALIGN_DEFAULT << 29)   /**< Shifted mode DEFAULT for AES_PUSHLEN        */
#define AES_PUSHLEN_DISCARD                                 (0x1UL << 30)                          /**< Discard data                                */
#define _AES_PUSHLEN_DISCARD_SHIFT                          30                                     /**< Shift value for AES_DISCARD                 */
#define _AES_PUSHLEN_DISCARD_MASK                           0x40000000UL                           /**< Bit mask for AES_DISCARD                    */
#define _AES_PUSHLEN_DISCARD_DEFAULT                        0x00000000UL                           /**< Mode DEFAULT for AES_PUSHLEN                */
#define AES_PUSHLEN_DISCARD_DEFAULT                         (_AES_PUSHLEN_DISCARD_DEFAULT << 30)   /**< Shifted mode DEFAULT for AES_PUSHLEN        */

/* Bit fields for AES IEN */
#define _AES_IEN_RESETVALUE                                 0x00000000UL                              /**< Default value for AES_IEN                   */
#define _AES_IEN_MASK                                       0x0000003FUL                              /**< Mask for AES_IEN                            */
#define AES_IEN_FETCHERENDOFBLOCK                           (0x1UL << 0)                              /**< End of block interrupt enable               */
#define _AES_IEN_FETCHERENDOFBLOCK_SHIFT                    0                                         /**< Shift value for AES_FETCHERENDOFBLOCK       */
#define _AES_IEN_FETCHERENDOFBLOCK_MASK                     0x1UL                                     /**< Bit mask for AES_FETCHERENDOFBLOCK          */
#define _AES_IEN_FETCHERENDOFBLOCK_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for AES_IEN                    */
#define AES_IEN_FETCHERENDOFBLOCK_DEFAULT                   (_AES_IEN_FETCHERENDOFBLOCK_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_IEN            */
#define AES_IEN_FETCHERSTOPPED                              (0x1UL << 1)                              /**< Stopped interrupt enable                    */
#define _AES_IEN_FETCHERSTOPPED_SHIFT                       1                                         /**< Shift value for AES_FETCHERSTOPPED          */
#define _AES_IEN_FETCHERSTOPPED_MASK                        0x2UL                                     /**< Bit mask for AES_FETCHERSTOPPED             */
#define _AES_IEN_FETCHERSTOPPED_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for AES_IEN                    */
#define AES_IEN_FETCHERSTOPPED_DEFAULT                      (_AES_IEN_FETCHERSTOPPED_DEFAULT << 1)    /**< Shifted mode DEFAULT for AES_IEN            */
#define AES_IEN_FETCHERERROR                                (0x1UL << 2)                              /**< Error interrupt enable                      */
#define _AES_IEN_FETCHERERROR_SHIFT                         2                                         /**< Shift value for AES_FETCHERERROR            */
#define _AES_IEN_FETCHERERROR_MASK                          0x4UL                                     /**< Bit mask for AES_FETCHERERROR               */
#define _AES_IEN_FETCHERERROR_DEFAULT                       0x00000000UL                              /**< Mode DEFAULT for AES_IEN                    */
#define AES_IEN_FETCHERERROR_DEFAULT                        (_AES_IEN_FETCHERERROR_DEFAULT << 2)      /**< Shifted mode DEFAULT for AES_IEN            */
#define AES_IEN_PUSHERENDOFBLOCK                            (0x1UL << 3)                              /**< End of block interrupt enable               */
#define _AES_IEN_PUSHERENDOFBLOCK_SHIFT                     3                                         /**< Shift value for AES_PUSHERENDOFBLOCK        */
#define _AES_IEN_PUSHERENDOFBLOCK_MASK                      0x8UL                                     /**< Bit mask for AES_PUSHERENDOFBLOCK           */
#define _AES_IEN_PUSHERENDOFBLOCK_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for AES_IEN                    */
#define AES_IEN_PUSHERENDOFBLOCK_DEFAULT                    (_AES_IEN_PUSHERENDOFBLOCK_DEFAULT << 3)  /**< Shifted mode DEFAULT for AES_IEN            */
#define AES_IEN_PUSHERSTOPPED                               (0x1UL << 4)                              /**< Stopped interrupt enable                    */
#define _AES_IEN_PUSHERSTOPPED_SHIFT                        4                                         /**< Shift value for AES_PUSHERSTOPPED           */
#define _AES_IEN_PUSHERSTOPPED_MASK                         0x10UL                                    /**< Bit mask for AES_PUSHERSTOPPED              */
#define _AES_IEN_PUSHERSTOPPED_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for AES_IEN                    */
#define AES_IEN_PUSHERSTOPPED_DEFAULT                       (_AES_IEN_PUSHERSTOPPED_DEFAULT << 4)     /**< Shifted mode DEFAULT for AES_IEN            */
#define AES_IEN_PUSHERERROR                                 (0x1UL << 5)                              /**< Error interrupt enable                      */
#define _AES_IEN_PUSHERERROR_SHIFT                          5                                         /**< Shift value for AES_PUSHERERROR             */
#define _AES_IEN_PUSHERERROR_MASK                           0x20UL                                    /**< Bit mask for AES_PUSHERERROR                */
#define _AES_IEN_PUSHERERROR_DEFAULT                        0x00000000UL                              /**< Mode DEFAULT for AES_IEN                    */
#define AES_IEN_PUSHERERROR_DEFAULT                         (_AES_IEN_PUSHERERROR_DEFAULT << 5)       /**< Shifted mode DEFAULT for AES_IEN            */

/* Bit fields for AES IF */
#define _AES_IF_RESETVALUE                                  0x00000000UL                             /**< Default value for AES_IF                    */
#define _AES_IF_MASK                                        0x0000003FUL                             /**< Mask for AES_IF                             */
#define AES_IF_FETCHERENDOFBLOCK                            (0x1UL << 0)                             /**< End of block interrupt flag                 */
#define _AES_IF_FETCHERENDOFBLOCK_SHIFT                     0                                        /**< Shift value for AES_FETCHERENDOFBLOCK       */
#define _AES_IF_FETCHERENDOFBLOCK_MASK                      0x1UL                                    /**< Bit mask for AES_FETCHERENDOFBLOCK          */
#define _AES_IF_FETCHERENDOFBLOCK_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for AES_IF                     */
#define AES_IF_FETCHERENDOFBLOCK_DEFAULT                    (_AES_IF_FETCHERENDOFBLOCK_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_IF             */
#define AES_IF_FETCHERSTOPPED                               (0x1UL << 1)                             /**< Stopped interrupt flag                      */
#define _AES_IF_FETCHERSTOPPED_SHIFT                        1                                        /**< Shift value for AES_FETCHERSTOPPED          */
#define _AES_IF_FETCHERSTOPPED_MASK                         0x2UL                                    /**< Bit mask for AES_FETCHERSTOPPED             */
#define _AES_IF_FETCHERSTOPPED_DEFAULT                      0x00000000UL                             /**< Mode DEFAULT for AES_IF                     */
#define AES_IF_FETCHERSTOPPED_DEFAULT                       (_AES_IF_FETCHERSTOPPED_DEFAULT << 1)    /**< Shifted mode DEFAULT for AES_IF             */
#define AES_IF_FETCHERERROR                                 (0x1UL << 2)                             /**< Error interrupt flag                        */
#define _AES_IF_FETCHERERROR_SHIFT                          2                                        /**< Shift value for AES_FETCHERERROR            */
#define _AES_IF_FETCHERERROR_MASK                           0x4UL                                    /**< Bit mask for AES_FETCHERERROR               */
#define _AES_IF_FETCHERERROR_DEFAULT                        0x00000000UL                             /**< Mode DEFAULT for AES_IF                     */
#define AES_IF_FETCHERERROR_DEFAULT                         (_AES_IF_FETCHERERROR_DEFAULT << 2)      /**< Shifted mode DEFAULT for AES_IF             */
#define AES_IF_PUSHERENDOFBLOCK                             (0x1UL << 3)                             /**< End of block interrupt flag                 */
#define _AES_IF_PUSHERENDOFBLOCK_SHIFT                      3                                        /**< Shift value for AES_PUSHERENDOFBLOCK        */
#define _AES_IF_PUSHERENDOFBLOCK_MASK                       0x8UL                                    /**< Bit mask for AES_PUSHERENDOFBLOCK           */
#define _AES_IF_PUSHERENDOFBLOCK_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for AES_IF                     */
#define AES_IF_PUSHERENDOFBLOCK_DEFAULT                     (_AES_IF_PUSHERENDOFBLOCK_DEFAULT << 3)  /**< Shifted mode DEFAULT for AES_IF             */
#define AES_IF_PUSHERSTOPPED                                (0x1UL << 4)                             /**< Stopped interrupt flag                      */
#define _AES_IF_PUSHERSTOPPED_SHIFT                         4                                        /**< Shift value for AES_PUSHERSTOPPED           */
#define _AES_IF_PUSHERSTOPPED_MASK                          0x10UL                                   /**< Bit mask for AES_PUSHERSTOPPED              */
#define _AES_IF_PUSHERSTOPPED_DEFAULT                       0x00000000UL                             /**< Mode DEFAULT for AES_IF                     */
#define AES_IF_PUSHERSTOPPED_DEFAULT                        (_AES_IF_PUSHERSTOPPED_DEFAULT << 4)     /**< Shifted mode DEFAULT for AES_IF             */
#define AES_IF_PUSHERERROR                                  (0x1UL << 5)                             /**< Error interrupt flag                        */
#define _AES_IF_PUSHERERROR_SHIFT                           5                                        /**< Shift value for AES_PUSHERERROR             */
#define _AES_IF_PUSHERERROR_MASK                            0x20UL                                   /**< Bit mask for AES_PUSHERERROR                */
#define _AES_IF_PUSHERERROR_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for AES_IF                     */
#define AES_IF_PUSHERERROR_DEFAULT                          (_AES_IF_PUSHERERROR_DEFAULT << 5)       /**< Shifted mode DEFAULT for AES_IF             */

/* Bit fields for AES IF_CLR */
#define _AES_IF_CLR_RESETVALUE                              0x00000000UL                                 /**< Default value for AES_IF_CLR                */
#define _AES_IF_CLR_MASK                                    0x0000003FUL                                 /**< Mask for AES_IF_CLR                         */
#define AES_IF_CLR_FETCHERENDOFBLOCK                        (0x1UL << 0)                                 /**< End of block interrupt flag clear           */
#define _AES_IF_CLR_FETCHERENDOFBLOCK_SHIFT                 0                                            /**< Shift value for AES_FETCHERENDOFBLOCK       */
#define _AES_IF_CLR_FETCHERENDOFBLOCK_MASK                  0x1UL                                        /**< Bit mask for AES_FETCHERENDOFBLOCK          */
#define _AES_IF_CLR_FETCHERENDOFBLOCK_DEFAULT               0x00000000UL                                 /**< Mode DEFAULT for AES_IF_CLR                 */
#define AES_IF_CLR_FETCHERENDOFBLOCK_DEFAULT                (_AES_IF_CLR_FETCHERENDOFBLOCK_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_IF_CLR         */
#define AES_IF_CLR_FETCHERSTOPPED                           (0x1UL << 1)                                 /**< Stopped interrupt flag clear                */
#define _AES_IF_CLR_FETCHERSTOPPED_SHIFT                    1                                            /**< Shift value for AES_FETCHERSTOPPED          */
#define _AES_IF_CLR_FETCHERSTOPPED_MASK                     0x2UL                                        /**< Bit mask for AES_FETCHERSTOPPED             */
#define _AES_IF_CLR_FETCHERSTOPPED_DEFAULT                  0x00000000UL                                 /**< Mode DEFAULT for AES_IF_CLR                 */
#define AES_IF_CLR_FETCHERSTOPPED_DEFAULT                   (_AES_IF_CLR_FETCHERSTOPPED_DEFAULT << 1)    /**< Shifted mode DEFAULT for AES_IF_CLR         */
#define AES_IF_CLR_FETCHERERROR                             (0x1UL << 2)                                 /**< Error interrupt flag clear                  */
#define _AES_IF_CLR_FETCHERERROR_SHIFT                      2                                            /**< Shift value for AES_FETCHERERROR            */
#define _AES_IF_CLR_FETCHERERROR_MASK                       0x4UL                                        /**< Bit mask for AES_FETCHERERROR               */
#define _AES_IF_CLR_FETCHERERROR_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for AES_IF_CLR                 */
#define AES_IF_CLR_FETCHERERROR_DEFAULT                     (_AES_IF_CLR_FETCHERERROR_DEFAULT << 2)      /**< Shifted mode DEFAULT for AES_IF_CLR         */
#define AES_IF_CLR_PUSHERENDOFBLOCK                         (0x1UL << 3)                                 /**< FETCHERENDOFBLOCKIFC                        */
#define _AES_IF_CLR_PUSHERENDOFBLOCK_SHIFT                  3                                            /**< Shift value for AES_PUSHERENDOFBLOCK        */
#define _AES_IF_CLR_PUSHERENDOFBLOCK_MASK                   0x8UL                                        /**< Bit mask for AES_PUSHERENDOFBLOCK           */
#define _AES_IF_CLR_PUSHERENDOFBLOCK_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for AES_IF_CLR                 */
#define AES_IF_CLR_PUSHERENDOFBLOCK_DEFAULT                 (_AES_IF_CLR_PUSHERENDOFBLOCK_DEFAULT << 3)  /**< Shifted mode DEFAULT for AES_IF_CLR         */
#define AES_IF_CLR_PUSHERSTOPPED                            (0x1UL << 4)                                 /**< FETCHERSTOPPEDIFC                           */
#define _AES_IF_CLR_PUSHERSTOPPED_SHIFT                     4                                            /**< Shift value for AES_PUSHERSTOPPED           */
#define _AES_IF_CLR_PUSHERSTOPPED_MASK                      0x10UL                                       /**< Bit mask for AES_PUSHERSTOPPED              */
#define _AES_IF_CLR_PUSHERSTOPPED_DEFAULT                   0x00000000UL                                 /**< Mode DEFAULT for AES_IF_CLR                 */
#define AES_IF_CLR_PUSHERSTOPPED_DEFAULT                    (_AES_IF_CLR_PUSHERSTOPPED_DEFAULT << 4)     /**< Shifted mode DEFAULT for AES_IF_CLR         */
#define AES_IF_CLR_PUSHERERROR                              (0x1UL << 5)                                 /**< FETCHERERRORIFC                             */
#define _AES_IF_CLR_PUSHERERROR_SHIFT                       5                                            /**< Shift value for AES_PUSHERERROR             */
#define _AES_IF_CLR_PUSHERERROR_MASK                        0x20UL                                       /**< Bit mask for AES_PUSHERERROR                */
#define _AES_IF_CLR_PUSHERERROR_DEFAULT                     0x00000000UL                                 /**< Mode DEFAULT for AES_IF_CLR                 */
#define AES_IF_CLR_PUSHERERROR_DEFAULT                      (_AES_IF_CLR_PUSHERERROR_DEFAULT << 5)       /**< Shifted mode DEFAULT for AES_IF_CLR         */

/* Bit fields for AES CTRL */
#define _AES_CTRL_RESETVALUE                                0x00000000UL                                  /**< Default value for AES_CTRL                  */
#define _AES_CTRL_MASK                                      0x0000001FUL                                  /**< Mask for AES_CTRL                           */
#define AES_CTRL_FETCHERSCATTERGATHER                       (0x1UL << 0)                                  /**< Fetcher scatter/gather                      */
#define _AES_CTRL_FETCHERSCATTERGATHER_SHIFT                0                                             /**< Shift value for AES_FETCHERSCATTERGATHER    */
#define _AES_CTRL_FETCHERSCATTERGATHER_MASK                 0x1UL                                         /**< Bit mask for AES_FETCHERSCATTERGATHER       */
#define _AES_CTRL_FETCHERSCATTERGATHER_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for AES_CTRL                   */
#define AES_CTRL_FETCHERSCATTERGATHER_DEFAULT               (_AES_CTRL_FETCHERSCATTERGATHER_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_CTRL           */
#define AES_CTRL_PUSHERSCATTERGATHER                        (0x1UL << 1)                                  /**< Pusher scatter/gather                       */
#define _AES_CTRL_PUSHERSCATTERGATHER_SHIFT                 1                                             /**< Shift value for AES_PUSHERSCATTERGATHER     */
#define _AES_CTRL_PUSHERSCATTERGATHER_MASK                  0x2UL                                         /**< Bit mask for AES_PUSHERSCATTERGATHER        */
#define _AES_CTRL_PUSHERSCATTERGATHER_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for AES_CTRL                   */
#define AES_CTRL_PUSHERSCATTERGATHER_DEFAULT                (_AES_CTRL_PUSHERSCATTERGATHER_DEFAULT << 1)  /**< Shifted mode DEFAULT for AES_CTRL           */
#define AES_CTRL_STOPFETCHER                                (0x1UL << 2)                                  /**< Stop fetcher                                */
#define _AES_CTRL_STOPFETCHER_SHIFT                         2                                             /**< Shift value for AES_STOPFETCHER             */
#define _AES_CTRL_STOPFETCHER_MASK                          0x4UL                                         /**< Bit mask for AES_STOPFETCHER                */
#define _AES_CTRL_STOPFETCHER_DEFAULT                       0x00000000UL                                  /**< Mode DEFAULT for AES_CTRL                   */
#define AES_CTRL_STOPFETCHER_DEFAULT                        (_AES_CTRL_STOPFETCHER_DEFAULT << 2)          /**< Shifted mode DEFAULT for AES_CTRL           */
#define AES_CTRL_STOPPUSHER                                 (0x1UL << 3)                                  /**< Stop pusher                                 */
#define _AES_CTRL_STOPPUSHER_SHIFT                          3                                             /**< Shift value for AES_STOPPUSHER              */
#define _AES_CTRL_STOPPUSHER_MASK                           0x8UL                                         /**< Bit mask for AES_STOPPUSHER                 */
#define _AES_CTRL_STOPPUSHER_DEFAULT                        0x00000000UL                                  /**< Mode DEFAULT for AES_CTRL                   */
#define AES_CTRL_STOPPUSHER_DEFAULT                         (_AES_CTRL_STOPPUSHER_DEFAULT << 3)           /**< Shifted mode DEFAULT for AES_CTRL           */
#define AES_CTRL_SWRESET                                    (0x1UL << 4)                                  /**< Software reset                              */
#define _AES_CTRL_SWRESET_SHIFT                             4                                             /**< Shift value for AES_SWRESET                 */
#define _AES_CTRL_SWRESET_MASK                              0x10UL                                        /**< Bit mask for AES_SWRESET                    */
#define _AES_CTRL_SWRESET_DEFAULT                           0x00000000UL                                  /**< Mode DEFAULT for AES_CTRL                   */
#define AES_CTRL_SWRESET_DEFAULT                            (_AES_CTRL_SWRESET_DEFAULT << 4)              /**< Shifted mode DEFAULT for AES_CTRL           */

/* Bit fields for AES CMD */
#define _AES_CMD_RESETVALUE                                 0x00000000UL                         /**< Default value for AES_CMD                   */
#define _AES_CMD_MASK                                       0x00000003UL                         /**< Mask for AES_CMD                            */
#define AES_CMD_STARTFETCHER                                (0x1UL << 0)                         /**< Start fetch                                 */
#define _AES_CMD_STARTFETCHER_SHIFT                         0                                    /**< Shift value for AES_STARTFETCHER            */
#define _AES_CMD_STARTFETCHER_MASK                          0x1UL                                /**< Bit mask for AES_STARTFETCHER               */
#define _AES_CMD_STARTFETCHER_DEFAULT                       0x00000000UL                         /**< Mode DEFAULT for AES_CMD                    */
#define AES_CMD_STARTFETCHER_DEFAULT                        (_AES_CMD_STARTFETCHER_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_CMD            */
#define AES_CMD_STARTPUSHER                                 (0x1UL << 1)                         /**< Start push                                  */
#define _AES_CMD_STARTPUSHER_SHIFT                          1                                    /**< Shift value for AES_STARTPUSHER             */
#define _AES_CMD_STARTPUSHER_MASK                           0x2UL                                /**< Bit mask for AES_STARTPUSHER                */
#define _AES_CMD_STARTPUSHER_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for AES_CMD                    */
#define AES_CMD_STARTPUSHER_DEFAULT                         (_AES_CMD_STARTPUSHER_DEFAULT << 1)  /**< Shifted mode DEFAULT for AES_CMD            */

/* Bit fields for AES STATUS */
#define _AES_STATUS_RESETVALUE                              0x00000000UL                            /**< Default value for AES_STATUS                */
#define _AES_STATUS_MASK                                    0xFFFF0073UL                            /**< Mask for AES_STATUS                         */
#define AES_STATUS_FETCHERBSY                               (0x1UL << 0)                            /**< Fetcher busy                                */
#define _AES_STATUS_FETCHERBSY_SHIFT                        0                                       /**< Shift value for AES_FETCHERBSY              */
#define _AES_STATUS_FETCHERBSY_MASK                         0x1UL                                   /**< Bit mask for AES_FETCHERBSY                 */
#define _AES_STATUS_FETCHERBSY_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for AES_STATUS                 */
#define AES_STATUS_FETCHERBSY_DEFAULT                       (_AES_STATUS_FETCHERBSY_DEFAULT << 0)   /**< Shifted mode DEFAULT for AES_STATUS         */
#define AES_STATUS_PUSHERBSY                                (0x1UL << 1)                            /**< Pusher busy                                 */
#define _AES_STATUS_PUSHERBSY_SHIFT                         1                                       /**< Shift value for AES_PUSHERBSY               */
#define _AES_STATUS_PUSHERBSY_MASK                          0x2UL                                   /**< Bit mask for AES_PUSHERBSY                  */
#define _AES_STATUS_PUSHERBSY_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for AES_STATUS                 */
#define AES_STATUS_PUSHERBSY_DEFAULT                        (_AES_STATUS_PUSHERBSY_DEFAULT << 1)    /**< Shifted mode DEFAULT for AES_STATUS         */
#define AES_STATUS_NOTEMPTY                                 (0x1UL << 4)                            /**< Not empty flag from input FIFO (fetcher)    */
#define _AES_STATUS_NOTEMPTY_SHIFT                          4                                       /**< Shift value for AES_NOTEMPTY                */
#define _AES_STATUS_NOTEMPTY_MASK                           0x10UL                                  /**< Bit mask for AES_NOTEMPTY                   */
#define _AES_STATUS_NOTEMPTY_DEFAULT                        0x00000000UL                            /**< Mode DEFAULT for AES_STATUS                 */
#define AES_STATUS_NOTEMPTY_DEFAULT                         (_AES_STATUS_NOTEMPTY_DEFAULT << 4)     /**< Shifted mode DEFAULT for AES_STATUS         */
#define AES_STATUS_WAITING                                  (0x1UL << 5)                            /**< Pusher waiting for FIFO                     */
#define _AES_STATUS_WAITING_SHIFT                           5                                       /**< Shift value for AES_WAITING                 */
#define _AES_STATUS_WAITING_MASK                            0x20UL                                  /**< Bit mask for AES_WAITING                    */
#define _AES_STATUS_WAITING_DEFAULT                         0x00000000UL                            /**< Mode DEFAULT for AES_STATUS                 */
#define AES_STATUS_WAITING_DEFAULT                          (_AES_STATUS_WAITING_DEFAULT << 5)      /**< Shifted mode DEFAULT for AES_STATUS         */
#define AES_STATUS_SOFTRSTBSY                               (0x1UL << 6)                            /**< Software reset busy                         */
#define _AES_STATUS_SOFTRSTBSY_SHIFT                        6                                       /**< Shift value for AES_SOFTRSTBSY              */
#define _AES_STATUS_SOFTRSTBSY_MASK                         0x40UL                                  /**< Bit mask for AES_SOFTRSTBSY                 */
#define _AES_STATUS_SOFTRSTBSY_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for AES_STATUS                 */
#define AES_STATUS_SOFTRSTBSY_DEFAULT                       (_AES_STATUS_SOFTRSTBSY_DEFAULT << 6)   /**< Shifted mode DEFAULT for AES_STATUS         */
#define _AES_STATUS_FIFODATANUM_SHIFT                       16                                      /**< Shift value for AES_FIFODATANUM             */
#define _AES_STATUS_FIFODATANUM_MASK                        0xFFFF0000UL                            /**< Bit mask for AES_FIFODATANUM                */
#define _AES_STATUS_FIFODATANUM_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for AES_STATUS                 */
#define AES_STATUS_FIFODATANUM_DEFAULT                      (_AES_STATUS_FIFODATANUM_DEFAULT << 16) /**< Shifted mode DEFAULT for AES_STATUS         */

/* Bit fields for AES INCL_IPS_HW_CFG */
#define _AES_INCL_IPS_HW_CFG_RESETVALUE                     0x00000001UL                                            /**< Default value for AES_INCL_IPS_HW_CFG       */
#define _AES_INCL_IPS_HW_CFG_MASK                           0x000007FFUL                                            /**< Mask for AES_INCL_IPS_HW_CFG                */
#define AES_INCL_IPS_HW_CFG_g_IncludeAES                    (0x1UL << 0)                                            /**< Generic g_IncludeAES value                  */
#define _AES_INCL_IPS_HW_CFG_g_IncludeAES_SHIFT             0                                                       /**< Shift value for AES_g_IncludeAES            */
#define _AES_INCL_IPS_HW_CFG_g_IncludeAES_MASK              0x1UL                                                   /**< Bit mask for AES_g_IncludeAES               */
#define _AES_INCL_IPS_HW_CFG_g_IncludeAES_DEFAULT           0x00000001UL                                            /**< Mode DEFAULT for AES_INCL_IPS_HW_CFG        */
#define AES_INCL_IPS_HW_CFG_g_IncludeAES_DEFAULT            (_AES_INCL_IPS_HW_CFG_g_IncludeAES_DEFAULT << 0)        /**< Shifted mode DEFAULT for AES_INCL_IPS_HW_CFG*/
#define AES_INCL_IPS_HW_CFG_g_IncludeAESGCM                 (0x1UL << 1)                                            /**< Generic g_IncludeAESGCM value               */
#define _AES_INCL_IPS_HW_CFG_g_IncludeAESGCM_SHIFT          1                                                       /**< Shift value for AES_g_IncludeAESGCM         */
#define _AES_INCL_IPS_HW_CFG_g_IncludeAESGCM_MASK           0x2UL                                                   /**< Bit mask for AES_g_IncludeAESGCM            */
#define _AES_INCL_IPS_HW_CFG_g_IncludeAESGCM_DEFAULT        0x00000000UL                                            /**< Mode DEFAULT for AES_INCL_IPS_HW_CFG        */
#define AES_INCL_IPS_HW_CFG_g_IncludeAESGCM_DEFAULT         (_AES_INCL_IPS_HW_CFG_g_IncludeAESGCM_DEFAULT << 1)     /**< Shifted mode DEFAULT for AES_INCL_IPS_HW_CFG*/
#define AES_INCL_IPS_HW_CFG_g_IncludeAESXTS                 (0x1UL << 2)                                            /**< Generic g_IncludeAESXTS value               */
#define _AES_INCL_IPS_HW_CFG_g_IncludeAESXTS_SHIFT          2                                                       /**< Shift value for AES_g_IncludeAESXTS         */
#define _AES_INCL_IPS_HW_CFG_g_IncludeAESXTS_MASK           0x4UL                                                   /**< Bit mask for AES_g_IncludeAESXTS            */
#define _AES_INCL_IPS_HW_CFG_g_IncludeAESXTS_DEFAULT        0x00000000UL                                            /**< Mode DEFAULT for AES_INCL_IPS_HW_CFG        */
#define AES_INCL_IPS_HW_CFG_g_IncludeAESXTS_DEFAULT         (_AES_INCL_IPS_HW_CFG_g_IncludeAESXTS_DEFAULT << 2)     /**< Shifted mode DEFAULT for AES_INCL_IPS_HW_CFG*/
#define AES_INCL_IPS_HW_CFG_g_IncludeDES                    (0x1UL << 3)                                            /**< Generic g_IncludeDES value                  */
#define _AES_INCL_IPS_HW_CFG_g_IncludeDES_SHIFT             3                                                       /**< Shift value for AES_g_IncludeDES            */
#define _AES_INCL_IPS_HW_CFG_g_IncludeDES_MASK              0x8UL                                                   /**< Bit mask for AES_g_IncludeDES               */
#define _AES_INCL_IPS_HW_CFG_g_IncludeDES_DEFAULT           0x00000000UL                                            /**< Mode DEFAULT for AES_INCL_IPS_HW_CFG        */
#define AES_INCL_IPS_HW_CFG_g_IncludeDES_DEFAULT            (_AES_INCL_IPS_HW_CFG_g_IncludeDES_DEFAULT << 3)        /**< Shifted mode DEFAULT for AES_INCL_IPS_HW_CFG*/
#define AES_INCL_IPS_HW_CFG_g_IncludeHASH                   (0x1UL << 4)                                            /**< Generic g_IncludeHASH value                 */
#define _AES_INCL_IPS_HW_CFG_g_IncludeHASH_SHIFT            4                                                       /**< Shift value for AES_g_IncludeHASH           */
#define _AES_INCL_IPS_HW_CFG_g_IncludeHASH_MASK             0x10UL                                                  /**< Bit mask for AES_g_IncludeHASH              */
#define _AES_INCL_IPS_HW_CFG_g_IncludeHASH_DEFAULT          0x00000000UL                                            /**< Mode DEFAULT for AES_INCL_IPS_HW_CFG        */
#define AES_INCL_IPS_HW_CFG_g_IncludeHASH_DEFAULT           (_AES_INCL_IPS_HW_CFG_g_IncludeHASH_DEFAULT << 4)       /**< Shifted mode DEFAULT for AES_INCL_IPS_HW_CFG*/
#define AES_INCL_IPS_HW_CFG_g_IncludeChachaPoly             (0x1UL << 5)                                            /**< Generic g_IncludeChachaPoly value           */
#define _AES_INCL_IPS_HW_CFG_g_IncludeChachaPoly_SHIFT      5                                                       /**< Shift value for AES_g_IncludeChachaPoly     */
#define _AES_INCL_IPS_HW_CFG_g_IncludeChachaPoly_MASK       0x20UL                                                  /**< Bit mask for AES_g_IncludeChachaPoly        */
#define _AES_INCL_IPS_HW_CFG_g_IncludeChachaPoly_DEFAULT    0x00000000UL                                            /**< Mode DEFAULT for AES_INCL_IPS_HW_CFG        */
#define AES_INCL_IPS_HW_CFG_g_IncludeChachaPoly_DEFAULT     (_AES_INCL_IPS_HW_CFG_g_IncludeChachaPoly_DEFAULT << 5) /**< Shifted mode DEFAULT for AES_INCL_IPS_HW_CFG*/
#define AES_INCL_IPS_HW_CFG_g_IncludeSHA3                   (0x1UL << 6)                                            /**< Generic g_IncludeSHA3 value                 */
#define _AES_INCL_IPS_HW_CFG_g_IncludeSHA3_SHIFT            6                                                       /**< Shift value for AES_g_IncludeSHA3           */
#define _AES_INCL_IPS_HW_CFG_g_IncludeSHA3_MASK             0x40UL                                                  /**< Bit mask for AES_g_IncludeSHA3              */
#define _AES_INCL_IPS_HW_CFG_g_IncludeSHA3_DEFAULT          0x00000000UL                                            /**< Mode DEFAULT for AES_INCL_IPS_HW_CFG        */
#define AES_INCL_IPS_HW_CFG_g_IncludeSHA3_DEFAULT           (_AES_INCL_IPS_HW_CFG_g_IncludeSHA3_DEFAULT << 6)       /**< Shifted mode DEFAULT for AES_INCL_IPS_HW_CFG*/
#define AES_INCL_IPS_HW_CFG_g_IncludeZUC                    (0x1UL << 7)                                            /**< Generic g_IncludeZUC value                  */
#define _AES_INCL_IPS_HW_CFG_g_IncludeZUC_SHIFT             7                                                       /**< Shift value for AES_g_IncludeZUC            */
#define _AES_INCL_IPS_HW_CFG_g_IncludeZUC_MASK              0x80UL                                                  /**< Bit mask for AES_g_IncludeZUC               */
#define _AES_INCL_IPS_HW_CFG_g_IncludeZUC_DEFAULT           0x00000000UL                                            /**< Mode DEFAULT for AES_INCL_IPS_HW_CFG        */
#define AES_INCL_IPS_HW_CFG_g_IncludeZUC_DEFAULT            (_AES_INCL_IPS_HW_CFG_g_IncludeZUC_DEFAULT << 7)        /**< Shifted mode DEFAULT for AES_INCL_IPS_HW_CFG*/
#define AES_INCL_IPS_HW_CFG_g_IncludeSM4                    (0x1UL << 8)                                            /**< Generic g_IncludeSM4 value                  */
#define _AES_INCL_IPS_HW_CFG_g_IncludeSM4_SHIFT             8                                                       /**< Shift value for AES_g_IncludeSM4            */
#define _AES_INCL_IPS_HW_CFG_g_IncludeSM4_MASK              0x100UL                                                 /**< Bit mask for AES_g_IncludeSM4               */
#define _AES_INCL_IPS_HW_CFG_g_IncludeSM4_DEFAULT           0x00000000UL                                            /**< Mode DEFAULT for AES_INCL_IPS_HW_CFG        */
#define AES_INCL_IPS_HW_CFG_g_IncludeSM4_DEFAULT            (_AES_INCL_IPS_HW_CFG_g_IncludeSM4_DEFAULT << 8)        /**< Shifted mode DEFAULT for AES_INCL_IPS_HW_CFG*/
#define AES_INCL_IPS_HW_CFG_g_IncludePKE                    (0x1UL << 9)                                            /**< Generic g_IncludePKE value                  */
#define _AES_INCL_IPS_HW_CFG_g_IncludePKE_SHIFT             9                                                       /**< Shift value for AES_g_IncludePKE            */
#define _AES_INCL_IPS_HW_CFG_g_IncludePKE_MASK              0x200UL                                                 /**< Bit mask for AES_g_IncludePKE               */
#define _AES_INCL_IPS_HW_CFG_g_IncludePKE_DEFAULT           0x00000000UL                                            /**< Mode DEFAULT for AES_INCL_IPS_HW_CFG        */
#define AES_INCL_IPS_HW_CFG_g_IncludePKE_DEFAULT            (_AES_INCL_IPS_HW_CFG_g_IncludePKE_DEFAULT << 9)        /**< Shifted mode DEFAULT for AES_INCL_IPS_HW_CFG*/
#define AES_INCL_IPS_HW_CFG_g_IncludeNDRNG                  (0x1UL << 10)                                           /**< Generic g_IncludeNDRNG value                */
#define _AES_INCL_IPS_HW_CFG_g_IncludeNDRNG_SHIFT           10                                                      /**< Shift value for AES_g_IncludeNDRNG          */
#define _AES_INCL_IPS_HW_CFG_g_IncludeNDRNG_MASK            0x400UL                                                 /**< Bit mask for AES_g_IncludeNDRNG             */
#define _AES_INCL_IPS_HW_CFG_g_IncludeNDRNG_DEFAULT         0x00000000UL                                            /**< Mode DEFAULT for AES_INCL_IPS_HW_CFG        */
#define AES_INCL_IPS_HW_CFG_g_IncludeNDRNG_DEFAULT          (_AES_INCL_IPS_HW_CFG_g_IncludeNDRNG_DEFAULT << 10)     /**< Shifted mode DEFAULT for AES_INCL_IPS_HW_CFG*/

/* Bit fields for AES BA411E_HW_CFG_1 */
#define _AES_BA411E_HW_CFG_1_RESETVALUE                     0x05010127UL                                       /**< Default value for AES_BA411E_HW_CFG_1       */
#define _AES_BA411E_HW_CFG_1_MASK                           0x070301FFUL                                       /**< Mask for AES_BA411E_HW_CFG_1                */
#define _AES_BA411E_HW_CFG_1_g_AesModesPoss_SHIFT           0                                                  /**< Shift value for AES_g_AesModesPoss          */
#define _AES_BA411E_HW_CFG_1_g_AesModesPoss_MASK            0x1FFUL                                            /**< Bit mask for AES_g_AesModesPoss             */
#define _AES_BA411E_HW_CFG_1_g_AesModesPoss_DEFAULT         0x00000127UL                                       /**< Mode DEFAULT for AES_BA411E_HW_CFG_1        */
#define AES_BA411E_HW_CFG_1_g_AesModesPoss_DEFAULT          (_AES_BA411E_HW_CFG_1_g_AesModesPoss_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_BA411E_HW_CFG_1*/
#define AES_BA411E_HW_CFG_1_g_CS                            (0x1UL << 16)                                      /**< Generic g_CS value                          */
#define _AES_BA411E_HW_CFG_1_g_CS_SHIFT                     16                                                 /**< Shift value for AES_g_CS                    */
#define _AES_BA411E_HW_CFG_1_g_CS_MASK                      0x10000UL                                          /**< Bit mask for AES_g_CS                       */
#define _AES_BA411E_HW_CFG_1_g_CS_DEFAULT                   0x00000001UL                                       /**< Mode DEFAULT for AES_BA411E_HW_CFG_1        */
#define AES_BA411E_HW_CFG_1_g_CS_DEFAULT                    (_AES_BA411E_HW_CFG_1_g_CS_DEFAULT << 16)          /**< Shifted mode DEFAULT for AES_BA411E_HW_CFG_1*/
#define AES_BA411E_HW_CFG_1_g_UseMasking                    (0x1UL << 17)                                      /**< Generic g_UseMasking value                  */
#define _AES_BA411E_HW_CFG_1_g_UseMasking_SHIFT             17                                                 /**< Shift value for AES_g_UseMasking            */
#define _AES_BA411E_HW_CFG_1_g_UseMasking_MASK              0x20000UL                                          /**< Bit mask for AES_g_UseMasking               */
#define _AES_BA411E_HW_CFG_1_g_UseMasking_DEFAULT           0x00000000UL                                       /**< Mode DEFAULT for AES_BA411E_HW_CFG_1        */
#define AES_BA411E_HW_CFG_1_g_UseMasking_DEFAULT            (_AES_BA411E_HW_CFG_1_g_UseMasking_DEFAULT << 17)  /**< Shifted mode DEFAULT for AES_BA411E_HW_CFG_1*/
#define _AES_BA411E_HW_CFG_1_g_Keysize_SHIFT                24                                                 /**< Shift value for AES_g_Keysize               */
#define _AES_BA411E_HW_CFG_1_g_Keysize_MASK                 0x7000000UL                                        /**< Bit mask for AES_g_Keysize                  */
#define _AES_BA411E_HW_CFG_1_g_Keysize_DEFAULT              0x00000005UL                                       /**< Mode DEFAULT for AES_BA411E_HW_CFG_1        */
#define AES_BA411E_HW_CFG_1_g_Keysize_DEFAULT               (_AES_BA411E_HW_CFG_1_g_Keysize_DEFAULT << 24)     /**< Shifted mode DEFAULT for AES_BA411E_HW_CFG_1*/

/* Bit fields for AES BA411E_HW_CFG_2 */
#define _AES_BA411E_HW_CFG_2_RESETVALUE                     0x00000080UL                                  /**< Default value for AES_BA411E_HW_CFG_2       */
#define _AES_BA411E_HW_CFG_2_MASK                           0x0000FFFFUL                                  /**< Mask for AES_BA411E_HW_CFG_2                */
#define _AES_BA411E_HW_CFG_2_g_CtrSize_SHIFT                0                                             /**< Shift value for AES_g_CtrSize               */
#define _AES_BA411E_HW_CFG_2_g_CtrSize_MASK                 0xFFFFUL                                      /**< Bit mask for AES_g_CtrSize                  */
#define _AES_BA411E_HW_CFG_2_g_CtrSize_DEFAULT              0x00000080UL                                  /**< Mode DEFAULT for AES_BA411E_HW_CFG_2        */
#define AES_BA411E_HW_CFG_2_g_CtrSize_DEFAULT               (_AES_BA411E_HW_CFG_2_g_CtrSize_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_BA411E_HW_CFG_2*/

/* Bit fields for AES BA413_HW_CFG */
#define _AES_BA413_HW_CFG_RESETVALUE                        0x00000000UL                                         /**< Default value for AES_BA413_HW_CFG          */
#define _AES_BA413_HW_CFG_MASK                              0x0007007FUL                                         /**< Mask for AES_BA413_HW_CFG                   */
#define _AES_BA413_HW_CFG_g_HashMaskFunc_SHIFT              0                                                    /**< Shift value for AES_g_HashMaskFunc          */
#define _AES_BA413_HW_CFG_g_HashMaskFunc_MASK               0x7FUL                                               /**< Bit mask for AES_g_HashMaskFunc             */
#define _AES_BA413_HW_CFG_g_HashMaskFunc_DEFAULT            0x00000000UL                                         /**< Mode DEFAULT for AES_BA413_HW_CFG           */
#define AES_BA413_HW_CFG_g_HashMaskFunc_DEFAULT             (_AES_BA413_HW_CFG_g_HashMaskFunc_DEFAULT << 0)      /**< Shifted mode DEFAULT for AES_BA413_HW_CFG   */
#define AES_BA413_HW_CFG_g_HashPadding                      (0x1UL << 16)                                        /**< Generic g_HashPadding value                 */
#define _AES_BA413_HW_CFG_g_HashPadding_SHIFT               16                                                   /**< Shift value for AES_g_HashPadding           */
#define _AES_BA413_HW_CFG_g_HashPadding_MASK                0x10000UL                                            /**< Bit mask for AES_g_HashPadding              */
#define _AES_BA413_HW_CFG_g_HashPadding_DEFAULT             0x00000000UL                                         /**< Mode DEFAULT for AES_BA413_HW_CFG           */
#define AES_BA413_HW_CFG_g_HashPadding_DEFAULT              (_AES_BA413_HW_CFG_g_HashPadding_DEFAULT << 16)      /**< Shifted mode DEFAULT for AES_BA413_HW_CFG   */
#define AES_BA413_HW_CFG_g_HMAC_enabled                     (0x1UL << 17)                                        /**< Generic g_HMAC_enabled value                */
#define _AES_BA413_HW_CFG_g_HMAC_enabled_SHIFT              17                                                   /**< Shift value for AES_g_HMAC_enabled          */
#define _AES_BA413_HW_CFG_g_HMAC_enabled_MASK               0x20000UL                                            /**< Bit mask for AES_g_HMAC_enabled             */
#define _AES_BA413_HW_CFG_g_HMAC_enabled_DEFAULT            0x00000000UL                                         /**< Mode DEFAULT for AES_BA413_HW_CFG           */
#define AES_BA413_HW_CFG_g_HMAC_enabled_DEFAULT             (_AES_BA413_HW_CFG_g_HMAC_enabled_DEFAULT << 17)     /**< Shifted mode DEFAULT for AES_BA413_HW_CFG   */
#define AES_BA413_HW_CFG_g_HashVerifyDigest                 (0x1UL << 18)                                        /**< Generic g_HashVerifyDigest value            */
#define _AES_BA413_HW_CFG_g_HashVerifyDigest_SHIFT          18                                                   /**< Shift value for AES_g_HashVerifyDigest      */
#define _AES_BA413_HW_CFG_g_HashVerifyDigest_MASK           0x40000UL                                            /**< Bit mask for AES_g_HashVerifyDigest         */
#define _AES_BA413_HW_CFG_g_HashVerifyDigest_DEFAULT        0x00000000UL                                         /**< Mode DEFAULT for AES_BA413_HW_CFG           */
#define AES_BA413_HW_CFG_g_HashVerifyDigest_DEFAULT         (_AES_BA413_HW_CFG_g_HashVerifyDigest_DEFAULT << 18) /**< Shifted mode DEFAULT for AES_BA413_HW_CFG   */

/* Bit fields for AES BA418_HW_CFG */
#define _AES_BA418_HW_CFG_RESETVALUE                        0x00000001UL                                  /**< Default value for AES_BA418_HW_CFG          */
#define _AES_BA418_HW_CFG_MASK                              0x00000001UL                                  /**< Mask for AES_BA418_HW_CFG                   */
#define AES_BA418_HW_CFG_g_Sha3CtxtEn                       (0x1UL << 0)                                  /**< Generic g_Sha3CtxtEn value                  */
#define _AES_BA418_HW_CFG_g_Sha3CtxtEn_SHIFT                0                                             /**< Shift value for AES_g_Sha3CtxtEn            */
#define _AES_BA418_HW_CFG_g_Sha3CtxtEn_MASK                 0x1UL                                         /**< Bit mask for AES_g_Sha3CtxtEn               */
#define _AES_BA418_HW_CFG_g_Sha3CtxtEn_DEFAULT              0x00000001UL                                  /**< Mode DEFAULT for AES_BA418_HW_CFG           */
#define AES_BA418_HW_CFG_g_Sha3CtxtEn_DEFAULT               (_AES_BA418_HW_CFG_g_Sha3CtxtEn_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_BA418_HW_CFG   */

/* Bit fields for AES BA419_HW_CFG */
#define _AES_BA419_HW_CFG_RESETVALUE                        0x00000000UL                                    /**< Default value for AES_BA419_HW_CFG          */
#define _AES_BA419_HW_CFG_MASK                              0x0000007FUL                                    /**< Mask for AES_BA419_HW_CFG                   */
#define _AES_BA419_HW_CFG_g_SM4ModesPoss_SHIFT              0                                               /**< Shift value for AES_g_SM4ModesPoss          */
#define _AES_BA419_HW_CFG_g_SM4ModesPoss_MASK               0x7FUL                                          /**< Bit mask for AES_g_SM4ModesPoss             */
#define _AES_BA419_HW_CFG_g_SM4ModesPoss_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for AES_BA419_HW_CFG           */
#define AES_BA419_HW_CFG_g_SM4ModesPoss_DEFAULT             (_AES_BA419_HW_CFG_g_SM4ModesPoss_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_BA419_HW_CFG   */

/** @} End of group EFR32ZG23_AES_BitFields */
/** @} End of group EFR32ZG23_AES */
/** @} End of group Parts */

#endif /* EFR32ZG23_AES_H */
#ifdef __cplusplus
}
#endif

