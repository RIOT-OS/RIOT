/**************************************************************************//**
 * @file efm32g_af_ports.h
 * @brief EFM32G_AF_PORTS register and bit field definitions
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
 * @defgroup EFM32G_AF_Ports
 * @{
 *****************************************************************************/

/** AF port number for location number i */
#define AF_CMU_CLK0_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 2 :  -1)
#define AF_CMU_CLK1_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 3 :  -1)
#define AF_EBI_AD00_PORT(i)         ((i) == 0 ? 4 :  -1)
#define AF_EBI_AD01_PORT(i)         ((i) == 0 ? 4 :  -1)
#define AF_EBI_AD02_PORT(i)         ((i) == 0 ? 4 :  -1)
#define AF_EBI_AD03_PORT(i)         ((i) == 0 ? 4 :  -1)
#define AF_EBI_AD04_PORT(i)         ((i) == 0 ? 4 :  -1)
#define AF_EBI_AD05_PORT(i)         ((i) == 0 ? 4 :  -1)
#define AF_EBI_AD06_PORT(i)         ((i) == 0 ? 4 :  -1)
#define AF_EBI_AD07_PORT(i)         ((i) == 0 ? 4 :  -1)
#define AF_EBI_AD08_PORT(i)         ((i) == 0 ? 0 :  -1)
#define AF_EBI_AD09_PORT(i)         ((i) == 0 ? 0 :  -1)
#define AF_EBI_AD10_PORT(i)         ((i) == 0 ? 0 :  -1)
#define AF_EBI_AD11_PORT(i)         ((i) == 0 ? 0 :  -1)
#define AF_EBI_AD12_PORT(i)         ((i) == 0 ? 0 :  -1)
#define AF_EBI_AD13_PORT(i)         ((i) == 0 ? 0 :  -1)
#define AF_EBI_AD14_PORT(i)         ((i) == 0 ? 0 :  -1)
#define AF_EBI_AD15_PORT(i)         ((i) == 0 ? 0 :  -1)
#define AF_EBI_CS0_PORT(i)          ((i) == 0 ? 3 :  -1)
#define AF_EBI_CS1_PORT(i)          ((i) == 0 ? 3 :  -1)
#define AF_EBI_CS2_PORT(i)          ((i) == 0 ? 3 :  -1)
#define AF_EBI_CS3_PORT(i)          ((i) == 0 ? 3 :  -1)
#define AF_EBI_WEn_PORT(i)          ((i) == 0 ? 5 :  -1)
#define AF_EBI_REn_PORT(i)          ((i) == 0 ? 5 :  -1)
#define AF_EBI_ARDY_PORT(i)         ((i) == 0 ? 5 :  -1)
#define AF_EBI_ALE_PORT(i)          ((i) == 0 ? 5 :  -1)
#define AF_TIMER0_CC0_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 5 : (i) == 3 ? 3 :  -1)
#define AF_TIMER0_CC1_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 5 : (i) == 3 ? 3 :  -1)
#define AF_TIMER0_CC2_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 5 : (i) == 3 ? 3 :  -1)
#define AF_TIMER0_CDTI0_PORT(i)     ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 5 : (i) == 3 ? 2 :  -1)
#define AF_TIMER0_CDTI1_PORT(i)     ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 5 : (i) == 3 ? 2 :  -1)
#define AF_TIMER0_CDTI2_PORT(i)     ((i) == 0 ? 0 : (i) == 1 ? 2 : (i) == 2 ? 5 : (i) == 3 ? 2 :  -1)
#define AF_TIMER1_CC0_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 1 :  -1)
#define AF_TIMER1_CC1_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 1 :  -1)
#define AF_TIMER1_CC2_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 1 :  -1)
#define AF_TIMER1_CDTI0_PORT(i)     (-1)
#define AF_TIMER1_CDTI1_PORT(i)     (-1)
#define AF_TIMER1_CDTI2_PORT(i)     (-1)
#define AF_TIMER2_CC0_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 2 :  -1)
#define AF_TIMER2_CC1_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 2 :  -1)
#define AF_TIMER2_CC2_PORT(i)       ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 2 :  -1)
#define AF_TIMER2_CDTI0_PORT(i)     (-1)
#define AF_TIMER2_CDTI1_PORT(i)     (-1)
#define AF_TIMER2_CDTI2_PORT(i)     (-1)
#define AF_USART0_TX_PORT(i)        ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 2 :  -1)
#define AF_USART0_RX_PORT(i)        ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 2 :  -1)
#define AF_USART0_CLK_PORT(i)       ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 2 :  -1)
#define AF_USART0_CS_PORT(i)        ((i) == 0 ? 4 : (i) == 1 ? 4 : (i) == 2 ? 2 :  -1)
#define AF_USART1_TX_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 3 :  -1)
#define AF_USART1_RX_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 3 :  -1)
#define AF_USART1_CLK_PORT(i)       ((i) == 0 ? 1 : (i) == 1 ? 3 :  -1)
#define AF_USART1_CS_PORT(i)        ((i) == 0 ? 1 : (i) == 1 ? 3 :  -1)
#define AF_USART2_TX_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 1 :  -1)
#define AF_USART2_RX_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 1 :  -1)
#define AF_USART2_CLK_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 1 :  -1)
#define AF_USART2_CS_PORT(i)        ((i) == 0 ? 2 : (i) == 1 ? 1 :  -1)
#define AF_UART0_TX_PORT(i)         ((i) == 0 ? 5 : (i) == 1 ? 4 : (i) == 2 ? 0 : (i) == 3 ? 2 :  -1)
#define AF_UART0_RX_PORT(i)         ((i) == 0 ? 5 : (i) == 1 ? 4 : (i) == 2 ? 0 : (i) == 3 ? 2 :  -1)
#define AF_UART0_CLK_PORT(i)        (-1)
#define AF_UART0_CS_PORT(i)         (-1)
#define AF_LEUART0_TX_PORT(i)       ((i) == 0 ? 3 : (i) == 1 ? 1 : (i) == 2 ? 4 :  -1)
#define AF_LEUART0_RX_PORT(i)       ((i) == 0 ? 3 : (i) == 1 ? 1 : (i) == 2 ? 4 :  -1)
#define AF_LEUART1_TX_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 0 :  -1)
#define AF_LEUART1_RX_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 0 :  -1)
#define AF_LETIMER0_OUT0_PORT(i)    ((i) == 0 ? 3 : (i) == 1 ? 1 : (i) == 2 ? 5 : (i) == 3 ? 2 :  -1)
#define AF_LETIMER0_OUT1_PORT(i)    ((i) == 0 ? 3 : (i) == 1 ? 1 : (i) == 2 ? 5 : (i) == 3 ? 2 :  -1)
#define AF_PCNT0_S0IN_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 2 :  -1)
#define AF_PCNT0_S1IN_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 4 : (i) == 2 ? 2 :  -1)
#define AF_PCNT1_S0IN_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 1 :  -1)
#define AF_PCNT1_S1IN_PORT(i)       ((i) == 0 ? 2 : (i) == 1 ? 1 :  -1)
#define AF_PCNT2_S0IN_PORT(i)       ((i) == 0 ? 3 : (i) == 1 ? 4 :  -1)
#define AF_PCNT2_S1IN_PORT(i)       ((i) == 0 ? 3 : (i) == 1 ? 4 :  -1)
#define AF_ACMP0_OUT_PORT(i)        ((i) == 0 ? 4 : (i) == 1 ? 4 :  -1)
#define AF_ACMP1_OUT_PORT(i)        ((i) == 0 ? 5 : (i) == 1 ? 4 :  -1)
#define AF_I2C0_SDA_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 2 : (i) == 3 ? 3 :  -1)
#define AF_I2C0_SCL_PORT(i)         ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 2 : (i) == 3 ? 3 :  -1)
#define AF_DBG_SWO_PORT(i)          ((i) == 0 ? 5 : (i) == 1 ? 2 :  -1)
#define AF_DBG_SWDIO_PORT(i)        ((i) == 0 ? 5 : (i) == 1 ? 5 :  -1)
#define AF_DBG_SWCLK_PORT(i)        ((i) == 0 ? 5 : (i) == 1 ? 5 :  -1)

/** @} End of group EFM32G_AF_Ports */


