/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup net_gnrc_zep
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <string.h>
#include <time.h>

#include "ringbuffer.h"
#include "hashes.h"
#include "msg.h"
#include "net/ieee802154.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc.h"
#include "net/gnrc/udp.h"
#include "periph/cpuid.h"
#include "random.h"

#include "net/gnrc/zep.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

#define _EVENT_RX_STARTED       (1)
#define _EVENT_RX_COMPLETE      (2)
#define _RX_BUF_SIZE            (16U * sizeof(gnrc_pktsnip_t *))
#define IEEE_802154_FCS_POLY    (0x8408)  /* x^16 + x^12 + x^5 + 1 for LSB first */

static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _rx_stack[GNRC_ZEP_STACK_SIZE];
static char _rx_buf_array[_RX_BUF_SIZE];
static ringbuffer_t _rx_buf = RINGBUFFER_INIT(_rx_buf_array);

/* gnrc_netdev driver definitions */
static int _send(gnrc_netdev_t *dev, gnrc_pktsnip_t *pkt);
static int _add_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb);
static int _rem_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb);
static int _get(gnrc_netdev_t *dev, netopt_t opt, void *value, size_t max_len);
static int _set(gnrc_netdev_t *dev, netopt_t opt, void *value, size_t value_len);
static void _isr_event(gnrc_netdev_t *dev, uint32_t event_type);

static const gnrc_netdev_driver_t _zep_driver = {
    _send,
    _add_cb,
    _rem_cb,
    _get,
    _set,
    _isr_event
};

/* Function for the ZEP thread */
void *_event_loop(void *args);

/* Builds (uninitialized) ZEP packet according to configured version */
static gnrc_pktsnip_t *_zep_hdr_build(gnrc_zep_t *dev, size_t size, bool ack);

/* Fills ZEP header according to gnrc_zep_t configuration */
static size_t _zep_hdr_fill(gnrc_zep_t *dev, gnrc_zep_hdr_t *hdr,
                            size_t payload_len);

/* Event handlers for ISR events */
static void _rx_started_event(gnrc_zep_t *dev);

/* IEEE 802.15.4 helper functions: TODO: generalize add to (gnrc_)ieee802154 */
static size_t _make_data_frame_hdr(gnrc_zep_t *dev, uint8_t *buf,
                                   gnrc_netif_hdr_t *hdr);
static size_t _get_frame_hdr_len(uint8_t *mhr);
gnrc_pktsnip_t *_make_netif_hdr(uint8_t *mhr);
static uint16_t _calc_fcs(uint16_t fcs, const uint8_t *frame, uint8_t frame_len);

