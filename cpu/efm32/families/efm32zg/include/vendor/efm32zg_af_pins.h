/**************************************************************************//**
 * @file efm32zg_af_pins.h
 * @brief EFM32ZG_AF_PINS register and bit field definitions
 * @version 5.4.0
 ******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. www.silabs.com</b>
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

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__ICCARM__)
#pragma system_include       /* Treat file as system include file. */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang system_header  /* Treat file as system include file. */
#endif

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFM32ZG_AF_Pins
 * @{
 *****************************************************************************/

#define AF_CMU_CLK0_PIN(i)        ((i) == 0 ? 2 : (i) == 1 ? -1 : (i) == 2 ? 7 :  -1)                                                               /**< Pin number for AF_CMU_CLK0 location number i */
#define AF_CMU_CLK1_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? -1 : (i) == 2 ? 12 :  -1)                                                              /**< Pin number for AF_CMU_CLK1 location number i */
#define AF_TIMER0_CC0_PIN(i)      ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 0 : (i) == 5 ? 0 :  -1)                 /**< Pin number for AF_TIMER0_CC0 location number i */
#define AF_TIMER0_CC1_PIN(i)      ((i) == 0 ? 1 : (i) == 1 ? 1 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 0 : (i) == 5 ? 1 :  -1)                 /**< Pin number for AF_TIMER0_CC1 location number i */
#define AF_TIMER0_CC2_PIN(i)      ((i) == 0 ? 2 : (i) == 1 ? 2 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 1 : (i) == 5 ? 2 :  -1)                 /**< Pin number for AF_TIMER0_CC2 location number i */
#define AF_TIMER0_CDTI0_PIN(i)    (-1)                                                                                                              /**< Pin number for AF_TIMER0_CDTI0 location number i */
#define AF_TIMER0_CDTI1_PIN(i)    (-1)                                                                                                              /**< Pin number for AF_TIMER0_CDTI1 location number i */
#define AF_TIMER0_CDTI2_PIN(i)    (-1)                                                                                                              /**< Pin number for AF_TIMER0_CDTI2 location number i */
#define AF_TIMER1_CC0_PIN(i)      ((i) == 0 ? 13 : (i) == 1 ? 10 : (i) == 2 ? -1 : (i) == 3 ? 7 : (i) == 4 ? 6 :  -1)                               /**< Pin number for AF_TIMER1_CC0 location number i */
#define AF_TIMER1_CC1_PIN(i)      ((i) == 0 ? 14 : (i) == 1 ? 11 : (i) == 2 ? -1 : (i) == 3 ? 8 : (i) == 4 ? 7 :  -1)                               /**< Pin number for AF_TIMER1_CC1 location number i */
#define AF_TIMER1_CC2_PIN(i)      ((i) == 0 ? 15 : (i) == 1 ? 12 : (i) == 2 ? -1 : (i) == 3 ? 11 : (i) == 4 ? 13 :  -1)                             /**< Pin number for AF_TIMER1_CC2 location number i */
#define AF_TIMER1_CDTI0_PIN(i)    (-1)                                                                                                              /**< Pin number for AF_TIMER1_CDTI0 location number i */
#define AF_TIMER1_CDTI1_PIN(i)    (-1)                                                                                                              /**< Pin number for AF_TIMER1_CDTI1 location number i */
#define AF_TIMER1_CDTI2_PIN(i)    (-1)                                                                                                              /**< Pin number for AF_TIMER1_CDTI2 location number i */
#define AF_ACMP0_OUT_PIN(i)       ((i) == 0 ? 13 : (i) == 1 ? -1 : (i) == 2 ? 6 :  -1)                                                              /**< Pin number for AF_ACMP0_OUT location number i */
#define AF_USART1_TX_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? -1 : (i) == 2 ? 7 : (i) == 3 ? 7 :  -1)                                                /**< Pin number for AF_USART1_TX location number i */
#define AF_USART1_RX_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? -1 : (i) == 2 ? 6 : (i) == 3 ? 6 :  -1)                                                /**< Pin number for AF_USART1_RX location number i */
#define AF_USART1_CLK_PIN(i)      ((i) == 0 ? 7 : (i) == 1 ? -1 : (i) == 2 ? 0 : (i) == 3 ? 15 :  -1)                                               /**< Pin number for AF_USART1_CLK location number i */
#define AF_USART1_CS_PIN(i)       ((i) == 0 ? 8 : (i) == 1 ? -1 : (i) == 2 ? 1 : (i) == 3 ? 14 :  -1)                                               /**< Pin number for AF_USART1_CS location number i */
#define AF_PRS_CH0_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 14 :  -1)                                                               /**< Pin number for AF_PRS_CH0 location number i */
#define AF_PRS_CH1_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? 4 : (i) == 2 ? 15 :  -1)                                                               /**< Pin number for AF_PRS_CH1 location number i */
#define AF_PRS_CH2_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 5 : (i) == 2 ? 10 :  -1)                                                               /**< Pin number for AF_PRS_CH2 location number i */
#define AF_PRS_CH3_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? -1 : (i) == 2 ? 11 :  -1)                                                              /**< Pin number for AF_PRS_CH3 location number i */
#define AF_LEUART0_TX_PIN(i)      ((i) == 0 ? 4 : (i) == 1 ? 13 : (i) == 2 ? -1 : (i) == 3 ? 0 : (i) == 4 ? 2 :  -1)                                /**< Pin number for AF_LEUART0_TX location number i */
#define AF_LEUART0_RX_PIN(i)      ((i) == 0 ? 5 : (i) == 1 ? 14 : (i) == 2 ? -1 : (i) == 3 ? 1 : (i) == 4 ? 0 :  -1)                                /**< Pin number for AF_LEUART0_RX location number i */
#define AF_PCNT0_S0IN_PIN(i)      ((i) == 0 ? 13 : (i) == 1 ? -1 : (i) == 2 ? 0 : (i) == 3 ? 6 :  -1)                                               /**< Pin number for AF_PCNT0_S0IN location number i */
#define AF_PCNT0_S1IN_PIN(i)      ((i) == 0 ? 14 : (i) == 1 ? -1 : (i) == 2 ? 1 : (i) == 3 ? 7 :  -1)                                               /**< Pin number for AF_PCNT0_S1IN location number i */
#define AF_I2C0_SDA_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? 6 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 12 :  -1) /**< Pin number for AF_I2C0_SDA location number i */
#define AF_I2C0_SCL_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 7 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 13 :  -1) /**< Pin number for AF_I2C0_SCL location number i */
#define AF_DBG_SWDIO_PIN(i)       ((i) == 0 ? 1 :  -1)                                                                                              /**< Pin number for AF_DBG_SWDIO location number i */
#define AF_DBG_SWCLK_PIN(i)       ((i) == 0 ? 0 :  -1)                                                                                              /**< Pin number for AF_DBG_SWCLK location number i */

/** @} End of group EFM32ZG_AF_Pins */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

