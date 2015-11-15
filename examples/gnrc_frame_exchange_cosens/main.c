/*
 * Copyright (C) 2015 INRIA
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
 * @brief       Example application for demonstrating the
 *              CoSenS MAC layer for RIOT "gnrc" network stack
 *              Right now, this example only works for AT86RF2XX radios...
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "board.h"
#include "msg.h"
#include "panic.h"

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#include "net/gnrc/cosens.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int cosens_send_cmd(int argc, char **argv);
extern void start_frame_rx_server(void);


static const shell_command_t shell_commands[] = {
    { "send_frame", "send frame (CoSenS)", cosens_send_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT CoSenS MAC layer example application.");

    /* Start IEEE 802.15.4 frame RX server */
    start_frame_rx_server();

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
