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
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "cpu_conf.h"
#include "periph/cpuid.h"
#include "test_utils/result_output.h"

int main(void)
{
    uint8_t id[CPUID_LEN];

    puts("Test for the CPUID driver");
    puts("This test is reading out the CPUID of the platforms CPU\n");

    printf("CPUID_LEN: %u\n", CPUID_LEN);

    /* read the CPUID */
    cpuid_get(id);

    /* print the CPUID */
    turo_data_u8_array(0, id, CPUID_LEN);
    turo_result_success(0);

    return 0;
}
