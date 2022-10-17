/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 DEVINFO register and bit field definitions
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

#ifndef EFR32ZG23_DEVINFO_H
#define EFR32ZG23_DEVINFO_H

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32ZG23_DEVINFO DEVINFO
 * @{
 * @brief EFR32ZG23 DEVINFO Register Declaration.
 *****************************************************************************/

/** DEVINFO HFRCODPLLCAL Register Group Declaration. */
typedef struct {
  __IM uint32_t HFRCODPLLCAL;                        /**< HFRCODPLL Calibration                              */
} DEVINFO_HFRCODPLLCAL_TypeDef;

/** DEVINFO HFRCOEM23CAL Register Group Declaration. */
typedef struct {
  __IM uint32_t HFRCOEM23CAL;                        /**< HFRCOEM23 Calibration                              */
} DEVINFO_HFRCOEM23CAL_TypeDef;

/** DEVINFO HFRCOSECAL Register Group Declaration. */
typedef struct {
  uint32_t RESERVED0[1U];                            /**< Reserved for future use                            */
} DEVINFO_HFRCOSECAL_TypeDef;

/** DEVINFO Register Declaration. */
typedef struct {
  __IM uint32_t                INFO;                  /**< DI Information                                     */
  __IM uint32_t                PART;                  /**< Part Info                                          */
  __IM uint32_t                MEMINFO;               /**< Memory Info                                        */
  __IM uint32_t                MSIZE;                 /**< Memory Size                                        */
  __IM uint32_t                PKGINFO;               /**< Misc Device Info                                   */
  __IM uint32_t                CUSTOMINFO;            /**< Custom Part Info                                   */
  __IM uint32_t                SWFIX;                 /**< SW Fix Register                                    */
  __IM uint32_t                SWCAPA0;               /**< Software Restriction                               */
  __IM uint32_t                SWCAPA1;               /**< Software Restriction                               */
  uint32_t                     RESERVED0[1U];         /**< Reserved for future use                            */
  __IM uint32_t                EXTINFO;               /**< External Component Info                            */
  uint32_t                     RESERVED1[2U];         /**< Reserved for future use                            */
  uint32_t                     RESERVED2[3U];         /**< Reserved for future use                            */
  __IM uint32_t                EUI48L;                /**< EUI 48 Low                                         */
  __IM uint32_t                EUI48H;                /**< EUI 48 High                                        */
  __IM uint32_t                EUI64L;                /**< EUI64 Low                                          */
  __IM uint32_t                EUI64H;                /**< EUI64 High                                         */
  __IM uint32_t                CALTEMP;               /**< Calibration temperature                            */
  __IM uint32_t                EMUTEMP;               /**< EMU Temp                                           */
  DEVINFO_HFRCODPLLCAL_TypeDef HFRCODPLLCAL[18U];     /**<                                                    */
  DEVINFO_HFRCOEM23CAL_TypeDef HFRCOEM23CAL[18U];     /**<                                                    */
  DEVINFO_HFRCOSECAL_TypeDef   HFRCOSECAL[18U];       /**<                                                    */
  __IM uint32_t                MODULENAME0;           /**< Module Name Information                            */
  __IM uint32_t                MODULENAME1;           /**< Module Name Information                            */
  __IM uint32_t                MODULENAME2;           /**< Module Name Information                            */
  __IM uint32_t                MODULENAME3;           /**< Module Name Information                            */
  __IM uint32_t                MODULENAME4;           /**< Module Name Information                            */
  __IM uint32_t                MODULENAME5;           /**< Module Name Information                            */
  __IM uint32_t                MODULENAME6;           /**< Module Name Information                            */
  __IM uint32_t                MODULEINFO;            /**< Module Information                                 */
  __IM uint32_t                MODXOCAL;              /**< Module External Oscillator Calibration Information */
  uint32_t                     RESERVED3[10U];        /**< Reserved for future use                            */
  __IM uint32_t                HFXOCAL;               /**< High Frequency Crystal Oscillator Calibration data */
  __IM uint32_t                IADC0GAIN0;            /**< IADC Gain Calibration                              */
  __IM uint32_t                IADC0GAIN1;            /**< IADC Gain Calibration                              */
  __IM uint32_t                IADC0OFFSETCAL0;       /**< IADC Offset Calibration                            */
  __IM uint32_t                IADC0NORMALOFFSETCAL0; /**< IADC Offset Calibration                            */
  __IM uint32_t                IADC0NORMALOFFSETCAL1; /**< IADC Offset Calibration                            */
  __IM uint32_t                IADC0HISPDOFFSETCAL0;  /**< IADC Offset Calibration                            */
  __IM uint32_t                IADC0HISPDOFFSETCAL1;  /**< IADC Offset Calibration                            */
  uint32_t                     RESERVED4[24U];        /**< Reserved for future use                            */
  __IM uint32_t                LEGACY;                /**< Legacy Device Info                                 */
  uint32_t                     RESERVED5[23U];        /**< Reserved for future use                            */
  __IM uint32_t                RTHERM;                /**<                                                    */
  uint32_t                     RESERVED6[80U];        /**< Reserved for future use                            */
  uint32_t                     RESERVED7[1U];         /**< Reserved for future use                            */
} DEVINFO_TypeDef;
/** @} End of group EFR32ZG23_DEVINFO */

/**************************************************************************//**
 * @addtogroup EFR32ZG23_DEVINFO
 * @{
 * @defgroup EFR32ZG23_DEVINFO_BitFields DEVINFO Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for DEVINFO INFO */
#define _DEVINFO_INFO_RESETVALUE                                 0x0C000000UL                             /**< Default value for DEVINFO_INFO              */
#define _DEVINFO_INFO_MASK                                       0xFFFFFFFFUL                             /**< Mask for DEVINFO_INFO                       */
#define _DEVINFO_INFO_CRC_SHIFT                                  0                                        /**< Shift value for DEVINFO_CRC                 */
#define _DEVINFO_INFO_CRC_MASK                                   0xFFFFUL                                 /**< Bit mask for DEVINFO_CRC                    */
#define _DEVINFO_INFO_CRC_DEFAULT                                0x00000000UL                             /**< Mode DEFAULT for DEVINFO_INFO               */
#define DEVINFO_INFO_CRC_DEFAULT                                 (_DEVINFO_INFO_CRC_DEFAULT << 0)         /**< Shifted mode DEFAULT for DEVINFO_INFO       */
#define _DEVINFO_INFO_PRODREV_SHIFT                              16                                       /**< Shift value for DEVINFO_PRODREV             */
#define _DEVINFO_INFO_PRODREV_MASK                               0xFF0000UL                               /**< Bit mask for DEVINFO_PRODREV                */
#define _DEVINFO_INFO_PRODREV_DEFAULT                            0x00000000UL                             /**< Mode DEFAULT for DEVINFO_INFO               */
#define DEVINFO_INFO_PRODREV_DEFAULT                             (_DEVINFO_INFO_PRODREV_DEFAULT << 16)    /**< Shifted mode DEFAULT for DEVINFO_INFO       */
#define _DEVINFO_INFO_DEVINFOREV_SHIFT                           24                                       /**< Shift value for DEVINFO_DEVINFOREV          */
#define _DEVINFO_INFO_DEVINFOREV_MASK                            0xFF000000UL                             /**< Bit mask for DEVINFO_DEVINFOREV             */
#define _DEVINFO_INFO_DEVINFOREV_DEFAULT                         0x0000000CUL                             /**< Mode DEFAULT for DEVINFO_INFO               */
#define DEVINFO_INFO_DEVINFOREV_DEFAULT                          (_DEVINFO_INFO_DEVINFOREV_DEFAULT << 24) /**< Shifted mode DEFAULT for DEVINFO_INFO       */

/* Bit fields for DEVINFO PART */
#define _DEVINFO_PART_RESETVALUE                                 0x00000000UL                            /**< Default value for DEVINFO_PART              */
#define _DEVINFO_PART_MASK                                       0x3F3FFFFFUL                            /**< Mask for DEVINFO_PART                       */
#define _DEVINFO_PART_DEVICENUM_SHIFT                            0                                       /**< Shift value for DEVINFO_DEVICENUM           */
#define _DEVINFO_PART_DEVICENUM_MASK                             0xFFFFUL                                /**< Bit mask for DEVINFO_DEVICENUM              */
#define _DEVINFO_PART_DEVICENUM_DEFAULT                          0x00000000UL                            /**< Mode DEFAULT for DEVINFO_PART               */
#define DEVINFO_PART_DEVICENUM_DEFAULT                           (_DEVINFO_PART_DEVICENUM_DEFAULT << 0)  /**< Shifted mode DEFAULT for DEVINFO_PART       */
#define _DEVINFO_PART_FAMILYNUM_SHIFT                            16                                      /**< Shift value for DEVINFO_FAMILYNUM           */
#define _DEVINFO_PART_FAMILYNUM_MASK                             0x3F0000UL                              /**< Bit mask for DEVINFO_FAMILYNUM              */
#define _DEVINFO_PART_FAMILYNUM_DEFAULT                          0x00000000UL                            /**< Mode DEFAULT for DEVINFO_PART               */
#define DEVINFO_PART_FAMILYNUM_DEFAULT                           (_DEVINFO_PART_FAMILYNUM_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_PART       */
#define _DEVINFO_PART_FAMILY_SHIFT                               24                                      /**< Shift value for DEVINFO_FAMILY              */
#define _DEVINFO_PART_FAMILY_MASK                                0x3F000000UL                            /**< Bit mask for DEVINFO_FAMILY                 */
#define _DEVINFO_PART_FAMILY_DEFAULT                             0x00000000UL                            /**< Mode DEFAULT for DEVINFO_PART               */
#define _DEVINFO_PART_FAMILY_FG                                  0x00000000UL                            /**< Mode FG for DEVINFO_PART                    */
#define _DEVINFO_PART_FAMILY_ZG                                  0x00000003UL                            /**< Mode ZG for DEVINFO_PART                    */
#define _DEVINFO_PART_FAMILY_PG                                  0x00000005UL                            /**< Mode PG for DEVINFO_PART                    */
#define DEVINFO_PART_FAMILY_DEFAULT                              (_DEVINFO_PART_FAMILY_DEFAULT << 24)    /**< Shifted mode DEFAULT for DEVINFO_PART       */
#define DEVINFO_PART_FAMILY_FG                                   (_DEVINFO_PART_FAMILY_FG << 24)         /**< Shifted mode FG for DEVINFO_PART            */
#define DEVINFO_PART_FAMILY_ZG                                   (_DEVINFO_PART_FAMILY_ZG << 24)         /**< Shifted mode ZG for DEVINFO_PART            */
#define DEVINFO_PART_FAMILY_PG                                   (_DEVINFO_PART_FAMILY_PG << 24)         /**< Shifted mode PG for DEVINFO_PART            */

/* Bit fields for DEVINFO MEMINFO */
#define _DEVINFO_MEMINFO_RESETVALUE                              0x00000000UL                                  /**< Default value for DEVINFO_MEMINFO           */
#define _DEVINFO_MEMINFO_MASK                                    0xFFFFFFFFUL                                  /**< Mask for DEVINFO_MEMINFO                    */
#define _DEVINFO_MEMINFO_FLASHPAGESIZE_SHIFT                     0                                             /**< Shift value for DEVINFO_FLASHPAGESIZE       */
#define _DEVINFO_MEMINFO_FLASHPAGESIZE_MASK                      0xFFUL                                        /**< Bit mask for DEVINFO_FLASHPAGESIZE          */
#define _DEVINFO_MEMINFO_FLASHPAGESIZE_DEFAULT                   0x00000000UL                                  /**< Mode DEFAULT for DEVINFO_MEMINFO            */
#define DEVINFO_MEMINFO_FLASHPAGESIZE_DEFAULT                    (_DEVINFO_MEMINFO_FLASHPAGESIZE_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_MEMINFO    */
#define _DEVINFO_MEMINFO_UDPAGESIZE_SHIFT                        8                                             /**< Shift value for DEVINFO_UDPAGESIZE          */
#define _DEVINFO_MEMINFO_UDPAGESIZE_MASK                         0xFF00UL                                      /**< Bit mask for DEVINFO_UDPAGESIZE             */
#define _DEVINFO_MEMINFO_UDPAGESIZE_DEFAULT                      0x00000000UL                                  /**< Mode DEFAULT for DEVINFO_MEMINFO            */
#define DEVINFO_MEMINFO_UDPAGESIZE_DEFAULT                       (_DEVINFO_MEMINFO_UDPAGESIZE_DEFAULT << 8)    /**< Shifted mode DEFAULT for DEVINFO_MEMINFO    */
#define _DEVINFO_MEMINFO_DILEN_SHIFT                             16                                            /**< Shift value for DEVINFO_DILEN               */
#define _DEVINFO_MEMINFO_DILEN_MASK                              0xFFFF0000UL                                  /**< Bit mask for DEVINFO_DILEN                  */
#define _DEVINFO_MEMINFO_DILEN_DEFAULT                           0x00000000UL                                  /**< Mode DEFAULT for DEVINFO_MEMINFO            */
#define DEVINFO_MEMINFO_DILEN_DEFAULT                            (_DEVINFO_MEMINFO_DILEN_DEFAULT << 16)        /**< Shifted mode DEFAULT for DEVINFO_MEMINFO    */

/* Bit fields for DEVINFO MSIZE */
#define _DEVINFO_MSIZE_RESETVALUE                                0x00000000UL                        /**< Default value for DEVINFO_MSIZE             */
#define _DEVINFO_MSIZE_MASK                                      0x07FFFFFFUL                        /**< Mask for DEVINFO_MSIZE                      */
#define _DEVINFO_MSIZE_FLASH_SHIFT                               0                                   /**< Shift value for DEVINFO_FLASH               */
#define _DEVINFO_MSIZE_FLASH_MASK                                0xFFFFUL                            /**< Bit mask for DEVINFO_FLASH                  */
#define _DEVINFO_MSIZE_FLASH_DEFAULT                             0x00000000UL                        /**< Mode DEFAULT for DEVINFO_MSIZE              */
#define DEVINFO_MSIZE_FLASH_DEFAULT                              (_DEVINFO_MSIZE_FLASH_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_MSIZE      */
#define _DEVINFO_MSIZE_SRAM_SHIFT                                16                                  /**< Shift value for DEVINFO_SRAM                */
#define _DEVINFO_MSIZE_SRAM_MASK                                 0x7FF0000UL                         /**< Bit mask for DEVINFO_SRAM                   */
#define _DEVINFO_MSIZE_SRAM_DEFAULT                              0x00000000UL                        /**< Mode DEFAULT for DEVINFO_MSIZE              */
#define DEVINFO_MSIZE_SRAM_DEFAULT                               (_DEVINFO_MSIZE_SRAM_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_MSIZE      */

/* Bit fields for DEVINFO PKGINFO */
#define _DEVINFO_PKGINFO_RESETVALUE                              0x00000000UL                               /**< Default value for DEVINFO_PKGINFO           */
#define _DEVINFO_PKGINFO_MASK                                    0x00FFFFFFUL                               /**< Mask for DEVINFO_PKGINFO                    */
#define _DEVINFO_PKGINFO_TEMPGRADE_SHIFT                         0                                          /**< Shift value for DEVINFO_TEMPGRADE           */
#define _DEVINFO_PKGINFO_TEMPGRADE_MASK                          0xFFUL                                     /**< Bit mask for DEVINFO_TEMPGRADE              */
#define _DEVINFO_PKGINFO_TEMPGRADE_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for DEVINFO_PKGINFO            */
#define _DEVINFO_PKGINFO_TEMPGRADE_N40TO85                       0x00000000UL                               /**< Mode N40TO85 for DEVINFO_PKGINFO            */
#define _DEVINFO_PKGINFO_TEMPGRADE_N40TO125                      0x00000001UL                               /**< Mode N40TO125 for DEVINFO_PKGINFO           */
#define _DEVINFO_PKGINFO_TEMPGRADE_N40TO105                      0x00000002UL                               /**< Mode N40TO105 for DEVINFO_PKGINFO           */
#define _DEVINFO_PKGINFO_TEMPGRADE_N0TO70                        0x00000003UL                               /**< Mode N0TO70 for DEVINFO_PKGINFO             */
#define DEVINFO_PKGINFO_TEMPGRADE_DEFAULT                        (_DEVINFO_PKGINFO_TEMPGRADE_DEFAULT << 0)  /**< Shifted mode DEFAULT for DEVINFO_PKGINFO    */
#define DEVINFO_PKGINFO_TEMPGRADE_N40TO85                        (_DEVINFO_PKGINFO_TEMPGRADE_N40TO85 << 0)  /**< Shifted mode N40TO85 for DEVINFO_PKGINFO    */
#define DEVINFO_PKGINFO_TEMPGRADE_N40TO125                       (_DEVINFO_PKGINFO_TEMPGRADE_N40TO125 << 0) /**< Shifted mode N40TO125 for DEVINFO_PKGINFO   */
#define DEVINFO_PKGINFO_TEMPGRADE_N40TO105                       (_DEVINFO_PKGINFO_TEMPGRADE_N40TO105 << 0) /**< Shifted mode N40TO105 for DEVINFO_PKGINFO   */
#define DEVINFO_PKGINFO_TEMPGRADE_N0TO70                         (_DEVINFO_PKGINFO_TEMPGRADE_N0TO70 << 0)   /**< Shifted mode N0TO70 for DEVINFO_PKGINFO     */
#define _DEVINFO_PKGINFO_PKGTYPE_SHIFT                           8                                          /**< Shift value for DEVINFO_PKGTYPE             */
#define _DEVINFO_PKGINFO_PKGTYPE_MASK                            0xFF00UL                                   /**< Bit mask for DEVINFO_PKGTYPE                */
#define _DEVINFO_PKGINFO_PKGTYPE_DEFAULT                         0x00000000UL                               /**< Mode DEFAULT for DEVINFO_PKGINFO            */
#define _DEVINFO_PKGINFO_PKGTYPE_WLCSP                           0x0000004AUL                               /**< Mode WLCSP for DEVINFO_PKGINFO              */
#define _DEVINFO_PKGINFO_PKGTYPE_BGA                             0x0000004CUL                               /**< Mode BGA for DEVINFO_PKGINFO                */
#define _DEVINFO_PKGINFO_PKGTYPE_QFN                             0x0000004DUL                               /**< Mode QFN for DEVINFO_PKGINFO                */
#define _DEVINFO_PKGINFO_PKGTYPE_QFP                             0x00000051UL                               /**< Mode QFP for DEVINFO_PKGINFO                */
#define DEVINFO_PKGINFO_PKGTYPE_DEFAULT                          (_DEVINFO_PKGINFO_PKGTYPE_DEFAULT << 8)    /**< Shifted mode DEFAULT for DEVINFO_PKGINFO    */
#define DEVINFO_PKGINFO_PKGTYPE_WLCSP                            (_DEVINFO_PKGINFO_PKGTYPE_WLCSP << 8)      /**< Shifted mode WLCSP for DEVINFO_PKGINFO      */
#define DEVINFO_PKGINFO_PKGTYPE_BGA                              (_DEVINFO_PKGINFO_PKGTYPE_BGA << 8)        /**< Shifted mode BGA for DEVINFO_PKGINFO        */
#define DEVINFO_PKGINFO_PKGTYPE_QFN                              (_DEVINFO_PKGINFO_PKGTYPE_QFN << 8)        /**< Shifted mode QFN for DEVINFO_PKGINFO        */
#define DEVINFO_PKGINFO_PKGTYPE_QFP                              (_DEVINFO_PKGINFO_PKGTYPE_QFP << 8)        /**< Shifted mode QFP for DEVINFO_PKGINFO        */
#define _DEVINFO_PKGINFO_PINCOUNT_SHIFT                          16                                         /**< Shift value for DEVINFO_PINCOUNT            */
#define _DEVINFO_PKGINFO_PINCOUNT_MASK                           0xFF0000UL                                 /**< Bit mask for DEVINFO_PINCOUNT               */
#define _DEVINFO_PKGINFO_PINCOUNT_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for DEVINFO_PKGINFO            */
#define DEVINFO_PKGINFO_PINCOUNT_DEFAULT                         (_DEVINFO_PKGINFO_PINCOUNT_DEFAULT << 16)  /**< Shifted mode DEFAULT for DEVINFO_PKGINFO    */

/* Bit fields for DEVINFO CUSTOMINFO */
#define _DEVINFO_CUSTOMINFO_RESETVALUE                           0x00000000UL                               /**< Default value for DEVINFO_CUSTOMINFO        */
#define _DEVINFO_CUSTOMINFO_MASK                                 0xFFFF0000UL                               /**< Mask for DEVINFO_CUSTOMINFO                 */
#define _DEVINFO_CUSTOMINFO_PARTNO_SHIFT                         16                                         /**< Shift value for DEVINFO_PARTNO              */
#define _DEVINFO_CUSTOMINFO_PARTNO_MASK                          0xFFFF0000UL                               /**< Bit mask for DEVINFO_PARTNO                 */
#define _DEVINFO_CUSTOMINFO_PARTNO_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for DEVINFO_CUSTOMINFO         */
#define DEVINFO_CUSTOMINFO_PARTNO_DEFAULT                        (_DEVINFO_CUSTOMINFO_PARTNO_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_CUSTOMINFO */

/* Bit fields for DEVINFO SWFIX */
#define _DEVINFO_SWFIX_RESETVALUE                                0xFFFFFFFFUL                      /**< Default value for DEVINFO_SWFIX             */
#define _DEVINFO_SWFIX_MASK                                      0xFFFFFFFFUL                      /**< Mask for DEVINFO_SWFIX                      */
#define _DEVINFO_SWFIX_RSV_SHIFT                                 0                                 /**< Shift value for DEVINFO_RSV                 */
#define _DEVINFO_SWFIX_RSV_MASK                                  0xFFFFFFFFUL                      /**< Bit mask for DEVINFO_RSV                    */
#define _DEVINFO_SWFIX_RSV_DEFAULT                               0xFFFFFFFFUL                      /**< Mode DEFAULT for DEVINFO_SWFIX              */
#define DEVINFO_SWFIX_RSV_DEFAULT                                (_DEVINFO_SWFIX_RSV_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_SWFIX      */

/* Bit fields for DEVINFO SWCAPA0 */
#define _DEVINFO_SWCAPA0_RESETVALUE                              0x00000000UL                             /**< Default value for DEVINFO_SWCAPA0           */
#define _DEVINFO_SWCAPA0_MASK                                    0x07333333UL                             /**< Mask for DEVINFO_SWCAPA0                    */
#define _DEVINFO_SWCAPA0_ZIGBEE_SHIFT                            0                                        /**< Shift value for DEVINFO_ZIGBEE              */
#define _DEVINFO_SWCAPA0_ZIGBEE_MASK                             0x3UL                                    /**< Bit mask for DEVINFO_ZIGBEE                 */
#define _DEVINFO_SWCAPA0_ZIGBEE_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for DEVINFO_SWCAPA0            */
#define _DEVINFO_SWCAPA0_ZIGBEE_LEVEL0                           0x00000000UL                             /**< Mode LEVEL0 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_ZIGBEE_LEVEL1                           0x00000001UL                             /**< Mode LEVEL1 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_ZIGBEE_LEVEL2                           0x00000002UL                             /**< Mode LEVEL2 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_ZIGBEE_LEVEL3                           0x00000003UL                             /**< Mode LEVEL3 for DEVINFO_SWCAPA0             */
#define DEVINFO_SWCAPA0_ZIGBEE_DEFAULT                           (_DEVINFO_SWCAPA0_ZIGBEE_DEFAULT << 0)   /**< Shifted mode DEFAULT for DEVINFO_SWCAPA0    */
#define DEVINFO_SWCAPA0_ZIGBEE_LEVEL0                            (_DEVINFO_SWCAPA0_ZIGBEE_LEVEL0 << 0)    /**< Shifted mode LEVEL0 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_ZIGBEE_LEVEL1                            (_DEVINFO_SWCAPA0_ZIGBEE_LEVEL1 << 0)    /**< Shifted mode LEVEL1 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_ZIGBEE_LEVEL2                            (_DEVINFO_SWCAPA0_ZIGBEE_LEVEL2 << 0)    /**< Shifted mode LEVEL2 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_ZIGBEE_LEVEL3                            (_DEVINFO_SWCAPA0_ZIGBEE_LEVEL3 << 0)    /**< Shifted mode LEVEL3 for DEVINFO_SWCAPA0     */
#define _DEVINFO_SWCAPA0_THREAD_SHIFT                            4                                        /**< Shift value for DEVINFO_THREAD              */
#define _DEVINFO_SWCAPA0_THREAD_MASK                             0x30UL                                   /**< Bit mask for DEVINFO_THREAD                 */
#define _DEVINFO_SWCAPA0_THREAD_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for DEVINFO_SWCAPA0            */
#define _DEVINFO_SWCAPA0_THREAD_LEVEL0                           0x00000000UL                             /**< Mode LEVEL0 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_THREAD_LEVEL1                           0x00000001UL                             /**< Mode LEVEL1 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_THREAD_LEVEL2                           0x00000002UL                             /**< Mode LEVEL2 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_THREAD_LEVEL3                           0x00000003UL                             /**< Mode LEVEL3 for DEVINFO_SWCAPA0             */
#define DEVINFO_SWCAPA0_THREAD_DEFAULT                           (_DEVINFO_SWCAPA0_THREAD_DEFAULT << 4)   /**< Shifted mode DEFAULT for DEVINFO_SWCAPA0    */
#define DEVINFO_SWCAPA0_THREAD_LEVEL0                            (_DEVINFO_SWCAPA0_THREAD_LEVEL0 << 4)    /**< Shifted mode LEVEL0 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_THREAD_LEVEL1                            (_DEVINFO_SWCAPA0_THREAD_LEVEL1 << 4)    /**< Shifted mode LEVEL1 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_THREAD_LEVEL2                            (_DEVINFO_SWCAPA0_THREAD_LEVEL2 << 4)    /**< Shifted mode LEVEL2 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_THREAD_LEVEL3                            (_DEVINFO_SWCAPA0_THREAD_LEVEL3 << 4)    /**< Shifted mode LEVEL3 for DEVINFO_SWCAPA0     */
#define _DEVINFO_SWCAPA0_RF4CE_SHIFT                             8                                        /**< Shift value for DEVINFO_RF4CE               */
#define _DEVINFO_SWCAPA0_RF4CE_MASK                              0x300UL                                  /**< Bit mask for DEVINFO_RF4CE                  */
#define _DEVINFO_SWCAPA0_RF4CE_DEFAULT                           0x00000000UL                             /**< Mode DEFAULT for DEVINFO_SWCAPA0            */
#define _DEVINFO_SWCAPA0_RF4CE_LEVEL0                            0x00000000UL                             /**< Mode LEVEL0 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_RF4CE_LEVEL1                            0x00000001UL                             /**< Mode LEVEL1 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_RF4CE_LEVEL2                            0x00000002UL                             /**< Mode LEVEL2 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_RF4CE_LEVEL3                            0x00000003UL                             /**< Mode LEVEL3 for DEVINFO_SWCAPA0             */
#define DEVINFO_SWCAPA0_RF4CE_DEFAULT                            (_DEVINFO_SWCAPA0_RF4CE_DEFAULT << 8)    /**< Shifted mode DEFAULT for DEVINFO_SWCAPA0    */
#define DEVINFO_SWCAPA0_RF4CE_LEVEL0                             (_DEVINFO_SWCAPA0_RF4CE_LEVEL0 << 8)     /**< Shifted mode LEVEL0 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_RF4CE_LEVEL1                             (_DEVINFO_SWCAPA0_RF4CE_LEVEL1 << 8)     /**< Shifted mode LEVEL1 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_RF4CE_LEVEL2                             (_DEVINFO_SWCAPA0_RF4CE_LEVEL2 << 8)     /**< Shifted mode LEVEL2 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_RF4CE_LEVEL3                             (_DEVINFO_SWCAPA0_RF4CE_LEVEL3 << 8)     /**< Shifted mode LEVEL3 for DEVINFO_SWCAPA0     */
#define _DEVINFO_SWCAPA0_BTSMART_SHIFT                           12                                       /**< Shift value for DEVINFO_BTSMART             */
#define _DEVINFO_SWCAPA0_BTSMART_MASK                            0x3000UL                                 /**< Bit mask for DEVINFO_BTSMART                */
#define _DEVINFO_SWCAPA0_BTSMART_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for DEVINFO_SWCAPA0            */
#define _DEVINFO_SWCAPA0_BTSMART_LEVEL0                          0x00000000UL                             /**< Mode LEVEL0 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_BTSMART_LEVEL1                          0x00000001UL                             /**< Mode LEVEL1 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_BTSMART_LEVEL2                          0x00000002UL                             /**< Mode LEVEL2 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_BTSMART_LEVEL3                          0x00000003UL                             /**< Mode LEVEL3 for DEVINFO_SWCAPA0             */
#define DEVINFO_SWCAPA0_BTSMART_DEFAULT                          (_DEVINFO_SWCAPA0_BTSMART_DEFAULT << 12) /**< Shifted mode DEFAULT for DEVINFO_SWCAPA0    */
#define DEVINFO_SWCAPA0_BTSMART_LEVEL0                           (_DEVINFO_SWCAPA0_BTSMART_LEVEL0 << 12)  /**< Shifted mode LEVEL0 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_BTSMART_LEVEL1                           (_DEVINFO_SWCAPA0_BTSMART_LEVEL1 << 12)  /**< Shifted mode LEVEL1 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_BTSMART_LEVEL2                           (_DEVINFO_SWCAPA0_BTSMART_LEVEL2 << 12)  /**< Shifted mode LEVEL2 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_BTSMART_LEVEL3                           (_DEVINFO_SWCAPA0_BTSMART_LEVEL3 << 12)  /**< Shifted mode LEVEL3 for DEVINFO_SWCAPA0     */
#define _DEVINFO_SWCAPA0_CONNECT_SHIFT                           16                                       /**< Shift value for DEVINFO_CONNECT             */
#define _DEVINFO_SWCAPA0_CONNECT_MASK                            0x30000UL                                /**< Bit mask for DEVINFO_CONNECT                */
#define _DEVINFO_SWCAPA0_CONNECT_DEFAULT                         0x00000000UL                             /**< Mode DEFAULT for DEVINFO_SWCAPA0            */
#define _DEVINFO_SWCAPA0_CONNECT_LEVEL0                          0x00000000UL                             /**< Mode LEVEL0 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_CONNECT_LEVEL1                          0x00000001UL                             /**< Mode LEVEL1 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_CONNECT_LEVEL2                          0x00000002UL                             /**< Mode LEVEL2 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_CONNECT_LEVEL3                          0x00000003UL                             /**< Mode LEVEL3 for DEVINFO_SWCAPA0             */
#define DEVINFO_SWCAPA0_CONNECT_DEFAULT                          (_DEVINFO_SWCAPA0_CONNECT_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_SWCAPA0    */
#define DEVINFO_SWCAPA0_CONNECT_LEVEL0                           (_DEVINFO_SWCAPA0_CONNECT_LEVEL0 << 16)  /**< Shifted mode LEVEL0 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_CONNECT_LEVEL1                           (_DEVINFO_SWCAPA0_CONNECT_LEVEL1 << 16)  /**< Shifted mode LEVEL1 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_CONNECT_LEVEL2                           (_DEVINFO_SWCAPA0_CONNECT_LEVEL2 << 16)  /**< Shifted mode LEVEL2 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_CONNECT_LEVEL3                           (_DEVINFO_SWCAPA0_CONNECT_LEVEL3 << 16)  /**< Shifted mode LEVEL3 for DEVINFO_SWCAPA0     */
#define _DEVINFO_SWCAPA0_SRI_SHIFT                               20                                       /**< Shift value for DEVINFO_SRI                 */
#define _DEVINFO_SWCAPA0_SRI_MASK                                0x300000UL                               /**< Bit mask for DEVINFO_SRI                    */
#define _DEVINFO_SWCAPA0_SRI_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for DEVINFO_SWCAPA0            */
#define _DEVINFO_SWCAPA0_SRI_LEVEL0                              0x00000000UL                             /**< Mode LEVEL0 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_SRI_LEVEL1                              0x00000001UL                             /**< Mode LEVEL1 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_SRI_LEVEL2                              0x00000002UL                             /**< Mode LEVEL2 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_SRI_LEVEL3                              0x00000003UL                             /**< Mode LEVEL3 for DEVINFO_SWCAPA0             */
#define DEVINFO_SWCAPA0_SRI_DEFAULT                              (_DEVINFO_SWCAPA0_SRI_DEFAULT << 20)     /**< Shifted mode DEFAULT for DEVINFO_SWCAPA0    */
#define DEVINFO_SWCAPA0_SRI_LEVEL0                               (_DEVINFO_SWCAPA0_SRI_LEVEL0 << 20)      /**< Shifted mode LEVEL0 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_SRI_LEVEL1                               (_DEVINFO_SWCAPA0_SRI_LEVEL1 << 20)      /**< Shifted mode LEVEL1 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_SRI_LEVEL2                               (_DEVINFO_SWCAPA0_SRI_LEVEL2 << 20)      /**< Shifted mode LEVEL2 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_SRI_LEVEL3                               (_DEVINFO_SWCAPA0_SRI_LEVEL3 << 20)      /**< Shifted mode LEVEL3 for DEVINFO_SWCAPA0     */
#define _DEVINFO_SWCAPA0_ZWAVE_SHIFT                             24                                       /**< Shift value for DEVINFO_ZWAVE               */
#define _DEVINFO_SWCAPA0_ZWAVE_MASK                              0x7000000UL                              /**< Bit mask for DEVINFO_ZWAVE                  */
#define _DEVINFO_SWCAPA0_ZWAVE_DEFAULT                           0x00000000UL                             /**< Mode DEFAULT for DEVINFO_SWCAPA0            */
#define _DEVINFO_SWCAPA0_ZWAVE_LEVEL0                            0x00000000UL                             /**< Mode LEVEL0 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_ZWAVE_LEVEL1                            0x00000001UL                             /**< Mode LEVEL1 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_ZWAVE_LEVEL2                            0x00000002UL                             /**< Mode LEVEL2 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_ZWAVE_LEVEL3                            0x00000003UL                             /**< Mode LEVEL3 for DEVINFO_SWCAPA0             */
#define _DEVINFO_SWCAPA0_ZWAVE_LEVEL4                            0x00000004UL                             /**< Mode LEVEL4 for DEVINFO_SWCAPA0             */
#define DEVINFO_SWCAPA0_ZWAVE_DEFAULT                            (_DEVINFO_SWCAPA0_ZWAVE_DEFAULT << 24)   /**< Shifted mode DEFAULT for DEVINFO_SWCAPA0    */
#define DEVINFO_SWCAPA0_ZWAVE_LEVEL0                             (_DEVINFO_SWCAPA0_ZWAVE_LEVEL0 << 24)    /**< Shifted mode LEVEL0 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_ZWAVE_LEVEL1                             (_DEVINFO_SWCAPA0_ZWAVE_LEVEL1 << 24)    /**< Shifted mode LEVEL1 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_ZWAVE_LEVEL2                             (_DEVINFO_SWCAPA0_ZWAVE_LEVEL2 << 24)    /**< Shifted mode LEVEL2 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_ZWAVE_LEVEL3                             (_DEVINFO_SWCAPA0_ZWAVE_LEVEL3 << 24)    /**< Shifted mode LEVEL3 for DEVINFO_SWCAPA0     */
#define DEVINFO_SWCAPA0_ZWAVE_LEVEL4                             (_DEVINFO_SWCAPA0_ZWAVE_LEVEL4 << 24)    /**< Shifted mode LEVEL4 for DEVINFO_SWCAPA0     */

/* Bit fields for DEVINFO SWCAPA1 */
#define _DEVINFO_SWCAPA1_RESETVALUE                              0x00000000UL                            /**< Default value for DEVINFO_SWCAPA1           */
#define _DEVINFO_SWCAPA1_MASK                                    0x0000001FUL                            /**< Mask for DEVINFO_SWCAPA1                    */
#define DEVINFO_SWCAPA1_RFMCUEN                                  (0x1UL << 0)                            /**< RF-MCU                                      */
#define _DEVINFO_SWCAPA1_RFMCUEN_SHIFT                           0                                       /**< Shift value for DEVINFO_RFMCUEN             */
#define _DEVINFO_SWCAPA1_RFMCUEN_MASK                            0x1UL                                   /**< Bit mask for DEVINFO_RFMCUEN                */
#define _DEVINFO_SWCAPA1_RFMCUEN_DEFAULT                         0x00000000UL                            /**< Mode DEFAULT for DEVINFO_SWCAPA1            */
#define DEVINFO_SWCAPA1_RFMCUEN_DEFAULT                          (_DEVINFO_SWCAPA1_RFMCUEN_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_SWCAPA1    */
#define DEVINFO_SWCAPA1_NCPEN                                    (0x1UL << 1)                            /**< NCP                                         */
#define _DEVINFO_SWCAPA1_NCPEN_SHIFT                             1                                       /**< Shift value for DEVINFO_NCPEN               */
#define _DEVINFO_SWCAPA1_NCPEN_MASK                              0x2UL                                   /**< Bit mask for DEVINFO_NCPEN                  */
#define _DEVINFO_SWCAPA1_NCPEN_DEFAULT                           0x00000000UL                            /**< Mode DEFAULT for DEVINFO_SWCAPA1            */
#define DEVINFO_SWCAPA1_NCPEN_DEFAULT                            (_DEVINFO_SWCAPA1_NCPEN_DEFAULT << 1)   /**< Shifted mode DEFAULT for DEVINFO_SWCAPA1    */
#define DEVINFO_SWCAPA1_GWEN                                     (0x1UL << 2)                            /**< Gateway                                     */
#define _DEVINFO_SWCAPA1_GWEN_SHIFT                              2                                       /**< Shift value for DEVINFO_GWEN                */
#define _DEVINFO_SWCAPA1_GWEN_MASK                               0x4UL                                   /**< Bit mask for DEVINFO_GWEN                   */
#define _DEVINFO_SWCAPA1_GWEN_DEFAULT                            0x00000000UL                            /**< Mode DEFAULT for DEVINFO_SWCAPA1            */
#define DEVINFO_SWCAPA1_GWEN_DEFAULT                             (_DEVINFO_SWCAPA1_GWEN_DEFAULT << 2)    /**< Shifted mode DEFAULT for DEVINFO_SWCAPA1    */
#define DEVINFO_SWCAPA1_XOUT                                     (0x1UL << 3)                            /**< XOUT                                        */
#define _DEVINFO_SWCAPA1_XOUT_SHIFT                              3                                       /**< Shift value for DEVINFO_XOUT                */
#define _DEVINFO_SWCAPA1_XOUT_MASK                               0x8UL                                   /**< Bit mask for DEVINFO_XOUT                   */
#define _DEVINFO_SWCAPA1_XOUT_DEFAULT                            0x00000000UL                            /**< Mode DEFAULT for DEVINFO_SWCAPA1            */
#define DEVINFO_SWCAPA1_XOUT_DEFAULT                             (_DEVINFO_SWCAPA1_XOUT_DEFAULT << 3)    /**< Shifted mode DEFAULT for DEVINFO_SWCAPA1    */

/* Bit fields for DEVINFO EXTINFO */
#define _DEVINFO_EXTINFO_RESETVALUE                              0x00000000UL                               /**< Default value for DEVINFO_EXTINFO           */
#define _DEVINFO_EXTINFO_MASK                                    0x00FFFFFFUL                               /**< Mask for DEVINFO_EXTINFO                    */
#define _DEVINFO_EXTINFO_TYPE_SHIFT                              0                                          /**< Shift value for DEVINFO_TYPE                */
#define _DEVINFO_EXTINFO_TYPE_MASK                               0xFFUL                                     /**< Bit mask for DEVINFO_TYPE                   */
#define _DEVINFO_EXTINFO_TYPE_DEFAULT                            0x00000000UL                               /**< Mode DEFAULT for DEVINFO_EXTINFO            */
#define _DEVINFO_EXTINFO_TYPE_NONE                               0x000000FFUL                               /**< Mode NONE for DEVINFO_EXTINFO               */
#define DEVINFO_EXTINFO_TYPE_DEFAULT                             (_DEVINFO_EXTINFO_TYPE_DEFAULT << 0)       /**< Shifted mode DEFAULT for DEVINFO_EXTINFO    */
#define DEVINFO_EXTINFO_TYPE_NONE                                (_DEVINFO_EXTINFO_TYPE_NONE << 0)          /**< Shifted mode NONE for DEVINFO_EXTINFO       */
#define _DEVINFO_EXTINFO_CONNECTION_SHIFT                        8                                          /**< Shift value for DEVINFO_CONNECTION          */
#define _DEVINFO_EXTINFO_CONNECTION_MASK                         0xFF00UL                                   /**< Bit mask for DEVINFO_CONNECTION             */
#define _DEVINFO_EXTINFO_CONNECTION_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for DEVINFO_EXTINFO            */
#define _DEVINFO_EXTINFO_CONNECTION_SPI                          0x00000000UL                               /**< Mode SPI for DEVINFO_EXTINFO                */
#define _DEVINFO_EXTINFO_CONNECTION_NONE                         0x000000FFUL                               /**< Mode NONE for DEVINFO_EXTINFO               */
#define DEVINFO_EXTINFO_CONNECTION_DEFAULT                       (_DEVINFO_EXTINFO_CONNECTION_DEFAULT << 8) /**< Shifted mode DEFAULT for DEVINFO_EXTINFO    */
#define DEVINFO_EXTINFO_CONNECTION_SPI                           (_DEVINFO_EXTINFO_CONNECTION_SPI << 8)     /**< Shifted mode SPI for DEVINFO_EXTINFO        */
#define DEVINFO_EXTINFO_CONNECTION_NONE                          (_DEVINFO_EXTINFO_CONNECTION_NONE << 8)    /**< Shifted mode NONE for DEVINFO_EXTINFO       */
#define _DEVINFO_EXTINFO_REV_SHIFT                               16                                         /**< Shift value for DEVINFO_REV                 */
#define _DEVINFO_EXTINFO_REV_MASK                                0xFF0000UL                                 /**< Bit mask for DEVINFO_REV                    */
#define _DEVINFO_EXTINFO_REV_DEFAULT                             0x00000000UL                               /**< Mode DEFAULT for DEVINFO_EXTINFO            */
#define DEVINFO_EXTINFO_REV_DEFAULT                              (_DEVINFO_EXTINFO_REV_DEFAULT << 16)       /**< Shifted mode DEFAULT for DEVINFO_EXTINFO    */

/* Bit fields for DEVINFO EUI48L */
#define _DEVINFO_EUI48L_RESETVALUE                               0x00000000UL                            /**< Default value for DEVINFO_EUI48L            */
#define _DEVINFO_EUI48L_MASK                                     0xFFFFFFFFUL                            /**< Mask for DEVINFO_EUI48L                     */
#define _DEVINFO_EUI48L_UNIQUEID_SHIFT                           0                                       /**< Shift value for DEVINFO_UNIQUEID            */
#define _DEVINFO_EUI48L_UNIQUEID_MASK                            0xFFFFFFUL                              /**< Bit mask for DEVINFO_UNIQUEID               */
#define _DEVINFO_EUI48L_UNIQUEID_DEFAULT                         0x00000000UL                            /**< Mode DEFAULT for DEVINFO_EUI48L             */
#define DEVINFO_EUI48L_UNIQUEID_DEFAULT                          (_DEVINFO_EUI48L_UNIQUEID_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_EUI48L     */
#define _DEVINFO_EUI48L_OUI48L_SHIFT                             24                                      /**< Shift value for DEVINFO_OUI48L              */
#define _DEVINFO_EUI48L_OUI48L_MASK                              0xFF000000UL                            /**< Bit mask for DEVINFO_OUI48L                 */
#define _DEVINFO_EUI48L_OUI48L_DEFAULT                           0x00000000UL                            /**< Mode DEFAULT for DEVINFO_EUI48L             */
#define DEVINFO_EUI48L_OUI48L_DEFAULT                            (_DEVINFO_EUI48L_OUI48L_DEFAULT << 24)  /**< Shifted mode DEFAULT for DEVINFO_EUI48L     */

/* Bit fields for DEVINFO EUI48H */
#define _DEVINFO_EUI48H_RESETVALUE                               0xFFFF0000UL                             /**< Default value for DEVINFO_EUI48H            */
#define _DEVINFO_EUI48H_MASK                                     0xFFFFFFFFUL                             /**< Mask for DEVINFO_EUI48H                     */
#define _DEVINFO_EUI48H_OUI48H_SHIFT                             0                                        /**< Shift value for DEVINFO_OUI48H              */
#define _DEVINFO_EUI48H_OUI48H_MASK                              0xFFFFUL                                 /**< Bit mask for DEVINFO_OUI48H                 */
#define _DEVINFO_EUI48H_OUI48H_DEFAULT                           0x00000000UL                             /**< Mode DEFAULT for DEVINFO_EUI48H             */
#define DEVINFO_EUI48H_OUI48H_DEFAULT                            (_DEVINFO_EUI48H_OUI48H_DEFAULT << 0)    /**< Shifted mode DEFAULT for DEVINFO_EUI48H     */
#define _DEVINFO_EUI48H_RESERVED_SHIFT                           16                                       /**< Shift value for DEVINFO_RESERVED            */
#define _DEVINFO_EUI48H_RESERVED_MASK                            0xFFFF0000UL                             /**< Bit mask for DEVINFO_RESERVED               */
#define _DEVINFO_EUI48H_RESERVED_DEFAULT                         0x0000FFFFUL                             /**< Mode DEFAULT for DEVINFO_EUI48H             */
#define DEVINFO_EUI48H_RESERVED_DEFAULT                          (_DEVINFO_EUI48H_RESERVED_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_EUI48H     */

/* Bit fields for DEVINFO EUI64L */
#define _DEVINFO_EUI64L_RESETVALUE                               0x00000000UL                           /**< Default value for DEVINFO_EUI64L            */
#define _DEVINFO_EUI64L_MASK                                     0xFFFFFFFFUL                           /**< Mask for DEVINFO_EUI64L                     */
#define _DEVINFO_EUI64L_UNIQUEL_SHIFT                            0                                      /**< Shift value for DEVINFO_UNIQUEL             */
#define _DEVINFO_EUI64L_UNIQUEL_MASK                             0xFFFFFFFFUL                           /**< Bit mask for DEVINFO_UNIQUEL                */
#define _DEVINFO_EUI64L_UNIQUEL_DEFAULT                          0x00000000UL                           /**< Mode DEFAULT for DEVINFO_EUI64L             */
#define DEVINFO_EUI64L_UNIQUEL_DEFAULT                           (_DEVINFO_EUI64L_UNIQUEL_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_EUI64L     */

/* Bit fields for DEVINFO EUI64H */
#define _DEVINFO_EUI64H_RESETVALUE                               0x00000000UL                           /**< Default value for DEVINFO_EUI64H            */
#define _DEVINFO_EUI64H_MASK                                     0xFFFFFFFFUL                           /**< Mask for DEVINFO_EUI64H                     */
#define _DEVINFO_EUI64H_UNIQUEH_SHIFT                            0                                      /**< Shift value for DEVINFO_UNIQUEH             */
#define _DEVINFO_EUI64H_UNIQUEH_MASK                             0xFFUL                                 /**< Bit mask for DEVINFO_UNIQUEH                */
#define _DEVINFO_EUI64H_UNIQUEH_DEFAULT                          0x00000000UL                           /**< Mode DEFAULT for DEVINFO_EUI64H             */
#define DEVINFO_EUI64H_UNIQUEH_DEFAULT                           (_DEVINFO_EUI64H_UNIQUEH_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_EUI64H     */
#define _DEVINFO_EUI64H_OUI64_SHIFT                              8                                      /**< Shift value for DEVINFO_OUI64               */
#define _DEVINFO_EUI64H_OUI64_MASK                               0xFFFFFF00UL                           /**< Bit mask for DEVINFO_OUI64                  */
#define _DEVINFO_EUI64H_OUI64_DEFAULT                            0x00000000UL                           /**< Mode DEFAULT for DEVINFO_EUI64H             */
#define DEVINFO_EUI64H_OUI64_DEFAULT                             (_DEVINFO_EUI64H_OUI64_DEFAULT << 8)   /**< Shifted mode DEFAULT for DEVINFO_EUI64H     */

/* Bit fields for DEVINFO CALTEMP */
#define _DEVINFO_CALTEMP_RESETVALUE                              0x00000000UL                         /**< Default value for DEVINFO_CALTEMP           */
#define _DEVINFO_CALTEMP_MASK                                    0x000000FFUL                         /**< Mask for DEVINFO_CALTEMP                    */
#define _DEVINFO_CALTEMP_TEMP_SHIFT                              0                                    /**< Shift value for DEVINFO_TEMP                */
#define _DEVINFO_CALTEMP_TEMP_MASK                               0xFFUL                               /**< Bit mask for DEVINFO_TEMP                   */
#define _DEVINFO_CALTEMP_TEMP_DEFAULT                            0x00000000UL                         /**< Mode DEFAULT for DEVINFO_CALTEMP            */
#define DEVINFO_CALTEMP_TEMP_DEFAULT                             (_DEVINFO_CALTEMP_TEMP_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_CALTEMP    */

/* Bit fields for DEVINFO EMUTEMP */
#define _DEVINFO_EMUTEMP_RESETVALUE                              0x00000000UL                                /**< Default value for DEVINFO_EMUTEMP           */
#define _DEVINFO_EMUTEMP_MASK                                    0x1FFF07FCUL                                /**< Mask for DEVINFO_EMUTEMP                    */
#define _DEVINFO_EMUTEMP_EMUTEMPROOM_SHIFT                       2                                           /**< Shift value for DEVINFO_EMUTEMPROOM         */
#define _DEVINFO_EMUTEMP_EMUTEMPROOM_MASK                        0x7FCUL                                     /**< Bit mask for DEVINFO_EMUTEMPROOM            */
#define _DEVINFO_EMUTEMP_EMUTEMPROOM_DEFAULT                     0x00000000UL                                /**< Mode DEFAULT for DEVINFO_EMUTEMP            */
#define DEVINFO_EMUTEMP_EMUTEMPROOM_DEFAULT                      (_DEVINFO_EMUTEMP_EMUTEMPROOM_DEFAULT << 2) /**< Shifted mode DEFAULT for DEVINFO_EMUTEMP    */

/* Bit fields for DEVINFO HFRCODPLLCAL */
#define _DEVINFO_HFRCODPLLCAL_RESETVALUE                         0x00000000UL                                    /**< Default value for DEVINFO_HFRCODPLLCAL      */
#define _DEVINFO_HFRCODPLLCAL_MASK                               0xFFFFBF7FUL                                    /**< Mask for DEVINFO_HFRCODPLLCAL               */
#define _DEVINFO_HFRCODPLLCAL_TUNING_SHIFT                       0                                               /**< Shift value for DEVINFO_TUNING              */
#define _DEVINFO_HFRCODPLLCAL_TUNING_MASK                        0x7FUL                                          /**< Bit mask for DEVINFO_TUNING                 */
#define _DEVINFO_HFRCODPLLCAL_TUNING_DEFAULT                     0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCODPLLCAL       */
#define DEVINFO_HFRCODPLLCAL_TUNING_DEFAULT                      (_DEVINFO_HFRCODPLLCAL_TUNING_DEFAULT << 0)     /**< Shifted mode DEFAULT for DEVINFO_HFRCODPLLCAL*/
#define _DEVINFO_HFRCODPLLCAL_FINETUNING_SHIFT                   8                                               /**< Shift value for DEVINFO_FINETUNING          */
#define _DEVINFO_HFRCODPLLCAL_FINETUNING_MASK                    0x3F00UL                                        /**< Bit mask for DEVINFO_FINETUNING             */
#define _DEVINFO_HFRCODPLLCAL_FINETUNING_DEFAULT                 0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCODPLLCAL       */
#define DEVINFO_HFRCODPLLCAL_FINETUNING_DEFAULT                  (_DEVINFO_HFRCODPLLCAL_FINETUNING_DEFAULT << 8) /**< Shifted mode DEFAULT for DEVINFO_HFRCODPLLCAL*/
#define DEVINFO_HFRCODPLLCAL_LDOHP                               (0x1UL << 15)                                   /**<                                             */
#define _DEVINFO_HFRCODPLLCAL_LDOHP_SHIFT                        15                                              /**< Shift value for DEVINFO_LDOHP               */
#define _DEVINFO_HFRCODPLLCAL_LDOHP_MASK                         0x8000UL                                        /**< Bit mask for DEVINFO_LDOHP                  */
#define _DEVINFO_HFRCODPLLCAL_LDOHP_DEFAULT                      0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCODPLLCAL       */
#define DEVINFO_HFRCODPLLCAL_LDOHP_DEFAULT                       (_DEVINFO_HFRCODPLLCAL_LDOHP_DEFAULT << 15)     /**< Shifted mode DEFAULT for DEVINFO_HFRCODPLLCAL*/
#define _DEVINFO_HFRCODPLLCAL_FREQRANGE_SHIFT                    16                                              /**< Shift value for DEVINFO_FREQRANGE           */
#define _DEVINFO_HFRCODPLLCAL_FREQRANGE_MASK                     0x1F0000UL                                      /**< Bit mask for DEVINFO_FREQRANGE              */
#define _DEVINFO_HFRCODPLLCAL_FREQRANGE_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCODPLLCAL       */
#define DEVINFO_HFRCODPLLCAL_FREQRANGE_DEFAULT                   (_DEVINFO_HFRCODPLLCAL_FREQRANGE_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_HFRCODPLLCAL*/
#define _DEVINFO_HFRCODPLLCAL_CMPBIAS_SHIFT                      21                                              /**< Shift value for DEVINFO_CMPBIAS             */
#define _DEVINFO_HFRCODPLLCAL_CMPBIAS_MASK                       0xE00000UL                                      /**< Bit mask for DEVINFO_CMPBIAS                */
#define _DEVINFO_HFRCODPLLCAL_CMPBIAS_DEFAULT                    0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCODPLLCAL       */
#define DEVINFO_HFRCODPLLCAL_CMPBIAS_DEFAULT                     (_DEVINFO_HFRCODPLLCAL_CMPBIAS_DEFAULT << 21)   /**< Shifted mode DEFAULT for DEVINFO_HFRCODPLLCAL*/
#define _DEVINFO_HFRCODPLLCAL_CLKDIV_SHIFT                       24                                              /**< Shift value for DEVINFO_CLKDIV              */
#define _DEVINFO_HFRCODPLLCAL_CLKDIV_MASK                        0x3000000UL                                     /**< Bit mask for DEVINFO_CLKDIV                 */
#define _DEVINFO_HFRCODPLLCAL_CLKDIV_DEFAULT                     0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCODPLLCAL       */
#define DEVINFO_HFRCODPLLCAL_CLKDIV_DEFAULT                      (_DEVINFO_HFRCODPLLCAL_CLKDIV_DEFAULT << 24)    /**< Shifted mode DEFAULT for DEVINFO_HFRCODPLLCAL*/
#define _DEVINFO_HFRCODPLLCAL_CMPSEL_SHIFT                       26                                              /**< Shift value for DEVINFO_CMPSEL              */
#define _DEVINFO_HFRCODPLLCAL_CMPSEL_MASK                        0xC000000UL                                     /**< Bit mask for DEVINFO_CMPSEL                 */
#define _DEVINFO_HFRCODPLLCAL_CMPSEL_DEFAULT                     0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCODPLLCAL       */
#define DEVINFO_HFRCODPLLCAL_CMPSEL_DEFAULT                      (_DEVINFO_HFRCODPLLCAL_CMPSEL_DEFAULT << 26)    /**< Shifted mode DEFAULT for DEVINFO_HFRCODPLLCAL*/
#define _DEVINFO_HFRCODPLLCAL_IREFTC_SHIFT                       28                                              /**< Shift value for DEVINFO_IREFTC              */
#define _DEVINFO_HFRCODPLLCAL_IREFTC_MASK                        0xF0000000UL                                    /**< Bit mask for DEVINFO_IREFTC                 */
#define _DEVINFO_HFRCODPLLCAL_IREFTC_DEFAULT                     0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCODPLLCAL       */
#define DEVINFO_HFRCODPLLCAL_IREFTC_DEFAULT                      (_DEVINFO_HFRCODPLLCAL_IREFTC_DEFAULT << 28)    /**< Shifted mode DEFAULT for DEVINFO_HFRCODPLLCAL*/

/* Bit fields for DEVINFO HFRCOEM23CAL */
#define _DEVINFO_HFRCOEM23CAL_RESETVALUE                         0x00000000UL                                    /**< Default value for DEVINFO_HFRCOEM23CAL      */
#define _DEVINFO_HFRCOEM23CAL_MASK                               0xFFFFBF7FUL                                    /**< Mask for DEVINFO_HFRCOEM23CAL               */
#define _DEVINFO_HFRCOEM23CAL_TUNING_SHIFT                       0                                               /**< Shift value for DEVINFO_TUNING              */
#define _DEVINFO_HFRCOEM23CAL_TUNING_MASK                        0x7FUL                                          /**< Bit mask for DEVINFO_TUNING                 */
#define _DEVINFO_HFRCOEM23CAL_TUNING_DEFAULT                     0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCOEM23CAL       */
#define DEVINFO_HFRCOEM23CAL_TUNING_DEFAULT                      (_DEVINFO_HFRCOEM23CAL_TUNING_DEFAULT << 0)     /**< Shifted mode DEFAULT for DEVINFO_HFRCOEM23CAL*/
#define _DEVINFO_HFRCOEM23CAL_FINETUNING_SHIFT                   8                                               /**< Shift value for DEVINFO_FINETUNING          */
#define _DEVINFO_HFRCOEM23CAL_FINETUNING_MASK                    0x3F00UL                                        /**< Bit mask for DEVINFO_FINETUNING             */
#define _DEVINFO_HFRCOEM23CAL_FINETUNING_DEFAULT                 0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCOEM23CAL       */
#define DEVINFO_HFRCOEM23CAL_FINETUNING_DEFAULT                  (_DEVINFO_HFRCOEM23CAL_FINETUNING_DEFAULT << 8) /**< Shifted mode DEFAULT for DEVINFO_HFRCOEM23CAL*/
#define DEVINFO_HFRCOEM23CAL_LDOHP                               (0x1UL << 15)                                   /**<                                             */
#define _DEVINFO_HFRCOEM23CAL_LDOHP_SHIFT                        15                                              /**< Shift value for DEVINFO_LDOHP               */
#define _DEVINFO_HFRCOEM23CAL_LDOHP_MASK                         0x8000UL                                        /**< Bit mask for DEVINFO_LDOHP                  */
#define _DEVINFO_HFRCOEM23CAL_LDOHP_DEFAULT                      0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCOEM23CAL       */
#define DEVINFO_HFRCOEM23CAL_LDOHP_DEFAULT                       (_DEVINFO_HFRCOEM23CAL_LDOHP_DEFAULT << 15)     /**< Shifted mode DEFAULT for DEVINFO_HFRCOEM23CAL*/
#define _DEVINFO_HFRCOEM23CAL_FREQRANGE_SHIFT                    16                                              /**< Shift value for DEVINFO_FREQRANGE           */
#define _DEVINFO_HFRCOEM23CAL_FREQRANGE_MASK                     0x1F0000UL                                      /**< Bit mask for DEVINFO_FREQRANGE              */
#define _DEVINFO_HFRCOEM23CAL_FREQRANGE_DEFAULT                  0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCOEM23CAL       */
#define DEVINFO_HFRCOEM23CAL_FREQRANGE_DEFAULT                   (_DEVINFO_HFRCOEM23CAL_FREQRANGE_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_HFRCOEM23CAL*/
#define _DEVINFO_HFRCOEM23CAL_CMPBIAS_SHIFT                      21                                              /**< Shift value for DEVINFO_CMPBIAS             */
#define _DEVINFO_HFRCOEM23CAL_CMPBIAS_MASK                       0xE00000UL                                      /**< Bit mask for DEVINFO_CMPBIAS                */
#define _DEVINFO_HFRCOEM23CAL_CMPBIAS_DEFAULT                    0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCOEM23CAL       */
#define DEVINFO_HFRCOEM23CAL_CMPBIAS_DEFAULT                     (_DEVINFO_HFRCOEM23CAL_CMPBIAS_DEFAULT << 21)   /**< Shifted mode DEFAULT for DEVINFO_HFRCOEM23CAL*/
#define _DEVINFO_HFRCOEM23CAL_CLKDIV_SHIFT                       24                                              /**< Shift value for DEVINFO_CLKDIV              */
#define _DEVINFO_HFRCOEM23CAL_CLKDIV_MASK                        0x3000000UL                                     /**< Bit mask for DEVINFO_CLKDIV                 */
#define _DEVINFO_HFRCOEM23CAL_CLKDIV_DEFAULT                     0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCOEM23CAL       */
#define DEVINFO_HFRCOEM23CAL_CLKDIV_DEFAULT                      (_DEVINFO_HFRCOEM23CAL_CLKDIV_DEFAULT << 24)    /**< Shifted mode DEFAULT for DEVINFO_HFRCOEM23CAL*/
#define _DEVINFO_HFRCOEM23CAL_CMPSEL_SHIFT                       26                                              /**< Shift value for DEVINFO_CMPSEL              */
#define _DEVINFO_HFRCOEM23CAL_CMPSEL_MASK                        0xC000000UL                                     /**< Bit mask for DEVINFO_CMPSEL                 */
#define _DEVINFO_HFRCOEM23CAL_CMPSEL_DEFAULT                     0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCOEM23CAL       */
#define DEVINFO_HFRCOEM23CAL_CMPSEL_DEFAULT                      (_DEVINFO_HFRCOEM23CAL_CMPSEL_DEFAULT << 26)    /**< Shifted mode DEFAULT for DEVINFO_HFRCOEM23CAL*/
#define _DEVINFO_HFRCOEM23CAL_IREFTC_SHIFT                       28                                              /**< Shift value for DEVINFO_IREFTC              */
#define _DEVINFO_HFRCOEM23CAL_IREFTC_MASK                        0xF0000000UL                                    /**< Bit mask for DEVINFO_IREFTC                 */
#define _DEVINFO_HFRCOEM23CAL_IREFTC_DEFAULT                     0x00000000UL                                    /**< Mode DEFAULT for DEVINFO_HFRCOEM23CAL       */
#define DEVINFO_HFRCOEM23CAL_IREFTC_DEFAULT                      (_DEVINFO_HFRCOEM23CAL_IREFTC_DEFAULT << 28)    /**< Shifted mode DEFAULT for DEVINFO_HFRCOEM23CAL*/

/* Bit fields for DEVINFO MODULENAME0 */
#define _DEVINFO_MODULENAME0_RESETVALUE                          0xFFFFFFFFUL                                  /**< Default value for DEVINFO_MODULENAME0       */
#define _DEVINFO_MODULENAME0_MASK                                0xFFFFFFFFUL                                  /**< Mask for DEVINFO_MODULENAME0                */
#define _DEVINFO_MODULENAME0_MODCHAR1_SHIFT                      0                                             /**< Shift value for DEVINFO_MODCHAR1            */
#define _DEVINFO_MODULENAME0_MODCHAR1_MASK                       0xFFUL                                        /**< Bit mask for DEVINFO_MODCHAR1               */
#define _DEVINFO_MODULENAME0_MODCHAR1_DEFAULT                    0x000000FFUL                                  /**< Mode DEFAULT for DEVINFO_MODULENAME0        */
#define DEVINFO_MODULENAME0_MODCHAR1_DEFAULT                     (_DEVINFO_MODULENAME0_MODCHAR1_DEFAULT << 0)  /**< Shifted mode DEFAULT for DEVINFO_MODULENAME0*/
#define _DEVINFO_MODULENAME0_MODCHAR2_SHIFT                      8                                             /**< Shift value for DEVINFO_MODCHAR2            */
#define _DEVINFO_MODULENAME0_MODCHAR2_MASK                       0xFF00UL                                      /**< Bit mask for DEVINFO_MODCHAR2               */
#define _DEVINFO_MODULENAME0_MODCHAR2_DEFAULT                    0x000000FFUL                                  /**< Mode DEFAULT for DEVINFO_MODULENAME0        */
#define DEVINFO_MODULENAME0_MODCHAR2_DEFAULT                     (_DEVINFO_MODULENAME0_MODCHAR2_DEFAULT << 8)  /**< Shifted mode DEFAULT for DEVINFO_MODULENAME0*/
#define _DEVINFO_MODULENAME0_MODCHAR3_SHIFT                      16                                            /**< Shift value for DEVINFO_MODCHAR3            */
#define _DEVINFO_MODULENAME0_MODCHAR3_MASK                       0xFF0000UL                                    /**< Bit mask for DEVINFO_MODCHAR3               */
#define _DEVINFO_MODULENAME0_MODCHAR3_DEFAULT                    0x000000FFUL                                  /**< Mode DEFAULT for DEVINFO_MODULENAME0        */
#define DEVINFO_MODULENAME0_MODCHAR3_DEFAULT                     (_DEVINFO_MODULENAME0_MODCHAR3_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME0*/
#define _DEVINFO_MODULENAME0_MODCHAR4_SHIFT                      24                                            /**< Shift value for DEVINFO_MODCHAR4            */
#define _DEVINFO_MODULENAME0_MODCHAR4_MASK                       0xFF000000UL                                  /**< Bit mask for DEVINFO_MODCHAR4               */
#define _DEVINFO_MODULENAME0_MODCHAR4_DEFAULT                    0x000000FFUL                                  /**< Mode DEFAULT for DEVINFO_MODULENAME0        */
#define DEVINFO_MODULENAME0_MODCHAR4_DEFAULT                     (_DEVINFO_MODULENAME0_MODCHAR4_DEFAULT << 24) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME0*/

/* Bit fields for DEVINFO MODULENAME1 */
#define _DEVINFO_MODULENAME1_RESETVALUE                          0xFFFFFFFFUL                                  /**< Default value for DEVINFO_MODULENAME1       */
#define _DEVINFO_MODULENAME1_MASK                                0xFFFFFFFFUL                                  /**< Mask for DEVINFO_MODULENAME1                */
#define _DEVINFO_MODULENAME1_MODCHAR5_SHIFT                      0                                             /**< Shift value for DEVINFO_MODCHAR5            */
#define _DEVINFO_MODULENAME1_MODCHAR5_MASK                       0xFFUL                                        /**< Bit mask for DEVINFO_MODCHAR5               */
#define _DEVINFO_MODULENAME1_MODCHAR5_DEFAULT                    0x000000FFUL                                  /**< Mode DEFAULT for DEVINFO_MODULENAME1        */
#define DEVINFO_MODULENAME1_MODCHAR5_DEFAULT                     (_DEVINFO_MODULENAME1_MODCHAR5_DEFAULT << 0)  /**< Shifted mode DEFAULT for DEVINFO_MODULENAME1*/
#define _DEVINFO_MODULENAME1_MODCHAR6_SHIFT                      8                                             /**< Shift value for DEVINFO_MODCHAR6            */
#define _DEVINFO_MODULENAME1_MODCHAR6_MASK                       0xFF00UL                                      /**< Bit mask for DEVINFO_MODCHAR6               */
#define _DEVINFO_MODULENAME1_MODCHAR6_DEFAULT                    0x000000FFUL                                  /**< Mode DEFAULT for DEVINFO_MODULENAME1        */
#define DEVINFO_MODULENAME1_MODCHAR6_DEFAULT                     (_DEVINFO_MODULENAME1_MODCHAR6_DEFAULT << 8)  /**< Shifted mode DEFAULT for DEVINFO_MODULENAME1*/
#define _DEVINFO_MODULENAME1_MODCHAR7_SHIFT                      16                                            /**< Shift value for DEVINFO_MODCHAR7            */
#define _DEVINFO_MODULENAME1_MODCHAR7_MASK                       0xFF0000UL                                    /**< Bit mask for DEVINFO_MODCHAR7               */
#define _DEVINFO_MODULENAME1_MODCHAR7_DEFAULT                    0x000000FFUL                                  /**< Mode DEFAULT for DEVINFO_MODULENAME1        */
#define DEVINFO_MODULENAME1_MODCHAR7_DEFAULT                     (_DEVINFO_MODULENAME1_MODCHAR7_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME1*/
#define _DEVINFO_MODULENAME1_MODCHAR8_SHIFT                      24                                            /**< Shift value for DEVINFO_MODCHAR8            */
#define _DEVINFO_MODULENAME1_MODCHAR8_MASK                       0xFF000000UL                                  /**< Bit mask for DEVINFO_MODCHAR8               */
#define _DEVINFO_MODULENAME1_MODCHAR8_DEFAULT                    0x000000FFUL                                  /**< Mode DEFAULT for DEVINFO_MODULENAME1        */
#define DEVINFO_MODULENAME1_MODCHAR8_DEFAULT                     (_DEVINFO_MODULENAME1_MODCHAR8_DEFAULT << 24) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME1*/

/* Bit fields for DEVINFO MODULENAME2 */
#define _DEVINFO_MODULENAME2_RESETVALUE                          0xFFFFFFFFUL                                   /**< Default value for DEVINFO_MODULENAME2       */
#define _DEVINFO_MODULENAME2_MASK                                0xFFFFFFFFUL                                   /**< Mask for DEVINFO_MODULENAME2                */
#define _DEVINFO_MODULENAME2_MODCHAR9_SHIFT                      0                                              /**< Shift value for DEVINFO_MODCHAR9            */
#define _DEVINFO_MODULENAME2_MODCHAR9_MASK                       0xFFUL                                         /**< Bit mask for DEVINFO_MODCHAR9               */
#define _DEVINFO_MODULENAME2_MODCHAR9_DEFAULT                    0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME2        */
#define DEVINFO_MODULENAME2_MODCHAR9_DEFAULT                     (_DEVINFO_MODULENAME2_MODCHAR9_DEFAULT << 0)   /**< Shifted mode DEFAULT for DEVINFO_MODULENAME2*/
#define _DEVINFO_MODULENAME2_MODCHAR10_SHIFT                     8                                              /**< Shift value for DEVINFO_MODCHAR10           */
#define _DEVINFO_MODULENAME2_MODCHAR10_MASK                      0xFF00UL                                       /**< Bit mask for DEVINFO_MODCHAR10              */
#define _DEVINFO_MODULENAME2_MODCHAR10_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME2        */
#define DEVINFO_MODULENAME2_MODCHAR10_DEFAULT                    (_DEVINFO_MODULENAME2_MODCHAR10_DEFAULT << 8)  /**< Shifted mode DEFAULT for DEVINFO_MODULENAME2*/
#define _DEVINFO_MODULENAME2_MODCHAR11_SHIFT                     16                                             /**< Shift value for DEVINFO_MODCHAR11           */
#define _DEVINFO_MODULENAME2_MODCHAR11_MASK                      0xFF0000UL                                     /**< Bit mask for DEVINFO_MODCHAR11              */
#define _DEVINFO_MODULENAME2_MODCHAR11_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME2        */
#define DEVINFO_MODULENAME2_MODCHAR11_DEFAULT                    (_DEVINFO_MODULENAME2_MODCHAR11_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME2*/
#define _DEVINFO_MODULENAME2_MODCHAR12_SHIFT                     24                                             /**< Shift value for DEVINFO_MODCHAR12           */
#define _DEVINFO_MODULENAME2_MODCHAR12_MASK                      0xFF000000UL                                   /**< Bit mask for DEVINFO_MODCHAR12              */
#define _DEVINFO_MODULENAME2_MODCHAR12_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME2        */
#define DEVINFO_MODULENAME2_MODCHAR12_DEFAULT                    (_DEVINFO_MODULENAME2_MODCHAR12_DEFAULT << 24) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME2*/

/* Bit fields for DEVINFO MODULENAME3 */
#define _DEVINFO_MODULENAME3_RESETVALUE                          0xFFFFFFFFUL                                   /**< Default value for DEVINFO_MODULENAME3       */
#define _DEVINFO_MODULENAME3_MASK                                0xFFFFFFFFUL                                   /**< Mask for DEVINFO_MODULENAME3                */
#define _DEVINFO_MODULENAME3_MODCHAR13_SHIFT                     0                                              /**< Shift value for DEVINFO_MODCHAR13           */
#define _DEVINFO_MODULENAME3_MODCHAR13_MASK                      0xFFUL                                         /**< Bit mask for DEVINFO_MODCHAR13              */
#define _DEVINFO_MODULENAME3_MODCHAR13_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME3        */
#define DEVINFO_MODULENAME3_MODCHAR13_DEFAULT                    (_DEVINFO_MODULENAME3_MODCHAR13_DEFAULT << 0)  /**< Shifted mode DEFAULT for DEVINFO_MODULENAME3*/
#define _DEVINFO_MODULENAME3_MODCHAR14_SHIFT                     8                                              /**< Shift value for DEVINFO_MODCHAR14           */
#define _DEVINFO_MODULENAME3_MODCHAR14_MASK                      0xFF00UL                                       /**< Bit mask for DEVINFO_MODCHAR14              */
#define _DEVINFO_MODULENAME3_MODCHAR14_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME3        */
#define DEVINFO_MODULENAME3_MODCHAR14_DEFAULT                    (_DEVINFO_MODULENAME3_MODCHAR14_DEFAULT << 8)  /**< Shifted mode DEFAULT for DEVINFO_MODULENAME3*/
#define _DEVINFO_MODULENAME3_MODCHAR15_SHIFT                     16                                             /**< Shift value for DEVINFO_MODCHAR15           */
#define _DEVINFO_MODULENAME3_MODCHAR15_MASK                      0xFF0000UL                                     /**< Bit mask for DEVINFO_MODCHAR15              */
#define _DEVINFO_MODULENAME3_MODCHAR15_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME3        */
#define DEVINFO_MODULENAME3_MODCHAR15_DEFAULT                    (_DEVINFO_MODULENAME3_MODCHAR15_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME3*/
#define _DEVINFO_MODULENAME3_MODCHAR16_SHIFT                     24                                             /**< Shift value for DEVINFO_MODCHAR16           */
#define _DEVINFO_MODULENAME3_MODCHAR16_MASK                      0xFF000000UL                                   /**< Bit mask for DEVINFO_MODCHAR16              */
#define _DEVINFO_MODULENAME3_MODCHAR16_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME3        */
#define DEVINFO_MODULENAME3_MODCHAR16_DEFAULT                    (_DEVINFO_MODULENAME3_MODCHAR16_DEFAULT << 24) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME3*/

/* Bit fields for DEVINFO MODULENAME4 */
#define _DEVINFO_MODULENAME4_RESETVALUE                          0xFFFFFFFFUL                                   /**< Default value for DEVINFO_MODULENAME4       */
#define _DEVINFO_MODULENAME4_MASK                                0xFFFFFFFFUL                                   /**< Mask for DEVINFO_MODULENAME4                */
#define _DEVINFO_MODULENAME4_MODCHAR17_SHIFT                     0                                              /**< Shift value for DEVINFO_MODCHAR17           */
#define _DEVINFO_MODULENAME4_MODCHAR17_MASK                      0xFFUL                                         /**< Bit mask for DEVINFO_MODCHAR17              */
#define _DEVINFO_MODULENAME4_MODCHAR17_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME4        */
#define DEVINFO_MODULENAME4_MODCHAR17_DEFAULT                    (_DEVINFO_MODULENAME4_MODCHAR17_DEFAULT << 0)  /**< Shifted mode DEFAULT for DEVINFO_MODULENAME4*/
#define _DEVINFO_MODULENAME4_MODCHAR18_SHIFT                     8                                              /**< Shift value for DEVINFO_MODCHAR18           */
#define _DEVINFO_MODULENAME4_MODCHAR18_MASK                      0xFF00UL                                       /**< Bit mask for DEVINFO_MODCHAR18              */
#define _DEVINFO_MODULENAME4_MODCHAR18_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME4        */
#define DEVINFO_MODULENAME4_MODCHAR18_DEFAULT                    (_DEVINFO_MODULENAME4_MODCHAR18_DEFAULT << 8)  /**< Shifted mode DEFAULT for DEVINFO_MODULENAME4*/
#define _DEVINFO_MODULENAME4_MODCHAR19_SHIFT                     16                                             /**< Shift value for DEVINFO_MODCHAR19           */
#define _DEVINFO_MODULENAME4_MODCHAR19_MASK                      0xFF0000UL                                     /**< Bit mask for DEVINFO_MODCHAR19              */
#define _DEVINFO_MODULENAME4_MODCHAR19_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME4        */
#define DEVINFO_MODULENAME4_MODCHAR19_DEFAULT                    (_DEVINFO_MODULENAME4_MODCHAR19_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME4*/
#define _DEVINFO_MODULENAME4_MODCHAR20_SHIFT                     24                                             /**< Shift value for DEVINFO_MODCHAR20           */
#define _DEVINFO_MODULENAME4_MODCHAR20_MASK                      0xFF000000UL                                   /**< Bit mask for DEVINFO_MODCHAR20              */
#define _DEVINFO_MODULENAME4_MODCHAR20_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME4        */
#define DEVINFO_MODULENAME4_MODCHAR20_DEFAULT                    (_DEVINFO_MODULENAME4_MODCHAR20_DEFAULT << 24) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME4*/

/* Bit fields for DEVINFO MODULENAME5 */
#define _DEVINFO_MODULENAME5_RESETVALUE                          0xFFFFFFFFUL                                   /**< Default value for DEVINFO_MODULENAME5       */
#define _DEVINFO_MODULENAME5_MASK                                0xFFFFFFFFUL                                   /**< Mask for DEVINFO_MODULENAME5                */
#define _DEVINFO_MODULENAME5_MODCHAR21_SHIFT                     0                                              /**< Shift value for DEVINFO_MODCHAR21           */
#define _DEVINFO_MODULENAME5_MODCHAR21_MASK                      0xFFUL                                         /**< Bit mask for DEVINFO_MODCHAR21              */
#define _DEVINFO_MODULENAME5_MODCHAR21_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME5        */
#define DEVINFO_MODULENAME5_MODCHAR21_DEFAULT                    (_DEVINFO_MODULENAME5_MODCHAR21_DEFAULT << 0)  /**< Shifted mode DEFAULT for DEVINFO_MODULENAME5*/
#define _DEVINFO_MODULENAME5_MODCHAR22_SHIFT                     8                                              /**< Shift value for DEVINFO_MODCHAR22           */
#define _DEVINFO_MODULENAME5_MODCHAR22_MASK                      0xFF00UL                                       /**< Bit mask for DEVINFO_MODCHAR22              */
#define _DEVINFO_MODULENAME5_MODCHAR22_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME5        */
#define DEVINFO_MODULENAME5_MODCHAR22_DEFAULT                    (_DEVINFO_MODULENAME5_MODCHAR22_DEFAULT << 8)  /**< Shifted mode DEFAULT for DEVINFO_MODULENAME5*/
#define _DEVINFO_MODULENAME5_MODCHAR23_SHIFT                     16                                             /**< Shift value for DEVINFO_MODCHAR23           */
#define _DEVINFO_MODULENAME5_MODCHAR23_MASK                      0xFF0000UL                                     /**< Bit mask for DEVINFO_MODCHAR23              */
#define _DEVINFO_MODULENAME5_MODCHAR23_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME5        */
#define DEVINFO_MODULENAME5_MODCHAR23_DEFAULT                    (_DEVINFO_MODULENAME5_MODCHAR23_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME5*/
#define _DEVINFO_MODULENAME5_MODCHAR24_SHIFT                     24                                             /**< Shift value for DEVINFO_MODCHAR24           */
#define _DEVINFO_MODULENAME5_MODCHAR24_MASK                      0xFF000000UL                                   /**< Bit mask for DEVINFO_MODCHAR24              */
#define _DEVINFO_MODULENAME5_MODCHAR24_DEFAULT                   0x000000FFUL                                   /**< Mode DEFAULT for DEVINFO_MODULENAME5        */
#define DEVINFO_MODULENAME5_MODCHAR24_DEFAULT                    (_DEVINFO_MODULENAME5_MODCHAR24_DEFAULT << 24) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME5*/

/* Bit fields for DEVINFO MODULENAME6 */
#define _DEVINFO_MODULENAME6_RESETVALUE                          0xFFFFFFFFUL                                  /**< Default value for DEVINFO_MODULENAME6       */
#define _DEVINFO_MODULENAME6_MASK                                0xFFFFFFFFUL                                  /**< Mask for DEVINFO_MODULENAME6                */
#define _DEVINFO_MODULENAME6_MODCHAR25_SHIFT                     0                                             /**< Shift value for DEVINFO_MODCHAR25           */
#define _DEVINFO_MODULENAME6_MODCHAR25_MASK                      0xFFUL                                        /**< Bit mask for DEVINFO_MODCHAR25              */
#define _DEVINFO_MODULENAME6_MODCHAR25_DEFAULT                   0x000000FFUL                                  /**< Mode DEFAULT for DEVINFO_MODULENAME6        */
#define DEVINFO_MODULENAME6_MODCHAR25_DEFAULT                    (_DEVINFO_MODULENAME6_MODCHAR25_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME6*/
#define _DEVINFO_MODULENAME6_MODCHAR26_SHIFT                     8                                             /**< Shift value for DEVINFO_MODCHAR26           */
#define _DEVINFO_MODULENAME6_MODCHAR26_MASK                      0xFF00UL                                      /**< Bit mask for DEVINFO_MODCHAR26              */
#define _DEVINFO_MODULENAME6_MODCHAR26_DEFAULT                   0x000000FFUL                                  /**< Mode DEFAULT for DEVINFO_MODULENAME6        */
#define DEVINFO_MODULENAME6_MODCHAR26_DEFAULT                    (_DEVINFO_MODULENAME6_MODCHAR26_DEFAULT << 8) /**< Shifted mode DEFAULT for DEVINFO_MODULENAME6*/
#define _DEVINFO_MODULENAME6_RSV_SHIFT                           16                                            /**< Shift value for DEVINFO_RSV                 */
#define _DEVINFO_MODULENAME6_RSV_MASK                            0xFFFF0000UL                                  /**< Bit mask for DEVINFO_RSV                    */
#define _DEVINFO_MODULENAME6_RSV_DEFAULT                         0x0000FFFFUL                                  /**< Mode DEFAULT for DEVINFO_MODULENAME6        */
#define DEVINFO_MODULENAME6_RSV_DEFAULT                          (_DEVINFO_MODULENAME6_RSV_DEFAULT << 16)      /**< Shifted mode DEFAULT for DEVINFO_MODULENAME6*/

/* Bit fields for DEVINFO MODULEINFO */
#define _DEVINFO_MODULEINFO_RESETVALUE                           0xFFFFFFFFUL                                     /**< Default value for DEVINFO_MODULEINFO        */
#define _DEVINFO_MODULEINFO_MASK                                 0xFFFFFFFFUL                                     /**< Mask for DEVINFO_MODULEINFO                 */
#define _DEVINFO_MODULEINFO_HWREV_SHIFT                          0                                                /**< Shift value for DEVINFO_HWREV               */
#define _DEVINFO_MODULEINFO_HWREV_MASK                           0x1FUL                                           /**< Bit mask for DEVINFO_HWREV                  */
#define _DEVINFO_MODULEINFO_HWREV_DEFAULT                        0x0000001FUL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define DEVINFO_MODULEINFO_HWREV_DEFAULT                         (_DEVINFO_MODULEINFO_HWREV_DEFAULT << 0)         /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define _DEVINFO_MODULEINFO_ANTENNA_SHIFT                        5                                                /**< Shift value for DEVINFO_ANTENNA             */
#define _DEVINFO_MODULEINFO_ANTENNA_MASK                         0xE0UL                                           /**< Bit mask for DEVINFO_ANTENNA                */
#define _DEVINFO_MODULEINFO_ANTENNA_DEFAULT                      0x00000007UL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_ANTENNA_BUILTIN                      0x00000000UL                                     /**< Mode BUILTIN for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_ANTENNA_CONNECTOR                    0x00000001UL                                     /**< Mode CONNECTOR for DEVINFO_MODULEINFO       */
#define _DEVINFO_MODULEINFO_ANTENNA_RFPAD                        0x00000002UL                                     /**< Mode RFPAD for DEVINFO_MODULEINFO           */
#define _DEVINFO_MODULEINFO_ANTENNA_INVERTEDF                    0x00000003UL                                     /**< Mode INVERTEDF for DEVINFO_MODULEINFO       */
#define DEVINFO_MODULEINFO_ANTENNA_DEFAULT                       (_DEVINFO_MODULEINFO_ANTENNA_DEFAULT << 5)       /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_ANTENNA_BUILTIN                       (_DEVINFO_MODULEINFO_ANTENNA_BUILTIN << 5)       /**< Shifted mode BUILTIN for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_ANTENNA_CONNECTOR                     (_DEVINFO_MODULEINFO_ANTENNA_CONNECTOR << 5)     /**< Shifted mode CONNECTOR for DEVINFO_MODULEINFO*/
#define DEVINFO_MODULEINFO_ANTENNA_RFPAD                         (_DEVINFO_MODULEINFO_ANTENNA_RFPAD << 5)         /**< Shifted mode RFPAD for DEVINFO_MODULEINFO   */
#define DEVINFO_MODULEINFO_ANTENNA_INVERTEDF                     (_DEVINFO_MODULEINFO_ANTENNA_INVERTEDF << 5)     /**< Shifted mode INVERTEDF for DEVINFO_MODULEINFO*/
#define _DEVINFO_MODULEINFO_MODNUMBER_SHIFT                      8                                                /**< Shift value for DEVINFO_MODNUMBER           */
#define _DEVINFO_MODULEINFO_MODNUMBER_MASK                       0x7F00UL                                         /**< Bit mask for DEVINFO_MODNUMBER              */
#define _DEVINFO_MODULEINFO_MODNUMBER_DEFAULT                    0x0000007FUL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define DEVINFO_MODULEINFO_MODNUMBER_DEFAULT                     (_DEVINFO_MODULEINFO_MODNUMBER_DEFAULT << 8)     /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_TYPE                                  (0x1UL << 15)                                    /**<                                             */
#define _DEVINFO_MODULEINFO_TYPE_SHIFT                           15                                               /**< Shift value for DEVINFO_TYPE                */
#define _DEVINFO_MODULEINFO_TYPE_MASK                            0x8000UL                                         /**< Bit mask for DEVINFO_TYPE                   */
#define _DEVINFO_MODULEINFO_TYPE_DEFAULT                         0x00000001UL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_TYPE_PCB                             0x00000000UL                                     /**< Mode PCB for DEVINFO_MODULEINFO             */
#define _DEVINFO_MODULEINFO_TYPE_SIP                             0x00000001UL                                     /**< Mode SIP for DEVINFO_MODULEINFO             */
#define DEVINFO_MODULEINFO_TYPE_DEFAULT                          (_DEVINFO_MODULEINFO_TYPE_DEFAULT << 15)         /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_TYPE_PCB                              (_DEVINFO_MODULEINFO_TYPE_PCB << 15)             /**< Shifted mode PCB for DEVINFO_MODULEINFO     */
#define DEVINFO_MODULEINFO_TYPE_SIP                              (_DEVINFO_MODULEINFO_TYPE_SIP << 15)             /**< Shifted mode SIP for DEVINFO_MODULEINFO     */
#define DEVINFO_MODULEINFO_LFXO                                  (0x1UL << 16)                                    /**<                                             */
#define _DEVINFO_MODULEINFO_LFXO_SHIFT                           16                                               /**< Shift value for DEVINFO_LFXO                */
#define _DEVINFO_MODULEINFO_LFXO_MASK                            0x10000UL                                        /**< Bit mask for DEVINFO_LFXO                   */
#define _DEVINFO_MODULEINFO_LFXO_DEFAULT                         0x00000001UL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_LFXO_NONE                            0x00000000UL                                     /**< Mode NONE for DEVINFO_MODULEINFO            */
#define _DEVINFO_MODULEINFO_LFXO_PRESENT                         0x00000001UL                                     /**< Mode PRESENT for DEVINFO_MODULEINFO         */
#define DEVINFO_MODULEINFO_LFXO_DEFAULT                          (_DEVINFO_MODULEINFO_LFXO_DEFAULT << 16)         /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_LFXO_NONE                             (_DEVINFO_MODULEINFO_LFXO_NONE << 16)            /**< Shifted mode NONE for DEVINFO_MODULEINFO    */
#define DEVINFO_MODULEINFO_LFXO_PRESENT                          (_DEVINFO_MODULEINFO_LFXO_PRESENT << 16)         /**< Shifted mode PRESENT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_EXPRESS                               (0x1UL << 17)                                    /**<                                             */
#define _DEVINFO_MODULEINFO_EXPRESS_SHIFT                        17                                               /**< Shift value for DEVINFO_EXPRESS             */
#define _DEVINFO_MODULEINFO_EXPRESS_MASK                         0x20000UL                                        /**< Bit mask for DEVINFO_EXPRESS                */
#define _DEVINFO_MODULEINFO_EXPRESS_DEFAULT                      0x00000001UL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_EXPRESS_SUPPORTED                    0x00000000UL                                     /**< Mode SUPPORTED for DEVINFO_MODULEINFO       */
#define _DEVINFO_MODULEINFO_EXPRESS_NONE                         0x00000001UL                                     /**< Mode NONE for DEVINFO_MODULEINFO            */
#define DEVINFO_MODULEINFO_EXPRESS_DEFAULT                       (_DEVINFO_MODULEINFO_EXPRESS_DEFAULT << 17)      /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_EXPRESS_SUPPORTED                     (_DEVINFO_MODULEINFO_EXPRESS_SUPPORTED << 17)    /**< Shifted mode SUPPORTED for DEVINFO_MODULEINFO*/
#define DEVINFO_MODULEINFO_EXPRESS_NONE                          (_DEVINFO_MODULEINFO_EXPRESS_NONE << 17)         /**< Shifted mode NONE for DEVINFO_MODULEINFO    */
#define DEVINFO_MODULEINFO_LFXOCALVAL                            (0x1UL << 18)                                    /**<                                             */
#define _DEVINFO_MODULEINFO_LFXOCALVAL_SHIFT                     18                                               /**< Shift value for DEVINFO_LFXOCALVAL          */
#define _DEVINFO_MODULEINFO_LFXOCALVAL_MASK                      0x40000UL                                        /**< Bit mask for DEVINFO_LFXOCALVAL             */
#define _DEVINFO_MODULEINFO_LFXOCALVAL_DEFAULT                   0x00000001UL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_LFXOCALVAL_VALID                     0x00000000UL                                     /**< Mode VALID for DEVINFO_MODULEINFO           */
#define _DEVINFO_MODULEINFO_LFXOCALVAL_NOTVALID                  0x00000001UL                                     /**< Mode NOTVALID for DEVINFO_MODULEINFO        */
#define DEVINFO_MODULEINFO_LFXOCALVAL_DEFAULT                    (_DEVINFO_MODULEINFO_LFXOCALVAL_DEFAULT << 18)   /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_LFXOCALVAL_VALID                      (_DEVINFO_MODULEINFO_LFXOCALVAL_VALID << 18)     /**< Shifted mode VALID for DEVINFO_MODULEINFO   */
#define DEVINFO_MODULEINFO_LFXOCALVAL_NOTVALID                   (_DEVINFO_MODULEINFO_LFXOCALVAL_NOTVALID << 18)  /**< Shifted mode NOTVALID for DEVINFO_MODULEINFO*/
#define DEVINFO_MODULEINFO_HFXOCALVAL                            (0x1UL << 19)                                    /**<                                             */
#define _DEVINFO_MODULEINFO_HFXOCALVAL_SHIFT                     19                                               /**< Shift value for DEVINFO_HFXOCALVAL          */
#define _DEVINFO_MODULEINFO_HFXOCALVAL_MASK                      0x80000UL                                        /**< Bit mask for DEVINFO_HFXOCALVAL             */
#define _DEVINFO_MODULEINFO_HFXOCALVAL_DEFAULT                   0x00000001UL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_HFXOCALVAL_VALID                     0x00000000UL                                     /**< Mode VALID for DEVINFO_MODULEINFO           */
#define _DEVINFO_MODULEINFO_HFXOCALVAL_NOTVALID                  0x00000001UL                                     /**< Mode NOTVALID for DEVINFO_MODULEINFO        */
#define DEVINFO_MODULEINFO_HFXOCALVAL_DEFAULT                    (_DEVINFO_MODULEINFO_HFXOCALVAL_DEFAULT << 19)   /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_HFXOCALVAL_VALID                      (_DEVINFO_MODULEINFO_HFXOCALVAL_VALID << 19)     /**< Shifted mode VALID for DEVINFO_MODULEINFO   */
#define DEVINFO_MODULEINFO_HFXOCALVAL_NOTVALID                   (_DEVINFO_MODULEINFO_HFXOCALVAL_NOTVALID << 19)  /**< Shifted mode NOTVALID for DEVINFO_MODULEINFO*/
#define _DEVINFO_MODULEINFO_MODNUMBERMSB_SHIFT                   20                                               /**< Shift value for DEVINFO_MODNUMBERMSB        */
#define _DEVINFO_MODULEINFO_MODNUMBERMSB_MASK                    0x1FF00000UL                                     /**< Bit mask for DEVINFO_MODNUMBERMSB           */
#define _DEVINFO_MODULEINFO_MODNUMBERMSB_DEFAULT                 0x000001FFUL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define DEVINFO_MODULEINFO_MODNUMBERMSB_DEFAULT                  (_DEVINFO_MODULEINFO_MODNUMBERMSB_DEFAULT << 20) /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_PADCDC                                (0x1UL << 29)                                    /**<                                             */
#define _DEVINFO_MODULEINFO_PADCDC_SHIFT                         29                                               /**< Shift value for DEVINFO_PADCDC              */
#define _DEVINFO_MODULEINFO_PADCDC_MASK                          0x20000000UL                                     /**< Bit mask for DEVINFO_PADCDC                 */
#define _DEVINFO_MODULEINFO_PADCDC_DEFAULT                       0x00000001UL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_PADCDC_VDCDC                         0x00000000UL                                     /**< Mode VDCDC for DEVINFO_MODULEINFO           */
#define _DEVINFO_MODULEINFO_PADCDC_OTHER                         0x00000001UL                                     /**< Mode OTHER for DEVINFO_MODULEINFO           */
#define DEVINFO_MODULEINFO_PADCDC_DEFAULT                        (_DEVINFO_MODULEINFO_PADCDC_DEFAULT << 29)       /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_PADCDC_VDCDC                          (_DEVINFO_MODULEINFO_PADCDC_VDCDC << 29)         /**< Shifted mode VDCDC for DEVINFO_MODULEINFO   */
#define DEVINFO_MODULEINFO_PADCDC_OTHER                          (_DEVINFO_MODULEINFO_PADCDC_OTHER << 29)         /**< Shifted mode OTHER for DEVINFO_MODULEINFO   */
#define DEVINFO_MODULEINFO_PHYLIMITED                            (0x1UL << 30)                                    /**<                                             */
#define _DEVINFO_MODULEINFO_PHYLIMITED_SHIFT                     30                                               /**< Shift value for DEVINFO_PHYLIMITED          */
#define _DEVINFO_MODULEINFO_PHYLIMITED_MASK                      0x40000000UL                                     /**< Bit mask for DEVINFO_PHYLIMITED             */
#define _DEVINFO_MODULEINFO_PHYLIMITED_DEFAULT                   0x00000001UL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_PHYLIMITED_LIMITED                   0x00000000UL                                     /**< Mode LIMITED for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_PHYLIMITED_UNLIMITED                 0x00000001UL                                     /**< Mode UNLIMITED for DEVINFO_MODULEINFO       */
#define DEVINFO_MODULEINFO_PHYLIMITED_DEFAULT                    (_DEVINFO_MODULEINFO_PHYLIMITED_DEFAULT << 30)   /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_PHYLIMITED_LIMITED                    (_DEVINFO_MODULEINFO_PHYLIMITED_LIMITED << 30)   /**< Shifted mode LIMITED for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_PHYLIMITED_UNLIMITED                  (_DEVINFO_MODULEINFO_PHYLIMITED_UNLIMITED << 30) /**< Shifted mode UNLIMITED for DEVINFO_MODULEINFO*/
#define DEVINFO_MODULEINFO_EXTVALID                              (0x1UL << 31)                                    /**<                                             */
#define _DEVINFO_MODULEINFO_EXTVALID_SHIFT                       31                                               /**< Shift value for DEVINFO_EXTVALID            */
#define _DEVINFO_MODULEINFO_EXTVALID_MASK                        0x80000000UL                                     /**< Bit mask for DEVINFO_EXTVALID               */
#define _DEVINFO_MODULEINFO_EXTVALID_DEFAULT                     0x00000001UL                                     /**< Mode DEFAULT for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_EXTVALID_EXTUSED                     0x00000000UL                                     /**< Mode EXTUSED for DEVINFO_MODULEINFO         */
#define _DEVINFO_MODULEINFO_EXTVALID_EXTUNUSED                   0x00000001UL                                     /**< Mode EXTUNUSED for DEVINFO_MODULEINFO       */
#define DEVINFO_MODULEINFO_EXTVALID_DEFAULT                      (_DEVINFO_MODULEINFO_EXTVALID_DEFAULT << 31)     /**< Shifted mode DEFAULT for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_EXTVALID_EXTUSED                      (_DEVINFO_MODULEINFO_EXTVALID_EXTUSED << 31)     /**< Shifted mode EXTUSED for DEVINFO_MODULEINFO */
#define DEVINFO_MODULEINFO_EXTVALID_EXTUNUSED                    (_DEVINFO_MODULEINFO_EXTVALID_EXTUNUSED << 31)   /**< Shifted mode EXTUNUSED for DEVINFO_MODULEINFO*/

/* Bit fields for DEVINFO MODXOCAL */
#define _DEVINFO_MODXOCAL_RESETVALUE                             0x007FFFFFUL                                    /**< Default value for DEVINFO_MODXOCAL          */
#define _DEVINFO_MODXOCAL_MASK                                   0x007FFFFFUL                                    /**< Mask for DEVINFO_MODXOCAL                   */
#define _DEVINFO_MODXOCAL_HFXOCTUNEXIANA_SHIFT                   0                                               /**< Shift value for DEVINFO_HFXOCTUNEXIANA      */
#define _DEVINFO_MODXOCAL_HFXOCTUNEXIANA_MASK                    0xFFUL                                          /**< Bit mask for DEVINFO_HFXOCTUNEXIANA         */
#define _DEVINFO_MODXOCAL_HFXOCTUNEXIANA_DEFAULT                 0x000000FFUL                                    /**< Mode DEFAULT for DEVINFO_MODXOCAL           */
#define DEVINFO_MODXOCAL_HFXOCTUNEXIANA_DEFAULT                  (_DEVINFO_MODXOCAL_HFXOCTUNEXIANA_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_MODXOCAL   */
#define _DEVINFO_MODXOCAL_HFXOCTUNEXOANA_SHIFT                   8                                               /**< Shift value for DEVINFO_HFXOCTUNEXOANA      */
#define _DEVINFO_MODXOCAL_HFXOCTUNEXOANA_MASK                    0xFF00UL                                        /**< Bit mask for DEVINFO_HFXOCTUNEXOANA         */
#define _DEVINFO_MODXOCAL_HFXOCTUNEXOANA_DEFAULT                 0x000000FFUL                                    /**< Mode DEFAULT for DEVINFO_MODXOCAL           */
#define DEVINFO_MODXOCAL_HFXOCTUNEXOANA_DEFAULT                  (_DEVINFO_MODXOCAL_HFXOCTUNEXOANA_DEFAULT << 8) /**< Shifted mode DEFAULT for DEVINFO_MODXOCAL   */
#define _DEVINFO_MODXOCAL_LFXOCAPTUNE_SHIFT                      16                                              /**< Shift value for DEVINFO_LFXOCAPTUNE         */
#define _DEVINFO_MODXOCAL_LFXOCAPTUNE_MASK                       0x7F0000UL                                      /**< Bit mask for DEVINFO_LFXOCAPTUNE            */
#define _DEVINFO_MODXOCAL_LFXOCAPTUNE_DEFAULT                    0x0000007FUL                                    /**< Mode DEFAULT for DEVINFO_MODXOCAL           */
#define DEVINFO_MODXOCAL_LFXOCAPTUNE_DEFAULT                     (_DEVINFO_MODXOCAL_LFXOCAPTUNE_DEFAULT << 16)   /**< Shifted mode DEFAULT for DEVINFO_MODXOCAL   */

/* Bit fields for DEVINFO HFXOCAL */
#define _DEVINFO_HFXOCAL_RESETVALUE                              0xFFFFFF00UL                                 /**< Default value for DEVINFO_HFXOCAL           */
#define _DEVINFO_HFXOCAL_MASK                                    0xFFFFFFFFUL                                 /**< Mask for DEVINFO_HFXOCAL                    */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_SHIFT                      0                                            /**< Shift value for DEVINFO_SHUNTBIASANA        */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_MASK                       0xFUL                                        /**< Bit mask for DEVINFO_SHUNTBIASANA           */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_DEFAULT                    0x00000000UL                                 /**< Mode DEFAULT for DEVINFO_HFXOCAL            */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I20UA                      0x00000000UL                                 /**< Mode I20UA for DEVINFO_HFXOCAL              */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I30UA                      0x00000001UL                                 /**< Mode I30UA for DEVINFO_HFXOCAL              */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I40UA                      0x00000002UL                                 /**< Mode I40UA for DEVINFO_HFXOCAL              */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I50UA                      0x00000003UL                                 /**< Mode I50UA for DEVINFO_HFXOCAL              */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I60UA                      0x00000004UL                                 /**< Mode I60UA for DEVINFO_HFXOCAL              */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I70UA                      0x00000005UL                                 /**< Mode I70UA for DEVINFO_HFXOCAL              */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I80UA                      0x00000006UL                                 /**< Mode I80UA for DEVINFO_HFXOCAL              */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I90UA                      0x00000007UL                                 /**< Mode I90UA for DEVINFO_HFXOCAL              */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I100UA                     0x00000008UL                                 /**< Mode I100UA for DEVINFO_HFXOCAL             */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I110UA                     0x00000009UL                                 /**< Mode I110UA for DEVINFO_HFXOCAL             */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I120UA                     0x0000000AUL                                 /**< Mode I120UA for DEVINFO_HFXOCAL             */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I130UA                     0x0000000BUL                                 /**< Mode I130UA for DEVINFO_HFXOCAL             */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I140UA                     0x0000000CUL                                 /**< Mode I140UA for DEVINFO_HFXOCAL             */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I150UA                     0x0000000DUL                                 /**< Mode I150UA for DEVINFO_HFXOCAL             */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I160UA                     0x0000000EUL                                 /**< Mode I160UA for DEVINFO_HFXOCAL             */
#define _DEVINFO_HFXOCAL_SHUNTBIASANA_I170UA                     0x0000000FUL                                 /**< Mode I170UA for DEVINFO_HFXOCAL             */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_DEFAULT                     (_DEVINFO_HFXOCAL_SHUNTBIASANA_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_HFXOCAL    */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I20UA                       (_DEVINFO_HFXOCAL_SHUNTBIASANA_I20UA << 0)   /**< Shifted mode I20UA for DEVINFO_HFXOCAL      */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I30UA                       (_DEVINFO_HFXOCAL_SHUNTBIASANA_I30UA << 0)   /**< Shifted mode I30UA for DEVINFO_HFXOCAL      */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I40UA                       (_DEVINFO_HFXOCAL_SHUNTBIASANA_I40UA << 0)   /**< Shifted mode I40UA for DEVINFO_HFXOCAL      */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I50UA                       (_DEVINFO_HFXOCAL_SHUNTBIASANA_I50UA << 0)   /**< Shifted mode I50UA for DEVINFO_HFXOCAL      */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I60UA                       (_DEVINFO_HFXOCAL_SHUNTBIASANA_I60UA << 0)   /**< Shifted mode I60UA for DEVINFO_HFXOCAL      */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I70UA                       (_DEVINFO_HFXOCAL_SHUNTBIASANA_I70UA << 0)   /**< Shifted mode I70UA for DEVINFO_HFXOCAL      */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I80UA                       (_DEVINFO_HFXOCAL_SHUNTBIASANA_I80UA << 0)   /**< Shifted mode I80UA for DEVINFO_HFXOCAL      */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I90UA                       (_DEVINFO_HFXOCAL_SHUNTBIASANA_I90UA << 0)   /**< Shifted mode I90UA for DEVINFO_HFXOCAL      */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I100UA                      (_DEVINFO_HFXOCAL_SHUNTBIASANA_I100UA << 0)  /**< Shifted mode I100UA for DEVINFO_HFXOCAL     */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I110UA                      (_DEVINFO_HFXOCAL_SHUNTBIASANA_I110UA << 0)  /**< Shifted mode I110UA for DEVINFO_HFXOCAL     */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I120UA                      (_DEVINFO_HFXOCAL_SHUNTBIASANA_I120UA << 0)  /**< Shifted mode I120UA for DEVINFO_HFXOCAL     */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I130UA                      (_DEVINFO_HFXOCAL_SHUNTBIASANA_I130UA << 0)  /**< Shifted mode I130UA for DEVINFO_HFXOCAL     */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I140UA                      (_DEVINFO_HFXOCAL_SHUNTBIASANA_I140UA << 0)  /**< Shifted mode I140UA for DEVINFO_HFXOCAL     */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I150UA                      (_DEVINFO_HFXOCAL_SHUNTBIASANA_I150UA << 0)  /**< Shifted mode I150UA for DEVINFO_HFXOCAL     */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I160UA                      (_DEVINFO_HFXOCAL_SHUNTBIASANA_I160UA << 0)  /**< Shifted mode I160UA for DEVINFO_HFXOCAL     */
#define DEVINFO_HFXOCAL_SHUNTBIASANA_I170UA                      (_DEVINFO_HFXOCAL_SHUNTBIASANA_I170UA << 0)  /**< Shifted mode I170UA for DEVINFO_HFXOCAL     */
#define _DEVINFO_HFXOCAL_VTRTRIMANA_SHIFT                        4                                            /**< Shift value for DEVINFO_VTRTRIMANA          */
#define _DEVINFO_HFXOCAL_VTRTRIMANA_MASK                         0xF0UL                                       /**< Bit mask for DEVINFO_VTRTRIMANA             */
#define _DEVINFO_HFXOCAL_VTRTRIMANA_DEFAULT                      0x00000000UL                                 /**< Mode DEFAULT for DEVINFO_HFXOCAL            */
#define DEVINFO_HFXOCAL_VTRTRIMANA_DEFAULT                       (_DEVINFO_HFXOCAL_VTRTRIMANA_DEFAULT << 4)   /**< Shifted mode DEFAULT for DEVINFO_HFXOCAL    */
#define _DEVINFO_HFXOCAL_RESERVED_SHIFT                          8                                            /**< Shift value for DEVINFO_RESERVED            */
#define _DEVINFO_HFXOCAL_RESERVED_MASK                           0xFFFFFF00UL                                 /**< Bit mask for DEVINFO_RESERVED               */
#define _DEVINFO_HFXOCAL_RESERVED_DEFAULT                        0x00FFFFFFUL                                 /**< Mode DEFAULT for DEVINFO_HFXOCAL            */
#define DEVINFO_HFXOCAL_RESERVED_DEFAULT                         (_DEVINFO_HFXOCAL_RESERVED_DEFAULT << 8)     /**< Shifted mode DEFAULT for DEVINFO_HFXOCAL    */

/* Bit fields for DEVINFO IADC0GAIN0 */
#define _DEVINFO_IADC0GAIN0_RESETVALUE                           0x00000000UL                                  /**< Default value for DEVINFO_IADC0GAIN0        */
#define _DEVINFO_IADC0GAIN0_MASK                                 0xFFFFFFFFUL                                  /**< Mask for DEVINFO_IADC0GAIN0                 */
#define _DEVINFO_IADC0GAIN0_GAINCANA1_SHIFT                      0                                             /**< Shift value for DEVINFO_GAINCANA1           */
#define _DEVINFO_IADC0GAIN0_GAINCANA1_MASK                       0xFFFFUL                                      /**< Bit mask for DEVINFO_GAINCANA1              */
#define _DEVINFO_IADC0GAIN0_GAINCANA1_DEFAULT                    0x00000000UL                                  /**< Mode DEFAULT for DEVINFO_IADC0GAIN0         */
#define DEVINFO_IADC0GAIN0_GAINCANA1_DEFAULT                     (_DEVINFO_IADC0GAIN0_GAINCANA1_DEFAULT << 0)  /**< Shifted mode DEFAULT for DEVINFO_IADC0GAIN0 */
#define _DEVINFO_IADC0GAIN0_GAINCANA2_SHIFT                      16                                            /**< Shift value for DEVINFO_GAINCANA2           */
#define _DEVINFO_IADC0GAIN0_GAINCANA2_MASK                       0xFFFF0000UL                                  /**< Bit mask for DEVINFO_GAINCANA2              */
#define _DEVINFO_IADC0GAIN0_GAINCANA2_DEFAULT                    0x00000000UL                                  /**< Mode DEFAULT for DEVINFO_IADC0GAIN0         */
#define DEVINFO_IADC0GAIN0_GAINCANA2_DEFAULT                     (_DEVINFO_IADC0GAIN0_GAINCANA2_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_IADC0GAIN0 */

/* Bit fields for DEVINFO IADC0GAIN1 */
#define _DEVINFO_IADC0GAIN1_RESETVALUE                           0x00000000UL                                  /**< Default value for DEVINFO_IADC0GAIN1        */
#define _DEVINFO_IADC0GAIN1_MASK                                 0xFFFFFFFFUL                                  /**< Mask for DEVINFO_IADC0GAIN1                 */
#define _DEVINFO_IADC0GAIN1_GAINCANA3_SHIFT                      0                                             /**< Shift value for DEVINFO_GAINCANA3           */
#define _DEVINFO_IADC0GAIN1_GAINCANA3_MASK                       0xFFFFUL                                      /**< Bit mask for DEVINFO_GAINCANA3              */
#define _DEVINFO_IADC0GAIN1_GAINCANA3_DEFAULT                    0x00000000UL                                  /**< Mode DEFAULT for DEVINFO_IADC0GAIN1         */
#define DEVINFO_IADC0GAIN1_GAINCANA3_DEFAULT                     (_DEVINFO_IADC0GAIN1_GAINCANA3_DEFAULT << 0)  /**< Shifted mode DEFAULT for DEVINFO_IADC0GAIN1 */
#define _DEVINFO_IADC0GAIN1_GAINCANA4_SHIFT                      16                                            /**< Shift value for DEVINFO_GAINCANA4           */
#define _DEVINFO_IADC0GAIN1_GAINCANA4_MASK                       0xFFFF0000UL                                  /**< Bit mask for DEVINFO_GAINCANA4              */
#define _DEVINFO_IADC0GAIN1_GAINCANA4_DEFAULT                    0x00000000UL                                  /**< Mode DEFAULT for DEVINFO_IADC0GAIN1         */
#define DEVINFO_IADC0GAIN1_GAINCANA4_DEFAULT                     (_DEVINFO_IADC0GAIN1_GAINCANA4_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_IADC0GAIN1 */

/* Bit fields for DEVINFO IADC0OFFSETCAL0 */
#define _DEVINFO_IADC0OFFSETCAL0_RESETVALUE                      0x00000000UL                                             /**< Default value for DEVINFO_IADC0OFFSETCAL0   */
#define _DEVINFO_IADC0OFFSETCAL0_MASK                            0xFFFFFFFFUL                                             /**< Mask for DEVINFO_IADC0OFFSETCAL0            */
#define _DEVINFO_IADC0OFFSETCAL0_OFFSETANABASE_SHIFT             0                                                        /**< Shift value for DEVINFO_OFFSETANABASE       */
#define _DEVINFO_IADC0OFFSETCAL0_OFFSETANABASE_MASK              0xFFFFUL                                                 /**< Bit mask for DEVINFO_OFFSETANABASE          */
#define _DEVINFO_IADC0OFFSETCAL0_OFFSETANABASE_DEFAULT           0x00000000UL                                             /**< Mode DEFAULT for DEVINFO_IADC0OFFSETCAL0    */
#define DEVINFO_IADC0OFFSETCAL0_OFFSETANABASE_DEFAULT            (_DEVINFO_IADC0OFFSETCAL0_OFFSETANABASE_DEFAULT << 0)    /**< Shifted mode DEFAULT for DEVINFO_IADC0OFFSETCAL0*/
#define _DEVINFO_IADC0OFFSETCAL0_OFFSETANA1HIACC_SHIFT           16                                                       /**< Shift value for DEVINFO_OFFSETANA1HIACC     */
#define _DEVINFO_IADC0OFFSETCAL0_OFFSETANA1HIACC_MASK            0xFFFF0000UL                                             /**< Bit mask for DEVINFO_OFFSETANA1HIACC        */
#define _DEVINFO_IADC0OFFSETCAL0_OFFSETANA1HIACC_DEFAULT         0x00000000UL                                             /**< Mode DEFAULT for DEVINFO_IADC0OFFSETCAL0    */
#define DEVINFO_IADC0OFFSETCAL0_OFFSETANA1HIACC_DEFAULT          (_DEVINFO_IADC0OFFSETCAL0_OFFSETANA1HIACC_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_IADC0OFFSETCAL0*/

/* Bit fields for DEVINFO IADC0NORMALOFFSETCAL0 */
#define _DEVINFO_IADC0NORMALOFFSETCAL0_RESETVALUE                0x00000000UL                                                  /**< Default value for DEVINFO_IADC0NORMALOFFSETCAL0*/
#define _DEVINFO_IADC0NORMALOFFSETCAL0_MASK                      0xFFFFFFFFUL                                                  /**< Mask for DEVINFO_IADC0NORMALOFFSETCAL0      */
#define _DEVINFO_IADC0NORMALOFFSETCAL0_OFFSETANA1NORM_SHIFT      0                                                             /**< Shift value for DEVINFO_OFFSETANA1NORM      */
#define _DEVINFO_IADC0NORMALOFFSETCAL0_OFFSETANA1NORM_MASK       0xFFFFUL                                                      /**< Bit mask for DEVINFO_OFFSETANA1NORM         */
#define _DEVINFO_IADC0NORMALOFFSETCAL0_OFFSETANA1NORM_DEFAULT    0x00000000UL                                                  /**< Mode DEFAULT for DEVINFO_IADC0NORMALOFFSETCAL0*/
#define DEVINFO_IADC0NORMALOFFSETCAL0_OFFSETANA1NORM_DEFAULT     (_DEVINFO_IADC0NORMALOFFSETCAL0_OFFSETANA1NORM_DEFAULT << 0)  /**< Shifted mode DEFAULT for DEVINFO_IADC0NORMALOFFSETCAL0*/
#define _DEVINFO_IADC0NORMALOFFSETCAL0_OFFSETANA2NORM_SHIFT      16                                                            /**< Shift value for DEVINFO_OFFSETANA2NORM      */
#define _DEVINFO_IADC0NORMALOFFSETCAL0_OFFSETANA2NORM_MASK       0xFFFF0000UL                                                  /**< Bit mask for DEVINFO_OFFSETANA2NORM         */
#define _DEVINFO_IADC0NORMALOFFSETCAL0_OFFSETANA2NORM_DEFAULT    0x00000000UL                                                  /**< Mode DEFAULT for DEVINFO_IADC0NORMALOFFSETCAL0*/
#define DEVINFO_IADC0NORMALOFFSETCAL0_OFFSETANA2NORM_DEFAULT     (_DEVINFO_IADC0NORMALOFFSETCAL0_OFFSETANA2NORM_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_IADC0NORMALOFFSETCAL0*/

/* Bit fields for DEVINFO IADC0NORMALOFFSETCAL1 */
#define _DEVINFO_IADC0NORMALOFFSETCAL1_RESETVALUE                0x00000000UL                                                 /**< Default value for DEVINFO_IADC0NORMALOFFSETCAL1*/
#define _DEVINFO_IADC0NORMALOFFSETCAL1_MASK                      0x0000FFFFUL                                                 /**< Mask for DEVINFO_IADC0NORMALOFFSETCAL1      */
#define _DEVINFO_IADC0NORMALOFFSETCAL1_OFFSETANA3NORM_SHIFT      0                                                            /**< Shift value for DEVINFO_OFFSETANA3NORM      */
#define _DEVINFO_IADC0NORMALOFFSETCAL1_OFFSETANA3NORM_MASK       0xFFFFUL                                                     /**< Bit mask for DEVINFO_OFFSETANA3NORM         */
#define _DEVINFO_IADC0NORMALOFFSETCAL1_OFFSETANA3NORM_DEFAULT    0x00000000UL                                                 /**< Mode DEFAULT for DEVINFO_IADC0NORMALOFFSETCAL1*/
#define DEVINFO_IADC0NORMALOFFSETCAL1_OFFSETANA3NORM_DEFAULT     (_DEVINFO_IADC0NORMALOFFSETCAL1_OFFSETANA3NORM_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_IADC0NORMALOFFSETCAL1*/

/* Bit fields for DEVINFO IADC0HISPDOFFSETCAL0 */
#define _DEVINFO_IADC0HISPDOFFSETCAL0_RESETVALUE                 0x00000000UL                                                  /**< Default value for DEVINFO_IADC0HISPDOFFSETCAL0*/
#define _DEVINFO_IADC0HISPDOFFSETCAL0_MASK                       0xFFFFFFFFUL                                                  /**< Mask for DEVINFO_IADC0HISPDOFFSETCAL0       */
#define _DEVINFO_IADC0HISPDOFFSETCAL0_OFFSETANA1HISPD_SHIFT      0                                                             /**< Shift value for DEVINFO_OFFSETANA1HISPD     */
#define _DEVINFO_IADC0HISPDOFFSETCAL0_OFFSETANA1HISPD_MASK       0xFFFFUL                                                      /**< Bit mask for DEVINFO_OFFSETANA1HISPD        */
#define _DEVINFO_IADC0HISPDOFFSETCAL0_OFFSETANA1HISPD_DEFAULT    0x00000000UL                                                  /**< Mode DEFAULT for DEVINFO_IADC0HISPDOFFSETCAL0*/
#define DEVINFO_IADC0HISPDOFFSETCAL0_OFFSETANA1HISPD_DEFAULT     (_DEVINFO_IADC0HISPDOFFSETCAL0_OFFSETANA1HISPD_DEFAULT << 0)  /**< Shifted mode DEFAULT for DEVINFO_IADC0HISPDOFFSETCAL0*/
#define _DEVINFO_IADC0HISPDOFFSETCAL0_OFFSETANA2HISPD_SHIFT      16                                                            /**< Shift value for DEVINFO_OFFSETANA2HISPD     */
#define _DEVINFO_IADC0HISPDOFFSETCAL0_OFFSETANA2HISPD_MASK       0xFFFF0000UL                                                  /**< Bit mask for DEVINFO_OFFSETANA2HISPD        */
#define _DEVINFO_IADC0HISPDOFFSETCAL0_OFFSETANA2HISPD_DEFAULT    0x00000000UL                                                  /**< Mode DEFAULT for DEVINFO_IADC0HISPDOFFSETCAL0*/
#define DEVINFO_IADC0HISPDOFFSETCAL0_OFFSETANA2HISPD_DEFAULT     (_DEVINFO_IADC0HISPDOFFSETCAL0_OFFSETANA2HISPD_DEFAULT << 16) /**< Shifted mode DEFAULT for DEVINFO_IADC0HISPDOFFSETCAL0*/

/* Bit fields for DEVINFO IADC0HISPDOFFSETCAL1 */
#define _DEVINFO_IADC0HISPDOFFSETCAL1_RESETVALUE                 0x00000000UL                                                 /**< Default value for DEVINFO_IADC0HISPDOFFSETCAL1*/
#define _DEVINFO_IADC0HISPDOFFSETCAL1_MASK                       0x0000FFFFUL                                                 /**< Mask for DEVINFO_IADC0HISPDOFFSETCAL1       */
#define _DEVINFO_IADC0HISPDOFFSETCAL1_OFFSETANA3HISPD_SHIFT      0                                                            /**< Shift value for DEVINFO_OFFSETANA3HISPD     */
#define _DEVINFO_IADC0HISPDOFFSETCAL1_OFFSETANA3HISPD_MASK       0xFFFFUL                                                     /**< Bit mask for DEVINFO_OFFSETANA3HISPD        */
#define _DEVINFO_IADC0HISPDOFFSETCAL1_OFFSETANA3HISPD_DEFAULT    0x00000000UL                                                 /**< Mode DEFAULT for DEVINFO_IADC0HISPDOFFSETCAL1*/
#define DEVINFO_IADC0HISPDOFFSETCAL1_OFFSETANA3HISPD_DEFAULT     (_DEVINFO_IADC0HISPDOFFSETCAL1_OFFSETANA3HISPD_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_IADC0HISPDOFFSETCAL1*/

/* Bit fields for DEVINFO LEGACY */
#define _DEVINFO_LEGACY_RESETVALUE                               0x00800000UL                                    /**< Default value for DEVINFO_LEGACY            */
#define _DEVINFO_LEGACY_MASK                                     0x00FF0000UL                                    /**< Mask for DEVINFO_LEGACY                     */
#define _DEVINFO_LEGACY_DEVICEFAMILY_SHIFT                       16                                              /**< Shift value for DEVINFO_DEVICEFAMILY        */
#define _DEVINFO_LEGACY_DEVICEFAMILY_MASK                        0xFF0000UL                                      /**< Bit mask for DEVINFO_DEVICEFAMILY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_DEFAULT                     0x00000080UL                                    /**< Mode DEFAULT for DEVINFO_LEGACY             */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG1P                   0x00000010UL                                    /**< Mode EFR32MG1P for DEVINFO_LEGACY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG1B                   0x00000011UL                                    /**< Mode EFR32MG1B for DEVINFO_LEGACY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG1V                   0x00000012UL                                    /**< Mode EFR32MG1V for DEVINFO_LEGACY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG1P                   0x00000013UL                                    /**< Mode EFR32BG1P for DEVINFO_LEGACY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG1B                   0x00000014UL                                    /**< Mode EFR32BG1B for DEVINFO_LEGACY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG1V                   0x00000015UL                                    /**< Mode EFR32BG1V for DEVINFO_LEGACY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG1P                   0x00000019UL                                    /**< Mode EFR32FG1P for DEVINFO_LEGACY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG1B                   0x0000001AUL                                    /**< Mode EFR32FG1B for DEVINFO_LEGACY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG1V                   0x0000001BUL                                    /**< Mode EFR32FG1V for DEVINFO_LEGACY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG12P                  0x0000001CUL                                    /**< Mode EFR32MG12P for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG12B                  0x0000001DUL                                    /**< Mode EFR32MG12B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG12V                  0x0000001EUL                                    /**< Mode EFR32MG12V for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG12P                  0x0000001FUL                                    /**< Mode EFR32BG12P for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG12B                  0x00000020UL                                    /**< Mode EFR32BG12B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG12V                  0x00000021UL                                    /**< Mode EFR32BG12V for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG12P                  0x00000025UL                                    /**< Mode EFR32FG12P for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG12B                  0x00000026UL                                    /**< Mode EFR32FG12B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG12V                  0x00000027UL                                    /**< Mode EFR32FG12V for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG13P                  0x00000028UL                                    /**< Mode EFR32MG13P for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG13B                  0x00000029UL                                    /**< Mode EFR32MG13B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG13V                  0x0000002AUL                                    /**< Mode EFR32MG13V for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG13P                  0x0000002BUL                                    /**< Mode EFR32BG13P for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG13B                  0x0000002CUL                                    /**< Mode EFR32BG13B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG13V                  0x0000002DUL                                    /**< Mode EFR32BG13V for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG13P                  0x00000031UL                                    /**< Mode EFR32FG13P for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG13B                  0x00000032UL                                    /**< Mode EFR32FG13B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG13V                  0x00000033UL                                    /**< Mode EFR32FG13V for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG14P                  0x00000034UL                                    /**< Mode EFR32MG14P for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG14B                  0x00000035UL                                    /**< Mode EFR32MG14B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG14V                  0x00000036UL                                    /**< Mode EFR32MG14V for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG14P                  0x00000037UL                                    /**< Mode EFR32BG14P for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG14B                  0x00000038UL                                    /**< Mode EFR32BG14B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG14V                  0x00000039UL                                    /**< Mode EFR32BG14V for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG14P                  0x0000003DUL                                    /**< Mode EFR32FG14P for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG14B                  0x0000003EUL                                    /**< Mode EFR32FG14B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG14V                  0x0000003FUL                                    /**< Mode EFR32FG14V for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32G                      0x00000047UL                                    /**< Mode EFM32G for DEVINFO_LEGACY              */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32GG                     0x00000048UL                                    /**< Mode EFM32GG for DEVINFO_LEGACY             */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32TG                     0x00000049UL                                    /**< Mode EFM32TG for DEVINFO_LEGACY             */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32LG                     0x0000004AUL                                    /**< Mode EFM32LG for DEVINFO_LEGACY             */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32WG                     0x0000004BUL                                    /**< Mode EFM32WG for DEVINFO_LEGACY             */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32ZG                     0x0000004CUL                                    /**< Mode EFM32ZG for DEVINFO_LEGACY             */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32HG                     0x0000004DUL                                    /**< Mode EFM32HG for DEVINFO_LEGACY             */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32PG1B                   0x00000051UL                                    /**< Mode EFM32PG1B for DEVINFO_LEGACY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32JG1B                   0x00000053UL                                    /**< Mode EFM32JG1B for DEVINFO_LEGACY           */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32PG12B                  0x00000055UL                                    /**< Mode EFM32PG12B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32JG12B                  0x00000057UL                                    /**< Mode EFM32JG12B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32PG13B                  0x00000059UL                                    /**< Mode EFM32PG13B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32JG13B                  0x0000005BUL                                    /**< Mode EFM32JG13B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32GG11B                  0x00000064UL                                    /**< Mode EFM32GG11B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EFM32TG11B                  0x00000067UL                                    /**< Mode EFM32TG11B for DEVINFO_LEGACY          */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EZR32LG                     0x00000078UL                                    /**< Mode EZR32LG for DEVINFO_LEGACY             */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EZR32WG                     0x00000079UL                                    /**< Mode EZR32WG for DEVINFO_LEGACY             */
#define _DEVINFO_LEGACY_DEVICEFAMILY_EZR32HG                     0x0000007AUL                                    /**< Mode EZR32HG for DEVINFO_LEGACY             */
#define _DEVINFO_LEGACY_DEVICEFAMILY_SERIES2V0                   0x00000080UL                                    /**< Mode SERIES2V0 for DEVINFO_LEGACY           */
#define DEVINFO_LEGACY_DEVICEFAMILY_DEFAULT                      (_DEVINFO_LEGACY_DEVICEFAMILY_DEFAULT << 16)    /**< Shifted mode DEFAULT for DEVINFO_LEGACY     */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG1P                    (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG1P << 16)  /**< Shifted mode EFR32MG1P for DEVINFO_LEGACY   */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG1B                    (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG1B << 16)  /**< Shifted mode EFR32MG1B for DEVINFO_LEGACY   */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG1V                    (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG1V << 16)  /**< Shifted mode EFR32MG1V for DEVINFO_LEGACY   */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG1P                    (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG1P << 16)  /**< Shifted mode EFR32BG1P for DEVINFO_LEGACY   */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG1B                    (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG1B << 16)  /**< Shifted mode EFR32BG1B for DEVINFO_LEGACY   */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG1V                    (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG1V << 16)  /**< Shifted mode EFR32BG1V for DEVINFO_LEGACY   */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG1P                    (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG1P << 16)  /**< Shifted mode EFR32FG1P for DEVINFO_LEGACY   */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG1B                    (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG1B << 16)  /**< Shifted mode EFR32FG1B for DEVINFO_LEGACY   */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG1V                    (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG1V << 16)  /**< Shifted mode EFR32FG1V for DEVINFO_LEGACY   */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG12P                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG12P << 16) /**< Shifted mode EFR32MG12P for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG12B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG12B << 16) /**< Shifted mode EFR32MG12B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG12V                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG12V << 16) /**< Shifted mode EFR32MG12V for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG12P                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG12P << 16) /**< Shifted mode EFR32BG12P for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG12B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG12B << 16) /**< Shifted mode EFR32BG12B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG12V                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG12V << 16) /**< Shifted mode EFR32BG12V for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG12P                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG12P << 16) /**< Shifted mode EFR32FG12P for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG12B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG12B << 16) /**< Shifted mode EFR32FG12B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG12V                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG12V << 16) /**< Shifted mode EFR32FG12V for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG13P                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG13P << 16) /**< Shifted mode EFR32MG13P for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG13B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG13B << 16) /**< Shifted mode EFR32MG13B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG13V                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG13V << 16) /**< Shifted mode EFR32MG13V for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG13P                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG13P << 16) /**< Shifted mode EFR32BG13P for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG13B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG13B << 16) /**< Shifted mode EFR32BG13B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG13V                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG13V << 16) /**< Shifted mode EFR32BG13V for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG13P                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG13P << 16) /**< Shifted mode EFR32FG13P for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG13B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG13B << 16) /**< Shifted mode EFR32FG13B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG13V                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG13V << 16) /**< Shifted mode EFR32FG13V for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG14P                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG14P << 16) /**< Shifted mode EFR32MG14P for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG14B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG14B << 16) /**< Shifted mode EFR32MG14B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG14V                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32MG14V << 16) /**< Shifted mode EFR32MG14V for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG14P                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG14P << 16) /**< Shifted mode EFR32BG14P for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG14B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG14B << 16) /**< Shifted mode EFR32BG14B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG14V                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32BG14V << 16) /**< Shifted mode EFR32BG14V for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG14P                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG14P << 16) /**< Shifted mode EFR32FG14P for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG14B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG14B << 16) /**< Shifted mode EFR32FG14B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG14V                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFR32FG14V << 16) /**< Shifted mode EFR32FG14V for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32G                       (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32G << 16)     /**< Shifted mode EFM32G for DEVINFO_LEGACY      */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32GG                      (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32GG << 16)    /**< Shifted mode EFM32GG for DEVINFO_LEGACY     */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32TG                      (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32TG << 16)    /**< Shifted mode EFM32TG for DEVINFO_LEGACY     */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32LG                      (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32LG << 16)    /**< Shifted mode EFM32LG for DEVINFO_LEGACY     */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32WG                      (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32WG << 16)    /**< Shifted mode EFM32WG for DEVINFO_LEGACY     */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32ZG                      (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32ZG << 16)    /**< Shifted mode EFM32ZG for DEVINFO_LEGACY     */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32HG                      (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32HG << 16)    /**< Shifted mode EFM32HG for DEVINFO_LEGACY     */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32PG1B                    (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32PG1B << 16)  /**< Shifted mode EFM32PG1B for DEVINFO_LEGACY   */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32JG1B                    (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32JG1B << 16)  /**< Shifted mode EFM32JG1B for DEVINFO_LEGACY   */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32PG12B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32PG12B << 16) /**< Shifted mode EFM32PG12B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32JG12B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32JG12B << 16) /**< Shifted mode EFM32JG12B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32PG13B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32PG13B << 16) /**< Shifted mode EFM32PG13B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32JG13B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32JG13B << 16) /**< Shifted mode EFM32JG13B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32GG11B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32GG11B << 16) /**< Shifted mode EFM32GG11B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EFM32TG11B                   (_DEVINFO_LEGACY_DEVICEFAMILY_EFM32TG11B << 16) /**< Shifted mode EFM32TG11B for DEVINFO_LEGACY  */
#define DEVINFO_LEGACY_DEVICEFAMILY_EZR32LG                      (_DEVINFO_LEGACY_DEVICEFAMILY_EZR32LG << 16)    /**< Shifted mode EZR32LG for DEVINFO_LEGACY     */
#define DEVINFO_LEGACY_DEVICEFAMILY_EZR32WG                      (_DEVINFO_LEGACY_DEVICEFAMILY_EZR32WG << 16)    /**< Shifted mode EZR32WG for DEVINFO_LEGACY     */
#define DEVINFO_LEGACY_DEVICEFAMILY_EZR32HG                      (_DEVINFO_LEGACY_DEVICEFAMILY_EZR32HG << 16)    /**< Shifted mode EZR32HG for DEVINFO_LEGACY     */
#define DEVINFO_LEGACY_DEVICEFAMILY_SERIES2V0                    (_DEVINFO_LEGACY_DEVICEFAMILY_SERIES2V0 << 16)  /**< Shifted mode SERIES2V0 for DEVINFO_LEGACY   */

/* Bit fields for DEVINFO RTHERM */
#define _DEVINFO_RTHERM_RESETVALUE                               0x00000000UL                          /**< Default value for DEVINFO_RTHERM            */
#define _DEVINFO_RTHERM_MASK                                     0x0000FFFFUL                          /**< Mask for DEVINFO_RTHERM                     */
#define _DEVINFO_RTHERM_RTHERM_SHIFT                             0                                     /**< Shift value for DEVINFO_RTHERM              */
#define _DEVINFO_RTHERM_RTHERM_MASK                              0xFFFFUL                              /**< Bit mask for DEVINFO_RTHERM                 */
#define _DEVINFO_RTHERM_RTHERM_DEFAULT                           0x00000000UL                          /**< Mode DEFAULT for DEVINFO_RTHERM             */
#define DEVINFO_RTHERM_RTHERM_DEFAULT                            (_DEVINFO_RTHERM_RTHERM_DEFAULT << 0) /**< Shifted mode DEFAULT for DEVINFO_RTHERM     */

/** @} End of group EFR32ZG23_DEVINFO_BitFields */
/** @} End of group EFR32ZG23_DEVINFO */
/** @} End of group Parts */

#endif /* EFR32ZG23_DEVINFO_H */
#ifdef __cplusplus
}
#endif

