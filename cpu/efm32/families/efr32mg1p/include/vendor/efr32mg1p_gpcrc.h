/***************************************************************************//**
 * @file
 * @brief EFR32MG1P_GPCRC register and bit field definitions
 * @version 5.7.0
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @defgroup EFR32MG1P_GPCRC GPCRC
 * @{
 * @brief EFR32MG1P_GPCRC Register Declaration
 ******************************************************************************/
/** GPCRC Register Declaration */
typedef struct {
  __IOM uint32_t CTRL;           /**< Control Register  */
  __IOM uint32_t CMD;            /**< Command Register  */
  __IOM uint32_t INIT;           /**< CRC Init Value  */
  __IOM uint32_t POLY;           /**< CRC Polynomial Value  */
  __IOM uint32_t INPUTDATA;      /**< Input 32-bit Data Register  */
  __IOM uint32_t INPUTDATAHWORD; /**< Input 16-bit Data Register  */
  __IOM uint32_t INPUTDATABYTE;  /**< Input 8-bit Data Register  */
  __IM uint32_t  DATA;           /**< CRC Data Register  */
  __IM uint32_t  DATAREV;        /**< CRC Data Reverse Register  */
  __IM uint32_t  DATABYTEREV;    /**< CRC Data Byte Reverse Register  */
} GPCRC_TypeDef;                 /** @} */

/***************************************************************************//**
 * @addtogroup EFR32MG1P_GPCRC
 * @{
 * @defgroup EFR32MG1P_GPCRC_BitFields  GPCRC Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for GPCRC CTRL */
