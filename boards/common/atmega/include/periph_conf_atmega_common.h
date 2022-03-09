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
 * @name    UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */

#ifndef UART_NUMOF
#if defined(CPU_ATMEGA128RFA1) || defined(CPU_ATMEGA256RFR2) || \
    defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA1284P) || defined(CPU_ATMEGA2560)
    #define UART_NUMOF          (2U)

    /* UART0 is used for stdio */
    #define UART_0              MEGA_UART0
    #define UART_0_ISR          USART0_RX_vect
    #define UART_0_ISR_TX       USART0_TX_vect

    #define UART_1              MEGA_UART1
    #define UART_1_ISR          USART1_RX_vect
    #define UART_1_ISR_TX       USART1_TX_vect
#elif defined(CPU_ATMEGA328P)
    #define UART_NUMOF          (1U)

    #define UART_0              MEGA_UART0
    #define UART_0_ISR          USART_RX_vect
    #define UART_0_ISR_TX       USART_TX_vect
#elif defined(CPU_ATMEGA32U4)
    #define UART_NUMOF          (1U)

    #define UART_0              MEGA_UART1
    #define UART_0_ISR          USART1_RX_vect
    #define UART_0_ISR_TX       USART1_TX_vect
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
 * The fixed pins ATmega328P are:
 *
 * | Function | Pin |
 * |:-------- |:--- |
 * | MOSI     | PB3 |
 * | MISO     | PB4 |
 * | SCK      | PB5 |
 * | SS       | PB2 |
 *
 * The fixed pins for the ATmega1284P are:
 *
 * | Function | Pin |
 * |:-------- |:--- |
 * | MOSI     | PB5 |
 * | MISO     | PB6 |
 * | SCK      | PB7 |
 * | SS       | PB4 |
 *
 * The fixed pins for the ATmega1281, ATmega256RFR2, ATmega2560, and ATmega32U4
 * are:
 *
 * | Function | Pin |
 * |:-------- |:--- |
 * | MOSI     | PB2 |
 * | MISO     | PB3 |
 * | SCK      | PB1 |
 * | SS       | PB0 |
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
#if defined(CPU_ATMEGA128RFA1) || defined(CPU_ATMEGA256RFR2) || defined(CPU_ATMEGA328P) ||  \
    defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA1284P) || defined(CPU_ATMEGA32U4)
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
 * These timers are typically timer 0 and timer 2 for
 * ATmega 328P/1281/1284P/2560.
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
#elif defined(CPU_ATMEGA1284P)
    #define PWM_PINS_CH0 { GPIO_PIN(PORT_B, 3), GPIO_PIN(PORT_B, 4) }
    #define PWM_PINS_CH1 { GPIO_PIN(PORT_D, 7), GPIO_PIN(PORT_D, 6) }
#elif defined(CPU_ATMEGA2560)
    #define PWM_PINS_CH0 { GPIO_PIN(PORT_B, 7), GPIO_PIN(PORT_G, 5) }
    #define PWM_PINS_CH1 { GPIO_PIN(PORT_B, 4), GPIO_PIN(PORT_H, 6) }
#elif defined(CPU_ATMEGA32U4)
    #define PWM_PINS_CH0 { GPIO_PIN(PORT_B, 7), GPIO_PIN(PORT_D, 0) }
#else
    #define PWM_NUMOF           (0U)
#endif

#if defined(CPU_ATMEGA32U4) || defined(CPU_ATMEGA328P) || \
    defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA1284P) || \
    defined(CPU_ATMEGA2560)
    static const pwm_conf_t pwm_conf[] = {
        {
            .dev = MINI_TIMER0,
            .pin_ch = PWM_PINS_CH0,
            .div = MINI_TIMER0_DIV,
        },
#ifndef CPU_ATMEGA32U4
        {
            .dev = MINI_TIMER2,
            .pin_ch = PWM_PINS_CH1,
            .div = MINI_TIMER2_DIV,
        }
#endif
    };

    #define PWM_NUMOF           ARRAY_SIZE(pwm_conf)
#endif
#endif /* PWM_NUMOF */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_ATMEGA_COMMON_H */
