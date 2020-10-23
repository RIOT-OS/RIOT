/*
 * Copyright (C) 2016 TriaGnoSys GmbH
 *               2013-15 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Víctor Ariño <victor.arino@zii.aero>
 */

#include <errno.h>
#include "irq.h"
#include "assert.h"
#include "sema.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void sema_create(sema_t *sema, unsigned int value)
{
    assert(sema != NULL);

    sema->value = value;
    sema->state = SEMA_OK;
    mutex_init(&sema->mutex);
    if (value == 0) {
        mutex_lock(&sema->mutex);
    }
}

void sema_destroy(sema_t *sema)
{
    assert(sema != NULL);

    sema->state = SEMA_DESTROY;
    mutex_unlock(&sema->mutex);
}

int _sema_wait(sema_t *sema, int block, uint64_t us)
{
    assert(sema != NULL);

    if (sema->state != SEMA_OK) {
        return -ECANCELED;
    }

    int did_block = block;
    unsigned old = irq_disable();
    while ((sema->value == 0) && block) {
        irq_restore(old);
        if (us == 0) {
            mutex_lock(&sema->mutex);
        }
        else {
            uint64_t start = xtimer_now_usec64();
            block = !xtimer_mutex_lock_timeout(&sema->mutex, us);
            uint64_t elapsed = xtimer_now_usec64() - start;

            if (elapsed < us) {
                us -= elapsed;
            }
            else {
                block = 0;
            }
        }

        if (sema->state != SEMA_OK) {
            mutex_unlock(&sema->mutex);
            return -ECANCELED;
        }

        old = irq_disable();
    }

    if (sema->value == 0) {
        irq_restore(old);
        return (did_block) ? -ETIMEDOUT : -EAGAIN;
    }

    unsigned int value = --sema->value;
    irq_restore(old);

    /* only unlock mutex if it was a blocking operation */
    if (did_block && value > 0) {
        mutex_unlock(&sema->mutex);
    }

    return 0;
}

int sema_post(sema_t *sema)
{
    assert(sema != NULL);

    unsigned old = irq_disable();
    if (sema->value == UINT_MAX) {
        irq_restore(old);
        return -EOVERFLOW;
    }

    unsigned value = sema->value++;
    irq_restore(old);

    if (value == 0) {
        mutex_unlock(&sema->mutex);
    }

    return 0;
}
/** @} */
