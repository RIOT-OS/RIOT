/***************************************************************************//**
 * @file
 * @brief EFM32GG11B_PRS register and bit field definitions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
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
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__ICCARM__)
#pragma system_include       /* Treat file as system include file. */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang system_header  /* Treat file as system include file. */
#endif

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @defgroup EFM32GG11B_PRS PRS
 * @{
 * @brief EFM32GG11B_PRS Register Declaration
 ******************************************************************************/
/** PRS Register Declaration */
typedef struct {
  __IOM uint32_t SWPULSE;       /**< Software Pulse Register  */
  __IOM uint32_t SWLEVEL;       /**< Software Level Register  */
  __IOM uint32_t ROUTEPEN;      /**< I/O Routing Pin Enable Register  */
  uint32_t       RESERVED0[1U]; /**< Reserved for future use **/
  __IOM uint32_t ROUTELOC0;     /**< I/O Routing Location Register  */
  __IOM uint32_t ROUTELOC1;     /**< I/O Routing Location Register  */
  __IOM uint32_t ROUTELOC2;     /**< I/O Routing Location Register  */
  __IOM uint32_t ROUTELOC3;     /**< I/O Routing Location Register  */
  __IOM uint32_t ROUTELOC4;     /**< I/O Routing Location Register  */
  __IOM uint32_t ROUTELOC5;     /**< I/O Routing Location Register  */
  uint32_t       RESERVED1[2U]; /**< Reserved for future use **/
  __IOM uint32_t CTRL;          /**< Control Register  */
  __IOM uint32_t DMAREQ0;       /**< DMA Request 0 Register  */
  __IOM uint32_t DMAREQ1;       /**< DMA Request 1 Register  */
  uint32_t       RESERVED2[1U]; /**< Reserved for future use **/
  __IM uint32_t  PEEK;          /**< PRS Channel Values  */

  uint32_t       RESERVED3[3U]; /**< Reserved registers */
  PRS_CH_TypeDef CH[24U];       /**< Channel registers */
} PRS_TypeDef;                  /** @} */

/***************************************************************************//**
 * @addtogroup EFM32GG11B_PRS
 * @{
 * @defgroup EFM32GG11B_PRS_BitFields  PRS Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for PRS SWPULSE */
#define _PRS_SWPULSE_RESETVALUE                    0x00000000UL                           /**< Default value for PRS_SWPULSE */
#define _PRS_SWPULSE_MASK                          0x00FFFFFFUL                           /**< Mask for PRS_SWPULSE */
#define PRS_SWPULSE_CH0PULSE                       (0x1UL << 0)                           /**< Channel 0 Pulse Generation */
#define _PRS_SWPULSE_CH0PULSE_SHIFT                0                                      /**< Shift value for PRS_CH0PULSE */
#define _PRS_SWPULSE_CH0PULSE_MASK                 0x1UL                                  /**< Bit mask for PRS_CH0PULSE */
#define _PRS_SWPULSE_CH0PULSE_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH0PULSE_DEFAULT               (_PRS_SWPULSE_CH0PULSE_DEFAULT << 0)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH1PULSE                       (0x1UL << 1)                           /**< Channel 1 Pulse Generation */
#define _PRS_SWPULSE_CH1PULSE_SHIFT                1                                      /**< Shift value for PRS_CH1PULSE */
#define _PRS_SWPULSE_CH1PULSE_MASK                 0x2UL                                  /**< Bit mask for PRS_CH1PULSE */
#define _PRS_SWPULSE_CH1PULSE_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH1PULSE_DEFAULT               (_PRS_SWPULSE_CH1PULSE_DEFAULT << 1)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH2PULSE                       (0x1UL << 2)                           /**< Channel 2 Pulse Generation */
#define _PRS_SWPULSE_CH2PULSE_SHIFT                2                                      /**< Shift value for PRS_CH2PULSE */
#define _PRS_SWPULSE_CH2PULSE_MASK                 0x4UL                                  /**< Bit mask for PRS_CH2PULSE */
#define _PRS_SWPULSE_CH2PULSE_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH2PULSE_DEFAULT               (_PRS_SWPULSE_CH2PULSE_DEFAULT << 2)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH3PULSE                       (0x1UL << 3)                           /**< Channel 3 Pulse Generation */
#define _PRS_SWPULSE_CH3PULSE_SHIFT                3                                      /**< Shift value for PRS_CH3PULSE */
#define _PRS_SWPULSE_CH3PULSE_MASK                 0x8UL                                  /**< Bit mask for PRS_CH3PULSE */
#define _PRS_SWPULSE_CH3PULSE_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH3PULSE_DEFAULT               (_PRS_SWPULSE_CH3PULSE_DEFAULT << 3)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH4PULSE                       (0x1UL << 4)                           /**< Channel 4 Pulse Generation */
#define _PRS_SWPULSE_CH4PULSE_SHIFT                4                                      /**< Shift value for PRS_CH4PULSE */
#define _PRS_SWPULSE_CH4PULSE_MASK                 0x10UL                                 /**< Bit mask for PRS_CH4PULSE */
#define _PRS_SWPULSE_CH4PULSE_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH4PULSE_DEFAULT               (_PRS_SWPULSE_CH4PULSE_DEFAULT << 4)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH5PULSE                       (0x1UL << 5)                           /**< Channel 5 Pulse Generation */
#define _PRS_SWPULSE_CH5PULSE_SHIFT                5                                      /**< Shift value for PRS_CH5PULSE */
#define _PRS_SWPULSE_CH5PULSE_MASK                 0x20UL                                 /**< Bit mask for PRS_CH5PULSE */
#define _PRS_SWPULSE_CH5PULSE_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH5PULSE_DEFAULT               (_PRS_SWPULSE_CH5PULSE_DEFAULT << 5)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH6PULSE                       (0x1UL << 6)                           /**< Channel 6 Pulse Generation */
#define _PRS_SWPULSE_CH6PULSE_SHIFT                6                                      /**< Shift value for PRS_CH6PULSE */
#define _PRS_SWPULSE_CH6PULSE_MASK                 0x40UL                                 /**< Bit mask for PRS_CH6PULSE */
#define _PRS_SWPULSE_CH6PULSE_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH6PULSE_DEFAULT               (_PRS_SWPULSE_CH6PULSE_DEFAULT << 6)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH7PULSE                       (0x1UL << 7)                           /**< Channel 7 Pulse Generation */
#define _PRS_SWPULSE_CH7PULSE_SHIFT                7                                      /**< Shift value for PRS_CH7PULSE */
#define _PRS_SWPULSE_CH7PULSE_MASK                 0x80UL                                 /**< Bit mask for PRS_CH7PULSE */
#define _PRS_SWPULSE_CH7PULSE_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH7PULSE_DEFAULT               (_PRS_SWPULSE_CH7PULSE_DEFAULT << 7)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH8PULSE                       (0x1UL << 8)                           /**< Channel 8 Pulse Generation */
#define _PRS_SWPULSE_CH8PULSE_SHIFT                8                                      /**< Shift value for PRS_CH8PULSE */
#define _PRS_SWPULSE_CH8PULSE_MASK                 0x100UL                                /**< Bit mask for PRS_CH8PULSE */
#define _PRS_SWPULSE_CH8PULSE_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH8PULSE_DEFAULT               (_PRS_SWPULSE_CH8PULSE_DEFAULT << 8)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH9PULSE                       (0x1UL << 9)                           /**< Channel 9 Pulse Generation */
#define _PRS_SWPULSE_CH9PULSE_SHIFT                9                                      /**< Shift value for PRS_CH9PULSE */
#define _PRS_SWPULSE_CH9PULSE_MASK                 0x200UL                                /**< Bit mask for PRS_CH9PULSE */
#define _PRS_SWPULSE_CH9PULSE_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH9PULSE_DEFAULT               (_PRS_SWPULSE_CH9PULSE_DEFAULT << 9)   /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH10PULSE                      (0x1UL << 10)                          /**< Channel 10 Pulse Generation */
#define _PRS_SWPULSE_CH10PULSE_SHIFT               10                                     /**< Shift value for PRS_CH10PULSE */
#define _PRS_SWPULSE_CH10PULSE_MASK                0x400UL                                /**< Bit mask for PRS_CH10PULSE */
#define _PRS_SWPULSE_CH10PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH10PULSE_DEFAULT              (_PRS_SWPULSE_CH10PULSE_DEFAULT << 10) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH11PULSE                      (0x1UL << 11)                          /**< Channel 11 Pulse Generation */
#define _PRS_SWPULSE_CH11PULSE_SHIFT               11                                     /**< Shift value for PRS_CH11PULSE */
#define _PRS_SWPULSE_CH11PULSE_MASK                0x800UL                                /**< Bit mask for PRS_CH11PULSE */
#define _PRS_SWPULSE_CH11PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH11PULSE_DEFAULT              (_PRS_SWPULSE_CH11PULSE_DEFAULT << 11) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH12PULSE                      (0x1UL << 12)                          /**< Channel 12 Pulse Generation */
#define _PRS_SWPULSE_CH12PULSE_SHIFT               12                                     /**< Shift value for PRS_CH12PULSE */
#define _PRS_SWPULSE_CH12PULSE_MASK                0x1000UL                               /**< Bit mask for PRS_CH12PULSE */
#define _PRS_SWPULSE_CH12PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH12PULSE_DEFAULT              (_PRS_SWPULSE_CH12PULSE_DEFAULT << 12) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH13PULSE                      (0x1UL << 13)                          /**< Channel 13 Pulse Generation */
#define _PRS_SWPULSE_CH13PULSE_SHIFT               13                                     /**< Shift value for PRS_CH13PULSE */
#define _PRS_SWPULSE_CH13PULSE_MASK                0x2000UL                               /**< Bit mask for PRS_CH13PULSE */
#define _PRS_SWPULSE_CH13PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH13PULSE_DEFAULT              (_PRS_SWPULSE_CH13PULSE_DEFAULT << 13) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH14PULSE                      (0x1UL << 14)                          /**< Channel 14 Pulse Generation */
#define _PRS_SWPULSE_CH14PULSE_SHIFT               14                                     /**< Shift value for PRS_CH14PULSE */
#define _PRS_SWPULSE_CH14PULSE_MASK                0x4000UL                               /**< Bit mask for PRS_CH14PULSE */
#define _PRS_SWPULSE_CH14PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH14PULSE_DEFAULT              (_PRS_SWPULSE_CH14PULSE_DEFAULT << 14) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH15PULSE                      (0x1UL << 15)                          /**< Channel 15 Pulse Generation */
#define _PRS_SWPULSE_CH15PULSE_SHIFT               15                                     /**< Shift value for PRS_CH15PULSE */
#define _PRS_SWPULSE_CH15PULSE_MASK                0x8000UL                               /**< Bit mask for PRS_CH15PULSE */
#define _PRS_SWPULSE_CH15PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH15PULSE_DEFAULT              (_PRS_SWPULSE_CH15PULSE_DEFAULT << 15) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH16PULSE                      (0x1UL << 16)                          /**< Channel 16 Pulse Generation */
#define _PRS_SWPULSE_CH16PULSE_SHIFT               16                                     /**< Shift value for PRS_CH16PULSE */
#define _PRS_SWPULSE_CH16PULSE_MASK                0x10000UL                              /**< Bit mask for PRS_CH16PULSE */
#define _PRS_SWPULSE_CH16PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH16PULSE_DEFAULT              (_PRS_SWPULSE_CH16PULSE_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH17PULSE                      (0x1UL << 17)                          /**< Channel 17 Pulse Generation */
#define _PRS_SWPULSE_CH17PULSE_SHIFT               17                                     /**< Shift value for PRS_CH17PULSE */
#define _PRS_SWPULSE_CH17PULSE_MASK                0x20000UL                              /**< Bit mask for PRS_CH17PULSE */
#define _PRS_SWPULSE_CH17PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH17PULSE_DEFAULT              (_PRS_SWPULSE_CH17PULSE_DEFAULT << 17) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH18PULSE                      (0x1UL << 18)                          /**< Channel 18 Pulse Generation */
#define _PRS_SWPULSE_CH18PULSE_SHIFT               18                                     /**< Shift value for PRS_CH18PULSE */
#define _PRS_SWPULSE_CH18PULSE_MASK                0x40000UL                              /**< Bit mask for PRS_CH18PULSE */
#define _PRS_SWPULSE_CH18PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH18PULSE_DEFAULT              (_PRS_SWPULSE_CH18PULSE_DEFAULT << 18) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH19PULSE                      (0x1UL << 19)                          /**< Channel 19 Pulse Generation */
#define _PRS_SWPULSE_CH19PULSE_SHIFT               19                                     /**< Shift value for PRS_CH19PULSE */
#define _PRS_SWPULSE_CH19PULSE_MASK                0x80000UL                              /**< Bit mask for PRS_CH19PULSE */
#define _PRS_SWPULSE_CH19PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH19PULSE_DEFAULT              (_PRS_SWPULSE_CH19PULSE_DEFAULT << 19) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH20PULSE                      (0x1UL << 20)                          /**< Channel 20 Pulse Generation */
#define _PRS_SWPULSE_CH20PULSE_SHIFT               20                                     /**< Shift value for PRS_CH20PULSE */
#define _PRS_SWPULSE_CH20PULSE_MASK                0x100000UL                             /**< Bit mask for PRS_CH20PULSE */
#define _PRS_SWPULSE_CH20PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH20PULSE_DEFAULT              (_PRS_SWPULSE_CH20PULSE_DEFAULT << 20) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH21PULSE                      (0x1UL << 21)                          /**< Channel 21 Pulse Generation */
#define _PRS_SWPULSE_CH21PULSE_SHIFT               21                                     /**< Shift value for PRS_CH21PULSE */
#define _PRS_SWPULSE_CH21PULSE_MASK                0x200000UL                             /**< Bit mask for PRS_CH21PULSE */
#define _PRS_SWPULSE_CH21PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH21PULSE_DEFAULT              (_PRS_SWPULSE_CH21PULSE_DEFAULT << 21) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH22PULSE                      (0x1UL << 22)                          /**< Channel 22 Pulse Generation */
#define _PRS_SWPULSE_CH22PULSE_SHIFT               22                                     /**< Shift value for PRS_CH22PULSE */
#define _PRS_SWPULSE_CH22PULSE_MASK                0x400000UL                             /**< Bit mask for PRS_CH22PULSE */
#define _PRS_SWPULSE_CH22PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH22PULSE_DEFAULT              (_PRS_SWPULSE_CH22PULSE_DEFAULT << 22) /**< Shifted mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH23PULSE                      (0x1UL << 23)                          /**< Channel 23 Pulse Generation */
#define _PRS_SWPULSE_CH23PULSE_SHIFT               23                                     /**< Shift value for PRS_CH23PULSE */
#define _PRS_SWPULSE_CH23PULSE_MASK                0x800000UL                             /**< Bit mask for PRS_CH23PULSE */
#define _PRS_SWPULSE_CH23PULSE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWPULSE */
#define PRS_SWPULSE_CH23PULSE_DEFAULT              (_PRS_SWPULSE_CH23PULSE_DEFAULT << 23) /**< Shifted mode DEFAULT for PRS_SWPULSE */

/* Bit fields for PRS SWLEVEL */
#define _PRS_SWLEVEL_RESETVALUE                    0x00000000UL                           /**< Default value for PRS_SWLEVEL */
#define _PRS_SWLEVEL_MASK                          0x00FFFFFFUL                           /**< Mask for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH0LEVEL                       (0x1UL << 0)                           /**< Channel 0 Software Level */
#define _PRS_SWLEVEL_CH0LEVEL_SHIFT                0                                      /**< Shift value for PRS_CH0LEVEL */
#define _PRS_SWLEVEL_CH0LEVEL_MASK                 0x1UL                                  /**< Bit mask for PRS_CH0LEVEL */
#define _PRS_SWLEVEL_CH0LEVEL_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH0LEVEL_DEFAULT               (_PRS_SWLEVEL_CH0LEVEL_DEFAULT << 0)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH1LEVEL                       (0x1UL << 1)                           /**< Channel 1 Software Level */
#define _PRS_SWLEVEL_CH1LEVEL_SHIFT                1                                      /**< Shift value for PRS_CH1LEVEL */
#define _PRS_SWLEVEL_CH1LEVEL_MASK                 0x2UL                                  /**< Bit mask for PRS_CH1LEVEL */
#define _PRS_SWLEVEL_CH1LEVEL_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH1LEVEL_DEFAULT               (_PRS_SWLEVEL_CH1LEVEL_DEFAULT << 1)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH2LEVEL                       (0x1UL << 2)                           /**< Channel 2 Software Level */
#define _PRS_SWLEVEL_CH2LEVEL_SHIFT                2                                      /**< Shift value for PRS_CH2LEVEL */
#define _PRS_SWLEVEL_CH2LEVEL_MASK                 0x4UL                                  /**< Bit mask for PRS_CH2LEVEL */
#define _PRS_SWLEVEL_CH2LEVEL_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH2LEVEL_DEFAULT               (_PRS_SWLEVEL_CH2LEVEL_DEFAULT << 2)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH3LEVEL                       (0x1UL << 3)                           /**< Channel 3 Software Level */
#define _PRS_SWLEVEL_CH3LEVEL_SHIFT                3                                      /**< Shift value for PRS_CH3LEVEL */
#define _PRS_SWLEVEL_CH3LEVEL_MASK                 0x8UL                                  /**< Bit mask for PRS_CH3LEVEL */
#define _PRS_SWLEVEL_CH3LEVEL_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH3LEVEL_DEFAULT               (_PRS_SWLEVEL_CH3LEVEL_DEFAULT << 3)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH4LEVEL                       (0x1UL << 4)                           /**< Channel 4 Software Level */
#define _PRS_SWLEVEL_CH4LEVEL_SHIFT                4                                      /**< Shift value for PRS_CH4LEVEL */
#define _PRS_SWLEVEL_CH4LEVEL_MASK                 0x10UL                                 /**< Bit mask for PRS_CH4LEVEL */
#define _PRS_SWLEVEL_CH4LEVEL_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH4LEVEL_DEFAULT               (_PRS_SWLEVEL_CH4LEVEL_DEFAULT << 4)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH5LEVEL                       (0x1UL << 5)                           /**< Channel 5 Software Level */
#define _PRS_SWLEVEL_CH5LEVEL_SHIFT                5                                      /**< Shift value for PRS_CH5LEVEL */
#define _PRS_SWLEVEL_CH5LEVEL_MASK                 0x20UL                                 /**< Bit mask for PRS_CH5LEVEL */
#define _PRS_SWLEVEL_CH5LEVEL_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH5LEVEL_DEFAULT               (_PRS_SWLEVEL_CH5LEVEL_DEFAULT << 5)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH6LEVEL                       (0x1UL << 6)                           /**< Channel 6 Software Level */
#define _PRS_SWLEVEL_CH6LEVEL_SHIFT                6                                      /**< Shift value for PRS_CH6LEVEL */
#define _PRS_SWLEVEL_CH6LEVEL_MASK                 0x40UL                                 /**< Bit mask for PRS_CH6LEVEL */
#define _PRS_SWLEVEL_CH6LEVEL_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH6LEVEL_DEFAULT               (_PRS_SWLEVEL_CH6LEVEL_DEFAULT << 6)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH7LEVEL                       (0x1UL << 7)                           /**< Channel 7 Software Level */
#define _PRS_SWLEVEL_CH7LEVEL_SHIFT                7                                      /**< Shift value for PRS_CH7LEVEL */
#define _PRS_SWLEVEL_CH7LEVEL_MASK                 0x80UL                                 /**< Bit mask for PRS_CH7LEVEL */
#define _PRS_SWLEVEL_CH7LEVEL_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH7LEVEL_DEFAULT               (_PRS_SWLEVEL_CH7LEVEL_DEFAULT << 7)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH8LEVEL                       (0x1UL << 8)                           /**< Channel 8 Software Level */
#define _PRS_SWLEVEL_CH8LEVEL_SHIFT                8                                      /**< Shift value for PRS_CH8LEVEL */
#define _PRS_SWLEVEL_CH8LEVEL_MASK                 0x100UL                                /**< Bit mask for PRS_CH8LEVEL */
#define _PRS_SWLEVEL_CH8LEVEL_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH8LEVEL_DEFAULT               (_PRS_SWLEVEL_CH8LEVEL_DEFAULT << 8)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH9LEVEL                       (0x1UL << 9)                           /**< Channel 9 Software Level */
#define _PRS_SWLEVEL_CH9LEVEL_SHIFT                9                                      /**< Shift value for PRS_CH9LEVEL */
#define _PRS_SWLEVEL_CH9LEVEL_MASK                 0x200UL                                /**< Bit mask for PRS_CH9LEVEL */
#define _PRS_SWLEVEL_CH9LEVEL_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH9LEVEL_DEFAULT               (_PRS_SWLEVEL_CH9LEVEL_DEFAULT << 9)   /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH10LEVEL                      (0x1UL << 10)                          /**< Channel 10 Software Level */
#define _PRS_SWLEVEL_CH10LEVEL_SHIFT               10                                     /**< Shift value for PRS_CH10LEVEL */
#define _PRS_SWLEVEL_CH10LEVEL_MASK                0x400UL                                /**< Bit mask for PRS_CH10LEVEL */
#define _PRS_SWLEVEL_CH10LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH10LEVEL_DEFAULT              (_PRS_SWLEVEL_CH10LEVEL_DEFAULT << 10) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH11LEVEL                      (0x1UL << 11)                          /**< Channel 11 Software Level */
#define _PRS_SWLEVEL_CH11LEVEL_SHIFT               11                                     /**< Shift value for PRS_CH11LEVEL */
#define _PRS_SWLEVEL_CH11LEVEL_MASK                0x800UL                                /**< Bit mask for PRS_CH11LEVEL */
#define _PRS_SWLEVEL_CH11LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH11LEVEL_DEFAULT              (_PRS_SWLEVEL_CH11LEVEL_DEFAULT << 11) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH12LEVEL                      (0x1UL << 12)                          /**< Channel 12 Software Level */
#define _PRS_SWLEVEL_CH12LEVEL_SHIFT               12                                     /**< Shift value for PRS_CH12LEVEL */
#define _PRS_SWLEVEL_CH12LEVEL_MASK                0x1000UL                               /**< Bit mask for PRS_CH12LEVEL */
#define _PRS_SWLEVEL_CH12LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH12LEVEL_DEFAULT              (_PRS_SWLEVEL_CH12LEVEL_DEFAULT << 12) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH13LEVEL                      (0x1UL << 13)                          /**< Channel 13 Software Level */
#define _PRS_SWLEVEL_CH13LEVEL_SHIFT               13                                     /**< Shift value for PRS_CH13LEVEL */
#define _PRS_SWLEVEL_CH13LEVEL_MASK                0x2000UL                               /**< Bit mask for PRS_CH13LEVEL */
#define _PRS_SWLEVEL_CH13LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH13LEVEL_DEFAULT              (_PRS_SWLEVEL_CH13LEVEL_DEFAULT << 13) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH14LEVEL                      (0x1UL << 14)                          /**< Channel 14 Software Level */
#define _PRS_SWLEVEL_CH14LEVEL_SHIFT               14                                     /**< Shift value for PRS_CH14LEVEL */
#define _PRS_SWLEVEL_CH14LEVEL_MASK                0x4000UL                               /**< Bit mask for PRS_CH14LEVEL */
#define _PRS_SWLEVEL_CH14LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH14LEVEL_DEFAULT              (_PRS_SWLEVEL_CH14LEVEL_DEFAULT << 14) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH15LEVEL                      (0x1UL << 15)                          /**< Channel 15 Software Level */
#define _PRS_SWLEVEL_CH15LEVEL_SHIFT               15                                     /**< Shift value for PRS_CH15LEVEL */
#define _PRS_SWLEVEL_CH15LEVEL_MASK                0x8000UL                               /**< Bit mask for PRS_CH15LEVEL */
#define _PRS_SWLEVEL_CH15LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH15LEVEL_DEFAULT              (_PRS_SWLEVEL_CH15LEVEL_DEFAULT << 15) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH16LEVEL                      (0x1UL << 16)                          /**< Channel 16 Software Level */
#define _PRS_SWLEVEL_CH16LEVEL_SHIFT               16                                     /**< Shift value for PRS_CH16LEVEL */
#define _PRS_SWLEVEL_CH16LEVEL_MASK                0x10000UL                              /**< Bit mask for PRS_CH16LEVEL */
#define _PRS_SWLEVEL_CH16LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH16LEVEL_DEFAULT              (_PRS_SWLEVEL_CH16LEVEL_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH17LEVEL                      (0x1UL << 17)                          /**< Channel 17 Software Level */
#define _PRS_SWLEVEL_CH17LEVEL_SHIFT               17                                     /**< Shift value for PRS_CH17LEVEL */
#define _PRS_SWLEVEL_CH17LEVEL_MASK                0x20000UL                              /**< Bit mask for PRS_CH17LEVEL */
#define _PRS_SWLEVEL_CH17LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH17LEVEL_DEFAULT              (_PRS_SWLEVEL_CH17LEVEL_DEFAULT << 17) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH18LEVEL                      (0x1UL << 18)                          /**< Channel 18 Software Level */
#define _PRS_SWLEVEL_CH18LEVEL_SHIFT               18                                     /**< Shift value for PRS_CH18LEVEL */
#define _PRS_SWLEVEL_CH18LEVEL_MASK                0x40000UL                              /**< Bit mask for PRS_CH18LEVEL */
#define _PRS_SWLEVEL_CH18LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH18LEVEL_DEFAULT              (_PRS_SWLEVEL_CH18LEVEL_DEFAULT << 18) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH19LEVEL                      (0x1UL << 19)                          /**< Channel 19 Software Level */
#define _PRS_SWLEVEL_CH19LEVEL_SHIFT               19                                     /**< Shift value for PRS_CH19LEVEL */
#define _PRS_SWLEVEL_CH19LEVEL_MASK                0x80000UL                              /**< Bit mask for PRS_CH19LEVEL */
#define _PRS_SWLEVEL_CH19LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH19LEVEL_DEFAULT              (_PRS_SWLEVEL_CH19LEVEL_DEFAULT << 19) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH20LEVEL                      (0x1UL << 20)                          /**< Channel 20 Software Level */
#define _PRS_SWLEVEL_CH20LEVEL_SHIFT               20                                     /**< Shift value for PRS_CH20LEVEL */
#define _PRS_SWLEVEL_CH20LEVEL_MASK                0x100000UL                             /**< Bit mask for PRS_CH20LEVEL */
#define _PRS_SWLEVEL_CH20LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH20LEVEL_DEFAULT              (_PRS_SWLEVEL_CH20LEVEL_DEFAULT << 20) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH21LEVEL                      (0x1UL << 21)                          /**< Channel 21 Software Level */
#define _PRS_SWLEVEL_CH21LEVEL_SHIFT               21                                     /**< Shift value for PRS_CH21LEVEL */
#define _PRS_SWLEVEL_CH21LEVEL_MASK                0x200000UL                             /**< Bit mask for PRS_CH21LEVEL */
#define _PRS_SWLEVEL_CH21LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH21LEVEL_DEFAULT              (_PRS_SWLEVEL_CH21LEVEL_DEFAULT << 21) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH22LEVEL                      (0x1UL << 22)                          /**< Channel 22 Software Level */
#define _PRS_SWLEVEL_CH22LEVEL_SHIFT               22                                     /**< Shift value for PRS_CH22LEVEL */
#define _PRS_SWLEVEL_CH22LEVEL_MASK                0x400000UL                             /**< Bit mask for PRS_CH22LEVEL */
#define _PRS_SWLEVEL_CH22LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH22LEVEL_DEFAULT              (_PRS_SWLEVEL_CH22LEVEL_DEFAULT << 22) /**< Shifted mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH23LEVEL                      (0x1UL << 23)                          /**< Channel 23 Software Level */
#define _PRS_SWLEVEL_CH23LEVEL_SHIFT               23                                     /**< Shift value for PRS_CH23LEVEL */
#define _PRS_SWLEVEL_CH23LEVEL_MASK                0x800000UL                             /**< Bit mask for PRS_CH23LEVEL */
#define _PRS_SWLEVEL_CH23LEVEL_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_SWLEVEL */
#define PRS_SWLEVEL_CH23LEVEL_DEFAULT              (_PRS_SWLEVEL_CH23LEVEL_DEFAULT << 23) /**< Shifted mode DEFAULT for PRS_SWLEVEL */

/* Bit fields for PRS ROUTEPEN */
#define _PRS_ROUTEPEN_RESETVALUE                   0x00000000UL                          /**< Default value for PRS_ROUTEPEN */
#define _PRS_ROUTEPEN_MASK                         0x00FFFFFFUL                          /**< Mask for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH0PEN                        (0x1UL << 0)                          /**< CH0 Pin Enable */
#define _PRS_ROUTEPEN_CH0PEN_SHIFT                 0                                     /**< Shift value for PRS_CH0PEN */
#define _PRS_ROUTEPEN_CH0PEN_MASK                  0x1UL                                 /**< Bit mask for PRS_CH0PEN */
#define _PRS_ROUTEPEN_CH0PEN_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH0PEN_DEFAULT                (_PRS_ROUTEPEN_CH0PEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH1PEN                        (0x1UL << 1)                          /**< CH1 Pin Enable */
#define _PRS_ROUTEPEN_CH1PEN_SHIFT                 1                                     /**< Shift value for PRS_CH1PEN */
#define _PRS_ROUTEPEN_CH1PEN_MASK                  0x2UL                                 /**< Bit mask for PRS_CH1PEN */
#define _PRS_ROUTEPEN_CH1PEN_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH1PEN_DEFAULT                (_PRS_ROUTEPEN_CH1PEN_DEFAULT << 1)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH2PEN                        (0x1UL << 2)                          /**< CH2 Pin Enable */
#define _PRS_ROUTEPEN_CH2PEN_SHIFT                 2                                     /**< Shift value for PRS_CH2PEN */
#define _PRS_ROUTEPEN_CH2PEN_MASK                  0x4UL                                 /**< Bit mask for PRS_CH2PEN */
#define _PRS_ROUTEPEN_CH2PEN_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH2PEN_DEFAULT                (_PRS_ROUTEPEN_CH2PEN_DEFAULT << 2)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH3PEN                        (0x1UL << 3)                          /**< CH3 Pin Enable */
#define _PRS_ROUTEPEN_CH3PEN_SHIFT                 3                                     /**< Shift value for PRS_CH3PEN */
#define _PRS_ROUTEPEN_CH3PEN_MASK                  0x8UL                                 /**< Bit mask for PRS_CH3PEN */
#define _PRS_ROUTEPEN_CH3PEN_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH3PEN_DEFAULT                (_PRS_ROUTEPEN_CH3PEN_DEFAULT << 3)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH4PEN                        (0x1UL << 4)                          /**< CH4 Pin Enable */
#define _PRS_ROUTEPEN_CH4PEN_SHIFT                 4                                     /**< Shift value for PRS_CH4PEN */
#define _PRS_ROUTEPEN_CH4PEN_MASK                  0x10UL                                /**< Bit mask for PRS_CH4PEN */
#define _PRS_ROUTEPEN_CH4PEN_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH4PEN_DEFAULT                (_PRS_ROUTEPEN_CH4PEN_DEFAULT << 4)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH5PEN                        (0x1UL << 5)                          /**< CH5 Pin Enable */
#define _PRS_ROUTEPEN_CH5PEN_SHIFT                 5                                     /**< Shift value for PRS_CH5PEN */
#define _PRS_ROUTEPEN_CH5PEN_MASK                  0x20UL                                /**< Bit mask for PRS_CH5PEN */
#define _PRS_ROUTEPEN_CH5PEN_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH5PEN_DEFAULT                (_PRS_ROUTEPEN_CH5PEN_DEFAULT << 5)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH6PEN                        (0x1UL << 6)                          /**< CH6 Pin Enable */
#define _PRS_ROUTEPEN_CH6PEN_SHIFT                 6                                     /**< Shift value for PRS_CH6PEN */
#define _PRS_ROUTEPEN_CH6PEN_MASK                  0x40UL                                /**< Bit mask for PRS_CH6PEN */
#define _PRS_ROUTEPEN_CH6PEN_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH6PEN_DEFAULT                (_PRS_ROUTEPEN_CH6PEN_DEFAULT << 6)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH7PEN                        (0x1UL << 7)                          /**< CH7 Pin Enable */
#define _PRS_ROUTEPEN_CH7PEN_SHIFT                 7                                     /**< Shift value for PRS_CH7PEN */
#define _PRS_ROUTEPEN_CH7PEN_MASK                  0x80UL                                /**< Bit mask for PRS_CH7PEN */
#define _PRS_ROUTEPEN_CH7PEN_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH7PEN_DEFAULT                (_PRS_ROUTEPEN_CH7PEN_DEFAULT << 7)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH8PEN                        (0x1UL << 8)                          /**< CH8 Pin Enable */
#define _PRS_ROUTEPEN_CH8PEN_SHIFT                 8                                     /**< Shift value for PRS_CH8PEN */
#define _PRS_ROUTEPEN_CH8PEN_MASK                  0x100UL                               /**< Bit mask for PRS_CH8PEN */
#define _PRS_ROUTEPEN_CH8PEN_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH8PEN_DEFAULT                (_PRS_ROUTEPEN_CH8PEN_DEFAULT << 8)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH9PEN                        (0x1UL << 9)                          /**< CH9 Pin Enable */
#define _PRS_ROUTEPEN_CH9PEN_SHIFT                 9                                     /**< Shift value for PRS_CH9PEN */
#define _PRS_ROUTEPEN_CH9PEN_MASK                  0x200UL                               /**< Bit mask for PRS_CH9PEN */
#define _PRS_ROUTEPEN_CH9PEN_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH9PEN_DEFAULT                (_PRS_ROUTEPEN_CH9PEN_DEFAULT << 9)   /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH10PEN                       (0x1UL << 10)                         /**< CH10 Pin Enable */
#define _PRS_ROUTEPEN_CH10PEN_SHIFT                10                                    /**< Shift value for PRS_CH10PEN */
#define _PRS_ROUTEPEN_CH10PEN_MASK                 0x400UL                               /**< Bit mask for PRS_CH10PEN */
#define _PRS_ROUTEPEN_CH10PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH10PEN_DEFAULT               (_PRS_ROUTEPEN_CH10PEN_DEFAULT << 10) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH11PEN                       (0x1UL << 11)                         /**< CH11 Pin Enable */
#define _PRS_ROUTEPEN_CH11PEN_SHIFT                11                                    /**< Shift value for PRS_CH11PEN */
#define _PRS_ROUTEPEN_CH11PEN_MASK                 0x800UL                               /**< Bit mask for PRS_CH11PEN */
#define _PRS_ROUTEPEN_CH11PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH11PEN_DEFAULT               (_PRS_ROUTEPEN_CH11PEN_DEFAULT << 11) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH12PEN                       (0x1UL << 12)                         /**< CH12 Pin Enable */
#define _PRS_ROUTEPEN_CH12PEN_SHIFT                12                                    /**< Shift value for PRS_CH12PEN */
#define _PRS_ROUTEPEN_CH12PEN_MASK                 0x1000UL                              /**< Bit mask for PRS_CH12PEN */
#define _PRS_ROUTEPEN_CH12PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH12PEN_DEFAULT               (_PRS_ROUTEPEN_CH12PEN_DEFAULT << 12) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH13PEN                       (0x1UL << 13)                         /**< CH13 Pin Enable */
#define _PRS_ROUTEPEN_CH13PEN_SHIFT                13                                    /**< Shift value for PRS_CH13PEN */
#define _PRS_ROUTEPEN_CH13PEN_MASK                 0x2000UL                              /**< Bit mask for PRS_CH13PEN */
#define _PRS_ROUTEPEN_CH13PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH13PEN_DEFAULT               (_PRS_ROUTEPEN_CH13PEN_DEFAULT << 13) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH14PEN                       (0x1UL << 14)                         /**< CH14 Pin Enable */
#define _PRS_ROUTEPEN_CH14PEN_SHIFT                14                                    /**< Shift value for PRS_CH14PEN */
#define _PRS_ROUTEPEN_CH14PEN_MASK                 0x4000UL                              /**< Bit mask for PRS_CH14PEN */
#define _PRS_ROUTEPEN_CH14PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH14PEN_DEFAULT               (_PRS_ROUTEPEN_CH14PEN_DEFAULT << 14) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH15PEN                       (0x1UL << 15)                         /**< CH15 Pin Enable */
#define _PRS_ROUTEPEN_CH15PEN_SHIFT                15                                    /**< Shift value for PRS_CH15PEN */
#define _PRS_ROUTEPEN_CH15PEN_MASK                 0x8000UL                              /**< Bit mask for PRS_CH15PEN */
#define _PRS_ROUTEPEN_CH15PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH15PEN_DEFAULT               (_PRS_ROUTEPEN_CH15PEN_DEFAULT << 15) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH16PEN                       (0x1UL << 16)                         /**< CH16 Pin Enable */
#define _PRS_ROUTEPEN_CH16PEN_SHIFT                16                                    /**< Shift value for PRS_CH16PEN */
#define _PRS_ROUTEPEN_CH16PEN_MASK                 0x10000UL                             /**< Bit mask for PRS_CH16PEN */
#define _PRS_ROUTEPEN_CH16PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH16PEN_DEFAULT               (_PRS_ROUTEPEN_CH16PEN_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH17PEN                       (0x1UL << 17)                         /**< CH17 Pin Enable */
#define _PRS_ROUTEPEN_CH17PEN_SHIFT                17                                    /**< Shift value for PRS_CH17PEN */
#define _PRS_ROUTEPEN_CH17PEN_MASK                 0x20000UL                             /**< Bit mask for PRS_CH17PEN */
#define _PRS_ROUTEPEN_CH17PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH17PEN_DEFAULT               (_PRS_ROUTEPEN_CH17PEN_DEFAULT << 17) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH18PEN                       (0x1UL << 18)                         /**< CH18 Pin Enable */
#define _PRS_ROUTEPEN_CH18PEN_SHIFT                18                                    /**< Shift value for PRS_CH18PEN */
#define _PRS_ROUTEPEN_CH18PEN_MASK                 0x40000UL                             /**< Bit mask for PRS_CH18PEN */
#define _PRS_ROUTEPEN_CH18PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH18PEN_DEFAULT               (_PRS_ROUTEPEN_CH18PEN_DEFAULT << 18) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH19PEN                       (0x1UL << 19)                         /**< CH19 Pin Enable */
#define _PRS_ROUTEPEN_CH19PEN_SHIFT                19                                    /**< Shift value for PRS_CH19PEN */
#define _PRS_ROUTEPEN_CH19PEN_MASK                 0x80000UL                             /**< Bit mask for PRS_CH19PEN */
#define _PRS_ROUTEPEN_CH19PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH19PEN_DEFAULT               (_PRS_ROUTEPEN_CH19PEN_DEFAULT << 19) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH20PEN                       (0x1UL << 20)                         /**< CH20 Pin Enable */
#define _PRS_ROUTEPEN_CH20PEN_SHIFT                20                                    /**< Shift value for PRS_CH20PEN */
#define _PRS_ROUTEPEN_CH20PEN_MASK                 0x100000UL                            /**< Bit mask for PRS_CH20PEN */
#define _PRS_ROUTEPEN_CH20PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH20PEN_DEFAULT               (_PRS_ROUTEPEN_CH20PEN_DEFAULT << 20) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH21PEN                       (0x1UL << 21)                         /**< CH21 Pin Enable */
#define _PRS_ROUTEPEN_CH21PEN_SHIFT                21                                    /**< Shift value for PRS_CH21PEN */
#define _PRS_ROUTEPEN_CH21PEN_MASK                 0x200000UL                            /**< Bit mask for PRS_CH21PEN */
#define _PRS_ROUTEPEN_CH21PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH21PEN_DEFAULT               (_PRS_ROUTEPEN_CH21PEN_DEFAULT << 21) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH22PEN                       (0x1UL << 22)                         /**< CH22 Pin Enable */
#define _PRS_ROUTEPEN_CH22PEN_SHIFT                22                                    /**< Shift value for PRS_CH22PEN */
#define _PRS_ROUTEPEN_CH22PEN_MASK                 0x400000UL                            /**< Bit mask for PRS_CH22PEN */
#define _PRS_ROUTEPEN_CH22PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH22PEN_DEFAULT               (_PRS_ROUTEPEN_CH22PEN_DEFAULT << 22) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH23PEN                       (0x1UL << 23)                         /**< CH23 Pin Enable */
#define _PRS_ROUTEPEN_CH23PEN_SHIFT                23                                    /**< Shift value for PRS_CH23PEN */
#define _PRS_ROUTEPEN_CH23PEN_MASK                 0x800000UL                            /**< Bit mask for PRS_CH23PEN */
#define _PRS_ROUTEPEN_CH23PEN_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTEPEN */
#define PRS_ROUTEPEN_CH23PEN_DEFAULT               (_PRS_ROUTEPEN_CH23PEN_DEFAULT << 23) /**< Shifted mode DEFAULT for PRS_ROUTEPEN */

/* Bit fields for PRS ROUTELOC0 */
#define _PRS_ROUTELOC0_RESETVALUE                  0x00000000UL                          /**< Default value for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_MASK                        0x03030303UL                          /**< Mask for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_SHIFT                0                                     /**< Shift value for PRS_CH0LOC */
#define _PRS_ROUTELOC0_CH0LOC_MASK                 0x3UL                                 /**< Bit mask for PRS_CH0LOC */
#define _PRS_ROUTELOC0_CH0LOC_LOC0                 0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC1                 0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC2                 0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH0LOC_LOC3                 0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC0                  (_PRS_ROUTELOC0_CH0LOC_LOC0 << 0)     /**< Shifted mode LOC0 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_DEFAULT               (_PRS_ROUTELOC0_CH0LOC_DEFAULT << 0)  /**< Shifted mode DEFAULT for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC1                  (_PRS_ROUTELOC0_CH0LOC_LOC1 << 0)     /**< Shifted mode LOC1 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC2                  (_PRS_ROUTELOC0_CH0LOC_LOC2 << 0)     /**< Shifted mode LOC2 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH0LOC_LOC3                  (_PRS_ROUTELOC0_CH0LOC_LOC3 << 0)     /**< Shifted mode LOC3 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_SHIFT                8                                     /**< Shift value for PRS_CH1LOC */
#define _PRS_ROUTELOC0_CH1LOC_MASK                 0x300UL                               /**< Bit mask for PRS_CH1LOC */
#define _PRS_ROUTELOC0_CH1LOC_LOC0                 0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_LOC1                 0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_LOC2                 0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH1LOC_LOC3                 0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC0                  (_PRS_ROUTELOC0_CH1LOC_LOC0 << 8)     /**< Shifted mode LOC0 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_DEFAULT               (_PRS_ROUTELOC0_CH1LOC_DEFAULT << 8)  /**< Shifted mode DEFAULT for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC1                  (_PRS_ROUTELOC0_CH1LOC_LOC1 << 8)     /**< Shifted mode LOC1 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC2                  (_PRS_ROUTELOC0_CH1LOC_LOC2 << 8)     /**< Shifted mode LOC2 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH1LOC_LOC3                  (_PRS_ROUTELOC0_CH1LOC_LOC3 << 8)     /**< Shifted mode LOC3 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_SHIFT                16                                    /**< Shift value for PRS_CH2LOC */
#define _PRS_ROUTELOC0_CH2LOC_MASK                 0x30000UL                             /**< Bit mask for PRS_CH2LOC */
#define _PRS_ROUTELOC0_CH2LOC_LOC0                 0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_LOC1                 0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_LOC2                 0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH2LOC_LOC3                 0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC0                  (_PRS_ROUTELOC0_CH2LOC_LOC0 << 16)    /**< Shifted mode LOC0 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_DEFAULT               (_PRS_ROUTELOC0_CH2LOC_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC1                  (_PRS_ROUTELOC0_CH2LOC_LOC1 << 16)    /**< Shifted mode LOC1 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC2                  (_PRS_ROUTELOC0_CH2LOC_LOC2 << 16)    /**< Shifted mode LOC2 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH2LOC_LOC3                  (_PRS_ROUTELOC0_CH2LOC_LOC3 << 16)    /**< Shifted mode LOC3 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_SHIFT                24                                    /**< Shift value for PRS_CH3LOC */
#define _PRS_ROUTELOC0_CH3LOC_MASK                 0x3000000UL                           /**< Bit mask for PRS_CH3LOC */
#define _PRS_ROUTELOC0_CH3LOC_LOC0                 0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC1                 0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC2                 0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC0 */
#define _PRS_ROUTELOC0_CH3LOC_LOC3                 0x00000003UL                          /**< Mode LOC3 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC0                  (_PRS_ROUTELOC0_CH3LOC_LOC0 << 24)    /**< Shifted mode LOC0 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_DEFAULT               (_PRS_ROUTELOC0_CH3LOC_DEFAULT << 24) /**< Shifted mode DEFAULT for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC1                  (_PRS_ROUTELOC0_CH3LOC_LOC1 << 24)    /**< Shifted mode LOC1 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC2                  (_PRS_ROUTELOC0_CH3LOC_LOC2 << 24)    /**< Shifted mode LOC2 for PRS_ROUTELOC0 */
#define PRS_ROUTELOC0_CH3LOC_LOC3                  (_PRS_ROUTELOC0_CH3LOC_LOC3 << 24)    /**< Shifted mode LOC3 for PRS_ROUTELOC0 */

/* Bit fields for PRS ROUTELOC1 */
#define _PRS_ROUTELOC1_RESETVALUE                  0x00000000UL                          /**< Default value for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_MASK                        0x03030303UL                          /**< Mask for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_SHIFT                0                                     /**< Shift value for PRS_CH4LOC */
#define _PRS_ROUTELOC1_CH4LOC_MASK                 0x3UL                                 /**< Bit mask for PRS_CH4LOC */
#define _PRS_ROUTELOC1_CH4LOC_LOC0                 0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_LOC1                 0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH4LOC_LOC2                 0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_LOC0                  (_PRS_ROUTELOC1_CH4LOC_LOC0 << 0)     /**< Shifted mode LOC0 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_DEFAULT               (_PRS_ROUTELOC1_CH4LOC_DEFAULT << 0)  /**< Shifted mode DEFAULT for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_LOC1                  (_PRS_ROUTELOC1_CH4LOC_LOC1 << 0)     /**< Shifted mode LOC1 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH4LOC_LOC2                  (_PRS_ROUTELOC1_CH4LOC_LOC2 << 0)     /**< Shifted mode LOC2 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_SHIFT                8                                     /**< Shift value for PRS_CH5LOC */
#define _PRS_ROUTELOC1_CH5LOC_MASK                 0x300UL                               /**< Bit mask for PRS_CH5LOC */
#define _PRS_ROUTELOC1_CH5LOC_LOC0                 0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_LOC1                 0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH5LOC_LOC2                 0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_LOC0                  (_PRS_ROUTELOC1_CH5LOC_LOC0 << 8)     /**< Shifted mode LOC0 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_DEFAULT               (_PRS_ROUTELOC1_CH5LOC_DEFAULT << 8)  /**< Shifted mode DEFAULT for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_LOC1                  (_PRS_ROUTELOC1_CH5LOC_LOC1 << 8)     /**< Shifted mode LOC1 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH5LOC_LOC2                  (_PRS_ROUTELOC1_CH5LOC_LOC2 << 8)     /**< Shifted mode LOC2 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_SHIFT                16                                    /**< Shift value for PRS_CH6LOC */
#define _PRS_ROUTELOC1_CH6LOC_MASK                 0x30000UL                             /**< Bit mask for PRS_CH6LOC */
#define _PRS_ROUTELOC1_CH6LOC_LOC0                 0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC1                 0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH6LOC_LOC2                 0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC0                  (_PRS_ROUTELOC1_CH6LOC_LOC0 << 16)    /**< Shifted mode LOC0 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_DEFAULT               (_PRS_ROUTELOC1_CH6LOC_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC1                  (_PRS_ROUTELOC1_CH6LOC_LOC1 << 16)    /**< Shifted mode LOC1 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH6LOC_LOC2                  (_PRS_ROUTELOC1_CH6LOC_LOC2 << 16)    /**< Shifted mode LOC2 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_SHIFT                24                                    /**< Shift value for PRS_CH7LOC */
#define _PRS_ROUTELOC1_CH7LOC_MASK                 0x3000000UL                           /**< Bit mask for PRS_CH7LOC */
#define _PRS_ROUTELOC1_CH7LOC_LOC0                 0x00000000UL                          /**< Mode LOC0 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC1                 0x00000001UL                          /**< Mode LOC1 for PRS_ROUTELOC1 */
#define _PRS_ROUTELOC1_CH7LOC_LOC2                 0x00000002UL                          /**< Mode LOC2 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC0                  (_PRS_ROUTELOC1_CH7LOC_LOC0 << 24)    /**< Shifted mode LOC0 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_DEFAULT               (_PRS_ROUTELOC1_CH7LOC_DEFAULT << 24) /**< Shifted mode DEFAULT for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC1                  (_PRS_ROUTELOC1_CH7LOC_LOC1 << 24)    /**< Shifted mode LOC1 for PRS_ROUTELOC1 */
#define PRS_ROUTELOC1_CH7LOC_LOC2                  (_PRS_ROUTELOC1_CH7LOC_LOC2 << 24)    /**< Shifted mode LOC2 for PRS_ROUTELOC1 */

/* Bit fields for PRS ROUTELOC2 */
#define _PRS_ROUTELOC2_RESETVALUE                  0x00000000UL                           /**< Default value for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_MASK                        0x03030303UL                           /**< Mask for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_SHIFT                0                                      /**< Shift value for PRS_CH8LOC */
#define _PRS_ROUTELOC2_CH8LOC_MASK                 0x3UL                                  /**< Bit mask for PRS_CH8LOC */
#define _PRS_ROUTELOC2_CH8LOC_LOC0                 0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC1                 0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH8LOC_LOC2                 0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC0                  (_PRS_ROUTELOC2_CH8LOC_LOC0 << 0)      /**< Shifted mode LOC0 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_DEFAULT               (_PRS_ROUTELOC2_CH8LOC_DEFAULT << 0)   /**< Shifted mode DEFAULT for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC1                  (_PRS_ROUTELOC2_CH8LOC_LOC1 << 0)      /**< Shifted mode LOC1 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH8LOC_LOC2                  (_PRS_ROUTELOC2_CH8LOC_LOC2 << 0)      /**< Shifted mode LOC2 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_SHIFT                8                                      /**< Shift value for PRS_CH9LOC */
#define _PRS_ROUTELOC2_CH9LOC_MASK                 0x300UL                                /**< Bit mask for PRS_CH9LOC */
#define _PRS_ROUTELOC2_CH9LOC_LOC0                 0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC1                 0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH9LOC_LOC2                 0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC0                  (_PRS_ROUTELOC2_CH9LOC_LOC0 << 8)      /**< Shifted mode LOC0 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_DEFAULT               (_PRS_ROUTELOC2_CH9LOC_DEFAULT << 8)   /**< Shifted mode DEFAULT for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC1                  (_PRS_ROUTELOC2_CH9LOC_LOC1 << 8)      /**< Shifted mode LOC1 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH9LOC_LOC2                  (_PRS_ROUTELOC2_CH9LOC_LOC2 << 8)      /**< Shifted mode LOC2 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH10LOC_SHIFT               16                                     /**< Shift value for PRS_CH10LOC */
#define _PRS_ROUTELOC2_CH10LOC_MASK                0x30000UL                              /**< Bit mask for PRS_CH10LOC */
#define _PRS_ROUTELOC2_CH10LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH10LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH10LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH10LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH10LOC_LOC0                 (_PRS_ROUTELOC2_CH10LOC_LOC0 << 16)    /**< Shifted mode LOC0 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH10LOC_DEFAULT              (_PRS_ROUTELOC2_CH10LOC_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH10LOC_LOC1                 (_PRS_ROUTELOC2_CH10LOC_LOC1 << 16)    /**< Shifted mode LOC1 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH10LOC_LOC2                 (_PRS_ROUTELOC2_CH10LOC_LOC2 << 16)    /**< Shifted mode LOC2 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH11LOC_SHIFT               24                                     /**< Shift value for PRS_CH11LOC */
#define _PRS_ROUTELOC2_CH11LOC_MASK                0x3000000UL                            /**< Bit mask for PRS_CH11LOC */
#define _PRS_ROUTELOC2_CH11LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH11LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH11LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC2 */
#define _PRS_ROUTELOC2_CH11LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH11LOC_LOC0                 (_PRS_ROUTELOC2_CH11LOC_LOC0 << 24)    /**< Shifted mode LOC0 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH11LOC_DEFAULT              (_PRS_ROUTELOC2_CH11LOC_DEFAULT << 24) /**< Shifted mode DEFAULT for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH11LOC_LOC1                 (_PRS_ROUTELOC2_CH11LOC_LOC1 << 24)    /**< Shifted mode LOC1 for PRS_ROUTELOC2 */
#define PRS_ROUTELOC2_CH11LOC_LOC2                 (_PRS_ROUTELOC2_CH11LOC_LOC2 << 24)    /**< Shifted mode LOC2 for PRS_ROUTELOC2 */

/* Bit fields for PRS ROUTELOC3 */
#define _PRS_ROUTELOC3_RESETVALUE                  0x00000000UL                           /**< Default value for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_MASK                        0x03030303UL                           /**< Mask for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH12LOC_SHIFT               0                                      /**< Shift value for PRS_CH12LOC */
#define _PRS_ROUTELOC3_CH12LOC_MASK                0x3UL                                  /**< Bit mask for PRS_CH12LOC */
#define _PRS_ROUTELOC3_CH12LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH12LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH12LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH12LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH12LOC_LOC0                 (_PRS_ROUTELOC3_CH12LOC_LOC0 << 0)     /**< Shifted mode LOC0 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH12LOC_DEFAULT              (_PRS_ROUTELOC3_CH12LOC_DEFAULT << 0)  /**< Shifted mode DEFAULT for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH12LOC_LOC1                 (_PRS_ROUTELOC3_CH12LOC_LOC1 << 0)     /**< Shifted mode LOC1 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH12LOC_LOC2                 (_PRS_ROUTELOC3_CH12LOC_LOC2 << 0)     /**< Shifted mode LOC2 for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH13LOC_SHIFT               8                                      /**< Shift value for PRS_CH13LOC */
#define _PRS_ROUTELOC3_CH13LOC_MASK                0x300UL                                /**< Bit mask for PRS_CH13LOC */
#define _PRS_ROUTELOC3_CH13LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH13LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH13LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH13LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH13LOC_LOC0                 (_PRS_ROUTELOC3_CH13LOC_LOC0 << 8)     /**< Shifted mode LOC0 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH13LOC_DEFAULT              (_PRS_ROUTELOC3_CH13LOC_DEFAULT << 8)  /**< Shifted mode DEFAULT for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH13LOC_LOC1                 (_PRS_ROUTELOC3_CH13LOC_LOC1 << 8)     /**< Shifted mode LOC1 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH13LOC_LOC2                 (_PRS_ROUTELOC3_CH13LOC_LOC2 << 8)     /**< Shifted mode LOC2 for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH14LOC_SHIFT               16                                     /**< Shift value for PRS_CH14LOC */
#define _PRS_ROUTELOC3_CH14LOC_MASK                0x30000UL                              /**< Bit mask for PRS_CH14LOC */
#define _PRS_ROUTELOC3_CH14LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH14LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH14LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH14LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH14LOC_LOC0                 (_PRS_ROUTELOC3_CH14LOC_LOC0 << 16)    /**< Shifted mode LOC0 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH14LOC_DEFAULT              (_PRS_ROUTELOC3_CH14LOC_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH14LOC_LOC1                 (_PRS_ROUTELOC3_CH14LOC_LOC1 << 16)    /**< Shifted mode LOC1 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH14LOC_LOC2                 (_PRS_ROUTELOC3_CH14LOC_LOC2 << 16)    /**< Shifted mode LOC2 for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH15LOC_SHIFT               24                                     /**< Shift value for PRS_CH15LOC */
#define _PRS_ROUTELOC3_CH15LOC_MASK                0x3000000UL                            /**< Bit mask for PRS_CH15LOC */
#define _PRS_ROUTELOC3_CH15LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH15LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH15LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC3 */
#define _PRS_ROUTELOC3_CH15LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH15LOC_LOC0                 (_PRS_ROUTELOC3_CH15LOC_LOC0 << 24)    /**< Shifted mode LOC0 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH15LOC_DEFAULT              (_PRS_ROUTELOC3_CH15LOC_DEFAULT << 24) /**< Shifted mode DEFAULT for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH15LOC_LOC1                 (_PRS_ROUTELOC3_CH15LOC_LOC1 << 24)    /**< Shifted mode LOC1 for PRS_ROUTELOC3 */
#define PRS_ROUTELOC3_CH15LOC_LOC2                 (_PRS_ROUTELOC3_CH15LOC_LOC2 << 24)    /**< Shifted mode LOC2 for PRS_ROUTELOC3 */

/* Bit fields for PRS ROUTELOC4 */
#define _PRS_ROUTELOC4_RESETVALUE                  0x00000000UL                           /**< Default value for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_MASK                        0x03030303UL                           /**< Mask for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH16LOC_SHIFT               0                                      /**< Shift value for PRS_CH16LOC */
#define _PRS_ROUTELOC4_CH16LOC_MASK                0x3UL                                  /**< Bit mask for PRS_CH16LOC */
#define _PRS_ROUTELOC4_CH16LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH16LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH16LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH16LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH16LOC_LOC0                 (_PRS_ROUTELOC4_CH16LOC_LOC0 << 0)     /**< Shifted mode LOC0 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH16LOC_DEFAULT              (_PRS_ROUTELOC4_CH16LOC_DEFAULT << 0)  /**< Shifted mode DEFAULT for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH16LOC_LOC1                 (_PRS_ROUTELOC4_CH16LOC_LOC1 << 0)     /**< Shifted mode LOC1 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH16LOC_LOC2                 (_PRS_ROUTELOC4_CH16LOC_LOC2 << 0)     /**< Shifted mode LOC2 for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH17LOC_SHIFT               8                                      /**< Shift value for PRS_CH17LOC */
#define _PRS_ROUTELOC4_CH17LOC_MASK                0x300UL                                /**< Bit mask for PRS_CH17LOC */
#define _PRS_ROUTELOC4_CH17LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH17LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH17LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH17LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH17LOC_LOC0                 (_PRS_ROUTELOC4_CH17LOC_LOC0 << 8)     /**< Shifted mode LOC0 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH17LOC_DEFAULT              (_PRS_ROUTELOC4_CH17LOC_DEFAULT << 8)  /**< Shifted mode DEFAULT for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH17LOC_LOC1                 (_PRS_ROUTELOC4_CH17LOC_LOC1 << 8)     /**< Shifted mode LOC1 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH17LOC_LOC2                 (_PRS_ROUTELOC4_CH17LOC_LOC2 << 8)     /**< Shifted mode LOC2 for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH18LOC_SHIFT               16                                     /**< Shift value for PRS_CH18LOC */
#define _PRS_ROUTELOC4_CH18LOC_MASK                0x30000UL                              /**< Bit mask for PRS_CH18LOC */
#define _PRS_ROUTELOC4_CH18LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH18LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH18LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH18LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH18LOC_LOC0                 (_PRS_ROUTELOC4_CH18LOC_LOC0 << 16)    /**< Shifted mode LOC0 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH18LOC_DEFAULT              (_PRS_ROUTELOC4_CH18LOC_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH18LOC_LOC1                 (_PRS_ROUTELOC4_CH18LOC_LOC1 << 16)    /**< Shifted mode LOC1 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH18LOC_LOC2                 (_PRS_ROUTELOC4_CH18LOC_LOC2 << 16)    /**< Shifted mode LOC2 for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH19LOC_SHIFT               24                                     /**< Shift value for PRS_CH19LOC */
#define _PRS_ROUTELOC4_CH19LOC_MASK                0x3000000UL                            /**< Bit mask for PRS_CH19LOC */
#define _PRS_ROUTELOC4_CH19LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH19LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH19LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC4 */
#define _PRS_ROUTELOC4_CH19LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH19LOC_LOC0                 (_PRS_ROUTELOC4_CH19LOC_LOC0 << 24)    /**< Shifted mode LOC0 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH19LOC_DEFAULT              (_PRS_ROUTELOC4_CH19LOC_DEFAULT << 24) /**< Shifted mode DEFAULT for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH19LOC_LOC1                 (_PRS_ROUTELOC4_CH19LOC_LOC1 << 24)    /**< Shifted mode LOC1 for PRS_ROUTELOC4 */
#define PRS_ROUTELOC4_CH19LOC_LOC2                 (_PRS_ROUTELOC4_CH19LOC_LOC2 << 24)    /**< Shifted mode LOC2 for PRS_ROUTELOC4 */

/* Bit fields for PRS ROUTELOC5 */
#define _PRS_ROUTELOC5_RESETVALUE                  0x00000000UL                           /**< Default value for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_MASK                        0x03030303UL                           /**< Mask for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH20LOC_SHIFT               0                                      /**< Shift value for PRS_CH20LOC */
#define _PRS_ROUTELOC5_CH20LOC_MASK                0x3UL                                  /**< Bit mask for PRS_CH20LOC */
#define _PRS_ROUTELOC5_CH20LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH20LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH20LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH20LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH20LOC_LOC0                 (_PRS_ROUTELOC5_CH20LOC_LOC0 << 0)     /**< Shifted mode LOC0 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH20LOC_DEFAULT              (_PRS_ROUTELOC5_CH20LOC_DEFAULT << 0)  /**< Shifted mode DEFAULT for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH20LOC_LOC1                 (_PRS_ROUTELOC5_CH20LOC_LOC1 << 0)     /**< Shifted mode LOC1 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH20LOC_LOC2                 (_PRS_ROUTELOC5_CH20LOC_LOC2 << 0)     /**< Shifted mode LOC2 for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH21LOC_SHIFT               8                                      /**< Shift value for PRS_CH21LOC */
#define _PRS_ROUTELOC5_CH21LOC_MASK                0x300UL                                /**< Bit mask for PRS_CH21LOC */
#define _PRS_ROUTELOC5_CH21LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH21LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH21LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH21LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH21LOC_LOC0                 (_PRS_ROUTELOC5_CH21LOC_LOC0 << 8)     /**< Shifted mode LOC0 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH21LOC_DEFAULT              (_PRS_ROUTELOC5_CH21LOC_DEFAULT << 8)  /**< Shifted mode DEFAULT for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH21LOC_LOC1                 (_PRS_ROUTELOC5_CH21LOC_LOC1 << 8)     /**< Shifted mode LOC1 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH21LOC_LOC2                 (_PRS_ROUTELOC5_CH21LOC_LOC2 << 8)     /**< Shifted mode LOC2 for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH22LOC_SHIFT               16                                     /**< Shift value for PRS_CH22LOC */
#define _PRS_ROUTELOC5_CH22LOC_MASK                0x30000UL                              /**< Bit mask for PRS_CH22LOC */
#define _PRS_ROUTELOC5_CH22LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH22LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH22LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH22LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH22LOC_LOC0                 (_PRS_ROUTELOC5_CH22LOC_LOC0 << 16)    /**< Shifted mode LOC0 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH22LOC_DEFAULT              (_PRS_ROUTELOC5_CH22LOC_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH22LOC_LOC1                 (_PRS_ROUTELOC5_CH22LOC_LOC1 << 16)    /**< Shifted mode LOC1 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH22LOC_LOC2                 (_PRS_ROUTELOC5_CH22LOC_LOC2 << 16)    /**< Shifted mode LOC2 for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH23LOC_SHIFT               24                                     /**< Shift value for PRS_CH23LOC */
#define _PRS_ROUTELOC5_CH23LOC_MASK                0x3000000UL                            /**< Bit mask for PRS_CH23LOC */
#define _PRS_ROUTELOC5_CH23LOC_LOC0                0x00000000UL                           /**< Mode LOC0 for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH23LOC_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH23LOC_LOC1                0x00000001UL                           /**< Mode LOC1 for PRS_ROUTELOC5 */
#define _PRS_ROUTELOC5_CH23LOC_LOC2                0x00000002UL                           /**< Mode LOC2 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH23LOC_LOC0                 (_PRS_ROUTELOC5_CH23LOC_LOC0 << 24)    /**< Shifted mode LOC0 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH23LOC_DEFAULT              (_PRS_ROUTELOC5_CH23LOC_DEFAULT << 24) /**< Shifted mode DEFAULT for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH23LOC_LOC1                 (_PRS_ROUTELOC5_CH23LOC_LOC1 << 24)    /**< Shifted mode LOC1 for PRS_ROUTELOC5 */
#define PRS_ROUTELOC5_CH23LOC_LOC2                 (_PRS_ROUTELOC5_CH23LOC_LOC2 << 24)    /**< Shifted mode LOC2 for PRS_ROUTELOC5 */

/* Bit fields for PRS CTRL */
#define _PRS_CTRL_RESETVALUE                       0x00000000UL                         /**< Default value for PRS_CTRL */
#define _PRS_CTRL_MASK                             0x0000003FUL                         /**< Mask for PRS_CTRL */
#define PRS_CTRL_SEVONPRS                          (0x1UL << 0)                         /**< Set Event on PRS */
#define _PRS_CTRL_SEVONPRS_SHIFT                   0                                    /**< Shift value for PRS_SEVONPRS */
#define _PRS_CTRL_SEVONPRS_MASK                    0x1UL                                /**< Bit mask for PRS_SEVONPRS */
#define _PRS_CTRL_SEVONPRS_DEFAULT                 0x00000000UL                         /**< Mode DEFAULT for PRS_CTRL */
#define PRS_CTRL_SEVONPRS_DEFAULT                  (_PRS_CTRL_SEVONPRS_DEFAULT << 0)    /**< Shifted mode DEFAULT for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_SHIFT                1                                    /**< Shift value for PRS_SEVONPRSSEL */
#define _PRS_CTRL_SEVONPRSSEL_MASK                 0x3EUL                               /**< Bit mask for PRS_SEVONPRSSEL */
#define _PRS_CTRL_SEVONPRSSEL_DEFAULT              0x00000000UL                         /**< Mode DEFAULT for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH0               0x00000000UL                         /**< Mode PRSCH0 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH1               0x00000001UL                         /**< Mode PRSCH1 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH2               0x00000002UL                         /**< Mode PRSCH2 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH3               0x00000003UL                         /**< Mode PRSCH3 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH4               0x00000004UL                         /**< Mode PRSCH4 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH5               0x00000005UL                         /**< Mode PRSCH5 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH6               0x00000006UL                         /**< Mode PRSCH6 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH7               0x00000007UL                         /**< Mode PRSCH7 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH8               0x00000008UL                         /**< Mode PRSCH8 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH9               0x00000009UL                         /**< Mode PRSCH9 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH10              0x0000000AUL                         /**< Mode PRSCH10 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH11              0x0000000BUL                         /**< Mode PRSCH11 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH12              0x0000000CUL                         /**< Mode PRSCH12 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH13              0x0000000DUL                         /**< Mode PRSCH13 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH14              0x0000000EUL                         /**< Mode PRSCH14 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH15              0x0000000FUL                         /**< Mode PRSCH15 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH16              0x00000010UL                         /**< Mode PRSCH16 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH17              0x00000011UL                         /**< Mode PRSCH17 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH18              0x00000012UL                         /**< Mode PRSCH18 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH19              0x00000013UL                         /**< Mode PRSCH19 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH20              0x00000014UL                         /**< Mode PRSCH20 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH21              0x00000015UL                         /**< Mode PRSCH21 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH22              0x00000016UL                         /**< Mode PRSCH22 for PRS_CTRL */
#define _PRS_CTRL_SEVONPRSSEL_PRSCH23              0x00000017UL                         /**< Mode PRSCH23 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_DEFAULT               (_PRS_CTRL_SEVONPRSSEL_DEFAULT << 1) /**< Shifted mode DEFAULT for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH0                (_PRS_CTRL_SEVONPRSSEL_PRSCH0 << 1)  /**< Shifted mode PRSCH0 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH1                (_PRS_CTRL_SEVONPRSSEL_PRSCH1 << 1)  /**< Shifted mode PRSCH1 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH2                (_PRS_CTRL_SEVONPRSSEL_PRSCH2 << 1)  /**< Shifted mode PRSCH2 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH3                (_PRS_CTRL_SEVONPRSSEL_PRSCH3 << 1)  /**< Shifted mode PRSCH3 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH4                (_PRS_CTRL_SEVONPRSSEL_PRSCH4 << 1)  /**< Shifted mode PRSCH4 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH5                (_PRS_CTRL_SEVONPRSSEL_PRSCH5 << 1)  /**< Shifted mode PRSCH5 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH6                (_PRS_CTRL_SEVONPRSSEL_PRSCH6 << 1)  /**< Shifted mode PRSCH6 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH7                (_PRS_CTRL_SEVONPRSSEL_PRSCH7 << 1)  /**< Shifted mode PRSCH7 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH8                (_PRS_CTRL_SEVONPRSSEL_PRSCH8 << 1)  /**< Shifted mode PRSCH8 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH9                (_PRS_CTRL_SEVONPRSSEL_PRSCH9 << 1)  /**< Shifted mode PRSCH9 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH10               (_PRS_CTRL_SEVONPRSSEL_PRSCH10 << 1) /**< Shifted mode PRSCH10 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH11               (_PRS_CTRL_SEVONPRSSEL_PRSCH11 << 1) /**< Shifted mode PRSCH11 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH12               (_PRS_CTRL_SEVONPRSSEL_PRSCH12 << 1) /**< Shifted mode PRSCH12 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH13               (_PRS_CTRL_SEVONPRSSEL_PRSCH13 << 1) /**< Shifted mode PRSCH13 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH14               (_PRS_CTRL_SEVONPRSSEL_PRSCH14 << 1) /**< Shifted mode PRSCH14 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH15               (_PRS_CTRL_SEVONPRSSEL_PRSCH15 << 1) /**< Shifted mode PRSCH15 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH16               (_PRS_CTRL_SEVONPRSSEL_PRSCH16 << 1) /**< Shifted mode PRSCH16 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH17               (_PRS_CTRL_SEVONPRSSEL_PRSCH17 << 1) /**< Shifted mode PRSCH17 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH18               (_PRS_CTRL_SEVONPRSSEL_PRSCH18 << 1) /**< Shifted mode PRSCH18 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH19               (_PRS_CTRL_SEVONPRSSEL_PRSCH19 << 1) /**< Shifted mode PRSCH19 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH20               (_PRS_CTRL_SEVONPRSSEL_PRSCH20 << 1) /**< Shifted mode PRSCH20 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH21               (_PRS_CTRL_SEVONPRSSEL_PRSCH21 << 1) /**< Shifted mode PRSCH21 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH22               (_PRS_CTRL_SEVONPRSSEL_PRSCH22 << 1) /**< Shifted mode PRSCH22 for PRS_CTRL */
#define PRS_CTRL_SEVONPRSSEL_PRSCH23               (_PRS_CTRL_SEVONPRSSEL_PRSCH23 << 1) /**< Shifted mode PRSCH23 for PRS_CTRL */

/* Bit fields for PRS DMAREQ0 */
#define _PRS_DMAREQ0_RESETVALUE                    0x00000000UL                       /**< Default value for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_MASK                          0x000007C0UL                       /**< Mask for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_SHIFT                  6                                  /**< Shift value for PRS_PRSSEL */
#define _PRS_DMAREQ0_PRSSEL_MASK                   0x7C0UL                            /**< Bit mask for PRS_PRSSEL */
#define _PRS_DMAREQ0_PRSSEL_DEFAULT                0x00000000UL                       /**< Mode DEFAULT for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH0                 0x00000000UL                       /**< Mode PRSCH0 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH1                 0x00000001UL                       /**< Mode PRSCH1 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH2                 0x00000002UL                       /**< Mode PRSCH2 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH3                 0x00000003UL                       /**< Mode PRSCH3 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH4                 0x00000004UL                       /**< Mode PRSCH4 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH5                 0x00000005UL                       /**< Mode PRSCH5 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH6                 0x00000006UL                       /**< Mode PRSCH6 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH7                 0x00000007UL                       /**< Mode PRSCH7 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH8                 0x00000008UL                       /**< Mode PRSCH8 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH9                 0x00000009UL                       /**< Mode PRSCH9 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH10                0x0000000AUL                       /**< Mode PRSCH10 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH11                0x0000000BUL                       /**< Mode PRSCH11 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH12                0x0000000CUL                       /**< Mode PRSCH12 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH13                0x0000000DUL                       /**< Mode PRSCH13 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH14                0x0000000EUL                       /**< Mode PRSCH14 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH15                0x0000000FUL                       /**< Mode PRSCH15 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH16                0x00000010UL                       /**< Mode PRSCH16 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH17                0x00000011UL                       /**< Mode PRSCH17 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH18                0x00000012UL                       /**< Mode PRSCH18 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH19                0x00000013UL                       /**< Mode PRSCH19 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH20                0x00000014UL                       /**< Mode PRSCH20 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH21                0x00000015UL                       /**< Mode PRSCH21 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH22                0x00000016UL                       /**< Mode PRSCH22 for PRS_DMAREQ0 */
#define _PRS_DMAREQ0_PRSSEL_PRSCH23                0x00000017UL                       /**< Mode PRSCH23 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_DEFAULT                 (_PRS_DMAREQ0_PRSSEL_DEFAULT << 6) /**< Shifted mode DEFAULT for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH0                  (_PRS_DMAREQ0_PRSSEL_PRSCH0 << 6)  /**< Shifted mode PRSCH0 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH1                  (_PRS_DMAREQ0_PRSSEL_PRSCH1 << 6)  /**< Shifted mode PRSCH1 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH2                  (_PRS_DMAREQ0_PRSSEL_PRSCH2 << 6)  /**< Shifted mode PRSCH2 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH3                  (_PRS_DMAREQ0_PRSSEL_PRSCH3 << 6)  /**< Shifted mode PRSCH3 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH4                  (_PRS_DMAREQ0_PRSSEL_PRSCH4 << 6)  /**< Shifted mode PRSCH4 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH5                  (_PRS_DMAREQ0_PRSSEL_PRSCH5 << 6)  /**< Shifted mode PRSCH5 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH6                  (_PRS_DMAREQ0_PRSSEL_PRSCH6 << 6)  /**< Shifted mode PRSCH6 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH7                  (_PRS_DMAREQ0_PRSSEL_PRSCH7 << 6)  /**< Shifted mode PRSCH7 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH8                  (_PRS_DMAREQ0_PRSSEL_PRSCH8 << 6)  /**< Shifted mode PRSCH8 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH9                  (_PRS_DMAREQ0_PRSSEL_PRSCH9 << 6)  /**< Shifted mode PRSCH9 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH10                 (_PRS_DMAREQ0_PRSSEL_PRSCH10 << 6) /**< Shifted mode PRSCH10 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH11                 (_PRS_DMAREQ0_PRSSEL_PRSCH11 << 6) /**< Shifted mode PRSCH11 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH12                 (_PRS_DMAREQ0_PRSSEL_PRSCH12 << 6) /**< Shifted mode PRSCH12 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH13                 (_PRS_DMAREQ0_PRSSEL_PRSCH13 << 6) /**< Shifted mode PRSCH13 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH14                 (_PRS_DMAREQ0_PRSSEL_PRSCH14 << 6) /**< Shifted mode PRSCH14 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH15                 (_PRS_DMAREQ0_PRSSEL_PRSCH15 << 6) /**< Shifted mode PRSCH15 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH16                 (_PRS_DMAREQ0_PRSSEL_PRSCH16 << 6) /**< Shifted mode PRSCH16 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH17                 (_PRS_DMAREQ0_PRSSEL_PRSCH17 << 6) /**< Shifted mode PRSCH17 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH18                 (_PRS_DMAREQ0_PRSSEL_PRSCH18 << 6) /**< Shifted mode PRSCH18 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH19                 (_PRS_DMAREQ0_PRSSEL_PRSCH19 << 6) /**< Shifted mode PRSCH19 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH20                 (_PRS_DMAREQ0_PRSSEL_PRSCH20 << 6) /**< Shifted mode PRSCH20 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH21                 (_PRS_DMAREQ0_PRSSEL_PRSCH21 << 6) /**< Shifted mode PRSCH21 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH22                 (_PRS_DMAREQ0_PRSSEL_PRSCH22 << 6) /**< Shifted mode PRSCH22 for PRS_DMAREQ0 */
#define PRS_DMAREQ0_PRSSEL_PRSCH23                 (_PRS_DMAREQ0_PRSSEL_PRSCH23 << 6) /**< Shifted mode PRSCH23 for PRS_DMAREQ0 */

/* Bit fields for PRS DMAREQ1 */
#define _PRS_DMAREQ1_RESETVALUE                    0x00000000UL                       /**< Default value for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_MASK                          0x000007C0UL                       /**< Mask for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_SHIFT                  6                                  /**< Shift value for PRS_PRSSEL */
#define _PRS_DMAREQ1_PRSSEL_MASK                   0x7C0UL                            /**< Bit mask for PRS_PRSSEL */
#define _PRS_DMAREQ1_PRSSEL_DEFAULT                0x00000000UL                       /**< Mode DEFAULT for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH0                 0x00000000UL                       /**< Mode PRSCH0 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH1                 0x00000001UL                       /**< Mode PRSCH1 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH2                 0x00000002UL                       /**< Mode PRSCH2 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH3                 0x00000003UL                       /**< Mode PRSCH3 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH4                 0x00000004UL                       /**< Mode PRSCH4 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH5                 0x00000005UL                       /**< Mode PRSCH5 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH6                 0x00000006UL                       /**< Mode PRSCH6 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH7                 0x00000007UL                       /**< Mode PRSCH7 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH8                 0x00000008UL                       /**< Mode PRSCH8 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH9                 0x00000009UL                       /**< Mode PRSCH9 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH10                0x0000000AUL                       /**< Mode PRSCH10 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH11                0x0000000BUL                       /**< Mode PRSCH11 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH12                0x0000000CUL                       /**< Mode PRSCH12 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH13                0x0000000DUL                       /**< Mode PRSCH13 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH14                0x0000000EUL                       /**< Mode PRSCH14 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH15                0x0000000FUL                       /**< Mode PRSCH15 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH16                0x00000010UL                       /**< Mode PRSCH16 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH17                0x00000011UL                       /**< Mode PRSCH17 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH18                0x00000012UL                       /**< Mode PRSCH18 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH19                0x00000013UL                       /**< Mode PRSCH19 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH20                0x00000014UL                       /**< Mode PRSCH20 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH21                0x00000015UL                       /**< Mode PRSCH21 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH22                0x00000016UL                       /**< Mode PRSCH22 for PRS_DMAREQ1 */
#define _PRS_DMAREQ1_PRSSEL_PRSCH23                0x00000017UL                       /**< Mode PRSCH23 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_DEFAULT                 (_PRS_DMAREQ1_PRSSEL_DEFAULT << 6) /**< Shifted mode DEFAULT for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH0                  (_PRS_DMAREQ1_PRSSEL_PRSCH0 << 6)  /**< Shifted mode PRSCH0 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH1                  (_PRS_DMAREQ1_PRSSEL_PRSCH1 << 6)  /**< Shifted mode PRSCH1 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH2                  (_PRS_DMAREQ1_PRSSEL_PRSCH2 << 6)  /**< Shifted mode PRSCH2 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH3                  (_PRS_DMAREQ1_PRSSEL_PRSCH3 << 6)  /**< Shifted mode PRSCH3 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH4                  (_PRS_DMAREQ1_PRSSEL_PRSCH4 << 6)  /**< Shifted mode PRSCH4 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH5                  (_PRS_DMAREQ1_PRSSEL_PRSCH5 << 6)  /**< Shifted mode PRSCH5 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH6                  (_PRS_DMAREQ1_PRSSEL_PRSCH6 << 6)  /**< Shifted mode PRSCH6 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH7                  (_PRS_DMAREQ1_PRSSEL_PRSCH7 << 6)  /**< Shifted mode PRSCH7 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH8                  (_PRS_DMAREQ1_PRSSEL_PRSCH8 << 6)  /**< Shifted mode PRSCH8 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH9                  (_PRS_DMAREQ1_PRSSEL_PRSCH9 << 6)  /**< Shifted mode PRSCH9 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH10                 (_PRS_DMAREQ1_PRSSEL_PRSCH10 << 6) /**< Shifted mode PRSCH10 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH11                 (_PRS_DMAREQ1_PRSSEL_PRSCH11 << 6) /**< Shifted mode PRSCH11 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH12                 (_PRS_DMAREQ1_PRSSEL_PRSCH12 << 6) /**< Shifted mode PRSCH12 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH13                 (_PRS_DMAREQ1_PRSSEL_PRSCH13 << 6) /**< Shifted mode PRSCH13 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH14                 (_PRS_DMAREQ1_PRSSEL_PRSCH14 << 6) /**< Shifted mode PRSCH14 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH15                 (_PRS_DMAREQ1_PRSSEL_PRSCH15 << 6) /**< Shifted mode PRSCH15 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH16                 (_PRS_DMAREQ1_PRSSEL_PRSCH16 << 6) /**< Shifted mode PRSCH16 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH17                 (_PRS_DMAREQ1_PRSSEL_PRSCH17 << 6) /**< Shifted mode PRSCH17 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH18                 (_PRS_DMAREQ1_PRSSEL_PRSCH18 << 6) /**< Shifted mode PRSCH18 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH19                 (_PRS_DMAREQ1_PRSSEL_PRSCH19 << 6) /**< Shifted mode PRSCH19 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH20                 (_PRS_DMAREQ1_PRSSEL_PRSCH20 << 6) /**< Shifted mode PRSCH20 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH21                 (_PRS_DMAREQ1_PRSSEL_PRSCH21 << 6) /**< Shifted mode PRSCH21 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH22                 (_PRS_DMAREQ1_PRSSEL_PRSCH22 << 6) /**< Shifted mode PRSCH22 for PRS_DMAREQ1 */
#define PRS_DMAREQ1_PRSSEL_PRSCH23                 (_PRS_DMAREQ1_PRSSEL_PRSCH23 << 6) /**< Shifted mode PRSCH23 for PRS_DMAREQ1 */

/* Bit fields for PRS PEEK */
#define _PRS_PEEK_RESETVALUE                       0x00000000UL                      /**< Default value for PRS_PEEK */
#define _PRS_PEEK_MASK                             0x00FFFFFFUL                      /**< Mask for PRS_PEEK */
#define PRS_PEEK_CH0VAL                            (0x1UL << 0)                      /**< Channel 0 Current Value */
#define _PRS_PEEK_CH0VAL_SHIFT                     0                                 /**< Shift value for PRS_CH0VAL */
#define _PRS_PEEK_CH0VAL_MASK                      0x1UL                             /**< Bit mask for PRS_CH0VAL */
#define _PRS_PEEK_CH0VAL_DEFAULT                   0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH0VAL_DEFAULT                    (_PRS_PEEK_CH0VAL_DEFAULT << 0)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH1VAL                            (0x1UL << 1)                      /**< Channel 1 Current Value */
#define _PRS_PEEK_CH1VAL_SHIFT                     1                                 /**< Shift value for PRS_CH1VAL */
#define _PRS_PEEK_CH1VAL_MASK                      0x2UL                             /**< Bit mask for PRS_CH1VAL */
#define _PRS_PEEK_CH1VAL_DEFAULT                   0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH1VAL_DEFAULT                    (_PRS_PEEK_CH1VAL_DEFAULT << 1)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH2VAL                            (0x1UL << 2)                      /**< Channel 2 Current Value */
#define _PRS_PEEK_CH2VAL_SHIFT                     2                                 /**< Shift value for PRS_CH2VAL */
#define _PRS_PEEK_CH2VAL_MASK                      0x4UL                             /**< Bit mask for PRS_CH2VAL */
#define _PRS_PEEK_CH2VAL_DEFAULT                   0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH2VAL_DEFAULT                    (_PRS_PEEK_CH2VAL_DEFAULT << 2)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH3VAL                            (0x1UL << 3)                      /**< Channel 3 Current Value */
#define _PRS_PEEK_CH3VAL_SHIFT                     3                                 /**< Shift value for PRS_CH3VAL */
#define _PRS_PEEK_CH3VAL_MASK                      0x8UL                             /**< Bit mask for PRS_CH3VAL */
#define _PRS_PEEK_CH3VAL_DEFAULT                   0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH3VAL_DEFAULT                    (_PRS_PEEK_CH3VAL_DEFAULT << 3)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH4VAL                            (0x1UL << 4)                      /**< Channel 4 Current Value */
#define _PRS_PEEK_CH4VAL_SHIFT                     4                                 /**< Shift value for PRS_CH4VAL */
#define _PRS_PEEK_CH4VAL_MASK                      0x10UL                            /**< Bit mask for PRS_CH4VAL */
#define _PRS_PEEK_CH4VAL_DEFAULT                   0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH4VAL_DEFAULT                    (_PRS_PEEK_CH4VAL_DEFAULT << 4)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH5VAL                            (0x1UL << 5)                      /**< Channel 5 Current Value */
#define _PRS_PEEK_CH5VAL_SHIFT                     5                                 /**< Shift value for PRS_CH5VAL */
#define _PRS_PEEK_CH5VAL_MASK                      0x20UL                            /**< Bit mask for PRS_CH5VAL */
#define _PRS_PEEK_CH5VAL_DEFAULT                   0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH5VAL_DEFAULT                    (_PRS_PEEK_CH5VAL_DEFAULT << 5)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH6VAL                            (0x1UL << 6)                      /**< Channel 6 Current Value */
#define _PRS_PEEK_CH6VAL_SHIFT                     6                                 /**< Shift value for PRS_CH6VAL */
#define _PRS_PEEK_CH6VAL_MASK                      0x40UL                            /**< Bit mask for PRS_CH6VAL */
#define _PRS_PEEK_CH6VAL_DEFAULT                   0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH6VAL_DEFAULT                    (_PRS_PEEK_CH6VAL_DEFAULT << 6)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH7VAL                            (0x1UL << 7)                      /**< Channel 7 Current Value */
#define _PRS_PEEK_CH7VAL_SHIFT                     7                                 /**< Shift value for PRS_CH7VAL */
#define _PRS_PEEK_CH7VAL_MASK                      0x80UL                            /**< Bit mask for PRS_CH7VAL */
#define _PRS_PEEK_CH7VAL_DEFAULT                   0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH7VAL_DEFAULT                    (_PRS_PEEK_CH7VAL_DEFAULT << 7)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH8VAL                            (0x1UL << 8)                      /**< Channel 8 Current Value */
#define _PRS_PEEK_CH8VAL_SHIFT                     8                                 /**< Shift value for PRS_CH8VAL */
#define _PRS_PEEK_CH8VAL_MASK                      0x100UL                           /**< Bit mask for PRS_CH8VAL */
#define _PRS_PEEK_CH8VAL_DEFAULT                   0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH8VAL_DEFAULT                    (_PRS_PEEK_CH8VAL_DEFAULT << 8)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH9VAL                            (0x1UL << 9)                      /**< Channel 9 Current Value */
#define _PRS_PEEK_CH9VAL_SHIFT                     9                                 /**< Shift value for PRS_CH9VAL */
#define _PRS_PEEK_CH9VAL_MASK                      0x200UL                           /**< Bit mask for PRS_CH9VAL */
#define _PRS_PEEK_CH9VAL_DEFAULT                   0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH9VAL_DEFAULT                    (_PRS_PEEK_CH9VAL_DEFAULT << 9)   /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH10VAL                           (0x1UL << 10)                     /**< Channel 10 Current Value */
#define _PRS_PEEK_CH10VAL_SHIFT                    10                                /**< Shift value for PRS_CH10VAL */
#define _PRS_PEEK_CH10VAL_MASK                     0x400UL                           /**< Bit mask for PRS_CH10VAL */
#define _PRS_PEEK_CH10VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH10VAL_DEFAULT                   (_PRS_PEEK_CH10VAL_DEFAULT << 10) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH11VAL                           (0x1UL << 11)                     /**< Channel 11 Current Value */
#define _PRS_PEEK_CH11VAL_SHIFT                    11                                /**< Shift value for PRS_CH11VAL */
#define _PRS_PEEK_CH11VAL_MASK                     0x800UL                           /**< Bit mask for PRS_CH11VAL */
#define _PRS_PEEK_CH11VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH11VAL_DEFAULT                   (_PRS_PEEK_CH11VAL_DEFAULT << 11) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH12VAL                           (0x1UL << 12)                     /**< Channel 12 Current Value */
#define _PRS_PEEK_CH12VAL_SHIFT                    12                                /**< Shift value for PRS_CH12VAL */
#define _PRS_PEEK_CH12VAL_MASK                     0x1000UL                          /**< Bit mask for PRS_CH12VAL */
#define _PRS_PEEK_CH12VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH12VAL_DEFAULT                   (_PRS_PEEK_CH12VAL_DEFAULT << 12) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH13VAL                           (0x1UL << 13)                     /**< Channel 13 Current Value */
#define _PRS_PEEK_CH13VAL_SHIFT                    13                                /**< Shift value for PRS_CH13VAL */
#define _PRS_PEEK_CH13VAL_MASK                     0x2000UL                          /**< Bit mask for PRS_CH13VAL */
#define _PRS_PEEK_CH13VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH13VAL_DEFAULT                   (_PRS_PEEK_CH13VAL_DEFAULT << 13) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH14VAL                           (0x1UL << 14)                     /**< Channel 14 Current Value */
#define _PRS_PEEK_CH14VAL_SHIFT                    14                                /**< Shift value for PRS_CH14VAL */
#define _PRS_PEEK_CH14VAL_MASK                     0x4000UL                          /**< Bit mask for PRS_CH14VAL */
#define _PRS_PEEK_CH14VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH14VAL_DEFAULT                   (_PRS_PEEK_CH14VAL_DEFAULT << 14) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH15VAL                           (0x1UL << 15)                     /**< Channel 15 Current Value */
#define _PRS_PEEK_CH15VAL_SHIFT                    15                                /**< Shift value for PRS_CH15VAL */
#define _PRS_PEEK_CH15VAL_MASK                     0x8000UL                          /**< Bit mask for PRS_CH15VAL */
#define _PRS_PEEK_CH15VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH15VAL_DEFAULT                   (_PRS_PEEK_CH15VAL_DEFAULT << 15) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH16VAL                           (0x1UL << 16)                     /**< Channel 16 Current Value */
#define _PRS_PEEK_CH16VAL_SHIFT                    16                                /**< Shift value for PRS_CH16VAL */
#define _PRS_PEEK_CH16VAL_MASK                     0x10000UL                         /**< Bit mask for PRS_CH16VAL */
#define _PRS_PEEK_CH16VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH16VAL_DEFAULT                   (_PRS_PEEK_CH16VAL_DEFAULT << 16) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH17VAL                           (0x1UL << 17)                     /**< Channel 17 Current Value */
#define _PRS_PEEK_CH17VAL_SHIFT                    17                                /**< Shift value for PRS_CH17VAL */
#define _PRS_PEEK_CH17VAL_MASK                     0x20000UL                         /**< Bit mask for PRS_CH17VAL */
#define _PRS_PEEK_CH17VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH17VAL_DEFAULT                   (_PRS_PEEK_CH17VAL_DEFAULT << 17) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH18VAL                           (0x1UL << 18)                     /**< Channel 18 Current Value */
#define _PRS_PEEK_CH18VAL_SHIFT                    18                                /**< Shift value for PRS_CH18VAL */
#define _PRS_PEEK_CH18VAL_MASK                     0x40000UL                         /**< Bit mask for PRS_CH18VAL */
#define _PRS_PEEK_CH18VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH18VAL_DEFAULT                   (_PRS_PEEK_CH18VAL_DEFAULT << 18) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH19VAL                           (0x1UL << 19)                     /**< Channel 19 Current Value */
#define _PRS_PEEK_CH19VAL_SHIFT                    19                                /**< Shift value for PRS_CH19VAL */
#define _PRS_PEEK_CH19VAL_MASK                     0x80000UL                         /**< Bit mask for PRS_CH19VAL */
#define _PRS_PEEK_CH19VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH19VAL_DEFAULT                   (_PRS_PEEK_CH19VAL_DEFAULT << 19) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH20VAL                           (0x1UL << 20)                     /**< Channel 20 Current Value */
#define _PRS_PEEK_CH20VAL_SHIFT                    20                                /**< Shift value for PRS_CH20VAL */
#define _PRS_PEEK_CH20VAL_MASK                     0x100000UL                        /**< Bit mask for PRS_CH20VAL */
#define _PRS_PEEK_CH20VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH20VAL_DEFAULT                   (_PRS_PEEK_CH20VAL_DEFAULT << 20) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH21VAL                           (0x1UL << 21)                     /**< Channel 21 Current Value */
#define _PRS_PEEK_CH21VAL_SHIFT                    21                                /**< Shift value for PRS_CH21VAL */
#define _PRS_PEEK_CH21VAL_MASK                     0x200000UL                        /**< Bit mask for PRS_CH21VAL */
#define _PRS_PEEK_CH21VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH21VAL_DEFAULT                   (_PRS_PEEK_CH21VAL_DEFAULT << 21) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH22VAL                           (0x1UL << 22)                     /**< Channel 22 Current Value */
#define _PRS_PEEK_CH22VAL_SHIFT                    22                                /**< Shift value for PRS_CH22VAL */
#define _PRS_PEEK_CH22VAL_MASK                     0x400000UL                        /**< Bit mask for PRS_CH22VAL */
#define _PRS_PEEK_CH22VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH22VAL_DEFAULT                   (_PRS_PEEK_CH22VAL_DEFAULT << 22) /**< Shifted mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH23VAL                           (0x1UL << 23)                     /**< Channel 23 Current Value */
#define _PRS_PEEK_CH23VAL_SHIFT                    23                                /**< Shift value for PRS_CH23VAL */
#define _PRS_PEEK_CH23VAL_MASK                     0x800000UL                        /**< Bit mask for PRS_CH23VAL */
#define _PRS_PEEK_CH23VAL_DEFAULT                  0x00000000UL                      /**< Mode DEFAULT for PRS_PEEK */
#define PRS_PEEK_CH23VAL_DEFAULT                   (_PRS_PEEK_CH23VAL_DEFAULT << 23) /**< Shifted mode DEFAULT for PRS_PEEK */

/* Bit fields for PRS CH_CTRL */
#define _PRS_CH_CTRL_RESETVALUE                    0x00000000UL                                   /**< Default value for PRS_CH_CTRL */
#define _PRS_CH_CTRL_MASK                          0x5E307F07UL                                   /**< Mask for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_SHIFT                  0                                              /**< Shift value for PRS_SIGSEL */
#define _PRS_CH_CTRL_SIGSEL_MASK                   0x7UL                                          /**< Bit mask for PRS_SIGSEL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH0                 0x00000000UL                                   /**< Mode PRSCH0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH8                 0x00000000UL                                   /**< Mode PRSCH8 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH16                0x00000000UL                                   /**< Mode PRSCH16 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_ACMP0OUT               0x00000000UL                                   /**< Mode ACMP0OUT for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_ACMP1OUT               0x00000000UL                                   /**< Mode ACMP1OUT for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_ADC0SINGLE             0x00000000UL                                   /**< Mode ADC0SINGLE for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCOF                  0x00000000UL                                   /**< Mode RTCOF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN0               0x00000000UL                                   /**< Mode GPIOPIN0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN8               0x00000000UL                                   /**< Mode GPIOPIN8 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LETIMER0CH0            0x00000000UL                                   /**< Mode LETIMER0CH0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LETIMER1CH0            0x00000000UL                                   /**< Mode LETIMER1CH0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PCNT0TCC               0x00000000UL                                   /**< Mode PCNT0TCC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PCNT1TCC               0x00000000UL                                   /**< Mode PCNT1TCC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PCNT2TCC               0x00000000UL                                   /**< Mode PCNT2TCC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_CRYOTIMERPERIOD        0x00000000UL                                   /**< Mode CRYOTIMERPERIOD for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_CMUCLKOUT0             0x00000000UL                                   /**< Mode CMUCLKOUT0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_VDAC0CH0               0x00000000UL                                   /**< Mode VDAC0CH0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES0        0x00000000UL                                   /**< Mode LESENSESCANRES0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES8        0x00000000UL                                   /**< Mode LESENSESCANRES8 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSEDEC0            0x00000000UL                                   /**< Mode LESENSEDEC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSEMEASACT         0x00000000UL                                   /**< Mode LESENSEMEASACT for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_ACMP2OUT               0x00000000UL                                   /**< Mode ACMP2OUT for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_ACMP3OUT               0x00000000UL                                   /**< Mode ACMP3OUT for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_ADC1SINGLE             0x00000000UL                                   /**< Mode ADC1SINGLE for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0IRTX             0x00000000UL                                   /**< Mode USART0IRTX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART2IRTX             0x00000000UL                                   /**< Mode USART2IRTX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER0UF               0x00000000UL                                   /**< Mode TIMER0UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1UF               0x00000000UL                                   /**< Mode TIMER1UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER2UF               0x00000000UL                                   /**< Mode TIMER2UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USBSOF                 0x00000000UL                                   /**< Mode USBSOF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_CM4TXEV                0x00000000UL                                   /**< Mode CM4TXEV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER3UF               0x00000000UL                                   /**< Mode TIMER3UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER0UF              0x00000000UL                                   /**< Mode WTIMER0UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER1UF              0x00000000UL                                   /**< Mode WTIMER1UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER2UF              0x00000000UL                                   /**< Mode WTIMER2UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER3UF              0x00000000UL                                   /**< Mode WTIMER3UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER4UF               0x00000000UL                                   /**< Mode TIMER4UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER5UF               0x00000000UL                                   /**< Mode TIMER5UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER6UF               0x00000000UL                                   /**< Mode TIMER6UF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH1                 0x00000001UL                                   /**< Mode PRSCH1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH9                 0x00000001UL                                   /**< Mode PRSCH9 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH17                0x00000001UL                                   /**< Mode PRSCH17 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_ADC0SCAN               0x00000001UL                                   /**< Mode ADC0SCAN for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCOMP0               0x00000001UL                                   /**< Mode RTCCOMP0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCCCV0               0x00000001UL                                   /**< Mode RTCCCCV0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN1               0x00000001UL                                   /**< Mode GPIOPIN1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN9               0x00000001UL                                   /**< Mode GPIOPIN9 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LETIMER0CH1            0x00000001UL                                   /**< Mode LETIMER0CH1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LETIMER1CH1            0x00000001UL                                   /**< Mode LETIMER1CH1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PCNT0UFOF              0x00000001UL                                   /**< Mode PCNT0UFOF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PCNT1UFOF              0x00000001UL                                   /**< Mode PCNT1UFOF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PCNT2UFOF              0x00000001UL                                   /**< Mode PCNT2UFOF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_CMUCLKOUT1             0x00000001UL                                   /**< Mode CMUCLKOUT1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_VDAC0CH1               0x00000001UL                                   /**< Mode VDAC0CH1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES1        0x00000001UL                                   /**< Mode LESENSESCANRES1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES9        0x00000001UL                                   /**< Mode LESENSESCANRES9 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSEDEC1            0x00000001UL                                   /**< Mode LESENSEDEC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_ADC1SCAN               0x00000001UL                                   /**< Mode ADC1SCAN for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0TXC              0x00000001UL                                   /**< Mode USART0TXC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART1TXC              0x00000001UL                                   /**< Mode USART1TXC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART2TXC              0x00000001UL                                   /**< Mode USART2TXC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART3TXC              0x00000001UL                                   /**< Mode USART3TXC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART4TXC              0x00000001UL                                   /**< Mode USART4TXC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART5TXC              0x00000001UL                                   /**< Mode USART5TXC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_UART0TXC               0x00000001UL                                   /**< Mode UART0TXC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_UART1TXC               0x00000001UL                                   /**< Mode UART1TXC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER0OF               0x00000001UL                                   /**< Mode TIMER0OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1OF               0x00000001UL                                   /**< Mode TIMER1OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER2OF               0x00000001UL                                   /**< Mode TIMER2OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USBSOFSR               0x00000001UL                                   /**< Mode USBSOFSR for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_CM4ICACHEPCHITSOF      0x00000001UL                                   /**< Mode CM4ICACHEPCHITSOF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER3OF               0x00000001UL                                   /**< Mode TIMER3OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER0OF              0x00000001UL                                   /**< Mode WTIMER0OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER1OF              0x00000001UL                                   /**< Mode WTIMER1OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER2OF              0x00000001UL                                   /**< Mode WTIMER2OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER3OF              0x00000001UL                                   /**< Mode WTIMER3OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER4OF               0x00000001UL                                   /**< Mode TIMER4OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER5OF               0x00000001UL                                   /**< Mode TIMER5OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER6OF               0x00000001UL                                   /**< Mode TIMER6OF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH2                 0x00000002UL                                   /**< Mode PRSCH2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH10                0x00000002UL                                   /**< Mode PRSCH10 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH18                0x00000002UL                                   /**< Mode PRSCH18 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCOMP1               0x00000002UL                                   /**< Mode RTCCOMP1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCCCV1               0x00000002UL                                   /**< Mode RTCCCCV1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN2               0x00000002UL                                   /**< Mode GPIOPIN2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN10              0x00000002UL                                   /**< Mode GPIOPIN10 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PCNT0DIR               0x00000002UL                                   /**< Mode PCNT0DIR for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PCNT1DIR               0x00000002UL                                   /**< Mode PCNT1DIR for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PCNT2DIR               0x00000002UL                                   /**< Mode PCNT2DIR for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_VDAC0OPA0              0x00000002UL                                   /**< Mode VDAC0OPA0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES2        0x00000002UL                                   /**< Mode LESENSESCANRES2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES10       0x00000002UL                                   /**< Mode LESENSESCANRES10 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSEDEC2            0x00000002UL                                   /**< Mode LESENSEDEC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0RXDATAV          0x00000002UL                                   /**< Mode USART0RXDATAV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART1RXDATAV          0x00000002UL                                   /**< Mode USART1RXDATAV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART2RXDATAV          0x00000002UL                                   /**< Mode USART2RXDATAV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART3RXDATAV          0x00000002UL                                   /**< Mode USART3RXDATAV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART4RXDATAV          0x00000002UL                                   /**< Mode USART4RXDATAV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART5RXDATAV          0x00000002UL                                   /**< Mode USART5RXDATAV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_UART0RXDATAV           0x00000002UL                                   /**< Mode UART0RXDATAV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_UART1RXDATAV           0x00000002UL                                   /**< Mode UART1RXDATAV for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER0CC0              0x00000002UL                                   /**< Mode TIMER0CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1CC0              0x00000002UL                                   /**< Mode TIMER1CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER2CC0              0x00000002UL                                   /**< Mode TIMER2CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_CM4ICACHEPCMISSESOF    0x00000002UL                                   /**< Mode CM4ICACHEPCMISSESOF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER3CC0              0x00000002UL                                   /**< Mode TIMER3CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER0CC0             0x00000002UL                                   /**< Mode WTIMER0CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER1CC0             0x00000002UL                                   /**< Mode WTIMER1CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER2CC0             0x00000002UL                                   /**< Mode WTIMER2CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER3CC0             0x00000002UL                                   /**< Mode WTIMER3CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER4CC0              0x00000002UL                                   /**< Mode TIMER4CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER5CC0              0x00000002UL                                   /**< Mode TIMER5CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER6CC0              0x00000002UL                                   /**< Mode TIMER6CC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH3                 0x00000003UL                                   /**< Mode PRSCH3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH11                0x00000003UL                                   /**< Mode PRSCH11 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH19                0x00000003UL                                   /**< Mode PRSCH19 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCOMP2               0x00000003UL                                   /**< Mode RTCCOMP2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCCCV2               0x00000003UL                                   /**< Mode RTCCCCV2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN3               0x00000003UL                                   /**< Mode GPIOPIN3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN11              0x00000003UL                                   /**< Mode GPIOPIN11 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_VDAC0OPA1              0x00000003UL                                   /**< Mode VDAC0OPA1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES3        0x00000003UL                                   /**< Mode LESENSESCANRES3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES11       0x00000003UL                                   /**< Mode LESENSESCANRES11 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSEDECCMP          0x00000003UL                                   /**< Mode LESENSEDECCMP for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0RTS              0x00000003UL                                   /**< Mode USART0RTS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART1RTS              0x00000003UL                                   /**< Mode USART1RTS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART2RTS              0x00000003UL                                   /**< Mode USART2RTS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART3RTS              0x00000003UL                                   /**< Mode USART3RTS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART4RTS              0x00000003UL                                   /**< Mode USART4RTS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART5RTS              0x00000003UL                                   /**< Mode USART5RTS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_UART0RTS               0x00000003UL                                   /**< Mode UART0RTS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_UART1RTS               0x00000003UL                                   /**< Mode UART1RTS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER0CC1              0x00000003UL                                   /**< Mode TIMER0CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1CC1              0x00000003UL                                   /**< Mode TIMER1CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER2CC1              0x00000003UL                                   /**< Mode TIMER2CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER3CC1              0x00000003UL                                   /**< Mode TIMER3CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER0CC1             0x00000003UL                                   /**< Mode WTIMER0CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER1CC1             0x00000003UL                                   /**< Mode WTIMER1CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER2CC1             0x00000003UL                                   /**< Mode WTIMER2CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER3CC1             0x00000003UL                                   /**< Mode WTIMER3CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER4CC1              0x00000003UL                                   /**< Mode TIMER4CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER5CC1              0x00000003UL                                   /**< Mode TIMER5CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER6CC1              0x00000003UL                                   /**< Mode TIMER6CC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH4                 0x00000004UL                                   /**< Mode PRSCH4 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH12                0x00000004UL                                   /**< Mode PRSCH12 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH20                0x00000004UL                                   /**< Mode PRSCH20 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCOMP3               0x00000004UL                                   /**< Mode RTCCOMP3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN4               0x00000004UL                                   /**< Mode GPIOPIN4 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN12              0x00000004UL                                   /**< Mode GPIOPIN12 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_VDAC0OPA2              0x00000004UL                                   /**< Mode VDAC0OPA2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES4        0x00000004UL                                   /**< Mode LESENSESCANRES4 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES12       0x00000004UL                                   /**< Mode LESENSESCANRES12 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER0CC2              0x00000004UL                                   /**< Mode TIMER0CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1CC2              0x00000004UL                                   /**< Mode TIMER1CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER2CC2              0x00000004UL                                   /**< Mode TIMER2CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER3CC2              0x00000004UL                                   /**< Mode TIMER3CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER0CC2             0x00000004UL                                   /**< Mode WTIMER0CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER1CC2             0x00000004UL                                   /**< Mode WTIMER1CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER2CC2             0x00000004UL                                   /**< Mode WTIMER2CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER3CC2             0x00000004UL                                   /**< Mode WTIMER3CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER4CC2              0x00000004UL                                   /**< Mode TIMER4CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER5CC2              0x00000004UL                                   /**< Mode TIMER5CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER6CC2              0x00000004UL                                   /**< Mode TIMER6CC2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH5                 0x00000005UL                                   /**< Mode PRSCH5 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH13                0x00000005UL                                   /**< Mode PRSCH13 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH21                0x00000005UL                                   /**< Mode PRSCH21 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCOMP4               0x00000005UL                                   /**< Mode RTCCOMP4 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN5               0x00000005UL                                   /**< Mode GPIOPIN5 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN13              0x00000005UL                                   /**< Mode GPIOPIN13 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_VDAC0OPA3              0x00000005UL                                   /**< Mode VDAC0OPA3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES5        0x00000005UL                                   /**< Mode LESENSESCANRES5 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES13       0x00000005UL                                   /**< Mode LESENSESCANRES13 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0TX               0x00000005UL                                   /**< Mode USART0TX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART1TX               0x00000005UL                                   /**< Mode USART1TX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART2TX               0x00000005UL                                   /**< Mode USART2TX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART3TX               0x00000005UL                                   /**< Mode USART3TX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART4TX               0x00000005UL                                   /**< Mode USART4TX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART5TX               0x00000005UL                                   /**< Mode USART5TX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_UART0TX                0x00000005UL                                   /**< Mode UART0TX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_UART1TX                0x00000005UL                                   /**< Mode UART1TX for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_TIMER1CC3              0x00000005UL                                   /**< Mode TIMER1CC3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_WTIMER1CC3             0x00000005UL                                   /**< Mode WTIMER1CC3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH6                 0x00000006UL                                   /**< Mode PRSCH6 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH14                0x00000006UL                                   /**< Mode PRSCH14 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH22                0x00000006UL                                   /**< Mode PRSCH22 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_RTCCOMP5               0x00000006UL                                   /**< Mode RTCCOMP5 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN6               0x00000006UL                                   /**< Mode GPIOPIN6 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN14              0x00000006UL                                   /**< Mode GPIOPIN14 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES6        0x00000006UL                                   /**< Mode LESENSESCANRES6 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES14       0x00000006UL                                   /**< Mode LESENSESCANRES14 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART0CS               0x00000006UL                                   /**< Mode USART0CS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART1CS               0x00000006UL                                   /**< Mode USART1CS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART2CS               0x00000006UL                                   /**< Mode USART2CS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART3CS               0x00000006UL                                   /**< Mode USART3CS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART4CS               0x00000006UL                                   /**< Mode USART4CS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_USART5CS               0x00000006UL                                   /**< Mode USART5CS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_UART0CS                0x00000006UL                                   /**< Mode UART0CS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_UART1CS                0x00000006UL                                   /**< Mode UART1CS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH7                 0x00000007UL                                   /**< Mode PRSCH7 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH15                0x00000007UL                                   /**< Mode PRSCH15 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_PRSCH23                0x00000007UL                                   /**< Mode PRSCH23 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN7               0x00000007UL                                   /**< Mode GPIOPIN7 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_GPIOPIN15              0x00000007UL                                   /**< Mode GPIOPIN15 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_CMUCLKOUT2             0x00000007UL                                   /**< Mode CMUCLKOUT2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES7        0x00000007UL                                   /**< Mode LESENSESCANRES7 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SIGSEL_LESENSESCANRES15       0x00000007UL                                   /**< Mode LESENSESCANRES15 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH0                  (_PRS_CH_CTRL_SIGSEL_PRSCH0 << 0)              /**< Shifted mode PRSCH0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH8                  (_PRS_CH_CTRL_SIGSEL_PRSCH8 << 0)              /**< Shifted mode PRSCH8 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH16                 (_PRS_CH_CTRL_SIGSEL_PRSCH16 << 0)             /**< Shifted mode PRSCH16 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_ACMP0OUT                (_PRS_CH_CTRL_SIGSEL_ACMP0OUT << 0)            /**< Shifted mode ACMP0OUT for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_ACMP1OUT                (_PRS_CH_CTRL_SIGSEL_ACMP1OUT << 0)            /**< Shifted mode ACMP1OUT for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_ADC0SINGLE              (_PRS_CH_CTRL_SIGSEL_ADC0SINGLE << 0)          /**< Shifted mode ADC0SINGLE for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCOF                   (_PRS_CH_CTRL_SIGSEL_RTCOF << 0)               /**< Shifted mode RTCOF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN0                (_PRS_CH_CTRL_SIGSEL_GPIOPIN0 << 0)            /**< Shifted mode GPIOPIN0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN8                (_PRS_CH_CTRL_SIGSEL_GPIOPIN8 << 0)            /**< Shifted mode GPIOPIN8 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LETIMER0CH0             (_PRS_CH_CTRL_SIGSEL_LETIMER0CH0 << 0)         /**< Shifted mode LETIMER0CH0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LETIMER1CH0             (_PRS_CH_CTRL_SIGSEL_LETIMER1CH0 << 0)         /**< Shifted mode LETIMER1CH0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PCNT0TCC                (_PRS_CH_CTRL_SIGSEL_PCNT0TCC << 0)            /**< Shifted mode PCNT0TCC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PCNT1TCC                (_PRS_CH_CTRL_SIGSEL_PCNT1TCC << 0)            /**< Shifted mode PCNT1TCC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PCNT2TCC                (_PRS_CH_CTRL_SIGSEL_PCNT2TCC << 0)            /**< Shifted mode PCNT2TCC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_CRYOTIMERPERIOD         (_PRS_CH_CTRL_SIGSEL_CRYOTIMERPERIOD << 0)     /**< Shifted mode CRYOTIMERPERIOD for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_CMUCLKOUT0              (_PRS_CH_CTRL_SIGSEL_CMUCLKOUT0 << 0)          /**< Shifted mode CMUCLKOUT0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_VDAC0CH0                (_PRS_CH_CTRL_SIGSEL_VDAC0CH0 << 0)            /**< Shifted mode VDAC0CH0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES0         (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES0 << 0)     /**< Shifted mode LESENSESCANRES0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES8         (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES8 << 0)     /**< Shifted mode LESENSESCANRES8 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSEDEC0             (_PRS_CH_CTRL_SIGSEL_LESENSEDEC0 << 0)         /**< Shifted mode LESENSEDEC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSEMEASACT          (_PRS_CH_CTRL_SIGSEL_LESENSEMEASACT << 0)      /**< Shifted mode LESENSEMEASACT for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_ACMP2OUT                (_PRS_CH_CTRL_SIGSEL_ACMP2OUT << 0)            /**< Shifted mode ACMP2OUT for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_ACMP3OUT                (_PRS_CH_CTRL_SIGSEL_ACMP3OUT << 0)            /**< Shifted mode ACMP3OUT for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_ADC1SINGLE              (_PRS_CH_CTRL_SIGSEL_ADC1SINGLE << 0)          /**< Shifted mode ADC1SINGLE for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0IRTX              (_PRS_CH_CTRL_SIGSEL_USART0IRTX << 0)          /**< Shifted mode USART0IRTX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART2IRTX              (_PRS_CH_CTRL_SIGSEL_USART2IRTX << 0)          /**< Shifted mode USART2IRTX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER0UF                (_PRS_CH_CTRL_SIGSEL_TIMER0UF << 0)            /**< Shifted mode TIMER0UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1UF                (_PRS_CH_CTRL_SIGSEL_TIMER1UF << 0)            /**< Shifted mode TIMER1UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER2UF                (_PRS_CH_CTRL_SIGSEL_TIMER2UF << 0)            /**< Shifted mode TIMER2UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USBSOF                  (_PRS_CH_CTRL_SIGSEL_USBSOF << 0)              /**< Shifted mode USBSOF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_CM4TXEV                 (_PRS_CH_CTRL_SIGSEL_CM4TXEV << 0)             /**< Shifted mode CM4TXEV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER3UF                (_PRS_CH_CTRL_SIGSEL_TIMER3UF << 0)            /**< Shifted mode TIMER3UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER0UF               (_PRS_CH_CTRL_SIGSEL_WTIMER0UF << 0)           /**< Shifted mode WTIMER0UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER1UF               (_PRS_CH_CTRL_SIGSEL_WTIMER1UF << 0)           /**< Shifted mode WTIMER1UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER2UF               (_PRS_CH_CTRL_SIGSEL_WTIMER2UF << 0)           /**< Shifted mode WTIMER2UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER3UF               (_PRS_CH_CTRL_SIGSEL_WTIMER3UF << 0)           /**< Shifted mode WTIMER3UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER4UF                (_PRS_CH_CTRL_SIGSEL_TIMER4UF << 0)            /**< Shifted mode TIMER4UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER5UF                (_PRS_CH_CTRL_SIGSEL_TIMER5UF << 0)            /**< Shifted mode TIMER5UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER6UF                (_PRS_CH_CTRL_SIGSEL_TIMER6UF << 0)            /**< Shifted mode TIMER6UF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH1                  (_PRS_CH_CTRL_SIGSEL_PRSCH1 << 0)              /**< Shifted mode PRSCH1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH9                  (_PRS_CH_CTRL_SIGSEL_PRSCH9 << 0)              /**< Shifted mode PRSCH9 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH17                 (_PRS_CH_CTRL_SIGSEL_PRSCH17 << 0)             /**< Shifted mode PRSCH17 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_ADC0SCAN                (_PRS_CH_CTRL_SIGSEL_ADC0SCAN << 0)            /**< Shifted mode ADC0SCAN for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCOMP0                (_PRS_CH_CTRL_SIGSEL_RTCCOMP0 << 0)            /**< Shifted mode RTCCOMP0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCCCV0                (_PRS_CH_CTRL_SIGSEL_RTCCCCV0 << 0)            /**< Shifted mode RTCCCCV0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN1                (_PRS_CH_CTRL_SIGSEL_GPIOPIN1 << 0)            /**< Shifted mode GPIOPIN1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN9                (_PRS_CH_CTRL_SIGSEL_GPIOPIN9 << 0)            /**< Shifted mode GPIOPIN9 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LETIMER0CH1             (_PRS_CH_CTRL_SIGSEL_LETIMER0CH1 << 0)         /**< Shifted mode LETIMER0CH1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LETIMER1CH1             (_PRS_CH_CTRL_SIGSEL_LETIMER1CH1 << 0)         /**< Shifted mode LETIMER1CH1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PCNT0UFOF               (_PRS_CH_CTRL_SIGSEL_PCNT0UFOF << 0)           /**< Shifted mode PCNT0UFOF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PCNT1UFOF               (_PRS_CH_CTRL_SIGSEL_PCNT1UFOF << 0)           /**< Shifted mode PCNT1UFOF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PCNT2UFOF               (_PRS_CH_CTRL_SIGSEL_PCNT2UFOF << 0)           /**< Shifted mode PCNT2UFOF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_CMUCLKOUT1              (_PRS_CH_CTRL_SIGSEL_CMUCLKOUT1 << 0)          /**< Shifted mode CMUCLKOUT1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_VDAC0CH1                (_PRS_CH_CTRL_SIGSEL_VDAC0CH1 << 0)            /**< Shifted mode VDAC0CH1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES1         (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES1 << 0)     /**< Shifted mode LESENSESCANRES1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES9         (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES9 << 0)     /**< Shifted mode LESENSESCANRES9 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSEDEC1             (_PRS_CH_CTRL_SIGSEL_LESENSEDEC1 << 0)         /**< Shifted mode LESENSEDEC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_ADC1SCAN                (_PRS_CH_CTRL_SIGSEL_ADC1SCAN << 0)            /**< Shifted mode ADC1SCAN for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0TXC               (_PRS_CH_CTRL_SIGSEL_USART0TXC << 0)           /**< Shifted mode USART0TXC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART1TXC               (_PRS_CH_CTRL_SIGSEL_USART1TXC << 0)           /**< Shifted mode USART1TXC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART2TXC               (_PRS_CH_CTRL_SIGSEL_USART2TXC << 0)           /**< Shifted mode USART2TXC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART3TXC               (_PRS_CH_CTRL_SIGSEL_USART3TXC << 0)           /**< Shifted mode USART3TXC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART4TXC               (_PRS_CH_CTRL_SIGSEL_USART4TXC << 0)           /**< Shifted mode USART4TXC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART5TXC               (_PRS_CH_CTRL_SIGSEL_USART5TXC << 0)           /**< Shifted mode USART5TXC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_UART0TXC                (_PRS_CH_CTRL_SIGSEL_UART0TXC << 0)            /**< Shifted mode UART0TXC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_UART1TXC                (_PRS_CH_CTRL_SIGSEL_UART1TXC << 0)            /**< Shifted mode UART1TXC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER0OF                (_PRS_CH_CTRL_SIGSEL_TIMER0OF << 0)            /**< Shifted mode TIMER0OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1OF                (_PRS_CH_CTRL_SIGSEL_TIMER1OF << 0)            /**< Shifted mode TIMER1OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER2OF                (_PRS_CH_CTRL_SIGSEL_TIMER2OF << 0)            /**< Shifted mode TIMER2OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USBSOFSR                (_PRS_CH_CTRL_SIGSEL_USBSOFSR << 0)            /**< Shifted mode USBSOFSR for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_CM4ICACHEPCHITSOF       (_PRS_CH_CTRL_SIGSEL_CM4ICACHEPCHITSOF << 0)   /**< Shifted mode CM4ICACHEPCHITSOF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER3OF                (_PRS_CH_CTRL_SIGSEL_TIMER3OF << 0)            /**< Shifted mode TIMER3OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER0OF               (_PRS_CH_CTRL_SIGSEL_WTIMER0OF << 0)           /**< Shifted mode WTIMER0OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER1OF               (_PRS_CH_CTRL_SIGSEL_WTIMER1OF << 0)           /**< Shifted mode WTIMER1OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER2OF               (_PRS_CH_CTRL_SIGSEL_WTIMER2OF << 0)           /**< Shifted mode WTIMER2OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER3OF               (_PRS_CH_CTRL_SIGSEL_WTIMER3OF << 0)           /**< Shifted mode WTIMER3OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER4OF                (_PRS_CH_CTRL_SIGSEL_TIMER4OF << 0)            /**< Shifted mode TIMER4OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER5OF                (_PRS_CH_CTRL_SIGSEL_TIMER5OF << 0)            /**< Shifted mode TIMER5OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER6OF                (_PRS_CH_CTRL_SIGSEL_TIMER6OF << 0)            /**< Shifted mode TIMER6OF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH2                  (_PRS_CH_CTRL_SIGSEL_PRSCH2 << 0)              /**< Shifted mode PRSCH2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH10                 (_PRS_CH_CTRL_SIGSEL_PRSCH10 << 0)             /**< Shifted mode PRSCH10 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH18                 (_PRS_CH_CTRL_SIGSEL_PRSCH18 << 0)             /**< Shifted mode PRSCH18 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCOMP1                (_PRS_CH_CTRL_SIGSEL_RTCCOMP1 << 0)            /**< Shifted mode RTCCOMP1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCCCV1                (_PRS_CH_CTRL_SIGSEL_RTCCCCV1 << 0)            /**< Shifted mode RTCCCCV1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN2                (_PRS_CH_CTRL_SIGSEL_GPIOPIN2 << 0)            /**< Shifted mode GPIOPIN2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN10               (_PRS_CH_CTRL_SIGSEL_GPIOPIN10 << 0)           /**< Shifted mode GPIOPIN10 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PCNT0DIR                (_PRS_CH_CTRL_SIGSEL_PCNT0DIR << 0)            /**< Shifted mode PCNT0DIR for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PCNT1DIR                (_PRS_CH_CTRL_SIGSEL_PCNT1DIR << 0)            /**< Shifted mode PCNT1DIR for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PCNT2DIR                (_PRS_CH_CTRL_SIGSEL_PCNT2DIR << 0)            /**< Shifted mode PCNT2DIR for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_VDAC0OPA0               (_PRS_CH_CTRL_SIGSEL_VDAC0OPA0 << 0)           /**< Shifted mode VDAC0OPA0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES2         (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES2 << 0)     /**< Shifted mode LESENSESCANRES2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES10        (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES10 << 0)    /**< Shifted mode LESENSESCANRES10 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSEDEC2             (_PRS_CH_CTRL_SIGSEL_LESENSEDEC2 << 0)         /**< Shifted mode LESENSEDEC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0RXDATAV           (_PRS_CH_CTRL_SIGSEL_USART0RXDATAV << 0)       /**< Shifted mode USART0RXDATAV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART1RXDATAV           (_PRS_CH_CTRL_SIGSEL_USART1RXDATAV << 0)       /**< Shifted mode USART1RXDATAV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART2RXDATAV           (_PRS_CH_CTRL_SIGSEL_USART2RXDATAV << 0)       /**< Shifted mode USART2RXDATAV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART3RXDATAV           (_PRS_CH_CTRL_SIGSEL_USART3RXDATAV << 0)       /**< Shifted mode USART3RXDATAV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART4RXDATAV           (_PRS_CH_CTRL_SIGSEL_USART4RXDATAV << 0)       /**< Shifted mode USART4RXDATAV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART5RXDATAV           (_PRS_CH_CTRL_SIGSEL_USART5RXDATAV << 0)       /**< Shifted mode USART5RXDATAV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_UART0RXDATAV            (_PRS_CH_CTRL_SIGSEL_UART0RXDATAV << 0)        /**< Shifted mode UART0RXDATAV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_UART1RXDATAV            (_PRS_CH_CTRL_SIGSEL_UART1RXDATAV << 0)        /**< Shifted mode UART1RXDATAV for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER0CC0               (_PRS_CH_CTRL_SIGSEL_TIMER0CC0 << 0)           /**< Shifted mode TIMER0CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1CC0               (_PRS_CH_CTRL_SIGSEL_TIMER1CC0 << 0)           /**< Shifted mode TIMER1CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER2CC0               (_PRS_CH_CTRL_SIGSEL_TIMER2CC0 << 0)           /**< Shifted mode TIMER2CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_CM4ICACHEPCMISSESOF     (_PRS_CH_CTRL_SIGSEL_CM4ICACHEPCMISSESOF << 0) /**< Shifted mode CM4ICACHEPCMISSESOF for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER3CC0               (_PRS_CH_CTRL_SIGSEL_TIMER3CC0 << 0)           /**< Shifted mode TIMER3CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER0CC0              (_PRS_CH_CTRL_SIGSEL_WTIMER0CC0 << 0)          /**< Shifted mode WTIMER0CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER1CC0              (_PRS_CH_CTRL_SIGSEL_WTIMER1CC0 << 0)          /**< Shifted mode WTIMER1CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER2CC0              (_PRS_CH_CTRL_SIGSEL_WTIMER2CC0 << 0)          /**< Shifted mode WTIMER2CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER3CC0              (_PRS_CH_CTRL_SIGSEL_WTIMER3CC0 << 0)          /**< Shifted mode WTIMER3CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER4CC0               (_PRS_CH_CTRL_SIGSEL_TIMER4CC0 << 0)           /**< Shifted mode TIMER4CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER5CC0               (_PRS_CH_CTRL_SIGSEL_TIMER5CC0 << 0)           /**< Shifted mode TIMER5CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER6CC0               (_PRS_CH_CTRL_SIGSEL_TIMER6CC0 << 0)           /**< Shifted mode TIMER6CC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH3                  (_PRS_CH_CTRL_SIGSEL_PRSCH3 << 0)              /**< Shifted mode PRSCH3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH11                 (_PRS_CH_CTRL_SIGSEL_PRSCH11 << 0)             /**< Shifted mode PRSCH11 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH19                 (_PRS_CH_CTRL_SIGSEL_PRSCH19 << 0)             /**< Shifted mode PRSCH19 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCOMP2                (_PRS_CH_CTRL_SIGSEL_RTCCOMP2 << 0)            /**< Shifted mode RTCCOMP2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCCCV2                (_PRS_CH_CTRL_SIGSEL_RTCCCCV2 << 0)            /**< Shifted mode RTCCCCV2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN3                (_PRS_CH_CTRL_SIGSEL_GPIOPIN3 << 0)            /**< Shifted mode GPIOPIN3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN11               (_PRS_CH_CTRL_SIGSEL_GPIOPIN11 << 0)           /**< Shifted mode GPIOPIN11 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_VDAC0OPA1               (_PRS_CH_CTRL_SIGSEL_VDAC0OPA1 << 0)           /**< Shifted mode VDAC0OPA1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES3         (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES3 << 0)     /**< Shifted mode LESENSESCANRES3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES11        (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES11 << 0)    /**< Shifted mode LESENSESCANRES11 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSEDECCMP           (_PRS_CH_CTRL_SIGSEL_LESENSEDECCMP << 0)       /**< Shifted mode LESENSEDECCMP for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0RTS               (_PRS_CH_CTRL_SIGSEL_USART0RTS << 0)           /**< Shifted mode USART0RTS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART1RTS               (_PRS_CH_CTRL_SIGSEL_USART1RTS << 0)           /**< Shifted mode USART1RTS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART2RTS               (_PRS_CH_CTRL_SIGSEL_USART2RTS << 0)           /**< Shifted mode USART2RTS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART3RTS               (_PRS_CH_CTRL_SIGSEL_USART3RTS << 0)           /**< Shifted mode USART3RTS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART4RTS               (_PRS_CH_CTRL_SIGSEL_USART4RTS << 0)           /**< Shifted mode USART4RTS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART5RTS               (_PRS_CH_CTRL_SIGSEL_USART5RTS << 0)           /**< Shifted mode USART5RTS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_UART0RTS                (_PRS_CH_CTRL_SIGSEL_UART0RTS << 0)            /**< Shifted mode UART0RTS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_UART1RTS                (_PRS_CH_CTRL_SIGSEL_UART1RTS << 0)            /**< Shifted mode UART1RTS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER0CC1               (_PRS_CH_CTRL_SIGSEL_TIMER0CC1 << 0)           /**< Shifted mode TIMER0CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1CC1               (_PRS_CH_CTRL_SIGSEL_TIMER1CC1 << 0)           /**< Shifted mode TIMER1CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER2CC1               (_PRS_CH_CTRL_SIGSEL_TIMER2CC1 << 0)           /**< Shifted mode TIMER2CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER3CC1               (_PRS_CH_CTRL_SIGSEL_TIMER3CC1 << 0)           /**< Shifted mode TIMER3CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER0CC1              (_PRS_CH_CTRL_SIGSEL_WTIMER0CC1 << 0)          /**< Shifted mode WTIMER0CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER1CC1              (_PRS_CH_CTRL_SIGSEL_WTIMER1CC1 << 0)          /**< Shifted mode WTIMER1CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER2CC1              (_PRS_CH_CTRL_SIGSEL_WTIMER2CC1 << 0)          /**< Shifted mode WTIMER2CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER3CC1              (_PRS_CH_CTRL_SIGSEL_WTIMER3CC1 << 0)          /**< Shifted mode WTIMER3CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER4CC1               (_PRS_CH_CTRL_SIGSEL_TIMER4CC1 << 0)           /**< Shifted mode TIMER4CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER5CC1               (_PRS_CH_CTRL_SIGSEL_TIMER5CC1 << 0)           /**< Shifted mode TIMER5CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER6CC1               (_PRS_CH_CTRL_SIGSEL_TIMER6CC1 << 0)           /**< Shifted mode TIMER6CC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH4                  (_PRS_CH_CTRL_SIGSEL_PRSCH4 << 0)              /**< Shifted mode PRSCH4 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH12                 (_PRS_CH_CTRL_SIGSEL_PRSCH12 << 0)             /**< Shifted mode PRSCH12 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH20                 (_PRS_CH_CTRL_SIGSEL_PRSCH20 << 0)             /**< Shifted mode PRSCH20 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCOMP3                (_PRS_CH_CTRL_SIGSEL_RTCCOMP3 << 0)            /**< Shifted mode RTCCOMP3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN4                (_PRS_CH_CTRL_SIGSEL_GPIOPIN4 << 0)            /**< Shifted mode GPIOPIN4 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN12               (_PRS_CH_CTRL_SIGSEL_GPIOPIN12 << 0)           /**< Shifted mode GPIOPIN12 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_VDAC0OPA2               (_PRS_CH_CTRL_SIGSEL_VDAC0OPA2 << 0)           /**< Shifted mode VDAC0OPA2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES4         (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES4 << 0)     /**< Shifted mode LESENSESCANRES4 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES12        (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES12 << 0)    /**< Shifted mode LESENSESCANRES12 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER0CC2               (_PRS_CH_CTRL_SIGSEL_TIMER0CC2 << 0)           /**< Shifted mode TIMER0CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1CC2               (_PRS_CH_CTRL_SIGSEL_TIMER1CC2 << 0)           /**< Shifted mode TIMER1CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER2CC2               (_PRS_CH_CTRL_SIGSEL_TIMER2CC2 << 0)           /**< Shifted mode TIMER2CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER3CC2               (_PRS_CH_CTRL_SIGSEL_TIMER3CC2 << 0)           /**< Shifted mode TIMER3CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER0CC2              (_PRS_CH_CTRL_SIGSEL_WTIMER0CC2 << 0)          /**< Shifted mode WTIMER0CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER1CC2              (_PRS_CH_CTRL_SIGSEL_WTIMER1CC2 << 0)          /**< Shifted mode WTIMER1CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER2CC2              (_PRS_CH_CTRL_SIGSEL_WTIMER2CC2 << 0)          /**< Shifted mode WTIMER2CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER3CC2              (_PRS_CH_CTRL_SIGSEL_WTIMER3CC2 << 0)          /**< Shifted mode WTIMER3CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER4CC2               (_PRS_CH_CTRL_SIGSEL_TIMER4CC2 << 0)           /**< Shifted mode TIMER4CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER5CC2               (_PRS_CH_CTRL_SIGSEL_TIMER5CC2 << 0)           /**< Shifted mode TIMER5CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER6CC2               (_PRS_CH_CTRL_SIGSEL_TIMER6CC2 << 0)           /**< Shifted mode TIMER6CC2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH5                  (_PRS_CH_CTRL_SIGSEL_PRSCH5 << 0)              /**< Shifted mode PRSCH5 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH13                 (_PRS_CH_CTRL_SIGSEL_PRSCH13 << 0)             /**< Shifted mode PRSCH13 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH21                 (_PRS_CH_CTRL_SIGSEL_PRSCH21 << 0)             /**< Shifted mode PRSCH21 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCOMP4                (_PRS_CH_CTRL_SIGSEL_RTCCOMP4 << 0)            /**< Shifted mode RTCCOMP4 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN5                (_PRS_CH_CTRL_SIGSEL_GPIOPIN5 << 0)            /**< Shifted mode GPIOPIN5 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN13               (_PRS_CH_CTRL_SIGSEL_GPIOPIN13 << 0)           /**< Shifted mode GPIOPIN13 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_VDAC0OPA3               (_PRS_CH_CTRL_SIGSEL_VDAC0OPA3 << 0)           /**< Shifted mode VDAC0OPA3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES5         (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES5 << 0)     /**< Shifted mode LESENSESCANRES5 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES13        (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES13 << 0)    /**< Shifted mode LESENSESCANRES13 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0TX                (_PRS_CH_CTRL_SIGSEL_USART0TX << 0)            /**< Shifted mode USART0TX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART1TX                (_PRS_CH_CTRL_SIGSEL_USART1TX << 0)            /**< Shifted mode USART1TX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART2TX                (_PRS_CH_CTRL_SIGSEL_USART2TX << 0)            /**< Shifted mode USART2TX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART3TX                (_PRS_CH_CTRL_SIGSEL_USART3TX << 0)            /**< Shifted mode USART3TX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART4TX                (_PRS_CH_CTRL_SIGSEL_USART4TX << 0)            /**< Shifted mode USART4TX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART5TX                (_PRS_CH_CTRL_SIGSEL_USART5TX << 0)            /**< Shifted mode USART5TX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_UART0TX                 (_PRS_CH_CTRL_SIGSEL_UART0TX << 0)             /**< Shifted mode UART0TX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_UART1TX                 (_PRS_CH_CTRL_SIGSEL_UART1TX << 0)             /**< Shifted mode UART1TX for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_TIMER1CC3               (_PRS_CH_CTRL_SIGSEL_TIMER1CC3 << 0)           /**< Shifted mode TIMER1CC3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_WTIMER1CC3              (_PRS_CH_CTRL_SIGSEL_WTIMER1CC3 << 0)          /**< Shifted mode WTIMER1CC3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH6                  (_PRS_CH_CTRL_SIGSEL_PRSCH6 << 0)              /**< Shifted mode PRSCH6 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH14                 (_PRS_CH_CTRL_SIGSEL_PRSCH14 << 0)             /**< Shifted mode PRSCH14 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH22                 (_PRS_CH_CTRL_SIGSEL_PRSCH22 << 0)             /**< Shifted mode PRSCH22 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_RTCCOMP5                (_PRS_CH_CTRL_SIGSEL_RTCCOMP5 << 0)            /**< Shifted mode RTCCOMP5 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN6                (_PRS_CH_CTRL_SIGSEL_GPIOPIN6 << 0)            /**< Shifted mode GPIOPIN6 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN14               (_PRS_CH_CTRL_SIGSEL_GPIOPIN14 << 0)           /**< Shifted mode GPIOPIN14 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES6         (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES6 << 0)     /**< Shifted mode LESENSESCANRES6 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES14        (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES14 << 0)    /**< Shifted mode LESENSESCANRES14 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART0CS                (_PRS_CH_CTRL_SIGSEL_USART0CS << 0)            /**< Shifted mode USART0CS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART1CS                (_PRS_CH_CTRL_SIGSEL_USART1CS << 0)            /**< Shifted mode USART1CS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART2CS                (_PRS_CH_CTRL_SIGSEL_USART2CS << 0)            /**< Shifted mode USART2CS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART3CS                (_PRS_CH_CTRL_SIGSEL_USART3CS << 0)            /**< Shifted mode USART3CS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART4CS                (_PRS_CH_CTRL_SIGSEL_USART4CS << 0)            /**< Shifted mode USART4CS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_USART5CS                (_PRS_CH_CTRL_SIGSEL_USART5CS << 0)            /**< Shifted mode USART5CS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_UART0CS                 (_PRS_CH_CTRL_SIGSEL_UART0CS << 0)             /**< Shifted mode UART0CS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_UART1CS                 (_PRS_CH_CTRL_SIGSEL_UART1CS << 0)             /**< Shifted mode UART1CS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH7                  (_PRS_CH_CTRL_SIGSEL_PRSCH7 << 0)              /**< Shifted mode PRSCH7 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH15                 (_PRS_CH_CTRL_SIGSEL_PRSCH15 << 0)             /**< Shifted mode PRSCH15 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_PRSCH23                 (_PRS_CH_CTRL_SIGSEL_PRSCH23 << 0)             /**< Shifted mode PRSCH23 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN7                (_PRS_CH_CTRL_SIGSEL_GPIOPIN7 << 0)            /**< Shifted mode GPIOPIN7 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_GPIOPIN15               (_PRS_CH_CTRL_SIGSEL_GPIOPIN15 << 0)           /**< Shifted mode GPIOPIN15 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_CMUCLKOUT2              (_PRS_CH_CTRL_SIGSEL_CMUCLKOUT2 << 0)          /**< Shifted mode CMUCLKOUT2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES7         (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES7 << 0)     /**< Shifted mode LESENSESCANRES7 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SIGSEL_LESENSESCANRES15        (_PRS_CH_CTRL_SIGSEL_LESENSESCANRES15 << 0)    /**< Shifted mode LESENSESCANRES15 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_SHIFT               8                                              /**< Shift value for PRS_SOURCESEL */
#define _PRS_CH_CTRL_SOURCESEL_MASK                0x7F00UL                                       /**< Bit mask for PRS_SOURCESEL */
#define _PRS_CH_CTRL_SOURCESEL_NONE                0x00000000UL                                   /**< Mode NONE for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_PRSL                0x00000001UL                                   /**< Mode PRSL for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_PRS                 0x00000002UL                                   /**< Mode PRS for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_PRSH                0x00000003UL                                   /**< Mode PRSH for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_ACMP0               0x00000004UL                                   /**< Mode ACMP0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_ACMP1               0x00000005UL                                   /**< Mode ACMP1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_ADC0                0x00000006UL                                   /**< Mode ADC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_RTC                 0x00000007UL                                   /**< Mode RTC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_RTCC                0x00000008UL                                   /**< Mode RTCC for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_GPIOL               0x00000009UL                                   /**< Mode GPIOL for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_GPIOH               0x0000000AUL                                   /**< Mode GPIOH for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_LETIMER0            0x0000000BUL                                   /**< Mode LETIMER0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_LETIMER1            0x0000000CUL                                   /**< Mode LETIMER1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_PCNT0               0x0000000DUL                                   /**< Mode PCNT0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_PCNT1               0x0000000EUL                                   /**< Mode PCNT1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_PCNT2               0x0000000FUL                                   /**< Mode PCNT2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_CRYOTIMER           0x00000010UL                                   /**< Mode CRYOTIMER for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_CMU                 0x00000011UL                                   /**< Mode CMU for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_VDAC0               0x00000017UL                                   /**< Mode VDAC0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_LESENSEL            0x00000018UL                                   /**< Mode LESENSEL for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_LESENSEH            0x00000019UL                                   /**< Mode LESENSEH for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_LESENSED            0x0000001AUL                                   /**< Mode LESENSED for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_LESENSE             0x0000001BUL                                   /**< Mode LESENSE for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_ACMP2               0x0000001CUL                                   /**< Mode ACMP2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_ACMP3               0x0000001DUL                                   /**< Mode ACMP3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_ADC1                0x0000001EUL                                   /**< Mode ADC1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_USART0              0x00000030UL                                   /**< Mode USART0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_USART1              0x00000031UL                                   /**< Mode USART1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_USART2              0x00000032UL                                   /**< Mode USART2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_USART3              0x00000033UL                                   /**< Mode USART3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_USART4              0x00000034UL                                   /**< Mode USART4 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_USART5              0x00000035UL                                   /**< Mode USART5 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_UART0               0x00000036UL                                   /**< Mode UART0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_UART1               0x00000037UL                                   /**< Mode UART1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_TIMER0              0x0000003CUL                                   /**< Mode TIMER0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_TIMER1              0x0000003DUL                                   /**< Mode TIMER1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_TIMER2              0x0000003EUL                                   /**< Mode TIMER2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_USB                 0x00000040UL                                   /**< Mode USB for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_CM4                 0x00000043UL                                   /**< Mode CM4 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_TIMER3              0x00000050UL                                   /**< Mode TIMER3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_WTIMER0             0x00000052UL                                   /**< Mode WTIMER0 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_WTIMER1             0x00000053UL                                   /**< Mode WTIMER1 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_WTIMER2             0x00000054UL                                   /**< Mode WTIMER2 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_WTIMER3             0x00000055UL                                   /**< Mode WTIMER3 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_TIMER4              0x00000062UL                                   /**< Mode TIMER4 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_TIMER5              0x00000063UL                                   /**< Mode TIMER5 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_SOURCESEL_TIMER6              0x00000064UL                                   /**< Mode TIMER6 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_NONE                 (_PRS_CH_CTRL_SOURCESEL_NONE << 8)             /**< Shifted mode NONE for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_PRSL                 (_PRS_CH_CTRL_SOURCESEL_PRSL << 8)             /**< Shifted mode PRSL for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_PRS                  (_PRS_CH_CTRL_SOURCESEL_PRS << 8)              /**< Shifted mode PRS for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_PRSH                 (_PRS_CH_CTRL_SOURCESEL_PRSH << 8)             /**< Shifted mode PRSH for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_ACMP0                (_PRS_CH_CTRL_SOURCESEL_ACMP0 << 8)            /**< Shifted mode ACMP0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_ACMP1                (_PRS_CH_CTRL_SOURCESEL_ACMP1 << 8)            /**< Shifted mode ACMP1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_ADC0                 (_PRS_CH_CTRL_SOURCESEL_ADC0 << 8)             /**< Shifted mode ADC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_RTC                  (_PRS_CH_CTRL_SOURCESEL_RTC << 8)              /**< Shifted mode RTC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_RTCC                 (_PRS_CH_CTRL_SOURCESEL_RTCC << 8)             /**< Shifted mode RTCC for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_GPIOL                (_PRS_CH_CTRL_SOURCESEL_GPIOL << 8)            /**< Shifted mode GPIOL for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_GPIOH                (_PRS_CH_CTRL_SOURCESEL_GPIOH << 8)            /**< Shifted mode GPIOH for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_LETIMER0             (_PRS_CH_CTRL_SOURCESEL_LETIMER0 << 8)         /**< Shifted mode LETIMER0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_LETIMER1             (_PRS_CH_CTRL_SOURCESEL_LETIMER1 << 8)         /**< Shifted mode LETIMER1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_PCNT0                (_PRS_CH_CTRL_SOURCESEL_PCNT0 << 8)            /**< Shifted mode PCNT0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_PCNT1                (_PRS_CH_CTRL_SOURCESEL_PCNT1 << 8)            /**< Shifted mode PCNT1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_PCNT2                (_PRS_CH_CTRL_SOURCESEL_PCNT2 << 8)            /**< Shifted mode PCNT2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_CRYOTIMER            (_PRS_CH_CTRL_SOURCESEL_CRYOTIMER << 8)        /**< Shifted mode CRYOTIMER for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_CMU                  (_PRS_CH_CTRL_SOURCESEL_CMU << 8)              /**< Shifted mode CMU for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_VDAC0                (_PRS_CH_CTRL_SOURCESEL_VDAC0 << 8)            /**< Shifted mode VDAC0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_LESENSEL             (_PRS_CH_CTRL_SOURCESEL_LESENSEL << 8)         /**< Shifted mode LESENSEL for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_LESENSEH             (_PRS_CH_CTRL_SOURCESEL_LESENSEH << 8)         /**< Shifted mode LESENSEH for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_LESENSED             (_PRS_CH_CTRL_SOURCESEL_LESENSED << 8)         /**< Shifted mode LESENSED for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_LESENSE              (_PRS_CH_CTRL_SOURCESEL_LESENSE << 8)          /**< Shifted mode LESENSE for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_ACMP2                (_PRS_CH_CTRL_SOURCESEL_ACMP2 << 8)            /**< Shifted mode ACMP2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_ACMP3                (_PRS_CH_CTRL_SOURCESEL_ACMP3 << 8)            /**< Shifted mode ACMP3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_ADC1                 (_PRS_CH_CTRL_SOURCESEL_ADC1 << 8)             /**< Shifted mode ADC1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_USART0               (_PRS_CH_CTRL_SOURCESEL_USART0 << 8)           /**< Shifted mode USART0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_USART1               (_PRS_CH_CTRL_SOURCESEL_USART1 << 8)           /**< Shifted mode USART1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_USART2               (_PRS_CH_CTRL_SOURCESEL_USART2 << 8)           /**< Shifted mode USART2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_USART3               (_PRS_CH_CTRL_SOURCESEL_USART3 << 8)           /**< Shifted mode USART3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_USART4               (_PRS_CH_CTRL_SOURCESEL_USART4 << 8)           /**< Shifted mode USART4 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_USART5               (_PRS_CH_CTRL_SOURCESEL_USART5 << 8)           /**< Shifted mode USART5 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_UART0                (_PRS_CH_CTRL_SOURCESEL_UART0 << 8)            /**< Shifted mode UART0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_UART1                (_PRS_CH_CTRL_SOURCESEL_UART1 << 8)            /**< Shifted mode UART1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_TIMER0               (_PRS_CH_CTRL_SOURCESEL_TIMER0 << 8)           /**< Shifted mode TIMER0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_TIMER1               (_PRS_CH_CTRL_SOURCESEL_TIMER1 << 8)           /**< Shifted mode TIMER1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_TIMER2               (_PRS_CH_CTRL_SOURCESEL_TIMER2 << 8)           /**< Shifted mode TIMER2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_USB                  (_PRS_CH_CTRL_SOURCESEL_USB << 8)              /**< Shifted mode USB for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_CM4                  (_PRS_CH_CTRL_SOURCESEL_CM4 << 8)              /**< Shifted mode CM4 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_TIMER3               (_PRS_CH_CTRL_SOURCESEL_TIMER3 << 8)           /**< Shifted mode TIMER3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_WTIMER0              (_PRS_CH_CTRL_SOURCESEL_WTIMER0 << 8)          /**< Shifted mode WTIMER0 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_WTIMER1              (_PRS_CH_CTRL_SOURCESEL_WTIMER1 << 8)          /**< Shifted mode WTIMER1 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_WTIMER2              (_PRS_CH_CTRL_SOURCESEL_WTIMER2 << 8)          /**< Shifted mode WTIMER2 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_WTIMER3              (_PRS_CH_CTRL_SOURCESEL_WTIMER3 << 8)          /**< Shifted mode WTIMER3 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_TIMER4               (_PRS_CH_CTRL_SOURCESEL_TIMER4 << 8)           /**< Shifted mode TIMER4 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_TIMER5               (_PRS_CH_CTRL_SOURCESEL_TIMER5 << 8)           /**< Shifted mode TIMER5 for PRS_CH_CTRL */
#define PRS_CH_CTRL_SOURCESEL_TIMER6               (_PRS_CH_CTRL_SOURCESEL_TIMER6 << 8)           /**< Shifted mode TIMER6 for PRS_CH_CTRL */
#define _PRS_CH_CTRL_EDSEL_SHIFT                   20                                             /**< Shift value for PRS_EDSEL */
#define _PRS_CH_CTRL_EDSEL_MASK                    0x300000UL                                     /**< Bit mask for PRS_EDSEL */
#define _PRS_CH_CTRL_EDSEL_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for PRS_CH_CTRL */
#define _PRS_CH_CTRL_EDSEL_OFF                     0x00000000UL                                   /**< Mode OFF for PRS_CH_CTRL */
#define _PRS_CH_CTRL_EDSEL_POSEDGE                 0x00000001UL                                   /**< Mode POSEDGE for PRS_CH_CTRL */
#define _PRS_CH_CTRL_EDSEL_NEGEDGE                 0x00000002UL                                   /**< Mode NEGEDGE for PRS_CH_CTRL */
#define _PRS_CH_CTRL_EDSEL_BOTHEDGES               0x00000003UL                                   /**< Mode BOTHEDGES for PRS_CH_CTRL */
#define PRS_CH_CTRL_EDSEL_DEFAULT                  (_PRS_CH_CTRL_EDSEL_DEFAULT << 20)             /**< Shifted mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_EDSEL_OFF                      (_PRS_CH_CTRL_EDSEL_OFF << 20)                 /**< Shifted mode OFF for PRS_CH_CTRL */
#define PRS_CH_CTRL_EDSEL_POSEDGE                  (_PRS_CH_CTRL_EDSEL_POSEDGE << 20)             /**< Shifted mode POSEDGE for PRS_CH_CTRL */
#define PRS_CH_CTRL_EDSEL_NEGEDGE                  (_PRS_CH_CTRL_EDSEL_NEGEDGE << 20)             /**< Shifted mode NEGEDGE for PRS_CH_CTRL */
#define PRS_CH_CTRL_EDSEL_BOTHEDGES                (_PRS_CH_CTRL_EDSEL_BOTHEDGES << 20)           /**< Shifted mode BOTHEDGES for PRS_CH_CTRL */
#define PRS_CH_CTRL_STRETCH                        (0x1UL << 25)                                  /**< Stretch Channel Output */
#define _PRS_CH_CTRL_STRETCH_SHIFT                 25                                             /**< Shift value for PRS_STRETCH */
#define _PRS_CH_CTRL_STRETCH_MASK                  0x2000000UL                                    /**< Bit mask for PRS_STRETCH */
#define _PRS_CH_CTRL_STRETCH_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_STRETCH_DEFAULT                (_PRS_CH_CTRL_STRETCH_DEFAULT << 25)           /**< Shifted mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_INV                            (0x1UL << 26)                                  /**< Invert Channel */
#define _PRS_CH_CTRL_INV_SHIFT                     26                                             /**< Shift value for PRS_INV */
#define _PRS_CH_CTRL_INV_MASK                      0x4000000UL                                    /**< Bit mask for PRS_INV */
#define _PRS_CH_CTRL_INV_DEFAULT                   0x00000000UL                                   /**< Mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_INV_DEFAULT                    (_PRS_CH_CTRL_INV_DEFAULT << 26)               /**< Shifted mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ORPREV                         (0x1UL << 27)                                  /**< Or Previous */
#define _PRS_CH_CTRL_ORPREV_SHIFT                  27                                             /**< Shift value for PRS_ORPREV */
#define _PRS_CH_CTRL_ORPREV_MASK                   0x8000000UL                                    /**< Bit mask for PRS_ORPREV */
#define _PRS_CH_CTRL_ORPREV_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ORPREV_DEFAULT                 (_PRS_CH_CTRL_ORPREV_DEFAULT << 27)            /**< Shifted mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ANDNEXT                        (0x1UL << 28)                                  /**< And Next */
#define _PRS_CH_CTRL_ANDNEXT_SHIFT                 28                                             /**< Shift value for PRS_ANDNEXT */
#define _PRS_CH_CTRL_ANDNEXT_MASK                  0x10000000UL                                   /**< Bit mask for PRS_ANDNEXT */
#define _PRS_CH_CTRL_ANDNEXT_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ANDNEXT_DEFAULT                (_PRS_CH_CTRL_ANDNEXT_DEFAULT << 28)           /**< Shifted mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ASYNC                          (0x1UL << 30)                                  /**< Asynchronous Reflex */
#define _PRS_CH_CTRL_ASYNC_SHIFT                   30                                             /**< Shift value for PRS_ASYNC */
#define _PRS_CH_CTRL_ASYNC_MASK                    0x40000000UL                                   /**< Bit mask for PRS_ASYNC */
#define _PRS_CH_CTRL_ASYNC_DEFAULT                 0x00000000UL                                   /**< Mode DEFAULT for PRS_CH_CTRL */
#define PRS_CH_CTRL_ASYNC_DEFAULT                  (_PRS_CH_CTRL_ASYNC_DEFAULT << 30)             /**< Shifted mode DEFAULT for PRS_CH_CTRL */

/** @} */
/** @} End of group EFM32GG11B_PRS */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

