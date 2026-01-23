/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_paa5100je
 * @{
 *
 * @file
 * @brief       SAUL adaption for the PAA5100JE/PMW3901 optical flow sensor
 *
 * The data being read from the sensor represents the relative motion since the last readout.
 * It depends on the surface, lighting conditions, and the sensor’s distance from the ground.
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 *
 * @}
 */

#include "saul.h"
#include "paa5100je.h"

static int read_motion(const void *dev, phydat_t *res)
{
    int err = paa5100je_get_motion_burst((paa5100je_t *) dev, &res->val[0], &res->val[1]);
    if (err) {
        return -ECANCELED;
    }
    res->unit = UNIT_M;
    res->scale = -3;

    return 2;
}

const saul_driver_t paa5100je_saul_driver = {
    .read = read_motion,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_DISTANCE,
};
