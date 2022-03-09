/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the ENCx26J600 Ethernet device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
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

    puts("Test application for the encx24j600 driver\n");
    puts("This test just pulls in parts of the GNRC network stack, use the\n"
         "provided shell commands (i.e. ifconfig, ping) to interact with\n"
         "your encx24j600 device.\n");

    /* start shell */
    puts("Starting the shell now...");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
