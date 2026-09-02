/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: MIT
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       ADC DMA test application
 *
 *              Sample ADC signals using DMA in free running mode.
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "macros/math.h"
#include "event/thread.h"
#include "periph/adc.h"
#include "shell.h"
#include "time_units.h"
#include "xtimer.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#ifndef ADC_VREF_MV
#  define ADC_VREF_MV                   1000
#endif

#ifndef CONFIG_TEST_ADC_LINE
#  define CONFIG_TEST_ADC_LINE          0
#endif

#ifndef CONFIG_TEST_ADC_BUF_SAMPLES
#  define CONFIG_TEST_ADC_BUF_SAMPLES   2048
#endif

#ifndef CONFIG_TEST_ADC_SLEEP_SEC
#  define CONFIG_TEST_ADC_SLEEP_SEC    3
#endif

typedef struct adc_process_event {
    event_t ev;
    adc_t adc;
    unsigned bits;
    unsigned avg_samples_numof;
} adc_process_event_t;

static event_queue_t *_adc_process_ev_queue = EVENT_PRIO_MEDIUM;
/* double buffering */
static uint16_t _buf[2][CONFIG_TEST_ADC_BUF_SAMPLES] __attribute__((aligned(4)));
static uint16_t *_buf_process = _buf[1];
static uint32_t _processed_numof;

static void _adc_process(event_t *arg)
{
    adc_process_event_t *adc_arg = container_of(arg, adc_process_event_t, ev);
    int32_t mean = 0;
    int32_t remainder = 0;
    size_t count = 0;
    for (unsigned i = 0; i < adc_arg->avg_samples_numof; ++i) {
        count++;
        uint16_t sample = _buf_process[i];
        /* Widen to 64-bit to prevent signed overflow on large differences */
        int64_t diff = (int64_t)sample - mean + remainder;
        mean += (int32_t)(diff / (int64_t)count);
        remainder = (int32_t)(diff % (int64_t)count);
    }
    _processed_numof++;
    printf("ADC%d: mean: %"PRId32"mV\n",
          adc_arg->adc, (mean * ADC_VREF_MV) / ((1 << adc_arg->bits) - 1));
}

static void _swap_buffers(void)
{
    if (_buf_process == _buf[0]) {
        _buf_process = _buf[1];
    }
    else {
        _buf_process = _buf[0];
    }
}

static void _adc_process_cb(void *arg)
{
    adc_process_event_t *adc_arg = (adc_process_event_t *)arg;
    if (event_is_queued(_adc_process_ev_queue, &adc_arg->ev)) {
        printf("ADC: processing too slow\n");
    }
    _swap_buffers();
    event_post(_adc_process_ev_queue, &adc_arg->ev);
    adc_dma_continue(adc_arg->adc);
}

static void _adc_dma_cb(void *arg)
{
    adc_process_event_t *adc_arg = (adc_process_event_t *)arg;
    _adc_process_cb(adc_arg);
}

int main(void)
{
    adc_continuous_begin(ADC_RES_12BIT);
    adc_process_event_t adc_arg = {
        .ev= {
            .handler = _adc_process,
        },
        .adc = CONFIG_TEST_ADC_LINE,
        .bits = 12,
        .avg_samples_numof = CONFIG_TEST_ADC_BUF_SAMPLES,
    };

    printf("Sampling ADC%d %u bits with %u samples at %" PRIu32 " sps from %" PRIu32 " Hz ADC clock\n",
           adc_arg.adc, adc_arg.bits, adc_arg.avg_samples_numof,
           adc_get_sample_rate(adc_arg.adc, 12),
           adc_get_freq());

    adc_dma_setup(adc_arg.adc, _adc_dma_cb, &adc_arg);
    uint16_t *dst[] = {_buf[0], _buf[1]};
    adc_dma_start(adc_arg.adc, dst, adc_arg.avg_samples_numof, 2);

    xtimer_msleep(CONFIG_TEST_ADC_SLEEP_SEC * MS_PER_SEC);

    adc_dma_stop(adc_arg.adc);
    adc_dma_release(adc_arg.adc);

    printf("ADC DMA test finished, processed %"PRIu32" from estimated %"PRIu32" samples\n",
           _processed_numof * adc_arg.avg_samples_numof,
           CONFIG_TEST_ADC_SLEEP_SEC * adc_get_sample_rate(adc_arg.adc, 12));

    return 0;
}
