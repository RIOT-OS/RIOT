/***************************************************************************//**
 * @file
 * @brief EFM32GG11B_EBI register and bit field definitions
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
 * @defgroup EFM32GG11B_EBI EBI
 * @{
 * @brief EFM32GG11B_EBI Register Declaration
 ******************************************************************************/
/** EBI Register Declaration */
typedef struct {
  __IOM uint32_t CTRL;           /**< Control Register  */
  __IOM uint32_t ADDRTIMING;     /**< Address Timing Register  */
  __IOM uint32_t RDTIMING;       /**< Read Timing Register  */
  __IOM uint32_t WRTIMING;       /**< Write Timing Register  */
  __IOM uint32_t POLARITY;       /**< Polarity Register  */
  uint32_t       RESERVED0[1U];  /**< Reserved for future use **/
  __IOM uint32_t ADDRTIMING1;    /**< Address Timing Register 1  */
  __IOM uint32_t RDTIMING1;      /**< Read Timing Register 1  */
  __IOM uint32_t WRTIMING1;      /**< Write Timing Register 1  */
  __IOM uint32_t POLARITY1;      /**< Polarity Register 1  */
  __IOM uint32_t ADDRTIMING2;    /**< Address Timing Register 2  */
  __IOM uint32_t RDTIMING2;      /**< Read Timing Register 2  */
  __IOM uint32_t WRTIMING2;      /**< Write Timing Register 2  */
  __IOM uint32_t POLARITY2;      /**< Polarity Register 2  */
  __IOM uint32_t ADDRTIMING3;    /**< Address Timing Register 3  */
  __IOM uint32_t RDTIMING3;      /**< Read Timing Register 3  */
  __IOM uint32_t WRTIMING3;      /**< Write Timing Register 3  */
  __IOM uint32_t POLARITY3;      /**< Polarity Register 3  */
  __IOM uint32_t PAGECTRL;       /**< Page Control Register  */
  __IOM uint32_t NANDCTRL;       /**< NAND Control Register  */
  __IOM uint32_t CMD;            /**< Command Register  */
  __IM uint32_t  STATUS;         /**< Status Register  */
  __IM uint32_t  ECCPARITY;      /**< ECC Parity Register  */
  __IOM uint32_t TFTCTRL;        /**< TFT Control Register  */
  __IM uint32_t  TFTSTATUS;      /**< TFT Status Register  */
  __IOM uint32_t TFTCOLORFORMAT; /**< Color Format Register  */
  __IOM uint32_t TFTFRAMEBASE;   /**< TFT Frame Base Register  */
  uint32_t       RESERVED1[1U];  /**< Reserved for future use **/
  __IOM uint32_t TFTSTRIDE;      /**< TFT Stride Register  */
  __IOM uint32_t TFTSIZE;        /**< TFT Size Register  */
  __IOM uint32_t TFTHPORCH;      /**< TFT Horizontal Porch Register  */
  __IOM uint32_t TFTVPORCH;      /**< TFT Vertical Porch Register  */
  __IOM uint32_t TFTTIMING;      /**< TFT Timing Register  */
  __IOM uint32_t TFTPOLARITY;    /**< TFT Polarity Register  */
  __IOM uint32_t TFTDD;          /**< TFT Direct Drive Data Register  */
  __IOM uint32_t TFTALPHA;       /**< TFT Alpha Blending Register  */
  __IOM uint32_t TFTPIXEL0;      /**< TFT Pixel 0 Register  */
  __IOM uint32_t TFTPIXEL1;      /**< TFT Pixel 1 Register  */
  __IM uint32_t  TFTPIXEL;       /**< TFT Alpha Blending Result Pixel Register  */
  __IOM uint32_t TFTMASK;        /**< TFT Masking Register  */
  __IM uint32_t  IF;             /**< Interrupt Flag Register  */
  __IOM uint32_t IFS;            /**< Interrupt Flag Set Register  */
  __IOM uint32_t IFC;            /**< Interrupt Flag Clear Register  */
  __IOM uint32_t IEN;            /**< Interrupt Enable Register  */
  __IOM uint32_t ROUTEPEN;       /**< I/O Routing Register  */
  __IOM uint32_t ROUTELOC0;      /**< I/O Routing Location Register  */
  __IOM uint32_t ROUTELOC1;      /**< I/O Routing Location Register  */
} EBI_TypeDef;                   /** @} */

/***************************************************************************//**
 * @addtogroup EFM32GG11B_EBI
 * @{
 * @defgroup EFM32GG11B_EBI_BitFields  EBI Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for EBI CTRL */
#define _EBI_CTRL_RESETVALUE                         0x00000000UL                         /**< Default value for EBI_CTRL */
#define _EBI_CTRL_MASK                               0xCFFFFFFFUL                         /**< Mask for EBI_CTRL */
#define _EBI_CTRL_MODE_SHIFT                         0                                    /**< Shift value for EBI_MODE */
#define _EBI_CTRL_MODE_MASK                          0x3UL                                /**< Bit mask for EBI_MODE */
#define _EBI_CTRL_MODE_DEFAULT                       0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define _EBI_CTRL_MODE_D8A8                          0x00000000UL                         /**< Mode D8A8 for EBI_CTRL */
#define _EBI_CTRL_MODE_D16A16ALE                     0x00000001UL                         /**< Mode D16A16ALE for EBI_CTRL */
#define _EBI_CTRL_MODE_D8A24ALE                      0x00000002UL                         /**< Mode D8A24ALE for EBI_CTRL */
#define _EBI_CTRL_MODE_D16                           0x00000003UL                         /**< Mode D16 for EBI_CTRL */
#define EBI_CTRL_MODE_DEFAULT                        (_EBI_CTRL_MODE_DEFAULT << 0)        /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_MODE_D8A8                           (_EBI_CTRL_MODE_D8A8 << 0)           /**< Shifted mode D8A8 for EBI_CTRL */
#define EBI_CTRL_MODE_D16A16ALE                      (_EBI_CTRL_MODE_D16A16ALE << 0)      /**< Shifted mode D16A16ALE for EBI_CTRL */
#define EBI_CTRL_MODE_D8A24ALE                       (_EBI_CTRL_MODE_D8A24ALE << 0)       /**< Shifted mode D8A24ALE for EBI_CTRL */
#define EBI_CTRL_MODE_D16                            (_EBI_CTRL_MODE_D16 << 0)            /**< Shifted mode D16 for EBI_CTRL */
#define _EBI_CTRL_MODE1_SHIFT                        2                                    /**< Shift value for EBI_MODE1 */
#define _EBI_CTRL_MODE1_MASK                         0xCUL                                /**< Bit mask for EBI_MODE1 */
#define _EBI_CTRL_MODE1_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define _EBI_CTRL_MODE1_D8A8                         0x00000000UL                         /**< Mode D8A8 for EBI_CTRL */
#define _EBI_CTRL_MODE1_D16A16ALE                    0x00000001UL                         /**< Mode D16A16ALE for EBI_CTRL */
#define _EBI_CTRL_MODE1_D8A24ALE                     0x00000002UL                         /**< Mode D8A24ALE for EBI_CTRL */
#define _EBI_CTRL_MODE1_D16                          0x00000003UL                         /**< Mode D16 for EBI_CTRL */
#define EBI_CTRL_MODE1_DEFAULT                       (_EBI_CTRL_MODE1_DEFAULT << 2)       /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_MODE1_D8A8                          (_EBI_CTRL_MODE1_D8A8 << 2)          /**< Shifted mode D8A8 for EBI_CTRL */
#define EBI_CTRL_MODE1_D16A16ALE                     (_EBI_CTRL_MODE1_D16A16ALE << 2)     /**< Shifted mode D16A16ALE for EBI_CTRL */
#define EBI_CTRL_MODE1_D8A24ALE                      (_EBI_CTRL_MODE1_D8A24ALE << 2)      /**< Shifted mode D8A24ALE for EBI_CTRL */
#define EBI_CTRL_MODE1_D16                           (_EBI_CTRL_MODE1_D16 << 2)           /**< Shifted mode D16 for EBI_CTRL */
#define _EBI_CTRL_MODE2_SHIFT                        4                                    /**< Shift value for EBI_MODE2 */
#define _EBI_CTRL_MODE2_MASK                         0x30UL                               /**< Bit mask for EBI_MODE2 */
#define _EBI_CTRL_MODE2_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define _EBI_CTRL_MODE2_D8A8                         0x00000000UL                         /**< Mode D8A8 for EBI_CTRL */
#define _EBI_CTRL_MODE2_D16A16ALE                    0x00000001UL                         /**< Mode D16A16ALE for EBI_CTRL */
#define _EBI_CTRL_MODE2_D8A24ALE                     0x00000002UL                         /**< Mode D8A24ALE for EBI_CTRL */
#define _EBI_CTRL_MODE2_D16                          0x00000003UL                         /**< Mode D16 for EBI_CTRL */
#define EBI_CTRL_MODE2_DEFAULT                       (_EBI_CTRL_MODE2_DEFAULT << 4)       /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_MODE2_D8A8                          (_EBI_CTRL_MODE2_D8A8 << 4)          /**< Shifted mode D8A8 for EBI_CTRL */
#define EBI_CTRL_MODE2_D16A16ALE                     (_EBI_CTRL_MODE2_D16A16ALE << 4)     /**< Shifted mode D16A16ALE for EBI_CTRL */
#define EBI_CTRL_MODE2_D8A24ALE                      (_EBI_CTRL_MODE2_D8A24ALE << 4)      /**< Shifted mode D8A24ALE for EBI_CTRL */
#define EBI_CTRL_MODE2_D16                           (_EBI_CTRL_MODE2_D16 << 4)           /**< Shifted mode D16 for EBI_CTRL */
#define _EBI_CTRL_MODE3_SHIFT                        6                                    /**< Shift value for EBI_MODE3 */
#define _EBI_CTRL_MODE3_MASK                         0xC0UL                               /**< Bit mask for EBI_MODE3 */
#define _EBI_CTRL_MODE3_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define _EBI_CTRL_MODE3_D8A8                         0x00000000UL                         /**< Mode D8A8 for EBI_CTRL */
#define _EBI_CTRL_MODE3_D16A16ALE                    0x00000001UL                         /**< Mode D16A16ALE for EBI_CTRL */
#define _EBI_CTRL_MODE3_D8A24ALE                     0x00000002UL                         /**< Mode D8A24ALE for EBI_CTRL */
#define _EBI_CTRL_MODE3_D16                          0x00000003UL                         /**< Mode D16 for EBI_CTRL */
#define EBI_CTRL_MODE3_DEFAULT                       (_EBI_CTRL_MODE3_DEFAULT << 6)       /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_MODE3_D8A8                          (_EBI_CTRL_MODE3_D8A8 << 6)          /**< Shifted mode D8A8 for EBI_CTRL */
#define EBI_CTRL_MODE3_D16A16ALE                     (_EBI_CTRL_MODE3_D16A16ALE << 6)     /**< Shifted mode D16A16ALE for EBI_CTRL */
#define EBI_CTRL_MODE3_D8A24ALE                      (_EBI_CTRL_MODE3_D8A24ALE << 6)      /**< Shifted mode D8A24ALE for EBI_CTRL */
#define EBI_CTRL_MODE3_D16                           (_EBI_CTRL_MODE3_D16 << 6)           /**< Shifted mode D16 for EBI_CTRL */
#define EBI_CTRL_BANK0EN                             (0x1UL << 8)                         /**< Bank 0 Enable */
#define _EBI_CTRL_BANK0EN_SHIFT                      8                                    /**< Shift value for EBI_BANK0EN */
#define _EBI_CTRL_BANK0EN_MASK                       0x100UL                              /**< Bit mask for EBI_BANK0EN */
#define _EBI_CTRL_BANK0EN_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BANK0EN_DEFAULT                     (_EBI_CTRL_BANK0EN_DEFAULT << 8)     /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BANK1EN                             (0x1UL << 9)                         /**< Bank 1 Enable */
#define _EBI_CTRL_BANK1EN_SHIFT                      9                                    /**< Shift value for EBI_BANK1EN */
#define _EBI_CTRL_BANK1EN_MASK                       0x200UL                              /**< Bit mask for EBI_BANK1EN */
#define _EBI_CTRL_BANK1EN_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BANK1EN_DEFAULT                     (_EBI_CTRL_BANK1EN_DEFAULT << 9)     /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BANK2EN                             (0x1UL << 10)                        /**< Bank 2 Enable */
#define _EBI_CTRL_BANK2EN_SHIFT                      10                                   /**< Shift value for EBI_BANK2EN */
#define _EBI_CTRL_BANK2EN_MASK                       0x400UL                              /**< Bit mask for EBI_BANK2EN */
#define _EBI_CTRL_BANK2EN_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BANK2EN_DEFAULT                     (_EBI_CTRL_BANK2EN_DEFAULT << 10)    /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BANK3EN                             (0x1UL << 11)                        /**< Bank 3 Enable */
#define _EBI_CTRL_BANK3EN_SHIFT                      11                                   /**< Shift value for EBI_BANK3EN */
#define _EBI_CTRL_BANK3EN_MASK                       0x800UL                              /**< Bit mask for EBI_BANK3EN */
#define _EBI_CTRL_BANK3EN_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BANK3EN_DEFAULT                     (_EBI_CTRL_BANK3EN_DEFAULT << 11)    /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_NOIDLE                              (0x1UL << 12)                        /**< No Idle Cycle Insertion on Bank 0 */
#define _EBI_CTRL_NOIDLE_SHIFT                       12                                   /**< Shift value for EBI_NOIDLE */
#define _EBI_CTRL_NOIDLE_MASK                        0x1000UL                             /**< Bit mask for EBI_NOIDLE */
#define _EBI_CTRL_NOIDLE_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_NOIDLE_DEFAULT                      (_EBI_CTRL_NOIDLE_DEFAULT << 12)     /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_NOIDLE1                             (0x1UL << 13)                        /**< No Idle Cycle Insertion on Bank 1 */
#define _EBI_CTRL_NOIDLE1_SHIFT                      13                                   /**< Shift value for EBI_NOIDLE1 */
#define _EBI_CTRL_NOIDLE1_MASK                       0x2000UL                             /**< Bit mask for EBI_NOIDLE1 */
#define _EBI_CTRL_NOIDLE1_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_NOIDLE1_DEFAULT                     (_EBI_CTRL_NOIDLE1_DEFAULT << 13)    /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_NOIDLE2                             (0x1UL << 14)                        /**< No Idle Cycle Insertion on Bank 2 */
#define _EBI_CTRL_NOIDLE2_SHIFT                      14                                   /**< Shift value for EBI_NOIDLE2 */
#define _EBI_CTRL_NOIDLE2_MASK                       0x4000UL                             /**< Bit mask for EBI_NOIDLE2 */
#define _EBI_CTRL_NOIDLE2_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_NOIDLE2_DEFAULT                     (_EBI_CTRL_NOIDLE2_DEFAULT << 14)    /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_NOIDLE3                             (0x1UL << 15)                        /**< No Idle Cycle Insertion on Bank 3 */
#define _EBI_CTRL_NOIDLE3_SHIFT                      15                                   /**< Shift value for EBI_NOIDLE3 */
#define _EBI_CTRL_NOIDLE3_MASK                       0x8000UL                             /**< Bit mask for EBI_NOIDLE3 */
#define _EBI_CTRL_NOIDLE3_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_NOIDLE3_DEFAULT                     (_EBI_CTRL_NOIDLE3_DEFAULT << 15)    /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDYEN                              (0x1UL << 16)                        /**< ARDY Enable */
#define _EBI_CTRL_ARDYEN_SHIFT                       16                                   /**< Shift value for EBI_ARDYEN */
#define _EBI_CTRL_ARDYEN_MASK                        0x10000UL                            /**< Bit mask for EBI_ARDYEN */
#define _EBI_CTRL_ARDYEN_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDYEN_DEFAULT                      (_EBI_CTRL_ARDYEN_DEFAULT << 16)     /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDYTODIS                           (0x1UL << 17)                        /**< ARDY Timeout Disable */
#define _EBI_CTRL_ARDYTODIS_SHIFT                    17                                   /**< Shift value for EBI_ARDYTODIS */
#define _EBI_CTRL_ARDYTODIS_MASK                     0x20000UL                            /**< Bit mask for EBI_ARDYTODIS */
#define _EBI_CTRL_ARDYTODIS_DEFAULT                  0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDYTODIS_DEFAULT                   (_EBI_CTRL_ARDYTODIS_DEFAULT << 17)  /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDY1EN                             (0x1UL << 18)                        /**< ARDY Enable for Bank 1 */
#define _EBI_CTRL_ARDY1EN_SHIFT                      18                                   /**< Shift value for EBI_ARDY1EN */
#define _EBI_CTRL_ARDY1EN_MASK                       0x40000UL                            /**< Bit mask for EBI_ARDY1EN */
#define _EBI_CTRL_ARDY1EN_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDY1EN_DEFAULT                     (_EBI_CTRL_ARDY1EN_DEFAULT << 18)    /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDYTO1DIS                          (0x1UL << 19)                        /**< ARDY Timeout Disable for Bank 1 */
#define _EBI_CTRL_ARDYTO1DIS_SHIFT                   19                                   /**< Shift value for EBI_ARDYTO1DIS */
#define _EBI_CTRL_ARDYTO1DIS_MASK                    0x80000UL                            /**< Bit mask for EBI_ARDYTO1DIS */
#define _EBI_CTRL_ARDYTO1DIS_DEFAULT                 0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDYTO1DIS_DEFAULT                  (_EBI_CTRL_ARDYTO1DIS_DEFAULT << 19) /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDY2EN                             (0x1UL << 20)                        /**< ARDY Enable for Bank 2 */
#define _EBI_CTRL_ARDY2EN_SHIFT                      20                                   /**< Shift value for EBI_ARDY2EN */
#define _EBI_CTRL_ARDY2EN_MASK                       0x100000UL                           /**< Bit mask for EBI_ARDY2EN */
#define _EBI_CTRL_ARDY2EN_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDY2EN_DEFAULT                     (_EBI_CTRL_ARDY2EN_DEFAULT << 20)    /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDYTO2DIS                          (0x1UL << 21)                        /**< ARDY Timeout Disable for Bank 2 */
#define _EBI_CTRL_ARDYTO2DIS_SHIFT                   21                                   /**< Shift value for EBI_ARDYTO2DIS */
#define _EBI_CTRL_ARDYTO2DIS_MASK                    0x200000UL                           /**< Bit mask for EBI_ARDYTO2DIS */
#define _EBI_CTRL_ARDYTO2DIS_DEFAULT                 0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDYTO2DIS_DEFAULT                  (_EBI_CTRL_ARDYTO2DIS_DEFAULT << 21) /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDY3EN                             (0x1UL << 22)                        /**< ARDY Enable for Bank 3 */
#define _EBI_CTRL_ARDY3EN_SHIFT                      22                                   /**< Shift value for EBI_ARDY3EN */
#define _EBI_CTRL_ARDY3EN_MASK                       0x400000UL                           /**< Bit mask for EBI_ARDY3EN */
#define _EBI_CTRL_ARDY3EN_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDY3EN_DEFAULT                     (_EBI_CTRL_ARDY3EN_DEFAULT << 22)    /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDYTO3DIS                          (0x1UL << 23)                        /**< ARDY Timeout Disable for Bank 3 */
#define _EBI_CTRL_ARDYTO3DIS_SHIFT                   23                                   /**< Shift value for EBI_ARDYTO3DIS */
#define _EBI_CTRL_ARDYTO3DIS_MASK                    0x800000UL                           /**< Bit mask for EBI_ARDYTO3DIS */
#define _EBI_CTRL_ARDYTO3DIS_DEFAULT                 0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ARDYTO3DIS_DEFAULT                  (_EBI_CTRL_ARDYTO3DIS_DEFAULT << 23) /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BL                                  (0x1UL << 24)                        /**< Byte Lane Enable for Bank 0 */
#define _EBI_CTRL_BL_SHIFT                           24                                   /**< Shift value for EBI_BL */
#define _EBI_CTRL_BL_MASK                            0x1000000UL                          /**< Bit mask for EBI_BL */
#define _EBI_CTRL_BL_DEFAULT                         0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BL_DEFAULT                          (_EBI_CTRL_BL_DEFAULT << 24)         /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BL1                                 (0x1UL << 25)                        /**< Byte Lane Enable for Bank 1 */
#define _EBI_CTRL_BL1_SHIFT                          25                                   /**< Shift value for EBI_BL1 */
#define _EBI_CTRL_BL1_MASK                           0x2000000UL                          /**< Bit mask for EBI_BL1 */
#define _EBI_CTRL_BL1_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BL1_DEFAULT                         (_EBI_CTRL_BL1_DEFAULT << 25)        /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BL2                                 (0x1UL << 26)                        /**< Byte Lane Enable for Bank 2 */
#define _EBI_CTRL_BL2_SHIFT                          26                                   /**< Shift value for EBI_BL2 */
#define _EBI_CTRL_BL2_MASK                           0x4000000UL                          /**< Bit mask for EBI_BL2 */
#define _EBI_CTRL_BL2_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BL2_DEFAULT                         (_EBI_CTRL_BL2_DEFAULT << 26)        /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BL3                                 (0x1UL << 27)                        /**< Byte Lane Enable for Bank 3 */
#define _EBI_CTRL_BL3_SHIFT                          27                                   /**< Shift value for EBI_BL3 */
#define _EBI_CTRL_BL3_MASK                           0x8000000UL                          /**< Bit mask for EBI_BL3 */
#define _EBI_CTRL_BL3_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_BL3_DEFAULT                         (_EBI_CTRL_BL3_DEFAULT << 27)        /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ITS                                 (0x1UL << 30)                        /**< Individual Timing Set, Line Polarity and Mode Definition Enable */
#define _EBI_CTRL_ITS_SHIFT                          30                                   /**< Shift value for EBI_ITS */
#define _EBI_CTRL_ITS_MASK                           0x40000000UL                         /**< Bit mask for EBI_ITS */
#define _EBI_CTRL_ITS_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ITS_DEFAULT                         (_EBI_CTRL_ITS_DEFAULT << 30)        /**< Shifted mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ALTMAP                              (0x1UL << 31)                        /**< Alternative Address Map Enable */
#define _EBI_CTRL_ALTMAP_SHIFT                       31                                   /**< Shift value for EBI_ALTMAP */
#define _EBI_CTRL_ALTMAP_MASK                        0x80000000UL                         /**< Bit mask for EBI_ALTMAP */
#define _EBI_CTRL_ALTMAP_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for EBI_CTRL */
#define EBI_CTRL_ALTMAP_DEFAULT                      (_EBI_CTRL_ALTMAP_DEFAULT << 31)     /**< Shifted mode DEFAULT for EBI_CTRL */

/* Bit fields for EBI ADDRTIMING */
#define _EBI_ADDRTIMING_RESETVALUE                   0x00000707UL                             /**< Default value for EBI_ADDRTIMING */
#define _EBI_ADDRTIMING_MASK                         0x10000707UL                             /**< Mask for EBI_ADDRTIMING */
#define _EBI_ADDRTIMING_ADDRSETUP_SHIFT              0                                        /**< Shift value for EBI_ADDRSETUP */
#define _EBI_ADDRTIMING_ADDRSETUP_MASK               0x7UL                                    /**< Bit mask for EBI_ADDRSETUP */
#define _EBI_ADDRTIMING_ADDRSETUP_DEFAULT            0x00000007UL                             /**< Mode DEFAULT for EBI_ADDRTIMING */
#define EBI_ADDRTIMING_ADDRSETUP_DEFAULT             (_EBI_ADDRTIMING_ADDRSETUP_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_ADDRTIMING */
#define _EBI_ADDRTIMING_ADDRHOLD_SHIFT               8                                        /**< Shift value for EBI_ADDRHOLD */
#define _EBI_ADDRTIMING_ADDRHOLD_MASK                0x700UL                                  /**< Bit mask for EBI_ADDRHOLD */
#define _EBI_ADDRTIMING_ADDRHOLD_DEFAULT             0x00000007UL                             /**< Mode DEFAULT for EBI_ADDRTIMING */
#define EBI_ADDRTIMING_ADDRHOLD_DEFAULT              (_EBI_ADDRTIMING_ADDRHOLD_DEFAULT << 8)  /**< Shifted mode DEFAULT for EBI_ADDRTIMING */
#define EBI_ADDRTIMING_HALFALE                       (0x1UL << 28)                            /**< Half Cycle ALE Strobe Duration Enable */
#define _EBI_ADDRTIMING_HALFALE_SHIFT                28                                       /**< Shift value for EBI_HALFALE */
#define _EBI_ADDRTIMING_HALFALE_MASK                 0x10000000UL                             /**< Bit mask for EBI_HALFALE */
#define _EBI_ADDRTIMING_HALFALE_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for EBI_ADDRTIMING */
#define EBI_ADDRTIMING_HALFALE_DEFAULT               (_EBI_ADDRTIMING_HALFALE_DEFAULT << 28)  /**< Shifted mode DEFAULT for EBI_ADDRTIMING */

