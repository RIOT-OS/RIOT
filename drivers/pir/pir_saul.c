/*
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pir
 * @{
 *
 * @file
 * @brief       PIR adaption to the RIOT actuator/sensor interface
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "pir.h"

static int read_occup(const void *dev, phydat_t *res) {
    pir_t *d = (pir_t *)dev;
    if (pir_get_occupancy(d, &(res->val[0]))) {
        /* Read failure */
        return -ECANCELED;
    }
    res->val[1] = 0;
    res->val[2] = 0;
    res->unit = UNIT_PERCENT;
    res->scale = -2;
    return 1;
}

const saul_driver_t pir_saul_occup_driver = {
    .read = read_occup,
    .write = saul_notsup,
    .type = SAUL_SENSE_OCCUP,
};
