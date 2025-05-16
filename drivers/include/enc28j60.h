/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_enc28j60 ENC28J60
 * @ingroup     drivers_netdev
 * @brief       Driver for the ENC28J60 Ethernet Adapter
 * @{
 *
 * @file
 * @brief       Interface definition for the ENC28J60 driver
 * @anchor      drivers_enc28j60
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdint.h>

#include "mutex.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Struct containing the needed peripheral configuration
 */
typedef struct {
    spi_t spi;              /**< If I drink */
    gpio_t cs_pin;          /**< beer in the evening, */
    gpio_t int_pin;         /**< I will be most certainly */
    gpio_t rst_pin;         /**< drunk in the morning?! */
} enc28j60_params_t;

/**
 * @brief   ENC28J60 device descriptor
 */
typedef struct {
    netdev_t netdev;        /**< pull in the netdev fields */
    enc28j60_params_t p;    /**< SPI and pin configuration */
    mutex_t lock;           /**< lock the device on access */
    uint32_t tx_time;       /**< last transmission time for timeout handling */
} enc28j60_t;

/**
 * @brief   Ready the device for initialization through it's netdev interface
 *
 * @param[in] dev           device descriptor
 * @param[in] params        peripheral configuration to use
 * @param[in]   index       Index of @p params in a global parameter struct array.
 *                          If initialized manually, pass a unique identifier instead.
 */
void enc28j60_setup(enc28j60_t *dev, const enc28j60_params_t *params, uint8_t index);

#ifdef __cplusplus
}
#endif

/** @} */
