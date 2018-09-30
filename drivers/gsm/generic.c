/*
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>

#include "log.h"
#include "xtimer.h"

#include "gsm/generic.h"

#include "include/gsm_internal.h"

/**
 * @ingroup     drivers_gsm
 * @{
 *
 * @file
 * @brief       Generic gsm implementation.
 *
 * @author      Max van Kessel
 *
 * @}
 */

#define MODULE  "gsm_generic: "

int generic_init(gsm_generic_t *dev)
{
    int err = -EINVAL;

    if (dev) {
        /* generic modem should be on by default */

        /* send AT to see if it's alive */
        bool alive = gsm_is_alive(dev, 3);

        if (!alive) {
            LOG_INFO(MODULE"no response on AT\n");

            /* TODO no response on AT check if device is in command mode */
            err = -1;
        }
        else {
            dev->state = GSM_ON;
            err = 0;
        }

    }
    return err;
}

const gsm_driver_t gsm_generic_driver = {
    .init_base = generic_init,
};
