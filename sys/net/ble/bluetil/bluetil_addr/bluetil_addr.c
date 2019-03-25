/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     ble_bluetil_addr
 * @{
 *
 * @file
 * @brief       Implementation of generic BLE address helper functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "fmt.h"
#include "assert.h"
#include "net/eui48.h"
#include "net/bluetil/addr.h"

void bluetil_addr_sprint(char *out, const uint8_t *addr)
{
    assert(out);
    assert(addr);

    fmt_byte_hex(out, addr[5]);
    for (int i = 4; i >= 0; i--) {
        out += 2;
        *out++ = ':';
        fmt_byte_hex(out, addr[i]);
    }
    out += 2;
    *out = '\0';
}

void bluetil_addr_print(const uint8_t *addr)
{
    assert(addr);

    char str[BLUETIL_ADDR_STRLEN];
    bluetil_addr_sprint(str, addr);
    printf("%s", str);
}

void bluetil_addr_ipv6_l2ll_sprint(char *out, const uint8_t *addr)
{
    assert(out);
    assert(addr);

    eui64_t iid;
    eui48_to_ipv6_iid(&iid, (const eui48_t *)addr);
    memcpy(out, "[FE80::", 6);
    out += 6;
    for (unsigned i = 0; i < 4; i++) {
        *out++ = ':';
        fmt_byte_hex(out, iid.uint8[i * 2]);
        out += 2;
        fmt_byte_hex(out, iid.uint8[(i * 2) + 1]);
        out += 2;
    }
    *out++ = ']';
    *out = '\0';
}

void bluetil_addr_ipv6_l2ll_print(const uint8_t *addr)
{
    assert(addr);

    char tmp[BLUETIL_IPV6_IID_STRLEN];
    bluetil_addr_ipv6_l2ll_sprint(tmp, addr);
    printf("%s", tmp);
}
