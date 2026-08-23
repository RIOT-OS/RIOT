/*
 * SPDX-FileCopyrightText: 2022 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "fmt.h"
#include "kernel_defines.h"
#include "net/ipv4/addr.h"

void ipv4_addr_print(const ipv4_addr_t *addr)
{
    assert(addr);
    char addr_str[IPV4_ADDR_MAX_STR_LEN];
    ipv4_addr_to_str(addr_str, addr, sizeof(addr_str));

    if (IS_USED(MODULE_FMT)) {
        print_str(addr_str);
    } else {
        printf("%s", addr_str);
    }
}

/**
 * @}
 */