/* Bit fields for EBI RDTIMING */
#define _EBI_RDTIMING_RESETVALUE                     0x00077F07UL                           /**< Default value for EBI_RDTIMING */
#define _EBI_RDTIMING_MASK                           0x70077F07UL                           /**< Mask for EBI_RDTIMING */
#define _EBI_RDTIMING_RDSETUP_SHIFT                  0                                      /**< Shift value for EBI_RDSETUP */
#define _EBI_RDTIMING_RDSETUP_MASK                   0x7UL                                  /**< Bit mask for EBI_RDSETUP */
#define _EBI_RDTIMING_RDSETUP_DEFAULT                0x00000007UL                           /**< Mode DEFAULT for EBI_RDTIMING */
#define EBI_RDTIMING_RDSETUP_DEFAULT                 (_EBI_RDTIMING_RDSETUP_DEFAULT << 0)   /**< Shifted mode DEFAULT for EBI_RDTIMING */
#define _EBI_RDTIMING_RDSTRB_SHIFT                   8                                      /**< Shift value for EBI_RDSTRB */
#define _EBI_RDTIMING_RDSTRB_MASK                    0x7F00UL                               /**< Bit mask for EBI_RDSTRB */
#define _EBI_RDTIMING_RDSTRB_DEFAULT                 0x0000007FUL                           /**< Mode DEFAULT for EBI_RDTIMING */
#define EBI_RDTIMING_RDSTRB_DEFAULT                  (_EBI_RDTIMING_RDSTRB_DEFAULT << 8)    /**< Shifted mode DEFAULT for EBI_RDTIMING */
#define _EBI_RDTIMING_RDHOLD_SHIFT                   16                                     /**< Shift value for EBI_RDHOLD */
#define _EBI_RDTIMING_RDHOLD_MASK                    0x70000UL                              /**< Bit mask for EBI_RDHOLD */
#define _EBI_RDTIMING_RDHOLD_DEFAULT                 0x00000007UL                           /**< Mode DEFAULT for EBI_RDTIMING */
#define EBI_RDTIMING_RDHOLD_DEFAULT                  (_EBI_RDTIMING_RDHOLD_DEFAULT << 16)   /**< Shifted mode DEFAULT for EBI_RDTIMING */
#define EBI_RDTIMING_HALFRE                          (0x1UL << 28)                          /**< Half Cycle REn Strobe Duration Enable */
#define _EBI_RDTIMING_HALFRE_SHIFT                   28                                     /**< Shift value for EBI_HALFRE */
#define _EBI_RDTIMING_HALFRE_MASK                    0x10000000UL                           /**< Bit mask for EBI_HALFRE */
#define _EBI_RDTIMING_HALFRE_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for EBI_RDTIMING */
#define EBI_RDTIMING_HALFRE_DEFAULT                  (_EBI_RDTIMING_HALFRE_DEFAULT << 28)   /**< Shifted mode DEFAULT for EBI_RDTIMING */
#define EBI_RDTIMING_PREFETCH                        (0x1UL << 29)                          /**< Prefetch Enable */
#define _EBI_RDTIMING_PREFETCH_SHIFT                 29                                     /**< Shift value for EBI_PREFETCH */
#define _EBI_RDTIMING_PREFETCH_MASK                  0x20000000UL                           /**< Bit mask for EBI_PREFETCH */
#define _EBI_RDTIMING_PREFETCH_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for EBI_RDTIMING */
#define EBI_RDTIMING_PREFETCH_DEFAULT                (_EBI_RDTIMING_PREFETCH_DEFAULT << 29) /**< Shifted mode DEFAULT for EBI_RDTIMING */
#define EBI_RDTIMING_PAGEMODE                        (0x1UL << 30)                          /**< Page Mode Access Enable */
#define _EBI_RDTIMING_PAGEMODE_SHIFT                 30                                     /**< Shift value for EBI_PAGEMODE */
#define _EBI_RDTIMING_PAGEMODE_MASK                  0x40000000UL                           /**< Bit mask for EBI_PAGEMODE */
#define _EBI_RDTIMING_PAGEMODE_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for EBI_RDTIMING */
#define EBI_RDTIMING_PAGEMODE_DEFAULT                (_EBI_RDTIMING_PAGEMODE_DEFAULT << 30) /**< Shifted mode DEFAULT for EBI_RDTIMING */

/* Bit fields for EBI WRTIMING */
#define _EBI_WRTIMING_RESETVALUE                     0x00077F07UL                          /**< Default value for EBI_WRTIMING */
#define _EBI_WRTIMING_MASK                           0x30077F07UL                          /**< Mask for EBI_WRTIMING */
#define _EBI_WRTIMING_WRSETUP_SHIFT                  0                                     /**< Shift value for EBI_WRSETUP */
#define _EBI_WRTIMING_WRSETUP_MASK                   0x7UL                                 /**< Bit mask for EBI_WRSETUP */
#define _EBI_WRTIMING_WRSETUP_DEFAULT                0x00000007UL                          /**< Mode DEFAULT for EBI_WRTIMING */
#define EBI_WRTIMING_WRSETUP_DEFAULT                 (_EBI_WRTIMING_WRSETUP_DEFAULT << 0)  /**< Shifted mode DEFAULT for EBI_WRTIMING */
#define _EBI_WRTIMING_WRSTRB_SHIFT                   8                                     /**< Shift value for EBI_WRSTRB */
#define _EBI_WRTIMING_WRSTRB_MASK                    0x7F00UL                              /**< Bit mask for EBI_WRSTRB */
#define _EBI_WRTIMING_WRSTRB_DEFAULT                 0x0000007FUL                          /**< Mode DEFAULT for EBI_WRTIMING */
#define EBI_WRTIMING_WRSTRB_DEFAULT                  (_EBI_WRTIMING_WRSTRB_DEFAULT << 8)   /**< Shifted mode DEFAULT for EBI_WRTIMING */
#define _EBI_WRTIMING_WRHOLD_SHIFT                   16                                    /**< Shift value for EBI_WRHOLD */
#define _EBI_WRTIMING_WRHOLD_MASK                    0x70000UL                             /**< Bit mask for EBI_WRHOLD */
#define _EBI_WRTIMING_WRHOLD_DEFAULT                 0x00000007UL                          /**< Mode DEFAULT for EBI_WRTIMING */
#define EBI_WRTIMING_WRHOLD_DEFAULT                  (_EBI_WRTIMING_WRHOLD_DEFAULT << 16)  /**< Shifted mode DEFAULT for EBI_WRTIMING */
#define EBI_WRTIMING_HALFWE                          (0x1UL << 28)                         /**< Half Cycle WEn Strobe Duration Enable */
#define _EBI_WRTIMING_HALFWE_SHIFT                   28                                    /**< Shift value for EBI_HALFWE */
#define _EBI_WRTIMING_HALFWE_MASK                    0x10000000UL                          /**< Bit mask for EBI_HALFWE */
#define _EBI_WRTIMING_HALFWE_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for EBI_WRTIMING */
#define EBI_WRTIMING_HALFWE_DEFAULT                  (_EBI_WRTIMING_HALFWE_DEFAULT << 28)  /**< Shifted mode DEFAULT for EBI_WRTIMING */
#define EBI_WRTIMING_WBUFDIS                         (0x1UL << 29)                         /**< Write Buffer Disable */
#define _EBI_WRTIMING_WBUFDIS_SHIFT                  29                                    /**< Shift value for EBI_WBUFDIS */
#define _EBI_WRTIMING_WBUFDIS_MASK                   0x20000000UL                          /**< Bit mask for EBI_WBUFDIS */
#define _EBI_WRTIMING_WBUFDIS_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for EBI_WRTIMING */
#define EBI_WRTIMING_WBUFDIS_DEFAULT                 (_EBI_WRTIMING_WBUFDIS_DEFAULT << 29) /**< Shifted mode DEFAULT for EBI_WRTIMING */

/* Bit fields for EBI POLARITY */
#define _EBI_POLARITY_RESETVALUE                     0x00000000UL                            /**< Default value for EBI_POLARITY */
#define _EBI_POLARITY_MASK                           0x0000003FUL                            /**< Mask for EBI_POLARITY */
#define EBI_POLARITY_CSPOL                           (0x1UL << 0)                            /**< Chip Select Polarity */
#define _EBI_POLARITY_CSPOL_SHIFT                    0                                       /**< Shift value for EBI_CSPOL */
#define _EBI_POLARITY_CSPOL_MASK                     0x1UL                                   /**< Bit mask for EBI_CSPOL */
#define _EBI_POLARITY_CSPOL_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EBI_POLARITY */
#define _EBI_POLARITY_CSPOL_ACTIVELOW                0x00000000UL                            /**< Mode ACTIVELOW for EBI_POLARITY */
#define _EBI_POLARITY_CSPOL_ACTIVEHIGH               0x00000001UL                            /**< Mode ACTIVEHIGH for EBI_POLARITY */
#define EBI_POLARITY_CSPOL_DEFAULT                   (_EBI_POLARITY_CSPOL_DEFAULT << 0)      /**< Shifted mode DEFAULT for EBI_POLARITY */
#define EBI_POLARITY_CSPOL_ACTIVELOW                 (_EBI_POLARITY_CSPOL_ACTIVELOW << 0)    /**< Shifted mode ACTIVELOW for EBI_POLARITY */
#define EBI_POLARITY_CSPOL_ACTIVEHIGH                (_EBI_POLARITY_CSPOL_ACTIVEHIGH << 0)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY */
#define EBI_POLARITY_REPOL                           (0x1UL << 1)                            /**< Read Enable Polarity */
#define _EBI_POLARITY_REPOL_SHIFT                    1                                       /**< Shift value for EBI_REPOL */
#define _EBI_POLARITY_REPOL_MASK                     0x2UL                                   /**< Bit mask for EBI_REPOL */
#define _EBI_POLARITY_REPOL_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EBI_POLARITY */
#define _EBI_POLARITY_REPOL_ACTIVELOW                0x00000000UL                            /**< Mode ACTIVELOW for EBI_POLARITY */
#define _EBI_POLARITY_REPOL_ACTIVEHIGH               0x00000001UL                            /**< Mode ACTIVEHIGH for EBI_POLARITY */
#define EBI_POLARITY_REPOL_DEFAULT                   (_EBI_POLARITY_REPOL_DEFAULT << 1)      /**< Shifted mode DEFAULT for EBI_POLARITY */
#define EBI_POLARITY_REPOL_ACTIVELOW                 (_EBI_POLARITY_REPOL_ACTIVELOW << 1)    /**< Shifted mode ACTIVELOW for EBI_POLARITY */
#define EBI_POLARITY_REPOL_ACTIVEHIGH                (_EBI_POLARITY_REPOL_ACTIVEHIGH << 1)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY */
#define EBI_POLARITY_WEPOL                           (0x1UL << 2)                            /**< Write Enable Polarity */
#define _EBI_POLARITY_WEPOL_SHIFT                    2                                       /**< Shift value for EBI_WEPOL */
#define _EBI_POLARITY_WEPOL_MASK                     0x4UL                                   /**< Bit mask for EBI_WEPOL */
#define _EBI_POLARITY_WEPOL_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EBI_POLARITY */
#define _EBI_POLARITY_WEPOL_ACTIVELOW                0x00000000UL                            /**< Mode ACTIVELOW for EBI_POLARITY */
#define _EBI_POLARITY_WEPOL_ACTIVEHIGH               0x00000001UL                            /**< Mode ACTIVEHIGH for EBI_POLARITY */
#define EBI_POLARITY_WEPOL_DEFAULT                   (_EBI_POLARITY_WEPOL_DEFAULT << 2)      /**< Shifted mode DEFAULT for EBI_POLARITY */
#define EBI_POLARITY_WEPOL_ACTIVELOW                 (_EBI_POLARITY_WEPOL_ACTIVELOW << 2)    /**< Shifted mode ACTIVELOW for EBI_POLARITY */
#define EBI_POLARITY_WEPOL_ACTIVEHIGH                (_EBI_POLARITY_WEPOL_ACTIVEHIGH << 2)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY */
#define EBI_POLARITY_ALEPOL                          (0x1UL << 3)                            /**< Address Latch Polarity */
#define _EBI_POLARITY_ALEPOL_SHIFT                   3                                       /**< Shift value for EBI_ALEPOL */
#define _EBI_POLARITY_ALEPOL_MASK                    0x8UL                                   /**< Bit mask for EBI_ALEPOL */
#define _EBI_POLARITY_ALEPOL_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EBI_POLARITY */
#define _EBI_POLARITY_ALEPOL_ACTIVELOW               0x00000000UL                            /**< Mode ACTIVELOW for EBI_POLARITY */
#define _EBI_POLARITY_ALEPOL_ACTIVEHIGH              0x00000001UL                            /**< Mode ACTIVEHIGH for EBI_POLARITY */
#define EBI_POLARITY_ALEPOL_DEFAULT                  (_EBI_POLARITY_ALEPOL_DEFAULT << 3)     /**< Shifted mode DEFAULT for EBI_POLARITY */
#define EBI_POLARITY_ALEPOL_ACTIVELOW                (_EBI_POLARITY_ALEPOL_ACTIVELOW << 3)   /**< Shifted mode ACTIVELOW for EBI_POLARITY */
#define EBI_POLARITY_ALEPOL_ACTIVEHIGH               (_EBI_POLARITY_ALEPOL_ACTIVEHIGH << 3)  /**< Shifted mode ACTIVEHIGH for EBI_POLARITY */
#define EBI_POLARITY_ARDYPOL                         (0x1UL << 4)                            /**< ARDY Polarity */
#define _EBI_POLARITY_ARDYPOL_SHIFT                  4                                       /**< Shift value for EBI_ARDYPOL */
#define _EBI_POLARITY_ARDYPOL_MASK                   0x10UL                                  /**< Bit mask for EBI_ARDYPOL */
#define _EBI_POLARITY_ARDYPOL_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for EBI_POLARITY */
#define _EBI_POLARITY_ARDYPOL_ACTIVELOW              0x00000000UL                            /**< Mode ACTIVELOW for EBI_POLARITY */
#define _EBI_POLARITY_ARDYPOL_ACTIVEHIGH             0x00000001UL                            /**< Mode ACTIVEHIGH for EBI_POLARITY */
#define EBI_POLARITY_ARDYPOL_DEFAULT                 (_EBI_POLARITY_ARDYPOL_DEFAULT << 4)    /**< Shifted mode DEFAULT for EBI_POLARITY */
#define EBI_POLARITY_ARDYPOL_ACTIVELOW               (_EBI_POLARITY_ARDYPOL_ACTIVELOW << 4)  /**< Shifted mode ACTIVELOW for EBI_POLARITY */
#define EBI_POLARITY_ARDYPOL_ACTIVEHIGH              (_EBI_POLARITY_ARDYPOL_ACTIVEHIGH << 4) /**< Shifted mode ACTIVEHIGH for EBI_POLARITY */
#define EBI_POLARITY_BLPOL                           (0x1UL << 5)                            /**< BL Polarity */
#define _EBI_POLARITY_BLPOL_SHIFT                    5                                       /**< Shift value for EBI_BLPOL */
#define _EBI_POLARITY_BLPOL_MASK                     0x20UL                                  /**< Bit mask for EBI_BLPOL */
#define _EBI_POLARITY_BLPOL_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EBI_POLARITY */
#define _EBI_POLARITY_BLPOL_ACTIVELOW                0x00000000UL                            /**< Mode ACTIVELOW for EBI_POLARITY */
#define _EBI_POLARITY_BLPOL_ACTIVEHIGH               0x00000001UL                            /**< Mode ACTIVEHIGH for EBI_POLARITY */
#define EBI_POLARITY_BLPOL_DEFAULT                   (_EBI_POLARITY_BLPOL_DEFAULT << 5)      /**< Shifted mode DEFAULT for EBI_POLARITY */
#define EBI_POLARITY_BLPOL_ACTIVELOW                 (_EBI_POLARITY_BLPOL_ACTIVELOW << 5)    /**< Shifted mode ACTIVELOW for EBI_POLARITY */
#define EBI_POLARITY_BLPOL_ACTIVEHIGH                (_EBI_POLARITY_BLPOL_ACTIVEHIGH << 5)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY */

/* Bit fields for EBI ADDRTIMING1 */
#define _EBI_ADDRTIMING1_RESETVALUE                  0x00000707UL                              /**< Default value for EBI_ADDRTIMING1 */
#define _EBI_ADDRTIMING1_MASK                        0x10000707UL                              /**< Mask for EBI_ADDRTIMING1 */
#define _EBI_ADDRTIMING1_ADDRSETUP_SHIFT             0                                         /**< Shift value for EBI_ADDRSETUP */
#define _EBI_ADDRTIMING1_ADDRSETUP_MASK              0x7UL                                     /**< Bit mask for EBI_ADDRSETUP */
#define _EBI_ADDRTIMING1_ADDRSETUP_DEFAULT           0x00000007UL                              /**< Mode DEFAULT for EBI_ADDRTIMING1 */
#define EBI_ADDRTIMING1_ADDRSETUP_DEFAULT            (_EBI_ADDRTIMING1_ADDRSETUP_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_ADDRTIMING1 */
#define _EBI_ADDRTIMING1_ADDRHOLD_SHIFT              8                                         /**< Shift value for EBI_ADDRHOLD */
#define _EBI_ADDRTIMING1_ADDRHOLD_MASK               0x700UL                                   /**< Bit mask for EBI_ADDRHOLD */
#define _EBI_ADDRTIMING1_ADDRHOLD_DEFAULT            0x00000007UL                              /**< Mode DEFAULT for EBI_ADDRTIMING1 */
#define EBI_ADDRTIMING1_ADDRHOLD_DEFAULT             (_EBI_ADDRTIMING1_ADDRHOLD_DEFAULT << 8)  /**< Shifted mode DEFAULT for EBI_ADDRTIMING1 */
#define EBI_ADDRTIMING1_HALFALE                      (0x1UL << 28)                             /**< Half Cycle ALE Strobe Duration Enable */
#define _EBI_ADDRTIMING1_HALFALE_SHIFT               28                                        /**< Shift value for EBI_HALFALE */
#define _EBI_ADDRTIMING1_HALFALE_MASK                0x10000000UL                              /**< Bit mask for EBI_HALFALE */
#define _EBI_ADDRTIMING1_HALFALE_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for EBI_ADDRTIMING1 */
#define EBI_ADDRTIMING1_HALFALE_DEFAULT              (_EBI_ADDRTIMING1_HALFALE_DEFAULT << 28)  /**< Shifted mode DEFAULT for EBI_ADDRTIMING1 */

/* Bit fields for EBI RDTIMING1 */
#define _EBI_RDTIMING1_RESETVALUE                    0x00077F07UL                            /**< Default value for EBI_RDTIMING1 */
#define _EBI_RDTIMING1_MASK                          0x70077F07UL                            /**< Mask for EBI_RDTIMING1 */
#define _EBI_RDTIMING1_RDSETUP_SHIFT                 0                                       /**< Shift value for EBI_RDSETUP */
#define _EBI_RDTIMING1_RDSETUP_MASK                  0x7UL                                   /**< Bit mask for EBI_RDSETUP */
#define _EBI_RDTIMING1_RDSETUP_DEFAULT               0x00000007UL                            /**< Mode DEFAULT for EBI_RDTIMING1 */
#define EBI_RDTIMING1_RDSETUP_DEFAULT                (_EBI_RDTIMING1_RDSETUP_DEFAULT << 0)   /**< Shifted mode DEFAULT for EBI_RDTIMING1 */
#define _EBI_RDTIMING1_RDSTRB_SHIFT                  8                                       /**< Shift value for EBI_RDSTRB */
#define _EBI_RDTIMING1_RDSTRB_MASK                   0x7F00UL                                /**< Bit mask for EBI_RDSTRB */
#define _EBI_RDTIMING1_RDSTRB_DEFAULT                0x0000007FUL                            /**< Mode DEFAULT for EBI_RDTIMING1 */
#define EBI_RDTIMING1_RDSTRB_DEFAULT                 (_EBI_RDTIMING1_RDSTRB_DEFAULT << 8)    /**< Shifted mode DEFAULT for EBI_RDTIMING1 */
#define _EBI_RDTIMING1_RDHOLD_SHIFT                  16                                      /**< Shift value for EBI_RDHOLD */
#define _EBI_RDTIMING1_RDHOLD_MASK                   0x70000UL                               /**< Bit mask for EBI_RDHOLD */
#define _EBI_RDTIMING1_RDHOLD_DEFAULT                0x00000007UL                            /**< Mode DEFAULT for EBI_RDTIMING1 */
#define EBI_RDTIMING1_RDHOLD_DEFAULT                 (_EBI_RDTIMING1_RDHOLD_DEFAULT << 16)   /**< Shifted mode DEFAULT for EBI_RDTIMING1 */
#define EBI_RDTIMING1_HALFRE                         (0x1UL << 28)                           /**< Half Cycle REn Strobe Duration Enable */
#define _EBI_RDTIMING1_HALFRE_SHIFT                  28                                      /**< Shift value for EBI_HALFRE */
#define _EBI_RDTIMING1_HALFRE_MASK                   0x10000000UL                            /**< Bit mask for EBI_HALFRE */
#define _EBI_RDTIMING1_HALFRE_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for EBI_RDTIMING1 */
#define EBI_RDTIMING1_HALFRE_DEFAULT                 (_EBI_RDTIMING1_HALFRE_DEFAULT << 28)   /**< Shifted mode DEFAULT for EBI_RDTIMING1 */
#define EBI_RDTIMING1_PREFETCH                       (0x1UL << 29)                           /**< Prefetch Enable */
#define _EBI_RDTIMING1_PREFETCH_SHIFT                29                                      /**< Shift value for EBI_PREFETCH */
#define _EBI_RDTIMING1_PREFETCH_MASK                 0x20000000UL                            /**< Bit mask for EBI_PREFETCH */
#define _EBI_RDTIMING1_PREFETCH_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EBI_RDTIMING1 */
#define EBI_RDTIMING1_PREFETCH_DEFAULT               (_EBI_RDTIMING1_PREFETCH_DEFAULT << 29) /**< Shifted mode DEFAULT for EBI_RDTIMING1 */
#define EBI_RDTIMING1_PAGEMODE                       (0x1UL << 30)                           /**< Page Mode Access Enable */
#define _EBI_RDTIMING1_PAGEMODE_SHIFT                30                                      /**< Shift value for EBI_PAGEMODE */
#define _EBI_RDTIMING1_PAGEMODE_MASK                 0x40000000UL                            /**< Bit mask for EBI_PAGEMODE */
#define _EBI_RDTIMING1_PAGEMODE_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EBI_RDTIMING1 */
#define EBI_RDTIMING1_PAGEMODE_DEFAULT               (_EBI_RDTIMING1_PAGEMODE_DEFAULT << 30) /**< Shifted mode DEFAULT for EBI_RDTIMING1 */

/* Bit fields for EBI WRTIMING1 */
#define _EBI_WRTIMING1_RESETVALUE                    0x00077F07UL                           /**< Default value for EBI_WRTIMING1 */
#define _EBI_WRTIMING1_MASK                          0x30077F07UL                           /**< Mask for EBI_WRTIMING1 */
#define _EBI_WRTIMING1_WRSETUP_SHIFT                 0                                      /**< Shift value for EBI_WRSETUP */
#define _EBI_WRTIMING1_WRSETUP_MASK                  0x7UL                                  /**< Bit mask for EBI_WRSETUP */
#define _EBI_WRTIMING1_WRSETUP_DEFAULT               0x00000007UL                           /**< Mode DEFAULT for EBI_WRTIMING1 */
#define EBI_WRTIMING1_WRSETUP_DEFAULT                (_EBI_WRTIMING1_WRSETUP_DEFAULT << 0)  /**< Shifted mode DEFAULT for EBI_WRTIMING1 */
#define _EBI_WRTIMING1_WRSTRB_SHIFT                  8                                      /**< Shift value for EBI_WRSTRB */
#define _EBI_WRTIMING1_WRSTRB_MASK                   0x7F00UL                               /**< Bit mask for EBI_WRSTRB */
#define _EBI_WRTIMING1_WRSTRB_DEFAULT                0x0000007FUL                           /**< Mode DEFAULT for EBI_WRTIMING1 */
#define EBI_WRTIMING1_WRSTRB_DEFAULT                 (_EBI_WRTIMING1_WRSTRB_DEFAULT << 8)   /**< Shifted mode DEFAULT for EBI_WRTIMING1 */
#define _EBI_WRTIMING1_WRHOLD_SHIFT                  16                                     /**< Shift value for EBI_WRHOLD */
#define _EBI_WRTIMING1_WRHOLD_MASK                   0x70000UL                              /**< Bit mask for EBI_WRHOLD */
#define _EBI_WRTIMING1_WRHOLD_DEFAULT                0x00000007UL                           /**< Mode DEFAULT for EBI_WRTIMING1 */
#define EBI_WRTIMING1_WRHOLD_DEFAULT                 (_EBI_WRTIMING1_WRHOLD_DEFAULT << 16)  /**< Shifted mode DEFAULT for EBI_WRTIMING1 */
#define EBI_WRTIMING1_HALFWE                         (0x1UL << 28)                          /**< Half Cycle WEn Strobe Duration Enable */
#define _EBI_WRTIMING1_HALFWE_SHIFT                  28                                     /**< Shift value for EBI_HALFWE */
#define _EBI_WRTIMING1_HALFWE_MASK                   0x10000000UL                           /**< Bit mask for EBI_HALFWE */
#define _EBI_WRTIMING1_HALFWE_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for EBI_WRTIMING1 */
#define EBI_WRTIMING1_HALFWE_DEFAULT                 (_EBI_WRTIMING1_HALFWE_DEFAULT << 28)  /**< Shifted mode DEFAULT for EBI_WRTIMING1 */
#define EBI_WRTIMING1_WBUFDIS                        (0x1UL << 29)                          /**< Write Buffer Disable */
#define _EBI_WRTIMING1_WBUFDIS_SHIFT                 29                                     /**< Shift value for EBI_WBUFDIS */
#define _EBI_WRTIMING1_WBUFDIS_MASK                  0x20000000UL                           /**< Bit mask for EBI_WBUFDIS */
#define _EBI_WRTIMING1_WBUFDIS_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for EBI_WRTIMING1 */
#define EBI_WRTIMING1_WBUFDIS_DEFAULT                (_EBI_WRTIMING1_WBUFDIS_DEFAULT << 29) /**< Shifted mode DEFAULT for EBI_WRTIMING1 */

