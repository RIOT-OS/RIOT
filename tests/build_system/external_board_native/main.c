/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       External Native application
 *
 * @author      Gaëtan Harter <gaetan.harter@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "buildinfo/board.h"
#include "external_native.h"

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("THIS_BOARD_IS %s\n", THIS_BOARD_IS);
    printf("This board is '%s'\n", external_native_board_description);

    return 0;
}
