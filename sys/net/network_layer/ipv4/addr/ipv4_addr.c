/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
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
