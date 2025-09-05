/*
 * SPDX-FileCopyrightText: 2024 CNRS, France
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_abp2
 * @{
 *
 * @file
 * @brief       ABP2 adaption to the SAUL framework.
 *
 * @author      David Picard <david.picard@clermont.in2p3.fr>
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "abp2.h"

static int read_press(const void *dev, phydat_t *press)
{
    abp2_raw_t rawData;

    if (abp2_measure_read((abp2_t *)dev, &rawData)) {
        return -ECANCELED;
    }

    /* abp2_pressure() returns an int32_t in thousandths of user units.
     * i.e. µbar in this context.
     * Convert to tenths of millibars to fit in phydat_t.val[0],
     * a int16_t, while maintaining a 0.1 mbar resolution */
    press->val[0] = abp2_pressure((abp2_t *)dev, &rawData) / 100;
    press->scale = -4;
    press->unit = UNIT_BAR;

    return 1;
}

static int read_temp(const void *dev, phydat_t *temp)
{
    abp2_raw_t rawData;

    if (abp2_measure_read((abp2_t *)dev, &rawData)) {
        return -ECANCELED;
    }

    /* abp2_temperature() returns milli-degrees Celsius */
    temp->val[0] = abp2_temperature((abp2_t *)dev, &rawData);
    temp->scale = -3;
    temp->unit = UNIT_TEMP_C;

    return 1;
}

const saul_driver_t abp2_saul_driver_press = {
    .read = read_press,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PRESS,
};

const saul_driver_t abp2_saul_driver_temp = {
    .read = read_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};
