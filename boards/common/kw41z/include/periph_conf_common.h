/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_common_kw41z
 * @{
 *
 * @file
 * @name        Common peripheral MCU configuration for KW41Z based boards
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef PERIPH_CONF_COMMON_H
#define PERIPH_CONF_COMMON_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Clock system configuration
 * @{
 */
static const clock_config_t clock_config = {
    /*
     * This configuration results in the system running with the internal clock
     * with the following clock frequencies:
     * Core:  48 MHz
     * Bus:   24 MHz
     * Flash: 24 MHz
     */
    .clkdiv1            = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(1),
    /* unsure if this RTC load cap configuration is correct, but it matches the
     * settings used by the example code in the NXP provided SDK */
    .rtc_clc            = 0,
    /* Use the 32 kHz oscillator as ERCLK32K. Note that the values here have a
     * different mapping for the KW41Z than the values used in the Kinetis K series */
    .osc32ksel          = SIM_SOPT1_OSC32KSEL(0),
    .clock_flags =
        KINETIS_CLOCK_OSC0_EN | /* Enable RSIM oscillator */
        KINETIS_CLOCK_RTCOSC_EN |
        KINETIS_CLOCK_USE_FAST_IRC |
        KINETIS_CLOCK_MCGIRCLK_EN | /* Used for LPUART clocking */
        KINETIS_CLOCK_MCGIRCLK_STOP_EN |
        0,
    /* Using FEI mode by default, the external crystal settings below are only
     * used if mode is changed to an external mode (PEE, FBE, or FEE) */
    .default_mode       = KINETIS_MCG_MODE_FEI,
    /* The crystal connected to RSIM OSC is 32 MHz */
    .erc_range          = KINETIS_MCG_ERC_RANGE_VERY_HIGH,
    .osc_clc            = 0, /* no load cap configuration */
    .oscsel             = MCG_C7_OSCSEL(0), /* Use RSIM for external clock */
    .fcrdiv             = MCG_SC_FCRDIV(0), /* Fast IRC divide by 1 => 4 MHz */
    .fll_frdiv          = MCG_C1_FRDIV(0b101), /* Divide by 1024 */
    .fll_factor_fei     = KINETIS_MCG_FLL_FACTOR_1464, /* FEI FLL freq = 48 MHz */
    .fll_factor_fee     = KINETIS_MCG_FLL_FACTOR_1280, /* FEE FLL freq = 40 MHz */
};
/* Radio xtal frequency, either 32 MHz or 26 MHz */
#define CLOCK_RADIOXTAL              (32000000ul)
/* CPU core clock, the MCG clock output frequency */
#define CLOCK_CORECLOCK              (48000000ul)
#define CLOCK_BUSCLOCK               (CLOCK_CORECLOCK / 2)
#define CLOCK_MCGIRCLK               (4000000ul)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define PIT_NUMOF               (1U)
#define PIT_CONFIG {                 \
        {                            \
            .prescaler_ch = 0,       \
            .count_ch = 1,           \
        },                           \
    }
#define LPTMR_NUMOF             (1U)
#define LPTMR_CONFIG { \
        { \
            .dev = LPTMR0, \
            .irqn = LPTMR0_IRQn, \
            .src = 2, \
            .base_freq = 32768u, \
        } \
    }
#define TIMER_NUMOF             ((PIT_NUMOF) + (LPTMR_NUMOF))
#define PIT_BASECLOCK           (CLOCK_BUSCLOCK)
#define LPTMR_ISR_0             isr_lptmr0
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev    = LPUART0,
        .freq   = CLOCK_MCGIRCLK,
        .pin_rx = GPIO_PIN(PORT_C,  6),
        .pin_tx = GPIO_PIN(PORT_C,  7),
        .pcr_rx = PORT_PCR_MUX(4),
        .pcr_tx = PORT_PCR_MUX(4),
        .irqn   = LPUART0_IRQn,
        .scgc_addr = &SIM->SCGC5,
        .scgc_bit = SIM_SCGC5_LPUART0_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_LPUART,
    },
};
#define UART_NUMOF          ARRAY_SIZE(uart_config)
#define LPUART_0_ISR        isr_lpuart0
/* Use MCGIRCLK (internal reference 4 MHz clock) */
#define LPUART_0_SRC        3
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define KINETIS_TRNG                TRNG
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_COMMON_H */
/** @} */
