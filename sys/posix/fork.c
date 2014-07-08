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
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>

#include "fork.h"

typedef unsigned int pid_t;
static sem_t fork_sem;
static mutex_t fork_mutex;

typedef struct {
    unsigned char in_use;
    char stack[FORK_MAX_STACKSIZE];
} fork_stack_t;

static fork_stack_t fork_stacks[FORK_MAXTHREADS];
uint8_t fork_stacks_len = sizeof(fork_stacks) / sizeof(fork_stack_t);
void *fork_reentry_point;

static void fork_init(void)
{
  for (i = 0; i < fork_stacks_len; i++) {
      fork_stacks[i]->in_use = 0;
  }
}

static pid_t fork_entry(void)
{
    /* signal original thread that new thread has been created, then
     * sleep
     */
    sem_post(fork_sem);
    thread_sleep();

    /* after wakeup, stack and sp have been copied. now go to reentry
     * label (return 0) in fork() method
     */
    thread_set_ip(fork_reentry_point);

    /* this is never reached */
    return thread_pid;
}

void *fork_alloc()
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

    /* wait for new thread to be created */
    sem_wait(&fork_sem);

    /* now we know the new thread is in fork_entry */

    /* duplicate stack */
    memcpy(new_stack, current->sp, current->stack_size);

    /* duplicate stack pointer */
    new_thread->sp = current->sp;

    mutex_lock(&fork_mutex);

    /* save reentry label for new thread in global variable */
    fork_reentry_point = &&fork_reentry;

    thread_wakeup(new_thread->pid);

    /* this point is only reached by original thread */
    return new_pid;

fork_reentry:
    /* new thread reenters here */
    mutex_unlock(&fork_mutex);
    return 0;
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
