/*
 * Copyright (C) 2013 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief       shows how to set up own and use the system shell commands.
 *              By typing help in the serial console, all the supported commands
 *              are listed.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "shell_commands.h"
#include "nrf_radio.h"
#include "vtimer.h"

#define SHELL_BUFSIZE   (128U)

static char *default_msg = "Hello Other Node";
static char receive_buf[NRF_RADIO_BUFSIZE];


static void send(int argc, char **argv)
{
    uint8_t addr;
    char *data;
    int size;
    int res;

    if (argc < 2) {
        puts("usage: send addr [string to sent]");
        return;
    }

    addr = (uint8_t)atoi(argv[1]);
    if (argc < 3) {
        data = default_msg;
        size = strlen(default_msg);
    }
    else {
        data = argv[2];
        size = strlen(argv[2]);
    }

    printf("Sending %i byte to addr %i\n", size, (int)addr);
    res = nrf_radio_send(addr, data, size);

    if (res > 0) {
        printf("Successfully sent out %i byte\n", res);
    }
    else {
        printf("Sending failed, return code is %i\n", res);
    }
}

static void receive(int argc, char **argv)
{
    uint8_t addr;
    int res;

    if (argc < 2) {
        puts("usage: receive addr");
        return;
    }

    addr = (uint8_t)atoi(argv[1]);

    printf("Going into receive mode now, listening on address %i\n", addr);
    res = nrf_radio_receive(addr, receive_buf, NRF_RADIO_BUFSIZE);

    if (res > 0) {
        printf("Successfully received %i byte\n", res);
        for (int i = 0; i < res; i++) {
            printf("%c", receive_buf[i + 1]);
        }
        printf("\n");
    }
    else {
        printf("Receiving failed, return code is %i\n", res);
    }
}

static int shell_readc(void)
{
    return (int)getchar();
}

static void shell_putchar(int c)
{
    putchar(c);
}

static const shell_command_t shell_commands[] = {
    { "send", "send a packet", send },
    { "receive", "put node into receive mode", receive },
    { NULL, NULL, NULL }
};

int main(void)
{

    printf("NRF RADIO test application\n");

    /* initialize the radio */
    nrf_radio_init();

    /* run the shell */
    shell_t shell;
    shell_init(&shell, shell_commands, SHELL_BUFSIZE, shell_readc, shell_putchar);
    shell_run(&shell);

    return 0;
}
