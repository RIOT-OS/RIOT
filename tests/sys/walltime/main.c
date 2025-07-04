/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Test for the walltime module with time change notifications
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 */

#include "shell.h"
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
