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
#include <stdlib.h>

#include "msg.h"
#include "thread.h"
#include "stdio_base.h"
#include "shell.h"
#include "periph/pdm.h"
#include "ztimer.h"

static kernel_pid_t _main_thread_pid = KERNEL_PID_UNDEF;

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

int16_t new_buf[NEW_BUF_SIZE];

static void _pdm_cb(void *arg, int16_t *buf)
{
    (void)arg;
    msg_t msg;
    msg.content.ptr = buf;
    if (_main_thread_pid != KERNEL_PID_UNDEF)
    {
        msg_send(&msg, _main_thread_pid);
    }
}

int start_recording_cmd(int argc, char **argv)
{
    if (argc != 1) {
        puts("Note: Start Recording \n");
        return 1;
    }
    (void)argv;
    _main_thread_pid = thread_getpid();
#if !PDM_DATA_PRINT_BINARY
    //puts("PDM peripheral driver test\n");
#endif

/*     msg_t temp;
    for (unsigned i = 0; i<1; i++)
    {
        msg_receive(&temp);
    }  */
    for (unsigned repeat = 0; repeat < NEW_BUF_SIZE / PDM_BUF_SIZE; repeat++) {
        msg_t msg;
        msg_receive(&msg);
        int16_t *buf = (int16_t *)msg.content.ptr;
#if PDM_DATA_PRINT_BINARY
        stdio_write((uint8_t *)buf, PDM_BUF_SIZE >> 2);
#else
    /*      printf("Start of the buffer\n");
        for (unsigned idx = 0; idx < PDM_BUF_SIZE; ++idx) {
            printf("%i\n", buf[idx]);
            buf_size = idx;
        }
        printf("End of the buffer\n");
        printf("%d\n", buf_size); */

        // Copy PDM_BUF_SIZE to NEW_BUF_SIZE repeatedly
        for (unsigned idx = 0; idx < PDM_BUF_SIZE; idx++) {
            new_buf[repeat*PDM_BUF_SIZE+idx] = buf[idx];
        }
#endif
    }
    // Parse the new_buf
    //pdm_stop();
    _main_thread_pid = KERNEL_PID_UNDEF;
    printf("Start of the new buffer\n");
    for (unsigned idx = 0; idx < NEW_BUF_SIZE ; idx++) {
        printf("%i\n", new_buf[idx]);
    }
    printf("End of the new buffer\n");
    printf("%d\n", NEW_BUF_SIZE);
    return 0;
}

SHELL_COMMAND(start,"Start Recording", start_recording_cmd);

int main(void)
{
char line_buf[SHELL_DEFAULT_BUFSIZE];
if (pdm_init(PDM_TEST_MODE, PDM_TEST_SAMPLE_RATE, PDM_TEST_GAIN, _pdm_cb, NULL) < 0) {
        puts("Failed to initialize PDM peripheral");
        return 1;
    }
    pdm_start();
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
return 0;
}
