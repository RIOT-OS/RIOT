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
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "msg.h"
#include "thread.h"

#include "net/eui48.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/nettype.h"

#include "ble-core.h"
#include "softdevice_handler.h"

#include "ble-mac.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if defined(MODULE_OD) && ENABLE_DEBUG
#include "od.h"
#endif

#define BLE_PRIO                    (GNRC_NETIF_PRIO)

/* XXX: netdev required by gnrc_netif, but not implemented fully for
 * nordic_softdevice_ble for legacy reasons */

static char _stack[(THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)];

static gnrc_netif_t *_ble_netif = NULL;

static uint8_t _sendbuf[BLE_SIXLOWPAN_MTU];

static void _ble_mac_callback(ble_mac_event_enum_t event, void* arg)
{
    msg_t m = { .type=event, .content.ptr=arg };

    if ((_ble_netif == NULL) || !msg_send_int(&m, _ble_netif->pid)) {
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

    gnrc_netif_hdr_init(netif_hdr->data, BLE_L2_ADDR_LEN, BLE_L2_ADDR_LEN);
    gnrc_netif_hdr_set_src_addr(netif_hdr->data, inbuf->src, BLE_L2_ADDR_LEN);
    gnrc_netif_hdr_set_dst_addr(netif_hdr->data, _ble_netif->l2addr, BLE_L2_ADDR_LEN);
    gnrc_netif_hdr_set_netif(netif_hdr->data, _ble_netif);

    DEBUG("_handle_raw_sixlowpan(): received packet from %02x:%02x:%02x:%02x:%02x:%02x "
            "of length %d\n",
            inbuf->src[0], inbuf->src[1], inbuf->src[2],
            inbuf->src[3], inbuf->src[4], inbuf->src[5], inbuf->len);
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

static int _netdev_init(netdev_t *dev)
{
    _ble_netif = dev->context;
    ble_stack_init();
    ble_mac_init(_ble_mac_callback);
    _ble_netif->l2addr_len = BLE_L2_ADDR_LEN;
    ble_get_mac(_ble_netif->l2addr);
    ble_advertising_init("RIOT BLE");
    ble_advertising_start();
    return 0;
}

static int _netdev_get(netdev_t *netdev, netopt_t opt,
                       void *v, size_t max_len)
{
    int res = -ENOTSUP;
    uint8_t *value = v;

    (void)netdev;
    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= BLE_L2_ADDR_LEN);
            memcpy(value, _ble_netif->l2addr, BLE_L2_ADDR_LEN);
            res = BLE_L2_ADDR_LEN;
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)value) = BLE_L2_ADDR_LEN;
            res = sizeof(uint16_t);
            break;
        case NETOPT_PROTO:
            assert(max_len == sizeof(gnrc_nettype_t));
            *((gnrc_nettype_t *)value) = GNRC_NETTYPE_SIXLOWPAN;
            res = sizeof(gnrc_nettype_t);
            break;
        case NETOPT_DEVICE_TYPE:
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)value) = NETDEV_TYPE_BLE;
            res = sizeof(uint16_t);
            break;
        case NETOPT_IPV6_IID:
            eui48_to_ipv6_iid((eui64_t *)value, (eui48_t *)_ble_netif->l2addr);
            res = sizeof(uint64_t);
            break;
        default:
            break;
    }
    return res;
}

static int _netif_send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    (void)netif;
    assert(netif == _ble_netif);
    return _send(pkt);
}

static gnrc_pktsnip_t *_netif_recv(gnrc_netif_t *netif)
{
    (void)netif;
    /* not supported */
    return NULL;
}

static void _netif_msg_handler(gnrc_netif_t *netif, msg_t *msg)
{
    switch (msg->type) {
        case BLE_EVENT_RX_DONE:
            {
                DEBUG("ble rx:\n");
                _handle_raw_sixlowpan(msg->content.ptr);
                ble_mac_busy_rx = 0;
                break;
            }
    }
}

static const gnrc_netif_ops_t _ble_ops = {
    .init = gnrc_netif_default_init,
    .send = _netif_send,
    .recv = _netif_recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
    .msg_handler = _netif_msg_handler,
};

static const netdev_driver_t _ble_netdev_driver = {
    .send = NULL,
    .recv = NULL,
    .init = _netdev_init,
    .isr  =  NULL,
    .get  = _netdev_get,
    .set  = netdev_set_notsup,
};

static netdev_t _ble_dummy_dev = {
    .driver = &_ble_netdev_driver,
};

void gnrc_nordic_ble_6lowpan_init(void)
{
    gnrc_netif_create(_stack, sizeof(_stack), BLE_PRIO,
                      "ble", &_ble_dummy_dev, &_ble_ops);
}
