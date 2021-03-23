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
#include <ctype.h>

#include "fmt.h"

#include "od.h"

void od_hex_dump_ext(const void *data, size_t data_len, uint8_t width, uint32_t offset)
{
    print_u32_hex(offset);

    if (width == 0) {
        width = OD_WIDTH_DEFAULT;
    }
#if MODULE_OD_STRING
    uint8_t str_pos = 0;
#endif
    for (unsigned int i = 0; i < data_len; i++) {
        print_str("  ");
        print_byte_hex(((uint8_t *)data)[i]);

        if ((((i + 1) % width) == 0) || i == (data_len - 1)) {
#if MODULE_OD_STRING
            /* fill in whitespace for incomplete hex lines */
            for (unsigned j = i; ((j + 1) % width) != 0; ++j) {
                print_str("    ");
            }
            print_str("  ");
            for (unsigned k = 0; k < (i % width) + 1; k++){
                if (isprint(((uint8_t *)data)[str_pos+k])) {
                    putchar(((uint8_t *)data)[str_pos+k]);
                }
                else {
                    putchar('.');
                }
            }
            /* safe position for next iteration */
            str_pos = i + 1;
#endif
            putchar('\n');

            if (i != (data_len - 1)) {
                print_u32_hex((uint32_t)(offset + i + 1));
            }
        }
    }
}

/** @} */
