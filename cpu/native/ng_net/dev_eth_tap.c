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
 * @ingroup net_dev_eth
 * @{
 * @brief   Low-level ethernet driver for tap interfaces
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @}
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

#include "native_internal.h"

#include "net/dev_eth.h"
#include "dev_eth_tap.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* support one tap interface for now */
dev_eth_tap_t dev_eth_tap;

/* dev_eth interface */
static int _init(dev_eth_t *ethdev);
static void _cleanup(dev_eth_t *ethdev);
static int _send(dev_eth_t *ethdev, char* buf, int n);
static int _recv(dev_eth_t *ethdev, char* buf, int n);

static inline void _get_mac_addr(dev_eth_t *ethdev, uint8_t *dst) {
    dev_eth_tap_t *dev = (dev_eth_tap_t*)ethdev;
    memcpy(dst, dev->addr, ETHERNET_ADDR_LEN);
}

static inline int _get_promiscous(dev_eth_t *ethdev) {
    dev_eth_tap_t *dev = (dev_eth_tap_t*)ethdev;
    return dev->promiscous;
}

static inline int _set_promiscous(dev_eth_t *ethdev, int value) {
    dev_eth_tap_t *dev = (dev_eth_tap_t*)ethdev;
    dev->promiscous = value;
    return value;
}

static inline void _isr(dev_eth_t *ethdev) {
    dev_eth_rx_handler(ethdev);
}

static eth_driver_t eth_driver_tap = {
    .init = _init,
    .cleanup = _cleanup,
    .send = _send,
    .recv = _recv,
    .get_mac_addr = _get_mac_addr,
    .get_promiscous = _get_promiscous,
    .set_promiscous = _set_promiscous,
    .isr = _isr,
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

static int _recv(dev_eth_t *dev_eth, char *buf, int len) {
    dev_eth_tap_t *dev = (dev_eth_tap_t*)dev_eth;

    int nread = real_read(dev->tap_fd, buf, len);
    DEBUG("ng_tapnet: read %d bytes\n", nread);

    if (nread > 0) {
        ethernet_hdr_t *hdr = (ethernet_hdr_t *)buf;
        if (!(dev->promiscous) && !_is_addr_multicast(hdr->dst) &&
            !_is_addr_broadcast(hdr->dst) &&
            (memcmp(hdr->dst, dev->addr, ETHERNET_ADDR_LEN) != 0)) {
            DEBUG("ng_eth_dev: received for %02x:%02x:%02x:%02x:%02x:%02x\n"
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
            DEBUG("dev_eth_tap: sigpend++\n");
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
            err(EXIT_FAILURE, "dev_eth_tap: read");
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

static int _send(dev_eth_t *ethdev, char* buf, int n) {
    dev_eth_tap_t *dev = (dev_eth_tap_t*)ethdev;
    return _native_write(dev->tap_fd, buf, n);
}

void dev_eth_tap_setup(dev_eth_tap_t *dev, const char *name) {
    dev->ethdev.driver = &eth_driver_tap;
    strncpy(dev->tap_name, name, IFNAMSIZ);
}

void dev_eth_tap_cleanup(dev_eth_tap_t *dev) {
    if (!dev) {
        return;
    }
    dev_eth_cleanup((dev_eth_t *) dev);
}

static void _tap_isr(void) {
    dev_eth_isr(((dev_eth_t *) &dev_eth_tap));
}

static int _init(dev_eth_t *ethdev)
{
    dev_eth_tap_t *dev = (dev_eth_tap_t*)ethdev;

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
    DEBUG("ng_tapnet_init(): dev->addr = %02x:%02x:%02x:%02x:%02x:%02x\n",
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

static void _cleanup(dev_eth_t *ethdev)
{
    dev_eth_tap_t *dev = (dev_eth_tap_t*)ethdev;

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
