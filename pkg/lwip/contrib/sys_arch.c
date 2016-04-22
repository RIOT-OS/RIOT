/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "arch/cc.h"
#include "arch/sys_arch.h"
#include "lwip/err.h"
#include "lwip/mem.h"
#include "lwip/opt.h"
#include "lwip/sys.h"

#include "msg.h"
#include "sema.h"
#include "thread.h"
#include "xtimer.h"

void sys_init(void)
{
    return;
}

err_t sys_mutex_new(sys_mutex_t *mutex)
{
    mutex_init((mutex_t *)mutex);
    return ERR_OK;
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
    mutex_lock((mutex_t *)mutex);
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
    mutex_unlock((mutex_t *)mutex);
}

void sys_mutex_free(sys_mutex_t *mutex)
{
    mem_free(mutex);
}

err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    if (sema_create((sema_t *)sem, (unsigned int)count) < 0) {
        return ERR_VAL;
    }
    return ERR_OK;
}

void sys_sem_free(sys_sem_t *sem)
{
    sema_destroy((sema_t *)sem);
}

void sys_sem_signal(sys_sem_t *sem)
{
    LWIP_ASSERT("invalid semaphor", sys_sem_valid(sem));
    sema_post((sema_t *)sem);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t count)
{
    LWIP_ASSERT("invalid semaphor", sys_sem_valid(sem));
    if (count != 0) {
        uint64_t stop, start;
        start = xtimer_now64();
        int res = sema_wait_timed((sema_t *)sem, count * MS_IN_USEC);
        stop = xtimer_now64() - start;
        if (res == -ETIMEDOUT) {
            return SYS_ARCH_TIMEOUT;
        }
        return (u32_t)(stop / MS_IN_USEC);
    }
    else {
        sema_wait_timed((sema_t *)sem, 0);
        return 0;
    }
}

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    (void)size;
    mbox->waiting = 0;
    cib_init(&mbox->cib, SYS_MBOX_SIZE);
    mutex_init(&mbox->mutex);
    if (sema_create(&mbox->not_empty, 0) < 0) {
        return ERR_VAL;
    }
    if (sema_create(&mbox->not_full, 0) < 0) {
        return ERR_VAL;
    }
    return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    sema_destroy(&mbox->not_empty);
    sema_destroy(&mbox->not_full);
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    int idx;

    LWIP_ASSERT("invalid mbox", sys_mbox_valid(mbox));
    mutex_lock(&mbox->mutex);
    while ((idx = cib_put(&mbox->cib)) < 0) {
        mbox->waiting++;
        mutex_unlock(&mbox->mutex);
        sema_wait_timed(&mbox->not_full, 0);
        mutex_lock(&mbox->mutex);
        mbox->waiting--;
    }
    mbox->msgs[idx] = msg;
    if (cib_avail(&mbox->cib) == 1) {
        sema_post(&mbox->not_empty);
    }
    mutex_unlock(&mbox->mutex);
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    int idx;

    LWIP_ASSERT("invalid mbox", sys_mbox_valid(mbox));
    mutex_lock(&mbox->mutex);
    if ((idx = cib_put(&mbox->cib)) < 0) {
        mutex_unlock(&mbox->mutex);
        return ERR_MEM;
    }
    mbox->msgs[idx] = msg;
    if (cib_avail(&mbox->cib) == 1) {
        sema_post(&mbox->not_empty);
    }
    mutex_unlock(&mbox->mutex);
    return ERR_OK;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    u32_t time_needed = 0;
    unsigned int idx;

    LWIP_ASSERT("invalid mbox", sys_mbox_valid(mbox));
    mutex_lock(&mbox->mutex);
    while (cib_avail(&mbox->cib) == 0) {
        sys_sem_t *not_empty = (sys_sem_t *)(&mbox->not_empty);
        mutex_unlock(&mbox->mutex);
        if (timeout != 0) {
            time_needed = sys_arch_sem_wait(not_empty, timeout);
            if (time_needed == SYS_ARCH_TIMEOUT) {
                return SYS_ARCH_TIMEOUT;
            }
        }
        else {
            sys_arch_sem_wait(not_empty, 0);
        }
        mutex_lock(&mbox->mutex);
    }
    idx = cib_get(&mbox->cib);
    if (msg != NULL) {
        *msg = mbox->msgs[idx];
    }
    if (mbox->waiting) {
        sema_post(&mbox->not_full);
    }
    mutex_unlock(&mbox->mutex);
    return time_needed;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    int idx;

    LWIP_ASSERT("invalid mbox", sys_mbox_valid(mbox));
    mutex_lock(&mbox->mutex);
    if (cib_avail(&mbox->cib) == 0) {
        mutex_unlock(&mbox->mutex);
        return SYS_MBOX_EMPTY;
    }
    idx = cib_get(&mbox->cib);
    if (msg != NULL) {
        *msg = mbox->msgs[idx];
    }
    mutex_unlock(&mbox->mutex);
    return 0;
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg,
                            int stacksize, int prio)
{
    kernel_pid_t res;
    char *stack = mem_malloc((size_t)stacksize);

    if (stack == NULL) {
        return ERR_MEM;
    }
    if ((res = thread_create(stack, stacksize, prio, THREAD_CREATE_STACKTEST,
                             (thread_task_func_t)thread, arg, name)) <= KERNEL_PID_UNDEF) {
        abort();
    }
    sched_switch((char)prio);
    return res;
}

/** @} */
