/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @{
 * @file    fd.c
 * @brief   Providing implementation for close for fds defined in fd.h.
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 */
#include <errno.h>

#include "sched.h"
#include "thread.h"

#include "fd.h"
#include "unistd.h"

int close(int fildes)
{
    fd_t *fd_obj = fd_get(fildes);

    if (!fd_obj) {
        errno = EBADF;
        return -1;
    }

    if (fd_obj->close(fd_obj->fd) < 0) {
        errno = EIO;    // EINTR may not occur since RIOT has no signals yet.
        return -1;
    }

    fd_destroy(fd_obj->fd);

    return 0;
}

int usleep(useconds_t useconds)
{
    timex_t time = timex_set(0, useconds);
    timex_normalize(&time);
    vtimer_sleep(time);
    return 0;
}

unsigned int sleep(unsigned int seconds)
{
    timex_t time = timex_set(seconds, 0);
    vtimer_sleep(time);
    return 0;
}

typedef unsigned int pid_t;
static sem_t fork_sem;
static mutex_t fork_mutex;

static pid_t __attribute__((no_instrument_function)) fork_entry(void)
{
    sem_post(fork_sem);
    thread_sleep();

    /*
     * parent has moved my sp, so I return to the
     * function in which fork was called
     */
    return thread_pid;
}

pid_t __attribute__((no_instrument_function)) fork(void)
{
    mutex_lock(&fork_mutex);
    tcb_t *current = (tcb_t *) active_thread;

    char *new_stack = malloc(current->stack_size);

    int new_pid = thread_create(new_stack, current->stack_size, current->priority, 0, fork_entry, current->name);
    tcb_t new_thread = (tcb_t *) sched_threads[new_pid];
    sem_wait(&fork_sem);

    /* now we know that new thread is in fork_entry */

    /* duplicate stack */
    memcpy(new_stack, current->sp, current->stack_size);

    new_thread->sp = NULL; /* TODO */

    mutex_unlock(&fork_mutex);
    return 0;
}

/**
 * @}
 */
