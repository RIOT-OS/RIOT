/**
 * hardware timer abstraction
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup kernel
 * @{
 * @file
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <hwtimer.h>
#include <hwtimer_cpu.h>
#include <hwtimer_arch.h>

#include <kernel.h>
#include <thread.h>
#include <lifo.h>

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

static void hwtimer_wakeup(void *ptr)
{
    int pid = (int)ptr;
    thread_wakeup(pid);
}

void hwtimer_spin(unsigned long ticks)
{
    unsigned long co = hwtimer_arch_now() + ticks;

    while(hwtimer_arch_now() > co);

    while(hwtimer_arch_now() < co);
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

    for(int i = 0; i < ARCH_MAXTIMERS; i++) {
        lifo_insert(lifo, i);
    }
}

/*---------------------------------------------------------------------------*/

int hwtimer_active(void)
{
    return (! lifo_empty(lifo));
}

/*---------------------------------------------------------------------------*/

unsigned long hwtimer_now(void)
{
    return hwtimer_arch_now();
}

/*---------------------------------------------------------------------------*/

void hwtimer_wait(unsigned long ticks)
{
    if(ticks <= 6 || inISR()) {
        hwtimer_spin(ticks);
        return;
    }

    /* -2 is to adjust the real value */
    int res = hwtimer_set(ticks - 2, hwtimer_wakeup, (void*)(unsigned int)(active_thread->pid));

    if(res == -1) {
        hwtimer_spin(ticks);
        return;
    }

    thread_sleep();
}

/*---------------------------------------------------------------------------*/


static int _hwtimer_set(unsigned long offset, void (*callback)(void*), void *ptr, bool absolute)
{
    if(!inISR()) {
        dINT();
    }

    int n = lifo_get(lifo);

    if(n == -1) {
        if(! inISR()) {
            eINT();
        }

        puts("No hwtimer left.");
        return -1;
    }

    timer[n].callback = callback;
    timer[n].data = ptr;

    if(absolute) {
        hwtimer_arch_set_absolute(offset, n);
    }
    else {
        hwtimer_arch_set(offset, n);
    }

    lpm_prevent_sleep++;

    if(!inISR()) {
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

