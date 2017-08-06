/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_can_trx
 * @brief       generic transceiver interface
 * @{
 *
 * @file
 * @brief       generic transceiver interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include <errno.h>
#include <stdlib.h>

#include "can/can_trx.h"

int can_trx_init(can_trx_t *dev)
{
    if (dev == NULL) {
        return -ENODEV;
    }

    if (dev->driver->init) {
        return dev->driver->init(dev);
    }
    else {
        return -ENOTSUP;
    }
}

int can_trx_set_mode(can_trx_t *dev, can_trx_mode_t mode)
{
    if (dev == NULL) {
        return -ENODEV;
    }

    if (dev->driver->set_mode) {
        return dev->driver->set_mode(dev, mode);
    }
    else {
        return -ENOTSUP;
    }
}
