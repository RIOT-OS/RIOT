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

#define _DEFAULT_TIMEOUT (500000LU)

static void _usage(char *cmd)
{
    printf("Usage: %s <server addr>[%%<interface>] [<timeout in us>]\n", cmd);
    printf("default: timeout = %lu\n", _DEFAULT_TIMEOUT);
}

int _ntpdate(int argc, char **argv)
{
    uint32_t timeout = _DEFAULT_TIMEOUT;

    if (argc < 2) {
        _usage(argv[0]);
        return 1;
    }
    sock_udp_ep_t server = { .port = NTP_PORT, .family = AF_INET6 };
    ipv6_addr_t *addr = (ipv6_addr_t *)&server.addr;

    char *iface = ipv6_addr_split_iface(argv[1]);
    kernel_pid_t src_iface = iface ? atoi(iface) : KERNEL_PID_UNDEF;

    if (ipv6_addr_from_str(addr, argv[1]) == NULL) {
        puts("error: malformed address");
        return 1;
    }

    if (ipv6_addr_is_link_local(addr) || (src_iface != KERNEL_PID_UNDEF)) {
        size_t ifnum = gnrc_netif_numof();

        if (src_iface == KERNEL_PID_UNDEF) {
            if (ifnum == 1) {
                src_iface = gnrc_netif_iter(NULL)->pid;
            }
            else {
                puts("error: link local target needs interface parameter (use \"<address>%<ifnum>\")\n");
                return 1;
            }
        }
        else {
            if (gnrc_netif_get_by_pid(src_iface) == NULL) {
                printf("error: %"PRIkernel_pid" is not a valid interface.\n", src_iface);
                return 1;
            }
        }
        server.netif = src_iface;
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
