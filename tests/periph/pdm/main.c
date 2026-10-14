/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-FileCopyrightText: 2024-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Low-level PDM driver test
 *
 * This test records into an application buffer and prints the samples, then
 * tests a range of sample rates across the achievable range. For each rate
 * it prints the requested rate, the rate the driver configured, and a measured
 * rate obtained by timing a fixed number of samples. The test script checks
 * that the measured rate matches the configured one.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 * @author      Bahareh Fatemi <bahareh.fatemi@haw-hamburg.de>
 * @author      Ben Wehrberger <ben.wehrberger@haw-hamburg.de>
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "test_utils/expect.h"
#include "time_units.h"
#include "macros/utils.h"
#include "periph/pdm.h"
#include "ztimer.h"

#ifndef PDM_TEST_MODE
#  define PDM_TEST_MODE (PDM_MODE_MONO)
#endif

#ifndef PDM_TEST_GAIN
#  define PDM_TEST_GAIN (-20)
#endif

#ifndef RECORD_SAMPLE_RATE
#  define RECORD_SAMPLE_RATE (16000)
#endif

#ifndef RECORD_TIME_IN_MS
#  define RECORD_TIME_IN_MS 2000
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

static uint32_t _setup_and_record(uint32_t sample_rate, unsigned samples,
                                  uint32_t *real_rate)
{
    uint32_t start, end;

    /* configure the peripheral and save the actually configured rate */
    _buf_idx = 0;
    int32_t rate = pdm_init(PDM_TEST_MODE, sample_rate, PDM_TEST_GAIN, _pdm_cb,
                            NULL);
    if (rate < 0) {
        puts("Failed to initialize PDM peripheral");
        expect(0);
    }
    *real_rate = (uint32_t)rate;

    /* drop first buffer so startup/filter-settling stays out of the timing */
    pdm_start();
    while (_buf_idx < PDM_BUF_SIZE) {}

    /* cap at the buffer end */
    unsigned target = _buf_idx + samples;
    if (target > APP_BUF_SIZE) {
        target = APP_BUF_SIZE;
    }

    /* dumb wait allows the fastest response */
    start = ztimer_now(ZTIMER_USEC);
    while (_buf_idx < target) {}
    end = ztimer_now(ZTIMER_USEC);

    pdm_stop();
    return end - start;
}

static void record_and_print_sample_rates(uint32_t sample_rate)
{
    /* keep the window short, a few callbacks are enough to time the rate */
    uint32_t real_rate;
    unsigned samples = MIN(APP_BUF_SIZE, PDM_BUF_SIZE * 4);
    uint32_t time_in_us = _setup_and_record(sample_rate, samples, &real_rate);
    uint32_t measured_rate = samples * US_PER_SEC / time_in_us;

    printf("{\"requested_sample_rate_in_Hz\":%" PRIu32 ",", sample_rate);
    printf("\"configured_sample_rate_in_Hz\":%" PRIu32 ",", real_rate);
    printf("\"measured_sample_rate_in_Hz\":%" PRIu32 "}\n", measured_rate);
}

static void record_and_print_samples(uint32_t sample_rate)
{
    puts("RECORDING START");
    uint32_t real_rate;
    _setup_and_record(sample_rate, APP_BUF_SIZE, &real_rate);
    puts("RECORDING STOP");
    printf("{\"configured_sample_rate_in_Hz\":%" PRIu32 ",", real_rate);
    printf("\"samples\":[");
    for (unsigned idx = 0; idx < APP_BUF_SIZE; idx++) {
        printf("%i", _buf[idx]);
        if (idx < APP_BUF_SIZE - 1) {
            printf(",");
        }
    }
    puts("]}");
}

int main(void)
{
    record_and_print_samples(RECORD_SAMPLE_RATE);

    /* edge cases, both should clamp to the minimum rate */
    record_and_print_sample_rates(0);
    record_and_print_sample_rates(1);

    /* sweep over a range of sample rates */
    for (uint32_t i = 10000; i < 60000; i += i / 16) {
        record_and_print_sample_rates(i);
    }

    puts("PASS");
    return 0;
}
