/*
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT's POSIX sockets
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "msg.h"
#include "shell.h"

#define MAIN_MSG_QUEUE_SIZE (4)
static msg_t main_msg_queue[MAIN_MSG_QUEUE_SIZE];

int main(void)
{
    /* a sendto() call performs an implicit bind(), hence, a message queue is
     * required for the thread executing the shell */
    msg_init_queue(main_msg_queue, MAIN_MSG_QUEUE_SIZE);
    puts("RIOT socket example application");
    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
