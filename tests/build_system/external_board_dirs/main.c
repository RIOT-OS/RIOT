/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", buildinfo_board_name);
    printf("This board features a(n) %s CPU.\n", buildinfo_cpu_name);

    return 0;
}
