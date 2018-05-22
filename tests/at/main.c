/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief    AT module test application
 *
 * @author   Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdio.h>

#include "at.h"
#include "shell.h"
#include "timex.h"

static at_dev_t at_dev;
static char buf[256];
static char resp[1024];

#ifndef UART_AT
#define UART_AT     UART_DEV(1)
#endif

#ifndef BAUDRATE_AT
#define BAUDRATE_AT 115200
#endif

static int init(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    at_dev_init(&at_dev, UART_AT, BAUDRATE_AT, buf, sizeof(buf));

    return 0;
}

static int send(int argc, char **argv)
{
    if (argc < 2) {
        puts("Please enter a command");
        return 1;
    }

    if (at_send_cmd_get_resp(&at_dev, argv[1], resp, sizeof(resp), 10 * US_PER_SEC) > 0) {
        puts("Response:");
        puts(resp);
    }
    else {
        puts("Error");
    }

    return 0;
}

static int drain(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    at_drain(&at_dev);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "Initialize AT device", init },
    { "send", "Send a command and wait response", send },
    { "drain", "Drain AT device", drain },
    { NULL, NULL, NULL },
};

int main(void)
{
    puts("AT command test app");

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
