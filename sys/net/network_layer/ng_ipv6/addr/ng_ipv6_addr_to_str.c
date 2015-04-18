/*
 * Copyright (c) 1996 by Internet Software Consortium.
 * Copyright (c) 2015 by Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/**
 * @ingroup     net_ipv6_addr
 * @{
 *
 * @file
 *
 * @author      Paul Vixie
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <inttypes.h>
#include <string.h>

#include "byteorder.h"
#include "net/ng_ipv6/addr.h"

/* Length of an IPv6 address in 16-bit words */
#define IPV6_ADDR_WORD_LEN  (sizeof(ng_ipv6_addr_t) / sizeof(uint16_t))

#define IPV4_ADDR_MIN_STR_LEN   (sizeof("255.255.255.255"))

#define HEX_L "0123456789abcdef"

/* XXX: move this to IPv4 when we have it */
/* based on inet_ntop4() by Paul Vixie */
static char *ipv4_addr_to_str(char *result, const network_uint32_t *addr,
                              uint8_t result_len)
{
    uint8_t n = 0;
    char *next = result;

    if (result_len < IPV4_ADDR_MIN_STR_LEN) {
        return NULL;
    }

    do {
        uint8_t u = addr->u8[n];

        if (u > 99) {
            *next++ = '0' + u / 100;
            u %= 100;
            *next++ = '0' + u / 10;
            u %= 10;
        }
        else if (u > 9) {
            *next++ = '0' + u / 10;
            u %= 10;
        }

        *next++ = '0' + u;
        *next++ = '.';
        n++;
    } while (n < 4);

    *--next = '\0';
    return result;
}

/* based on inet_ntop6() by Paul Vixie */
char *ng_ipv6_addr_to_str(char *result, const ng_ipv6_addr_t *addr,
                          uint8_t result_len)
{
    char tmp[NG_IPV6_ADDR_MAX_STR_LEN], *tp;
    struct {
        int16_t base, len;
    } best = { -1, 0}, cur = { -1, 0};

    if ((result == NULL) || (addr == NULL)) {
        return NULL;
    }

    /*
     * Preprocess:
     *  Find the longest run of 0x0000's in address for :: shorthanding.
     */
    for (uint8_t i = 0; i < IPV6_ADDR_WORD_LEN; i++) {
        if (addr->u16[i].u16 == 0) {
            if (cur.base == -1) {
                cur.base = i;
                cur.len = 1;
            }
            else {
                cur.len++;
            }
        }
        else {
            if (cur.base != -1) {
                if (best.base == -1 || cur.len > best.len) {
                    best = cur;
                }

                cur.base = -1;
            }
        }
    }

    if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len) {
            best = cur;
        }
    }

    if (best.base != -1 && best.len < 2) {
        best.base = -1;
    }

    /*
     * Format the result.
     */
    tp = tmp;

    for (int i = 0; i < ((int)IPV6_ADDR_WORD_LEN);) {
        /* Are we inside the best run of 0x00's? */
        if (i == best.base) {
            *tp++ = ':';
            i += best.len;
            continue;
        }

        /* Are we following an initial run of 0x00s or any real hex? */
        if (i != 0) {
            *tp++ = ':';
        }

        /* Is this address an encapsulated IPv4? */
        if (i == 6 && best.base == 0 &&
            (best.len == 6 || (best.len == 5 && addr->u16[5].u16 == 0xffff))) {
            if (!ipv4_addr_to_str(tp, &addr->u32[3], sizeof(tmp) - (tp - tmp))) {
                return (NULL);
            }

            tp += strlen(tp);
            break;
        }

        if ((addr->u16[i].u8[0] & 0xf0) != 0x00) {
            *tp++ = HEX_L[addr->u16[i].u8[0] >> 4];
            *tp++ = HEX_L[addr->u16[i].u8[0] & 0x0f];
            *tp++ = HEX_L[addr->u16[i].u8[1] >> 4];
        }
        else if ((addr->u16[i].u8[0] & 0x0f) != 0x00) {
            *tp++ = HEX_L[addr->u16[i].u8[0] & 0x0f];
            *tp++ = HEX_L[addr->u16[i].u8[1] >> 4];
        }
        else if ((addr->u16[i].u8[1] & 0xf0) != 0x00) {
            *tp++ = HEX_L[addr->u16[i].u8[1] >> 4];
        }

        *tp++ = HEX_L[addr->u16[i].u8[1] & 0xf];

        i++;
    }

    /* Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) == IPV6_ADDR_WORD_LEN) {
        *tp++ = ':';
    }

    *tp++ = '\0';

    /*
     * Check for overflow, copy, and we're done.
     */
    if ((size_t)(tp - tmp) > result_len) {
        return NULL;
    }

    strcpy(result, tmp);
    return result;
}

/**
 * @}
 */
