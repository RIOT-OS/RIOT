/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests od module.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "od.h"

#define CALL(fn)    puts(#fn); fn

static const char short_str[] = "AB";
static const char long_str[] = "\xff,a\xff.bcdefghijklmnop";

int main(void)
{
    /* test data width vs. output width discrepency */
    CALL(od(short_str, sizeof(short_str), OD_WIDTH_DEFAULT, 0));

    /* Test different output width in default configuration*/
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT, 0));
    CALL(od(long_str, sizeof(long_str), 4, 0));
    CALL(od(long_str, sizeof(long_str), 3, 0));
    CALL(od(long_str, sizeof(long_str), 8, 0));

    /* Test different address formats in default configuration */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_ADDRESS_OCTAL));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_ADDRESS_HEX));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_ADDRESS_DECIMAL));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_ADDRESS_NONE));

    /* Test octal data format with different address formats  */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_OCTAL));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_ADDRESS_OCTAL | OD_FLAGS_BYTES_OCTAL));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_ADDRESS_HEX | OD_FLAGS_BYTES_OCTAL));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_ADDRESS_DECIMAL | OD_FLAGS_BYTES_OCTAL));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_ADDRESS_NONE | OD_FLAGS_BYTES_OCTAL));

    /* Test different data formats */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_CHAR));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_INT));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_DECIMAL));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_UINT));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_HEX));

    /* Test octal byte-wise output with different output lengths */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_1));
    CALL(od(long_str, sizeof(long_str), 4,
            OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_1));
    CALL(od(long_str, sizeof(long_str), 3,
            OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_1));
    CALL(od(long_str, sizeof(long_str), 8,
            OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_1));

    /* Test different data formats with byte-wise output */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_CHAR | OD_FLAGS_LENGTH_1));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_1));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_DECIMAL | OD_FLAGS_LENGTH_1));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_1));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_1));

    /* Test octal 2-byte-wise output with different output lengths */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_2));
    CALL(od(long_str, sizeof(long_str), 4,
            OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_2));
    CALL(od(long_str, sizeof(long_str), 3,
            OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_2));
    CALL(od(long_str, sizeof(long_str), 8,
            OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_2));

    /* Test 2-byte-char output (should just return normal char output) */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_CHAR | OD_FLAGS_LENGTH_2));

    /* Test 2-byte-int output with different output widths */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_2));
    CALL(od(long_str, sizeof(long_str), 5,
            OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_2));

    /* Test 2-byte-decimal output with different output widths */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_DECIMAL | OD_FLAGS_LENGTH_2));
    CALL(od(long_str, sizeof(long_str), 5,
            OD_FLAGS_BYTES_DECIMAL | OD_FLAGS_LENGTH_2));

    /* Test 2-byte-unsigned-int output with different output widths */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_2));
    CALL(od(long_str, sizeof(long_str), 5,
            OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_2));

    /* Test 2-byte-hex output with different output widths */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_2));
    CALL(od(long_str, sizeof(long_str), 5,
            OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_2));

    /* Test different 4-byte-wise byte formats */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_4));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_DECIMAL | OD_FLAGS_LENGTH_4));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_4));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_4));

    /* Test different 8-byte-wise byte formats */
#ifndef MODULE_NEWLIB
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_8));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_DECIMAL | OD_FLAGS_LENGTH_8));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_8));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_8));
#endif

    /* Test different char-wise byte formats */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_CHAR));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_DECIMAL | OD_FLAGS_LENGTH_CHAR));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_CHAR));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_CHAR));

    /* Test different short-wise byte formats */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_SHORT));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_DECIMAL | OD_FLAGS_LENGTH_SHORT));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_SHORT));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_SHORT));

    /* Test different long-wise byte formats */
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_LONG));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_DECIMAL | OD_FLAGS_LENGTH_LONG));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_LONG));
    CALL(od(long_str, sizeof(long_str), OD_WIDTH_DEFAULT,
            OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_LONG));

    return 0;
}
