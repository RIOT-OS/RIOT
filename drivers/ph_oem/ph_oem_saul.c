/*
 * Copyright (C) 2019 University of Applied Sciences Emden / Leer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ph_oem
 * @{
 *
 * @file
 * @brief       pH OEM adaption to the sensor/actuator abstraction layer
 *
 * @author      Igor Knippenberg <igor.knippenberg@gmail.com>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "ph_oem.h"
#include "ph_oem_regs.h"

static int read_ph(const void *dev, phydat_t *res)
{
    const ph_oem_t *mydev = dev;
    uint16_t ph_reading;

    if (gpio_is_valid(mydev->params.interrupt_pin)) {
        puts("interrupt pin not supported with SAUL yet");
        return -ENOTSUP;
    }

    if (ph_oem_start_new_reading(mydev) < 0) {
        return -ECANCELED;
    }

    /* Read raw pH value */
    if (ph_oem_read_ph(mydev, &ph_reading) < 0) {
        return -ECANCELED;
    }
    res->val[0] = (int16_t)ph_reading;
    res->unit = UNIT_PH;
    res->scale = -3;

    return 1;
}

/* Sets the temperature compensation for taking accurate pH readings.
 * Valid temperature range is 1 - 20000 (0.01 °C  to  200.0 °C) */
static int set_temp_compensation(const void *dev, phydat_t *res)
{
    const ph_oem_t *mydev = dev;

    if (!(res->val[0] >= 1 && res->val[0] <= 20000)) {
        return -ECANCELED;
    }

    if (ph_oem_set_compensation(mydev, res->val[0]) < 0) {
        return -ECANCELED;
    }
    return 1;
}

const saul_driver_t ph_oem_saul_driver = {
    .read = read_ph,
    .write = set_temp_compensation,
    .type = SAUL_SENSE_PH,
};
