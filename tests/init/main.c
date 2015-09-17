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

int driver_init_test(void)
{
    init_pattern |= (1 << INIT_ORDER_DRIVER);
    return 0;
}

int module_init_test(void)
{
    init_pattern |= (1 << INIT_ORDER_MODULE);
    return 0;
}

int submod_init_test(void)
{
    init_pattern |= (1 << INIT_ORDER_SUBMOD);
    return 0;
}

core_init(core_init_test);
driver_init(driver_init_test);
module_init(module_init_test);
submod_init(submod_init_test);

int main(void)
{
    puts("RIOT init test application");

    printf("init_pattern: 0x%08" PRIx32 "\n", init_pattern);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
