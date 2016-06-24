/*
 * Copyright (C) 2016 Leon George
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
 * @brief       Example for using the radio unit of the cc26x0
 *
 * @author      Leon M. George <leon@georgemail.eu>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"

/* TODO derive an interface when there is enough to derive from */
void rfc_prepare(void);
bool rfc_ping_test(void);
bool rfc_nop_test(void);
bool rfc_setup_ble(void);
void rfc_ble_beacon(void);

int main(void)
{
    puts("Hello World!");
    rfc_prepare();

    if (!rfc_ping_test()) {
        fprintf(stderr, "rfc_ping_test failed\n");
        return 1;
    }

    if (!rfc_nop_test()) {
        fprintf(stderr, "rfc_nop_test failed\n");
        return 1;
    }

    if (!rfc_setup_ble()) {
        fprintf(stderr, "rfc_setup_ble failed\n");
        return 1;
    }

    uint32_t cnt;
    while (1) {
        for (cnt = 0; cnt < 1000000; cnt++) {
            asm volatile("nop\n");
            __asm__ volatile("nop\n");
        }

        rfc_ble_beacon();

        LED1_TOGGLE;
    }

    return 0;
}
