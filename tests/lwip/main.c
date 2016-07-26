/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Test for raw IPv6 connections
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This test application tests the gnrc_conn_ip module. If you select protocol 58 you can also
 * test if gnrc is able to deal with multiple subscribers to ICMPv6 (gnrc_icmpv6 and this
 * application).
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "common.h"
#include "lwip.h"
#include "lwip/netif.h"
#include "net/ipv6/addr.h"
#include "shell.h"

static int ifconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    for (struct netif *iface = netif_list; iface != NULL; iface = iface->next) {
        printf("%s_%02u: ", iface->name, iface->num);
#ifdef MODULE_LWIP_IPV6
        char addrstr[IPV6_ADDR_MAX_STR_LEN];
        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (!ipv6_addr_is_unspecified((ipv6_addr_t *)&iface->ip6_addr[i])) {
                printf(" inet6 %s\n", ipv6_addr_to_str(addrstr, (ipv6_addr_t *)&iface->ip6_addr[i],
                                                       sizeof(addrstr)));
            }
        }
#endif
        puts("");
    }
    return 0;
}

static const shell_command_t shell_commands[] = {
#ifdef MODULE_CONN_IP
    { "ip", "Send IP packets and listen for packets of certain type", ip_cmd },
#endif
#ifdef MODULE_CONN_UDP
    { "udp", "Send UDP messages and listen for messages on UDP port", udp_cmd },
#endif
    { "ifconfig", "Shows assigned IPv6 addresses", ifconfig },
    { NULL, NULL, NULL }
};
static char line_buf[SHELL_DEFAULT_BUFSIZE];

char conn_inbuf[CONN_INBUF_SIZE];

int main(void)
{
    puts("RIOT lwip test application");
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
