/***************************************************************************//**
 * @file
 * @brief EFM32GG12B_PDM register and bit field definitions
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
 * @defgroup EFM32GG12B_PDM PDM
 * @{
 * @brief EFM32GG12B_PDM Register Declaration
 ******************************************************************************/
/** PDM Register Declaration */
typedef struct {
  __IM uint32_t  IPVERSION;     /**< IP Version ID  */
  __IOM uint32_t EN;            /**< PDM Module enable Register  */
  __IOM uint32_t CTRL;          /**< PDM Core Control Register  */
  __IOM uint32_t CMD;           /**< PDM Core Command Register  */
  __IM uint32_t  STATUS;        /**< PDM Status register  */
  __IOM uint32_t CFG0;          /**< PDM Core Configuration Register0  */
  __IOM uint32_t CFG1;          /**< PDM Core Configuration Register1  */
  uint32_t       RESERVED0[1U]; /**< Reserved for future use **/
  __IM uint32_t  RXDATA;        /**< PDM Received Data Register  */
  uint32_t       RESERVED1[7U]; /**< Reserved for future use **/
  __IM uint32_t  IF;            /**< Interrupt Flag Register  */
  __IOM uint32_t IFS;           /**< Interrupt Flag Set Register  */
  __IOM uint32_t IFC;           /**< Interrupt Flag Clear Register  */
  __IOM uint32_t IEN;           /**< Interrupt Enable Register  */
  __IOM uint32_t ROUTEPEN;      /**< I/O LOCATION Enable Register  */
  __IOM uint32_t ROUTELOC0;     /**< I/O LOCATION Register   */
  __IOM uint32_t ROUTELOC1;     /**< I/O LOCATION Register   */
  uint32_t       RESERVED2[1U]; /**< Reserved for future use **/
  __IM uint32_t  SYNCBUSY;      /**< Synchronization Busy Register   */
} PDM_TypeDef;                  /** @} */

/***************************************************************************//**
 * @addtogroup EFM32GG12B_PDM
 * @{
 * @defgroup EFM32GG12B_PDM_BitFields  PDM Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for PDM IPVERSION */
#define _PDM_IPVERSION_RESETVALUE              0x00000000UL                            /**< Default value for PDM_IPVERSION */
#define _PDM_IPVERSION_MASK                    0xFFFFFFFFUL                            /**< Mask for PDM_IPVERSION */
#define _PDM_IPVERSION_IPVERSION_SHIFT         0                                       /**< Shift value for PDM_IPVERSION */
#define _PDM_IPVERSION_IPVERSION_MASK          0xFFFFFFFFUL                            /**< Bit mask for PDM_IPVERSION */
#define _PDM_IPVERSION_IPVERSION_DEFAULT       0x00000000UL                            /**< Mode DEFAULT for PDM_IPVERSION */
#define PDM_IPVERSION_IPVERSION_DEFAULT        (_PDM_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for PDM_IPVERSION */

/* Bit fields for PDM EN */
#define _PDM_EN_RESETVALUE                     0x00000000UL              /**< Default value for PDM_EN */
#define _PDM_EN_MASK                           0x00000001UL              /**< Mask for PDM_EN */
#define PDM_EN_EN                              (0x1UL << 0)              /**< Module enable */
#define _PDM_EN_EN_SHIFT                       0                         /**< Shift value for PDM_EN */
#define _PDM_EN_EN_MASK                        0x1UL                     /**< Bit mask for PDM_EN */
#define _PDM_EN_EN_DEFAULT                     0x00000000UL              /**< Mode DEFAULT for PDM_EN */
#define _PDM_EN_EN_DISABLE                     0x00000000UL              /**< Mode DISABLE for PDM_EN */
#define _PDM_EN_EN_ENABLE                      0x00000001UL              /**< Mode ENABLE for PDM_EN */
#define PDM_EN_EN_DEFAULT                      (_PDM_EN_EN_DEFAULT << 0) /**< Shifted mode DEFAULT for PDM_EN */
#define PDM_EN_EN_DISABLE                      (_PDM_EN_EN_DISABLE << 0) /**< Shifted mode DISABLE for PDM_EN */
#define PDM_EN_EN_ENABLE                       (_PDM_EN_EN_ENABLE << 0)  /**< Shifted mode ENABLE for PDM_EN */

/* Bit fields for PDM CTRL */
#define _PDM_CTRL_RESETVALUE                   0x00000000UL                       /**< Default value for PDM_CTRL */
#define _PDM_CTRL_MASK                         0x800FFF1FUL                       /**< Mask for PDM_CTRL */
#define _PDM_CTRL_GAIN_SHIFT                   0                                  /**< Shift value for PDM_GAIN */
#define _PDM_CTRL_GAIN_MASK                    0x1FUL                             /**< Bit mask for PDM_GAIN */
#define _PDM_CTRL_GAIN_DEFAULT                 0x00000000UL                       /**< Mode DEFAULT for PDM_CTRL */
#define PDM_CTRL_GAIN_DEFAULT                  (_PDM_CTRL_GAIN_DEFAULT << 0)      /**< Shifted mode DEFAULT for PDM_CTRL */
#define _PDM_CTRL_DSR_SHIFT                    8                                  /**< Shift value for PDM_DSR */
#define _PDM_CTRL_DSR_MASK                     0xFFF00UL                          /**< Bit mask for PDM_DSR */
#define _PDM_CTRL_DSR_DEFAULT                  0x00000000UL                       /**< Mode DEFAULT for PDM_CTRL */
#define PDM_CTRL_DSR_DEFAULT                   (_PDM_CTRL_DSR_DEFAULT << 8)       /**< Shifted mode DEFAULT for PDM_CTRL */
#define PDM_CTRL_OUTCLKEN                      (0x1UL << 31)                      /**< PDM Clock enable */
#define _PDM_CTRL_OUTCLKEN_SHIFT               31                                 /**< Shift value for PDM_OUTCLKEN */
#define _PDM_CTRL_OUTCLKEN_MASK                0x80000000UL                       /**< Bit mask for PDM_OUTCLKEN */
#define _PDM_CTRL_OUTCLKEN_DEFAULT             0x00000000UL                       /**< Mode DEFAULT for PDM_CTRL */
#define PDM_CTRL_OUTCLKEN_DEFAULT              (_PDM_CTRL_OUTCLKEN_DEFAULT << 31) /**< Shifted mode DEFAULT for PDM_CTRL */

