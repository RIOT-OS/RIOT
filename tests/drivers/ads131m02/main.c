/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief      Test application for the ADS131M02 ADC driver
 *
 * @author     Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author     Fabian Hüßler <fabian.huessler@ml-pa.com>
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "ads131m02.h"
#include "ads131m02_params.h"
#include "event.h"
#include "event/timeout.h"
#include "periph_cpu.h"
#include "shell.h"
#include "time_units.h"
#include "thread.h"
#include "cib.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef ADS131M02_TEST_CLOCK_HZ
#  define ADS131M02_TEST_CLOCK_HZ           8192000
#endif
#ifndef ADS131M02_TEST_CLOCK_ACTUAL_HZ
#  define ADS131M02_TEST_CLOCK_ACTUAL_HZ    ADS131M02_TEST_CLOCK_HZ
#endif
#ifndef ADS131M02_TEST_FRAMES_NUMOF
#  define ADS131M02_TEST_FRAMES_NUMOF         32
#endif
#ifndef ADS131M02_TEST_FRAME_BUFFERS_NUMOF
#  define ADS131M02_TEST_FRAME_BUFFERS_NUMOF  4
#endif
#define ADS131M02_TEST_SPS                  8000
#define ADS131M02_TEST_DURATION_S           3
#define ADS131M02_TEST_SAMPLES_NUMOF        ((uint32_t)(ADS131M02_TEST_SPS * ADS131M02_TEST_DURATION_S))
#define ADS131M02_TEST_OUTLIERS_NUMOF       3 /* 3 samples are erroneous due to sinc3 filter */


/* The clock frequency and SPS determine the oversampling rate.
 * If the actual clock frequency is a little lower, the actual SPS is also slightly lower.
 * This leads to slightly less than expected samples.
 * To get the actual expected number of samples:
 * (Actual_Clock_Hz / 2 / OSR(SPS)) * Test_Duration_S
 * Example with desired 4000 SPS, 3 seconds duration, and actually 8MHz clock:
 * (8000000 / 2 / 1024) * 3 = 11718.75 samples instead of the expected 12000 samples.
 */
#define ADS131M02_TEST_SAMPLES_EXPECTED   \
    ((uint32_t)((ADS131M02_TEST_DURATION_S * (ADS131M02_TEST_CLOCK_ACTUAL_HZ / 2)) /  \
    (ADS131M02_TEST_CLOCK_HZ / 2 / ADS131M02_TEST_SPS)))

typedef struct {
    event_t ev;
    ads131m02_t dev;
} ads131m02_event_t;

static void _drdy_event_handler(event_t *ev);

static ads131m02_event_t _ads_ev = {
    .ev = {
        .handler = _drdy_event_handler,
    },
};

/* use own event queue to get events for tets signal first, and then start event thread */
static event_queue_t _ads_ev_queue = EVENT_QUEUE_INIT_DETACHED;
static ads131m02_frame_t _ads_frames[ADS131M02_TEST_FRAME_BUFFERS_NUMOF][ADS131M02_TEST_FRAMES_NUMOF];
static ads131m02_frame_buffer_t _ads_frame_bufs[ADS131M02_TEST_FRAME_BUFFERS_NUMOF];
static cib_t _ads_frame_bufs_cib = CIB_INIT(ADS131M02_TEST_FRAME_BUFFERS_NUMOF);

static volatile uint32_t _drdy_count;

static void _isr_drdy(ads131m02_t *dev, ads131m02_frame_buffer_t *buf)
{
    (void)buf;
    ads131m02_event_t *ads_ev = container_of(dev, ads131m02_event_t, dev);
    cib_put(&_ads_frame_bufs_cib);
    event_post(&_ads_ev_queue, &ads_ev->ev);
    _drdy_count++;
}

static void _drdy_event_handler(event_t *ev)
{
    ads131m02_event_t *ads_ev = container_of(ev, ads131m02_event_t, ev);
    (void)ads_ev;
    int peek;
    while ((peek = cib_peek(&_ads_frame_bufs_cib)) != -1) {
        unsigned status = irq_disable();
        cib_get_unsafe(&_ads_frame_bufs_cib);
        irq_restore(status);
    }
}

