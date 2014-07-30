/**
 * tap.h implementation
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup native_cpu
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <errno.h>

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

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "cpu.h"
#include "cpu-conf.h"
#include "tap.h"
#include "nativenet.h"
#include "nativenet_internal.h"
#include "native_internal.h"

#include "hwtimer.h"
#include "timex.h"

#ifdef MODULE_GTIMER
#include "gtimer.h"
#endif

#define TAP_BUFFER_LENGTH (ETHER_MAX_LEN)
int _native_marshall_ethernet(uint8_t *framebuf, radio_packet_t *packet);

int _native_tap_fd = -1;
unsigned char _native_tap_mac[ETHER_ADDR_LEN];

#ifdef __MACH__
pid_t sigio_child_pid;
#endif

void _native_handle_tap_input(void)
{
    int nread;
    union eth_frame frame;
    radio_packet_t p;

#ifdef MODULE_GTIMER
    gtimer_timeval_t gtimer_toa;
    gtimer_sync_now(&gtimer_toa);
#else
    unsigned long t = hwtimer_now();
#endif

    DEBUG("_native_handle_tap_input\n");

    /* TODO: check whether this is an input or an output event
       TODO: refactor this into general io-signal multiplexer */

    nread = real_read(_native_tap_fd, &frame, sizeof(union eth_frame));
    DEBUG("_native_handle_tap_input - read %d bytes\n", nread);
    if (nread > 0) {
        if (ntohs(frame.field.header.ether_type) == NATIVE_ETH_PROTO) {
            nread = nread - ETHER_HDR_LEN;
            if ((nread - 1) <= 0) {
                DEBUG("_native_handle_tap_input: no payload\n");
            }
            else {
                p.processing = 0;
                p.src = ntohs(frame.field.payload.nn_header.src);
                p.dst = ntohs(frame.field.payload.nn_header.dst);
                p.rssi = 0;
                p.lqi = 0;
#ifdef MODULE_GTIMER
                p.toa = gtimer_toa;
#else
                p.toa.seconds = HWTIMER_TICKS_TO_US(t)/1000000;
                p.toa.microseconds = HWTIMER_TICKS_TO_US(t)%1000000;
#endif
                /* XXX: check overflow */
                p.length = ntohs(frame.field.payload.nn_header.length);
                p.data = frame.field.payload.data;
                if (p.length > (nread - sizeof(struct nativenet_header))) {
                    warnx("_native_handle_tap_input: packet with malicious length field received, discarding");
                }
                else {
                    DEBUG("_native_handle_tap_input: received packet of length %" PRIu16 " for %" PRIu16 " from %" PRIu16 "\n", p.length, p.dst, p.src);
                    _nativenet_handle_packet(&p);
                }
            }
        }
        else {
            DEBUG("ignoring non-native frame\n");
        }

        /* work around lost signals */
        fd_set rfds;
        struct timeval t;
        memset(&t, 0, sizeof(t));
        FD_ZERO(&rfds);
        FD_SET(_native_tap_fd, &rfds);

        _native_in_syscall++; // no switching here
        if (select(_native_tap_fd +1, &rfds, NULL, NULL, &t) == 1) {
            int sig = SIGIO;
            extern int _sig_pipefd[2];
            extern ssize_t (*real_write)(int fd, const void *buf, size_t count);
            real_write(_sig_pipefd[1], &sig, sizeof(int));
            _native_sigpend++;
            DEBUG("_native_handle_tap_input: sigpend++\n");
        }
        else {
            DEBUG("_native_handle_tap_input: no more pending tap data\n");
#ifdef __MACH__
            kill(sigio_child_pid, SIGCONT);
#endif
        }
        _native_in_syscall--;
    }
    else if (nread == -1) {
        if ((errno == EAGAIN ) || (errno == EWOULDBLOCK)) {
            //warn("read");
        }
        else {
            err(EXIT_FAILURE, "_native_handle_tap_input: read");
        }
    }
    else {
        errx(EXIT_FAILURE, "internal error _native_handle_tap_input");
    }
}

