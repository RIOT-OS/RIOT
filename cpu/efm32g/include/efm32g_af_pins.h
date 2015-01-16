/**************************************************************************//**
 * @file efm32g_af_pins.h
 * @brief EFM32G_AF_PINS register and bit field definitions
 * @version 3.20.6
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Laboratories, Inc. http://www.silabs.com</b>
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.@n
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.@n
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Laboratories, Inc.
 * has no obligation to support this Software. Silicon Laboratories, Inc. is
 * providing the Software "AS IS", with no express or implied warranties of any
 * kind, including, but not limited to, any implied warranties of
 * merchantability or fitness for any particular purpose or warranties against
 * infringement of any proprietary rights of a third party.
 *
 * Silicon Laboratories, Inc. will not be liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this Software.
 *
 *****************************************************************************/
/**************************************************************************//**
 * @defgroup EFM32G_AF_Pins
 * @{
 *****************************************************************************/

/** AF pin number for location number i */
#define AF_CMU_CLK0_PIN(i)         ((i) == 0 ? 2 : (i) == 1 ? 12 :  -1)
#define AF_CMU_CLK1_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? 8 :  -1)
#define AF_EBI_AD00_PIN(i)         ((i) == 0 ? 8 :  -1)
#define AF_EBI_AD01_PIN(i)         ((i) == 0 ? 9 :  -1)
#define AF_EBI_AD02_PIN(i)         ((i) == 0 ? 10 :  -1)
#define AF_EBI_AD03_PIN(i)         ((i) == 0 ? 11 :  -1)
#define AF_EBI_AD04_PIN(i)         ((i) == 0 ? 12 :  -1)
#define AF_EBI_AD05_PIN(i)         ((i) == 0 ? 13 :  -1)
#define AF_EBI_AD06_PIN(i)         ((i) == 0 ? 14 :  -1)
#define AF_EBI_AD07_PIN(i)         ((i) == 0 ? 15 :  -1)
#define AF_EBI_AD08_PIN(i)         ((i) == 0 ? 15 :  -1)
#define AF_EBI_AD09_PIN(i)         ((i) == 0 ? 0 :  -1)
#define AF_EBI_AD10_PIN(i)         ((i) == 0 ? 1 :  -1)
#define AF_EBI_AD11_PIN(i)         ((i) == 0 ? 2 :  -1)
#define AF_EBI_AD12_PIN(i)         ((i) == 0 ? 3 :  -1)
#define AF_EBI_AD13_PIN(i)         ((i) == 0 ? 4 :  -1)
#define AF_EBI_AD14_PIN(i)         ((i) == 0 ? 5 :  -1)
#define AF_EBI_AD15_PIN(i)         ((i) == 0 ? 6 :  -1)
#define AF_EBI_CS0_PIN(i)          ((i) == 0 ? 9 :  -1)
#define AF_EBI_CS1_PIN(i)          ((i) == 0 ? 10 :  -1)
#define AF_EBI_CS2_PIN(i)          ((i) == 0 ? 11 :  -1)
#define AF_EBI_CS3_PIN(i)          ((i) == 0 ? 12 :  -1)
#define AF_EBI_WEn_PIN(i)          ((i) == 0 ? 4 :  -1)
#define AF_EBI_REn_PIN(i)          ((i) == 0 ? 5 :  -1)
#define AF_EBI_ARDY_PIN(i)         ((i) == 0 ? 2 :  -1)
#define AF_EBI_ALE_PIN(i)          ((i) == 0 ? 3 :  -1)
#define AF_TIMER0_CC0_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 6 : (i) == 3 ? 1 :  -1)
#define AF_TIMER0_CC1_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 1 : (i) == 2 ? 7 : (i) == 3 ? 2 :  -1)
#define AF_TIMER0_CC2_PIN(i)       ((i) == 0 ? 2 : (i) == 1 ? 2 : (i) == 2 ? 8 : (i) == 3 ? 3 :  -1)
#define AF_TIMER0_CDTI0_PIN(i)     ((i) == 0 ? 3 : (i) == 1 ? 13 : (i) == 2 ? 3 : (i) == 3 ? 13 :  -1)
#define AF_TIMER0_CDTI1_PIN(i)     ((i) == 0 ? 4 : (i) == 1 ? 14 : (i) == 2 ? 4 : (i) == 3 ? 14 :  -1)
#define AF_TIMER0_CDTI2_PIN(i)     ((i) == 0 ? 5 : (i) == 1 ? 15 : (i) == 2 ? 5 : (i) == 3 ? 15 :  -1)
#define AF_TIMER1_CC0_PIN(i)       ((i) == 0 ? 13 : (i) == 1 ? 10 : (i) == 2 ? 0 :  -1)
#define AF_TIMER1_CC1_PIN(i)       ((i) == 0 ? 14 : (i) == 1 ? 11 : (i) == 2 ? 1 :  -1)
#define AF_TIMER1_CC2_PIN(i)       ((i) == 0 ? 15 : (i) == 1 ? 12 : (i) == 2 ? 2 :  -1)
#define AF_TIMER1_CDTI0_PIN(i)     (-1)
#define AF_TIMER1_CDTI1_PIN(i)     (-1)
#define AF_TIMER1_CDTI2_PIN(i)     (-1)
#define AF_TIMER2_CC0_PIN(i)       ((i) == 0 ? 8 : (i) == 1 ? 12 : (i) == 2 ? 8 :  -1)
#define AF_TIMER2_CC1_PIN(i)       ((i) == 0 ? 9 : (i) == 1 ? 13 : (i) == 2 ? 9 :  -1)
#define AF_TIMER2_CC2_PIN(i)       ((i) == 0 ? 10 : (i) == 1 ? 14 : (i) == 2 ? 10 :  -1)
#define AF_TIMER2_CDTI0_PIN(i)     (-1)
#define AF_TIMER2_CDTI1_PIN(i)     (-1)
#define AF_TIMER2_CDTI2_PIN(i)     (-1)
#define AF_USART0_TX_PIN(i)        ((i) == 0 ? 10 : (i) == 1 ? 7 : (i) == 2 ? 11 :  -1)
#define AF_USART0_RX_PIN(i)        ((i) == 0 ? 11 : (i) == 1 ? 6 : (i) == 2 ? 10 :  -1)
#define AF_USART0_CLK_PIN(i)       ((i) == 0 ? 12 : (i) == 1 ? 5 : (i) == 2 ? 9 :  -1)
#define AF_USART0_CS_PIN(i)        ((i) == 0 ? 13 : (i) == 1 ? 4 : (i) == 2 ? 8 :  -1)
#define AF_USART1_TX_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 0 :  -1)
#define AF_USART1_RX_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 1 :  -1)
#define AF_USART1_CLK_PIN(i)       ((i) == 0 ? 7 : (i) == 1 ? 2 :  -1)
#define AF_USART1_CS_PIN(i)        ((i) == 0 ? 8 : (i) == 1 ? 3 :  -1)
#define AF_USART2_TX_PIN(i)        ((i) == 0 ? 2 : (i) == 1 ? 3 :  -1)
#define AF_USART2_RX_PIN(i)        ((i) == 0 ? 3 : (i) == 1 ? 4 :  -1)
#define AF_USART2_CLK_PIN(i)       ((i) == 0 ? 4 : (i) == 1 ? 5 :  -1)
#define AF_USART2_CS_PIN(i)        ((i) == 0 ? 5 : (i) == 1 ? 6 :  -1)
#define AF_UART0_TX_PIN(i)         ((i) == 0 ? 6 : (i) == 1 ? 0 : (i) == 2 ? 3 : (i) == 3 ? 14 :  -1)
#define AF_UART0_RX_PIN(i)         ((i) == 0 ? 7 : (i) == 1 ? 1 : (i) == 2 ? 4 : (i) == 3 ? 15 :  -1)
#define AF_UART0_CLK_PIN(i)        (-1)
#define AF_UART0_CS_PIN(i)         (-1)
#define AF_LEUART0_TX_PIN(i)       ((i) == 0 ? 4 : (i) == 1 ? 13 : (i) == 2 ? 14 :  -1)
#define AF_LEUART0_RX_PIN(i)       ((i) == 0 ? 5 : (i) == 1 ? 14 : (i) == 2 ? 15 :  -1)
#define AF_LEUART1_TX_PIN(i)       ((i) == 0 ? 6 : (i) == 1 ? 5 :  -1)
#define AF_LEUART1_RX_PIN(i)       ((i) == 0 ? 7 : (i) == 1 ? 6 :  -1)
#define AF_LETIMER0_OUT0_PIN(i)    ((i) == 0 ? 6 : (i) == 1 ? 11 : (i) == 2 ? 0 : (i) == 3 ? 4 :  -1)
#define AF_LETIMER0_OUT1_PIN(i)    ((i) == 0 ? 7 : (i) == 1 ? 12 : (i) == 2 ? 1 : (i) == 3 ? 5 :  -1)
#define AF_PCNT0_S0IN_PIN(i)       ((i) == 0 ? 13 : (i) == 1 ? 0 : (i) == 2 ? 0 :  -1)
#define AF_PCNT0_S1IN_PIN(i)       ((i) == 0 ? 14 : (i) == 1 ? 1 : (i) == 2 ? 1 :  -1)
#define AF_PCNT1_S0IN_PIN(i)       ((i) == 0 ? 4 : (i) == 1 ? 3 :  -1)
#define AF_PCNT1_S1IN_PIN(i)       ((i) == 0 ? 5 : (i) == 1 ? 4 :  -1)
#define AF_PCNT2_S0IN_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? 8 :  -1)
#define AF_PCNT2_S1IN_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 9 :  -1)
#define AF_ACMP0_OUT_PIN(i)        ((i) == 0 ? 13 : (i) == 1 ? 2 :  -1)
#define AF_ACMP1_OUT_PIN(i)        ((i) == 0 ? 2 : (i) == 1 ? 3 :  -1)
#define AF_I2C0_SDA_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 6 : (i) == 2 ? 6 : (i) == 3 ? 14 :  -1)
#define AF_I2C0_SCL_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? 7 : (i) == 2 ? 7 : (i) == 3 ? 15 :  -1)
#define AF_DBG_SWO_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 15 :  -1)
#define AF_DBG_SWDIO_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 1 :  -1)
#define AF_DBG_SWCLK_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 0 :  -1)

/** @} End of group EFM32G_AF_Pins */


