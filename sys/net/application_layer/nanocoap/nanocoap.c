/*
 * Copyright (C) 2016-17 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_net_nanocoap
 * @{
 *
 * @file
 * @brief       Nanocoap implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "net/nanocoap.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _decode_value(unsigned val, uint8_t **pkt_pos_ptr, uint8_t *pkt_end);
static uint32_t _decode_uint(uint8_t *pkt_pos, unsigned nbytes);

/* http://tools.ietf.org/html/rfc7252#section-3
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Ver| T |  TKL  |      Code     |          Message ID           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Token (if any, TKL bytes) ...
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Options (if any) ...
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |1 1 1 1 1 1 1 1|    Payload (if any) ...
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
int coap_parse(coap_pkt_t *pkt, uint8_t *buf, size_t len)
{
    uint8_t *urlpos = pkt->url;
    coap_hdr_t *hdr = (coap_hdr_t *)buf;

    pkt->hdr = hdr;

    uint8_t *pkt_pos = hdr->data;
    uint8_t *pkt_end = buf + len;

    memset(pkt->url, '\0', NANOCOAP_URL_MAX);
    pkt->payload_len = 0;
    pkt->observe_value = UINT32_MAX;

    /* token value (tkl bytes) */
    if (coap_get_token_len(pkt)) {
        pkt->token = pkt_pos;
        pkt_pos += coap_get_token_len(pkt);
    }
    else {
        pkt->token = NULL;
    }

    /* parse options */
    int option_nr = 0;
    while (pkt_pos != pkt_end) {
        uint8_t option_byte = *pkt_pos++;
        if (option_byte == 0xff) {
            pkt->payload = pkt_pos;
            pkt->payload_len = buf + len - pkt_pos;
            DEBUG("payload len = %u\n", pkt->payload_len);
            break;
        }
        else {
            int option_delta = _decode_value(option_byte >> 4, &pkt_pos, pkt_end);
            if (option_delta < 0) {
                DEBUG("bad op delta\n");
                return -EBADMSG;
            }
            int option_len = _decode_value(option_byte & 0xf, &pkt_pos, pkt_end);
            if (option_len < 0) {
                DEBUG("bad op len\n");
                return -EBADMSG;
            }
            option_nr += option_delta;
            DEBUG("option nr=%i len=%i\n", option_nr, option_len);

            switch (option_nr) {
                case COAP_OPT_URI_HOST:
                    DEBUG("nanocoap: ignoring Uri-Host option!\n");
                    break;
                case COAP_OPT_URI_PATH:
                    *urlpos++ = '/';
                    memcpy(urlpos, pkt_pos, option_len);
                    urlpos += option_len;
                    break;
                case COAP_OPT_CONTENT_FORMAT:
                    if (option_len == 0) {
                        pkt->content_type = 0;
                    }
                    else if (option_len == 1) {
                        pkt->content_type = *pkt_pos;
                    }
                    else if (option_len == 2) {
                        memcpy(&pkt->content_type, pkt_pos, 2);
                        pkt->content_type = ntohs(pkt->content_type);
                    }
                    break;
                case COAP_OPT_OBSERVE:
                    if (option_len < 4) {
                        pkt->observe_value = _decode_uint(pkt_pos, option_len);
                    }
                    else {
                        DEBUG("nanocoap: discarding packet with invalid option length.\n");
                        return -EBADMSG;
                    }
                    break;
                default:
                    DEBUG("nanocoap: unhandled option nr=%i len=%i critical=%u\n", option_nr, option_len, option_nr & 1);
                    if (option_nr & 1) {
                        DEBUG("nanocoap: discarding packet with unknown critical option.\n");
                        return -EBADMSG;
                    }
            }

            pkt_pos += option_len;
        }
    }

    DEBUG("coap pkt parsed. code=%u detail=%u payload_len=%u, 0x%02x\n",
          coap_get_code_class(pkt),
          coap_get_code_detail(pkt),
          pkt->payload_len, hdr->code);

    return 0;
}

