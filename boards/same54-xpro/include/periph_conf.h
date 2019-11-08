/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_same54-xpro
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the Microchip SAM E54 Xplained
 *              Pro board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
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
#define CLOCK_CORECLOCK     (120000000U)

/**
 * @name Timer peripheral configuration
 * @{
 */
static const tc32_conf_t timer_config[] = {
    {   /* Timer 0 - System Clock */
        .dev            = TC0,
        .irq            = TC0_IRQn,
        .mclk           = &MCLK->APBAMASK.reg,
        .mclk_mask      = MCLK_APBAMASK_TC0 | MCLK_APBAMASK_TC1,
        .gclk_id        = TC0_GCLK_ID,
        .gclk_src       = GCLK_PCHCTRL_GEN(5),
        .prescaler      = TC_CTRLA_PRESCALER_DIV8,
        .flags          = TC_CTRLA_MODE_COUNT32,
    },
    {   /* Timer 1 */
        .dev            = TC2,
        .irq            = TC2_IRQn,
        .mclk           = &MCLK->APBBMASK.reg,
        .mclk_mask      = MCLK_APBBMASK_TC2 | MCLK_APBBMASK_TC3,
        .gclk_id        = TC2_GCLK_ID,
        .gclk_src       = GCLK_PCHCTRL_GEN(5),
        .prescaler      = TC_CTRLA_PRESCALER_DIV8,
        .flags          = TC_CTRLA_MODE_COUNT32,
    }
};

/* Timer 0 configuration */
#define TIMER_0_CHANNELS    2
#define TIMER_0_ISR         isr_tc0

/* Timer 1 configuration */
#define TIMER_1_CHANNELS    2
#define TIMER_1_ISR         isr_tc2

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {    /* Virtual COM Port */
        .dev      = &SERCOM2->USART,
        .rx_pin   = GPIO_PIN(PB,24),
        .tx_pin   = GPIO_PIN(PB,25),
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = GCLK_PCHCTRL_GEN_GCLK0
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom2_2

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = &(SERCOM6->SPI),
        .miso_pin = GPIO_PIN(PC, 7),
        .mosi_pin = GPIO_PIN(PC, 4),
        .clk_pin  = GPIO_PIN(PC, 5),
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1

    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev      = &(SERCOM7->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PD, 9),
        .sda_pin  = GPIO_PIN(PD, 8),
        .mux      = GPIO_MUX_C,
        .gclk_src = GCLK_PCHCTRL_GEN_GCLK0,
        .flags    = I2C_FLAG_NONE
    }
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */


/**
 * @name    RTC configuration
 * @{
 */
#define EXTERNAL_OSC32_SOURCE                    1
#define INTERNAL_OSC32_SOURCE                    0
#define ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE      0
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#define RTT_FREQUENCY       (32768U)
#define RTT_MAX_VALUE       (0xffffffffU)
/** @} */

/**
 * @name USB peripheral configuration
 * @{
 */
static const sam0_common_usb_config_t sam_usbdev_config[] = {
    {
        .dm     = GPIO_PIN(PA, 24),
        .dp     = GPIO_PIN(PA, 25),
        .d_mux  = GPIO_MUX_H,
        .device = &USB->DEVICE,
    }
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
