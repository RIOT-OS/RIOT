/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2016 RWTH Aachen, Josua Arndt
 *               2016 INRIA, Francisco Acosta
 *               2017 HAW Hamburg, Dimitri Nahm
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *               2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_arduino-atmega
 * @{
 *
 * @file
 * @brief       Common configuration of MCU periphery for ATmega boards
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef PERIPH_CONF_ATMEGA_COMMON_H
#define PERIPH_CONF_ATMEGA_COMMON_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#ifndef CLOCK_CORECLOCK
#define CLOCK_CORECLOCK     (16000000UL)
#endif /* CLOCK_CORECLOCK */
/** @} */

/**
 * @name    Timer configuration
 *
 * @details Only the 16 bit timers are used by xtimer
 *
 * ATmega256RFR2
 * =============
 * The timer driver only supports the four 16-bit timers
 * (Timer1, TST, Timer3, Timer4, Timer5), so those are the Timer we can use here.
 * Timer 1, TST, PORT B5/6/7 Out, Port D4/6 In,  Analog Comparator Input Capture, Output Compare Modulator, PWM
 * Timer 3, TST, PORT E3/4/5 Out, Port E/6/7 In, Input or Output Compare and PWM Output
 * Timer 4, TST, It can not be connected to any I/O Pin,
 * Timer 5, TST, It can not be connected to any I/O Pin,
 *
 * Using Atmel Timer 4 and 5 seems to be the best choice
 * Using Atmel Timer 4 as Xtimer
 * and Atmel Timer 5 as timer available for the the application seems to be the best choice,
 * as the special functions of the other timer are not lost.
 * Atmel Timer1 to be used as PWM timer for RGB LED
 *
 * ATmega328p
 * ==========
 * The timer driver only supports the 16-bit timer (Timer1)
 * so this is the only one we can use here.
 *
 * ATmega1281
 * ==========
 * The ATmega1281 has 6 timers. Timer0 and Timer2 are 8 Bit Timers,
 * Timer0 has special uses too and therefore we'll avoid using it.
 *
 * The timer driver only supports the four 16-bit timers (Timer1, Timer3,
 * Timer4, Timer5), so those are the only ones we can use here.
 *
 * ATmega1284P
 * ===========
 * The ATmega1284P has 4 timers. Timer0 and Timer2 are 8 Bit Timers.
 *
 * The timer driver only supports the two 16-bit timers (Timer1 and
 * Timer3), so those are the only ones we can use here.
 *
 * ATmega2560
 * ==========
 * The timer driver only supports the four 16-bit timers (Timer1, Timer3,
 * Timer4, Timer5), so those are the only onces we can use here.
 *
 * @{
 */
#ifndef TIMER_NUMOF
#if defined(CPU_ATMEGA256RFR2)
    #define TIMER_NUMOF         (3U)
    #define TIMER_CHANNELS      (3)

    #define TIMER_0             MEGA_TIMER4
    #define TIMER_0_MASK        &TIMSK4
    #define TIMER_0_FLAG        &TIFR4
    #define TIMER_0_ISRA        TIMER4_COMPA_vect
    #define TIMER_0_ISRB        TIMER4_COMPB_vect
    #define TIMER_0_ISRC        TIMER4_COMPC_vect

    #define TIMER_1             MEGA_TIMER5
    #define TIMER_1_MASK        &TIMSK5
    #define TIMER_1_FLAG        &TIFR5
    #define TIMER_1_ISRA        TIMER5_COMPA_vect
    #define TIMER_1_ISRB        TIMER5_COMPB_vect
    #define TIMER_1_ISRC        TIMER5_COMPC_vect

    #define TIMER_2             MEGA_TIMER1
    #define TIMER_2_MASK        &TIMSK1
    #define TIMER_2_FLAG        &TIFR1
    #define TIMER_2_ISRA        TIMER1_COMPA_vect
    #define TIMER_2_ISRB        TIMER1_COMPB_vect
    #define TIMER_2_ISRC        TIMER1_COMPC_vect
#elif defined(CPU_ATMEGA328P)
    #define TIMER_NUMOF         (1U)
    #define TIMER_CHANNELS      (2)

    #define TIMER_0             MEGA_TIMER1
    #define TIMER_0_MASK        &TIMSK1
    #define TIMER_0_FLAG        &TIFR1
    #define TIMER_0_ISRA        TIMER1_COMPA_vect
    #define TIMER_0_ISRB        TIMER1_COMPB_vect
#elif defined(CPU_ATMEGA1284P)
    #define TIMER_NUMOF         (2U)
    #define TIMER_CHANNELS      (2)

    #define TIMER_0             MEGA_TIMER1
    #define TIMER_0_MASK        &TIMSK1
    #define TIMER_0_FLAG        &TIFR1
    #define TIMER_0_ISRA        TIMER1_COMPA_vect
    #define TIMER_0_ISRB        TIMER1_COMPB_vect

    #define TIMER_1             MEGA_TIMER3
    #define TIMER_1_MASK        &TIMSK3
    #define TIMER_1_FLAG        &TIFR3
    #define TIMER_1_ISRA        TIMER3_COMPA_vect
    #define TIMER_1_ISRB        TIMER3_COMPB_vect
#elif defined(CPU_ATMEGA2560) || defined(CPU_ATMEGA1281)
    #define TIMER_NUMOF         (2U)
    #define TIMER_CHANNELS      (3)

    #define TIMER_0             MEGA_TIMER1
    #define TIMER_0_MASK        &TIMSK1
    #define TIMER_0_FLAG        &TIFR1
    #define TIMER_0_ISRA        TIMER1_COMPA_vect
    #define TIMER_0_ISRB        TIMER1_COMPB_vect
    #define TIMER_0_ISRC        TIMER1_COMPC_vect

    #define TIMER_1             MEGA_TIMER4
    #define TIMER_1_MASK        &TIMSK4
    #define TIMER_1_FLAG        &TIFR4
    #define TIMER_1_ISRA        TIMER4_COMPA_vect
    #define TIMER_1_ISRB        TIMER4_COMPB_vect
    #define TIMER_1_ISRC        TIMER4_COMPC_vect
#else
    #define TIMER_NUMOF         (0U)
#endif
#endif /* TIMER_NUMOF */
/** @} */

/**
 * @name    UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */

#ifndef UART_NUMOF
#if defined(CPU_ATMEGA256RFR2) || defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA1284P) || defined(CPU_ATMEGA2560)
    #define UART_NUMOF          (2U)

    /* UART0 is used for stdio */
    #define UART_0              MEGA_UART0
    #define UART_0_ISR          USART0_RX_vect

    #define UART_1              MEGA_UART1
    #define UART_1_ISR          USART1_RX_vect
#elif defined(CPU_ATMEGA328P)
    #define UART_NUMOF          (1U)

    #define UART_0              MEGA_UART0
    #define UART_0_ISR          USART_RX_vect
#else
    #define UART_NUMOF          (0U)
#endif
#endif /* UART_NUMOF */
/** @} */


/**
 * @name    SPI configuration
 *
 * All currently supported ATmega MCUs have only one hardware SPI with fixed pin
 * configuration, so all we can do here, is to enable or disable it...
 *
 * The fixed pins ATmega328p are:
 * MOSI - PB3
 * MISO - PB4
 * SCK  - PB5
 * SS   - PB2 -> this pin is configured as output, but not used
 *
 * The fixed pins for the ATmega128rp are:
 * MOSI - PB5
 * MISO - PB6
 * SCK  - PB7
 * SS   - PB4 -> this pin is configured as output, but not used
 *
 * The fixed pins for the ATmega1281, ATmega256rfr2, and ATmega2560 are:
 * MOSI - PB2
 * MISO - PB3
 * SCK  - PB1
 * SS   - PB0 -> this pin is configured as output, but not used
 *
 * The SS pin must be configured as output for the SPI device to work as
 * master correctly, though we do not use it for now (as we handle the chip
 * select externally for now)
 *
 * @{
 */
#ifndef SPI_NUMOF
#define SPI_NUMOF               (1U)
#endif /* SPI_NUMOF */
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
#ifndef I2C_NUMOF
#define I2C_BUS_SPEED           I2C_SPEED_FAST
#define I2C_NUMOF               (1U)
#endif /* I2C_NUMOF */
/** @} */

/**
 * @name ADC Configuration
 *
 * The number of ADC channels of the atmega328p depends on the package:
 *  - 6-channel 10-bit ADC in PDIP package
 *  - 8-channel 10-bit ADC in TQFP and QFN/MLF package
 * Arduino UNO / Duemilanove has thereby 6 channels. But only 5 channels can be
 * used for ADC, because the pin of ADC5 emulate a software triggered interrupt.
 * @{
 */
#ifndef ADC_NUMOF
#if defined(CPU_ATMEGA256RFR2) || defined(CPU_ATMEGA328P) || defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA1284P)
    #define ADC_NUMOF           (8U)
#elif defined (CPU_ATMEGA2560)
    #define ADC_NUMOF           (16U)
#else
    #define ADC_NUMOF           (0U)
#endif
#endif /* ADC_NUMOF */
/** @} */

/**
 * @name   PWM configuration
 *
 * The current implementation supports only 8-bit timers for PWM generation.
 * These timers are typically timer 0 and timer 2 in Atmega2560/1281/328p.
 *
 * Setting the first channel to GPIO_UNDEF allows multiple resolutions for the
 * PWM channel. Otherwise the resolution is fixed to 256, allowing duty cycle
 * values ranging from 0 to 255.
 *
 * @{
 */
#ifndef PWM_NUMOF
#if defined(CPU_ATMEGA328P)
    #define PWM_PINS_CH0 { GPIO_PIN(PORT_D, 6), GPIO_PIN(PORT_D, 5) }
    #define PWM_PINS_CH1 { GPIO_PIN(PORT_B, 3), GPIO_PIN(PORT_D, 3) }
#elif defined(CPU_ATMEGA1281)
    #define PWM_PINS_CH0 { GPIO_PIN(PORT_B, 7), GPIO_PIN(PORT_G, 5) }
    #define PWM_PINS_CH1 { GPIO_PIN(PORT_B, 4), GPIO_UNDEF }
#elif defined(CPU_ATMEGA2560)
    #define PWM_PINS_CH0 { GPIO_PIN(PORT_B, 7), GPIO_PIN(PORT_G, 5) }
    #define PWM_PINS_CH1 { GPIO_PIN(PORT_B, 4), GPIO_PIN(PORT_H, 6) }
#else
    #define PWM_NUMOF           (0U)
#endif

#if defined(CPU_ATMEGA328P) || defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA2560)
    static const pwm_conf_t pwm_conf[] = {
        {
            .dev = MINI_TIMER0,
            .pin_ch = PWM_PINS_CH0,
            .div = MINI_TIMER0_DIV,
        },
        {
            .dev = MINI_TIMER2,
            .pin_ch = PWM_PINS_CH1,
            .div = MINI_TIMER2_DIV,
        }
    };

    #define PWM_NUMOF           (sizeof(pwm_conf) / sizeof(pwm_conf[0]))
#endif
#endif /* PWM_NUMOF */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_ATMEGA_COMMON_H */
