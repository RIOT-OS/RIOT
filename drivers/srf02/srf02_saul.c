/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_srf02
 * @{
 *
 * @file
 * @brief       SAUL interface for SRF02 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "srf02.h"
#include "saul.h"


static int read(void *dev, phydat_t *data)
{
    srf02_t *d = (srf02_t *)dev;

    data->val[0] = (int16_t)srf02_get_distance(d, SRF02_MODE_REAL_CM);
    memset(&data->val[1], 0, 2 * sizeof(int16_t));
    data->unit = UNIT_M;
    data->scale = -2;
    return 1;
}

const saul_driver_t srf02_saul_driver = {
    .read = read,
    .write = saul_notsup,
    .type = SAUL_SENSE_DIST
};
