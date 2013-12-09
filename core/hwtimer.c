/**
 * hardware timer abstraction
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup kernel
 * @{
 * @file
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@fu-berlin.de>
 * @}
 */

#include <stdio.h>

#include "kernel.h"
#include "thread.h"
#include "lifo.h"
#include "mutex.h"

#include "hwtimer.h"
#include "hwtimer_cpu.h"
#include "hwtimer_arch.h"

/*---------------------------------------------------------------------------*/

typedef struct hwtimer_t {
    void (*callback)(void*);
    void *data;
} hwtimer_t;

static hwtimer_t timer[ARCH_MAXTIMERS];
static int lifo[ARCH_MAXTIMERS + 1];

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
    unsigned long t = hwtimer_arch_now();

    /**
     * If hwtimer_arch_now + ticks results in an overflow,
     * hwtimer_arch_now needs to spin until it has overflowed as well.
     *
     * If the destination time will result in an overflow, the result
     * is smaller than ticks by at least one.
     */
    if (t + ticks < ticks) {
        while (hwtimer_arch_now() > t);
    }

    /**
     * set t to destination time, possibly overflowing it
     */
    t += ticks;

    /**
     * wait until the present has past destination time t
     */
    while (hwtimer_arch_now() < t);
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

    lifo_init(lifo, ARCH_MAXTIMERS);

    for (int i = 0; i < ARCH_MAXTIMERS; i++) {
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
    mutex_t mutex;

    if (ticks <= 6 || inISR()) {
        hwtimer_spin(ticks);
        return;
    }
    mutex_init(&mutex);
    mutex_lock(&mutex);
    /* -2 is to adjust the real value */
    int res = hwtimer_set(ticks - 2, hwtimer_releasemutex, &mutex);
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
    if (!inISR()) {
        dINT();
    }

    int n = lifo_get(lifo);

    if (n == -1) {
        if (!inISR()) {
            eINT();
        }

        puts("No hwtimer left.");
        return -1;
    }

    timer[n].callback = callback;
    timer[n].data = ptr;

    if (absolute) {
        hwtimer_arch_set_absolute(offset, n);
    }
    else {
        hwtimer_arch_set(offset, n);
    }

    lpm_prevent_sleep++;

    if (!inISR()) {
        eINT();
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
    hwtimer_arch_disable_interrupt();
    hwtimer_arch_unset(n);

    lifo_insert(lifo, n);
    timer[n].callback = NULL;

    lpm_prevent_sleep--;

    hwtimer_arch_enable_interrupt();
    return 1;
}

