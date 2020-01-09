/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
