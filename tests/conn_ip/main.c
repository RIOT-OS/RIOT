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

#include <stdio.h>

#include "msg.h"
#include "shell.h"

extern int ip_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "ip", "send hex over IP and listen for IP packets of certain type", ip_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("RIOT socket example application");
    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
