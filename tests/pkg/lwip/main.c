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
