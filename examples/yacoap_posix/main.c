/*
 * Copyright (C) 2016 smlng <s@mlng.net>
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
 * @brief       YaCoAP example showing CoAP client and server functions
 *
 * @author      smlng <s@mlng.net>
 *
 * @}
 */

// standard
 #include <inttypes.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
// riot
#include "board.h"
#include "periph/gpio.h"
#include "shell.h"

int coap_pid = -1;

extern int coap_start_thread(void);
extern int coap_get(const char *ip);
extern int coap_put(const char *ip, const char *path, const char *content);

static int cmd_get(int argc, char **argv);
static int cmd_put(int argc, char **argv);

// array with available shell commands
static const shell_command_t shell_commands[] = {
    { "get", "get ./well-known/core", cmd_get },
    { "put", "modify a resource",  cmd_put },
    { NULL, NULL, NULL }
};

/**
 * @brief the main programm loop
 *
 * @return non zero on error
 */
int main(void)
{
    // some initial infos
    puts(" YaCoAP example - Yet another CoAP library");
    puts("==========================================");
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);
    puts("==========================================");
    LED0_ON;
    // start coap receiver
    puts(". init coap");
    if ((coap_pid = coap_start_thread()) < 0) {
        puts("failed to init coap thread!");
        return 1;
    }
    puts(":");
    // start shell
    puts(". init shell");
    puts(":");
    LED0_OFF;
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    // should be never reached
    return 0;
}

int cmd_get(int argc, char **argv)
{
    if (argc != 2) {
        puts ("USAGE: get <ip>");
        return 1;
    }
    coap_get(argv[1]);
    return 0;
}

int cmd_put(int argc, char **argv)
{
    if (argc != 4) {
        puts("USAGE: put <ip> <path> <content>");
        return 1;
    }
    coap_put(argv[1], argv[2], argv[3]);
    return 0;
}
