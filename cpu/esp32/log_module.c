/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Log module to realize consistent log messages
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdio.h>
#include <string.h>

#include "rom/ets_sys.h"

#include "cpu_conf.h"
#include "log.h"
#include "syscalls.h"

extern char _printf_buf[PRINTF_BUFSIZ];
bool _new_line = true;

void log_write(unsigned level, const char *format, ...)
{
    if (level == LOG_NONE) {
        return;
    }

    if (_new_line) {
        /* if we are in new line, we print the prefix */
        char lc = 'U';
        switch (level) {
            case LOG_ERROR  : lc = 'E'; break;
            case LOG_WARNING: lc = 'W'; break;
            case LOG_INFO   : lc = 'I'; break;
            case LOG_DEBUG  : lc = 'D'; break;
            case LOG_ALL    : lc = 'V'; break;
        }
        ets_printf("%c (%u) ", lc, system_get_time_ms());
    }

    va_list arglist;
    va_start(arglist, format);

    int ret = vsnprintf(_printf_buf, PRINTF_BUFSIZ, format, arglist);

    if (ret > 0) {
        ets_printf (_printf_buf);
    }

    va_end(arglist);

    _new_line = (strrchr(format, '\n') != NULL);
}

void log_write_tagged(unsigned level, const char *tag, const char *format, ...)
{
    if (level == LOG_NONE) {
        return;
    }

    if (_new_line) {
        /* if we are in new line, we print the prefix */
        char lc = 'U';
        switch (level) {
            case LOG_ERROR  : lc = 'E'; break;
            case LOG_WARNING: lc = 'W'; break;
            case LOG_INFO   : lc = 'I'; break;
            case LOG_DEBUG  : lc = 'D'; break;
            case LOG_ALL    : lc = 'V'; break;
        }
        #if LOG_TAG_IN_BRACKETS
        ets_printf("%c (%u) [%10s]: ", lc, system_get_time_ms(), tag);
        #else
        ets_printf("%c (%u) %10s: ", lc, system_get_time_ms(), tag);
        #endif
    }

    va_list arglist;
    va_start(arglist, format);

    int ret = vsnprintf(_printf_buf, PRINTF_BUFSIZ, format, arglist);

    if (ret > 0) {
        ets_printf (_printf_buf);
    }

    va_end(arglist);

    _new_line = (strrchr(format, '\n') != NULL);
}
