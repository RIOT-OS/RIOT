/*
 * Copyright (C) 2016 Alexander Aring <aar@pengutronix.de>
 * Copyright (C) 2015 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>,
 *                    Martine Lenders <mlenders@inf.fu-berlin.de>
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *                    Ell-i open source co-operative
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* needs to be included before native's declarations of ntohl etc. */
#include "byteorder.h"

#ifdef __MACH__
#define _POSIX_C_SOURCE
#include <net/if.h>
#undef _POSIX_C_SOURCE
#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if_dl.h>
#elif defined(__FreeBSD__)
#include <sys/socket.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <net/if_dl.h>
#else
#include <net/if.h>
#include <linux/if_tun.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#endif

#include "native_internal.h"

#include "net/netdev.h"
#include "netdev_raw802154.h"
#include "net/netopt.h"

#include "periph/hwrng.h"

#include "async_read.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef ETH_P_IEEE802154
#define ETH_P_IEEE802154 0x00F6
#endif

#define RAW802154_DEFAULT_ADDR_PAN      (0x0023)
#define RAW802154_DEFAULT_ADDR_SHORT    (0xfffe)
#define RAW802154_DEFAULT_ADDR_LONG     (0x1222334455667788)

/* support one raw802154 interface for now */
netdev_raw802154_t netdev_raw802154;

void _set_page(netdev_raw802154_t *dev, uint8_t page)
{
    dev->page = page;
}

static uint8_t _get_page(netdev_raw802154_t *dev)
{
    return dev->page;
}

static void _set_chan(netdev_raw802154_t *dev, uint8_t channel)
{
    dev->netdev.chan = channel;
}

static inline void _isr(netdev_t *netdev)
{
    if (netdev->event_callback)
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
}

static void _raw802154_isr(int fd, void *arg) {
    (void) fd;
    (void) arg;
    netdev_t *netdev = (netdev_t *)&netdev_raw802154;

    if (netdev->event_callback)
        netdev->event_callback(netdev, NETDEV_EVENT_ISR);
}

static void _set_addr_short(netdev_raw802154_t *dev, uint16_t addr)
{
    dev->netdev.short_addr[0] = (uint8_t)(addr);
    dev->netdev.short_addr[1] = (uint8_t)(addr >> 8);
}

static void _set_addr_long(netdev_raw802154_t *dev, uint64_t addr)
{
    for (int i = 0; i < 8; i++)
        dev->netdev.long_addr[i] = (uint8_t)(addr >> (i * 8));
}

static void _set_pan(netdev_raw802154_t *dev, uint16_t pan)
{
    dev->netdev.pan = pan;
}

static int16_t _get_txpower(netdev_raw802154_t *dev)
{
    (void)dev;
    return 0;
}

static uint8_t _get_max_retries(netdev_raw802154_t *dev)
{
    (void)dev;
    return 0;
}

#define _MAX_MHR_OVERHEAD   (25)
static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    netdev_raw802154_t *dev = (netdev_raw802154_t *)netdev;

    switch (opt) {
        case NETOPT_STATE:
            if (max_len < sizeof(netopt_state_t)) {
                return -EOVERFLOW;
            }
            *((netopt_state_t *)val) = NETOPT_STATE_RX;
            return sizeof(netopt_state_t);
        case NETOPT_CHANNEL_PAGE:
            if (max_len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            ((uint8_t *)val)[1] = 0;
            ((uint8_t *)val)[0] = _get_page(dev);
            return sizeof(uint16_t);
        case NETOPT_MAX_PACKET_SIZE:
            if (max_len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)val) = 127 - _MAX_MHR_OVERHEAD;
            return sizeof(uint16_t);
        case NETOPT_TX_POWER:
            if (max_len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)val) = _get_txpower(dev);
            return sizeof(uint16_t);
            break;
        case NETOPT_CSMA:
            if (max_len < sizeof(netopt_enable_t)) {
                return -EOVERFLOW;
            }
            *((netopt_enable_t *)val) = NETOPT_DISABLE;
            return sizeof(netopt_enable_t);
        case NETOPT_AUTOCCA:
            if (max_len < sizeof(netopt_enable_t)) {
                return -EOVERFLOW;
            }
            *((netopt_enable_t *)val) = NETOPT_DISABLE;
            return sizeof(netopt_enable_t);
        case NETOPT_RETRANS:
            if (max_len < sizeof(uint8_t)) {
                return -EOVERFLOW;
            }
            *((uint8_t *)val) = _get_max_retries(dev);
            return sizeof(uint8_t);
            break;
        default:
            break;
    }
    int res;
    if (((res = netdev_ieee802154_get((netdev_ieee802154_t *)netdev, opt, val,
                                       max_len)) >= 0) || (res != -ENOTSUP)) {
        return res;
    }
    return 0;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    return netdev_ieee802154_set((netdev_ieee802154_t *)netdev, opt,
                                  val, len);
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    netdev_raw802154_t *dev = (netdev_raw802154_t *)netdev;
    (void)info;
    int nread;

    if (!buf) {
        if (len > 0)
            native_async_continue_reading(dev->fd);
        else
            return 127;
    }

    nread = real_read(dev->fd, buf, len) - 2;
    if (nread > 0) {
        native_async_continue_reading(dev->fd);

#ifdef MODULE_NETSTATS_L2
        netdev->stats.rx_count++;
        netdev->stats.rx_bytes += nread;
#endif
    }

    return nread;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    netdev_raw802154_t *dev = (netdev_raw802154_t *)netdev;
    struct iovec iov[iolist_count(iolist)];
    size_t bytes;
    unsigned n;

    bytes = iolist_to_iovec(iolist, iov, &n);

    int res = _native_writev(dev->fd, iov, n);
