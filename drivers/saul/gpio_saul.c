/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
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
#include "saul/periph.h"


static int read(const void *dev, phydat_t *res)
{
    const saul_gpio_params_t *p = (const saul_gpio_params_t *)dev;
    int inverted = (p->flags & SAUL_GPIO_INVERTED);

    res->val[0] = (gpio_read(p->pin)) ? !inverted : inverted;

    res->unit = UNIT_BOOL;
    res->scale = 0;
    return 1;
}

static int write(const void *dev, phydat_t *state)
{
    const saul_gpio_params_t *p = (const saul_gpio_params_t *)dev;
    int inverted = (p->flags & SAUL_GPIO_INVERTED);
    int value = (state->val[0] ? !inverted : inverted);

    gpio_write(p->pin, value);
    return 1;
}

const saul_driver_t gpio_out_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_ACT_SWITCH
};

const saul_driver_t gpio_in_saul_driver = {
    .read = read,
    .write = saul_notsup,
    .type = SAUL_SENSE_BTN
};
