/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_microbit_v2
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the BBC micro:bit v2
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "board.h"
#include "periph_cpu.h"
#include "cfg_clock_32_0.h"
#include "cfg_timer_default.h"
#include "cfg_rtt_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    { /* Mapped to USB virtual COM port */
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(1, 8),
        .tx_pin     = GPIO_PIN(0, 6),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
};

#define UART_0_ISR          (isr_uart0)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        NRF_PWM0,
        {
            SPEAKER_PIN,    /* configure Speaker pin as PWM */
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF,
        }
    },
};
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {   /* Internal I2C */
        .dev = NRF_TWIM1,
        .scl = GPIO_PIN(0, 8),
        .sda = GPIO_PIN(0, 16),
        .speed = I2C_SPEED_NORMAL
    },
    {   /* External I2C */
        .dev = NRF_TWIM0,
        .scl = GPIO_PIN(0, 26),     /* P19 */
        .sda = GPIO_PIN(1, 0),      /* P20 */
        .speed = I2C_SPEED_NORMAL
    },
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(0, 17),    /* P13 */
        .mosi = GPIO_PIN(0, 13),    /* P15 */
        .miso = GPIO_PIN(0, 1),     /* P14 */
#ifdef ERRATA_SPI_SINGLE_BYTE_WORKAROUND
        .ppi = 0,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
