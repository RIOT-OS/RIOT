/***************************************************************************//**
 * @file
 * @brief EFM32GG11B_PRS_SIGNALS register and bit field definitions
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
 * @addtogroup EFM32GG11B_PRS
 * @{
 * @addtogroup EFM32GG11B_PRS_Signals PRS Signals
 * @{
 * @brief PRS Signal names
 ******************************************************************************/
#define PRS_PRS_CH0                 ((1 << 8) + 0)   /**< PRS PRS channel 0 */
#define PRS_PRS_CH1                 ((1 << 8) + 1)   /**< PRS PRS channel 1 */
#define PRS_PRS_CH2                 ((1 << 8) + 2)   /**< PRS PRS channel 2 */
#define PRS_PRS_CH3                 ((1 << 8) + 3)   /**< PRS PRS channel 3 */
#define PRS_PRS_CH4                 ((1 << 8) + 4)   /**< PRS PRS channel 4 */
#define PRS_PRS_CH5                 ((1 << 8) + 5)   /**< PRS PRS channel 5 */
#define PRS_PRS_CH6                 ((1 << 8) + 6)   /**< PRS PRS channel 6 */
#define PRS_PRS_CH7                 ((1 << 8) + 7)   /**< PRS PRS channel 7 */
#define PRS_PRS_CH8                 ((2 << 8) + 0)   /**< PRS PRS channel 8 */
#define PRS_PRS_CH9                 ((2 << 8) + 1)   /**< PRS PRS channel 9 */
#define PRS_PRS_CH10                ((2 << 8) + 2)   /**< PRS PRS channel 10 */
#define PRS_PRS_CH11                ((2 << 8) + 3)   /**< PRS PRS channel 11 */
#define PRS_PRS_CH12                ((2 << 8) + 4)   /**< PRS PRS channel 12 */
#define PRS_PRS_CH13                ((2 << 8) + 5)   /**< PRS PRS channel 13 */
#define PRS_PRS_CH14                ((2 << 8) + 6)   /**< PRS PRS channel 14 */
#define PRS_PRS_CH15                ((2 << 8) + 7)   /**< PRS PRS channel 15 */
#define PRS_PRS_CH16                ((3 << 8) + 0)   /**< PRS PRS channel 16 */
#define PRS_PRS_CH17                ((3 << 8) + 1)   /**< PRS PRS channel 17 */
#define PRS_PRS_CH18                ((3 << 8) + 2)   /**< PRS PRS channel 18 */
#define PRS_PRS_CH19                ((3 << 8) + 3)   /**< PRS PRS channel 19 */
#define PRS_PRS_CH20                ((3 << 8) + 4)   /**< PRS PRS channel 20 */
#define PRS_PRS_CH21                ((3 << 8) + 5)   /**< PRS PRS channel 21 */
#define PRS_PRS_CH22                ((3 << 8) + 6)   /**< PRS PRS channel 22 */
#define PRS_PRS_CH23                ((3 << 8) + 7)   /**< PRS PRS channel 23 */
#define PRS_ACMP0_OUT               ((4 << 8) + 0)   /**< PRS Analog comparator output */
#define PRS_ACMP1_OUT               ((5 << 8) + 0)   /**< PRS Analog comparator output */
#define PRS_ADC0_SINGLE             ((6 << 8) + 0)   /**< PRS ADC single conversion done */
#define PRS_ADC0_SCAN               ((6 << 8) + 1)   /**< PRS ADC scan conversion done */
#define PRS_RTC_OF                  ((7 << 8) + 0)   /**< PRS RTC Overflow */
#define PRS_RTC_COMP0               ((7 << 8) + 1)   /**< PRS RTC Compare 0 */
#define PRS_RTC_COMP1               ((7 << 8) + 2)   /**< PRS RTC Compare 1 */
#define PRS_RTC_COMP2               ((7 << 8) + 3)   /**< PRS RTC Compare 2 */
#define PRS_RTC_COMP3               ((7 << 8) + 4)   /**< PRS RTC Compare 3 */
#define PRS_RTC_COMP4               ((7 << 8) + 5)   /**< PRS RTC Compare 4 */
#define PRS_RTC_COMP5               ((7 << 8) + 6)   /**< PRS RTC Compare 5 */
#define PRS_RTCC_CCV0               ((8 << 8) + 1)   /**< PRS RTCC Compare 0 */
#define PRS_RTCC_CCV1               ((8 << 8) + 2)   /**< PRS RTCC Compare 1 */
#define PRS_RTCC_CCV2               ((8 << 8) + 3)   /**< PRS RTCC Compare 2 */
#define PRS_GPIO_PIN0               ((9 << 8) + 0)   /**< PRS GPIO pin 0 */
#define PRS_GPIO_PIN1               ((9 << 8) + 1)   /**< PRS GPIO pin 1 */
#define PRS_GPIO_PIN2               ((9 << 8) + 2)   /**< PRS GPIO pin 2 */
#define PRS_GPIO_PIN3               ((9 << 8) + 3)   /**< PRS GPIO pin 3 */
#define PRS_GPIO_PIN4               ((9 << 8) + 4)   /**< PRS GPIO pin 4 */
#define PRS_GPIO_PIN5               ((9 << 8) + 5)   /**< PRS GPIO pin 5 */
#define PRS_GPIO_PIN6               ((9 << 8) + 6)   /**< PRS GPIO pin 6 */
#define PRS_GPIO_PIN7               ((9 << 8) + 7)   /**< PRS GPIO pin 7 */
#define PRS_GPIO_PIN8               ((10 << 8) + 0)  /**< PRS GPIO pin 8 */
#define PRS_GPIO_PIN9               ((10 << 8) + 1)  /**< PRS GPIO pin 9 */
#define PRS_GPIO_PIN10              ((10 << 8) + 2)  /**< PRS GPIO pin 10 */
#define PRS_GPIO_PIN11              ((10 << 8) + 3)  /**< PRS GPIO pin 11 */
#define PRS_GPIO_PIN12              ((10 << 8) + 4)  /**< PRS GPIO pin 12 */
#define PRS_GPIO_PIN13              ((10 << 8) + 5)  /**< PRS GPIO pin 13 */
#define PRS_GPIO_PIN14              ((10 << 8) + 6)  /**< PRS GPIO pin 14 */
#define PRS_GPIO_PIN15              ((10 << 8) + 7)  /**< PRS GPIO pin 15 */
#define PRS_LETIMER0_CH0            ((11 << 8) + 0)  /**< PRS LETIMER CH0 Out */
#define PRS_LETIMER0_CH1            ((11 << 8) + 1)  /**< PRS LETIMER CH1 Out */
#define PRS_LETIMER1_CH0            ((12 << 8) + 0)  /**< PRS LETIMER CH0 Out */
#define PRS_LETIMER1_CH1            ((12 << 8) + 1)  /**< PRS LETIMER CH1 Out */
#define PRS_PCNT0_TCC               ((13 << 8) + 0)  /**< PRS Triggered compare match */
#define PRS_PCNT0_UFOF              ((13 << 8) + 1)  /**< PRS Counter overflow or underflow */
#define PRS_PCNT0_DIR               ((13 << 8) + 2)  /**< PRS Counter direction */
#define PRS_PCNT1_TCC               ((14 << 8) + 0)  /**< PRS Triggered compare match */
#define PRS_PCNT1_UFOF              ((14 << 8) + 1)  /**< PRS Counter overflow or underflow */
#define PRS_PCNT1_DIR               ((14 << 8) + 2)  /**< PRS Counter direction */
#define PRS_PCNT2_TCC               ((15 << 8) + 0)  /**< PRS Triggered compare match */
#define PRS_PCNT2_UFOF              ((15 << 8) + 1)  /**< PRS Counter overflow or underflow */
#define PRS_PCNT2_DIR               ((15 << 8) + 2)  /**< PRS Counter direction */
#define PRS_CRYOTIMER_PERIOD        ((16 << 8) + 0)  /**< PRS CRYOTIMER Output */
#define PRS_CMU_CLKOUT0             ((17 << 8) + 0)  /**< PRS Clock Output 0 */
#define PRS_CMU_CLKOUT1             ((17 << 8) + 1)  /**< PRS Clock Output 1 */
#define PRS_CMU_CLKOUT2             ((17 << 8) + 7)  /**< PRS Clock Output 2 */
#define PRS_VDAC0_CH0               ((23 << 8) + 0)  /**< PRS DAC ch0 conversion done */
#define PRS_VDAC0_CH1               ((23 << 8) + 1)  /**< PRS DAC ch1 conversion done */
#define PRS_VDAC0_OPA0              ((23 << 8) + 2)  /**< PRS OPA0 warmed up. output is valid. */
#define PRS_VDAC0_OPA1              ((23 << 8) + 3)  /**< PRS OPA1 warmed up. output is valid. */
#define PRS_VDAC0_OPA2              ((23 << 8) + 4)  /**< PRS OPA2 warmed up. output is valid. */
#define PRS_VDAC0_OPA3              ((23 << 8) + 5)  /**< PRS OPA3 warmed up. output is valid. */
#define PRS_LESENSE_SCANRES0        ((24 << 8) + 0)  /**< PRS LESENSE SCANRES register, bit 0 */
#define PRS_LESENSE_SCANRES1        ((24 << 8) + 1)  /**< PRS LESENSE SCANRES register, bit 1 */
#define PRS_LESENSE_SCANRES2        ((24 << 8) + 2)  /**< PRS LESENSE SCANRES register, bit 2 */
#define PRS_LESENSE_SCANRES3        ((24 << 8) + 3)  /**< PRS LESENSE SCANRES register, bit 3 */
#define PRS_LESENSE_SCANRES4        ((24 << 8) + 4)  /**< PRS LESENSE SCANRES register, bit 4 */
#define PRS_LESENSE_SCANRES5        ((24 << 8) + 5)  /**< PRS LESENSE SCANRES register, bit 5 */
#define PRS_LESENSE_SCANRES6        ((24 << 8) + 6)  /**< PRS LESENSE SCANRES register, bit 6 */
#define PRS_LESENSE_SCANRES7        ((24 << 8) + 7)  /**< PRS LESENSE SCANRES register, bit 7 */
#define PRS_LESENSE_SCANRES8        ((25 << 8) + 0)  /**< PRS LESENSE SCANRES register, bit 8 */
#define PRS_LESENSE_SCANRES9        ((25 << 8) + 1)  /**< PRS LESENSE SCANRES register, bit 9 */
#define PRS_LESENSE_SCANRES10       ((25 << 8) + 2)  /**< PRS LESENSE SCANRES register, bit 10 */
#define PRS_LESENSE_SCANRES11       ((25 << 8) + 3)  /**< PRS LESENSE SCANRES register, bit 11 */
#define PRS_LESENSE_SCANRES12       ((25 << 8) + 4)  /**< PRS LESENSE SCANRES register, bit 12 */
#define PRS_LESENSE_SCANRES13       ((25 << 8) + 5)  /**< PRS LESENSE SCANRES register, bit 13 */
#define PRS_LESENSE_SCANRES14       ((25 << 8) + 6)  /**< PRS LESENSE SCANRES register, bit 14 */
#define PRS_LESENSE_SCANRES15       ((25 << 8) + 7)  /**< PRS LESENSE SCANRES register, bit 15 */
#define PRS_LESENSE_DEC0            ((26 << 8) + 0)  /**< PRS LESENSE Decoder PRS out 0 */
#define PRS_LESENSE_DEC1            ((26 << 8) + 1)  /**< PRS LESENSE Decoder PRS out 1 */
#define PRS_LESENSE_DEC2            ((26 << 8) + 2)  /**< PRS LESENSE Decoder PRS out 2 */
#define PRS_LESENSE_DECCMP          ((26 << 8) + 3)  /**< PRS LESENSE Decoder PRS compare value match channel */
#define PRS_LESENSE_MEASACT         ((27 << 8) + 0)  /**< PRS LESENSE Measurement active */
#define PRS_ACMP2_OUT               ((28 << 8) + 0)  /**< PRS Analog comparator output */
#define PRS_ACMP3_OUT               ((29 << 8) + 0)  /**< PRS Analog comparator output */
#define PRS_ADC1_SINGLE             ((30 << 8) + 0)  /**< PRS ADC single conversion done */
#define PRS_ADC1_SCAN               ((30 << 8) + 1)  /**< PRS ADC scan conversion done */
#define PRS_USART0_IRTX             ((48 << 8) + 0)  /**< PRS  */
#define PRS_USART0_TXC              ((48 << 8) + 1)  /**< PRS  */
#define PRS_USART0_RXDATAV          ((48 << 8) + 2)  /**< PRS  */
#define PRS_USART0_RTS              ((48 << 8) + 3)  /**< PRS  */
#define PRS_USART0_TX               ((48 << 8) + 5)  /**< PRS  */
#define PRS_USART0_CS               ((48 << 8) + 6)  /**< PRS  */
#define PRS_USART1_TXC              ((49 << 8) + 1)  /**< PRS  */
#define PRS_USART1_RXDATAV          ((49 << 8) + 2)  /**< PRS  */
#define PRS_USART1_RTS              ((49 << 8) + 3)  /**< PRS  */
#define PRS_USART1_TX               ((49 << 8) + 5)  /**< PRS  */
#define PRS_USART1_CS               ((49 << 8) + 6)  /**< PRS  */
#define PRS_USART2_IRTX             ((50 << 8) + 0)  /**< PRS USART 2 IRDA out */
#define PRS_USART2_TXC              ((50 << 8) + 1)  /**< PRS  */
#define PRS_USART2_RXDATAV          ((50 << 8) + 2)  /**< PRS  */
#define PRS_USART2_RTS              ((50 << 8) + 3)  /**< PRS  */
#define PRS_USART2_TX               ((50 << 8) + 5)  /**< PRS  */
#define PRS_USART2_CS               ((50 << 8) + 6)  /**< PRS  */
#define PRS_USART3_TXC              ((51 << 8) + 1)  /**< PRS  */
#define PRS_USART3_RXDATAV          ((51 << 8) + 2)  /**< PRS  */
#define PRS_USART3_RTS              ((51 << 8) + 3)  /**< PRS  */
#define PRS_USART3_TX               ((51 << 8) + 5)  /**< PRS  */
#define PRS_USART3_CS               ((51 << 8) + 6)  /**< PRS  */
#define PRS_USART4_TXC              ((52 << 8) + 1)  /**< PRS  */
#define PRS_USART4_RXDATAV          ((52 << 8) + 2)  /**< PRS  */
#define PRS_USART4_RTS              ((52 << 8) + 3)  /**< PRS  */
#define PRS_USART4_TX               ((52 << 8) + 5)  /**< PRS  */
#define PRS_USART4_CS               ((52 << 8) + 6)  /**< PRS  */
#define PRS_USART5_TXC              ((53 << 8) + 1)  /**< PRS  */
#define PRS_USART5_RXDATAV          ((53 << 8) + 2)  /**< PRS  */
#define PRS_USART5_RTS              ((53 << 8) + 3)  /**< PRS  */
#define PRS_USART5_TX               ((53 << 8) + 5)  /**< PRS  */
#define PRS_USART5_CS               ((53 << 8) + 6)  /**< PRS  */
#define PRS_UART0_TXC               ((54 << 8) + 1)  /**< PRS  */
#define PRS_UART0_RXDATAV           ((54 << 8) + 2)  /**< PRS  */
#define PRS_UART0_RTS               ((54 << 8) + 3)  /**< PRS  */
#define PRS_UART0_TX                ((54 << 8) + 5)  /**< PRS  */
#define PRS_UART0_CS                ((54 << 8) + 6)  /**< PRS  */
#define PRS_UART1_TXC               ((55 << 8) + 1)  /**< PRS  */
#define PRS_UART1_RXDATAV           ((55 << 8) + 2)  /**< PRS  */
#define PRS_UART1_RTS               ((55 << 8) + 3)  /**< PRS  */
#define PRS_UART1_TX                ((55 << 8) + 5)  /**< PRS  */
#define PRS_UART1_CS                ((55 << 8) + 6)  /**< PRS  */
#define PRS_TIMER0_UF               ((60 << 8) + 0)  /**< PRS  */
#define PRS_TIMER0_OF               ((60 << 8) + 1)  /**< PRS  */
#define PRS_TIMER0_CC0              ((60 << 8) + 2)  /**< PRS  */
#define PRS_TIMER0_CC1              ((60 << 8) + 3)  /**< PRS  */
#define PRS_TIMER0_CC2              ((60 << 8) + 4)  /**< PRS  */
#define PRS_TIMER1_UF               ((61 << 8) + 0)  /**< PRS  */
#define PRS_TIMER1_OF               ((61 << 8) + 1)  /**< PRS  */
#define PRS_TIMER1_CC0              ((61 << 8) + 2)  /**< PRS  */
#define PRS_TIMER1_CC1              ((61 << 8) + 3)  /**< PRS  */
#define PRS_TIMER1_CC2              ((61 << 8) + 4)  /**< PRS  */
#define PRS_TIMER1_CC3              ((61 << 8) + 5)  /**< PRS  */
#define PRS_TIMER2_UF               ((62 << 8) + 0)  /**< PRS  */
#define PRS_TIMER2_OF               ((62 << 8) + 1)  /**< PRS  */
#define PRS_TIMER2_CC0              ((62 << 8) + 2)  /**< PRS  */
#define PRS_TIMER2_CC1              ((62 << 8) + 3)  /**< PRS  */
#define PRS_TIMER2_CC2              ((62 << 8) + 4)  /**< PRS  */
#define PRS_USB_SOF                 ((64 << 8) + 0)  /**< PRS USB Start of Frame */
#define PRS_USB_SOFSR               ((64 << 8) + 1)  /**< PRS USB Start of Frame Sent/Received */
#define PRS_CM4_TXEV                ((67 << 8) + 0)  /**< PRS  */
#define PRS_CM4_ICACHEPCHITSOF      ((67 << 8) + 1)  /**< PRS  */
#define PRS_CM4_ICACHEPCMISSESOF    ((67 << 8) + 2)  /**< PRS  */
#define PRS_TIMER3_UF               ((80 << 8) + 0)  /**< PRS  */
#define PRS_TIMER3_OF               ((80 << 8) + 1)  /**< PRS  */
#define PRS_TIMER3_CC0              ((80 << 8) + 2)  /**< PRS  */
#define PRS_TIMER3_CC1              ((80 << 8) + 3)  /**< PRS  */
#define PRS_TIMER3_CC2              ((80 << 8) + 4)  /**< PRS  */
#define PRS_WTIMER0_UF              ((82 << 8) + 0)  /**< PRS  */
#define PRS_WTIMER0_OF              ((82 << 8) + 1)  /**< PRS  */
#define PRS_WTIMER0_CC0             ((82 << 8) + 2)  /**< PRS  */
#define PRS_WTIMER0_CC1             ((82 << 8) + 3)  /**< PRS  */
#define PRS_WTIMER0_CC2             ((82 << 8) + 4)  /**< PRS  */
#define PRS_WTIMER1_UF              ((83 << 8) + 0)  /**< PRS  */
#define PRS_WTIMER1_OF              ((83 << 8) + 1)  /**< PRS  */
#define PRS_WTIMER1_CC0             ((83 << 8) + 2)  /**< PRS  */
#define PRS_WTIMER1_CC1             ((83 << 8) + 3)  /**< PRS  */
#define PRS_WTIMER1_CC2             ((83 << 8) + 4)  /**< PRS  */
#define PRS_WTIMER1_CC3             ((83 << 8) + 5)  /**< PRS  */
#define PRS_WTIMER2_UF              ((84 << 8) + 0)  /**< PRS  */
#define PRS_WTIMER2_OF              ((84 << 8) + 1)  /**< PRS  */
#define PRS_WTIMER2_CC0             ((84 << 8) + 2)  /**< PRS  */
#define PRS_WTIMER2_CC1             ((84 << 8) + 3)  /**< PRS  */
#define PRS_WTIMER2_CC2             ((84 << 8) + 4)  /**< PRS  */
#define PRS_WTIMER3_UF              ((85 << 8) + 0)  /**< PRS  */
#define PRS_WTIMER3_OF              ((85 << 8) + 1)  /**< PRS  */
#define PRS_WTIMER3_CC0             ((85 << 8) + 2)  /**< PRS  */
#define PRS_WTIMER3_CC1             ((85 << 8) + 3)  /**< PRS  */
#define PRS_WTIMER3_CC2             ((85 << 8) + 4)  /**< PRS  */
#define PRS_TIMER4_UF               ((98 << 8) + 0)  /**< PRS  */
#define PRS_TIMER4_OF               ((98 << 8) + 1)  /**< PRS  */
#define PRS_TIMER4_CC0              ((98 << 8) + 2)  /**< PRS  */
#define PRS_TIMER4_CC1              ((98 << 8) + 3)  /**< PRS  */
#define PRS_TIMER4_CC2              ((98 << 8) + 4)  /**< PRS  */
#define PRS_TIMER5_UF               ((99 << 8) + 0)  /**< PRS  */
#define PRS_TIMER5_OF               ((99 << 8) + 1)  /**< PRS  */
#define PRS_TIMER5_CC0              ((99 << 8) + 2)  /**< PRS  */
#define PRS_TIMER5_CC1              ((99 << 8) + 3)  /**< PRS  */
#define PRS_TIMER5_CC2              ((99 << 8) + 4)  /**< PRS  */
#define PRS_TIMER6_UF               ((100 << 8) + 0) /**< PRS  */
#define PRS_TIMER6_OF               ((100 << 8) + 1) /**< PRS  */
#define PRS_TIMER6_CC0              ((100 << 8) + 2) /**< PRS  */
#define PRS_TIMER6_CC1              ((100 << 8) + 3) /**< PRS  */
#define PRS_TIMER6_CC2              ((100 << 8) + 4) /**< PRS  */

/** @} */
/** @} End of group EFM32GG11B_PRS */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

