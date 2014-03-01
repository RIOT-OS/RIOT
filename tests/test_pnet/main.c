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
#include "ipv6.h"

#ifdef MODULE_NATIVENET
#define TRANSCEIVER TRANSCEIVER_NATIVE
#else
#define TRANSCEIVER TRANSCEIVER_CC1100
#endif

#ifndef R_ADDR
#define R_ADDR  (1)
#endif

#define PORT    (1234)

int init_local_address(uint16_t r_addr)
{
    ipv6_addr_t std_addr;
    ipv6_addr_init(&std_addr, 0xabcd, 0xef12, 0, 0, 0x1034, 0x00ff, 0xfe00,
                   0);
    return net_if_set_hardware_address(r_addr) &&
           sixlowpan_lowpan_adhoc_init(0, &std_addr);
}

int main(void)
{
    int sockfd, res;
    struct sockaddr_in6 my_addr, their_addr = {
        .sin6_family = AF_INET6,
#if R_ADDR == 1
        .sin6_port = PORT,
#else
        .sin6_port = 0,
#endif
        .sin6_flowinfo = 0,
#if R_ADDR == 1
        .sin6_addr = {{{
                    0xab, 0xcd, 0xef, 0x12, 0x00, 0x00, 0x00, 0x00,
                    0x10, 0x34, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x02
                }
            }
        },
#else
        .sin6_addr = {{{
                    0xab, 0xcd, 0xef, 0x12, 0x00, 0x00, 0x00, 0x00,
                    0x10, 0x34, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x01
                }
            }
        },
#endif
        .sin6_scope_id = 0,
    };
    char buffer[14];

    if (!init_local_address(R_ADDR)) {
        fprintf(stderr, "Can not initialize IP for hardware address %d.", R_ADDR);
        return 1;
    }

    memcpy(buffer, "Hello, World!", 14);
    memcpy(&my_addr, &in6addr_any, sizeof(my_addr));

    my_addr.sin6_port = PORT;

    sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    res = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));

    if (res < 0) {
        perror("Socket could not be bind");
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
        fprintf(stderr, "Wrong sender address: %d\n", their_addr.sin6_addr.uint8[11]);
        return 1;
    }

    printf("Port: %d\n", their_addr.sin6_port);
#endif

    if (res < 0) {
        perror("Message error");
        return 1;
    }

    close(sockfd);
    printf("All tests successful.\n");
    return 0;
}
