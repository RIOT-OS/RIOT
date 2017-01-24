/*
 * Copyright (C) 2016 Freie Universität Berlin
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
 * @brief       Test the Calliope mini support library
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "timex.h"
#include "mini.h"

#define DELAY           (120 * US_PER_MS)

int main(void)
{
    puts("Welcome to RIOT!\n");
    puts("Please refer to the README.md for more information about this app\n");

    mini_matrix_init();

    while (1) {
        mini_matrix_shift_str("Welcome RIOT @ Calliope mini!", DELAY);
    }

    return 0;
}
