/***************************************************************************//**
 * @file
 * @brief EFM32GG_AES register and bit field definitions
 * @version 5.8.3
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @defgroup EFM32GG_AES
 * @{
 * @brief EFM32GG_AES Register Declaration
 ******************************************************************************/
typedef struct {
  __IOM uint32_t CTRL;          /**< Control Register  */
  __IOM uint32_t CMD;           /**< Command Register  */
  __IM uint32_t  STATUS;        /**< Status Register  */
  __IOM uint32_t IEN;           /**< Interrupt Enable Register  */
  __IM uint32_t  IF;            /**< Interrupt Flag Register  */
  __IOM uint32_t IFS;           /**< Interrupt Flag Set Register  */
  __IOM uint32_t IFC;           /**< Interrupt Flag Clear Register  */
  __IOM uint32_t DATA;          /**< DATA Register  */
  __IOM uint32_t XORDATA;       /**< XORDATA Register  */
  uint32_t       RESERVED0[3U]; /**< Reserved for future use **/
  __IOM uint32_t KEYLA;         /**< KEY Low Register  */
  __IOM uint32_t KEYLB;         /**< KEY Low Register  */
  __IOM uint32_t KEYLC;         /**< KEY Low Register  */
  __IOM uint32_t KEYLD;         /**< KEY Low Register  */
  __IOM uint32_t KEYHA;         /**< KEY High Register  */
  __IOM uint32_t KEYHB;         /**< KEY High Register  */
  __IOM uint32_t KEYHC;         /**< KEY High Register  */
  __IOM uint32_t KEYHD;         /**< KEY High Register  */
} AES_TypeDef;                  /**< AES Register Declaration *//** @} */

/***************************************************************************//**
 * @defgroup EFM32GG_AES_BitFields
 * @{
 ******************************************************************************/

