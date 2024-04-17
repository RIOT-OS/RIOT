/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ws281x
 * @{
 *
 * @file
 * @brief       NEOPixel/ws281x adaption to SAUL
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "ws281x.h"

static int set_rgb_led(const void *dev, const phydat_t *res)
{
    ws281x_t *ws281x = (ws281x_t *)dev;
    color_rgb_t color = {
        .r = res->val[0],
        .g = res->val[1],
        .b = res->val[2]
    };
    for (unsigned idx = 0; idx < ws281x->params.numof; idx++) {
        puts("Setting LED");
        ws281x_set(ws281x, idx, color);
    }
    ws281x_write(ws281x);
    return 1;
}

const saul_driver_t ws281x_saul_driver = {
    .read = saul_read_notsup,
    .write = set_rgb_led,
    .type = SAUL_ACT_LED_RGB,
};
