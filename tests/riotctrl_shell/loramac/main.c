/*
 * SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Tests the basic shell commands always provided.
 *
 * @author      Martine S. Lenders <mlenders@riot-os.org>
 *
 * @note        IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
 *              YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mutex.h"
#include "net/netdev_test.h"
#include "semtech_loramac.h"
#include "shell.h"

static netdev_test_t _lora_dev;
semtech_loramac_t loramac;

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("Starting riotctrl_shell.loramac test");
    netdev_test_setup(&_lora_dev, NULL);
    loramac.netdev = &_lora_dev.netdev.netdev;
    semtech_loramac_init(&loramac);

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