#define _GPCRC_CTRL_RESETVALUE                          0x00000000UL                             /**< Default value for GPCRC_CTRL */
#define _GPCRC_CTRL_MASK                                0x00002711UL                             /**< Mask for GPCRC_CTRL */
#define GPCRC_CTRL_EN                                   (0x1UL << 0)                             /**< CRC Functionality Enable */
#define _GPCRC_CTRL_EN_SHIFT                            0                                        /**< Shift value for GPCRC_EN */
#define _GPCRC_CTRL_EN_MASK                             0x1UL                                    /**< Bit mask for GPCRC_EN */
#define _GPCRC_CTRL_EN_DEFAULT                          0x00000000UL                             /**< Mode DEFAULT for GPCRC_CTRL */
#define _GPCRC_CTRL_EN_DISABLE                          0x00000000UL                             /**< Mode DISABLE for GPCRC_CTRL */
#define _GPCRC_CTRL_EN_ENABLE                           0x00000001UL                             /**< Mode ENABLE for GPCRC_CTRL */
#define GPCRC_CTRL_EN_DEFAULT                           (_GPCRC_CTRL_EN_DEFAULT << 0)            /**< Shifted mode DEFAULT for GPCRC_CTRL */
#define GPCRC_CTRL_EN_DISABLE                           (_GPCRC_CTRL_EN_DISABLE << 0)            /**< Shifted mode DISABLE for GPCRC_CTRL */
#define GPCRC_CTRL_EN_ENABLE                            (_GPCRC_CTRL_EN_ENABLE << 0)             /**< Shifted mode ENABLE for GPCRC_CTRL */
#define GPCRC_CTRL_POLYSEL                              (0x1UL << 4)                             /**< Polynomial Select */
#define _GPCRC_CTRL_POLYSEL_SHIFT                       4                                        /**< Shift value for GPCRC_POLYSEL */
#define _GPCRC_CTRL_POLYSEL_MASK                        0x10UL                                   /**< Bit mask for GPCRC_POLYSEL */
#define _GPCRC_CTRL_POLYSEL_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for GPCRC_CTRL */
#define _GPCRC_CTRL_POLYSEL_CRC32                       0x00000000UL                             /**< Mode CRC32 for GPCRC_CTRL */
#define _GPCRC_CTRL_POLYSEL_16                          0x00000001UL                             /**< Mode 16 for GPCRC_CTRL */
#define GPCRC_CTRL_POLYSEL_DEFAULT                      (_GPCRC_CTRL_POLYSEL_DEFAULT << 4)       /**< Shifted mode DEFAULT for GPCRC_CTRL */
#define GPCRC_CTRL_POLYSEL_CRC32                        (_GPCRC_CTRL_POLYSEL_CRC32 << 4)         /**< Shifted mode CRC32 for GPCRC_CTRL */
#define GPCRC_CTRL_POLYSEL_16                           (_GPCRC_CTRL_POLYSEL_16 << 4)            /**< Shifted mode 16 for GPCRC_CTRL */
#define GPCRC_CTRL_BYTEMODE                             (0x1UL << 8)                             /**< Byte Mode Enable */
#define _GPCRC_CTRL_BYTEMODE_SHIFT                      8                                        /**< Shift value for GPCRC_BYTEMODE */
#define _GPCRC_CTRL_BYTEMODE_MASK                       0x100UL                                  /**< Bit mask for GPCRC_BYTEMODE */
#define _GPCRC_CTRL_BYTEMODE_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for GPCRC_CTRL */
#define GPCRC_CTRL_BYTEMODE_DEFAULT                     (_GPCRC_CTRL_BYTEMODE_DEFAULT << 8)      /**< Shifted mode DEFAULT for GPCRC_CTRL */
#define GPCRC_CTRL_BITREVERSE                           (0x1UL << 9)                             /**< Byte-level Bit Reverse Enable */
#define _GPCRC_CTRL_BITREVERSE_SHIFT                    9                                        /**< Shift value for GPCRC_BITREVERSE */
#define _GPCRC_CTRL_BITREVERSE_MASK                     0x200UL                                  /**< Bit mask for GPCRC_BITREVERSE */
#define _GPCRC_CTRL_BITREVERSE_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for GPCRC_CTRL */
#define _GPCRC_CTRL_BITREVERSE_NORMAL                   0x00000000UL                             /**< Mode NORMAL for GPCRC_CTRL */
#define _GPCRC_CTRL_BITREVERSE_REVERSED                 0x00000001UL                             /**< Mode REVERSED for GPCRC_CTRL */
#define GPCRC_CTRL_BITREVERSE_DEFAULT                   (_GPCRC_CTRL_BITREVERSE_DEFAULT << 9)    /**< Shifted mode DEFAULT for GPCRC_CTRL */
#define GPCRC_CTRL_BITREVERSE_NORMAL                    (_GPCRC_CTRL_BITREVERSE_NORMAL << 9)     /**< Shifted mode NORMAL for GPCRC_CTRL */
#define GPCRC_CTRL_BITREVERSE_REVERSED                  (_GPCRC_CTRL_BITREVERSE_REVERSED << 9)   /**< Shifted mode REVERSED for GPCRC_CTRL */
#define GPCRC_CTRL_BYTEREVERSE                          (0x1UL << 10)                            /**< Byte Reverse Mode */
#define _GPCRC_CTRL_BYTEREVERSE_SHIFT                   10                                       /**< Shift value for GPCRC_BYTEREVERSE */
#define _GPCRC_CTRL_BYTEREVERSE_MASK                    0x400UL                                  /**< Bit mask for GPCRC_BYTEREVERSE */
#define _GPCRC_CTRL_BYTEREVERSE_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPCRC_CTRL */
#define _GPCRC_CTRL_BYTEREVERSE_NORMAL                  0x00000000UL                             /**< Mode NORMAL for GPCRC_CTRL */
#define _GPCRC_CTRL_BYTEREVERSE_REVERSED                0x00000001UL                             /**< Mode REVERSED for GPCRC_CTRL */
#define GPCRC_CTRL_BYTEREVERSE_DEFAULT                  (_GPCRC_CTRL_BYTEREVERSE_DEFAULT << 10)  /**< Shifted mode DEFAULT for GPCRC_CTRL */
#define GPCRC_CTRL_BYTEREVERSE_NORMAL                   (_GPCRC_CTRL_BYTEREVERSE_NORMAL << 10)   /**< Shifted mode NORMAL for GPCRC_CTRL */
#define GPCRC_CTRL_BYTEREVERSE_REVERSED                 (_GPCRC_CTRL_BYTEREVERSE_REVERSED << 10) /**< Shifted mode REVERSED for GPCRC_CTRL */
#define GPCRC_CTRL_AUTOINIT                             (0x1UL << 13)                            /**< Auto Init Enable */
#define _GPCRC_CTRL_AUTOINIT_SHIFT                      13                                       /**< Shift value for GPCRC_AUTOINIT */
#define _GPCRC_CTRL_AUTOINIT_MASK                       0x2000UL                                 /**< Bit mask for GPCRC_AUTOINIT */
#define _GPCRC_CTRL_AUTOINIT_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for GPCRC_CTRL */
#define GPCRC_CTRL_AUTOINIT_DEFAULT                     (_GPCRC_CTRL_AUTOINIT_DEFAULT << 13)     /**< Shifted mode DEFAULT for GPCRC_CTRL */