/* Bit fields for PDM CMD */
#define _PDM_CMD_RESETVALUE                    0x00000000UL                    /**< Default value for PDM_CMD */
#define _PDM_CMD_MASK                          0x00010111UL                    /**< Mask for PDM_CMD */
#define PDM_CMD_START                          (0x1UL << 0)                    /**< Start DCF */
#define _PDM_CMD_START_SHIFT                   0                               /**< Shift value for PDM_START */
#define _PDM_CMD_START_MASK                    0x1UL                           /**< Bit mask for PDM_START */
#define _PDM_CMD_START_DEFAULT                 0x00000000UL                    /**< Mode DEFAULT for PDM_CMD */
#define PDM_CMD_START_DEFAULT                  (_PDM_CMD_START_DEFAULT << 0)   /**< Shifted mode DEFAULT for PDM_CMD */
#define PDM_CMD_STOP                           (0x1UL << 4)                    /**< Stop DCF */
#define _PDM_CMD_STOP_SHIFT                    4                               /**< Shift value for PDM_STOP */
#define _PDM_CMD_STOP_MASK                     0x10UL                          /**< Bit mask for PDM_STOP */
#define _PDM_CMD_STOP_DEFAULT                  0x00000000UL                    /**< Mode DEFAULT for PDM_CMD */
#define PDM_CMD_STOP_DEFAULT                   (_PDM_CMD_STOP_DEFAULT << 4)    /**< Shifted mode DEFAULT for PDM_CMD */
#define PDM_CMD_CLEAR                          (0x1UL << 8)                    /**< Clear DCF */
#define _PDM_CMD_CLEAR_SHIFT                   8                               /**< Shift value for PDM_CLEAR */
#define _PDM_CMD_CLEAR_MASK                    0x100UL                         /**< Bit mask for PDM_CLEAR */
#define _PDM_CMD_CLEAR_DEFAULT                 0x00000000UL                    /**< Mode DEFAULT for PDM_CMD */
#define PDM_CMD_CLEAR_DEFAULT                  (_PDM_CMD_CLEAR_DEFAULT << 8)   /**< Shifted mode DEFAULT for PDM_CMD */
#define PDM_CMD_FIFOFL                         (0x1UL << 16)                   /**< FIFO Flush */
#define _PDM_CMD_FIFOFL_SHIFT                  16                              /**< Shift value for PDM_FIFOFL */
#define _PDM_CMD_FIFOFL_MASK                   0x10000UL                       /**< Bit mask for PDM_FIFOFL */
#define _PDM_CMD_FIFOFL_DEFAULT                0x00000000UL                    /**< Mode DEFAULT for PDM_CMD */
#define PDM_CMD_FIFOFL_DEFAULT                 (_PDM_CMD_FIFOFL_DEFAULT << 16) /**< Shifted mode DEFAULT for PDM_CMD */

/* Bit fields for PDM STATUS */
#define _PDM_STATUS_RESETVALUE                 0x00000020UL                       /**< Default value for PDM_STATUS */
#define _PDM_STATUS_MASK                       0x00000731UL                       /**< Mask for PDM_STATUS */
#define PDM_STATUS_ACT                         (0x1UL << 0)                       /**< PDM is active */
#define _PDM_STATUS_ACT_SHIFT                  0                                  /**< Shift value for PDM_ACT */
#define _PDM_STATUS_ACT_MASK                   0x1UL                              /**< Bit mask for PDM_ACT */
#define _PDM_STATUS_ACT_DEFAULT                0x00000000UL                       /**< Mode DEFAULT for PDM_STATUS */
#define PDM_STATUS_ACT_DEFAULT                 (_PDM_STATUS_ACT_DEFAULT << 0)     /**< Shifted mode DEFAULT for PDM_STATUS */
#define PDM_STATUS_FULL                        (0x1UL << 4)                       /**< FIFO FULL Status */
#define _PDM_STATUS_FULL_SHIFT                 4                                  /**< Shift value for PDM_FULL */
#define _PDM_STATUS_FULL_MASK                  0x10UL                             /**< Bit mask for PDM_FULL */
#define _PDM_STATUS_FULL_DEFAULT               0x00000000UL                       /**< Mode DEFAULT for PDM_STATUS */
#define PDM_STATUS_FULL_DEFAULT                (_PDM_STATUS_FULL_DEFAULT << 4)    /**< Shifted mode DEFAULT for PDM_STATUS */
#define PDM_STATUS_EMPTY                       (0x1UL << 5)                       /**< FIFO EMPTY Status */
#define _PDM_STATUS_EMPTY_SHIFT                5                                  /**< Shift value for PDM_EMPTY */
#define _PDM_STATUS_EMPTY_MASK                 0x20UL                             /**< Bit mask for PDM_EMPTY */
#define _PDM_STATUS_EMPTY_DEFAULT              0x00000001UL                       /**< Mode DEFAULT for PDM_STATUS */
#define PDM_STATUS_EMPTY_DEFAULT               (_PDM_STATUS_EMPTY_DEFAULT << 5)   /**< Shifted mode DEFAULT for PDM_STATUS */
#define _PDM_STATUS_FIFOCNT_SHIFT              8                                  /**< Shift value for PDM_FIFOCNT */
#define _PDM_STATUS_FIFOCNT_MASK               0x700UL                            /**< Bit mask for PDM_FIFOCNT */
#define _PDM_STATUS_FIFOCNT_DEFAULT            0x00000000UL                       /**< Mode DEFAULT for PDM_STATUS */
#define PDM_STATUS_FIFOCNT_DEFAULT             (_PDM_STATUS_FIFOCNT_DEFAULT << 8) /**< Shifted mode DEFAULT for PDM_STATUS */

