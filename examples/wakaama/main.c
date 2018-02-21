/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
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
 * @brief       Example application  for Eclipse Wakaama LwM2M Client
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "msg.h"
#include "shell.h"

#include "memarray_limits.h"

#include "connection.h"
#include "lwm2mclient.h"

#define READER_QUEUE_SIZE (8U)
#define SHELL_QUEUE_SIZE (8U)
char _server_stack[THREAD_STACKSIZE_MAIN + THREAD_EXTRA_STACKSIZE_PRINTF];

static kernel_pid_t _wakaama_kernel_pid = KERNEL_PID_UNDEF;

/*
 * Workaround for stuck mainloop, since run_server() apparently doesn't
 * clean up properly yet.
 */
#ifdef BOARD_NATIVE
static int exit_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    exit(0);
}
#endif /* BOARD_NATIVE */

static const shell_command_t shell_commands[] = {
#ifdef BOARD_NATIVE
    { "exit", "Exit the shell", exit_cmd },
#endif /* BOARD_NATIVE */
    { NULL, NULL, NULL }
};

int main(void)
{
    lwm2m_memarray_init();

#ifdef MODULE_TINYDTLS
    dtls_init();
    dtls_set_log_level(DTLS_LOG_LEVEL);
#endif /* MODULE_TINYDTLS */

    /* Only one instance of the server */
    if (_wakaama_kernel_pid != KERNEL_PID_UNDEF) {
        printf("Error: server already running\n");
        return -1;
    }

    _wakaama_kernel_pid = thread_create(
        _server_stack, sizeof(_server_stack), THREAD_PRIORITY_MAIN - 1,
        THREAD_CREATE_STACKTEST, lwm2m_run_server, NULL, "wakaama client");

    msg_t _shell_queue[SHELL_QUEUE_SIZE];
    msg_init_queue(_shell_queue, SHELL_QUEUE_SIZE);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
