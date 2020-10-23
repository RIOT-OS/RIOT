/*
 * Copyright (C) 2017 RWTH Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tps6274x
 * @{
 *
 * @file
 * @brief       Device driver implementation for the TPS6274x family DC/DC-converter.
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */

#include "tps6274x.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int tps6274x_init(tps6274x_t *dev, const tps6274x_params_t *params)
{
    int ret;

    dev->params = *params;
    for (uint8_t i = 0; i < 4; i++) {
        if (gpio_is_valid(dev->params.vsel[i])) {
            ret = gpio_init(dev->params.vsel[i], GPIO_OUT);
            if(ret < 0) {
                return TPS6274X_ERR_INIT;
            }
        }
    }
    if (gpio_is_valid(dev->params.ctrl_pin)) {
        ret = gpio_init(dev->params.ctrl_pin, GPIO_OUT);
        if(ret < 0) {
            return TPS6274X_ERR_INIT;
        }
    }
    return TPS6274X_OK;
}

uint16_t tps6274x_switch_voltage(tps6274x_t *dev, uint16_t voltage)
{
    if (voltage < 1800) {
        voltage = 1800;
    }
    else if (voltage > 3300) {
        voltage = 3300;
    }
    uint8_t vsel = (voltage - 1800) / 100;
    uint8_t vsel_set = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if (gpio_is_valid(dev->params.vsel[i])) {
            gpio_write(dev->params.vsel[i], (vsel & (0x01 << i)));
            /* mark pins that could and had to be set */
            vsel_set |= vsel & (1 << i);
        }
        else {
            DEBUG("[tps6274x] Pin vsel%u is not connected but is required for "
                  "selected voltage level\n", i + 1);
        }
    }
    return ((uint16_t)vsel_set) * 100 + 1800;
}

void tps6274x_load_ctrl(tps6274x_t *dev, int status)
{
    if (gpio_is_valid(dev->params.ctrl_pin)) {
        gpio_write(dev->params.ctrl_pin, status);
    }
    else {
        DEBUG("[tps6274x] CTRL Pin not defined, no load activation "
              "possible\n");
    }
}
