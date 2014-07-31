/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief PNet test application
 *
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "net_if.h"
#include "sixlowpan.h"
#include "ipv6.h"

#ifndef R_ADDR
#define R_ADDR  (1)
#endif

#define PORT    (1234)

#define ERROR(...)  printf("ERROR: " __VA_ARGS__)

int init_local_address(uint16_t r_addr)
{
    ipv6_addr_t std_addr;
    ipv6_addr_init(&std_addr, 0xabcd, 0xef12, 0, 0, 0x1034, 0x00ff, 0xfe00,
                   0);
    net_if_set_src_address_mode(0, NET_IF_TRANS_ADDR_M_SHORT);
    return net_if_set_hardware_address(0, r_addr) &&
           sixlowpan_lowpan_init_adhoc_interface(0, &std_addr);
}

int main(void)
{
    int sockfd, res;
    struct sockaddr_in6 my_addr, their_addr;
    char buffer[14];

    their_addr.sin6_family = AF_INET6;
#if R_ADDR == 1
    their_addr.sin6_port = PORT;
#else
    their_addr.sin6_port = 0;
#endif
    their_addr.sin6_flowinfo = 0;
    memset(&(their_addr.sin6_addr), 0, sizeof(their_addr.sin6_addr));
    their_addr.sin6_addr.uint16[0] = htons(0xabcd);
    their_addr.sin6_addr.uint16[1] = htons(0xef12);
    their_addr.sin6_addr.uint16[5] = htons(0x00ff);
    their_addr.sin6_addr.uint16[6] = htons(0xfe00);
#if R_ADDR == 1
    their_addr.sin6_addr.uint16[7] = htons(2);
#else
    their_addr.sin6_addr.uint16[7] = htons(1);
#endif
    their_addr.sin6_scope_id = 0;

    if (!init_local_address(R_ADDR)) {
        ERROR("Can not initialize IP for hardware address %d.", R_ADDR);
        return 1;
    }

    memcpy(buffer, "Hello, World!", 14);
    memcpy(&my_addr, &in6addr_any, sizeof(my_addr));

    my_addr.sin6_port = PORT;

    sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    res = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));

    if (res < 0) {
        ERROR("Socket could not be bind");
        return 1;
    }

#if R_ADDR == 1
    res = sendto(sockfd, buffer, strlen(buffer), 0,
                 (struct sockaddr *)&their_addr,
                 (socklen_t) sizeof(their_addr));
#else
    socklen_t their_len;
    res = recvfrom(sockfd, buffer, strlen(buffer), 0,
                   (struct sockaddr *)&their_addr,
                   &their_len);

    if (their_addr.sin6_addr.uint8[15] != 1) {
        ERROR("Wrong sender address: %d\n", their_addr.sin6_addr.uint8[11]);
        return 1;
    }

    printf("Port: %d\n", their_addr.sin6_port);
#endif

    if (res < 0) {
        ERROR("Message error");
        return 1;
    }

    close(sockfd);
    printf("All tests successful.\n");
    return 0;
}
