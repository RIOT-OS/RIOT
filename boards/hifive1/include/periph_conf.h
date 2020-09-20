/*
 * Copyright (C) 2017 Ken Rabold
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_hifive1
 * @{
 *
 * @file
 * @brief       Peripheral specific definitions for the HiFive1 RISC-V board
 *
 * @author      Ken Rabold
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Core Clock configuration
 * @{
 */
#ifndef CONFIG_USE_CLOCK_HFXOSC_PLL
#if IS_ACTIVE(CONFIG_USE_CLOCK_HFXOSC) || IS_ACTIVE(CONFIG_USE_CLOCK_HFROSC_PLL) || \
    IS_ACTIVE(CONFIG_USE_CLOCK_HFROSC)
#define CONFIG_USE_CLOCK_HFXOSC_PLL         0
#else
#define CONFIG_USE_CLOCK_HFXOSC_PLL         1       /* Use PLL clocked by HFXOSC by default */
#endif
#endif /* CONFIG_USE_CLOCK_HFXOSC_PLL */

#ifndef CONFIG_USE_CLOCK_HFXOSC
#define CONFIG_USE_CLOCK_HFXOSC             0
#endif /* CONFIG_USE_CLOCK_HFXOSC */

#ifndef CONFIG_USE_CLOCK_HFROSC_PLL
#define CONFIG_USE_CLOCK_HFROSC_PLL         0
#endif /* CONFIG_USE_CLOCK_HFROSC_PLL */

#ifndef CONFIG_USE_CLOCK_HFROSC
#define CONFIG_USE_CLOCK_HFROSC             0
#endif /* CONFIG_USE_CLOCK_HFROSC */

#if CONFIG_USE_CLOCK_HFXOSC_PLL && \
    (CONFIG_USE_CLOCK_HFROSC_PLL || CONFIG_USE_CLOCK_HFROSC || CONFIG_USE_CLOCK_HFXOSC)
#error "Cannot use HFXOSC_PLL with other clock configurations"
#endif

#if CONFIG_USE_CLOCK_HFXOSC && \
    (CONFIG_USE_CLOCK_HFROSC_PLL || CONFIG_USE_CLOCK_HFROSC || CONFIG_USE_CLOCK_HFXOSC_PLL)
#error "Cannot use HFXOSC with other clock configurations"
#endif

#if CONFIG_USE_CLOCK_HFROSC_PLL && \
    (CONFIG_USE_CLOCK_HFXOSC_PLL || CONFIG_USE_CLOCK_HFXOSC || CONFIG_USE_CLOCK_HFROSC)
#error "Cannot use HFROSC_PLL with other clock configurations"
#endif

#if CONFIG_USE_CLOCK_HFROSC && \
    (CONFIG_USE_CLOCK_HFXOSC_PLL || CONFIG_USE_CLOCK_HFXOSC || CONFIG_USE_CLOCK_HFROSC_PLL)
#error "Cannot use HFROSC with other clock configurations"
#endif

#define CONFIG_CLOCK_PLL_R                  (1)     /* Divide input clock by 2, mandatory with HFXOSC */
#ifndef CONFIG_CLOCK_PLL_F
#define CONFIG_CLOCK_PLL_F                  (39)    /* Multiply REFR by 80, e.g 2 * (39 + 1) */
#endif
#ifndef CONFIG_CLOCK_PLL_Q
#define CONFIG_CLOCK_PLL_Q                  (1)     /* Divide VCO by 2, e.g 2^1 */
#endif

#if CONFIG_USE_CLOCK_HFXOSC_PLL
#define CLOCK_PLL_INPUT_CLOCK               MHZ(16)
#define CLOCK_PLL_REFR                      (CLOCK_PLL_INPUT_CLOCK / (CONFIG_CLOCK_PLL_R + 1))
#define CLOCK_PLL_VCO                       (CLOCK_PLL_REFR * (2 * (CONFIG_CLOCK_PLL_F + 1)))
#define CLOCK_PLL_OUT                       (CLOCK_PLL_VCO / (1 << CONFIG_CLOCK_PLL_Q))
#define CLOCK_CORECLOCK                     (CLOCK_PLL_OUT) /* 320000000Hz with the values used above */

/* Check PLL settings */
#if CLOCK_PLL_REFR != MHZ(8)
#error "Only R=2 can be used when using HFXOSC"
#endif
#if (CLOCK_PLL_VCO < MHZ(384)) || (CLOCK_PLL_VCO > MHZ(768))
#error "VCO frequency must be in the range [384MHz - 768MHz], check the CLOCK_PLL_F value"
#endif
#if (CLOCK_PLL_OUT < MHZ(48)) || (CLOCK_PLL_OUT > MHZ(384))
#error "PLL output frequency must be in the range [48MHz - 384MHz], check the CLOCK_PLL_Q value"
#endif

#elif CONFIG_USE_CLOCK_HFXOSC
#define CLOCK_CORECLOCK                     MHZ(16)
#endif

/*
  When using HFROSC input clock, the core clock cannot be computed from settings,
  call cpu_freq() to get the configured CPU frequency.
*/
#ifndef CONFIG_CLOCK_DESIRED_FREQUENCY
#define CONFIG_CLOCK_DESIRED_FREQUENCY     MHZ(320)
#endif

#ifndef CONFIG_CLOCK_HFROSC_TRIM
#define CONFIG_CLOCK_HFROSC_TRIM           (6)      /* ~72000000Hz input freq */
#endif
#ifndef CONFIG_CLOCK_HFROSC_DIV
#define CONFIG_CLOCK_HFROSC_DIV            (1)      /* Divide by 2 */
#endif
/** @} */

/**
 * @name    Timer configuration
 *
 * @{
 */
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

/**
 * @name    SPI device configuration
 *
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .addr       = SPI1_CTRL_ADDR,
        .mosi       = GPIO_PIN(0, 3), /* D11 */
        .miso       = GPIO_PIN(0, 4), /* D12 */
        .sclk       = GPIO_PIN(0, 5), /* D13 */
    },
};

#define SPI_NUMOF                  ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    PWM configuration
 *
 * @{
 */
#define PWM_NUMOF                   (3)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
