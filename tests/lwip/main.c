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
 * @brief       Test for lwIP
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This test application tests the lwIP package.
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "common.h"
#include "lwip.h"
#include "lwip/netif.h"
#if LWIP_IPV4
#include "net/ipv6/addr.h"
#else
#include "net/ipv4/addr.h"
#endif
#include "shell.h"

#define IFCONFIG_FILLER     "       "

static int ifconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    for (struct netif *iface = netif_list; iface != NULL; iface = iface->next) {
        printf("%s_%02u: ", iface->name, iface->num);
#if IS_USED(MODULE_LWIP_IPV6)
        char addrstr[IPV6_ADDR_MAX_STR_LEN];
#elif IS_USED(MODULE_LWIP_IPV4)
        char addrstr[IPV4_ADDR_MAX_STR_LEN];
#endif
#ifdef MODULE_LWIP_IPV6
        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (!ipv6_addr_is_unspecified((ipv6_addr_t *)&iface->ip6_addr[i])) {
                printf("%s inet6 %s\n", (i == 0) ? "" : IFCONFIG_FILLER,
                       ipv6_addr_to_str(addrstr,
                                        (ipv6_addr_t *)&iface->ip6_addr[i],
                                        sizeof(addrstr)));
            }
        }
#endif
#ifdef MODULE_LWIP_IPV4
        if (IS_USED(MODULE_LWIP_IPV6)) {
            /* insert spaces to be aligned with inet6 */
            printf(IFCONFIG_FILLER);
        }
        printf(" inet %s\n", ipv4_addr_to_str(addrstr,
                                              (ipv4_addr_t *)&iface->ip_addr,
                                              sizeof(addrstr)));
#endif
        puts("");
    }
    return 0;
}

static const shell_command_t shell_commands[] = {
#ifdef MODULE_SOCK_IP
    { "ip", "Send IP packets and listen for packets of certain type", ip_cmd },
#endif
#ifdef MODULE_SOCK_TCP
    { "tcp", "Send TCP messages and listen for messages on TCP port", tcp_cmd },
#endif
#ifdef MODULE_SOCK_UDP
    { "udp", "Send UDP messages and listen for messages on UDP port", udp_cmd },
#endif
    { "ifconfig", "Shows assigned IP addresses", ifconfig },
    { NULL, NULL, NULL }
};

static char line_buf[SHELL_DEFAULT_BUFSIZE];

int main(void)
{
    puts("RIOT lwip test application");
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
