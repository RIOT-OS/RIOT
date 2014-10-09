/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     sixlowpan
 * @{
 *
 * @file        core.c
 * @brief       6LoWPAN core
 * @see         <a href="http://tools.ietf.org/html/rfc4944">
 *                  RFC 4944
 *              </a>
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "ipv6.h"
#include "kernel_types.h"
#include "msg.h"
#include "mutex.h"
#include "netapi.h"
#include "netdev/base.h"
#include "pktbuf.h"
#include "timex.h"
#include "thread.h"
#include "vtimer.h"

#include "sixlowpan.h"

#ifdef MODULE_SIXLOWPAN_HC
#include "sixlowpan/hc.h"
#include "sixlowpan/iphc_cbuf.h"
#endif

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include "debug.h"

#define _DEFAULT_MAX_MAC_DATA_LEN   (127)
#define _L2_ADDR_MAX_LEN            (8)
#define _MSG_QUEUE_SIZE             (32)
#define _REASSEMBLY_BUFFER_TIMEOUT  (3 * 1000 * 1000)
#define _REASSEMBLY_BUFFER_MAX_DG   (10)
#define _REGISTRY_SIZE              (2)

/* Check if data starts with fragmentation dispatch */
#define SIXLOWPAN_FRAG_DISPATCH(data) \
    (((data[0] & SIXLOWPAN_FRAG_HDR_MASK) == SIXLOWPAN_FRAG1_DISPATCH) || \
     ((data[0] & SIXLOWPAN_FRAG_HDR_MASK) == SIXLOWPAN_FRAGN_DISPATCH))

/* Check if address family is supported */
#define _AF_SUPPORTED(addr_len) (addr_len == 2 || addr_len == 8)

/*
 * Interval list to identify interval limits of fragments
 */
typedef struct __attribute__((packed)) _interval_list_t {
    uint16_t start;
    uint16_t end;
    struct _interval_list_t *next;
} _interval_list_t;

/*
 * Representation of IPv6 datagrams in 6LoWPAN reassembly buffer
 *
 * http://tools.ietf.org/html/rfc4944#section-5.3
 */
typedef struct __attribute__((packed)) _reassembly_buf_t {
    struct _reassembly_buf_t *next; /* Pointer to next IPv6
                                       datagram */
    uint8_t src[_L2_ADDR_MAX_LEN];  /* Source address */
    uint8_t src_len;                /* Source address length */
    uint8_t dest[_L2_ADDR_MAX_LEN]; /* Destination address */
    uint8_t dest_len;               /* Destination address length */
    uint16_t datagram_size;         /* Datagram size */
    uint16_t datagram_tag;          /* Datagram tag */
    uint64_t timestamp;             /* Timestamp of last packet fragment */
    uint16_t current_size;          /* cumulative size of currently received
                                     * fragments */
    _interval_list_t *intervals;    /* intervals of the fragments */
    uint8_t *packet;                /* Pointer to reassembled packet
                                     * + 6LoWPAN Dispatch byte */
} _reassembly_buf_t;

static uint16_t _current_frag_tag = 0;
static mutex_t _current_frag_tag_mutex = MUTEX_INIT;
static _reassembly_buf_t *_reassembly_buf_head = NULL;
static mutex_t _reassembly_mutex = MUTEX_INIT;

int _transfer_received(netapi_rcv_pkt_t *rcv, kernel_pid_t *registry)
{
    msg_t msg_rcv, msg_ack;
    netapi_ack_t ack;

    msg_rcv.type = NETAPI_MSG_TYPE;
    msg_rcv.content.ptr = (char *)rcv;
    rcv->ack = &ack;
    rcv->type = NETAPI_CMD_RCV;

    for (int i = 0; i < _REGISTRY_SIZE; i++) {
        msg_send_receive(&msg_rcv, &msg_ack, registry[i]);

        if (ack.result < 0) {
            return ack.result;
        }
    }

    return ack.result;
}

#ifdef TEST_SUITES
/* only call this if you know what you are doing */
void sixlowpan_reset(void)
{
    _reassembly_buf_head = NULL;
    _current_frag_tag = 0;
}
#endif

_reassembly_buf_t *_reassembly_buf_garbage_collect(_reassembly_buf_t *current)
{
    _interval_list_t *current_interval;
    _reassembly_buf_t *tmp, *ptr, *return_ptr;

    tmp = _reassembly_buf_head;
    ptr = _reassembly_buf_head;

    if (_reassembly_buf_head == current) {
        _reassembly_buf_head = current->next;
        return_ptr = current->next;
    }
    else {
        while (tmp != current) {
            ptr = tmp;
            tmp = tmp->next;
        }

        ptr->next = current->next;
        return_ptr = current->next;
    }

    current_interval = current->intervals;

    while (current_interval != NULL) {
        _interval_list_t *tmp_interval = current_interval->next;
        pktbuf_release(current_interval);
        current_interval = tmp_interval;
    }

    pktbuf_release(current->packet);
    pktbuf_release(current);

    if (_reassembly_buf_head == current) {
        _reassembly_buf_head = NULL;
    }

    return return_ptr;
}

static inline uint8_t _reassembly_buf_is_in_interval(_interval_list_t *i,
        uint16_t start, uint16_t end)
{
    return (((i->start < start) && (start <= i->end)) ||
            ((start < i->start) && (i->start <= end)) ||
            ((i->start == start) && (i->end == end)));
}

static uint8_t _reassembly_buf_update_intervals(_reassembly_buf_t *current,
        uint16_t datagram_offset,
        uint8_t fragment_size)
{
    /* 0: Error, discard fragment */
    /* 1: Finished correctly */
    _interval_list_t *last = NULL, *current_interval, *new;

    current_interval = current->intervals;

    while (current_interval != NULL) {
        if (_reassembly_buf_is_in_interval(current_interval, datagram_offset,
                                           datagram_offset + fragment_size - 1)) {
            /* Interval is overlapping or the same as one of a previous fragment, discard fragment */
            return 0;
        }

        last = current_interval;
        current_interval = current_interval->next;
    }

    new = pktbuf_alloc(sizeof(_interval_list_t));

    if (new != NULL) {
        new->start = datagram_offset;
        new->end = datagram_offset + fragment_size - 1;
        new->next = NULL;

        if (last == NULL) {
            current->intervals = new;
        }
        else {
            last->next = new;
        }

        return 1;
    }

    return 0;
}

static void _reassembly_buf_check_timeout(void)
{
    _reassembly_buf_t *tmp, *smallest_time = NULL;
    uint8_t count = 0;
    timex_t now;

    vtimer_now(&now);
    tmp = _reassembly_buf_head;

    while (tmp != NULL) {
        if ((timex_uint64(now) - tmp->timestamp) >= _REASSEMBLY_BUFFER_TIMEOUT) {
            DEBUG("reassembly buffer timeout: now: %" PRIu64
                  ", fragment timestamp: %" PRIu64 "\n",
                  timex_uint64(now), tmp->timestamp);
            tmp = _reassembly_buf_garbage_collect(tmp);
        }
        else {
            if (smallest_time == NULL) {
                smallest_time = tmp;
            }
            else if (tmp->timestamp < smallest_time->timestamp) {
                smallest_time = tmp;
            }

            tmp = tmp->next;
            count++;
        }
    }

    if ((count > _REASSEMBLY_BUFFER_MAX_DG) & (smallest_time != NULL)) {
        _reassembly_buf_garbage_collect(smallest_time);
    }
}

static _reassembly_buf_t *_reassembly_buf_new(uint16_t datagram_size, uint16_t datagram_tag,
        void *src, uint8_t src_len, void *dest, uint8_t dest_len, _reassembly_buf_t *prev)
{
    /* Allocate new memory for a new packet to be reassembled */
    _reassembly_buf_t *new = pktbuf_alloc(sizeof(_reassembly_buf_t));

    if (new != NULL) {
        timex_t now;
        pktbuf_copy(&(new->src), src, src_len);
        new->src_len = src_len;
        pktbuf_copy(&(new->dest), dest, dest_len);
        new->dest_len = dest_len;
        new->datagram_size = datagram_size;
        new->datagram_tag = datagram_tag;
        vtimer_now(&now);
        new->timestamp = timex_uint64(now);
        new->current_size = 0;
        new->intervals = NULL;
        new->packet = (uint8_t *)pktbuf_alloc(datagram_size);

        if (new->packet == NULL) {
            return NULL;
        }

        if (prev == NULL) {
            _reassembly_buf_head = new;
        }
        else {
            prev->next = new;
        }

        return new;
    }

    return NULL;
}

static _reassembly_buf_t *_reassembly_buf_get(uint16_t datagram_size, uint16_t datagram_tag,
        void *src, uint8_t src_len, void *dest, uint8_t dest_len)
{
    _reassembly_buf_t *current = _reassembly_buf_head, *prev = NULL;

    while (current != NULL) {
        if (current->src_len == src_len && current->dest_len == dest_len &&
            current->datagram_size == datagram_size &&
            current->datagram_tag == datagram_tag &&
            current->intervals != NULL &&
            memcmp(current->src, src, src_len) == 0 &&
            memcmp(current->dest, dest, dest_len) == 0) {
            timex_t now;
            /* Found buffer for current packet fragment */
            vtimer_now(&now);
            current->timestamp = timex_uint64(now);
            return current;
        }

        prev = current;
        current = current->next;
    }

    return _reassembly_buf_new(datagram_size, datagram_tag, src, (uint8_t)src_len,
                               dest, (uint8_t)dest_len, prev);
}

static int _reassembly_buf_add_fragment(netapi_rcv_pkt_t *rcv, void *src,
                                        size_t src_len, void *dest,
                                        size_t dest_len, uint8_t *fragment_data,
                                        uint16_t datagram_offset,
                                        uint16_t datagram_size,
                                        uint16_t datagram_tag,
                                        uint8_t dispatch_len,
                                        uint8_t fragment_size)
{
    _reassembly_buf_t *current;

    mutex_lock(&_reassembly_mutex);
    current = _reassembly_buf_get(datagram_size, datagram_tag, src,
                                  (uint8_t)src_len, dest, (uint8_t)dest_len);

    if (current == NULL) {
        mutex_unlock(&_reassembly_mutex);
        return -ENOBUFS;
    }

    if (_reassembly_buf_update_intervals(current, datagram_offset, fragment_size)) {
        pktbuf_copy(current->packet + datagram_offset, fragment_data + dispatch_len,
                    fragment_size);
        current->current_size += fragment_size;
    }

    if (current->current_size == current->datagram_size) {
        int res = 0;
        pktbuf_hold(current);
        pktbuf_hold(current->packet);

#ifdef MODULE_SIXLOWPAN_HC

        if ((current->packet[0] & SIXLOWPAN_IPHC1_HDR_MASK) == SIXLOWPAN_IPHC1_DISPATCH) {
            res = sixlowpan_hc_decode(rcv, current->src, current->src_len,
                                      current->dest, current->dest_len,
                                      current->packet, current->datagram_size);
        }
        else {
            if (current->packet[0] == SIXLOWPAN_IPV6_DISPATCH) {
                (current->packet)++;
                (current->datagram_size)--;
            }

            rcv->src = (void *)(&(current->src[0]));
            rcv->src_len = (size_t)(current->src_len);
            rcv->dest = (void *)(&(current->dest[0]));
            rcv->dest_len = (size_t)(current->dest_len);
            rcv->data = (void *)(current->packet);
            rcv->data_len = (size_t)(current->datagram_size);
        }

#else

        if (current->packet[0] == SIXLOWPAN_IPV6_DISPATCH) {
            (current->packet)++;
            (current->datagram_size)--;
        }

        rcv->src = (void *)(&(current->src[0]));
        rcv->src_len = (size_t)(current->src_len);
        rcv->dest = (void *)(&(current->dest[0]));
        rcv->dest_len = (size_t)(current->dest_len);
        rcv->data = (void *)(current->packet);
        rcv->data_len = (size_t)(current->datagram_size);

#endif

        _reassembly_buf_garbage_collect(current);

        if (res < 0) {
            return res;
        }
    }
    else {
        rcv->data_len = 0;
    }

    mutex_unlock(&_reassembly_mutex);

    return dispatch_len + fragment_size;
}

static int _handle_fragment(netapi_rcv_pkt_t *rcv, void *src, size_t src_len,
                            void *dest, size_t dest_len, uint8_t *data,
                            size_t data_len)
{
    uint8_t dispatch_len, datagram_offset, fragment_size;
    uint16_t datagram_size, datagram_tag;
    uint16_t byte_offset;

    mutex_lock(&_reassembly_mutex);
    _reassembly_buf_check_timeout();
    mutex_unlock(&_reassembly_mutex);

    DEBUG("Fragmentation dispatch 0x%02x received\n",
          data[0] & SIXLOWPAN_FRAG_HDR_MASK);

    /* get 11-bit from first 2 bytes as datagram_size */
    datagram_size = (((uint16_t)(data[0] << 8)) | data[1]) & 0x07ff;

    /* get 16-bit datagram tag */
    datagram_tag = (((uint16_t)(data[2] << 8)) | data[3]);

    switch (data[0] & SIXLOWPAN_FRAG_HDR_MASK) {
        case (SIXLOWPAN_FRAG1_DISPATCH):
            /* First Fragment */
            datagram_offset = 0;
            dispatch_len = SIXLOWPAN_FRAG1_HDR_LEN;

            break;

        case (SIXLOWPAN_FRAGN_DISPATCH):
            datagram_offset = data[4];
            dispatch_len = SIXLOWPAN_FRAGN_HDR_LEN;

            break;

        default:
            DEBUG("Unrecognize fragmentation dispatch\n");
            return -EINVAL;
    }

    fragment_size = data_len - dispatch_len;
    byte_offset = datagram_offset * 8;

    DEBUG(" * datagram_tag = %" PRIu16", fragment_size = %" PRIu8
          ", byte_offset is %" PRIu16 ", datagram_size is %" PRIu16 "\n",
          datagram_tag, fragments, byte_offset, datagram_size);

    if (((fragment_size % 8) != 0) &&   /* only the last fragment may not be divisible by 8 */
        (byte_offset + fragment_size) != datagram_size) {
        DEBUG("Invalid fragment size\n");
        return -EMSGSIZE;
    }

    return _reassembly_buf_add_fragment(rcv, src, src_len, dest, dest_len,
                                        data, byte_offset, datagram_size,
                                        datagram_tag, dispatch_len,
                                        fragment_size);
}

static int _handle_received(netapi_rcv_pkt_t *rcv, void *src, size_t src_len,
                            void *dest, size_t dest_len, uint8_t *data,
                            size_t data_len)
{
    if (!_AF_SUPPORTED(src_len) || !_AF_SUPPORTED(dest_len)) {
        return -EAFNOSUPPORT;
    }

    rcv->data_len = 0;  /* silence valgrind */

    if (data[0] == SIXLOWPAN_IPV6_DISPATCH) {
        data++;
        data_len--;

        rcv->src = src;
        rcv->src_len = src_len;
        rcv->dest = dest;
        rcv->dest_len = dest_len;
        rcv->data = (void *)data;
        rcv->data_len = data_len;

        return data_len;
    }
    else if (SIXLOWPAN_FRAG_DISPATCH(data)) {
        return _handle_fragment(rcv, src, src_len, dest, dest_len, data, data_len);
    }

#ifdef MODULE_SIXLOWPAN_HC
    else if ((data[0] & SIXLOWPAN_IPHC1_HDR_MASK) == SIXLOWPAN_IPHC1_DISPATCH) {
        int res = sixlowpan_hc_decode(rcv, src, src_len, dest, dest_len, data,
                                      data_len);

        if (res < 0) {
            return res;
        }

        return data_len;
    }

#endif
    else {
        DEBUG("Packet with unknown dispatch (0x%02x) received\n", data[0]);
        return -EINVAL;
    }
}

/* TODO: define transceiver equivalent */
static inline int _transfer_send(kernel_pid_t mac_pid, netdev_hlist_t *ulhs,
                                 void *dest, size_t dest_len, void *data,
                                 size_t data_len)
{
    return netapi_send_packet(mac_pid, ulhs, dest, dest_len, data, data_len);
}

static inline uint16_t _min(uint16_t a, size_t b)
{
    return (a < b) ? a : (uint16_t)b;
}

static uint16_t _snd_copy_headers(uint8_t *frag, netdev_hlist_t *ulh,
                                  netdev_hlist_t *ptr, uint8_t frag_hdr_len,
                                  uint16_t max_frag_size, uint16_t local_offset)
{
    do {
        uint16_t copy_len = _min(max_frag_size - local_offset + frag_hdr_len,
                                 ptr->header_len);

        if (local_offset >= max_frag_size) {
            break;
        }

        pktbuf_copy(&(frag[local_offset]), ptr->header, copy_len);
        local_offset += copy_len;
        netdev_hlist_advance(&ptr);
    } while (ptr != ulh);

    return local_offset;
}

static int _snd_send_1st_fragment(uint16_t *hdr_offset, kernel_pid_t mac_pid,
                                  netdev_hlist_t *ulh, size_t ulh_len, void *dest,
                                  size_t dest_len, uint8_t *data, size_t data_len,
                                  uint16_t max_frag_size, uint16_t tag)
{
    uint8_t *frag;
    uint16_t local_offset = 0;

    max_frag_size = ((max_frag_size - SIXLOWPAN_FRAG1_HDR_LEN) / 8) * 8;
                            /* round length down to nearest 8 byte alignment */

    frag = (uint8_t *)pktbuf_alloc(SIXLOWPAN_FRAG1_HDR_LEN + max_frag_size);

    if (frag == NULL) {
        return -ENOBUFS;
    }

    sixlowpan_init_frag1_dispatch(frag, data_len + ulh_len, tag);
                            /* no need to check result, since _snd_send_fragmented
                             * ensures maximum value of max_frag_size */

    local_offset += SIXLOWPAN_FRAG1_HDR_LEN;

    local_offset = _snd_copy_headers(frag, ulh, ulh, SIXLOWPAN_FRAG1_HDR_LEN,
                                     max_frag_size, local_offset);

    *hdr_offset = local_offset - SIXLOWPAN_FRAG1_HDR_LEN;

    if (local_offset < max_frag_size) {
        uint16_t remainding_bytes = _min(data_len + ulh_len,
                                         SIXLOWPAN_FRAG1_HDR_LEN + max_frag_size - local_offset);
        pktbuf_copy(&(frag[local_offset]), data, remainding_bytes);
        local_offset += remainding_bytes;
    }

    _transfer_send(mac_pid, NULL, dest, dest_len, frag, local_offset);

    pktbuf_release(frag);

    return local_offset - SIXLOWPAN_FRAG1_HDR_LEN;
}

static int _snd_send_nth_fragment(uint16_t *hdr_offset, kernel_pid_t mac_pid,
                                  netdev_hlist_t *ulh, size_t ulh_len, void *dest,
                                  size_t dest_len, uint8_t *data, size_t data_len,
                                  uint16_t max_frag_size, uint16_t tag,
                                  uint16_t datagram_byte_offset)
{
    uint8_t *frag;
    uint16_t local_offset = 0;
    uint16_t orig_hdr_offset = *hdr_offset;

    if ((datagram_byte_offset % 8) != 0) {
        return -EINVAL;
    }

    max_frag_size = ((max_frag_size - SIXLOWPAN_FRAGN_HDR_LEN) / 8) * 8;
                            /* round length down to nearest 8 byte alignment */

    frag = (uint8_t *)pktbuf_alloc(SIXLOWPAN_FRAGN_HDR_LEN + max_frag_size);

    if (frag == NULL) {
        return -ENOBUFS;
    }

    sixlowpan_init_fragn_dispatch(frag, data_len + ulh_len, tag, datagram_byte_offset / 8);
                            /* no need to check result, since _handle_send
                             * ensures maximum value of max_frag_size */

    local_offset += SIXLOWPAN_FRAGN_HDR_LEN;

    if (ulh_len > orig_hdr_offset) {
        netdev_hlist_t *ptr = ulh;

        if ((orig_hdr_offset - datagram_byte_offset) >= 0) {
            /* collect already started headers */
            uint8_t *hdr_data;
            uint16_t search_hdr_offset = ptr->header_len;

            while (search_hdr_offset < orig_hdr_offset) {
                netdev_hlist_advance(&ptr);
                search_hdr_offset += ptr->header_len;
            }

            hdr_data = (uint8_t *)(ptr->header);
            search_hdr_offset -= orig_hdr_offset;
            pktbuf_copy(&(frag[local_offset]), &(hdr_data[ptr->header_len - search_hdr_offset]),
                        search_hdr_offset);
            local_offset += search_hdr_offset;
        }

        if (ulh != ptr->next) {
            /* collect the rest of the headers */
            local_offset = _snd_copy_headers(frag, ulh, ptr->next, SIXLOWPAN_FRAGN_HDR_LEN,
                                             max_frag_size, local_offset);
        }

        *hdr_offset = orig_hdr_offset + local_offset - SIXLOWPAN_FRAGN_HDR_LEN;
    }

    if (local_offset < max_frag_size) {
        uint16_t remainding_bytes = _min(data_len + ulh_len - datagram_byte_offset - *hdr_offset +
                                         orig_hdr_offset,
                                         SIXLOWPAN_FRAGN_HDR_LEN + max_frag_size - local_offset);
        pktbuf_copy(&(frag[local_offset]), &(data[datagram_byte_offset - orig_hdr_offset]),
                    remainding_bytes);
        local_offset += remainding_bytes;
    }

    _transfer_send(mac_pid, NULL, dest, dest_len, frag, local_offset);

    pktbuf_release(frag);

    return local_offset - SIXLOWPAN_FRAGN_HDR_LEN;
}

static int _snd_send_fragmented(kernel_pid_t mac_pid, netdev_hlist_t *ulh,
                                size_t ulh_len, void *dest, size_t dest_len,
                                void *data, size_t data_len,
                                size_t max_frag_size)
{
    uint16_t tag;
    int res;
    uint16_t datagram_offset = 0, hdr_offset = 0;

    mutex_lock(&_current_frag_tag_mutex);
    tag = _current_frag_tag++;
    mutex_unlock(&_current_frag_tag_mutex);

    if ((res = _snd_send_1st_fragment(&hdr_offset, mac_pid, ulh, ulh_len, dest, dest_len, data,
                                      data_len, max_frag_size, tag)) < 0) {
        return res;
    }

    datagram_offset += (uint16_t)res;

    while (datagram_offset < ulh_len + data_len) {
        if ((res = _snd_send_nth_fragment(&hdr_offset, mac_pid, ulh, ulh_len, dest, dest_len, data,
                                          data_len, max_frag_size, tag, datagram_offset)) < 0) {
            return res;
        }

        datagram_offset += (uint16_t)res;
    }

    return data_len;
}

static int _handle_send(kernel_pid_t mac_pid, netdev_hlist_t **ulh, void *dest,
                        size_t dest_len, void *data, size_t data_len,
                        size_t max_frame_size, sixlowpan_hc_status_t hc_status)
{
    int res;
    size_t ulh_len = netdev_get_hlist_len(*ulh);
    uint8_t *dispatch = (uint8_t *)pktbuf_alloc(ulh_len + 1); /* 6LoWPAN dispatch data */
    size_t dispatch_len = 0;
    netdev_hlist_t *dispatch_hdr = (netdev_hlist_t *)pktbuf_alloc(sizeof(netdev_hlist_t));

    dispatch_hdr->header = (void *)(&(dispatch[0]));

#ifdef MODULE_SIXLOWPAN_HC

    if (hc_status == SIXLOWPAN_HC_ENABLE) {
        size_t src_len = 8;
        uint8_t src[src_len];

        if ((res = netapi_get_option(mac_pid, (netapi_conf_type_t)NETDEV_OPT_SRC_LEN,
                                     &src_len, sizeof(size_t))) < 0) {
            if (res == -ENOTSUP) {
                src_len = 2;
            }
            else {
                return res;
            }
        }

        if (src_len > 2) {
            if ((res = netapi_get_option(mac_pid, (netapi_conf_type_t)NETDEV_OPT_ADDRESS_LONG,
                                         &src, sizeof(src))) < 0) {
                if (res == -ENOTSUP) {
                    if ((res = netapi_get_option(mac_pid,
                                                 (netapi_conf_type_t)NETDEV_OPT_ADDRESS, &src,
                                                 sizeof(src))) < 0) {
                        return res;
                    }
                }
                else {
                    return res;
                }
            }
        }
        else {
            if ((res = netapi_get_option(mac_pid, (netapi_conf_type_t)NETDEV_OPT_ADDRESS,
                                         &src, sizeof(src))) < 0) {
                return res;
            }
        }

        src_len = (size_t)res;

        dispatch_hdr->header_len = ulh_len + 1;

        if ((res = sixlowpan_hc_encode(dispatch_hdr, ulh, src, src_len, dest,
                                       dest_len)) < 0) {
            return res;
        }
    }
    else {
        dispatch[dispatch_len++] = SIXLOWPAN_IPV6_DISPATCH;
        dispatch_hdr->header_len = dispatch_len;
    }

#else

    (void)hc_status;
    dispatch[dispatch_len++] = SIXLOWPAN_IPV6_DISPATCH;
    dispatch_hdr->header_len = dispatch_len;

#endif

    dispatch_hdr->protocol = NETDEV_PROTO_6LOWPAN;

    netdev_hlist_add(ulh, dispatch_hdr);
    ulh_len += dispatch_len;

    if (ulh_len + data_len > max_frame_size) {
        if (ulh_len + data_len > SIXLOWPAN_FRAG_MAX_DATAGRAM_LEN) {
            if (ulh != NULL && *ulh != NULL) {
                netdev_hlist_remove(ulh, dispatch_hdr);
            }

            pktbuf_release(dispatch);
            pktbuf_release(dispatch_hdr);

            return -EMSGSIZE;
        }

        res = _snd_send_fragmented(mac_pid, *ulh, ulh_len, dest, dest_len, data, data_len, max_frame_size);
    }
    else {
        res = _transfer_send(mac_pid, *ulh, dest, dest_len, data, data_len);
    }

    if (ulh != NULL && *ulh != NULL) {
        netdev_hlist_remove(ulh, dispatch_hdr);
    }

    pktbuf_release(dispatch);
    pktbuf_release(dispatch_hdr);

    return res;
}

static int _handle_get_option(kernel_pid_t mac_pid, kernel_pid_t *registry,
                              sixlowpan_hc_status_t hc_status,
                              netapi_conf_t *conf)
{
#ifndef MODULE_SIXLOWPAN_HC
    (void)hc_status;
#endif

    switch ((sixlowpan_conf_t)conf->param) {
        case SIXLOWPAN_CONF_CHANNEL:
        case SIXLOWPAN_CONF_ADDRESS:
        case SIXLOWPAN_CONF_ADDRESS_LONG:
        case SIXLOWPAN_CONF_PAN:
        case SIXLOWPAN_CONF_SRC_LEN:
            return netapi_get_option(mac_pid, conf->param, conf->data,
                                     conf->data_len);

        case SIXLOWPAN_CONF_PROTO:
            if (conf->data_len < sizeof(netdev_proto_t)) {
                return -EOVERFLOW;
            }
            else {
                netdev_proto_t *proto = conf->data;
                *proto = NETDEV_PROTO_6LOWPAN;
                conf->data_len = sizeof(netdev_proto_t);
                return conf->data_len;
            }

        case SIXLOWPAN_CONF_MAX_PACKET_SIZE:
            if (conf->data_len < sizeof(size_t)) {
                return -EOVERFLOW;
            }
            else {
                size_t *max_packet_size = conf->data;
                *max_packet_size = SIXLOWPAN_FRAG_MAX_DATAGRAM_LEN;
                conf->data_len = sizeof(size_t);
                return conf->data_len;
            }

        case SIXLOWPAN_CONF_REGISTRY:
            do {
                int len = 0;
                kernel_pid_t *reg = conf->data;

                for (int i = 0; i < _REGISTRY_SIZE; i++) {
                    if (registry[i] != KERNEL_PID_UNDEF) {
                        len++;

                        if (conf->data_len < sizeof(kernel_pid_t) * len) {
                            return -EOVERFLOW;
                        }

                        reg[len - 1] = registry[i];
                    }
                }

                conf->data_len = sizeof(kernel_pid_t) * len;
                return conf->data_len;
            } while (0);


#ifdef MODULE_SIXLOWPAN_HC

        case SIXLOWPAN_CONF_IPHC_CONTEXT:
            conf->data = (void *)sixlowpan_iphc_cbuf_lookup_cid(0);
            conf->data_len = sizeof(sixlowpan_iphc_cbuf_t) *
                             SIXLOWPAN_IPHC_CBUF_SIZE;

            return conf->data_len;

        case SIXLOWPAN_CONF_HC_STATUS:
            if (conf->data_len < sizeof(sixlowpan_hc_status_t)) {
                return -EOVERFLOW;
            }
            else {
                sixlowpan_hc_status_t *status = conf->data;
                *status = hc_status;
                conf->data_len = sizeof(sixlowpan_hc_status_t);
                return conf->data_len;
            }

#endif /* MODULE_SIXLOWPAN_HC */

        default:
            break;
    }

    return -ENOTSUP;
}

static int _handle_set_option(kernel_pid_t mac_pid,
                              sixlowpan_hc_status_t *hc_status,
                              netapi_conf_t *conf)
{
#ifndef MODULE_SIXLOWPAN_HC
    (void)hc_status;
#endif

    switch ((sixlowpan_conf_t)conf->param) {
        case SIXLOWPAN_CONF_CHANNEL:
        case SIXLOWPAN_CONF_ADDRESS:
        case SIXLOWPAN_CONF_ADDRESS_LONG:
        case SIXLOWPAN_CONF_PAN:
        case SIXLOWPAN_CONF_SRC_LEN:
            return netapi_set_option(mac_pid, conf->param, conf->data,
                                     conf->data_len);

#ifdef MODULE_SIXLOWPAN_HC

        case SIXLOWPAN_CONF_IPHC_CONTEXT:
            do {
                sixlowpan_iphc_cbuf_t *c = (sixlowpan_iphc_cbuf_t *)conf->data;

                if (conf->data_len != sizeof(sixlowpan_iphc_cbuf_t)) {
                    return -EINVAL;
                }

                if (sixlowpan_iphc_cbuf_update(c->cid, &c->prefix, c->prefix_len,
                                               c->lifetime) == NULL) {
                    return -EINVAL;
                }
            } while (0);

            return NETAPI_STATUS_OK;

        case SIXLOWPAN_CONF_HC_STATUS:
            if (conf->data_len != sizeof(sixlowpan_hc_status_t)) {
                return -EINVAL;
            }
            else {
                sixlowpan_hc_status_t *status = conf->data;
                *hc_status = *status;
                return NETAPI_STATUS_OK;
            }

            break;

#endif /* MODULE_SIXLOWPAN_HC */

        default:
            break;
    }

    return -ENOTSUP;
}

static void _pktbuf_hold_receive(netapi_rcv_pkt_t *rcv)
{
    if (!pktbuf_contains(rcv->data)) {
        rcv->data = pktbuf_insert(rcv->data, rcv->data_len);
    }
    else {
        pktbuf_hold(rcv->data);
    }
}

static void _pktbuf_release_receive(netapi_rcv_pkt_t *rcv)
{
    pktbuf_release(rcv->data);
}

static void _pktbuf_hold_send(netapi_snd_pkt_t *snd)
{
    netdev_hlist_t *ptr = snd->ulh;

    if (!pktbuf_contains(snd->data)) {
        snd->data = pktbuf_insert(snd->data, snd->data_len);
    }
    else {
        pktbuf_hold(snd->data);
    }

    if (!pktbuf_contains(snd->dest)) {
        snd->dest = pktbuf_insert(snd->dest, snd->dest_len);
    }
    else {
        pktbuf_hold(snd->dest);
    }

    if (ptr == NULL) {
        snd->ulh = NULL;
        return;
    }

    do {
        if (!pktbuf_contains(ptr->header)) {
            ptr->header = pktbuf_insert(ptr->header, ptr->header_len);
        }
        else {
            pktbuf_hold(ptr->header);
        }

        netdev_hlist_advance(&ptr);
    } while (ptr != snd->ulh);
}

static void _pktbuf_release_send(netapi_snd_pkt_t *snd)
{
    netdev_hlist_t *ptr = snd->ulh;

    pktbuf_release(snd->data);
    pktbuf_release(snd->dest);


    if (ptr == NULL) {
        return;
    }

    do {
        netdev_hlist_t *prev = ptr;
        pktbuf_release(ptr->header);
        netdev_hlist_advance(&ptr);
        pktbuf_release(prev);
    } while (ptr != snd->ulh);
}

static void *_control(void *arg)
{
    msg_t msg_cmd, msg_ack, msg_queue[_MSG_QUEUE_SIZE];
    netapi_cmd_t *cmd;
    kernel_pid_t mac_pid, registry[_REGISTRY_SIZE];
    size_t max_mac_data_len;
    uint8_t replied = 0;

#ifdef MODULE_SIXLOWPAN_HC
    sixlowpan_hc_status_t hc_status = SIXLOWPAN_HC_ENABLE;
#else
    sixlowpan_hc_status_t hc_status = SIXLOWPAN_HC_DISABLE;
#endif

    for (int i = 0; i < _REGISTRY_SIZE; i++) {
        registry[i] = KERNEL_PID_UNDEF;
    }

    msg_init_queue(msg_queue, _MSG_QUEUE_SIZE);

    mac_pid = *((kernel_pid_t *)arg);

    do {
        int res;

        if ((res = netapi_get_option(mac_pid, NETAPI_CONF_MAX_PACKET_SIZE, &max_mac_data_len,
                                     sizeof(size_t))) < 0) {
            if (res == -ENOTSUP) {
                max_mac_data_len = _DEFAULT_MAX_MAC_DATA_LEN;
            }
            else {
                DEBUG("Critical error: Thread communication could not be established properly\n");
                return NULL;
            }
        }
    } while (0);

    while (1) {
        netapi_ack_t *ack;

        msg_receive(&msg_cmd);

        /* TODO: add transceive backwards compatibility */

        if (msg_cmd.type != NETAPI_MSG_TYPE) {
            msg_ack.type = 0;
            msg_reply(&msg_cmd, &msg_ack);
        }

        msg_ack.type = NETAPI_MSG_TYPE;

        cmd = (netapi_cmd_t *)msg_cmd.content.ptr;

        ack = cmd->ack;
        ack->type = NETAPI_CMD_ACK;
        ack->orig = cmd->type;

        msg_ack.content.ptr = (char *)ack;

        switch (cmd->type) {
            case NETAPI_CMD_RCV:
                do {
                    DEBUG("Received receive command from %d\n", msg_cmd.sender_pid);
                    netapi_rcv_pkt_t rcv;

                    _pktbuf_hold_receive((netapi_rcv_pkt_t *)cmd);
                    ack->result = _handle_received(&rcv,
                                                   ((netapi_rcv_pkt_t *)cmd)->src,
                                                   ((netapi_rcv_pkt_t *)cmd)->src_len,
                                                   ((netapi_rcv_pkt_t *)cmd)->dest,
                                                   ((netapi_rcv_pkt_t *)cmd)->dest_len,
                                                   ((netapi_rcv_pkt_t *)cmd)->data,
                                                   ((netapi_rcv_pkt_t *)cmd)->data_len);
                    _pktbuf_release_receive((netapi_rcv_pkt_t *)cmd);

                    msg_reply(&msg_cmd, &msg_ack);

                    replied = 1;

                    if (rcv.data_len > 0) {
                        /* TODO: check rcv.data for HC */
                        if (_transfer_received(&rcv, registry)) {
                            DEBUG("Received error for datagram %" PRIu16 "\n",
                                  current->datagram_tag);
                        }

                        pktbuf_release(rcv.src);    /* release entry in reassembly buffer
                                                     * since position just needs to be in the frame
                                                     */
                        pktbuf_release(rcv.data);   /* release packet data */

                    }

                } while (0);

                break;

            case NETAPI_CMD_SND:
                DEBUG("Received send command from %d\n", msg_cmd.sender_pid);
                ack->result = -EAFNOSUPPORT;

                if (_AF_SUPPORTED(((netapi_snd_pkt_t *)cmd)->dest_len)) {
                    ack->result = ((netapi_snd_pkt_t *)cmd)->data_len;
                }

                _pktbuf_hold_send((netapi_snd_pkt_t *)cmd);

                msg_reply(&msg_cmd, &msg_ack);

                replied = 1;

                if (ack->result >= 0) {
                    int result = _handle_send(mac_pid, &(((netapi_snd_pkt_t *)cmd)->ulh),
                                              ((netapi_snd_pkt_t *)cmd)->dest,
                                              ((netapi_snd_pkt_t *)cmd)->dest_len,
                                              ((netapi_snd_pkt_t *)cmd)->data,
                                              ((netapi_snd_pkt_t *)cmd)->data_len,
                                              max_mac_data_len, hc_status);
                    (void)result;
                    DEBUG((result < 0) ?
                          "Error on sending to lower layer: error code %d\n" :
                          "Successfully transfered %d byte to lower level\n",
                          result);
                }

                _pktbuf_release_send((netapi_snd_pkt_t *)cmd);

                break;

            case NETAPI_CMD_GET:
                DEBUG("Received get option command from %d\n", msg_cmd.sender_pid);

                ack->result = _handle_get_option(mac_pid, registry, hc_status, (netapi_conf_t *)cmd);

                break;

            case NETAPI_CMD_SET:
                DEBUG("Received set option command from %d\n", msg_cmd.sender_pid);

                ack->result = _handle_set_option(mac_pid, &hc_status, (netapi_conf_t *)cmd);

                break;

            case NETAPI_CMD_REG:
                DEBUG("Received registration command from %d\n", msg_cmd.sender_pid);

                ack->result = -ENOBUFS;

                for (unsigned int i = 0; i < _REGISTRY_SIZE; i++) {
                    if (registry[i] == KERNEL_PID_UNDEF) {
                        registry[i] = ((netapi_reg_t *)cmd)->reg_pid;
                        ack->result = NETAPI_STATUS_OK;

                        break;
                    }
                }

                break;

            case NETAPI_CMD_UNREG:
                DEBUG("Received unregistration command from %d\n", msg_cmd.sender_pid);

                ack->result = NETAPI_STATUS_OK;

                for (unsigned int i = 0; i < _REGISTRY_SIZE; i++) {
                    if (registry[i] == ((netapi_reg_t *)cmd)->reg_pid) {
                        registry[i] = KERNEL_PID_UNDEF;

                        break;
                    }
                }

                break;

            default:
                DEBUG("Received unknown command from %d\n", msg_cmd.sender_pid);

                ack->result = -ENOTSUP;

                break;
        }

        if (!replied) {
            msg_reply(&msg_cmd, &msg_ack);
        }
        else {
            replied = 0;
        }
    }

    return NULL;
}

sixlowpan_hc_status_t sixlowpan_hc_get_status(kernel_pid_t pid)
{
#ifdef MODULE_SIXLOWPAN_HC
    sixlowpan_hc_status_t status;

    if (netapi_set_option(pid, (netapi_conf_type_t)SIXLOWPAN_CONF_HC_STATUS,
                          &status, sizeof(sixlowpan_hc_status_t)) < 0) {
        return SIXLOWPAN_HC_DISABLE;
    }

    return status;
#else
    (void)pid;
    return SIXLOWPAN_HC_DISABLE;
#endif
}

kernel_pid_t sixlowpan_init(kernel_pid_t mac_pid, char *stack, int stacksize,
                            char priority, int flags, const char *name)
{
    kernel_pid_t mypid = thread_create(stack, stacksize, priority, flags, _control, &mac_pid, name);
    netapi_register(mac_pid, mypid, 0);
    return mypid;
}

/**
 * @}
 */
