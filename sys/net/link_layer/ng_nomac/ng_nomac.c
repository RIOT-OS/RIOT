/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_nomac
 * @file
 * @brief       Implementation of the NOMAC MAC protocol
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <errno.h>

#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "net/ng_nomac.h"
#include "net/ng_netbase.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

/**
 * @brief   Function called by the device driver on device events
 *
 * @param[in] event         type of event
 * @param[in] data          optional parameter
 */
static void _event_cb(ng_netdev_event_t event, void *data)
{
    DEBUG("nomac: event triggered -> RX_COMPLETE\n");
    /* NOMAC only understands the RX_COMPLETE event... */
    if (event == NETDEV_EVENT_RX_COMPLETE) {
        ng_pktsnip_t *pkt;
        ng_netreg_entry_t *sendto;

        /* get pointer to the received packet */
        pkt = (ng_pktsnip_t *)data;
        /* find out, who to send the packet to */
        sendto = ng_netreg_lookup(pkt->type, NG_NETREG_DEMUX_CTX_ALL);
        /* throw away packet if no one is interested */
        if (sendto == NULL) {
            DEBUG("nomac: unable to forward packet of type %i (sendto == NULL)\n", pkt->type);
            ng_pktbuf_release(pkt);
            return;
        }
        /* send the packet to everyone interested in it's type */
        ng_pktbuf_hold(pkt, ng_netreg_num(pkt->type, NG_NETREG_DEMUX_CTX_ALL) - 1);
        while (sendto != NULL) {
            DEBUG("nomac: sending pkt %p to PID %u\n", (void*)pkt, sendto->pid);
            ng_netapi_receive(sendto->pid, pkt);
            sendto = ng_netreg_getnext(sendto);
        }
    } else {
       DEBUG("nomac: event triggered -> unknown event (%i)\n", event);
    }
}

/**
 * @brief   Startup code and event loop of the NOMAC layer
 *
 * @param[in] args          expects a pointer to the underlying netdev device
 *
 * @return                  never returns
 */
static void *_nomac_thread(void *args)
{
    ng_netdev_t *dev = (ng_netdev_t *)args;
    ng_netapi_opt_t *opt;
    int res;
    msg_t msg, reply, msg_queue[NG_NOMAC_MSG_QUEUE_SIZE];

    /* setup the MAC layers message queue */
    msg_init_queue(msg_queue, NG_NOMAC_MSG_QUEUE_SIZE);
    /* save the PID to the device descriptor and register the device */
    dev->mac_pid = thread_getpid();
    ng_netif_add(dev->mac_pid);
    /* register the event callback with the device driver */
    dev->driver->add_event_callback(dev, _event_cb);

    /* start the event loop */
    DEBUG("nomac: starting main event loop...\n");
    while (1) {
        DEBUG("\nnomac: waiting for next incoming message (msg_receive)...\n");
        msg_receive(&msg);
        /* dispatch NETDEV and NETAPI messages */
        switch (msg.type) {
            case NG_NETDEV_MSG_TYPE_EVENT:
                DEBUG("nomac: NG_NETDEV_MSG_TYPE_EVENT received\n");
                dev->driver->isr_event(dev, msg.content.value);
                break;
            case NG_NETAPI_MSG_TYPE_SND:
                DEBUG("nomac: NG_NETAPI_MSG_TYPE_SND received\n");
                dev->driver->send_data(dev, (ng_pktsnip_t *)msg.content.ptr);
                break;
            case NG_NETAPI_MSG_TYPE_SET:
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                DEBUG("nomac: NG_NETAPI_MSG_TYPE_SET received\n");
                /* read incoming options */
                opt = (ng_netapi_opt_t *)msg.content.ptr;
                /* set option for device driver */
                res = dev->driver->set(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("nomac: response of netdev->set: %i\n", res);
                /* send reply to calling thread */
                reply.type = NG_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            case NG_NETAPI_MSG_TYPE_GET:
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                DEBUG("nomac: NG_NETAPI_MSG_TYPE_GET received\n");
                /* read incoming options */
                opt = (ng_netapi_opt_t *)msg.content.ptr;
                /* get option from device driver */
                res = dev->driver->get(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("nomac: response of netdev->get: %i\n", res);
                /* send reply to calling thread */
                reply.type = NG_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            default:
                DEBUG("nomac: unknown command %" PRIu16 "\n", msg.type);
                break;
        }
    }
    /* never reached */
    return NULL;
}

kernel_pid_t ng_nomac_init(char *stack, int stacksize, char priority,
                        const char *name, ng_netdev_t *dev)
{
    kernel_pid_t res;

    /* check if given netdev device is defined and the driver is set */
    if (dev == NULL || dev->driver == NULL) {
        return -ENODEV;
    }
    /* create new NOMAC thread */
    res = thread_create(stack, stacksize, priority, CREATE_STACKTEST,
                         _nomac_thread, (void *)dev, name);
    if (res <= 0) {
        return -EINVAL;
    }
    return res;
}
