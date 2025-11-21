/*
 * SPDX-FileCopyrightText: 2019 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the USBUS CDC ECM interface
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE     (8U)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("Test application for the USBUS CDC ECM interface\n");
    puts("This test pulls in parts of the GNRC network stack, use the\n"
         "provided shell commands (i.e. ifconfig, ping) to interact with\n"
         "the CDC ECM based network interface.\n");

    /* start shell */
    puts("Starting the shell now...");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
