/*
 * SPDX-FileCopyrightText: 2018 UC Berkeley
 * SPDX-License-Identifier: LGPL-2.1-only
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
    .write = saul_write_notsup,
    .type = SAUL_SENSE_OCCUP,
};
