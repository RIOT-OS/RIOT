/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>

#include "irq.h"
#include "tasklet.h"
#include "thread.h"
#include "thread_flags.h"

static thread_t *__tasklet_thread;
static clist_node_t tasklet_list;

static char tasklet_stack[THREAD_STACKSIZE_TASKLET];
const char *tasklet_name = "tasklet";

void tasklet_add(tasklet_t *tasklet)
{
    unsigned state = irq_disable();
    if (!tasklet->tasklet_node.next) {
        clist_rpush(&tasklet_list, &tasklet->tasklet_node);
    }
    irq_restore(state);
}

void tasklet_schedule(tasklet_t *tasklet)
{
    assert(tasklet);
    assert(__tasklet_thread);

    tasklet_add(tasklet);
    thread_flags_set(__tasklet_thread, THREAD_FLAG_TASKLET);
}

tasklet_t *tasklet_pop(void)
{
    tasklet_t *res;
    unsigned state = irq_disable();
    res = (tasklet_t *)clist_lpop(&tasklet_list);
    if(res) {
        res->tasklet_node.next = NULL;
    }
    irq_restore(state);
    return res;
}

static void *tasklet_thread(void *arg)
{
    (void) arg;
    tasklet_t *res;
    while(thread_flags_wait_any(THREAD_FLAG_TASKLET)) {
        while((res = tasklet_pop())) {
            res->task(res->arg);
        }
    }
    return NULL;
}

void tasklet_list_reset(void)
{
    memset(&tasklet_list, '\0', sizeof(clist_node_t));
}

void tasklet_thread_create(void)
{
    __tasklet_thread = (thread_t*) thread_get(thread_create(tasklet_stack, sizeof(tasklet_stack),
            THREAD_PRIORITY_TASKLET,
            THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
            tasklet_thread, NULL, tasklet_name));
}
