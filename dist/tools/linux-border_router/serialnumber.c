/*
 * Copyright (C) 2014 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "serialnumber.h"

int serial_add8(uint8_t s, uint8_t n)
{
    if (n > 127) {
        return -1;
    }

    uint16_t sum = s + n;
    return (uint8_t)(sum % 256);
}

int serial_add16(uint16_t s, uint16_t n)
{
    if (n > 32767) {
        return -1;
    }

    uint32_t sum = s + n;
    return (uint16_t)(sum % 65536);
}

int serial_add32(uint32_t s, uint32_t n)
{
    if (n > 2147483647) {
        return -1;
    }

    uint64_t sum = s + n;
    return (uint32_t)(sum % 4294967296);
}

serial_comp_res_t serial_comp8(uint8_t s1, uint8_t s2)
{
    if (s1 == s2) {
        return EQUAL;
    }

    if ((s1 < s2 && s1 - s2 < 128) || (s1 > s2 && s1 - s2 > 128)) {
        return LESS;
    }

    if ((s1 < s2 && s1 - s2 > 128) || (s1 > s2 && s1 - s2 < 128)) {
        return GREATER;
    }

    return UNDEF;
}

serial_comp_res_t serial_comp16(uint16_t s1, uint16_t s2)
{
    if (s1 == s2) {
        return EQUAL;
    }

    if ((s1 < s2 && s1 - s2 < 32768) || (s1 > s2 && s1 - s2 > 32768)) {
        return LESS;
    }

    if ((s1 < s2 && s1 - s2 > 32768) || (s1 > s2 && s1 - s2 < 32768)) {
        return GREATER;
    }

    return UNDEF;
}

serial_comp_res_t serial_comp32(uint32_t s1, uint32_t s2)
{
    if (s1 == s2) {
        return EQUAL;
    }

    if ((s1 < s2 && s1 - s2 < 2147483648) || (s1 > s2 && s1 - s2 > 2147483648)) {
        return LESS;
    }

    if ((s1 < s2 && s1 - s2 > 2147483648) || (s1 > s2 && s1 - s2 < 2147483648)) {
        return GREATER;
    }

    return UNDEF;
}
