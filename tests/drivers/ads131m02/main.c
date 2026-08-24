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
#include <math.h>

#include "ads131m02.h"
#include "ads131m02_params.h"
#include "event.h"
#include "periph_cpu.h"
#include "shell.h"
#include "time_units.h"
#include "thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define ADS131M02_TEST_CLOCK_HZ          8192000
#define ADS131M02_TEST_SPS               250
#define ADS131M02_TEST_SAMPLES_NUMOF     100
#define ADS131M02_TEST_OUTLIERS_NUMOF    3 /* 3 samples are erroneous due to sinc3 filter */

typedef struct {
    event_t ev;
    ads131m02_t dev;
    int32_t chan0[ADS131M02_FIFO_LEN + 1];
    int32_t chan1[ADS131M02_FIFO_LEN + 1];
} ads131m02_event_t;

static void _drdy_event_handler(event_t *ev);

static ads131m02_event_t _ads_ev = {
    .ev = {
        .handler = _drdy_event_handler,
    },
};

/* use own event queue to get events for tets signal first, and then start event thread */
static event_queue_t _ads_ev_queue = EVENT_QUEUE_INIT_DETACHED;

static void _drdy_isr(ads131m02_t *dev)
{
    ads131m02_event_t *ads_ev = container_of(dev, ads131m02_event_t, dev);
    assert(ads_ev == &_ads_ev);
    event_post(&_ads_ev_queue, &ads_ev->ev);
}

