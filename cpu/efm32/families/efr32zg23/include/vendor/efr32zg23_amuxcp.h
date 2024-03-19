/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 AMUXCP register and bit field definitions
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

#ifndef EFR32ZG23_AMUXCP_H
#define EFR32ZG23_AMUXCP_H
#define AMUXCP_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_AMUXCP AMUXCP
 * @{
 * @brief EFR32ZG23 AMUXCP Register Declaration.
 *****************************************************************************/

/** AMUXCP Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IPVERSION                                          */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CTRL;                          /**< Control                                            */
  __IM uint32_t  STATUS;                        /**< Status                                             */
  __IOM uint32_t TEST;                          /**< Test                                               */
  __IOM uint32_t TRIM;                          /**< Trim                                               */
  uint32_t       RESERVED1[1018U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IPVERSION                                          */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CTRL_SET;                      /**< Control                                            */
  __IM uint32_t  STATUS_SET;                    /**< Status                                             */
  __IOM uint32_t TEST_SET;                      /**< Test                                               */
  __IOM uint32_t TRIM_SET;                      /**< Trim                                               */
  uint32_t       RESERVED3[1018U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IPVERSION                                          */
  uint32_t       RESERVED4[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CTRL_CLR;                      /**< Control                                            */
  __IM uint32_t  STATUS_CLR;                    /**< Status                                             */
  __IOM uint32_t TEST_CLR;                      /**< Test                                               */
  __IOM uint32_t TRIM_CLR;                      /**< Trim                                               */
  uint32_t       RESERVED5[1018U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IPVERSION                                          */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CTRL_TGL;                      /**< Control                                            */
  __IM uint32_t  STATUS_TGL;                    /**< Status                                             */
  __IOM uint32_t TEST_TGL;                      /**< Test                                               */
  __IOM uint32_t TRIM_TGL;                      /**< Trim                                               */
} AMUXCP_TypeDef;
/** @} End of group EFR32ZG23_AMUXCP */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_AMUXCP
 * @{
 * @defgroup EFR32ZG23_AMUXCP_BitFields AMUXCP Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for AMUXCP IPVERSION */
#define _AMUXCP_IPVERSION_RESETVALUE           0x00000001UL                               /**< Default value for AMUXCP_IPVERSION          */
#define _AMUXCP_IPVERSION_MASK                 0xFFFFFFFFUL                               /**< Mask for AMUXCP_IPVERSION                   */
#define _AMUXCP_IPVERSION_IPVERSION_SHIFT      0                                          /**< Shift value for AMUXCP_IPVERSION            */
#define _AMUXCP_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                               /**< Bit mask for AMUXCP_IPVERSION               */
#define _AMUXCP_IPVERSION_IPVERSION_DEFAULT    0x00000001UL                               /**< Mode DEFAULT for AMUXCP_IPVERSION           */
#define AMUXCP_IPVERSION_IPVERSION_DEFAULT     (_AMUXCP_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for AMUXCP_IPVERSION   */

/* Bit fields for AMUXCP CTRL */
#define _AMUXCP_CTRL_RESETVALUE                0x00000000UL                             /**< Default value for AMUXCP_CTRL               */
#define _AMUXCP_CTRL_MASK                      0x00000033UL                             /**< Mask for AMUXCP_CTRL                        */
#define AMUXCP_CTRL_FORCEHP                    (0x1UL << 0)                             /**< Force High Power                            */
#define _AMUXCP_CTRL_FORCEHP_SHIFT             0                                        /**< Shift value for AMUXCP_FORCEHP              */
#define _AMUXCP_CTRL_FORCEHP_MASK              0x1UL                                    /**< Bit mask for AMUXCP_FORCEHP                 */
#define _AMUXCP_CTRL_FORCEHP_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for AMUXCP_CTRL                */
#define AMUXCP_CTRL_FORCEHP_DEFAULT            (_AMUXCP_CTRL_FORCEHP_DEFAULT << 0)      /**< Shifted mode DEFAULT for AMUXCP_CTRL        */
#define AMUXCP_CTRL_FORCELP                    (0x1UL << 1)                             /**< Force Low Power                             */
#define _AMUXCP_CTRL_FORCELP_SHIFT             1                                        /**< Shift value for AMUXCP_FORCELP              */
#define _AMUXCP_CTRL_FORCELP_MASK              0x2UL                                    /**< Bit mask for AMUXCP_FORCELP                 */
#define _AMUXCP_CTRL_FORCELP_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for AMUXCP_CTRL                */
#define AMUXCP_CTRL_FORCELP_DEFAULT            (_AMUXCP_CTRL_FORCELP_DEFAULT << 1)      /**< Shifted mode DEFAULT for AMUXCP_CTRL        */
#define AMUXCP_CTRL_FORCERUN                   (0x1UL << 4)                             /**< Force run                                   */
#define _AMUXCP_CTRL_FORCERUN_SHIFT            4                                        /**< Shift value for AMUXCP_FORCERUN             */
#define _AMUXCP_CTRL_FORCERUN_MASK             0x10UL                                   /**< Bit mask for AMUXCP_FORCERUN                */
#define _AMUXCP_CTRL_FORCERUN_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for AMUXCP_CTRL                */
#define AMUXCP_CTRL_FORCERUN_DEFAULT           (_AMUXCP_CTRL_FORCERUN_DEFAULT << 4)     /**< Shifted mode DEFAULT for AMUXCP_CTRL        */
#define AMUXCP_CTRL_FORCESTOP                  (0x1UL << 5)                             /**< Force stop                                  */
#define _AMUXCP_CTRL_FORCESTOP_SHIFT           5                                        /**< Shift value for AMUXCP_FORCESTOP            */
#define _AMUXCP_CTRL_FORCESTOP_MASK            0x20UL                                   /**< Bit mask for AMUXCP_FORCESTOP               */
#define _AMUXCP_CTRL_FORCESTOP_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for AMUXCP_CTRL                */
#define AMUXCP_CTRL_FORCESTOP_DEFAULT          (_AMUXCP_CTRL_FORCESTOP_DEFAULT << 5)    /**< Shifted mode DEFAULT for AMUXCP_CTRL        */

/* Bit fields for AMUXCP STATUS */
#define _AMUXCP_STATUS_RESETVALUE              0x00000000UL                             /**< Default value for AMUXCP_STATUS             */
#define _AMUXCP_STATUS_MASK                    0x00000003UL                             /**< Mask for AMUXCP_STATUS                      */
#define AMUXCP_STATUS_RUN                      (0x1UL << 0)                             /**< running                                     */
#define _AMUXCP_STATUS_RUN_SHIFT               0                                        /**< Shift value for AMUXCP_RUN                  */
#define _AMUXCP_STATUS_RUN_MASK                0x1UL                                    /**< Bit mask for AMUXCP_RUN                     */
#define _AMUXCP_STATUS_RUN_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for AMUXCP_STATUS              */
#define AMUXCP_STATUS_RUN_DEFAULT              (_AMUXCP_STATUS_RUN_DEFAULT << 0)        /**< Shifted mode DEFAULT for AMUXCP_STATUS      */
#define AMUXCP_STATUS_HICAP                    (0x1UL << 1)                             /**< high cap                                    */
#define _AMUXCP_STATUS_HICAP_SHIFT             1                                        /**< Shift value for AMUXCP_HICAP                */
#define _AMUXCP_STATUS_HICAP_MASK              0x2UL                                    /**< Bit mask for AMUXCP_HICAP                   */
#define _AMUXCP_STATUS_HICAP_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for AMUXCP_STATUS              */
#define AMUXCP_STATUS_HICAP_DEFAULT            (_AMUXCP_STATUS_HICAP_DEFAULT << 1)      /**< Shifted mode DEFAULT for AMUXCP_STATUS      */

/* Bit fields for AMUXCP TEST */
#define _AMUXCP_TEST_RESETVALUE                0x00000000UL                               /**< Default value for AMUXCP_TEST               */
#define _AMUXCP_TEST_MASK                      0x80003313UL                               /**< Mask for AMUXCP_TEST                        */
#define AMUXCP_TEST_SYNCCLK                    (0x1UL << 0)                               /**< Sync Clock                                  */
#define _AMUXCP_TEST_SYNCCLK_SHIFT             0                                          /**< Shift value for AMUXCP_SYNCCLK              */
#define _AMUXCP_TEST_SYNCCLK_MASK              0x1UL                                      /**< Bit mask for AMUXCP_SYNCCLK                 */
#define _AMUXCP_TEST_SYNCCLK_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for AMUXCP_TEST                */
#define AMUXCP_TEST_SYNCCLK_DEFAULT            (_AMUXCP_TEST_SYNCCLK_DEFAULT << 0)        /**< Shifted mode DEFAULT for AMUXCP_TEST        */
#define AMUXCP_TEST_SYNCMODE                   (0x1UL << 1)                               /**< Sync Mode                                   */
#define _AMUXCP_TEST_SYNCMODE_SHIFT            1                                          /**< Shift value for AMUXCP_SYNCMODE             */
#define _AMUXCP_TEST_SYNCMODE_MASK             0x2UL                                      /**< Bit mask for AMUXCP_SYNCMODE                */
#define _AMUXCP_TEST_SYNCMODE_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for AMUXCP_TEST                */
#define AMUXCP_TEST_SYNCMODE_DEFAULT           (_AMUXCP_TEST_SYNCMODE_DEFAULT << 1)       /**< Shifted mode DEFAULT for AMUXCP_TEST        */
#define AMUXCP_TEST_FORCEREQUEST               (0x1UL << 4)                               /**< Force Request                               */
#define _AMUXCP_TEST_FORCEREQUEST_SHIFT        4                                          /**< Shift value for AMUXCP_FORCEREQUEST         */
#define _AMUXCP_TEST_FORCEREQUEST_MASK         0x10UL                                     /**< Bit mask for AMUXCP_FORCEREQUEST            */
#define _AMUXCP_TEST_FORCEREQUEST_DEFAULT      0x00000000UL                               /**< Mode DEFAULT for AMUXCP_TEST                */
#define AMUXCP_TEST_FORCEREQUEST_DEFAULT       (_AMUXCP_TEST_FORCEREQUEST_DEFAULT << 4)   /**< Shifted mode DEFAULT for AMUXCP_TEST        */
#define AMUXCP_TEST_FORCEHICAP                 (0x1UL << 8)                               /**< Force high capacitance driver               */
#define _AMUXCP_TEST_FORCEHICAP_SHIFT          8                                          /**< Shift value for AMUXCP_FORCEHICAP           */
#define _AMUXCP_TEST_FORCEHICAP_MASK           0x100UL                                    /**< Bit mask for AMUXCP_FORCEHICAP              */
#define _AMUXCP_TEST_FORCEHICAP_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for AMUXCP_TEST                */
#define AMUXCP_TEST_FORCEHICAP_DEFAULT         (_AMUXCP_TEST_FORCEHICAP_DEFAULT << 8)     /**< Shifted mode DEFAULT for AMUXCP_TEST        */
#define AMUXCP_TEST_FORCELOCAP                 (0x1UL << 9)                               /**< Force low capacitance driver                */
#define _AMUXCP_TEST_FORCELOCAP_SHIFT          9                                          /**< Shift value for AMUXCP_FORCELOCAP           */
#define _AMUXCP_TEST_FORCELOCAP_MASK           0x200UL                                    /**< Bit mask for AMUXCP_FORCELOCAP              */
#define _AMUXCP_TEST_FORCELOCAP_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for AMUXCP_TEST                */
#define AMUXCP_TEST_FORCELOCAP_DEFAULT         (_AMUXCP_TEST_FORCELOCAP_DEFAULT << 9)     /**< Shifted mode DEFAULT for AMUXCP_TEST        */
#define AMUXCP_TEST_FORCEBOOSTON               (0x1UL << 12)                              /**< Force Boost On                              */
#define _AMUXCP_TEST_FORCEBOOSTON_SHIFT        12                                         /**< Shift value for AMUXCP_FORCEBOOSTON         */
#define _AMUXCP_TEST_FORCEBOOSTON_MASK         0x1000UL                                   /**< Bit mask for AMUXCP_FORCEBOOSTON            */
#define _AMUXCP_TEST_FORCEBOOSTON_DEFAULT      0x00000000UL                               /**< Mode DEFAULT for AMUXCP_TEST                */
#define AMUXCP_TEST_FORCEBOOSTON_DEFAULT       (_AMUXCP_TEST_FORCEBOOSTON_DEFAULT << 12)  /**< Shifted mode DEFAULT for AMUXCP_TEST        */
#define AMUXCP_TEST_FORCEBOOSTOFF              (0x1UL << 13)                              /**< Force Boost Off                             */
#define _AMUXCP_TEST_FORCEBOOSTOFF_SHIFT       13                                         /**< Shift value for AMUXCP_FORCEBOOSTOFF        */
#define _AMUXCP_TEST_FORCEBOOSTOFF_MASK        0x2000UL                                   /**< Bit mask for AMUXCP_FORCEBOOSTOFF           */
#define _AMUXCP_TEST_FORCEBOOSTOFF_DEFAULT     0x00000000UL                               /**< Mode DEFAULT for AMUXCP_TEST                */
#define AMUXCP_TEST_FORCEBOOSTOFF_DEFAULT      (_AMUXCP_TEST_FORCEBOOSTOFF_DEFAULT << 13) /**< Shifted mode DEFAULT for AMUXCP_TEST        */
#define AMUXCP_TEST_STATUSEN                   (0x1UL << 31)                              /**< Enable write to status bits                 */
#define _AMUXCP_TEST_STATUSEN_SHIFT            31                                         /**< Shift value for AMUXCP_STATUSEN             */
#define _AMUXCP_TEST_STATUSEN_MASK             0x80000000UL                               /**< Bit mask for AMUXCP_STATUSEN                */
#define _AMUXCP_TEST_STATUSEN_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for AMUXCP_TEST                */
#define AMUXCP_TEST_STATUSEN_DEFAULT           (_AMUXCP_TEST_STATUSEN_DEFAULT << 31)      /**< Shifted mode DEFAULT for AMUXCP_TEST        */

/* Bit fields for AMUXCP TRIM */
#define _AMUXCP_TRIM_RESETVALUE                0x77E44AA1UL                                /**< Default value for AMUXCP_TRIM               */
#define _AMUXCP_TRIM_MASK                      0x77FFEFFFUL                                /**< Mask for AMUXCP_TRIM                        */
#define _AMUXCP_TRIM_WARMUPTIME_SHIFT          0                                           /**< Shift value for AMUXCP_WARMUPTIME           */
#define _AMUXCP_TRIM_WARMUPTIME_MASK           0x3UL                                       /**< Bit mask for AMUXCP_WARMUPTIME              */
#define _AMUXCP_TRIM_WARMUPTIME_DEFAULT        0x00000001UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define _AMUXCP_TRIM_WARMUPTIME_WUCYCLES72     0x00000000UL                                /**< Mode WUCYCLES72 for AMUXCP_TRIM             */
#define _AMUXCP_TRIM_WARMUPTIME_WUCYCLES96     0x00000001UL                                /**< Mode WUCYCLES96 for AMUXCP_TRIM             */
#define _AMUXCP_TRIM_WARMUPTIME_WUCYCLES128    0x00000002UL                                /**< Mode WUCYCLES128 for AMUXCP_TRIM            */
#define _AMUXCP_TRIM_WARMUPTIME_WUCYCLES160    0x00000003UL                                /**< Mode WUCYCLES160 for AMUXCP_TRIM            */
#define AMUXCP_TRIM_WARMUPTIME_DEFAULT         (_AMUXCP_TRIM_WARMUPTIME_DEFAULT << 0)      /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define AMUXCP_TRIM_WARMUPTIME_WUCYCLES72      (_AMUXCP_TRIM_WARMUPTIME_WUCYCLES72 << 0)   /**< Shifted mode WUCYCLES72 for AMUXCP_TRIM     */
#define AMUXCP_TRIM_WARMUPTIME_WUCYCLES96      (_AMUXCP_TRIM_WARMUPTIME_WUCYCLES96 << 0)   /**< Shifted mode WUCYCLES96 for AMUXCP_TRIM     */
#define AMUXCP_TRIM_WARMUPTIME_WUCYCLES128     (_AMUXCP_TRIM_WARMUPTIME_WUCYCLES128 << 0)  /**< Shifted mode WUCYCLES128 for AMUXCP_TRIM    */
#define AMUXCP_TRIM_WARMUPTIME_WUCYCLES160     (_AMUXCP_TRIM_WARMUPTIME_WUCYCLES160 << 0)  /**< Shifted mode WUCYCLES160 for AMUXCP_TRIM    */
#define AMUXCP_TRIM_FLOATVDDCPLO               (0x1UL << 2)                                /**< Float VDDCP Low Power                       */
#define _AMUXCP_TRIM_FLOATVDDCPLO_SHIFT        2                                           /**< Shift value for AMUXCP_FLOATVDDCPLO         */
#define _AMUXCP_TRIM_FLOATVDDCPLO_MASK         0x4UL                                       /**< Bit mask for AMUXCP_FLOATVDDCPLO            */
#define _AMUXCP_TRIM_FLOATVDDCPLO_DEFAULT      0x00000000UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_FLOATVDDCPLO_DEFAULT       (_AMUXCP_TRIM_FLOATVDDCPLO_DEFAULT << 2)    /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define AMUXCP_TRIM_FLOATVDDCPHI               (0x1UL << 3)                                /**< Float VDDCP High Power                      */
#define _AMUXCP_TRIM_FLOATVDDCPHI_SHIFT        3                                           /**< Shift value for AMUXCP_FLOATVDDCPHI         */
#define _AMUXCP_TRIM_FLOATVDDCPHI_MASK         0x8UL                                       /**< Bit mask for AMUXCP_FLOATVDDCPHI            */
#define _AMUXCP_TRIM_FLOATVDDCPHI_DEFAULT      0x00000000UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_FLOATVDDCPHI_DEFAULT       (_AMUXCP_TRIM_FLOATVDDCPHI_DEFAULT << 3)    /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define AMUXCP_TRIM_BYPASSDIV2LO               (0x1UL << 4)                                /**< Bypass Div2 Low Power                       */
#define _AMUXCP_TRIM_BYPASSDIV2LO_SHIFT        4                                           /**< Shift value for AMUXCP_BYPASSDIV2LO         */
#define _AMUXCP_TRIM_BYPASSDIV2LO_MASK         0x10UL                                      /**< Bit mask for AMUXCP_BYPASSDIV2LO            */
#define _AMUXCP_TRIM_BYPASSDIV2LO_DEFAULT      0x00000000UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_BYPASSDIV2LO_DEFAULT       (_AMUXCP_TRIM_BYPASSDIV2LO_DEFAULT << 4)    /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define AMUXCP_TRIM_BYPASSDIV2HI               (0x1UL << 5)                                /**< Bypass Div2 High Power                      */
#define _AMUXCP_TRIM_BYPASSDIV2HI_SHIFT        5                                           /**< Shift value for AMUXCP_BYPASSDIV2HI         */
#define _AMUXCP_TRIM_BYPASSDIV2HI_MASK         0x20UL                                      /**< Bit mask for AMUXCP_BYPASSDIV2HI            */
#define _AMUXCP_TRIM_BYPASSDIV2HI_DEFAULT      0x00000001UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_BYPASSDIV2HI_DEFAULT       (_AMUXCP_TRIM_BYPASSDIV2HI_DEFAULT << 5)    /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define AMUXCP_TRIM_BUMP0P5XLO                 (0x1UL << 6)                                /**< Bump 0.5X Low Power                         */
#define _AMUXCP_TRIM_BUMP0P5XLO_SHIFT          6                                           /**< Shift value for AMUXCP_BUMP0P5XLO           */
#define _AMUXCP_TRIM_BUMP0P5XLO_MASK           0x40UL                                      /**< Bit mask for AMUXCP_BUMP0P5XLO              */
#define _AMUXCP_TRIM_BUMP0P5XLO_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_BUMP0P5XLO_DEFAULT         (_AMUXCP_TRIM_BUMP0P5XLO_DEFAULT << 6)      /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define AMUXCP_TRIM_BUMP0P5XHI                 (0x1UL << 7)                                /**< Bump 0.5X High Power                        */
#define _AMUXCP_TRIM_BUMP0P5XHI_SHIFT          7                                           /**< Shift value for AMUXCP_BUMP0P5XHI           */
#define _AMUXCP_TRIM_BUMP0P5XHI_MASK           0x80UL                                      /**< Bit mask for AMUXCP_BUMP0P5XHI              */
#define _AMUXCP_TRIM_BUMP0P5XHI_DEFAULT        0x00000001UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_BUMP0P5XHI_DEFAULT         (_AMUXCP_TRIM_BUMP0P5XHI_DEFAULT << 7)      /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define AMUXCP_TRIM_BIAS2XLO                   (0x1UL << 8)                                /**< Bias 2x Low Power                           */
#define _AMUXCP_TRIM_BIAS2XLO_SHIFT            8                                           /**< Shift value for AMUXCP_BIAS2XLO             */
#define _AMUXCP_TRIM_BIAS2XLO_MASK             0x100UL                                     /**< Bit mask for AMUXCP_BIAS2XLO                */
#define _AMUXCP_TRIM_BIAS2XLO_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_BIAS2XLO_DEFAULT           (_AMUXCP_TRIM_BIAS2XLO_DEFAULT << 8)        /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define AMUXCP_TRIM_BIAS2XHI                   (0x1UL << 9)                                /**< Bias 2x High Power                          */
#define _AMUXCP_TRIM_BIAS2XHI_SHIFT            9                                           /**< Shift value for AMUXCP_BIAS2XHI             */
#define _AMUXCP_TRIM_BIAS2XHI_MASK             0x200UL                                     /**< Bit mask for AMUXCP_BIAS2XHI                */
#define _AMUXCP_TRIM_BIAS2XHI_DEFAULT          0x00000001UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_BIAS2XHI_DEFAULT           (_AMUXCP_TRIM_BIAS2XHI_DEFAULT << 9)        /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define _AMUXCP_TRIM_VOLTAGECTRLLO_SHIFT       10                                          /**< Shift value for AMUXCP_VOLTAGECTRLLO        */
#define _AMUXCP_TRIM_VOLTAGECTRLLO_MASK        0xC00UL                                     /**< Bit mask for AMUXCP_VOLTAGECTRLLO           */
#define _AMUXCP_TRIM_VOLTAGECTRLLO_DEFAULT     0x00000002UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_VOLTAGECTRLLO_DEFAULT      (_AMUXCP_TRIM_VOLTAGECTRLLO_DEFAULT << 10)  /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define _AMUXCP_TRIM_VOLTAGECTRLHI_SHIFT       13                                          /**< Shift value for AMUXCP_VOLTAGECTRLHI        */
#define _AMUXCP_TRIM_VOLTAGECTRLHI_MASK        0x6000UL                                    /**< Bit mask for AMUXCP_VOLTAGECTRLHI           */
#define _AMUXCP_TRIM_VOLTAGECTRLHI_DEFAULT     0x00000002UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_VOLTAGECTRLHI_DEFAULT      (_AMUXCP_TRIM_VOLTAGECTRLHI_DEFAULT << 13)  /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define _AMUXCP_TRIM_BIASCTRLLO_SHIFT          15                                          /**< Shift value for AMUXCP_BIASCTRLLO           */
#define _AMUXCP_TRIM_BIASCTRLLO_MASK           0x38000UL                                   /**< Bit mask for AMUXCP_BIASCTRLLO              */
#define _AMUXCP_TRIM_BIASCTRLLO_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_BIASCTRLLO_DEFAULT         (_AMUXCP_TRIM_BIASCTRLLO_DEFAULT << 15)     /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define _AMUXCP_TRIM_BIASCTRLLOCONT_SHIFT      18                                          /**< Shift value for AMUXCP_BIASCTRLLOCONT       */
#define _AMUXCP_TRIM_BIASCTRLLOCONT_MASK       0x1C0000UL                                  /**< Bit mask for AMUXCP_BIASCTRLLOCONT          */
#define _AMUXCP_TRIM_BIASCTRLLOCONT_DEFAULT    0x00000001UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_BIASCTRLLOCONT_DEFAULT     (_AMUXCP_TRIM_BIASCTRLLOCONT_DEFAULT << 18) /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define _AMUXCP_TRIM_BIASCTRLHI_SHIFT          21                                          /**< Shift value for AMUXCP_BIASCTRLHI           */
#define _AMUXCP_TRIM_BIASCTRLHI_MASK           0xE00000UL                                  /**< Bit mask for AMUXCP_BIASCTRLHI              */
#define _AMUXCP_TRIM_BIASCTRLHI_DEFAULT        0x00000007UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_BIASCTRLHI_DEFAULT         (_AMUXCP_TRIM_BIASCTRLHI_DEFAULT << 21)     /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define _AMUXCP_TRIM_PUMPCAPLO_SHIFT           24                                          /**< Shift value for AMUXCP_PUMPCAPLO            */
#define _AMUXCP_TRIM_PUMPCAPLO_MASK            0x7000000UL                                 /**< Bit mask for AMUXCP_PUMPCAPLO               */
#define _AMUXCP_TRIM_PUMPCAPLO_DEFAULT         0x00000007UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_PUMPCAPLO_DEFAULT          (_AMUXCP_TRIM_PUMPCAPLO_DEFAULT << 24)      /**< Shifted mode DEFAULT for AMUXCP_TRIM        */
#define _AMUXCP_TRIM_PUMPCAPHI_SHIFT           28                                          /**< Shift value for AMUXCP_PUMPCAPHI            */
#define _AMUXCP_TRIM_PUMPCAPHI_MASK            0x70000000UL                                /**< Bit mask for AMUXCP_PUMPCAPHI               */
#define _AMUXCP_TRIM_PUMPCAPHI_DEFAULT         0x00000007UL                                /**< Mode DEFAULT for AMUXCP_TRIM                */
#define AMUXCP_TRIM_PUMPCAPHI_DEFAULT          (_AMUXCP_TRIM_PUMPCAPHI_DEFAULT << 28)      /**< Shifted mode DEFAULT for AMUXCP_TRIM        */

/** @} End of group EFR32ZG23_AMUXCP_BitFields */
/** @} End of group EFR32ZG23_AMUXCP */
/** @} End of group Parts */

#endif /* EFR32ZG23_AMUXCP_H */
#ifdef __cplusplus
}
#endif

