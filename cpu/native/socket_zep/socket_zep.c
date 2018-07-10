/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "async_read.h"
#include "byteorder.h"
#include "checksum/ucrc16.h"
#include "native_internal.h"
#include "random.h"

#include "socket_zep.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _UNIX_NTP_ERA_OFFSET    (2208988800U)

static size_t _zep_hdr_fill_v2_data(socket_zep_t *dev, zep_v2_data_hdr_t *hdr,
                                    size_t payload_len)
{
    struct timeval tv;

    real_gettimeofday(&tv, NULL);
    hdr->hdr.version = 2;
    hdr->type = ZEP_V2_TYPE_DATA;
    hdr->chan = dev->netdev.chan;
    hdr->dev = byteorder_htons((uint16_t)((((intptr_t)dev)) & 0xffff));
    hdr->lqi_mode = 1;
    hdr->lqi_val = 0xff;                /* TODO: set */
    hdr->time.seconds = byteorder_htonl(tv.tv_sec + _UNIX_NTP_ERA_OFFSET);
    hdr->time.fraction = byteorder_htonl((tv.tv_usec * 1000000) / 232);
    hdr->seq = byteorder_htonl(dev->seq);
    memset(hdr->resv, 0, sizeof(hdr->resv));
    hdr->length = payload_len;

    return sizeof(zep_v2_data_hdr_t);
}

static inline size_t _zep_hdr_fill(socket_zep_t *dev, zep_hdr_t *hdr,
                                   size_t payload_len)
{
    hdr->preamble[0] = 'E';
    hdr->preamble[1] = 'X';

    /* keep possibility for ZEPv1 open */
    return _zep_hdr_fill_v2_data(dev, (zep_v2_data_hdr_t *)hdr,
                                 payload_len);
}

static size_t _prep_vector(socket_zep_t *dev, const iolist_t *iolist,
                           unsigned n, struct iovec *out)
{
    size_t bytes;
    dev->chksum_buf = 0;

    bytes = iolist_size(iolist);
    bytes += sizeof(uint16_t); /* FCS field */
    out[0].iov_base = &dev->snd_hdr_buf;
    out[0].iov_len = _zep_hdr_fill(dev, out[0].iov_base, bytes);
    for (unsigned i = 0; i < n; i++) {
        /* discard const qualifier, we won't change anything. Promise! */
        out[i + 1].iov_base = iolist->iol_base;
        out[i + 1].iov_len = iolist->iol_len;
        dev->chksum_buf = ucrc16_calc_le(out[i + 1].iov_base, out[i + 1].iov_len,
                                         UCRC16_CCITT_POLY_LE, dev->chksum_buf);
        iolist = iolist->iol_next;
    }
    dev->chksum_buf = byteorder_btols(byteorder_htons(dev->chksum_buf)).u16;
    out[n + 1].iov_base = &dev->chksum_buf;
    out[n + 1].iov_len = sizeof(uint16_t);
    return bytes;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    socket_zep_t *dev = (socket_zep_t *)netdev;
    unsigned n = iolist_count(iolist);
    struct iovec v[n + 2];
    size_t bytes;
    int res;

    assert((dev != NULL) && (dev->sock_fd != 0));
    bytes = _prep_vector(dev, iolist, n, v);
    DEBUG("socket_zep::send(%p, %p, %u)\n", (void *)netdev, (void *)iolist, n);
    /* simulate TX_STARTED interrupt */
    if (netdev->event_callback) {
        dev->last_event = NETDEV_EVENT_TX_STARTED;
        netdev->event_callback(netdev, NETDEV_EVENT_ISR);
        thread_yield();
    }
    res = writev(dev->sock_fd, v, n + 2);
    if (res < 0) {
        DEBUG("socket_zep::send: error writing packet: %s\n", strerror(errno));
        return res;
    }
    /* simulate TX_COMPLETE interrupt */
    if (netdev->event_callback) {
        dev->last_event = NETDEV_EVENT_TX_COMPLETE;
        netdev->event_callback(netdev, NETDEV_EVENT_ISR);
        thread_yield();
    }
#ifdef MODULE_NETSTATS_L2
    netdev->stats.tx_bytes += bytes;
#else
    (void)bytes;
#endif

    return res - v[0].iov_len - v[n + 1].iov_len;
}

static void _continue_reading(socket_zep_t *dev)
{
    /* work around lost signals */
    fd_set rfds;
    struct timeval t;
    memset(&t, 0, sizeof(t));
    FD_ZERO(&rfds);
    FD_SET(dev->sock_fd, &rfds);

    _native_in_syscall++; /* no switching here */

    if (real_select(dev->sock_fd + 1, &rfds, NULL, NULL, &t) == 1) {
        int sig = SIGIO;
        extern int _sig_pipefd[2];
        extern ssize_t (*real_write)(int fd, const void * buf, size_t count);
        real_write(_sig_pipefd[1], &sig, sizeof(int));
        _native_sigpend++;
    }
    else {
        native_async_read_continue(dev->sock_fd);
    }

    _native_in_syscall--;
}

