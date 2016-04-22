/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_pir
 * @{
 *
 * @file
 * @brief       Device driver implementation for the PIR motion sensor
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include "pir.h"
#include "thread.h"
#include "msg.h"

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

int pir_init(pir_t *dev, gpio_t gpio)
{
    dev->gpio_dev = gpio;
    dev->msg_thread_pid = KERNEL_PID_UNDEF;
    return gpio_init(dev->gpio_dev, GPIO_IN);
}

pir_event_t pir_get_status(pir_t *dev)
{
    return ((gpio_read(dev->gpio_dev) == 0) ? PIR_STATUS_LO : PIR_STATUS_HI);
}

int pir_register_thread(pir_t *dev)
{
    if (dev->msg_thread_pid != KERNEL_PID_UNDEF) {
        if (dev->msg_thread_pid != thread_getpid()) {
            DEBUG("pir_register_thread: already registered to another thread\n");
            return -2;
        }
    }
    else {
        DEBUG("pir_register_thread: activating interrupt for %p..\n", (void *)dev);
        if (pir_activate_int(dev) != 0) {
            DEBUG("\tfailed\n");
            return -1;
        }
        DEBUG("\tsuccess\n");
    }
    dev->msg_thread_pid = thread_getpid();

    return 0;
}

/**********************************************************************
 * internal API implementation
 **********************************************************************/

static void pir_send_msg(pir_t *dev, pir_event_t event)
{
    DEBUG("pir_send_msg\n");
    msg_t m = { .type = event, .content.ptr = (char*) dev, };

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
}

static void pir_callback(void *arg)
{
    DEBUG("pir_callback: %p\n", arg);
    pir_t *dev = (pir_t*) arg;
    if (dev->msg_thread_pid != KERNEL_PID_UNDEF) {
        pir_send_msg(dev, pir_get_status(dev));
    }
}

static int pir_activate_int(pir_t *dev)
{
    return gpio_init_int(dev->gpio_dev, GPIO_IN, GPIO_BOTH, pir_callback, dev);
}
