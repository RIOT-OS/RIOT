/*
 * Copyright (C) 2020 Thomas Stilwell <stilwellt@openlabs.co>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup examples
 * @{
 *
 * @file
 * @brief   BLE rshell application
 *
 * @author  Thomas Stilwell <stilwellt@openlabs.co>
 *
 * @}
 */

#include "ble_rshell.h"
#include "shell.h"

static const char *device_name = "NimBLE rshell on RIOT-OS";

int main(void)
{
    int rc;

    puts("NimBLE rshell example");

    ble_rshell_t bleshell;
    bleshell.device_name = device_name;
    bleshell.shell_commands = NULL;

    /* run the BLE shell */
    rc = ble_rshell_init(&bleshell);
    assert(!rc);

    /* run the local shell - this never returns */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run_forever(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