static inline bool _dst_not_me(socket_zep_t *dev, const void *buf)
{
    uint8_t dst_addr[IEEE802154_LONG_ADDRESS_LEN] = { 0 };
    int dst_len;
    le_uint16_t dst_pan = { .u16 = 0 };

    dst_len = ieee802154_get_dst(buf, dst_addr,
                                 &dst_pan);
    switch (dst_len) {
        case IEEE802154_LONG_ADDRESS_LEN:
            return memcmp(dst_addr, dev->netdev.long_addr, dst_len) != 0;
        case IEEE802154_SHORT_ADDRESS_LEN:
            return (memcmp(dst_addr, ieee802154_addr_bcast, dst_len) != 0) &&
                   (memcmp(dst_addr, dev->netdev.short_addr, dst_len) != 0);
        default:
            return false;    /* better safe than sorry ;-) */
    }
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    socket_zep_t *dev = (socket_zep_t *)netdev;
    int size = 0;

    DEBUG("socket_zep::recv(%p, %p, %u, %p)\n", (void *)netdev, buf,
          (unsigned)len, (void *)info);
    if ((buf == NULL) || (len == 0)) {
        int res = real_ioctl(dev->sock_fd, FIONREAD, &size);
#if ENABLE_DEBUG
        if (res < 0) {
            DEBUG("socket_zep::recv: error reading FIONREAD: %s",
                  strerror(errno));
        }
#else
        (void)res;
#endif
        return size;
    }
    else if (len > 0) {
        size = real_read(dev->sock_fd, dev->rcv_buf, sizeof(dev->rcv_buf));

        if (size > 0) {
            zep_hdr_t *tmp = (zep_hdr_t *)&dev->rcv_buf;

            if ((tmp->preamble[0] != 'E') || (tmp->preamble[1] != 'X')) {
                DEBUG("socket_zep::recv: invalid ZEP header");
                return -1;
            }
            switch (tmp->version) {
                case 2: {
                    zep_v2_data_hdr_t *zep = (zep_v2_data_hdr_t *)tmp;
                    void *payload = &dev->rcv_buf[sizeof(zep_v2_data_hdr_t)];

                    if (zep->type != ZEP_V2_TYPE_DATA) {
                        DEBUG("socket_zep::recv: unexpect ZEP type\n");
                        /* don't support ACK frames for now*/
                        return -1;
                    }
                    if (((sizeof(zep_v2_data_hdr_t) + zep->length) != (unsigned)size) ||
                        (zep->length > len) || (zep->chan != dev->netdev.chan) ||
                        /* TODO promiscous mode */
                        _dst_not_me(dev, payload)) {
                        /* TODO: check checksum */
                        return -1;
                    }
                    /* don't hand FCS to stack */
                    size = zep->length - sizeof(uint16_t);
                    if (buf != NULL) {
                        memcpy(buf, payload, size);
                        if (info != NULL) {
                            struct netdev_radio_rx_info *rx_info = info;
                            rx_info->lqi = zep->lqi_val;
                            rx_info->rssi = UINT8_MAX;
                        }
                    }
                    break;
                }
                default:
                    DEBUG("socket_zep::recv: unexpected ZEP version\n");
                    return -1;
            }
        }
        else if (size == 0) {
            DEBUG("socket_zep::recv: ignoring null-event\n");
            return -1;
        }
        else if (size == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            }
            else {
                err(EXIT_FAILURE, "zep: read");
            }
        }
        else {
            errx(EXIT_FAILURE, "internal error _rx_event");
        }
    }
    _continue_reading(dev);
#ifdef MODULE_NETSTATS_L2
    netdev->stats.rx_count++;
    netdev->stats.rx_bytes += size;
#endif
    return size;
}

static void _isr(netdev_t *netdev)
{
    if (netdev->event_callback) {
        socket_zep_t *dev = (socket_zep_t *)netdev;

        DEBUG("socket_zep::isr: firing %u\n", (unsigned)dev->last_event);
        netdev->event_callback(netdev, dev->last_event);
    }
    return;
}

static void _socket_isr(int fd, void *arg)
{
    (void)fd;
    (void)arg;
    netdev_t *netdev = (netdev_t *)arg;

    DEBUG("socket_zep::_socket_isr: %d, %p (netdev == %p)\n",
          fd, arg, (void *)netdev);
    if (netdev == NULL) {
        return;
    }
    if (netdev->event_callback) {
        socket_zep_t *dev = (socket_zep_t *)netdev;

        dev->last_event = NETDEV_EVENT_RX_COMPLETE;
        netdev->event_callback(netdev, NETDEV_EVENT_ISR);
    }
}

