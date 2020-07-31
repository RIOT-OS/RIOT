/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Test for module core_msg_prio
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>

#include "msg.h"
#include "mutex.h"
#include "thread.h"

static char stack[THREAD_STACKSIZE_DEFAULT];

static kernel_pid_t pid;
static int failed = 0;
static msg_t msg_queue[4];
static mutex_t mut = MUTEX_INIT_LOCKED;

static void *t1_func(void *args)
{
    (void)args;
    msg_t m;

    msg_init_queue(msg_queue, ARRAY_SIZE(msg_queue));

    msg_receive_prio(&m);
    if (m.content.value != 1) {
        failed |= 1;
    }

    msg_receive(&m);
    if (m.content.value != 0) {
        failed |= 1;
    }

    return NULL;
}

static void *t2_func(void *args)
{
    (void)args;
    msg_t m;

    /* t1 exited by now, we can just reuse the message queue */
    msg_init_queue(msg_queue, ARRAY_SIZE(msg_queue));
    /* allow main to continue */
    mutex_unlock(&mut);

    msg_receive(&m);
    if (m.content.value != 4) {
        failed |= 1;
    }

    msg_receive(&m);
    if (m.content.value != 2) {
        failed |= 1;
    }

    msg_receive(&m);
    if (m.content.value != 5) {
        failed |= 1;
    }

    mutex_unlock(&mut);

    return NULL;
}

int main(void)
{
    pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                        THREAD_CREATE_STACKTEST, t1_func, NULL, "t1");
    msg_t m = { .content.value = 0 };

    msg_send(&m, pid);
    m.content.value = 1;
    msg_send_prio(&m, pid);

    /* t1 exited by now, we can reuse the stack */
    pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN + 1,
                        THREAD_CREATE_STACKTEST, t2_func, NULL, "t2");
    /* allow t2 to initialize message queue */
    mutex_lock(&mut);

    m.content.value = 2;
    msg_send(&m, pid);
    m.content.value = 3;
    msg_send_prio(&m, pid);
    m.content.value = 4;
    msg_send_prio(&m, pid);
    m.content.value = 5;
    msg_send(&m, pid);

    /* allow t2 to execute */
    mutex_lock(&mut);

    if (failed) {
        puts("TEST FAILED!");
    }
    else {
        puts("Test succeeded.");
    }
    return 0;
}
