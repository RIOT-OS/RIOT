/*
 * Copyright (C) 2014 INRIA
 *
 * The source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @ingroup chronos
 * @{
 */

/**
 * @file
 * @brief       eZ430-chronos putchar dummy implementation
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

static void _dummy(int c)
{
}

void (*_putchar)(int c) = _dummy;

int putchar(int c)
{
    _putchar(c);
    return c;
}