/* Bit fields for PDM CFG0 */
#define _PDM_CFG0_RESETVALUE                   0x00000000UL                                /**< Default value for PDM_CFG0 */
#define _PDM_CFG0_MASK                         0x0F033733UL                                /**< Mask for PDM_CFG0 */
#define _PDM_CFG0_FORDER_SHIFT                 0                                           /**< Shift value for PDM_FORDER */
#define _PDM_CFG0_FORDER_MASK                  0x3UL                                       /**< Bit mask for PDM_FORDER */
#define _PDM_CFG0_FORDER_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0 */
#define _PDM_CFG0_FORDER_SECOND                0x00000000UL                                /**< Mode SECOND for PDM_CFG0 */
#define _PDM_CFG0_FORDER_THIRD                 0x00000001UL                                /**< Mode THIRD for PDM_CFG0 */
#define _PDM_CFG0_FORDER_FOURTH                0x00000002UL                                /**< Mode FOURTH for PDM_CFG0 */
#define _PDM_CFG0_FORDER_FIFTH                 0x00000003UL                                /**< Mode FIFTH for PDM_CFG0 */
#define PDM_CFG0_FORDER_DEFAULT                (_PDM_CFG0_FORDER_DEFAULT << 0)             /**< Shifted mode DEFAULT for PDM_CFG0 */
#define PDM_CFG0_FORDER_SECOND                 (_PDM_CFG0_FORDER_SECOND << 0)              /**< Shifted mode SECOND for PDM_CFG0 */
#define PDM_CFG0_FORDER_THIRD                  (_PDM_CFG0_FORDER_THIRD << 0)               /**< Shifted mode THIRD for PDM_CFG0 */
#define PDM_CFG0_FORDER_FOURTH                 (_PDM_CFG0_FORDER_FOURTH << 0)              /**< Shifted mode FOURTH for PDM_CFG0 */
#define PDM_CFG0_FORDER_FIFTH                  (_PDM_CFG0_FORDER_FIFTH << 0)               /**< Shifted mode FIFTH for PDM_CFG0 */
#define _PDM_CFG0_NUMCH_SHIFT                  4                                           /**< Shift value for PDM_NUMCH */
#define _PDM_CFG0_NUMCH_MASK                   0x30UL                                      /**< Bit mask for PDM_NUMCH */
#define _PDM_CFG0_NUMCH_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0 */
#define _PDM_CFG0_NUMCH_ONE                    0x00000000UL                                /**< Mode ONE for PDM_CFG0 */
#define _PDM_CFG0_NUMCH_TWO                    0x00000001UL                                /**< Mode TWO for PDM_CFG0 */
#define _PDM_CFG0_NUMCH_THREE                  0x00000002UL                                /**< Mode THREE for PDM_CFG0 */
#define _PDM_CFG0_NUMCH_FOUR                   0x00000003UL                                /**< Mode FOUR for PDM_CFG0 */
#define PDM_CFG0_NUMCH_DEFAULT                 (_PDM_CFG0_NUMCH_DEFAULT << 4)              /**< Shifted mode DEFAULT for PDM_CFG0 */
#define PDM_CFG0_NUMCH_ONE                     (_PDM_CFG0_NUMCH_ONE << 4)                  /**< Shifted mode ONE for PDM_CFG0 */
#define PDM_CFG0_NUMCH_TWO                     (_PDM_CFG0_NUMCH_TWO << 4)                  /**< Shifted mode TWO for PDM_CFG0 */
#define PDM_CFG0_NUMCH_THREE                   (_PDM_CFG0_NUMCH_THREE << 4)                /**< Shifted mode THREE for PDM_CFG0 */
#define PDM_CFG0_NUMCH_FOUR                    (_PDM_CFG0_NUMCH_FOUR << 4)                 /**< Shifted mode FOUR for PDM_CFG0 */
#define _PDM_CFG0_DATAFORMAT_SHIFT             8                                           /**< Shift value for PDM_DATAFORMAT */
#define _PDM_CFG0_DATAFORMAT_MASK              0x700UL                                     /**< Bit mask for PDM_DATAFORMAT */
#define _PDM_CFG0_DATAFORMAT_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0 */
#define _PDM_CFG0_DATAFORMAT_RIGHT16           0x00000000UL                                /**< Mode RIGHT16 for PDM_CFG0 */
#define _PDM_CFG0_DATAFORMAT_DOUBLE16          0x00000001UL                                /**< Mode DOUBLE16 for PDM_CFG0 */
#define _PDM_CFG0_DATAFORMAT_RIGHT24           0x00000002UL                                /**< Mode RIGHT24 for PDM_CFG0 */
#define _PDM_CFG0_DATAFORMAT_FULL32BIT         0x00000003UL                                /**< Mode FULL32BIT for PDM_CFG0 */
#define _PDM_CFG0_DATAFORMAT_LEFT16            0x00000004UL                                /**< Mode LEFT16 for PDM_CFG0 */
#define _PDM_CFG0_DATAFORMAT_LEFT24            0x00000005UL                                /**< Mode LEFT24 for PDM_CFG0 */
#define _PDM_CFG0_DATAFORMAT_RAW32BIT          0x00000006UL                                /**< Mode RAW32BIT for PDM_CFG0 */
#define PDM_CFG0_DATAFORMAT_DEFAULT            (_PDM_CFG0_DATAFORMAT_DEFAULT << 8)         /**< Shifted mode DEFAULT for PDM_CFG0 */
#define PDM_CFG0_DATAFORMAT_RIGHT16            (_PDM_CFG0_DATAFORMAT_RIGHT16 << 8)         /**< Shifted mode RIGHT16 for PDM_CFG0 */
#define PDM_CFG0_DATAFORMAT_DOUBLE16           (_PDM_CFG0_DATAFORMAT_DOUBLE16 << 8)        /**< Shifted mode DOUBLE16 for PDM_CFG0 */
#define PDM_CFG0_DATAFORMAT_RIGHT24            (_PDM_CFG0_DATAFORMAT_RIGHT24 << 8)         /**< Shifted mode RIGHT24 for PDM_CFG0 */
#define PDM_CFG0_DATAFORMAT_FULL32BIT          (_PDM_CFG0_DATAFORMAT_FULL32BIT << 8)       /**< Shifted mode FULL32BIT for PDM_CFG0 */
#define PDM_CFG0_DATAFORMAT_LEFT16             (_PDM_CFG0_DATAFORMAT_LEFT16 << 8)          /**< Shifted mode LEFT16 for PDM_CFG0 */
#define PDM_CFG0_DATAFORMAT_LEFT24             (_PDM_CFG0_DATAFORMAT_LEFT24 << 8)          /**< Shifted mode LEFT24 for PDM_CFG0 */
#define PDM_CFG0_DATAFORMAT_RAW32BIT           (_PDM_CFG0_DATAFORMAT_RAW32BIT << 8)        /**< Shifted mode RAW32BIT for PDM_CFG0 */
#define _PDM_CFG0_FIFODVL_SHIFT                12                                          /**< Shift value for PDM_FIFODVL */
#define _PDM_CFG0_FIFODVL_MASK                 0x3000UL                                    /**< Bit mask for PDM_FIFODVL */
#define _PDM_CFG0_FIFODVL_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0 */
#define _PDM_CFG0_FIFODVL_ONE                  0x00000000UL                                /**< Mode ONE for PDM_CFG0 */
#define _PDM_CFG0_FIFODVL_TWO                  0x00000001UL                                /**< Mode TWO for PDM_CFG0 */
#define _PDM_CFG0_FIFODVL_THREE                0x00000002UL                                /**< Mode THREE for PDM_CFG0 */
#define _PDM_CFG0_FIFODVL_FOUR                 0x00000003UL                                /**< Mode FOUR for PDM_CFG0 */
#define PDM_CFG0_FIFODVL_DEFAULT               (_PDM_CFG0_FIFODVL_DEFAULT << 12)           /**< Shifted mode DEFAULT for PDM_CFG0 */
#define PDM_CFG0_FIFODVL_ONE                   (_PDM_CFG0_FIFODVL_ONE << 12)               /**< Shifted mode ONE for PDM_CFG0 */
#define PDM_CFG0_FIFODVL_TWO                   (_PDM_CFG0_FIFODVL_TWO << 12)               /**< Shifted mode TWO for PDM_CFG0 */
#define PDM_CFG0_FIFODVL_THREE                 (_PDM_CFG0_FIFODVL_THREE << 12)             /**< Shifted mode THREE for PDM_CFG0 */
#define PDM_CFG0_FIFODVL_FOUR                  (_PDM_CFG0_FIFODVL_FOUR << 12)              /**< Shifted mode FOUR for PDM_CFG0 */
#define PDM_CFG0_STEREOMODECH01                (0x1UL << 16)                               /**< Stereo mode CH01 */
#define _PDM_CFG0_STEREOMODECH01_SHIFT         16                                          /**< Shift value for PDM_STEREOMODECH01 */
#define _PDM_CFG0_STEREOMODECH01_MASK          0x10000UL                                   /**< Bit mask for PDM_STEREOMODECH01 */
#define _PDM_CFG0_STEREOMODECH01_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0 */
#define _PDM_CFG0_STEREOMODECH01_DISABLE       0x00000000UL                                /**< Mode DISABLE for PDM_CFG0 */
#define _PDM_CFG0_STEREOMODECH01_CH01ENABLE    0x00000001UL                                /**< Mode CH01ENABLE for PDM_CFG0 */
#define PDM_CFG0_STEREOMODECH01_DEFAULT        (_PDM_CFG0_STEREOMODECH01_DEFAULT << 16)    /**< Shifted mode DEFAULT for PDM_CFG0 */
#define PDM_CFG0_STEREOMODECH01_DISABLE        (_PDM_CFG0_STEREOMODECH01_DISABLE << 16)    /**< Shifted mode DISABLE for PDM_CFG0 */
#define PDM_CFG0_STEREOMODECH01_CH01ENABLE     (_PDM_CFG0_STEREOMODECH01_CH01ENABLE << 16) /**< Shifted mode CH01ENABLE for PDM_CFG0 */
#define PDM_CFG0_STEREOMODECH23                (0x1UL << 17)                               /**< Stereo mode CH23 */
#define _PDM_CFG0_STEREOMODECH23_SHIFT         17                                          /**< Shift value for PDM_STEREOMODECH23 */
#define _PDM_CFG0_STEREOMODECH23_MASK          0x20000UL                                   /**< Bit mask for PDM_STEREOMODECH23 */
#define _PDM_CFG0_STEREOMODECH23_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0 */
#define _PDM_CFG0_STEREOMODECH23_DISABLE       0x00000000UL                                /**< Mode DISABLE for PDM_CFG0 */
#define _PDM_CFG0_STEREOMODECH23_CH23ENABLE    0x00000001UL                                /**< Mode CH23ENABLE for PDM_CFG0 */
#define PDM_CFG0_STEREOMODECH23_DEFAULT        (_PDM_CFG0_STEREOMODECH23_DEFAULT << 17)    /**< Shifted mode DEFAULT for PDM_CFG0 */
#define PDM_CFG0_STEREOMODECH23_DISABLE        (_PDM_CFG0_STEREOMODECH23_DISABLE << 17)    /**< Shifted mode DISABLE for PDM_CFG0 */
#define PDM_CFG0_STEREOMODECH23_CH23ENABLE     (_PDM_CFG0_STEREOMODECH23_CH23ENABLE << 17) /**< Shifted mode CH23ENABLE for PDM_CFG0 */
#define PDM_CFG0_CH0CLKPOL                     (0x1UL << 24)                               /**< CH0 CLK Polarity */
#define _PDM_CFG0_CH0CLKPOL_SHIFT              24                                          /**< Shift value for PDM_CH0CLKPOL */
#define _PDM_CFG0_CH0CLKPOL_MASK               0x1000000UL                                 /**< Bit mask for PDM_CH0CLKPOL */
#define _PDM_CFG0_CH0CLKPOL_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0 */
#define _PDM_CFG0_CH0CLKPOL_NORMAL             0x00000000UL                                /**< Mode NORMAL for PDM_CFG0 */
#define _PDM_CFG0_CH0CLKPOL_INVERT             0x00000001UL                                /**< Mode INVERT for PDM_CFG0 */
#define PDM_CFG0_CH0CLKPOL_DEFAULT             (_PDM_CFG0_CH0CLKPOL_DEFAULT << 24)         /**< Shifted mode DEFAULT for PDM_CFG0 */
#define PDM_CFG0_CH0CLKPOL_NORMAL              (_PDM_CFG0_CH0CLKPOL_NORMAL << 24)          /**< Shifted mode NORMAL for PDM_CFG0 */
#define PDM_CFG0_CH0CLKPOL_INVERT              (_PDM_CFG0_CH0CLKPOL_INVERT << 24)          /**< Shifted mode INVERT for PDM_CFG0 */
#define PDM_CFG0_CH1CLKPOL                     (0x1UL << 25)                               /**< CH1 CLK Polarity */
#define _PDM_CFG0_CH1CLKPOL_SHIFT              25                                          /**< Shift value for PDM_CH1CLKPOL */
#define _PDM_CFG0_CH1CLKPOL_MASK               0x2000000UL                                 /**< Bit mask for PDM_CH1CLKPOL */
#define _PDM_CFG0_CH1CLKPOL_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0 */
#define _PDM_CFG0_CH1CLKPOL_NORMAL             0x00000000UL                                /**< Mode NORMAL for PDM_CFG0 */
#define _PDM_CFG0_CH1CLKPOL_INVERT             0x00000001UL                                /**< Mode INVERT for PDM_CFG0 */
#define PDM_CFG0_CH1CLKPOL_DEFAULT             (_PDM_CFG0_CH1CLKPOL_DEFAULT << 25)         /**< Shifted mode DEFAULT for PDM_CFG0 */
#define PDM_CFG0_CH1CLKPOL_NORMAL              (_PDM_CFG0_CH1CLKPOL_NORMAL << 25)          /**< Shifted mode NORMAL for PDM_CFG0 */
#define PDM_CFG0_CH1CLKPOL_INVERT              (_PDM_CFG0_CH1CLKPOL_INVERT << 25)          /**< Shifted mode INVERT for PDM_CFG0 */
#define PDM_CFG0_CH2CLKPOL                     (0x1UL << 26)                               /**< CH2 CLK Polarity */
#define _PDM_CFG0_CH2CLKPOL_SHIFT              26                                          /**< Shift value for PDM_CH2CLKPOL */
#define _PDM_CFG0_CH2CLKPOL_MASK               0x4000000UL                                 /**< Bit mask for PDM_CH2CLKPOL */
#define _PDM_CFG0_CH2CLKPOL_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0 */
#define _PDM_CFG0_CH2CLKPOL_NORMAL             0x00000000UL                                /**< Mode NORMAL for PDM_CFG0 */
#define _PDM_CFG0_CH2CLKPOL_INVERT             0x00000001UL                                /**< Mode INVERT for PDM_CFG0 */
#define PDM_CFG0_CH2CLKPOL_DEFAULT             (_PDM_CFG0_CH2CLKPOL_DEFAULT << 26)         /**< Shifted mode DEFAULT for PDM_CFG0 */
#define PDM_CFG0_CH2CLKPOL_NORMAL              (_PDM_CFG0_CH2CLKPOL_NORMAL << 26)          /**< Shifted mode NORMAL for PDM_CFG0 */
#define PDM_CFG0_CH2CLKPOL_INVERT              (_PDM_CFG0_CH2CLKPOL_INVERT << 26)          /**< Shifted mode INVERT for PDM_CFG0 */
#define PDM_CFG0_CH3CLKPOL                     (0x1UL << 27)                               /**< CH3 CLK Polarity */
#define _PDM_CFG0_CH3CLKPOL_SHIFT              27                                          /**< Shift value for PDM_CH3CLKPOL */
#define _PDM_CFG0_CH3CLKPOL_MASK               0x8000000UL                                 /**< Bit mask for PDM_CH3CLKPOL */
#define _PDM_CFG0_CH3CLKPOL_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0 */
#define _PDM_CFG0_CH3CLKPOL_NORMAL             0x00000000UL                                /**< Mode NORMAL for PDM_CFG0 */
#define _PDM_CFG0_CH3CLKPOL_INVERT             0x00000001UL                                /**< Mode INVERT for PDM_CFG0 */
#define PDM_CFG0_CH3CLKPOL_DEFAULT             (_PDM_CFG0_CH3CLKPOL_DEFAULT << 27)         /**< Shifted mode DEFAULT for PDM_CFG0 */
#define PDM_CFG0_CH3CLKPOL_NORMAL              (_PDM_CFG0_CH3CLKPOL_NORMAL << 27)          /**< Shifted mode NORMAL for PDM_CFG0 */
#define PDM_CFG0_CH3CLKPOL_INVERT              (_PDM_CFG0_CH3CLKPOL_INVERT << 27)          /**< Shifted mode INVERT for PDM_CFG0 */

