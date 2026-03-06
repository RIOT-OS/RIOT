/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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

static int set_ledbar(const void *dev, const phydat_t *res)
{
    uint8_t lvl = (uint8_t)res->val[0];
    grove_ledbar_set((grove_ledbar_t *)dev, lvl);
    return 1;
}

const saul_driver_t grove_ledbar_saul_driver = {
    .read = saul_read_notsup,
    .write = set_ledbar,
    .type = SAUL_ACT_LED_RGB,
};
