/*
 * Copyright (C) 2019 Ken Rabold
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_hifive1b
 * @{
 *
 * @file
 * @brief       Peripheral specific definitions for the HiFive1b RISC-V board
 *
 * @author      Ken Rabold
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
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
#define USE_CLOCK_HFXOSC_PLL        (1)
#define USE_CLOCK_HFXOSC            (0)
#define USE_CLOCK_HFROSC_PLL        (0)

#if USE_CLOCK_HFROSC_PLL && (USE_CLOCK_HFXOSC_PLL || USE_CLOCK_HFXOSC)
#error "Cannot use HFROSC_PLL with HFXOSC based configurations"
#endif

#if USE_CLOCK_HFXOSC_PLL && USE_CLOCK_HFXOSC
#error "Cannot use HFXOSC with HFXOSC_PLL"
#endif

#if USE_CLOCK_HFXOSC_PLL
#define CLOCK_PLL_R                 (1)             /* Divide input clock by 2, mandatory with HFXOSC */
#define CLOCK_PLL_F                 (39)            /* Multiply REFR by 80, e.g 2 * (39 + 1) */
#define CLOCK_PLL_Q                 (1)             /* Divide VCO by 2, e.g 2^1 */
#define CLOCK_PLL_INPUT_CLOCK       (16000000UL)
#define CLOCK_PLL_REFR              (CLOCK_PLL_INPUT_CLOCK / (CLOCK_PLL_R + 1))
#define CLOCK_PLL_VCO               (CLOCK_PLL_REFR * (2 * (CLOCK_PLL_F + 1)))
#define CLOCK_PLL_OUT               (CLOCK_PLL_VCO / (1 << CLOCK_PLL_Q))
#define CLOCK_CORECLOCK             (CLOCK_PLL_OUT) /* 320000000Hz with the values used above */

/* Check PLL settings */
#if CLOCK_PLL_REFR != 8000000
#error "Only R=2 can be used when using HFXOSC"
#endif
#if (CLOCK_PLL_VCO < 384000000) || (CLOCK_PLL_VCO > 768000000)
#error "VCO frequency must be in the range [384MHz - 768MHz], check the CLOCK_PLL_F value"
#endif
#if (CLOCK_PLL_OUT < 48000000) || (CLOCK_PLL_OUT > 384000000)
#error "PLL output frequency must be in the range [48MHz - 384MHz], check the CLOCK_PLL_Q value"
#endif

#elif USE_CLOCK_HFXOSC
#define CLOCK_CORECLOCK             (16000000UL)

/*
  When using HFROSC input clock, the core clock cannot be computed from settings,
  call cpu_freq() to get the configured CPU frequency.
*/
#elif USE_CLOCK_HFROSC_PLL
#define CLOCK_DESIRED_FREQUENCY     (320000000UL)

#else
#define CLOCK_HFROSC_TRIM           (6)             /* ~72000000Hz input freq */
#define CLOCK_HFROSC_DIV            (1)             /* Divide by 2 */
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
 * @name    RTT/RTC configuration
 *
 * @{
 */
#define RTT_FREQUENCY               (1)             /* in Hz */
#define RTT_MAX_VALUE               (0xFFFFFFFF)
#define RTT_INTR_PRIORITY           (2)

/** @} */

/**
 * @name    PWM configuration
 *
 * @{
 */
#define PWM_NUMOF                   (3)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .addr   = I2C0_CTRL_ADDR,
        .scl    = GPIO_PIN(0, 13),
        .sda    = GPIO_PIN(0, 12),
        .speed  = I2C_SPEED_NORMAL,
     },
};

#define I2C_NUMOF                   ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