/* Bit fields for EBI POLARITY1 */
#define _EBI_POLARITY1_RESETVALUE                    0x00000000UL                             /**< Default value for EBI_POLARITY1 */
#define _EBI_POLARITY1_MASK                          0x0000003FUL                             /**< Mask for EBI_POLARITY1 */
#define EBI_POLARITY1_CSPOL                          (0x1UL << 0)                             /**< Chip Select Polarity */
#define _EBI_POLARITY1_CSPOL_SHIFT                   0                                        /**< Shift value for EBI_CSPOL */
#define _EBI_POLARITY1_CSPOL_MASK                    0x1UL                                    /**< Bit mask for EBI_CSPOL */
#define _EBI_POLARITY1_CSPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY1 */
#define _EBI_POLARITY1_CSPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY1 */
#define _EBI_POLARITY1_CSPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY1 */
#define EBI_POLARITY1_CSPOL_DEFAULT                  (_EBI_POLARITY1_CSPOL_DEFAULT << 0)      /**< Shifted mode DEFAULT for EBI_POLARITY1 */
#define EBI_POLARITY1_CSPOL_ACTIVELOW                (_EBI_POLARITY1_CSPOL_ACTIVELOW << 0)    /**< Shifted mode ACTIVELOW for EBI_POLARITY1 */
#define EBI_POLARITY1_CSPOL_ACTIVEHIGH               (_EBI_POLARITY1_CSPOL_ACTIVEHIGH << 0)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY1 */
#define EBI_POLARITY1_REPOL                          (0x1UL << 1)                             /**< Read Enable Polarity */
#define _EBI_POLARITY1_REPOL_SHIFT                   1                                        /**< Shift value for EBI_REPOL */
#define _EBI_POLARITY1_REPOL_MASK                    0x2UL                                    /**< Bit mask for EBI_REPOL */
#define _EBI_POLARITY1_REPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY1 */
#define _EBI_POLARITY1_REPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY1 */
#define _EBI_POLARITY1_REPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY1 */
#define EBI_POLARITY1_REPOL_DEFAULT                  (_EBI_POLARITY1_REPOL_DEFAULT << 1)      /**< Shifted mode DEFAULT for EBI_POLARITY1 */
#define EBI_POLARITY1_REPOL_ACTIVELOW                (_EBI_POLARITY1_REPOL_ACTIVELOW << 1)    /**< Shifted mode ACTIVELOW for EBI_POLARITY1 */
#define EBI_POLARITY1_REPOL_ACTIVEHIGH               (_EBI_POLARITY1_REPOL_ACTIVEHIGH << 1)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY1 */
#define EBI_POLARITY1_WEPOL                          (0x1UL << 2)                             /**< Write Enable Polarity */
#define _EBI_POLARITY1_WEPOL_SHIFT                   2                                        /**< Shift value for EBI_WEPOL */
#define _EBI_POLARITY1_WEPOL_MASK                    0x4UL                                    /**< Bit mask for EBI_WEPOL */
#define _EBI_POLARITY1_WEPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY1 */
#define _EBI_POLARITY1_WEPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY1 */
#define _EBI_POLARITY1_WEPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY1 */
#define EBI_POLARITY1_WEPOL_DEFAULT                  (_EBI_POLARITY1_WEPOL_DEFAULT << 2)      /**< Shifted mode DEFAULT for EBI_POLARITY1 */
#define EBI_POLARITY1_WEPOL_ACTIVELOW                (_EBI_POLARITY1_WEPOL_ACTIVELOW << 2)    /**< Shifted mode ACTIVELOW for EBI_POLARITY1 */
#define EBI_POLARITY1_WEPOL_ACTIVEHIGH               (_EBI_POLARITY1_WEPOL_ACTIVEHIGH << 2)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY1 */
#define EBI_POLARITY1_ALEPOL                         (0x1UL << 3)                             /**< Address Latch Polarity */
#define _EBI_POLARITY1_ALEPOL_SHIFT                  3                                        /**< Shift value for EBI_ALEPOL */
#define _EBI_POLARITY1_ALEPOL_MASK                   0x8UL                                    /**< Bit mask for EBI_ALEPOL */
#define _EBI_POLARITY1_ALEPOL_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY1 */
#define _EBI_POLARITY1_ALEPOL_ACTIVELOW              0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY1 */
#define _EBI_POLARITY1_ALEPOL_ACTIVEHIGH             0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY1 */
#define EBI_POLARITY1_ALEPOL_DEFAULT                 (_EBI_POLARITY1_ALEPOL_DEFAULT << 3)     /**< Shifted mode DEFAULT for EBI_POLARITY1 */
#define EBI_POLARITY1_ALEPOL_ACTIVELOW               (_EBI_POLARITY1_ALEPOL_ACTIVELOW << 3)   /**< Shifted mode ACTIVELOW for EBI_POLARITY1 */
#define EBI_POLARITY1_ALEPOL_ACTIVEHIGH              (_EBI_POLARITY1_ALEPOL_ACTIVEHIGH << 3)  /**< Shifted mode ACTIVEHIGH for EBI_POLARITY1 */
#define EBI_POLARITY1_ARDYPOL                        (0x1UL << 4)                             /**< ARDY Polarity */
#define _EBI_POLARITY1_ARDYPOL_SHIFT                 4                                        /**< Shift value for EBI_ARDYPOL */
#define _EBI_POLARITY1_ARDYPOL_MASK                  0x10UL                                   /**< Bit mask for EBI_ARDYPOL */
#define _EBI_POLARITY1_ARDYPOL_DEFAULT               0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY1 */
#define _EBI_POLARITY1_ARDYPOL_ACTIVELOW             0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY1 */
#define _EBI_POLARITY1_ARDYPOL_ACTIVEHIGH            0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY1 */
#define EBI_POLARITY1_ARDYPOL_DEFAULT                (_EBI_POLARITY1_ARDYPOL_DEFAULT << 4)    /**< Shifted mode DEFAULT for EBI_POLARITY1 */
#define EBI_POLARITY1_ARDYPOL_ACTIVELOW              (_EBI_POLARITY1_ARDYPOL_ACTIVELOW << 4)  /**< Shifted mode ACTIVELOW for EBI_POLARITY1 */
#define EBI_POLARITY1_ARDYPOL_ACTIVEHIGH             (_EBI_POLARITY1_ARDYPOL_ACTIVEHIGH << 4) /**< Shifted mode ACTIVEHIGH for EBI_POLARITY1 */
#define EBI_POLARITY1_BLPOL                          (0x1UL << 5)                             /**< BL Polarity */
#define _EBI_POLARITY1_BLPOL_SHIFT                   5                                        /**< Shift value for EBI_BLPOL */
#define _EBI_POLARITY1_BLPOL_MASK                    0x20UL                                   /**< Bit mask for EBI_BLPOL */
#define _EBI_POLARITY1_BLPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY1 */
#define _EBI_POLARITY1_BLPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY1 */
#define _EBI_POLARITY1_BLPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY1 */
#define EBI_POLARITY1_BLPOL_DEFAULT                  (_EBI_POLARITY1_BLPOL_DEFAULT << 5)      /**< Shifted mode DEFAULT for EBI_POLARITY1 */
#define EBI_POLARITY1_BLPOL_ACTIVELOW                (_EBI_POLARITY1_BLPOL_ACTIVELOW << 5)    /**< Shifted mode ACTIVELOW for EBI_POLARITY1 */
#define EBI_POLARITY1_BLPOL_ACTIVEHIGH               (_EBI_POLARITY1_BLPOL_ACTIVEHIGH << 5)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY1 */

/* Bit fields for EBI ADDRTIMING2 */
#define _EBI_ADDRTIMING2_RESETVALUE                  0x00000707UL                              /**< Default value for EBI_ADDRTIMING2 */
#define _EBI_ADDRTIMING2_MASK                        0x10000707UL                              /**< Mask for EBI_ADDRTIMING2 */
#define _EBI_ADDRTIMING2_ADDRSETUP_SHIFT             0                                         /**< Shift value for EBI_ADDRSETUP */
#define _EBI_ADDRTIMING2_ADDRSETUP_MASK              0x7UL                                     /**< Bit mask for EBI_ADDRSETUP */
#define _EBI_ADDRTIMING2_ADDRSETUP_DEFAULT           0x00000007UL                              /**< Mode DEFAULT for EBI_ADDRTIMING2 */
#define EBI_ADDRTIMING2_ADDRSETUP_DEFAULT            (_EBI_ADDRTIMING2_ADDRSETUP_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_ADDRTIMING2 */
#define _EBI_ADDRTIMING2_ADDRHOLD_SHIFT              8                                         /**< Shift value for EBI_ADDRHOLD */
#define _EBI_ADDRTIMING2_ADDRHOLD_MASK               0x700UL                                   /**< Bit mask for EBI_ADDRHOLD */
#define _EBI_ADDRTIMING2_ADDRHOLD_DEFAULT            0x00000007UL                              /**< Mode DEFAULT for EBI_ADDRTIMING2 */
#define EBI_ADDRTIMING2_ADDRHOLD_DEFAULT             (_EBI_ADDRTIMING2_ADDRHOLD_DEFAULT << 8)  /**< Shifted mode DEFAULT for EBI_ADDRTIMING2 */
#define EBI_ADDRTIMING2_HALFALE                      (0x1UL << 28)                             /**< Half Cycle ALE Strobe Duration Enable */
#define _EBI_ADDRTIMING2_HALFALE_SHIFT               28                                        /**< Shift value for EBI_HALFALE */
#define _EBI_ADDRTIMING2_HALFALE_MASK                0x10000000UL                              /**< Bit mask for EBI_HALFALE */
#define _EBI_ADDRTIMING2_HALFALE_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for EBI_ADDRTIMING2 */
#define EBI_ADDRTIMING2_HALFALE_DEFAULT              (_EBI_ADDRTIMING2_HALFALE_DEFAULT << 28)  /**< Shifted mode DEFAULT for EBI_ADDRTIMING2 */

/* Bit fields for EBI RDTIMING2 */
#define _EBI_RDTIMING2_RESETVALUE                    0x00077F07UL                            /**< Default value for EBI_RDTIMING2 */
#define _EBI_RDTIMING2_MASK                          0x70077F07UL                            /**< Mask for EBI_RDTIMING2 */
#define _EBI_RDTIMING2_RDSETUP_SHIFT                 0                                       /**< Shift value for EBI_RDSETUP */
#define _EBI_RDTIMING2_RDSETUP_MASK                  0x7UL                                   /**< Bit mask for EBI_RDSETUP */
#define _EBI_RDTIMING2_RDSETUP_DEFAULT               0x00000007UL                            /**< Mode DEFAULT for EBI_RDTIMING2 */
#define EBI_RDTIMING2_RDSETUP_DEFAULT                (_EBI_RDTIMING2_RDSETUP_DEFAULT << 0)   /**< Shifted mode DEFAULT for EBI_RDTIMING2 */
#define _EBI_RDTIMING2_RDSTRB_SHIFT                  8                                       /**< Shift value for EBI_RDSTRB */
#define _EBI_RDTIMING2_RDSTRB_MASK                   0x7F00UL                                /**< Bit mask for EBI_RDSTRB */
#define _EBI_RDTIMING2_RDSTRB_DEFAULT                0x0000007FUL                            /**< Mode DEFAULT for EBI_RDTIMING2 */
#define EBI_RDTIMING2_RDSTRB_DEFAULT                 (_EBI_RDTIMING2_RDSTRB_DEFAULT << 8)    /**< Shifted mode DEFAULT for EBI_RDTIMING2 */
#define _EBI_RDTIMING2_RDHOLD_SHIFT                  16                                      /**< Shift value for EBI_RDHOLD */
#define _EBI_RDTIMING2_RDHOLD_MASK                   0x70000UL                               /**< Bit mask for EBI_RDHOLD */
#define _EBI_RDTIMING2_RDHOLD_DEFAULT                0x00000007UL                            /**< Mode DEFAULT for EBI_RDTIMING2 */
#define EBI_RDTIMING2_RDHOLD_DEFAULT                 (_EBI_RDTIMING2_RDHOLD_DEFAULT << 16)   /**< Shifted mode DEFAULT for EBI_RDTIMING2 */
#define EBI_RDTIMING2_HALFRE                         (0x1UL << 28)                           /**< Half Cycle REn Strobe Duration Enable */
#define _EBI_RDTIMING2_HALFRE_SHIFT                  28                                      /**< Shift value for EBI_HALFRE */
#define _EBI_RDTIMING2_HALFRE_MASK                   0x10000000UL                            /**< Bit mask for EBI_HALFRE */
#define _EBI_RDTIMING2_HALFRE_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for EBI_RDTIMING2 */
#define EBI_RDTIMING2_HALFRE_DEFAULT                 (_EBI_RDTIMING2_HALFRE_DEFAULT << 28)   /**< Shifted mode DEFAULT for EBI_RDTIMING2 */
#define EBI_RDTIMING2_PREFETCH                       (0x1UL << 29)                           /**< Prefetch Enable */
#define _EBI_RDTIMING2_PREFETCH_SHIFT                29                                      /**< Shift value for EBI_PREFETCH */
#define _EBI_RDTIMING2_PREFETCH_MASK                 0x20000000UL                            /**< Bit mask for EBI_PREFETCH */
#define _EBI_RDTIMING2_PREFETCH_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EBI_RDTIMING2 */
#define EBI_RDTIMING2_PREFETCH_DEFAULT               (_EBI_RDTIMING2_PREFETCH_DEFAULT << 29) /**< Shifted mode DEFAULT for EBI_RDTIMING2 */
#define EBI_RDTIMING2_PAGEMODE                       (0x1UL << 30)                           /**< Page Mode Access Enable */
#define _EBI_RDTIMING2_PAGEMODE_SHIFT                30                                      /**< Shift value for EBI_PAGEMODE */
#define _EBI_RDTIMING2_PAGEMODE_MASK                 0x40000000UL                            /**< Bit mask for EBI_PAGEMODE */
#define _EBI_RDTIMING2_PAGEMODE_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EBI_RDTIMING2 */
#define EBI_RDTIMING2_PAGEMODE_DEFAULT               (_EBI_RDTIMING2_PAGEMODE_DEFAULT << 30) /**< Shifted mode DEFAULT for EBI_RDTIMING2 */

/* Bit fields for EBI WRTIMING2 */
#define _EBI_WRTIMING2_RESETVALUE                    0x00077F07UL                           /**< Default value for EBI_WRTIMING2 */
#define _EBI_WRTIMING2_MASK                          0x30077F07UL                           /**< Mask for EBI_WRTIMING2 */
#define _EBI_WRTIMING2_WRSETUP_SHIFT                 0                                      /**< Shift value for EBI_WRSETUP */
#define _EBI_WRTIMING2_WRSETUP_MASK                  0x7UL                                  /**< Bit mask for EBI_WRSETUP */
#define _EBI_WRTIMING2_WRSETUP_DEFAULT               0x00000007UL                           /**< Mode DEFAULT for EBI_WRTIMING2 */
#define EBI_WRTIMING2_WRSETUP_DEFAULT                (_EBI_WRTIMING2_WRSETUP_DEFAULT << 0)  /**< Shifted mode DEFAULT for EBI_WRTIMING2 */
#define _EBI_WRTIMING2_WRSTRB_SHIFT                  8                                      /**< Shift value for EBI_WRSTRB */
#define _EBI_WRTIMING2_WRSTRB_MASK                   0x7F00UL                               /**< Bit mask for EBI_WRSTRB */
#define _EBI_WRTIMING2_WRSTRB_DEFAULT                0x0000007FUL                           /**< Mode DEFAULT for EBI_WRTIMING2 */
#define EBI_WRTIMING2_WRSTRB_DEFAULT                 (_EBI_WRTIMING2_WRSTRB_DEFAULT << 8)   /**< Shifted mode DEFAULT for EBI_WRTIMING2 */
#define _EBI_WRTIMING2_WRHOLD_SHIFT                  16                                     /**< Shift value for EBI_WRHOLD */
#define _EBI_WRTIMING2_WRHOLD_MASK                   0x70000UL                              /**< Bit mask for EBI_WRHOLD */
#define _EBI_WRTIMING2_WRHOLD_DEFAULT                0x00000007UL                           /**< Mode DEFAULT for EBI_WRTIMING2 */
#define EBI_WRTIMING2_WRHOLD_DEFAULT                 (_EBI_WRTIMING2_WRHOLD_DEFAULT << 16)  /**< Shifted mode DEFAULT for EBI_WRTIMING2 */
#define EBI_WRTIMING2_HALFWE                         (0x1UL << 28)                          /**< Half Cycle WEn Strobe Duration Enable */
#define _EBI_WRTIMING2_HALFWE_SHIFT                  28                                     /**< Shift value for EBI_HALFWE */
#define _EBI_WRTIMING2_HALFWE_MASK                   0x10000000UL                           /**< Bit mask for EBI_HALFWE */
#define _EBI_WRTIMING2_HALFWE_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for EBI_WRTIMING2 */
#define EBI_WRTIMING2_HALFWE_DEFAULT                 (_EBI_WRTIMING2_HALFWE_DEFAULT << 28)  /**< Shifted mode DEFAULT for EBI_WRTIMING2 */
#define EBI_WRTIMING2_WBUFDIS                        (0x1UL << 29)                          /**< Write Buffer Disable */
#define _EBI_WRTIMING2_WBUFDIS_SHIFT                 29                                     /**< Shift value for EBI_WBUFDIS */
#define _EBI_WRTIMING2_WBUFDIS_MASK                  0x20000000UL                           /**< Bit mask for EBI_WBUFDIS */
#define _EBI_WRTIMING2_WBUFDIS_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for EBI_WRTIMING2 */
#define EBI_WRTIMING2_WBUFDIS_DEFAULT                (_EBI_WRTIMING2_WBUFDIS_DEFAULT << 29) /**< Shifted mode DEFAULT for EBI_WRTIMING2 */

/* Bit fields for EBI POLARITY2 */
#define _EBI_POLARITY2_RESETVALUE                    0x00000000UL                             /**< Default value for EBI_POLARITY2 */
#define _EBI_POLARITY2_MASK                          0x0000003FUL                             /**< Mask for EBI_POLARITY2 */
#define EBI_POLARITY2_CSPOL                          (0x1UL << 0)                             /**< Chip Select Polarity */
#define _EBI_POLARITY2_CSPOL_SHIFT                   0                                        /**< Shift value for EBI_CSPOL */
#define _EBI_POLARITY2_CSPOL_MASK                    0x1UL                                    /**< Bit mask for EBI_CSPOL */
#define _EBI_POLARITY2_CSPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY2 */
#define _EBI_POLARITY2_CSPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY2 */
#define _EBI_POLARITY2_CSPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY2 */
#define EBI_POLARITY2_CSPOL_DEFAULT                  (_EBI_POLARITY2_CSPOL_DEFAULT << 0)      /**< Shifted mode DEFAULT for EBI_POLARITY2 */
#define EBI_POLARITY2_CSPOL_ACTIVELOW                (_EBI_POLARITY2_CSPOL_ACTIVELOW << 0)    /**< Shifted mode ACTIVELOW for EBI_POLARITY2 */
#define EBI_POLARITY2_CSPOL_ACTIVEHIGH               (_EBI_POLARITY2_CSPOL_ACTIVEHIGH << 0)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY2 */
#define EBI_POLARITY2_REPOL                          (0x1UL << 1)                             /**< Read Enable Polarity */
#define _EBI_POLARITY2_REPOL_SHIFT                   1                                        /**< Shift value for EBI_REPOL */
#define _EBI_POLARITY2_REPOL_MASK                    0x2UL                                    /**< Bit mask for EBI_REPOL */
#define _EBI_POLARITY2_REPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY2 */
#define _EBI_POLARITY2_REPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY2 */
#define _EBI_POLARITY2_REPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY2 */
#define EBI_POLARITY2_REPOL_DEFAULT                  (_EBI_POLARITY2_REPOL_DEFAULT << 1)      /**< Shifted mode DEFAULT for EBI_POLARITY2 */
#define EBI_POLARITY2_REPOL_ACTIVELOW                (_EBI_POLARITY2_REPOL_ACTIVELOW << 1)    /**< Shifted mode ACTIVELOW for EBI_POLARITY2 */
#define EBI_POLARITY2_REPOL_ACTIVEHIGH               (_EBI_POLARITY2_REPOL_ACTIVEHIGH << 1)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY2 */
#define EBI_POLARITY2_WEPOL                          (0x1UL << 2)                             /**< Write Enable Polarity */
#define _EBI_POLARITY2_WEPOL_SHIFT                   2                                        /**< Shift value for EBI_WEPOL */
#define _EBI_POLARITY2_WEPOL_MASK                    0x4UL                                    /**< Bit mask for EBI_WEPOL */
#define _EBI_POLARITY2_WEPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY2 */
#define _EBI_POLARITY2_WEPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY2 */
#define _EBI_POLARITY2_WEPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY2 */
#define EBI_POLARITY2_WEPOL_DEFAULT                  (_EBI_POLARITY2_WEPOL_DEFAULT << 2)      /**< Shifted mode DEFAULT for EBI_POLARITY2 */
#define EBI_POLARITY2_WEPOL_ACTIVELOW                (_EBI_POLARITY2_WEPOL_ACTIVELOW << 2)    /**< Shifted mode ACTIVELOW for EBI_POLARITY2 */
#define EBI_POLARITY2_WEPOL_ACTIVEHIGH               (_EBI_POLARITY2_WEPOL_ACTIVEHIGH << 2)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY2 */
#define EBI_POLARITY2_ALEPOL                         (0x1UL << 3)                             /**< Address Latch Polarity */
#define _EBI_POLARITY2_ALEPOL_SHIFT                  3                                        /**< Shift value for EBI_ALEPOL */
#define _EBI_POLARITY2_ALEPOL_MASK                   0x8UL                                    /**< Bit mask for EBI_ALEPOL */
#define _EBI_POLARITY2_ALEPOL_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY2 */
#define _EBI_POLARITY2_ALEPOL_ACTIVELOW              0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY2 */
#define _EBI_POLARITY2_ALEPOL_ACTIVEHIGH             0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY2 */
#define EBI_POLARITY2_ALEPOL_DEFAULT                 (_EBI_POLARITY2_ALEPOL_DEFAULT << 3)     /**< Shifted mode DEFAULT for EBI_POLARITY2 */
#define EBI_POLARITY2_ALEPOL_ACTIVELOW               (_EBI_POLARITY2_ALEPOL_ACTIVELOW << 3)   /**< Shifted mode ACTIVELOW for EBI_POLARITY2 */
#define EBI_POLARITY2_ALEPOL_ACTIVEHIGH              (_EBI_POLARITY2_ALEPOL_ACTIVEHIGH << 3)  /**< Shifted mode ACTIVEHIGH for EBI_POLARITY2 */
#define EBI_POLARITY2_ARDYPOL                        (0x1UL << 4)                             /**< ARDY Polarity */
#define _EBI_POLARITY2_ARDYPOL_SHIFT                 4                                        /**< Shift value for EBI_ARDYPOL */
#define _EBI_POLARITY2_ARDYPOL_MASK                  0x10UL                                   /**< Bit mask for EBI_ARDYPOL */
#define _EBI_POLARITY2_ARDYPOL_DEFAULT               0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY2 */
#define _EBI_POLARITY2_ARDYPOL_ACTIVELOW             0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY2 */
#define _EBI_POLARITY2_ARDYPOL_ACTIVEHIGH            0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY2 */
#define EBI_POLARITY2_ARDYPOL_DEFAULT                (_EBI_POLARITY2_ARDYPOL_DEFAULT << 4)    /**< Shifted mode DEFAULT for EBI_POLARITY2 */
#define EBI_POLARITY2_ARDYPOL_ACTIVELOW              (_EBI_POLARITY2_ARDYPOL_ACTIVELOW << 4)  /**< Shifted mode ACTIVELOW for EBI_POLARITY2 */
#define EBI_POLARITY2_ARDYPOL_ACTIVEHIGH             (_EBI_POLARITY2_ARDYPOL_ACTIVEHIGH << 4) /**< Shifted mode ACTIVEHIGH for EBI_POLARITY2 */
#define EBI_POLARITY2_BLPOL                          (0x1UL << 5)                             /**< BL Polarity */
#define _EBI_POLARITY2_BLPOL_SHIFT                   5                                        /**< Shift value for EBI_BLPOL */
#define _EBI_POLARITY2_BLPOL_MASK                    0x20UL                                   /**< Bit mask for EBI_BLPOL */
#define _EBI_POLARITY2_BLPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY2 */
#define _EBI_POLARITY2_BLPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY2 */
#define _EBI_POLARITY2_BLPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY2 */
#define EBI_POLARITY2_BLPOL_DEFAULT                  (_EBI_POLARITY2_BLPOL_DEFAULT << 5)      /**< Shifted mode DEFAULT for EBI_POLARITY2 */
#define EBI_POLARITY2_BLPOL_ACTIVELOW                (_EBI_POLARITY2_BLPOL_ACTIVELOW << 5)    /**< Shifted mode ACTIVELOW for EBI_POLARITY2 */
#define EBI_POLARITY2_BLPOL_ACTIVEHIGH               (_EBI_POLARITY2_BLPOL_ACTIVEHIGH << 5)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY2 */

/* Bit fields for EBI ADDRTIMING3 */
#define _EBI_ADDRTIMING3_RESETVALUE                  0x00000707UL                              /**< Default value for EBI_ADDRTIMING3 */
#define _EBI_ADDRTIMING3_MASK                        0x10000707UL                              /**< Mask for EBI_ADDRTIMING3 */
#define _EBI_ADDRTIMING3_ADDRSETUP_SHIFT             0                                         /**< Shift value for EBI_ADDRSETUP */
#define _EBI_ADDRTIMING3_ADDRSETUP_MASK              0x7UL                                     /**< Bit mask for EBI_ADDRSETUP */
#define _EBI_ADDRTIMING3_ADDRSETUP_DEFAULT           0x00000007UL                              /**< Mode DEFAULT for EBI_ADDRTIMING3 */
#define EBI_ADDRTIMING3_ADDRSETUP_DEFAULT            (_EBI_ADDRTIMING3_ADDRSETUP_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_ADDRTIMING3 */
#define _EBI_ADDRTIMING3_ADDRHOLD_SHIFT              8                                         /**< Shift value for EBI_ADDRHOLD */
#define _EBI_ADDRTIMING3_ADDRHOLD_MASK               0x700UL                                   /**< Bit mask for EBI_ADDRHOLD */
#define _EBI_ADDRTIMING3_ADDRHOLD_DEFAULT            0x00000007UL                              /**< Mode DEFAULT for EBI_ADDRTIMING3 */
#define EBI_ADDRTIMING3_ADDRHOLD_DEFAULT             (_EBI_ADDRTIMING3_ADDRHOLD_DEFAULT << 8)  /**< Shifted mode DEFAULT for EBI_ADDRTIMING3 */
#define EBI_ADDRTIMING3_HALFALE                      (0x1UL << 28)                             /**< Half Cycle ALE Strobe Duration Enable */
#define _EBI_ADDRTIMING3_HALFALE_SHIFT               28                                        /**< Shift value for EBI_HALFALE */
#define _EBI_ADDRTIMING3_HALFALE_MASK                0x10000000UL                              /**< Bit mask for EBI_HALFALE */
#define _EBI_ADDRTIMING3_HALFALE_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for EBI_ADDRTIMING3 */
#define EBI_ADDRTIMING3_HALFALE_DEFAULT              (_EBI_ADDRTIMING3_HALFALE_DEFAULT << 28)  /**< Shifted mode DEFAULT for EBI_ADDRTIMING3 */

