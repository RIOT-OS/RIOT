/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief coap shell test application
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msg.h"
#include "posix_io.h"
#include "shell.h"
#include "board_uart0.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#define SHELL_MSG_BUFFER_SIZE (8)
msg_t msg_buffer_shell[SHELL_MSG_BUFFER_SIZE];

shell_t shell;

void test1(int argc, char **argv)
{
    etsi_main(argc, argv);
}

static const shell_command_t sc[] = {
        { "test", "call with: etsi_main <addr> <port> <log_level>", test1 },
        { NULL, NULL, NULL }
};

int main(void)
{
    puts("CCN!");

    puts("starting shell...");

    if (msg_init_queue(msg_buffer_shell, SHELL_MSG_BUFFER_SIZE) != 0) {
        DEBUG("msg init queue failed...abording\n");
        return -1;
    }

    puts("posix open");
    posix_open(uart0_handler_pid, 0);
    puts("shell init");
    shell_init(&shell, sc, UART0_BUFSIZE, uart0_readc, uart0_putc);
    puts("shell run");
    shell_run(&shell);

    return 0;
}
