/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_encx24j600 ENCX24J600
 * @ingroup     drivers_netdev
 * @brief       Driver for the ENCX24J600 Ethernet Adapter
 * @{
 *
 * @file
 * @brief       Interface definition for the ENCX24J600 driver
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ENCX24J600_H
#define ENCX24J600_H

#include "mutex.h"
#include "kernel_types.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/netdev2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief encx24j600 netdev2 device
 * @extends netdev2_t
 */
typedef struct {
    netdev2_t netdev;       /**< extended netdev2 structure */
    spi_t spi;              /**< SPI device the enc is connected to*/
    gpio_t cs;              /**< SPI chip select pin */
    gpio_t int_pin;         /**< SPI interrupt pin */
    uint16_t rx_next_ptr;   /**< ptr to next packet whithin devices memory */
} encx24j600_t;

/**
 * @brief   Struct containing the needed peripheral configuration
 */
typedef struct {
    spi_t spi;              /**< SPI line */
    gpio_t cs_pin;          /**< chip select pin */
    gpio_t int_pin;         /**< interrupt pin */
} encx24j600_params_t;

/**
 * @brief Setup an encx24j600 based device state.
 *
 * This function sets SPI pins, initializes the device state structure.
 * It does not initialize the device itself.
 *
 * @param[out] dev     the handle of the device to initialize
 * @param[in]  params  parameters for device initialization
 */
void encx24j600_setup(encx24j600_t *dev, const encx24j600_params_t *params);

#ifdef __cplusplus
}
#endif
#endif /* ENCX24J600_H */
/** @} */
