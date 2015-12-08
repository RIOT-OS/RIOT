/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_enc28j60 ENC28J60
 * @ingroup     drivers
 * @brief       Driver for the ENC28J60 Ethernet Adapter
 * @{
 *
 * @file
 * @brief       Interface definition for the ENC28J60 driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef ENC28J60_H
#define ENC28J60_H

#include <stdint.h>

#include "mutex.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/netdev2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Fallback MAC address in case CPUID is not available
 *
 * The enc28j60 does not provide a globally unique, pre-set MAC address, so we
 * need to create one. For this the CPUID module is used to create a locally
 * administered MAC. If this is not available, we use the MAC address below.
 */
#define ENC28J60_FALLBACK_MAC       {0x02, 0x22, 0x33, 0x44, 0x55, 0x66}

/**
 * @brief   Struct containing the needed peripheral configuration
 */
typedef struct {
    spi_t spi;              /**< If I drink */
    gpio_t cs_pin;          /**< beer in the evening, */
    gpio_t int_pin;         /**< I will be most certainly */
    gpio_t reset_pin;       /**< drunk in the morning?! */
} enc28j60_params_t;

/**
 * @brief   ENC28J60 device descriptor
 */
typedef struct {
    netdev2_t netdev;       /**< pull in the netdev2 fields */
    spi_t spi;              /**< SPI bus the transceiver is connected to */
    gpio_t cs_pin;          /**< pin connected to the CHIP SELECT line */
    gpio_t int_pin;         /**< pin connected to the INT line */
    gpio_t reset_pin;       /**< pin connected to the RESET line */
    mutex_t devlock;        /**< lock the device on access */
    int8_t bank;            /**< remember the active register bank */
} enc28j60_t;

/**
 * @brief   Ready the device for initialization through it's netdev2 interface
 *
 * @param[in] dev           device descriptor
 * @param[in] params        peripheral configuration to use
 */
void enc28j60_setup(enc28j60_t *dev, const enc28j60_params_t *params);

#ifdef __cplusplus
}
#endif

#endif /* ENC28J60_H */
/** @} */