/* Bit fields for EBI RDTIMING3 */
#define _EBI_RDTIMING3_RESETVALUE                    0x00077F07UL                            /**< Default value for EBI_RDTIMING3 */
#define _EBI_RDTIMING3_MASK                          0x70077F07UL                            /**< Mask for EBI_RDTIMING3 */
#define _EBI_RDTIMING3_RDSETUP_SHIFT                 0                                       /**< Shift value for EBI_RDSETUP */
#define _EBI_RDTIMING3_RDSETUP_MASK                  0x7UL                                   /**< Bit mask for EBI_RDSETUP */
#define _EBI_RDTIMING3_RDSETUP_DEFAULT               0x00000007UL                            /**< Mode DEFAULT for EBI_RDTIMING3 */
#define EBI_RDTIMING3_RDSETUP_DEFAULT                (_EBI_RDTIMING3_RDSETUP_DEFAULT << 0)   /**< Shifted mode DEFAULT for EBI_RDTIMING3 */
#define _EBI_RDTIMING3_RDSTRB_SHIFT                  8                                       /**< Shift value for EBI_RDSTRB */
#define _EBI_RDTIMING3_RDSTRB_MASK                   0x7F00UL                                /**< Bit mask for EBI_RDSTRB */
#define _EBI_RDTIMING3_RDSTRB_DEFAULT                0x0000007FUL                            /**< Mode DEFAULT for EBI_RDTIMING3 */
#define EBI_RDTIMING3_RDSTRB_DEFAULT                 (_EBI_RDTIMING3_RDSTRB_DEFAULT << 8)    /**< Shifted mode DEFAULT for EBI_RDTIMING3 */
#define _EBI_RDTIMING3_RDHOLD_SHIFT                  16                                      /**< Shift value for EBI_RDHOLD */
#define _EBI_RDTIMING3_RDHOLD_MASK                   0x70000UL                               /**< Bit mask for EBI_RDHOLD */
#define _EBI_RDTIMING3_RDHOLD_DEFAULT                0x00000007UL                            /**< Mode DEFAULT for EBI_RDTIMING3 */
#define EBI_RDTIMING3_RDHOLD_DEFAULT                 (_EBI_RDTIMING3_RDHOLD_DEFAULT << 16)   /**< Shifted mode DEFAULT for EBI_RDTIMING3 */
#define EBI_RDTIMING3_HALFRE                         (0x1UL << 28)                           /**< Half Cycle REn Strobe Duration Enable */
#define _EBI_RDTIMING3_HALFRE_SHIFT                  28                                      /**< Shift value for EBI_HALFRE */
#define _EBI_RDTIMING3_HALFRE_MASK                   0x10000000UL                            /**< Bit mask for EBI_HALFRE */
#define _EBI_RDTIMING3_HALFRE_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for EBI_RDTIMING3 */
#define EBI_RDTIMING3_HALFRE_DEFAULT                 (_EBI_RDTIMING3_HALFRE_DEFAULT << 28)   /**< Shifted mode DEFAULT for EBI_RDTIMING3 */
#define EBI_RDTIMING3_PREFETCH                       (0x1UL << 29)                           /**< Prefetch Enable */
#define _EBI_RDTIMING3_PREFETCH_SHIFT                29                                      /**< Shift value for EBI_PREFETCH */
#define _EBI_RDTIMING3_PREFETCH_MASK                 0x20000000UL                            /**< Bit mask for EBI_PREFETCH */
#define _EBI_RDTIMING3_PREFETCH_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EBI_RDTIMING3 */
#define EBI_RDTIMING3_PREFETCH_DEFAULT               (_EBI_RDTIMING3_PREFETCH_DEFAULT << 29) /**< Shifted mode DEFAULT for EBI_RDTIMING3 */
#define EBI_RDTIMING3_PAGEMODE                       (0x1UL << 30)                           /**< Page Mode Access Enable */
#define _EBI_RDTIMING3_PAGEMODE_SHIFT                30                                      /**< Shift value for EBI_PAGEMODE */
#define _EBI_RDTIMING3_PAGEMODE_MASK                 0x40000000UL                            /**< Bit mask for EBI_PAGEMODE */
#define _EBI_RDTIMING3_PAGEMODE_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EBI_RDTIMING3 */
#define EBI_RDTIMING3_PAGEMODE_DEFAULT               (_EBI_RDTIMING3_PAGEMODE_DEFAULT << 30) /**< Shifted mode DEFAULT for EBI_RDTIMING3 */

/* Bit fields for EBI WRTIMING3 */
#define _EBI_WRTIMING3_RESETVALUE                    0x00077F07UL                           /**< Default value for EBI_WRTIMING3 */
#define _EBI_WRTIMING3_MASK                          0x30077F07UL                           /**< Mask for EBI_WRTIMING3 */
#define _EBI_WRTIMING3_WRSETUP_SHIFT                 0                                      /**< Shift value for EBI_WRSETUP */
#define _EBI_WRTIMING3_WRSETUP_MASK                  0x7UL                                  /**< Bit mask for EBI_WRSETUP */
#define _EBI_WRTIMING3_WRSETUP_DEFAULT               0x00000007UL                           /**< Mode DEFAULT for EBI_WRTIMING3 */
#define EBI_WRTIMING3_WRSETUP_DEFAULT                (_EBI_WRTIMING3_WRSETUP_DEFAULT << 0)  /**< Shifted mode DEFAULT for EBI_WRTIMING3 */
#define _EBI_WRTIMING3_WRSTRB_SHIFT                  8                                      /**< Shift value for EBI_WRSTRB */
#define _EBI_WRTIMING3_WRSTRB_MASK                   0x7F00UL                               /**< Bit mask for EBI_WRSTRB */
#define _EBI_WRTIMING3_WRSTRB_DEFAULT                0x0000007FUL                           /**< Mode DEFAULT for EBI_WRTIMING3 */
#define EBI_WRTIMING3_WRSTRB_DEFAULT                 (_EBI_WRTIMING3_WRSTRB_DEFAULT << 8)   /**< Shifted mode DEFAULT for EBI_WRTIMING3 */
#define _EBI_WRTIMING3_WRHOLD_SHIFT                  16                                     /**< Shift value for EBI_WRHOLD */
#define _EBI_WRTIMING3_WRHOLD_MASK                   0x70000UL                              /**< Bit mask for EBI_WRHOLD */
#define _EBI_WRTIMING3_WRHOLD_DEFAULT                0x00000007UL                           /**< Mode DEFAULT for EBI_WRTIMING3 */
#define EBI_WRTIMING3_WRHOLD_DEFAULT                 (_EBI_WRTIMING3_WRHOLD_DEFAULT << 16)  /**< Shifted mode DEFAULT for EBI_WRTIMING3 */
#define EBI_WRTIMING3_HALFWE                         (0x1UL << 28)                          /**< Half Cycle WEn Strobe Duration Enable */
#define _EBI_WRTIMING3_HALFWE_SHIFT                  28                                     /**< Shift value for EBI_HALFWE */
#define _EBI_WRTIMING3_HALFWE_MASK                   0x10000000UL                           /**< Bit mask for EBI_HALFWE */
#define _EBI_WRTIMING3_HALFWE_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for EBI_WRTIMING3 */
#define EBI_WRTIMING3_HALFWE_DEFAULT                 (_EBI_WRTIMING3_HALFWE_DEFAULT << 28)  /**< Shifted mode DEFAULT for EBI_WRTIMING3 */
#define EBI_WRTIMING3_WBUFDIS                        (0x1UL << 29)                          /**< Write Buffer Disable */
#define _EBI_WRTIMING3_WBUFDIS_SHIFT                 29                                     /**< Shift value for EBI_WBUFDIS */
#define _EBI_WRTIMING3_WBUFDIS_MASK                  0x20000000UL                           /**< Bit mask for EBI_WBUFDIS */
#define _EBI_WRTIMING3_WBUFDIS_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for EBI_WRTIMING3 */
#define EBI_WRTIMING3_WBUFDIS_DEFAULT                (_EBI_WRTIMING3_WBUFDIS_DEFAULT << 29) /**< Shifted mode DEFAULT for EBI_WRTIMING3 */

/* Bit fields for EBI POLARITY3 */
#define _EBI_POLARITY3_RESETVALUE                    0x00000000UL                             /**< Default value for EBI_POLARITY3 */
#define _EBI_POLARITY3_MASK                          0x0000003FUL                             /**< Mask for EBI_POLARITY3 */
#define EBI_POLARITY3_CSPOL                          (0x1UL << 0)                             /**< Chip Select Polarity */
#define _EBI_POLARITY3_CSPOL_SHIFT                   0                                        /**< Shift value for EBI_CSPOL */
#define _EBI_POLARITY3_CSPOL_MASK                    0x1UL                                    /**< Bit mask for EBI_CSPOL */
#define _EBI_POLARITY3_CSPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY3 */
#define _EBI_POLARITY3_CSPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY3 */
#define _EBI_POLARITY3_CSPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY3 */
#define EBI_POLARITY3_CSPOL_DEFAULT                  (_EBI_POLARITY3_CSPOL_DEFAULT << 0)      /**< Shifted mode DEFAULT for EBI_POLARITY3 */
#define EBI_POLARITY3_CSPOL_ACTIVELOW                (_EBI_POLARITY3_CSPOL_ACTIVELOW << 0)    /**< Shifted mode ACTIVELOW for EBI_POLARITY3 */
#define EBI_POLARITY3_CSPOL_ACTIVEHIGH               (_EBI_POLARITY3_CSPOL_ACTIVEHIGH << 0)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY3 */
#define EBI_POLARITY3_REPOL                          (0x1UL << 1)                             /**< Read Enable Polarity */
#define _EBI_POLARITY3_REPOL_SHIFT                   1                                        /**< Shift value for EBI_REPOL */
#define _EBI_POLARITY3_REPOL_MASK                    0x2UL                                    /**< Bit mask for EBI_REPOL */
#define _EBI_POLARITY3_REPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY3 */
#define _EBI_POLARITY3_REPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY3 */
#define _EBI_POLARITY3_REPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY3 */
#define EBI_POLARITY3_REPOL_DEFAULT                  (_EBI_POLARITY3_REPOL_DEFAULT << 1)      /**< Shifted mode DEFAULT for EBI_POLARITY3 */
#define EBI_POLARITY3_REPOL_ACTIVELOW                (_EBI_POLARITY3_REPOL_ACTIVELOW << 1)    /**< Shifted mode ACTIVELOW for EBI_POLARITY3 */
#define EBI_POLARITY3_REPOL_ACTIVEHIGH               (_EBI_POLARITY3_REPOL_ACTIVEHIGH << 1)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY3 */
#define EBI_POLARITY3_WEPOL                          (0x1UL << 2)                             /**< Write Enable Polarity */
#define _EBI_POLARITY3_WEPOL_SHIFT                   2                                        /**< Shift value for EBI_WEPOL */
#define _EBI_POLARITY3_WEPOL_MASK                    0x4UL                                    /**< Bit mask for EBI_WEPOL */
#define _EBI_POLARITY3_WEPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY3 */
#define _EBI_POLARITY3_WEPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY3 */
#define _EBI_POLARITY3_WEPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY3 */
#define EBI_POLARITY3_WEPOL_DEFAULT                  (_EBI_POLARITY3_WEPOL_DEFAULT << 2)      /**< Shifted mode DEFAULT for EBI_POLARITY3 */
#define EBI_POLARITY3_WEPOL_ACTIVELOW                (_EBI_POLARITY3_WEPOL_ACTIVELOW << 2)    /**< Shifted mode ACTIVELOW for EBI_POLARITY3 */
#define EBI_POLARITY3_WEPOL_ACTIVEHIGH               (_EBI_POLARITY3_WEPOL_ACTIVEHIGH << 2)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY3 */
#define EBI_POLARITY3_ALEPOL                         (0x1UL << 3)                             /**< Address Latch Polarity */
#define _EBI_POLARITY3_ALEPOL_SHIFT                  3                                        /**< Shift value for EBI_ALEPOL */
#define _EBI_POLARITY3_ALEPOL_MASK                   0x8UL                                    /**< Bit mask for EBI_ALEPOL */
#define _EBI_POLARITY3_ALEPOL_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY3 */
#define _EBI_POLARITY3_ALEPOL_ACTIVELOW              0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY3 */
#define _EBI_POLARITY3_ALEPOL_ACTIVEHIGH             0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY3 */
#define EBI_POLARITY3_ALEPOL_DEFAULT                 (_EBI_POLARITY3_ALEPOL_DEFAULT << 3)     /**< Shifted mode DEFAULT for EBI_POLARITY3 */
#define EBI_POLARITY3_ALEPOL_ACTIVELOW               (_EBI_POLARITY3_ALEPOL_ACTIVELOW << 3)   /**< Shifted mode ACTIVELOW for EBI_POLARITY3 */
#define EBI_POLARITY3_ALEPOL_ACTIVEHIGH              (_EBI_POLARITY3_ALEPOL_ACTIVEHIGH << 3)  /**< Shifted mode ACTIVEHIGH for EBI_POLARITY3 */
#define EBI_POLARITY3_ARDYPOL                        (0x1UL << 4)                             /**< ARDY Polarity */
#define _EBI_POLARITY3_ARDYPOL_SHIFT                 4                                        /**< Shift value for EBI_ARDYPOL */
#define _EBI_POLARITY3_ARDYPOL_MASK                  0x10UL                                   /**< Bit mask for EBI_ARDYPOL */
#define _EBI_POLARITY3_ARDYPOL_DEFAULT               0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY3 */
#define _EBI_POLARITY3_ARDYPOL_ACTIVELOW             0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY3 */
#define _EBI_POLARITY3_ARDYPOL_ACTIVEHIGH            0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY3 */
#define EBI_POLARITY3_ARDYPOL_DEFAULT                (_EBI_POLARITY3_ARDYPOL_DEFAULT << 4)    /**< Shifted mode DEFAULT for EBI_POLARITY3 */
#define EBI_POLARITY3_ARDYPOL_ACTIVELOW              (_EBI_POLARITY3_ARDYPOL_ACTIVELOW << 4)  /**< Shifted mode ACTIVELOW for EBI_POLARITY3 */
#define EBI_POLARITY3_ARDYPOL_ACTIVEHIGH             (_EBI_POLARITY3_ARDYPOL_ACTIVEHIGH << 4) /**< Shifted mode ACTIVEHIGH for EBI_POLARITY3 */
#define EBI_POLARITY3_BLPOL                          (0x1UL << 5)                             /**< BL Polarity */
#define _EBI_POLARITY3_BLPOL_SHIFT                   5                                        /**< Shift value for EBI_BLPOL */
#define _EBI_POLARITY3_BLPOL_MASK                    0x20UL                                   /**< Bit mask for EBI_BLPOL */
#define _EBI_POLARITY3_BLPOL_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for EBI_POLARITY3 */
#define _EBI_POLARITY3_BLPOL_ACTIVELOW               0x00000000UL                             /**< Mode ACTIVELOW for EBI_POLARITY3 */
#define _EBI_POLARITY3_BLPOL_ACTIVEHIGH              0x00000001UL                             /**< Mode ACTIVEHIGH for EBI_POLARITY3 */
#define EBI_POLARITY3_BLPOL_DEFAULT                  (_EBI_POLARITY3_BLPOL_DEFAULT << 5)      /**< Shifted mode DEFAULT for EBI_POLARITY3 */
#define EBI_POLARITY3_BLPOL_ACTIVELOW                (_EBI_POLARITY3_BLPOL_ACTIVELOW << 5)    /**< Shifted mode ACTIVELOW for EBI_POLARITY3 */
#define EBI_POLARITY3_BLPOL_ACTIVEHIGH               (_EBI_POLARITY3_BLPOL_ACTIVEHIGH << 5)   /**< Shifted mode ACTIVEHIGH for EBI_POLARITY3 */

/* Bit fields for EBI PAGECTRL */
#define _EBI_PAGECTRL_RESETVALUE                     0x00000F00UL                           /**< Default value for EBI_PAGECTRL */
#define _EBI_PAGECTRL_MASK                           0x07F00F13UL                           /**< Mask for EBI_PAGECTRL */
#define _EBI_PAGECTRL_PAGELEN_SHIFT                  0                                      /**< Shift value for EBI_PAGELEN */
#define _EBI_PAGECTRL_PAGELEN_MASK                   0x3UL                                  /**< Bit mask for EBI_PAGELEN */
#define _EBI_PAGECTRL_PAGELEN_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for EBI_PAGECTRL */
#define _EBI_PAGECTRL_PAGELEN_MEMBER4                0x00000000UL                           /**< Mode MEMBER4 for EBI_PAGECTRL */
#define _EBI_PAGECTRL_PAGELEN_MEMBER8                0x00000001UL                           /**< Mode MEMBER8 for EBI_PAGECTRL */
#define _EBI_PAGECTRL_PAGELEN_MEMBER16               0x00000002UL                           /**< Mode MEMBER16 for EBI_PAGECTRL */
#define _EBI_PAGECTRL_PAGELEN_MEMBER32               0x00000003UL                           /**< Mode MEMBER32 for EBI_PAGECTRL */
#define EBI_PAGECTRL_PAGELEN_DEFAULT                 (_EBI_PAGECTRL_PAGELEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for EBI_PAGECTRL */
#define EBI_PAGECTRL_PAGELEN_MEMBER4                 (_EBI_PAGECTRL_PAGELEN_MEMBER4 << 0)   /**< Shifted mode MEMBER4 for EBI_PAGECTRL */
#define EBI_PAGECTRL_PAGELEN_MEMBER8                 (_EBI_PAGECTRL_PAGELEN_MEMBER8 << 0)   /**< Shifted mode MEMBER8 for EBI_PAGECTRL */
#define EBI_PAGECTRL_PAGELEN_MEMBER16                (_EBI_PAGECTRL_PAGELEN_MEMBER16 << 0)  /**< Shifted mode MEMBER16 for EBI_PAGECTRL */
#define EBI_PAGECTRL_PAGELEN_MEMBER32                (_EBI_PAGECTRL_PAGELEN_MEMBER32 << 0)  /**< Shifted mode MEMBER32 for EBI_PAGECTRL */
#define EBI_PAGECTRL_INCHIT                          (0x1UL << 4)                           /**< Intrapage Hit Only on Incremental Addresses */
#define _EBI_PAGECTRL_INCHIT_SHIFT                   4                                      /**< Shift value for EBI_INCHIT */
#define _EBI_PAGECTRL_INCHIT_MASK                    0x10UL                                 /**< Bit mask for EBI_INCHIT */
#define _EBI_PAGECTRL_INCHIT_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for EBI_PAGECTRL */
#define EBI_PAGECTRL_INCHIT_DEFAULT                  (_EBI_PAGECTRL_INCHIT_DEFAULT << 4)    /**< Shifted mode DEFAULT for EBI_PAGECTRL */
#define _EBI_PAGECTRL_RDPA_SHIFT                     8                                      /**< Shift value for EBI_RDPA */
#define _EBI_PAGECTRL_RDPA_MASK                      0xF00UL                                /**< Bit mask for EBI_RDPA */
#define _EBI_PAGECTRL_RDPA_DEFAULT                   0x0000000FUL                           /**< Mode DEFAULT for EBI_PAGECTRL */
#define EBI_PAGECTRL_RDPA_DEFAULT                    (_EBI_PAGECTRL_RDPA_DEFAULT << 8)      /**< Shifted mode DEFAULT for EBI_PAGECTRL */
#define _EBI_PAGECTRL_KEEPOPEN_SHIFT                 20                                     /**< Shift value for EBI_KEEPOPEN */
#define _EBI_PAGECTRL_KEEPOPEN_MASK                  0x7F00000UL                            /**< Bit mask for EBI_KEEPOPEN */
#define _EBI_PAGECTRL_KEEPOPEN_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for EBI_PAGECTRL */
#define EBI_PAGECTRL_KEEPOPEN_DEFAULT                (_EBI_PAGECTRL_KEEPOPEN_DEFAULT << 20) /**< Shifted mode DEFAULT for EBI_PAGECTRL */

/* Bit fields for EBI NANDCTRL */
#define _EBI_NANDCTRL_RESETVALUE                     0x00000000UL                         /**< Default value for EBI_NANDCTRL */
#define _EBI_NANDCTRL_MASK                           0x00000031UL                         /**< Mask for EBI_NANDCTRL */
#define EBI_NANDCTRL_EN                              (0x1UL << 0)                         /**< NAND Flash Control Enable */
#define _EBI_NANDCTRL_EN_SHIFT                       0                                    /**< Shift value for EBI_EN */
#define _EBI_NANDCTRL_EN_MASK                        0x1UL                                /**< Bit mask for EBI_EN */
#define _EBI_NANDCTRL_EN_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for EBI_NANDCTRL */
#define EBI_NANDCTRL_EN_DEFAULT                      (_EBI_NANDCTRL_EN_DEFAULT << 0)      /**< Shifted mode DEFAULT for EBI_NANDCTRL */
#define _EBI_NANDCTRL_BANKSEL_SHIFT                  4                                    /**< Shift value for EBI_BANKSEL */
#define _EBI_NANDCTRL_BANKSEL_MASK                   0x30UL                               /**< Bit mask for EBI_BANKSEL */
#define _EBI_NANDCTRL_BANKSEL_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for EBI_NANDCTRL */
#define _EBI_NANDCTRL_BANKSEL_BANK0                  0x00000000UL                         /**< Mode BANK0 for EBI_NANDCTRL */
#define _EBI_NANDCTRL_BANKSEL_BANK1                  0x00000001UL                         /**< Mode BANK1 for EBI_NANDCTRL */
#define _EBI_NANDCTRL_BANKSEL_BANK2                  0x00000002UL                         /**< Mode BANK2 for EBI_NANDCTRL */
#define _EBI_NANDCTRL_BANKSEL_BANK3                  0x00000003UL                         /**< Mode BANK3 for EBI_NANDCTRL */
#define EBI_NANDCTRL_BANKSEL_DEFAULT                 (_EBI_NANDCTRL_BANKSEL_DEFAULT << 4) /**< Shifted mode DEFAULT for EBI_NANDCTRL */
#define EBI_NANDCTRL_BANKSEL_BANK0                   (_EBI_NANDCTRL_BANKSEL_BANK0 << 4)   /**< Shifted mode BANK0 for EBI_NANDCTRL */
#define EBI_NANDCTRL_BANKSEL_BANK1                   (_EBI_NANDCTRL_BANKSEL_BANK1 << 4)   /**< Shifted mode BANK1 for EBI_NANDCTRL */
#define EBI_NANDCTRL_BANKSEL_BANK2                   (_EBI_NANDCTRL_BANKSEL_BANK2 << 4)   /**< Shifted mode BANK2 for EBI_NANDCTRL */
#define EBI_NANDCTRL_BANKSEL_BANK3                   (_EBI_NANDCTRL_BANKSEL_BANK3 << 4)   /**< Shifted mode BANK3 for EBI_NANDCTRL */

/* Bit fields for EBI CMD */
#define _EBI_CMD_RESETVALUE                          0x00000000UL                     /**< Default value for EBI_CMD */
#define _EBI_CMD_MASK                                0x00000007UL                     /**< Mask for EBI_CMD */
#define EBI_CMD_ECCSTART                             (0x1UL << 0)                     /**< Error Correction Code Generation Start */
#define _EBI_CMD_ECCSTART_SHIFT                      0                                /**< Shift value for EBI_ECCSTART */
#define _EBI_CMD_ECCSTART_MASK                       0x1UL                            /**< Bit mask for EBI_ECCSTART */
#define _EBI_CMD_ECCSTART_DEFAULT                    0x00000000UL                     /**< Mode DEFAULT for EBI_CMD */
#define EBI_CMD_ECCSTART_DEFAULT                     (_EBI_CMD_ECCSTART_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_CMD */
#define EBI_CMD_ECCSTOP                              (0x1UL << 1)                     /**< Error Correction Code Generation Stop */
#define _EBI_CMD_ECCSTOP_SHIFT                       1                                /**< Shift value for EBI_ECCSTOP */
#define _EBI_CMD_ECCSTOP_MASK                        0x2UL                            /**< Bit mask for EBI_ECCSTOP */
#define _EBI_CMD_ECCSTOP_DEFAULT                     0x00000000UL                     /**< Mode DEFAULT for EBI_CMD */
#define EBI_CMD_ECCSTOP_DEFAULT                      (_EBI_CMD_ECCSTOP_DEFAULT << 1)  /**< Shifted mode DEFAULT for EBI_CMD */
#define EBI_CMD_ECCCLEAR                             (0x1UL << 2)                     /**< Error Correction Code Clear */
#define _EBI_CMD_ECCCLEAR_SHIFT                      2                                /**< Shift value for EBI_ECCCLEAR */
#define _EBI_CMD_ECCCLEAR_MASK                       0x4UL                            /**< Bit mask for EBI_ECCCLEAR */
#define _EBI_CMD_ECCCLEAR_DEFAULT                    0x00000000UL                     /**< Mode DEFAULT for EBI_CMD */
#define EBI_CMD_ECCCLEAR_DEFAULT                     (_EBI_CMD_ECCCLEAR_DEFAULT << 2) /**< Shifted mode DEFAULT for EBI_CMD */

