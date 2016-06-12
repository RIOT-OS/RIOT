/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       xtimer_now64 continuity test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "xtimer.h"
#include "fmt.h"
#include "irq.h"

int main(void)
{
    uint32_t before = xtimer_now();
    uint32_t now;

    print_u32_hex(XTIMER_MSBMASK);
    print_u32_hex(XTIMER_MSBMASK);
    puts("");

    do {
        now = xtimer_now();
        irq_disable();
        print_u32_hex(now);
        puts("");
        if (now < before) {
            puts("TEST FAILED.");
            break;
        }
        before = now;
        irq_enable();
    } while(1);

    return 0;
}
