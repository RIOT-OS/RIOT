/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Test for the walltime module with time change notifications
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 */

#include "shell.h"
#include "fmt.h"
#include "rtc_utils.h"
#include "walltime.h"

static void _time_change_cb(void *ctx, int32_t diff_sec, int16_t diff_ms)
{
    (void)ctx;
    printf("time changed by %d sec, %d ms\n", (int)diff_sec, (int)diff_ms);
}

static void _echo_cb(void *ctx, int32_t diff_sec, int16_t diff_ms)
{
    (void)diff_sec;
    (void)diff_ms;

    puts(ctx);
}

static void _add_and_remove_dummy_cb(void)
{
    walltime_change_sub_t sub_a = {
        .cb = _echo_cb,
        .ctx = "Dummy callback A",
    };
    walltime_change_sub_t sub_b = {
        .cb = _echo_cb,
        .ctx = "Dummy callback B",
    };

    walltime_change_subscribe(&sub_a);
    walltime_change_subscribe(&sub_b);

    walltime_change_unsubscribe(&sub_a);
    walltime_change_unsubscribe(&sub_b);
}

#define TEST_RES(res, func)                                         \
    if (res < 0) {                                                  \
        printf(func " failed (%d) on line %u\n", res, __LINE__);    \
        goto fail;                                                  \
    }

static int _cmd_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    struct tm now, expect;
    int res;

    res = scn_time_tm_iso8601(&expect, "2020-01-01 12:34:56", ' ');
    TEST_RES(res, "parse time");
    res = walltime_set(&expect);
    TEST_RES(res, "walltime_set()");
    res = walltime_get(&now, NULL);
    TEST_RES(res, "walltime_get()");
    res = rtc_tm_compare(&now, &expect);
    TEST_RES(res, "rtc_tm_compare()");

    res = scn_time_tm_iso8601(&expect, "2046-01-01 12:34:56", ' ');
    TEST_RES(res, "parse time");
    res = walltime_set(&expect);
    TEST_RES(res, "walltime_set()");
    res = walltime_get(&now, NULL);
    TEST_RES(res, "walltime_get()");
    res = rtc_tm_compare(&now, &expect);
    TEST_RES(res, "rtc_tm_compare()");

    puts("TEST PASSED");
    return res;
fail:
    puts("TEST FAILED");
    return res;
}
SHELL_COMMAND(test, "test the walltime backend", _cmd_test);

int main(void)
{
    _add_and_remove_dummy_cb();

    static walltime_change_sub_t sub = {
        .cb = _time_change_cb,
    };
    walltime_change_subscribe(&sub);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
