/*
 * Copyright (C) 2018 OTA keys S.A.
 *               2018 Inria
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
 * @author   Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "at.h"
#include "shell.h"
#include "timex.h"

#include "periph/uart.h"

static at_dev_t at_dev;
static char buf[256];
static char resp[1024];

static int init(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (argc < 3) {
        printf("Usage: %s <uart> <baudrate>\n", argv[0]);
        return 1;
    }

    uint8_t uart = atoi(argv[1]);
    uint32_t baudrate = atoi(argv[2]);

    at_dev_init(&at_dev, UART_DEV(uart), baudrate, buf, sizeof(buf));

    return 0;
}

static int send(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <command>\n", argv[0]);
        return 1;
    }

    ssize_t len;
    if ((len = at_send_cmd_get_resp(&at_dev, argv[1], resp, sizeof(resp), 10 * US_PER_SEC)) < 0) {
        puts("Error");
        return 1;
    }

    printf("Response (len=%d): %s\n", (int)len, resp);

    return 0;
}

static int send_ok(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <command>\n", argv[0]);
        return 1;
    }

    if (at_send_cmd_wait_ok(&at_dev, argv[1], 10 * US_PER_SEC) < 0) {
        puts("Error");
        return 1;
    }

    puts("OK");

    return 0;
}

static int send_lines(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <command>\n", argv[0]);
        return 1;
    }

    ssize_t len;
    if ((len = at_send_cmd_get_lines(&at_dev, argv[1], resp, sizeof(resp), 10 * US_PER_SEC)) < 0) {
        puts("Error");
        return 1;
    }

    printf("Response (len=%d): %s\n", (int)len, resp);

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
    { "send_ok", "Send a command and wait OK", send_ok },
    { "send_lines", "Send a command and wait lines", send_lines },
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
