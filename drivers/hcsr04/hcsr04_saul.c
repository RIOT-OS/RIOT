/*
 * Copyright (C) 2021 AUTH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hcsr04
 * @{
 *
 * @file
 * @brief       HC-SR04 adaption to the RIOT actuator/sensor(SAUL) interface
 *
 * @author      Evripidis Chondromatidis <eurichon1996@gmail.com>
 *
 * @}
 */


#include "saul.h"
#include "hcsr04.h"


static int read_hcsr04_distance(const void *dev, phydat_t *res)
{
    int ret = hcsr04_get_distance((hcsr04_t *)dev, (uint16_t *)(&(res->val[0])));

    if (ret < 0) {
        return -ECANCELED;
    }

    res->val[1] = 0;
    res->val[2] = 0;
    res->scale = -3;
    res->unit = UNIT_M;

    return 3;
}

const saul_driver_t hcsr04_distance_saul_driver = {
    .read = read_hcsr04_distance,
    .write = saul_notsup,
    .type = SAUL_SENSE_DISTANCE,
};
