/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     cpu_native_ng_tapnet
 * @{
 *
 * @file
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

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

#ifdef __MACH__
#define _POSIX_C_SOURCE
#include <net/if.h>
#undef _POSIX_C_SOURCE
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
#endif

#include "byteorder.h"
#include "native_internal.h"
#include "net/ng_ethernet.h"
#include "net/ng_ethertype.h"
#include "net/ng_netdev.h"
#include "net/ng_netif/hdr.h"
#include "net/ng_pkt.h"
#include "net/ng_pktbuf.h"
#include "od.h"
#include "thread.h"
#include "utlist.h"

#include "ng_tapnet.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _ISR_EVENT_RX   (1U)

ng_tapnet_t ng_tapnet;

static uint8_t send_buffer[NG_ETHERNET_MAX_LEN];
static uint8_t recv_buffer[NG_ETHERNET_MAX_LEN];

/* MAC OSX specific variables and functions */
#ifdef __MACH__
pid_t _sigio_child_pid;

static void _sigio_child(ng_tapnet_t *dev);
#endif

/* driver function definitions */
static int _send_data(ng_netdev_t *netdev, ng_pktsnip_t *pkt);
static int _add_event_callback(ng_netdev_t *dev, ng_netdev_event_cb_t cb);
static int _rem_event_callback(ng_netdev_t *dev, ng_netdev_event_cb_t cb);
static int _get(ng_netdev_t *dev, ng_netconf_opt_t opt, void *value,
                size_t max_len);
static int _set(ng_netdev_t *dev, ng_netconf_opt_t opt, void *value,
                size_t value_len);
static void _isr_event(ng_netdev_t *dev, uint32_t event_type);

/* netdev driver struct */
const ng_netdev_driver_t ng_tapnet_driver = {
    _send_data,
    _add_event_callback,
    _rem_event_callback,
    _get,
    _set,
    _isr_event,
};

/* internal function definitions */

static inline bool _has_src(ng_pktsnip_t *pkt)
{
    return ((pkt->type == NG_NETTYPE_NETIF) &&
            ((ng_netif_hdr_t *)pkt->data)->src_l2addr_len == NG_ETHERNET_ADDR_LEN);
}

static inline bool _for_ethernet(ng_pktsnip_t *pkt)
{
    return (((pkt->type == NG_NETTYPE_NETIF) &&
             (((ng_netif_hdr_t *)pkt->data)->dst_l2addr_len == NG_ETHERNET_ADDR_LEN)) ||
            _has_src(pkt));
}

static inline bool _addr_broadcast(uint8_t *addr)
{
    return ((addr[0] == 0xff) && (addr[1] == 0xff) && (addr[2] == 0xff) &&
            (addr[3] == 0xff) && (addr[4] == 0xff) && (addr[5] == 0xff));
}

/* build Ethernet packet from pkt */
static int _marshall_ethernet(ng_tapnet_t *dev, uint8_t *buffer, ng_pktsnip_t *pkt);

/* build ISR handler for ISR events */
void _trigger_isr_event(void);

/* driver implementation */
int ng_tapnet_init(ng_tapnet_t *dev, char *name, size_t name_len)
{
#ifdef __MACH__ /* OSX */
    char clonedev[255] = "/dev/"; /* XXX bad size */
    strncpy(clonedev + 5, name, 250);
#elif defined(__FreeBSD__)
    char clonedev[255] = "/dev/"; /* XXX bad size */
    strncpy(clonedev + 5, name, 250);
#else /* Linux */
    struct ifreq ifr;
    const char *clonedev = "/dev/net/tun";
#endif

    /* check device parametrs */
    if ((dev == NULL) || (dev != &ng_tapnet)) {
        return -ENODEV;
    }

    /* initialize device descriptor */
    dev->driver = (ng_netdev_driver_t *)(&ng_tapnet_driver);
    dev->promiscous = 0;

    /* implicitly create the tap interface */
    if ((dev->tap_fd = real_open(clonedev , O_RDWR)) == -1) {
        err(EXIT_FAILURE, "open(%s)", clonedev);
    }

#if (defined(__MACH__) || defined(__FreeBSD__)) /* OSX/FreeBSD */
    struct ifaddrs *iflist;

    if (real_getifaddrs(&iflist) == 0) {
        for (struct ifaddrs *cur = iflist; cur; cur = cur->ifa_next) {
            if ((cur->ifa_addr->sa_family == AF_LINK) && (strcmp(cur->ifa_name, name) == 0) && cur->ifa_addr) {
                struct sockaddr_dl *sdl = (struct sockaddr_dl *)cur->ifa_addr;
                memcpy(dev->addr, LLADDR(sdl), sdl->sdl_alen);
                break;
            }
        }

        real_freeifaddrs(iflist);
    }

#else /* Linux */
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strncpy(ifr.ifr_name, name, IFNAMSIZ);

    if (real_ioctl(dev->tap_fd, TUNSETIFF, (void *)&ifr) == -1) {
        _native_in_syscall++;
        warn("ioctl TUNSETIFF");
        warnx("probably the tap interface (%s) does not exist or is already in use", name);
        real_exit(EXIT_FAILURE);
    }

    strncpy(name, ifr.ifr_name, name_len);


    /* get MAC address */
    memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", name);

    if (real_ioctl(dev->tap_fd, SIOCGIFHWADDR, &ifr) == -1) {
        _native_in_syscall++;
        warn("ioctl SIOCGIFHWADDR");

        if (real_close(dev->tap_fd) == -1) {
            warn("close");
        }

        real_exit(EXIT_FAILURE);
    }

    memcpy(dev->addr, ifr.ifr_hwaddr.sa_data, NG_ETHERNET_ADDR_LEN);
#endif
    DEBUG("ng_tapnet_init(): dev->addr = %02x:%02x:%02x:%02x:%02x:%02x\n",
          dev->addr[0], dev->addr[1], dev->addr[2],
          dev->addr[3], dev->addr[4], dev->addr[5]);

    /* configure signal handler for fds */
    register_interrupt(SIGIO, _trigger_isr_event);

#ifdef __MACH__
    /* tuntap signalled IO is not working in OSX,
     * check http://sourceforge.net/p/tuntaposx/bugs/17/ */
    _sigio_child(dev);
#else

    /* configure fds to send signals on io */
    if (fcntl(dev->tap_fd, F_SETOWN, _native_pid) == -1) {
        err(EXIT_FAILURE, "ng_tapnet_init(): fcntl(F_SETOWN)");
    }

    /* set file access mode to non-blocking */
    if (fcntl(dev->tap_fd, F_SETFL, O_NONBLOCK | O_ASYNC) == -1) {
        err(EXIT_FAILURE, "ng_tabnet_init(): fcntl(F_SETFL)");
    }

#endif /* not OSX */

    DEBUG("ng_tapnet: initialized.\n");

    return 0;
}

static int _send_data(ng_netdev_t *netdev, ng_pktsnip_t *pkt)
{
    int nsent, to_send;
    ng_tapnet_t *dev = (ng_tapnet_t *)netdev;

    DEBUG("ng_tapnet: send data ");

    if (pkt == NULL) {
        return -EFAULT;
    }

    if ((dev == NULL) || (dev->driver != &ng_tapnet_driver)) {
        DEBUG("[wrong device descriptor]\n");
        ng_pktbuf_release(pkt);
        return -ENODEV;
    }

    DEBUG("\n");

    to_send = _marshall_ethernet(dev, send_buffer, pkt);
    ng_pktbuf_release(pkt);

    if (to_send < 0) {
        errno = -to_send;
        warn("marshall");
        return to_send;
    }

    DEBUG("ng_tapnet: send %d bytes\n", to_send);
#if MODULE_OD && defined(ENABLE_DEBUG)
    od_hex_dump(send_buffer, to_send, OD_WIDTH_DEFAULT);
#endif

    if ((nsent = _native_write(dev->tap_fd, send_buffer, to_send)) < 0) {
        warn("write");
        return -EIO;
    }

    return nsent;
}

static int _add_event_callback(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
{
    DEBUG("ng_tapnet: add event callback");

    if ((dev == NULL) || (dev->driver != &ng_tapnet_driver)) {
        DEBUG(" [wrong device descriptor]\n");
        return -ENODEV;
    }

    if (dev->event_cb != NULL) {
        DEBUG(" [no space left]\n");
        return -ENOBUFS;
    }

    DEBUG("\n");
    dev->event_cb = cb;

    return 0;
}

static int _rem_event_callback(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
{
    DEBUG("ng_tapnet: remove event callback");

    if ((dev == NULL) || (dev->driver != &ng_tapnet_driver)) {
        DEBUG(" [wrong device descriptor]\n");
        return -ENODEV;
    }

    if (dev->event_cb != cb) {
        DEBUG(" [not found]\n");
        return -ENOENT;
    }

    DEBUG("\n");
    dev->event_cb = NULL;

    return 0;
}

/* individual option getters to be called by _get() */
static inline int _get_addr(ng_tapnet_t *dev, uint8_t *value, size_t max_len)
{
    if (max_len < NG_ETHERNET_ADDR_LEN) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    memcpy(value, dev->addr, NG_ETHERNET_ADDR_LEN);

    return NG_ETHERNET_ADDR_LEN;
}

static inline int _get_addr_len(uint16_t *value, size_t max_len)
{
    if (max_len != sizeof(uint16_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    *value = NG_ETHERNET_ADDR_LEN;

    return sizeof(uint16_t);
}

static inline int _get_max_pkt_sz(uint16_t *value, size_t max_len)
{
    if (max_len != sizeof(uint16_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    *value = NG_ETHERNET_MAX_LEN;

    return sizeof(uint16_t);
}

static inline int _get_promiscousmode(ng_tapnet_t *dev, ng_netconf_enable_t *value,
                                      size_t max_len)
{
    if (max_len != sizeof(ng_netconf_enable_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    *value = (ng_netconf_enable_t)dev->promiscous;

    return sizeof(ng_netconf_enable_t);
}

static int _get(ng_netdev_t *dev, ng_netconf_opt_t opt, void *value,
                size_t max_len)
{
    DEBUG("ng_tapnet: get ");

    if ((dev == NULL) || (dev->driver != &ng_tapnet_driver)) {
        DEBUG("[wrong device descriptor]\n");
        return -ENODEV;
    }

    switch (opt) {
        case NETCONF_OPT_ADDRESS:
            DEBUG("address\n");
            return _get_addr((ng_tapnet_t *)dev, value, max_len);

        case NETCONF_OPT_ADDR_LEN:
            DEBUG("address length\n");
            return _get_addr_len(value, max_len);

        case NETCONF_OPT_MAX_PACKET_SIZE:
            DEBUG("maximum packet size\n");
            return _get_max_pkt_sz(value, max_len);

        case NETCONF_OPT_PROMISCUOUSMODE:
            DEBUG("promiscous mode\n");
            return _get_promiscousmode((ng_tapnet_t *)dev, value, max_len);

        default:
            DEBUG("[not supported: %d]\n", opt);
            return -ENOTSUP;
    }
}

/* individual option getters to be called by _get() */
static inline int _set_promiscousmode(ng_tapnet_t *dev, ng_netconf_enable_t *value,
                                      size_t value_len)
{
    if (value_len != sizeof(ng_netconf_enable_t)) {
        /* value buffer not big enough */
        return -EOVERFLOW;
    }

    dev->promiscous = (uint8_t)(*value);

    return sizeof(ng_netconf_enable_t);
}

static int _set(ng_netdev_t *dev, ng_netconf_opt_t opt, void *value,
                size_t value_len)
{
    DEBUG("ng_tapnet: set ");

    if ((dev == NULL) || (dev->driver != &ng_tapnet_driver)) {
        DEBUG("[wrong device descriptor]\n");
        return -ENODEV;
    }

    switch (opt) {
        case NETCONF_OPT_PROMISCUOUSMODE:
            DEBUG("promiscous mode\n");
            return _set_promiscousmode((ng_tapnet_t *)dev, value, value_len);

        default:
            DEBUG("[not supported: %d]\n", opt);
            return -ENOTSUP;
    }
}

/* individual event handlers called by _isr_event() */
static void _rx_event(ng_tapnet_t *dev);

static void _isr_event(ng_netdev_t *dev, uint32_t event_type)
{
    DEBUG("ng_tapnet: ISR event ");

    if ((dev == NULL) || (dev->driver != &ng_tapnet_driver)) {
        return;
    }

    switch (event_type) {
        case _ISR_EVENT_RX:
            DEBUG("[RX]\n");
            _rx_event((ng_tapnet_t *)dev);
            break;

        default:
            DEBUG("[unknown event_type]\n");
            break;
    }
}

/* internal implementations */
#ifdef __MACH__
static void _sigio_child(ng_tapnet_t *dev)
{
    pid_t parent = _native_pid;

    if ((_sigio_child_pid = real_fork()) == -1) {
        err(EXIT_FAILURE, "sigio_child: fork");
    }

    if (_sigio_child_pid > 0) {
        /* return in parent process */
        return;
    }

    /* watch tap interface and signal parent process if data is
     * available */

    fd_set rfds;

    while (1) {
        FD_ZERO(&rfds);
        FD_SET(dev->tap_fd, &rfds);

        if (real_select(dev->tap_fd + 1, &rfds, NULL, NULL, NULL) == 1) {
            kill(parent, SIGIO);
        }
        else {
            kill(parent, SIGKILL);
            err(EXIT_FAILURE, "osx_sigio_child: select");
        }

        pause();
    }
}
#endif

static int _marshall_ethernet(ng_tapnet_t *dev, uint8_t *buffer, ng_pktsnip_t *pkt)
{
    int data_len = 0;
    ng_ethernet_hdr_t *hdr = (ng_ethernet_hdr_t *)buffer;
    ng_netif_hdr_t *netif_hdr;
    ng_pktsnip_t *payload;

    if (pkt == NULL) {
        DEBUG("ng_tapnet: pkt was NULL");
        return -EINVAL;
    }

    payload = pkt->next;

    if (!_for_ethernet(pkt)) {
        DEBUG("ng_tapnet: First header was not generic netif header for Ethernet\n");
        return -EBADMSG;
    }

    hdr->type = byteorder_htons(ng_nettype_to_ethertype(pkt->next->type));

    netif_hdr = pkt->data;

    /* set ethernet header */
    if (_has_src(pkt)) {
        memcpy(hdr->dst, ng_netif_hdr_get_src_addr(netif_hdr),
               netif_hdr->src_l2addr_len);
    }
    else {
        memcpy(hdr->src, dev->addr, NG_ETHERNET_ADDR_LEN);
    }

    memcpy(hdr->dst, ng_netif_hdr_get_dst_addr(netif_hdr),
           netif_hdr->dst_l2addr_len);
    DEBUG("ng_tapnet: send to %02x:%02x:%02x:%02x:%02x:%02x\n",
          hdr->dst[0], hdr->dst[1], hdr->dst[2],
          hdr->dst[3], hdr->dst[4], hdr->dst[5]);

    data_len += sizeof(ng_ethernet_hdr_t);

    while (payload != NULL) {
        if ((data_len + payload->size) > NG_ETHERNET_MAX_LEN) {
            DEBUG("ng_tapnet: Packet too big for ethernet frame\n");
            return -ENOBUFS;
        }

        memcpy(send_buffer + data_len, payload->data, payload->size);

        data_len += payload->size;
        payload = payload->next;
    }

    /* Pad to minimum payload size.
     * Linux does this on its own, but it doesn't hurt to do it here.
     * As of now only tuntaposx needs this. */
    if (data_len < (NG_ETHERNET_MIN_LEN)) {
        DEBUG("ng_tapnet: padding data! (%d -> ", data_len);
        memset(send_buffer + data_len, 0, NG_ETHERNET_MIN_LEN - data_len);
        data_len = NG_ETHERNET_MIN_LEN;
        DEBUG("%d)\n", data_len);
    }

    return data_len;
}

void _trigger_isr_event(void)
{
    msg_t msg;

    DEBUG("ng_tapnet: Trigger ISR event\n");

    /* TODO: check whether this is an input or an output event
       TODO: refactor this into general io-signal multiplexer */

    msg.type = NG_NETDEV_MSG_TYPE_EVENT;
    msg.content.value = _ISR_EVENT_RX;

    msg_send(&msg, ng_tapnet.mac_pid);
}

static void _rx_event(ng_tapnet_t *dev)
{
    int nread = real_read(dev->tap_fd, recv_buffer, NG_ETHERNET_MAX_LEN);

    DEBUG("ng_tapnet: read %d bytes\n", nread);

    if (nread > 0) {
        ng_ethernet_hdr_t *hdr = (ng_ethernet_hdr_t *)recv_buffer;
        ng_pktsnip_t *netif_hdr, *pkt;
        ng_nettype_t receive_type = NG_NETTYPE_UNDEF;
        size_t data_len = (nread - sizeof(ng_ethernet_hdr_t));

        if (!(dev->promiscous) && (memcmp(hdr->dst, dev->addr, NG_ETHERNET_ADDR_LEN) != 0) &&
            (!_addr_broadcast(hdr->dst))) {
            /* TODO: check multicast */
            DEBUG("ng_tapnet: received for %02x:%02x:%02x:%02x:%02x:%02x\n"
                  "That's not me => Dropped\n",
                  hdr->dst[0], hdr->dst[1], hdr->dst[2],
                  hdr->dst[3], hdr->dst[4], hdr->dst[5]);
            return;
        }

        netif_hdr = ng_pktbuf_add(NULL, NULL, sizeof(ng_netif_hdr_t) + (2 * NG_ETHERNET_ADDR_LEN),
                                  NG_NETTYPE_NETIF);

        if (netif_hdr == NULL) {
            DEBUG("ng_tapnet: no space left in packet buffer\n");
            return;
        }

        ng_netif_hdr_init(netif_hdr->data, NG_ETHERNET_ADDR_LEN, NG_ETHERNET_ADDR_LEN);
        ng_netif_hdr_set_src_addr(netif_hdr->data, hdr->src, NG_ETHERNET_ADDR_LEN);
        ng_netif_hdr_set_dst_addr(netif_hdr->data, hdr->dst, NG_ETHERNET_ADDR_LEN);
        ((ng_netif_hdr_t *)netif_hdr->data)->if_pid = thread_getpid();

        receive_type = ng_nettype_from_ethertype(byteorder_ntohs(hdr->type));

        DEBUG("ng_tapnet: received packet from %02x:%02x:%02x:%02x:%02x:%02x "
              "of length %zu\n",
              hdr->src[0], hdr->src[1], hdr->src[2], hdr->src[3], hdr->src[4],
              hdr->src[5], data_len);
#if defined(MODULE_OD) && ENABLE_DEBUG
        od_hex_dump(hdr, nread, OD_WIDTH_DEFAULT);
#endif

        /* Mark netif header and payload for next layer */
        if ((pkt = ng_pktbuf_add(netif_hdr, recv_buffer + sizeof(ng_ethernet_hdr_t),
                                 data_len, receive_type)) == NULL) {
            ng_pktbuf_release(netif_hdr);
            DEBUG("ng_tapnet: no space left in packet buffer\n");
            return;
        }

        if (dev->event_cb != NULL) {
            dev->event_cb(NETDEV_EVENT_RX_COMPLETE, pkt);
        }
        else {
            ng_pktbuf_release(pkt); /* netif_hdr is released automatically too */
        }

        /* work around lost signals */
        fd_set rfds;
        struct timeval t;
        memset(&t, 0, sizeof(t));
        FD_ZERO(&rfds);
        FD_SET(dev->tap_fd, &rfds);

        _native_in_syscall++; // no switching here

        if (real_select(dev->tap_fd + 1, &rfds, NULL, NULL, &t) == 1) {
            int sig = SIGIO;
            extern int _sig_pipefd[2];
            extern ssize_t (*real_write)(int fd, const void * buf, size_t count);
            real_write(_sig_pipefd[1], &sig, sizeof(int));
            _native_sigpend++;
            DEBUG("ng_tapnet: sigpend++\n");
        }
        else {
            DEBUG("ng_tapnet: no more pending tap data\n");
#ifdef __MACH__
            kill(_sigio_child_pid, SIGCONT);
#endif
        }

        _native_in_syscall--;
    }
    else if (nread == -1) {
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            //warn("read");
        }
        else {
            err(EXIT_FAILURE, "ng_tapnet: read");
        }
    }
    else {
        errx(EXIT_FAILURE, "internal error _rx_event");
    }
}

/**
 * @}
 */