static int _init(netdev_t *netdev)
{
    socket_zep_t *dev = (socket_zep_t *)netdev;

    netdev_ieee802154_reset(&dev->netdev);

    assert(dev != NULL);
    dev->netdev.chan = IEEE802154_DEFAULT_CHANNEL;
    dev->netdev.pan = IEEE802154_DEFAULT_PANID;

    return 0;
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    socket_zep_t *dev = (socket_zep_t *)netdev;
    uint16_t *v = value;

    assert((dev != NULL));
    switch (opt) {
        case NETOPT_MAX_PACKET_SIZE:
            assert(value != NULL);
            if (max_len != sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            *v = SOCKET_ZEP_FRAME_PAYLOAD_LEN;
            return sizeof(uint16_t);
        default:
            return netdev_ieee802154_get(&dev->netdev, opt, value, max_len);
    }
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value,
                size_t value_len)
{
    assert(netdev != NULL);
    return netdev_ieee802154_set((netdev_ieee802154_t *)netdev, opt,
                                  value, value_len);
}

static const netdev_driver_t socket_zep_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

void socket_zep_setup(socket_zep_t *dev, const socket_zep_params_t *params)
{
    static const struct addrinfo hints = { .ai_family = AF_UNSPEC,
                                           .ai_socktype = SOCK_DGRAM };
    struct addrinfo *ai = NULL, *remote;
    int res;

    DEBUG("socket_zep_setup(%p, %p)\n", (void *)dev, (void *)params);
    assert((params->local_addr != NULL) && (params->local_port != NULL) &&
           (params->remote_addr != NULL) && (params->remote_port != NULL));
    memset(dev, 0, sizeof(socket_zep_t));
    dev->netdev.netdev.driver = &socket_zep_driver;
    /* bind and connect socket */
    if ((res = real_getaddrinfo(params->local_addr, params->local_port, &hints,
                                &ai)) < 0) {
        errx(EXIT_FAILURE, "ZEP: unable to get local address: %s\n",
             gai_strerror(res));
    }
    for (struct addrinfo *local = ai; local != NULL; local = local->ai_next) {
        if ((res = real_socket(local->ai_family, local->ai_socktype,
                               local->ai_protocol)) < 0) {
            continue;
        }
        if (real_bind(res, local->ai_addr, local->ai_addrlen) == 0) {
            break;  /* successfully bound */
        }
    }
    real_freeaddrinfo(ai);
    if (res < 0) {
        err(EXIT_FAILURE, "ZEP: Unable to bind socket");
    }
    dev->sock_fd = res;
    ai = NULL;
    if ((res = real_getaddrinfo(params->remote_addr, params->remote_port, &hints,
                                &ai)) < 0) {
        errx(EXIT_FAILURE, "ZEP: unable to get remote address: %s\n",
             gai_strerror(res));
    }
    for (remote = ai; remote != NULL; remote = remote->ai_next) {
        if (real_connect(dev->sock_fd, remote->ai_addr, remote->ai_addrlen) == 0) {
            break;  /* successfully connected */
        }
    }
    if (remote == NULL) {
        err(EXIT_FAILURE, "ZEP: Unable to connect socket");
    }
    real_freeaddrinfo(ai);

    /* generate hardware address from local address */
    uint8_t ss_array[sizeof(struct sockaddr_storage)] = { 0 };
    socklen_t ss_len = sizeof(struct sockaddr_storage);

    if (getpeername(dev->sock_fd, (struct sockaddr *)&ss_array, &ss_len) < 0) {
        err(EXIT_FAILURE, "ZEP: Unable to retrieve remote address");
    }
    assert(ss_len >= IEEE802154_LONG_ADDRESS_LEN);
    if (getsockname(dev->sock_fd, (struct sockaddr *)&ss_array, &ss_len) < 0) {
        err(EXIT_FAILURE, "ZEP: Unable to retrieve local address");
    }
    assert(ss_len >= IEEE802154_LONG_ADDRESS_LEN);

    /* generate hardware address from socket address and port info */
    dev->netdev.long_addr[1] = 'Z';     /* The "OUI" */
    dev->netdev.long_addr[2] = 'E';
    dev->netdev.long_addr[3] = 'P';
    for (unsigned i = 0; i < ss_len; i++) { /* generate NIC from local source */
        unsigned addr_idx = (i % (IEEE802154_LONG_ADDRESS_LEN / 2)) +
                            (IEEE802154_LONG_ADDRESS_LEN / 2);
        dev->netdev.long_addr[addr_idx] ^= ss_array[i];
    }
    dev->netdev.short_addr[0] = dev->netdev.long_addr[6];
    dev->netdev.short_addr[1] = dev->netdev.long_addr[7];
    native_async_read_setup();
    native_async_read_add_handler(dev->sock_fd, dev, _socket_isr);
#ifdef MODULE_NETSTATS_L2
    memset(&dev->netdev.netdev.stats, 0, sizeof(netstats_t));
#endif
}

void socket_zep_cleanup(socket_zep_t *dev)
{
    assert(dev != NULL);
    /* cleanup signal handling */
    native_async_read_cleanup();
    /* close the socket */
    close(dev->sock_fd);
    dev->sock_fd = 0;
}

/** @} */