kernel_pid_t gnrc_zep_init(gnrc_zep_t *dev, uint16_t src_port, ipv6_addr_t *dst,
                           uint16_t dst_port)
{
#if CPUID_LEN
    uint8_t cpuid[CPUID_LEN];
    uint32_t hash1, hash2;
#endif

    if (_pid != KERNEL_PID_UNDEF) {
        DEBUG("zep: ZEP thread already running at pid=%" PRIkernel_pid "\n", _pid);
        return -EEXIST;
    }

    if (dev == NULL) {
        DEBUG("zep: dev was NULL\n");
        return -ENODEV;
    }

    if ((dst == NULL) || (ipv6_addr_is_unspecified(dst))) {
        DEBUG("zep: dst (%s) was NULL or unspecified\n", dst);
        return -ENOTSUP;
    }

    if (gnrc_netreg_lookup(GNRC_NETTYPE_UDP, src_port)) {
        DEBUG("zep: port (%" PRIu16 ") already registered\n", src_port);
        return -EADDRINUSE;
    }

    dev->driver = (gnrc_netdev_driver_t *)&_zep_driver;
    dev->chan = GNRC_ZEP_DEFAULT_CHANNEL;
    dev->pan = byteorder_btols(byteorder_htons(GNRC_ZEP_DEFAULT_PANID));
    dev->flags = GNRC_ZEP_FLAGS_USE_SRC_PAN;
#if CPUID_LEN
    /* initialize dev->addr and dev->eui64 from cpuid if available */
    cpuid_get(cpuid);

    hash1 = djb2_hash(cpuid, CPUID_LEN / 2);
    dev->addr.u16 = (uint16_t)((hash1 >> 16) ^ (hash1 & 0xffff));

    if (CPUID_LEN % 2) {
        hash2 = djb2_hash(cpuid + (CPUID_LEN / 2), (CPUID_LEN / 2) - 1);
    }
    else {
        hash2 = djb2_hash(cpuid + (CPUID_LEN / 2), CPUID_LEN / 2);
    }

    dev->eui64.u32[0] = hash1;
    dev->eui64.u32[1] = hash2;

    dev->eui64.u8[7] &= 0xfe;   /* set to unicast */
    dev->eui64.u8[7] |= 0x02;   /* set to locally administered */
#else
    dev->addr = GNRC_ZEP_DEFAULT_ADDR_SHORT;
    dev->eui64 = GNRC_ZEP_DEFAULT_ADDR_LONG;
#endif
    DEBUG("zep: initialized radio parameters: chan: %" PRIu8 ", pan: 0x%04" PRIx16
          "addr: 0x%04" PRIx16 ", eui64: %016" PRIx64 "\n", dev->chan,
          dev->pan, byteorder_ltobs(dev->addr).u16,
          byteorder_ltobll(dev->eui64).u64);

#ifdef MODULE_GNRC_SIXLOWPAN
    dev->proto = GNRC_NETTYPE_SIXLOWPAN;
#else
    dev->proto = GNRC_NETTYPE_UNDEF;
#endif

    dev->seq = random_uint32();
    dev->src_port = src_port;
    dev->dst.u64[0] = dst->u64[0];
    dev->dst.u64[1] = dst->u64[1];
    dev->dst_port = dst_port;
    dev->version = 2;
    dev->lqi_mode = 1;

    _pid = thread_create(_rx_stack, GNRC_ZEP_STACK_SIZE, GNRC_ZEP_PRIO,
                         THREAD_CREATE_STACKTEST, _event_loop, dev, "zep_app");

    DEBUG("zep: started thread with PID %" PRIkernel_pid "\n", _pid);

    return _pid;
}

/* helper functions for options to avoid type pruning */
static inline void _set_uint16_ptr(uint16_t *ptr, uint16_t val)
{
    *ptr = val;
}

static inline void _set_uint64_ptr(uint64_t *ptr, uint64_t val)
{
    *ptr = val;
}

static inline void _set_flag_ptr(netopt_enable_t *enable,
                                 uint16_t flag_field, uint16_t flag)
{
    if (flag_field & flag) {
        *enable = NETOPT_ENABLE;
    }
    else {
        *enable = NETOPT_DISABLE;
    }
}

static inline uint16_t *_get_uint16_ptr(void *ptr)
{
    return ptr;
}

static int _send(gnrc_netdev_t *netdev, gnrc_pktsnip_t *pkt)
{
    if ((netdev == NULL) || (netdev->driver != &_zep_driver)) {
        DEBUG("zep: wrong device on sending\n");
        gnrc_pktbuf_release(pkt);
        return -ENODEV;
    }

    gnrc_zep_t *dev = (gnrc_zep_t *)netdev;
    gnrc_pktsnip_t *ptr, *new_pkt, *hdr;
    gnrc_zep_hdr_t *zep;
    size_t payload_len = gnrc_pkt_len(pkt->next), hdr_len, mhr_offset;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN], *data;
    uint16_t fcs = 0;

    /* create 802.15.4 header */
    hdr_len = _make_data_frame_hdr(dev, mhr, (gnrc_netif_hdr_t *)pkt->data);

    if (hdr_len == 0) {
        DEBUG("zep: error on frame creation\n");
        gnrc_pktbuf_release(pkt);
        return -ENOMSG;
    }

    new_pkt = _zep_hdr_build(dev, hdr_len + payload_len + IEEE802154_FCS_LEN, false);

    if (new_pkt == NULL) {
        DEBUG("zep: could not allocate ZEP header in pktbuf\n");
        gnrc_pktbuf_release(pkt);
        return -ENOBUFS;
    }

    zep = new_pkt->data;

    hdr = gnrc_udp_hdr_build(new_pkt, dev->src_port, dev->dst_port);

    if (hdr == NULL) {
        DEBUG("zep: could not allocate UDP header in pktbuf\n");
        gnrc_pktbuf_release(pkt);
        gnrc_pktbuf_release(new_pkt);
        return -ENOBUFS;
    }

    new_pkt = hdr;

    hdr = gnrc_ipv6_hdr_build(new_pkt, NULL, &(dev->dst));

    if (hdr == NULL) {
        DEBUG("zep: could not allocate IPv6 header in pktbuf\n");
        gnrc_pktbuf_release(pkt);
        gnrc_pktbuf_release(new_pkt);
        return -ENOBUFS;
    }

    new_pkt = hdr;

    mhr_offset = _zep_hdr_fill(dev, zep, payload_len + hdr_len + IEEE802154_FCS_LEN);

    if (mhr_offset == 0) {
        DEBUG("zep: error filling ZEP header\n");
        gnrc_pktbuf_release(pkt);
        gnrc_pktbuf_release(new_pkt);
        return -EINVAL;
    }

    memcpy(((uint8_t *)zep) + mhr_offset, mhr, hdr_len);

    fcs = _calc_fcs(fcs, ((uint8_t *)zep) + mhr_offset, hdr_len);
    data = ((uint8_t *)zep) + mhr_offset + hdr_len;
    ptr = pkt->next;

    while (ptr != NULL) {
        fcs = _calc_fcs(fcs, ptr->data, ptr->size);
        memcpy(data, ptr->data, ptr->size);
        data += ptr->size;
        ptr = ptr->next;
    }

    gnrc_pktbuf_release(pkt);

    DEBUG("zep: set frame FCS to 0x%04 " PRIx16 "\n", fcs);
    _set_uint16_ptr((uint16_t *)data, byteorder_btols(byteorder_htons(fcs)).u16);

    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, new_pkt)) {
        DEBUG("zep: no UDP handler found: dropping packet\n");
        gnrc_pktbuf_release(new_pkt);
        return -ENOENT;
    }

    return payload_len + hdr_len + IEEE802154_FCS_LEN;
}

