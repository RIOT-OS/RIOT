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
    /* test data width vs. output width discrepancy */
    CALL(od_hex_dump(short_str, sizeof(short_str), OD_WIDTH_DEFAULT));

    /* Test different output width in default configuration*/
    CALL(od_hex_dump(long_str, sizeof(long_str), OD_WIDTH_DEFAULT));
    CALL(od_hex_dump(long_str, sizeof(long_str), 4));
    CALL(od_hex_dump(long_str, sizeof(long_str), 3));
    CALL(od_hex_dump(long_str, sizeof(long_str), 8));
    CALL(od_hex_dump_ext(long_str, sizeof(long_str), 8, 0x100));

    return 0;
}
