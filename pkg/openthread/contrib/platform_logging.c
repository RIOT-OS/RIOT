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
 * @}
 */

#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "openthread/platform/logging.h"

/* adapted from OpenThread posix example:
 * See: https://github.com/openthread/openthread/blob/master/examples/platforms/posix/logging.c */
__attribute__((__format__ (__printf__, 3, 4)))
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    va_list args;

    switch (aLogLevel) {
        case kLogLevelNone:
            fprintf(stderr, "NONE ");
            break;

        case kLogLevelCrit:
            fprintf(stderr, "CRIT ");
            break;

        case kLogLevelWarn:
            fprintf(stderr, "WARN ");
            break;

        case kLogLevelInfo:
            fprintf(stderr, "INFO ");
            break;

        case kLogLevelDebg:
            fprintf(stderr, "DEBG ");
            break;
    }

    switch (aLogRegion) {
        case kLogRegionApi:
            fprintf(stderr, "API  ");
            break;

        case kLogRegionMle:
            fprintf(stderr, "MLE  ");
            break;

        case kLogRegionArp:
            fprintf(stderr, "ARP  ");
            break;

        case kLogRegionNetData:
            fprintf(stderr, "NETD ");
            break;

        case kLogRegionIp6:
            fprintf(stderr, "IPV6 ");
            break;

        case kLogRegionIcmp:
            fprintf(stderr, "ICMP ");
            break;

        case kLogRegionMac:
            fprintf(stderr, "MAC  ");
            break;

        case kLogRegionMem:
            fprintf(stderr, "MEM  ");
            break;
        default:
            break;
    }

    va_start(args, aFormat);
    vfprintf(stderr, aFormat, args);
    fprintf(stderr, "\r");
    va_end(args);
}