static int _add_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{
    if ((dev == NULL) || (dev->driver != &_zep_driver)) {
        return -ENODEV;
    }

    if (dev->event_cb != NULL) {
        return -ENOBUFS;
    }

    dev->event_cb = cb;

    return 0;
}

static int _rem_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{
    if ((dev == NULL) || (dev->driver != &_zep_driver)) {
        return -ENODEV;
    }

    if (dev->event_cb != cb) {
        return -ENOENT;
    }

    dev->event_cb = NULL;

    return 0;
}

static int _get(gnrc_netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    gnrc_zep_t *dev = (gnrc_zep_t *)netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_CHANNEL:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            _set_uint16_ptr(value, (uint16_t)dev->chan);
            return sizeof(uint16_t);

        case NETOPT_ADDRESS:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            _set_uint16_ptr(value, byteorder_ltobs(dev->addr).u16);
            return sizeof(uint16_t);

        case NETOPT_ADDRESS_LONG:
            if (max_len < sizeof(uint64_t)) {
                return -EOVERFLOW;
            }

            _set_uint64_ptr(value, byteorder_ltobll(dev->eui64).u64);
            return sizeof(uint64_t);

        case NETOPT_ADDR_LEN:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            if (dev->flags & GNRC_ZEP_FLAGS_DST_ADDR_LONG) {
                _set_uint16_ptr(value, IEEE802154_LONG_ADDRESS_LEN);
            }
            else {
                _set_uint16_ptr(value, IEEE802154_SHORT_ADDRESS_LEN);
            }

            return sizeof(uint16_t);

        case NETOPT_SRC_LEN:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            if (dev->flags & GNRC_ZEP_FLAGS_SRC_ADDR_LONG) {
                _set_uint16_ptr(value, IEEE802154_LONG_ADDRESS_LEN);
            }
            else {
                _set_uint16_ptr(value, IEEE802154_SHORT_ADDRESS_LEN);
            }

            return sizeof(uint16_t);

        case NETOPT_PROTO:
            if (max_len < sizeof(gnrc_nettype_t)) {
                return -EOVERFLOW;
            }

            *((gnrc_nettype_t *)value) = dev->proto;
            return sizeof(gnrc_nettype_t);

        case NETOPT_NID:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            _set_uint16_ptr(value, byteorder_ltobs(dev->pan).u16);
            return sizeof(uint16_t);

        case NETOPT_IPV6_IID:
            if (max_len < sizeof(eui64_t)) {
                return -EOVERFLOW;
            }
            if (dev->flags & GNRC_ZEP_FLAGS_SRC_ADDR_LONG) {
                uint64_t addr = byteorder_ltobll(dev->eui64).u64;
                ieee802154_get_iid(value, (uint8_t *)&addr, IEEE802154_LONG_ADDRESS_LEN);
            }
            else {
                uint16_t addr = byteorder_ltobs(dev->addr).u16;
                ieee802154_get_iid(value, (uint8_t *)&addr, IEEE802154_SHORT_ADDRESS_LEN);
            }
            return sizeof(eui64_t);

        case NETOPT_MAX_PACKET_SIZE:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            _set_uint16_ptr(value, GNRC_ZEP_MAX_PKT_LENGTH);
            return sizeof(uint16_t);

        case NETOPT_AUTOACK:
            if (max_len < sizeof(netopt_enable_t)) {
                return -EOVERFLOW;
            }

            _set_flag_ptr(value, dev->flags, GNRC_ZEP_FLAGS_AUTOACK);
            return sizeof(uint16_t);

        default:
            return -ENOTSUP;
    }
}

