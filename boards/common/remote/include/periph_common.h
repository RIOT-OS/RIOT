/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_common_remote
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Re-Mote boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 */

#ifndef PERIPH_COMMON_H
#define PERIPH_COMMON_H

#include "periph_cpu.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_CORECLOCK     (32000000U) /* 32MHz */
/** @} */

/**
 * @name Timer configuration
 *
 * General purpose timers (GPT[0-3]) are configured consecutively and in order
 * (without gaps) starting from GPT0, i.e. if multiple timers are enabled.
 *
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .chn = 2,
        .cfg = GPTMCFG_16_BIT_TIMER, /* required for XTIMER */
    },
    {
        .chn = 1,
        .cfg = GPTMCFG_32_BIT_TIMER,
    },
    {
        .chn = 2,
        .cfg = GPTMCFG_16_BIT_TIMER,
    },
    {
        .chn = 1,
        .cfg = GPTMCFG_32_BIT_TIMER,
    },
};

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))

#define TIMER_IRQ_PRIO      1
/** @} */

/**
 * @name Radio peripheral configuration
 * @{
 */
#define RADIO_IRQ_PRIO      1
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    /* UART0 is mapped to debug usb */
    {
        .dev      = UART0_BASEADDR,
        .rx_pin   = GPIO_PIN(PORT_A, 0),
        .tx_pin   = GPIO_PIN(PORT_A, 1),
        .cts_pin  = GPIO_UNDEF,
        .rts_pin  = GPIO_UNDEF
    },
    {
        .dev      = UART1_BASEADDR,
        .rx_pin   = GPIO_PIN(PORT_C, 1),
        .tx_pin   = GPIO_PIN(PORT_C, 0),
        .cts_pin  = GPIO_UNDEF,
        .rts_pin  = GPIO_UNDEF
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_uart0
#define UART_1_ISR          isr_uart1

/* macros common across all UARTs */
#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))

/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_COMMON_H */
/** @} */
