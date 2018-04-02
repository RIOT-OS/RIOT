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
#include "stdio.h"

int main(void)
{
    char str[22]; // UINT64_MAX 18446744073709551615
    uint16_t temp16;
    uint32_t temp32;
    uint64_t temp64;
    size_t len;

    print_str("Testing 'print_str' if you can read this it works.\n");

    /* 16 bit conversion */
    printf("\nTesting 16 bit to char conversion:\n");

    temp16 = 1;
    len = fmt_u16_dec( str, temp16);
    if(len != 1){
        printf("\tError length %i != 1\n", len);
    }
    printf("1 = %s\n", str);

    temp16 = UINT16_MAX;
    len = fmt_u16_dec( str, temp16);
    if(len != 5){
        printf("\tError length %i != 5\n", len);
    }
    printf("65 535 = %s\n", str);

    /* 32 bit conversion */
    printf("\nTesting 32 bit to char conversion:\n");

    temp32 = UINT16_MAX +1;
    len = fmt_u32_dec( str, temp32);
    if(len != 5){
        printf("Error length %i != 7\n", len);
    }
    printf("1 000 000 = %s\n", str);

    temp32 = UINT32_MAX;
    len = fmt_u32_dec( str, temp32);
    if(len != 10){
        printf("Error length %i != 10\n", len);
    }
    printf("4 294 967 295 = %s\n", str);

    /* 64 bit conversion */
    printf("\nTesting 64 bit to char conversion:\n");
    temp64 = UINT32_MAX+1;
    len = fmt_u64_dec( str, temp64);
    if(len != 10){
        printf("Error length %i != 10\n", len);
    }
    printf("4 294 967 296 = %s\n", str);

    temp64 = UINT64_MAX;
    len = fmt_u32_dec( str, temp32);
    if(len != 19){
        printf("Error length %i != 19\n", len);
    }
    printf("18 446 744 073 709 551 615 = %s\n", str);

    printf("\nMore testing has to be implemented jet.\n");

    return 0;
}
