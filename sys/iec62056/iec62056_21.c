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
 * @brief       iec62056-21 implementation
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "checksum/ucrc16.h"
#include "iec62056/21.h"
#include "iec62056/obis.h"
#include "fmt.h"
#include "macros/utils.h"
#include "rtc_utils.h"

#define ENABLE_DEBUG 0
#include "debug.h"


static bool _is_whitespace(char c)
{
    return (
        (c == '\n') ||
        (c == '\r') ||
        (c == '\t') ||
        (c == ' ')
        );
}

static const char *_skip_space(const char *str)
{
    while (_is_whitespace(*str)) {
        str++;
    }
    return str;
}

ssize_t iec62056_21_get_header(const iec62056_21_parser_t *parser, const char **header)
{
    const char *telegram = parser->telegram;

    assert(telegram);
    if (*telegram != '/') {
        return -1;
    }
    const char *end_of_header = memchr(telegram, '\r', parser->telegram_length);

    if (end_of_header) {
        const char *start_of_header = telegram + 1;
        *header = start_of_header;
        return end_of_header - start_of_header;
    }
    return -1;
}

enum {
    IEC62056_21_STATE_OBIS,
    IEC62056_21_STATE_NEXT,
    IEC62056_21_STATE_COSEM_OBJECT,
    IEC62056_21_STATE_LASTLINE,
};

static int _obis_to_dataset(iec62056_21_dataset_t *dataset, const char *obis, size_t obis_len)
{
    dataset->value = NULL;
    dataset->value_length = 0;
    dataset->value_index = 0;
    return iec62056_obis_from_string(&dataset->obis, obis, obis_len);
}

static int _next_state(char next_char)
{
    if (fmt_is_digit(next_char)) {
        return IEC62056_21_STATE_OBIS;
    }
    if (next_char == '(') {
        return IEC62056_21_STATE_COSEM_OBJECT;
    }
    if (next_char == '!') {
        return IEC62056_21_STATE_LASTLINE;
    }
    return -1;
}

static ssize_t _parse_state_obis(const iec62056_21_parser_t *parser, iec62056_21_dataset_t *dataset,
                                 const char *telegram, size_t len, bool *skip)
{
    /* find the end of the obis ID */
    const char *obj = memchr(telegram, '(',  len);

    if (!obj) {
        return -1;
    }
    size_t obis_len = obj - telegram;

    if (_obis_to_dataset(dataset, telegram, obis_len) < 0) {
        return -1;
    }
    *skip = parser->callback(parser->callback_arg, dataset) < 0;

    return obis_len;
}

static ssize_t _parse_state_value(const iec62056_21_parser_t *parser,
                                  iec62056_21_dataset_t *dataset,
                                  const char *telegram, size_t len, bool skip)
{
    const char *obj_end = memchr(telegram, ')', len);

    if (!obj_end) {
        return -1;
    }

    const char *cosem_start = telegram + 1;

    size_t val_len = obj_end - cosem_start;

    dataset->value = cosem_start;
    dataset->value_length = val_len;
    if (!skip) {
        parser->callback(parser->callback_arg, dataset);
    }
    return val_len + 2;
}

int iec62056_21_parse_telegram(const iec62056_21_parser_t *parser)
{
    size_t len = parser->telegram_length;
    bool skip = false;
    iec62056_21_dataset_t dataset = { 0 };

    const char *header = NULL;
    ssize_t res = iec62056_21_get_header(parser, &header);

    if (res < 0) {
        return res;
    }

    const char *telegram = header;

    telegram += res;
    telegram = _skip_space(telegram);
    len -= telegram - parser->telegram;

    unsigned state = IEC62056_21_STATE_OBIS;

    while (len) {
        switch (state) {
        case IEC62056_21_STATE_OBIS:
        {
            ssize_t obis_len = _parse_state_obis(parser, &dataset, telegram, len, &skip);
            if (obis_len < 0) {
                return obis_len;
            }
            len -= obis_len;
            telegram += obis_len;
            state = IEC62056_21_STATE_COSEM_OBJECT;
            break;
        }
        case IEC62056_21_STATE_COSEM_OBJECT:
        {
            ssize_t val_len = _parse_state_value(parser, &dataset, telegram, len, skip);
            if (val_len < 0) {
                return -1;
            }

            dataset.value_index++;
            len -= val_len;
            telegram += val_len;
            if (len > 0) {
                const char *next_obj = _skip_space(telegram);
                int next_state = _next_state(*next_obj);
                if (next_state < 0) {
                    return -1;
                }
                state = next_state;

                len -= next_obj - telegram;
                telegram = next_obj;
            }
            break;
        }
        case IEC62056_21_STATE_LASTLINE:
            len = 0;
            break;
        default:
            return -1;
        }
    }
    return 0;
}

bool _register_part_is_numeric(const char *begin, const char *end)
{
    while (begin < end) {
        if (!fmt_is_digit(*begin)) {
            return false;
        }
        begin++;
    }
    return true;
}