ssize_t coap_handle_req(coap_pkt_t *pkt, uint8_t *resp_buf, unsigned resp_buf_len)
{
    if (coap_get_code_class(pkt) != COAP_REQ) {
        DEBUG("coap_handle_req(): not a request.\n");
        return -EBADMSG;
    }

    if (pkt->hdr->code == 0) {
        return coap_build_reply(pkt, COAP_CODE_EMPTY, resp_buf, resp_buf_len, 0);
    }

    unsigned method_flag = coap_method2flag(coap_get_code_detail(pkt));

    for (unsigned i = 0; i < coap_resources_numof; i++) {
        const coap_resource_t *resource = &coap_resources[i];
        if (!(resource->methods & method_flag)) {
            continue;
        }

        int res = strcmp((char *)pkt->url, resource->path);
        if (res > 0) {
            continue;
        }
        else if (res < 0) {
            break;
        }
        else {
            return resource->handler(pkt, resp_buf, resp_buf_len, resource->context);
        }
    }

    return coap_build_reply(pkt, COAP_CODE_404, resp_buf, resp_buf_len, 0);
}

ssize_t coap_reply_simple(coap_pkt_t *pkt,
                          unsigned code,
                          uint8_t *buf, size_t len,
                          unsigned ct,
                          const uint8_t *payload, uint8_t payload_len)
{
    uint8_t *payload_start = buf + coap_get_total_hdr_len(pkt);
    uint8_t *bufpos = payload_start;

    if (payload_len) {
        bufpos += coap_put_option_ct(bufpos, 0, ct);
        *bufpos++ = 0xff;

        memcpy(bufpos, payload, payload_len);
        bufpos += payload_len;
    }

    return coap_build_reply(pkt, code, buf, len, bufpos - payload_start);
}

ssize_t coap_build_reply(coap_pkt_t *pkt, unsigned code,
                         uint8_t *rbuf, unsigned rlen, unsigned payload_len)
{
    unsigned tkl = coap_get_token_len(pkt);
    unsigned len = sizeof(coap_hdr_t) + tkl;

    if ((len + payload_len + 1) > rlen) {
        return -ENOSPC;
    }

    /* if code is COAP_CODE_EMPTY (zero), use RST as type, else RESP */
    unsigned type = code ? COAP_RESP : COAP_RST;

    coap_build_hdr((coap_hdr_t *)rbuf, type, pkt->token, tkl, code,
                   ntohs(pkt->hdr->id));
    coap_hdr_set_type((coap_hdr_t *)rbuf, type);
    coap_hdr_set_code((coap_hdr_t *)rbuf, code);

    len += payload_len;

    return len;
}

ssize_t coap_build_hdr(coap_hdr_t *hdr, unsigned type, uint8_t *token, size_t token_len, unsigned code, uint16_t id)
{
    assert(!(type & ~0x3));
    assert(!(token_len & ~0x1f));

    memset(hdr, 0, sizeof(coap_hdr_t));
    hdr->ver_t_tkl = (0x1 << 6) | (type << 4) | token_len;
    hdr->code = code;
    hdr->id = htons(id);

    if (token_len) {
        memcpy(hdr->data, token, token_len);
    }

    return sizeof(coap_hdr_t) + token_len;
}

static int _decode_value(unsigned val, uint8_t **pkt_pos_ptr, uint8_t *pkt_end)
{
    uint8_t *pkt_pos = *pkt_pos_ptr;
    size_t left = pkt_end - pkt_pos;
    int res;

    switch (val) {
        case 13:
        {
            /* An 8-bit unsigned integer follows the initial byte and
               indicates the Option Delta minus 13. */
            if (left < 1) {
                return -ENOSPC;
            }
            uint8_t delta = *pkt_pos++;
            res = delta + 13;
            break;
        }
        case 14:
        {
            /* A 16-bit unsigned integer in network byte order follows
             * the initial byte and indicates the Option Delta minus
             * 269. */
            if (left < 2) {
                return -ENOSPC;
            }
            uint16_t delta;
            uint8_t *_tmp = (uint8_t *)&delta;
            *_tmp++ = *pkt_pos++;
            *_tmp++ = *pkt_pos++;
            res = ntohs(delta) + 269;
            break;
        }
        case 15:
            /* Reserved for the Payload Marker.  If the field is set to
             * this value but the entire byte is not the payload
             * marker, this MUST be processed as a message format
             * error. */
            return -EBADMSG;
        default:
            res = val;
    }

    *pkt_pos_ptr = pkt_pos;
    return res;
}

