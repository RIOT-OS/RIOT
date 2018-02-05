/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_grove_ledbar
 * @{
 *
 * @file
 * @brief       Grove LED bar adaption to SAUL
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "grove_ledbar.h"

static int set_ledbar(const void *dev, phydat_t *res)
{
    uint8_t lvl = (uint8_t)res->val[0];
    grove_ledbar_set((grove_ledbar_t *)dev, lvl);
    return 1;
}

const saul_driver_t grove_ledbar_saul_driver = {
    .read = saul_notsup,
    .write = set_ledbar,
    .type = SAUL_ACT_LED_RGB,
};
