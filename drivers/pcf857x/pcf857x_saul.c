/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pcf857x
 * @brief       PCF857X adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */
#if MODULE_SAUL_GPIO

#include <string.h>

#include "saul.h"
#include "pcf857x.h"

extern pcf857x_t pcf857x_devs[];

static int read(const void *dev, phydat_t *res)
{
    const pcf857x_saul_gpio_params_t *p = (const pcf857x_saul_gpio_params_t *)dev;
    int inverted = (p->gpio.flags & SAUL_GPIO_INVERTED);

    res->val[0] = (pcf857x_gpio_read(&pcf857x_devs[p->dev],
                                     p->gpio.pin)) ? !inverted : inverted;
    res->unit = UNIT_BOOL;
    res->scale = 0;
    return 1;
}

static int write(const void *dev, phydat_t *state)
{
    const pcf857x_saul_gpio_params_t *p = (const pcf857x_saul_gpio_params_t *)dev;
    int inverted = (p->gpio.flags & SAUL_GPIO_INVERTED);
    int value = (state->val[0] ? !inverted : inverted);

    pcf857x_gpio_write(&pcf857x_devs[p->dev], p->gpio.pin, value);
    return 1;
}

const saul_driver_t pcf857x_gpio_out_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_ACT_SWITCH
};

const saul_driver_t pcf857x_gpio_in_saul_driver = {
    .read = read,
    .write = saul_notsup,
    .type = SAUL_SENSE_BTN
};
#endif /* MODULE_SAUL_GPIO */
