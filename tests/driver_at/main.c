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
#include <string.h>

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
    if ((len = at_send_cmd_get_lines(&at_dev, argv[1], resp, sizeof(resp), true, 10 * US_PER_SEC)) < 0) {
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

static int power_on(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    at_dev_poweron(&at_dev);

    puts("Powered on");

    return 0;
}

static int power_off(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    at_dev_poweroff(&at_dev);

    puts("Powered off");

    return 0;
}

#ifdef MODULE_AT_URC
#ifndef MAX_URC_NB
#define MAX_URC_NB  5
#endif

#ifndef MAX_URC_LEN
#define MAX_URC_LEN 32
#endif

static at_urc_t urc_list[MAX_URC_NB];
static char urc_str[MAX_URC_NB][MAX_URC_LEN];
static bool urc_used[MAX_URC_NB];

static void _urc_cb(void *arg, const char *urc)
{
    (void)arg;
    printf("urc received: %s\n", urc);
}

static int add_urc(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <urc>\n", argv[0]);
        return 1;
    }

    if (strlen(argv[1]) > MAX_URC_LEN - 1) {
        puts("urc is too long");
        return 1;
    }

    for (size_t i = 0; i < MAX_URC_NB; i++) {
        if (!urc_used[i]) {
            strcpy(urc_str[i], argv[1]);
            urc_list[i].code = urc_str[i];
            urc_list[i].arg = NULL;
            urc_list[i].cb = _urc_cb;
            urc_used[i] = true;
            at_add_urc(&at_dev, &urc_list[i]);
            puts("urc registered");
            return 0;
        }
    }

    puts("Not enough memory, urc is not registered");
    return 1;
}

static int process_urc(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <timeout>\n", argv[0]);
        return 1;
    }

    uint32_t timeout = strtoul(argv[1], NULL, 0);
    at_process_urc(&at_dev, timeout);

    puts("urc processed");

    return 0;
}

static int remove_urc(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <urc>\n", argv[0]);
        return 1;
    }

    for (size_t i = 0; i < MAX_URC_NB; i++) {
        if (urc_used[i] && strcmp(urc_list[i].code, argv[1]) == 0) {
            at_remove_urc(&at_dev, &urc_list[i]);
            urc_used[i] = false;
            puts("urc removed");
            return 0;
        }
    }

    puts("urc not found");
    return 1;
}
#endif

static const shell_command_t shell_commands[] = {
    { "init", "Initialize AT device", init },
    { "send", "Send a command and wait response", send },
    { "send_ok", "Send a command and wait OK", send_ok },
    { "send_lines", "Send a command and wait lines", send_lines },
    { "drain", "Drain AT device", drain },
    { "power_on", "Power on AT device", power_on },
    { "power_off", "Power off AT device", power_off },
#ifdef MODULE_AT_URC
    { "add_urc", "Register an URC", add_urc },
    { "remove_urc", "De-register an URC", remove_urc },
    { "process_urc", "Process the URCs", process_urc },
#endif
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
