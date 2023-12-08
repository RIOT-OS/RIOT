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

#include <stdbool.h>
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
static char rp_buf[256];
static bool initialized = false;
static bool is_power_on = false;

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

    if (uart >= UART_NUMOF) {
        printf("Wrong UART device number - should be in range 0-%d.\n", UART_NUMOF - 1);
        return 1;
    }
    at_dev_init_t at_init_params = {
        .baudrate = baudrate,
        .rp_buf = rp_buf,
        .rp_buf_size = sizeof(rp_buf),
        .rx_buf = buf,
        .rx_buf_size = sizeof(buf),
        .uart = UART_DEV(uart),
    };
    int res = at_dev_init(&at_dev, &at_init_params);
    /* check the UART initialization return value and respond as needed */
    if (res == UART_NODEV) {
        puts("Invalid UART device given!");
        return 1;
    }
    else if (res == UART_NOBAUD) {
        puts("Baudrate is not applicable!");
        return 1;
    }

    initialized = true;

    return res;
}

static int send(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <command>\n", argv[0]);
        return 1;
    }

    if (initialized == false) {
        puts("Error: AT device not initialized.\n");
        puts("Execute init function first.\n");
        return 1;
    }

    if (is_power_on == false) {
        puts("Error: AT device not power on.\n");
        puts("Execute power_on function first.\n");
        return 1;
    }

    ssize_t len;
    if ((len = at_send_cmd_get_resp(&at_dev, argv[1], resp, sizeof(resp), 10 * US_PER_SEC)) < 0) {
        puts("Error");
        return 1;
    }

    printf("Response (len=%" PRIdSIZE "): %s\n", len, resp);

    return 0;
}

static int send_ok(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <command>\n", argv[0]);
        return 1;
    }

    if (initialized == false) {
        puts("Error: AT device not initialized.\n");
        puts("Execute init function first.\n");
        return 1;
    }

    if (is_power_on == false) {
        puts("Error: AT device not power on.\n");
        puts("Execute power_on function first.\n");
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

    if (initialized == false) {
        puts("Error: AT device not initialized.\n");
        puts("Execute init function first.\n");
        return 1;
    }

    if (is_power_on == false) {
        puts("Error: AT device not power on.\n");
        puts("Execute power_on function first.\n");
        return 1;
    }

    ssize_t len;
    if ((len = at_send_cmd_get_lines(&at_dev, argv[1], resp, sizeof(resp),
                                         10 * US_PER_SEC)) < 0) {
        puts("Error");
        return 1;
    }

    printf("Response (len=%" PRIdSIZE "): %s\n", len, resp);

    return 0;
}

static int send_recv_bytes(int argc, char **argv)
{
    char buffer[64];

    if (argc < 3) {
        printf("Usage: %s <command> <number of bytes>\n", argv[0]);
        return 1;
    }

    if (initialized == false) {
        puts("Error: AT device not initialized.\n");
        puts("Execute init function first.\n");
        return 1;
    }

    if (is_power_on == false) {
        puts("Error: AT device not power on.\n");
        puts("Execute power_on function first.\n");
        return 1;
    }

    sprintf(buffer, "%s%s", argv[1], CONFIG_AT_SEND_EOL);
    at_send_bytes(&at_dev, buffer, strlen(buffer));

    ssize_t len = at_recv_bytes(&at_dev, buffer, atoi(argv[2]), 10 * US_PER_SEC);

    printf("Response (len=%" PRIdSIZE "): %s\n", len, buffer);

    return 0;
}

static int send_recv_bytes_until_string(int argc, char **argv)
{
    char buffer[128];
    size_t len = sizeof(buffer);

    memset(buffer, 0, sizeof(buffer));

    if (argc < 3) {
        printf("Usage: %s <command> <string to expect>\n", argv[0]);
        return 1;
    }

    if (initialized == false) {
        puts("Error: AT device not initialized.\n");
        puts("Execute init function first.\n");
        return 1;
    }

    if (is_power_on == false) {
        puts("Error: AT device not power on.\n");
        puts("Execute power_on function first.\n");
        return 1;
    }

    sprintf(buffer, "%s%s", argv[1], CONFIG_AT_SEND_EOL);
    at_send_bytes(&at_dev, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));

    int res = at_recv_bytes_until_string(&at_dev, argv[2], buffer, &len,
                                         10 * US_PER_SEC);

    if (res) {
        puts("Error");
        return 1;
    }

    printf("Response (len=%" PRIdSIZE "): %s\n", len, buffer);
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
    is_power_on = true;

    puts("Powered on");

    return 0;
}

