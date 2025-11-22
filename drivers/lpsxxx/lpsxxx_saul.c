/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_lpsxxx
 * @{
 *
 * @file
 * @brief       LPSXXX (LPS331ap/LPS25HB/LPS22HB/LPS22HH/LPS22CH) adaption to SAUL
 *              interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "lpsxxx.h"

static int read_pres(const void *dev, phydat_t *res)
{
    if (lpsxxx_read_pres((const lpsxxx_t *)dev, (uint16_t *)&res->val[0]) == LPSXXX_OK) {
        res->unit = UNIT_PA;
        res->scale = 2;
        return 1;
    }

    return -ECANCELED;
}

static int read_temp(const void *dev, phydat_t *res)
{
    if (lpsxxx_read_temp((const lpsxxx_t *)dev, &res->val[0]) == LPSXXX_OK) {
        res->unit = UNIT_TEMP_C;
        res->scale = -2;
        return 1;
    }

    return -ECANCELED;
}

const saul_driver_t lpsxxx_saul_pres_driver = {
    .read = read_pres,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PRESS,
};

const saul_driver_t lpsxxx_saul_temp_driver = {
    .read = read_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};
