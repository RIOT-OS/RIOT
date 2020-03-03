/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp23x17
 * @brief       MCP23x17 adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */
#if MODULE_SAUL_GPIO

#include <string.h>

#include "saul.h"
#include "mcp23x17.h"

extern mcp23x17_t mcp23x17_devs[];

static int read(const void *dev, phydat_t *res)
{
    const mcp23x17_saul_gpio_params_t *p = (const mcp23x17_saul_gpio_params_t *)dev;
    int inverted = (p->gpio.flags & SAUL_GPIO_INVERTED);

    res->val[0] = (mcp23x17_gpio_read(&mcp23x17_devs[p->dev],
                                     p->gpio.pin)) ? !inverted : inverted;
    res->unit = UNIT_BOOL;
    res->scale = 0;
    return 1;
}

static int write(const void *dev, phydat_t *state)
{
    const mcp23x17_saul_gpio_params_t *p = (const mcp23x17_saul_gpio_params_t *)dev;
    int inverted = (p->gpio.flags & SAUL_GPIO_INVERTED);
    int value = (state->val[0] ? !inverted : inverted);

    mcp23x17_gpio_write(&mcp23x17_devs[p->dev], p->gpio.pin, value);
    return 1;
}

const saul_driver_t mcp23x17_gpio_out_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_ACT_SWITCH
};

const saul_driver_t mcp23x17_gpio_in_saul_driver = {
    .read = read,
    .write = saul_notsup,
    .type = SAUL_SENSE_BTN
};
#endif /* MODULE_SAUL_GPIO */
