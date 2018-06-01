/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_adxl362
 * @{
 *
 * @file
 * @brief       SAUL adaptation layer for ADXL362 accelerometer
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <errno.h>
#include "saul.h"

#include "adxl362.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static int read_acceleration(void *ptr, phydat_t *res)
{
    adxl362_t *dev = ptr;

    adxl362_data_t data;

    int status = adxl362_read_acc_now(dev, &data);
    if (status < 0) {
        DEBUG("adxl362_saul: Communication error!\n");
        return -ECANCELED;
    }

    res->val[0] = (int32_t)data.acc_x;
    res->val[1] = (int32_t)data.acc_y;
    res->val[2] = (int32_t)data.acc_z;
    res->unit = UNIT_G;
    res->scale = -3; /* low level device driver uses milli-G */

    return 3;
}

static int read_temperature(void *ptr, phydat_t *res)
{
    adxl362_t *dev = ptr;
    int32_t temp = 0;

    int status = adxl362_read_temp(dev, &temp);
    if (status < 0) {
        DEBUG("adxl362_saul: Communication error!\n");
        return -ECANCELED;
    }

    res->val[0] = temp;
    res->unit = UNIT_TEMP_C;
    res->scale = -3; /* low level device driver uses milli-Celsius */

    return 1;
}

const saul_driver_t adxl362_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t adxl362_acceleration_saul_driver = {
    .read = read_acceleration,
    .write = saul_notsup,
    .type = SAUL_SENSE_ACCEL
};
