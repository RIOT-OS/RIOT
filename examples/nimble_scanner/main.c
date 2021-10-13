/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief       Example for using NimBLE as a BLE scanner
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timex.h"
#include "ztimer.h"
#include "shell.h"
#include "shell_commands.h"

#include "nimble_scanner.h"
#include "nimble_scanlist.h"

/* default scan duration (1s) */
#define DEFAULT_DURATION_MS        (1 * MS_PER_SEC)

int _cmd_scan(int argc, char **argv)
{
    uint32_t timeout = DEFAULT_DURATION_MS;

    if ((argc == 2) && (memcmp(argv[1], "help", 4) == 0)) {
        printf("usage: %s [timeout in ms]\n", argv[0]);
        return 0;
    }
    if (argc >= 2) {
        timeout = atoi(argv[1]);
    }

    nimble_scanlist_clear();
    printf("Scanning for %"PRIu32" ms now ...", timeout);
    nimble_scanner_set_scan_duration(timeout);
    nimble_scanner_start();
    ztimer_sleep(ZTIMER_MSEC, timeout);
    puts(" done\n\nResults:");
    nimble_scanlist_print();
    puts("");

    return 0;
}

static const shell_command_t _commands[] = {
    { "scan", "trigger a BLE scan", _cmd_scan },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("NimBLE Scanner Example Application");
    puts("Type `scan help` for more information");

    /* in this example, we want Nimble to scan 'full time', so we set the
     * window equal the interval */
    struct ble_gap_disc_params scan_params = {
        .itvl = BLE_GAP_LIM_DISC_SCAN_INT,
        .window = BLE_GAP_LIM_DISC_SCAN_WINDOW,
        .filter_policy = 0,                         /* don't use */
        .limited = 0,                               /* no limited discovery */
        .passive = 0,                               /* no passive scanning */
        . filter_duplicates = 0,                    /* no duplicate filtering */
    };

    /* initialize the nimble scanner */
    nimble_scanlist_init();
    nimble_scanner_init(&scan_params, nimble_scanlist_update);

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
