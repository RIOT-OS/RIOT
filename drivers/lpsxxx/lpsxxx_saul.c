/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lpsxxx
 * @{
 *
 * @file
 * @brief       LPSXXX (LPS331ap/LPS25HB/LPS22HB/LPS22HH) adaption to SAUL
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
    .write = saul_notsup,
    .type = SAUL_SENSE_PRESS,
};

const saul_driver_t lpsxxx_saul_temp_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};
