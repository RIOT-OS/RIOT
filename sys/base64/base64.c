/*
 * Copyright (C) 2014 Hochschule für Angewandte Wissenschaften Hamburg (HAW)
 * Copyright (C) 2014 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_base64
 * @{
 * @file
 * @brief   Functions to encode and decode base64
 *
 * @author  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 * @}
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "base64.h"
#include "kernel_defines.h"

#define BASE64_CAPITAL_UPPER_BOUND     (25)     /**< base64 'Z'           */
#define BASE64_SMALL_UPPER_BOUND       (51)     /**< base64 'z'           */
#define BASE64_NUMBER_UPPER_BOUND      (61)     /**< base64 '9'           */
#define BASE64_PLUS                    (62)     /**< base64 '+'           */
#define BASE64_MINUS                   (62)     /**< base64 '-'           */
#define BASE64_SLASH                   (63)     /**< base64 '/'           */
#define BASE64_UNDERLINE               (63)     /**< base64 '_'           */
#define BASE64_EQUALS                  (0xFE)   /**< no base64 symbol '=' */
#define BASE64_NOT_DEFINED             (0xFF)   /**< no base64 symbol     */

/*
 * returns the corresponding ascii symbol value for the given base64 code
 */
static char getsymbol(uint8_t code, bool urlsafe)
{
    if (!IS_ACTIVE(MODULE_BASE64URL)) {
        urlsafe = false;
    }

    if (urlsafe && code == BASE64_UNDERLINE) {
        return '_';
    }

    if (urlsafe && code == BASE64_MINUS) {
        return '-';
    }

    if (!urlsafe && code == BASE64_SLASH) {
        return '/';
    }

    if (!urlsafe && code == BASE64_PLUS) {
        return '+';
    }

    if (code <= BASE64_CAPITAL_UPPER_BOUND) {
        return (code + 'A');
    }

    if (code <= BASE64_SMALL_UPPER_BOUND) {
        return (code + ('z' - BASE64_SMALL_UPPER_BOUND));
    }

    if (code <= BASE64_NUMBER_UPPER_BOUND) {
        return (code + ('9' - BASE64_NUMBER_UPPER_BOUND));
    }

    return (char)BASE64_NOT_DEFINED;
}

static int base64_encode_base(const void *data_in, size_t data_in_size,
                              void *base64_out, size_t *base64_out_size,
                              bool urlsafe)
{
    const uint8_t *in = data_in;
    uint8_t *out = base64_out;
    size_t required_size = base64_estimate_encode_size(data_in_size);

    if (data_in == NULL) {
        return BASE64_ERROR_DATA_IN;
    }

    if (data_in_size == 0) {
        *base64_out_size = 0;
        return BASE64_SUCCESS;
    }

    if (*base64_out_size < required_size) {
        *base64_out_size = required_size;
        return BASE64_ERROR_BUFFER_OUT_SIZE;
    }

    if (out == NULL) {
        return BASE64_ERROR_BUFFER_OUT;
    }

    int iterate_base64_buffer = 0;
    uint8_t n_num = 0;
    int nLst = 0;
    int njump = 0;

    for (int i = 0; i < (int)(data_in_size); ++i) {
        uint8_t tmpval;
        njump++;
        tmpval = *(in + i);

        n_num = (tmpval >> (2 * njump));

        if (njump == 4) {
            n_num = nLst << (8 - 2 * njump);
            njump = 0;
            nLst = 0;
            --i;
        }
        else {
            n_num += nLst << (8 - 2 * njump);
            nLst =  tmpval & ((1 << njump * 2) - 1);
        }

        out[iterate_base64_buffer++] = getsymbol(n_num, urlsafe);
    }

    /* The last character is not finished yet */
    njump++;

    n_num = nLst << (8 - 2 * njump);
    out[iterate_base64_buffer++] = getsymbol(n_num, urlsafe);

    /* if required we append '=' for the required dividability */
    while (iterate_base64_buffer % 4) {
        out[iterate_base64_buffer++] = '=';
    }

    *base64_out_size = iterate_base64_buffer;

    return BASE64_SUCCESS;
}

int base64_encode(const void *data_in, size_t data_in_size,
                  void *base64_out, size_t *base64_out_size)
{
    return base64_encode_base(data_in, data_in_size, base64_out, base64_out_size, false);
}

#if IS_ACTIVE(MODULE_BASE64URL)
int base64url_encode(const void *data_in, size_t data_in_size,
                     void *base64_out, size_t *base64_out_size)
{
    return base64_encode_base(data_in, data_in_size, base64_out, base64_out_size, true);
}
#endif

/*
 *  returns the corresponding base64 code for the given ascii symbol
 */
static uint8_t getcode(char symbol)
{
    if (symbol == '/') {
        return BASE64_SLASH;
    }

    if (symbol == '_') {
        return BASE64_UNDERLINE;
    }

    if (symbol == '+') {
        return BASE64_PLUS;
    }

    if (symbol == '-') {
        return BASE64_MINUS;
    }

    if (symbol == '=') {
        /* indicates a padded base64 end */
        return BASE64_EQUALS;
    }

    if (symbol < '0') {
        /* indicates that the given symbol is not base64 and should be ignored */
        return BASE64_NOT_DEFINED;
    }

    if (symbol <= '9' && symbol >= '0') {
        return (symbol + (BASE64_NUMBER_UPPER_BOUND - '9'));
    }

    if (symbol <= 'Z' && symbol >= 'A') {
        return (symbol - 'A');
    }

    if (symbol <= 'z' && symbol >= 'a') {
        return (symbol + (BASE64_SMALL_UPPER_BOUND - 'z'));
    }

    /* indicates that the given symbol is not base64 and should be ignored */
    return BASE64_NOT_DEFINED;
}

int base64_decode(const void *base64_in, size_t base64_in_size,
                  void *data_out, size_t *data_out_size)
{
    uint8_t *out = data_out;
    const uint8_t *in = base64_in;
    size_t required_size = base64_estimate_decode_size(base64_in_size);

    if (in == NULL) {
        return BASE64_ERROR_DATA_IN;
    }

    if (base64_in_size == 0) {
        *data_out_size = 0;
        return BASE64_SUCCESS;
    }

    if (base64_in_size < 4) {
        return BASE64_ERROR_DATA_IN_SIZE;
    }

    if (*data_out_size < required_size) {
        *data_out_size = required_size;
        return BASE64_ERROR_BUFFER_OUT_SIZE;
    }

    if (data_out == NULL) {
        return BASE64_ERROR_BUFFER_OUT;
    }

    int iterate_data_buffer = 0;
    uint8_t n_num = 0;
    int nLst = getcode(in[0]) << 2;
    int code = 0;

    int mask = 2;

    for (int i = 1; i < (int)(base64_in_size); i++) {
        code = getcode(in[i]);

        if (code == BASE64_NOT_DEFINED || code == BASE64_EQUALS) {
            continue;
        }

        int nm = (0xFF << (2 * mask));

        n_num = nLst + ((code & (0xFF & nm)) >> (2 * mask));
        nLst = (code & (0xFF & ~nm)) << (8 - (2 * mask));

        (mask != 3) ? out[iterate_data_buffer++] = n_num : n_num;
        (mask == 0) ? mask = 3 : mask--;
    }

    if (code == BASE64_EQUALS) {
        /* add the last character to the data_out buffer */
        out[iterate_data_buffer] = n_num;
    }

    *data_out_size = iterate_data_buffer;
    return BASE64_SUCCESS;
}
