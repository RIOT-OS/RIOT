/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Test for module pipe.
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "thread.h"
#include "pipe.h"
#include "pipe.h"

#define BYTES_TOTAL (26)

static char stacks[2][THREAD_STACKSIZE_MAIN];

static char pipe_bufs[2][6];
static ringbuffer_t rbs[2];

static pipe_t pipes[2];

static void *run_middle(void *arg)
{
    (void) arg;

    unsigned read_total = 0;
    while (read_total < BYTES_TOTAL) {
        char buf[5];
        unsigned read = pipe_read(&pipes[0], buf, sizeof(buf) - 1);
        buf[read] = 0;
        unsigned read_start = read_total;
        read_total += read;
        printf("Middle read: <%s> [%u:%u]\n", buf,
               read_start, read_total);

        unsigned written_total = 0;
        while (written_total < read) {
            int written = pipe_write(&pipes[1], &buf[written_total],
                                     read - written_total);
            written_total += written;
        }
    }

    puts("Middle done.");
    return NULL;
}

static void *run_end(void *arg)
{
    (void) arg;

    unsigned read_total = 0;
    while (read_total < BYTES_TOTAL) {
        char buf[4];
        int read = pipe_read(&pipes[1], buf, sizeof(buf) - 1);
        buf[read] = 0;
        unsigned read_start = read_total;
        read_total += read;
        printf("End read: <%s> [%u:%u]\n", buf,
               read_start, read_total);
    }

    puts("End done.");
    return NULL;
}

static unsigned min(unsigned a, unsigned b)
{
    return a < b ? a : b;
}

int main(void)
{
    puts("Start.");

    for (int i = 0; i < 2; ++i) {
        ringbuffer_init(&rbs[i], pipe_bufs[i], sizeof (pipe_bufs[i]));
        pipe_init(&pipes[i], &rbs[i], NULL);
    }

    thread_create(stacks[0], sizeof (stacks[0]),
                  THREAD_PRIORITY_MAIN,
                  THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  run_middle, NULL, "middle");
    thread_create(stacks[1], sizeof (stacks[1]),
                  THREAD_PRIORITY_MAIN,
                  THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  run_end, NULL, "end");

    unsigned total = 0;
    while (total < BYTES_TOTAL) {
        char buf[5];
        unsigned bytes_cur = min(BYTES_TOTAL - total, sizeof (buf));
        for (unsigned i = 0; i < bytes_cur; ++i) {
            buf[i] = 'A' + total;
            ++total;
        }

        unsigned written_total = 0;
        while (written_total < bytes_cur) {
            int written = pipe_write(&pipes[0], &buf[written_total],
                                     bytes_cur - written_total);
            written_total += written;
        }
    }

    puts("Main done.");
    return 0;
}
