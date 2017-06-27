/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       ssp test application
 *
 * This test should crash badly when *not* using the ssp module, and panic if
 * using it.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

void test_func(void)
{
    char buf[16];

    /* cppcheck-suppress bufferAccessOutOfBounds
     * (reason: deliberately overflowing stack) */
    memset(buf, 0, 32);
}

int main(void)
{
    puts("calling stack corruption function");

    test_func();

    puts("back to main");

    return 0;
}