static int _set(gnrc_netdev_t *netdev, netopt_t opt, void *value, size_t value_len)
{
    gnrc_zep_t *dev = (gnrc_zep_t *)netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_CHANNEL:
            if (value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            if ((*_get_uint16_ptr(value) < GNRC_ZEP_MIN_CHANNEL) ||
                (*_get_uint16_ptr(value)) > GNRC_ZEP_MAX_CHANNEL) {
                return -ENOTSUP;
            }

            dev->chan = *_get_uint16_ptr(value);
            return sizeof(uint16_t);

        case NETOPT_ADDRESS:
            if (value_len < sizeof(be_uint16_t)) {
                return -EOVERFLOW;
            }
            else {
                be_uint16_t *val = value;

                dev->addr = byteorder_btols(*val);
                return sizeof(be_uint16_t);
            }

        case NETOPT_ADDRESS_LONG:
            if (value_len < sizeof(be_uint64_t)) {
                return -EOVERFLOW;
            }
            else {
                be_uint64_t *val = value;

                dev->eui64 = byteorder_btolll(*val);
                return sizeof(be_uint64_t);
            }

        case NETOPT_ADDR_LEN:
            if (value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            switch (*_get_uint16_ptr(value)) {
                case IEEE802154_SHORT_ADDRESS_LEN:
                    dev->flags &= ~GNRC_ZEP_FLAGS_DST_ADDR_LONG;
                    break;

                case IEEE802154_LONG_ADDRESS_LEN:
                    dev->flags |= GNRC_ZEP_FLAGS_DST_ADDR_LONG;
                    break;

                default:
                    return -ENOTSUP;
            }

            return sizeof(uint16_t);

        case NETOPT_SRC_LEN:
            if (value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            switch (*_get_uint16_ptr(value)) {
                case IEEE802154_SHORT_ADDRESS_LEN:
                    dev->flags &= ~GNRC_ZEP_FLAGS_SRC_ADDR_LONG;
                    break;

                case IEEE802154_LONG_ADDRESS_LEN:
                    dev->flags |= GNRC_ZEP_FLAGS_SRC_ADDR_LONG;
                    break;

                default:
                    return -ENOTSUP;
            }

            return sizeof(uint16_t);

        case NETOPT_NID:
            if (value_len < sizeof(be_uint16_t)) {
                return -EOVERFLOW;
            }
            else {
                be_uint16_t *val = value;

                dev->pan = byteorder_btols(*val);
                return sizeof(be_uint16_t);
            }

        case NETOPT_AUTOACK:
            if (value_len < sizeof(netopt_enable_t)) {
                return -EOVERFLOW;
            }

            _set_flag_ptr(value, dev->flags, GNRC_ZEP_FLAGS_AUTOACK);
            return sizeof(uint16_t);

        default:
            return -ENOTSUP;
    }
}

static void _isr_event(gnrc_netdev_t *dev, uint32_t event_type)
{
    switch (event_type) {
        case _EVENT_RX_STARTED:
            DEBUG("zep: ISR event: RX started\n");
            _rx_started_event((gnrc_zep_t *)dev);
            break;

        default:
            DEBUG("zep: event %" PRIu32 " not handled\n", event_type);
            break;
    }
}

void *_event_loop(void *args)
{
    msg_t msg, ack, msg_q[GNRC_ZEP_MSG_QUEUE_SIZE];
    gnrc_netdev_t *dev = (gnrc_netdev_t *)args;
    gnrc_netapi_opt_t *opt;
    gnrc_netreg_entry_t my_reg = GNRC_NETREG_ENTRY_INIT_PID(((gnrc_zep_t *)args)->src_port,
                                                            sched_active_pid);

    msg_init_queue(msg_q, GNRC_ZEP_MSG_QUEUE_SIZE);

    gnrc_netreg_register(GNRC_NETTYPE_UDP, &my_reg);

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("zep: GNRC_NETAPI_MSG_TYPE_RCV\n");
                ringbuffer_add(&_rx_buf, (void*)&msg.content.ptr,
                               sizeof(gnrc_pktsnip_t *));
                ack.type = GNRC_NETDEV_MSG_TYPE_EVENT;
                ack.content.value = _EVENT_RX_STARTED;
                msg_send_int(&ack, dev->mac_pid);
                break;

            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("zep: GNRC_NETAPI_MSG_TYPE_SND\n");
                _send(dev, msg.content.ptr);
                break;

            case GNRC_NETAPI_MSG_TYPE_GET:
                DEBUG("zep: GNRC_NETAPI_MSG_TYPE_GET\n");
                opt = msg.content.ptr;
                ack.type = GNRC_NETAPI_MSG_TYPE_ACK;
                ack.content.value = _get(dev, opt->opt, opt->data, opt->data_len);
                msg_reply(&msg, &ack);
                break;

            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("zep: GNRC_NETAPI_MSG_TYPE_SET\n");
                opt = msg.content.ptr;
                ack.type = GNRC_NETAPI_MSG_TYPE_ACK;
                ack.content.value = _set(dev, opt->opt, opt->data, opt->data_len);
                msg_reply(&msg, &ack);
                break;

            default:
                DEBUG("udp: received unidentified message 0x%04" PRIx16 "\n",
                      msg.type);
                break;
        }
    }

    return NULL;
}

