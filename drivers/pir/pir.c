/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pir
 * @{
 *
 * @file
 * @brief       Device driver implementation for the PIR motion sensor
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include "pir.h"
#include "irq.h"
#include "thread.h"
#include "msg.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**********************************************************************
 * internal API declaration
 **********************************************************************/

static int pir_activate_int(pir_t *dev);
static void pir_callback(void *dev);
static void pir_send_msg(pir_t *dev, pir_event_t event);

/**********************************************************************
 * public API implementation
 **********************************************************************/

int pir_init(pir_t *dev, const pir_params_t *params)
{
    dev->p.gpio = params->gpio;
    dev->p.active_high = params->active_high;
    dev->msg_thread_pid = KERNEL_PID_UNDEF;

    dev->active = false;
    dev->accum_active_time = 0;
    dev->start_active_time = 0;
    dev->last_read_time = xtimer_now_usec64();

    gpio_mode_t gpio_mode;
    if (dev->p.active_high) {
        gpio_mode = GPIO_IN_PD;
    }
    else {
        gpio_mode = GPIO_IN_PU;
    }

    if (gpio_init_int(dev->p.gpio, gpio_mode, GPIO_BOTH, pir_callback, dev)) {
        return PIR_NOGPIO;
    }
    return PIR_OK;
}

pir_event_t pir_get_status(const pir_t *dev)
{
    return (((gpio_read(dev->p.gpio) > 0) == dev->p.active_high) ?
            PIR_STATUS_ACTIVE : PIR_STATUS_INACTIVE);
}

int pir_get_occupancy(pir_t *dev, int16_t *occup) {
    int irq_state = irq_disable();
    uint64_t now = xtimer_now_usec64();
    uint64_t total_time = now - dev->last_read_time;
    if (total_time == 0) {
        irq_restore(irq_state);
        return PIR_TIMEERR;
    }

    /* We were busy counting */
    if (dev->active) {
        dev->accum_active_time += (now - dev->start_active_time);
        dev->start_active_time = now;
    }
    *occup = (int16_t)((dev->accum_active_time * 10000) / total_time);
    dev->last_read_time = now;
    dev->accum_active_time = 0;
    irq_restore(irq_state);
    return PIR_OK;
}

int pir_register_thread(pir_t *dev)
{
    if (dev->msg_thread_pid != KERNEL_PID_UNDEF) {
        if (dev->msg_thread_pid != thread_getpid()) {
            DEBUG("pir_register_thread: already registered to another thread\n");
            return PIR_NOTHREAD;
        }
    }
    else {
        DEBUG("pir_register_thread: activating interrupt for %p..\n", (void *)dev);
        if (pir_activate_int(dev) != PIR_OK) {
            DEBUG("\tfailed\n");
            return PIR_NOGPIO;
        }
        DEBUG("\tsuccess\n");
    }
    dev->msg_thread_pid = thread_getpid();

    return PIR_OK;
}

/**********************************************************************
 * internal API implementation
 **********************************************************************/

static void pir_send_msg(pir_t *dev, pir_event_t event)
{
    DEBUG("pir_send_msg\n");
    msg_t m = { .type = event, .content.ptr = dev, };

    int ret = msg_send_int(&m, dev->msg_thread_pid);
    DEBUG("pir_send_msg: msg_send_int: %i\n", ret);
    switch (ret) {
        case 0:
            DEBUG("pir_send_msg: msg_thread_pid not receptive, event is lost");
            break;
        case 1:
            DEBUG("pir_send_msg: OK");
            break;
        case -1:
            DEBUG("pir_send_msg: msg_thread_pid is gone, clearing it");
            dev->msg_thread_pid = KERNEL_PID_UNDEF;
            break;
    }
    DEBUG("\n");
}

static void pir_callback(void *arg)
{
    DEBUG("pir_callback: %p\n", arg);
    pir_t *dev = (pir_t*) arg;
    bool pin_now = gpio_read(dev->p.gpio);
    uint64_t now = xtimer_now_usec64();

    /* We were busy counting */
    if (dev->active) {
        /* Add into accumulation */
        dev->accum_active_time += (now - dev->start_active_time);
    }
    /* Pin is rising */
    if (pin_now == dev->p.active_high) {
        dev->start_active_time = now;
        dev->active = true;
    /* Pin is falling */
    } else {
        dev->active = false;
    }

    if (dev->msg_thread_pid != KERNEL_PID_UNDEF) {
        pir_send_msg(dev, pir_get_status(dev));
    }
}

static int pir_activate_int(pir_t *dev)
{
    gpio_mode_t gpio_mode;
    if (dev->p.active_high) {
        gpio_mode = GPIO_IN_PD;
    }
    else {
        gpio_mode = GPIO_IN_PU;
    }

    if (gpio_init_int(dev->p.gpio, gpio_mode, GPIO_BOTH, pir_callback, dev)) {
        return PIR_NOGPIO;
    }
    return PIR_OK;
}