/* Bit fields for AES CTRL */
#define _AES_CTRL_RESETVALUE            0x00000000UL                       /**< Default value for AES_CTRL */
#define _AES_CTRL_MASK                  0x00000077UL                       /**< Mask for AES_CTRL */
#define AES_CTRL_DECRYPT                (0x1UL << 0)                       /**< Decryption/Encryption Mode */
#define _AES_CTRL_DECRYPT_SHIFT         0                                  /**< Shift value for AES_DECRYPT */
#define _AES_CTRL_DECRYPT_MASK          0x1UL                              /**< Bit mask for AES_DECRYPT */
#define _AES_CTRL_DECRYPT_DEFAULT       0x00000000UL                       /**< Mode DEFAULT for AES_CTRL */
#define AES_CTRL_DECRYPT_DEFAULT        (_AES_CTRL_DECRYPT_DEFAULT << 0)   /**< Shifted mode DEFAULT for AES_CTRL */
#define AES_CTRL_AES256                 (0x1UL << 1)                       /**< AES-256 Mode */
#define _AES_CTRL_AES256_SHIFT          1                                  /**< Shift value for AES_AES256 */
#define _AES_CTRL_AES256_MASK           0x2UL                              /**< Bit mask for AES_AES256 */
#define _AES_CTRL_AES256_DEFAULT        0x00000000UL                       /**< Mode DEFAULT for AES_CTRL */
#define AES_CTRL_AES256_DEFAULT         (_AES_CTRL_AES256_DEFAULT << 1)    /**< Shifted mode DEFAULT for AES_CTRL */
#define AES_CTRL_KEYBUFEN               (0x1UL << 2)                       /**< Key Buffer Enable */
#define _AES_CTRL_KEYBUFEN_SHIFT        2                                  /**< Shift value for AES_KEYBUFEN */
#define _AES_CTRL_KEYBUFEN_MASK         0x4UL                              /**< Bit mask for AES_KEYBUFEN */
#define _AES_CTRL_KEYBUFEN_DEFAULT      0x00000000UL                       /**< Mode DEFAULT for AES_CTRL */
#define AES_CTRL_KEYBUFEN_DEFAULT       (_AES_CTRL_KEYBUFEN_DEFAULT << 2)  /**< Shifted mode DEFAULT for AES_CTRL */
#define AES_CTRL_DATASTART              (0x1UL << 4)                       /**< AES_DATA Write Start */
#define _AES_CTRL_DATASTART_SHIFT       4                                  /**< Shift value for AES_DATASTART */
#define _AES_CTRL_DATASTART_MASK        0x10UL                             /**< Bit mask for AES_DATASTART */
#define _AES_CTRL_DATASTART_DEFAULT     0x00000000UL                       /**< Mode DEFAULT for AES_CTRL */
#define AES_CTRL_DATASTART_DEFAULT      (_AES_CTRL_DATASTART_DEFAULT << 4) /**< Shifted mode DEFAULT for AES_CTRL */
#define AES_CTRL_XORSTART               (0x1UL << 5)                       /**< AES_XORDATA Write Start */
#define _AES_CTRL_XORSTART_SHIFT        5                                  /**< Shift value for AES_XORSTART */
#define _AES_CTRL_XORSTART_MASK         0x20UL                             /**< Bit mask for AES_XORSTART */
#define _AES_CTRL_XORSTART_DEFAULT      0x00000000UL                       /**< Mode DEFAULT for AES_CTRL */
#define AES_CTRL_XORSTART_DEFAULT       (_AES_CTRL_XORSTART_DEFAULT << 5)  /**< Shifted mode DEFAULT for AES_CTRL */
#define AES_CTRL_BYTEORDER              (0x1UL << 6)                       /**< Configure byte order in data and key registers */
#define _AES_CTRL_BYTEORDER_SHIFT       6                                  /**< Shift value for AES_BYTEORDER */
#define _AES_CTRL_BYTEORDER_MASK        0x40UL                             /**< Bit mask for AES_BYTEORDER */
#define _AES_CTRL_BYTEORDER_DEFAULT     0x00000000UL                       /**< Mode DEFAULT for AES_CTRL */
#define AES_CTRL_BYTEORDER_DEFAULT      (_AES_CTRL_BYTEORDER_DEFAULT << 6) /**< Shifted mode DEFAULT for AES_CTRL */

/* Bit fields for AES CMD */
#define _AES_CMD_RESETVALUE             0x00000000UL                  /**< Default value for AES_CMD */
#define _AES_CMD_MASK                   0x00000003UL                  /**< Mask for AES_CMD */
#define AES_CMD_START                   (0x1UL << 0)                  /**< Encryption/Decryption Start */
#define _AES_CMD_START_SHIFT            0                             /**< Shift value for AES_START */
#define _AES_CMD_START_MASK             0x1UL                         /**< Bit mask for AES_START */
#define _AES_CMD_START_DEFAULT          0x00000000UL                  /**< Mode DEFAULT for AES_CMD */
#define AES_CMD_START_DEFAULT           (_AES_CMD_START_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_CMD */
#define AES_CMD_STOP                    (0x1UL << 1)                  /**< Encryption/Decryption Stop */
#define _AES_CMD_STOP_SHIFT             1                             /**< Shift value for AES_STOP */
#define _AES_CMD_STOP_MASK              0x2UL                         /**< Bit mask for AES_STOP */
#define _AES_CMD_STOP_DEFAULT           0x00000000UL                  /**< Mode DEFAULT for AES_CMD */
#define AES_CMD_STOP_DEFAULT            (_AES_CMD_STOP_DEFAULT << 1)  /**< Shifted mode DEFAULT for AES_CMD */