static gnrc_pktsnip_t *_zep_hdr_build(gnrc_zep_t *dev, size_t size, bool ack)
{
    gnrc_pktsnip_t *zep;

    switch (dev->version) {
        case 1:
            DEBUG("zep: Build ZEPv1 data header in pktbuf\n");
            zep = gnrc_pktbuf_add(NULL, NULL, sizeof(gnrc_zep_v1_hdr_t) + size,
                                  GNRC_NETTYPE_UNDEF);
            break;

        case 2:
            if (ack) {
                DEBUG("zep: Build ZEPv2 ACK header in pktbuf\n");
                zep = gnrc_pktbuf_add(NULL, NULL, sizeof(gnrc_zep_v2_ack_hdr_t) + size,
                                      GNRC_NETTYPE_UNDEF);
            }
            else {
                DEBUG("zep: Build ZEPv2 data header in pktbuf\n");
                zep = gnrc_pktbuf_add(NULL, NULL, sizeof(gnrc_zep_v2_data_hdr_t) + size,
                                      GNRC_NETTYPE_UNDEF);
            }

            break;

        default:
            DEBUG("zep: malconfigured version: %" PRIu8 "\n", dev->version);
            return NULL;
    }

    return zep;
}

static inline size_t _zep_hdr_fill_v1(gnrc_zep_t *dev, gnrc_zep_v1_hdr_t *hdr,
                                      size_t payload_len)
{
    hdr->version = 1;
    hdr->chan = dev->chan - GNRC_ZEP_MIN_CHANNEL;
    hdr->dev = byteorder_htons(1);
    hdr->lqi_mode = dev->lqi_mode;
    hdr->lqi_val = 0xff;                /* TODO: set */
    memset(hdr->resv, 0, sizeof(hdr->resv));
    hdr->length = payload_len;

    return sizeof(gnrc_zep_v1_hdr_t);
}

static size_t _zep_hdr_fill_v2_data(gnrc_zep_t *dev, gnrc_zep_v2_data_hdr_t *hdr,
                                    size_t payload_len)
{
    uint32_t epoch_sec;

    epoch_sec = (uint32_t)time(NULL);

    hdr->version = 2;
    hdr->type = GNRC_ZEP_V2_TYPE_DATA;
    hdr->chan = dev->chan - GNRC_ZEP_MIN_CHANNEL;
    hdr->dev = byteorder_htons(1);
    hdr->lqi_mode = dev->lqi_mode;
    hdr->lqi_val = 0xff;                /* TODO: set */
    hdr->time.b32[0] = byteorder_htonl(epoch_sec);
    hdr->time.u32[1] = 0;
    hdr->seq = byteorder_htonl(dev->seq);
    memset(hdr->resv, 0, sizeof(hdr->resv));
    hdr->length = payload_len;

    return sizeof(gnrc_zep_v2_data_hdr_t);
}

