/*
 * Copyright (C) 2018 Max van Kessel <maxvankessel@betronic.nl>
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
#include "generic.h"

/**
 * @ingroup     drivers_gsm
 * @{
 *
 * @file
 * @brief       Generic gsm implementation.
 *
 * @author      Max van Kessel <maxvankessel@betronic.nl>
 *
 * @}
 */

#define LOG_HEADER  "gsm_generic: "

int generic_init(generic_t *dev)
{
    int err = -EINVAL;

    if(dev){
        uint8_t retries = 3;
        //gsm_generic_t * priv = (gsm_generic_t *)dev;

        /* generic modem should be on by default */

        /* send AT to see if it's alive */
        err = gsm_is_alive(dev, 3);

        if(err) {
            LOG_INFO(LOG_HEADER"no response on AT\n");

            /* TODO no response on AT check if device is in command mode */
        }
        else {
            dev->state = GSM_ON;
        }

    }
    return err;
}

const gsm_driver_t generic_driver = {
    .init_base = generic_init,
};
