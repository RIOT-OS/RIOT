/*
 * Copyright (C) 2015 James Hollister
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     ble_ll
 * @file
 * @brief       Implementation of the bluetooth low energy link layer
 *
 * @author      James Hollister <jhollisterjr@gmail.com>
 * @}
 */

#include <errno.h>

#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "net/gnrc/nomac.h"
#include "net/gnrc.h"
#include "net/ble_ll.h"

#define ENABLE_DEBUG    (1)
#define ENABLE_BLE_PKT_DEBUG	(0)
#include "debug.h"

/**
 * @brief   Possible BLE link layer states
 */
typedef enum {
    LL_STANDBY,		    /**< link layer is in standby mode */
    LL_ADVERTISING,         /**< link layer is in advertising mode */
    LL_CONNECTION,          /**< link layer is in connection mode */
    LL_INITIATING,          /**< link layer is in initiating mode */
    LL_SCANNING,            /**< link layer is in scanning mode */
} ll_state_t;

/**
 * @brief   Current state of the BLE Link Layer
 */
static volatile ll_state_t _ll_state = LL_ADVERTISING;

/**
 * @brief   BLE Advertising address
 */
static uint8_t _adv_addr[BLE_ADDR_LEN + 1];

/* Get the BLE Advertising Address of the interface */
static int _get_adv_address(uint8_t *val, size_t max_len)
{
    /* check parameters */
    if (max_len < BLE_ADDR_LEN) {
        return -EOVERFLOW;
    }

    /* get address */
    val[0] = _adv_addr[0];
    val[1] = _adv_addr[1];
    val[2] = _adv_addr[2];
    val[3] = _adv_addr[3];
    val[4] = _adv_addr[4];
    val[5] = _adv_addr[5];

    return BLE_ADDR_LEN;
}

/* Set the BLE Advertising Address of the interface */
static int _set_adv_address(uint8_t *val, size_t len)
{
    /* check parameters */
    if (len != BLE_ADDR_LEN) {
        return -EINVAL;
    }

    /* set address */
    _adv_addr[0] = val[5];
    _adv_addr[1] = val[4];
    _adv_addr[2] = val[3];
    _adv_addr[3] = val[2];
    _adv_addr[4] = val[1];
    _adv_addr[5] = val[0];

    return BLE_ADDR_LEN;
}

/* Set netopt parameter value in current BLE interface configuration */
static int _set(gnrc_netdev_t *dev, netopt_t opt,
         void *value, size_t value_len)
{
    switch (opt) {
        case NETOPT_BLE_ADV_ADDRESS:
            return _set_adv_address(value, value_len);

        default:
            return dev->driver->set(dev, opt, value, value_len);
    }
}

/* Get netopt parameter value from current BLE interface configuration */
static int _get(gnrc_netdev_t *dev, netopt_t opt,
         void *value, size_t max_len)
{
    switch (opt) {
        case NETOPT_BLE_ADV_ADDRESS:
            return _get_adv_address(value, max_len);

        default:
            return dev->driver->get(dev, opt, value, max_len);
    }
}

/**
 * @brief   BLE link layer send function
 */
static int _send(gnrc_netdev_t *dev, gnrc_pktsnip_t *pkt)
{
    uint8_t payload[BLE_MAX_PKT_LEN];
    gnrc_pktsnip_t *ble_pkt = NULL;
    uint8_t ble_pkt_size;

    if (pkt == NULL) {
        DEBUG("ble_ll: Error handling packet: packet incomplete\n");
        return -ENOMSG;
    }

    ble_pkt_size = gnrc_pkt_len(pkt);

    if (ble_pkt_size > BLE_MAX_PKT_LEN) {
        gnrc_pktbuf_release(pkt);
        DEBUG("ble_ll: Error sending packet: invalid BLE pdu header length\n");
        return -EOVERFLOW;
    }

    memcpy(payload, pkt->data, BLE_PDU_HDR_LEN);
    /* write advertising address to payload */
    memcpy((payload + BLE_PDU_HDR_LEN), _adv_addr, BLE_ADDR_LEN);
    ble_pkt_size += BLE_ADDR_LEN;

    /* write optional payload data */
    if (pkt->next != NULL) {
        ble_pkt_size += gnrc_pkt_len(pkt->next);
        if (ble_pkt_size > BLE_MAX_PKT_LEN) {
            gnrc_pktbuf_release(pkt);
            DEBUG("ble_ll: Error handling packet: payload too large\n");
            return -EOVERFLOW;
        }
        memcpy((payload + BLE_PDU_HDR_LEN + BLE_ADDR_LEN),
                pkt->next->data, gnrc_pkt_len(pkt->next));
    }

    ble_pkt = gnrc_pktbuf_add(NULL, payload, ble_pkt_size, GNRC_NETTYPE_UNDEF);
    gnrc_pktbuf_release(pkt);

    return dev->driver->send_data(dev, ble_pkt);
}

/**
 * @brief    Handle incoming link layer packet
 */
