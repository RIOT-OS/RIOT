/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdbool.h>

#include "utilities.h"

size_t hex2ints(uint8_t *out, const char *in)
{
    bool upper = true;
    size_t out_size = 0;

    while (*in != '\0') {
        char c;
        if ((*in >= '0') && (*in <= '9')) {
            c = '0';
        }
        else if ((*in >= 'a') && (*in <= 'f')) {
            c = 'a' - 10;
        }
        else if ((*in >= 'A') && (*in <= 'F')) {
            c = 'A' - 10;
        }
        else {
            in++;
            continue;
        }
        if (upper) {
            *out = (char)(*in - c) << 4;
        }
        else {
            *out |= (char)(*in - c);
            out++;
            out_size++;
        }
        upper = !upper;
        in++;
    }
    if (!upper) {
        out_size++;
    }
    return out_size;
}

/** @} */
