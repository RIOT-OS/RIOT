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

#define _MSG_SUCCESS    (0x5cac)
#define _MSG_TIMEOUT    (0x5cad)

void sys_init(void)
{
    return;
}

u32_t sys_now(void)
{
    return (uint32_t)(xtimer_now_usec64() / US_PER_MS);
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
    sema_create((sema_t *)sem, (unsigned int)count);
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
        start = xtimer_now_usec64();
        int res = sema_wait_timed((sema_t *)sem, count * US_PER_MS);
        stop = xtimer_now_usec64() - start;
        if (res == -ETIMEDOUT) {
            return SYS_ARCH_TIMEOUT;
        }
        return (u32_t)(stop / US_PER_MS);
    }
    else {
        sema_wait((sema_t *)sem);
        return 0;
    }
}

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    (void)size;
    mbox_init(&mbox->mbox, mbox->msgs, SYS_MBOX_SIZE);
    return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    (void)mbox;
    return;
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    msg_t m = { .content = { .ptr = msg }, .type = _MSG_SUCCESS };

    LWIP_ASSERT("invalid mbox", sys_mbox_valid(mbox));
    mbox_put(&mbox->mbox, &m);
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    msg_t m = { .content = { .ptr = msg }, .type = _MSG_SUCCESS };

    if (mbox_try_put(&mbox->mbox, &m)) {
        return ERR_OK;
    }
    else {
        return ERR_MEM;
    }
}

static void _mbox_timeout(void *arg)
{
    msg_t m = { .type = _MSG_TIMEOUT };
    mbox_put(arg, &m);
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    msg_t m;
    xtimer_t timer = { .callback = _mbox_timeout, .arg = &mbox->mbox };
    uint64_t start, stop;

    start = xtimer_now_usec64();
    if (timeout > 0) {
        uint64_t u_timeout = (timeout * US_PER_MS);
        _xtimer_set64(&timer, (uint32_t)u_timeout, (uint32_t)(u_timeout >> 32));
    }
    mbox_get(&mbox->mbox, &m);
    stop = xtimer_now_usec64();
    xtimer_remove(&timer);  /* in case timer did not time out */
    switch (m.type) {
        case _MSG_SUCCESS:
            *msg = m.content.ptr;
            return (u32_t)((stop - start) / US_PER_MS);
        case _MSG_TIMEOUT:
            break;
        default:    /* should not happen */
            LWIP_ASSERT("invalid message received", false);
            break;
    }
    return SYS_ARCH_TIMEOUT;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    msg_t m;

    if (mbox_try_get(&mbox->mbox, &m)) {
        LWIP_ASSERT("invalid message received", (m.type == _MSG_SUCCESS));
        *msg = m.content.ptr;
        return ERR_OK;
    }
    else {
        return SYS_MBOX_EMPTY;
    }
}

/**
 * @brief   parameters to _lwip_thread_wrapper
 */
typedef struct {
    mutex_t sync;
    lwip_thread_fn thread;
    void *arg;
} _lwip_thread_params_t;

static void *_lwip_thread_wrapper(void *params_ptr)
{
    _lwip_thread_params_t *params = params_ptr;
    lwip_thread_fn thread = params->thread;
    void *arg = params->arg;
    mutex_unlock(&params->sync);
    thread(arg);
    /* TODO: free stack? */
    return NULL;
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg,
                            int stacksize, int prio)
{
    kernel_pid_t res;
    char *stack = mem_malloc((size_t)stacksize);
    _lwip_thread_params_t params = {
        .sync = MUTEX_INIT_LOCKED,
        .thread = thread,
        .arg = arg,
    };

    if (stack == NULL) {
        return ERR_MEM;
    }
    if ((res = thread_create(stack, stacksize, prio, THREAD_CREATE_STACKTEST,
                             _lwip_thread_wrapper, &params,
                             name)) <= KERNEL_PID_UNDEF) {
        abort();
    }
    mutex_lock(&params.sync);
    sched_switch((char)prio);
    return res;
}

/** @} */
