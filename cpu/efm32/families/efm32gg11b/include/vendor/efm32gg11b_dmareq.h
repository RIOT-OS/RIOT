/***************************************************************************//**
 * @file
 * @brief EFM32GG11B_DMAREQ register and bit field definitions
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
 * @addtogroup EFM32GG11B_DMAREQ DMAREQ
 * @{
 * @defgroup EFM32GG11B_DMAREQ_BitFields DMAREQ Bit Fields
 * @{
 ******************************************************************************/
#define DMAREQ_PRS_REQ0               ((1 << 16) + 0)  /**< DMA channel select for PRS_REQ0 */
#define DMAREQ_PRS_REQ1               ((1 << 16) + 1)  /**< DMA channel select for PRS_REQ1 */
#define DMAREQ_ADC0_SINGLE            ((8 << 16) + 0)  /**< DMA channel select for ADC0_SINGLE */
#define DMAREQ_ADC0_SCAN              ((8 << 16) + 1)  /**< DMA channel select for ADC0_SCAN */
#define DMAREQ_ADC1_SINGLE            ((9 << 16) + 0)  /**< DMA channel select for ADC1_SINGLE */
#define DMAREQ_ADC1_SCAN              ((9 << 16) + 1)  /**< DMA channel select for ADC1_SCAN */
#define DMAREQ_VDAC0_CH0              ((10 << 16) + 0) /**< DMA channel select for VDAC0_CH0 */
#define DMAREQ_VDAC0_CH1              ((10 << 16) + 1) /**< DMA channel select for VDAC0_CH1 */
#define DMAREQ_USART0_RXDATAV         ((12 << 16) + 0) /**< DMA channel select for USART0_RXDATAV */
#define DMAREQ_USART0_TXBL            ((12 << 16) + 1) /**< DMA channel select for USART0_TXBL */
#define DMAREQ_USART0_TXEMPTY         ((12 << 16) + 2) /**< DMA channel select for USART0_TXEMPTY */
#define DMAREQ_USART1_RXDATAV         ((13 << 16) + 0) /**< DMA channel select for USART1_RXDATAV */
#define DMAREQ_USART1_TXBL            ((13 << 16) + 1) /**< DMA channel select for USART1_TXBL */
#define DMAREQ_USART1_TXEMPTY         ((13 << 16) + 2) /**< DMA channel select for USART1_TXEMPTY */
#define DMAREQ_USART1_RXDATAVRIGHT    ((13 << 16) + 3) /**< DMA channel select for USART1_RXDATAVRIGHT */
#define DMAREQ_USART1_TXBLRIGHT       ((13 << 16) + 4) /**< DMA channel select for USART1_TXBLRIGHT */
#define DMAREQ_USART2_RXDATAV         ((14 << 16) + 0) /**< DMA channel select for USART2_RXDATAV */
#define DMAREQ_USART2_TXBL            ((14 << 16) + 1) /**< DMA channel select for USART2_TXBL */
#define DMAREQ_USART2_TXEMPTY         ((14 << 16) + 2) /**< DMA channel select for USART2_TXEMPTY */
#define DMAREQ_USART3_RXDATAV         ((15 << 16) + 0) /**< DMA channel select for USART3_RXDATAV */
#define DMAREQ_USART3_TXBL            ((15 << 16) + 1) /**< DMA channel select for USART3_TXBL */
#define DMAREQ_USART3_TXEMPTY         ((15 << 16) + 2) /**< DMA channel select for USART3_TXEMPTY */
#define DMAREQ_USART3_RXDATAVRIGHT    ((15 << 16) + 3) /**< DMA channel select for USART3_RXDATAVRIGHT */
#define DMAREQ_USART3_TXBLRIGHT       ((15 << 16) + 4) /**< DMA channel select for USART3_TXBLRIGHT */
#define DMAREQ_USART4_RXDATAV         ((16 << 16) + 0) /**< DMA channel select for USART4_RXDATAV */
#define DMAREQ_USART4_TXBL            ((16 << 16) + 1) /**< DMA channel select for USART4_TXBL */
#define DMAREQ_USART4_TXEMPTY         ((16 << 16) + 2) /**< DMA channel select for USART4_TXEMPTY */
#define DMAREQ_USART4_RXDATAVRIGHT    ((16 << 16) + 3) /**< DMA channel select for USART4_RXDATAVRIGHT */
#define DMAREQ_USART4_TXBLRIGHT       ((16 << 16) + 4) /**< DMA channel select for USART4_TXBLRIGHT */
#define DMAREQ_USART5_RXDATAV         ((17 << 16) + 0) /**< DMA channel select for USART5_RXDATAV */
#define DMAREQ_USART5_TXBL            ((17 << 16) + 1) /**< DMA channel select for USART5_TXBL */
#define DMAREQ_USART5_TXEMPTY         ((17 << 16) + 2) /**< DMA channel select for USART5_TXEMPTY */
#define DMAREQ_UART0_RXDATAV          ((18 << 16) + 0) /**< DMA channel select for UART0_RXDATAV */
#define DMAREQ_UART0_TXBL             ((18 << 16) + 1) /**< DMA channel select for UART0_TXBL */
#define DMAREQ_UART0_TXEMPTY          ((18 << 16) + 2) /**< DMA channel select for UART0_TXEMPTY */
#define DMAREQ_UART1_RXDATAV          ((19 << 16) + 0) /**< DMA channel select for UART1_RXDATAV */
#define DMAREQ_UART1_TXBL             ((19 << 16) + 1) /**< DMA channel select for UART1_TXBL */
#define DMAREQ_UART1_TXEMPTY          ((19 << 16) + 2) /**< DMA channel select for UART1_TXEMPTY */
#define DMAREQ_LEUART0_RXDATAV        ((20 << 16) + 0) /**< DMA channel select for LEUART0_RXDATAV */
#define DMAREQ_LEUART0_TXBL           ((20 << 16) + 1) /**< DMA channel select for LEUART0_TXBL */
#define DMAREQ_LEUART0_TXEMPTY        ((20 << 16) + 2) /**< DMA channel select for LEUART0_TXEMPTY */
#define DMAREQ_LEUART1_RXDATAV        ((21 << 16) + 0) /**< DMA channel select for LEUART1_RXDATAV */
#define DMAREQ_LEUART1_TXBL           ((21 << 16) + 1) /**< DMA channel select for LEUART1_TXBL */
#define DMAREQ_LEUART1_TXEMPTY        ((21 << 16) + 2) /**< DMA channel select for LEUART1_TXEMPTY */
#define DMAREQ_I2C0_RXDATAV           ((22 << 16) + 0) /**< DMA channel select for I2C0_RXDATAV */
#define DMAREQ_I2C0_TXBL              ((22 << 16) + 1) /**< DMA channel select for I2C0_TXBL */
#define DMAREQ_I2C1_RXDATAV           ((23 << 16) + 0) /**< DMA channel select for I2C1_RXDATAV */
#define DMAREQ_I2C1_TXBL              ((23 << 16) + 1) /**< DMA channel select for I2C1_TXBL */
#define DMAREQ_I2C2_RXDATAV           ((24 << 16) + 0) /**< DMA channel select for I2C2_RXDATAV */
#define DMAREQ_I2C2_TXBL              ((24 << 16) + 1) /**< DMA channel select for I2C2_TXBL */
#define DMAREQ_TIMER0_UFOF            ((25 << 16) + 0) /**< DMA channel select for TIMER0_UFOF */
#define DMAREQ_TIMER0_CC0             ((25 << 16) + 1) /**< DMA channel select for TIMER0_CC0 */
#define DMAREQ_TIMER0_CC1             ((25 << 16) + 2) /**< DMA channel select for TIMER0_CC1 */
#define DMAREQ_TIMER0_CC2             ((25 << 16) + 3) /**< DMA channel select for TIMER0_CC2 */
#define DMAREQ_TIMER1_UFOF            ((26 << 16) + 0) /**< DMA channel select for TIMER1_UFOF */
#define DMAREQ_TIMER1_CC0             ((26 << 16) + 1) /**< DMA channel select for TIMER1_CC0 */
#define DMAREQ_TIMER1_CC1             ((26 << 16) + 2) /**< DMA channel select for TIMER1_CC1 */
#define DMAREQ_TIMER1_CC2             ((26 << 16) + 3) /**< DMA channel select for TIMER1_CC2 */
#define DMAREQ_TIMER1_CC3             ((26 << 16) + 4) /**< DMA channel select for TIMER1_CC3 */
#define DMAREQ_TIMER2_UFOF            ((27 << 16) + 0) /**< DMA channel select for TIMER2_UFOF */
#define DMAREQ_TIMER2_CC0             ((27 << 16) + 1) /**< DMA channel select for TIMER2_CC0 */
#define DMAREQ_TIMER2_CC1             ((27 << 16) + 2) /**< DMA channel select for TIMER2_CC1 */
#define DMAREQ_TIMER2_CC2             ((27 << 16) + 3) /**< DMA channel select for TIMER2_CC2 */
#define DMAREQ_TIMER3_UFOF            ((28 << 16) + 0) /**< DMA channel select for TIMER3_UFOF */
#define DMAREQ_TIMER3_CC0             ((28 << 16) + 1) /**< DMA channel select for TIMER3_CC0 */
#define DMAREQ_TIMER3_CC1             ((28 << 16) + 2) /**< DMA channel select for TIMER3_CC1 */
#define DMAREQ_TIMER3_CC2             ((28 << 16) + 3) /**< DMA channel select for TIMER3_CC2 */
#define DMAREQ_TIMER4_UFOF            ((29 << 16) + 0) /**< DMA channel select for TIMER4_UFOF */
#define DMAREQ_TIMER4_CC0             ((29 << 16) + 1) /**< DMA channel select for TIMER4_CC0 */
#define DMAREQ_TIMER4_CC1             ((29 << 16) + 2) /**< DMA channel select for TIMER4_CC1 */
#define DMAREQ_TIMER4_CC2             ((29 << 16) + 3) /**< DMA channel select for TIMER4_CC2 */
#define DMAREQ_TIMER5_UFOF            ((30 << 16) + 0) /**< DMA channel select for TIMER5_UFOF */
#define DMAREQ_TIMER5_CC0             ((30 << 16) + 1) /**< DMA channel select for TIMER5_CC0 */
#define DMAREQ_TIMER5_CC1             ((30 << 16) + 2) /**< DMA channel select for TIMER5_CC1 */
#define DMAREQ_TIMER5_CC2             ((30 << 16) + 3) /**< DMA channel select for TIMER5_CC2 */
#define DMAREQ_TIMER6_UFOF            ((31 << 16) + 0) /**< DMA channel select for TIMER6_UFOF */
#define DMAREQ_TIMER6_CC0             ((31 << 16) + 1) /**< DMA channel select for TIMER6_CC0 */
#define DMAREQ_TIMER6_CC1             ((31 << 16) + 2) /**< DMA channel select for TIMER6_CC1 */
#define DMAREQ_TIMER6_CC2             ((31 << 16) + 3) /**< DMA channel select for TIMER6_CC2 */
#define DMAREQ_WTIMER0_UFOF           ((32 << 16) + 0) /**< DMA channel select for WTIMER0_UFOF */
#define DMAREQ_WTIMER0_CC0            ((32 << 16) + 1) /**< DMA channel select for WTIMER0_CC0 */
#define DMAREQ_WTIMER0_CC1            ((32 << 16) + 2) /**< DMA channel select for WTIMER0_CC1 */
#define DMAREQ_WTIMER0_CC2            ((32 << 16) + 3) /**< DMA channel select for WTIMER0_CC2 */
#define DMAREQ_WTIMER1_UFOF           ((33 << 16) + 0) /**< DMA channel select for WTIMER1_UFOF */
#define DMAREQ_WTIMER1_CC0            ((33 << 16) + 1) /**< DMA channel select for WTIMER1_CC0 */
#define DMAREQ_WTIMER1_CC1            ((33 << 16) + 2) /**< DMA channel select for WTIMER1_CC1 */
#define DMAREQ_WTIMER1_CC2            ((33 << 16) + 3) /**< DMA channel select for WTIMER1_CC2 */
#define DMAREQ_WTIMER1_CC3            ((33 << 16) + 4) /**< DMA channel select for WTIMER1_CC3 */
#define DMAREQ_WTIMER2_UFOF           ((34 << 16) + 0) /**< DMA channel select for WTIMER2_UFOF */
#define DMAREQ_WTIMER2_CC0            ((34 << 16) + 1) /**< DMA channel select for WTIMER2_CC0 */
#define DMAREQ_WTIMER2_CC1            ((34 << 16) + 2) /**< DMA channel select for WTIMER2_CC1 */
#define DMAREQ_WTIMER2_CC2            ((34 << 16) + 3) /**< DMA channel select for WTIMER2_CC2 */
#define DMAREQ_WTIMER3_UFOF           ((35 << 16) + 0) /**< DMA channel select for WTIMER3_UFOF */
#define DMAREQ_WTIMER3_CC0            ((35 << 16) + 1) /**< DMA channel select for WTIMER3_CC0 */
#define DMAREQ_WTIMER3_CC1            ((35 << 16) + 2) /**< DMA channel select for WTIMER3_CC1 */
#define DMAREQ_WTIMER3_CC2            ((35 << 16) + 3) /**< DMA channel select for WTIMER3_CC2 */
#define DMAREQ_MSC_WDATA              ((48 << 16) + 0) /**< DMA channel select for MSC_WDATA */
#define DMAREQ_CRYPTO0_DATA0WR        ((49 << 16) + 0) /**< DMA channel select for CRYPTO0_DATA0WR */
#define DMAREQ_CRYPTO0_DATA0XWR       ((49 << 16) + 1) /**< DMA channel select for CRYPTO0_DATA0XWR */
#define DMAREQ_CRYPTO0_DATA0RD        ((49 << 16) + 2) /**< DMA channel select for CRYPTO0_DATA0RD */
#define DMAREQ_CRYPTO0_DATA1WR        ((49 << 16) + 3) /**< DMA channel select for CRYPTO0_DATA1WR */
#define DMAREQ_CRYPTO0_DATA1RD        ((49 << 16) + 4) /**< DMA channel select for CRYPTO0_DATA1RD */
#define DMAREQ_EBI_PXL0EMPTY          ((50 << 16) + 0) /**< DMA channel select for EBI_PXL0EMPTY */
#define DMAREQ_EBI_PXL1EMPTY          ((50 << 16) + 1) /**< DMA channel select for EBI_PXL1EMPTY */
#define DMAREQ_EBI_PXLFULL            ((50 << 16) + 2) /**< DMA channel select for EBI_PXLFULL */
#define DMAREQ_EBI_DDEMPTY            ((50 << 16) + 3) /**< DMA channel select for EBI_DDEMPTY */
#define DMAREQ_EBI_VSYNC              ((50 << 16) + 4) /**< DMA channel select for EBI_VSYNC */
#define DMAREQ_EBI_HSYNC              ((50 << 16) + 5) /**< DMA channel select for EBI_HSYNC */
#define DMAREQ_CSEN_DATA              ((61 << 16) + 0) /**< DMA channel select for CSEN_DATA */
#define DMAREQ_CSEN_BSLN              ((61 << 16) + 1) /**< DMA channel select for CSEN_BSLN */
#define DMAREQ_LESENSE_BUFDATAV       ((62 << 16) + 0) /**< DMA channel select for LESENSE_BUFDATAV */

/** @} */
/** @} End of group EFM32GG11B_DMAREQ */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

