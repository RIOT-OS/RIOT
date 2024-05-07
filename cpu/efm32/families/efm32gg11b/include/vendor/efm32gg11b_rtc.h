/***************************************************************************//**
 * @file
 * @brief EFM32GG11B_RTC register and bit field definitions
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
 * @defgroup EFM32GG11B_RTC RTC
 * @{
 * @brief EFM32GG11B_RTC Register Declaration
 ******************************************************************************/
/** RTC Register Declaration */
typedef struct {
  __IOM uint32_t   CTRL;          /**< Control Register  */
  __IOM uint32_t   CNT;           /**< Counter Value Register  */
  __IM uint32_t    IF;            /**< Interrupt Flag Register  */
  __IOM uint32_t   IFS;           /**< Interrupt Flag Set Register  */
  __IOM uint32_t   IFC;           /**< Interrupt Flag Clear Register  */
  __IOM uint32_t   IEN;           /**< Interrupt Enable Register  */

  uint32_t         RESERVED0[2U]; /**< Reserved registers */
  RTC_COMP_TypeDef COMP[6U];      /**< Compare channels */
} RTC_TypeDef;                    /** @} */

/***************************************************************************//**
 * @addtogroup EFM32GG11B_RTC
 * @{
 * @defgroup EFM32GG11B_RTC_BitFields  RTC Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for RTC CTRL */
#define _RTC_CTRL_RESETVALUE           0x00000000UL                      /**< Default value for RTC_CTRL */
#define _RTC_CTRL_MASK                 0x00000007UL                      /**< Mask for RTC_CTRL */
#define RTC_CTRL_EN                    (0x1UL << 0)                      /**< RTC Enable */
#define _RTC_CTRL_EN_SHIFT             0                                 /**< Shift value for RTC_EN */
#define _RTC_CTRL_EN_MASK              0x1UL                             /**< Bit mask for RTC_EN */
#define _RTC_CTRL_EN_DEFAULT           0x00000000UL                      /**< Mode DEFAULT for RTC_CTRL */
#define RTC_CTRL_EN_DEFAULT            (_RTC_CTRL_EN_DEFAULT << 0)       /**< Shifted mode DEFAULT for RTC_CTRL */
#define RTC_CTRL_DEBUGRUN              (0x1UL << 1)                      /**< Debug Mode Run Enable */
#define _RTC_CTRL_DEBUGRUN_SHIFT       1                                 /**< Shift value for RTC_DEBUGRUN */
#define _RTC_CTRL_DEBUGRUN_MASK        0x2UL                             /**< Bit mask for RTC_DEBUGRUN */
#define _RTC_CTRL_DEBUGRUN_DEFAULT     0x00000000UL                      /**< Mode DEFAULT for RTC_CTRL */
#define RTC_CTRL_DEBUGRUN_DEFAULT      (_RTC_CTRL_DEBUGRUN_DEFAULT << 1) /**< Shifted mode DEFAULT for RTC_CTRL */
#define RTC_CTRL_COMP0TOP              (0x1UL << 2)                      /**< Compare Channel 0 is Top Value */
#define _RTC_CTRL_COMP0TOP_SHIFT       2                                 /**< Shift value for RTC_COMP0TOP */
#define _RTC_CTRL_COMP0TOP_MASK        0x4UL                             /**< Bit mask for RTC_COMP0TOP */
#define _RTC_CTRL_COMP0TOP_DEFAULT     0x00000000UL                      /**< Mode DEFAULT for RTC_CTRL */
#define _RTC_CTRL_COMP0TOP_DISABLE     0x00000000UL                      /**< Mode DISABLE for RTC_CTRL */
#define _RTC_CTRL_COMP0TOP_ENABLE      0x00000001UL                      /**< Mode ENABLE for RTC_CTRL */
#define RTC_CTRL_COMP0TOP_DEFAULT      (_RTC_CTRL_COMP0TOP_DEFAULT << 2) /**< Shifted mode DEFAULT for RTC_CTRL */
#define RTC_CTRL_COMP0TOP_DISABLE      (_RTC_CTRL_COMP0TOP_DISABLE << 2) /**< Shifted mode DISABLE for RTC_CTRL */
#define RTC_CTRL_COMP0TOP_ENABLE       (_RTC_CTRL_COMP0TOP_ENABLE << 2)  /**< Shifted mode ENABLE for RTC_CTRL */

