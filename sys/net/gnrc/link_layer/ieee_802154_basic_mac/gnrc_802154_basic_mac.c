/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_80215
 * @file
 * @brief       Implementation of the IEEE 802.15.4 basic MAC layer
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @}
 */

#include <errno.h>

#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "panic.h"
#include "net/gnrc/ieee_802154_basic_mac.h"
#include "net/gnrc/csma_sender.h"
#include "net/gnrc.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

/**
 * @brief   Function called by the device driver on device events
 *
 * @param[in] event         type of event
 * @param[in] data          optional parameter
 */
static void _event_cb(gnrc_netdev_event_t event, void *data)
{
    DEBUG("802154_basic_mac: event triggered -> %i\n", event);
    /* The 802.15.4 basic MAC only understands the RX_COMPLETE event... */
    if (event == NETDEV_EVENT_RX_COMPLETE) {
        gnrc_pktsnip_t *pkt;

        /* get pointer to the received packet */
        pkt = (gnrc_pktsnip_t *)data;
        /* send the packet to everyone interested in it's type */
        if (!gnrc_netapi_dispatch_receive(pkt->type,
                                          GNRC_NETREG_DEMUX_CTX_ALL,
                                          pkt))
        {
            DEBUG("802154_basic_mac: unable to forward packet of type %i\n",
                  pkt->type);
            gnrc_pktbuf_release(pkt);
        }
    }
}

/**
 * @brief   Startup code and event loop of the 802.15.4 basic MAC layer
 *
 * @param[in] args          expects a pointer to the underlying netdev device
 *
 * @return                  never returns
 */
static void *_802154_basic_mac_thread(void *args)
{
    gnrc_netdev_t *dev = (gnrc_netdev_t *)args;
    gnrc_netreg_entry_t regentry;
    gnrc_netapi_opt_t *opt;
    int res;
    msg_t msg, reply, msg_queue[GNRC_802154_BASIC_MAC_QUEUE_SIZE];

    /* setup the MAC layers message queue */
    msg_init_queue(msg_queue, GNRC_802154_BASIC_MAC_QUEUE_SIZE);
    /* save the PID to the device descriptor and register the device */
    dev->mac_pid = thread_getpid();
    gnrc_netif_add(dev->mac_pid);
    /* register the event callback with the device driver */
    dev->driver->add_event_callback(dev, _event_cb);

    /* register our thread to receive the wanted events from the netstack */
    regentry.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    regentry.pid = thread_getpid();
    /* we want the raw 802.15.4 frames => GNRC_NETTYPE_UNDEF */
    res = gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &regentry);
    if (res < 0) {
        core_panic(PANIC_ASSERT_FAIL,
                   "Could not register 802.15.4 basic MAC thread to netreg");
    }

    /* start the event loop */
    while (1) {
        DEBUG("802154_basic_mac: waiting for incoming messages\n");
        msg_receive(&msg);
        /* dispatch NETDEV and NETAPI messages */
        switch (msg.type) {
            case GNRC_NETDEV_MSG_TYPE_EVENT:
                DEBUG("802154_basic_mac: GNRC_NETDEV_MSG_TYPE_EVENT received\n");
                dev->driver->isr_event(dev, msg.content.value);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("802154_basic_mac: GNRC_NETAPI_MSG_TYPE_SND received\n");
                /* send the packet with CSMA/CA method */
                res = csma_ca_send(dev, (gnrc_pktsnip_t *) msg.content.ptr);
                DEBUG("802154_basic_mac: TX operation result: %i\n", res);
                /* send the result of TX to calling thread */  //XXX is that useful, harmful?...
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t) res;
                msg_reply(&msg, &reply);
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                DEBUG("802154_basic_mac: GNRC_NETAPI_MSG_TYPE_SET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* set option for device driver */
                res = dev->driver->set(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("802154_basic_mac: response of netdev->set: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
                /* TODO: filter out MAC layer options -> for now forward
                         everything to the device driver */
                DEBUG("802154_basic_mac: GNRC_NETAPI_MSG_TYPE_GET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* get option from device driver */
                res = dev->driver->get(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("802154_basic_mac: response of netdev->get: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            default:
                DEBUG("802154_basic_mac: Unknown command %" PRIu16 "\n",
                      msg.type);
                break;
        }
    }
    /* never reached */
    return NULL;
}

kernel_pid_t gnrc_802154_basic_mac_init(char *stack,
                                        int stacksize,
                                        char priority,
                                        const char *name,
                                        gnrc_netdev_t *dev)
{
    kernel_pid_t res;

    /* check if given netdev device is defined and the driver is set */
    if (dev == NULL || dev->driver == NULL) {
        return -ENODEV;
    }

    /* create new 802.15.4 basic MAC thread */
    res = thread_create(stack,
                        stacksize,
                        priority,
                        THREAD_CREATE_STACKTEST,
                        _802154_basic_mac_thread,
                        (void *)dev,
                        name);
    if (res <= 0) {
        return -EINVAL;
    }

    return res;
}
