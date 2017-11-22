/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2018 Inria
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
 * @brief       Example application  for TinyDTLS
 *
 * @author      Raul Fuentes <>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#include "dtls.h"

/* TinyDTLS WARNING check */
#ifdef WITH_RIOT_SOCKETS
#error TinyDTLS is set to use sockets but the app is configured for socks.
#endif

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int udp_client_cmd(int argc, char **argv);
extern int udp_server_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "dtlsc", "Start a DTLS client", udp_client_cmd },
    { "dtlss", "Start and stop a DTLS server", udp_server_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT (Tiny)DTLS testing implementation");

    /* TinyDTLS settings (Universal and called only one time by reboot) */
    dtls_init();

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
