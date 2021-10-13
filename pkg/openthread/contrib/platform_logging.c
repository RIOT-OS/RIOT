/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

/* adapted from OpenThread posix example:
 * See: https://github.com/openthread/openthread/blob/master/examples/platforms/posix/logging.c */
__attribute__((__format__ (__printf__, 3, 4)))
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    (void) aLogLevel;
    (void) aLogRegion;
    va_list args;
    va_start(args, aFormat);
    vfprintf(stderr, aFormat, args);
    fprintf(stderr, "\n");
    va_end(args);
}
