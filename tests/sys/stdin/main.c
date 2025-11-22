/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Shows that stdin module is required when one wants to use getchar
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#include <stdio.h>

int main(void)
{
    while (1) {
        int value = getchar();
        printf("You entered '%c'\n", (char)value);
    }

    return 0;
}
