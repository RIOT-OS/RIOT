/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Netdev adoption for OpenThread
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @}
 */

#include <errno.h>
#include <string.h>
#include "msg.h"
#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/ip6.h"
#include "openthread/platform/alarm-milli.h"
#include "openthread/platform/uart.h"
#include "openthread/thread.h"
#include "random.h"
#include "ot.h"
#include "event.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static otInstance *sInstance;   /**< global OpenThread instance */
static netdev_t *_dev;          /**< netdev descriptor for OpenThread */
static event_queue_t ev_queue;  /**< the event queue for OpenThread */

static void _ev_isr_handler(event_t *event)
{
    (void) event;
    _dev->driver->isr(_dev);
}

static event_t ev_isr = {
    .handler = _ev_isr_handler
};

event_queue_t *openthread_get_evq(void)
{
    return &ev_queue;
}

otInstance* openthread_get_instance(void)
{
    return sInstance;
}

static void _event_cb(netdev_t *dev, netdev_event_t event) {
    switch (event) {
        case NETDEV_EVENT_ISR:
            event_post(&ev_queue, &ev_isr);
            break;
        case NETDEV_EVENT_RX_COMPLETE:
            DEBUG("openthread_netdev: Reception of a packet\n");
            recv_pkt(sInstance, dev);
            break;
        case NETDEV_EVENT_TX_COMPLETE:
#ifndef MODULE_NETDEV_NEW_API
        case NETDEV_EVENT_TX_NOACK:
        case NETDEV_EVENT_TX_MEDIUM_BUSY:
#endif
            DEBUG("openthread_netdev: Transmission of a packet\n");
            send_pkt(sInstance, dev, event);
            break;
        default:
            break;
    }
}

static void *_openthread_event_loop(void *arg)
{
    _dev = arg;
    netdev_t *netdev = arg;

    event_queue_init(&ev_queue);

    netdev->event_callback = _event_cb;
    netdev->driver->init(netdev);

    netopt_enable_t enable = NETOPT_ENABLE;
    netdev->driver->set(netdev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));

    /* init OpenThread */
    sInstance = otInstanceInitSingle();

#if defined(MODULE_OPENTHREAD_CLI_FTD) || defined(MODULE_OPENTHREAD_CLI_MTD)
    otCliUartInit(sInstance);
    /* Init default parameters */
    otPanId panid = OPENTHREAD_PANID;
    uint8_t channel = OPENTHREAD_CHANNEL;
    otLinkSetPanId(sInstance, panid);
    otLinkSetChannel(sInstance, channel);
    /* Bring up the IPv6 interface  */
    otIp6SetEnabled(sInstance, true);
    /* Start Thread protocol operation */
    otThreadSetEnabled(sInstance, true);
#else
    /* enable OpenThread UART */
    otPlatUartEnable();
#endif

#if OPENTHREAD_ENABLE_DIAG
    diagInit(sInstance);
#endif

    while (1) {
        event_loop(&ev_queue);
    }

    return NULL;
}

/* starts OpenThread thread */
int openthread_netdev_init(char *stack, int stacksize, char priority,
                           const char *name, netdev_t *netdev) {
    if (thread_create(stack, stacksize,
                         priority, 0,
                         _openthread_event_loop, netdev, name) < 0) {
        return -EINVAL;
    }

    return 0;
}
