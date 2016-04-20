/*
 * Copyright (C) 2014 INRIA
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup chronos
 * @{
 */

/**
 * @file
 * @brief       eZ430-chronos getchar/putchar dummy implementation
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#include <stdio.h>

static void _dummy(int c)
{
}

void (*_putchar)(int c) = _dummy;

int putchar(int c)
{
    _putchar(c);
    return c;
}

int getchar(void)
{
    /* dummy implementation */
    return EOF;
}

ssize_t write(int fildes, const void *buf, size_t nbyte)
{
    return -1;
}