/* Bit fields for AES STATUS */
#define _AES_STATUS_RESETVALUE          0x00000000UL                       /**< Default value for AES_STATUS */
#define _AES_STATUS_MASK                0x00000001UL                       /**< Mask for AES_STATUS */
#define AES_STATUS_RUNNING              (0x1UL << 0)                       /**< AES Running */
#define _AES_STATUS_RUNNING_SHIFT       0                                  /**< Shift value for AES_RUNNING */
#define _AES_STATUS_RUNNING_MASK        0x1UL                              /**< Bit mask for AES_RUNNING */
#define _AES_STATUS_RUNNING_DEFAULT     0x00000000UL                       /**< Mode DEFAULT for AES_STATUS */
#define AES_STATUS_RUNNING_DEFAULT      (_AES_STATUS_RUNNING_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_STATUS */

/* Bit fields for AES IEN */
#define _AES_IEN_RESETVALUE             0x00000000UL                 /**< Default value for AES_IEN */
#define _AES_IEN_MASK                   0x00000001UL                 /**< Mask for AES_IEN */
#define AES_IEN_DONE                    (0x1UL << 0)                 /**< Encryption/Decryption Done Interrupt Enable */
#define _AES_IEN_DONE_SHIFT             0                            /**< Shift value for AES_DONE */
#define _AES_IEN_DONE_MASK              0x1UL                        /**< Bit mask for AES_DONE */
#define _AES_IEN_DONE_DEFAULT           0x00000000UL                 /**< Mode DEFAULT for AES_IEN */
#define AES_IEN_DONE_DEFAULT            (_AES_IEN_DONE_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_IEN */

/* Bit fields for AES IF */
#define _AES_IF_RESETVALUE              0x00000000UL                /**< Default value for AES_IF */
#define _AES_IF_MASK                    0x00000001UL                /**< Mask for AES_IF */
#define AES_IF_DONE                     (0x1UL << 0)                /**< Encryption/Decryption Done Interrupt Flag */
#define _AES_IF_DONE_SHIFT              0                           /**< Shift value for AES_DONE */
#define _AES_IF_DONE_MASK               0x1UL                       /**< Bit mask for AES_DONE */
#define _AES_IF_DONE_DEFAULT            0x00000000UL                /**< Mode DEFAULT for AES_IF */
#define AES_IF_DONE_DEFAULT             (_AES_IF_DONE_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_IF */

/* Bit fields for AES IFS */
#define _AES_IFS_RESETVALUE             0x00000000UL                 /**< Default value for AES_IFS */
#define _AES_IFS_MASK                   0x00000001UL                 /**< Mask for AES_IFS */
#define AES_IFS_DONE                    (0x1UL << 0)                 /**< Encryption/Decryption Done Interrupt Flag Set */
#define _AES_IFS_DONE_SHIFT             0                            /**< Shift value for AES_DONE */
#define _AES_IFS_DONE_MASK              0x1UL                        /**< Bit mask for AES_DONE */
#define _AES_IFS_DONE_DEFAULT           0x00000000UL                 /**< Mode DEFAULT for AES_IFS */
#define AES_IFS_DONE_DEFAULT            (_AES_IFS_DONE_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_IFS */

/* Bit fields for AES IFC */
#define _AES_IFC_RESETVALUE             0x00000000UL                 /**< Default value for AES_IFC */
#define _AES_IFC_MASK                   0x00000001UL                 /**< Mask for AES_IFC */
#define AES_IFC_DONE                    (0x1UL << 0)                 /**< Encryption/Decryption Done Interrupt Flag Clear */
#define _AES_IFC_DONE_SHIFT             0                            /**< Shift value for AES_DONE */
#define _AES_IFC_DONE_MASK              0x1UL                        /**< Bit mask for AES_DONE */
#define _AES_IFC_DONE_DEFAULT           0x00000000UL                 /**< Mode DEFAULT for AES_IFC */
#define AES_IFC_DONE_DEFAULT            (_AES_IFC_DONE_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_IFC */

