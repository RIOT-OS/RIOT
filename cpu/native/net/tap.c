#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <arpa/inet.h>

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
    unsigned char buf[TAP_BUFFER_LENGTH];
    union eth_frame *f;
    radio_packet_t p;

    DEBUG("_native_handle_tap_input\n");

    /* TODO: check whether this is an input or an output event
       TODO: refactor this into general io-signal multiplexer */

    _native_in_syscall = 1;
    nread = read(_native_tap_fd, buf, TAP_BUFFER_LENGTH);
    _native_in_syscall = 0;
    DEBUG("_native_handle_tap_input - read %d bytes\n", nread);
    if (nread > 0) {
        f = (union eth_frame*)&buf;
        if (ntohs(f->field.header.ether_type) == NATIVE_ETH_PROTO) {
            nread = nread - ETHER_HDR_LEN;
            if ((nread - 1) <= 0) {
                DEBUG("_native_handle_tap_input: no payload");
            }
            else {
                /* XXX: check overflow */
                p.length = (uint8_t)buf[ETHER_HDR_LEN];
                p.dst = (uint8_t)buf[ETHER_HDR_LEN+1];
                p.data = buf+ETHER_HDR_LEN+2;
                DEBUG("_native_handle_tap_input: received packet of length %u for %u: %s\n", p.length, p.dst, (char*) p.data);
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
    memcpy(f->field.data+2, packet->data, packet->length);
    f->field.data[0] = packet->length;
    f->field.data[1] = packet->dst;
    data_len = packet->length + 2;

    return data_len;
}

int send_buf(radio_packet_t *packet)
{
    uint8_t buf[TAP_BUFFER_LENGTH];
    int nsent, to_send;

    DEBUG("send_buf:  Sending packet of length %u to %u: %s\n", packet->length, packet->dst, (char*) packet->data);
    to_send = _native_marshall_ethernet(buf, packet);

    if ((ETHER_HDR_LEN + to_send) < ETHERMIN) {
        DEBUG("padding data! (%d ->", to_send);
        to_send = ETHERMIN - ETHER_HDR_LEN;
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