bool iec62056_21_dataset_is_register(const iec62056_21_dataset_t *dataset)
{
    const char *end_of_register = &dataset->value[dataset->value_length];
    const char *fractional_part = memchr(dataset->value, '.', dataset->value_length);
    const char *register_unit = memchr(dataset->value, '*', dataset->value_length);

    if (dataset->value_length == 0) {
        return false;
    }

    if (!register_unit) {
        register_unit = end_of_register;
    }

    if (!fractional_part) {
        fractional_part = register_unit;
    }

    if (!_register_part_is_numeric(dataset->value, fractional_part)) {
        return false;
    }

    if (fractional_part == end_of_register) {
        return true;
    }
    fractional_part++;

    return _register_part_is_numeric(fractional_part, register_unit);
}

int iec62056_21_dataset_parse_register_to_decfrac(const iec62056_21_dataset_t *dataset,
                                                  int64_t *mantissa, int64_t *scaling)
{
    /* parse a string with format '001234.567*kWh' to components */
    if (dataset->value_length == 0) {
        return -1;
    }

    const char *reg = dataset->value;

    const char *end_of_register = &reg[dataset->value_length];
    const char *frac = memchr(reg, '.', dataset->value_length);
    const char *unit = memchr(
        frac ? frac : reg,
        '*',
        frac ? (size_t)(end_of_register - frac) : dataset->value_length);

    if (!unit) {
        unit = end_of_register;
    }

    bool negative = false;

    if ((dataset->value_length >= 2) && (*reg == '-')) {
        negative = true;
        reg++;
    }

    size_t num_len = frac ? frac - reg : unit - reg;

    for (size_t i = 0; i < num_len; i++) {
        if (!fmt_is_digit(reg[i])) {
            return -1;
        }
    }
    uint32_t num = scn_u32_dec(reg, num_len);

    *mantissa = num;
    *scaling = 0;

    if (frac) {
        frac++;
        size_t fraction_len = unit - frac;
        unsigned power = 1;
        for (unsigned i = 0; i < fraction_len; i++) {
            if (!fmt_is_digit(frac[i])) {
                return -1;
            }
            power *= 10;
        }
        uint32_t fraction = scn_u32_dec(frac, fraction_len);
        *mantissa *= power;
        *mantissa += fraction;
        *scaling = ((int64_t)fraction_len) * -1;
    }
    if (negative) {
        *mantissa *= -1;
    }
    return 0;
}

int iec62056_21_dataset_parse_register_to_float(const iec62056_21_dataset_t *dataset,
                                                 float *value)
{
    int64_t mantissa = 0;
    int64_t scaling = 0;
    int res = iec62056_21_dataset_parse_register_to_decfrac(dataset, &mantissa, &scaling);
    if (res < 0) {
        return res;
    }

    *value = (float)mantissa * pow(10, scaling);
    return 0;
}

int iec62056_21_dataset_parse_register_to_double(const iec62056_21_dataset_t *dataset,
                                                 double *value)
{
    int64_t mantissa = 0;
    int64_t scaling = 0;
    int res = iec62056_21_dataset_parse_register_to_decfrac(dataset, &mantissa, &scaling);

    if (res < 0) {
        return res;
    }

    *value = (double)mantissa * pow(10, scaling);
    return 0;
}

int iec62056_21_dataset_parse_register_unit(const iec62056_21_dataset_t *dataset,
                                            char *register_unit,
                                            size_t register_unit_max_length)
{
    const char *end_of_register = &dataset->value[dataset->value_length];
    const char *unit = memchr(dataset->value, '*', dataset->value_length);

    if (unit) {
        unit++;
        size_t max_to_copy = MIN(register_unit_max_length - 1, (size_t)(end_of_register - unit));
        memcpy(register_unit, unit, max_to_copy);
        register_unit[max_to_copy] = '\0';
        return max_to_copy + 1;
    }
    return 0;
}

static bool _timestamp_has_correct_length(size_t len)
{
    return len == 13;
}

static bool _timestamp_is_all_digits(const char *str)
{
    for (size_t i = 0; i < 12; i++) {
        if (!fmt_is_digit(str[i])) {
            return false;
        }
    }
    return true;
}

static bool _timestamp_ends_with_dst(const char *str)
{
    if ((str[12] == 'W') || (str[12] == 'S')) {
        return true;
    }
    return false;
}

bool iec62056_21_dataset_is_timestamp(const iec62056_21_dataset_t *dataset)
{
    return _timestamp_has_correct_length(dataset->value_length) &&
           _timestamp_is_all_digits(dataset->value) &&
           _timestamp_ends_with_dst(dataset->value);
}

int iec62056_21_dataset_parse_timestamp(const iec62056_21_dataset_t *dataset, struct tm *time)
{
    if (!iec62056_21_dataset_is_timestamp(dataset)) {
        return -1;
    }

    memset(time, 0, sizeof(struct tm));

    time->tm_year = scn_u32_dec(dataset->value, 2) + 100;
    time->tm_mon = scn_u32_dec(dataset->value + 2, 2) - 1;
    time->tm_mday = scn_u32_dec(dataset->value + 4, 2);
    time->tm_hour = scn_u32_dec(dataset->value + 6, 2);
    time->tm_min = scn_u32_dec(dataset->value + 8, 2);
    time->tm_sec = scn_u32_dec(dataset->value + 10, 2);
    time->tm_isdst = dataset->value[12] == 'S';

    rtc_tm_normalize(time); /* Normalize just in case */

    return 0;
}
