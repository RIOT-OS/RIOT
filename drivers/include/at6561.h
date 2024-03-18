/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         drivers_at6561
 *
 * @file
 * @brief           Driver for AT6561 CAN transceiver
 *
 * @author          Firas Hamdi <firas.hamdi@ml-pa.com>
 */

#include "periph/gpio.h"
#include "can/can_trx.h"

/**
 * @brief AT6561 CAN transceiver descriptor
 */
typedef struct at6561_trx {
	/* AT6561 standby pin */
	gpio_t stby_pin;
	/* AT6561 interface */
	can_trx_t trx;
} at6561_trx_t;

/* AT6561 driver */
extern const trx_driver_t at6561_driver;