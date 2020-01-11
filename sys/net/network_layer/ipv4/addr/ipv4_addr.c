/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
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
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "net/ipv4/addr.h"

#ifdef MODULE_FMT
#include "fmt.h"
#else
#include <stdio.h>
#endif

const ipv4_addr_t ipv4_addr_unspecified = IPV4_ADDR_UNSPECIFIED;
const ipv4_addr_t ipv4_addr_loopback = IPV4_ADDR_LOOPBACK;

int ipv4_addr_split(char *addr_str, char seperator, int _default)
{
    char *sep = addr_str;
    while(*++sep) {
        if (*sep == seperator) {
            *sep++ = '\0';
            if (*sep) {
                _default = atoi(sep);
            }
            break;
        }
    }

    return _default;
}

/**
 * @}
 */
