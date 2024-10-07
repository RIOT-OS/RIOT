/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
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
 * @brief       Example application to access RIOT shell commands from Linux
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include "shell.h"
#include "ztimer.h"

int main(int argc, char **argv)
{
    if (argc < 1) {
        printf("usage: <command> [args]\n");
        return 0;
    }

    /* argv[0] will be the first 'user' argument to native */
    shell_command_handler_t handler = shell_find_handler(NULL, argv[0]);
    if (handler == NULL) {
        return -EINVAL;
    }

    /* wait some time for the network to be ready */
    ztimer_sleep(ZTIMER_MSEC, 1000);

    msg_t msg_queue[8];
    msg_init_queue(msg_queue, ARRAY_SIZE(msg_queue));

    return handler(argc, argv);
}
