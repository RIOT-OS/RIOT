/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Init Test
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 */
#include <stdio.h>
#include <inttypes.h>

#include "shell_commands.h"
#include "shell.h"
#include "init.h"

volatile static uint32_t init_pattern;

int core_init_test(void)
{
    init_pattern |= (1 << INIT_ORDER_CORE);
    return 0;
}

int postcore_init_test(void)
{
    init_pattern |= (1 << INIT_ORDER_POSTCORE);
    return 0;
}

int peripheral_init_test(void)
{
    puts("Init: peripheral_init_test");
    init_pattern |= (1 << INIT_ORDER_PERIPHERAL);
    return 0;
}

int board_init_test(void)
{
    puts("Init: board_init_test");
    init_pattern |= (1 << INIT_ORDER_BOARD);
    return 0;
}

int subsys_init_test(void)
{
    puts("Init: subsys_init_test");
    init_pattern |= (1 << INIT_ORDER_SUBSYS);
    return 0;
}

int system_init_test(void)
{
    puts("Init: system_init_test");
    init_pattern |= (1 << INIT_ORDER_SYSTEM);
    return 0;
}

int device_init_test(void)
{
    puts("Init: device_init_test");
    init_pattern |= (1 << INIT_ORDER_DEVICE);
    return 0;
}

int liftoff_init_test(void)
{
    puts("Init: liftoff_init_test");
    init_pattern |= (1 << INIT_ORDER_LIFTOFF);
    return 0;
}

core_init(core_init_test);
postcore_init(postcore_init_test);
peripheral_init(peripheral_init_test);
deck_init(board_init_test);
subsys_init(subsys_init_test);
system_init(system_init_test);
device_init(device_init_test);
liftoff_init(liftoff_init_test);

int main(void)
{
    puts("RIOT init test application");

    printf("init_pattern: 0x%08" PRIx32 "\n", init_pattern);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