/* Bit fields for PDM CFG1 */
#define _PDM_CFG1_RESETVALUE                   0x00000000UL                   /**< Default value for PDM_CFG1 */
#define _PDM_CFG1_MASK                         0x000003FFUL                   /**< Mask for PDM_CFG1 */
#define _PDM_CFG1_PRESC_SHIFT                  0                              /**< Shift value for PDM_PRESC */
#define _PDM_CFG1_PRESC_MASK                   0x3FFUL                        /**< Bit mask for PDM_PRESC */
#define _PDM_CFG1_PRESC_DEFAULT                0x00000000UL                   /**< Mode DEFAULT for PDM_CFG1 */
#define PDM_CFG1_PRESC_DEFAULT                 (_PDM_CFG1_PRESC_DEFAULT << 0) /**< Shifted mode DEFAULT for PDM_CFG1 */

/* Bit fields for PDM RXDATA */
#define _PDM_RXDATA_RESETVALUE                 0x00000000UL                      /**< Default value for PDM_RXDATA */
#define _PDM_RXDATA_MASK                       0xFFFFFFFFUL                      /**< Mask for PDM_RXDATA */
#define _PDM_RXDATA_RXDATA_SHIFT               0                                 /**< Shift value for PDM_RXDATA */
#define _PDM_RXDATA_RXDATA_MASK                0xFFFFFFFFUL                      /**< Bit mask for PDM_RXDATA */
#define _PDM_RXDATA_RXDATA_DEFAULT             0x00000000UL                      /**< Mode DEFAULT for PDM_RXDATA */
#define PDM_RXDATA_RXDATA_DEFAULT              (_PDM_RXDATA_RXDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for PDM_RXDATA */

