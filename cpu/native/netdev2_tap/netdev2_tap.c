/*
 * Copyright (C) 2015 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>,
 *                    Martine Lenders <mlenders@inf.fu-berlin.de>
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *                    Ell-i open source co-operative
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/*
 * @ingroup netdev2
 * @{
 * @brief   Low-level ethernet driver for tap interfaces
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @}
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
#endif

#include "native_internal.h"

#include "net/eui64.h"
#include "net/netdev2.h"
#include "net/ethernet.h"
#include "net/ethernet/hdr.h"
#include "netdev2_tap.h"
#include "net/netopt.h"
#include "net/eui64.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* support one tap interface for now */
netdev2_tap_t netdev2_tap;

#ifdef __MACH__
pid_t _sigio_child_pid;
static void _sigio_child(netdev2_tap_t *dev);
#endif

/* netdev2 interface */
static int _init(netdev2_t *netdev);
static int _send(netdev2_t *netdev, const struct iovec *vector, int n);
static int _recv(netdev2_t *netdev, char* buf, int n);

static inline void _get_mac_addr(netdev2_t *netdev, uint8_t *dst)
{
    netdev2_tap_t *dev = (netdev2_tap_t*)netdev;
    memcpy(dst, dev->addr, ETHERNET_ADDR_LEN);
}

static inline void _set_mac_addr(netdev2_t *netdev, uint8_t *src)
{
    netdev2_tap_t *dev = (netdev2_tap_t*)netdev;
    memcpy(dev->addr, src, ETHERNET_ADDR_LEN);
}

static inline int _get_promiscous(netdev2_t *netdev)
{
    netdev2_tap_t *dev = (netdev2_tap_t*)netdev;
    return dev->promiscous;
}

static inline int _set_promiscous(netdev2_t *netdev, int value)
{
    netdev2_tap_t *dev = (netdev2_tap_t*)netdev;
    dev->promiscous = value;
    return value;
}

static inline int _get_iid(netdev2_t *netdev, eui64_t *value, size_t max_len)
{
    if (max_len < sizeof(eui64_t)) {
        return -EOVERFLOW;
    }

    uint8_t addr[ETHERNET_ADDR_LEN];
    _get_mac_addr(netdev, addr);
    ethernet_get_iid(value, addr);

    return sizeof(eui64_t);
}
static inline void _isr(netdev2_t *netdev)
{
    if (netdev->event_callback) {
        netdev->event_callback(netdev, NETDEV2_EVENT_RX_COMPLETE, (void*)NETDEV2_TYPE_ETHERNET);
    }
#if DEVELHELP
    else {
        puts("netdev2_tap: _isr(): no event_callback set.");
    }
#endif
}

int _get(netdev2_t *dev, netopt_t opt, void *value, size_t max_len)
{
    if (dev != (netdev2_t *)&netdev2_tap) {
        return -ENODEV;
    }

    int res = 0;

    switch (opt) {
        case NETOPT_DEVICE_TYPE:
            {
               uint16_t *tgt = (uint16_t *)value;
                *tgt = NETDEV2_TYPE_ETHERNET;
                res = 2;
                break;
            }
        case NETOPT_ADDRESS:
            if (max_len < ETHERNET_ADDR_LEN) {
                res = -EINVAL;
            }
            else {
                _get_mac_addr(dev, (uint8_t*)value);
                res = ETHERNET_ADDR_LEN;
            }
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            assert(max_len == 2);
            uint16_t *tgt = (uint16_t*)value;
            *tgt=6;
            res = sizeof(uint16_t);
            break;
        case NETOPT_PROMISCUOUSMODE:
            *((bool*)value) = (bool)_get_promiscous(dev);
            res = sizeof(bool);
            break;
        case NETOPT_IPV6_IID:
            return _get_iid(dev, value, max_len);
        case NETOPT_IS_WIRED:
            res = 1;
            break;
        default:
            res = -ENOTSUP;
            break;
    }

    return res;
}

int _set(netdev2_t *dev, netopt_t opt, void *value, size_t value_len)
{
    (void)value_len;

    if (dev != (netdev2_t *)&netdev2_tap) {
        return -ENODEV;
    }

    int res = 0;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(value_len==ETHERNET_ADDR_LEN);
            _set_mac_addr(dev, (uint8_t*)value);
            break;
        case NETOPT_PROMISCUOUSMODE:
            _set_promiscous(dev, ((bool *)value)[0]);
            break;
        default:
            return -ENOTSUP;
    }

    return res;
}

static netdev2_driver_t netdev2_driver_tap = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

/* driver implementation */
static inline bool _is_addr_broadcast(uint8_t *addr)
{
    return ((addr[0] == 0xff) && (addr[1] == 0xff) && (addr[2] == 0xff) &&
            (addr[3] == 0xff) && (addr[4] == 0xff) && (addr[5] == 0xff));
}

static inline bool _is_addr_multicast(uint8_t *addr)
{
    /* source: http://ieee802.org/secmail/pdfocSP2xXA6d.pdf */
    return (addr[0] & 0x01);
}

