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

#include "fmt.h"

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
    print_float(1.2345, 5);
    print_str("\n");
    print_str("Test successful.\n");

    return 0;
}
