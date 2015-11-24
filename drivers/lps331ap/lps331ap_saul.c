/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_lps331ap
 * @{
 *
 * @file
 * @brief       LPS331ap adaption to the RIOT actuator/sensor interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "lps331ap.h"

static int read(void *dev, phydat_t *res)
{
    lps331ap_t *d = (lps331ap_t *)dev;
    res->val[0] = (int16_t)lps331ap_read_pres(d);
    memset(&(res->val[1]), 0, 2 * sizeof(int16_t));
    res->unit = UNIT_BAR;
    res->scale = -3;
    return 1;
}

static int write(void *dev, phydat_t *state)
{
    return -ENOTSUP;
}

const saul_driver_t lps331ap_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_SENSE_PRESS,
};
