/*
 * Copyright (C) 2016 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for esp8266 network device driver
 *
 * @author      Markus Blechschmidt <markus.blechschmidt@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>
#include <net/ipv4/addr.h>
#include "net/af.h"
#include "net/sock/udp.h"

#define ENABLE_DEBUG  (1)
#include "debug.h"

char payload[] = "hello world\n";
uint8_t buf[128];

int main(void)
{
    DEBUG("starting %s\r\n", __FILE__);
    sock_udp_t sock;
    sock_udp_ep_t local = SOCK_IPV4_EP_ANY;

    local.port = 1337;

    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return 1;
    }
    while (1) {
        sock_udp_ep_t remote;
        ssize_t res;
        if ((res = sock_udp_recv(&sock, buf, sizeof(buf), SOCK_NO_TIMEOUT,
                                 &remote)) >= 0) {
            printf("Received a message\r\n");
            /* print message characterwise */
            for  (int i = 0; i < res; i ++) {
                printf("%c\t0x%02x\r\n", buf[i], buf[i]);
            }
            printf("Sending echo\r\n");
            if (sock_udp_send(&sock, buf, res, &remote) < 0) {
                puts("Error sending reply");
            }
        }
    }
    return 0;
}
