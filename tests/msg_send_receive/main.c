/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test msg_send_receive().
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "cpu-conf.h"
#include "thread.h"

#define THREAD1_STACKSIZE   (KERNEL_CONF_STACKSIZE_PRINTF)
#define THREAD2_STACKSIZE   (KERNEL_CONF_STACKSIZE_PRINTF)

#ifndef TEST_EXECUTION_NUM
#define TEST_EXECUTION_NUM  (10)
#endif

static char thread1_stack[THREAD1_STACKSIZE];
static char thread2_stack[THREAD2_STACKSIZE];

static kernel_pid_t thread1_pid, thread2_pid;

static void *thread1(void *args)
{
    (void)args;

    msg_t msg_req = {0}, msg_resp = {0};
    int counter = 0;

    msg_resp.content.ptr = NULL;
    msg_req.content.ptr = (void *) &counter;

    for (int i = 0; i < TEST_EXECUTION_NUM; i++) {
        msg_t msg_req_pristine;
        memcpy(&msg_req_pristine, &msg_req, sizeof(msg_t));

        msg_send_receive(&msg_req, &msg_resp, thread2_pid);

        if (memcmp(&msg_req_pristine, &msg_req, sizeof(msg_req)) != 0) {
            puts("FAIL #1: msg_req was corrupted");
            break;
        }
        else if (counter != *(int *) msg_resp.content.ptr) {
            puts("FAIL #1: counter != *(int *) msg_resp.content.ptr");
            break;
        }
        else if (counter != *(int *) msg_req.content.ptr) {
            puts("FAIL #1: counter != *(int *) msg_req.content.ptr");
            break;
        }
    }

    return NULL;
}

static void *thread2(void *args)
{
    (void)args;

    msg_t msg_req = {0}, msg_resp = {0};
    int counter = 0;

    msg_resp.content.ptr = (void *) &counter;

    for (int i = 0; i < TEST_EXECUTION_NUM; i++) {
        msg_receive(&msg_req);

        ++*(int *) msg_req.content.ptr;
        ++*(int *) msg_resp.content.ptr;
        printf("Incremented counters to %d and %d\n",
               *(int *) msg_req.content.ptr, *(int *) msg_resp.content.ptr);

        msg_t msg_req_pristine, msg_resp_pristine;
        memcpy(&msg_req_pristine, &msg_req, sizeof(msg_t));
        memcpy(&msg_resp_pristine, &msg_resp, sizeof(msg_t));

        msg_reply(&msg_req, &msg_resp);

        if (memcmp(&msg_req_pristine, &msg_req, sizeof(msg_t)) != 0) {
            puts("FAIL #2: msg_req was corrupted");
            break;
        }
        else if (memcmp(&msg_resp_pristine, &msg_resp, sizeof(msg_t)) != 0) {
            puts("FAIL #2: msg_resp was corrupted");
            break;
        }
    }

    return NULL;
}

int main(void)
{
    thread2_pid = thread_create(thread2_stack, THREAD2_STACKSIZE, PRIORITY_MAIN - 1,
                                0, thread2, NULL, "thread2");
    thread1_pid = thread_create(thread1_stack, THREAD1_STACKSIZE, PRIORITY_MAIN - 2,
                                0, thread1, NULL, "thread1");
    return 0;
}
