/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for libcoap client
 *
 * @author      Raul Fuentes <> <raul.fuentes-samaniego@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#include "coap3/coap.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int client_coap_init(int argc, char **argv);

SHELL_COMMAND(coapc, "Start a libcoap client", client_coap_init);

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT libcoap client testing implementation");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
