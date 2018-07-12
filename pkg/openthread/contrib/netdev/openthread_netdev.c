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
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include "msg.h"
#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/ip6.h"
#include "openthread/platform/alarm.h"
#include "openthread/platform/uart.h"
#include "openthread/tasklet.h"
#include "openthread/thread.h"
#include "random.h"
#include "ot.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define OPENTHREAD_QUEUE_LEN      (8)
static msg_t _queue[OPENTHREAD_QUEUE_LEN];

static kernel_pid_t _pid;
static otInstance *sInstance;

/**
 * @name    Default configuration for OpenThread network
 * @{
 */
#ifndef OPENTHREAD_PANID
#define OPENTHREAD_PANID            0x1234
#endif
#ifndef OPENTHREAD_CHANNEL
#define OPENTHREAD_CHANNEL          (26U)
#endif
/** @} */

uint8_t ot_call_command(char* command, void *arg, void* answer) {
    ot_job_t job;

    job.command = command;
    job.arg = arg;
    job.answer = answer;

    msg_t msg, reply;
    msg.type = OPENTHREAD_JOB_MSG_TYPE_EVENT;
    msg.content.ptr = &job;
    msg_send_receive(&msg, &reply, openthread_get_pid());
    return (uint8_t)reply.content.value;
}

/* OpenThread will call this when switching state from empty tasklet to non-empty tasklet. */
void otTaskletsSignalPending(otInstance *aInstance) {
    otTaskletsProcess(aInstance);
}

static void *_openthread_event_loop(void *arg) {
    (void)arg;
    _pid = thread_getpid();

    /* enable OpenThread UART */
    otPlatUartEnable();

    /* init OpenThread */
    sInstance = otInstanceInit();

    msg_init_queue(_queue, OPENTHREAD_QUEUE_LEN);
    netdev_t *dev;
    msg_t msg, reply;

    otCliUartInit(sInstance);

#if OPENTHREAD_ENABLE_DIAG
    diagInit(sInstance);
#endif

    /* Init default parameters */
    otPanId panid = OPENTHREAD_PANID;
    uint8_t channel = OPENTHREAD_CHANNEL;
    otLinkSetPanId(sInstance, panid);
    otLinkSetChannel(sInstance, channel);
    /* Bring up the IPv6 interface  */
    otIp6SetEnabled(sInstance, true);
    /* Start Thread protocol operation */
    otThreadSetEnabled(sInstance, true);

    uint8_t *buf;
    ot_job_t *job;
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            case OPENTHREAD_XTIMER_MSG_TYPE_EVENT:
                /* Tell OpenThread a time event was received */
                otPlatAlarmFired(sInstance);
                break;
            case OPENTHREAD_NETDEV_MSG_TYPE_EVENT:
                /* Received an event from driver */
                dev = msg.content.ptr;
                dev->driver->isr(dev);
                break;
            case OPENTHREAD_SERIAL_MSG_TYPE_EVENT:
                /* Tell OpenThread about the reception of a CLI command */
                buf = msg.content.ptr;
                otPlatUartReceived(buf, strlen((char *) buf));
                break;
            case OPENTHREAD_JOB_MSG_TYPE_EVENT:
                job = msg.content.ptr;
                reply.content.value = ot_exec_command(sInstance, job->command, job->arg, job->answer);
                msg_reply(&msg, &reply);
                break;
        }
    }

    return NULL;
}

static void _event_cb(netdev_t *dev, netdev_event_t event) {
    switch (event) {
        case NETDEV_EVENT_ISR:
            {
                msg_t msg;
                assert(_pid != KERNEL_PID_UNDEF);

                msg.type = OPENTHREAD_NETDEV_MSG_TYPE_EVENT;
                msg.content.ptr = dev;

                if (msg_send(&msg, _pid) <= 0) {
                    DEBUG("openthread_netdev: possibly lost interrupt.\n");
                }
                break;
            }

        case NETDEV_EVENT_RX_COMPLETE:
            DEBUG("openthread_netdev: Reception of a packet\n");
            recv_pkt(sInstance, dev);
            break;
        case NETDEV_EVENT_TX_COMPLETE:
        case NETDEV_EVENT_TX_NOACK:
        case NETDEV_EVENT_TX_MEDIUM_BUSY:
            DEBUG("openthread_netdev: Transmission of a packet\n");
            send_pkt(sInstance, dev, event);
            break;
        default:
            break;
    }
}

/* get OpenThread thread pid */
kernel_pid_t openthread_get_pid(void) {
    return _pid;
}

/* starts OpenThread thread */
int openthread_netdev_init(char *stack, int stacksize, char priority,
                           const char *name, netdev_t *netdev) {
    netdev->driver->init(netdev);
    netdev->event_callback = _event_cb;

    netopt_enable_t enable = NETOPT_ENABLE;
    netdev->driver->set(netdev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));
    netdev->driver->set(netdev, NETOPT_RX_END_IRQ, &enable, sizeof(enable));

    _pid = thread_create(stack, stacksize,
                         priority, THREAD_CREATE_STACKTEST,
                         _openthread_event_loop, NULL, name);

    if (_pid <= 0) {
        return -EINVAL;
    }

    return _pid;
}