static int power_off(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    at_dev_poweroff(&at_dev);
    is_power_on = false;

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

static int emulate_dce(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int res = 0;
    char resp_buf[128];

#ifdef MODULE_AT_URC
    at_urc_t urc = {.cb = _urc_cb, .code = "+CSCON"};
    at_add_urc(&at_dev, &urc);
#endif

    res = at_send_cmd_wait_ok(&at_dev, "AT+CFUN=1", US_PER_SEC);
    if (res) {
        printf("%u: Error AT+CFUN=1: %d\n", __LINE__, res);
        res = 1;
        goto exit;
    }

    res = at_send_cmd(&at_dev, "AT+CFUN=1", US_PER_SEC);
    if (res) {
        printf("%u: Error AT+CFUN=1: %d\n", __LINE__, res);
        res = 1;
        goto exit;
    }
    res = at_wait_ok(&at_dev, US_PER_SEC);
    if (res) {
        printf("%u: Error AT+CFUN=1: %d\n", __LINE__, res);
        res = 1;
        goto exit;
    }

    res = at_send_cmd_get_resp_wait_ok(&at_dev, "AT+CEREG?",
                                    "+CEREG:", resp_buf,
                                    sizeof(resp_buf), US_PER_SEC);
    if (res < 0) {
        printf("%u: Error AT+CEREG?: %d\n", __LINE__, res);
        res = 1;
        goto exit;
    }

    res = at_send_cmd_wait_prompt(&at_dev, "AT+USECMNG=0,0,\"cert\",128", US_PER_SEC);
    if (res) {
        printf("%u: Error AT+USECMNG: %d\n", __LINE__, res);
        res =  1;
        goto exit;
    }

    res = at_send_cmd_wait_prompt(&at_dev, "AT+PROMPTERROR", US_PER_SEC);
    if (res != -AT_ERR_EXTENDED) {
        printf("%u: Error AT+PROMPTERROR: %d\n", __LINE__, res);
        res =  1;
        goto exit;
    }
    res = atol(at_get_err_info(&at_dev));
    if (res != 1984) {
        printf("%u: Error AT+PROMPTERROR: %d\n", __LINE__, res);
        res =  1;
        goto exit;
    }

    res = at_send_cmd_wait_ok(&at_dev, "AT+CFUN=8", US_PER_SEC);
    if (res != -1) {
        printf("%u: Error AT+CFUN=8: %d\n", __LINE__, res);
        res =  1;
        goto exit;
    }

    res = at_send_cmd_wait_ok(&at_dev, "AT+CFUN=9", US_PER_SEC);
    if (res != -AT_ERR_EXTENDED) {
        printf("%u: Error AT+CFUN=9: %d\n", __LINE__, res);
        res =  1;
        goto exit;
    }

    res = atol(at_get_err_info(&at_dev));
    if (res != 666) {
        printf("%u: Error AT+CFUN=9: %d\n", __LINE__, res);
        res = 1;
        goto exit;
    }

    res = at_send_cmd_get_lines(&at_dev, "AT+GETTWOLINES", resp_buf,
                                sizeof(resp_buf), US_PER_SEC);
    if (res < 0) {
        printf("%u: Error AT+GETTWOLINES: %d\n", __LINE__, res);
        res = 1;
        goto exit;
    }

    if (strcmp(resp_buf, "first_line\nsecond_line\nOK")) {
        printf("%u: Error AT+GETTWOLINES: response not matching\n", __LINE__);
        res = 1;
        goto exit;
    }

    res = 0;
exit:
#ifdef MODULE_AT_URC
    at_remove_urc(&at_dev, &urc);
#endif
    printf("%s finished with %d\n", __func__, res);
    return res;
}

static const shell_command_t shell_commands[] = {
    { "init", "Initialize AT device", init },
    { "send", "Send a command and wait response", send },
    { "send_ok", "Send a command and wait OK", send_ok },
    { "send_lines", "Send a command and wait lines", send_lines },
    { "send_recv_bytes", "Send a command and wait response as raw bytes", send_recv_bytes },
    { "send_recv_until_string", "Send a command and receive response until "
      "the expected pattern arrives", send_recv_bytes_until_string },
    { "drain", "Drain AT device", drain },
    { "power_on", "Power on AT device", power_on },
    { "power_off", "Power off AT device", power_off },
    { "emulate_dce", "Test against the DCE emulation script.", emulate_dce},
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
