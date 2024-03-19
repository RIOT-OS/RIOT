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
 * @{
 * @file
 * @brief           Driver for AT6561 CAN transceiver
 *
 * @author          Firas Hamdi <firas.hamdi@ml-pa.com>
 *
 * @}
 */

#include "can/can_trx.h"
#include "at6561.h"
#include "periph/gpio.h"
#include "assert.h"
#include "container.h"

static int _init(can_trx_t *can_trx);
static int _set_mode(can_trx_t *can_trx, can_trx_mode_t mode);

const trx_driver_t at6561_driver = {
    .init = _init,
    .set_mode = _set_mode,
};

static int _init(can_trx_t *can_trx)
{
    at6561_trx_t *dev = container_of(can_trx, at6561_trx_t, trx);

    gpio_init(dev->stby_pin, GPIO_OUT);
    /* A high level on the standby pin of the CAN transceiver will
        select the standby mode. Check AT6560/1 datasheet, table 1-1 */
    gpio_set(dev->stby_pin);

    return 1;
}

/* Set the mode of the CAN transceiver */
static int _set_mode(can_trx_t *can_trx, can_trx_mode_t mode)
{
    at6561_trx_t *dev = container_of(can_trx, at6561_trx_t, trx);

    /* Check AT6560/1 datasheet, table 1-1 */
    switch (mode) {
        case TRX_NORMAL_MODE:
            gpio_clear(dev->stby_pin);
            break;
        case TRX_SLEEP_MODE:
            gpio_set(dev->stby_pin);
            break;
        default:
            assert(0);
    }

    return 1;
}
