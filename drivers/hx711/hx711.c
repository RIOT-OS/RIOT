/*
 * Copyright (C) 2018 - 2020 Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sensors
 * @brief       Driver for the HX711 weigh scale ADC
 *
 * @{
 * @file
 * @brief       HX711 driver
 *
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "periph/gpio.h"
#include "periph/gpio_util.h"
#include "xtimer.h"

#include "hx711.h"
#include "hx711_params.h"

/**
 * @brief Needs to wait for device to be ready while initialization
 */
static int _hx711_is_ready(hx711_t *dev)
{
    return gpio_read(dev->params.dout) == 0;
}

static void _hx711_set_gain(hx711_t *dev, hx711_gain_t gain)
{
    dev->params.gain = gain;
    gpio_clear(dev->params.sck);
    hx711_read(dev);
}

static void _hx711_set_offset(hx711_t *dev, int32_t offset)
{
    dev->offset = offset;
}

void hx711_init(hx711_t *dev, const hx711_params_t *params)
{
    dev->params = *params;
    dev->offset = 0;

    gpio_init(params->sck, GPIO_OUT);
    gpio_init(params->dout, GPIO_IN);

    _hx711_set_gain(dev, params->gain);
}

int32_t hx711_read(hx711_t *dev)
{
    while (!_hx711_is_ready(dev)) {}

    union {
        int32_t i32;
        struct {
            uint8_t data[3];
            uint8_t fill_byte;
        } u8;
    } value;

    value.i32 = 0;

    gpio_t sck = dev->params.sck;
    gpio_t dout = dev->params.dout;

    unsigned gain = dev->params.gain;

    value.u8.data[2] = gpio_util_shiftin(dout, sck);
    value.u8.data[1] = gpio_util_shiftin(dout, sck);
    value.u8.data[0] = gpio_util_shiftin(dout, sck);

    for (unsigned i = 0; i < gain; i++) {
        gpio_set(sck);
        gpio_clear(sck);
    }

    if (value.u8.data[2] & 0x80) {
        value.u8.fill_byte = 0xFF;
    }

    return value.i32;
}

int32_t hx711_read_average(hx711_t *dev, uint8_t times)
{
    int32_t sum = 0;
    uint8_t i = times;

    while (i--) {
        sum += hx711_read(dev);
    }

    return sum / times;
}

int32_t hx711_get_value(hx711_t *dev, uint8_t times)
{
    return hx711_read_average(dev, times) - dev->offset;
}


int32_t hx711_get_units(hx711_t *dev, int8_t times)
{
    return (int32_t) (hx711_get_value(dev, times) / dev->params.divider);
}

void hx711_tare(hx711_t *dev, uint8_t times)
{
    int32_t sum = hx711_read_average(dev, times);

    _hx711_set_offset(dev, sum);
}

void hx711_power_down(hx711_t *dev)
{
    gpio_clear(dev->params.sck);
    gpio_set(dev->params.sck);
}

void hx711_power_up(hx711_t *dev)
{
    gpio_clear(dev->params.sck);
}