static size_t _zep_hdr_fill(gnrc_zep_t *dev, gnrc_zep_hdr_t *hdr,
                            size_t payload_len)
{
    hdr->preamble[0] = 'E';
    hdr->preamble[1] = 'X';

    switch (dev->version) {
        case 1:
            return _zep_hdr_fill_v1(dev, (gnrc_zep_v1_hdr_t *)hdr, payload_len);

        case 2:
            return _zep_hdr_fill_v2_data(dev, (gnrc_zep_v2_data_hdr_t *)hdr,
                                         payload_len);
            break;

        default:
            return 0;
    }
}

static gnrc_pktsnip_t *_create_received(gnrc_zep_t *dev, gnrc_pktsnip_t *pkt,
                                        uint8_t lqi, uint8_t frame_len,
                                        uint8_t version)
{
    gnrc_pktsnip_t *payload, *mhr, *netif;
    size_t mhr_len;

    (void)version;

    if ((frame_len != pkt->size) || (_calc_fcs(0, pkt->data, pkt->size) != 0)) {
        return NULL;
    }

    payload = gnrc_pktbuf_mark(pkt, pkt->size - 2, dev->proto);

    if (payload == NULL) {
        return NULL;
    }

    pkt = gnrc_pktbuf_remove_snip(pkt, pkt);    /* remove FCS */

    mhr_len = _get_frame_hdr_len(pkt->data);

    if (mhr_len == 0) {
        return NULL;
    }

    mhr = gnrc_pktbuf_mark(pkt, mhr_len, GNRC_NETTYPE_UNDEF);

    /* TODO: send ACK */

    netif = _make_netif_hdr(mhr->data);

    pkt = gnrc_pktbuf_remove_snip(pkt, mhr);

    ((gnrc_netif_hdr_t *)netif->data)->if_pid = dev->mac_pid;
    ((gnrc_netif_hdr_t *)netif->data)->lqi = lqi;
    ((gnrc_netif_hdr_t *)netif->data)->rssi = 0;

    LL_APPEND(pkt, netif);

    return pkt;
}

static gnrc_pktsnip_t *_recv_v1(gnrc_zep_t *dev, gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *zep;
    gnrc_zep_v1_hdr_t *hdr = pkt->data;
    uint8_t lqi, frame_len;

    if (pkt->size < sizeof(gnrc_zep_v1_hdr_t)) {
        return NULL;
    }

    lqi = hdr->lqi_val;
    frame_len = hdr->length;

    if ((hdr->chan + GNRC_ZEP_MIN_CHANNEL) != dev->chan) {
        return NULL;
    }

    zep = gnrc_pktbuf_mark(pkt, sizeof(gnrc_zep_v1_hdr_t), GNRC_NETTYPE_UNDEF);

    if (zep == NULL) {
        return NULL;
    }

    pkt = gnrc_pktbuf_remove_snip(pkt, zep);

    return _create_received(dev, pkt, lqi, frame_len, 2);
}

static gnrc_pktsnip_t *_recv_v2(gnrc_zep_t *dev, gnrc_pktsnip_t *pkt)
{
    gnrc_zep_v2_data_hdr_t *hdr = pkt->data;

    if (pkt->size < sizeof(gnrc_zep_v2_ack_hdr_t)) {
        return NULL;
    }

    if (hdr->type == GNRC_ZEP_V2_TYPE_ACK) {
        /* TODO handle correctly */
    }
    else if ((hdr->type == GNRC_ZEP_V2_TYPE_DATA) &&
             (pkt->size >= sizeof(gnrc_zep_v2_data_hdr_t))) {
        gnrc_pktsnip_t *zep;
        uint8_t lqi = hdr->lqi_val, frame_len = hdr->length;

        if ((hdr->chan + GNRC_ZEP_MIN_CHANNEL) != dev->chan) {
            return NULL;
        }

        zep = gnrc_pktbuf_mark(pkt, sizeof(gnrc_zep_v2_data_hdr_t), GNRC_NETTYPE_UNDEF);

        if (zep == NULL) {
            return NULL;
        }

        pkt = gnrc_pktbuf_remove_snip(pkt, zep);

        return _create_received(dev, pkt, lqi, frame_len, 2);
    }

    return NULL;
}

