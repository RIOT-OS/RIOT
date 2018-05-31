/*
 * Copyright (C) 2018 FU Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Test standard C functions for getting absolute time (using RTC)
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <time.h>

int main(void)
{
    while (1) {
        time_t t;

        puts("Current time: ");
        t = time(NULL);
        puts(ctime(&t));
        putchar('\n');

        getchar();
    }
    return 0;
}
