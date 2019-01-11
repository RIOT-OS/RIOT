/***************************************************************************//**
 * @file
 * @brief EFM32PG12B_PRS_SIGNALS register and bit field definitions
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
 * @addtogroup EFM32PG12B_PRS
 * @{
 * @addtogroup EFM32PG12B_PRS_Signals PRS Signals
 * @{
 * @brief PRS Signal names
 ******************************************************************************/
#define PRS_PRS_CH0                 ((1 << 8) + 0)  /**< PRS PRS channel 0 */
#define PRS_PRS_CH1                 ((1 << 8) + 1)  /**< PRS PRS channel 1 */
#define PRS_PRS_CH2                 ((1 << 8) + 2)  /**< PRS PRS channel 2 */
#define PRS_PRS_CH3                 ((1 << 8) + 3)  /**< PRS PRS channel 3 */
#define PRS_PRS_CH4                 ((1 << 8) + 4)  /**< PRS PRS channel 4 */
#define PRS_PRS_CH5                 ((1 << 8) + 5)  /**< PRS PRS channel 5 */
#define PRS_PRS_CH6                 ((1 << 8) + 6)  /**< PRS PRS channel 6 */
#define PRS_PRS_CH7                 ((1 << 8) + 7)  /**< PRS PRS channel 7 */
#define PRS_PRS_CH8                 ((2 << 8) + 0)  /**< PRS PRS channel 8 */
#define PRS_PRS_CH9                 ((2 << 8) + 1)  /**< PRS PRS channel 9 */
#define PRS_PRS_CH10                ((2 << 8) + 2)  /**< PRS PRS channel 10 */
#define PRS_PRS_CH11                ((2 << 8) + 3)  /**< PRS PRS channel 11 */
#define PRS_ACMP0_OUT               ((3 << 8) + 0)  /**< PRS Analog comparator output */
#define PRS_ACMP1_OUT               ((4 << 8) + 0)  /**< PRS Analog comparator output */
#define PRS_ADC0_SINGLE             ((5 << 8) + 0)  /**< PRS ADC single conversion done */
#define PRS_ADC0_SCAN               ((5 << 8) + 1)  /**< PRS ADC scan conversion done */
#define PRS_LESENSE_SCANRES0        ((7 << 8) + 0)  /**< PRS LESENSE SCANRES register, bit 0 */
#define PRS_LESENSE_SCANRES1        ((7 << 8) + 1)  /**< PRS LESENSE SCANRES register, bit 1 */
#define PRS_LESENSE_SCANRES2        ((7 << 8) + 2)  /**< PRS LESENSE SCANRES register, bit 2 */
#define PRS_LESENSE_SCANRES3        ((7 << 8) + 3)  /**< PRS LESENSE SCANRES register, bit 3 */
#define PRS_LESENSE_SCANRES4        ((7 << 8) + 4)  /**< PRS LESENSE SCANRES register, bit 4 */
#define PRS_LESENSE_SCANRES5        ((7 << 8) + 5)  /**< PRS LESENSE SCANRES register, bit 5 */
#define PRS_LESENSE_SCANRES6        ((7 << 8) + 6)  /**< PRS LESENSE SCANRES register, bit 6 */
#define PRS_LESENSE_SCANRES7        ((7 << 8) + 7)  /**< PRS LESENSE SCANRES register, bit 7 */
#define PRS_LESENSE_SCANRES8        ((8 << 8) + 0)  /**< PRS LESENSE SCANRES register, bit 8 */
#define PRS_LESENSE_SCANRES9        ((8 << 8) + 1)  /**< PRS LESENSE SCANRES register, bit 9 */
#define PRS_LESENSE_SCANRES10       ((8 << 8) + 2)  /**< PRS LESENSE SCANRES register, bit 10 */
#define PRS_LESENSE_SCANRES11       ((8 << 8) + 3)  /**< PRS LESENSE SCANRES register, bit 11 */
#define PRS_LESENSE_SCANRES12       ((8 << 8) + 4)  /**< PRS LESENSE SCANRES register, bit 12 */
#define PRS_LESENSE_SCANRES13       ((8 << 8) + 5)  /**< PRS LESENSE SCANRES register, bit 13 */
#define PRS_LESENSE_SCANRES14       ((8 << 8) + 6)  /**< PRS LESENSE SCANRES register, bit 14 */
#define PRS_LESENSE_SCANRES15       ((8 << 8) + 7)  /**< PRS LESENSE SCANRES register, bit 15 */
#define PRS_LESENSE_DEC0            ((9 << 8) + 0)  /**< PRS LESENSE Decoder PRS out 0 */
#define PRS_LESENSE_DEC1            ((9 << 8) + 1)  /**< PRS LESENSE Decoder PRS out 1 */
#define PRS_LESENSE_DEC2            ((9 << 8) + 2)  /**< PRS LESENSE Decoder PRS out 2 */
#define PRS_LESENSE_DECCMP          ((9 << 8) + 3)  /**< PRS LESENSE Decoder PRS compare value match channel */
#define PRS_LESENSE_MEASACT         ((10 << 8) + 0) /**< PRS LESENSE Measurement active */
#define PRS_RTCC_CCV0               ((11 << 8) + 1) /**< PRS RTCC Compare 0 */
#define PRS_RTCC_CCV1               ((11 << 8) + 2) /**< PRS RTCC Compare 1 */
#define PRS_RTCC_CCV2               ((11 << 8) + 3) /**< PRS RTCC Compare 2 */
#define PRS_GPIO_PIN0               ((12 << 8) + 0) /**< PRS GPIO pin 0 */
#define PRS_GPIO_PIN1               ((12 << 8) + 1) /**< PRS GPIO pin 1 */
#define PRS_GPIO_PIN2               ((12 << 8) + 2) /**< PRS GPIO pin 2 */
#define PRS_GPIO_PIN3               ((12 << 8) + 3) /**< PRS GPIO pin 3 */
#define PRS_GPIO_PIN4               ((12 << 8) + 4) /**< PRS GPIO pin 4 */
#define PRS_GPIO_PIN5               ((12 << 8) + 5) /**< PRS GPIO pin 5 */
#define PRS_GPIO_PIN6               ((12 << 8) + 6) /**< PRS GPIO pin 6 */
#define PRS_GPIO_PIN7               ((12 << 8) + 7) /**< PRS GPIO pin 7 */
#define PRS_GPIO_PIN8               ((13 << 8) + 0) /**< PRS GPIO pin 8 */
#define PRS_GPIO_PIN9               ((13 << 8) + 1) /**< PRS GPIO pin 9 */
#define PRS_GPIO_PIN10              ((13 << 8) + 2) /**< PRS GPIO pin 10 */
#define PRS_GPIO_PIN11              ((13 << 8) + 3) /**< PRS GPIO pin 11 */
#define PRS_GPIO_PIN12              ((13 << 8) + 4) /**< PRS GPIO pin 12 */
#define PRS_GPIO_PIN13              ((13 << 8) + 5) /**< PRS GPIO pin 13 */
#define PRS_GPIO_PIN14              ((13 << 8) + 6) /**< PRS GPIO pin 14 */
#define PRS_GPIO_PIN15              ((13 << 8) + 7) /**< PRS GPIO pin 15 */
#define PRS_LETIMER0_CH0            ((14 << 8) + 0) /**< PRS LETIMER CH0 Out */
#define PRS_LETIMER0_CH1            ((14 << 8) + 1) /**< PRS LETIMER CH1 Out */
#define PRS_PCNT0_TCC               ((15 << 8) + 0) /**< PRS PCNT0 Triggered compare match */
#define PRS_PCNT0_UFOF              ((15 << 8) + 1) /**< PRS PCNT0 Counter overflow or underflow */
#define PRS_PCNT0_DIR               ((15 << 8) + 2) /**< PRS PCNT0 Counter direction */
#define PRS_PCNT1_TCC               ((16 << 8) + 0) /**< PRS PCNT1 Triggered compare match */
#define PRS_PCNT1_UFOF              ((16 << 8) + 1) /**< PRS PCNT1 Counter overflow or underflow */
#define PRS_PCNT1_DIR               ((16 << 8) + 2) /**< PRS PCNT1 Counter direction */
#define PRS_PCNT2_TCC               ((17 << 8) + 0) /**< PRS PCNT2 Triggered compare match */
#define PRS_PCNT2_UFOF              ((17 << 8) + 1) /**< PRS PCNT2 Counter overflow or underflow */
#define PRS_PCNT2_DIR               ((17 << 8) + 2) /**< PRS PCNT2 Counter direction */
#define PRS_CMU_CLKOUT0             ((18 << 8) + 0) /**< PRS Clock Output 0 */
#define PRS_CMU_CLKOUT1             ((18 << 8) + 1) /**< PRS Clock Output 1 */
#define PRS_VDAC0_CH0               ((24 << 8) + 0) /**< PRS DAC ch0 conversion done */
#define PRS_VDAC0_CH1               ((24 << 8) + 1) /**< PRS DAC ch1 conversion done */
#define PRS_VDAC0_OPA0              ((24 << 8) + 2) /**< PRS OPA0 warmedup or outputvalid based on OPA0PRSOUTMODE mode in OPACTRL. */
#define PRS_VDAC0_OPA1              ((24 << 8) + 3) /**< PRS OPA1 warmedup or outputvalid based on OPA1PRSOUTMODE mode in OPACTRL. */
#define PRS_VDAC0_OPA2              ((24 << 8) + 4) /**< PRS OPA2 warmedup or outputvalid based on OPA2PRSOUTMODE mode in OPACTRL. */
#define PRS_RFSENSE_WU              ((25 << 8) + 0) /**< PRS RFSENSE Output */
#define PRS_CRYOTIMER_PERIOD        ((26 << 8) + 0) /**< PRS CRYOTIMER Output */
#define PRS_USART0_IRTX             ((48 << 8) + 0) /**< PRS USART 0 IRDA out */
#define PRS_USART0_TXC              ((48 << 8) + 1) /**< PRS USART 0 TX complete */
#define PRS_USART0_RXDATAV          ((48 << 8) + 2) /**< PRS USART 0 RX Data Valid */
#define PRS_USART0_RTS              ((48 << 8) + 3) /**< PRS USART 0 RTS */
#define PRS_USART0_TX               ((48 << 8) + 5) /**< PRS USART 0 TX */
#define PRS_USART0_CS               ((48 << 8) + 6) /**< PRS USART 0 CS */
#define PRS_USART1_TXC              ((49 << 8) + 1) /**< PRS USART 1 TX complete */
#define PRS_USART1_RXDATAV          ((49 << 8) + 2) /**< PRS USART 1 RX Data Valid */
#define PRS_USART1_RTS              ((49 << 8) + 3) /**< PRS USART 1 RTS */
#define PRS_USART1_TX               ((49 << 8) + 5) /**< PRS USART 1 TX */
#define PRS_USART1_CS               ((49 << 8) + 6) /**< PRS USART 1 CS */
#define PRS_USART2_IRTX             ((50 << 8) + 0) /**< PRS USART 2 IRDA out */
#define PRS_USART2_TXC              ((50 << 8) + 1) /**< PRS USART 2 TX complete */
#define PRS_USART2_RXDATAV          ((50 << 8) + 2) /**< PRS USART 2 RX Data Valid */
#define PRS_USART2_RTS              ((50 << 8) + 3) /**< PRS USART 2 RTS */
#define PRS_USART2_TX               ((50 << 8) + 5) /**< PRS USART 2 TX */
#define PRS_USART2_CS               ((50 << 8) + 6) /**< PRS USART 2 CS */
#define PRS_USART3_TXC              ((51 << 8) + 1) /**< PRS USART 3 TX complete */
#define PRS_USART3_RXDATAV          ((51 << 8) + 2) /**< PRS USART 3 RX Data Valid */
#define PRS_USART3_RTS              ((51 << 8) + 3) /**< PRS USART 3 RTS */
#define PRS_USART3_TX               ((51 << 8) + 5) /**< PRS USART 3 TX */
#define PRS_USART3_CS               ((51 << 8) + 6) /**< PRS USART 3 CS */
#define PRS_TIMER0_UF               ((60 << 8) + 0) /**< PRS Timer 0 Underflow */
#define PRS_TIMER0_OF               ((60 << 8) + 1) /**< PRS Timer 0 Overflow */
#define PRS_TIMER0_CC0              ((60 << 8) + 2) /**< PRS Timer 0 Compare/Capture 0 */
#define PRS_TIMER0_CC1              ((60 << 8) + 3) /**< PRS Timer 0 Compare/Capture 1 */
#define PRS_TIMER0_CC2              ((60 << 8) + 4) /**< PRS Timer 0 Compare/Capture 2 */
#define PRS_TIMER1_UF               ((61 << 8) + 0) /**< PRS Timer 1 Underflow */
#define PRS_TIMER1_OF               ((61 << 8) + 1) /**< PRS Timer 1 Overflow */
#define PRS_TIMER1_CC0              ((61 << 8) + 2) /**< PRS Timer 1 Compare/Capture 0 */
#define PRS_TIMER1_CC1              ((61 << 8) + 3) /**< PRS Timer 1 Compare/Capture 1 */
#define PRS_TIMER1_CC2              ((61 << 8) + 4) /**< PRS Timer 1 Compare/Capture 2 */
#define PRS_TIMER1_CC3              ((61 << 8) + 5) /**< PRS Timer 1 Compare/Capture 3 */
#define PRS_WTIMER0_UF              ((62 << 8) + 0) /**< PRS Timer 2 Underflow */
#define PRS_WTIMER0_OF              ((62 << 8) + 1) /**< PRS Timer 2 Overflow */
#define PRS_WTIMER0_CC0             ((62 << 8) + 2) /**< PRS Timer 2 Compare/Capture 0 */
#define PRS_WTIMER0_CC1             ((62 << 8) + 3) /**< PRS Timer 2 Compare/Capture 1 */
#define PRS_WTIMER0_CC2             ((62 << 8) + 4) /**< PRS Timer 2 Compare/Capture 2 */
#define PRS_WTIMER1_UF              ((63 << 8) + 0) /**< PRS Timer 3 Underflow */
#define PRS_WTIMER1_OF              ((63 << 8) + 1) /**< PRS Timer 3 Overflow */
#define PRS_WTIMER1_CC0             ((63 << 8) + 2) /**< PRS Timer 3 Compare/Capture 0 */
#define PRS_WTIMER1_CC1             ((63 << 8) + 3) /**< PRS Timer 3 Compare/Capture 1 */
#define PRS_WTIMER1_CC2             ((63 << 8) + 4) /**< PRS Timer 3 Compare/Capture 2 */
#define PRS_WTIMER1_CC3             ((63 << 8) + 5) /**< PRS Timer 3 Compare/Capture 3 */
#define PRS_CM4_TXEV                ((67 << 8) + 0) /**< PRS  */
#define PRS_CM4_ICACHEPCHITSOF      ((67 << 8) + 1) /**< PRS  */
#define PRS_CM4_ICACHEPCMISSESOF    ((67 << 8) + 2) /**< PRS  */
#define PRS_RAC_ACTIVE              ((81 << 8) + 0) /**< PRS RAC is active */
#define PRS_RAC_TX                  ((81 << 8) + 1) /**< PRS RAC is in TX */
#define PRS_RAC_RX                  ((81 << 8) + 2) /**< PRS RAC is in RX */
#define PRS_RAC_LNAEN               ((81 << 8) + 3) /**< PRS LNA enable */
#define PRS_RAC_PAEN                ((81 << 8) + 4) /**< PRS PA enable */
#define PRS_PROTIMER_LBTS           ((84 << 8) + 5) /**< PRS Listen Before Talk Success */
#define PRS_PROTIMER_LBTR           ((84 << 8) + 6) /**< PRS Listen Before Talk Retry */
#define PRS_PROTIMER_LBTF           ((84 << 8) + 7) /**< PRS Listen Before Talk Failure */
#define PRS_MODEM_FRAMEDET          ((86 << 8) + 0) /**< PRS Frame detected */
#define PRS_MODEM_PREDET            ((86 << 8) + 1) /**< PRS Receive preamble detected */
#define PRS_MODEM_TIMDET            ((86 << 8) + 2) /**< PRS Receive timing detected */
#define PRS_MODEM_FRAMESENT         ((86 << 8) + 3) /**< PRS Entire frame transmitted */
#define PRS_MODEM_SYNCSENT          ((86 << 8) + 4) /**< PRS Syncword transmitted */
#define PRS_MODEM_PRESENT           ((86 << 8) + 5) /**< PRS Preamble transmitted */
#define PRS_MODEM_ANT0              ((87 << 8) + 5) /**< PRS Antenna 0 select */
#define PRS_MODEM_ANT1              ((87 << 8) + 6) /**< PRS Antenna 1 select */

/** @} */
/** @} End of group EFM32PG12B_PRS */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

