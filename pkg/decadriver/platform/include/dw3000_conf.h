/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_decadriver
 * @{
 *
 * @file
 * @brief       Device configuration structs
 *
 * @author      Simon Grund <mail@simongrund.de>
 */

#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    spi_t spi;                  /**< SPI */
    gpio_t spi_cs;              /**< SPI CS pin */
    /** @brief SPI operating mode
     *
     * If a something different than the default SPI mode (SPI_MODE_0) should be
     * used, on the DW3000 two pins are used at startup to signal this. They are
     * expected to be pulled to the correct setting manually, outside of this
     * implementation. */
    spi_mode_t spi_mode;
    gpio_t irq;                 /**< pin to get interrupts from */
    /** reset pin (optional, if not used set to GPIO_UNDEF) */
    gpio_t reset;
    /** wakeup pin (optional, if not used set to GPIO_UNDEF) */
    gpio_t wakeup;
    /** The fast mode speed (set with dw3000_spi_speed_fast()) */
    spi_clk_t spi_speed_fast;
    /** The slow mode speed (set with dw3000_spi_speed_slow()) */
    spi_clk_t spi_speed_slow;
} dw3000_params_t;

#ifdef __cplusplus
}
#endif

/** @} */
