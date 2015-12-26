/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       SAUL wrapper for direct access to GPIO pins
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "phydat.h"
#include "periph/gpio.h"


static int read(void *dev, phydat_t *res)
{
    gpio_t pin = *((gpio_t *)dev);
    res->val[0] = (gpio_read(pin)) ? 1 : 0;
    memset(&(res->val[1]), 0, 2 * sizeof(int16_t));
    res->unit = UNIT_BOOL;
    res->scale = 0;
    return 1;
}

static int write(void *dev, phydat_t *state)
{
    gpio_t pin = *((gpio_t *)dev);
    gpio_write(pin, state->val[0]);
    return 1;
}

const saul_driver_t gpio_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_ACT_SWITCH,
};
