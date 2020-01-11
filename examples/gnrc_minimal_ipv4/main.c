/*
 * Copyright (C) 2015 Inria
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
 * @brief       Showing minimum memory footprint of gnrc network stack
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "msg.h"
#include "net/ipv4/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "shell.h"
#include "xtimer.h"


#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int udp_cmd(int argc, char **argv);
#ifdef MODULE_GNRC_TCP
    extern int tcp_cmd(int argc, char **argv);
#endif
extern int _gnrc_icmpv4_ping(int argc, char **argv);


int _gnrc_icmpv4_ping_fake(int argc, char **argv) {
    char * argvv[2] = {"ping4", "192.168.0.254"};
    return _gnrc_icmpv4_ping(2,argvv);
}

static const shell_command_t shell_commands[] = {
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
#ifdef MODULE_GNRC_TCP
    { "tcp", "send data over TCP and listen on TCP ports", tcp_cmd },
#endif
    { "ping4", "Ping via ICMPv4", _gnrc_icmpv4_ping },
    { "pt", "ping4 192.168.0.254", _gnrc_icmpv4_ping_fake },
    { NULL, NULL, NULL }
};

int main(void)
{

    puts("RIOT network stack example application");

    /* get interfaces and print their addresses */
    gnrc_netif_t *netif = NULL;
    ipv4_addr_t addr_mask;
    netif = gnrc_netif_iter(netif);
    ipv4_addr_t ipv4_addrs[GNRC_NETIF_IPV4_ADDRS_NUMOF];
    //ipv6_addr_t ipv6_addrs[GNRC_NETIF_IPV4_ADDRS_NUMOF];
    char ipv4_addr[IPV4_ADDR_MAX_STR_LEN];
    ipv4_addr_t set_ipv4_addr = {{192, 168, 0, 222}};
    //ipv4_addr_t set_ipv4_addr = {{192, 168, 11, 222}};
    printf("My netif->pid is %d\n", netif->pid);
    addr_mask = ipv4_mask_to_addr(24);
    printf("My mask is %s\n", ipv4_addr_to_str(ipv4_addr, &addr_mask, IPV4_ADDR_MAX_STR_LEN));
    //xtimer_usleep(1000000); // 100ms

    int res = gnrc_netif_ipv4_addr_add(netif, &set_ipv4_addr, 24, 0);

    /*printf("My res= is %d\n", res);
    res = gnrc_netapi_get(netif->pid, NETOPT_IPV6_ADDR, 0, ipv6_addrs, sizeof(ipv6_addrs));
    printf("My res= is %d\n", res);
    res = gnrc_netapi_set(netif->pid, NETOPT_IPV6_ADDR_REMOVE, 24, ipv6_addrs, sizeof(ipv6_addrs));*/
    printf("My res= is %d\n", res);
    res = gnrc_netapi_get(netif->pid, NETOPT_IPV4_ADDR, 0, ipv4_addrs, sizeof(ipv4_addrs));
    printf("My res= is %d\n", res);
    if (res < 0) {
        
    } else {
        for (unsigned i = 0; i < (unsigned)(res / sizeof(ipv4_addr_t)); i++) {
            //char ipv4_addr[IPV4_ADDR_MAX_STR_LEN];

            //ipv4_addr = ipv4_addr_to_str(ipv4_addr, &ipv4_addrs[i], IPV4_ADDR_MAX_STR_LEN);
            printf("My address is %s\n", ipv4_addr_to_str(ipv4_addr, &ipv4_addrs[i], IPV4_ADDR_MAX_STR_LEN));
        }
    }




    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
