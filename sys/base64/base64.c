/*
 * Copyright (C) 2014 Hochschule für Angewandte Wissenschaften Hamburg (HAW)
 * Copyright (C) 2014 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup base64
 * @{
 * @file    base64.c
 * @brief   Functions to encode and decode base64
 *
 * @author  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 * @}
 *
 */

#include "base64.h"

#define BASE64_CAPITAL_UPPER_BOUND     (25)     /**< base64 'Z'           */
#define BASE64_SMALL_UPPER_BOUND       (51)     /**< base64 'z'           */
#define BASE64_NUMBER_UPPER_BOUND      (61)     /**< base64 '9'           */
#define BASE64_PLUS                    (62)     /**< base64 '+'           */
#define BASE64_SLASH                   (63)     /**< base64 '/'           */
#define BASE64_EQUALS                  (0xFE)   /**< no base64 symbol '=' */
#define BASE64_NOT_DEFINED             (0xFF)   /**< no base64 symbol     */

/*
 * returns the corresponding ascii symbol value for the given base64 code
 */
static char getsymbol(unsigned char code)
{
    if (code == BASE64_SLASH) {
        return '/';
    }

    if (code == BASE64_PLUS) {
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

int base64_encode(unsigned char *data_in, size_t data_in_size, \
                  unsigned char *base64_out, size_t *base64_out_size)
{
    size_t padding_bytes = ((data_in_size % 3) ? (3 - (data_in_size % 3)) : 0);
    size_t required_size = (4 * (data_in_size + 2 - ((data_in_size + 2) % 3)) / 3) + padding_bytes;

    if (data_in == NULL) {
        return BASE64_ERROR_DATA_IN;
    }

    if (data_in_size < 1) {
        return BASE64_ERROR_DATA_IN_SIZE;
    }

    if (*base64_out_size < required_size) {
        *base64_out_size = required_size;
        return BASE64_ERROR_BUFFER_OUT_SIZE;
    }

    if (base64_out == NULL) {
        return BASE64_ERROR_BUFFER_OUT;
    }

    int iterate_base64_buffer = 0;
    unsigned char nNum = 0;
    int nLst = 0;
    int njump = 0;
    unsigned char tmpval;

    for (int i = 0; i < (int)(data_in_size); ++i) {
        njump++;
        tmpval = *(data_in + i);

        nNum = (tmpval >> (2 * njump));

        if (njump == 4) {
            nNum = nLst << (8 - 2 * njump);
            njump = 0;
            nLst = 0;
            --i;
        }
        else {
            nNum += nLst << (8 - 2 * njump);
            nLst =  tmpval & ((1 << njump * 2) - 1);
        }

        base64_out[iterate_base64_buffer++] = getsymbol(nNum);
    }

    /* The last character is not finished yet */
    njump++;

    if (njump == 4) {
        nNum = (tmpval >> (2 * njump));
    }

    nNum = nLst << (8 - 2 * njump);
    base64_out[iterate_base64_buffer++] = getsymbol(nNum);

    /* if required we append '=' for the required dividability */
    while (iterate_base64_buffer % 4) {
        base64_out[iterate_base64_buffer++] = '=';
    }

    *base64_out_size = iterate_base64_buffer;

    return BASE64_SUCCESS;
}

/*
 *  returns the corresponding base64 code for the given ascii symbol
 */
static int getcode(char symbol)
{
    if (symbol == '/') {
        return BASE64_SLASH;
    }

    if (symbol == '+') {
        return BASE64_PLUS;
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

int base64_decode(unsigned char *base64_in, size_t base64_in_size, \
                  unsigned char *data_out, size_t *data_out_size)
{
    size_t required_size = ((base64_in_size / 4) * 3);

    if (base64_in == NULL) {
        return BASE64_ERROR_DATA_IN;
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
    unsigned char nNum = 0;
    int nLst = getcode(base64_in[0]) << 2;
    int code = 0;

    int mask = 2;

    for (int i = 1; i < (int)(base64_in_size); i++) {
        code = getcode(base64_in[i]);

        if (code == BASE64_NOT_DEFINED || code == BASE64_EQUALS) {
            continue;
        }

        int nm = (0xFF << (2 * mask));

        nNum = nLst + ((code & (0xFF & nm)) >> (2 * mask));
        nLst = (code & (0xFF & ~nm)) << (8 - (2 * mask));

        (mask != 3) ? data_out[iterate_data_buffer++] = nNum : nNum;
        (mask == 0) ? mask = 3 : mask--;
    }

    if (code == BASE64_EQUALS) {
        /* add the last character to the data_out buffer */
        data_out[iterate_data_buffer] = nNum;
    }

    *data_out_size = iterate_data_buffer;
    return BASE64_SUCCESS;
}
