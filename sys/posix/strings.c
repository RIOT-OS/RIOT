/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file    strings.c
 * @brief   Providing implementation for prototypes defined in strings.h.
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 */
#include <strings.h>
#include <ctype.h>

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    while (n-- && tolower((unsigned char) *s1) == tolower((unsigned char) *s2)) {
        if (!n && !*s1) {
            break;
        }

        s1++;
        s2++;
    }

    return (tolower((unsigned char) *s1) - tolower((unsigned char) *s2));
}

/**
 * @}
 */