/* Bit fields for PDM IF */
#define _PDM_IF_RESETVALUE                     0x00000000UL               /**< Default value for PDM_IF */
#define _PDM_IF_MASK                           0x0000000FUL               /**< Mask for PDM_IF */
#define PDM_IF_DV                              (0x1UL << 0)               /**< Data Valid Interrupt Flag */
#define _PDM_IF_DV_SHIFT                       0                          /**< Shift value for PDM_DV */
#define _PDM_IF_DV_MASK                        0x1UL                      /**< Bit mask for PDM_DV */
#define _PDM_IF_DV_DEFAULT                     0x00000000UL               /**< Mode DEFAULT for PDM_IF */
#define PDM_IF_DV_DEFAULT                      (_PDM_IF_DV_DEFAULT << 0)  /**< Shifted mode DEFAULT for PDM_IF */
#define PDM_IF_DVL                             (0x1UL << 1)               /**< Data Valid Level Interrupt Flag */
#define _PDM_IF_DVL_SHIFT                      1                          /**< Shift value for PDM_DVL */
#define _PDM_IF_DVL_MASK                       0x2UL                      /**< Bit mask for PDM_DVL */
#define _PDM_IF_DVL_DEFAULT                    0x00000000UL               /**< Mode DEFAULT for PDM_IF */
#define PDM_IF_DVL_DEFAULT                     (_PDM_IF_DVL_DEFAULT << 1) /**< Shifted mode DEFAULT for PDM_IF */
#define PDM_IF_OF                              (0x1UL << 2)               /**< FIFO Overflow Interrupt Flag */
#define _PDM_IF_OF_SHIFT                       2                          /**< Shift value for PDM_OF */
#define _PDM_IF_OF_MASK                        0x4UL                      /**< Bit mask for PDM_OF */
#define _PDM_IF_OF_DEFAULT                     0x00000000UL               /**< Mode DEFAULT for PDM_IF */
#define PDM_IF_OF_DEFAULT                      (_PDM_IF_OF_DEFAULT << 2)  /**< Shifted mode DEFAULT for PDM_IF */
#define PDM_IF_UF                              (0x1UL << 3)               /**< FIFO Undeflow Interrupt Flag */
#define _PDM_IF_UF_SHIFT                       3                          /**< Shift value for PDM_UF */
#define _PDM_IF_UF_MASK                        0x8UL                      /**< Bit mask for PDM_UF */
#define _PDM_IF_UF_DEFAULT                     0x00000000UL               /**< Mode DEFAULT for PDM_IF */
#define PDM_IF_UF_DEFAULT                      (_PDM_IF_UF_DEFAULT << 3)  /**< Shifted mode DEFAULT for PDM_IF */

/* Bit fields for PDM IFS */
#define _PDM_IFS_RESETVALUE                    0x00000000UL                /**< Default value for PDM_IFS */
#define _PDM_IFS_MASK                          0x0000000FUL                /**< Mask for PDM_IFS */
#define PDM_IFS_DV                             (0x1UL << 0)                /**< Set DV Interrupt Flag */
#define _PDM_IFS_DV_SHIFT                      0                           /**< Shift value for PDM_DV */
#define _PDM_IFS_DV_MASK                       0x1UL                       /**< Bit mask for PDM_DV */
#define _PDM_IFS_DV_DEFAULT                    0x00000000UL                /**< Mode DEFAULT for PDM_IFS */
#define PDM_IFS_DV_DEFAULT                     (_PDM_IFS_DV_DEFAULT << 0)  /**< Shifted mode DEFAULT for PDM_IFS */
#define PDM_IFS_DVL                            (0x1UL << 1)                /**< Set DVL Interrupt Flag */
#define _PDM_IFS_DVL_SHIFT                     1                           /**< Shift value for PDM_DVL */
#define _PDM_IFS_DVL_MASK                      0x2UL                       /**< Bit mask for PDM_DVL */
#define _PDM_IFS_DVL_DEFAULT                   0x00000000UL                /**< Mode DEFAULT for PDM_IFS */
#define PDM_IFS_DVL_DEFAULT                    (_PDM_IFS_DVL_DEFAULT << 1) /**< Shifted mode DEFAULT for PDM_IFS */
#define PDM_IFS_OF                             (0x1UL << 2)                /**< Set OF Interrupt Flag */
#define _PDM_IFS_OF_SHIFT                      2                           /**< Shift value for PDM_OF */
#define _PDM_IFS_OF_MASK                       0x4UL                       /**< Bit mask for PDM_OF */
#define _PDM_IFS_OF_DEFAULT                    0x00000000UL                /**< Mode DEFAULT for PDM_IFS */
#define PDM_IFS_OF_DEFAULT                     (_PDM_IFS_OF_DEFAULT << 2)  /**< Shifted mode DEFAULT for PDM_IFS */
#define PDM_IFS_UF                             (0x1UL << 3)                /**< Set UF Interrupt Flag */
#define _PDM_IFS_UF_SHIFT                      3                           /**< Shift value for PDM_UF */
#define _PDM_IFS_UF_MASK                       0x8UL                       /**< Bit mask for PDM_UF */
#define _PDM_IFS_UF_DEFAULT                    0x00000000UL                /**< Mode DEFAULT for PDM_IFS */
#define PDM_IFS_UF_DEFAULT                     (_PDM_IFS_UF_DEFAULT << 3)  /**< Shifted mode DEFAULT for PDM_IFS */

/* Bit fields for PDM IFC */
#define _PDM_IFC_RESETVALUE                    0x00000000UL                /**< Default value for PDM_IFC */
#define _PDM_IFC_MASK                          0x0000000FUL                /**< Mask for PDM_IFC */
#define PDM_IFC_DV                             (0x1UL << 0)                /**< Clear DV Interrupt Flag */
#define _PDM_IFC_DV_SHIFT                      0                           /**< Shift value for PDM_DV */
#define _PDM_IFC_DV_MASK                       0x1UL                       /**< Bit mask for PDM_DV */
#define _PDM_IFC_DV_DEFAULT                    0x00000000UL                /**< Mode DEFAULT for PDM_IFC */
#define PDM_IFC_DV_DEFAULT                     (_PDM_IFC_DV_DEFAULT << 0)  /**< Shifted mode DEFAULT for PDM_IFC */
#define PDM_IFC_DVL                            (0x1UL << 1)                /**< Clear DVL Interrupt Flag */
#define _PDM_IFC_DVL_SHIFT                     1                           /**< Shift value for PDM_DVL */
#define _PDM_IFC_DVL_MASK                      0x2UL                       /**< Bit mask for PDM_DVL */
#define _PDM_IFC_DVL_DEFAULT                   0x00000000UL                /**< Mode DEFAULT for PDM_IFC */
#define PDM_IFC_DVL_DEFAULT                    (_PDM_IFC_DVL_DEFAULT << 1) /**< Shifted mode DEFAULT for PDM_IFC */
#define PDM_IFC_OF                             (0x1UL << 2)                /**< Clear OF Interrupt Flag */
#define _PDM_IFC_OF_SHIFT                      2                           /**< Shift value for PDM_OF */
#define _PDM_IFC_OF_MASK                       0x4UL                       /**< Bit mask for PDM_OF */
#define _PDM_IFC_OF_DEFAULT                    0x00000000UL                /**< Mode DEFAULT for PDM_IFC */
#define PDM_IFC_OF_DEFAULT                     (_PDM_IFC_OF_DEFAULT << 2)  /**< Shifted mode DEFAULT for PDM_IFC */
#define PDM_IFC_UF                             (0x1UL << 3)                /**< Clear UF Interrupt Flag */
#define _PDM_IFC_UF_SHIFT                      3                           /**< Shift value for PDM_UF */
#define _PDM_IFC_UF_MASK                       0x8UL                       /**< Bit mask for PDM_UF */
#define _PDM_IFC_UF_DEFAULT                    0x00000000UL                /**< Mode DEFAULT for PDM_IFC */
#define PDM_IFC_UF_DEFAULT                     (_PDM_IFC_UF_DEFAULT << 3)  /**< Shifted mode DEFAULT for PDM_IFC */

