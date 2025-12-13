/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       MCUBoot compile test application
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "buildinfo/board.h"
#include "buildinfo/cpu.h"
#include "cpu.h"

int main(void)
{
    puts("Hello MCUBoot!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s CPU.\n", RIOT_CPU);
    printf("The startup address is: %p\n", (void*)SCB->VTOR);

    return 0;
}
