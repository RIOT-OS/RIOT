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

#include "cpu.h"

int main(void)
{
    puts("Hello MCUBoot!");

    printf("You are running RIOT on a(n) %s board.\n", buildinfo_board_name);
    printf("This board features a(n) %s CPU.\n", buildinfo_cpu_name);
    printf("The startup address is: %p\n", (void*)SCB->VTOR);

    return 0;
}