#ifdef __MACH__
void sigio_child()
{
    pid_t parent = _native_pid;

    if ((sigio_child_pid = real_fork()) == -1) {
        err(EXIT_FAILURE, "sigio_child: fork");
    }

    if (sigio_child_pid > 0) {
        /* return in parent process */
        return;
    }

    /* watch tap interface and signal parent process if data is
     * available */

    fd_set rfds;
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(_native_tap_fd, &rfds);
        if (select(_native_tap_fd +1, &rfds, NULL, NULL, NULL) == 1) {
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

int _native_marshall_ethernet(uint8_t *framebuf, radio_packet_t *packet)
{
    int data_len;
    union eth_frame *f;
    unsigned char addr[ETHER_ADDR_LEN];

    f = (union eth_frame*)framebuf;
    addr[0] = addr[1] = addr[2] = addr[3] = addr[4] = addr[5] = 0xFF;

    memcpy(f->field.header.ether_dhost, addr, ETHER_ADDR_LEN);
    memcpy(f->field.header.ether_shost, _native_tap_mac, ETHER_ADDR_LEN);
    f->field.header.ether_type = htons(NATIVE_ETH_PROTO);

    /* XXX: check overflow */
    memcpy(f->field.payload.data, packet->data, packet->length);
    f->field.payload.nn_header.length = htons(packet->length);
    f->field.payload.nn_header.dst = htons(packet->dst);
    f->field.payload.nn_header.src = htons(packet->src);

    data_len = packet->length + sizeof(struct nativenet_header);

    /* Pad to minimum payload size.
     * Linux does this on its own, but it doesn't hurt to do it here.
     * As of now only tuntaposx needs this. */
    if (data_len < ETHERMIN) {
        DEBUG("padding data! (%d -> ", data_len);
        data_len = ETHERMIN;
        DEBUG("%d)\n", data_len);
    }

    return data_len + ETHER_HDR_LEN;
}

int8_t send_buf(radio_packet_t *packet)
{
    uint8_t buf[TAP_BUFFER_LENGTH];
    int nsent, to_send;

    memset(buf, 0, sizeof(buf));

    DEBUG("send_buf:  Sending packet of length %" PRIu16 " from %" PRIu16 " to %" PRIu16 "\n", packet->length, packet->src, packet->dst);
    to_send = _native_marshall_ethernet(buf, packet);

    DEBUG("send_buf: trying to send %d bytes\n", to_send);

    if ((nsent = _native_write(_native_tap_fd, buf, to_send)) == -1) {;
        warn("write");
        return -1;
    }
    return (nsent > INT8_MAX ? INT8_MAX : nsent);
}

int tap_init(char *name)
{

#ifdef __MACH__ /* OSX */
    char clonedev[255] = "/dev/"; /* XXX bad size */
    strncpy(clonedev+5, name, 250);
#elif defined(__FreeBSD__)
    char clonedev[255] = "/dev/"; /* XXX bad size */
    strncpy(clonedev+5, name, 250);
#else /* Linux */
    struct ifreq ifr;
    const char *clonedev = "/dev/net/tun";
#endif

    /* implicitly create the tap interface */
    if ((_native_tap_fd = open(clonedev , O_RDWR)) == -1) {
        err(EXIT_FAILURE, "open(%s)", clonedev);
    }

#if (defined(__MACH__) || defined(__FreeBSD__)) /* OSX/FreeBSD */
    struct ifaddrs* iflist;
    if (getifaddrs(&iflist) == 0) {
        for (struct ifaddrs *cur = iflist; cur; cur = cur->ifa_next) {
            if ((cur->ifa_addr->sa_family == AF_LINK) && (strcmp(cur->ifa_name, name) == 0) && cur->ifa_addr) {
                struct sockaddr_dl* sdl = (struct sockaddr_dl*)cur->ifa_addr;
                memcpy(_native_tap_mac, LLADDR(sdl), sdl->sdl_alen);
                break;
            }
        }

        freeifaddrs(iflist);
    }
#else /* Linux */
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strncpy(ifr.ifr_name, name, IFNAMSIZ);

    if (ioctl(_native_tap_fd, TUNSETIFF, (void *)&ifr) == -1) {
        _native_in_syscall++;
        warn("ioctl TUNSETIFF");
        warnx("probably the tap interface (%s) does not exist or is already in use", name);
        exit(EXIT_FAILURE);
    }

    /* TODO: use strncpy */
    strcpy(name, ifr.ifr_name);


    /* get MAC address */
    memset (&ifr, 0, sizeof (ifr));
    snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", name);
    if (ioctl(_native_tap_fd, SIOCGIFHWADDR, &ifr) == -1) {
        _native_in_syscall++;
        warn("ioctl SIOCGIFHWADDR");
        if (real_close(_native_tap_fd) == -1) {
            warn("close");
        }
        exit(EXIT_FAILURE);
    }
    memcpy(_native_tap_mac, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN);
#endif
    DEBUG("_native_tap_mac: %02x:%02x:%02x:%02x:%02x:%02x\n", _native_tap_mac[0], _native_tap_mac[1], _native_tap_mac[2], _native_tap_mac[3], _native_tap_mac[4], _native_tap_mac[5]);

    unsigned char *eui_64 = (unsigned char*)&_native_net_addr_long;
    eui_64[0] = _native_tap_mac[0];
    eui_64[1] = _native_tap_mac[1];
    eui_64[2] = _native_tap_mac[2];
    eui_64[3] = 0xff;
    eui_64[4] = 0xfe;
    eui_64[5] = _native_tap_mac[3];
    eui_64[6] = _native_tap_mac[4];
    eui_64[7] = _native_tap_mac[5];

    /* configure signal handler for fds */
    register_interrupt(SIGIO, _native_handle_tap_input);

#ifdef __MACH__
    /* tuntap signalled IO is not working in OSX,
     * check http://sourceforge.net/p/tuntaposx/bugs/17/ */
    sigio_child();
#else
    /* configure fds to send signals on io */
    if (fcntl(_native_tap_fd, F_SETOWN, _native_pid) == -1) {
        err(EXIT_FAILURE, "tap_init(): fcntl(F_SETOWN)");
    }

    /* set file access mode to nonblocking */
    if (fcntl(_native_tap_fd, F_SETFL, O_NONBLOCK|O_ASYNC) == -1) {
        err(EXIT_FAILURE, "tap_init(): fcntl(F_SETFL)");
    }
#endif /* not OSX */

    DEBUG("RIOT native tap initialized.\n");
    return _native_tap_fd;
}
/** @} */