static uint32_t _decode_uint(uint8_t *pkt_pos, unsigned nbytes)
{
    assert(nbytes <= 4);

    uint32_t res = 0;
    if (nbytes) {
        memcpy(((uint8_t *)&res) + (4 - nbytes), pkt_pos, nbytes);
    }
    return ntohl(res);
}

static unsigned _put_delta_optlen(uint8_t *buf, unsigned offset, unsigned shift, unsigned val)
{
    if (val < 13) {
        *buf |= (val << shift);
    }
    else if (val < (256 + 13)) {
        *buf |= (13 << shift);
        buf[offset++] = (val - 13);
    }
    else {
        *buf |= (14 << shift);
        uint16_t tmp = (val - 269);
        tmp = htons(tmp);
        memcpy(buf + offset, &tmp, 2);
        offset += 2;
    }
    return offset;
}

size_t coap_put_option(uint8_t *buf, uint16_t lastonum, uint16_t onum, uint8_t *odata, size_t olen)
{
    assert(lastonum <= onum);

    unsigned delta = (onum - lastonum);
    *buf = 0;

    /* write delta value to option header: 4 upper bits of header (shift 4) +
     * 1 or 2 optional bytes depending on delta value) */
    unsigned n = _put_delta_optlen(buf, 1, 4, delta);
    /* write option length to option header: 4 lower bits of header (shift 0) +
     * 1 or 2 optional bytes depending of the length of the option */
    n = _put_delta_optlen(buf, n, 0, olen);
    if (olen) {
        memcpy(buf + n, odata, olen);
        n += olen;
    }
    return (size_t)n;
}

size_t coap_put_option_ct(uint8_t *buf, uint16_t lastonum, uint16_t content_type)
{
    if (content_type == 0) {
        return coap_put_option(buf, lastonum, COAP_OPT_CONTENT_FORMAT, NULL, 0);
    }
    else if (content_type <= 255) {
        uint8_t tmp = content_type;
        return coap_put_option(buf, lastonum, COAP_OPT_CONTENT_FORMAT, &tmp, sizeof(tmp));
    }
    else {
        return coap_put_option(buf, lastonum, COAP_OPT_CONTENT_FORMAT, (uint8_t *)&content_type, sizeof(content_type));
    }
}

size_t coap_put_option_uri(uint8_t *buf, uint16_t lastonum, const char *uri, uint16_t optnum)
{
    char separator = (optnum == COAP_OPT_URI_PATH) ? '/' : '&';
    size_t uri_len = strlen(uri);

    if (uri_len == 0) {
        return 0;
    }

    uint8_t *bufpos = buf;
    char *uripos = (char *)uri;

    while (uri_len) {
        size_t part_len;
        uripos++;
        uint8_t *part_start = (uint8_t *)uripos;

        while (uri_len--) {
            if ((*uripos == separator) || (*uripos == '\0')) {
                break;
            }
            uripos++;
        }

        part_len = (uint8_t *)uripos - part_start;

        if (part_len) {
            bufpos += coap_put_option(bufpos, lastonum, optnum, part_start, part_len);
            lastonum = optnum;
        }
    }

    return bufpos - buf;
}

ssize_t coap_well_known_core_default_handler(coap_pkt_t *pkt, uint8_t *buf, \
                                             size_t len, void *context)
{
    (void)context;

    uint8_t *payload = buf + coap_get_total_hdr_len(pkt);

    uint8_t *bufpos = payload;

    bufpos += coap_put_option_ct(bufpos, 0, COAP_CT_LINK_FORMAT);
    *bufpos++ = 0xff;

    for (unsigned i = 0; i < coap_resources_numof; i++) {
        if (i) {
            *bufpos++ = ',';
        }
        *bufpos++ = '<';
        unsigned url_len = strlen(coap_resources[i].path);
        memcpy(bufpos, coap_resources[i].path, url_len);
        bufpos += url_len;
        *bufpos++ = '>';
    }

    unsigned payload_len = bufpos - payload;

    return coap_build_reply(pkt, COAP_CODE_205, buf, len, payload_len);
}