static int _recv(netdev2_t *netdev2, char *buf, int len)
{
    netdev2_tap_t *dev = (netdev2_tap_t*)netdev2;

    if (!buf) {
        /* no way of figuring out packet size without racey buffering,
         * so we return the maximum possible size */
        return 576;
    }

    int nread = real_read(dev->tap_fd, buf, len);
    DEBUG("netdev2_tap: read %d bytes\n", nread);

    if (nread > 0) {
        ethernet_hdr_t *hdr = (ethernet_hdr_t *)buf;
        if (!(dev->promiscous) && !_is_addr_multicast(hdr->dst) &&
            !_is_addr_broadcast(hdr->dst) &&
            (memcmp(hdr->dst, dev->addr, ETHERNET_ADDR_LEN) != 0)) {
            DEBUG("netdev2_tap: received for %02x:%02x:%02x:%02x:%02x:%02x\n"
                  "That's not me => Dropped\n",
                  hdr->dst[0], hdr->dst[1], hdr->dst[2],
                  hdr->dst[3], hdr->dst[4], hdr->dst[5]);
#ifdef __MACH__
            kill(_sigio_child_pid, SIGCONT);
#endif
            return 0;
        }
        /* work around lost signals */
        fd_set rfds;
        struct timeval t;
        memset(&t, 0, sizeof(t));
        FD_ZERO(&rfds);
        FD_SET(dev->tap_fd, &rfds);

        _native_in_syscall++; /* no switching here */

        if (real_select(dev->tap_fd + 1, &rfds, NULL, NULL, &t) == 1) {
            int sig = SIGIO;
            extern int _sig_pipefd[2];
            extern ssize_t (*real_write)(int fd, const void * buf, size_t count);
            real_write(_sig_pipefd[1], &sig, sizeof(int));
            _native_sigpend++;
            DEBUG("netdev2_tap: sigpend++\n");
        }
        else {
#ifdef __MACH__
        kill(_sigio_child_pid, SIGCONT);
#endif
        }

        _native_in_syscall--;

        return nread;
    }
    else if (nread == -1) {
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
        }
        else {
            err(EXIT_FAILURE, "netdev2_tap: read");
        }
    }
    else if (nread == 0) {
        DEBUG("_native_handle_tap_input: ignoring null-event");
    }
    else {
        errx(EXIT_FAILURE, "internal error _rx_event");
    }

    return -1;
}

static int _send(netdev2_t *netdev, const struct iovec *vector, int n)
{
    netdev2_tap_t *dev = (netdev2_tap_t*)netdev;
    return _native_writev(dev->tap_fd, vector, n);
}

void netdev2_tap_setup(netdev2_tap_t *dev, const char *name) {
    dev->netdev.driver = &netdev2_driver_tap;
    strncpy(dev->tap_name, name, IFNAMSIZ);
}

static void _tap_isr(void) {
    netdev2_t *netdev = (netdev2_t *)&netdev2_tap;

    if (netdev->event_callback) {
        netdev->event_callback(netdev, NETDEV2_EVENT_ISR, netdev->isr_arg);
    }
    else {
        puts("netdev2_tap: _isr: no event callback.");
    }
}

static int _init(netdev2_t *netdev)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

    netdev2_tap_t *dev = (netdev2_tap_t*)netdev;

    /* check device parametrs */
    if (dev == NULL) {
        return -ENODEV;
    }

    char *name = dev->tap_name;
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
    /* initialize device descriptor */
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
    memcpy(dev->addr, ifr.ifr_hwaddr.sa_data, ETHERNET_ADDR_LEN);

    /* change mac addr so it differs from what the host is using */
    dev->addr[5]++;
#endif
    DEBUG("gnrc_tapnet_init(): dev->addr = %02x:%02x:%02x:%02x:%02x:%02x\n",
            dev->addr[0], dev->addr[1], dev->addr[2],
            dev->addr[3], dev->addr[4], dev->addr[5]);
    /* configure signal handler for fds */
    register_interrupt(SIGIO, _tap_isr);
#ifdef __MACH__
    /* tuntap signalled IO is not working in OSX,
     * * check http://sourceforge.net/p/tuntaposx/bugs/17/ */
    _sigio_child(dev);
#else
    /* configure fds to send signals on io */
    if (fcntl(dev->tap_fd, F_SETOWN, _native_pid) == -1) {
        err(EXIT_FAILURE, "gnrc_tapnet_init(): fcntl(F_SETOWN)");
    }
    /* set file access mode to non-blocking */
    if (fcntl(dev->tap_fd, F_SETFL, O_NONBLOCK | O_ASYNC) == -1) {
        err(EXIT_FAILURE, "gnrc_tabnet_init(): fcntl(F_SETFL)");
    }
#endif /* not OSX */
    DEBUG("gnrc_tapnet: initialized.\n");
    return 0;
}

void netdev2_tap_cleanup(netdev2_tap_t *dev)
{
    /* Do we have a device */
    if (!dev) {
        return;
    }

    /* cleanup signal handling */
    unregister_interrupt(SIGIO);
#ifdef __MACH__
    kill(_sigio_child_pid, SIGKILL);
#endif

    /* close the tap device */
    real_close(dev->tap_fd);
}

#ifdef __MACH__
static void _sigio_child(netdev2_tap_t *dev)
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
