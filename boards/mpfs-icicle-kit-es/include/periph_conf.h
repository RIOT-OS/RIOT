/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_mpfs-icicle-kit-es
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the MPFS Icicle Kit
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GCLK reference speed
 */
#define CLOCK_CORECLOCK     (600000000U)

/**
 * @name    Timer peripheral configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {   /* Timer 0 - System Clock */
        .dev            = TIMER_LO,
        .num            = 1,
    },
    {   /* Timer 0 - System Clock */
        .dev            = TIMER_LO,
        .num            = 2,
    }
};

/* Timer 0 configuration */
#define TIMER_0_CHANNELS    1u
#define TIMER_NUMOF         2u
/** @} */

/**
 * @name    UART configuration
 * @{
 */
#define MSS_UART0_LO_BASE           (MSS_UART_TypeDef*)0x20000000UL
static const uart_conf_t uart_config[] = {
    {    /* E51 UART */
        .dev = &g_mss_uart0_lo,
        .base_addr =  MSS_UART0_LO_BASE,
        .irqn = MMUART0_PLIC_77,
        .clk = MSS_PERIPH_MMUART0,
    },
};

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
