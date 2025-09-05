/*
 * SPDX-FileCopyrightText: 2018 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_sht1x
 * @{
 *
 * @file
 * @brief       SAUL adaption for SHT10/SHT11/SHT15 devices
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "phydat.h"
#include "saul.h"
#include "sht1x_defines.h"
#include "sht1x.h"

static int read(const sht1x_dev_t *dev, int16_t *temp, int16_t *hum)
{
    for (int retries = 0; retries < SHT1X_SAUL_RETRIES; retries++) {
        switch (sht1x_read(dev, temp, hum)) {
        case 0:
            return 0;
        case -EBADMSG:
            puts("[sht1x] CRC");
            continue;
        case -EPROTO:
            puts("[sht1x] Sensor did not acknowledge measurement command");
            continue;
        case -ECANCELED:
            puts("[sht1x] Measurement times out");
            /* falls through */
        default:
            /* Other failure, cannot recover so giving up */
            return -1;
        }
    }

    printf("[sht1x] Giving up after %i tries\n", SHT1X_SAUL_RETRIES);

    return -1;
}

static int read_temp(const void *dev, phydat_t *res)
{
    if (read(dev, &res->val[0], NULL) == 0) {
        res->unit = UNIT_TEMP_C;
        res->scale = -2;
        return 1;
    }

    return -ECANCELED;
}

static int read_hum(const void *dev, phydat_t *res)
{
    if (read(dev, NULL, &res->val[0]) == 0) {
        res->unit = UNIT_PERCENT;
        res->scale = -2;
        return 1;
    }

    return -ECANCELED;
}

const saul_driver_t sht1x_saul_temp_driver = {
    .read = read_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t sht1x_saul_hum_driver = {
    .read = read_hum,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_HUM
};
