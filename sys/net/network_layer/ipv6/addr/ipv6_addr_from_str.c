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
#include <stdlib.h>
#include <string.h>

#include "byteorder.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"

#define HEX_L   "0123456789abcdef"
#define HEX_U   "0123456789ABCDEF"

/* based on inet_pton6() by Paul Vixie */
ipv6_addr_t *ipv6_addr_from_str(ipv6_addr_t *result, const char *addr)
{
    uint8_t *colonp = 0;
#ifdef MODULE_IPV4_ADDR
    const char *curtok = addr;
#endif
    uint32_t val = 0;
    char ch;
    uint8_t saw_xdigit = 0;
    uint8_t i = 0;

    if ((result == NULL) || (addr == NULL)) {
        return NULL;
    }

    ipv6_addr_set_unspecified(result);

    /* Leading :: requires some special handling. */
    if (*addr == ':') {
        if (*++addr != ':') {
            return NULL;
        }
    }

    while ((ch = *addr++) != '\0') {
        const char *pch;
        const char *xdigits;

        if ((pch = strchr((xdigits = HEX_L), ch)) == NULL) {
            pch = strchr((xdigits = HEX_U), ch);
        }

        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);

            if (val > 0xffff) {
                return NULL;
            }

            saw_xdigit = 1;
            continue;
        }

        if (ch == ':') {
#ifdef MODULE_IPV4_ADDR
            curtok = addr;
#endif

            if (!saw_xdigit) {
                if (colonp != NULL) {
                    return NULL;
                }

                colonp = &(result->u8[i]);
                continue;
            }

            if (i > sizeof(ipv6_addr_t)) {
                return NULL;
            }

            result->u8[i++] = (uint8_t)(val >> 8) & 0xff;
            result->u8[i++] = (uint8_t) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }

#ifdef MODULE_IPV4_ADDR
        if (ch == '.' && (i <= sizeof(ipv6_addr_t)) &&
            ipv4_addr_from_str((ipv4_addr_t *)(&(result->u8[i])),
                               curtok) != NULL) {
            i += sizeof(ipv4_addr_t);
            saw_xdigit = 0;
            break;  /* '\0' was seen by ipv4_addr_from_str(). */
        }
#endif

        return NULL;
    }

    if (saw_xdigit) {
        if (i + sizeof(uint16_t) > sizeof(ipv6_addr_t)) {
            return NULL;
        }

        result->u8[i++] = (uint8_t)(val >> 8) & 0xff;
        result->u8[i++] = (uint8_t) val & 0xff;
    }

    if (colonp != NULL) {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int32_t n = &(result->u8[i++]) - colonp;

        for (int32_t j = 1; j <= n; j++) {
            result->u8[sizeof(ipv6_addr_t) - j] = colonp[n - j];
            colonp[n - j] = 0;
        }

        i = sizeof(ipv6_addr_t);
    }

    if (i != sizeof(ipv6_addr_t)) {
        return NULL;
    }

    return result;
}

/**
 * @}
 */