static void _rx_started_event(gnrc_zep_t *dev)
{
    gnrc_pktsnip_t *tmp, *pkt;
    gnrc_zep_hdr_t *hdr;

    if (ringbuffer_get(&_rx_buf, (char *)(&pkt),
                       sizeof(gnrc_pktsnip_t *)) != sizeof(gnrc_pktsnip_t *)) {
        return;
    }

    tmp = gnrc_pktbuf_start_write(pkt);

    if (tmp == NULL) {
        DEBUG("zep: Could not get write access to received packet\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    pkt = tmp;

    while (pkt->next) {
        /* remove everything below UDP */
        gnrc_pktbuf_remove_snip(pkt, pkt->next);
    }

    hdr = pkt->data;

    if ((pkt->size < 2) || (hdr->preamble[0] != 'E') ||
        (hdr->preamble[1] != 'X')) {
        gnrc_pktbuf_release(pkt);
        return;
    }

    switch (hdr->version) {
        case 1:
            pkt = _recv_v1(dev, pkt);
            break;

        case 2:
            pkt = _recv_v2(dev, pkt);
            break;

        default:
            gnrc_pktbuf_release(pkt);
            return;
    }

    if (pkt != NULL && dev->event_cb != NULL) {
        dev->event_cb(NETDEV_EVENT_RX_COMPLETE, pkt);
    }
    else if (pkt != NULL) {
        gnrc_pktbuf_release(pkt);
    }
}

/* TODO: Generalize and move all below to ieee802154 */
static size_t _make_data_frame_hdr(gnrc_zep_t *dev, uint8_t *buf,
                                   gnrc_netif_hdr_t *hdr)
{
    int pos = 0;

    /* we are building a data frame here */
    buf[0] = IEEE802154_FCF_TYPE_DATA;
    buf[1] = 0x88;      /* use short src and dst addresses as starting point */

    /* if AUTOACK is enabled, then we also expect ACKs for this packet */
    if (dev->flags & GNRC_ZEP_FLAGS_AUTOACK) {
        buf[0] |= IEEE802154_FCF_ACK_REQ;
    }

    /* fill in destination PAN ID */
    pos = 3;
    buf[pos++] = dev->pan.u8[0];
    buf[pos++] = dev->pan.u8[1];

    /* fill in destination address */
    if (hdr->flags &
        (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        buf[pos++] = 0xff;
        buf[pos++] = 0xff;
    }
    else if (hdr->dst_l2addr_len == IEEE802154_SHORT_ADDRESS_LEN) {
        uint8_t *dst_addr = gnrc_netif_hdr_get_dst_addr(hdr);
        buf[pos++] = dst_addr[1];
        buf[pos++] = dst_addr[0];
    }
    else if (hdr->dst_l2addr_len == IEEE802154_LONG_ADDRESS_LEN) {
        buf[1] |= 0x04;
        uint8_t *dst_addr = gnrc_netif_hdr_get_dst_addr(hdr);

        for (int i = (IEEE802154_LONG_ADDRESS_LEN - 1);  i >= 0; i--) {
            buf[pos++] = dst_addr[i];
        }
    }
    else {
        /* unsupported address length */
        return 0;
    }

    /* fill in source PAN ID (if applicable) */
    if (dev->flags & GNRC_ZEP_FLAGS_USE_SRC_PAN) {
        buf[pos++] = dev->pan.u8[0];
        buf[pos++] = dev->pan.u8[1];
    }
    else {
        buf[0] |= IEEE802154_FCF_PAN_COMP;
    }

    /* fill in source address */
    if (dev->flags & GNRC_ZEP_FLAGS_SRC_ADDR_LONG) {
        buf[1] |= 0x40;
        memcpy(&(buf[pos]), &dev->eui64, IEEE802154_LONG_ADDRESS_LEN);
        pos += IEEE802154_LONG_ADDRESS_LEN;
    }
    else {
        memcpy(&(buf[pos]), &dev->addr, IEEE802154_SHORT_ADDRESS_LEN);
        pos += IEEE802154_SHORT_ADDRESS_LEN;
    }

    /* set sequence number */
    buf[2] = dev->seq++;
    /* return actual header length */
    return pos;
}

static size_t _get_frame_hdr_len(uint8_t *mhr)
{
    uint8_t tmp;
    size_t len = 3;

    /* figure out address sizes */
    tmp = (mhr[1] & IEEE802154_FCF_DST_ADDR_MASK);

    if (tmp == IEEE802154_FCF_DST_ADDR_SHORT) {
        len += 4;
    }
    else if (tmp == IEEE802154_FCF_DST_ADDR_LONG) {
        len += 10;
    }
    else if (tmp != IEEE802154_FCF_DST_ADDR_VOID) {
        return 0;
    }

    tmp = (mhr[1] & IEEE802154_FCF_SRC_ADDR_MASK);

    if (tmp == IEEE802154_FCF_SRC_ADDR_VOID) {
        return len;
    }
    else {
        if (!(mhr[0] & IEEE802154_FCF_PAN_COMP)) {
            len += 2;
        }

        if (tmp == IEEE802154_FCF_SRC_ADDR_SHORT) {
            return (len + IEEE802154_SHORT_ADDRESS_LEN);
        }
        else if (tmp == IEEE802154_FCF_SRC_ADDR_LONG) {
            return (len + IEEE802154_LONG_ADDRESS_LEN);
        }
    }

    return 0;
}

gnrc_pktsnip_t *_make_netif_hdr(uint8_t *mhr)
{
    uint8_t tmp;
    uint8_t *addr;
    uint8_t src_len, dst_len;
    gnrc_pktsnip_t *snip;
    gnrc_netif_hdr_t *hdr;

    /* figure out address sizes */
    tmp = mhr[1] & IEEE802154_FCF_SRC_ADDR_MASK;

    if (tmp == IEEE802154_FCF_SRC_ADDR_SHORT) {
        src_len = IEEE802154_SHORT_ADDRESS_LEN;
    }
    else if (tmp == IEEE802154_FCF_SRC_ADDR_LONG) {
        src_len = IEEE802154_LONG_ADDRESS_LEN;
    }
    else if (tmp == 0) {
        src_len = 0;
    }
    else {
        return NULL;
    }

    tmp = mhr[1] & IEEE802154_FCF_DST_ADDR_MASK;

    if (tmp == IEEE802154_FCF_DST_ADDR_SHORT) {
        dst_len = IEEE802154_SHORT_ADDRESS_LEN;
    }
    else if (tmp == IEEE802154_FCF_DST_ADDR_LONG) {
        dst_len = IEEE802154_LONG_ADDRESS_LEN;
    }
    else if (tmp == 0) {
        dst_len = 0;
    }
    else {
        return NULL;
    }

    /* allocate space for header */
    snip = gnrc_pktbuf_add(NULL, NULL, sizeof(gnrc_netif_hdr_t) + src_len + dst_len,
                           GNRC_NETTYPE_NETIF);

    if (snip == NULL) {
        return NULL;
    }

    /* fill header */
    hdr = (gnrc_netif_hdr_t *)snip->data;
    gnrc_netif_hdr_init(hdr, src_len, dst_len);

    if (dst_len > 0) {
        tmp = 5 + dst_len;
        addr = gnrc_netif_hdr_get_dst_addr(hdr);

        for (int i = 0; i < dst_len; i++) {
            addr[i] = mhr[5 + (dst_len - i) - 1];
        }
    }
    else {
        tmp = 3;
    }

    if (!(mhr[0] & IEEE802154_FCF_PAN_COMP)) {
        tmp += 2;
    }

    if (src_len > 0) {
        addr = gnrc_netif_hdr_get_src_addr(hdr);

        for (int i = 0; i < src_len; i++) {
            addr[i] = mhr[tmp + (src_len - i) - 1];
        }
    }

    return snip;
}

static uint16_t _calc_fcs(uint16_t fcs, const uint8_t *frame, uint8_t frame_len)
{
    for (uint8_t byte = 0; byte < frame_len; ++byte) {
        fcs ^= frame[byte];

        for (uint8_t bit = 8; bit > 0; --bit) {
            if (fcs & 0x0001) {
                fcs = (fcs >> 1) ^ IEEE_802154_FCS_POLY;
            }
            else {
                fcs = (fcs >> 1);
            }
        }
    }

    return fcs;
}

/**
 * @}
 */
