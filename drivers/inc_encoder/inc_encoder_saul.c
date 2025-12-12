/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_inc_encoder
 * @{
 *
 * @file
 * @brief       Generic incremental rotary encoder adaption to the RIOT actuator/sensor interface
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 *
 * @}
 */

#include "saul.h"
#include "inc_encoder.h"

static int read_rpm(const void *dev, phydat_t *res)
{
    inc_encoder_t *d = (inc_encoder_t *) dev;
    int32_t mrpm;
    if (inc_encoder_read_rpm(d, &mrpm)) {
        /* Read failure */
        return -ECANCELED;
    }
    res->val[0] = (uint16_t) mrpm / 100;
    res->unit = UNIT_RPM;
    res->scale = -1;
    return 1;
}

static int read_reset_rev_counter(const void *dev, phydat_t *res)
{
    inc_encoder_t *d = (inc_encoder_t *)dev;
    int32_t rev_counter;
    if (inc_encoder_read_reset_milli_revs(d, &rev_counter)) {
        /* Read failure */
        return -ECANCELED;
    }
    res->val[0] = (int16_t) rev_counter;
    res->unit = UNIT_CTS;
    res->scale = -3; /* millirevolutions */
    return 1;
}

const saul_driver_t inc_encoder_rpm_saul_driver = {
    .read = read_rpm,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_SPEED,
};

const saul_driver_t inc_encoder_rev_count_saul_driver = {
    .read = read_reset_rev_counter,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_COUNT,
};