/* Bit fields for GPCRC CMD */
#define _GPCRC_CMD_RESETVALUE                           0x00000000UL                   /**< Default value for GPCRC_CMD */
#define _GPCRC_CMD_MASK                                 0x00000001UL                   /**< Mask for GPCRC_CMD */
#define GPCRC_CMD_INIT                                  (0x1UL << 0)                   /**< Initialization Enable */
#define _GPCRC_CMD_INIT_SHIFT                           0                              /**< Shift value for GPCRC_INIT */
#define _GPCRC_CMD_INIT_MASK                            0x1UL                          /**< Bit mask for GPCRC_INIT */
#define _GPCRC_CMD_INIT_DEFAULT                         0x00000000UL                   /**< Mode DEFAULT for GPCRC_CMD */
#define GPCRC_CMD_INIT_DEFAULT                          (_GPCRC_CMD_INIT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPCRC_CMD */

/* Bit fields for GPCRC INIT */
#define _GPCRC_INIT_RESETVALUE                          0x00000000UL                    /**< Default value for GPCRC_INIT */
#define _GPCRC_INIT_MASK                                0xFFFFFFFFUL                    /**< Mask for GPCRC_INIT */
#define _GPCRC_INIT_INIT_SHIFT                          0                               /**< Shift value for GPCRC_INIT */
#define _GPCRC_INIT_INIT_MASK                           0xFFFFFFFFUL                    /**< Bit mask for GPCRC_INIT */
#define _GPCRC_INIT_INIT_DEFAULT                        0x00000000UL                    /**< Mode DEFAULT for GPCRC_INIT */
#define GPCRC_INIT_INIT_DEFAULT                         (_GPCRC_INIT_INIT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPCRC_INIT */

/* Bit fields for GPCRC POLY */
#define _GPCRC_POLY_RESETVALUE                          0x00000000UL                    /**< Default value for GPCRC_POLY */
#define _GPCRC_POLY_MASK                                0x0000FFFFUL                    /**< Mask for GPCRC_POLY */
#define _GPCRC_POLY_POLY_SHIFT                          0                               /**< Shift value for GPCRC_POLY */
#define _GPCRC_POLY_POLY_MASK                           0xFFFFUL                        /**< Bit mask for GPCRC_POLY */
#define _GPCRC_POLY_POLY_DEFAULT                        0x00000000UL                    /**< Mode DEFAULT for GPCRC_POLY */
#define GPCRC_POLY_POLY_DEFAULT                         (_GPCRC_POLY_POLY_DEFAULT << 0) /**< Shifted mode DEFAULT for GPCRC_POLY */

/* Bit fields for GPCRC INPUTDATA */
#define _GPCRC_INPUTDATA_RESETVALUE                     0x00000000UL                              /**< Default value for GPCRC_INPUTDATA */
#define _GPCRC_INPUTDATA_MASK                           0xFFFFFFFFUL                              /**< Mask for GPCRC_INPUTDATA */
#define _GPCRC_INPUTDATA_INPUTDATA_SHIFT                0                                         /**< Shift value for GPCRC_INPUTDATA */
#define _GPCRC_INPUTDATA_INPUTDATA_MASK                 0xFFFFFFFFUL                              /**< Bit mask for GPCRC_INPUTDATA */
#define _GPCRC_INPUTDATA_INPUTDATA_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for GPCRC_INPUTDATA */
#define GPCRC_INPUTDATA_INPUTDATA_DEFAULT               (_GPCRC_INPUTDATA_INPUTDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for GPCRC_INPUTDATA */

/* Bit fields for GPCRC INPUTDATAHWORD */
#define _GPCRC_INPUTDATAHWORD_RESETVALUE                0x00000000UL                                        /**< Default value for GPCRC_INPUTDATAHWORD */
#define _GPCRC_INPUTDATAHWORD_MASK                      0x0000FFFFUL                                        /**< Mask for GPCRC_INPUTDATAHWORD */
#define _GPCRC_INPUTDATAHWORD_INPUTDATAHWORD_SHIFT      0                                                   /**< Shift value for GPCRC_INPUTDATAHWORD */
#define _GPCRC_INPUTDATAHWORD_INPUTDATAHWORD_MASK       0xFFFFUL                                            /**< Bit mask for GPCRC_INPUTDATAHWORD */
#define _GPCRC_INPUTDATAHWORD_INPUTDATAHWORD_DEFAULT    0x00000000UL                                        /**< Mode DEFAULT for GPCRC_INPUTDATAHWORD */
#define GPCRC_INPUTDATAHWORD_INPUTDATAHWORD_DEFAULT     (_GPCRC_INPUTDATAHWORD_INPUTDATAHWORD_DEFAULT << 0) /**< Shifted mode DEFAULT for GPCRC_INPUTDATAHWORD */

