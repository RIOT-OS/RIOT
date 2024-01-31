/***************************************************************************//**
 * @file
 * @brief EFM32GG11B_AF_PINS register and bit field definitions
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
 * @addtogroup EFM32GG11B_Alternate_Function Alternate Function
 * @{
 * @defgroup EFM32GG11B_AF_Pins Alternate Function Pins
 * @{
 ******************************************************************************/

#define AF_CMU_CLK0_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 12 : (i) == 2 ? 7 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 12 :  -1)                                   /**< Pin number for AF_CMU_CLK0 location number i */
#define AF_CMU_CLK1_PIN(i)          ((i) == 0 ? 1 : (i) == 1 ? 8 : (i) == 2 ? 12 : (i) == 3 ? 1 : (i) == 4 ? 3 : (i) == 5 ? 11 :  -1)                                   /**< Pin number for AF_CMU_CLK1 location number i */
#define AF_CMU_CLK2_PIN(i)          ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 6 : (i) == 3 ? 0 : (i) == 4 ? 3 : (i) == 5 ? 10 :  -1)                                    /**< Pin number for AF_CMU_CLK2 location number i */
#define AF_CMU_CLKI0_PIN(i)         ((i) == 0 ? 4 : (i) == 1 ? 3 : (i) == 2 ? 8 : (i) == 3 ? 13 : (i) == 4 ? 1 : (i) == 5 ? 10 : (i) == 6 ? 12 : (i) == 7 ? 11 :  -1)   /**< Pin number for AF_CMU_CLKI0 location number i */
#define AF_CMU_DIGEXTCLK_PIN(i)     ((i) == 0 ? 14 :  -1)                                                                                                               /**< Pin number for AF_CMU_DIGEXTCLK location number i */
#define AF_CMU_IOPOVR_PIN(i)        ((i) == 0 ? 13 :  -1)                                                                                                               /**< Pin number for AF_CMU_IOPOVR location number i */
#define AF_CMU_IONOVR_PIN(i)        ((i) == 0 ? 14 :  -1)                                                                                                               /**< Pin number for AF_CMU_IONOVR location number i */
#define AF_LESENSE_CH0_PIN(i)       ((i) == 0 ? 0 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH0 location number i */
#define AF_LESENSE_CH1_PIN(i)       ((i) == 0 ? 1 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH1 location number i */
#define AF_LESENSE_CH2_PIN(i)       ((i) == 0 ? 2 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH2 location number i */
#define AF_LESENSE_CH3_PIN(i)       ((i) == 0 ? 3 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH3 location number i */
#define AF_LESENSE_CH4_PIN(i)       ((i) == 0 ? 4 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH4 location number i */
#define AF_LESENSE_CH5_PIN(i)       ((i) == 0 ? 5 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH5 location number i */
#define AF_LESENSE_CH6_PIN(i)       ((i) == 0 ? 6 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH6 location number i */
#define AF_LESENSE_CH7_PIN(i)       ((i) == 0 ? 7 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH7 location number i */
#define AF_LESENSE_CH8_PIN(i)       ((i) == 0 ? 8 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH8 location number i */
#define AF_LESENSE_CH9_PIN(i)       ((i) == 0 ? 9 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH9 location number i */
#define AF_LESENSE_CH10_PIN(i)      ((i) == 0 ? 10 :  -1)                                                                                                               /**< Pin number for AF_LESENSE_CH10 location number i */
#define AF_LESENSE_CH11_PIN(i)      ((i) == 0 ? 11 :  -1)                                                                                                               /**< Pin number for AF_LESENSE_CH11 location number i */
#define AF_LESENSE_CH12_PIN(i)      ((i) == 0 ? 12 :  -1)                                                                                                               /**< Pin number for AF_LESENSE_CH12 location number i */
#define AF_LESENSE_CH13_PIN(i)      ((i) == 0 ? 13 :  -1)                                                                                                               /**< Pin number for AF_LESENSE_CH13 location number i */
#define AF_LESENSE_CH14_PIN(i)      ((i) == 0 ? 14 :  -1)                                                                                                               /**< Pin number for AF_LESENSE_CH14 location number i */
#define AF_LESENSE_CH15_PIN(i)      ((i) == 0 ? 15 :  -1)                                                                                                               /**< Pin number for AF_LESENSE_CH15 location number i */
#define AF_LESENSE_ALTEX0_PIN(i)    ((i) == 0 ? 6 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_ALTEX0 location number i */
#define AF_LESENSE_ALTEX1_PIN(i)    ((i) == 0 ? 7 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_ALTEX1 location number i */
#define AF_LESENSE_ALTEX2_PIN(i)    ((i) == 0 ? 3 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_ALTEX2 location number i */
#define AF_LESENSE_ALTEX3_PIN(i)    ((i) == 0 ? 4 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_ALTEX3 location number i */
#define AF_LESENSE_ALTEX4_PIN(i)    ((i) == 0 ? 5 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_ALTEX4 location number i */
#define AF_LESENSE_ALTEX5_PIN(i)    ((i) == 0 ? 11 :  -1)                                                                                                               /**< Pin number for AF_LESENSE_ALTEX5 location number i */
#define AF_LESENSE_ALTEX6_PIN(i)    ((i) == 0 ? 12 :  -1)                                                                                                               /**< Pin number for AF_LESENSE_ALTEX6 location number i */
#define AF_LESENSE_ALTEX7_PIN(i)    ((i) == 0 ? 13 :  -1)                                                                                                               /**< Pin number for AF_LESENSE_ALTEX7 location number i */
#define AF_EBI_AD00_PIN(i)          ((i) == 0 ? 8 : (i) == 1 ? 0 : (i) == 2 ? 0 :  -1)                                                                                  /**< Pin number for AF_EBI_AD00 location number i */
#define AF_EBI_AD01_PIN(i)          ((i) == 0 ? 9 : (i) == 1 ? 1 : (i) == 2 ? 1 :  -1)                                                                                  /**< Pin number for AF_EBI_AD01 location number i */
#define AF_EBI_AD02_PIN(i)          ((i) == 0 ? 10 : (i) == 1 ? 2 : (i) == 2 ? 2 :  -1)                                                                                 /**< Pin number for AF_EBI_AD02 location number i */
#define AF_EBI_AD03_PIN(i)          ((i) == 0 ? 11 : (i) == 1 ? 3 : (i) == 2 ? 3 :  -1)                                                                                 /**< Pin number for AF_EBI_AD03 location number i */
#define AF_EBI_AD04_PIN(i)          ((i) == 0 ? 12 : (i) == 1 ? 4 : (i) == 2 ? 4 :  -1)                                                                                 /**< Pin number for AF_EBI_AD04 location number i */
#define AF_EBI_AD05_PIN(i)          ((i) == 0 ? 13 : (i) == 1 ? 5 : (i) == 2 ? 5 :  -1)                                                                                 /**< Pin number for AF_EBI_AD05 location number i */
#define AF_EBI_AD06_PIN(i)          ((i) == 0 ? 14 : (i) == 1 ? 6 : (i) == 2 ? 6 :  -1)                                                                                 /**< Pin number for AF_EBI_AD06 location number i */
#define AF_EBI_AD07_PIN(i)          ((i) == 0 ? 15 : (i) == 1 ? 0 : (i) == 2 ? 7 :  -1)                                                                                 /**< Pin number for AF_EBI_AD07 location number i */
#define AF_EBI_AD08_PIN(i)          ((i) == 0 ? 15 : (i) == 1 ? 1 : (i) == 2 ? 8 :  -1)                                                                                 /**< Pin number for AF_EBI_AD08 location number i */
#define AF_EBI_AD09_PIN(i)          ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 9 :  -1)                                                                                  /**< Pin number for AF_EBI_AD09 location number i */
#define AF_EBI_AD10_PIN(i)          ((i) == 0 ? 1 : (i) == 1 ? 3 : (i) == 2 ? 10 :  -1)                                                                                 /**< Pin number for AF_EBI_AD10 location number i */
#define AF_EBI_AD11_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 11 :  -1)                                                                                 /**< Pin number for AF_EBI_AD11 location number i */
#define AF_EBI_AD12_PIN(i)          ((i) == 0 ? 3 : (i) == 1 ? 5 : (i) == 2 ? 12 :  -1)                                                                                 /**< Pin number for AF_EBI_AD12 location number i */
#define AF_EBI_AD13_PIN(i)          ((i) == 0 ? 4 : (i) == 1 ? 7 : (i) == 2 ? 13 :  -1)                                                                                 /**< Pin number for AF_EBI_AD13 location number i */
#define AF_EBI_AD14_PIN(i)          ((i) == 0 ? 5 : (i) == 1 ? 8 : (i) == 2 ? 14 :  -1)                                                                                 /**< Pin number for AF_EBI_AD14 location number i */
#define AF_EBI_AD15_PIN(i)          ((i) == 0 ? 6 : (i) == 1 ? 9 : (i) == 2 ? 15 :  -1)                                                                                 /**< Pin number for AF_EBI_AD15 location number i */
#define AF_EBI_CS0_PIN(i)           ((i) == 0 ? 9 : (i) == 1 ? 10 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 8 :  -1)                                                   /**< Pin number for AF_EBI_CS0 location number i */
#define AF_EBI_CS1_PIN(i)           ((i) == 0 ? 10 : (i) == 1 ? 11 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 9 :  -1)                                                  /**< Pin number for AF_EBI_CS1 location number i */
#define AF_EBI_CS2_PIN(i)           ((i) == 0 ? 11 : (i) == 1 ? 12 : (i) == 2 ? 2 : (i) == 3 ? 2 : (i) == 4 ? 10 :  -1)                                                 /**< Pin number for AF_EBI_CS2 location number i */
#define AF_EBI_CS3_PIN(i)           ((i) == 0 ? 12 : (i) == 1 ? 15 : (i) == 2 ? 3 : (i) == 3 ? 3 : (i) == 4 ? 11 :  -1)                                                 /**< Pin number for AF_EBI_CS3 location number i */
#define AF_EBI_ARDY_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 13 : (i) == 2 ? 15 : (i) == 3 ? 4 : (i) == 4 ? 13 : (i) == 5 ? 10 :  -1)                                 /**< Pin number for AF_EBI_ARDY location number i */
#define AF_EBI_ALE_PIN(i)           ((i) == 0 ? 3 : (i) == 1 ? 9 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 11 : (i) == 5 ? 11 :  -1)                                   /**< Pin number for AF_EBI_ALE location number i */
#define AF_EBI_WEn_PIN(i)           ((i) == 0 ? 4 : (i) == 1 ? 13 : (i) == 2 ? 5 : (i) == 3 ? 6 : (i) == 4 ? 8 : (i) == 5 ? 4 :  -1)                                    /**< Pin number for AF_EBI_WEn location number i */
#define AF_EBI_REn_PIN(i)           ((i) == 0 ? 5 : (i) == 1 ? 14 : (i) == 2 ? 12 : (i) == 3 ? 0 : (i) == 4 ? 9 : (i) == 5 ? 5 :  -1)                                   /**< Pin number for AF_EBI_REn location number i */
#define AF_EBI_BL0_PIN(i)           ((i) == 0 ? 6 : (i) == 1 ? 8 : (i) == 2 ? 10 : (i) == 3 ? 1 : (i) == 4 ? 6 : (i) == 5 ? 6 :  -1)                                    /**< Pin number for AF_EBI_BL0 location number i */
#define AF_EBI_BL1_PIN(i)           ((i) == 0 ? 7 : (i) == 1 ? 9 : (i) == 2 ? 11 : (i) == 3 ? 3 : (i) == 4 ? 7 : (i) == 5 ? 7 :  -1)                                    /**< Pin number for AF_EBI_BL1 location number i */
#define AF_EBI_NANDWEn_PIN(i)       ((i) == 0 ? 5 : (i) == 1 ? 14 : (i) == 2 ? 13 : (i) == 3 ? 2 : (i) == 4 ? 14 : (i) == 5 ? 11 :  -1)                                 /**< Pin number for AF_EBI_NANDWEn location number i */
#define AF_EBI_NANDREn_PIN(i)       ((i) == 0 ? 3 : (i) == 1 ? 15 : (i) == 2 ? 9 : (i) == 3 ? 4 : (i) == 4 ? 15 : (i) == 5 ? 12 :  -1)                                  /**< Pin number for AF_EBI_NANDREn location number i */
#define AF_EBI_A00_PIN(i)           ((i) == 0 ? 12 : (i) == 1 ? 9 : (i) == 2 ? 0 : (i) == 3 ? 5 :  -1)                                                                  /**< Pin number for AF_EBI_A00 location number i */
#define AF_EBI_A01_PIN(i)           ((i) == 0 ? 13 : (i) == 1 ? 10 : (i) == 2 ? 1 : (i) == 3 ? 7 :  -1)                                                                 /**< Pin number for AF_EBI_A01 location number i */
#define AF_EBI_A02_PIN(i)           ((i) == 0 ? 14 : (i) == 1 ? 11 : (i) == 2 ? 0 : (i) == 3 ? 8 :  -1)                                                                 /**< Pin number for AF_EBI_A02 location number i */
#define AF_EBI_A03_PIN(i)           ((i) == 0 ? 9 : (i) == 1 ? 12 : (i) == 2 ? 1 : (i) == 3 ? 9 :  -1)                                                                  /**< Pin number for AF_EBI_A03 location number i */
#define AF_EBI_A04_PIN(i)           ((i) == 0 ? 10 : (i) == 1 ? 0 : (i) == 2 ? 2 : (i) == 3 ? 10 :  -1)                                                                 /**< Pin number for AF_EBI_A04 location number i */
#define AF_EBI_A05_PIN(i)           ((i) == 0 ? 6 : (i) == 1 ? 1 : (i) == 2 ? 3 : (i) == 3 ? 11 :  -1)                                                                  /**< Pin number for AF_EBI_A05 location number i */
#define AF_EBI_A06_PIN(i)           ((i) == 0 ? 7 : (i) == 1 ? 2 : (i) == 2 ? 4 : (i) == 3 ? 12 :  -1)                                                                  /**< Pin number for AF_EBI_A06 location number i */
#define AF_EBI_A07_PIN(i)           ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 5 : (i) == 3 ? 13 :  -1)                                                                  /**< Pin number for AF_EBI_A07 location number i */
#define AF_EBI_A08_PIN(i)           ((i) == 0 ? 1 : (i) == 1 ? 4 : (i) == 2 ? 8 : (i) == 3 ? 14 :  -1)                                                                  /**< Pin number for AF_EBI_A08 location number i */
#define AF_EBI_A09_PIN(i)           ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? 9 : (i) == 3 ? 9 :  -1)                                                                   /**< Pin number for AF_EBI_A09 location number i */
#define AF_EBI_A10_PIN(i)           ((i) == 0 ? 3 : (i) == 1 ? 6 : (i) == 2 ? 10 : (i) == 3 ? 10 :  -1)                                                                 /**< Pin number for AF_EBI_A10 location number i */
#define AF_EBI_A11_PIN(i)           ((i) == 0 ? 4 : (i) == 1 ? 7 : (i) == 2 ? 6 : (i) == 3 ? 11 :  -1)                                                                  /**< Pin number for AF_EBI_A11 location number i */
#define AF_EBI_A12_PIN(i)           ((i) == 0 ? 5 : (i) == 1 ? 8 : (i) == 2 ? 7 : (i) == 3 ? 12 :  -1)                                                                  /**< Pin number for AF_EBI_A12 location number i */
#define AF_EBI_A13_PIN(i)           ((i) == 0 ? 6 : (i) == 1 ? 7 : (i) == 2 ? 8 : (i) == 3 ? 0 :  -1)                                                                   /**< Pin number for AF_EBI_A13 location number i */
#define AF_EBI_A14_PIN(i)           ((i) == 0 ? 7 : (i) == 1 ? 2 : (i) == 2 ? 9 : (i) == 3 ? 1 :  -1)                                                                   /**< Pin number for AF_EBI_A14 location number i */
#define AF_EBI_A15_PIN(i)           ((i) == 0 ? 8 : (i) == 1 ? 3 : (i) == 2 ? 10 : (i) == 3 ? 2 :  -1)                                                                  /**< Pin number for AF_EBI_A15 location number i */
#define AF_EBI_A16_PIN(i)           ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 4 : (i) == 3 ? 3 :  -1)                                                                   /**< Pin number for AF_EBI_A16 location number i */
#define AF_EBI_A17_PIN(i)           ((i) == 0 ? 1 : (i) == 1 ? 5 : (i) == 2 ? 5 : (i) == 3 ? 4 :  -1)                                                                   /**< Pin number for AF_EBI_A17 location number i */
#define AF_EBI_A18_PIN(i)           ((i) == 0 ? 2 : (i) == 1 ? 6 : (i) == 2 ? 6 : (i) == 3 ? 5 :  -1)                                                                   /**< Pin number for AF_EBI_A18 location number i */
#define AF_EBI_A19_PIN(i)           ((i) == 0 ? 3 : (i) == 1 ? 7 : (i) == 2 ? 7 : (i) == 3 ? 6 :  -1)                                                                   /**< Pin number for AF_EBI_A19 location number i */
#define AF_EBI_A20_PIN(i)           ((i) == 0 ? 4 : (i) == 1 ? 8 : (i) == 2 ? 8 : (i) == 3 ? 7 :  -1)                                                                   /**< Pin number for AF_EBI_A20 location number i */
#define AF_EBI_A21_PIN(i)           ((i) == 0 ? 5 : (i) == 1 ? 9 : (i) == 2 ? 9 : (i) == 3 ? 7 :  -1)                                                                   /**< Pin number for AF_EBI_A21 location number i */
#define AF_EBI_A22_PIN(i)           ((i) == 0 ? 6 : (i) == 1 ? 10 : (i) == 2 ? 10 : (i) == 3 ? 4 :  -1)                                                                 /**< Pin number for AF_EBI_A22 location number i */
#define AF_EBI_A23_PIN(i)           ((i) == 0 ? 0 : (i) == 1 ? 11 : (i) == 2 ? 11 : (i) == 3 ? 5 :  -1)                                                                 /**< Pin number for AF_EBI_A23 location number i */
#define AF_EBI_A24_PIN(i)           ((i) == 0 ? 1 : (i) == 1 ? 0 : (i) == 2 ? 12 : (i) == 3 ? 6 :  -1)                                                                  /**< Pin number for AF_EBI_A24 location number i */
#define AF_EBI_A25_PIN(i)           ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 13 : (i) == 3 ? 7 :  -1)                                                                  /**< Pin number for AF_EBI_A25 location number i */
#define AF_EBI_A26_PIN(i)           ((i) == 0 ? 4 : (i) == 1 ? 2 : (i) == 2 ? 14 : (i) == 3 ? 8 :  -1)                                                                  /**< Pin number for AF_EBI_A26 location number i */
#define AF_EBI_A27_PIN(i)           ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? 15 : (i) == 3 ? 9 :  -1)                                                                  /**< Pin number for AF_EBI_A27 location number i */
#define AF_EBI_CSTFT_PIN(i)         ((i) == 0 ? 7 : (i) == 1 ? 6 : (i) == 2 ? 12 : (i) == 3 ? 0 :  -1)                                                                  /**< Pin number for AF_EBI_CSTFT location number i */
#define AF_EBI_DCLK_PIN(i)          ((i) == 0 ? 8 : (i) == 1 ? 7 : (i) == 2 ? 0 : (i) == 3 ? 1 :  -1)                                                                   /**< Pin number for AF_EBI_DCLK location number i */
#define AF_EBI_DTEN_PIN(i)          ((i) == 0 ? 9 : (i) == 1 ? 9 : (i) == 2 ? 1 : (i) == 3 ? 2 :  -1)                                                                   /**< Pin number for AF_EBI_DTEN location number i */
#define AF_EBI_VSNC_PIN(i)          ((i) == 0 ? 10 : (i) == 1 ? 10 : (i) == 2 ? 2 : (i) == 3 ? 3 :  -1)                                                                 /**< Pin number for AF_EBI_VSNC location number i */
#define AF_EBI_HSNC_PIN(i)          ((i) == 0 ? 11 : (i) == 1 ? 11 : (i) == 2 ? 3 : (i) == 3 ? 4 :  -1)                                                                 /**< Pin number for AF_EBI_HSNC location number i */
#define AF_ETH_MIITXCLK_PIN(i)      ((i) == 0 ? 0 : (i) == 1 ? 0 :  -1)                                                                                                 /**< Pin number for AF_ETH_MIITXCLK location number i */
#define AF_ETH_MIITXD3_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 1 :  -1)                                                                                                 /**< Pin number for AF_ETH_MIITXD3 location number i */
#define AF_ETH_MIITXD2_PIN(i)       ((i) == 0 ? 2 : (i) == 1 ? 2 :  -1)                                                                                                 /**< Pin number for AF_ETH_MIITXD2 location number i */
#define AF_ETH_MIITXD1_PIN(i)       ((i) == 0 ? 3 : (i) == 1 ? 3 :  -1)                                                                                                 /**< Pin number for AF_ETH_MIITXD1 location number i */
#define AF_ETH_MIITXD0_PIN(i)       ((i) == 0 ? 4 : (i) == 1 ? 4 :  -1)                                                                                                 /**< Pin number for AF_ETH_MIITXD0 location number i */
#define AF_ETH_MIITXEN_PIN(i)       ((i) == 0 ? 5 : (i) == 1 ? 5 :  -1)                                                                                                 /**< Pin number for AF_ETH_MIITXEN location number i */
#define AF_ETH_MIITXER_PIN(i)       ((i) == 0 ? 6 : (i) == 1 ? 6 :  -1)                                                                                                 /**< Pin number for AF_ETH_MIITXER location number i */
#define AF_ETH_MIIRXCLK_PIN(i)      ((i) == 0 ? 15 : (i) == 1 ? 7 : (i) == 2 ? 12 :  -1)                                                                                /**< Pin number for AF_ETH_MIIRXCLK location number i */
#define AF_ETH_MIIRXD3_PIN(i)       ((i) == 0 ? 15 : (i) == 1 ? 8 : (i) == 2 ? 11 :  -1)                                                                                /**< Pin number for AF_ETH_MIIRXD3 location number i */
#define AF_ETH_MIIRXD2_PIN(i)       ((i) == 0 ? 14 : (i) == 1 ? 9 : (i) == 2 ? 10 :  -1)                                                                                /**< Pin number for AF_ETH_MIIRXD2 location number i */
#define AF_ETH_MIIRXD1_PIN(i)       ((i) == 0 ? 13 : (i) == 1 ? 10 : (i) == 2 ? 9 :  -1)                                                                                /**< Pin number for AF_ETH_MIIRXD1 location number i */
#define AF_ETH_MIIRXD0_PIN(i)       ((i) == 0 ? 12 : (i) == 1 ? 11 : (i) == 2 ? 9 :  -1)                                                                                /**< Pin number for AF_ETH_MIIRXD0 location number i */
#define AF_ETH_MIIRXDV_PIN(i)       ((i) == 0 ? 11 : (i) == 1 ? 12 : (i) == 2 ? 8 :  -1)                                                                                /**< Pin number for AF_ETH_MIIRXDV location number i */
#define AF_ETH_MIIRXER_PIN(i)       ((i) == 0 ? 10 : (i) == 1 ? 13 : (i) == 2 ? 7 :  -1)                                                                                /**< Pin number for AF_ETH_MIIRXER location number i */
#define AF_ETH_MIICRS_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 14 : (i) == 2 ? 3 :  -1)                                                                                 /**< Pin number for AF_ETH_MIICRS location number i */
#define AF_ETH_MIICOL_PIN(i)        ((i) == 0 ? 2 : (i) == 1 ? 15 : (i) == 2 ? 4 :  -1)                                                                                 /**< Pin number for AF_ETH_MIICOL location number i */
#define AF_ETH_MDIO_PIN(i)          ((i) == 0 ? 3 : (i) == 1 ? 13 : (i) == 2 ? 0 : (i) == 3 ? 15 :  -1)                                                                 /**< Pin number for AF_ETH_MDIO location number i */
#define AF_ETH_MDC_PIN(i)           ((i) == 0 ? 4 : (i) == 1 ? 14 : (i) == 2 ? 1 : (i) == 3 ? 6 :  -1)                                                                  /**< Pin number for AF_ETH_MDC location number i */
#define AF_ETH_TSUEXTCLK_PIN(i)     ((i) == 0 ? 5 : (i) == 1 ? 15 : (i) == 2 ? 2 : (i) == 3 ? 8 :  -1)                                                                  /**< Pin number for AF_ETH_TSUEXTCLK location number i */
#define AF_ETH_TSUTMRTOG_PIN(i)     ((i) == 0 ? 6 : (i) == 1 ? 15 : (i) == 2 ? 3 : (i) == 3 ? 9 :  -1)                                                                  /**< Pin number for AF_ETH_TSUTMRTOG location number i */
#define AF_ETH_RMIITXD1_PIN(i)      ((i) == 0 ? 14 : (i) == 1 ? 6 :  -1)                                                                                                /**< Pin number for AF_ETH_RMIITXD1 location number i */
#define AF_ETH_RMIITXD0_PIN(i)      ((i) == 0 ? 15 : (i) == 1 ? 7 :  -1)                                                                                                /**< Pin number for AF_ETH_RMIITXD0 location number i */
#define AF_ETH_RMIITXEN_PIN(i)      ((i) == 0 ? 0 : (i) == 1 ? 8 :  -1)                                                                                                 /**< Pin number for AF_ETH_RMIITXEN location number i */
#define AF_ETH_RMIIRXD1_PIN(i)      ((i) == 0 ? 1 : (i) == 1 ? 9 :  -1)                                                                                                 /**< Pin number for AF_ETH_RMIIRXD1 location number i */
#define AF_ETH_RMIIRXD0_PIN(i)      ((i) == 0 ? 2 : (i) == 1 ? 9 :  -1)                                                                                                 /**< Pin number for AF_ETH_RMIIRXD0 location number i */
#define AF_ETH_RMIIREFCLK_PIN(i)    ((i) == 0 ? 3 : (i) == 1 ? 10 :  -1)                                                                                                /**< Pin number for AF_ETH_RMIIREFCLK location number i */
#define AF_ETH_RMIICRSDV_PIN(i)     ((i) == 0 ? 4 : (i) == 1 ? 11 :  -1)                                                                                                /**< Pin number for AF_ETH_RMIICRSDV location number i */
#define AF_ETH_RMIIRXER_PIN(i)      ((i) == 0 ? 5 : (i) == 1 ? 12 :  -1)                                                                                                /**< Pin number for AF_ETH_RMIIRXER location number i */
#define AF_SDIO_CLK_PIN(i)          ((i) == 0 ? 13 : (i) == 1 ? 14 :  -1)                                                                                               /**< Pin number for AF_SDIO_CLK location number i */
#define AF_SDIO_CMD_PIN(i)          ((i) == 0 ? 12 : (i) == 1 ? 15 :  -1)                                                                                               /**< Pin number for AF_SDIO_CMD location number i */
#define AF_SDIO_DAT0_PIN(i)         ((i) == 0 ? 11 : (i) == 1 ? 0 :  -1)                                                                                                /**< Pin number for AF_SDIO_DAT0 location number i */
#define AF_SDIO_DAT1_PIN(i)         ((i) == 0 ? 10 : (i) == 1 ? 1 :  -1)                                                                                                /**< Pin number for AF_SDIO_DAT1 location number i */
#define AF_SDIO_DAT2_PIN(i)         ((i) == 0 ? 9 : (i) == 1 ? 2 :  -1)                                                                                                 /**< Pin number for AF_SDIO_DAT2 location number i */
#define AF_SDIO_DAT3_PIN(i)         ((i) == 0 ? 8 : (i) == 1 ? 3 :  -1)                                                                                                 /**< Pin number for AF_SDIO_DAT3 location number i */
#define AF_SDIO_DAT4_PIN(i)         ((i) == 0 ? 12 : (i) == 1 ? 4 :  -1)                                                                                                /**< Pin number for AF_SDIO_DAT4 location number i */
#define AF_SDIO_DAT5_PIN(i)         ((i) == 0 ? 11 : (i) == 1 ? 5 :  -1)                                                                                                /**< Pin number for AF_SDIO_DAT5 location number i */
#define AF_SDIO_DAT6_PIN(i)         ((i) == 0 ? 10 : (i) == 1 ? 3 :  -1)                                                                                                /**< Pin number for AF_SDIO_DAT6 location number i */
#define AF_SDIO_DAT7_PIN(i)         ((i) == 0 ? 9 : (i) == 1 ? 4 :  -1)                                                                                                 /**< Pin number for AF_SDIO_DAT7 location number i */
#define AF_SDIO_CD_PIN(i)           ((i) == 0 ? 8 : (i) == 1 ? 4 : (i) == 2 ? 6 : (i) == 3 ? 10 :  -1)                                                                  /**< Pin number for AF_SDIO_CD location number i */
#define AF_SDIO_WP_PIN(i)           ((i) == 0 ? 9 : (i) == 1 ? 5 : (i) == 2 ? 15 : (i) == 3 ? 9 :  -1)                                                                  /**< Pin number for AF_SDIO_WP location number i */
#define AF_PRS_CH0_PIN(i)           ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 14 : (i) == 3 ? 2 :  -1)                                                                  /**< Pin number for AF_PRS_CH0 location number i */
#define AF_PRS_CH1_PIN(i)           ((i) == 0 ? 1 : (i) == 1 ? 4 : (i) == 2 ? 15 : (i) == 3 ? 12 :  -1)                                                                 /**< Pin number for AF_PRS_CH1 location number i */
#define AF_PRS_CH2_PIN(i)           ((i) == 0 ? 0 : (i) == 1 ? 5 : (i) == 2 ? 10 : (i) == 3 ? 13 :  -1)                                                                 /**< Pin number for AF_PRS_CH2 location number i */
#define AF_PRS_CH3_PIN(i)           ((i) == 0 ? 1 : (i) == 1 ? 8 : (i) == 2 ? 11 : (i) == 3 ? 0 :  -1)                                                                  /**< Pin number for AF_PRS_CH3 location number i */
#define AF_PRS_CH4_PIN(i)           ((i) == 0 ? 8 : (i) == 1 ? 0 : (i) == 2 ? 1 :  -1)                                                                                  /**< Pin number for AF_PRS_CH4 location number i */
#define AF_PRS_CH5_PIN(i)           ((i) == 0 ? 9 : (i) == 1 ? 1 : (i) == 2 ? 6 :  -1)                                                                                  /**< Pin number for AF_PRS_CH5 location number i */
#define AF_PRS_CH6_PIN(i)           ((i) == 0 ? 6 : (i) == 1 ? 14 : (i) == 2 ? 6 :  -1)                                                                                 /**< Pin number for AF_PRS_CH6 location number i */
#define AF_PRS_CH7_PIN(i)           ((i) == 0 ? 13 : (i) == 1 ? 7 : (i) == 2 ? 7 :  -1)                                                                                 /**< Pin number for AF_PRS_CH7 location number i */
#define AF_PRS_CH8_PIN(i)           ((i) == 0 ? 8 : (i) == 1 ? 2 : (i) == 2 ? 9 :  -1)                                                                                  /**< Pin number for AF_PRS_CH8 location number i */
#define AF_PRS_CH9_PIN(i)           ((i) == 0 ? 9 : (i) == 1 ? 3 : (i) == 2 ? 10 :  -1)                                                                                 /**< Pin number for AF_PRS_CH9 location number i */
#define AF_PRS_CH10_PIN(i)          ((i) == 0 ? 10 : (i) == 1 ? 2 : (i) == 2 ? 4 :  -1)                                                                                 /**< Pin number for AF_PRS_CH10 location number i */
#define AF_PRS_CH11_PIN(i)          ((i) == 0 ? 11 : (i) == 1 ? 3 : (i) == 2 ? 5 :  -1)                                                                                 /**< Pin number for AF_PRS_CH11 location number i */
#define AF_PRS_CH12_PIN(i)          ((i) == 0 ? 12 : (i) == 1 ? 6 : (i) == 2 ? 8 :  -1)                                                                                 /**< Pin number for AF_PRS_CH12 location number i */
#define AF_PRS_CH13_PIN(i)          ((i) == 0 ? 13 : (i) == 1 ? 9 : (i) == 2 ? 14 :  -1)                                                                                /**< Pin number for AF_PRS_CH13 location number i */
#define AF_PRS_CH14_PIN(i)          ((i) == 0 ? 14 : (i) == 1 ? 6 : (i) == 2 ? 15 :  -1)                                                                                /**< Pin number for AF_PRS_CH14 location number i */
#define AF_PRS_CH15_PIN(i)          ((i) == 0 ? 15 : (i) == 1 ? 7 : (i) == 2 ? 0 :  -1)                                                                                 /**< Pin number for AF_PRS_CH15 location number i */
#define AF_PRS_CH16_PIN(i)          ((i) == 0 ? 4 : (i) == 1 ? 12 : (i) == 2 ? 4 :  -1)                                                                                 /**< Pin number for AF_PRS_CH16 location number i */
#define AF_PRS_CH17_PIN(i)          ((i) == 0 ? 5 : (i) == 1 ? 15 : (i) == 2 ? 5 :  -1)                                                                                 /**< Pin number for AF_PRS_CH17 location number i */
#define AF_PRS_CH18_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 10 : (i) == 2 ? 4 :  -1)                                                                                 /**< Pin number for AF_PRS_CH18 location number i */
#define AF_PRS_CH19_PIN(i)          ((i) == 0 ? 3 : (i) == 1 ? 11 : (i) == 2 ? 5 :  -1)                                                                                 /**< Pin number for AF_PRS_CH19 location number i */
#define AF_PRS_CH20_PIN(i)          ((i) == 0 ? 4 : (i) == 1 ? 12 : (i) == 2 ? 2 :  -1)                                                                                 /**< Pin number for AF_PRS_CH20 location number i */
#define AF_PRS_CH21_PIN(i)          ((i) == 0 ? 5 : (i) == 1 ? 13 : (i) == 2 ? 11 :  -1)                                                                                /**< Pin number for AF_PRS_CH21 location number i */
#define AF_PRS_CH22_PIN(i)          ((i) == 0 ? 7 : (i) == 1 ? 0 : (i) == 2 ? 6 :  -1)                                                                                  /**< Pin number for AF_PRS_CH22 location number i */
#define AF_PRS_CH23_PIN(i)          ((i) == 0 ? 8 : (i) == 1 ? 1 : (i) == 2 ? 7 :  -1)                                                                                  /**< Pin number for AF_PRS_CH23 location number i */
#define AF_CAN0_RX_PIN(i)           ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 9 : (i) == 4 ? 8 : (i) == 5 ? 14 : (i) == 6 ? 0 : (i) == 7 ? 12 :  -1)     /**< Pin number for AF_CAN0_RX location number i */
#define AF_CAN0_TX_PIN(i)           ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 1 : (i) == 3 ? 10 : (i) == 4 ? 9 : (i) == 5 ? 15 : (i) == 6 ? 1 : (i) == 7 ? 13 :  -1)    /**< Pin number for AF_CAN0_TX location number i */
#define AF_CAN1_RX_PIN(i)           ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 3 : (i) == 3 ? 9 : (i) == 4 ? 12 : (i) == 5 ? 12 : (i) == 6 ? 10 : (i) == 7 ? 14 :  -1)   /**< Pin number for AF_CAN1_RX location number i */
#define AF_CAN1_TX_PIN(i)           ((i) == 0 ? 3 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? 10 : (i) == 4 ? 11 : (i) == 5 ? 13 : (i) == 6 ? 11 : (i) == 7 ? 15 :  -1)  /**< Pin number for AF_CAN1_TX location number i */
#define AF_TIMER0_CC0_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 6 : (i) == 2 ? 1 : (i) == 3 ? 6 : (i) == 4 ? 0 : (i) == 5 ? 4 : (i) == 6 ? 8 : (i) == 7 ? 1 :  -1)       /**< Pin number for AF_TIMER0_CC0 location number i */
#define AF_TIMER0_CC1_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 7 : (i) == 2 ? 2 : (i) == 3 ? 0 : (i) == 4 ? 1 : (i) == 5 ? 5 : (i) == 6 ? 9 : (i) == 7 ? 0 :  -1)       /**< Pin number for AF_TIMER0_CC1 location number i */
#define AF_TIMER0_CC2_PIN(i)        ((i) == 0 ? 2 : (i) == 1 ? 8 : (i) == 2 ? 3 : (i) == 3 ? 1 : (i) == 4 ? 2 : (i) == 5 ? 7 : (i) == 6 ? 10 : (i) == 7 ? 13 :  -1)     /**< Pin number for AF_TIMER0_CC2 location number i */
#define AF_TIMER0_CC3_PIN(i)        (-1)                                                                                                                                /**< Pin number for AF_TIMER0_CC3 location number i */
#define AF_TIMER0_CDTI0_PIN(i)      ((i) == 0 ? 3 : (i) == 1 ? 13 : (i) == 2 ? 3 : (i) == 3 ? 2 : (i) == 4 ? 7 :  -1)                                                   /**< Pin number for AF_TIMER0_CDTI0 location number i */
#define AF_TIMER0_CDTI1_PIN(i)      ((i) == 0 ? 4 : (i) == 1 ? 14 : (i) == 2 ? 4 : (i) == 3 ? 3 : (i) == 4 ? 8 :  -1)                                                   /**< Pin number for AF_TIMER0_CDTI1 location number i */
#define AF_TIMER0_CDTI2_PIN(i)      ((i) == 0 ? 5 : (i) == 1 ? 15 : (i) == 2 ? 5 : (i) == 3 ? 4 : (i) == 4 ? 11 :  -1)                                                  /**< Pin number for AF_TIMER0_CDTI2 location number i */
#define AF_TIMER0_CDTI3_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER0_CDTI3 location number i */
#define AF_TIMER1_CC0_PIN(i)        ((i) == 0 ? 13 : (i) == 1 ? 10 : (i) == 2 ? 0 : (i) == 3 ? 7 : (i) == 4 ? 6 : (i) == 5 ? 2 : (i) == 6 ? 13 : (i) == 7 ? 6 :  -1)    /**< Pin number for AF_TIMER1_CC0 location number i */
#define AF_TIMER1_CC1_PIN(i)        ((i) == 0 ? 14 : (i) == 1 ? 11 : (i) == 2 ? 1 : (i) == 3 ? 8 : (i) == 4 ? 7 : (i) == 5 ? 3 : (i) == 6 ? 14 : (i) == 7 ? 7 :  -1)    /**< Pin number for AF_TIMER1_CC1 location number i */
#define AF_TIMER1_CC2_PIN(i)        ((i) == 0 ? 15 : (i) == 1 ? 12 : (i) == 2 ? 2 : (i) == 3 ? 11 : (i) == 4 ? 13 : (i) == 5 ? 4 : (i) == 6 ? 15 : (i) == 7 ? 8 :  -1)  /**< Pin number for AF_TIMER1_CC2 location number i */
#define AF_TIMER1_CC3_PIN(i)        ((i) == 0 ? 12 : (i) == 1 ? 13 : (i) == 2 ? 3 : (i) == 3 ? 12 : (i) == 4 ? 14 : (i) == 5 ? 12 : (i) == 6 ? 5 : (i) == 7 ? 9 :  -1)  /**< Pin number for AF_TIMER1_CC3 location number i */
#define AF_TIMER1_CDTI0_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER1_CDTI0 location number i */
#define AF_TIMER1_CDTI1_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER1_CDTI1 location number i */
#define AF_TIMER1_CDTI2_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER1_CDTI2 location number i */
#define AF_TIMER1_CDTI3_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER1_CDTI3 location number i */
#define AF_TIMER2_CC0_PIN(i)        ((i) == 0 ? 8 : (i) == 1 ? 12 : (i) == 2 ? 8 : (i) == 3 ? 2 : (i) == 4 ? 6 : (i) == 5 ? 2 : (i) == 6 ? 8 : (i) == 7 ? 5 :  -1)      /**< Pin number for AF_TIMER2_CC0 location number i */
#define AF_TIMER2_CC1_PIN(i)        ((i) == 0 ? 9 : (i) == 1 ? 13 : (i) == 2 ? 9 : (i) == 3 ? 12 : (i) == 4 ? 0 : (i) == 5 ? 3 : (i) == 6 ? 9 : (i) == 7 ? 6 :  -1)     /**< Pin number for AF_TIMER2_CC1 location number i */
#define AF_TIMER2_CC2_PIN(i)        ((i) == 0 ? 10 : (i) == 1 ? 14 : (i) == 2 ? 10 : (i) == 3 ? 13 : (i) == 4 ? 1 : (i) == 5 ? 4 : (i) == 6 ? 10 : (i) == 7 ? 7 :  -1)  /**< Pin number for AF_TIMER2_CC2 location number i */
#define AF_TIMER2_CC3_PIN(i)        (-1)                                                                                                                                /**< Pin number for AF_TIMER2_CC3 location number i */
#define AF_TIMER2_CDTI0_PIN(i)      ((i) == 0 ? 0 : (i) == 1 ? 13 : (i) == 2 ? 8 : (i) == 3 ? 0 :  -1)                                                                  /**< Pin number for AF_TIMER2_CDTI0 location number i */
#define AF_TIMER2_CDTI1_PIN(i)      ((i) == 0 ? 1 : (i) == 1 ? 14 : (i) == 2 ? 14 : (i) == 3 ? 1 :  -1)                                                                 /**< Pin number for AF_TIMER2_CDTI1 location number i */
#define AF_TIMER2_CDTI2_PIN(i)      ((i) == 0 ? 2 : (i) == 1 ? 15 : (i) == 2 ? 15 : (i) == 3 ? 2 :  -1)                                                                 /**< Pin number for AF_TIMER2_CDTI2 location number i */
#define AF_TIMER2_CDTI3_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER2_CDTI3 location number i */
#define AF_TIMER3_CC0_PIN(i)        ((i) == 0 ? 14 : (i) == 1 ? 0 : (i) == 2 ? 3 : (i) == 3 ? 5 : (i) == 4 ? 0 : (i) == 5 ? 3 : (i) == 6 ? 6 : (i) == 7 ? 15 :  -1)     /**< Pin number for AF_TIMER3_CC0 location number i */
#define AF_TIMER3_CC1_PIN(i)        ((i) == 0 ? 15 : (i) == 1 ? 1 : (i) == 2 ? 4 : (i) == 3 ? 6 : (i) == 4 ? 1 : (i) == 5 ? 4 : (i) == 6 ? 13 : (i) == 7 ? 15 :  -1)    /**< Pin number for AF_TIMER3_CC1 location number i */
#define AF_TIMER3_CC2_PIN(i)        ((i) == 0 ? 15 : (i) == 1 ? 2 : (i) == 2 ? 5 : (i) == 3 ? 7 : (i) == 4 ? 2 : (i) == 5 ? 5 : (i) == 6 ? 14 : (i) == 7 ? 0 :  -1)     /**< Pin number for AF_TIMER3_CC2 location number i */
#define AF_TIMER3_CC3_PIN(i)        (-1)                                                                                                                                /**< Pin number for AF_TIMER3_CC3 location number i */
#define AF_TIMER3_CDTI0_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER3_CDTI0 location number i */
#define AF_TIMER3_CDTI1_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER3_CDTI1 location number i */
#define AF_TIMER3_CDTI2_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER3_CDTI2 location number i */
#define AF_TIMER3_CDTI3_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER3_CDTI3 location number i */
#define AF_TIMER4_CC0_PIN(i)        ((i) == 0 ? 3 : (i) == 1 ? 13 : (i) == 2 ? 5 : (i) == 3 ? 8 : (i) == 4 ? 6 : (i) == 5 ? 9 : (i) == 6 ? 11 : (i) == 7 ? 9 :  -1)     /**< Pin number for AF_TIMER4_CC0 location number i */
#define AF_TIMER4_CC1_PIN(i)        ((i) == 0 ? 4 : (i) == 1 ? 14 : (i) == 2 ? 6 : (i) == 3 ? 9 : (i) == 4 ? 7 : (i) == 5 ? 9 : (i) == 6 ? 12 : (i) == 7 ? 10 :  -1)    /**< Pin number for AF_TIMER4_CC1 location number i */
#define AF_TIMER4_CC2_PIN(i)        ((i) == 0 ? 12 : (i) == 1 ? 15 : (i) == 2 ? 7 : (i) == 3 ? 10 : (i) == 4 ? 8 : (i) == 5 ? 10 : (i) == 6 ? 8 : (i) == 7 ? 11 :  -1)  /**< Pin number for AF_TIMER4_CC2 location number i */
#define AF_TIMER4_CC3_PIN(i)        (-1)                                                                                                                                /**< Pin number for AF_TIMER4_CC3 location number i */
#define AF_TIMER4_CDTI0_PIN(i)      ((i) == 0 ? 0 :  -1)                                                                                                                /**< Pin number for AF_TIMER4_CDTI0 location number i */
#define AF_TIMER4_CDTI1_PIN(i)      ((i) == 0 ? 1 :  -1)                                                                                                                /**< Pin number for AF_TIMER4_CDTI1 location number i */
#define AF_TIMER4_CDTI2_PIN(i)      ((i) == 0 ? 3 :  -1)                                                                                                                /**< Pin number for AF_TIMER4_CDTI2 location number i */
#define AF_TIMER4_CDTI3_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER4_CDTI3 location number i */
#define AF_TIMER5_CC0_PIN(i)        ((i) == 0 ? 4 : (i) == 1 ? 7 : (i) == 2 ? 13 : (i) == 3 ? 0 : (i) == 4 ? 8 : (i) == 5 ? 11 : (i) == 6 ? 14 : (i) == 7 ? 12 :  -1)   /**< Pin number for AF_TIMER5_CC0 location number i */
#define AF_TIMER5_CC1_PIN(i)        ((i) == 0 ? 5 : (i) == 1 ? 11 : (i) == 2 ? 14 : (i) == 3 ? 1 : (i) == 4 ? 9 : (i) == 5 ? 12 : (i) == 6 ? 10 : (i) == 7 ? 13 :  -1)  /**< Pin number for AF_TIMER5_CC1 location number i */
#define AF_TIMER5_CC2_PIN(i)        ((i) == 0 ? 6 : (i) == 1 ? 12 : (i) == 2 ? 15 : (i) == 3 ? 2 : (i) == 4 ? 10 : (i) == 5 ? 13 : (i) == 6 ? 11 : (i) == 7 ? 14 :  -1) /**< Pin number for AF_TIMER5_CC2 location number i */
#define AF_TIMER5_CC3_PIN(i)        (-1)                                                                                                                                /**< Pin number for AF_TIMER5_CC3 location number i */
#define AF_TIMER5_CDTI0_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER5_CDTI0 location number i */
#define AF_TIMER5_CDTI1_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER5_CDTI1 location number i */
#define AF_TIMER5_CDTI2_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER5_CDTI2 location number i */
#define AF_TIMER5_CDTI3_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER5_CDTI3 location number i */
#define AF_TIMER6_CC0_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 6 : (i) == 2 ? 12 : (i) == 3 ? 2 : (i) == 4 ? 8 : (i) == 5 ? 13 : (i) == 6 ? 1 : (i) == 7 ? 4 :  -1)     /**< Pin number for AF_TIMER6_CC0 location number i */
#define AF_TIMER6_CC1_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 7 : (i) == 2 ? 13 : (i) == 3 ? 3 : (i) == 4 ? 9 : (i) == 5 ? 14 : (i) == 6 ? 2 : (i) == 7 ? 5 :  -1)     /**< Pin number for AF_TIMER6_CC1 location number i */
#define AF_TIMER6_CC2_PIN(i)        ((i) == 0 ? 2 : (i) == 1 ? 8 : (i) == 2 ? 14 : (i) == 3 ? 4 : (i) == 4 ? 10 : (i) == 5 ? 0 : (i) == 6 ? 3 : (i) == 7 ? 6 :  -1)     /**< Pin number for AF_TIMER6_CC2 location number i */
#define AF_TIMER6_CC3_PIN(i)        (-1)                                                                                                                                /**< Pin number for AF_TIMER6_CC3 location number i */
#define AF_TIMER6_CDTI0_PIN(i)      ((i) == 0 ? 3 : (i) == 1 ? 9 : (i) == 2 ? 4 : (i) == 3 ? 5 :  -1)                                                                   /**< Pin number for AF_TIMER6_CDTI0 location number i */
#define AF_TIMER6_CDTI1_PIN(i)      ((i) == 0 ? 4 : (i) == 1 ? 10 : (i) == 2 ? 5 : (i) == 3 ? 6 :  -1)                                                                  /**< Pin number for AF_TIMER6_CDTI1 location number i */
#define AF_TIMER6_CDTI2_PIN(i)      ((i) == 0 ? 5 : (i) == 1 ? 11 : (i) == 2 ? 6 : (i) == 3 ? 7 :  -1)                                                                  /**< Pin number for AF_TIMER6_CDTI2 location number i */
#define AF_TIMER6_CDTI3_PIN(i)      (-1)                                                                                                                                /**< Pin number for AF_TIMER6_CDTI3 location number i */
#define AF_WTIMER0_CC0_PIN(i)       ((i) == 0 ? 4 : (i) == 1 ? 6 : (i) == 2 ? 2 : (i) == 3 ? 8 : (i) == 4 ? 15 : (i) == 5 ? 0 : (i) == 6 ? 3 : (i) == 7 ? 1 :  -1)      /**< Pin number for AF_WTIMER0_CC0 location number i */
#define AF_WTIMER0_CC1_PIN(i)       ((i) == 0 ? 5 : (i) == 1 ? 13 : (i) == 2 ? 3 : (i) == 3 ? 9 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 4 : (i) == 7 ? 2 :  -1)      /**< Pin number for AF_WTIMER0_CC1 location number i */
#define AF_WTIMER0_CC2_PIN(i)       ((i) == 0 ? 6 : (i) == 1 ? 14 : (i) == 2 ? 4 : (i) == 3 ? 10 : (i) == 4 ? 1 : (i) == 5 ? 2 : (i) == 6 ? 5 : (i) == 7 ? 3 :  -1)     /**< Pin number for AF_WTIMER0_CC2 location number i */
#define AF_WTIMER0_CC3_PIN(i)       (-1)                                                                                                                                /**< Pin number for AF_WTIMER0_CC3 location number i */
#define AF_WTIMER0_CDTI0_PIN(i)     ((i) == 0 ? 10 : (i) == 1 ? 15 : (i) == 2 ? 12 : (i) == 3 ? 11 : (i) == 4 ? 4 :  -1)                                                /**< Pin number for AF_WTIMER0_CDTI0 location number i */
#define AF_WTIMER0_CDTI1_PIN(i)     ((i) == 0 ? 11 : (i) == 1 ? 0 : (i) == 2 ? 13 : (i) == 3 ? 12 : (i) == 4 ? 5 :  -1)                                                 /**< Pin number for AF_WTIMER0_CDTI1 location number i */
#define AF_WTIMER0_CDTI2_PIN(i)     ((i) == 0 ? 12 : (i) == 1 ? 1 : (i) == 2 ? 14 : (i) == 3 ? 13 : (i) == 4 ? 6 :  -1)                                                 /**< Pin number for AF_WTIMER0_CDTI2 location number i */
#define AF_WTIMER0_CDTI3_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER0_CDTI3 location number i */
#define AF_WTIMER1_CC0_PIN(i)       ((i) == 0 ? 13 : (i) == 1 ? 2 : (i) == 2 ? 6 : (i) == 3 ? 7 : (i) == 4 ? 3 : (i) == 5 ? 7 : (i) == 6 ? 8 : (i) == 7 ? 12 :  -1)     /**< Pin number for AF_WTIMER1_CC0 location number i */
#define AF_WTIMER1_CC1_PIN(i)       ((i) == 0 ? 14 : (i) == 1 ? 3 : (i) == 2 ? 7 : (i) == 3 ? 0 : (i) == 4 ? 4 : (i) == 5 ? 0 : (i) == 6 ? 9 : (i) == 7 ? 13 :  -1)     /**< Pin number for AF_WTIMER1_CC1 location number i */
#define AF_WTIMER1_CC2_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 8 : (i) == 3 ? 1 : (i) == 4 ? 5 : (i) == 5 ? 1 : (i) == 6 ? 10 : (i) == 7 ? 14 :  -1)     /**< Pin number for AF_WTIMER1_CC2 location number i */
#define AF_WTIMER1_CC3_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 5 : (i) == 2 ? 6 : (i) == 3 ? 2 : (i) == 4 ? 6 : (i) == 5 ? 2 : (i) == 6 ? 11 : (i) == 7 ? 15 :  -1)     /**< Pin number for AF_WTIMER1_CC3 location number i */
#define AF_WTIMER1_CDTI0_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER1_CDTI0 location number i */
#define AF_WTIMER1_CDTI1_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER1_CDTI1 location number i */
#define AF_WTIMER1_CDTI2_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER1_CDTI2 location number i */
#define AF_WTIMER1_CDTI3_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER1_CDTI3 location number i */
#define AF_WTIMER2_CC0_PIN(i)       ((i) == 0 ? 9 : (i) == 1 ? 12 : (i) == 2 ? 9 : (i) == 3 ? 12 : (i) == 4 ? 14 : (i) == 5 ? 3 : (i) == 6 ? 4 : (i) == 7 ? 7 :  -1)    /**< Pin number for AF_WTIMER2_CC0 location number i */
#define AF_WTIMER2_CC1_PIN(i)       ((i) == 0 ? 10 : (i) == 1 ? 13 : (i) == 2 ? 10 : (i) == 3 ? 12 : (i) == 4 ? 15 : (i) == 5 ? 4 : (i) == 6 ? 5 : (i) == 7 ? 8 :  -1)  /**< Pin number for AF_WTIMER2_CC1 location number i */
#define AF_WTIMER2_CC2_PIN(i)       ((i) == 0 ? 11 : (i) == 1 ? 14 : (i) == 2 ? 11 : (i) == 3 ? 13 : (i) == 4 ? 0 : (i) == 5 ? 5 : (i) == 6 ? 6 : (i) == 7 ? 9 :  -1)   /**< Pin number for AF_WTIMER2_CC2 location number i */
#define AF_WTIMER2_CC3_PIN(i)       (-1)                                                                                                                                /**< Pin number for AF_WTIMER2_CC3 location number i */
#define AF_WTIMER2_CDTI0_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER2_CDTI0 location number i */
#define AF_WTIMER2_CDTI1_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER2_CDTI1 location number i */
#define AF_WTIMER2_CDTI2_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER2_CDTI2 location number i */
#define AF_WTIMER2_CDTI3_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER2_CDTI3 location number i */
#define AF_WTIMER3_CC0_PIN(i)       ((i) == 0 ? 9 : (i) == 1 ? 8 : (i) == 2 ? 11 : (i) == 3 ? 14 : (i) == 4 ? 3 : (i) == 5 ? 6 : (i) == 6 ? 6 : (i) == 7 ? 13 :  -1)    /**< Pin number for AF_WTIMER3_CC0 location number i */
#define AF_WTIMER3_CC1_PIN(i)       ((i) == 0 ? 10 : (i) == 1 ? 9 : (i) == 2 ? 12 : (i) == 3 ? 10 : (i) == 4 ? 4 : (i) == 5 ? 7 : (i) == 6 ? 4 : (i) == 7 ? 14 :  -1)   /**< Pin number for AF_WTIMER3_CC1 location number i */
#define AF_WTIMER3_CC2_PIN(i)       ((i) == 0 ? 11 : (i) == 1 ? 10 : (i) == 2 ? 13 : (i) == 3 ? 11 : (i) == 4 ? 5 : (i) == 5 ? 6 : (i) == 6 ? 12 : (i) == 7 ? 15 :  -1) /**< Pin number for AF_WTIMER3_CC2 location number i */
#define AF_WTIMER3_CC3_PIN(i)       (-1)                                                                                                                                /**< Pin number for AF_WTIMER3_CC3 location number i */
#define AF_WTIMER3_CDTI0_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER3_CDTI0 location number i */
#define AF_WTIMER3_CDTI1_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER3_CDTI1 location number i */
#define AF_WTIMER3_CDTI2_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER3_CDTI2 location number i */
#define AF_WTIMER3_CDTI3_PIN(i)     (-1)                                                                                                                                /**< Pin number for AF_WTIMER3_CDTI3 location number i */
#define AF_USART0_TX_PIN(i)         ((i) == 0 ? 10 : (i) == 1 ? 7 : (i) == 2 ? 11 : (i) == 3 ? 13 : (i) == 4 ? 7 : (i) == 5 ? 0 : (i) == 6 ? 12 :  -1)                  /**< Pin number for AF_USART0_TX location number i */
#define AF_USART0_RX_PIN(i)         ((i) == 0 ? 11 : (i) == 1 ? 6 : (i) == 2 ? 10 : (i) == 3 ? 12 : (i) == 4 ? 8 : (i) == 5 ? 1 : (i) == 6 ? 13 :  -1)                  /**< Pin number for AF_USART0_RX location number i */
#define AF_USART0_CLK_PIN(i)        ((i) == 0 ? 12 : (i) == 1 ? 5 : (i) == 2 ? 9 : (i) == 3 ? 15 : (i) == 4 ? 13 : (i) == 5 ? 12 : (i) == 6 ? 14 :  -1)                 /**< Pin number for AF_USART0_CLK location number i */
#define AF_USART0_CS_PIN(i)         ((i) == 0 ? 13 : (i) == 1 ? 4 : (i) == 2 ? 8 : (i) == 3 ? 14 : (i) == 4 ? 14 : (i) == 5 ? 13 : (i) == 6 ? 15 :  -1)                 /**< Pin number for AF_USART0_CS location number i */
#define AF_USART0_CTS_PIN(i)        ((i) == 0 ? 14 : (i) == 1 ? 3 : (i) == 2 ? 7 : (i) == 3 ? 13 : (i) == 4 ? 6 : (i) == 5 ? 11 : (i) == 6 ? 0 :  -1)                   /**< Pin number for AF_USART0_CTS location number i */
#define AF_USART0_RTS_PIN(i)        ((i) == 0 ? 15 : (i) == 1 ? 2 : (i) == 2 ? 6 : (i) == 3 ? 12 : (i) == 4 ? 5 : (i) == 5 ? 6 : (i) == 6 ? 1 :  -1)                    /**< Pin number for AF_USART0_RTS location number i */
#define AF_USART1_TX_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 7 : (i) == 3 ? 6 : (i) == 4 ? 1 : (i) == 5 ? 2 : (i) == 6 ? 14 :  -1)                     /**< Pin number for AF_USART1_TX location number i */
#define AF_USART1_RX_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? 1 : (i) == 2 ? 6 : (i) == 3 ? 7 : (i) == 4 ? 2 : (i) == 5 ? 0 : (i) == 6 ? 2 :  -1)                      /**< Pin number for AF_USART1_RX location number i */
#define AF_USART1_CLK_PIN(i)        ((i) == 0 ? 7 : (i) == 1 ? 2 : (i) == 2 ? 0 : (i) == 3 ? 15 : (i) == 4 ? 3 : (i) == 5 ? 11 : (i) == 6 ? 5 :  -1)                    /**< Pin number for AF_USART1_CLK location number i */
#define AF_USART1_CS_PIN(i)         ((i) == 0 ? 8 : (i) == 1 ? 3 : (i) == 2 ? 1 : (i) == 3 ? 14 : (i) == 4 ? 0 : (i) == 5 ? 4 : (i) == 6 ? 2 :  -1)                     /**< Pin number for AF_USART1_CS location number i */
#define AF_USART1_CTS_PIN(i)        ((i) == 0 ? 9 : (i) == 1 ? 4 : (i) == 2 ? 3 : (i) == 3 ? 6 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? 2 :  -1)                    /**< Pin number for AF_USART1_CTS location number i */
#define AF_USART1_RTS_PIN(i)        ((i) == 0 ? 10 : (i) == 1 ? 5 : (i) == 2 ? 4 : (i) == 3 ? 7 : (i) == 4 ? 13 : (i) == 5 ? 14 : (i) == 6 ? 3 :  -1)                   /**< Pin number for AF_USART1_RTS location number i */
#define AF_USART2_TX_PIN(i)         ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 7 : (i) == 3 ? 13 : (i) == 4 ? 6 : (i) == 5 ? 0 :  -1)                                    /**< Pin number for AF_USART2_TX location number i */
#define AF_USART2_RX_PIN(i)         ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 8 : (i) == 3 ? 14 : (i) == 4 ? 7 : (i) == 5 ? 1 :  -1)                                    /**< Pin number for AF_USART2_RX location number i */
#define AF_USART2_CLK_PIN(i)        ((i) == 0 ? 4 : (i) == 1 ? 5 : (i) == 2 ? 9 : (i) == 3 ? 15 : (i) == 4 ? 8 : (i) == 5 ? 2 :  -1)                                    /**< Pin number for AF_USART2_CLK location number i */
#define AF_USART2_CS_PIN(i)         ((i) == 0 ? 5 : (i) == 1 ? 6 : (i) == 2 ? 10 : (i) == 3 ? 11 : (i) == 4 ? 9 : (i) == 5 ? 5 :  -1)                                   /**< Pin number for AF_USART2_CS location number i */
#define AF_USART2_CTS_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 12 : (i) == 2 ? 11 : (i) == 3 ? 10 : (i) == 4 ? 12 : (i) == 5 ? 6 :  -1)                                 /**< Pin number for AF_USART2_CTS location number i */
#define AF_USART2_RTS_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 15 : (i) == 2 ? 12 : (i) == 3 ? 14 : (i) == 4 ? 13 : (i) == 5 ? 8 :  -1)                                 /**< Pin number for AF_USART2_RTS location number i */
#define AF_USART3_TX_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 6 : (i) == 2 ? 3 : (i) == 3 ? 6 : (i) == 4 ? 0 : (i) == 5 ? 12 :  -1)                                    /**< Pin number for AF_USART3_TX location number i */
#define AF_USART3_RX_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? 7 : (i) == 2 ? 7 : (i) == 3 ? 7 : (i) == 4 ? 1 : (i) == 5 ? 13 :  -1)                                    /**< Pin number for AF_USART3_RX location number i */
#define AF_USART3_CLK_PIN(i)        ((i) == 0 ? 2 : (i) == 1 ? 7 : (i) == 2 ? 4 : (i) == 3 ? 8 : (i) == 4 ? 2 : (i) == 5 ? 14 :  -1)                                    /**< Pin number for AF_USART3_CLK location number i */
#define AF_USART3_CS_PIN(i)         ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 14 : (i) == 3 ? 0 : (i) == 4 ? 3 : (i) == 5 ? 15 :  -1)                                   /**< Pin number for AF_USART3_CS location number i */
#define AF_USART3_CTS_PIN(i)        ((i) == 0 ? 4 : (i) == 1 ? 5 : (i) == 2 ? 6 : (i) == 3 ? 10 : (i) == 4 ? 4 : (i) == 5 ? 9 :  -1)                                    /**< Pin number for AF_USART3_CTS location number i */
#define AF_USART3_RTS_PIN(i)        ((i) == 0 ? 5 : (i) == 1 ? 1 : (i) == 2 ? 14 : (i) == 3 ? 15 : (i) == 4 ? 5 : (i) == 5 ? 11 :  -1)                                  /**< Pin number for AF_USART3_RTS location number i */
#define AF_USART4_TX_PIN(i)         ((i) == 0 ? 7 : (i) == 1 ? 9 : (i) == 2 ? 0 : (i) == 3 ? 6 : (i) == 4 ? 4 :  -1)                                                    /**< Pin number for AF_USART4_TX location number i */
#define AF_USART4_RX_PIN(i)         ((i) == 0 ? 8 : (i) == 1 ? 10 : (i) == 2 ? 1 : (i) == 3 ? 7 : (i) == 4 ? 5 :  -1)                                                   /**< Pin number for AF_USART4_RX location number i */
#define AF_USART4_CLK_PIN(i)        ((i) == 0 ? 4 : (i) == 1 ? 11 : (i) == 2 ? 2 : (i) == 3 ? 8 : (i) == 4 ? 6 :  -1)                                                   /**< Pin number for AF_USART4_CLK location number i */
#define AF_USART4_CS_PIN(i)         ((i) == 0 ? 5 : (i) == 1 ? 12 : (i) == 2 ? 3 : (i) == 3 ? 9 : (i) == 4 ? 7 :  -1)                                                   /**< Pin number for AF_USART4_CS location number i */
#define AF_USART4_CTS_PIN(i)        ((i) == 0 ? 7 : (i) == 1 ? 13 : (i) == 2 ? 4 : (i) == 3 ? 10 : (i) == 4 ? 8 :  -1)                                                  /**< Pin number for AF_USART4_CTS location number i */
#define AF_USART4_RTS_PIN(i)        ((i) == 0 ? 8 : (i) == 1 ? 14 : (i) == 2 ? 5 : (i) == 3 ? 11 : (i) == 4 ? 9 :  -1)                                                  /**< Pin number for AF_USART4_RTS location number i */
#define AF_USART5_TX_PIN(i)         ((i) == 0 ? 8 : (i) == 1 ? 6 : (i) == 2 ? 15 : (i) == 3 ? 10 :  -1)                                                                 /**< Pin number for AF_USART5_TX location number i */
#define AF_USART5_RX_PIN(i)         ((i) == 0 ? 9 : (i) == 1 ? 7 : (i) == 2 ? 1 : (i) == 3 ? 11 :  -1)                                                                  /**< Pin number for AF_USART5_RX location number i */
#define AF_USART5_CLK_PIN(i)        ((i) == 0 ? 11 : (i) == 1 ? 13 : (i) == 2 ? 13 : (i) == 3 ? 12 :  -1)                                                               /**< Pin number for AF_USART5_CLK location number i */
#define AF_USART5_CS_PIN(i)         ((i) == 0 ? 13 : (i) == 1 ? 14 : (i) == 2 ? 12 : (i) == 3 ? 13 :  -1)                                                               /**< Pin number for AF_USART5_CS location number i */
#define AF_USART5_CTS_PIN(i)        ((i) == 0 ? 14 : (i) == 1 ? 15 : (i) == 2 ? 11 : (i) == 3 ? 14 :  -1)                                                               /**< Pin number for AF_USART5_CTS location number i */
#define AF_USART5_RTS_PIN(i)        ((i) == 0 ? 12 : (i) == 1 ? 15 : (i) == 2 ? 10 : (i) == 3 ? 15 :  -1)                                                               /**< Pin number for AF_USART5_RTS location number i */
#define AF_UART0_TX_PIN(i)          ((i) == 0 ? 6 : (i) == 1 ? 0 : (i) == 2 ? 3 : (i) == 3 ? 14 : (i) == 4 ? 4 : (i) == 5 ? 1 : (i) == 6 ? 7 :  -1)                     /**< Pin number for AF_UART0_TX location number i */
#define AF_UART0_RX_PIN(i)          ((i) == 0 ? 7 : (i) == 1 ? 1 : (i) == 2 ? 4 : (i) == 3 ? 15 : (i) == 4 ? 5 : (i) == 5 ? 2 : (i) == 6 ? 4 :  -1)                     /**< Pin number for AF_UART0_RX location number i */
#define AF_UART0_CLK_PIN(i)         (-1)                                                                                                                                /**< Pin number for AF_UART0_CLK location number i */
#define AF_UART0_CS_PIN(i)          (-1)                                                                                                                                /**< Pin number for AF_UART0_CS location number i */
#define AF_UART0_CTS_PIN(i)         ((i) == 0 ? 8 : (i) == 1 ? 2 : (i) == 2 ? 5 : (i) == 3 ? 13 : (i) == 4 ? 7 : (i) == 5 ? 5 :  -1)                                    /**< Pin number for AF_UART0_CTS location number i */
#define AF_UART0_RTS_PIN(i)         ((i) == 0 ? 9 : (i) == 1 ? 3 : (i) == 2 ? 6 : (i) == 3 ? 12 : (i) == 4 ? 8 : (i) == 5 ? 6 :  -1)                                    /**< Pin number for AF_UART0_RTS location number i */
#define AF_UART1_TX_PIN(i)          ((i) == 0 ? 12 : (i) == 1 ? 10 : (i) == 2 ? 9 : (i) == 3 ? 2 : (i) == 4 ? 12 : (i) == 5 ? 11 :  -1)                                 /**< Pin number for AF_UART1_TX location number i */
#define AF_UART1_RX_PIN(i)          ((i) == 0 ? 13 : (i) == 1 ? 11 : (i) == 2 ? 10 : (i) == 3 ? 3 : (i) == 4 ? 13 : (i) == 5 ? 12 :  -1)                                /**< Pin number for AF_UART1_RX location number i */
#define AF_UART1_CLK_PIN(i)         (-1)                                                                                                                                /**< Pin number for AF_UART1_CLK location number i */
#define AF_UART1_CS_PIN(i)          (-1)                                                                                                                                /**< Pin number for AF_UART1_CS location number i */
#define AF_UART1_CTS_PIN(i)         ((i) == 0 ? 14 : (i) == 1 ? 9 : (i) == 2 ? 11 : (i) == 3 ? 4 : (i) == 4 ? 4 : (i) == 5 ? 13 :  -1)                                  /**< Pin number for AF_UART1_CTS location number i */
#define AF_UART1_RTS_PIN(i)         ((i) == 0 ? 15 : (i) == 1 ? 8 : (i) == 2 ? 12 : (i) == 3 ? 5 : (i) == 4 ? 5 : (i) == 5 ? 14 :  -1)                                  /**< Pin number for AF_UART1_RTS location number i */
#define AF_QSPI0_SCLK_PIN(i)        ((i) == 0 ? 6 : (i) == 1 ? 14 : (i) == 2 ? 0 :  -1)                                                                                 /**< Pin number for AF_QSPI0_SCLK location number i */
#define AF_QSPI0_DQ0_PIN(i)         ((i) == 0 ? 9 : (i) == 1 ? 2 : (i) == 2 ? 1 :  -1)                                                                                  /**< Pin number for AF_QSPI0_DQ0 location number i */
#define AF_QSPI0_DQ1_PIN(i)         ((i) == 0 ? 10 : (i) == 1 ? 3 : (i) == 2 ? 2 :  -1)                                                                                 /**< Pin number for AF_QSPI0_DQ1 location number i */
#define AF_QSPI0_DQ2_PIN(i)         ((i) == 0 ? 11 : (i) == 1 ? 4 : (i) == 2 ? 3 :  -1)                                                                                 /**< Pin number for AF_QSPI0_DQ2 location number i */
#define AF_QSPI0_DQ3_PIN(i)         ((i) == 0 ? 12 : (i) == 1 ? 5 : (i) == 2 ? 4 :  -1)                                                                                 /**< Pin number for AF_QSPI0_DQ3 location number i */
#define AF_QSPI0_DQ4_PIN(i)         ((i) == 0 ? 8 : (i) == 1 ? 3 : (i) == 2 ? 5 :  -1)                                                                                  /**< Pin number for AF_QSPI0_DQ4 location number i */
#define AF_QSPI0_DQ5_PIN(i)         ((i) == 0 ? 9 : (i) == 1 ? 4 : (i) == 2 ? 6 :  -1)                                                                                  /**< Pin number for AF_QSPI0_DQ5 location number i */
#define AF_QSPI0_DQ6_PIN(i)         ((i) == 0 ? 10 : (i) == 1 ? 5 : (i) == 2 ? 7 :  -1)                                                                                 /**< Pin number for AF_QSPI0_DQ6 location number i */
#define AF_QSPI0_DQ7_PIN(i)         ((i) == 0 ? 11 : (i) == 1 ? 6 : (i) == 2 ? 8 :  -1)                                                                                 /**< Pin number for AF_QSPI0_DQ7 location number i */
#define AF_QSPI0_CS0_PIN(i)         ((i) == 0 ? 7 : (i) == 1 ? 0 : (i) == 2 ? 9 :  -1)                                                                                  /**< Pin number for AF_QSPI0_CS0 location number i */
#define AF_QSPI0_CS1_PIN(i)         ((i) == 0 ? 8 : (i) == 1 ? 1 : (i) == 2 ? 10 :  -1)                                                                                 /**< Pin number for AF_QSPI0_CS1 location number i */
#define AF_QSPI0_DQS_PIN(i)         ((i) == 0 ? 9 : (i) == 1 ? 15 : (i) == 2 ? 11 :  -1)                                                                                /**< Pin number for AF_QSPI0_DQS location number i */
#define AF_LEUART0_TX_PIN(i)        ((i) == 0 ? 4 : (i) == 1 ? 13 : (i) == 2 ? 14 : (i) == 3 ? 0 : (i) == 4 ? 2 : (i) == 5 ? 14 :  -1)                                  /**< Pin number for AF_LEUART0_TX location number i */
#define AF_LEUART0_RX_PIN(i)        ((i) == 0 ? 5 : (i) == 1 ? 14 : (i) == 2 ? 15 : (i) == 3 ? 1 : (i) == 4 ? 0 : (i) == 5 ? 15 :  -1)                                  /**< Pin number for AF_LEUART0_RX location number i */
#define AF_LEUART1_TX_PIN(i)        ((i) == 0 ? 6 : (i) == 1 ? 5 : (i) == 2 ? 2 : (i) == 3 ? 0 : (i) == 4 ? 4 : (i) == 5 ? 0 :  -1)                                     /**< Pin number for AF_LEUART1_TX location number i */
#define AF_LEUART1_RX_PIN(i)        ((i) == 0 ? 7 : (i) == 1 ? 6 : (i) == 2 ? 3 : (i) == 3 ? 1 : (i) == 4 ? 5 : (i) == 5 ? 1 :  -1)                                     /**< Pin number for AF_LEUART1_RX location number i */
#define AF_LETIMER0_OUT0_PIN(i)     ((i) == 0 ? 6 : (i) == 1 ? 11 : (i) == 2 ? 0 : (i) == 3 ? 4 : (i) == 4 ? 12 : (i) == 5 ? 14 : (i) == 6 ? 8 : (i) == 7 ? 9 :  -1)    /**< Pin number for AF_LETIMER0_OUT0 location number i */
#define AF_LETIMER0_OUT1_PIN(i)     ((i) == 0 ? 7 : (i) == 1 ? 12 : (i) == 2 ? 1 : (i) == 3 ? 5 : (i) == 4 ? 13 : (i) == 5 ? 15 : (i) == 6 ? 9 : (i) == 7 ? 10 :  -1)   /**< Pin number for AF_LETIMER0_OUT1 location number i */
#define AF_LETIMER1_OUT0_PIN(i)     ((i) == 0 ? 7 : (i) == 1 ? 11 : (i) == 2 ? 12 : (i) == 3 ? 2 : (i) == 4 ? 5 : (i) == 5 ? 2 : (i) == 6 ? 0 : (i) == 7 ? 2 :  -1)     /**< Pin number for AF_LETIMER1_OUT0 location number i */
#define AF_LETIMER1_OUT1_PIN(i)     ((i) == 0 ? 6 : (i) == 1 ? 13 : (i) == 2 ? 14 : (i) == 3 ? 3 : (i) == 4 ? 6 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 3 :  -1)     /**< Pin number for AF_LETIMER1_OUT1 location number i */
#define AF_PCNT0_S0IN_PIN(i)        ((i) == 0 ? 13 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 6 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 5 : (i) == 7 ? 12 :  -1)     /**< Pin number for AF_PCNT0_S0IN location number i */
#define AF_PCNT0_S1IN_PIN(i)        ((i) == 0 ? 14 : (i) == 1 ? 1 : (i) == 2 ? 1 : (i) == 3 ? 7 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 6 : (i) == 7 ? 11 :  -1)     /**< Pin number for AF_PCNT0_S1IN location number i */
#define AF_PCNT1_S0IN_PIN(i)        ((i) == 0 ? 5 : (i) == 1 ? 3 : (i) == 2 ? 15 : (i) == 3 ? 4 : (i) == 4 ? 7 : (i) == 5 ? 12 : (i) == 6 ? 11 : (i) == 7 ? 14 :  -1)   /**< Pin number for AF_PCNT1_S0IN location number i */
#define AF_PCNT1_S1IN_PIN(i)        ((i) == 0 ? 6 : (i) == 1 ? 4 : (i) == 2 ? 0 : (i) == 3 ? 5 : (i) == 4 ? 8 : (i) == 5 ? 13 : (i) == 6 ? 12 : (i) == 7 ? 15 :  -1)    /**< Pin number for AF_PCNT1_S1IN location number i */
#define AF_PCNT2_S0IN_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 8 : (i) == 2 ? 13 : (i) == 3 ? 10 : (i) == 4 ? 12 : (i) == 5 ? 2 : (i) == 6 ? 0 : (i) == 7 ? 14 :  -1)   /**< Pin number for AF_PCNT2_S0IN location number i */
#define AF_PCNT2_S1IN_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 9 : (i) == 2 ? 14 : (i) == 3 ? 11 : (i) == 4 ? 13 : (i) == 5 ? 1 : (i) == 6 ? 15 : (i) == 7 ? 13 :  -1)  /**< Pin number for AF_PCNT2_S1IN location number i */
#define AF_I2C0_SDA_PIN(i)          ((i) == 0 ? 0 : (i) == 1 ? 6 : (i) == 2 ? 6 : (i) == 3 ? 14 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 12 : (i) == 7 ? 4 :  -1)     /**< Pin number for AF_I2C0_SDA location number i */
#define AF_I2C0_SCL_PIN(i)          ((i) == 0 ? 1 : (i) == 1 ? 7 : (i) == 2 ? 7 : (i) == 3 ? 15 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 13 : (i) == 7 ? 5 :  -1)     /**< Pin number for AF_I2C0_SCL location number i */
#define AF_I2C1_SDA_PIN(i)          ((i) == 0 ? 4 : (i) == 1 ? 11 : (i) == 2 ? 0 : (i) == 3 ? 4 : (i) == 4 ? 11 : (i) == 5 ? 11 : (i) == 6 ? 13 : (i) == 7 ? 2 :  -1)   /**< Pin number for AF_I2C1_SDA location number i */
#define AF_I2C1_SCL_PIN(i)          ((i) == 0 ? 5 : (i) == 1 ? 12 : (i) == 2 ? 1 : (i) == 3 ? 5 : (i) == 4 ? 2 : (i) == 5 ? 12 : (i) == 6 ? 14 : (i) == 7 ? 3 :  -1)    /**< Pin number for AF_I2C1_SCL location number i */
#define AF_I2C2_SDA_PIN(i)          ((i) == 0 ? 8 : (i) == 1 ? 14 : (i) == 2 ? 10 : (i) == 3 ? 4 : (i) == 4 ? 13 : (i) == 5 ? 15 : (i) == 6 ? 12 : (i) == 7 ? 4 :  -1)  /**< Pin number for AF_I2C2_SDA location number i */
#define AF_I2C2_SCL_PIN(i)          ((i) == 0 ? 5 : (i) == 1 ? 15 : (i) == 2 ? 11 : (i) == 3 ? 12 : (i) == 4 ? 14 : (i) == 5 ? 3 : (i) == 6 ? 13 : (i) == 7 ? 5 :  -1)  /**< Pin number for AF_I2C2_SCL location number i */
#define AF_ACMP0_OUT_PIN(i)         ((i) == 0 ? 13 : (i) == 1 ? 2 : (i) == 2 ? 6 : (i) == 3 ? 11 : (i) == 4 ? 6 : (i) == 5 ? 0 : (i) == 6 ? 2 : (i) == 7 ? 3 :  -1)     /**< Pin number for AF_ACMP0_OUT location number i */
#define AF_ACMP1_OUT_PIN(i)         ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 7 : (i) == 3 ? 12 : (i) == 4 ? 14 : (i) == 5 ? 9 : (i) == 6 ? 10 : (i) == 7 ? 5 :  -1)    /**< Pin number for AF_ACMP1_OUT location number i */
#define AF_ACMP2_OUT_PIN(i)         ((i) == 0 ? 8 : (i) == 1 ? 0 : (i) == 2 ? 1 : (i) == 3 ? 0 : (i) == 4 ? 1 : (i) == 5 ? 2 :  -1)                                     /**< Pin number for AF_ACMP2_OUT location number i */
#define AF_ACMP3_OUT_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 15 : (i) == 2 ? 14 : (i) == 3 ? 13 : (i) == 4 ? 4 : (i) == 5 ? 5 :  -1)                                  /**< Pin number for AF_ACMP3_OUT location number i */
#define AF_USB_VBUSEN_PIN(i)        ((i) == 0 ? 5 :  -1)                                                                                                                /**< Pin number for AF_USB_VBUSEN location number i */
#define AF_DBG_TDI_PIN(i)           ((i) == 0 ? 5 :  -1)                                                                                                                /**< Pin number for AF_DBG_TDI location number i */
#define AF_DBG_TDO_PIN(i)           ((i) == 0 ? 2 :  -1)                                                                                                                /**< Pin number for AF_DBG_TDO location number i */
#define AF_DBG_SWV_PIN(i)           ((i) == 0 ? 2 : (i) == 1 ? 15 : (i) == 2 ? 1 : (i) == 3 ? 2 :  -1)                                                                  /**< Pin number for AF_DBG_SWV location number i */
#define AF_DBG_SWDIOTMS_PIN(i)      ((i) == 0 ? 1 :  -1)                                                                                                                /**< Pin number for AF_DBG_SWDIOTMS location number i */
#define AF_DBG_SWCLKTCK_PIN(i)      ((i) == 0 ? 0 :  -1)                                                                                                                /**< Pin number for AF_DBG_SWCLKTCK location number i */
#define AF_ETM_TCLK_PIN(i)          ((i) == 0 ? 7 : (i) == 1 ? 8 : (i) == 2 ? 6 : (i) == 3 ? 6 : (i) == 4 ? 11 : (i) == 5 ? 15 :  -1)                                   /**< Pin number for AF_ETM_TCLK location number i */
#define AF_ETM_TD0_PIN(i)           ((i) == 0 ? 6 : (i) == 1 ? 9 : (i) == 2 ? 7 : (i) == 3 ? 2 : (i) == 4 ? 12 : (i) == 5 ? 14 :  -1)                                   /**< Pin number for AF_ETM_TD0 location number i */
#define AF_ETM_TD1_PIN(i)           ((i) == 0 ? 3 : (i) == 1 ? 13 : (i) == 2 ? 3 : (i) == 3 ? 3 : (i) == 4 ? 13 : (i) == 5 ? 13 :  -1)                                  /**< Pin number for AF_ETM_TD1 location number i */
#define AF_ETM_TD2_PIN(i)           ((i) == 0 ? 4 : (i) == 1 ? 15 : (i) == 2 ? 4 : (i) == 3 ? 4 : (i) == 4 ? 14 : (i) == 5 ? 12 :  -1)                                  /**< Pin number for AF_ETM_TD2 location number i */
#define AF_ETM_TD3_PIN(i)           ((i) == 0 ? 5 : (i) == 1 ? 3 : (i) == 2 ? 5 : (i) == 3 ? 5 : (i) == 4 ? 15 : (i) == 5 ? 11 :  -1)                                   /**< Pin number for AF_ETM_TD3 location number i */

/** @} */
/** @} End of group EFM32GG11B_AF_Pins */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

