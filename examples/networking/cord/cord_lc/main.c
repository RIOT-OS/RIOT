/*
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory lookup (cord_lc) example
 *
 * @author      Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("CoRE RD lookup client example!\n");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