/* Bit fields for RTC CNT */
#define _RTC_CNT_RESETVALUE            0x00000000UL                /**< Default value for RTC_CNT */
#define _RTC_CNT_MASK                  0x00FFFFFFUL                /**< Mask for RTC_CNT */
#define _RTC_CNT_CNT_SHIFT             0                           /**< Shift value for RTC_CNT */
#define _RTC_CNT_CNT_MASK              0xFFFFFFUL                  /**< Bit mask for RTC_CNT */
#define _RTC_CNT_CNT_DEFAULT           0x00000000UL                /**< Mode DEFAULT for RTC_CNT */
#define RTC_CNT_CNT_DEFAULT            (_RTC_CNT_CNT_DEFAULT << 0) /**< Shifted mode DEFAULT for RTC_CNT */

/* Bit fields for RTC IF */
#define _RTC_IF_RESETVALUE             0x00000000UL                /**< Default value for RTC_IF */
#define _RTC_IF_MASK                   0x0000007FUL                /**< Mask for RTC_IF */
#define RTC_IF_OF                      (0x1UL << 0)                /**< Overflow Interrupt Flag */
#define _RTC_IF_OF_SHIFT               0                           /**< Shift value for RTC_OF */
#define _RTC_IF_OF_MASK                0x1UL                       /**< Bit mask for RTC_OF */
#define _RTC_IF_OF_DEFAULT             0x00000000UL                /**< Mode DEFAULT for RTC_IF */
#define RTC_IF_OF_DEFAULT              (_RTC_IF_OF_DEFAULT << 0)   /**< Shifted mode DEFAULT for RTC_IF */
#define _RTC_IF_COMP_SHIFT             1                           /**< Shift value for RTC_COMP */
#define _RTC_IF_COMP_MASK              0x7EUL                      /**< Bit mask for RTC_COMP */
#define _RTC_IF_COMP_DEFAULT           0x00000000UL                /**< Mode DEFAULT for RTC_IF */
#define RTC_IF_COMP_DEFAULT            (_RTC_IF_COMP_DEFAULT << 1) /**< Shifted mode DEFAULT for RTC_IF */

/* Bit fields for RTC IFS */
#define _RTC_IFS_RESETVALUE            0x00000000UL                 /**< Default value for RTC_IFS */
#define _RTC_IFS_MASK                  0x0000007FUL                 /**< Mask for RTC_IFS */
#define RTC_IFS_OF                     (0x1UL << 0)                 /**< Set OF Interrupt Flag */
#define _RTC_IFS_OF_SHIFT              0                            /**< Shift value for RTC_OF */
#define _RTC_IFS_OF_MASK               0x1UL                        /**< Bit mask for RTC_OF */
#define _RTC_IFS_OF_DEFAULT            0x00000000UL                 /**< Mode DEFAULT for RTC_IFS */
#define RTC_IFS_OF_DEFAULT             (_RTC_IFS_OF_DEFAULT << 0)   /**< Shifted mode DEFAULT for RTC_IFS */
#define _RTC_IFS_COMP_SHIFT            1                            /**< Shift value for RTC_COMP */
#define _RTC_IFS_COMP_MASK             0x7EUL                       /**< Bit mask for RTC_COMP */
#define _RTC_IFS_COMP_DEFAULT          0x00000000UL                 /**< Mode DEFAULT for RTC_IFS */
#define RTC_IFS_COMP_DEFAULT           (_RTC_IFS_COMP_DEFAULT << 1) /**< Shifted mode DEFAULT for RTC_IFS */

