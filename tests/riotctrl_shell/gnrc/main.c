/*
 * SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Tests the GNRC shell commands.
 *
 * @author      Martine S. Lenders <mlenders@riot-os.org>
 *
 * @note        IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
 *              YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
 *              See https://github.com/RIOT-OS/Release-Specs/
 */

#include <stdio.h>

#include "container.h"
#include "msg.h"
#include "netdev_mock.h"
#include "shell.h"

int main(void)
{
    static msg_t msg_queue[8];
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("Starting riotctrl_shell.gnrc and riotctrl_shell.netif test");

    msg_init_queue(msg_queue, ARRAY_SIZE(msg_queue));

    netdev_mock_init_eth();
    netdev_mock_init_ieee802154();
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
