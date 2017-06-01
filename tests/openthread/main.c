/*
 * Copyright (C) 2017 Baptiste CLENET
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       OpenThread test application
 *
 * @author      Baptiste Clenet <baptiste.clenet@xsoen.com>
 */

#include <stdio.h>

#include "net/ipv6/addr.h"
#include "openthread/ip6.h"
#include "openthread/thread.h"
#include "openthread/udp.h"
#include "ot.h"
#include "shell.h"
#include "shell_commands.h"
int main(void)
{
    printf("Get PANID\n");
    uint16_t panid = 0;
    uint8_t res = ot_call_command("panid", NULL, (void*)&panid);
    printf("Current panid: 0x%x (res:%x)\n", panid, res);

    openthread_uart_run();
    return 0;
}
