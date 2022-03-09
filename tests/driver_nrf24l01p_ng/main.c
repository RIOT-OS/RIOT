/*
 * Copyright (C) 2019 Otto-von-Guericke Universität Magdeburg
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
 * @brief       Test application for the NRF24L01+ transceiver
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */
#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "shell_commands.h"
#include "net/gnrc.h"

#define MAIN_QUEUE_SIZE     (8)

int sc_nrf24ctl(int argc, char *argv[]);

static const shell_command_t shell_commands[] = {
    { "nrf24ctl", "Configure an NRF24L01+ (NG) device", sc_nrf24ctl },
    { NULL, NULL, NULL }
};

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