/* Bit fields for PDM IEN */
#define _PDM_IEN_RESETVALUE                    0x00000000UL                /**< Default value for PDM_IEN */
#define _PDM_IEN_MASK                          0x0000000FUL                /**< Mask for PDM_IEN */
#define PDM_IEN_DV                             (0x1UL << 0)                /**< DV Interrupt Enable */
#define _PDM_IEN_DV_SHIFT                      0                           /**< Shift value for PDM_DV */
#define _PDM_IEN_DV_MASK                       0x1UL                       /**< Bit mask for PDM_DV */
#define _PDM_IEN_DV_DEFAULT                    0x00000000UL                /**< Mode DEFAULT for PDM_IEN */
#define PDM_IEN_DV_DEFAULT                     (_PDM_IEN_DV_DEFAULT << 0)  /**< Shifted mode DEFAULT for PDM_IEN */
#define PDM_IEN_DVL                            (0x1UL << 1)                /**< DVL Interrupt Enable */
#define _PDM_IEN_DVL_SHIFT                     1                           /**< Shift value for PDM_DVL */
#define _PDM_IEN_DVL_MASK                      0x2UL                       /**< Bit mask for PDM_DVL */
#define _PDM_IEN_DVL_DEFAULT                   0x00000000UL                /**< Mode DEFAULT for PDM_IEN */
#define PDM_IEN_DVL_DEFAULT                    (_PDM_IEN_DVL_DEFAULT << 1) /**< Shifted mode DEFAULT for PDM_IEN */
#define PDM_IEN_OF                             (0x1UL << 2)                /**< OF Interrupt Enable */
#define _PDM_IEN_OF_SHIFT                      2                           /**< Shift value for PDM_OF */
#define _PDM_IEN_OF_MASK                       0x4UL                       /**< Bit mask for PDM_OF */
#define _PDM_IEN_OF_DEFAULT                    0x00000000UL                /**< Mode DEFAULT for PDM_IEN */
#define PDM_IEN_OF_DEFAULT                     (_PDM_IEN_OF_DEFAULT << 2)  /**< Shifted mode DEFAULT for PDM_IEN */
#define PDM_IEN_UF                             (0x1UL << 3)                /**< UF Interrupt Enable */
#define _PDM_IEN_UF_SHIFT                      3                           /**< Shift value for PDM_UF */
#define _PDM_IEN_UF_MASK                       0x8UL                       /**< Bit mask for PDM_UF */
#define _PDM_IEN_UF_DEFAULT                    0x00000000UL                /**< Mode DEFAULT for PDM_IEN */
#define PDM_IEN_UF_DEFAULT                     (_PDM_IEN_UF_DEFAULT << 3)  /**< Shifted mode DEFAULT for PDM_IEN */

/* Bit fields for PDM ROUTEPEN */
#define _PDM_ROUTEPEN_RESETVALUE               0x00000000UL                         /**< Default value for PDM_ROUTEPEN */
#define _PDM_ROUTEPEN_MASK                     0x0000010FUL                         /**< Mask for PDM_ROUTEPEN */
#define PDM_ROUTEPEN_DAT0PEN                   (0x1UL << 0)                         /**< DAT0 I/O Enable */
#define _PDM_ROUTEPEN_DAT0PEN_SHIFT            0                                    /**< Shift value for PDM_DAT0PEN */
#define _PDM_ROUTEPEN_DAT0PEN_MASK             0x1UL                                /**< Bit mask for PDM_DAT0PEN */
#define _PDM_ROUTEPEN_DAT0PEN_DEFAULT          0x00000000UL                         /**< Mode DEFAULT for PDM_ROUTEPEN */
#define PDM_ROUTEPEN_DAT0PEN_DEFAULT           (_PDM_ROUTEPEN_DAT0PEN_DEFAULT << 0) /**< Shifted mode DEFAULT for PDM_ROUTEPEN */
#define PDM_ROUTEPEN_DAT1PEN                   (0x1UL << 1)                         /**< DAT1 I/O Enable */
#define _PDM_ROUTEPEN_DAT1PEN_SHIFT            1                                    /**< Shift value for PDM_DAT1PEN */
#define _PDM_ROUTEPEN_DAT1PEN_MASK             0x2UL                                /**< Bit mask for PDM_DAT1PEN */
#define _PDM_ROUTEPEN_DAT1PEN_DEFAULT          0x00000000UL                         /**< Mode DEFAULT for PDM_ROUTEPEN */
#define PDM_ROUTEPEN_DAT1PEN_DEFAULT           (_PDM_ROUTEPEN_DAT1PEN_DEFAULT << 1) /**< Shifted mode DEFAULT for PDM_ROUTEPEN */
#define PDM_ROUTEPEN_DAT2PEN                   (0x1UL << 2)                         /**< DAT2 I/O Enable */
#define _PDM_ROUTEPEN_DAT2PEN_SHIFT            2                                    /**< Shift value for PDM_DAT2PEN */
#define _PDM_ROUTEPEN_DAT2PEN_MASK             0x4UL                                /**< Bit mask for PDM_DAT2PEN */
#define _PDM_ROUTEPEN_DAT2PEN_DEFAULT          0x00000000UL                         /**< Mode DEFAULT for PDM_ROUTEPEN */
#define PDM_ROUTEPEN_DAT2PEN_DEFAULT           (_PDM_ROUTEPEN_DAT2PEN_DEFAULT << 2) /**< Shifted mode DEFAULT for PDM_ROUTEPEN */
#define PDM_ROUTEPEN_DAT3PEN                   (0x1UL << 3)                         /**< DAT3 I/O Enable */
#define _PDM_ROUTEPEN_DAT3PEN_SHIFT            3                                    /**< Shift value for PDM_DAT3PEN */
#define _PDM_ROUTEPEN_DAT3PEN_MASK             0x8UL                                /**< Bit mask for PDM_DAT3PEN */
#define _PDM_ROUTEPEN_DAT3PEN_DEFAULT          0x00000000UL                         /**< Mode DEFAULT for PDM_ROUTEPEN */
#define PDM_ROUTEPEN_DAT3PEN_DEFAULT           (_PDM_ROUTEPEN_DAT3PEN_DEFAULT << 3) /**< Shifted mode DEFAULT for PDM_ROUTEPEN */
#define PDM_ROUTEPEN_CLKPEN                    (0x1UL << 8)                         /**< CLK I/O Enable */
#define _PDM_ROUTEPEN_CLKPEN_SHIFT             8                                    /**< Shift value for PDM_CLKPEN */
#define _PDM_ROUTEPEN_CLKPEN_MASK              0x100UL                              /**< Bit mask for PDM_CLKPEN */
#define _PDM_ROUTEPEN_CLKPEN_DEFAULT           0x00000000UL                         /**< Mode DEFAULT for PDM_ROUTEPEN */
#define PDM_ROUTEPEN_CLKPEN_DEFAULT            (_PDM_ROUTEPEN_CLKPEN_DEFAULT << 8)  /**< Shifted mode DEFAULT for PDM_ROUTEPEN */

