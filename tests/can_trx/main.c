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
tja1042_trx_t tja1042 = { .trx.driver = &tja1042_driver,
                          .stb_pin = TJA1042_STB_PIN
};
#endif

#ifdef MODULE_NCV7356
#include "ncv7356.h"
ncv7356_trx_t ncv7356 ={ .trx.driver = &ncv7356_driver,
                         .mode0_pin = NCV7356_MODE0_PIN,
                         .mode1_pin = NCV7356_MODE1_PIN
};
#endif

static can_trx_t *devs[] = {
#ifdef MODULE_TJA1042
    (can_trx_t *)&tja1042,
#endif
#ifdef MODULE_NCV7356
    (can_trx_t *)&ncv7356,
#endif
    NULL,
};

static int init(int argc, char **argv) {

    if (argc < 2) {
        puts("usage: init [trx_id]");
        return 1;
    }

    unsigned trx = atoi(argv[1]);
    if (trx >= ARRAY_SIZE(devs)) {
        puts("Invalid trx_id");
        return 1;
    }

    int res = can_trx_init(devs[trx]);
    if (res < 0) {
        printf("Error when initializing trx: %d\n", res);
        return 1;
    }

    puts("Trx successfully initialized");
    return 0;
}

static int set_mode(int argc, char **argv) {

    if (argc < 3) {
        puts("usage: set_mode [trx_id] [mode]");
        puts("modes:");
        puts("\t0: normal mode");
        puts("\t1: silent mode");
        puts("\t2: standby mode");
        puts("\t3: high-speed mode (SW CAN only)");
        puts("\t4: high-voltage wakeup mode (SW CAN only)");
        return 1;
    }
    unsigned trx = atoi(argv[1]);
    unsigned mode = atoi(argv[2]);
    if ((trx >= ARRAY_SIZE(devs)) ||
            (mode > TRX_HIGH_VOLTAGE_WAKE_UP_MODE)) {
        puts("Invalid trx_id or mode");
        return 1;
    }

    int res = can_trx_set_mode(devs[trx], mode);
    if (res < 0) {
        printf("Error when setting mode: %d\n", res);
        return 1;
    }

    puts("Mode successfully set");
    return 0;
}

static const shell_command_t cmds[] = {
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
