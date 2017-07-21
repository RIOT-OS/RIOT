/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
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
 * @brief       Bootloader test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "firmware.h"
#include "cpu.h"

int main(void)
{
    int slot = firmware_current_slot();
    puts("Hello Bootloader!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    if (slot != -1) {
        printf("Active firmware slot: %i\n", slot);
        firmware_dump_slot_addrs();
    }
    else {
        puts("TEST FAILED!\n");
    }


    return 0;
}