/* Bit fields for PDM ROUTELOC0 */
#define _PDM_ROUTELOC0_RESETVALUE              0x00000000UL                           /**< Default value for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_MASK                    0x07070707UL                           /**< Mask for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT0LOC_SHIFT           0                                      /**< Shift value for PDM_DAT0LOC */
#define _PDM_ROUTELOC0_DAT0LOC_MASK            0x7UL                                  /**< Bit mask for PDM_DAT0LOC */
#define _PDM_ROUTELOC0_DAT0LOC_LOC0            0x00000000UL                           /**< Mode LOC0 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT0LOC_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT0LOC_LOC1            0x00000001UL                           /**< Mode LOC1 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT0LOC_LOC2            0x00000002UL                           /**< Mode LOC2 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT0LOC_LOC3            0x00000003UL                           /**< Mode LOC3 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT0LOC_LOC4            0x00000004UL                           /**< Mode LOC4 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT0LOC_LOC0             (_PDM_ROUTELOC0_DAT0LOC_LOC0 << 0)     /**< Shifted mode LOC0 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT0LOC_DEFAULT          (_PDM_ROUTELOC0_DAT0LOC_DEFAULT << 0)  /**< Shifted mode DEFAULT for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT0LOC_LOC1             (_PDM_ROUTELOC0_DAT0LOC_LOC1 << 0)     /**< Shifted mode LOC1 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT0LOC_LOC2             (_PDM_ROUTELOC0_DAT0LOC_LOC2 << 0)     /**< Shifted mode LOC2 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT0LOC_LOC3             (_PDM_ROUTELOC0_DAT0LOC_LOC3 << 0)     /**< Shifted mode LOC3 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT0LOC_LOC4             (_PDM_ROUTELOC0_DAT0LOC_LOC4 << 0)     /**< Shifted mode LOC4 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT1LOC_SHIFT           8                                      /**< Shift value for PDM_DAT1LOC */
#define _PDM_ROUTELOC0_DAT1LOC_MASK            0x700UL                                /**< Bit mask for PDM_DAT1LOC */
#define _PDM_ROUTELOC0_DAT1LOC_LOC0            0x00000000UL                           /**< Mode LOC0 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT1LOC_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT1LOC_LOC1            0x00000001UL                           /**< Mode LOC1 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT1LOC_LOC2            0x00000002UL                           /**< Mode LOC2 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT1LOC_LOC3            0x00000003UL                           /**< Mode LOC3 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT1LOC_LOC4            0x00000004UL                           /**< Mode LOC4 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT1LOC_LOC0             (_PDM_ROUTELOC0_DAT1LOC_LOC0 << 8)     /**< Shifted mode LOC0 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT1LOC_DEFAULT          (_PDM_ROUTELOC0_DAT1LOC_DEFAULT << 8)  /**< Shifted mode DEFAULT for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT1LOC_LOC1             (_PDM_ROUTELOC0_DAT1LOC_LOC1 << 8)     /**< Shifted mode LOC1 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT1LOC_LOC2             (_PDM_ROUTELOC0_DAT1LOC_LOC2 << 8)     /**< Shifted mode LOC2 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT1LOC_LOC3             (_PDM_ROUTELOC0_DAT1LOC_LOC3 << 8)     /**< Shifted mode LOC3 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT1LOC_LOC4             (_PDM_ROUTELOC0_DAT1LOC_LOC4 << 8)     /**< Shifted mode LOC4 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT2LOC_SHIFT           16                                     /**< Shift value for PDM_DAT2LOC */
#define _PDM_ROUTELOC0_DAT2LOC_MASK            0x70000UL                              /**< Bit mask for PDM_DAT2LOC */
#define _PDM_ROUTELOC0_DAT2LOC_LOC0            0x00000000UL                           /**< Mode LOC0 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT2LOC_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT2LOC_LOC1            0x00000001UL                           /**< Mode LOC1 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT2LOC_LOC2            0x00000002UL                           /**< Mode LOC2 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT2LOC_LOC3            0x00000003UL                           /**< Mode LOC3 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT2LOC_LOC4            0x00000004UL                           /**< Mode LOC4 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT2LOC_LOC0             (_PDM_ROUTELOC0_DAT2LOC_LOC0 << 16)    /**< Shifted mode LOC0 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT2LOC_DEFAULT          (_PDM_ROUTELOC0_DAT2LOC_DEFAULT << 16) /**< Shifted mode DEFAULT for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT2LOC_LOC1             (_PDM_ROUTELOC0_DAT2LOC_LOC1 << 16)    /**< Shifted mode LOC1 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT2LOC_LOC2             (_PDM_ROUTELOC0_DAT2LOC_LOC2 << 16)    /**< Shifted mode LOC2 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT2LOC_LOC3             (_PDM_ROUTELOC0_DAT2LOC_LOC3 << 16)    /**< Shifted mode LOC3 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT2LOC_LOC4             (_PDM_ROUTELOC0_DAT2LOC_LOC4 << 16)    /**< Shifted mode LOC4 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT3LOC_SHIFT           24                                     /**< Shift value for PDM_DAT3LOC */
#define _PDM_ROUTELOC0_DAT3LOC_MASK            0x7000000UL                            /**< Bit mask for PDM_DAT3LOC */
#define _PDM_ROUTELOC0_DAT3LOC_LOC0            0x00000000UL                           /**< Mode LOC0 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT3LOC_DEFAULT         0x00000000UL                           /**< Mode DEFAULT for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT3LOC_LOC1            0x00000001UL                           /**< Mode LOC1 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT3LOC_LOC2            0x00000002UL                           /**< Mode LOC2 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT3LOC_LOC3            0x00000003UL                           /**< Mode LOC3 for PDM_ROUTELOC0 */
#define _PDM_ROUTELOC0_DAT3LOC_LOC4            0x00000004UL                           /**< Mode LOC4 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT3LOC_LOC0             (_PDM_ROUTELOC0_DAT3LOC_LOC0 << 24)    /**< Shifted mode LOC0 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT3LOC_DEFAULT          (_PDM_ROUTELOC0_DAT3LOC_DEFAULT << 24) /**< Shifted mode DEFAULT for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT3LOC_LOC1             (_PDM_ROUTELOC0_DAT3LOC_LOC1 << 24)    /**< Shifted mode LOC1 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT3LOC_LOC2             (_PDM_ROUTELOC0_DAT3LOC_LOC2 << 24)    /**< Shifted mode LOC2 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT3LOC_LOC3             (_PDM_ROUTELOC0_DAT3LOC_LOC3 << 24)    /**< Shifted mode LOC3 for PDM_ROUTELOC0 */
#define PDM_ROUTELOC0_DAT3LOC_LOC4             (_PDM_ROUTELOC0_DAT3LOC_LOC4 << 24)    /**< Shifted mode LOC4 for PDM_ROUTELOC0 */

