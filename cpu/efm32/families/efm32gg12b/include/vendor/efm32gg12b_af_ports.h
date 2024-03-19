/***************************************************************************//**
 * @file
 * @brief EFM32GG12B_AF_PORTS register and bit field definitions
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
 * @addtogroup EFM32GG12B_Alternate_Function Alternate Function
 * @{
 * @defgroup EFM32GG12B_AF_Ports Alternate Function Ports
 * @{
 ******************************************************************************/

#define AF_CMU_CLK0_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? -1 : (i) == 4 ? 5 : (i) == 5 ? 0 :  -1)                                /**< Port number for AF_CMU_CLK0 location number i */
#define AF_CMU_CLK1_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 4 : (i) == 3 ? -1 : (i) == 4 ? 5 : (i) == 5 ? 1 :  -1)                                /**< Port number for AF_CMU_CLK1 location number i */
#define AF_CMU_CLK2_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 3 : (i) == 3 ? -1 : (i) == 4 ? 0 : (i) == 5 ? 3 :  -1)                                /**< Port number for AF_CMU_CLK2 location number i */
#define AF_CMU_CLKI0_PORT(i)         ((i) == 0 ? 3 : (i) == 1 ? 0 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 4 : (i) == 5 ? 3 : (i) == 6 ? 4 : (i) == 7 ? 1 :  -1)   /**< Port number for AF_CMU_CLKI0 location number i */
#define AF_CMU_DIGEXTCLK_PORT(i)     ((i) == 0 ? 1 :  -1)                                                                                                            /**< Port number for AF_CMU_DIGEXTCLK location number i */
#define AF_CMU_IOPOVR_PORT(i)        ((i) == 0 ? 1 :  -1)                                                                                                            /**< Port number for AF_CMU_IOPOVR location number i */
#define AF_CMU_IONOVR_PORT(i)        ((i) == 0 ? 1 :  -1)                                                                                                            /**< Port number for AF_CMU_IONOVR location number i */
#define AF_LESENSE_CH0_PORT(i)       ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH0 location number i */
#define AF_LESENSE_CH1_PORT(i)       ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH1 location number i */
#define AF_LESENSE_CH2_PORT(i)       ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH2 location number i */
#define AF_LESENSE_CH3_PORT(i)       ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH3 location number i */
#define AF_LESENSE_CH4_PORT(i)       ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH4 location number i */
#define AF_LESENSE_CH5_PORT(i)       ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH5 location number i */
#define AF_LESENSE_CH6_PORT(i)       ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH6 location number i */
#define AF_LESENSE_CH7_PORT(i)       ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH7 location number i */
#define AF_LESENSE_CH8_PORT(i)       ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH8 location number i */
#define AF_LESENSE_CH9_PORT(i)       ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH9 location number i */
#define AF_LESENSE_CH10_PORT(i)      ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH10 location number i */
#define AF_LESENSE_CH11_PORT(i)      ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH11 location number i */
#define AF_LESENSE_CH12_PORT(i)      ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH12 location number i */
#define AF_LESENSE_CH13_PORT(i)      ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH13 location number i */
#define AF_LESENSE_CH14_PORT(i)      ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH14 location number i */
#define AF_LESENSE_CH15_PORT(i)      ((i) == 0 ? 2 :  -1)                                                                                                            /**< Port number for AF_LESENSE_CH15 location number i */
#define AF_LESENSE_ALTEX0_PORT(i)    ((i) == 0 ? 3 :  -1)                                                                                                            /**< Port number for AF_LESENSE_ALTEX0 location number i */
#define AF_LESENSE_ALTEX1_PORT(i)    ((i) == 0 ? 3 :  -1)                                                                                                            /**< Port number for AF_LESENSE_ALTEX1 location number i */
#define AF_LESENSE_ALTEX2_PORT(i)    ((i) == 0 ? 0 :  -1)                                                                                                            /**< Port number for AF_LESENSE_ALTEX2 location number i */
#define AF_LESENSE_ALTEX3_PORT(i)    ((i) == 0 ? 0 :  -1)                                                                                                            /**< Port number for AF_LESENSE_ALTEX3 location number i */
#define AF_LESENSE_ALTEX4_PORT(i)    ((i) == 0 ? 0 :  -1)                                                                                                            /**< Port number for AF_LESENSE_ALTEX4 location number i */
#define AF_LESENSE_ALTEX5_PORT(i)    ((i) == 0 ? 4 :  -1)                                                                                                            /**< Port number for AF_LESENSE_ALTEX5 location number i */
#define AF_LESENSE_ALTEX6_PORT(i)    ((i) == 0 ? 4 :  -1)                                                                                                            /**< Port number for AF_LESENSE_ALTEX6 location number i */
#define AF_LESENSE_ALTEX7_PORT(i)    ((i) == 0 ? 4 :  -1)                                                                                                            /**< Port number for AF_LESENSE_ALTEX7 location number i */
#define AF_EBI_AD00_PORT(i)          ((i) == 0 ? 4 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_EBI_AD00 location number i */
#define AF_EBI_AD01_PORT(i)          ((i) == 0 ? 4 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_EBI_AD01 location number i */
#define AF_EBI_AD02_PORT(i)          ((i) == 0 ? 4 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_EBI_AD02 location number i */
#define AF_EBI_AD03_PORT(i)          ((i) == 0 ? 4 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_EBI_AD03 location number i */
#define AF_EBI_AD04_PORT(i)          ((i) == 0 ? 4 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_EBI_AD04 location number i */
#define AF_EBI_AD05_PORT(i)          ((i) == 0 ? 4 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_EBI_AD05 location number i */
#define AF_EBI_AD06_PORT(i)          ((i) == 0 ? 4 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_EBI_AD06 location number i */
#define AF_EBI_AD07_PORT(i)          ((i) == 0 ? 4 : (i) == 1 ? 2 :  -1)                                                                                             /**< Port number for AF_EBI_AD07 location number i */
#define AF_EBI_AD08_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 2 :  -1)                                                                                             /**< Port number for AF_EBI_AD08 location number i */
#define AF_EBI_AD09_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 2 :  -1)                                                                                             /**< Port number for AF_EBI_AD09 location number i */
#define AF_EBI_AD10_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 2 :  -1)                                                                                             /**< Port number for AF_EBI_AD10 location number i */
#define AF_EBI_AD11_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 2 :  -1)                                                                                             /**< Port number for AF_EBI_AD11 location number i */
#define AF_EBI_AD12_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 2 :  -1)                                                                                             /**< Port number for AF_EBI_AD12 location number i */
#define AF_EBI_AD13_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_EBI_AD13 location number i */
#define AF_EBI_AD14_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_EBI_AD14 location number i */
#define AF_EBI_AD15_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_EBI_AD15 location number i */
#define AF_EBI_CS0_PORT(i)           ((i) == 0 ? 3 : (i) == 1 ? 0 : (i) == 2 ? 2 : (i) == 3 ? 1 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_EBI_CS0 location number i */
#define AF_EBI_CS1_PORT(i)           ((i) == 0 ? 3 : (i) == 1 ? 0 : (i) == 2 ? 2 : (i) == 3 ? 1 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_EBI_CS1 location number i */
#define AF_EBI_CS2_PORT(i)           ((i) == 0 ? 3 : (i) == 1 ? 0 : (i) == 2 ? 2 : (i) == 3 ? 1 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_EBI_CS2 location number i */
#define AF_EBI_CS3_PORT(i)           ((i) == 0 ? 3 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 1 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_EBI_CS3 location number i */
#define AF_EBI_ARDY_PORT(i)          ((i) == 0 ? 5 : (i) == 1 ? 3 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 2 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_EBI_ARDY location number i */
#define AF_EBI_ALE_PORT(i)           ((i) == 0 ? 5 : (i) == 1 ? 1 : (i) == 2 ? 2 : (i) == 3 ? 1 : (i) == 4 ? 2 : (i) == 5 ? 2 :  -1)                                 /**< Port number for AF_EBI_ALE location number i */
#define AF_EBI_WEn_PORT(i)           ((i) == 0 ? 5 : (i) == 1 ? 0 : (i) == 2 ? 2 : (i) == 3 ? 1 : (i) == 4 ? 5 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_EBI_WEn location number i */
#define AF_EBI_REn_PORT(i)           ((i) == 0 ? 5 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 2 : (i) == 4 ? 5 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_EBI_REn location number i */
#define AF_EBI_BL0_PORT(i)           ((i) == 0 ? 5 : (i) == 1 ? 5 : (i) == 2 ? 1 : (i) == 3 ? 2 : (i) == 4 ? 5 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_EBI_BL0 location number i */
#define AF_EBI_BL1_PORT(i)           ((i) == 0 ? 5 : (i) == 1 ? 5 : (i) == 2 ? 1 : (i) == 3 ? 2 : (i) == 4 ? 5 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_EBI_BL1 location number i */
#define AF_EBI_NANDWEn_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 0 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_EBI_NANDWEn location number i */
#define AF_EBI_NANDREn_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 1 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_EBI_NANDREn location number i */
#define AF_EBI_A00_PORT(i)           ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? -1 : (i) == 3 ? 2 :  -1)                                                              /**< Port number for AF_EBI_A00 location number i */
#define AF_EBI_A01_PORT(i)           ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_A01 location number i */
#define AF_EBI_A02_PORT(i)           ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_A02 location number i */
#define AF_EBI_A03_PORT(i)           ((i) == 0 ? 1 : (i) == 1 ? 1 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_A03 location number i */
#define AF_EBI_A04_PORT(i)           ((i) == 0 ? 1 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_A04 location number i */
#define AF_EBI_A05_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_A05 location number i */
#define AF_EBI_A06_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_A06 location number i */
#define AF_EBI_A07_PORT(i)           ((i) == 0 ? 4 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_A07 location number i */
#define AF_EBI_A08_PORT(i)           ((i) == 0 ? 4 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_A08 location number i */
#define AF_EBI_A09_PORT(i)           ((i) == 0 ? 4 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 1 :  -1)                                                              /**< Port number for AF_EBI_A09 location number i */
#define AF_EBI_A10_PORT(i)           ((i) == 0 ? 4 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 1 :  -1)                                                              /**< Port number for AF_EBI_A10 location number i */
#define AF_EBI_A11_PORT(i)           ((i) == 0 ? 4 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 1 :  -1)                                                              /**< Port number for AF_EBI_A11 location number i */
#define AF_EBI_A12_PORT(i)           ((i) == 0 ? 4 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 1 :  -1)                                                              /**< Port number for AF_EBI_A12 location number i */
#define AF_EBI_A13_PORT(i)           ((i) == 0 ? 4 : (i) == 1 ? 2 : (i) == 2 ? -1 : (i) == 3 ? 3 :  -1)                                                              /**< Port number for AF_EBI_A13 location number i */
#define AF_EBI_A14_PORT(i)           ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? -1 : (i) == 3 ? 3 :  -1)                                                              /**< Port number for AF_EBI_A14 location number i */
#define AF_EBI_A15_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? -1 : (i) == 3 ? 3 :  -1)                                                              /**< Port number for AF_EBI_A15 location number i */
#define AF_EBI_A16_PORT(i)           ((i) == 0 ? 1 : (i) == 1 ? 4 : (i) == 2 ? -1 : (i) == 3 ? 3 :  -1)                                                              /**< Port number for AF_EBI_A16 location number i */
#define AF_EBI_A17_PORT(i)           ((i) == 0 ? 1 : (i) == 1 ? 4 : (i) == 2 ? -1 : (i) == 3 ? 3 :  -1)                                                              /**< Port number for AF_EBI_A17 location number i */
#define AF_EBI_A18_PORT(i)           ((i) == 0 ? 1 : (i) == 1 ? 4 : (i) == 2 ? -1 : (i) == 3 ? 3 :  -1)                                                              /**< Port number for AF_EBI_A18 location number i */
#define AF_EBI_A19_PORT(i)           ((i) == 0 ? 1 : (i) == 1 ? 4 : (i) == 2 ? -1 : (i) == 3 ? 3 :  -1)                                                              /**< Port number for AF_EBI_A19 location number i */
#define AF_EBI_A20_PORT(i)           ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? -1 : (i) == 3 ? 3 :  -1)                                                              /**< Port number for AF_EBI_A20 location number i */
#define AF_EBI_A21_PORT(i)           ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? -1 : (i) == 3 ? 2 :  -1)                                                              /**< Port number for AF_EBI_A21 location number i */
#define AF_EBI_A22_PORT(i)           ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? -1 : (i) == 3 ? 4 :  -1)                                                              /**< Port number for AF_EBI_A22 location number i */
#define AF_EBI_A23_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 2 : (i) == 2 ? -1 : (i) == 3 ? 4 :  -1)                                                              /**< Port number for AF_EBI_A23 location number i */
#define AF_EBI_A24_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? -1 : (i) == 3 ? 4 :  -1)                                                              /**< Port number for AF_EBI_A24 location number i */
#define AF_EBI_A25_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? -1 : (i) == 3 ? 4 :  -1)                                                              /**< Port number for AF_EBI_A25 location number i */
#define AF_EBI_A26_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? -1 : (i) == 3 ? 2 :  -1)                                                              /**< Port number for AF_EBI_A26 location number i */
#define AF_EBI_A27_PORT(i)           ((i) == 0 ? 3 : (i) == 1 ? 5 : (i) == 2 ? -1 : (i) == 3 ? 2 :  -1)                                                              /**< Port number for AF_EBI_A27 location number i */
#define AF_EBI_CSTFT_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 5 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_CSTFT location number i */
#define AF_EBI_DCLK_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 5 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_DCLK location number i */
#define AF_EBI_DTEN_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_DTEN location number i */
#define AF_EBI_VSNC_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_VSNC location number i */
#define AF_EBI_HSNC_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 0 :  -1)                                                              /**< Port number for AF_EBI_HSNC location number i */
#define AF_SDIO_CLK_PORT(i)          ((i) == 0 ? 4 : (i) == 1 ? 4 :  -1)                                                                                             /**< Port number for AF_SDIO_CLK location number i */
#define AF_SDIO_CMD_PORT(i)          ((i) == 0 ? 4 : (i) == 1 ? 4 :  -1)                                                                                             /**< Port number for AF_SDIO_CMD location number i */
#define AF_SDIO_DAT0_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_SDIO_DAT0 location number i */
#define AF_SDIO_DAT1_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_SDIO_DAT1 location number i */
#define AF_SDIO_DAT2_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_SDIO_DAT2 location number i */
#define AF_SDIO_DAT3_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_SDIO_DAT3 location number i */
#define AF_SDIO_DAT4_PORT(i)         ((i) == 0 ? 3 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_SDIO_DAT4 location number i */
#define AF_SDIO_DAT5_PORT(i)         ((i) == 0 ? 3 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_SDIO_DAT5 location number i */
#define AF_SDIO_DAT6_PORT(i)         ((i) == 0 ? 3 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_SDIO_DAT6 location number i */
#define AF_SDIO_DAT7_PORT(i)         ((i) == 0 ? 3 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_SDIO_DAT7 location number i */
#define AF_SDIO_CD_PORT(i)           ((i) == 0 ? 5 : (i) == 1 ? 2 : (i) == 2 ? 0 : (i) == 3 ? 1 :  -1)                                                               /**< Port number for AF_SDIO_CD location number i */
#define AF_SDIO_WP_PORT(i)           ((i) == 0 ? 5 : (i) == 1 ? 2 : (i) == 2 ? 1 : (i) == 3 ? 1 :  -1)                                                               /**< Port number for AF_SDIO_WP location number i */
#define AF_PDM_CLK_PORT(i)           ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 1 : (i) == 4 ? 3 :  -1)                                                /**< Port number for AF_PDM_CLK location number i */
#define AF_PDM_DAT0_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 1 : (i) == 4 ? 3 :  -1)                                                /**< Port number for AF_PDM_DAT0 location number i */
#define AF_PDM_DAT1_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 1 : (i) == 4 ? 3 :  -1)                                                /**< Port number for AF_PDM_DAT1 location number i */
#define AF_PDM_DAT2_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 5 : (i) == 3 ? 1 : (i) == 4 ? 3 :  -1)                                                /**< Port number for AF_PDM_DAT2 location number i */
#define AF_PDM_DAT3_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 3 : (i) == 3 ? 0 : (i) == 4 ? 3 :  -1)                                                /**< Port number for AF_PDM_DAT3 location number i */
#define AF_PRS_CH0_PORT(i)           ((i) == 0 ? 0 : (i) == 1 ? 5 : (i) == 2 ? 2 : (i) == 3 ? 5 :  -1)                                                               /**< Port number for AF_PRS_CH0 location number i */
#define AF_PRS_CH1_PORT(i)           ((i) == 0 ? 0 : (i) == 1 ? 5 : (i) == 2 ? 2 : (i) == 3 ? 4 :  -1)                                                               /**< Port number for AF_PRS_CH1 location number i */
#define AF_PRS_CH2_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? 4 : (i) == 3 ? 4 :  -1)                                                               /**< Port number for AF_PRS_CH2 location number i */
#define AF_PRS_CH3_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 4 : (i) == 3 ? 0 :  -1)                                                               /**< Port number for AF_PRS_CH3 location number i */
#define AF_PRS_CH4_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 5 :  -1)                                                                              /**< Port number for AF_PRS_CH4 location number i */
#define AF_PRS_CH5_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 3 :  -1)                                                                              /**< Port number for AF_PRS_CH5 location number i */
#define AF_PRS_CH6_PORT(i)           ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 4 :  -1)                                                                              /**< Port number for AF_PRS_CH6 location number i */
#define AF_PRS_CH7_PORT(i)           ((i) == 0 ? 1 : (i) == 1 ? 0 : (i) == 2 ? 4 :  -1)                                                                              /**< Port number for AF_PRS_CH7 location number i */
#define AF_PRS_CH8_PORT(i)           ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 4 :  -1)                                                                              /**< Port number for AF_PRS_CH8 location number i */
#define AF_PRS_CH9_PORT(i)           ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 1 :  -1)                                                                              /**< Port number for AF_PRS_CH9 location number i */
#define AF_PRS_CH10_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 3 :  -1)                                                                              /**< Port number for AF_PRS_CH10 location number i */
#define AF_PRS_CH11_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 3 :  -1)                                                                              /**< Port number for AF_PRS_CH11 location number i */
#define AF_PRS_CH12_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 3 :  -1)                                                                              /**< Port number for AF_PRS_CH12 location number i */
#define AF_PRS_CH13_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 4 :  -1)                                                                              /**< Port number for AF_PRS_CH13 location number i */
#define AF_PRS_CH14_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 4 :  -1)                                                                              /**< Port number for AF_PRS_CH14 location number i */
#define AF_PRS_CH15_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 5 :  -1)                                                                              /**< Port number for AF_PRS_CH15 location number i */
#define AF_CAN0_RX_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? 3 : (i) == 3 ? 1 : (i) == 4 ? -1 : (i) == 5 ? 3 : (i) == 6 ? 4 :  -1)                 /**< Port number for AF_CAN0_RX location number i */
#define AF_CAN0_TX_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? 3 : (i) == 3 ? 1 : (i) == 4 ? -1 : (i) == 5 ? 3 : (i) == 6 ? 4 :  -1)                 /**< Port number for AF_CAN0_TX location number i */
#define AF_CAN1_RX_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? 3 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 0 :  -1)                                 /**< Port number for AF_CAN1_RX location number i */
#define AF_CAN1_TX_PORT(i)           ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? 3 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 0 :  -1)                                 /**< Port number for AF_CAN1_TX location number i */
#define AF_TIMER0_CC0_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 5 : (i) == 2 ? 3 : (i) == 3 ? 1 : (i) == 4 ? 5 : (i) == 5 ? 2 : (i) == 6 ? 0 : (i) == 7 ? 0 :  -1)   /**< Port number for AF_TIMER0_CC0 location number i */
#define AF_TIMER0_CC1_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 5 : (i) == 2 ? 3 : (i) == 3 ? 2 : (i) == 4 ? 5 : (i) == 5 ? 2 : (i) == 6 ? 0 : (i) == 7 ? 0 :  -1)   /**< Port number for AF_TIMER0_CC1 location number i */
#define AF_TIMER0_CC2_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 5 : (i) == 2 ? 3 : (i) == 3 ? 2 : (i) == 4 ? 5 : (i) == 5 ? 0 : (i) == 6 ? 0 : (i) == 7 ? 0 :  -1)   /**< Port number for AF_TIMER0_CC2 location number i */
#define AF_TIMER0_CC3_PORT(i)        (-1)                                                                                                                            /**< Port number for AF_TIMER0_CC3 location number i */
#define AF_TIMER0_CDTI0_PORT(i)      ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 5 : (i) == 3 ? 2 : (i) == 4 ? 1 :  -1)                                                /**< Port number for AF_TIMER0_CDTI0 location number i */
#define AF_TIMER0_CDTI1_PORT(i)      ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 5 : (i) == 3 ? 2 : (i) == 4 ? 1 :  -1)                                                /**< Port number for AF_TIMER0_CDTI1 location number i */
#define AF_TIMER0_CDTI2_PORT(i)      ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 5 : (i) == 3 ? 2 : (i) == 4 ? 1 :  -1)                                                /**< Port number for AF_TIMER0_CDTI2 location number i */
#define AF_TIMER0_CDTI3_PORT(i)      (-1)                                                                                                                            /**< Port number for AF_TIMER0_CDTI3 location number i */
#define AF_TIMER1_CC0_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 3 : (i) == 5 ? 5 : (i) == 6 ? 5 :  -1)                  /**< Port number for AF_TIMER1_CC0 location number i */
#define AF_TIMER1_CC1_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 3 : (i) == 5 ? 5 : (i) == 6 ? 5 :  -1)                  /**< Port number for AF_TIMER1_CC1 location number i */
#define AF_TIMER1_CC2_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 2 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_TIMER1_CC2 location number i */
#define AF_TIMER1_CC3_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 1 : (i) == 3 ? 1 : (i) == 4 ? 2 : (i) == 5 ? 5 : (i) == 6 ? 5 :  -1)                  /**< Port number for AF_TIMER1_CC3 location number i */
#define AF_TIMER1_CDTI0_PORT(i)      (-1)                                                                                                                            /**< Port number for AF_TIMER1_CDTI0 location number i */
#define AF_TIMER1_CDTI1_PORT(i)      (-1)                                                                                                                            /**< Port number for AF_TIMER1_CDTI1 location number i */
#define AF_TIMER1_CDTI2_PORT(i)      (-1)                                                                                                                            /**< Port number for AF_TIMER1_CDTI2 location number i */
#define AF_TIMER1_CDTI3_PORT(i)      (-1)                                                                                                                            /**< Port number for AF_TIMER1_CDTI3 location number i */
#define AF_TIMER2_CC0_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 2 : (i) == 3 ? 5 : (i) == 4 ? 1 : (i) == 5 ? 2 :  -1)                                 /**< Port number for AF_TIMER2_CC0 location number i */
#define AF_TIMER2_CC1_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 2 : (i) == 3 ? 4 : (i) == 4 ? 2 : (i) == 5 ? 2 :  -1)                                 /**< Port number for AF_TIMER2_CC1 location number i */
#define AF_TIMER2_CC2_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 2 : (i) == 3 ? 4 : (i) == 4 ? 2 : (i) == 5 ? 2 :  -1)                                 /**< Port number for AF_TIMER2_CC2 location number i */
#define AF_TIMER2_CC3_PORT(i)        (-1)                                                                                                                            /**< Port number for AF_TIMER2_CC3 location number i */
#define AF_TIMER2_CDTI0_PORT(i)      ((i) == 0 ? 1 : (i) == 1 ? 3 : (i) == 2 ? 4 :  -1)                                                                              /**< Port number for AF_TIMER2_CDTI0 location number i */
#define AF_TIMER2_CDTI1_PORT(i)      ((i) == 0 ? 1 : (i) == 1 ? 3 : (i) == 2 ? 4 :  -1)                                                                              /**< Port number for AF_TIMER2_CDTI1 location number i */
#define AF_TIMER2_CDTI2_PORT(i)      ((i) == 0 ? 1 : (i) == 1 ? 3 : (i) == 2 ? 4 :  -1)                                                                              /**< Port number for AF_TIMER2_CDTI2 location number i */
#define AF_TIMER2_CDTI3_PORT(i)      (-1)                                                                                                                            /**< Port number for AF_TIMER2_CDTI3 location number i */
#define AF_TIMER3_CC0_PORT(i)        ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 4 : (i) == 3 ? 4 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 0 : (i) == 7 ? 3 :  -1)   /**< Port number for AF_TIMER3_CC0 location number i */
#define AF_TIMER3_CC1_PORT(i)        ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 4 : (i) == 3 ? 4 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 3 : (i) == 7 ? 1 :  -1)   /**< Port number for AF_TIMER3_CC1 location number i */
#define AF_TIMER3_CC2_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 4 : (i) == 3 ? 4 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 3 : (i) == 7 ? 1 :  -1)   /**< Port number for AF_TIMER3_CC2 location number i */
#define AF_TIMER3_CC3_PORT(i)        (-1)                                                                                                                            /**< Port number for AF_TIMER3_CC3 location number i */
#define AF_TIMER3_CDTI0_PORT(i)      (-1)                                                                                                                            /**< Port number for AF_TIMER3_CDTI0 location number i */
#define AF_TIMER3_CDTI1_PORT(i)      (-1)                                                                                                                            /**< Port number for AF_TIMER3_CDTI1 location number i */
#define AF_TIMER3_CDTI2_PORT(i)      (-1)                                                                                                                            /**< Port number for AF_TIMER3_CDTI2 location number i */
#define AF_TIMER3_CDTI3_PORT(i)      (-1)                                                                                                                            /**< Port number for AF_TIMER3_CDTI3 location number i */
#define AF_WTIMER0_CC0_PORT(i)       ((i) == 0 ? 4 : (i) == 1 ? 0 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 2 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 2 :  -1) /**< Port number for AF_WTIMER0_CC0 location number i */
#define AF_WTIMER0_CC1_PORT(i)       ((i) == 0 ? 4 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 5 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 2 :  -1) /**< Port number for AF_WTIMER0_CC1 location number i */
#define AF_WTIMER0_CC2_PORT(i)       ((i) == 0 ? 4 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 5 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 2 :  -1) /**< Port number for AF_WTIMER0_CC2 location number i */
#define AF_WTIMER0_CC3_PORT(i)       (-1)                                                                                                                            /**< Port number for AF_WTIMER0_CC3 location number i */
#define AF_WTIMER0_CDTI0_PORT(i)     ((i) == 0 ? 4 : (i) == 1 ? 3 : (i) == 2 ? 0 : (i) == 3 ? -1 : (i) == 4 ? 3 :  -1)                                               /**< Port number for AF_WTIMER0_CDTI0 location number i */
#define AF_WTIMER0_CDTI1_PORT(i)     ((i) == 0 ? 4 : (i) == 1 ? -1 : (i) == 2 ? 0 : (i) == 3 ? -1 : (i) == 4 ? 3 :  -1)                                              /**< Port number for AF_WTIMER0_CDTI1 location number i */
#define AF_WTIMER0_CDTI2_PORT(i)     ((i) == 0 ? 4 : (i) == 1 ? -1 : (i) == 2 ? 0 : (i) == 3 ? -1 : (i) == 4 ? 3 :  -1)                                              /**< Port number for AF_WTIMER0_CDTI2 location number i */
#define AF_WTIMER0_CDTI3_PORT(i)     (-1)                                                                                                                            /**< Port number for AF_WTIMER0_CDTI3 location number i */
#define AF_WTIMER1_CC0_PORT(i)       ((i) == 0 ? 1 : (i) == 1 ? 3 : (i) == 2 ? 3 : (i) == 3 ? 2 : (i) == 4 ? 4 : (i) == 5 ? 4 :  -1)                                 /**< Port number for AF_WTIMER1_CC0 location number i */
#define AF_WTIMER1_CC1_PORT(i)       ((i) == 0 ? 1 : (i) == 1 ? 3 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_WTIMER1_CC1 location number i */
#define AF_WTIMER1_CC2_PORT(i)       ((i) == 0 ? 3 : (i) == 1 ? 3 : (i) == 2 ? 3 : (i) == 3 ? 4 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_WTIMER1_CC2 location number i */
#define AF_WTIMER1_CC3_PORT(i)       ((i) == 0 ? 3 : (i) == 1 ? 3 : (i) == 2 ? 2 : (i) == 3 ? 4 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_WTIMER1_CC3 location number i */
#define AF_WTIMER1_CDTI0_PORT(i)     (-1)                                                                                                                            /**< Port number for AF_WTIMER1_CDTI0 location number i */
#define AF_WTIMER1_CDTI1_PORT(i)     (-1)                                                                                                                            /**< Port number for AF_WTIMER1_CDTI1 location number i */
#define AF_WTIMER1_CDTI2_PORT(i)     (-1)                                                                                                                            /**< Port number for AF_WTIMER1_CDTI2 location number i */
#define AF_WTIMER1_CDTI3_PORT(i)     (-1)                                                                                                                            /**< Port number for AF_WTIMER1_CDTI3 location number i */
#define AF_USART0_TX_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 2 : (i) == 3 ? 4 : (i) == 4 ? 1 : (i) == 5 ? 2 :  -1)                                 /**< Port number for AF_USART0_TX location number i */
#define AF_USART0_RX_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 2 : (i) == 3 ? 4 : (i) == 4 ? 1 : (i) == 5 ? 2 :  -1)                                 /**< Port number for AF_USART0_RX location number i */
#define AF_USART0_CLK_PORT(i)        ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 2 : (i) == 3 ? 2 : (i) == 4 ? 1 : (i) == 5 ? 0 :  -1)                                 /**< Port number for AF_USART0_CLK location number i */
#define AF_USART0_CS_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 2 : (i) == 3 ? 2 : (i) == 4 ? 1 : (i) == 5 ? 0 :  -1)                                 /**< Port number for AF_USART0_CS location number i */
#define AF_USART0_CTS_PORT(i)        ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 2 : (i) == 3 ? 2 : (i) == 4 ? 1 : (i) == 5 ? 1 :  -1)                                 /**< Port number for AF_USART0_CTS location number i */
#define AF_USART0_RTS_PORT(i)        ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 2 : (i) == 3 ? 2 : (i) == 4 ? 1 : (i) == 5 ? 3 :  -1)                                 /**< Port number for AF_USART0_RTS location number i */
#define AF_USART1_TX_PORT(i)         ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 3 : (i) == 3 ? 5 : (i) == 4 ? 2 : (i) == 5 ? 5 : (i) == 6 ? 0 :  -1)                  /**< Port number for AF_USART1_TX location number i */
#define AF_USART1_RX_PORT(i)         ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? 3 : (i) == 3 ? 5 : (i) == 4 ? 2 : (i) == 5 ? 0 : (i) == 6 ? 0 :  -1)                  /**< Port number for AF_USART1_RX location number i */
#define AF_USART1_CLK_PORT(i)        ((i) == 0 ? 1 : (i) == 1 ? 3 : (i) == 2 ? 5 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 1 : (i) == 6 ? 4 :  -1)                  /**< Port number for AF_USART1_CLK location number i */
#define AF_USART1_CS_PORT(i)         ((i) == 0 ? 1 : (i) == 1 ? 3 : (i) == 2 ? 5 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 4 : (i) == 6 ? 1 :  -1)                  /**< Port number for AF_USART1_CS location number i */
#define AF_USART1_CTS_PORT(i)        ((i) == 0 ? 1 : (i) == 1 ? 3 : (i) == 2 ? 5 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 1 :  -1)                                 /**< Port number for AF_USART1_CTS location number i */
#define AF_USART1_RTS_PORT(i)        ((i) == 0 ? 1 : (i) == 1 ? 3 : (i) == 2 ? 5 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 1 :  -1)                                 /**< Port number for AF_USART1_RTS location number i */
#define AF_USART2_TX_PORT(i)         ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 5 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_USART2_TX location number i */
#define AF_USART2_RX_PORT(i)         ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 5 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_USART2_RX location number i */
#define AF_USART2_CLK_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 0 : (i) == 3 ? 0 : (i) == 4 ? 5 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_USART2_CLK location number i */
#define AF_USART2_CS_PORT(i)         ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 0 : (i) == 3 ? 1 : (i) == 4 ? 5 : (i) == 5 ? 5 :  -1)                                 /**< Port number for AF_USART2_CS location number i */
#define AF_USART2_CTS_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 0 : (i) == 3 ? 1 : (i) == 4 ? 2 : (i) == 5 ? 3 :  -1)                                 /**< Port number for AF_USART2_CTS location number i */
#define AF_USART2_RTS_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 0 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 3 :  -1)                                 /**< Port number for AF_USART2_RTS location number i */
#define AF_USART3_TX_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 1 :  -1)                                                                              /**< Port number for AF_USART3_TX location number i */
#define AF_USART3_RX_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 1 :  -1)                                                                              /**< Port number for AF_USART3_RX location number i */
#define AF_USART3_CLK_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 3 :  -1)                                                                              /**< Port number for AF_USART3_CLK location number i */
#define AF_USART3_CS_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 2 : (i) == 3 ? 2 :  -1)                                                               /**< Port number for AF_USART3_CS location number i */
#define AF_USART3_CTS_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 3 :  -1)                                                                              /**< Port number for AF_USART3_CTS location number i */
#define AF_USART3_RTS_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 0 : (i) == 3 ? 2 :  -1)                                                               /**< Port number for AF_USART3_RTS location number i */
#define AF_USART4_TX_PORT(i)         ((i) == 0 ? 1 : (i) == 1 ? 3 :  -1)                                                                                             /**< Port number for AF_USART4_TX location number i */
#define AF_USART4_RX_PORT(i)         ((i) == 0 ? 1 : (i) == 1 ? 3 :  -1)                                                                                             /**< Port number for AF_USART4_RX location number i */
#define AF_USART4_CLK_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 3 :  -1)                                                                                             /**< Port number for AF_USART4_CLK location number i */
#define AF_USART4_CS_PORT(i)         ((i) == 0 ? 2 : (i) == 1 ? 3 :  -1)                                                                                             /**< Port number for AF_USART4_CS location number i */
#define AF_USART4_CTS_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 3 :  -1)                                                                                             /**< Port number for AF_USART4_CTS location number i */
#define AF_USART4_RTS_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 3 :  -1)                                                                                             /**< Port number for AF_USART4_RTS location number i */
#define AF_UART0_TX_PORT(i)          ((i) == 0 ? 5 : (i) == 1 ? 4 : (i) == 2 ? 0 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 5 : (i) == 6 ? 3 :  -1)                  /**< Port number for AF_UART0_TX location number i */
#define AF_UART0_RX_PORT(i)          ((i) == 0 ? 5 : (i) == 1 ? 4 : (i) == 2 ? 0 : (i) == 3 ? 2 : (i) == 4 ? 2 : (i) == 5 ? 5 : (i) == 6 ? 4 :  -1)                  /**< Port number for AF_UART0_RX location number i */
#define AF_UART0_CLK_PORT(i)         (-1)                                                                                                                            /**< Port number for AF_UART0_CLK location number i */
#define AF_UART0_CS_PORT(i)          (-1)                                                                                                                            /**< Port number for AF_UART0_CS location number i */
#define AF_UART0_CTS_PORT(i)         ((i) == 0 ? 5 : (i) == 1 ? 4 : (i) == 2 ? 0 : (i) == 3 ? 2 : (i) == 4 ? 1 : (i) == 5 ? 3 :  -1)                                 /**< Port number for AF_UART0_CTS location number i */
#define AF_UART0_RTS_PORT(i)         ((i) == 0 ? 5 : (i) == 1 ? 4 : (i) == 2 ? 0 : (i) == 3 ? 2 : (i) == 4 ? 1 : (i) == 5 ? 3 :  -1)                                 /**< Port number for AF_UART0_RTS location number i */
#define AF_UART1_TX_PORT(i)          ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? 1 : (i) == 3 ? 4 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_UART1_TX location number i */
#define AF_UART1_RX_PORT(i)          ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? 1 : (i) == 3 ? 4 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_UART1_RX location number i */
#define AF_UART1_CLK_PORT(i)         (-1)                                                                                                                            /**< Port number for AF_UART1_CLK location number i */
#define AF_UART1_CS_PORT(i)          (-1)                                                                                                                            /**< Port number for AF_UART1_CS location number i */
#define AF_UART1_CTS_PORT(i)         ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? 1 : (i) == 3 ? 4 : (i) == 4 ? 2 :  -1)                                                /**< Port number for AF_UART1_CTS location number i */
#define AF_UART1_RTS_PORT(i)         ((i) == 0 ? 2 : (i) == 1 ? 5 : (i) == 2 ? 1 : (i) == 3 ? 4 : (i) == 4 ? 2 :  -1)                                                /**< Port number for AF_UART1_RTS location number i */
#define AF_QSPI0_SCLK_PORT(i)        ((i) == 0 ? 5 : (i) == 1 ? 4 :  -1)                                                                                             /**< Port number for AF_QSPI0_SCLK location number i */
#define AF_QSPI0_DQ0_PORT(i)         ((i) == 0 ? 3 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_QSPI0_DQ0 location number i */
#define AF_QSPI0_DQ1_PORT(i)         ((i) == 0 ? 3 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_QSPI0_DQ1 location number i */
#define AF_QSPI0_DQ2_PORT(i)         ((i) == 0 ? 3 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_QSPI0_DQ2 location number i */
#define AF_QSPI0_DQ3_PORT(i)         ((i) == 0 ? 3 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_QSPI0_DQ3 location number i */
#define AF_QSPI0_DQ4_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_QSPI0_DQ4 location number i */
#define AF_QSPI0_DQ5_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_QSPI0_DQ5 location number i */
#define AF_QSPI0_DQ6_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_QSPI0_DQ6 location number i */
#define AF_QSPI0_DQ7_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 1 :  -1)                                                                                             /**< Port number for AF_QSPI0_DQ7 location number i */
#define AF_QSPI0_CS0_PORT(i)         ((i) == 0 ? 5 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_QSPI0_CS0 location number i */
#define AF_QSPI0_CS1_PORT(i)         ((i) == 0 ? 5 : (i) == 1 ? 0 :  -1)                                                                                             /**< Port number for AF_QSPI0_CS1 location number i */
#define AF_QSPI0_DQS_PORT(i)         ((i) == 0 ? 5 : (i) == 1 ? 4 :  -1)                                                                                             /**< Port number for AF_QSPI0_DQS location number i */
#define AF_QSPI0_RST0_PORT(i)        ((i) == 0 ? 4 : (i) == 1 ? 2 :  -1)                                                                                             /**< Port number for AF_QSPI0_RST0 location number i */
#define AF_QSPI0_RST1_PORT(i)        ((i) == 0 ? 4 : (i) == 1 ? 2 :  -1)                                                                                             /**< Port number for AF_QSPI0_RST1 location number i */
#define AF_LEUART0_TX_PORT(i)        ((i) == 0 ? 3 : (i) == 1 ? 1 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 5 : (i) == 5 ? 2 :  -1)                                 /**< Port number for AF_LEUART0_TX location number i */
#define AF_LEUART0_RX_PORT(i)        ((i) == 0 ? 3 : (i) == 1 ? 1 : (i) == 2 ? 4 : (i) == 3 ? 5 : (i) == 4 ? 0 : (i) == 5 ? 2 :  -1)                                 /**< Port number for AF_LEUART0_RX location number i */
#define AF_LEUART1_TX_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 0 : (i) == 2 ? 3 : (i) == 3 ? 1 : (i) == 4 ? 1 :  -1)                                                /**< Port number for AF_LEUART1_TX location number i */
#define AF_LEUART1_RX_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 0 : (i) == 2 ? 3 : (i) == 3 ? 1 : (i) == 4 ? 1 :  -1)                                                /**< Port number for AF_LEUART1_RX location number i */
#define AF_LETIMER0_OUT0_PORT(i)     ((i) == 0 ? 3 : (i) == 1 ? 1 : (i) == 2 ? 5 : (i) == 3 ? 2 : (i) == 4 ? 4 : (i) == 5 ? 2 : (i) == 6 ? 0 : (i) == 7 ? 1 :  -1)   /**< Port number for AF_LETIMER0_OUT0 location number i */
#define AF_LETIMER0_OUT1_PORT(i)     ((i) == 0 ? 3 : (i) == 1 ? 1 : (i) == 2 ? 5 : (i) == 3 ? 2 : (i) == 4 ? 4 : (i) == 5 ? 2 : (i) == 6 ? 0 : (i) == 7 ? 1 :  -1)   /**< Port number for AF_LETIMER0_OUT1 location number i */
#define AF_LETIMER1_OUT0_PORT(i)     ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 2 : (i) == 4 ? 1 : (i) == 5 ? 1 :  -1)                                 /**< Port number for AF_LETIMER1_OUT0 location number i */
#define AF_LETIMER1_OUT1_PORT(i)     ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? 2 : (i) == 4 ? 1 : (i) == 5 ? 1 :  -1)                                 /**< Port number for AF_LETIMER1_OUT1 location number i */
#define AF_PCNT0_S0IN_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 :  -1)   /**< Port number for AF_PCNT0_S0IN location number i */
#define AF_PCNT0_S1IN_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 :  -1)   /**< Port number for AF_PCNT0_S1IN location number i */
#define AF_PCNT1_S0IN_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 3 : (i) == 3 ? 2 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 1 :  -1)                  /**< Port number for AF_PCNT1_S0IN location number i */
#define AF_PCNT1_S1IN_PORT(i)        ((i) == 0 ? 0 : (i) == 1 ? 1 : (i) == 2 ? 1 : (i) == 3 ? 2 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 1 :  -1)                  /**< Port number for AF_PCNT1_S1IN location number i */
#define AF_PCNT2_S0IN_PORT(i)        ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 1 : (i) == 3 ? 5 : (i) == 4 ? 2 :  -1)                                                /**< Port number for AF_PCNT2_S0IN location number i */
#define AF_PCNT2_S1IN_PORT(i)        ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 1 : (i) == 3 ? 5 : (i) == 4 ? 2 :  -1)                                                /**< Port number for AF_PCNT2_S1IN location number i */
#define AF_I2C0_SDA_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 2 : (i) == 5 ? 5 : (i) == 6 ? 4 : (i) == 7 ? 4 :  -1)   /**< Port number for AF_I2C0_SDA location number i */
#define AF_I2C0_SCL_PORT(i)          ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 2 : (i) == 3 ? 3 : (i) == 4 ? 2 : (i) == 5 ? 5 : (i) == 6 ? 4 : (i) == 7 ? 4 :  -1)   /**< Port number for AF_I2C0_SCL location number i */
#define AF_I2C1_SDA_PORT(i)          ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 4 : (i) == 3 ? 3 : (i) == 4 ? 2 :  -1)                                                /**< Port number for AF_I2C1_SDA location number i */
#define AF_I2C1_SCL_PORT(i)          ((i) == 0 ? 2 : (i) == 1 ? 1 : (i) == 2 ? 4 : (i) == 3 ? 3 : (i) == 4 ? 5 :  -1)                                                /**< Port number for AF_I2C1_SCL location number i */
#define AF_ACMP0_OUT_PORT(i)         ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 3 : (i) == 3 ? 1 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 1 :  -1)   /**< Port number for AF_ACMP0_OUT location number i */
#define AF_ACMP1_OUT_PORT(i)         ((i) == 0 ? 5 : (i) == 1 ? 4 : (i) == 2 ? 3 : (i) == 3 ? 0 : (i) == 4 ? 0 : (i) == 5 ? 1 : (i) == 6 ? 1 : (i) == 7 ? 0 :  -1)   /**< Port number for AF_ACMP1_OUT location number i */
#define AF_ACMP2_OUT_PORT(i)         ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 4 :  -1)                                                                              /**< Port number for AF_ACMP2_OUT location number i */
#define AF_USB_VBUSEN_PORT(i)        ((i) == 0 ? 5 : (i) == 1 ? 4 : (i) == 2 ? 3 :  -1)                                                                              /**< Port number for AF_USB_VBUSEN location number i */
#define AF_DBG_TDI_PORT(i)           ((i) == 0 ? 5 :  -1)                                                                                                            /**< Port number for AF_DBG_TDI location number i */
#define AF_DBG_TDO_PORT(i)           ((i) == 0 ? 5 :  -1)                                                                                                            /**< Port number for AF_DBG_TDO location number i */
#define AF_DBG_SWV_PORT(i)           ((i) == 0 ? 5 : (i) == 1 ? 2 : (i) == 2 ? 3 : (i) == 3 ? 3 :  -1)                                                               /**< Port number for AF_DBG_SWV location number i */
#define AF_DBG_SWDIOTMS_PORT(i)      ((i) == 0 ? 5 :  -1)                                                                                                            /**< Port number for AF_DBG_SWDIOTMS location number i */
#define AF_DBG_SWCLKTCK_PORT(i)      ((i) == 0 ? 5 :  -1)                                                                                                            /**< Port number for AF_DBG_SWCLKTCK location number i */
#define AF_ETM_TCLK_PORT(i)          ((i) == 0 ? 3 : (i) == 1 ? 5 : (i) == 2 ? 2 : (i) == 3 ? 0 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_ETM_TCLK location number i */
#define AF_ETM_TD0_PORT(i)           ((i) == 0 ? 3 : (i) == 1 ? 5 : (i) == 2 ? 2 : (i) == 3 ? 0 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_ETM_TD0 location number i */
#define AF_ETM_TD1_PORT(i)           ((i) == 0 ? 3 : (i) == 1 ? 3 : (i) == 2 ? 3 : (i) == 3 ? 0 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_ETM_TD1 location number i */
#define AF_ETM_TD2_PORT(i)           ((i) == 0 ? 3 : (i) == 1 ? 1 : (i) == 2 ? 3 : (i) == 3 ? 0 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_ETM_TD2 location number i */
#define AF_ETM_TD3_PORT(i)           ((i) == 0 ? 3 : (i) == 1 ? 5 : (i) == 2 ? 3 : (i) == 3 ? 0 : (i) == 4 ? 4 :  -1)                                                /**< Port number for AF_ETM_TD3 location number i */

/** @} */
/** @} End of group EFM32GG12B_AF_Ports */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

