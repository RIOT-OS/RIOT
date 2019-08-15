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

static int _is_hex_char(char c)
{
    return (((c >= '0') && (c <= '9')) ||
            ((c >= 'A') && (c <= 'F')) ||
            ((c >= 'a') && (c <= 'f')));
}

void bluetil_addr_swapped_cp(const uint8_t *src, uint8_t *dst)
{
    dst[0] = src[5];
    dst[1] = src[4];
    dst[2] = src[3];
    dst[3] = src[2];
    dst[4] = src[1];
    dst[5] = src[0];
}

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

uint8_t *bluetil_addr_from_str(uint8_t *addr, const char *addr_str)
{
    assert(addr);
    assert(addr_str);

    /* check for colons */
    for (unsigned i = 2; i < (BLUETIL_ADDR_STRLEN - 1); i += 3) {
        if (addr_str[i] != ':') {
            return NULL;
        }
    }

    unsigned pos = BLE_ADDR_LEN;
    for (unsigned i = 0; i < (BLUETIL_ADDR_STRLEN - 1); i += 3) {
        if (!_is_hex_char(addr_str[i]) || !_is_hex_char(addr_str[i + 1])) {
            return NULL;
        }
        addr[--pos] = fmt_hex_byte(addr_str + i);
    }
    return addr;
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