/* Bit fields for GPCRC INPUTDATABYTE */
#define _GPCRC_INPUTDATABYTE_RESETVALUE                 0x00000000UL                                      /**< Default value for GPCRC_INPUTDATABYTE */
#define _GPCRC_INPUTDATABYTE_MASK                       0x000000FFUL                                      /**< Mask for GPCRC_INPUTDATABYTE */
#define _GPCRC_INPUTDATABYTE_INPUTDATABYTE_SHIFT        0                                                 /**< Shift value for GPCRC_INPUTDATABYTE */
#define _GPCRC_INPUTDATABYTE_INPUTDATABYTE_MASK         0xFFUL                                            /**< Bit mask for GPCRC_INPUTDATABYTE */
#define _GPCRC_INPUTDATABYTE_INPUTDATABYTE_DEFAULT      0x00000000UL                                      /**< Mode DEFAULT for GPCRC_INPUTDATABYTE */
#define GPCRC_INPUTDATABYTE_INPUTDATABYTE_DEFAULT       (_GPCRC_INPUTDATABYTE_INPUTDATABYTE_DEFAULT << 0) /**< Shifted mode DEFAULT for GPCRC_INPUTDATABYTE */

/* Bit fields for GPCRC DATA */
#define _GPCRC_DATA_RESETVALUE                          0x00000000UL                    /**< Default value for GPCRC_DATA */
#define _GPCRC_DATA_MASK                                0xFFFFFFFFUL                    /**< Mask for GPCRC_DATA */
#define _GPCRC_DATA_DATA_SHIFT                          0                               /**< Shift value for GPCRC_DATA */
#define _GPCRC_DATA_DATA_MASK                           0xFFFFFFFFUL                    /**< Bit mask for GPCRC_DATA */
#define _GPCRC_DATA_DATA_DEFAULT                        0x00000000UL                    /**< Mode DEFAULT for GPCRC_DATA */
#define GPCRC_DATA_DATA_DEFAULT                         (_GPCRC_DATA_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for GPCRC_DATA */

/* Bit fields for GPCRC DATAREV */
#define _GPCRC_DATAREV_RESETVALUE                       0x00000000UL                          /**< Default value for GPCRC_DATAREV */
#define _GPCRC_DATAREV_MASK                             0xFFFFFFFFUL                          /**< Mask for GPCRC_DATAREV */
#define _GPCRC_DATAREV_DATAREV_SHIFT                    0                                     /**< Shift value for GPCRC_DATAREV */
#define _GPCRC_DATAREV_DATAREV_MASK                     0xFFFFFFFFUL                          /**< Bit mask for GPCRC_DATAREV */
#define _GPCRC_DATAREV_DATAREV_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for GPCRC_DATAREV */
#define GPCRC_DATAREV_DATAREV_DEFAULT                   (_GPCRC_DATAREV_DATAREV_DEFAULT << 0) /**< Shifted mode DEFAULT for GPCRC_DATAREV */

/* Bit fields for GPCRC DATABYTEREV */
#define _GPCRC_DATABYTEREV_RESETVALUE                   0x00000000UL                                  /**< Default value for GPCRC_DATABYTEREV */
#define _GPCRC_DATABYTEREV_MASK                         0xFFFFFFFFUL                                  /**< Mask for GPCRC_DATABYTEREV */
#define _GPCRC_DATABYTEREV_DATABYTEREV_SHIFT            0                                             /**< Shift value for GPCRC_DATABYTEREV */
#define _GPCRC_DATABYTEREV_DATABYTEREV_MASK             0xFFFFFFFFUL                                  /**< Bit mask for GPCRC_DATABYTEREV */
#define _GPCRC_DATABYTEREV_DATABYTEREV_DEFAULT          0x00000000UL                                  /**< Mode DEFAULT for GPCRC_DATABYTEREV */
#define GPCRC_DATABYTEREV_DATABYTEREV_DEFAULT           (_GPCRC_DATABYTEREV_DATABYTEREV_DEFAULT << 0) /**< Shifted mode DEFAULT for GPCRC_DATABYTEREV */

/** @} */
/** @} End of group EFR32MG1P_GPCRC */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

