/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       CPU-ID driver implementation
 *
 * The CC2538 provides a 64-bit unique identifier, that is unique for each shipped unit.
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include <string.h>

#include "cpu.h"
#include "periph/cpuid.h"

#define BITS_PER_BYTE 8

void cpuid_get(void *id)
{
    uint8_t *dest = (uint8_t *)id;

    /**
     * The byte-order is big-endian but the word order is little endian.
     * Make some sense of it:
     */
    dest[0] = IEEE_ADDR_MSWORD >> (3 * BITS_PER_BYTE);
    dest[1] = IEEE_ADDR_MSWORD >> (2 * BITS_PER_BYTE);
    dest[2] = IEEE_ADDR_MSWORD >> (1 * BITS_PER_BYTE);
    dest[3] = IEEE_ADDR_MSWORD >> (0 * BITS_PER_BYTE);
    dest[4] = IEEE_ADDR_LSWORD >> (3 * BITS_PER_BYTE);
    dest[5] = IEEE_ADDR_LSWORD >> (2 * BITS_PER_BYTE);
    dest[6] = IEEE_ADDR_LSWORD >> (1 * BITS_PER_BYTE);
    dest[7] = IEEE_ADDR_LSWORD >> (0 * BITS_PER_BYTE);
}
/** @} */
