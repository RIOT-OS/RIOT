/***************************************************************************//**
 * @file
 * @brief EFR32MG1P_PRS_SIGNALS register and bit field definitions
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
 * @addtogroup EFR32MG1P_PRS
 * @{
 * @addtogroup EFR32MG1P_PRS_Signals PRS Signals
 * @{
 * @brief PRS Signal names
 ******************************************************************************/
#define PRS_PRS_CH0             ((1 << 8) + 0)  /**< PRS PRS channel 0 */
#define PRS_PRS_CH1             ((1 << 8) + 1)  /**< PRS PRS channel 1 */
#define PRS_PRS_CH2             ((1 << 8) + 2)  /**< PRS PRS channel 2 */
#define PRS_PRS_CH3             ((1 << 8) + 3)  /**< PRS PRS channel 3 */
#define PRS_PRS_CH4             ((1 << 8) + 4)  /**< PRS PRS channel 4 */
#define PRS_PRS_CH5             ((1 << 8) + 5)  /**< PRS PRS channel 5 */
#define PRS_PRS_CH6             ((1 << 8) + 6)  /**< PRS PRS channel 6 */
#define PRS_PRS_CH7             ((1 << 8) + 7)  /**< PRS PRS channel 7 */
#define PRS_PRS_CH8             ((2 << 8) + 0)  /**< PRS PRS channel 8 */
#define PRS_PRS_CH9             ((2 << 8) + 1)  /**< PRS PRS channel 9 */
#define PRS_PRS_CH10            ((2 << 8) + 2)  /**< PRS PRS channel 10 */
#define PRS_PRS_CH11            ((2 << 8) + 3)  /**< PRS PRS channel 11 */
#define PRS_ACMP0_OUT           ((6 << 8) + 0)  /**< PRS Analog comparator output */
#define PRS_ACMP1_OUT           ((7 << 8) + 0)  /**< PRS Analog comparator output */
#define PRS_ADC0_SINGLE         ((8 << 8) + 0)  /**< PRS ADC single conversion done */
#define PRS_ADC0_SCAN           ((8 << 8) + 1)  /**< PRS ADC scan conversion done */
#define PRS_USART0_IRTX         ((16 << 8) + 0) /**< PRS USART 0 IRDA out */
#define PRS_USART0_TXC          ((16 << 8) + 1) /**< PRS USART 0 TX complete */
#define PRS_USART0_RXDATAV      ((16 << 8) + 2) /**< PRS USART 0 RX Data Valid */
#define PRS_USART0_RTS          ((16 << 8) + 3) /**< PRS USART 0 RTS */
#define PRS_USART0_TX           ((16 << 8) + 5) /**< PRS USART 0 TX */
#define PRS_USART0_CS           ((16 << 8) + 6) /**< PRS USART 0 CS */
#define PRS_USART1_TXC          ((17 << 8) + 1) /**< PRS USART 1 TX complete */
#define PRS_USART1_RXDATAV      ((17 << 8) + 2) /**< PRS USART 1 RX Data Valid */
#define PRS_USART1_RTS          ((17 << 8) + 3) /**< PRS USART 0 RTS */
#define PRS_USART1_TX           ((17 << 8) + 5) /**< PRS USART 1 TX */
#define PRS_USART1_CS           ((17 << 8) + 6) /**< PRS USART 1 CS */
#define PRS_TIMER0_UF           ((28 << 8) + 0) /**< PRS Timer 0 Underflow */
#define PRS_TIMER0_OF           ((28 << 8) + 1) /**< PRS Timer 0 Overflow */
#define PRS_TIMER0_CC0          ((28 << 8) + 2) /**< PRS Timer 0 Compare/Capture 0 */
#define PRS_TIMER0_CC1          ((28 << 8) + 3) /**< PRS Timer 0 Compare/Capture 1 */
#define PRS_TIMER0_CC2          ((28 << 8) + 4) /**< PRS Timer 0 Compare/Capture 2 */
#define PRS_TIMER1_UF           ((29 << 8) + 0) /**< PRS Timer 1 Underflow */
#define PRS_TIMER1_OF           ((29 << 8) + 1) /**< PRS Timer 1 Overflow */
#define PRS_TIMER1_CC0          ((29 << 8) + 2) /**< PRS Timer 1 Compare/Capture 0 */
#define PRS_TIMER1_CC1          ((29 << 8) + 3) /**< PRS Timer 1 Compare/Capture 1 */
#define PRS_TIMER1_CC2          ((29 << 8) + 4) /**< PRS Timer 1 Compare/Capture 2 */
#define PRS_TIMER1_CC3          ((29 << 8) + 5) /**< PRS Timer 1 Compare/Capture 3 */
#define PRS_RAC_ACTIVE          ((32 << 8) + 0) /**< PRS RAC is active */
#define PRS_RAC_TX              ((32 << 8) + 1) /**< PRS RAC is in TX */
#define PRS_RAC_RX              ((32 << 8) + 2) /**< PRS RAC is in RX */
#define PRS_RAC_LNAEN           ((32 << 8) + 3) /**< PRS LNA enable */
#define PRS_RAC_PAEN            ((32 << 8) + 4) /**< PRS PA enable */
#define PRS_PROTIMER_LBTS       ((35 << 8) + 5) /**< PRS Listen Before Talk Success */
#define PRS_PROTIMER_LBTR       ((35 << 8) + 6) /**< PRS Listen Before Talk Retry */
#define PRS_PROTIMER_LBTF       ((35 << 8) + 7) /**< PRS Listen Before Talk Failure */
#define PRS_MODEM_FRAMEDET      ((38 << 8) + 0) /**< PRS Frame detected */
#define PRS_MODEM_PREDET        ((38 << 8) + 1) /**< PRS Receive preamble detected */
#define PRS_MODEM_TIMDET        ((38 << 8) + 2) /**< PRS Receive timing detected */
#define PRS_MODEM_FRAMESENT     ((38 << 8) + 3) /**< PRS Entire frame transmitted */
#define PRS_MODEM_SYNCSENT      ((38 << 8) + 4) /**< PRS Syncword transmitted */
#define PRS_MODEM_PRESENT       ((38 << 8) + 5) /**< PRS Preamble transmitted */
#define PRS_MODEM_ANT0          ((39 << 8) + 5) /**< PRS Antenna 0 select */
#define PRS_MODEM_ANT1          ((39 << 8) + 6) /**< PRS Antenna 1 select */
#define PRS_RTCC_CCV0           ((41 << 8) + 1) /**< PRS RTCC Compare 0 */
#define PRS_RTCC_CCV1           ((41 << 8) + 2) /**< PRS RTCC Compare 1 */
#define PRS_RTCC_CCV2           ((41 << 8) + 3) /**< PRS RTCC Compare 2 */
#define PRS_GPIO_PIN0           ((48 << 8) + 0) /**< PRS GPIO pin 0 */
#define PRS_GPIO_PIN1           ((48 << 8) + 1) /**< PRS GPIO pin 1 */
#define PRS_GPIO_PIN2           ((48 << 8) + 2) /**< PRS GPIO pin 2 */
#define PRS_GPIO_PIN3           ((48 << 8) + 3) /**< PRS GPIO pin 3 */
#define PRS_GPIO_PIN4           ((48 << 8) + 4) /**< PRS GPIO pin 4 */
#define PRS_GPIO_PIN5           ((48 << 8) + 5) /**< PRS GPIO pin 5 */
#define PRS_GPIO_PIN6           ((48 << 8) + 6) /**< PRS GPIO pin 6 */
#define PRS_GPIO_PIN7           ((48 << 8) + 7) /**< PRS GPIO pin 7 */
#define PRS_GPIO_PIN8           ((49 << 8) + 0) /**< PRS GPIO pin 8 */
#define PRS_GPIO_PIN9           ((49 << 8) + 1) /**< PRS GPIO pin 9 */
#define PRS_GPIO_PIN10          ((49 << 8) + 2) /**< PRS GPIO pin 10 */
#define PRS_GPIO_PIN11          ((49 << 8) + 3) /**< PRS GPIO pin 11 */
#define PRS_GPIO_PIN12          ((49 << 8) + 4) /**< PRS GPIO pin 12 */
#define PRS_GPIO_PIN13          ((49 << 8) + 5) /**< PRS GPIO pin 13 */
#define PRS_GPIO_PIN14          ((49 << 8) + 6) /**< PRS GPIO pin 14 */
#define PRS_GPIO_PIN15          ((49 << 8) + 7) /**< PRS GPIO pin 15 */
#define PRS_LETIMER0_CH0        ((52 << 8) + 0) /**< PRS LETIMER CH0 Out */
#define PRS_LETIMER0_CH1        ((52 << 8) + 1) /**< PRS LETIMER CH1 Out */
#define PRS_PCNT0_TCC           ((54 << 8) + 0) /**< PRS Triggered compare match */
#define PRS_PCNT0_UFOF          ((54 << 8) + 1) /**< PRS Counter overflow or underflow */
#define PRS_PCNT0_DIR           ((54 << 8) + 2) /**< PRS Counter direction */
#define PRS_RFSENSE_WU          ((59 << 8) + 0) /**< PRS RFSENSE Output */
#define PRS_CRYOTIMER_PERIOD    ((60 << 8) + 0) /**< PRS CRYOTIMER Output */
#define PRS_CMU_CLKOUT0         ((61 << 8) + 0) /**< PRS Clock Output 0 */
#define PRS_CMU_CLKOUT1         ((61 << 8) + 1) /**< PRS Clock Output 1 */
#define PRS_CM4_TXEV            ((67 << 8) + 0) /**< PRS  */

/** @} */
/** @} End of group EFR32MG1P_PRS */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