/* Bit fields for AES DATA */
#define _AES_DATA_RESETVALUE            0x00000000UL                  /**< Default value for AES_DATA */
#define _AES_DATA_MASK                  0xFFFFFFFFUL                  /**< Mask for AES_DATA */
#define _AES_DATA_DATA_SHIFT            0                             /**< Shift value for AES_DATA */
#define _AES_DATA_DATA_MASK             0xFFFFFFFFUL                  /**< Bit mask for AES_DATA */
#define _AES_DATA_DATA_DEFAULT          0x00000000UL                  /**< Mode DEFAULT for AES_DATA */
#define AES_DATA_DATA_DEFAULT           (_AES_DATA_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_DATA */

/* Bit fields for AES XORDATA */
#define _AES_XORDATA_RESETVALUE         0x00000000UL                        /**< Default value for AES_XORDATA */
#define _AES_XORDATA_MASK               0xFFFFFFFFUL                        /**< Mask for AES_XORDATA */
#define _AES_XORDATA_XORDATA_SHIFT      0                                   /**< Shift value for AES_XORDATA */
#define _AES_XORDATA_XORDATA_MASK       0xFFFFFFFFUL                        /**< Bit mask for AES_XORDATA */
#define _AES_XORDATA_XORDATA_DEFAULT    0x00000000UL                        /**< Mode DEFAULT for AES_XORDATA */
#define AES_XORDATA_XORDATA_DEFAULT     (_AES_XORDATA_XORDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_XORDATA */

/* Bit fields for AES KEYLA */
#define _AES_KEYLA_RESETVALUE           0x00000000UL                    /**< Default value for AES_KEYLA */
#define _AES_KEYLA_MASK                 0xFFFFFFFFUL                    /**< Mask for AES_KEYLA */
#define _AES_KEYLA_KEYLA_SHIFT          0                               /**< Shift value for AES_KEYLA */
#define _AES_KEYLA_KEYLA_MASK           0xFFFFFFFFUL                    /**< Bit mask for AES_KEYLA */
#define _AES_KEYLA_KEYLA_DEFAULT        0x00000000UL                    /**< Mode DEFAULT for AES_KEYLA */
#define AES_KEYLA_KEYLA_DEFAULT         (_AES_KEYLA_KEYLA_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_KEYLA */

/* Bit fields for AES KEYLB */
#define _AES_KEYLB_RESETVALUE           0x00000000UL                    /**< Default value for AES_KEYLB */
#define _AES_KEYLB_MASK                 0xFFFFFFFFUL                    /**< Mask for AES_KEYLB */
#define _AES_KEYLB_KEYLB_SHIFT          0                               /**< Shift value for AES_KEYLB */
#define _AES_KEYLB_KEYLB_MASK           0xFFFFFFFFUL                    /**< Bit mask for AES_KEYLB */
#define _AES_KEYLB_KEYLB_DEFAULT        0x00000000UL                    /**< Mode DEFAULT for AES_KEYLB */
#define AES_KEYLB_KEYLB_DEFAULT         (_AES_KEYLB_KEYLB_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_KEYLB */

/* Bit fields for AES KEYLC */
#define _AES_KEYLC_RESETVALUE           0x00000000UL                    /**< Default value for AES_KEYLC */
#define _AES_KEYLC_MASK                 0xFFFFFFFFUL                    /**< Mask for AES_KEYLC */
#define _AES_KEYLC_KEYLC_SHIFT          0                               /**< Shift value for AES_KEYLC */
#define _AES_KEYLC_KEYLC_MASK           0xFFFFFFFFUL                    /**< Bit mask for AES_KEYLC */
#define _AES_KEYLC_KEYLC_DEFAULT        0x00000000UL                    /**< Mode DEFAULT for AES_KEYLC */
#define AES_KEYLC_KEYLC_DEFAULT         (_AES_KEYLC_KEYLC_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_KEYLC */