/* Bit fields for EBI STATUS */
#define _EBI_STATUS_RESETVALUE                       0x00000000UL                              /**< Default value for EBI_STATUS */
#define _EBI_STATUS_MASK                             0x00003711UL                              /**< Mask for EBI_STATUS */
#define EBI_STATUS_AHBACT                            (0x1UL << 0)                              /**< EBI Busy With AHB Transaction */
#define _EBI_STATUS_AHBACT_SHIFT                     0                                         /**< Shift value for EBI_AHBACT */
#define _EBI_STATUS_AHBACT_MASK                      0x1UL                                     /**< Bit mask for EBI_AHBACT */
#define _EBI_STATUS_AHBACT_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_AHBACT_DEFAULT                    (_EBI_STATUS_AHBACT_DEFAULT << 0)         /**< Shifted mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_ECCACT                            (0x1UL << 4)                              /**< EBI ECC Generation Active */
#define _EBI_STATUS_ECCACT_SHIFT                     4                                         /**< Shift value for EBI_ECCACT */
#define _EBI_STATUS_ECCACT_MASK                      0x10UL                                    /**< Bit mask for EBI_ECCACT */
#define _EBI_STATUS_ECCACT_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_ECCACT_DEFAULT                    (_EBI_STATUS_ECCACT_DEFAULT << 4)         /**< Shifted mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_TFTPIXEL0EMPTY                    (0x1UL << 8)                              /**< EBI_TFTPIXEL0 is Empty */
#define _EBI_STATUS_TFTPIXEL0EMPTY_SHIFT             8                                         /**< Shift value for EBI_TFTPIXEL0EMPTY */
#define _EBI_STATUS_TFTPIXEL0EMPTY_MASK              0x100UL                                   /**< Bit mask for EBI_TFTPIXEL0EMPTY */
#define _EBI_STATUS_TFTPIXEL0EMPTY_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_TFTPIXEL0EMPTY_DEFAULT            (_EBI_STATUS_TFTPIXEL0EMPTY_DEFAULT << 8) /**< Shifted mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_TFTPIXEL1EMPTY                    (0x1UL << 9)                              /**< EBI_TFTPIXEL1 is Empty */
#define _EBI_STATUS_TFTPIXEL1EMPTY_SHIFT             9                                         /**< Shift value for EBI_TFTPIXEL1EMPTY */
#define _EBI_STATUS_TFTPIXEL1EMPTY_MASK              0x200UL                                   /**< Bit mask for EBI_TFTPIXEL1EMPTY */
#define _EBI_STATUS_TFTPIXEL1EMPTY_DEFAULT           0x00000000UL                              /**< Mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_TFTPIXEL1EMPTY_DEFAULT            (_EBI_STATUS_TFTPIXEL1EMPTY_DEFAULT << 9) /**< Shifted mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_TFTPIXELFULL                      (0x1UL << 10)                             /**< EBI_TFTPIXEL0 is Full */
#define _EBI_STATUS_TFTPIXELFULL_SHIFT               10                                        /**< Shift value for EBI_TFTPIXELFULL */
#define _EBI_STATUS_TFTPIXELFULL_MASK                0x400UL                                   /**< Bit mask for EBI_TFTPIXELFULL */
#define _EBI_STATUS_TFTPIXELFULL_DEFAULT             0x00000000UL                              /**< Mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_TFTPIXELFULL_DEFAULT              (_EBI_STATUS_TFTPIXELFULL_DEFAULT << 10)  /**< Shifted mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_DDACT                             (0x1UL << 12)                             /**< EBI Busy With Direct Drive Transactions */
#define _EBI_STATUS_DDACT_SHIFT                      12                                        /**< Shift value for EBI_DDACT */
#define _EBI_STATUS_DDACT_MASK                       0x1000UL                                  /**< Bit mask for EBI_DDACT */
#define _EBI_STATUS_DDACT_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_DDACT_DEFAULT                     (_EBI_STATUS_DDACT_DEFAULT << 12)         /**< Shifted mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_TFTDDEMPTY                        (0x1UL << 13)                             /**< EBI_TFTDD Register is Empty */
#define _EBI_STATUS_TFTDDEMPTY_SHIFT                 13                                        /**< Shift value for EBI_TFTDDEMPTY */
#define _EBI_STATUS_TFTDDEMPTY_MASK                  0x2000UL                                  /**< Bit mask for EBI_TFTDDEMPTY */
#define _EBI_STATUS_TFTDDEMPTY_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for EBI_STATUS */
#define EBI_STATUS_TFTDDEMPTY_DEFAULT                (_EBI_STATUS_TFTDDEMPTY_DEFAULT << 13)    /**< Shifted mode DEFAULT for EBI_STATUS */

/* Bit fields for EBI ECCPARITY */
#define _EBI_ECCPARITY_RESETVALUE                    0x00000000UL                            /**< Default value for EBI_ECCPARITY */
#define _EBI_ECCPARITY_MASK                          0xFFFFFFFFUL                            /**< Mask for EBI_ECCPARITY */
#define _EBI_ECCPARITY_ECCPARITY_SHIFT               0                                       /**< Shift value for EBI_ECCPARITY */
#define _EBI_ECCPARITY_ECCPARITY_MASK                0xFFFFFFFFUL                            /**< Bit mask for EBI_ECCPARITY */
#define _EBI_ECCPARITY_ECCPARITY_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for EBI_ECCPARITY */
#define EBI_ECCPARITY_ECCPARITY_DEFAULT              (_EBI_ECCPARITY_ECCPARITY_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_ECCPARITY */

/* Bit fields for EBI TFTCTRL */
#define _EBI_TFTCTRL_RESETVALUE                      0x00000000UL                               /**< Default value for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASK                            0x00FB1F3FUL                               /**< Mask for EBI_TFTCTRL */
#define _EBI_TFTCTRL_DD_SHIFT                        0                                          /**< Shift value for EBI_DD */
#define _EBI_TFTCTRL_DD_MASK                         0x3UL                                      /**< Bit mask for EBI_DD */
#define _EBI_TFTCTRL_DD_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for EBI_TFTCTRL */
#define _EBI_TFTCTRL_DD_DISABLED                     0x00000000UL                               /**< Mode DISABLED for EBI_TFTCTRL */
#define _EBI_TFTCTRL_DD_INTERNAL                     0x00000001UL                               /**< Mode INTERNAL for EBI_TFTCTRL */
#define _EBI_TFTCTRL_DD_EXTERNAL                     0x00000002UL                               /**< Mode EXTERNAL for EBI_TFTCTRL */
#define EBI_TFTCTRL_DD_DEFAULT                       (_EBI_TFTCTRL_DD_DEFAULT << 0)             /**< Shifted mode DEFAULT for EBI_TFTCTRL */
#define EBI_TFTCTRL_DD_DISABLED                      (_EBI_TFTCTRL_DD_DISABLED << 0)            /**< Shifted mode DISABLED for EBI_TFTCTRL */
#define EBI_TFTCTRL_DD_INTERNAL                      (_EBI_TFTCTRL_DD_INTERNAL << 0)            /**< Shifted mode INTERNAL for EBI_TFTCTRL */
#define EBI_TFTCTRL_DD_EXTERNAL                      (_EBI_TFTCTRL_DD_EXTERNAL << 0)            /**< Shifted mode EXTERNAL for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASKBLEND_SHIFT                 2                                          /**< Shift value for EBI_MASKBLEND */
#define _EBI_TFTCTRL_MASKBLEND_MASK                  0x3CUL                                     /**< Bit mask for EBI_MASKBLEND */
#define _EBI_TFTCTRL_MASKBLEND_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASKBLEND_DISABLED              0x00000000UL                               /**< Mode DISABLED for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASKBLEND_IMASK                 0x00000001UL                               /**< Mode IMASK for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASKBLEND_IALPHA                0x00000002UL                               /**< Mode IALPHA for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASKBLEND_IMASKALPHA            0x00000003UL                               /**< Mode IMASKALPHA for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASKBLEND_EFBMASK               0x00000004UL                               /**< Mode EFBMASK for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASKBLEND_EFBALPHA              0x00000005UL                               /**< Mode EFBALPHA for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASKBLEND_EFBMASKALPHA          0x00000006UL                               /**< Mode EFBMASKALPHA for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASKBLEND_IFBMASK               0x00000007UL                               /**< Mode IFBMASK for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASKBLEND_IFBALPHA              0x00000008UL                               /**< Mode IFBALPHA for EBI_TFTCTRL */
#define _EBI_TFTCTRL_MASKBLEND_IFBMASKALPHA          0x00000009UL                               /**< Mode IFBMASKALPHA for EBI_TFTCTRL */
#define EBI_TFTCTRL_MASKBLEND_DEFAULT                (_EBI_TFTCTRL_MASKBLEND_DEFAULT << 2)      /**< Shifted mode DEFAULT for EBI_TFTCTRL */
#define EBI_TFTCTRL_MASKBLEND_DISABLED               (_EBI_TFTCTRL_MASKBLEND_DISABLED << 2)     /**< Shifted mode DISABLED for EBI_TFTCTRL */
#define EBI_TFTCTRL_MASKBLEND_IMASK                  (_EBI_TFTCTRL_MASKBLEND_IMASK << 2)        /**< Shifted mode IMASK for EBI_TFTCTRL */
#define EBI_TFTCTRL_MASKBLEND_IALPHA                 (_EBI_TFTCTRL_MASKBLEND_IALPHA << 2)       /**< Shifted mode IALPHA for EBI_TFTCTRL */
#define EBI_TFTCTRL_MASKBLEND_IMASKALPHA             (_EBI_TFTCTRL_MASKBLEND_IMASKALPHA << 2)   /**< Shifted mode IMASKALPHA for EBI_TFTCTRL */
#define EBI_TFTCTRL_MASKBLEND_EFBMASK                (_EBI_TFTCTRL_MASKBLEND_EFBMASK << 2)      /**< Shifted mode EFBMASK for EBI_TFTCTRL */
#define EBI_TFTCTRL_MASKBLEND_EFBALPHA               (_EBI_TFTCTRL_MASKBLEND_EFBALPHA << 2)     /**< Shifted mode EFBALPHA for EBI_TFTCTRL */
#define EBI_TFTCTRL_MASKBLEND_EFBMASKALPHA           (_EBI_TFTCTRL_MASKBLEND_EFBMASKALPHA << 2) /**< Shifted mode EFBMASKALPHA for EBI_TFTCTRL */
#define EBI_TFTCTRL_MASKBLEND_IFBMASK                (_EBI_TFTCTRL_MASKBLEND_IFBMASK << 2)      /**< Shifted mode IFBMASK for EBI_TFTCTRL */
#define EBI_TFTCTRL_MASKBLEND_IFBALPHA               (_EBI_TFTCTRL_MASKBLEND_IFBALPHA << 2)     /**< Shifted mode IFBALPHA for EBI_TFTCTRL */
#define EBI_TFTCTRL_MASKBLEND_IFBMASKALPHA           (_EBI_TFTCTRL_MASKBLEND_IFBMASKALPHA << 2) /**< Shifted mode IFBMASKALPHA for EBI_TFTCTRL */
#define EBI_TFTCTRL_SHIFTDCLKEN                      (0x1UL << 8)                               /**< TFT EBI_DCLK Shift Enable */
#define _EBI_TFTCTRL_SHIFTDCLKEN_SHIFT               8                                          /**< Shift value for EBI_SHIFTDCLKEN */
#define _EBI_TFTCTRL_SHIFTDCLKEN_MASK                0x100UL                                    /**< Bit mask for EBI_SHIFTDCLKEN */
#define _EBI_TFTCTRL_SHIFTDCLKEN_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for EBI_TFTCTRL */
#define EBI_TFTCTRL_SHIFTDCLKEN_DEFAULT              (_EBI_TFTCTRL_SHIFTDCLKEN_DEFAULT << 8)    /**< Shifted mode DEFAULT for EBI_TFTCTRL */
#define EBI_TFTCTRL_FBCTRIG                          (0x1UL << 9)                               /**< TFT Frame Base Copy Trigger */
#define _EBI_TFTCTRL_FBCTRIG_SHIFT                   9                                          /**< Shift value for EBI_FBCTRIG */
#define _EBI_TFTCTRL_FBCTRIG_MASK                    0x200UL                                    /**< Bit mask for EBI_FBCTRIG */
#define _EBI_TFTCTRL_FBCTRIG_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for EBI_TFTCTRL */
#define _EBI_TFTCTRL_FBCTRIG_VSYNC                   0x00000000UL                               /**< Mode VSYNC for EBI_TFTCTRL */
#define _EBI_TFTCTRL_FBCTRIG_HSYNC                   0x00000001UL                               /**< Mode HSYNC for EBI_TFTCTRL */
#define EBI_TFTCTRL_FBCTRIG_DEFAULT                  (_EBI_TFTCTRL_FBCTRIG_DEFAULT << 9)        /**< Shifted mode DEFAULT for EBI_TFTCTRL */
#define EBI_TFTCTRL_FBCTRIG_VSYNC                    (_EBI_TFTCTRL_FBCTRIG_VSYNC << 9)          /**< Shifted mode VSYNC for EBI_TFTCTRL */
#define EBI_TFTCTRL_FBCTRIG_HSYNC                    (_EBI_TFTCTRL_FBCTRIG_HSYNC << 9)          /**< Shifted mode HSYNC for EBI_TFTCTRL */
#define _EBI_TFTCTRL_INTERLEAVE_SHIFT                10                                         /**< Shift value for EBI_INTERLEAVE */
#define _EBI_TFTCTRL_INTERLEAVE_MASK                 0xC00UL                                    /**< Bit mask for EBI_INTERLEAVE */
#define _EBI_TFTCTRL_INTERLEAVE_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for EBI_TFTCTRL */
#define _EBI_TFTCTRL_INTERLEAVE_UNLIMITED            0x00000000UL                               /**< Mode UNLIMITED for EBI_TFTCTRL */
#define _EBI_TFTCTRL_INTERLEAVE_ONEPERDCLK           0x00000001UL                               /**< Mode ONEPERDCLK for EBI_TFTCTRL */
#define _EBI_TFTCTRL_INTERLEAVE_PORCH                0x00000002UL                               /**< Mode PORCH for EBI_TFTCTRL */
#define EBI_TFTCTRL_INTERLEAVE_DEFAULT               (_EBI_TFTCTRL_INTERLEAVE_DEFAULT << 10)    /**< Shifted mode DEFAULT for EBI_TFTCTRL */
#define EBI_TFTCTRL_INTERLEAVE_UNLIMITED             (_EBI_TFTCTRL_INTERLEAVE_UNLIMITED << 10)  /**< Shifted mode UNLIMITED for EBI_TFTCTRL */
#define EBI_TFTCTRL_INTERLEAVE_ONEPERDCLK            (_EBI_TFTCTRL_INTERLEAVE_ONEPERDCLK << 10) /**< Shifted mode ONEPERDCLK for EBI_TFTCTRL */
#define EBI_TFTCTRL_INTERLEAVE_PORCH                 (_EBI_TFTCTRL_INTERLEAVE_PORCH << 10)      /**< Shifted mode PORCH for EBI_TFTCTRL */
#define EBI_TFTCTRL_COLOR1SRC                        (0x1UL << 12)                              /**< Masking/Alpha Blending Color1 Source */
#define _EBI_TFTCTRL_COLOR1SRC_SHIFT                 12                                         /**< Shift value for EBI_COLOR1SRC */
#define _EBI_TFTCTRL_COLOR1SRC_MASK                  0x1000UL                                   /**< Bit mask for EBI_COLOR1SRC */
#define _EBI_TFTCTRL_COLOR1SRC_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for EBI_TFTCTRL */
#define _EBI_TFTCTRL_COLOR1SRC_MEM                   0x00000000UL                               /**< Mode MEM for EBI_TFTCTRL */
#define _EBI_TFTCTRL_COLOR1SRC_PIXEL1                0x00000001UL                               /**< Mode PIXEL1 for EBI_TFTCTRL */
#define EBI_TFTCTRL_COLOR1SRC_DEFAULT                (_EBI_TFTCTRL_COLOR1SRC_DEFAULT << 12)     /**< Shifted mode DEFAULT for EBI_TFTCTRL */
#define EBI_TFTCTRL_COLOR1SRC_MEM                    (_EBI_TFTCTRL_COLOR1SRC_MEM << 12)         /**< Shifted mode MEM for EBI_TFTCTRL */
#define EBI_TFTCTRL_COLOR1SRC_PIXEL1                 (_EBI_TFTCTRL_COLOR1SRC_PIXEL1 << 12)      /**< Shifted mode PIXEL1 for EBI_TFTCTRL */
#define _EBI_TFTCTRL_WIDTH_SHIFT                     16                                         /**< Shift value for EBI_WIDTH */
#define _EBI_TFTCTRL_WIDTH_MASK                      0x30000UL                                  /**< Bit mask for EBI_WIDTH */
#define _EBI_TFTCTRL_WIDTH_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for EBI_TFTCTRL */
#define _EBI_TFTCTRL_WIDTH_BYTE                      0x00000000UL                               /**< Mode BYTE for EBI_TFTCTRL */
#define _EBI_TFTCTRL_WIDTH_HALFWORD                  0x00000001UL                               /**< Mode HALFWORD for EBI_TFTCTRL */
#define EBI_TFTCTRL_WIDTH_DEFAULT                    (_EBI_TFTCTRL_WIDTH_DEFAULT << 16)         /**< Shifted mode DEFAULT for EBI_TFTCTRL */
#define EBI_TFTCTRL_WIDTH_BYTE                       (_EBI_TFTCTRL_WIDTH_BYTE << 16)            /**< Shifted mode BYTE for EBI_TFTCTRL */
#define EBI_TFTCTRL_WIDTH_HALFWORD                   (_EBI_TFTCTRL_WIDTH_HALFWORD << 16)        /**< Shifted mode HALFWORD for EBI_TFTCTRL */
#define EBI_TFTCTRL_ALIASBANKEN                      (0x1UL << 19)                              /**< Alias to Graphics Bank Enable */
#define _EBI_TFTCTRL_ALIASBANKEN_SHIFT               19                                         /**< Shift value for EBI_ALIASBANKEN */
#define _EBI_TFTCTRL_ALIASBANKEN_MASK                0x80000UL                                  /**< Bit mask for EBI_ALIASBANKEN */
#define _EBI_TFTCTRL_ALIASBANKEN_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for EBI_TFTCTRL */
#define _EBI_TFTCTRL_ALIASBANKEN_DISABLE             0x00000000UL                               /**< Mode DISABLE for EBI_TFTCTRL */
#define _EBI_TFTCTRL_ALIASBANKEN_ENABLE              0x00000001UL                               /**< Mode ENABLE for EBI_TFTCTRL */
#define EBI_TFTCTRL_ALIASBANKEN_DEFAULT              (_EBI_TFTCTRL_ALIASBANKEN_DEFAULT << 19)   /**< Shifted mode DEFAULT for EBI_TFTCTRL */
#define EBI_TFTCTRL_ALIASBANKEN_DISABLE              (_EBI_TFTCTRL_ALIASBANKEN_DISABLE << 19)   /**< Shifted mode DISABLE for EBI_TFTCTRL */
#define EBI_TFTCTRL_ALIASBANKEN_ENABLE               (_EBI_TFTCTRL_ALIASBANKEN_ENABLE << 19)    /**< Shifted mode ENABLE for EBI_TFTCTRL */
#define _EBI_TFTCTRL_BANKSEL_SHIFT                   20                                         /**< Shift value for EBI_BANKSEL */
#define _EBI_TFTCTRL_BANKSEL_MASK                    0x300000UL                                 /**< Bit mask for EBI_BANKSEL */
#define _EBI_TFTCTRL_BANKSEL_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for EBI_TFTCTRL */
#define _EBI_TFTCTRL_BANKSEL_BANK0                   0x00000000UL                               /**< Mode BANK0 for EBI_TFTCTRL */
#define _EBI_TFTCTRL_BANKSEL_BANK1                   0x00000001UL                               /**< Mode BANK1 for EBI_TFTCTRL */
#define _EBI_TFTCTRL_BANKSEL_BANK2                   0x00000002UL                               /**< Mode BANK2 for EBI_TFTCTRL */
#define _EBI_TFTCTRL_BANKSEL_BANK3                   0x00000003UL                               /**< Mode BANK3 for EBI_TFTCTRL */
#define EBI_TFTCTRL_BANKSEL_DEFAULT                  (_EBI_TFTCTRL_BANKSEL_DEFAULT << 20)       /**< Shifted mode DEFAULT for EBI_TFTCTRL */
#define EBI_TFTCTRL_BANKSEL_BANK0                    (_EBI_TFTCTRL_BANKSEL_BANK0 << 20)         /**< Shifted mode BANK0 for EBI_TFTCTRL */
#define EBI_TFTCTRL_BANKSEL_BANK1                    (_EBI_TFTCTRL_BANKSEL_BANK1 << 20)         /**< Shifted mode BANK1 for EBI_TFTCTRL */
#define EBI_TFTCTRL_BANKSEL_BANK2                    (_EBI_TFTCTRL_BANKSEL_BANK2 << 20)         /**< Shifted mode BANK2 for EBI_TFTCTRL */
#define EBI_TFTCTRL_BANKSEL_BANK3                    (_EBI_TFTCTRL_BANKSEL_BANK3 << 20)         /**< Shifted mode BANK3 for EBI_TFTCTRL */
#define _EBI_TFTCTRL_ALIASBANK_SHIFT                 22                                         /**< Shift value for EBI_ALIASBANK */
#define _EBI_TFTCTRL_ALIASBANK_MASK                  0xC00000UL                                 /**< Bit mask for EBI_ALIASBANK */
#define _EBI_TFTCTRL_ALIASBANK_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for EBI_TFTCTRL */
#define _EBI_TFTCTRL_ALIASBANK_ALIASBANK0            0x00000000UL                               /**< Mode ALIASBANK0 for EBI_TFTCTRL */
#define _EBI_TFTCTRL_ALIASBANK_ALIASBANK1            0x00000001UL                               /**< Mode ALIASBANK1 for EBI_TFTCTRL */
#define _EBI_TFTCTRL_ALIASBANK_ALIASBANK2            0x00000002UL                               /**< Mode ALIASBANK2 for EBI_TFTCTRL */
#define _EBI_TFTCTRL_ALIASBANK_ALIASBANK3            0x00000003UL                               /**< Mode ALIASBANK3 for EBI_TFTCTRL */
#define EBI_TFTCTRL_ALIASBANK_DEFAULT                (_EBI_TFTCTRL_ALIASBANK_DEFAULT << 22)     /**< Shifted mode DEFAULT for EBI_TFTCTRL */
#define EBI_TFTCTRL_ALIASBANK_ALIASBANK0             (_EBI_TFTCTRL_ALIASBANK_ALIASBANK0 << 22)  /**< Shifted mode ALIASBANK0 for EBI_TFTCTRL */
#define EBI_TFTCTRL_ALIASBANK_ALIASBANK1             (_EBI_TFTCTRL_ALIASBANK_ALIASBANK1 << 22)  /**< Shifted mode ALIASBANK1 for EBI_TFTCTRL */
#define EBI_TFTCTRL_ALIASBANK_ALIASBANK2             (_EBI_TFTCTRL_ALIASBANK_ALIASBANK2 << 22)  /**< Shifted mode ALIASBANK2 for EBI_TFTCTRL */
#define EBI_TFTCTRL_ALIASBANK_ALIASBANK3             (_EBI_TFTCTRL_ALIASBANK_ALIASBANK3 << 22)  /**< Shifted mode ALIASBANK3 for EBI_TFTCTRL */

/* Bit fields for EBI TFTSTATUS */
#define _EBI_TFTSTATUS_RESETVALUE                    0x00000000UL                        /**< Default value for EBI_TFTSTATUS */
#define _EBI_TFTSTATUS_MASK                          0x7FFF07FFUL                        /**< Mask for EBI_TFTSTATUS */
#define _EBI_TFTSTATUS_HCNT_SHIFT                    0                                   /**< Shift value for EBI_HCNT */
#define _EBI_TFTSTATUS_HCNT_MASK                     0x7FFUL                             /**< Bit mask for EBI_HCNT */
#define _EBI_TFTSTATUS_HCNT_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for EBI_TFTSTATUS */
#define EBI_TFTSTATUS_HCNT_DEFAULT                   (_EBI_TFTSTATUS_HCNT_DEFAULT << 0)  /**< Shifted mode DEFAULT for EBI_TFTSTATUS */
#define _EBI_TFTSTATUS_VCNT_SHIFT                    16                                  /**< Shift value for EBI_VCNT */
#define _EBI_TFTSTATUS_VCNT_MASK                     0x7FFF0000UL                        /**< Bit mask for EBI_VCNT */
#define _EBI_TFTSTATUS_VCNT_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for EBI_TFTSTATUS */
#define EBI_TFTSTATUS_VCNT_DEFAULT                   (_EBI_TFTSTATUS_VCNT_DEFAULT << 16) /**< Shifted mode DEFAULT for EBI_TFTSTATUS */

