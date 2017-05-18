/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Jose Ignacio Alamos <jialamos@uc.cl>
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

#ifdef MODULE_OPENTHREAD_NCP
#include "openthread/ncp.h"
#endif

#define ENABLE_DEBUG (1)
#include "debug.h"

#define OPENTHREAD_QUEUE_LEN      (8)
static msg_t _queue[OPENTHREAD_QUEUE_LEN];

static kernel_pid_t _pid;
static otInstance *sInstance;

void ot_exec_job(OT_JOB (*job)(otInstance *, void *), void *context)
{
    ot_job_t _job;

    _job.function = job;
    _job.context = context;

    msg_t msg, reply;
    msg.type = OPENTHREAD_JOB_MSG_TYPE_EVENT;
    msg.content.ptr = &_job;
    msg_send_receive(&msg, &reply, openthread_get_pid());
}

/* OpenThread will call this when switching state from empty tasklet to non-empty tasklet. */
void otSignalTaskletPending(otInstance *aInstance)
{
    /* Unused */
    (void) aInstance;
}

static void *_openthread_event_loop(void *arg)
{
    _pid = thread_getpid();

    /* enable OpenThread UART */
    otPlatUartEnable();

    /* init OpenThread */
    sInstance = otInstanceInit();

    msg_init_queue(_queue, OPENTHREAD_QUEUE_LEN);
    netdev_t *dev;
    msg_t msg, reply;

#ifdef MODULE_OPENTHREAD_CLI
    otCliUartInit(sInstance);
#else

#ifdef MODULE_OPENTHREAD_NCP
    otNcpInit(sInstance);
#endif

#endif

#if OPENTHREAD_ENABLE_DIAG
    diagInit(sInstance);
#endif

    uint8_t *buf;
    (void) buf;
    ot_job_t *job;
    while (1) {
        otTaskletsProcess(sInstance);
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
#if defined(MODULE_OPENTHREAD_CLI) || defined(MODULE_OPENTHREAD_NCP)
            case OPENTHREAD_SERIAL_MSG_TYPE_EVENT:
                /* Tell OpenThread about the receotion of a CLI command */
                buf = msg.content.ptr;
                otPlatUartReceived(buf, strlen((char *) buf));
                break;
#endif
            case OPENTHREAD_JOB_MSG_TYPE_EVENT:
                job = msg.content.ptr;
                job->function(sInstance, job->context);
                msg_reply(&msg, &reply);
                break;
        }
    }

    return NULL;
}

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    msg_t msg;

    switch (event) {
        case NETDEV_EVENT_ISR:
            assert(_pid != KERNEL_PID_UNDEF);

            msg.type = OPENTHREAD_NETDEV_MSG_TYPE_EVENT;
            msg.content.ptr = dev;

            if (msg_send(&msg, _pid) <= 0) {
                DEBUG("openthread_netdev: possibly lost interrupt.\n");
            }
            break;

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
kernel_pid_t openthread_get_pid(void)
{
    return _pid;
}

/* starts OpenThread thread */
int openthread_netdev_init(char *stack, int stacksize, char priority,
                           const char *name, netdev_t *netdev)
{
    netdev->driver->init(netdev);
    netdev->event_callback = _event_cb;

    netopt_enable_t enable = NETOPT_ENABLE;
    netdev->driver->set(netdev, NETOPT_TX_END_IRQ, &enable, sizeof(enable));

    _pid = thread_create(stack, stacksize,
                         priority, THREAD_CREATE_STACKTEST,
                         _openthread_event_loop, NULL, name);

    if (_pid <= 0) {
        return -EINVAL;
    }

    return _pid;
}
/** @} */