/* Bit fields for AES KEYLD */
#define _AES_KEYLD_RESETVALUE           0x00000000UL                    /**< Default value for AES_KEYLD */
#define _AES_KEYLD_MASK                 0xFFFFFFFFUL                    /**< Mask for AES_KEYLD */
#define _AES_KEYLD_KEYLD_SHIFT          0                               /**< Shift value for AES_KEYLD */
#define _AES_KEYLD_KEYLD_MASK           0xFFFFFFFFUL                    /**< Bit mask for AES_KEYLD */
#define _AES_KEYLD_KEYLD_DEFAULT        0x00000000UL                    /**< Mode DEFAULT for AES_KEYLD */
#define AES_KEYLD_KEYLD_DEFAULT         (_AES_KEYLD_KEYLD_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_KEYLD */

/* Bit fields for AES KEYHA */
#define _AES_KEYHA_RESETVALUE           0x00000000UL                    /**< Default value for AES_KEYHA */
#define _AES_KEYHA_MASK                 0xFFFFFFFFUL                    /**< Mask for AES_KEYHA */
#define _AES_KEYHA_KEYHA_SHIFT          0                               /**< Shift value for AES_KEYHA */
#define _AES_KEYHA_KEYHA_MASK           0xFFFFFFFFUL                    /**< Bit mask for AES_KEYHA */
#define _AES_KEYHA_KEYHA_DEFAULT        0x00000000UL                    /**< Mode DEFAULT for AES_KEYHA */
#define AES_KEYHA_KEYHA_DEFAULT         (_AES_KEYHA_KEYHA_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_KEYHA */

/* Bit fields for AES KEYHB */
#define _AES_KEYHB_RESETVALUE           0x00000000UL                    /**< Default value for AES_KEYHB */
#define _AES_KEYHB_MASK                 0xFFFFFFFFUL                    /**< Mask for AES_KEYHB */
#define _AES_KEYHB_KEYHB_SHIFT          0                               /**< Shift value for AES_KEYHB */
#define _AES_KEYHB_KEYHB_MASK           0xFFFFFFFFUL                    /**< Bit mask for AES_KEYHB */
#define _AES_KEYHB_KEYHB_DEFAULT        0x00000000UL                    /**< Mode DEFAULT for AES_KEYHB */
#define AES_KEYHB_KEYHB_DEFAULT         (_AES_KEYHB_KEYHB_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_KEYHB */

/* Bit fields for AES KEYHC */
#define _AES_KEYHC_RESETVALUE           0x00000000UL                    /**< Default value for AES_KEYHC */
#define _AES_KEYHC_MASK                 0xFFFFFFFFUL                    /**< Mask for AES_KEYHC */
#define _AES_KEYHC_KEYHC_SHIFT          0                               /**< Shift value for AES_KEYHC */
#define _AES_KEYHC_KEYHC_MASK           0xFFFFFFFFUL                    /**< Bit mask for AES_KEYHC */
#define _AES_KEYHC_KEYHC_DEFAULT        0x00000000UL                    /**< Mode DEFAULT for AES_KEYHC */
#define AES_KEYHC_KEYHC_DEFAULT         (_AES_KEYHC_KEYHC_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_KEYHC */

/* Bit fields for AES KEYHD */
#define _AES_KEYHD_RESETVALUE           0x00000000UL                    /**< Default value for AES_KEYHD */
#define _AES_KEYHD_MASK                 0xFFFFFFFFUL                    /**< Mask for AES_KEYHD */
#define _AES_KEYHD_KEYHD_SHIFT          0                               /**< Shift value for AES_KEYHD */
#define _AES_KEYHD_KEYHD_MASK           0xFFFFFFFFUL                    /**< Bit mask for AES_KEYHD */
#define _AES_KEYHD_KEYHD_DEFAULT        0x00000000UL                    /**< Mode DEFAULT for AES_KEYHD */
#define AES_KEYHD_KEYHD_DEFAULT         (_AES_KEYHD_KEYHD_DEFAULT << 0) /**< Shifted mode DEFAULT for AES_KEYHD */

/** @} End of group EFM32GG_AES */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

