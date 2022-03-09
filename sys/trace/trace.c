/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Freie Universität Berlin
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Execution tracing module implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "irq.h"
#include "ztimer.h"

#ifndef CONFIG_TRACE_BUFSIZE
#define CONFIG_TRACE_BUFSIZE 512
#endif

typedef struct {
    uint32_t time;
    uint32_t val;
} tracebuf_entry_t;

static tracebuf_entry_t tracebuf[CONFIG_TRACE_BUFSIZE];
static size_t tracebuf_pos;

void trace(uint32_t val)
{
    unsigned state = irq_disable();

    tracebuf[tracebuf_pos % CONFIG_TRACE_BUFSIZE] =
        (tracebuf_entry_t){ .time = ztimer_now(ZTIMER_USEC), .val = val };
    tracebuf_pos++;
    irq_restore(state);
}

void trace_dump(void)
{
    size_t n = tracebuf_pos >
               CONFIG_TRACE_BUFSIZE ? CONFIG_TRACE_BUFSIZE : tracebuf_pos;
    uint32_t t_last = 0;

    for (size_t i = 0; i < n; i++) {
        printf("n=%4lu t=%s%8" PRIu32 " v=0x%08lx\n", (unsigned long)i,
               i ? "+" : " ",
               tracebuf[i].time - t_last, (unsigned long)tracebuf[i].val);
        t_last = tracebuf[i].time;
    }
}

void trace_reset(void)
{
    unsigned state = irq_disable();

    tracebuf_pos = 0;
    irq_restore(state);
}
