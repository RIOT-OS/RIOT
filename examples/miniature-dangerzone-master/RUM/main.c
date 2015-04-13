/*
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup examples
 * @{
 *
 * @file
 * @brief UDP RPL example application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "net_if.h"
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"
#include "destiny.h"
#include "kernel.h"

#include "demo.h"

const shell_command_t shell_commands[] = {
    {"init", "Initialize network", rpl_udp_init},
    {"set", "Set ID", rpl_udp_set_id},
    {"table", "Shows the routing table", rpl_udp_table},
    {"dodag", "Shows the dodag", rpl_udp_dodag},
    {"loop", "", rpl_udp_loop},
    {"server", "Starts a UDP server", udp_server},
    {"send", "Send a UDP datagram", udp_send},
    {"ign", "ignore node", rpl_udp_ignore},
    {NULL, NULL, NULL}
};

void udpif_get_ipv6_address(ipv6_addr_t *addr, uint16_t local_addr)
{
    ipv6_addr_init(addr, 0xfe80, 0x0, 0x0, 0x0, 0x0, 0x00ff, 0xfe00, local_addr);
}

void fill_nc(void)
{
    int numne = 20;
    uint16_t neighbors[] = {12, 42, 56, 43, 45, 10, 25, 54, 52, 35, 26, 29, 38, 46, 11, 53, 27, 47, 6, 36};

    ipv6_addr_t r_addr;
    uint16_t l_addr;

    for (int i = 0; i < numne; i++) {
        printf("Adding %u as neighbor\n", neighbors[i]);
        udpif_get_ipv6_address(&r_addr, neighbors[i]);
        l_addr = HTONS(neighbors[i]);
        ndp_neighbor_cache_add(0, &r_addr, &l_addr, 2, 0,
                               NDP_NCE_STATUS_REACHABLE, 
                               NDP_NCE_TYPE_TENTATIVE, 
                               0xffff);
    }
}


int main(void)
{
    puts("RPL router v"APP_VERSION);

    fill_nc();
    /* start shell */
    posix_open(uart0_handler_pid, 0);
    net_if_set_src_address_mode(0, NET_IF_TRANS_ADDR_M_SHORT);

    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);

    shell_run(&shell);
    return 0;
}
