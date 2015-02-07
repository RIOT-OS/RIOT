/*
 * Copyright (C) 2014 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdio.h>

#include "net_if.h"
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"
#include "kernel.h"
#include "thread.h"
#include "coap_server.h"
#include "udp.h"

#define IF_ID   (0)

char addr_str[IPV6_MAX_ADDR_STR_LEN];
char coap_server_stack_buffer[COAP_SERVER_STACK_SIZE];

int main(void)
{
    ipv6_addr_t tmp;

    /* configure the network interface to use short address mode */
    net_if_set_src_address_mode(IF_ID, NET_IF_TRANS_ADDR_M_SHORT);

    /* configure link-local address */
    ipv6_addr_set_link_local_prefix(&tmp);

    if (!ipv6_addr_set_by_eui64(&tmp, IF_ID, &tmp)) {
        printf("Can not set link-local by EUI-64 on interface %d\n", IF_ID);
        return 0;
    }


    if (!ipv6_net_if_add_addr(IF_ID, &tmp, NDP_ADDR_STATE_PREFERRED,
                              NDP_OPT_PI_VLIFETIME_INFINITE,
                              NDP_OPT_PI_PLIFETIME_INFINITE, 0)) {
        printf("Can not add link-local address to interface %d\n", IF_ID);
        return 0;
    }

    /* add all nodes multicast address */
    ipv6_addr_set_all_nodes_addr(&tmp);
    printf("Add all nodes multicast address to interface %d: %s\n", IF_ID,
           inet_ntop(AF_INET6, &tmp, addr_str, IPV6_MAX_ADDR_STR_LEN));

    if (!ipv6_net_if_add_addr(IF_ID, &tmp, NDP_ADDR_STATE_PREFERRED,
                              NDP_OPT_PI_VLIFETIME_INFINITE,
                              NDP_OPT_PI_PLIFETIME_INFINITE, 0)) {
        printf("Can not add all nodes address to interface %d\n", IF_ID);
        return 0;
    }

    /* start CoAP server thread */
    thread_create(coap_server_stack_buffer, COAP_SERVER_STACK_SIZE, PRIORITY_MAIN - 2, CREATE_STACKTEST,
                  coap_server, NULL, "coap-server");

    /* Open the UART0 for the shell */
    (void) posix_open(uart0_handler_pid, 0);

    /* initialize the shell */
    shell_t shell;
    shell_init(&shell, NULL, UART0_BUFSIZE, uart0_readc, uart0_putc);

    /* start the shell loop */
    shell_run(&shell);
    return 0;
}
