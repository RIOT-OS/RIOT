/*
 * Copyright (C) 2022 Koen Zandberg
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
 * @brief       Test application for the matrix_keypad driver
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>

#include "matrix_keypad.h"
#include "matrix_keypad_params.h"
#include "ztimer.h"

void _callback(void *arg, size_t col, size_t row, bool state)
{
    (void)arg;
    printf("Key switch at column %" PRIuSIZE " and row %" PRIuSIZE " is ", col, row);
    if (state) {
        puts("pressed!");
    }
    else {
        puts("released!");
    }
}

int main(void)
{
    matrix_keypad_t dev;

    puts("Generated RIOT application: 'matrix_keypad'");
    if (matrix_keypad_init(&dev, &matrix_keypad_params[0], _callback, NULL) == 0) {
        puts("[OK]\n");
    }

    while (true) {
        matrix_keypad_scan(&dev);
        ztimer_sleep(ZTIMER_MSEC, 1);
    }
    return 0;
}
