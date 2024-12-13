/*
 * Copyright 2020 Christian Friedrich Coors
 *           2023 Gunar Schorcht
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
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "board.h"
#include "log.h"
#include "periph_cpu.h"

#include "ws281x.h"
#include "ws281x_params.h"
#include "ws281x_constants.h"

#include "esp_private/esp_clk.h"
#include "hal/cpu_hal.h"
#include "soc/rtc.h"

#ifdef MODULE_WS281X_ESP32_HW
#include "esp_intr_alloc.h"
#include "driver/rmt.h"
#include "hal/rmt_types.h"
#include "hal/rmt_ll.h"
#include "soc/rmt_struct.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef MODULE_WS281X_ESP32_HW

static uint32_t _ws281x_one_on;
static uint32_t _ws281x_one_off;
static uint32_t _ws281x_zero_on;
static uint32_t _ws281x_zero_off;
static uint8_t channel;

static uint8_t _rmt_channel(ws281x_t *dev)
{
    for (unsigned i = 0; i < RMT_CH_NUMOF; i++) {
        if (rmt_channel_config[i].gpio == dev->params.pin) {
            return rmt_channel_config[i].channel;
        }
    }
    assert(0);
}
#endif

void ws281x_write_buffer(ws281x_t *dev, const void *buf, size_t size)
{
    assert(dev);

#ifdef MODULE_WS281X_ESP32_HW
    rmt_write_sample(channel, (const uint8_t *)buf, size, false);
#else
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
            data <<= 1;
            while (cpu_hal_get_cycle_count() < current_wait) { }
            /* end of LOW phase and start of HIGH phase */
            start = cpu_hal_get_cycle_count();
            gpio_set(dev->params.pin);
            current_wait = start + on_wait;
            while (cpu_hal_get_cycle_count() < current_wait) { }
            /* end of HIGH phase and start of HIGH phase */
            start = cpu_hal_get_cycle_count();
            gpio_clear(dev->params.pin);
            current_wait = start + off_wait;
        }
        pos++;
    }
    /* final LOW phase */
    current_wait = cpu_hal_get_cycle_count();
    /* end of final LOW phase */
#endif
}

#ifdef MODULE_WS281X_ESP32_HW
static void IRAM_ATTR ws2812_rmt_adapter(const void *src,
                                         rmt_item32_t *dest,
                                         size_t src_size,
                                         size_t wanted_num,
                                         size_t *translated_size,
                                         size_t *item_num) {
    if ((src == NULL) || (dest == NULL)) {
        *translated_size = 0;
        *item_num = 0;
        return;
    }

    const rmt_item32_t bit0 = {{{ _ws281x_zero_on, 1, _ws281x_zero_off, 0 }}};
    const rmt_item32_t bit1 = {{{ _ws281x_one_on, 1, _ws281x_one_off, 0 }}};

    size_t size = 0;
    size_t num = 0;
    uint8_t *psrc = (uint8_t *)src;

    while ((size < src_size) && (num < wanted_num)) {
        uint8_t data = *psrc;
        for (uint8_t cnt = 8; cnt > 0; cnt--) {
            dest->val = (data & 0b10000000) ? bit1.val : bit0.val;
            dest++;
            num++;
            data <<= 1;
        }
        size++;
        psrc++;
    }
    *translated_size = size;
    *item_num = num;
}
#endif

int ws281x_init(ws281x_t *dev, const ws281x_params_t *params)
{
    if (!dev || !params || !params->buf) {
        return -EINVAL;
    }

    memset(dev, 0, sizeof(ws281x_t));
    dev->params = *params;

#ifdef MODULE_WS281X_ESP32_HW
    static_assert(RMT_CH_NUMOF <= RMT_CH_NUMOF_MAX,
                  "[ws281x_esp32] RMT configuration problem");

    /* determine used RMT channel from configured GPIO */
    channel = _rmt_channel(dev);

    rmt_config_t cfg = RMT_DEFAULT_CONFIG_TX(params->pin, channel);
    cfg.clk_div = 2;

    if ((rmt_config(&cfg) != ESP_OK) ||
        (rmt_driver_install(channel, 0, 0) != ESP_OK) ||
        (rmt_translator_init(channel, ws2812_rmt_adapter) != ESP_OK)) {
        LOG_ERROR("[ws281x_esp32] RMT initialization failed\n");
        return -EIO;
    }

    /* determine the current clock frequency */
    uint32_t freq;
    if (rmt_get_counter_clock(channel, &freq) != ESP_OK) {
        LOG_ERROR("[ws281x_esp32] Could not get RMT counter clock\n");
        return -EIO;
    }

    /* compute phase times used in ws2812_rmt_adapter */
    uint32_t total_cycles = freq / (NS_PER_SEC / WS281X_T_DATA_NS);
    _ws281x_one_on = freq / (NS_PER_SEC / WS281X_T_DATA_ONE_NS);
    _ws281x_one_off = total_cycles - _ws281x_one_on;
    _ws281x_zero_on = freq / (NS_PER_SEC / WS281X_T_DATA_ZERO_NS);
    _ws281x_zero_off = total_cycles - _ws281x_zero_on;

#else
    if (gpio_init(dev->params.pin, GPIO_OUT)) {
        return -EIO;
    }
#endif
    return 0;
}
