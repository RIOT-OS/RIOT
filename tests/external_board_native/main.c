/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief       External Native application
 *
 * @author      Gaëtan Harter <gaetan.harter@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "external_native.h"

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("THIS_BOARD_IS %s\n", THIS_BOARD_IS);
    printf("This board is '%s'\n", external_native_board_description);

    return 0;
}
