/*
 * Copyright (C) 2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_ikea-tradfri
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the IKEA TRÅDFRI modules
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "cpu.h"
#include "periph_cpu.h"
#include "em_cmu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#ifndef CLOCK_HF
#define CLOCK_HF            cmuSelect_HFRCO
#endif
#ifndef CLOCK_CORE_DIV
#define CLOCK_CORE_DIV      cmuClkDiv_1
#endif
#ifndef CLOCK_LFA
#define CLOCK_LFA           cmuSelect_LFRCO
#endif
#ifndef CLOCK_LFB
#define CLOCK_LFB           cmuSelect_LFRCO
#endif
#ifndef CLOCK_LFE
#define CLOCK_LFE           cmuSelect_LFRCO
#endif
/** @} */

/**
 * @name    RTT configuration
 * @{
 */

#define RTT_MAX_VALUE       (0xFFFFFFFF)
#define RTT_FREQUENCY       (1U)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_dev_t spi_config[] = {
    {
        .dev = USART1,
        .mosi_pin = GPIO_PIN(PD, 15),
        .miso_pin = GPIO_PIN(PD, 14),
        .clk_pin = GPIO_PIN(PD, 13),
        .loc = USART_ROUTELOC0_RXLOC_LOC21 |
               USART_ROUTELOC0_TXLOC_LOC23 |
               USART_ROUTELOC0_CLKLOC_LOC19,
        .cmu = cmuClock_USART1,
        .irq = USART1_RX_IRQn
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    Timer configuration
 *
 * The implementation can use one low-energy timer
 * or two regular timers in cascade mode.
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .prescaler = {
            .dev = TIMER0,
            .cmu = cmuClock_TIMER0
        },
        .timer = {
            .dev = TIMER1,
            .cmu = cmuClock_TIMER1
        },
        .irq = TIMER1_IRQn,
        .channel_numof = 3
    },
    {
        .prescaler = {
            .dev = NULL,
            .cmu = cmuClock_LETIMER0
        },
        .timer = {
            .dev = LETIMER0,
            .cmu = cmuClock_LETIMER0
        },
        .irq = LETIMER0_IRQn,
        .channel_numof = 2
    },
};

#define TIMER_0_ISR         isr_timer1
#define TIMER_1_ISR         isr_letimer0

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
#ifndef EFM32_USE_LEUART
#define EFM32_USE_LEUART   0
#endif

#if EFM32_USE_LEUART

#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE (9600)
#endif

static const uart_conf_t uart_config[] = {
    {
        .dev = LEUART0,
        .rx_pin = GPIO_PIN(PB, 15),
        .tx_pin = GPIO_PIN(PB, 14),
        .loc = USART_ROUTELOC0_RXLOC_LOC9 |
               USART_ROUTELOC0_TXLOC_LOC9,
        .cmu = cmuClock_LEUART0,
        .irq = LEUART0_IRQn
    }
};
#define UART_0_ISR_RX       isr_leuart0

#else /* EFM32_USE_LEUART */

static const uart_conf_t uart_config[] = {
    {
        .dev = USART0,
        .rx_pin = GPIO_PIN(PB, 15),
        .tx_pin = GPIO_PIN(PB, 14),
        .loc = USART_ROUTELOC0_RXLOC_LOC9 |
               USART_ROUTELOC0_TXLOC_LOC9,
        .cmu = cmuClock_USART0,
        .irq = USART0_RX_IRQn
    }
};
#define UART_0_ISR_RX       isr_usart0_rx

#endif /* EFM32_USE_LEUART */
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
