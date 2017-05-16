/*
 * Copyright (c) 2016, Nordic Semiconductor
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Glue for Nordic's SoftDevice BLE 6lowpan blob to netapi
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "msg.h"
#include "thread.h"

#include "net/gnrc.h"
#include "net/gnrc/nettype.h"

#include "ble-core.h"
#include "softdevice_handler.h"

#include "ble-mac.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if defined(MODULE_OD) && ENABLE_DEBUG
#include "od.h"
#endif

#define BLE_NETAPI_MSG_QUEUE_SIZE   (8U)
#define BLE_PRIO                    (THREAD_PRIORITY_MAIN - 1)

kernel_pid_t gnrc_nordic_ble_6lowpan_pid;
static char _stack[(THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)];

static uint8_t _own_mac_addr[BLE_SIXLOWPAN_L2_ADDR_LEN];

static uint8_t _sendbuf[BLE_SIXLOWPAN_MTU];

static void _ble_mac_callback(ble_mac_event_enum_t event, void* arg)
{
    msg_t m = { .type=event, .content.ptr=arg };

    if (!msg_send_int(&m, gnrc_nordic_ble_6lowpan_pid)) {
        puts("_ble_mac_callback(): possibly lost interrupt");
    }
}

static void _handle_raw_sixlowpan(ble_mac_inbuf_t *inbuf)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, inbuf->payload,
            inbuf->len,
            GNRC_NETTYPE_SIXLOWPAN);

    if(!pkt) {
        DEBUG("_handle_raw_sixlowpan(): no space left in packet buffer.\n");
        return;
    }

    /* create netif header */
    gnrc_pktsnip_t *netif_hdr;
    netif_hdr = gnrc_pktbuf_add(NULL, NULL,
            sizeof(gnrc_netif_hdr_t) + (2 * sizeof(eui64_t)),
            GNRC_NETTYPE_NETIF);

    if (netif_hdr == NULL) {
        DEBUG("_handle_raw_sixlowpan(): no space left in packet buffer.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    gnrc_netif_hdr_init(netif_hdr->data, BLE_SIXLOWPAN_L2_ADDR_LEN, BLE_SIXLOWPAN_L2_ADDR_LEN);
    gnrc_netif_hdr_set_src_addr(netif_hdr->data, inbuf->src, BLE_SIXLOWPAN_L2_ADDR_LEN);
    gnrc_netif_hdr_set_dst_addr(netif_hdr->data, _own_mac_addr, BLE_SIXLOWPAN_L2_ADDR_LEN);
    ((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid = gnrc_nordic_ble_6lowpan_pid;

    DEBUG("_handle_raw_sixlowpan(): received packet from %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x "
            "of length %d\n",
            inbuf->src[0], inbuf->src[1], inbuf->src[2], inbuf->src[3], inbuf->src[4],
            inbuf->src[5], inbuf->src[6], inbuf->src[7], inbuf->len);
#if defined(MODULE_OD) && ENABLE_DEBUG
    od_hex_dump(inbuf->payload, inbuf->len, OD_WIDTH_DEFAULT);
#endif

    LL_APPEND(pkt, netif_hdr);

    /* throw away packet if no one is interested */
    if (!gnrc_netapi_dispatch_receive(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        DEBUG("_handle_raw_sixlowpan: unable to forward packet of type %i\n", pkt->type);
        gnrc_pktbuf_release(pkt);
    }
}

static int _send(gnrc_pktsnip_t *pkt)
{
    if (pkt == NULL) {
        DEBUG("_send_ble: pkt was NULL\n");
        return -EINVAL;
    }

    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *payload = pkt->next;
    uint8_t *dst;

    if (ble_mac_busy_tx) {
        DEBUG("_send(): ble_mac_busy_tx\n");
        return -EBUSY;
    }

    uint8_t *buf = _sendbuf;
    unsigned len = 0;

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("_send_ble: first header is not generic netif header\n");
        return -EBADMSG;
    }

    netif_hdr = pkt->data;

    /* prepare destination address */
    if (netif_hdr->flags &
        (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        dst = NULL;
    }
    else {
        dst = gnrc_netif_hdr_get_dst_addr(netif_hdr);
    }

    /* prepare packet for sending */
    while (payload) {
        memcpy(buf, payload->data, payload->size);
        len += payload->size;
        buf +=  payload->size;
        payload = payload->next;
    }

    gnrc_pktbuf_release(pkt);

    if (ble_mac_send(dst, _sendbuf, len) == -1) {
        return -EBADMSG;
    }

    return 0;
}
static int _handle_get(gnrc_netapi_opt_t *_opt)
{
    int res = -ENOTSUP;
    uint8_t *value = _opt->data;

    switch (_opt->opt) {
        case NETOPT_ACK_REQ:
        case NETOPT_CHANNEL:
        case NETOPT_NID:
        case NETOPT_ADDRESS:
            /* -ENOTSUP */
            break;
        case NETOPT_ADDRESS_LONG:
            assert(_opt->data_len >= BLE_SIXLOWPAN_L2_ADDR_LEN);
            memcpy(value, _own_mac_addr, BLE_SIXLOWPAN_L2_ADDR_LEN);
            value[0] = IPV6_IID_FLIP_VALUE;
            res = BLE_SIXLOWPAN_L2_ADDR_LEN;
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            assert(_opt->data_len == sizeof(uint16_t));
            *((uint16_t *)value) = BLE_SIXLOWPAN_L2_ADDR_LEN;
            res = sizeof(uint16_t);
            break;
#ifdef MODULE_GNRC
        case NETOPT_PROTO:
            assert(_opt->data_len == sizeof(gnrc_nettype_t));
            *((gnrc_nettype_t *)value) = GNRC_NETTYPE_SIXLOWPAN;
            res = sizeof(gnrc_nettype_t);
            break;
#endif
/*        case NETOPT_DEVICE_TYPE:
            assert(_opt->data_len == sizeof(uint16_t));
            *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
            res = sizeof(uint16_t);
            break;*/
        case NETOPT_IPV6_IID:
            memcpy(value, _own_mac_addr, BLE_SIXLOWPAN_L2_ADDR_LEN);
            value[0] = IPV6_IID_FLIP_VALUE;
            res = BLE_SIXLOWPAN_L2_ADDR_LEN;
            break;
        default:
            break;
    }
    return res;
}

/**
 * @brief   Startup code and event loop of the gnrc_nordic_ble_6lowpan layer
 *
 * @return          never returns
 */
static void *_gnrc_nordic_ble_6lowpan_thread(void *args)
{
    (void)args;

    DEBUG("gnrc_nordic_ble_6lowpan: starting thread\n");

    gnrc_nordic_ble_6lowpan_pid = thread_getpid();

    gnrc_netapi_opt_t *opt;
    int res;
    msg_t msg, reply, msg_queue[BLE_NETAPI_MSG_QUEUE_SIZE];

    /* setup the message queue */
    msg_init_queue(msg_queue, BLE_NETAPI_MSG_QUEUE_SIZE);

    /* initialize BLE stack */
    assert((unsigned)softdevice_handler_isEnabled());

    ble_stack_init();
    ble_get_mac(_own_mac_addr);

    ble_mac_init(_ble_mac_callback);

    ble_advertising_init("RIOT BLE");
    ble_advertising_start();

    /* register the device to the network stack*/
    gnrc_netif_add(thread_getpid());

    /* start the event loop */
    while (1) {
//        DEBUG("gnrc_nordic_ble_6lowpan: waiting for incoming messages\n");
        msg_receive(&msg);
        /* dispatch NETDEV and NETAPI messages */
        switch (msg.type) {
            case BLE_EVENT_RX_DONE:
                {
                    DEBUG("ble rx:\n");
                    _handle_raw_sixlowpan(msg.content.ptr);
                    ble_mac_busy_rx = 0;
                    break;
                }
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("gnrc_nordic_ble_6lowpan: GNRC_NETAPI_MSG_TYPE_SND received\n");
                _send(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
                /* read incoming options */
                opt = msg.content.ptr;
                DEBUG("gnrc_nordic_ble_6lowpan: GNRC_NETAPI_MSG_TYPE_SET received. opt=%s\n",
                      netopt2str(opt->opt));
                /* set option for device driver */
                res = ENOTSUP;
                DEBUG("gnrc_nordic_ble_6lowpan: response of netdev->set: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
                /* read incoming options */
                opt = msg.content.ptr;
                DEBUG("gnrc_nordic_ble_6lowpan: GNRC_NETAPI_MSG_TYPE_GET received. opt=%s\n",
                      netopt2str(opt->opt));
                res = _handle_get(opt);
                DEBUG("gnrc_nordic_ble_6lowpan: response of netdev->get: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            default:
                DEBUG("gnrc_nordic_ble_6lowpan: Unknown command %" PRIu16 "\n", msg.type);
                break;
        }
    }
    /* never reached */
    return NULL;
}

void gnrc_nordic_ble_6lowpan_init(void)
{
    kernel_pid_t res = thread_create(_stack, sizeof(_stack), BLE_PRIO,
                        THREAD_CREATE_STACKTEST,
                        _gnrc_nordic_ble_6lowpan_thread, NULL,
                        "ble");
    assert(res > 0);
    (void)res;
}
