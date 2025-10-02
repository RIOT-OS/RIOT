/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       fmt print test application
 *
 * This test is supposed to check for "compilabilty" of the fmt print_* instructions.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "fmt.h"
#include "modules.h"

int main(void)
{
    print_str("If you can read this:\n");
    print_u32_dec(0xFFFFFFFF);
    print_str("\n");
    print_s32_dec(0x80000000);
    print_str("\n");
    print_byte_hex(0xFA);
    print_str("\n");
    print_s32_dec(0x80000000);
    print_str("\n");
    print_u32_hex(0x12345678);
    print_str("\n");
    print_u64_hex(0x123456789ABCDEF0);
    print_str("\n");
    print_u64_dec(0xFFFFFFFFFFFFFFFF);
    print_str("\n");
    print_s64_dec(0x8000000000000000);
    print_str("\n");
    if (IS_USED(MODULE_FLOAT_MATH)) {
        print_float(1.2345, 5);
    }
    else {
        print_str("print_float() disabled, feature `float_math` not used.");
    }
    print_str("\n");
    print_bytes_hex("0123456789", 10);
    print_str("\n");
    /* test mixing of printf() and fmt's print() works fine */
    printf("%s", "Test");
    /* test fmt's print indeed only honors the length parameter and doesn't
     * print until the terminated zero byte */
    print(" not ", 1);
    print_str("successful.");
    puts("");

    return 0;
}