/* Bit fields for RTC IFC */
#define _RTC_IFC_RESETVALUE            0x00000000UL                 /**< Default value for RTC_IFC */
#define _RTC_IFC_MASK                  0x0000007FUL                 /**< Mask for RTC_IFC */
#define RTC_IFC_OF                     (0x1UL << 0)                 /**< Clear OF Interrupt Flag */
#define _RTC_IFC_OF_SHIFT              0                            /**< Shift value for RTC_OF */
#define _RTC_IFC_OF_MASK               0x1UL                        /**< Bit mask for RTC_OF */
#define _RTC_IFC_OF_DEFAULT            0x00000000UL                 /**< Mode DEFAULT for RTC_IFC */
#define RTC_IFC_OF_DEFAULT             (_RTC_IFC_OF_DEFAULT << 0)   /**< Shifted mode DEFAULT for RTC_IFC */
#define _RTC_IFC_COMP_SHIFT            1                            /**< Shift value for RTC_COMP */
#define _RTC_IFC_COMP_MASK             0x7EUL                       /**< Bit mask for RTC_COMP */
#define _RTC_IFC_COMP_DEFAULT          0x00000000UL                 /**< Mode DEFAULT for RTC_IFC */
#define RTC_IFC_COMP_DEFAULT           (_RTC_IFC_COMP_DEFAULT << 1) /**< Shifted mode DEFAULT for RTC_IFC */

/* Bit fields for RTC IEN */
#define _RTC_IEN_RESETVALUE            0x00000000UL                 /**< Default value for RTC_IEN */
#define _RTC_IEN_MASK                  0x0000007FUL                 /**< Mask for RTC_IEN */
#define RTC_IEN_OF                     (0x1UL << 0)                 /**< OF Interrupt Enable */
#define _RTC_IEN_OF_SHIFT              0                            /**< Shift value for RTC_OF */
#define _RTC_IEN_OF_MASK               0x1UL                        /**< Bit mask for RTC_OF */
#define _RTC_IEN_OF_DEFAULT            0x00000000UL                 /**< Mode DEFAULT for RTC_IEN */
#define RTC_IEN_OF_DEFAULT             (_RTC_IEN_OF_DEFAULT << 0)   /**< Shifted mode DEFAULT for RTC_IEN */
#define _RTC_IEN_COMP_SHIFT            1                            /**< Shift value for RTC_COMP */
#define _RTC_IEN_COMP_MASK             0x7EUL                       /**< Bit mask for RTC_COMP */
#define _RTC_IEN_COMP_DEFAULT          0x00000000UL                 /**< Mode DEFAULT for RTC_IEN */
#define RTC_IEN_COMP_DEFAULT           (_RTC_IEN_COMP_DEFAULT << 1) /**< Shifted mode DEFAULT for RTC_IEN */

/* Bit fields for RTC COMP_COMP */
#define _RTC_COMP_COMP_RESETVALUE      0x00000000UL                       /**< Default value for RTC_COMP_COMP */
#define _RTC_COMP_COMP_MASK            0x00FFFFFFUL                       /**< Mask for RTC_COMP_COMP */
#define _RTC_COMP_COMP_COMP_SHIFT      0                                  /**< Shift value for RTC_COMP */
#define _RTC_COMP_COMP_COMP_MASK       0xFFFFFFUL                         /**< Bit mask for RTC_COMP */
#define _RTC_COMP_COMP_COMP_DEFAULT    0x00000000UL                       /**< Mode DEFAULT for RTC_COMP_COMP */
#define RTC_COMP_COMP_COMP_DEFAULT     (_RTC_COMP_COMP_COMP_DEFAULT << 0) /**< Shifted mode DEFAULT for RTC_COMP_COMP */

/** @} */
/** @} End of group EFM32GG11B_RTC */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

