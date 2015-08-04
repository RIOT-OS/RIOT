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
#ifdef MODULE_NEWLIB
#   include "uart_stdio.h"
#else
#   include "posix_io.h"
#   include "board_uart0.h"
#endif
#include "shell.h"
#include "shell_commands.h"
#include "udp.h"

#include "rpl_udp.h"

static const shell_command_t shell_commands[] = {
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

    net_if_set_src_address_mode(0, NET_IF_TRANS_ADDR_M_SHORT);
    id = net_if_get_hardware_address(0);

    shell_t shell;
    /* start shell */
#ifndef MODULE_NEWLIB
    (void) posix_open(uart0_handler_pid, 0);
    shell_init(&shell, NULL, UART0_BUFSIZE, uart0_readc, uart0_putc);
#else
    shell_init(&shell, NULL, UART0_BUFSIZE, getchar, putchar);
#endif

    shell_run(&shell);
    return 0;
}
