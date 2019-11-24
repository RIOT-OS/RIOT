/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#define UHCP_MCAST_ADDR "ff15::ABCD"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "net/uhcp.h"

char _prefix[16];
unsigned _prefix_len;

static const char *BIND_OPTION = "--bind-to-device";
static void bind_to_device(int sock, const char *interface);

int ipv6_addr_split(char *addr_str, char separator, int _default)
{
    char *sep = addr_str;
    while(*++sep) {
        if (*sep == separator) {
            *sep++ = '\0';
            if (*sep) {
                _default = atoi(sep);
            }
            break;
        }
    }

    return _default;
}

int main(int argc, char *argv[])
{
    unsigned ifindex;
    unsigned _bind_to_device = 0;

    if (argc < 3) {
        fprintf(stderr, "usage: uhcpd <interface> <prefix/prefix_length> [%s]\n",
                BIND_OPTION);
        exit(1);
    }

    ifindex = if_nametoindex(argv[1]);
    if (!ifindex) {
        fprintf(stderr, "error: invalid interface \"%s\"\n", argv[1]);
        exit(1);
    }

    _prefix_len = ipv6_addr_split(argv[2], '/', 64);
    if ((!inet_pton(AF_INET6, argv[2], _prefix)) || (_prefix_len > 128)) {
        fprintf(stderr, "error: cannot parse prefix\n");
        exit(1);
    }

    if (argc == 4) {
        if (strcmp(BIND_OPTION, argv[3])) {
            fprintf(stderr, "error: unknown option\n");
            exit(1);
        }
        _bind_to_device = 1;
    }

    char *addr_str = UHCP_MCAST_ADDR;

    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET6;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_protocol = IPPROTO_UDP;
    hint.ai_flags |= AI_NUMERICHOST;

    struct addrinfo *mcast_addr;
    int res = getaddrinfo(addr_str, UHCP_PORT_STR,
                          &hint, &mcast_addr);
    if (res != 0) {
        perror("getaddrinfo()");
        exit(1);
    }

    int sock = socket(mcast_addr->ai_family, mcast_addr->ai_socktype,
                      mcast_addr->ai_protocol);
    if (sock < 0) {
        perror("socket() failed");
        exit(1);
    }

    if (_bind_to_device) {
        bind_to_device(sock, argv[1]);
    }

    if (bind(sock, mcast_addr->ai_addr, mcast_addr->ai_addrlen) < 0) {
        perror("bind() failed");
        exit(1);
    }

    /* join multicast group */
    struct ipv6_mreq mreq;
    memcpy(&mreq.ipv6mr_multiaddr,
           &((struct sockaddr_in6 *)mcast_addr->ai_addr)->sin6_addr,
           sizeof(struct in6_addr));

    mreq.ipv6mr_interface = ifindex;

    puts("Joining IPv6 multicast group...");
    if (setsockopt(sock, IPPROTO_IPV6, IPV6_JOIN_GROUP,
                   &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt(IPV6_JOIN_GROUP) failed");
        exit(1);
    }
    freeaddrinfo(mcast_addr);

    char buf[2048];
    struct sockaddr_in6 src_addr;
    unsigned n = sizeof(src_addr);;

    puts("entering loop...");
    while(1) {
        int nbytes = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&src_addr, &n);
        if (nbytes < 0) {
            perror("recvfrom() failed");
            continue;
        }
        uhcp_handle_udp((uint8_t *)buf, nbytes, (uint8_t *)&src_addr.sin6_addr, ntohs(src_addr.sin6_port), ifindex);
    }

    close(sock);
    exit(0);
}

int udp_sendto(uint8_t *buf, size_t len, uint8_t *dst, uint16_t dst_port, uhcp_iface_t iface)
{
    struct sockaddr_in6 dst_addr;
    memset(&dst_addr, '\0', sizeof(dst_addr));
    dst_addr.sin6_family = AF_INET6;
    memcpy(&dst_addr.sin6_addr, dst, 16);
    dst_addr.sin6_port = htons(dst_port);
    dst_addr.sin6_scope_id = iface;

    int fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("creating send socket");
        return -1;
    }

    ssize_t res;
    if ((res = sendto(fd, buf, len, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr))) == -1) {
        perror("udp_sendto(): sendto()");
    }

    close(fd);

    return res;
}

/* Requires to be 'root', I didn't find another solution for the moment */
static void bind_to_device(int sock, const char *interface)
{
    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,
                   interface, IF_NAMESIZE) < 0 ) {
        perror("setsockopt(SO_BINDTODEVICE) failed");
        exit(1);
    }
}