#ifdef MODULE_NETSTATS_L2
    netdev->stats.tx_bytes += bytes;
#else
    (void)bytes;
#endif
    if (netdev->event_callback)
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);

    return res;
}

static int _init(netdev_t *netdev)
{
    netdev_raw802154_t *dev = (netdev_raw802154_t *)netdev;
    uint64_t extended_rand_addr;
    struct sockaddr_ll sll;
    struct ifreq ifr;
    int ret;

#ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0, sizeof(netstats_t));
#endif

    /* reset options and sequence number */
    dev->netdev.seq = 0;

    _set_page(dev, 0);
    _set_chan(dev, 11);

    if (dev->rand_addr) {
        hwrng_init();
        hwrng_read((uint8_t *)&extended_rand_addr, sizeof(extended_rand_addr));
        /* make sure we mark the address as non-multicast and not globally unique */
        ((uint8_t *)&extended_rand_addr)[0] &= ~(0x01);
        ((uint8_t *)&extended_rand_addr)[0] |= 0x02;
        _set_addr_long(dev, extended_rand_addr);
    } else {
        _set_addr_long(dev, RAW802154_DEFAULT_ADDR_LONG);
    }
    _set_addr_short(dev, RAW802154_DEFAULT_ADDR_SHORT);
    _set_pan(dev, RAW802154_DEFAULT_ADDR_PAN);

#ifdef MODULE_GNRC_SIXLOWPAN
    dev->netdev.proto = GNRC_NETTYPE_SIXLOWPAN;
#elif MODULE_GNRC
    dev->netdev.proto = GNRC_NETTYPE_UNDEF;
#endif

    dev->fd = real_socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IEEE802154));
    if (dev->fd == -1) {
        err(EXIT_FAILURE, "Couldn't create a raw 802154 socket\n");
    }

    strncpy(ifr.ifr_name, dev->ifname, IFNAMSIZ);
    ret = real_ioctl(dev->fd, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        real_close(dev->fd);
        err(EXIT_FAILURE, "Failed SIOCGIFINDEX on interface %s", dev->ifname);
    }

    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_IEEE802154);
    ret = real_bind(dev->fd, (struct sockaddr *)&sll, sizeof(sll));
    if (ret == -1) {
        real_close(dev->fd);
        err(EXIT_FAILURE, "Failed to bind interface");
    }

    /* configure signal handler for fds */
    native_async_read_add_handler(dev->fd, netdev, _raw802154_isr);

    return 0;
}

void netdev_raw802154_cleanup(netdev_raw802154_t *dev)
{
    /* Do we have a device */
    if (!dev) {
        return;
    }

    /* cleanup signal handling */
    native_async_read_cleanup();

    /* close the raw802154 device */
    real_shutdown(dev->fd, SHUT_RDWR);
    real_close(dev->fd);
}

static netdev_driver_t netdev_driver_raw802154 = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

void netdev_raw802154_setup(netdev_raw802154_t *dev, const char *ifname, int rand_addr) {
    dev->netdev.netdev.driver = &netdev_driver_raw802154;
    strncpy(dev->ifname, ifname, IFNAMSIZ);
    dev->rand_addr = rand_addr;
}
