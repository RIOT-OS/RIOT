/*
 * Copyright (C) 2018 Ken Bannister <kb2ma@runbox.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "net/nanocoap.h"

extern size_t _encode_uint(uint32_t *val);

/* Common functionality for addition of an option */
static ssize_t _add_opt_pkt(coap_pkt_t *pkt, uint16_t optnum, uint8_t *val,
                            size_t val_len)
{
    assert(pkt->options_len < NANOCOAP_NOPTS_MAX);

    uint16_t lastonum = (pkt->options_len)
            ? pkt->options[pkt->options_len - 1].opt_num : 0;
    assert(optnum >= lastonum);

    size_t optlen = coap_put_option(pkt->payload, lastonum, optnum, val, val_len);
    assert(pkt->payload_len > optlen);

    pkt->options[pkt->options_len].opt_num = optnum;
    pkt->options[pkt->options_len].offset = pkt->payload - (uint8_t *)pkt->hdr;
    pkt->options_len++;
    pkt->payload += optlen;
    pkt->payload_len -= optlen;

    return optlen;
}

ssize_t coap_opt_add_string(coap_pkt_t *pkt, uint16_t optnum, const char *string,
                           char separator)
{
    size_t unread_len = strlen(string);
    if (!unread_len) {
        return 0;
    }
    char *uripos = (char *)string;
    size_t write_len = 0;

    while (unread_len) {
        size_t part_len;
        uripos++;
        uint8_t *part_start = (uint8_t *)uripos;

        while (unread_len--) {
            if ((*uripos == separator) || (*uripos == '\0')) {
                break;
            }
            uripos++;
        }

        part_len = (uint8_t *)uripos - part_start;

        if (part_len) {
            if (pkt->options_len == NANOCOAP_NOPTS_MAX) {
                return -ENOSPC;
            }
            write_len += _add_opt_pkt(pkt, optnum, part_start, part_len);
        }
    }

    return write_len;
}

ssize_t coap_opt_add_uint(coap_pkt_t *pkt, uint16_t optnum, uint32_t value)
{
    uint32_t tmp = value;
    unsigned tmp_len = _encode_uint(&tmp);
    return _add_opt_pkt(pkt, optnum, (uint8_t *)&tmp, tmp_len);
}

ssize_t coap_opt_finish(coap_pkt_t *pkt, uint16_t flags)
{
    if (flags & COAP_OPT_FINISH_PAYLOAD) {
        assert(pkt->payload_len > 1);

        *pkt->payload++ = 0xFF;
        pkt->payload_len--;
    }
    else {
        pkt->payload_len = 0;
    }

    return pkt->payload - (uint8_t *)pkt->hdr;
}

void coap_pkt_init(coap_pkt_t *pkt, uint8_t *buf, size_t len, size_t header_len)
{
    memset(pkt, 0, sizeof(coap_pkt_t));
    pkt->hdr = (coap_hdr_t *)buf;
    pkt->token = buf + sizeof(coap_hdr_t);
    pkt->payload = buf + header_len;
    pkt->payload_len = len - header_len;
}
