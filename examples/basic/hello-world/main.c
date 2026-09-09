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

/* Do's */

/* positive: conditional compilation is indented after the `#` */
#ifndef FMT_TEST_SCALE
#  define FMT_TEST_SCALE 2
#endif

/* negative: the `#define` below misses the indent after the `#` */
#ifndef FMT_TEST_OFFSET
#define FMT_TEST_OFFSET 1
#endif

/* positive: this function is clang-format compliant */
static int _fmt_test_positive(int val)
{
    int *ptr = &val;

    switch (val) {
    case 0:
        return 0;
    default:
        break;
    }
    if (val < 0) {
        val = -val * FMT_TEST_SCALE;
    }
    else {
        val = (unsigned)val * FMT_TEST_SCALE;
    }
    return val + *ptr - (int)sizeof(int);
}

/* Don'ts */
static int _fmt_test_negative(int val) {
    int* ptr = &val;

#if FOO
#if BAR
#   define FOOBAR
#   endif
#       endif

    switch (val) {
        case 0:
            return 0;
        default:
            break;
    }
    /* negative: cuddled else, non-conforming assignment spacing, and a
     * single-statement `if` without braces */
    if (val < 0) {
        val = -val - FMT_TEST_OFFSET;
    } else {
        val=  0;
    }
    if (val > 100) val = 100;
    /* negative: the cast and the `sizeof` are followed by a space */
    return val + *ptr - (int) sizeof (int);
}

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s CPU.\n", RIOT_CPU);

    return 0;
}
