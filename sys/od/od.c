/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
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

#include <stdio.h>
#include <stdint.h>

#include "fmt.h"

#include "od.h"

void od_hex_dump(const void *data, size_t data_len, uint8_t width)
{
    print_str("00000000");

    if (width == 0) {
        width = OD_WIDTH_DEFAULT;
    }

    for (unsigned int i = 0; i < data_len; i++) {
        print_str("  ");
        print_byte_hex(((uint8_t *)data)[i]);

        if ((((i + 1) % width) == 0) || i == (data_len - 1)) {
            puts("");

            if (i != (data_len - 1)) {
                print_u32_hex((uint32_t)(i + 1));
            }
        }
    }
}

/** @} */
