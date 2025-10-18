/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_hall_effect
 * @{
 *
 * @file
 * @brief       Hall effect sensor adaption to the RIOT actuator/sensor interface
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "hall_effect.h"

static int read_rpm(const void *dev, phydat_t *res)
{
    hall_effect_t *d = (hall_effect_t *) dev;
    int32_t rpm;
    if (hall_effect_read_rpm(d, &rpm)) {
        /* Read failure */
        return -ECANCELED;
    }
    res->val[0] = (uint16_t) rpm;
    res->unit = UNIT_RPM;
    res->scale = 0;
    return 1;
}

static int read_reset_pulse_counter(const void *dev, phydat_t *res)
{
    hall_effect_t *d = (hall_effect_t *)dev;
    int32_t counter;
    if (hall_effect_read_reset_ceti_revs(d, &counter)) {
        /* Read failure */
        return -ECANCELED;
    }
    res->val[0] = (int16_t) counter;
    res->unit = UNIT_CTS;
    res->scale = -2;
    return 1;
}

const saul_driver_t hall_effect_rpm_saul_driver = {
    .read = read_rpm,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_SPEED,
};

const saul_driver_t hall_pulse_count_saul_driver = {
    .read = read_reset_pulse_counter,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_COUNT,
};
