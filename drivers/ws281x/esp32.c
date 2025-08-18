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
#include "driver/rmt_encoder.h"
#include "driver/rmt_tx.h"
#include "hal/rmt_types.h"
#include "hal/rmt_ll.h"
#include "soc/rmt_struct.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef MODULE_WS281X_ESP32_HW

static uint8_t channel;

static rmt_channel_handle_t ws821x_rmt_chn = NULL;
static rmt_encoder_handle_t ws821x_rmt_enc = NULL;

static uint8_t _rmt_channel(ws281x_t *dev)
{
    for (unsigned i = 0; i < RMT_CH_NUMOF; i++) {
        if (rmt_channel_config[i].gpio == dev->params.pin) {
            return rmt_channel_config[i].channel;
        }
    }
    assert(0);
    return 0;   /* avoid compilation problems with NDEBUG */
}
#endif

void ws281x_write_buffer(ws281x_t *dev, const void *buf, size_t size)
{
    assert(dev);

#ifdef MODULE_WS281X_ESP32_HW

    rmt_transmit_config_t tx_config = {
        .loop_count = 0,
        .flags = { .eot_level = 0, .queue_nonblocking = 1, },
    };

    rmt_transmit(ws821x_rmt_chn, ws821x_rmt_enc, buf, size, &tx_config);

#else
    const uint8_t *pos = buf;
    const uint8_t *end = pos + size;

    /* Cycles */
    uint32_t total_cycles, one_on, one_off, zero_on, zero_off, on_wait, off_wait;

    /* Current frequency */
    int freq = esp_clk_cpu_freq();

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
            while (esp_cpu_get_cycle_count() < current_wait) { }
            /* end of LOW phase and start of HIGH phase */
            start = esp_cpu_get_cycle_count();
            gpio_set(dev->params.pin);
            current_wait = start + on_wait;
            while (esp_cpu_get_cycle_count() < current_wait) { }
            /* end of HIGH phase and start of HIGH phase */
            start = esp_cpu_get_cycle_count();
            gpio_clear(dev->params.pin);
            current_wait = start + off_wait;
        }
        pos++;
    }
    /* final LOW phase */
    current_wait = esp_cpu_get_cycle_count();
    /* end of final LOW phase */
#endif
}

#ifdef MODULE_WS281X_ESP32_HW

#define WS281x_RMT_FREQ     (40 * MHZ)
#define WS281x_RMT_CYCLES   (WS281x_RMT_FREQ / (NS_PER_SEC / WS281X_T_DATA_NS))

#define WS281X_RMT_ZERO_ON  (WS281x_RMT_FREQ / (NS_PER_SEC / WS281X_T_DATA_ZERO_NS))
#define WS281X_RMT_ZERO_OFF (WS281x_RMT_CYCLES - WS281X_RMT_ZERO_ON)

#define WS281X_RMT_ONE_ON   (WS281x_RMT_FREQ / (NS_PER_SEC / WS281X_T_DATA_ONE_NS))
#define WS281X_RMT_ONE_OFF  (WS281x_RMT_CYCLES - WS281X_RMT_ONE_ON)

#define WS281X_RMT_RST_ON   ((WS281x_RMT_CYCLES * 50) >> 1)
#define WS281X_RMT_RST_OFF  ((WS281x_RMT_CYCLES * 50) >> 1)

#define WS2812_RMT_ZERO     (rmt_symbol_word_t){ .level0 = 1, .duration0 = WS281X_RMT_ZERO_ON, \
                                                 .level1 = 0, .duration1 = WS281X_RMT_ZERO_OFF, }
#define WS2812_RMT_ONE      (rmt_symbol_word_t){ .level0 = 1, .duration0 = WS281X_RMT_ONE_ON, \
                                                 .level1 = 0, .duration1 = WS281X_RMT_ONE_OFF, }
#define WS2812_RMT_RST      (rmt_symbol_word_t){ .level0 = 1, .duration0 = WS281X_RMT_RST_ON, \
                                                 .level1 = 0, .duration1 = WS281X_RMT_RST_OFF, }

static size_t IRAM_ATTR ws2812_rmt_encoder_cb(const void *data, size_t data_size,
                                              size_t symbols_written,
                                              size_t symbols_free,
                                              rmt_symbol_word_t *symbols,
                                              bool *done, void *arg)
{
    assert(data != NULL);
    assert(symbols != NULL);
    assert(done != NULL);

    if (symbols_free < 8) {
        /* 8 symbols are required to encode one byte */
        return 0;
    }

    uint8_t *data_bytes = (uint8_t *)data;
    size_t data_pos = symbols_written >> 3;  /* current byte position in data */
    size_t symbol_num = 0;

    while ((data_pos < data_size) && (symbols_free >= 8)) {
        /* write one byte */
        uint8_t byte = data_bytes[data_pos];
        for (uint8_t cnt = 8; cnt > 0; cnt--) {
            /* write one symbol for the current bit of current data byte */
            *symbols = (byte & 0b10000000) ? WS2812_RMT_ONE : WS2812_RMT_ZERO;
            symbols++;      /* next symbol in symbol memory */
            symbol_num++;   /* number of symbols written */
            symbols_free--;
            byte <<= 1;
        }
        data_pos++;
    }

    if (data_pos == data_size) {
        *symbols = WS2812_RMT_RST;
        symbol_num++;
        *done = true;
    }

    return symbol_num;
}

#endif /* MODULE_WS281X_ESP32_HW */

int ws281x_init(ws281x_t *dev, const ws281x_params_t *params)
{
    if (!dev || !params || !params->buf) {
        return -EINVAL;
    }

    memset(dev, 0, sizeof(ws281x_t));
    dev->params = *params;

#ifdef MODULE_WS281X_ESP32_HW
    const rmt_tx_channel_config_t ws821x_rmt_chn_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = WS281x_RMT_FREQ,
        .gpio_num = params->pin,
        .trans_queue_depth = 4,
#if 0 /* SOC_RMT_SUPPORT_DMA - does not work out-of-the-box with DMA */
        .flags.with_dma = true,
        .mem_block_symbols = 64, /* can be 1.024 symbols (4 kByte) */
#else
        .flags.with_dma = false,
        .mem_block_symbols = 64, /* at least 48, increase by multiples of 48 */
#endif /* SOC_RMT_SUPPORT_DMA */
        };

    const rmt_simple_encoder_config_t ws821x_rmt_enc_config = {
            .callback = ws2812_rmt_encoder_cb,
            .min_chunk_size = 64,
    };

    static_assert(RMT_CH_NUMOF <= RMT_CH_NUMOF_MAX,
                  "[ws281x_esp32] RMT configuration problem");

    /* determine used RMT channel from configured GPIO */
    channel = _rmt_channel(dev);

    rmt_new_tx_channel(&ws821x_rmt_chn_config, &ws821x_rmt_chn);
    rmt_new_simple_encoder(&ws821x_rmt_enc_config, &ws821x_rmt_enc);
    rmt_enable(ws821x_rmt_chn);
#else
    if (gpio_init(dev->params.pin, GPIO_OUT)) {
        return -EIO;
    }
#endif
    return 0;
}
