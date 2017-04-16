/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       sock DNS client test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stddef.h>
#include <stdio.h>

#include <arpa/inet.h>

#include "net/sock/dns.h"
#include "net/sock/util.h"
#include "xtimer.h"

#ifndef TEST_NAME
#define TEST_NAME   "example.org"
#endif

#ifndef DNS_SERVER
#define DNS_SERVER  "[2001:db8::1]:53"
#endif

/* global DNS server UDP endpoint */
sock_udp_ep_t sock_dns_server;

/* import "ifconfig" shell command, used for printing addresses */

extern int _netif_config(int argc, char **argv);

int main(void)
{
    uint8_t addr[16] = {0};

    sock_udp_str2ep(&sock_dns_server, DNS_SERVER);

    puts("waiting for router advertisement...");
    xtimer_usleep(1U*1000000);

    /* print network addresses */
    puts("Configured network interfaces:");
    _netif_config(0, NULL);

    int res = sock_dns_query(TEST_NAME, addr, AF_UNSPEC);
    if (res > 0) {
        char addrstr[INET6_ADDRSTRLEN];
        inet_ntop(res == 4 ? AF_INET : AF_INET6, addr, addrstr, sizeof(addrstr));
        printf("%s resolves to %s\n", TEST_NAME, addrstr);
    }
    else {
        printf("error resolving %s\n", TEST_NAME);
    }

    return 0;
}
