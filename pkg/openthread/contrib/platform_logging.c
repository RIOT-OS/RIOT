/*
 * SPDX-FileCopyrightText: 2017 Fundacion Inria Chile
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread logging platform abstraction
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @}
 */

#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "openthread/platform/logging.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* adapted from OpenThread posix example:
 * See: https://github.com/openthread/openthread/blob/master/examples/platforms/posix/logging.c */
__attribute__((__format__(__printf__, 3, 4)))
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    (void)aLogLevel;
    (void)aLogRegion;
    va_list args;
    va_start(args, aFormat);
    /*stderr is not redirected to make term*/
    vprintf(aFormat, args);
    printf("\n");
    va_end(args);
}

static const char *_log_level_as_str[] = {
    "NONE",
    "CRIT",
    "WARN",
    "NOTE",
    "INFO",
    "DEBG"
};

void otPlatLogHandleLevelChanged(otLogLevel aLogLevel)
{
    DEBUG("openthread: log level changed to %s", _log_level_as_str[aLogLevel]);
}
