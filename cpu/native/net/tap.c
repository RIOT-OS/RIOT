/**
 * tap.h implementation
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU Lesser General
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

#ifdef __MACH__
#define _POSIX_C_SOURCE
#include <net/if.h>
#undef _POSIX_C_SOURCE
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

#define TAP_BUFFER_LENGTH (ETHER_MAX_LEN)
int _native_marshall_ethernet(uint8_t *framebuf, radio_packet_t *packet);

int _native_tap_fd;
unsigned char _native_tap_mac[ETHER_ADDR_LEN];

void _native_handle_tap_input(void)
{
    int nread;
    union eth_frame frame;
    radio_packet_t p;

    DEBUG("_native_handle_tap_input\n");

    /* TODO: check whether this is an input or an output event
       TODO: refactor this into general io-signal multiplexer */

    _native_in_syscall = 1;
    nread = read(_native_tap_fd, &frame, sizeof(union eth_frame));
    _native_in_syscall = 0;
    DEBUG("_native_handle_tap_input - read %d bytes\n", nread);
    if (nread > 0) {
        if (ntohs(frame.field.header.ether_type) == NATIVE_ETH_PROTO) {
            nread = nread - ETHER_HDR_LEN;
            if ((nread - 1) <= 0) {
                DEBUG("_native_handle_tap_input: no payload");
            }
            else {
                /* XXX: check overflow */
                p.length = ntohs(frame.field.payload.nn_header.length);
                p.dst = ntohs(frame.field.payload.nn_header.dst);
                p.src = ntohs(frame.field.payload.nn_header.src);
                p.rssi = 0;
                p.lqi = 0;
                p.data = frame.field.payload.data;
                DEBUG("_native_handle_tap_input: received packet of length %"PRIu16" for %"PRIu16" from %"PRIu16": %s\n", p.length, p.dst, p.src, (char*) p.data);
                _nativenet_handle_packet(&p);
            }
        }
        else {
            DEBUG("ignoring non-native frame\n");
        }
    }
    else if (nread == -1) {
        err(EXIT_FAILURE, "read");
    }
    else {
        errx(EXIT_FAILURE, "internal error _native_handle_tap_input");
    }
}

int _native_marshall_ethernet(uint8_t *framebuf, radio_packet_t *packet)
{
    int data_len;
    union eth_frame *f;
    unsigned char addr[ETHER_ADDR_LEN];

    f = (union eth_frame*)framebuf;
    addr[0] = addr[1] = addr[2] = addr[3] = addr[4] = addr[5] = (char)0xFF;

    memcpy(f->field.header.ether_dhost, addr, ETHER_ADDR_LEN);
    memcpy(f->field.header.ether_shost, _native_tap_mac, ETHER_ADDR_LEN);
    f->field.header.ether_type = htons(NATIVE_ETH_PROTO);

    /* XXX: check overflow */
    memcpy(f->field.payload.data, packet->data, packet->length);
    f->field.payload.nn_header.length = htons(packet->length);
    f->field.payload.nn_header.dst = htons(packet->dst);
    f->field.payload.nn_header.src = htons(packet->src);

    data_len = packet->length + sizeof(struct nativenet_header);

    return data_len;
}

int send_buf(radio_packet_t *packet)
{
    uint8_t buf[TAP_BUFFER_LENGTH];
    int nsent, to_send;

    DEBUG("send_buf:  Sending packet of length %"PRIu16" from %"PRIu16" to %"PRIu16": %s\n", packet->length, packet->src, packet->dst, (char*) packet->data);
    to_send = _native_marshall_ethernet(buf, packet);

    if ((ETHER_HDR_LEN + to_send) < ETHERMIN) {
        DEBUG("padding data! (%d ->", to_send);
        to_send = ETHERMIN - (ETHER_HDR_LEN + sizeof(struct nativenet_header));
        DEBUG("%d)\n", to_send);
    }

    DEBUG("send_buf: trying to send %d bytes\n", to_send);

    if ((nsent = write(_native_tap_fd, buf, to_send + ETHER_HDR_LEN)) == -1) {;
        warn("write");
        return -1;
    }
    return 0;
}

int tap_init(char *name)
{

#ifdef __MACH__ /* OSX */
    char clonedev[255] = "/dev/"; /* XXX bad size */
    strncpy(clonedev+5, name, 250);
#else /* Linux */
    struct ifreq ifr;
    char *clonedev = "/dev/net/tun";
#endif

    /* implicitly create the tap interface */
    if ((_native_tap_fd = open(clonedev , O_RDWR)) == -1) {
        err(EXIT_FAILURE, "open(%s)", clonedev);
    }

#ifdef __MACH__ /* OSX */
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
        warn("ioctl");
        if (close(_native_tap_fd) == -1) {
            warn("close");
        }
        exit(EXIT_FAILURE);
    }

    /* TODO: use strncpy */
    strcpy(name, ifr.ifr_name);


    /* get MAC address */
    memset (&ifr, 0, sizeof (ifr));
    snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", name);
    if (ioctl(_native_tap_fd, SIOCGIFHWADDR, &ifr) == -1) {
        warn("ioctl");
        if (close(_native_tap_fd) == -1) {
            warn("close");
        }
        exit(EXIT_FAILURE);
    }
    memcpy(_native_tap_mac, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN);
#endif

    /* configure signal handler for fds */
    register_interrupt(SIGIO, _native_handle_tap_input);

#ifndef __MACH__ /* tuntap signalled IO not working in OSX */
    /* configure fds to send signals on io */
    if (fcntl(_native_tap_fd, F_SETOWN, getpid()) == -1) {
        err(1, "tap_init(): fcntl(F_SETOWN)");
    }

    /* set file access mode to nonblocking */
    if (fcntl(_native_tap_fd, F_SETFL, O_NONBLOCK|O_ASYNC) == -1) {
        err(1, "tap_init(): fcntl(F_SETFL)");
    }
#endif /* OSX */

    puts("RIOT native tap initialized.");
    return _native_tap_fd;
}
/** @} */
