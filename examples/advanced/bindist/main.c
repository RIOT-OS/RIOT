/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Binary distribution example
 *
 * This application serves as simple example for "make bindist", a makefile
 * target that can be used to ship proprietary, compiled objects together
 * with a compiled binary in a way that allows re-linking and makes
 * verification possible.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

extern void abc(void);

int main(void)
{
    puts("Hello closed-source!");
    abc();

    return 0;
}
