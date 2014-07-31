/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief GET_CPU_ID() test application
 *
 * @author Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "cpu-conf.h"
#include "periph/cpuid.h"

#ifndef CPUID_ID_LEN
#error "cpuid is not defined for the CPU type of this board"
#endif

int main(void)
{
    uint8_t id[CPUID_ID_LEN];

    cpuid_get(id);

    for (unsigned int i = 0; i < CPUID_ID_LEN; i++) {
        printf("0x%02x ", id[i]);
    }

    printf("\n");

    return 0;
}
