/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 * @file
 * @brief    Test if the queue used for messaging is fair.
 * @author   René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include <stdio.h>

#include "msg.h"
#include "sched.h"
#include "thread.h"
#include "vtimer.h"

#define STACK_SIZE (KERNEL_CONF_STACKSIZE_DEFAULT + KERNEL_CONF_STACKSIZE_MAIN)

#define NUM_CHILDREN (3)
#define NUM_ITERATIONS (10)

static char stacks[NUM_CHILDREN][STACK_SIZE];
static kernel_pid_t pids[NUM_CHILDREN];
static char names[NUM_CHILDREN][8];

static kernel_pid_t parent_pid = KERNEL_PID_UNDEF;

static void *child_fun(void *arg)
{
    printf("Start of %s.\n", (char*) arg);

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        msg_t m;
        m.type = i + 1;
        m.content.ptr = (char*) arg;
        msg_send(&m, parent_pid);
    }

    printf("End of %s.\n", (char*) arg);
    return NULL;
}

int main(void)
{
    puts("Start.");
    parent_pid = sched_active_pid;

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        snprintf(names[i], sizeof(names[i]), "child%2u", i + 1);
        pids[i] = thread_create(stacks[i],
                                sizeof(stacks[i]),
                                PRIORITY_MAIN + 1,
                                CREATE_WOUT_YIELD | CREATE_STACKTEST,
                                child_fun,
                                names[i],
                                names[i]);
    }

    int last_iteration = 0;
    for (int i = 0; i < NUM_ITERATIONS * NUM_CHILDREN; ++i) {
        msg_t m;
        msg_receive(&m);
        int cur_iteration = (int) m.type;
        char *child = (char *) m.content.ptr;

        printf("Received message from %s, iteration %d / %u: %s\n",
               child, cur_iteration, NUM_ITERATIONS,
               cur_iteration >= last_iteration ? "okay" : "ERROR");

        last_iteration = cur_iteration;
        vtimer_usleep(25 * 1000);
    }

    puts("Done.");
    return 0;
}
