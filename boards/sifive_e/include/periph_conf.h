/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_sifive_e
 * @{
 *
 * @file
 * @brief       Peripheral specific definitions for the Qemu sifive_e RISC-V
 *              board
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "macros/units.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Core Clock configuration
 *
 * Contains the absolute minimum to make the clock code happy. Intentionally
 * doesn't define CLOCK_CORECLOCK. Qemu doesn't pretend to run at a specific
 * clock frequency.
 * @{
 */
#define CONFIG_USE_CLOCK_HFXOSC_PLL         (0)
#define CONFIG_USE_CLOCK_HFXOSC             (0)
#define CONFIG_CLOCK_HFROSC_DIV             (1)
#define CONFIG_CLOCK_HFROSC_TRIM            (6)
/** @} */

/**
 * @name    Timer configuration
 *
 * @{
 */
#define CLOCK_TIMERCLOCK            (MHZ(10))
#define TIMER_NUMOF                 (1)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .addr       = UART0_CTRL_ADDR,
        .rx         = GPIO_PIN(0, 16),
        .tx         = GPIO_PIN(0, 17),
        .isr_num    = INT_UART0_BASE,
    },
    {
        .addr       = UART1_CTRL_ADDR,
        .rx         = GPIO_PIN(0, 18),
        .tx         = GPIO_PIN(0, 23),
        .isr_num    = INT_UART1_BASE,
    },
};

#define UART_NUMOF                  ARRAY_SIZE(uart_config)
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */

