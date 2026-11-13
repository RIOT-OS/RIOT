// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
/*
 * hal_task.c
 *
 * Description: contains the RIOT implementation of the hardware
 * abstraction layer interface for thread-related functionality
 *
 */
#include "ud3tn/common.h"
#include "ud3tn/result.h"

#include "platform/hal_io.h"
#include "platform/hal_task.h"
#include "platform/hal_time.h"
#include "platform/hal_types.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "thread.h"
#include "ztimer.h"

struct task_description {
    void (*task_function)(void *param);
    void *task_parameter;
};

static void *execute_riot_compat(void *task_description)
{
    struct task_description *desc =
        (struct task_description *)task_description;

    void (*task_function)(void *param) = desc->task_function;
    void *task_parameter = desc->task_parameter;

    free(task_description);
    task_function(task_parameter);
    return NULL;
}

enum ud3tn_result hal_task_create(
    void (*task_function)(void *),
    void *task_parameters,
    bool detach,
    TaskIdentifier_t *task_id)
{
    struct task_description *desc = malloc(sizeof(*desc));
    char *stack = malloc(THREAD_STACKSIZE_DEFAULT);

    if (desc == NULL || stack == NULL) {
        UD3TN_LOG_ERROR("Allocating the task structure or stack failed!");
        free(desc);
        free(stack);
        return UD3TN_FAIL;
    }

    desc->task_function = task_function;
    desc->task_parameter = task_parameters;

    kernel_pid_t pid = thread_create(
        stack,
        THREAD_STACKSIZE_DEFAULT,
        THREAD_PRIORITY_MAIN - 1,
        THREAD_CREATE_WOUT_YIELD,
        execute_riot_compat,
        desc,
        "ud3tn_task"
        );

    if (pid <= KERNEL_PID_UNDEF) {
        UD3TN_LOG_ERROR("Thread Creation failed!");
        free(desc);
        free(stack);
        return UD3TN_FAIL;
    }

    /* Set the task_id to pid */
    if (task_id) {
        /* non-detached handling not fully mapped in RIOT out of the box, return PID regardless */
        *task_id = (TaskIdentifier_t)(uintptr_t)pid;
    }

    return UD3TN_OK;
}

enum ud3tn_result hal_task_start_scheduler(void)
{
    while (true) {
        ztimer_sleep(ZTIMER_MSEC, 1000);
    }
    return UD3TN_OK;
}

void hal_task_delay(int delay)
{
    if (delay <= 0) {
        return;
    }
    ztimer_sleep(ZTIMER_MSEC, delay);
}
