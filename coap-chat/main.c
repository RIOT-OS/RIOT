/*
 * Copyright (c) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     applications
 * @{
 *
 * @file
 * @brief       coap-chat - Main loop and shell handlers
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>
#include "msg.h"

#include "net/gcoap.h"
#include "kernel_types.h"
#include "shell.h"

#define BUFLEN          (64U)
#define MAIN_QUEUE_SIZE (4)

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int coap_post(const char *addr, const char *buf, size_t buflen);
extern void coap_init(void);

int chat(int argc, char **argv)
{
    if (argc < 4) {
        puts("usage: chat <addr> <nick> <msg>");
        return 1;
    }

    char buf[BUFLEN] = { 0 };
    size_t len = snprintf(buf, BUFLEN, "%s: %s", argv[2], argv[3]);
    for (int i = 4; i < argc; ++i) {
        len += snprintf(buf + len, BUFLEN - len, " %s", argv[i]);
    }
    coap_post(argv[1], buf, len);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "chat", "CoAP chat", chat },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    /* init coap listener */
    coap_init();
    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}
