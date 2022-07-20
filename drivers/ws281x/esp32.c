/*
 * Copyright 2020 Christian Friedrich Coors
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 *
 * @file
 * @brief       Implementation of `ws281x_write_buffer()` for the ESP32x CPU
 *
 * @author      Christian Friedrich Coors <me@ccoors.de>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "ws281x.h"
#include "ws281x_params.h"
#include "ws281x_constants.h"
#include "periph_cpu.h"

#include "esp_private/esp_clk.h"
#include "hal/cpu_hal.h"
#include "soc/rtc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void ws281x_write_buffer(ws281x_t *dev, const void *buf, size_t size)
{
    assert(dev);
    const uint8_t *pos = buf;
    const uint8_t *end = pos + size;

    /* Cycles */
    uint32_t total_cycles, one_on, one_off, zero_on, zero_off, on_wait, off_wait;

    /* Current frequency */
    rtc_cpu_freq_t freq = esp_clk_cpu_freq();

    total_cycles = freq / (NS_PER_SEC / WS281X_T_DATA_NS);
    one_on = freq / (NS_PER_SEC / WS281X_T_DATA_ONE_NS);
    one_off = total_cycles - one_on;
    zero_on = freq / (NS_PER_SEC / WS281X_T_DATA_ZERO_NS);
    zero_off = total_cycles - zero_on;

    DEBUG("[ws281x] esp32 freq=%d total=%"PRIu32"\n", freq, total_cycles);
    DEBUG("[ws281x] esp32 cycles %"PRIu32"/%"PRIu32"/%"PRIu32"/%"PRIu32"\n",
          one_on, one_off, zero_on, zero_off);

    uint32_t current_wait = 0, start = 0;

    while (pos < end) {
        uint8_t data = *pos;
        for (uint8_t cnt = 8; cnt > 0; cnt--) {
            if (data & 0b10000000) {
                on_wait = one_on;
                off_wait = one_off;
            }
            else {
                on_wait = zero_on;
                off_wait = zero_off;
            }
            start = cpu_hal_get_cycle_count();
            gpio_set(dev->params.pin);
            current_wait = start + on_wait;
            while (cpu_hal_get_cycle_count() < current_wait) { }
            gpio_clear(dev->params.pin);
            start = cpu_hal_get_cycle_count();
            current_wait = start + off_wait;
            while (cpu_hal_get_cycle_count() < current_wait) { }
            data <<= 1;
        }
        pos++;
    }
}

int ws281x_init(ws281x_t *dev, const ws281x_params_t *params)
{
    if (!dev || !params || !params->buf) {
        return -EINVAL;
    }

    memset(dev, 0, sizeof(ws281x_t));
    dev->params = *params;

    if (gpio_init(dev->params.pin, GPIO_OUT)) {
        return -EIO;
    }

    return 0;
}
