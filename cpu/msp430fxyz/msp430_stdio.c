/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430fxyz
 * @{
 *
 * @file
 * @brief       Implementation of getchar and putchar for MSP430 CPUs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "uart_stdio.h"

/**
 * @brief   Get one character from STDIO - used by the libc
 */
int getchar(void)
{
    char c;
    uart_stdio_read(&c, 1);
    return c;
}

/**
 * @brief   Write one character to the STDIO UART interface - used by e.g.
 *          printf and puts
 */
int putchar(int c)
{
    char _c = c;
    uart_stdio_write(&_c, 1);
    return 1;
}
