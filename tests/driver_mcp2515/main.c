/*
 * Copyright (C) 2016 OTA keys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the MCP2515 can driver
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 *
 * @}
 */

#ifndef TEST_MCP2515_SPI
#error "TEST_MCP2515_SPI not defined"
#endif
#ifndef TEST_MCP2515_CS
#error "TEST_MCP2515_CS not defined"
#endif
#ifndef TEST_MCP2515_RESET
#error "TEST_MCP2515_RESET not defined"
#endif
#ifndef TEST_MCP2515_MODE
#error "TEST_MCP2515_MODE not defined"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "candev_mcp2515.h"

static candev_mcp2515_t dev;
struct can_bittiming timing;

static const candev_mcp2515_conf_t conf = {
    .spi = SPI_1,
    .cs_pin = CAN2_SS_PIN,
    .rst_pin = CAN2_RST_PIN,
    .int_pin = CAN2_INT_PIN,
};


static const shell_command_t shell_commands[] = {
    { "send", "send some data", _send },
    { "receive", "receive some data", _receive },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("MCP2515 can driver test application\n");
    printf("Initializing MCP2515 at SPI_%i... ", TEST_MCP2515_SPI);
    if (candev_mcp2515_init(&dev, &timing, &conf) != 0) {
        puts("Failed to initialize MCP2515 driver");
        return 1;
    }

    /* run shell */
    puts("All OK, running shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
