/*
 * Copyright (C) 2016 Luminița Lăzărescu <cluminita.lazarescu@gmail.com>
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
 */


#include "net/sntp.h"
#include "net/ntp_packet.h"
#include "net/af.h"
#include "net/ipv6/addr.h"

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
    if (argc > 2) {
        timeout = atoi(argv[2]);
    }
    if (sntp_sync(&server, timeout) < 0) {
        puts("Error in synchronization");
        return 1;
    }
    printf("Offset: %i\n", (int)sntp_get_offset());
    return 0;
}
