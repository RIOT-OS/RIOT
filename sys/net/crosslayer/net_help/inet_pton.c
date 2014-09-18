/* Copyright (c) 1996 by Internet Software Consortium.
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#if defined(MCU_ATMEGA2560)
#include <sys/types.h>
#endif

#include "kernel.h"

#include "inet_pton.h"

/* int
 * inet_pton4(src, dst)
 *  like inet_aton() but without all the hexadecimal and shorthand.
 * return:
 *  1 if `src' is a valid dotted quad, else 0.
 * notice:
 *  does not touch `dst' unless it's returning 1.
 * author:
 *  Paul Vixie, 1996.
 */
static int inet_pton4(const char *src, unsigned char *dst)
{
    static const char digits[] = "0123456789";
    int saw_digit, octets, ch;
    unsigned char tmp[INADDRSZ], *tp;

    saw_digit = 0;
    octets = 0;
    *(tp = tmp) = 0;

    while ((ch = *src++) != '\0') {
        const char *pch;

        if ((pch = strchr(digits, ch)) != NULL) {
            unsigned int new = *tp * 10 + (unsigned int)(pch - digits);

            if (new > 255) {
                return (0);
            }

            *tp = new;

            if (!saw_digit) {
                if (++octets > 4) {
                    return (0);
                }

                saw_digit = 1;
            }
        }
        else if (ch == '.' && saw_digit) {
            if (octets == 4) {
                return (0);
            }

            *++tp = 0;
            saw_digit = 0;
        }
        else {
            return (0);
        }
    }

    if (octets < 4) {
        return (0);
    }

    memcpy(dst, tmp, INADDRSZ);
    return (1);
}

static const char XDIGITS_L[] = "0123456789abcdef";
static const char XDIGITS_U[] = "0123456789ABCDEF";

/* int
 * inet_pton6(src, dst)
 *  convert presentation level address to network order binary form.
 * return:
 *  1 if `src' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *  (1) does not touch `dst' unless it's returning 1.
 *  (2) :: in a full address is silently ignored.
 * credit:
 *  inspired by Mark Andrews.
 * author:
 *  Paul Vixie, 1996.
 */
static int inet_pton6(const char *src, unsigned char *dst)
{
    unsigned char tmp[IN6ADDRSZ], *tp, *endp, *colonp;
    const char *curtok;
    int ch, saw_xdigit;
    unsigned int val;

    memset((tp = tmp), '\0', IN6ADDRSZ);
    endp = tp + IN6ADDRSZ;
    colonp = NULL;

    /* Leading :: requires some special handling. */
    if (*src == ':') {
        if (*++src != ':') {
            return (0);
        }
    }

    curtok = src;
    saw_xdigit = 0;
    val = 0;

    while ((ch = *src++) != '\0') {
        const char *pch;
        const char *xdigits;

        if ((pch = strchr((xdigits = XDIGITS_L), ch)) == NULL) {
            pch = strchr((xdigits = XDIGITS_U), ch);
        }

        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);

            if (val > 0xffff) {
                return (0);
            }

            saw_xdigit = 1;
            continue;
        }

        if (ch == ':') {
            curtok = src;

            if (!saw_xdigit) {
                if (colonp) {
                    return (0);
                }

                colonp = tp;
                continue;
            }

            if (tp + INT16SZ > endp) {
                return (0);
            }

            *tp++ = (unsigned char)(val >> 8) & 0xff;
            *tp++ = (unsigned char) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }

        if (ch == '.' && ((tp + INADDRSZ) <= endp) &&
           inet_pton4(curtok, tp) > 0) {
            tp += INADDRSZ;
            saw_xdigit = 0;
            break;  /* '\0' was seen by inet_pton4(). */
        }

        return (0);
    }

    if (saw_xdigit) {
        if (tp + INT16SZ > endp) {
            return (0);
        }

        *tp++ = (unsigned char)(val >> 8) & 0xff;
        *tp++ = (unsigned char) val & 0xff;
    }

    if (colonp != NULL) {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const ssize_t n = tp - colonp;
        ssize_t i;

        for (i = 1; i <= n; i++) {
            endp[- i] = colonp[n - i];
            colonp[n - i] = 0;
        }

        tp = endp;
    }

    if (tp != endp) {
        return (0);
    }

    memcpy(dst, tmp, IN6ADDRSZ);
    return (1);
}

/* int
 * inet_pton(af, src, dst)
 *  convert from presentation format (which usually means ASCII printable)
 *  to network format (which is usually some kind of binary format).
 * return:
 *  1 if the address was valid for the specified address family
 *  0 if the address wasn't valid (`dst' is untouched in this case)
 *  -1 if some other error occurred (`dst' is untouched in this case, too)
 * author:
 *  Paul Vixie, 1996.
 */
int inet_pton(int af, const char *src, void *dst)
{
    switch(af) {
        case AF_INET:
            return (inet_pton4(src, dst));

        case AF_INET6:
            return (inet_pton6(src, dst));

        default:
            return (-1);
    }

    /* NOTREACHED */
}
