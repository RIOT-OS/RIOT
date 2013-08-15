/*
 * microeval.c - Logging of the secure routing.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */


/*
 * @file        microeval.c
 * @brief       Logging of the secure routing.
 *              It enables the logging during the secure routing process.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics.
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @date        21.09.2013 18:04:23
 *
 **/


#include "microeval.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

// Specifies how deep in the function calling heirachy we are
int print_level = 1;    // The function call depth to be printed
int flag_have_newline;  // says if last print had a newline

char spacing[30];

void ulogdf_impl_simple(char *format, ...)
{
    // mutex_lock(&print_mutex);

    if (!log_enabled) {
        return;
    }

    char out[200];

    va_list args;
    va_start(args, format);
    vsnprintf(out, 200, format, args);
    va_end(args);

    printf(out);
    printf("\n");
    fflush(stdout);

    // mutex_unlock(&print_mutex, true);
}

void ulogdf_direct(char *s, ...)
{
    va_list arg;
    va_start(arg, s);
    vprintf(s, arg);
    va_end(arg);

    fflush(stdout);

    flag_have_newline = 0;
}


void ulogdf_impl(char *s, int log_level, char *filename, int line,
                 const char *functionname, ...)
{
    if (flag_have_newline == 0) {
        printf("\n");
    }

    char *short_file_name = strrchr(filename, '/') + 1;

    char msgbuffer[200];
    sprintf(msgbuffer, "%s# %-50s      @ln%d in %s -- function %s",
            spacing, s, line, short_file_name, functionname);

    va_list args;
    va_start(args, functionname);
    vprintf(msgbuffer, args);
    va_end(args);

    fflush(stdout);

    flag_have_newline = 0;
}

void ulog_result(int res)
{
}



