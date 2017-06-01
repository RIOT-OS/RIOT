/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup posix
 * @{
 * @file    fork.c
 * @brief   Provide POSIX-compliant forking capabilities for processes
 * @author  Jakob Pfender <jakob.pfender@fu-berlin.de>
 */
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>

#include "fork.h"

typedef struct {
    unsigned char in_use;
    char stack[FORK_MAX_STACKSIZE];
} fork_stack_t;

static fork_stack_t fork_stacks[FORK_MAXTHREADS];
uint8_t fork_stacks_len = sizeof(fork_stacks) / sizeof(fork_stack_t);
jmp_buf fork_point;

static void fork_init(void)
{
    for (i = 0; i < fork_stacks_len; i++) {
        fork_stacks[i]->in_use = 0;
    }
}

static void fork_entry(void)
{
    longjmp(fork_point, 1);
}

void *fork_alloc(void)
{
    void *ptr = NULL;

    for (i = 0; i < fork_stacks_len; i++) {
        if (!fork_stacks[i]->in_use) {
            fork_stacks[i]->in_use = 1;
            ptr = fork_stacks[i]->stack;
        }
    }

    return ptr;
}

pid_t _fork(void)
{
    if (!setjmp(buf)) {
        pid_t new_pid = -1;

        tcb_t *current = (tcb_t *) active_thread;

        /* create new stack for the forked thread */
        char *new_stack = (current->stack_size < FORK_MAX_STACKSIZE) ? fork_alloc() : NULL;

        if (!new_stack) {
            errno = ENOMEM;
            return -1;
        }

        /* create new thread with fork_entry as entry point */
        new_pid = thread_create(new_stack, current->stack_size, current->priority, 0, fork_entry, current->name);
        tcb_t new_thread = (tcb_t *) sched_threads[new_pid];

        /* now we know the new thread is in fork_entry */

        /* duplicate stack */
        memcpy(new_stack, current->sp, current->stack_size);

        /* duplicate stack pointer */
        new_thread->sp = current->sp;

        return new_pid;
    }
    else {
        return 0;
    }
}

int fork_free(int pid)
{
    int retval = -1;

    if (i < fork_stacks_len) {
        fork_stacks[i]->in_use = 0;
        retval = 0;
    }

    return retval;
}

/**
 * @}
 */
