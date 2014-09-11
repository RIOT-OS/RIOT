/*
 * Copyright (C) 2013, 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include "udp.h"

#include "rpl_udp.h"

const shell_command_t shell_commands[] = {
    {"init", "Initialize network", rpl_udp_init},
    {"set", "Set ID", rpl_udp_set_id},
    {"dodag", "Shows the dodag", rpl_udp_dodag},
    {"server", "Starts a UDP server", udp_server},
    {"send", "Send a UDP datagram", udp_send},
    {"ign", "Ignore a node", rpl_udp_ignore},
    {NULL, NULL, NULL}
};

int main(void)
{
    puts("RPL router v"APP_VERSION);

    /* start shell */
    posix_open(uart0_handler_pid, 0);
    net_if_set_src_address_mode(0, NET_IF_TRANS_ADDR_M_SHORT);

    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);

    shell_run(&shell);
    return 0;
}
