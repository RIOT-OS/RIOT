/*
 * Copyright (C) 2016-2018  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
* @defgroup    drivers_ncv7356 NCV7356 Single Wire CAN Transceiver
* @ingroup     drivers_can
* @ingroup     drivers_can_trx
* @brief       Device driver for the NCV7356 Single Wire CAN Transceiver
*
* The NCV7356 is a physical layer device for a single wire data connection
* such as Bosch's Controller Area Network (CAN) protocol.
* The device is capable of transmissions of up to 100 kbps and operates in a
* voltage range of 5 to 27 V. All logic inputs are compatible with 3.3 V and
* 5 V supply systems. The pins MODE0 and MODE1 indicate the mode of the
* transceiver, with the transceiver being initially in sleep mode.
* An example application circuitry for the 8 pin package can be found at page
* 15 of https://www.onsemi.com/pub/Collateral/NCV7356-D.PDF.
 *
 * @{
 *
 * @file
 * @brief       ncv7356 generic CAN transceiver interface initialization
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 */
#ifndef NCV7356_H
#define NCV7356_H

#include <stdio.h>

#include "periph/gpio.h"
#include "can/can_trx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ncv7356 trx descriptor
 */
typedef struct ncv7356_trx {
    /**
     *  set mode interface
     */
    can_trx_t trx;

    /**
     *  Mode 0 pin of ncv7356 device
     */
    gpio_t mode0_pin;

    /**
     *  Mode 1 pin of ncv7356 device
     */
    gpio_t mode1_pin;

} ncv7356_trx_t;

/**
 * @brief   Set mode interface
 *
 * @param[in] dev      Pointer to the ncv7356 descriptor
 * @param[in] mode     mode to set
 *
 * @return  0 on success
 * @return -1 on error
 */
int ncv7356_trx_set_mode(can_trx_t *dev, can_trx_mode_t mode);

/**
 * @brief   Initialize the given ncv7356
 *
 * @param[in] dev      Pointer to the ncv7356 descriptor
 *
 * @return  0 on success
 * @return -1 on error
 */
int ncv7356_trx_init(can_trx_t *dev);

/**
 * @brief   ncv7356 driver
 */
extern const trx_driver_t ncv7356_driver;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NCV7356_H */
