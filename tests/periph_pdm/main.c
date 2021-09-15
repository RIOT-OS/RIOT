/*
 * Copyright (C) 2020 Inria
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
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "msg.h"
#include "thread.h"
#include "stdio_base.h"

#include "periph/pdm.h"

static kernel_pid_t _main_thread_pid;

#ifndef PDM_DATA_PRINT_BINARY
#define PDM_DATA_PRINT_BINARY   (0)
#endif

#ifndef PDM_TEST_MODE
#define PDM_TEST_MODE           (PDM_MODE_MONO)
#endif

#ifndef PDM_TEST_SAMPLE_RATE
#define PDM_TEST_SAMPLE_RATE    (PDM_SAMPLE_RATE_16KHZ)
#endif

#ifndef PDM_TEST_GAIN
#define PDM_TEST_GAIN           (-10)
#endif

static void _pdm_cb(void *arg, int16_t *buf)
{
    (void)arg;
    msg_t msg;
    msg.content.ptr = buf;
    msg_send(&msg, _main_thread_pid);
}

int main(void)
{
#if !PDM_DATA_PRINT_BINARY
    puts("PDM peripheral driver test\n");
#endif

    if (pdm_init(PDM_TEST_MODE, PDM_TEST_SAMPLE_RATE, PDM_TEST_GAIN, _pdm_cb, NULL) < 0) {
        puts("Failed to initialize PDM peripheral");
        return 1;
    }

    pdm_start();

    _main_thread_pid = thread_getpid();

    while (1) {
        msg_t msg;
        msg_receive(&msg);
        int16_t *buf = (int16_t *)msg.content.ptr;
#if PDM_DATA_PRINT_BINARY
        stdio_write((uint8_t *)buf, PDM_BUF_SIZE >> 2);
#else
        for (unsigned idx = 0; idx < PDM_BUF_SIZE; ++idx) {
            printf("%i\n", buf[idx]);
        }
#endif
    }

    return 0;
}
