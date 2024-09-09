/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#include "net/dns.h"

#ifdef RIOT_VERSION
#include "byteorder.h"
#endif

#include "net/dns/msg.h"

static ssize_t _enc_domain_name(uint8_t *out, const char *domain_name)
{
    /*
     * DNS encodes domain names with "<len><part><len><part>", e.g.,
     * "example.org" ends up as "\7example\3org" in the packet.
     */
    uint8_t *part_start = out;
    uint8_t *out_pos = ++out;

    char c;

    while ((c = *domain_name)) {
        if (c == '.') {
            /* replace dot with length of name part as byte */
            *part_start = (out_pos - part_start - 1);
            part_start = out_pos++;
        }
        else {
            *out_pos++ = c;
        }
        domain_name++;
    }

    *part_start = (out_pos - part_start - 1);
    *out_pos++ = 0;

    return out_pos - out + 1;
}

static unsigned _put_short(uint8_t *out, uint16_t val)
{
    memcpy(out, &val, 2);
    return 2;
}

static unsigned _get_short(const uint8_t *buf)
{
    uint16_t _tmp;
    memcpy(&_tmp, buf, 2);
    return _tmp;
}

static ssize_t _skip_hostname(const uint8_t *buf, size_t len,
                              const uint8_t *bufpos)
{
    const uint8_t *buflim = buf + len;
    unsigned res = 0;

    if (bufpos >= buflim) {
        /* out-of-bound */
        return -EBADMSG;
    }
    /* handle DNS Message Compression */
    if (*bufpos >= 192) {
        if ((bufpos + 2) >= buflim) {
            return -EBADMSG;
        }
        return 2;
    }

    while (bufpos[res]) {
        res += bufpos[res] + 1;
        if ((&bufpos[res]) >= buflim) {
            /* out-of-bound */
            return -EBADMSG;
        }
    }
    return res + 1;
}

size_t dns_msg_compose_query(void *dns_buf, const char *domain_name,
                             uint16_t id, int family)
{
    uint8_t *buf = dns_buf;

    dns_hdr_t *hdr = (dns_hdr_t*) buf;
    memset(hdr, 0, sizeof(*hdr));
    hdr->id = htons(id);
    hdr->flags = htons(0x0120);
    hdr->qdcount = htons(1 + (family == AF_UNSPEC));

    uint8_t *bufpos = buf + sizeof(*hdr);

    unsigned _name_ptr;
    if ((family == AF_INET6) || (family == AF_UNSPEC)) {
        _name_ptr = (bufpos - buf);
        bufpos += _enc_domain_name(bufpos, domain_name);
        bufpos += _put_short(bufpos, htons(DNS_TYPE_AAAA));
        bufpos += _put_short(bufpos, htons(DNS_CLASS_IN));
    }

    if ((family == AF_INET) || (family == AF_UNSPEC)) {
        if (family == AF_UNSPEC) {
            bufpos += _put_short(bufpos, htons((0xc000) | (_name_ptr)));
        }
        else {
            bufpos += _enc_domain_name(bufpos, domain_name);
        }
        bufpos += _put_short(bufpos, htons(DNS_TYPE_A));
        bufpos += _put_short(bufpos, htons(DNS_CLASS_IN));
    }
    return bufpos - buf;
}

int dns_msg_parse_reply(const uint8_t *buf, size_t len, int family,
                        void *addr_out, uint32_t *ttl)
{
    const uint8_t *buflim = buf + len;
    const dns_hdr_t *hdr = (dns_hdr_t *)buf;
    const uint8_t *bufpos = buf + sizeof(*hdr);

    /* skip all queries that are part of the reply */
    for (unsigned n = 0; n < ntohs(hdr->qdcount); n++) {
        ssize_t tmp = _skip_hostname(buf, len, bufpos);
        if (tmp < 0) {
            return tmp;
        }
        bufpos += tmp;
        /* skip type and class of query */
        bufpos += (RR_TYPE_LENGTH + RR_CLASS_LENGTH);
    }

    for (unsigned n = 0; n < ntohs(hdr->ancount); n++) {
        ssize_t tmp = _skip_hostname(buf, len, bufpos);
        if (tmp < 0) {
            return tmp;
        }
        bufpos += tmp;
        if ((bufpos + RR_TYPE_LENGTH + RR_CLASS_LENGTH +
             RR_TTL_LENGTH + sizeof(uint16_t)) >= buflim) {
            return -EBADMSG;
        }
        uint16_t _type = ntohs(_get_short(bufpos));
        bufpos += RR_TYPE_LENGTH;
        uint16_t class = ntohs(_get_short(bufpos));
        bufpos += RR_CLASS_LENGTH;
        if (ttl) {
            *ttl = byteorder_bebuftohl(bufpos);
        }
        bufpos += RR_TTL_LENGTH;

        unsigned addrlen = ntohs(_get_short(bufpos));
        bufpos += RR_RDLENGTH_LENGTH;
        if ((bufpos + addrlen) > buflim) {
            return -EBADMSG;
        }

        /* skip unwanted answers */
        if ((class != DNS_CLASS_IN) ||
                ((_type == DNS_TYPE_A) && (family == AF_INET6)) ||
                ((_type == DNS_TYPE_AAAA) && (family == AF_INET)) ||
                ! ((_type == DNS_TYPE_A) || ((_type == DNS_TYPE_AAAA))
                    )) {
            if (addrlen > len) {
                /* buffer wraps around memory space */
                return -EBADMSG;
            }
            bufpos += addrlen;
            /* other out-of-bound is checked in `_skip_hostname()` at start of
             * loop */
            continue;
        }
        if (((addrlen != INADDRSZ) && (family == AF_INET)) ||
            ((addrlen != IN6ADDRSZ) && (family == AF_INET6)) ||
            ((addrlen != IN6ADDRSZ) && (addrlen != INADDRSZ) &&
             (family == AF_UNSPEC))) {
            return -EBADMSG;
        }

        memcpy(addr_out, bufpos, addrlen);
        return addrlen;
    }

    return -EBADMSG;
}

/** @} */
