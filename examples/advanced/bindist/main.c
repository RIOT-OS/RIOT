/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
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