/* Bit fields for EBI TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_RESETVALUE               0x00000000UL                                     /**< Default value for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_MASK                     0x00000307UL                                     /**< Mask for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL0FORMAT_SHIFT       0                                                /**< Shift value for EBI_PIXEL0FORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL0FORMAT_MASK        0x7UL                                            /**< Bit mask for EBI_PIXEL0FORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL0FORMAT_DEFAULT     0x00000000UL                                     /**< Mode DEFAULT for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0555    0x00000000UL                                     /**< Mode ARGB0555 for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0565    0x00000001UL                                     /**< Mode ARGB0565 for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0666    0x00000002UL                                     /**< Mode ARGB0666 for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0888    0x00000003UL                                     /**< Mode ARGB0888 for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB5555    0x00000004UL                                     /**< Mode ARGB5555 for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB6565    0x00000005UL                                     /**< Mode ARGB6565 for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB6666    0x00000006UL                                     /**< Mode ARGB6666 for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB8888    0x00000007UL                                     /**< Mode ARGB8888 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL0FORMAT_DEFAULT      (_EBI_TFTCOLORFORMAT_PIXEL0FORMAT_DEFAULT << 0)  /**< Shifted mode DEFAULT for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0555     (_EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0555 << 0) /**< Shifted mode ARGB0555 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0565     (_EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0565 << 0) /**< Shifted mode ARGB0565 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0666     (_EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0666 << 0) /**< Shifted mode ARGB0666 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0888     (_EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB0888 << 0) /**< Shifted mode ARGB0888 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB5555     (_EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB5555 << 0) /**< Shifted mode ARGB5555 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB6565     (_EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB6565 << 0) /**< Shifted mode ARGB6565 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB6666     (_EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB6666 << 0) /**< Shifted mode ARGB6666 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB8888     (_EBI_TFTCOLORFORMAT_PIXEL0FORMAT_ARGB8888 << 0) /**< Shifted mode ARGB8888 for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL1FORMAT_SHIFT       8                                                /**< Shift value for EBI_PIXEL1FORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL1FORMAT_MASK        0x300UL                                          /**< Bit mask for EBI_PIXEL1FORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL1FORMAT_DEFAULT     0x00000000UL                                     /**< Mode DEFAULT for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB555      0x00000000UL                                     /**< Mode RGB555 for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB565      0x00000001UL                                     /**< Mode RGB565 for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB666      0x00000002UL                                     /**< Mode RGB666 for EBI_TFTCOLORFORMAT */
#define _EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB888      0x00000003UL                                     /**< Mode RGB888 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL1FORMAT_DEFAULT      (_EBI_TFTCOLORFORMAT_PIXEL1FORMAT_DEFAULT << 8)  /**< Shifted mode DEFAULT for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB555       (_EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB555 << 8)   /**< Shifted mode RGB555 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB565       (_EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB565 << 8)   /**< Shifted mode RGB565 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB666       (_EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB666 << 8)   /**< Shifted mode RGB666 for EBI_TFTCOLORFORMAT */
#define EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB888       (_EBI_TFTCOLORFORMAT_PIXEL1FORMAT_RGB888 << 8)   /**< Shifted mode RGB888 for EBI_TFTCOLORFORMAT */

/* Bit fields for EBI TFTFRAMEBASE */
#define _EBI_TFTFRAMEBASE_RESETVALUE                 0x00000000UL                               /**< Default value for EBI_TFTFRAMEBASE */
#define _EBI_TFTFRAMEBASE_MASK                       0x0FFFFFFFUL                               /**< Mask for EBI_TFTFRAMEBASE */
#define _EBI_TFTFRAMEBASE_FRAMEBASE_SHIFT            0                                          /**< Shift value for EBI_FRAMEBASE */
#define _EBI_TFTFRAMEBASE_FRAMEBASE_MASK             0xFFFFFFFUL                                /**< Bit mask for EBI_FRAMEBASE */
#define _EBI_TFTFRAMEBASE_FRAMEBASE_DEFAULT          0x00000000UL                               /**< Mode DEFAULT for EBI_TFTFRAMEBASE */
#define EBI_TFTFRAMEBASE_FRAMEBASE_DEFAULT           (_EBI_TFTFRAMEBASE_FRAMEBASE_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_TFTFRAMEBASE */

/* Bit fields for EBI TFTSTRIDE */
#define _EBI_TFTSTRIDE_RESETVALUE                    0x00000000UL                          /**< Default value for EBI_TFTSTRIDE */
#define _EBI_TFTSTRIDE_MASK                          0x00000FFFUL                          /**< Mask for EBI_TFTSTRIDE */
#define _EBI_TFTSTRIDE_HSTRIDE_SHIFT                 0                                     /**< Shift value for EBI_HSTRIDE */
#define _EBI_TFTSTRIDE_HSTRIDE_MASK                  0xFFFUL                               /**< Bit mask for EBI_HSTRIDE */
#define _EBI_TFTSTRIDE_HSTRIDE_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for EBI_TFTSTRIDE */
#define EBI_TFTSTRIDE_HSTRIDE_DEFAULT                (_EBI_TFTSTRIDE_HSTRIDE_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_TFTSTRIDE */

/* Bit fields for EBI TFTSIZE */
#define _EBI_TFTSIZE_RESETVALUE                      0x00000000UL                     /**< Default value for EBI_TFTSIZE */
#define _EBI_TFTSIZE_MASK                            0x03FF03FFUL                     /**< Mask for EBI_TFTSIZE */
#define _EBI_TFTSIZE_HSZ_SHIFT                       0                                /**< Shift value for EBI_HSZ */
#define _EBI_TFTSIZE_HSZ_MASK                        0x3FFUL                          /**< Bit mask for EBI_HSZ */
#define _EBI_TFTSIZE_HSZ_DEFAULT                     0x00000000UL                     /**< Mode DEFAULT for EBI_TFTSIZE */
#define EBI_TFTSIZE_HSZ_DEFAULT                      (_EBI_TFTSIZE_HSZ_DEFAULT << 0)  /**< Shifted mode DEFAULT for EBI_TFTSIZE */
#define _EBI_TFTSIZE_VSZ_SHIFT                       16                               /**< Shift value for EBI_VSZ */
#define _EBI_TFTSIZE_VSZ_MASK                        0x3FF0000UL                      /**< Bit mask for EBI_VSZ */
#define _EBI_TFTSIZE_VSZ_DEFAULT                     0x00000000UL                     /**< Mode DEFAULT for EBI_TFTSIZE */
#define EBI_TFTSIZE_VSZ_DEFAULT                      (_EBI_TFTSIZE_VSZ_DEFAULT << 16) /**< Shifted mode DEFAULT for EBI_TFTSIZE */

/* Bit fields for EBI TFTHPORCH */
#define _EBI_TFTHPORCH_RESETVALUE                    0x00000000UL                              /**< Default value for EBI_TFTHPORCH */
#define _EBI_TFTHPORCH_MASK                          0x33FCFF7FUL                              /**< Mask for EBI_TFTHPORCH */
#define _EBI_TFTHPORCH_HSYNC_SHIFT                   0                                         /**< Shift value for EBI_HSYNC */
#define _EBI_TFTHPORCH_HSYNC_MASK                    0x7FUL                                    /**< Bit mask for EBI_HSYNC */
#define _EBI_TFTHPORCH_HSYNC_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for EBI_TFTHPORCH */
#define EBI_TFTHPORCH_HSYNC_DEFAULT                  (_EBI_TFTHPORCH_HSYNC_DEFAULT << 0)       /**< Shifted mode DEFAULT for EBI_TFTHPORCH */
#define _EBI_TFTHPORCH_HFPORCH_SHIFT                 8                                         /**< Shift value for EBI_HFPORCH */
#define _EBI_TFTHPORCH_HFPORCH_MASK                  0xFF00UL                                  /**< Bit mask for EBI_HFPORCH */
#define _EBI_TFTHPORCH_HFPORCH_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for EBI_TFTHPORCH */
#define EBI_TFTHPORCH_HFPORCH_DEFAULT                (_EBI_TFTHPORCH_HFPORCH_DEFAULT << 8)     /**< Shifted mode DEFAULT for EBI_TFTHPORCH */
#define _EBI_TFTHPORCH_HBPORCH_SHIFT                 18                                        /**< Shift value for EBI_HBPORCH */
#define _EBI_TFTHPORCH_HBPORCH_MASK                  0x3FC0000UL                               /**< Bit mask for EBI_HBPORCH */
#define _EBI_TFTHPORCH_HBPORCH_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for EBI_TFTHPORCH */
#define EBI_TFTHPORCH_HBPORCH_DEFAULT                (_EBI_TFTHPORCH_HBPORCH_DEFAULT << 18)    /**< Shifted mode DEFAULT for EBI_TFTHPORCH */
#define _EBI_TFTHPORCH_HSYNCSTART_SHIFT              28                                        /**< Shift value for EBI_HSYNCSTART */
#define _EBI_TFTHPORCH_HSYNCSTART_MASK               0x30000000UL                              /**< Bit mask for EBI_HSYNCSTART */
#define _EBI_TFTHPORCH_HSYNCSTART_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for EBI_TFTHPORCH */
#define EBI_TFTHPORCH_HSYNCSTART_DEFAULT             (_EBI_TFTHPORCH_HSYNCSTART_DEFAULT << 28) /**< Shifted mode DEFAULT for EBI_TFTHPORCH */

/* Bit fields for EBI TFTVPORCH */
#define _EBI_TFTVPORCH_RESETVALUE                    0x00000000UL                           /**< Default value for EBI_TFTVPORCH */
#define _EBI_TFTVPORCH_MASK                          0xFFFFFF7FUL                           /**< Mask for EBI_TFTVPORCH */
#define _EBI_TFTVPORCH_VSYNC_SHIFT                   0                                      /**< Shift value for EBI_VSYNC */
#define _EBI_TFTVPORCH_VSYNC_MASK                    0x7FUL                                 /**< Bit mask for EBI_VSYNC */
#define _EBI_TFTVPORCH_VSYNC_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for EBI_TFTVPORCH */
#define EBI_TFTVPORCH_VSYNC_DEFAULT                  (_EBI_TFTVPORCH_VSYNC_DEFAULT << 0)    /**< Shifted mode DEFAULT for EBI_TFTVPORCH */
#define _EBI_TFTVPORCH_VFPORCH_SHIFT                 8                                      /**< Shift value for EBI_VFPORCH */
#define _EBI_TFTVPORCH_VFPORCH_MASK                  0xFFF00UL                              /**< Bit mask for EBI_VFPORCH */
#define _EBI_TFTVPORCH_VFPORCH_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for EBI_TFTVPORCH */
#define EBI_TFTVPORCH_VFPORCH_DEFAULT                (_EBI_TFTVPORCH_VFPORCH_DEFAULT << 8)  /**< Shifted mode DEFAULT for EBI_TFTVPORCH */
#define _EBI_TFTVPORCH_VBPORCH_SHIFT                 20                                     /**< Shift value for EBI_VBPORCH */
#define _EBI_TFTVPORCH_VBPORCH_MASK                  0xFFF00000UL                           /**< Bit mask for EBI_VBPORCH */
#define _EBI_TFTVPORCH_VBPORCH_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for EBI_TFTVPORCH */
#define EBI_TFTVPORCH_VBPORCH_DEFAULT                (_EBI_TFTVPORCH_VBPORCH_DEFAULT << 20) /**< Shifted mode DEFAULT for EBI_TFTVPORCH */

/* Bit fields for EBI TFTTIMING */
#define _EBI_TFTTIMING_RESETVALUE                    0x00000000UL                             /**< Default value for EBI_TFTTIMING */
#define _EBI_TFTTIMING_MASK                          0x77FFFFFFUL                             /**< Mask for EBI_TFTTIMING */
#define _EBI_TFTTIMING_DCLKPERIOD_SHIFT              0                                        /**< Shift value for EBI_DCLKPERIOD */
#define _EBI_TFTTIMING_DCLKPERIOD_MASK               0xFFFUL                                  /**< Bit mask for EBI_DCLKPERIOD */
#define _EBI_TFTTIMING_DCLKPERIOD_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for EBI_TFTTIMING */
#define EBI_TFTTIMING_DCLKPERIOD_DEFAULT             (_EBI_TFTTIMING_DCLKPERIOD_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_TFTTIMING */
#define _EBI_TFTTIMING_TFTSTART_SHIFT                12                                       /**< Shift value for EBI_TFTSTART */
#define _EBI_TFTTIMING_TFTSTART_MASK                 0xFFF000UL                               /**< Bit mask for EBI_TFTSTART */
#define _EBI_TFTTIMING_TFTSTART_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for EBI_TFTTIMING */
#define EBI_TFTTIMING_TFTSTART_DEFAULT               (_EBI_TFTTIMING_TFTSTART_DEFAULT << 12)  /**< Shifted mode DEFAULT for EBI_TFTTIMING */
#define _EBI_TFTTIMING_TFTSETUP_SHIFT                24                                       /**< Shift value for EBI_TFTSETUP */
#define _EBI_TFTTIMING_TFTSETUP_MASK                 0x7000000UL                              /**< Bit mask for EBI_TFTSETUP */
#define _EBI_TFTTIMING_TFTSETUP_DEFAULT              0x00000000UL                             /**< Mode DEFAULT for EBI_TFTTIMING */
#define EBI_TFTTIMING_TFTSETUP_DEFAULT               (_EBI_TFTTIMING_TFTSETUP_DEFAULT << 24)  /**< Shifted mode DEFAULT for EBI_TFTTIMING */
#define _EBI_TFTTIMING_TFTHOLD_SHIFT                 28                                       /**< Shift value for EBI_TFTHOLD */
#define _EBI_TFTTIMING_TFTHOLD_MASK                  0x70000000UL                             /**< Bit mask for EBI_TFTHOLD */
#define _EBI_TFTTIMING_TFTHOLD_DEFAULT               0x00000000UL                             /**< Mode DEFAULT for EBI_TFTTIMING */
#define EBI_TFTTIMING_TFTHOLD_DEFAULT                (_EBI_TFTTIMING_TFTHOLD_DEFAULT << 28)   /**< Shifted mode DEFAULT for EBI_TFTTIMING */

/* Bit fields for EBI TFTPOLARITY */
#define _EBI_TFTPOLARITY_RESETVALUE                  0x00000000UL                                  /**< Default value for EBI_TFTPOLARITY */
#define _EBI_TFTPOLARITY_MASK                        0x0000001FUL                                  /**< Mask for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_CSPOL                        (0x1UL << 0)                                  /**< TFT Chip Select Polarity */
#define _EBI_TFTPOLARITY_CSPOL_SHIFT                 0                                             /**< Shift value for EBI_CSPOL */
#define _EBI_TFTPOLARITY_CSPOL_MASK                  0x1UL                                         /**< Bit mask for EBI_CSPOL */
#define _EBI_TFTPOLARITY_CSPOL_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for EBI_TFTPOLARITY */
#define _EBI_TFTPOLARITY_CSPOL_ACTIVELOW             0x00000000UL                                  /**< Mode ACTIVELOW for EBI_TFTPOLARITY */
#define _EBI_TFTPOLARITY_CSPOL_ACTIVEHIGH            0x00000001UL                                  /**< Mode ACTIVEHIGH for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_CSPOL_DEFAULT                (_EBI_TFTPOLARITY_CSPOL_DEFAULT << 0)         /**< Shifted mode DEFAULT for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_CSPOL_ACTIVELOW              (_EBI_TFTPOLARITY_CSPOL_ACTIVELOW << 0)       /**< Shifted mode ACTIVELOW for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_CSPOL_ACTIVEHIGH             (_EBI_TFTPOLARITY_CSPOL_ACTIVEHIGH << 0)      /**< Shifted mode ACTIVEHIGH for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_DCLKPOL                      (0x1UL << 1)                                  /**< TFT DCLK Polarity */
#define _EBI_TFTPOLARITY_DCLKPOL_SHIFT               1                                             /**< Shift value for EBI_DCLKPOL */
#define _EBI_TFTPOLARITY_DCLKPOL_MASK                0x2UL                                         /**< Bit mask for EBI_DCLKPOL */
#define _EBI_TFTPOLARITY_DCLKPOL_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for EBI_TFTPOLARITY */
#define _EBI_TFTPOLARITY_DCLKPOL_ACTIVEFALLING       0x00000000UL                                  /**< Mode ACTIVEFALLING for EBI_TFTPOLARITY */
#define _EBI_TFTPOLARITY_DCLKPOL_ACTIVERISING        0x00000001UL                                  /**< Mode ACTIVERISING for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_DCLKPOL_DEFAULT              (_EBI_TFTPOLARITY_DCLKPOL_DEFAULT << 1)       /**< Shifted mode DEFAULT for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_DCLKPOL_ACTIVEFALLING        (_EBI_TFTPOLARITY_DCLKPOL_ACTIVEFALLING << 1) /**< Shifted mode ACTIVEFALLING for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_DCLKPOL_ACTIVERISING         (_EBI_TFTPOLARITY_DCLKPOL_ACTIVERISING << 1)  /**< Shifted mode ACTIVERISING for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_DATAENPOL                    (0x1UL << 2)                                  /**< TFT DATAEN Polarity */
#define _EBI_TFTPOLARITY_DATAENPOL_SHIFT             2                                             /**< Shift value for EBI_DATAENPOL */
#define _EBI_TFTPOLARITY_DATAENPOL_MASK              0x4UL                                         /**< Bit mask for EBI_DATAENPOL */
#define _EBI_TFTPOLARITY_DATAENPOL_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for EBI_TFTPOLARITY */
#define _EBI_TFTPOLARITY_DATAENPOL_ACTIVELOW         0x00000000UL                                  /**< Mode ACTIVELOW for EBI_TFTPOLARITY */
#define _EBI_TFTPOLARITY_DATAENPOL_ACTIVEHIGH        0x00000001UL                                  /**< Mode ACTIVEHIGH for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_DATAENPOL_DEFAULT            (_EBI_TFTPOLARITY_DATAENPOL_DEFAULT << 2)     /**< Shifted mode DEFAULT for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_DATAENPOL_ACTIVELOW          (_EBI_TFTPOLARITY_DATAENPOL_ACTIVELOW << 2)   /**< Shifted mode ACTIVELOW for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_DATAENPOL_ACTIVEHIGH         (_EBI_TFTPOLARITY_DATAENPOL_ACTIVEHIGH << 2)  /**< Shifted mode ACTIVEHIGH for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_HSYNCPOL                     (0x1UL << 3)                                  /**< Address Latch Polarity */
#define _EBI_TFTPOLARITY_HSYNCPOL_SHIFT              3                                             /**< Shift value for EBI_HSYNCPOL */
#define _EBI_TFTPOLARITY_HSYNCPOL_MASK               0x8UL                                         /**< Bit mask for EBI_HSYNCPOL */
#define _EBI_TFTPOLARITY_HSYNCPOL_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for EBI_TFTPOLARITY */
#define _EBI_TFTPOLARITY_HSYNCPOL_ACTIVELOW          0x00000000UL                                  /**< Mode ACTIVELOW for EBI_TFTPOLARITY */
#define _EBI_TFTPOLARITY_HSYNCPOL_ACTIVEHIGH         0x00000001UL                                  /**< Mode ACTIVEHIGH for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_HSYNCPOL_DEFAULT             (_EBI_TFTPOLARITY_HSYNCPOL_DEFAULT << 3)      /**< Shifted mode DEFAULT for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_HSYNCPOL_ACTIVELOW           (_EBI_TFTPOLARITY_HSYNCPOL_ACTIVELOW << 3)    /**< Shifted mode ACTIVELOW for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_HSYNCPOL_ACTIVEHIGH          (_EBI_TFTPOLARITY_HSYNCPOL_ACTIVEHIGH << 3)   /**< Shifted mode ACTIVEHIGH for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_VSYNCPOL                     (0x1UL << 4)                                  /**< VSYNC Polarity */
#define _EBI_TFTPOLARITY_VSYNCPOL_SHIFT              4                                             /**< Shift value for EBI_VSYNCPOL */
#define _EBI_TFTPOLARITY_VSYNCPOL_MASK               0x10UL                                        /**< Bit mask for EBI_VSYNCPOL */
#define _EBI_TFTPOLARITY_VSYNCPOL_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for EBI_TFTPOLARITY */
#define _EBI_TFTPOLARITY_VSYNCPOL_ACTIVELOW          0x00000000UL                                  /**< Mode ACTIVELOW for EBI_TFTPOLARITY */
#define _EBI_TFTPOLARITY_VSYNCPOL_ACTIVEHIGH         0x00000001UL                                  /**< Mode ACTIVEHIGH for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_VSYNCPOL_DEFAULT             (_EBI_TFTPOLARITY_VSYNCPOL_DEFAULT << 4)      /**< Shifted mode DEFAULT for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_VSYNCPOL_ACTIVELOW           (_EBI_TFTPOLARITY_VSYNCPOL_ACTIVELOW << 4)    /**< Shifted mode ACTIVELOW for EBI_TFTPOLARITY */
#define EBI_TFTPOLARITY_VSYNCPOL_ACTIVEHIGH          (_EBI_TFTPOLARITY_VSYNCPOL_ACTIVEHIGH << 4)   /**< Shifted mode ACTIVEHIGH for EBI_TFTPOLARITY */

/* Bit fields for EBI TFTDD */
#define _EBI_TFTDD_RESETVALUE                        0x00000000UL                   /**< Default value for EBI_TFTDD */
#define _EBI_TFTDD_MASK                              0x00FFFFFFUL                   /**< Mask for EBI_TFTDD */
#define _EBI_TFTDD_DATA_SHIFT                        0                              /**< Shift value for EBI_DATA */
#define _EBI_TFTDD_DATA_MASK                         0xFFFFFFUL                     /**< Bit mask for EBI_DATA */
#define _EBI_TFTDD_DATA_DEFAULT                      0x00000000UL                   /**< Mode DEFAULT for EBI_TFTDD */
#define EBI_TFTDD_DATA_DEFAULT                       (_EBI_TFTDD_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_TFTDD */

/* Bit fields for EBI TFTALPHA */
#define _EBI_TFTALPHA_RESETVALUE                     0x00000000UL                       /**< Default value for EBI_TFTALPHA */
#define _EBI_TFTALPHA_MASK                           0x000001FFUL                       /**< Mask for EBI_TFTALPHA */
#define _EBI_TFTALPHA_ALPHA_SHIFT                    0                                  /**< Shift value for EBI_ALPHA */
#define _EBI_TFTALPHA_ALPHA_MASK                     0x1FFUL                            /**< Bit mask for EBI_ALPHA */
#define _EBI_TFTALPHA_ALPHA_DEFAULT                  0x00000000UL                       /**< Mode DEFAULT for EBI_TFTALPHA */
#define EBI_TFTALPHA_ALPHA_DEFAULT                   (_EBI_TFTALPHA_ALPHA_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_TFTALPHA */

/* Bit fields for EBI TFTPIXEL0 */
#define _EBI_TFTPIXEL0_RESETVALUE                    0x00000000UL                       /**< Default value for EBI_TFTPIXEL0 */
#define _EBI_TFTPIXEL0_MASK                          0x00FFFFFFUL                       /**< Mask for EBI_TFTPIXEL0 */
#define _EBI_TFTPIXEL0_DATA_SHIFT                    0                                  /**< Shift value for EBI_DATA */
#define _EBI_TFTPIXEL0_DATA_MASK                     0xFFFFFFUL                         /**< Bit mask for EBI_DATA */
#define _EBI_TFTPIXEL0_DATA_DEFAULT                  0x00000000UL                       /**< Mode DEFAULT for EBI_TFTPIXEL0 */
#define EBI_TFTPIXEL0_DATA_DEFAULT                   (_EBI_TFTPIXEL0_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_TFTPIXEL0 */

/* Bit fields for EBI TFTPIXEL1 */
#define _EBI_TFTPIXEL1_RESETVALUE                    0x00000000UL                       /**< Default value for EBI_TFTPIXEL1 */
#define _EBI_TFTPIXEL1_MASK                          0x00FFFFFFUL                       /**< Mask for EBI_TFTPIXEL1 */
#define _EBI_TFTPIXEL1_DATA_SHIFT                    0                                  /**< Shift value for EBI_DATA */
#define _EBI_TFTPIXEL1_DATA_MASK                     0xFFFFFFUL                         /**< Bit mask for EBI_DATA */
#define _EBI_TFTPIXEL1_DATA_DEFAULT                  0x00000000UL                       /**< Mode DEFAULT for EBI_TFTPIXEL1 */
#define EBI_TFTPIXEL1_DATA_DEFAULT                   (_EBI_TFTPIXEL1_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_TFTPIXEL1 */

/* Bit fields for EBI TFTPIXEL */
#define _EBI_TFTPIXEL_RESETVALUE                     0x00000000UL                      /**< Default value for EBI_TFTPIXEL */
#define _EBI_TFTPIXEL_MASK                           0x00FFFFFFUL                      /**< Mask for EBI_TFTPIXEL */
#define _EBI_TFTPIXEL_DATA_SHIFT                     0                                 /**< Shift value for EBI_DATA */
#define _EBI_TFTPIXEL_DATA_MASK                      0xFFFFFFUL                        /**< Bit mask for EBI_DATA */
#define _EBI_TFTPIXEL_DATA_DEFAULT                   0x00000000UL                      /**< Mode DEFAULT for EBI_TFTPIXEL */
#define EBI_TFTPIXEL_DATA_DEFAULT                    (_EBI_TFTPIXEL_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_TFTPIXEL */

/* Bit fields for EBI TFTMASK */
#define _EBI_TFTMASK_RESETVALUE                      0x00000000UL                        /**< Default value for EBI_TFTMASK */
#define _EBI_TFTMASK_MASK                            0x00FFFFFFUL                        /**< Mask for EBI_TFTMASK */
#define _EBI_TFTMASK_TFTMASK_SHIFT                   0                                   /**< Shift value for EBI_TFTMASK */
#define _EBI_TFTMASK_TFTMASK_MASK                    0xFFFFFFUL                          /**< Bit mask for EBI_TFTMASK */
#define _EBI_TFTMASK_TFTMASK_DEFAULT                 0x00000000UL                        /**< Mode DEFAULT for EBI_TFTMASK */
#define EBI_TFTMASK_TFTMASK_DEFAULT                  (_EBI_TFTMASK_TFTMASK_DEFAULT << 0) /**< Shifted mode DEFAULT for EBI_TFTMASK */

