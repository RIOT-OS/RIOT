/*
 * SPDX-FileCopyrightText: 2026 Mathis LECRIVAIN <lecrivain.mathis@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_max1161x
 * @{
 *
 * @file
 * @brief       MAX1161X adaption to the RIOT actuator/sensor interface
 *
 * @author      Mathis Lécrivain <lecrivain.mathis@gmail.com>
 *
 * @}
 */

#include <errno.h>

#include "max1161x.h"
#include "saul.h"

static int read_adc(const void *dev, phydat_t *res)
{
    if (max1161x_read_raw((const max1161x_t *)dev, &res->val[0])) {
        return -ECANCELED;
    }

    res->unit = UNIT_NONE;
    res->scale = 0;

    return 1;
}

const saul_driver_t max1161x_saul_driver = {
    .read = read_adc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ANALOG,
};
