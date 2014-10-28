/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_hwtimer
 * @{
 *
 * @file        hwtimer.c
 * @brief       Hardware timer abstraction implementation
 *
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@fu-berlin.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "kernel.h"
#include "thread.h"
#include "lifo.h"
#include "mutex.h"
#include "irq.h"
#include "board.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "hwtimer.h"
#include "hwtimer_cpu.h"
#include "arch/hwtimer_arch.h"

/*---------------------------------------------------------------------------*/

typedef struct hwtimer_t {
    void (*callback)(void*);
    void *data;
} hwtimer_t;

static hwtimer_t timer[HWTIMER_MAXTIMERS];
static int lifo[HWTIMER_MAXTIMERS + 1];

/*---------------------------------------------------------------------------*/

static void multiplexer(int source)
{
    lifo_insert(lifo, source);
    lpm_prevent_sleep--;

    timer[source].callback(timer[source].data);
}

static void hwtimer_releasemutex(void* mutex) {
    mutex_unlock((mutex_t*) mutex);
}

void hwtimer_spin(unsigned long ticks)
{
    DEBUG("hwtimer_spin ticks=%lu\n", ticks);

    unsigned long start = hwtimer_arch_now();
    /* compute destination time, possibly resulting in an overflow */
    unsigned long stop = ((start + ticks) & HWTIMER_MAXTICKS);

    /*
     * If there is an overflow (that is: stop time is inferior to start),
     * hwtimer_arch_now needs to spin until it has overflowed as well.
     */
    if (stop < start) {
        while (hwtimer_arch_now() > start) /* do nothing */;
    }

    /* wait until we have passed destination (stop) time */
    while (hwtimer_arch_now() < stop) /* do nothing again */;
}

/*---------------------------------------------------------------------------*/

void hwtimer_init(void)
{
    hwtimer_init_comp(F_CPU);
}

/*---------------------------------------------------------------------------*/

void hwtimer_init_comp(uint32_t fcpu)
{
    hwtimer_arch_init(multiplexer, fcpu);

    lifo_init(lifo, HWTIMER_MAXTIMERS);

    for (int i = 0; i < HWTIMER_MAXTIMERS; i++) {
        lifo_insert(lifo, i);
    }
}

/*---------------------------------------------------------------------------*/

int hwtimer_active(void)
{
    return (!lifo_empty(lifo));
}

/*---------------------------------------------------------------------------*/

unsigned long hwtimer_now(void)
{
    return hwtimer_arch_now();
}

/*---------------------------------------------------------------------------*/

void hwtimer_wait(unsigned long ticks)
{
    DEBUG("hwtimer_wait ticks=%lu\n", ticks);

    if ((ticks <= (HWTIMER_SPIN_BARRIER)) || inISR()) {
        hwtimer_spin(ticks);
        return;
    }

    mutex_t mutex = MUTEX_INIT;
    mutex_lock(&mutex);
    int res = hwtimer_set(ticks - (HWTIMER_WAIT_OVERHEAD), hwtimer_releasemutex, &mutex);
    if (res == -1) {
        mutex_unlock(&mutex);
        hwtimer_spin(ticks);
        return;
    }

    /* try to lock mutex again will cause the thread to go into
     * STATUS_MUTEX_BLOCKED until hwtimer fires the releasemutex */
    mutex_lock(&mutex);
}

/*---------------------------------------------------------------------------*/


static int _hwtimer_set(unsigned long offset, void (*callback)(void*), void *ptr, bool absolute)
{
    DEBUG("_hwtimer_set: offset=%lu callback=%p ptr=%p absolute=%d\n", offset, callback, ptr, absolute);

    unsigned state;

    if (!inISR()) {
        state = disableIRQ();
    }

    int n = lifo_get(lifo);

    if (n == -1) {
        if (!inISR()) {
            restoreIRQ(state);
        }

        puts("No hwtimer left.");
        return -1;
    }

    timer[n].callback = callback;
    timer[n].data = ptr;

    if (absolute) {
        DEBUG("hwtimer_arch_set_absolute n=%d\n", n);
        hwtimer_arch_set_absolute(offset, n);
    }
    else {
        DEBUG("hwtimer_arch_set n=%d\n", n);
        hwtimer_arch_set(offset, n);
    }

    lpm_prevent_sleep++;

    if (!inISR()) {
        restoreIRQ(state);
    }

    return n;
}

int hwtimer_set(unsigned long offset, void (*callback)(void*), void *ptr)
{
    return _hwtimer_set(offset, callback, ptr, false);
}

int hwtimer_set_absolute(unsigned long offset, void (*callback)(void*), void *ptr)
{
    return _hwtimer_set(offset, callback, ptr, true);
}


/*---------------------------------------------------------------------------*/

int hwtimer_remove(int n)
{
    DEBUG("hwtimer_remove n=%d\n", n);

    unsigned state = disableIRQ();
    hwtimer_arch_unset(n);

    lifo_insert(lifo, n);
    timer[n].callback = NULL;

    lpm_prevent_sleep--;

    restoreIRQ(state);

    return 1;
}