static void _drdy_event_handler(event_t *ev)
{
    ads131m02_event_t *ads_ev = container_of(ev, ads131m02_event_t, ev);
    unsigned chan0_numof = 0, chan1_numof = 0;
    int res;
    do {
        res = ads131m02_sample(&ads_ev->dev, ads_ev->chan0, ads_ev->chan1, &chan0_numof, &chan1_numof);
        for (unsigned i = 0; i < chan0_numof; ++i) {
            DEBUG("chan0[%u] = %" PRId32 " nV\n", i, ads_ev->chan0[i]);
        }
        for (unsigned i = 0; i < chan1_numof; ++i) {
            DEBUG("chan1[%u] = %" PRId32 " nV\n", i, ads_ev->chan1[i]);
        }
        if (res < 0) {
            printf("ads131m02_sample: %d\n", res);
        }
    } while (res == -EAGAIN);
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
    else if (strcmp(argv[1], "start") == 0) {
        if (argc < 5) {
            printf("usage: ads start <channel> <sps> <abs_mv>\n");
            return 1;
        }
        long channel = strtol(argv[2], NULL, 10);
        ads131m02_start_t start;
        start.ch_mask = ADS131M02_CHANNEL_MASK(channel);
        start.sps = ADS131M02_SPS(strtol(argv[3], NULL, 10));
        start.ch[ADS131M02_CHANNEL(channel)].abs_mv = strtoul(argv[4], NULL, 10);
        if ((ret = ads131m02_start(&_ads_ev.dev, &start, ADS131M02_TEST_CLOCK_HZ)) != 0) {
            printf("ads131m02_start: %d\n", ret);
            return ret;
        }
    }
    else if (strcmp(argv[1], "stop") == 0) {
        if (argc < 3) {
            printf("usage: ads stop <channel>\n");
            return 1;
        }
        ads131m02_stop_t stop = {
            .ch_mask = ADS131M02_CHANNEL_MASK(strtol(argv[2], NULL, 10)),
        };
        if ((ret = ads131m02_stop(&_ads_ev.dev, &stop)) != 0) {
            printf("ads131m02_stop: %d\n", ret);
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
    int res;
    ads131m02_init_t init = {
        .isr_drdy = _drdy_isr,
    };
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
    ads131m02_start_t start = {
        .ch_mask = ADS131M02_CHANNEL_MASK(0) | ADS131M02_CHANNEL_MASK(1),
        .sps = ADS131M02_SPS(ADS131M02_TEST_SPS),
        .ch[0].abs_mv = ADS131M02_VREF_NV / 1000 / 1000,
        .ch[1].abs_mv = ADS131M02_VREF_NV / 1000 / 1000,
    };
    if ((res = ads131m02_start(&_ads_ev.dev, &start, ADS131M02_TEST_CLOCK_HZ))) {
        printf("ads131m02: start: %d\n", res);
        goto failure;
    }

    event_t *ev;
    unsigned ch0_out_of_range = 0;
    unsigned ch1_out_of_range = 0;
    uint32_t ch0_rmse = 0;
    uint32_t ch1_rmse = 0;
    for (unsigned i = 0; i < ADS131M02_TEST_SAMPLES_NUMOF; ++i) {
        if (!(ev = event_wait_timeout(&_ads_ev_queue, (MS_PER_SEC * US_PER_MS) / ADS131M02_TEST_SPS))) {
            printf("ads131m02: sample timeout\n");
            goto failure;
        }
        ev->handler(ev);
        int32_t test_signal_nv = (2 * ADS131M02_VREF_NV) / 15 / _ads_ev.dev.gain[0];
        int32_t ch0_nv = _ads_ev.chan0[0];
        int32_t ch1_nv = _ads_ev.chan1[0];
        int32_t tolerance_nv = 10 * 1000 * 1000; /* 10 mV */
        ch0_rmse += (ch0_nv - test_signal_nv) * (ch0_nv - test_signal_nv);
        ch1_rmse += (ch1_nv - test_signal_nv) * (ch1_nv - test_signal_nv);
        if (ch0_nv < test_signal_nv - tolerance_nv || ch0_nv > test_signal_nv + tolerance_nv) {
            printf("ads131m02: channel 0 not in expected range: %" PRId32 " nV [%" PRId32 " nV, %" PRId32 " nV]\n",
                   ch0_nv, test_signal_nv - tolerance_nv, test_signal_nv + tolerance_nv);
            ch0_out_of_range++;
        }
        if (ch1_nv < test_signal_nv - tolerance_nv || ch1_nv > test_signal_nv + tolerance_nv) {
            printf("ads131m02: channel 1 not in expected range: %" PRId32 " nV [%" PRId32 " nV, %" PRId32 " nV]\n",
                   ch1_nv, test_signal_nv - tolerance_nv, test_signal_nv + tolerance_nv);
            ch1_out_of_range++;
        }
    }
    ch0_rmse = (uint32_t)sqrt((double)ch0_rmse / ADS131M02_TEST_SAMPLES_NUMOF);
    ch1_rmse = (uint32_t)sqrt((double)ch1_rmse / ADS131M02_TEST_SAMPLES_NUMOF);
    printf("ads131m02: channel 0 RMSE = %" PRIu32 " nV\n", ch0_rmse);
    printf("ads131m02: channel 1 RMSE = %" PRIu32 " nV\n", ch1_rmse);
    if (ch0_out_of_range > ADS131M02_TEST_OUTLIERS_NUMOF) {
        printf("ads131m02: channel 0 out of range %u times\n", ch0_out_of_range);
        goto failure;
    }
    if (ch1_out_of_range > ADS131M02_TEST_OUTLIERS_NUMOF) {
        printf("ads131m02: channel 1 out of range %u times\n", ch1_out_of_range);
        goto failure;
    }

    if ((res = ads131m02_standby(&_ads_ev.dev))) {
        printf("ads131m02: standby: %d\n", res);
        goto failure;
    }
    /* stale event in queue */
    if ((ev = event_wait_timeout(&_ads_ev_queue, (MS_PER_SEC * US_PER_MS) / ADS131M02_TEST_SPS)) &&
        (ev = event_wait_timeout(&_ads_ev_queue, (MS_PER_SEC * US_PER_MS) / ADS131M02_TEST_SPS))) {
        printf("ads131m02: device not in standby\n");
        goto failure;
    }

    if ((res = ads131m02_wakeup(&_ads_ev.dev))) {
        printf("ads131m02: wakeup: %d\n", res);
        goto failure;
    }
    if (!(ev = event_wait_timeout(&_ads_ev_queue, (MS_PER_SEC * US_PER_MS) / ADS131M02_TEST_SPS))) {
        printf("ads131m02: device not awake\n");
        goto failure;
    }

    ads131m02_stop_t stop = {
        .ch_mask = ADS131M02_CHANNEL_MASK(0) | ADS131M02_CHANNEL_MASK(1),
    };
    if ((res = ads131m02_stop(&_ads_ev.dev, &stop))) {
        printf("ads131m02: stop: %d\n", res);
        goto failure;
    }
    if ((ev = event_wait_timeout(&_ads_ev_queue, (MS_PER_SEC * US_PER_MS) / ADS131M02_TEST_SPS))) {
        printf("ads131m02: ADC conversion not stopped\n");
        goto failure;
    }

    ads131m02_resume_t resume = {
        .ch_mask = ADS131M02_CHANNEL_MASK(0) | ADS131M02_CHANNEL_MASK(1),
    };
    if ((res = ads131m02_resume(&_ads_ev.dev, &resume))) {
        printf("ads131m02: resume: %d\n", res);
        goto failure;
    }
    if (!(ev = event_wait_timeout(&_ads_ev_queue, (MS_PER_SEC * US_PER_MS) / ADS131M02_TEST_SPS))) {
        printf("ads131m02: ADC conversion not resumed\n");
        goto failure;
    }

    ads131m02_stop(&_ads_ev.dev, &stop);
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
