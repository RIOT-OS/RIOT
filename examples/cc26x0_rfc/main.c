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
void rfc_setup_ble(void);
void rfc_beacon(void);

int main(void)
{
    puts("Hello World!");
    rfc_prepare();
    rfc_setup_ble();

    uint32_t cnt;
    while (1) {
        for (cnt = 0; cnt < 100000; cnt++) {
            asm volatile("nop\n");
            __asm__ volatile("nop\n");
        }

        rfc_beacon();

        LED1_TOGGLE;
    }

    return 0;
}