/* Bit fields for PDM ROUTELOC1 */
#define _PDM_ROUTELOC1_RESETVALUE              0x00000000UL                         /**< Default value for PDM_ROUTELOC1 */
#define _PDM_ROUTELOC1_MASK                    0x00000007UL                         /**< Mask for PDM_ROUTELOC1 */
#define _PDM_ROUTELOC1_CLKLOC_SHIFT            0                                    /**< Shift value for PDM_CLKLOC */
#define _PDM_ROUTELOC1_CLKLOC_MASK             0x7UL                                /**< Bit mask for PDM_CLKLOC */
#define _PDM_ROUTELOC1_CLKLOC_LOC0             0x00000000UL                         /**< Mode LOC0 for PDM_ROUTELOC1 */
#define _PDM_ROUTELOC1_CLKLOC_DEFAULT          0x00000000UL                         /**< Mode DEFAULT for PDM_ROUTELOC1 */
#define _PDM_ROUTELOC1_CLKLOC_LOC1             0x00000001UL                         /**< Mode LOC1 for PDM_ROUTELOC1 */
#define _PDM_ROUTELOC1_CLKLOC_LOC2             0x00000002UL                         /**< Mode LOC2 for PDM_ROUTELOC1 */
#define _PDM_ROUTELOC1_CLKLOC_LOC3             0x00000003UL                         /**< Mode LOC3 for PDM_ROUTELOC1 */
#define _PDM_ROUTELOC1_CLKLOC_LOC4             0x00000004UL                         /**< Mode LOC4 for PDM_ROUTELOC1 */
#define PDM_ROUTELOC1_CLKLOC_LOC0              (_PDM_ROUTELOC1_CLKLOC_LOC0 << 0)    /**< Shifted mode LOC0 for PDM_ROUTELOC1 */
#define PDM_ROUTELOC1_CLKLOC_DEFAULT           (_PDM_ROUTELOC1_CLKLOC_DEFAULT << 0) /**< Shifted mode DEFAULT for PDM_ROUTELOC1 */
#define PDM_ROUTELOC1_CLKLOC_LOC1              (_PDM_ROUTELOC1_CLKLOC_LOC1 << 0)    /**< Shifted mode LOC1 for PDM_ROUTELOC1 */
#define PDM_ROUTELOC1_CLKLOC_LOC2              (_PDM_ROUTELOC1_CLKLOC_LOC2 << 0)    /**< Shifted mode LOC2 for PDM_ROUTELOC1 */
#define PDM_ROUTELOC1_CLKLOC_LOC3              (_PDM_ROUTELOC1_CLKLOC_LOC3 << 0)    /**< Shifted mode LOC3 for PDM_ROUTELOC1 */
#define PDM_ROUTELOC1_CLKLOC_LOC4              (_PDM_ROUTELOC1_CLKLOC_LOC4 << 0)    /**< Shifted mode LOC4 for PDM_ROUTELOC1 */

/* Bit fields for PDM SYNCBUSY */
#define _PDM_SYNCBUSY_RESETVALUE               0x00000000UL                              /**< Default value for PDM_SYNCBUSY */
#define _PDM_SYNCBUSY_MASK                     0x0000050FUL                              /**< Mask for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_STARTBUSY                 (0x1UL << 0)                              /**< START sync busy */
#define _PDM_SYNCBUSY_STARTBUSY_SHIFT          0                                         /**< Shift value for PDM_STARTBUSY */
#define _PDM_SYNCBUSY_STARTBUSY_MASK           0x1UL                                     /**< Bit mask for PDM_STARTBUSY */
#define _PDM_SYNCBUSY_STARTBUSY_DEFAULT        0x00000000UL                              /**< Mode DEFAULT for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_STARTBUSY_DEFAULT         (_PDM_SYNCBUSY_STARTBUSY_DEFAULT << 0)    /**< Shifted mode DEFAULT for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_STOPBUSY                  (0x1UL << 1)                              /**< STOP sync busy */
#define _PDM_SYNCBUSY_STOPBUSY_SHIFT           1                                         /**< Shift value for PDM_STOPBUSY */
#define _PDM_SYNCBUSY_STOPBUSY_MASK            0x2UL                                     /**< Bit mask for PDM_STOPBUSY */
#define _PDM_SYNCBUSY_STOPBUSY_DEFAULT         0x00000000UL                              /**< Mode DEFAULT for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_STOPBUSY_DEFAULT          (_PDM_SYNCBUSY_STOPBUSY_DEFAULT << 1)     /**< Shifted mode DEFAULT for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_CLEARBUSY                 (0x1UL << 2)                              /**< CLEAR sync busy */
#define _PDM_SYNCBUSY_CLEARBUSY_SHIFT          2                                         /**< Shift value for PDM_CLEARBUSY */
#define _PDM_SYNCBUSY_CLEARBUSY_MASK           0x4UL                                     /**< Bit mask for PDM_CLEARBUSY */
#define _PDM_SYNCBUSY_CLEARBUSY_DEFAULT        0x00000000UL                              /**< Mode DEFAULT for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_CLEARBUSY_DEFAULT         (_PDM_SYNCBUSY_CLEARBUSY_DEFAULT << 2)    /**< Shifted mode DEFAULT for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_FIFOFLBUSY                (0x1UL << 3)                              /**< FIFO Flush Sync busy */
#define _PDM_SYNCBUSY_FIFOFLBUSY_SHIFT         3                                         /**< Shift value for PDM_FIFOFLBUSY */
#define _PDM_SYNCBUSY_FIFOFLBUSY_MASK          0x8UL                                     /**< Bit mask for PDM_FIFOFLBUSY */
#define _PDM_SYNCBUSY_FIFOFLBUSY_DEFAULT       0x00000000UL                              /**< Mode DEFAULT for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_FIFOFLBUSY_DEFAULT        (_PDM_SYNCBUSY_FIFOFLBUSY_DEFAULT << 3)   /**< Shifted mode DEFAULT for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_PRESCBUSY                 (0x1UL << 8)                              /**< PRESC Sync busy */
#define _PDM_SYNCBUSY_PRESCBUSY_SHIFT          8                                         /**< Shift value for PDM_PRESCBUSY */
#define _PDM_SYNCBUSY_PRESCBUSY_MASK           0x100UL                                   /**< Bit mask for PDM_PRESCBUSY */
#define _PDM_SYNCBUSY_PRESCBUSY_DEFAULT        0x00000000UL                              /**< Mode DEFAULT for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_PRESCBUSY_DEFAULT         (_PDM_SYNCBUSY_PRESCBUSY_DEFAULT << 8)    /**< Shifted mode DEFAULT for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_CTRLREGBUSY               (0x1UL << 10)                             /**< CTRLREGBUSY busy */
#define _PDM_SYNCBUSY_CTRLREGBUSY_SHIFT        10                                        /**< Shift value for PDM_CTRLREGBUSY */
#define _PDM_SYNCBUSY_CTRLREGBUSY_MASK         0x400UL                                   /**< Bit mask for PDM_CTRLREGBUSY */
#define _PDM_SYNCBUSY_CTRLREGBUSY_DEFAULT      0x00000000UL                              /**< Mode DEFAULT for PDM_SYNCBUSY */
#define PDM_SYNCBUSY_CTRLREGBUSY_DEFAULT       (_PDM_SYNCBUSY_CTRLREGBUSY_DEFAULT << 10) /**< Shifted mode DEFAULT for PDM_SYNCBUSY */

/** @} */
/** @} End of group EFM32GG12B_PDM */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

