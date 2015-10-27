/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_ksz8851snl ksz8851snl
 * @ingroup     drivers
 * @brief       Driver for the ksz8851snl Ethernet Adapter
 * @{
 *
 * @file
 * @brief       Interface definition for the ksz8851snl driver
 *
 * @author      Matt Poppe <matt@poppe.me>
 */

#ifndef KSZ8851SNL_H
#define KSZ8851SNL_H

#include "mutex.h"
#include "kernel_types.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/netdev2.h"

#include "ksz8851snl_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ksz8851snl netdev2 device
 * @extends netdev2_t
 */
typedef struct {
    netdev2_t netdev;       /**< extended netdev2 structure */
    spi_t spi;              /**< SPI device the enc is connected to*/
    gpio_t cs;              /**< SPI chip select pin */
    gpio_t rst;             /**< Reset Ethernet chip pin */
    gpio_t int_pin;         /**< SPI interrupt pin */
    mutex_t mutex;          /**< mutex used to lock device access */
} ksz8851snl_t;

/**
 * @brief Setup an ksz8851snl based device state.
 *
 * This function sets SPI pins, initializes the device state structure.
 * It does not initialize the device itself.
 *
 * @param[out]  dev     the handle of the device to initialize
 * @param[in]   spi     SPI device the device is connected to
 * @param[in]   cs_pin  SPI chip select pin used to select the device
 * @param[in]   int_pin pin the device will trigger an interrupt on
 */
 void ksz8851snl_setup(ksz8851snl_t *dev, spi_t spi, gpio_t cs, gpio_t int_pin,
     gpio_t rst);
#ifdef __cplusplus
}
#endif
#endif /* ksz8851snl_H */
/** @} */
