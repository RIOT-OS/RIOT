/**
 * hardware timer abstraction
 *
 * Copyright (C) 2010 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
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

#include <bitarithm.h>

#include <kernel.h>
#include <thread.h>

/*---------------------------------------------------------------------------*/

typedef struct hwtimer_t {
    void (*callback)(void*);
    void* data;
    uint8_t checksum;
} hwtimer_t;

typedef struct hwtimer_wait_t {
    unsigned int pid; /**< pid of waiting thread */
    uint8_t state; /**<state of waiting thread */
} hwtimer_wait_t;

#define HWTIMER_QUEUESIZE   ARCH_MAXTIMERS
#define Q_FULL              HWTIMER_QUEUESIZE + 1
#define HWTIMER_WAIT_BACKOFF    (10)

static hwtimer_t timer[HWTIMER_QUEUESIZE];
static int queue[HWTIMER_QUEUESIZE];
static short queue_head = 0;
static short queue_tail = 0;
static short queue_items = 0;

static uint8_t timer_id = 0;
static volatile long available_timers = 0;

/*---------------------------------------------------------------------------*/

static int enqueue(int item) {
    /* Test if timer is already cleared:
     * (hack to prevent race-condition with proccing timer (ISR) and manual hwtimer_remove) */
    if (available_timers & (1 << item)) {
        return 1;
    }
    queue[queue_tail] = item;
    available_timers |= (1 << item);
    queue_tail = (queue_tail + 1) % HWTIMER_QUEUESIZE;
    queue_items++;
    if (queue_items == HWTIMER_QUEUESIZE) {
        /* Allow power down */
        lpm_prevent_sleep &= ~LPM_PREVENT_SLEEP_HWTIMER; 
    }
    return 1;
}

static int dequeue(void) {
    register int ret;
    if (!queue_items)
        return Q_FULL;
    /* No power down while a timer is active */
    lpm_prevent_sleep |= LPM_PREVENT_SLEEP_HWTIMER;  
    queue_items--;
    ret = queue[queue_head];
    /* Mark as empty */
    queue[queue_head] = 0xff;
    available_timers &= ~(1 << ret);
    queue_head = (queue_head + 1) % HWTIMER_QUEUESIZE;
    return ret;
}

static void multiplexer(int source) {
    enqueue(source);
    timer[source].callback(timer[source].data);
}

static void hwtimer_wakeup(void* hwt) {
    ((hwtimer_wait_t*)hwt)->state = 0;
    while (!(thread_wakeup((*((hwtimer_wait_t*)hwt)).pid))) {
        hwtimer_set(HWTIMER_WAIT_BACKOFF, hwtimer_wakeup, (void*) &hwt);
    }
}

void hwtimer_spin(unsigned long ticks)
{
    unsigned long co = hwtimer_arch_now() + ticks;
    while (hwtimer_arch_now() > co);
    while (hwtimer_arch_now() < co);
}

/*---------------------------------------------------------------------------*/

void hwtimer_init(void) {
    hwtimer_init_comp(F_CPU);
}

/*---------------------------------------------------------------------------*/

void hwtimer_init_comp(uint32_t fcpu) {
    int i;
    queue_head = 0;
    queue_tail = 0;
    queue_items = 0;
    timer_id = 0;
    available_timers = 0;
    hwtimer_arch_init(multiplexer, fcpu);
    for (i = 0; i < HWTIMER_QUEUESIZE; i++) {
        /* init queue as empty */
        queue[i] = 0xff;
    }
    for (i = 0; i < HWTIMER_QUEUESIZE; i++) {
        enqueue(i);
    }
}

/*---------------------------------------------------------------------------*/

int hwtimer_active(void) {
    return (queue_items != HWTIMER_QUEUESIZE);
}

/*---------------------------------------------------------------------------*/

unsigned long hwtimer_now(void)
{
    return hwtimer_arch_now();
}

/*---------------------------------------------------------------------------*/

void hwtimer_wait(unsigned long ticks)
{
    if (ticks <= 6 || inISR()) {
        hwtimer_spin(ticks);
        return;
    }
    hwtimer_wait_t hwt;
    hwt.pid = active_thread->pid;
    hwt.state = 1;
    /* -2 is to adjust the real value */
    int res = hwtimer_set(ticks-2, hwtimer_wakeup, (void*) &hwt);
    if (res == -1) {
        hwtimer_spin(ticks);
        return;
    }
    while (hwt.state) {
        thread_sleep();
    }
}

/*---------------------------------------------------------------------------*/

static int _hwtimer_set(unsigned long offset, void (*callback)(void*), void *ptr, bool absolute)
{
    if (!inISR()) {
        dINT();
    }
    int x = dequeue();
    if (x == Q_FULL) {
        if (! inISR()) {
            eINT();
        }
        printf("[KT] no timers left\n");
        return -1;
    }
    
    timer[x].callback = callback;
    timer[x].data = ptr;
    timer[x].checksum = ++timer_id;

    if (absolute) {
        hwtimer_arch_set_absolute(offset, x);
    }
    else {
        hwtimer_arch_set(offset, x);
    }

    if (!inISR()) {
        eINT();
    }
    return (timer[x].checksum << 8) + x;
}

int hwtimer_set(unsigned long offset, void (*callback)(void*), void *ptr) {
    return _hwtimer_set(offset, callback, ptr, false);
}

int hwtimer_set_absolute(unsigned long offset, void (*callback)(void*), void *ptr) {
    return _hwtimer_set(offset, callback, ptr, true);
}


/*---------------------------------------------------------------------------*/

int hwtimer_remove(int x)
{
    int t = x & 0xff;
    uint8_t checksum = (uint8_t) (x >> 8);
    if (t < 0 || t >= HWTIMER_QUEUESIZE || timer[t].callback == NULL || timer[t].checksum != checksum) {
        return -1;
    }
    hwtimer_arch_disable_interrupt();
    hwtimer_arch_unset(t);
    enqueue(t);
    timer[t].callback = NULL;
    hwtimer_arch_enable_interrupt();
    return 1;
}

/*---------------------------------------------------------------------------*/

void hwtimer_debug(int timer)
{
    printf("queue size: %i\n", queue_items);
    printf("available timers: %lu\n", available_timers);
    int t = timer & 0xff;
    if (available_timers & (1 << t)) {
        printf("timer %i is: not set\n", timer);
    } else {
        printf("timer %i is: set\n", timer);
    }
}
