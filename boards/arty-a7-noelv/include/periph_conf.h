/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arty-a7-noelv
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Gaisler NOEL-V Arty A7
 *
 * The addresses below are the ones of the NOEL-ARTYA7 example designs, which
 * share a single memory map across all of their processor configurations.
 * They can be confirmed at runtime with: grmon4> info sys
 *
 * @author      Matvii Ivashchenko
 */

#include "kernel_defines.h"
#include "periph_cpu.h"

/**
 * @name    Clock configuration
 *
 * Must match the frequency of the FPGA bitstream (GRMON `info sys` reports
 * the actual AHB frequency). The ACLINT mtime counter runs at half of it.
 * @{
 */
#define CLOCK_CORECLOCK     (40000000UL)    /**< CPU core clock, 40 MHz */
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 *
 * One timer via RISC-V ACLINT (CLINT). The GPTIMER unit at 0xfc000000 is
 * left unused.
 * CLINT base address is defined in cpu_conf.h (0xe0000000).
 * @{
 */
#define TIMER_NUMOF         (1)
/** @} */

/**
 * @name    GPIO configuration
 *
 * grmon: gpio0  APB: fc083000 - fc083100
 *
 * The GRGPIO port is 20 bits wide on this board.
 * @{
 */
#define GPIO0_BASE_ADDR     (0xfc083000UL)  /**< GRGPIO base address */
/** @} */

/**
 * @name    UART configuration
 *
 * grmon: uart0  APB: fc001000 - fc001100  IRQ: 1
 *
 * SW3 = OFF: UART0 routed to RIOT application
 * SW3 = ON:  UART0 routed to debug (GRMON)
 * @{
 */
#define UART0_BASE_ADDR     (0xfc001000UL)  /**< APBUART0 base address */
#define UART0_IRQ           (1U)            /**< APBUART0 PLIC interrupt line */

static const uart_conf_t uart_config[] = {
    {
        .addr   = UART0_BASE_ADDR,
        .irq    = UART0_IRQ,
    },
};

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    GRETH Ethernet configuration
 *
 * grmon: greth0  APB: fc084000 - fc084100  IRQ: 5
 * @{
 */
#define GRETH_PARAM_BASE    (0xfc084000UL)  /**< GRETH base address */
#define GRETH_PARAM_IRQ     (5U)            /**< GRETH interrupt line */
/** @} */


#ifdef __cplusplus
}
#endif

/** @} */
