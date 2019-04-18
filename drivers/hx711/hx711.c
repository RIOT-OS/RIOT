/*
 * Copyright (C) 2019 Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_hx711 HX711 digital scale sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for the HX711 digital scale sensor
 *
 * @{
 * @file
 * @brief       HX711 driver
 *
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 */

#include "periph/gpio.h"
#include "periph/gpio_util.h"
#include "xtimer.h"

#include "hx711.h"
#include "hx711_params.h"

/**
 * @brief Needs to wait for device to be ready while initialization
 */
void wait_for_ready(void)
{
    while(gpio_read(HX711_PARAM_DOUT) > 0){
        xtimer_usleep(HX711_PARAM_SLEEP_TIME);
    }
}

/**
 * @brief Gets a single 24-Bit value from the HX711.
 * @return The 24-Bit shift in value
 */
int32_t hx711_read(void)
{
    wait_for_ready();

    float value = 0;
    uint8_t data[3];
    uint8_t fill_byte = 0x00;

    data[2] = gpio_util_shiftin(HX711_PARAM_DOUT, HX711_PARAM_SCK);
    data[1] = gpio_util_shiftin(HX711_PARAM_DOUT, HX711_PARAM_SCK);
    data[0] = gpio_util_shiftin(HX711_PARAM_DOUT, HX711_PARAM_SCK);


    for (int i = 0; i < HX711_PARAM_GAIN_PULSES; i++) {
        gpio_set(HX711_PARAM_SCK);
        gpio_clear(HX711_PARAM_SCK);
    }


    if (data[2] & 0x80) {
        fill_byte = 0xFF;
    } else {
        fill_byte = 0x00;
    }

    value = ( (unsigned long)(fill_byte) << 24
              | (unsigned long)(data[2]) << 16
              | (unsigned long)(data[1]) << 8
              | (unsigned long)(data[0]) );

    return (int32_t) value;
}

void hx711_init(void)
{
    gpio_init(HX711_PARAM_SCK, GPIO_OUT);
    gpio_init(HX711_PARAM_DOUT, GPIO_IN);

    gpio_clear(HX711_PARAM_SCK);
    hx711_read();
}

/**
 * @brief Read a raw value a configurable times and return the average raw value for it
 * @param times
 * @return the average value. Always rounded up.
 */
int32_t hx711_read_average(uint8_t times)
{
    int32_t sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += hx711_read();
    }
    return (int32_t) ((sum / times) + 0.5);
}

/**
 * @brief Read a value a configurable times and return the average value. Always rounded up.
 * @param times
 * @return returns AVG(RAW_VALUE) - HX711_PARAM_OFFSET
 */
int32_t hx711_get_value(uint8_t times)
{
    return (int32_t) ((hx711_read_average(times) - HX711_PARAM_OFFSET) + 0.5);
}

/**
 * @brief Read the average of a configurable times of a cleared and scaled value. Always rounded up.
 * @param times
 * @return returns (AVG(RAW_VALUE) - HX711_PARAM_OFFSET) / HX711_PARAM_SCALE
 */
int32_t hx711_get_units(uint8_t times)
{
    return (int32_t) ((hx711_get_value(times) / HX711_PARAM_SCALE) + 0.5);
}

void hx711_power_down(void)
{
    gpio_clear(HX711_PARAM_SCK);
    gpio_set(HX711_PARAM_SCK);
}

void hx711_power_up(void)
{
    gpio_clear(HX711_PARAM_SCK);
}