/* Bit fields for EBI IF */
#define _EBI_IF_RESETVALUE                           0x00000000UL                          /**< Default value for EBI_IF */
#define _EBI_IF_MASK                                 0x000003FFUL                          /**< Mask for EBI_IF */
#define EBI_IF_VSYNC                                 (0x1UL << 0)                          /**< Vertical Sync Interrupt Flag */
#define _EBI_IF_VSYNC_SHIFT                          0                                     /**< Shift value for EBI_VSYNC */
#define _EBI_IF_VSYNC_MASK                           0x1UL                                 /**< Bit mask for EBI_VSYNC */
#define _EBI_IF_VSYNC_DEFAULT                        0x00000000UL                          /**< Mode DEFAULT for EBI_IF */
#define EBI_IF_VSYNC_DEFAULT                         (_EBI_IF_VSYNC_DEFAULT << 0)          /**< Shifted mode DEFAULT for EBI_IF */
#define EBI_IF_HSYNC                                 (0x1UL << 1)                          /**< Horizontal Sync Interrupt Flag */
#define _EBI_IF_HSYNC_SHIFT                          1                                     /**< Shift value for EBI_HSYNC */
#define _EBI_IF_HSYNC_MASK                           0x2UL                                 /**< Bit mask for EBI_HSYNC */
#define _EBI_IF_HSYNC_DEFAULT                        0x00000000UL                          /**< Mode DEFAULT for EBI_IF */
#define EBI_IF_HSYNC_DEFAULT                         (_EBI_IF_HSYNC_DEFAULT << 1)          /**< Shifted mode DEFAULT for EBI_IF */
#define EBI_IF_VBPORCH                               (0x1UL << 2)                          /**< Vertical Back Porch Interrupt Flag */
#define _EBI_IF_VBPORCH_SHIFT                        2                                     /**< Shift value for EBI_VBPORCH */
#define _EBI_IF_VBPORCH_MASK                         0x4UL                                 /**< Bit mask for EBI_VBPORCH */
#define _EBI_IF_VBPORCH_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for EBI_IF */
#define EBI_IF_VBPORCH_DEFAULT                       (_EBI_IF_VBPORCH_DEFAULT << 2)        /**< Shifted mode DEFAULT for EBI_IF */
#define EBI_IF_VFPORCH                               (0x1UL << 3)                          /**< Vertical Front Porch Interrupt Flag */
#define _EBI_IF_VFPORCH_SHIFT                        3                                     /**< Shift value for EBI_VFPORCH */
#define _EBI_IF_VFPORCH_MASK                         0x8UL                                 /**< Bit mask for EBI_VFPORCH */
#define _EBI_IF_VFPORCH_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for EBI_IF */
#define EBI_IF_VFPORCH_DEFAULT                       (_EBI_IF_VFPORCH_DEFAULT << 3)        /**< Shifted mode DEFAULT for EBI_IF */
#define EBI_IF_DDEMPTY                               (0x1UL << 4)                          /**< Direct Drive Data Empty Interrupt Flag */
#define _EBI_IF_DDEMPTY_SHIFT                        4                                     /**< Shift value for EBI_DDEMPTY */
#define _EBI_IF_DDEMPTY_MASK                         0x10UL                                /**< Bit mask for EBI_DDEMPTY */
#define _EBI_IF_DDEMPTY_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for EBI_IF */
#define EBI_IF_DDEMPTY_DEFAULT                       (_EBI_IF_DDEMPTY_DEFAULT << 4)        /**< Shifted mode DEFAULT for EBI_IF */
#define EBI_IF_DDJIT                                 (0x1UL << 5)                          /**< Direct Drive Jitter Interrupt Flag */
#define _EBI_IF_DDJIT_SHIFT                          5                                     /**< Shift value for EBI_DDJIT */
#define _EBI_IF_DDJIT_MASK                           0x20UL                                /**< Bit mask for EBI_DDJIT */
#define _EBI_IF_DDJIT_DEFAULT                        0x00000000UL                          /**< Mode DEFAULT for EBI_IF */
#define EBI_IF_DDJIT_DEFAULT                         (_EBI_IF_DDJIT_DEFAULT << 5)          /**< Shifted mode DEFAULT for EBI_IF */
#define EBI_IF_TFTPIXEL0EMPTY                        (0x1UL << 6)                          /**< EBI_TFTPIXEL0 is Empty Interrupt Flag */
#define _EBI_IF_TFTPIXEL0EMPTY_SHIFT                 6                                     /**< Shift value for EBI_TFTPIXEL0EMPTY */
#define _EBI_IF_TFTPIXEL0EMPTY_MASK                  0x40UL                                /**< Bit mask for EBI_TFTPIXEL0EMPTY */
#define _EBI_IF_TFTPIXEL0EMPTY_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for EBI_IF */
#define EBI_IF_TFTPIXEL0EMPTY_DEFAULT                (_EBI_IF_TFTPIXEL0EMPTY_DEFAULT << 6) /**< Shifted mode DEFAULT for EBI_IF */
#define EBI_IF_TFTPIXEL1EMPTY                        (0x1UL << 7)                          /**< EBI_TFTPIXEL1 is Empty Interrupt Flag */
#define _EBI_IF_TFTPIXEL1EMPTY_SHIFT                 7                                     /**< Shift value for EBI_TFTPIXEL1EMPTY */
#define _EBI_IF_TFTPIXEL1EMPTY_MASK                  0x80UL                                /**< Bit mask for EBI_TFTPIXEL1EMPTY */
#define _EBI_IF_TFTPIXEL1EMPTY_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for EBI_IF */
#define EBI_IF_TFTPIXEL1EMPTY_DEFAULT                (_EBI_IF_TFTPIXEL1EMPTY_DEFAULT << 7) /**< Shifted mode DEFAULT for EBI_IF */
#define EBI_IF_TFTPIXELFULL                          (0x1UL << 8)                          /**< EBI_TFTPIXEL is Full Interrupt Flag */
#define _EBI_IF_TFTPIXELFULL_SHIFT                   8                                     /**< Shift value for EBI_TFTPIXELFULL */
#define _EBI_IF_TFTPIXELFULL_MASK                    0x100UL                               /**< Bit mask for EBI_TFTPIXELFULL */
#define _EBI_IF_TFTPIXELFULL_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for EBI_IF */
#define EBI_IF_TFTPIXELFULL_DEFAULT                  (_EBI_IF_TFTPIXELFULL_DEFAULT << 8)   /**< Shifted mode DEFAULT for EBI_IF */
#define EBI_IF_TFTPIXELOF                            (0x1UL << 9)                          /**< EBI_TFTPIXEL Register Overflow Interrupt Flag */
#define _EBI_IF_TFTPIXELOF_SHIFT                     9                                     /**< Shift value for EBI_TFTPIXELOF */
#define _EBI_IF_TFTPIXELOF_MASK                      0x200UL                               /**< Bit mask for EBI_TFTPIXELOF */
#define _EBI_IF_TFTPIXELOF_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for EBI_IF */
#define EBI_IF_TFTPIXELOF_DEFAULT                    (_EBI_IF_TFTPIXELOF_DEFAULT << 9)     /**< Shifted mode DEFAULT for EBI_IF */

/* Bit fields for EBI IFS */
#define _EBI_IFS_RESETVALUE                          0x00000000UL                           /**< Default value for EBI_IFS */
#define _EBI_IFS_MASK                                0x000003FFUL                           /**< Mask for EBI_IFS */
#define EBI_IFS_VSYNC                                (0x1UL << 0)                           /**< Vertical Sync Interrupt Flag Set */
#define _EBI_IFS_VSYNC_SHIFT                         0                                      /**< Shift value for EBI_VSYNC */
#define _EBI_IFS_VSYNC_MASK                          0x1UL                                  /**< Bit mask for EBI_VSYNC */
#define _EBI_IFS_VSYNC_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for EBI_IFS */
#define EBI_IFS_VSYNC_DEFAULT                        (_EBI_IFS_VSYNC_DEFAULT << 0)          /**< Shifted mode DEFAULT for EBI_IFS */
#define EBI_IFS_HSYNC                                (0x1UL << 1)                           /**< Horizontal Sync Interrupt Flag Set */
#define _EBI_IFS_HSYNC_SHIFT                         1                                      /**< Shift value for EBI_HSYNC */
#define _EBI_IFS_HSYNC_MASK                          0x2UL                                  /**< Bit mask for EBI_HSYNC */
#define _EBI_IFS_HSYNC_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for EBI_IFS */
#define EBI_IFS_HSYNC_DEFAULT                        (_EBI_IFS_HSYNC_DEFAULT << 1)          /**< Shifted mode DEFAULT for EBI_IFS */
#define EBI_IFS_VBPORCH                              (0x1UL << 2)                           /**< Vertical Back Porch Interrupt Flag Set */
#define _EBI_IFS_VBPORCH_SHIFT                       2                                      /**< Shift value for EBI_VBPORCH */
#define _EBI_IFS_VBPORCH_MASK                        0x4UL                                  /**< Bit mask for EBI_VBPORCH */
#define _EBI_IFS_VBPORCH_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for EBI_IFS */
#define EBI_IFS_VBPORCH_DEFAULT                      (_EBI_IFS_VBPORCH_DEFAULT << 2)        /**< Shifted mode DEFAULT for EBI_IFS */
#define EBI_IFS_VFPORCH                              (0x1UL << 3)                           /**< Vertical Front Porch Interrupt Flag Set */
#define _EBI_IFS_VFPORCH_SHIFT                       3                                      /**< Shift value for EBI_VFPORCH */
#define _EBI_IFS_VFPORCH_MASK                        0x8UL                                  /**< Bit mask for EBI_VFPORCH */
#define _EBI_IFS_VFPORCH_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for EBI_IFS */
#define EBI_IFS_VFPORCH_DEFAULT                      (_EBI_IFS_VFPORCH_DEFAULT << 3)        /**< Shifted mode DEFAULT for EBI_IFS */
#define EBI_IFS_DDEMPTY                              (0x1UL << 4)                           /**< Direct Drive Data Empty Interrupt Flag Set */
#define _EBI_IFS_DDEMPTY_SHIFT                       4                                      /**< Shift value for EBI_DDEMPTY */
#define _EBI_IFS_DDEMPTY_MASK                        0x10UL                                 /**< Bit mask for EBI_DDEMPTY */
#define _EBI_IFS_DDEMPTY_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for EBI_IFS */
#define EBI_IFS_DDEMPTY_DEFAULT                      (_EBI_IFS_DDEMPTY_DEFAULT << 4)        /**< Shifted mode DEFAULT for EBI_IFS */
#define EBI_IFS_DDJIT                                (0x1UL << 5)                           /**< Direct Drive Jitter Interrupt Flag Set */
#define _EBI_IFS_DDJIT_SHIFT                         5                                      /**< Shift value for EBI_DDJIT */
#define _EBI_IFS_DDJIT_MASK                          0x20UL                                 /**< Bit mask for EBI_DDJIT */
#define _EBI_IFS_DDJIT_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for EBI_IFS */
#define EBI_IFS_DDJIT_DEFAULT                        (_EBI_IFS_DDJIT_DEFAULT << 5)          /**< Shifted mode DEFAULT for EBI_IFS */
#define EBI_IFS_TFTPIXEL0EMPTY                       (0x1UL << 6)                           /**< EBI_TFTPIXEL0 Empty Interrupt Flag Set */
#define _EBI_IFS_TFTPIXEL0EMPTY_SHIFT                6                                      /**< Shift value for EBI_TFTPIXEL0EMPTY */
#define _EBI_IFS_TFTPIXEL0EMPTY_MASK                 0x40UL                                 /**< Bit mask for EBI_TFTPIXEL0EMPTY */
#define _EBI_IFS_TFTPIXEL0EMPTY_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for EBI_IFS */
#define EBI_IFS_TFTPIXEL0EMPTY_DEFAULT               (_EBI_IFS_TFTPIXEL0EMPTY_DEFAULT << 6) /**< Shifted mode DEFAULT for EBI_IFS */
#define EBI_IFS_TFTPIXEL1EMPTY                       (0x1UL << 7)                           /**< EBI_TFTPIXEL1 Empty Interrupt Flag Set */
#define _EBI_IFS_TFTPIXEL1EMPTY_SHIFT                7                                      /**< Shift value for EBI_TFTPIXEL1EMPTY */
#define _EBI_IFS_TFTPIXEL1EMPTY_MASK                 0x80UL                                 /**< Bit mask for EBI_TFTPIXEL1EMPTY */
#define _EBI_IFS_TFTPIXEL1EMPTY_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for EBI_IFS */
#define EBI_IFS_TFTPIXEL1EMPTY_DEFAULT               (_EBI_IFS_TFTPIXEL1EMPTY_DEFAULT << 7) /**< Shifted mode DEFAULT for EBI_IFS */
#define EBI_IFS_TFTPIXELFULL                         (0x1UL << 8)                           /**< EBI_TFTPIXEL Full Interrupt Flag Set */
#define _EBI_IFS_TFTPIXELFULL_SHIFT                  8                                      /**< Shift value for EBI_TFTPIXELFULL */
#define _EBI_IFS_TFTPIXELFULL_MASK                   0x100UL                                /**< Bit mask for EBI_TFTPIXELFULL */
#define _EBI_IFS_TFTPIXELFULL_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for EBI_IFS */
#define EBI_IFS_TFTPIXELFULL_DEFAULT                 (_EBI_IFS_TFTPIXELFULL_DEFAULT << 8)   /**< Shifted mode DEFAULT for EBI_IFS */
#define EBI_IFS_TFTPIXELOF                           (0x1UL << 9)                           /**< EBI_TFTPIXEL Overflow Interrupt Flag Set */
#define _EBI_IFS_TFTPIXELOF_SHIFT                    9                                      /**< Shift value for EBI_TFTPIXELOF */
#define _EBI_IFS_TFTPIXELOF_MASK                     0x200UL                                /**< Bit mask for EBI_TFTPIXELOF */
#define _EBI_IFS_TFTPIXELOF_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for EBI_IFS */
#define EBI_IFS_TFTPIXELOF_DEFAULT                   (_EBI_IFS_TFTPIXELOF_DEFAULT << 9)     /**< Shifted mode DEFAULT for EBI_IFS */

/* Bit fields for EBI IFC */
#define _EBI_IFC_RESETVALUE                          0x00000000UL                           /**< Default value for EBI_IFC */
#define _EBI_IFC_MASK                                0x000003FFUL                           /**< Mask for EBI_IFC */
#define EBI_IFC_VSYNC                                (0x1UL << 0)                           /**< Vertical Sync Interrupt Flag Clear */
#define _EBI_IFC_VSYNC_SHIFT                         0                                      /**< Shift value for EBI_VSYNC */
#define _EBI_IFC_VSYNC_MASK                          0x1UL                                  /**< Bit mask for EBI_VSYNC */
#define _EBI_IFC_VSYNC_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for EBI_IFC */
#define EBI_IFC_VSYNC_DEFAULT                        (_EBI_IFC_VSYNC_DEFAULT << 0)          /**< Shifted mode DEFAULT for EBI_IFC */
#define EBI_IFC_HSYNC                                (0x1UL << 1)                           /**< Horizontal Sync Interrupt Flag Clear */
#define _EBI_IFC_HSYNC_SHIFT                         1                                      /**< Shift value for EBI_HSYNC */
#define _EBI_IFC_HSYNC_MASK                          0x2UL                                  /**< Bit mask for EBI_HSYNC */
#define _EBI_IFC_HSYNC_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for EBI_IFC */
#define EBI_IFC_HSYNC_DEFAULT                        (_EBI_IFC_HSYNC_DEFAULT << 1)          /**< Shifted mode DEFAULT for EBI_IFC */
#define EBI_IFC_VBPORCH                              (0x1UL << 2)                           /**< Vertical Back Porch Interrupt Flag Clear */
#define _EBI_IFC_VBPORCH_SHIFT                       2                                      /**< Shift value for EBI_VBPORCH */
#define _EBI_IFC_VBPORCH_MASK                        0x4UL                                  /**< Bit mask for EBI_VBPORCH */
#define _EBI_IFC_VBPORCH_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for EBI_IFC */
#define EBI_IFC_VBPORCH_DEFAULT                      (_EBI_IFC_VBPORCH_DEFAULT << 2)        /**< Shifted mode DEFAULT for EBI_IFC */
#define EBI_IFC_VFPORCH                              (0x1UL << 3)                           /**< Vertical Front Porch Interrupt Flag Clear */
#define _EBI_IFC_VFPORCH_SHIFT                       3                                      /**< Shift value for EBI_VFPORCH */
#define _EBI_IFC_VFPORCH_MASK                        0x8UL                                  /**< Bit mask for EBI_VFPORCH */
#define _EBI_IFC_VFPORCH_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for EBI_IFC */
#define EBI_IFC_VFPORCH_DEFAULT                      (_EBI_IFC_VFPORCH_DEFAULT << 3)        /**< Shifted mode DEFAULT for EBI_IFC */
#define EBI_IFC_DDEMPTY                              (0x1UL << 4)                           /**< Direct Drive Data Empty Interrupt Flag Clear */
#define _EBI_IFC_DDEMPTY_SHIFT                       4                                      /**< Shift value for EBI_DDEMPTY */
#define _EBI_IFC_DDEMPTY_MASK                        0x10UL                                 /**< Bit mask for EBI_DDEMPTY */
#define _EBI_IFC_DDEMPTY_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for EBI_IFC */
#define EBI_IFC_DDEMPTY_DEFAULT                      (_EBI_IFC_DDEMPTY_DEFAULT << 4)        /**< Shifted mode DEFAULT for EBI_IFC */
#define EBI_IFC_DDJIT                                (0x1UL << 5)                           /**< Direct Drive Jitter Interrupt Flag Clear */
#define _EBI_IFC_DDJIT_SHIFT                         5                                      /**< Shift value for EBI_DDJIT */
#define _EBI_IFC_DDJIT_MASK                          0x20UL                                 /**< Bit mask for EBI_DDJIT */
#define _EBI_IFC_DDJIT_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for EBI_IFC */
#define EBI_IFC_DDJIT_DEFAULT                        (_EBI_IFC_DDJIT_DEFAULT << 5)          /**< Shifted mode DEFAULT for EBI_IFC */
#define EBI_IFC_TFTPIXEL0EMPTY                       (0x1UL << 6)                           /**< EBI_TFTPIXEL0 Empty Interrupt Flag Clear */
#define _EBI_IFC_TFTPIXEL0EMPTY_SHIFT                6                                      /**< Shift value for EBI_TFTPIXEL0EMPTY */
#define _EBI_IFC_TFTPIXEL0EMPTY_MASK                 0x40UL                                 /**< Bit mask for EBI_TFTPIXEL0EMPTY */
#define _EBI_IFC_TFTPIXEL0EMPTY_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for EBI_IFC */
#define EBI_IFC_TFTPIXEL0EMPTY_DEFAULT               (_EBI_IFC_TFTPIXEL0EMPTY_DEFAULT << 6) /**< Shifted mode DEFAULT for EBI_IFC */
#define EBI_IFC_TFTPIXEL1EMPTY                       (0x1UL << 7)                           /**< EBI_TFTPIXEL1 Empty Interrupt Flag Clear */
#define _EBI_IFC_TFTPIXEL1EMPTY_SHIFT                7                                      /**< Shift value for EBI_TFTPIXEL1EMPTY */
#define _EBI_IFC_TFTPIXEL1EMPTY_MASK                 0x80UL                                 /**< Bit mask for EBI_TFTPIXEL1EMPTY */
#define _EBI_IFC_TFTPIXEL1EMPTY_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for EBI_IFC */
#define EBI_IFC_TFTPIXEL1EMPTY_DEFAULT               (_EBI_IFC_TFTPIXEL1EMPTY_DEFAULT << 7) /**< Shifted mode DEFAULT for EBI_IFC */
#define EBI_IFC_TFTPIXELFULL                         (0x1UL << 8)                           /**< EBI_TFTPIXEL Full Interrupt Flag Clear */
#define _EBI_IFC_TFTPIXELFULL_SHIFT                  8                                      /**< Shift value for EBI_TFTPIXELFULL */
#define _EBI_IFC_TFTPIXELFULL_MASK                   0x100UL                                /**< Bit mask for EBI_TFTPIXELFULL */
#define _EBI_IFC_TFTPIXELFULL_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for EBI_IFC */
#define EBI_IFC_TFTPIXELFULL_DEFAULT                 (_EBI_IFC_TFTPIXELFULL_DEFAULT << 8)   /**< Shifted mode DEFAULT for EBI_IFC */
#define EBI_IFC_TFTPIXELOF                           (0x1UL << 9)                           /**< EBI_TFTPIXEL Overflow Interrupt Flag Clear */
#define _EBI_IFC_TFTPIXELOF_SHIFT                    9                                      /**< Shift value for EBI_TFTPIXELOF */
#define _EBI_IFC_TFTPIXELOF_MASK                     0x200UL                                /**< Bit mask for EBI_TFTPIXELOF */
#define _EBI_IFC_TFTPIXELOF_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for EBI_IFC */
#define EBI_IFC_TFTPIXELOF_DEFAULT                   (_EBI_IFC_TFTPIXELOF_DEFAULT << 9)     /**< Shifted mode DEFAULT for EBI_IFC */

/* Bit fields for EBI IEN */
#define _EBI_IEN_RESETVALUE                          0x00000000UL                           /**< Default value for EBI_IEN */
#define _EBI_IEN_MASK                                0x000003FFUL                           /**< Mask for EBI_IEN */
#define EBI_IEN_VSYNC                                (0x1UL << 0)                           /**< Vertical Sync Interrupt Enable */
#define _EBI_IEN_VSYNC_SHIFT                         0                                      /**< Shift value for EBI_VSYNC */
#define _EBI_IEN_VSYNC_MASK                          0x1UL                                  /**< Bit mask for EBI_VSYNC */
#define _EBI_IEN_VSYNC_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for EBI_IEN */
#define EBI_IEN_VSYNC_DEFAULT                        (_EBI_IEN_VSYNC_DEFAULT << 0)          /**< Shifted mode DEFAULT for EBI_IEN */
#define EBI_IEN_HSYNC                                (0x1UL << 1)                           /**< Horizontal Sync Interrupt Enable */
#define _EBI_IEN_HSYNC_SHIFT                         1                                      /**< Shift value for EBI_HSYNC */
#define _EBI_IEN_HSYNC_MASK                          0x2UL                                  /**< Bit mask for EBI_HSYNC */
#define _EBI_IEN_HSYNC_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for EBI_IEN */
#define EBI_IEN_HSYNC_DEFAULT                        (_EBI_IEN_HSYNC_DEFAULT << 1)          /**< Shifted mode DEFAULT for EBI_IEN */
#define EBI_IEN_VBPORCH                              (0x1UL << 2)                           /**< Vertical Back Porch Interrupt Enable */
#define _EBI_IEN_VBPORCH_SHIFT                       2                                      /**< Shift value for EBI_VBPORCH */
#define _EBI_IEN_VBPORCH_MASK                        0x4UL                                  /**< Bit mask for EBI_VBPORCH */
#define _EBI_IEN_VBPORCH_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for EBI_IEN */
#define EBI_IEN_VBPORCH_DEFAULT                      (_EBI_IEN_VBPORCH_DEFAULT << 2)        /**< Shifted mode DEFAULT for EBI_IEN */
#define EBI_IEN_VFPORCH                              (0x1UL << 3)                           /**< Vertical Front Porch Interrupt Enable */
#define _EBI_IEN_VFPORCH_SHIFT                       3                                      /**< Shift value for EBI_VFPORCH */
#define _EBI_IEN_VFPORCH_MASK                        0x8UL                                  /**< Bit mask for EBI_VFPORCH */
#define _EBI_IEN_VFPORCH_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for EBI_IEN */
#define EBI_IEN_VFPORCH_DEFAULT                      (_EBI_IEN_VFPORCH_DEFAULT << 3)        /**< Shifted mode DEFAULT for EBI_IEN */
#define EBI_IEN_DDEMPTY                              (0x1UL << 4)                           /**< Direct Drive Data Empty Interrupt Enable */
#define _EBI_IEN_DDEMPTY_SHIFT                       4                                      /**< Shift value for EBI_DDEMPTY */
#define _EBI_IEN_DDEMPTY_MASK                        0x10UL                                 /**< Bit mask for EBI_DDEMPTY */
#define _EBI_IEN_DDEMPTY_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for EBI_IEN */
#define EBI_IEN_DDEMPTY_DEFAULT                      (_EBI_IEN_DDEMPTY_DEFAULT << 4)        /**< Shifted mode DEFAULT for EBI_IEN */
#define EBI_IEN_DDJIT                                (0x1UL << 5)                           /**< Direct Drive Jitter Interrupt Enable */
#define _EBI_IEN_DDJIT_SHIFT                         5                                      /**< Shift value for EBI_DDJIT */
#define _EBI_IEN_DDJIT_MASK                          0x20UL                                 /**< Bit mask for EBI_DDJIT */
#define _EBI_IEN_DDJIT_DEFAULT                       0x00000000UL                           /**< Mode DEFAULT for EBI_IEN */
#define EBI_IEN_DDJIT_DEFAULT                        (_EBI_IEN_DDJIT_DEFAULT << 5)          /**< Shifted mode DEFAULT for EBI_IEN */
#define EBI_IEN_TFTPIXEL0EMPTY                       (0x1UL << 6)                           /**< EBI_TFTPIXEL0 Empty Interrupt Enable */
#define _EBI_IEN_TFTPIXEL0EMPTY_SHIFT                6                                      /**< Shift value for EBI_TFTPIXEL0EMPTY */
#define _EBI_IEN_TFTPIXEL0EMPTY_MASK                 0x40UL                                 /**< Bit mask for EBI_TFTPIXEL0EMPTY */
#define _EBI_IEN_TFTPIXEL0EMPTY_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for EBI_IEN */
#define EBI_IEN_TFTPIXEL0EMPTY_DEFAULT               (_EBI_IEN_TFTPIXEL0EMPTY_DEFAULT << 6) /**< Shifted mode DEFAULT for EBI_IEN */
#define EBI_IEN_TFTPIXEL1EMPTY                       (0x1UL << 7)                           /**< EBI_TFTPIXEL1 Empty Interrupt Enable */
#define _EBI_IEN_TFTPIXEL1EMPTY_SHIFT                7                                      /**< Shift value for EBI_TFTPIXEL1EMPTY */
#define _EBI_IEN_TFTPIXEL1EMPTY_MASK                 0x80UL                                 /**< Bit mask for EBI_TFTPIXEL1EMPTY */
#define _EBI_IEN_TFTPIXEL1EMPTY_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for EBI_IEN */
#define EBI_IEN_TFTPIXEL1EMPTY_DEFAULT               (_EBI_IEN_TFTPIXEL1EMPTY_DEFAULT << 7) /**< Shifted mode DEFAULT for EBI_IEN */
#define EBI_IEN_TFTPIXELFULL                         (0x1UL << 8)                           /**< EBI_TFTPIXEL Full Interrupt Enable */
#define _EBI_IEN_TFTPIXELFULL_SHIFT                  8                                      /**< Shift value for EBI_TFTPIXELFULL */
#define _EBI_IEN_TFTPIXELFULL_MASK                   0x100UL                                /**< Bit mask for EBI_TFTPIXELFULL */
#define _EBI_IEN_TFTPIXELFULL_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for EBI_IEN */
#define EBI_IEN_TFTPIXELFULL_DEFAULT                 (_EBI_IEN_TFTPIXELFULL_DEFAULT << 8)   /**< Shifted mode DEFAULT for EBI_IEN */
#define EBI_IEN_TFTPIXELOF                           (0x1UL << 9)                           /**< EBI_TFTPIXEL Overflow Interrupt Enable */
#define _EBI_IEN_TFTPIXELOF_SHIFT                    9                                      /**< Shift value for EBI_TFTPIXELOF */
#define _EBI_IEN_TFTPIXELOF_MASK                     0x200UL                                /**< Bit mask for EBI_TFTPIXELOF */
#define _EBI_IEN_TFTPIXELOF_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for EBI_IEN */
#define EBI_IEN_TFTPIXELOF_DEFAULT                   (_EBI_IEN_TFTPIXELOF_DEFAULT << 9)     /**< Shifted mode DEFAULT for EBI_IEN */