static void _receive(gnrc_pktsnip_t *pkt)
{
    ble_pkt_t *data;

    data = (ble_pkt_t *)pkt->data;

    if (_ll_state == LL_ADVERTISING) {
        /* Send respone to received BLE SCAN_REQ packet */
        if (data->header.pdu_type == SCAN_REQ_TYPE) {
            DEBUG("ble_ll: receive_data -> SCAN_REQ received\n");
            /*_send_scanresponse_pkt();*/
        }

        /* Send respone to received BLE CONNECT_REQ packet */
        else if (data->header.pdu_type == CONNECT_REQ_TYPE) {
            DEBUG("ble_ll: receive_data -> CONNECT_REQ received\n");
        }
    }

#if ENABLE_BLE_PKT_DEBUG
    int i;
    static const char *pdu_type_name[] = {
        "ADV_IND", "ADV_DIRECT_IND", "ADV_NONCONN_IND", "SCAN_REQ", "SCAN_RSP",
        "CONNECT_REQ", "ADV_SCAN_IND"
    };

    printf("Type: %s  Len: %u %s Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           pdu_type_name[data->header.pdu_type], data->header.length,
           (data->header.pdu_type == SCAN_REQ_TYPE) ? "Scanning" : "Advertising",
           data->payload[5], data->payload[4], data->payload[3], data->payload[2], data->payload[1],
           data->payload[0]);

    for (i = 0; i < data->header.length; i++) {
        printf("%02x ", data->payload[i]);
    }

    printf("\n");
#endif

}


/**
 * @brief   Function called by the device driver on device events
 *
 * @param[in] event         type of event
 * @param[in] data          optional parameter
 */
static void _event_cb(gnrc_netdev_event_t event, void *data)
{
    DEBUG("ble_ll: event triggered -> RX_COMPLETE\n");
    /* NOMAC only understands the RX_COMPLETE event... */
    if (event == NETDEV_EVENT_RX_COMPLETE) {
        gnrc_pktsnip_t *pkt;
        gnrc_netreg_entry_t *sendto;

        /* get pointer to the received packet */
        pkt = (gnrc_pktsnip_t *)data;
        /* handle recieved packet */
        _receive(pkt);
        /* find out, who to send the packet to */
        sendto = gnrc_netreg_lookup(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL);
        /* throw away packet if no one is interested */
        if (sendto == NULL) {
            DEBUG("ble_ll: unable to forward packet of type %i (sendto == NULL)\n", pkt->type);
            gnrc_pktbuf_release(pkt);
            return;
        }
        /* send the packet to everyone interested in it's type */
        gnrc_pktbuf_hold(pkt, gnrc_netreg_num(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL) - 1);
        while (sendto != NULL) {
            DEBUG("ble_ll: sending pkt %p to PID %u\n", (void*)pkt, sendto->pid);
            gnrc_netapi_receive(sendto->pid, pkt);
            sendto = gnrc_netreg_getnext(sendto);
        }
    } else {
       DEBUG("ble_ll: event triggered -> unknown event (%i)\n", event);
    }
}

/**
 * @brief   Startup code and event loop of the BLE link layer
 *
 * @param[in] args          expects a pointer to the underlying netdev device
 *
 * @return                  never returns
 */
static void *_ble_ll_thread(void *args)
{
    gnrc_netdev_t *dev = (gnrc_netdev_t *)args;
    gnrc_netapi_opt_t *opt;
    int res;
    msg_t msg, reply, msg_queue[GNRC_NOMAC_MSG_QUEUE_SIZE];

    /* setup the MAC layers message queue */
    msg_init_queue(msg_queue, GNRC_NOMAC_MSG_QUEUE_SIZE);
    /* save the PID to the device descriptor and register the device */
    dev->mac_pid = thread_getpid();
    gnrc_netif_add(dev->mac_pid);
    /* register the event callback with the device driver */
    dev->driver->add_event_callback(dev, _event_cb);

    /* start the event loop */
    DEBUG("ble_ll: starting main event loop...\n");
    while (1) {
        DEBUG("\nble_ll: waiting for next incoming message (msg_receive)...\n");
        msg_receive(&msg);
        /* dispatch NETDEV and NETAPI messages */
        switch (msg.type) {
            case GNRC_NETDEV_MSG_TYPE_EVENT:
                DEBUG("ble_ll: GNRC_NETDEV_MSG_TYPE_EVENT received\n");
                dev->driver->isr_event(dev, msg.content.value);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("ble_ll: GNRC_NETAPI_MSG_TYPE_SND received\n");
                _send(dev, (gnrc_pktsnip_t *)msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("ble_ll: GNRC_NETAPI_MSG_TYPE_SET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* set option for device driver */
                res = _set(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("ble_ll: response of netdev->set: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
                DEBUG("ble_ll: GNRC_NETAPI_MSG_TYPE_GET received\n");
                /* read incoming options */
                opt = (gnrc_netapi_opt_t *)msg.content.ptr;
                /* get option from device driver */
                res = _get(dev, opt->opt, opt->data, opt->data_len);
                DEBUG("ble_ll: response of netdev->get: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            default:
                DEBUG("ble_ll: unknown command %" PRIu16 "\n", msg.type);
                break;
        }
    }
    /* never reached */
    return NULL;
}

kernel_pid_t ble_ll_init(char *stack, int stacksize, char priority,
                        const char *name, gnrc_netdev_t *dev)
{
    kernel_pid_t res;

    /* check if given netdev device is defined and the driver is set */
    if (dev == NULL || dev->driver == NULL) {
        return -ENODEV;
    }
    /* create new NOMAC thread */
    res = thread_create(stack, stacksize, priority, CREATE_STACKTEST,
                         _ble_ll_thread, (void *)dev, name);
    if (res <= 0) {
        return -EINVAL;
    }
    return res;
}
