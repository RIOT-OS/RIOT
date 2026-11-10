// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
/*
 * hal_semaphore.c
 *
 * Description: contains the RIOT implementation of the hardware
 * abstraction layer interface for semaphore functionality
 *
 */

#include "ud3tn/result.h"
#include "platform/hal_semaphore.h"
#include "platform/hal_types.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "sema.h"
#include "ztimer.h"

void hal_semaphore_init_inplace(Semaphore_t sem, int value)
{
    sema_create(&sem->sem, value);
}

void hal_semaphore_take_blocking(Semaphore_t sem)
{
    sema_wait(&sem->sem);
}

void hal_semaphore_release(Semaphore_t sem)
{
    sema_post(&sem->sem);
}

bool hal_semaphore_is_blocked(Semaphore_t sem)
{
    /* Try to wait without blocking */
    if (sema_try_wait(&sem->sem) == 0) {
        sema_post(&sem->sem); // Restore if it succeeded
        return false;
    }
    return true;
}

void hal_semaphore_delete(Semaphore_t sem)
{
    sema_destroy(&sem->sem);
    free(sem);
}

enum ud3tn_result hal_semaphore_try_take(Semaphore_t sem, int64_t timeout_ms)
{
    if (timeout_ms < 0) {
        hal_semaphore_take_blocking(sem);
        return UD3TN_OK;
    }
    else if (timeout_ms == 0) {
        return sema_try_wait(&sem->sem) == 0 ? UD3TN_OK : UD3TN_FAIL;
    }
    else {
        int res = sema_wait_timed_ztimer(&sem->sem, ZTIMER_MSEC, timeout_ms);
        if (res == 0) {
            return UD3TN_OK;
        }
        return UD3TN_FAIL;
    }
}

Semaphore_t hal_semaphore_init(void)
{
    Semaphore_t sem = malloc(sizeof(struct Semaphore));

    if (sem) {
        hal_semaphore_init_inplace(sem, 0);
    }
    return sem;
}

Semaphore_t hal_semaphore_init_binary(void)
{
    Semaphore_t sem = malloc(sizeof(struct Semaphore));

    if (sem) {
        hal_semaphore_init_inplace(sem, 0);
    }
    return sem;
}

Semaphore_t hal_semaphore_init_value(int value)
{
    Semaphore_t sem = malloc(sizeof(struct Semaphore));

    if (sem) {
        hal_semaphore_init_inplace(sem, value);
    }
    return sem;
}