/* Bit fields for EBI ROUTEPEN */
#define _EBI_ROUTEPEN_RESETVALUE                     0x00000000UL                            /**< Default value for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_MASK                           0x077F10FFUL                            /**< Mask for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_EBIPEN                          (0x1UL << 0)                            /**< EBI Pin Enable */
#define _EBI_ROUTEPEN_EBIPEN_SHIFT                   0                                       /**< Shift value for EBI_EBIPEN */
#define _EBI_ROUTEPEN_EBIPEN_MASK                    0x1UL                                   /**< Bit mask for EBI_EBIPEN */
#define _EBI_ROUTEPEN_EBIPEN_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_EBIPEN_DEFAULT                  (_EBI_ROUTEPEN_EBIPEN_DEFAULT << 0)     /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_CS0PEN                          (0x1UL << 1)                            /**< EBI_CS0 Pin Enable */
#define _EBI_ROUTEPEN_CS0PEN_SHIFT                   1                                       /**< Shift value for EBI_CS0PEN */
#define _EBI_ROUTEPEN_CS0PEN_MASK                    0x2UL                                   /**< Bit mask for EBI_CS0PEN */
#define _EBI_ROUTEPEN_CS0PEN_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_CS0PEN_DEFAULT                  (_EBI_ROUTEPEN_CS0PEN_DEFAULT << 1)     /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_CS1PEN                          (0x1UL << 2)                            /**< EBI_CS1 Pin Enable */
#define _EBI_ROUTEPEN_CS1PEN_SHIFT                   2                                       /**< Shift value for EBI_CS1PEN */
#define _EBI_ROUTEPEN_CS1PEN_MASK                    0x4UL                                   /**< Bit mask for EBI_CS1PEN */
#define _EBI_ROUTEPEN_CS1PEN_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_CS1PEN_DEFAULT                  (_EBI_ROUTEPEN_CS1PEN_DEFAULT << 2)     /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_CS2PEN                          (0x1UL << 3)                            /**< EBI_CS2 Pin Enable */
#define _EBI_ROUTEPEN_CS2PEN_SHIFT                   3                                       /**< Shift value for EBI_CS2PEN */
#define _EBI_ROUTEPEN_CS2PEN_MASK                    0x8UL                                   /**< Bit mask for EBI_CS2PEN */
#define _EBI_ROUTEPEN_CS2PEN_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_CS2PEN_DEFAULT                  (_EBI_ROUTEPEN_CS2PEN_DEFAULT << 3)     /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_CS3PEN                          (0x1UL << 4)                            /**< EBI_CS3 Pin Enable */
#define _EBI_ROUTEPEN_CS3PEN_SHIFT                   4                                       /**< Shift value for EBI_CS3PEN */
#define _EBI_ROUTEPEN_CS3PEN_MASK                    0x10UL                                  /**< Bit mask for EBI_CS3PEN */
#define _EBI_ROUTEPEN_CS3PEN_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_CS3PEN_DEFAULT                  (_EBI_ROUTEPEN_CS3PEN_DEFAULT << 4)     /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_ALEPEN                          (0x1UL << 5)                            /**< EBI_ALE Pin Enable */
#define _EBI_ROUTEPEN_ALEPEN_SHIFT                   5                                       /**< Shift value for EBI_ALEPEN */
#define _EBI_ROUTEPEN_ALEPEN_MASK                    0x20UL                                  /**< Bit mask for EBI_ALEPEN */
#define _EBI_ROUTEPEN_ALEPEN_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_ALEPEN_DEFAULT                  (_EBI_ROUTEPEN_ALEPEN_DEFAULT << 5)     /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_ARDYPEN                         (0x1UL << 6)                            /**< EBI_ARDY Pin Enable */
#define _EBI_ROUTEPEN_ARDYPEN_SHIFT                  6                                       /**< Shift value for EBI_ARDYPEN */
#define _EBI_ROUTEPEN_ARDYPEN_MASK                   0x40UL                                  /**< Bit mask for EBI_ARDYPEN */
#define _EBI_ROUTEPEN_ARDYPEN_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_ARDYPEN_DEFAULT                 (_EBI_ROUTEPEN_ARDYPEN_DEFAULT << 6)    /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_BLPEN                           (0x1UL << 7)                            /**< EBI_BL[1:0] Pin Enable */
#define _EBI_ROUTEPEN_BLPEN_SHIFT                    7                                       /**< Shift value for EBI_BLPEN */
#define _EBI_ROUTEPEN_BLPEN_MASK                     0x80UL                                  /**< Bit mask for EBI_BLPEN */
#define _EBI_ROUTEPEN_BLPEN_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_BLPEN_DEFAULT                   (_EBI_ROUTEPEN_BLPEN_DEFAULT << 7)      /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_NANDPEN                         (0x1UL << 12)                           /**< NANDRE and NANDWE Pin Enable */
#define _EBI_ROUTEPEN_NANDPEN_SHIFT                  12                                      /**< Shift value for EBI_NANDPEN */
#define _EBI_ROUTEPEN_NANDPEN_MASK                   0x1000UL                                /**< Bit mask for EBI_NANDPEN */
#define _EBI_ROUTEPEN_NANDPEN_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_NANDPEN_DEFAULT                 (_EBI_ROUTEPEN_NANDPEN_DEFAULT << 12)   /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_ALB_SHIFT                      16                                      /**< Shift value for EBI_ALB */
#define _EBI_ROUTEPEN_ALB_MASK                       0x30000UL                               /**< Bit mask for EBI_ALB */
#define _EBI_ROUTEPEN_ALB_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_ALB_A0                         0x00000000UL                            /**< Mode A0 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_ALB_A8                         0x00000001UL                            /**< Mode A8 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_ALB_A16                        0x00000002UL                            /**< Mode A16 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_ALB_A24                        0x00000003UL                            /**< Mode A24 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_ALB_DEFAULT                     (_EBI_ROUTEPEN_ALB_DEFAULT << 16)       /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_ALB_A0                          (_EBI_ROUTEPEN_ALB_A0 << 16)            /**< Shifted mode A0 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_ALB_A8                          (_EBI_ROUTEPEN_ALB_A8 << 16)            /**< Shifted mode A8 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_ALB_A16                         (_EBI_ROUTEPEN_ALB_A16 << 16)           /**< Shifted mode A16 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_ALB_A24                         (_EBI_ROUTEPEN_ALB_A24 << 16)           /**< Shifted mode A24 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_SHIFT                     18                                      /**< Shift value for EBI_APEN */
#define _EBI_ROUTEPEN_APEN_MASK                      0x7C0000UL                              /**< Bit mask for EBI_APEN */
#define _EBI_ROUTEPEN_APEN_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A0                        0x00000000UL                            /**< Mode A0 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A5                        0x00000005UL                            /**< Mode A5 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A6                        0x00000006UL                            /**< Mode A6 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A7                        0x00000007UL                            /**< Mode A7 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A8                        0x00000008UL                            /**< Mode A8 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A9                        0x00000009UL                            /**< Mode A9 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A10                       0x0000000AUL                            /**< Mode A10 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A11                       0x0000000BUL                            /**< Mode A11 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A12                       0x0000000CUL                            /**< Mode A12 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A13                       0x0000000DUL                            /**< Mode A13 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A14                       0x0000000EUL                            /**< Mode A14 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A15                       0x0000000FUL                            /**< Mode A15 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A16                       0x00000010UL                            /**< Mode A16 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A17                       0x00000011UL                            /**< Mode A17 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A18                       0x00000012UL                            /**< Mode A18 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A19                       0x00000013UL                            /**< Mode A19 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A20                       0x00000014UL                            /**< Mode A20 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A21                       0x00000015UL                            /**< Mode A21 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A22                       0x00000016UL                            /**< Mode A22 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A23                       0x00000017UL                            /**< Mode A23 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A24                       0x00000018UL                            /**< Mode A24 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A25                       0x00000019UL                            /**< Mode A25 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A26                       0x0000001AUL                            /**< Mode A26 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A27                       0x0000001BUL                            /**< Mode A27 for EBI_ROUTEPEN */
#define _EBI_ROUTEPEN_APEN_A28                       0x0000001CUL                            /**< Mode A28 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_DEFAULT                    (_EBI_ROUTEPEN_APEN_DEFAULT << 18)      /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A0                         (_EBI_ROUTEPEN_APEN_A0 << 18)           /**< Shifted mode A0 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A5                         (_EBI_ROUTEPEN_APEN_A5 << 18)           /**< Shifted mode A5 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A6                         (_EBI_ROUTEPEN_APEN_A6 << 18)           /**< Shifted mode A6 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A7                         (_EBI_ROUTEPEN_APEN_A7 << 18)           /**< Shifted mode A7 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A8                         (_EBI_ROUTEPEN_APEN_A8 << 18)           /**< Shifted mode A8 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A9                         (_EBI_ROUTEPEN_APEN_A9 << 18)           /**< Shifted mode A9 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A10                        (_EBI_ROUTEPEN_APEN_A10 << 18)          /**< Shifted mode A10 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A11                        (_EBI_ROUTEPEN_APEN_A11 << 18)          /**< Shifted mode A11 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A12                        (_EBI_ROUTEPEN_APEN_A12 << 18)          /**< Shifted mode A12 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A13                        (_EBI_ROUTEPEN_APEN_A13 << 18)          /**< Shifted mode A13 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A14                        (_EBI_ROUTEPEN_APEN_A14 << 18)          /**< Shifted mode A14 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A15                        (_EBI_ROUTEPEN_APEN_A15 << 18)          /**< Shifted mode A15 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A16                        (_EBI_ROUTEPEN_APEN_A16 << 18)          /**< Shifted mode A16 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A17                        (_EBI_ROUTEPEN_APEN_A17 << 18)          /**< Shifted mode A17 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A18                        (_EBI_ROUTEPEN_APEN_A18 << 18)          /**< Shifted mode A18 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A19                        (_EBI_ROUTEPEN_APEN_A19 << 18)          /**< Shifted mode A19 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A20                        (_EBI_ROUTEPEN_APEN_A20 << 18)          /**< Shifted mode A20 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A21                        (_EBI_ROUTEPEN_APEN_A21 << 18)          /**< Shifted mode A21 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A22                        (_EBI_ROUTEPEN_APEN_A22 << 18)          /**< Shifted mode A22 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A23                        (_EBI_ROUTEPEN_APEN_A23 << 18)          /**< Shifted mode A23 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A24                        (_EBI_ROUTEPEN_APEN_A24 << 18)          /**< Shifted mode A24 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A25                        (_EBI_ROUTEPEN_APEN_A25 << 18)          /**< Shifted mode A25 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A26                        (_EBI_ROUTEPEN_APEN_A26 << 18)          /**< Shifted mode A26 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A27                        (_EBI_ROUTEPEN_APEN_A27 << 18)          /**< Shifted mode A27 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_APEN_A28                        (_EBI_ROUTEPEN_APEN_A28 << 18)          /**< Shifted mode A28 for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_TFTPEN                          (0x1UL << 24)                           /**< EBI_TFT Pin Enable */
#define _EBI_ROUTEPEN_TFTPEN_SHIFT                   24                                      /**< Shift value for EBI_TFTPEN */
#define _EBI_ROUTEPEN_TFTPEN_MASK                    0x1000000UL                             /**< Bit mask for EBI_TFTPEN */
#define _EBI_ROUTEPEN_TFTPEN_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_TFTPEN_DEFAULT                  (_EBI_ROUTEPEN_TFTPEN_DEFAULT << 24)    /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_DATAENPEN                       (0x1UL << 25)                           /**< EBI_DATA Pin Enable */
#define _EBI_ROUTEPEN_DATAENPEN_SHIFT                25                                      /**< Shift value for EBI_DATAENPEN */
#define _EBI_ROUTEPEN_DATAENPEN_MASK                 0x2000000UL                             /**< Bit mask for EBI_DATAENPEN */
#define _EBI_ROUTEPEN_DATAENPEN_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_DATAENPEN_DEFAULT               (_EBI_ROUTEPEN_DATAENPEN_DEFAULT << 25) /**< Shifted mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_CSTFTPEN                        (0x1UL << 26)                           /**< EBI_CSTFT Pin Enable */
#define _EBI_ROUTEPEN_CSTFTPEN_SHIFT                 26                                      /**< Shift value for EBI_CSTFTPEN */
#define _EBI_ROUTEPEN_CSTFTPEN_MASK                  0x4000000UL                             /**< Bit mask for EBI_CSTFTPEN */
#define _EBI_ROUTEPEN_CSTFTPEN_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for EBI_ROUTEPEN */
#define EBI_ROUTEPEN_CSTFTPEN_DEFAULT                (_EBI_ROUTEPEN_CSTFTPEN_DEFAULT << 26)  /**< Shifted mode DEFAULT for EBI_ROUTEPEN */

/* Bit fields for EBI ROUTELOC0 */
#define _EBI_ROUTELOC0_RESETVALUE                    0x00000000UL                           /**< Default value for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_MASK                          0x03070707UL                           /**< Mask for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_EBILOC_SHIFT                  0                                      /**< Shift value for EBI_EBILOC */
#define _EBI_ROUTELOC0_EBILOC_MASK                   0x7UL                                  /**< Bit mask for EBI_EBILOC */
#define _EBI_ROUTELOC0_EBILOC_LOC0                   0x00000000UL                           /**< Mode LOC0 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_EBILOC_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_EBILOC_LOC1                   0x00000001UL                           /**< Mode LOC1 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_EBILOC_LOC2                   0x00000002UL                           /**< Mode LOC2 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_EBILOC_LOC3                   0x00000003UL                           /**< Mode LOC3 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_EBILOC_LOC4                   0x00000004UL                           /**< Mode LOC4 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_EBILOC_LOC5                   0x00000005UL                           /**< Mode LOC5 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_EBILOC_LOC0                    (_EBI_ROUTELOC0_EBILOC_LOC0 << 0)      /**< Shifted mode LOC0 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_EBILOC_DEFAULT                 (_EBI_ROUTELOC0_EBILOC_DEFAULT << 0)   /**< Shifted mode DEFAULT for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_EBILOC_LOC1                    (_EBI_ROUTELOC0_EBILOC_LOC1 << 0)      /**< Shifted mode LOC1 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_EBILOC_LOC2                    (_EBI_ROUTELOC0_EBILOC_LOC2 << 0)      /**< Shifted mode LOC2 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_EBILOC_LOC3                    (_EBI_ROUTELOC0_EBILOC_LOC3 << 0)      /**< Shifted mode LOC3 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_EBILOC_LOC4                    (_EBI_ROUTELOC0_EBILOC_LOC4 << 0)      /**< Shifted mode LOC4 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_EBILOC_LOC5                    (_EBI_ROUTELOC0_EBILOC_LOC5 << 0)      /**< Shifted mode LOC5 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_CSLOC_SHIFT                   8                                      /**< Shift value for EBI_CSLOC */
#define _EBI_ROUTELOC0_CSLOC_MASK                    0x700UL                                /**< Bit mask for EBI_CSLOC */
#define _EBI_ROUTELOC0_CSLOC_LOC0                    0x00000000UL                           /**< Mode LOC0 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_CSLOC_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_CSLOC_LOC1                    0x00000001UL                           /**< Mode LOC1 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_CSLOC_LOC2                    0x00000002UL                           /**< Mode LOC2 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_CSLOC_LOC3                    0x00000003UL                           /**< Mode LOC3 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_CSLOC_LOC4                    0x00000004UL                           /**< Mode LOC4 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_CSLOC_LOC0                     (_EBI_ROUTELOC0_CSLOC_LOC0 << 8)       /**< Shifted mode LOC0 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_CSLOC_DEFAULT                  (_EBI_ROUTELOC0_CSLOC_DEFAULT << 8)    /**< Shifted mode DEFAULT for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_CSLOC_LOC1                     (_EBI_ROUTELOC0_CSLOC_LOC1 << 8)       /**< Shifted mode LOC1 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_CSLOC_LOC2                     (_EBI_ROUTELOC0_CSLOC_LOC2 << 8)       /**< Shifted mode LOC2 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_CSLOC_LOC3                     (_EBI_ROUTELOC0_CSLOC_LOC3 << 8)       /**< Shifted mode LOC3 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_CSLOC_LOC4                     (_EBI_ROUTELOC0_CSLOC_LOC4 << 8)       /**< Shifted mode LOC4 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_NANDLOC_SHIFT                 16                                     /**< Shift value for EBI_NANDLOC */
#define _EBI_ROUTELOC0_NANDLOC_MASK                  0x70000UL                              /**< Bit mask for EBI_NANDLOC */
#define _EBI_ROUTELOC0_NANDLOC_LOC0                  0x00000000UL                           /**< Mode LOC0 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_NANDLOC_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_NANDLOC_LOC1                  0x00000001UL                           /**< Mode LOC1 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_NANDLOC_LOC2                  0x00000002UL                           /**< Mode LOC2 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_NANDLOC_LOC3                  0x00000003UL                           /**< Mode LOC3 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_NANDLOC_LOC4                  0x00000004UL                           /**< Mode LOC4 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_NANDLOC_LOC5                  0x00000005UL                           /**< Mode LOC5 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_NANDLOC_LOC0                   (_EBI_ROUTELOC0_NANDLOC_LOC0 << 16)    /**< Shifted mode LOC0 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_NANDLOC_DEFAULT                (_EBI_ROUTELOC0_NANDLOC_DEFAULT << 16) /**< Shifted mode DEFAULT for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_NANDLOC_LOC1                   (_EBI_ROUTELOC0_NANDLOC_LOC1 << 16)    /**< Shifted mode LOC1 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_NANDLOC_LOC2                   (_EBI_ROUTELOC0_NANDLOC_LOC2 << 16)    /**< Shifted mode LOC2 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_NANDLOC_LOC3                   (_EBI_ROUTELOC0_NANDLOC_LOC3 << 16)    /**< Shifted mode LOC3 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_NANDLOC_LOC4                   (_EBI_ROUTELOC0_NANDLOC_LOC4 << 16)    /**< Shifted mode LOC4 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_NANDLOC_LOC5                   (_EBI_ROUTELOC0_NANDLOC_LOC5 << 16)    /**< Shifted mode LOC5 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_TFTLOC_SHIFT                  24                                     /**< Shift value for EBI_TFTLOC */
#define _EBI_ROUTELOC0_TFTLOC_MASK                   0x3000000UL                            /**< Bit mask for EBI_TFTLOC */
#define _EBI_ROUTELOC0_TFTLOC_LOC0                   0x00000000UL                           /**< Mode LOC0 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_TFTLOC_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_TFTLOC_LOC1                   0x00000001UL                           /**< Mode LOC1 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_TFTLOC_LOC2                   0x00000002UL                           /**< Mode LOC2 for EBI_ROUTELOC0 */
#define _EBI_ROUTELOC0_TFTLOC_LOC3                   0x00000003UL                           /**< Mode LOC3 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_TFTLOC_LOC0                    (_EBI_ROUTELOC0_TFTLOC_LOC0 << 24)     /**< Shifted mode LOC0 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_TFTLOC_DEFAULT                 (_EBI_ROUTELOC0_TFTLOC_DEFAULT << 24)  /**< Shifted mode DEFAULT for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_TFTLOC_LOC1                    (_EBI_ROUTELOC0_TFTLOC_LOC1 << 24)     /**< Shifted mode LOC1 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_TFTLOC_LOC2                    (_EBI_ROUTELOC0_TFTLOC_LOC2 << 24)     /**< Shifted mode LOC2 for EBI_ROUTELOC0 */
#define EBI_ROUTELOC0_TFTLOC_LOC3                    (_EBI_ROUTELOC0_TFTLOC_LOC3 << 24)     /**< Shifted mode LOC3 for EBI_ROUTELOC0 */

/* Bit fields for EBI ROUTELOC1 */
#define _EBI_ROUTELOC1_RESETVALUE                    0x00000000UL                          /**< Default value for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_MASK                          0x00070303UL                          /**< Mask for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_ADLOC_SHIFT                   0                                     /**< Shift value for EBI_ADLOC */
#define _EBI_ROUTELOC1_ADLOC_MASK                    0x3UL                                 /**< Bit mask for EBI_ADLOC */
#define _EBI_ROUTELOC1_ADLOC_LOC0                    0x00000000UL                          /**< Mode LOC0 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_ADLOC_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_ADLOC_LOC1                    0x00000001UL                          /**< Mode LOC1 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_ADLOC_LOC2                    0x00000002UL                          /**< Mode LOC2 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_ADLOC_LOC0                     (_EBI_ROUTELOC1_ADLOC_LOC0 << 0)      /**< Shifted mode LOC0 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_ADLOC_DEFAULT                  (_EBI_ROUTELOC1_ADLOC_DEFAULT << 0)   /**< Shifted mode DEFAULT for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_ADLOC_LOC1                     (_EBI_ROUTELOC1_ADLOC_LOC1 << 0)      /**< Shifted mode LOC1 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_ADLOC_LOC2                     (_EBI_ROUTELOC1_ADLOC_LOC2 << 0)      /**< Shifted mode LOC2 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_ALOC_SHIFT                    8                                     /**< Shift value for EBI_ALOC */
#define _EBI_ROUTELOC1_ALOC_MASK                     0x300UL                               /**< Bit mask for EBI_ALOC */
#define _EBI_ROUTELOC1_ALOC_LOC0                     0x00000000UL                          /**< Mode LOC0 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_ALOC_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_ALOC_LOC1                     0x00000001UL                          /**< Mode LOC1 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_ALOC_LOC2                     0x00000002UL                          /**< Mode LOC2 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_ALOC_LOC3                     0x00000003UL                          /**< Mode LOC3 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_ALOC_LOC0                      (_EBI_ROUTELOC1_ALOC_LOC0 << 8)       /**< Shifted mode LOC0 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_ALOC_DEFAULT                   (_EBI_ROUTELOC1_ALOC_DEFAULT << 8)    /**< Shifted mode DEFAULT for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_ALOC_LOC1                      (_EBI_ROUTELOC1_ALOC_LOC1 << 8)       /**< Shifted mode LOC1 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_ALOC_LOC2                      (_EBI_ROUTELOC1_ALOC_LOC2 << 8)       /**< Shifted mode LOC2 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_ALOC_LOC3                      (_EBI_ROUTELOC1_ALOC_LOC3 << 8)       /**< Shifted mode LOC3 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_RDYLOC_SHIFT                  16                                    /**< Shift value for EBI_RDYLOC */
#define _EBI_ROUTELOC1_RDYLOC_MASK                   0x70000UL                             /**< Bit mask for EBI_RDYLOC */
#define _EBI_ROUTELOC1_RDYLOC_LOC0                   0x00000000UL                          /**< Mode LOC0 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_RDYLOC_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_RDYLOC_LOC1                   0x00000001UL                          /**< Mode LOC1 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_RDYLOC_LOC2                   0x00000002UL                          /**< Mode LOC2 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_RDYLOC_LOC3                   0x00000003UL                          /**< Mode LOC3 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_RDYLOC_LOC4                   0x00000004UL                          /**< Mode LOC4 for EBI_ROUTELOC1 */
#define _EBI_ROUTELOC1_RDYLOC_LOC5                   0x00000005UL                          /**< Mode LOC5 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_RDYLOC_LOC0                    (_EBI_ROUTELOC1_RDYLOC_LOC0 << 16)    /**< Shifted mode LOC0 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_RDYLOC_DEFAULT                 (_EBI_ROUTELOC1_RDYLOC_DEFAULT << 16) /**< Shifted mode DEFAULT for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_RDYLOC_LOC1                    (_EBI_ROUTELOC1_RDYLOC_LOC1 << 16)    /**< Shifted mode LOC1 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_RDYLOC_LOC2                    (_EBI_ROUTELOC1_RDYLOC_LOC2 << 16)    /**< Shifted mode LOC2 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_RDYLOC_LOC3                    (_EBI_ROUTELOC1_RDYLOC_LOC3 << 16)    /**< Shifted mode LOC3 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_RDYLOC_LOC4                    (_EBI_ROUTELOC1_RDYLOC_LOC4 << 16)    /**< Shifted mode LOC4 for EBI_ROUTELOC1 */
#define EBI_ROUTELOC1_RDYLOC_LOC5                    (_EBI_ROUTELOC1_RDYLOC_LOC5 << 16)    /**< Shifted mode LOC5 for EBI_ROUTELOC1 */

/** @} */
/** @} End of group EFM32GG11B_EBI */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