static void _ev_stop(event_t *ev)
{
    (void)ev;
}

#if MODULE_SHELL
static char _ads_stack[THREAD_STACKSIZE_MEDIUM];

static void *_ads_event_thread(void *arg)
{
    (void)arg;
    event_queue_init(&_ads_ev_queue);
    event_loop(&_ads_ev_queue);
    return NULL;
}

static int _cmd_ads(int argc, char **argv)
{
    int ret;
    if (argc < 2) {
help:
        printf("usage: ads start|stop|resume|standby|wakeup\n");
        return 1;
    }
    if (strcmp(argv[1], "start") == 0) {
        if (argc < 5) {
            printf("usage: ads start <channel> <sps> <abs_mv>\n");
            return 1;
        }
        long channel = strtol(argv[2], NULL, 10);
        ads131m02_start_t start;
        start.ch_mask = ADS131M02_CHANNEL_MASK(channel);
        start.sps = strtol(argv[3], NULL, 10);
        start.ch[ADS131M02_CHANNEL(channel)].abs_mv = strtoul(argv[4], NULL, 10);
        if ((ret = ads131m02_start(&_ads_ev.dev, &start, ADS131M02_TEST_CLOCK_HZ, _isr_drdy)) != 0) {
            printf("ads131m02_start: %d\n", ret);
            return ret;
        }
    }
    else if (strcmp(argv[1], "pause") == 0) {
        if (argc < 3) {
            printf("usage: ads pause <channel>\n");
            return 1;
        }
        ads131m02_pause_t pause = {
            .ch_mask = ADS131M02_CHANNEL_MASK(strtol(argv[2], NULL, 10)),
        };
        if ((ret = ads131m02_pause(&_ads_ev.dev, &pause)) != 0) {
            printf("ads131m02_pause: %d\n", ret);
            return ret;
        }
    }
    else if (strcmp(argv[1], "resume") == 0) {
        if (argc < 3) {
            printf("usage: ads resume <channel>\n");
            return 1;
        }
        ads131m02_resume_t resume = {
            .ch_mask = ADS131M02_CHANNEL_MASK(strtol(argv[2], NULL, 10)),
        };
        if ((ret = ads131m02_resume(&_ads_ev.dev, &resume)) != 0) {
            printf("ads131m02_resume: %d\n", ret);
            return ret;
        }
    }
    else if (strcmp(argv[1], "stop") == 0) {
        if (argc < 2) {
            printf("usage: ads stop\n");
            return 1;
        }
        if ((ret = ads131m02_stop(&_ads_ev.dev)) != 0) {
            printf("ads131m02_stop: %d\n", ret);
            return ret;
        }
    }
    else if (strcmp(argv[1], "standby") == 0) {
        if ((ret = ads131m02_standby(&_ads_ev.dev)) != 0) {
            printf("ads131m02_standby: %d\n", ret);
            return ret;
        }
    }
    else if (strcmp(argv[1], "wakeup") == 0) {
        if ((ret = ads131m02_wakeup(&_ads_ev.dev)) != 0) {
            printf("ads131m02_wakeup: %d\n", ret);
            return ret;
        }
    }
    else {
        goto help;
    }
    return 0;
}

SHELL_COMMAND(ads, "ADS131M02 ADC driver test", _cmd_ads);
#endif

