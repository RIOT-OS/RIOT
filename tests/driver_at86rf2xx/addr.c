/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdio.h>
#include <stdint.h>

#include "common.h"

void print_addr(uint8_t *addr, size_t addr_len)
{
    for (size_t i = 0; i < addr_len; i++) {
        if (i != 0) {
            printf(":");
        }
        printf("%02x", (unsigned)addr[i]);
    }
}

/** @} */
