/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_teensy31
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the Teensy3.1 & 3.2
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 */
static const clock_config_t clock_config = {
    /*
     * This configuration results in the system running from the FLL output with
     * the following clock frequencies:
     * Core:  48 MHz
     * Bus:   48 MHz
     * Flex:  24 MHz
     * Flash: 24 MHz
     */
    /* The board has a 16 MHz crystal, though it is not used in this configuration */
    /* This configuration uses the RTC crystal to provide the base clock, it
     * should have better accuracy than the internal slow clock, and lower power
     * consumption than using the 16 MHz crystal and the OSC0 module */
    .clkdiv1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(0) |
               SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(1),
    /* RTC crystal has to be soldered by the user, we can't know the load cap requirements */
    .rtc_clc = 0,
    .osc32ksel = SIM_SOPT1_OSC32KSEL(2),
    .clock_flags =
        KINETIS_CLOCK_RTCOSC_EN |
        KINETIS_CLOCK_USE_FAST_IRC |
        0,
    .default_mode = KINETIS_MCG_MODE_FEE,
    .erc_range = KINETIS_MCG_ERC_RANGE_LOW, /* Input clock is 32768 Hz */
    /* 16 pF capacitors yield ca 10 pF load capacitance as required by the
     * onboard xtal, not used when OSC0 is disabled */
    .osc_clc = OSC_CR_SC16P_MASK,
    .oscsel = MCG_C7_OSCSEL(1), /* Use RTC oscillator as external clock */
    .fcrdiv = MCG_SC_FCRDIV(0), /* Fast IRC divide by 1 => 4 MHz */
    .fll_frdiv = MCG_C1_FRDIV(0b000), /* Divide by 1 => FLL input 32768 Hz */
    .fll_factor_fei = KINETIS_MCG_FLL_FACTOR_1464, /* FLL freq = 48 MHz */
    .fll_factor_fee = KINETIS_MCG_FLL_FACTOR_1464, /* FLL freq = 48 MHz */
    /* PLL is unavailable when using a 32768 Hz source clock, so the
     * configuration below can only be used if the above config is modified to
     * use the 16 MHz crystal instead of the RTC. */
    .pll_prdiv = MCG_C5_PRDIV0(0b00111), /* Divide by 8 */
    .pll_vdiv  = MCG_C6_VDIV0(0b01100), /* Multiply by 36 => PLL freq = 72 MHz */
};
#define CLOCK_CORECLOCK              (48000000ul)
#define CLOCK_BUSCLOCK               (CLOCK_CORECLOCK / 1)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define PIT_NUMOF               (2U)
#define PIT_CONFIG {                 \
        {                            \
            .prescaler_ch = 0,       \
            .count_ch = 1,           \
        },                           \
        {                            \
            .prescaler_ch = 2,       \
            .count_ch = 3,           \
        },                           \
    }
#define LPTMR_NUMOF             (0U)
#define LPTMR_CONFIG { \
    }
#define TIMER_NUMOF             ((PIT_NUMOF) + (LPTMR_NUMOF))

#define PIT_BASECLOCK           (CLOCK_BUSCLOCK)
#define PIT_ISR_0               isr_pit1
#define PIT_ISR_1               isr_pit3
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev    = UART0,
        .freq   = CLOCK_CORECLOCK,
        .pin_rx = GPIO_PIN(PORT_B, 16), /* TEENSY PIN 0 */
        .pin_tx = GPIO_PIN(PORT_B, 17), /* TEENSY PIN 1 */
        .pcr_rx = PORT_PCR_MUX(3),
        .pcr_tx = PORT_PCR_MUX(3),
        .irqn   = UART0_RX_TX_IRQn,
        .scgc_addr = &SIM->SCGC4,
        .scgc_bit = SIM_SCGC4_UART0_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_UART,
    },
    {
        .dev    = UART1,
        .freq   = CLOCK_CORECLOCK,
        .pin_rx = GPIO_PIN(PORT_C, 3), /* TEENSY PIN 9 */
        .pin_tx = GPIO_PIN(PORT_C, 4), /* TEENSY PIN 10 */
        .pcr_rx = PORT_PCR_MUX(3),
        .pcr_tx = PORT_PCR_MUX(3),
        .irqn   = UART1_RX_TX_IRQn,
        .scgc_addr = &SIM->SCGC4,
        .scgc_bit = SIM_SCGC4_UART1_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_UART,
    },
};

#define UART_0_ISR          (isr_uart0_rx_tx)
#define UART_1_ISR          (isr_uart1_rx_tx)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .ftm        = FTM0,
        .chan       = {
            { .pin = GPIO_PIN(PORT_C, 1), .af = 4, .ftm_chan = 0 }, /* TEENSY PIN 22 */
            { .pin = GPIO_PIN(PORT_C, 2), .af = 4, .ftm_chan = 1 }, /* TEENSY PIN 23 */
            { .pin = GPIO_UNDEF,          .af = 0, .ftm_chan = 0 },
            { .pin = GPIO_UNDEF,          .af = 0, .ftm_chan = 0 }
        },
        .chan_numof = 2,
        .ftm_num    = 0
    },
    {
        .ftm        = FTM1,
        .chan       = {
            { .pin = GPIO_PIN(PORT_A, 12), .af = 3, .ftm_chan = 0 }, /* TEENSY PIN 3 */
            { .pin = GPIO_PIN(PORT_A, 13), .af = 3, .ftm_chan = 1 }, /* TEENSY PIN 4 */
            { .pin = GPIO_UNDEF,           .af = 0, .ftm_chan = 0 },
            { .pin = GPIO_UNDEF,           .af = 0, .ftm_chan = 0 }
        },
        .chan_numof = 2,
        .ftm_num    = 1
    }
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
