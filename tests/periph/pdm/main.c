/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Low-level PDM driver test
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 * @author      Bahareh Fatemi <bahareh.fatemi@haw-hamburg.de>
 * @}
 */

#include <assert.h>
#include <stdio.h>
#include <inttypes.h>

#include "macros/utils.h"
#include "periph/pdm.h"
#include "ztimer.h"

#ifndef PDM_TEST_MODE
#define PDM_TEST_MODE (PDM_MODE_MONO)
#endif

#ifndef PDM_TEST_GAIN
#define PDM_TEST_GAIN (-10)
#endif

#ifndef RECORD_SAMPLE_RATE
#define RECORD_SAMPLE_RATE (16000)
#endif

#ifndef RECORD_TIME_IN_MS
#define RECORD_TIME_IN_MS 2000
#endif

#define APP_BUF_SIZE (RECORD_SAMPLE_RATE * RECORD_TIME_IN_MS / 1000)

static int16_t _buf[(uint32_t)APP_BUF_SIZE];
static volatile unsigned _buf_idx = 0;

static void _pdm_cb(void *arg, int16_t *buf)
{
    (void)arg;

    for (unsigned idx = 0; idx < PDM_BUF_SIZE; idx++) {
        if (_buf_idx < APP_BUF_SIZE) {
            _buf[_buf_idx++] = buf[idx];
        }
    }
}

uint32_t _setup_and_record(uint32_t sample_rate, unsigned samples)
{
    uint32_t start, end;
    _buf_idx = 0;
    if (pdm_init(PDM_TEST_MODE, sample_rate, PDM_TEST_GAIN, _pdm_cb, NULL) < 0)
    {
        puts("Failed to initialize PDM peripheral");
        assert(0);
    }
    pdm_start();
    start = ztimer_now(ZTIMER_USEC);
    /* dumb wait allows the fastest response */
    while (_buf_idx < samples);
    end = ztimer_now(ZTIMER_USEC);
    pdm_stop();
    return end - start;
}

void record_and_print_sample_rates(uint32_t sample_rate) {
    /* Since we want to only test sample rates, lets try not to run this */
    /* too long. Hopefully enough to trigger at least 4 callbacks*/
    unsigned samples = MIN(APP_BUF_SIZE, PDM_BUF_SIZE * 4);
    uint32_t time_in_us = _setup_and_record(sample_rate,
                                            MIN(APP_BUF_SIZE, samples));
    printf("{\"set_sample_rate_in_Hz\":%" PRIu32 ",", sample_rate);
    printf("\"expected_sample_rate_in_Hz\":%" PRIu32 ",",
           get_pdm_sample_rate());
    printf("\"measured_sample_rate_in_Hz\":%" PRIu32 "}\n",
           samples * 1000000 / time_in_us);
}

void record_and_print_samples(uint32_t sample_rate) {
    puts("RECORDING START");
    _setup_and_record(sample_rate, APP_BUF_SIZE);
    puts("RECORDING STOP");
    printf("{\"expected_sample_rate_in_Hz\":%" PRIu32 ",", get_pdm_sample_rate());
    printf("\"samples\":[");
    for (unsigned idx = 0; idx < APP_BUF_SIZE; idx++) {
        printf("%i", _buf[idx]);
        if (idx < APP_BUF_SIZE - 1) {
            printf(",");
        }
    }
    puts("]}");
}

int main(void) {
    record_and_print_samples(RECORD_SAMPLE_RATE);
    record_and_print_sample_rates(0);
    record_and_print_sample_rates(1);
    /* I am too lazy to write these by hand so we can just jump through the */
    /* different rates to make sure they all work. */
    for (uint32_t i = 10000; i < 100000; i += i / 16) {
        record_and_print_sample_rates(i);
    }

    puts("PASS");
    return 0;
}
