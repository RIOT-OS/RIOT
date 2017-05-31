/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file main.c
 * @brief posix dns test application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <arpa/inet.h>

#include "dns.h"

#define IN6ADDRSZ 16

int main(void)
{
    char *host = "www.google.com";
    uint16_t type = DNS_TYPE_A;
    uint8_t ip[IN6ADDRSZ];
    dns_resolv(host, type, ip);

    char ip_buf[64];
    inet_ntop(AF_INET6, &ip, ip_buf, IN6ADDRSZ);
    printf("host: '%s' ip: '%s'\n", host, ip_buf);

    puts("main: finished");
    return 0;
}
