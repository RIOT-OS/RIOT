/*
 * Copyright (C) 2022 Universidade de São Paulo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pulga
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Pulga board
 *
 * @author      Geovane Fedrecheski <geonnave@gmail.com>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "kernel_defines.h"
#include "periph_cpu.h"
#include "cfg_clock_32_1.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(0, 8),
        .mosi = GPIO_PIN(0, 4),
        .miso = GPIO_PIN(0, 6),
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
        .dev = NRF_TWIM1,
        .scl = GPIO_PIN(0, 15),
        .sda = GPIO_PIN(0, 13),
        .speed = I2C_SPEED_NORMAL
    }
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */


/* Default pins for UART0 connection. */
#ifndef PULGA_UART0_RX
#define PULGA_UART0_RX GPIO_PIN(0,25)
#endif
#ifndef PULGA_UART0_TX
#define PULGA_UART0_TX GPIO_PIN(0,28)
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    { /* Mapped to USB virtual COM port */
        .dev        = NRF_UARTE0,
        .rx_pin     = PULGA_UART0_RX,
        .tx_pin     = PULGA_UART0_TX,
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
    {
        .dev        = NRF_UARTE1,
        /* Configuration for the Pulga GPS board.
         * These definitions are only used/needed when
         * the Pulga GPS shield is connected.
         */
        .rx_pin     = GPIO_PIN(0,7),
        .tx_pin     = GPIO_PIN(0,5),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE1_IRQn,
    },
};

#define UART_0_ISR          (isr_uart0)
#define UART_1_ISR          (isr_uarte1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */


/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    /* Beware: Keep pwm_params.h in sync with the definitions here */
    { NRF_PWM0, {
        /* configure LED0 as PWM */
#ifdef LED0_PIN
        LED0_PIN,
#else
        GPIO_UNDEF,
#endif
        /* configure LED1 as PWM */
#ifdef LED1_PIN
        LED1_PIN,
#else
        GPIO_UNDEF,
#endif
    } },
};
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
