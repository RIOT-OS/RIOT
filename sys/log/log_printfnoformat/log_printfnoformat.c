/*
 * Copyright 2015 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     core_log
 * @{
 *
 * @file
 * @brief       logging implementation that only prints the first argument
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdarg.h>

#include "log.h"

void log_info(const char *format, ...)
{
    putchar('I');
    putchar(':');
    putchar(' ');
    puts(format);
}

void log_warning(const char *format, ...)
{
    putchar('W');
    putchar(':');
    putchar(' ');
    puts(format);
}

void log_error(const char *format, ...)
{
    putchar('E');
    putchar(':');
    putchar(' ');
    puts(format);
}
