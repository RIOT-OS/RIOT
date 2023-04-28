/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         boards_rpi_pico
 * @{
 *
 * @file
 * @brief           Configuration of CPU peripherals for the Raspberry Pi Pico
 * @author          Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#include "kernel_defines.h"
#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

static const uart_conf_t uart_config[] = {
    {
        .dev = UART0,
        .rx_pin = GPIO_PIN(0, 1),
        .tx_pin = GPIO_PIN(0, 0),
        .irqn = UART0_IRQ_IRQn
    },
    {
        .dev = UART1,
        .rx_pin = GPIO_PIN(0, 9),
        .tx_pin = GPIO_PIN(0, 8),
        .irqn = UART1_IRQ_IRQn
    }
};

#define UART_0_ISR      (isr_uart0)
#define UART_1_ISR      (isr_uart1)

#define UART_NUMOF      ARRAY_SIZE(uart_config)

static const timer_channel_conf_t timer0_channel_config[] = {
    {
        .irqn = TIMER_IRQ_0_IRQn
    },
    {
        .irqn = TIMER_IRQ_1_IRQn
    },
    {
        .irqn = TIMER_IRQ_2_IRQn
    },
    {
        .irqn = TIMER_IRQ_3_IRQn
    }
};

static const timer_conf_t timer_config[] = {
    {
        .dev = TIMER,
        .ch = timer0_channel_config,
        .ch_numof = ARRAY_SIZE(timer0_channel_config)
    }
};

#define TIMER_0_ISRA    isr_timer0
#define TIMER_0_ISRB    isr_timer1
#define TIMER_0_ISRC    isr_timer2
#define TIMER_0_ISRD    isr_timer3

#define TIMER_NUMOF     ARRAY_SIZE(timer_config)

/**
 * @name   ADC configuration
 *
 * The configuration consists simply of a list of channels that should be used
 * @{
 */
static const adc_conf_t adc_config[] = {
                                        { .pin = GPIO_PIN(0, 26), .chan = 0},
                                        { .pin = GPIO_PIN(0, 27), .chan = 1},
                                        { .pin = GPIO_PIN(0, 28), .chan = 2},
                                        };

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
