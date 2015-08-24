/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <stdint.h>

#include "timex.h"

#define RADIX   (10)

static unsigned int u32_to_str(char *str, uint32_t u32)
{
    int len;
    int i = 0;
    if (u32 == 0) {
        str[i] = '0';
        return 1;
    }
    for (; u32 > 0; u32 /= RADIX) {
        char digit = (char)(u32 % RADIX);
        str[i++] = '0' + digit;
    }
    len = i;
    i--;    /* go back to last character */
    for (int j = 0; j < i; j++, i--) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
    return len;
}

const char *timex_to_str(timex_t t, char *timestamp)
{
    timex_normalize(&t);
    char *dec;
    uint8_t offset = 6;
    /* get seconds and set terminating '\0' byte */
    dec = &timestamp[u32_to_str(timestamp, t.seconds)];
    *(dec++) = '.';                         /* set decimal point */
    for (uint32_t i = 100000; i > 1; i /= 10) {  /* timex_normalize() ensures that i < 1000000 */
        if (t.microseconds < i) {           /* pad with 0's */
            *(dec++) = '0';
            offset--;
        }
    }
    u32_to_str(dec, t.microseconds);        /* convert microseconds */
    dec += offset;
    *(dec++) = ' ';                         /* append unit */
    *(dec++) = 's';
    *(dec) = '\0';
    return timestamp;
}

/** @} */