int main(void)
{
    for (unsigned i = 0; i < ADS131M02_TEST_FRAME_BUFFERS_NUMOF; ++i) {
        _ads_frame_bufs[i].frames = _ads_frames[i];
    }
    ads131m02_init_t init = {
        .frame_bufs = _ads_frame_bufs,
        .numof = ARRAY_SIZE(_ads_frame_bufs),
    };
    int res;
    if ((res = ads131m02_init(&_ads_ev.dev, &ads131m02_params[0], &init))) {
        printf("ads131m02: init: %d\n", res);
        goto failure;
    }
    if ((res = ads131m02_mux_test_signal_positive(&_ads_ev.dev, ads131m02_channel_0))) {
        printf("ads131m02: mux test signal positive channel 0: %d\n", res);
        goto failure;
    }
    if ((res = ads131m02_mux_test_signal_positive(&_ads_ev.dev, ads131m02_channel_1))) {
        printf("ads131m02: mux test signal positive channel 1: %d\n", res);
        goto failure;
    }
    event_queue_init(&_ads_ev_queue);
    /* Either allocate all frame buffers or allocate next in drdy handler and free after processing */
    for (unsigned i = 0; i < ADS131M02_TEST_FRAME_BUFFERS_NUMOF; ++i) {
        ads131m02_frame_buffer_alloc(&_ads_ev.dev, &_ads_frame_bufs[i]);
    }
    ads131m02_start_t start = {
        .ch_mask = ADS131M02_CHANNEL_MASK(0) | ADS131M02_CHANNEL_MASK(1),
        .sps = ADS131M02_TEST_SPS,
        .ch[0].abs_mv = ADS131M02_VREF_NV / 1000 / 1000,
        .ch[1].abs_mv = ADS131M02_VREF_NV / 1000 / 1000,
    };
    event_timeout_t timeout = { 0 };
    event_t ev_timeout = { .handler = _ev_stop };
    event_timeout_init(&timeout, &_ads_ev_queue, &ev_timeout);
    if ((res = ads131m02_start(&_ads_ev.dev, &start, ADS131M02_TEST_CLOCK_HZ, _isr_drdy))) {
        printf("ads131m02: start: %d\n", res);
        goto failure;
    }
    event_timeout_set(&timeout, ADS131M02_TEST_DURATION_S * US_PER_SEC);
    unsigned ch_out_of_range[ADS131M02_CHANNELS_NUMOF] = {0};
    uint32_t ch_rmse[ADS131M02_CHANNELS_NUMOF] = {0};
    uint32_t samples_numof[ADS131M02_CHANNELS_NUMOF] = {0};
    int32_t test_signal_nv = (2 * ADS131M02_VREF_NV) / 15 / _ads_ev.dev.gain[0];
    int32_t tolerance_nv = 10 * 1000 * 1000; /* 10 mV */
    event_t *ev;
    while ((ev = event_wait(&_ads_ev_queue)) != &ev_timeout) {
        ads131m02_frame_buffer_t *buf;
        int peek;
        while((peek = cib_peek(&_ads_frame_bufs_cib)) != -1) {
            buf = &_ads_frame_bufs[peek];
            for (unsigned frame = 0; frame < ADS131M02_TEST_FRAMES_NUMOF; frame++) {
                int32_t sample[ADS131M02_CHANNELS_NUMOF];
                ads131m02_channel_mask_t ch_mask = ads131m02_parse_frame(&_ads_ev.dev, sample, true, &buf->frames[frame]);
                for (unsigned ch = 0; ch < ARRAY_SIZE(sample); ++ch) {
                    if ((ch_mask & ADS131M02_CHANNEL_MASK(ch))) {
                        samples_numof[ch]++;
                        ch_rmse[ch] += ((sample[ch] - test_signal_nv) * (sample[ch] - test_signal_nv));
                        if (sample[ch] < test_signal_nv - tolerance_nv ||
                            sample[ch] > test_signal_nv + tolerance_nv) {
                            DEBUG("ads131m02: channel %u not in expected range: "
                                "%" PRId32 " nV [%" PRId32 " nV, %" PRId32 " nV]\n",
                                ch, sample[ch],
                                test_signal_nv - tolerance_nv,
                                test_signal_nv + tolerance_nv);
                            ch_out_of_range[ch]++;
                        }
                    }
                }
            }
            unsigned state = irq_disable();
            cib_get_unsafe(&_ads_frame_bufs_cib);
            irq_restore(state);
        }
    }
    printf("Count: %" PRIu32 "\n", _drdy_count);
    for (unsigned ch = 0; ch < ADS131M02_CHANNELS_NUMOF; ++ch) {
        ch_rmse[ch] = (uint32_t)sqrt((double)ch_rmse[ch] / samples_numof[ch]);
        printf("ads131m02: channel %u RMSE = %" PRIu32 " nV, %"PRIu32"/%"PRIu32" (%"PRIu32")\n",
               ch, ch_rmse[ch], samples_numof[ch],
               ADS131M02_TEST_SAMPLES_NUMOF, ADS131M02_TEST_SAMPLES_EXPECTED);
        if (ch_out_of_range[ch] > ADS131M02_TEST_OUTLIERS_NUMOF) {
            printf("ads131m02: channel %u out of range %u times\n", ch, ch_out_of_range[ch]);
            goto failure;
        }
    }
    /* go from measuring to standby */
    ads131m02_stop(&_ads_ev.dev);
    if ((res = ads131m02_standby(&_ads_ev.dev))) {
        printf("ads131m02: standby: %d\n", res);
        goto failure;
    }
    /* at most one stale event in queue */
    if ((ev = event_get(&_ads_ev_queue)) &&
        (ev = event_wait_timeout(&_ads_ev_queue,
            ((MS_PER_SEC * US_PER_MS) * ADS131M02_TEST_FRAMES_NUMOF) / ADS131M02_TEST_SPS))) {
        printf("ads131m02: device not in standby\n");
        goto failure;
    }
    /* wake up from standby */
    if ((res = ads131m02_wakeup(&_ads_ev.dev))) {
        printf("ads131m02: wakeup: %d\n", res);
        goto failure;
    }
    /* expect data */
    cib_init(&_ads_frame_bufs_cib, ADS131M02_TEST_FRAME_BUFFERS_NUMOF);
    ads131m02_start(&_ads_ev.dev, &start, ADS131M02_TEST_CLOCK_HZ, _isr_drdy);
    if (!(ev = event_wait_timeout(&_ads_ev_queue,
            2 * ((MS_PER_SEC * US_PER_MS) * ADS131M02_TEST_FRAMES_NUMOF) / ADS131M02_TEST_SPS))) {
        printf("ads131m02: device not woken up\n");
        goto failure;
    }
    /* pause measuring */
    ads131m02_pause_t pause = {
        .ch_mask = ADS131M02_CHANNEL_MASK(0) | ADS131M02_CHANNEL_MASK(1),
    };
    if ((res = ads131m02_pause(&_ads_ev.dev, &pause))) {
        printf("ads131m02: pause: %d\n", res);
        goto failure;
    }
    /* at most one stale event in queue */
    if ((ev = event_get(&_ads_ev_queue)) &&
        (ev = event_wait_timeout(&_ads_ev_queue,
            2 * ((MS_PER_SEC * US_PER_MS) * ADS131M02_TEST_FRAMES_NUMOF) / ADS131M02_TEST_SPS))) {
        printf("ads131m02: ADC conversion not paused\n");
        goto failure;
    }
    ads131m02_resume_t resume = {
        .ch_mask = ADS131M02_CHANNEL_MASK(0) | ADS131M02_CHANNEL_MASK(1),
    };
    if ((res = ads131m02_resume(&_ads_ev.dev, &resume))) {
        printf("ads131m02: resume: %d\n", res);
        goto failure;
    }
    /* expect data after resume */
    if (!(ev = event_wait_timeout(&_ads_ev_queue,
            2  * ((MS_PER_SEC * US_PER_MS) * ADS131M02_TEST_FRAMES_NUMOF) / ADS131M02_TEST_SPS))) {
        printf("ads131m02: ADC conversion not resumed\n");
        goto failure;
    }
    /* stop everything */
    ads131m02_stop(&_ads_ev.dev);
    if ((res = ads131m02_mux_ain(&_ads_ev.dev, ads131m02_channel_0))) {
        printf("ads131m02: mux ain channel 0: %d\n", res);
        goto failure;
    }
    if ((res = ads131m02_mux_ain(&_ads_ev.dev, ads131m02_channel_1))) {
        printf("ads131m02: mux ain channel 1: %d\n", res);
        goto failure;
    }
    printf("ads131m02: Success\n");

#if MODULE_SHELL
    thread_create(_ads_stack, sizeof(_ads_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  _ads_event_thread, NULL, "ads131m02");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, sizeof(line_buf));
#else
    return 0;
#endif

failure:
    printf("ads131m02: Failure\n");
    return 1;
}
