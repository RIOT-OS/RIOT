/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief GET_CPU_ID() test application
 *
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "cpu_conf.h"
#include "periph/cpuid.h"

int main(void)
{
    uint8_t id[CPUID_LEN];

    puts("Test for the CPUID driver");
    puts("This test is reading out the CPUID of the platforms CPU\n");

    printf("CPUID_LEN: %u\n", CPUID_LEN);

    /* read the CPUID */
    cpuid_get(id);

    /* print the CPUID */
    printf("CPUID:");
    for (unsigned int i = 0; i < CPUID_LEN; i++) {
        printf(" 0x%02x", id[i]);
    }
    printf("\n");

    return 0;
}
