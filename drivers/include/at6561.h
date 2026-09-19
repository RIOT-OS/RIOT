/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        drivers_at6561 AT6561 CAN transceiver driver
 * @ingroup         drivers_can
 * @ingroup         drivers_can_trx
 *
 * @brief           Driver for AT6561 CAN transceiver
 * @{
 *
 * @file
 *
 * @author          Firas Hamdi <firas.hamdi@ml-pa.com>
 */

#ifndef AT6561_H
#define AT6561_H

#include "periph/gpio.h"
#include "can/can_trx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AT6561 CAN transceiver descriptor
 */
typedef struct at6561_trx {
    gpio_t stby_pin;            /**< AT6561 standby pin */
    can_trx_t trx;              /**< AT6561 interface */
} at6561_trx_t;

/**
 * @brief AT6561 driver
 */
extern const trx_driver_t at6561_driver;

#ifdef __cplusplus
}
#endif

#endif /* AT6561_H */
/** @} */
