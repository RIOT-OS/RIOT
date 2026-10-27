/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_ksz8851 KSZ8851SNL ethernet driver
 * @ingroup     drivers_netdev
 * @brief       Driver for Microchip KSZ8851SNL SPI ethernet devices
 *
 * This device driver uses the KSZ8851SNL's QMU (Queue Management Unit) to
 * send and receive raw Ethernet frames, so it does not offer any support
 * for the chip's optional offload features (checksum generation, wakeup
 * frame / magic packet detection, ...) beyond what is required to get
 * plain Ethernet frames in and out through netdev.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the KSZ8851SNL device driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdbool.h>
#include <stdint.h>

#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Struct containing the peripheral configuration for the device
 */
typedef struct {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_clk_t clk;          /**< clock speed used on the selected SPI bus */
    gpio_t cs;              /**< pin connected to the chip select line */
    gpio_t irq;             /**< pin connected to the interrupt line */
    gpio_t reset;           /**< pin connected to the reset line, may be
                              *  @ref GPIO_UNDEF if not wired up */
} ksz8851_params_t;

/**
 * @brief   KSZ8851SNL device descriptor
 */
typedef struct {
    netdev_t netdev;        /**< extends the netdev structure */
    ksz8851_params_t p;     /**< peripheral configuration */
    bool link_up;           /**< used to only emit a LINK event on change */
    uint8_t frame_id;       /**< running TX frame ID, used for QMU bookkeeping */
} ksz8851_t;

/**
 * @brief   Setup the device descriptor and the used peripherals
 *
 * This function pre-initializes the netdev structure, saves the
 * configuration parameters and initializes the chip select and interrupt
 * GPIO pins. The actual device initialization happens in the netdev
 * `init()` function, which is called by @ref netdev_t::driver.
 *
 * @param[out] dev      device descriptor to initialize
 * @param[in]  params   peripheral configuration to use
 * @param[in]  index    index of @p params in a global parameter struct
 *                       array. If initialized manually, pass a unique
 *                       identifier instead.
 */
void ksz8851_setup(ksz8851_t *dev, const ksz8851_params_t *params, uint8_t index);

#ifdef __cplusplus
}
#endif

/** @} */
