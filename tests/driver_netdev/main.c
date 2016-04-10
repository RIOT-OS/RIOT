/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Freie Universität Berlin
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
 * @brief       Test application for network device drivers
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
    puts("Test application for network device drivers\n");

#ifdef MULTIPLE_DRIVERS
    printf("The makefile includes multiple drivers.\n"
            "As the default parameters overlap, either remove all but one driver\n"
            "from the Makefile or remove the MULTIPLE_DRIVERS define.\n");
    return 0;
#endif

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("This test just pulls in parts of the GNRC network stack, use the\n"
         "provided shell commands (i.e. ifconfig, ping6) to interact with\n"
         "your device.\n");

    /* start shell */
    puts("Starting the shell now...");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
