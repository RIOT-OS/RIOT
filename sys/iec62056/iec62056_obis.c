/*
 * Copyright (C) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     module_iec62056
 * @{
 *
 * @file
 * @brief       iec62056-related function implementations
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "fmt.h"
#include "iec62056.h"
#include "iec62056/obis.h"
#include "macros/utils.h"


static const char *digitnsep(const char *str, size_t n)
{
    while (n--) {
        if (!fmt_is_digit(*str)) {
            return str;
        }
        str++;
    }
    return NULL;
}

static const char *_get_obis_group_value(const char *str, uint8_t *dest)
{
    const char *separator_pos = digitnsep(str, 4);

    if (!separator_pos) {
        return NULL;
    }

    uint16_t group_value = scn_u32_dec(str, separator_pos - str);

    if (group_value > UINT8_MAX) {
        return NULL;
    }

    *dest = (uint8_t)group_value;

    return separator_pos;
}

int iec62056_obis_from_string(iec62056_obis_t *obis, const char *str, size_t len)
{
    /* Convert a string with format "1-2:3.4.5" into a iec62056_obis_t struct */
    const char *end_of_str = &str[len];
    unsigned scan_group = 0; /* 0 = scan_group A, 5 = scan_group f */
    int res = 0;
    char previous_separator = '\0';

    memset(obis, 255, sizeof(iec62056_obis_t));
    do {
        uint8_t value = 0;

        str = _get_obis_group_value(str, &value);
        if (!str) {
            res = -1;
            break;
        }

        char separator = *str;
        if ((separator == '-') && (scan_group == 0)) {
            obis->a = value;
            scan_group = 1;
        }
        else if ((separator == ':') && (scan_group < 2)) {
            obis->b = value;
            scan_group = 2;
        }
        else if ((separator == '.') && scan_group < 3) {
            obis->c = value;
            scan_group = 3;
        }
        else if ((previous_separator == '.') && (scan_group < 5)) {
            if (scan_group == 3) {
                obis->d = value;
            }
            else if (scan_group == 4) {
                obis->e = value;
            }
            scan_group++;
        }
        else if (((previous_separator != '.') && (scan_group > 2))
                 || (scan_group == 5)) {
            obis->f = value;
            break;
        }
        else {
            res = -1;
            break;
        }
        str++;
        previous_separator = separator;
    } while (str < end_of_str);
    return res;
}

bool iec62056_obis_equal(const iec62056_obis_t *first, const iec62056_obis_t *second,
                         bool include_channel)
{
    return (first->a == second->a)  &&
           ((first->b == second->b) || !include_channel) &&
           (first->c == second->c) &&
           (first->d == second->d) &&
           (first->e == second->e) &&
           (first->f == second->f);
}
