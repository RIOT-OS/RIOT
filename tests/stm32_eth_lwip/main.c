/*
 * Copyright (C) 2017 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Show case application for stm32 eth driver
 *
 * @author      Víctor Ariño <victor.arino@zii.aero>
 *
 * @}
 */

#include <stdio.h>

#include "net/netdev.h"
#include "net/netdev/eth.h"
#include "net/sock/udp.h"
#include "net/sock/udp.h"
#include "net/phy.h"

#include "lwip.h"
#include "lwip/netif.h"
#include "lwip/netif/netdev.h"

#include "xtimer.h"

#define SET_IP(x, ...) IP4_ADDR(x, __VA_ARGS__)

static netdev_t stm32f4eth;
static struct netif netif;
static ip4_addr_t local_ip, netmask, gw;
static sock_udp_ep_t local = SOCK_IPV4_EP_ANY;
static sock_udp_ep_t remote;
static uint8_t buff[128];

void eth_netdev_setup(netdev_t *netdev);

int main(void)
{
    puts("Booting");

    eth_netdev_setup(&stm32f4eth);

    SET_IP(&local_ip, 172, 16, 19, 5);
    SET_IP(&netmask, 255, 255, 255, 0);
    SET_IP(&gw, 172, 16, 19, 1);

    if (netif_add(&netif, &local_ip, &netmask, &gw, &stm32f4eth,
                  lwip_netdev_init, tcpip_input) == NULL) {
        puts("error netif_add");
        return -1;
    }
    else if (netif.state != NULL) {
        netif_set_default(&netif);
    }

    sock_udp_t sock;
    local.port = 12345;

    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return 1;
    }

    puts("Waiting for messages...");
    while (1) {
        ssize_t res;
        if ((res = sock_udp_recv(&sock, buff, sizeof(buff), SOCK_NO_TIMEOUT,
                                 &remote)) >= 0) {
            puts("Received a message");
            if (sock_udp_send(&sock, buff, res, &remote) < 0) {
                puts("Error sending reply");
            }
        }
    }

    return 0;
}
