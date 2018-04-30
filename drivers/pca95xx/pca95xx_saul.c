/*
 * Copyright (C) 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pca95xx
 * @{
 *
 * @file
 * @brief       PCA95xx adaption to the RIOT actuator/sensor interface
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "pca95xx.h"

static int read(const void *dev, phydat_t *res)
{
    res->val = pca95xx_read((const pca95xx_t *)dev, dev->params.pin);

    res->unit = UNIT_BOOL;
    res->scale = 0;

    return 1;
}

static int write(const void *dev, phydat_t *state)
{
    pca95xx_write((const pca95xx_t *)dev, dev->params.pin, state->val);

    return 1;
}

const saul_driver_t pca95xx_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_ACT_SWITCH
};
