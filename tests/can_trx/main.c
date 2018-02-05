/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       test a CAN transceiver through can_trx interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "can/can_trx.h"

#include "shell.h"

#ifdef MODULE_TJA1042
#include "tja1042.h"
static can_trx_t tja1042 = {
        .driver = &tja1042_driver,
};
#endif

static can_trx_t *devs[] = {
#ifdef MODULE_TJA1042
    &tja1042,
#endif
    NULL,
};

static int help(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    puts("Help:");
    puts("\tinit [trx_id] - initialize a trx");
    puts("\tset_mode [trx_id] [mode] - set a mode on the trx");
    printf("trx_id: 0..%u\n", (unsigned)(sizeof(devs) / sizeof(devs[0])));
    puts("modes:");
    puts("\t0: normal mode");
    puts("\t1: silent mode");
    puts("\t2: standby mode");
    puts("\t3: high-speed mode (SW CAN only)");
    puts("\t4: high-voltage wakeup mode (SW CAN only)");

    return 0;
}

static int init(int argc, char **argv) {

    if (argc < 2) {
        puts("trx_id needed");
        help(0, NULL);
        return 1;
    }

    unsigned trx = atoi(argv[1]);
    if (trx < (sizeof(devs) / sizeof(devs[0]))) {
        int res = can_trx_init(devs[trx]);
        if (res >= 0) {
            puts("Trx successfully initialized");
            return 0;
        }
        else {
            printf("Error when initializing trx: %d\n", res);
        }
    }
    else {
        puts("Invalid trx_id");
    }

    return 1;
}

static int set_mode(int argc, char **argv) {

    if (argc < 3) {
        puts("trx_id and mode needed");
        help(0, NULL);
        return 1;
    }
    unsigned trx = atoi(argv[1]);
    unsigned mode = atoi(argv[2]);
    if ((trx < (sizeof(devs) / sizeof(devs[0]))) &&
            (mode <= TRX_HIGH_VOLTAGE_WAKE_UP_MODE)) {
        int res = can_trx_set_mode(devs[trx], mode);
        if (res >= 0) {
            puts("Mode successfully set");
            return 0;
        }
        else {
            printf("Error when setting mode: %d\n", res);
        }
    }
    else {
        puts("Invalid trx_id or mode");
    }

    return 1;
}

static const shell_command_t cmds[] = {
        { "help", "help", help },
        { "init", "initialize a can trx", init },
        { "set_mode", "set a can trx mode", set_mode },
        { NULL, NULL, NULL },
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(cmds, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
