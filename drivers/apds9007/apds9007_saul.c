/*
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_apds9007
 * @{
 *
 * @file
 * @brief       APDS9007 adaption to the RIOT actuator/sensor interface
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "apds9007.h"

static int read(const void *dev, phydat_t *res)
{
    if (apds9007_read((const apds9007_t *)dev, &(res->val[0])) != APDS9007_OK) {
        /* Read failure */
        return -ECANCELED;
    }
    memset(&(res->val[1]), 0, 2 * sizeof(int16_t));
    res->unit = UNIT_NONE;
    res->scale = 0;
    return 1;
}

const saul_driver_t apds9007_saul_light_driver = {
    .read = read,
    .write = saul_notsup,
    .type = SAUL_SENSE_LIGHT,
};
