/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief GET_CPU_ID() test application
 *
 * @author Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "cpu-conf.h"
#include "periph/cpuid.h"

/* only compile this test if CPUID getter is defined for the platform */
#if CPUID_ID_LEN

int main(void)
{
    uint8_t id[CPUID_ID_LEN];

    puts("Test for the CPUID driver");
    puts("This test is reading out the CPUID of the platforms CPU\n");

    /* read the CPUID */
    cpuid_get(id);

    /* print the CPUID */
    printf("CPUID:");
    for (unsigned int i = 0; i < CPUID_ID_LEN; i++) {
        printf(" 0x%02x", id[i]);
    }
    printf("\n");

    return 0;
}

#else

int main(void)
{
    puts("Test for the CPUID driver");
    puts("Error: the CPUID driver is not defined for this platform");
    return 0;
}

#endif /* CPUID_ID_LEN */
