/*
 * Copyright (C) 2016 Luminița Lăzărescu <cluminita.lazarescu@gmail.com>
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Prints the real time offset from the system time
 *
 * @author      Luminița Lăzărescu <cluminita.lazarescu@gmail.com>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdio.h>
#include <time.h>

#include "net/sntp.h"
#include "net/ntp_packet.h"
#include "net/af.h"
#include "net/ipv6/addr.h"
#include "timex.h"

#define _DEFAULT_TIMEOUT (5000U);

static void _usage(char *cmd)
{
    printf("Usage: %s <server addr> [<timeout>]\n", cmd);
    puts("default: timeout = 5000");
}

int _ntpdate(int argc, char **argv)
{
    uint32_t timeout = _DEFAULT_TIMEOUT;

    if (argc < 2) {
        _usage(argv[0]);
        return 1;
    }
    sock_udp_ep_t server = { .port = NTP_PORT, .family = AF_INET6 };
    ipv6_addr_from_str((ipv6_addr_t *)&server.addr, argv[1]);

    if (ipv6_addr_is_link_local((ipv6_addr_t *)&server.addr)) {
        /* choose first interface when address is link local */
        gnrc_netif_t *netif = gnrc_netif_iter(NULL);
        server.netif = (uint16_t)netif->pid;
    }

    if (argc > 2) {
        timeout = atoi(argv[2]);
    }
    if (sntp_sync(&server, timeout) < 0) {
        puts("Error in synchronization");
        return 1;
    }
#ifdef MODULE_NEWLIB
    struct tm *tm;
    time_t time = (time_t)(sntp_get_unix_usec() / US_PER_SEC);

    tm = gmtime(&time);
    printf("%04i-%02i-%02i %02i:%02i:%02i UTC (%i us)\n",
           tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
           tm->tm_hour, tm->tm_min, tm->tm_sec,
           (int)sntp_get_offset());
#else
    uint64_t time = sntp_get_unix_usec();
    printf("%" PRIu32 ".%" PRIu32 " (%i us)\n",
           (uint32_t)(time / US_PER_SEC),
           (uint32_t)(time / US_PER_SEC),
           (int)sntp_get_offset());
#endif
    return 0;
}
