/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_cc3200-launchxl
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the CC3200 board
 *
 * @author      Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Timer peripheral configuration
 *
 */

#include "periph_cpu.h"

#define CLOCK_CORECLOCK (80000000U) /* desired core clock frequency */

/*  CC3200 has 4 timer blocks */
#define TIMER_0_EN 1
#define TIMER_1_EN 1
#define TIMER_2_EN 1
#define TIMER_3_EN 1

/**
 * @brief number of timer blocks
 *
 */
#define TIMER_NUMOF 4U
/** @} */

/**
 * @name SPI configuration
 * @{
 */
/**
 * @brief spi port configurations
 *
 */
static const spi_conf_t spi_config[] = {
    {
            .base_addr = GSPI_BASE,
            .gpio_port = 0,
            .pins = (spi_pins_t){ .miso = 05, .sck = 04, .mosi = 06, .cs = 07 },
            .config = (SPI_HW_CTRL_CS | SPI_4PIN_MODE | SPI_TURBO_OFF |
                       SPI_CS_ACTIVELOW | SPI_WL_8 | PRCM_RUN_MODE_CLK),
    },
    {
            .base_addr = LSPI_BASE,
            .gpio_port = 1,
            .pins = (spi_pins_t){ .miso = 0, .sck = 0, .mosi = 0, .cs = 0 },
            .config    = (SPI_SW_CTRL_CS | SPI_4PIN_MODE | SPI_TURBO_OFF |
                       SPI_CS_ACTIVEHIGH | SPI_WL_32 | PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK),
    }
};

/**
 *
 */
#define SPI_NUMOF (sizeof(spi_config) / sizeof(spi_conf_t))
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF 2

#define UART_0_EN 1
#define UART_1_EN 1

/**
 * @brief UART device configuration
 *
 */
static const uart_conf_t uart_config[] = {
    {
            .dev    = UART0,
            .pin_tx = 54,
            .pin_rx = 56,
            .irqn   = INT_UARTA0,
    },
    {
            .dev    = UART1,
            .pin_rx = 6,
            .pin_tx = 8,
            .irqn   = INT_UARTA1,
    },
};
/** @} */

#include "board.h"

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H */
/** @} */
