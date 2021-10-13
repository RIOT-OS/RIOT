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

    fmt_byte_hex(out, addr[0]);
    for (unsigned i = 1; i < BLE_ADDR_LEN; i++) {
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

    unsigned pos = 0;
    for (unsigned i = 0; i < (BLUETIL_ADDR_STRLEN - 1); i += 3) {
        if (!_is_hex_char(addr_str[i]) || !_is_hex_char(addr_str[i + 1])) {
            return NULL;
        }
        addr[pos++] = fmt_hex_byte(addr_str + i);
    }
    return addr;
}

void bluetil_addr_ipv6_l2ll_sprint(char *out, const uint8_t *addr)
{
    assert(out);
    assert(addr);

    memcpy(out, "[FE80::", 7);
    out += 7;
    out += fmt_byte_hex(out, addr[0]);
    out += fmt_byte_hex(out, addr[1]);
    *out++ = ':';
    out += fmt_byte_hex(out, addr[2]);
    memcpy(out, "FF:FE", 5);
    out += 5;
    out += fmt_byte_hex(out, addr[3]);
    *out++ = ':';
    out += fmt_byte_hex(out, addr[4]);
    out += fmt_byte_hex(out, addr[5]);
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
