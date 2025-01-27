/*
 * Copyright (C) 2016-18 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Inria
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap
 * @{
 *
 * @file
 * @brief       nanoCoAP implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "bitarithm.h"
#include "net/nanocoap.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @name    Internally used CoAP packet types
 * @{
 */
#define COAP_REQ                (0)
#define COAP_RESP               (2)
#define COAP_RST                (3)
/** @} */

#ifdef MODULE_NANOCOAP_RESOURCES
/**
 * @brief   CoAP resources XFA
 */
XFA_INIT_CONST(coap_resource_t, coap_resources_xfa);

/**
 * @brief   Add well-known .core handler
 */
#if CONFIG_NANOCOAP_SERVER_WELL_KNOWN_CORE
NANOCOAP_RESOURCE(well_known_core) COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER;
#endif

/* re-define coap_resources for compatibility with non-XFA version */
#define coap_resources ((const coap_resource_t *)coap_resources_xfa)
#define coap_resources_numof XFA_LEN(coap_resource_t, coap_resources_xfa)
#endif

static int _decode_value(unsigned val, uint8_t **pkt_pos_ptr, uint8_t *pkt_end);
static uint32_t _decode_uint(uint8_t *pkt_pos, unsigned nbytes);
static size_t _encode_uint(uint32_t *val);

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
    coap_hdr_t *hdr = (coap_hdr_t *)buf;
    pkt->hdr = hdr;

    uint8_t *pkt_pos = coap_hdr_data_ptr(hdr);
    uint8_t *pkt_end = buf + len;

    pkt->payload = NULL;
    pkt->payload_len = 0;
    memset(pkt->opt_crit, 0, sizeof(pkt->opt_crit));
    pkt->snips = NULL;

    if (len < sizeof(coap_hdr_t)) {
        DEBUG("msg too short\n");
        return -EBADMSG;
    }
    else if ((coap_get_code_raw(pkt) == 0) && (len > sizeof(coap_hdr_t))) {
        DEBUG("empty msg too long\n");
        return -EBADMSG;
    }

    if (IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        if ((pkt->hdr->ver_t_tkl & 0xf) == 15) {
            DEBUG("nanocoap: token length is reserved value 15,"
                  "invalid for extended token length field.\n");
            return -EBADMSG;
        }
    } else if (coap_get_token_len(pkt) > COAP_TOKEN_LENGTH_MAX) {
        DEBUG("nanocoap: token length invalid\n");
        return -EBADMSG;
    }
    /* pkt_pos range is validated after options parsing loop below */
    pkt_pos += coap_get_token_len(pkt);

    coap_optpos_t *optpos = pkt->options;
    unsigned option_count = 0;
    unsigned option_nr = 0;

    /* parse options */
    while (pkt_pos < pkt_end) {
        uint8_t *option_start = pkt_pos;
        uint8_t option_byte = *pkt_pos++;
        if (option_byte == COAP_PAYLOAD_MARKER) {
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
            DEBUG("option count=%u nr=%u len=%i\n", option_count, option_nr, option_len);

            if (option_delta) {
                if (option_count >= CONFIG_NANOCOAP_NOPTS_MAX) {
                    DEBUG("nanocoap: max nr of options exceeded\n");
                    return -ENOMEM;
                }

                /* check if option is critical */
                if (option_nr & 1) {
                    bf_set(pkt->opt_crit, option_count);
                }
                optpos->opt_num = option_nr;
                optpos->offset = (uintptr_t)option_start - (uintptr_t)hdr;
                DEBUG("optpos option_nr=%u %u\n", (unsigned)option_nr, (unsigned)optpos->offset);
                optpos++;
                option_count++;
            }

            pkt_pos += option_len;
        }
    }

    if (pkt_pos > pkt_end) {
        DEBUG("nanocoap: bad packet length\n");
        return -EBADMSG;
    }

    pkt->options_len = option_count;
    if (!pkt->payload) {
        pkt->payload = pkt_pos;
    }

#ifdef MODULE_GCOAP
    if (coap_opt_get_uint(pkt, COAP_OPT_OBSERVE, &pkt->observe_value) != 0) {
        pkt->observe_value = UINT32_MAX;
    }
#endif

    DEBUG("coap pkt parsed. code=%u detail=%u payload_len=%u, nopts=%u, 0x%02x\n",
          coap_get_code_class(pkt),
          coap_get_code_detail(pkt),
          pkt->payload_len, option_count, hdr->code);

    return 0;
}

int coap_match_path(const coap_resource_t *resource, const uint8_t *uri)
{
    assert(resource && uri);
    int res;

    if (resource->methods & COAP_MATCH_SUBTREE) {
        int len = strlen(resource->path);
        res = strncmp((char *)uri, resource->path, len);
    }
    else {
        res = strcmp((char *)uri, resource->path);
    }
    return res;
}

uint8_t *coap_find_option(coap_pkt_t *pkt, unsigned opt_num)
{
    const coap_optpos_t *optpos = pkt->options;
    unsigned opt_count = pkt->options_len;

    while (opt_count--) {
        if (optpos->opt_num == opt_num) {
            unsigned idx = index_of(pkt->options, optpos);
            bf_unset(pkt->opt_crit, idx);
            return (uint8_t*)pkt->hdr + optpos->offset;
        }
        optpos++;
    }
    return NULL;
}

/*
 * Parse option attributes
 *
 * pkt[in]        coap_pkt_t for buffer
 * pkt_pos[in]    first byte of option in buffer
 * delta[out]     option delta from previous option
 * opt_len[out]   length of option value
 *
 * return         next byte after option header, usually the option value
 * return         NULL if initial pkt_pos is payload marker or past options
 */
static uint8_t *_parse_option(const coap_pkt_t *pkt,
                              uint8_t *pkt_pos, uint16_t *delta, int *opt_len)
{
    uint8_t *hdr_end = pkt->payload;

    if ((pkt_pos >= hdr_end)
            || (((pkt_pos + 1) == hdr_end) && (*pkt_pos == COAP_PAYLOAD_MARKER))) {
        return NULL;
    }

    uint8_t option_byte = *pkt_pos++;

    *delta = _decode_value(option_byte >> 4, &pkt_pos, hdr_end);
    *opt_len = _decode_value(option_byte & 0xf, &pkt_pos, hdr_end);

    return pkt_pos;
}

ssize_t coap_opt_get_opaque(coap_pkt_t *pkt, unsigned opt_num, uint8_t **value)
{
    uint8_t *start = coap_find_option(pkt, opt_num);
    if (!start) {
        return -ENOENT;
    }

    uint16_t delta;
    int len;

    *value = _parse_option(pkt, start, &delta, &len);
    if (!*value) {
        return -EINVAL;
    }

    return len;
}

int coap_opt_get_uint(coap_pkt_t *pkt, uint16_t opt_num, uint32_t *target)
{
    assert(target);

    uint8_t *opt_pos = coap_find_option(pkt, opt_num);
    if (opt_pos) {
        uint16_t delta;
        int option_len = 0;
        uint8_t *pkt_pos = _parse_option(pkt, opt_pos, &delta, &option_len);
        if (option_len >= 0) {
            if (option_len > 4) {
                DEBUG("nanocoap: uint option with len > 4 (unsupported).\n");
                return -ENOSPC;
            }
            *target = _decode_uint(pkt_pos, option_len);
            return 0;
        }
        else {
            DEBUG("nanocoap: discarding packet with invalid option length.\n");
            return -EBADMSG;
        }
    }
    return -ENOENT;
}

uint8_t *coap_iterate_option(coap_pkt_t *pkt, unsigned optnum,
                             uint8_t **opt_pos, int *opt_len)
{
    uint8_t *data_start;

    bool first = false;
    if (*opt_pos == NULL) {
        *opt_pos = coap_find_option(pkt, optnum);
        first = true;
        if (*opt_pos == NULL) {
            return NULL;
        }
    }

    uint16_t delta = 0;
    data_start = _parse_option(pkt, *opt_pos, &delta, opt_len);
    if (data_start && (first || !delta)) {
        *opt_pos = data_start + *opt_len;
        return data_start;
    }
    else {
        return NULL;
    }
}

static unsigned _get_content_format(coap_pkt_t *pkt, unsigned int opt_num)
{
    uint8_t *opt_pos = coap_find_option(pkt, opt_num);
    unsigned content_type = COAP_FORMAT_NONE;
    if (opt_pos) {
        uint16_t delta;
        int option_len = 0;
        uint8_t *pkt_pos = _parse_option(pkt, opt_pos, &delta, &option_len);

        if (option_len == 0) {
            content_type = 0;
        } else if (option_len == 1) {
            content_type = *pkt_pos;
        } else if (option_len == 2) {
            memcpy(&content_type, pkt_pos, 2);
            content_type = ntohs(content_type);
        }
    }

    return content_type;
}

unsigned coap_get_content_type(coap_pkt_t *pkt)
{
    return _get_content_format(pkt, COAP_OPT_CONTENT_FORMAT);
}

unsigned coap_get_accept(coap_pkt_t *pkt)
{
    return _get_content_format(pkt, COAP_OPT_ACCEPT);
}

ssize_t coap_opt_get_next(const coap_pkt_t *pkt, coap_optpos_t *opt,
                          uint8_t **value, bool init_opt)
{
    if (init_opt) {
        opt->opt_num = 0;
        opt->offset = coap_get_total_hdr_len(pkt);
    }
    uint8_t *start = (uint8_t*)pkt->hdr + opt->offset;

    /* Find start of option value and value length. */
    uint16_t delta;
    int len;

    start = _parse_option(pkt, start, &delta, &len);
    if (!start) {
        return -ENOENT;
    }

    *value = start;
    opt->opt_num += delta;
    opt->offset = start + len - (uint8_t*)pkt->hdr;
    return len;
}

ssize_t coap_opt_get_string(coap_pkt_t *pkt, uint16_t optnum,
                            uint8_t *target, size_t max_len, char separator)
{
    assert(pkt && target && (max_len > 1));

    uint8_t *opt_pos = NULL;
    int opt_len;
    unsigned left = max_len;

    while (1) {
        uint8_t *part_start = coap_iterate_option(pkt, optnum, &opt_pos, &opt_len);

        if (part_start == NULL) {
            /* if option was not found still return separator */
            if (opt_pos == NULL) {
                *target++ = (uint8_t)separator;
                --left;
            }
            break;
        }

        /* separator and terminating \0 have to fit */
        if (left < (unsigned)(opt_len + 2)) {
            return -ENOSPC;
        }

        *target++ = (uint8_t)separator;
        memcpy(target, part_start, opt_len);
        target += opt_len;
        left -= opt_len + 1;
    }

    *target = '\0';
    left--;

    return (int)(max_len - left);
}

int coap_get_blockopt(coap_pkt_t *pkt, uint16_t option, uint32_t *blknum, uint8_t *szx)
{
    uint8_t *optpos = coap_find_option(pkt, option);
    if (!optpos) {
        *blknum = 0;
        *szx = 0;
        return -1;
    }

    int option_len;
    uint16_t delta;

    uint8_t *data_start = _parse_option(pkt, optpos, &delta, &option_len);
    if (!data_start) {
        DEBUG("nanocoap: invalid start data\n");
        return -1;
    }

    if (option_len > 4) {
        DEBUG("nanocoap: invalid option length\n");
        return -1;
    }

    uint32_t blkopt = _decode_uint(data_start, option_len);

    DEBUG("nanocoap: blkopt len: %i\n", option_len);
    DEBUG("nanocoap: blkopt: 0x%08x\n", (unsigned)blkopt);
    *blknum = blkopt >> COAP_BLOCKWISE_NUM_OFF;
    *szx = blkopt & COAP_BLOCKWISE_SZX_MASK;

    return (blkopt & 0x8) ? 1 : 0;
}

bool coap_find_uri_query(coap_pkt_t *pkt, const char *key, const char **value, size_t *len)
{
    uint8_t *opt_pos = NULL;

    while (1) {
        int len_query;
        const void *key_data = coap_iterate_option(pkt, COAP_OPT_URI_QUERY,
                                                   &opt_pos, &len_query);
        if (key_data == NULL) {
            return false;
        }

        const char *separator = memchr(key_data, '=', len_query);
        size_t len_key = separator
                       ? (separator - (char *)key_data)
                       : len_query;

        if (memcmp(key, key_data, len_key)) {
            continue;
        }

        if (value == NULL) {
            return true;
        }

        assert(len);
        if (separator) {
            *value = separator + 1;
            *len = len_query - len_key - 1;
        } else {
            *value = NULL;
            *len   = 0;
        }
        return true;
    }

    return false;
}

bool coap_has_unprocessed_critical_options(const coap_pkt_t *pkt)
{
    for (unsigned i = 0; i < sizeof(pkt->opt_crit); ++i){
        if (pkt->opt_crit[i]) {
            return true;
        }
    }

    return false;
}

ssize_t coap_handle_req(coap_pkt_t *pkt, uint8_t *resp_buf, unsigned resp_buf_len,
                        coap_request_ctx_t *ctx)
{
    assert(ctx);

    switch (coap_get_type(pkt)) {
    case COAP_TYPE_CON:
    case COAP_TYPE_NON:
        /* could be a request ==> proceed */
        break;
    default:
        DEBUG_PUTS("coap_handle_req(): ignoring RST/ACK");
        return -EBADMSG;
    }

    if (coap_get_code_class(pkt) != COAP_REQ) {
        DEBUG_PUTS("coap_handle_req(): not a request --> ignore");
        return -EBADMSG;
    }

    if (coap_get_code_raw(pkt) == COAP_CODE_EMPTY) {
        /* we are not able to process a CON/NON message with an empty code,
         * so we reply with a RST, unless we got a multicast message */
        if (!sock_udp_ep_is_multicast(coap_request_ctx_get_local_udp(ctx))) {
            return coap_build_reply(pkt, COAP_CODE_EMPTY, resp_buf, resp_buf_len, 0);
        }
    }

    ssize_t retval = coap_tree_handler(pkt, resp_buf, resp_buf_len, ctx,
                                       coap_resources, coap_resources_numof);
    if (retval < 0) {
        /* handlers were not able to process this, so we reply with a RST,
         * unless we got a multicast message */
        if (!sock_udp_ep_is_multicast(coap_request_ctx_get_local_udp(ctx))) {
            return coap_build_reply(pkt, COAP_CODE_EMPTY, resp_buf, resp_buf_len, 0);
        }
    }

    return retval;
}

ssize_t coap_subtree_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                             coap_request_ctx_t *context)
{
    assert(context);
    coap_resource_subtree_t *subtree = coap_request_ctx_get_context(context);
    return coap_tree_handler(pkt, buf, len, context, subtree->resources,
                             subtree->resources_numof);
}

ssize_t coap_tree_handler(coap_pkt_t *pkt, uint8_t *resp_buf, unsigned resp_buf_len,
                          coap_request_ctx_t *ctx, const coap_resource_t *resources,
                          size_t resources_numof)
{
    coap_method_flags_t method_flag = coap_method2flag(coap_get_code_detail(pkt));

    uint8_t uri[CONFIG_NANOCOAP_URI_MAX];
    if (coap_get_uri_path(pkt, uri) <= 0) {
        return -EBADMSG;
    }
    DEBUG("nanocoap: URI path: \"%s\"\n", uri);

    for (unsigned i = 0; i < resources_numof; i++) {
        const coap_resource_t *resource = &resources[i];
        if (!(resource->methods & method_flag)) {
            continue;
        }

        int res = coap_match_path(resource, uri);
        if (res != 0) {
            continue;
        }

        ctx->resource = resource;
        return resource->handler(pkt, resp_buf, resp_buf_len, ctx);
    }

    return coap_build_reply(pkt, COAP_CODE_404, resp_buf, resp_buf_len, 0);
}

ssize_t coap_build_reply_header(coap_pkt_t *pkt, unsigned code,
                                void *buf, size_t len,
                                int ct,
                                void **payload, size_t *payload_len_max)
{
    uint8_t *bufpos = buf;
    uint32_t no_response;
    unsigned tkl = coap_get_token_len(pkt);
    size_t hdr_len = sizeof(coap_hdr_t) + tkl;
    uint8_t type = coap_get_type(pkt) == COAP_TYPE_CON
                 ? COAP_TYPE_ACK
                 : COAP_TYPE_NON;

    if (IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        /* Worst case: 2 byte extended token length field.
         * See https://www.rfc-editor.org/rfc/rfc8974#name-extended-token-length-tkl-f
         */
        hdr_len += 2;
    }

    if (hdr_len > len) {
        return -ENOBUFS;
    }

    bufpos += coap_build_hdr(buf, type, coap_get_token(pkt), tkl,
                             code, ntohs(pkt->hdr->id));

    if (coap_opt_get_uint(pkt, COAP_OPT_NO_RESPONSE, &no_response) == 0) {
        const uint8_t no_response_index = (code >> 5) - 1;
        /* If the handler code misbehaved here, we'd face UB otherwise */
        assume(no_response_index < 7);

        const uint8_t mask = 1 << no_response_index;
        if (no_response & mask) {
            if (payload) {
                *payload = NULL;
                *payload_len_max = 0;
                payload = NULL;
            }

            /* no-response requested, only send empty ACK or nothing */
            if (type != COAP_TYPE_ACK) {
                return 0;
            }
        }
    }

    if (IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        /* we need to update the header length with the actual one, as we may
         * have used less bytes for the extended token length fields as our
         * worst case assumption */
        hdr_len  = bufpos - (uint8_t *)buf;
    }

    if (payload) {
        if (ct >= 0) {
            bufpos += coap_put_option_ct(bufpos, 0, ct);
        }
        *bufpos++ = COAP_PAYLOAD_MARKER;
        *payload = bufpos;
        hdr_len  = bufpos - (uint8_t *)buf;
        *payload_len_max = len - hdr_len;
    }

    /* with the nanoCoAP API we can't detect the overflow before it happens */
    assert(hdr_len <= len);

    return hdr_len;
}

ssize_t coap_reply_simple(coap_pkt_t *pkt,
                          unsigned code,
                          uint8_t *buf, size_t len,
                          unsigned ct,
                          const void *payload, size_t payload_len)
{
    void *payload_start;
    size_t payload_len_max;

    ssize_t header_len = coap_build_reply_header(pkt, code, buf, len, ct,
                                                payload ? &payload_start : NULL,
                                                &payload_len_max);
    if (payload == NULL || header_len <= 0) {
        return header_len;
    }

    if (payload_len > payload_len_max) {
        return -ENOBUFS;
    }

    memcpy(payload_start, payload, payload_len);

    return header_len + payload_len;
}

ssize_t coap_build_empty_ack(coap_pkt_t *pkt, coap_hdr_t *ack)
{
    if (coap_get_type(pkt) != COAP_TYPE_CON) {
        return 0;
    }

    coap_build_hdr(ack, COAP_TYPE_ACK, NULL, 0,
                   COAP_CODE_EMPTY, ntohs(pkt->hdr->id));

    return sizeof(*ack);
}

ssize_t coap_build_reply(coap_pkt_t *pkt, unsigned code,
                         uint8_t *rbuf, unsigned rlen, unsigned payload_len)
{
    unsigned tkl = coap_get_token_len(pkt);
    unsigned type = COAP_TYPE_NON;

    if (!code) {
        /* if code is COAP_CODE_EMPTY (zero), assume Reset (RST) type.
         * RST message have no token */
        type = COAP_TYPE_RST;
        tkl = 0;
    }
    else if (coap_get_type(pkt) == COAP_TYPE_CON) {
        type = COAP_TYPE_ACK;
    }
    unsigned len = sizeof(coap_hdr_t) + tkl;

    if ((len + payload_len) > rlen) {
        return -ENOSPC;
    }

    uint32_t no_response;
    if (coap_opt_get_uint(pkt, COAP_OPT_NO_RESPONSE, &no_response) == 0) {

        const uint8_t no_response_index = (code >> 5) - 1;
        /* If the handler code misbehaved here, we'd face UB otherwise */
        assert(no_response_index < 7);

        const uint8_t mask = 1 << no_response_index;

        /* option contains bitmap of disinterest */
        if (no_response & mask) {
            switch (coap_get_type(pkt)) {
            case COAP_TYPE_NON:
                /* no response and no ACK */
                return 0;
            default:
                 /* There is an immediate ACK response, but it is an empty response */
                code = COAP_CODE_EMPTY;
                len = sizeof(coap_hdr_t);
                tkl = 0;
                payload_len = 0;
                break;
            }
        }
    }

    coap_build_hdr((coap_hdr_t *)rbuf, type, coap_get_token(pkt), tkl, code,
                   ntohs(pkt->hdr->id));
    len += payload_len;

    return len;
}

ssize_t coap_build_hdr(coap_hdr_t *hdr, unsigned type, const void *token,
                       size_t token_len, unsigned code, uint16_t id)
{
    assert(!(type & ~0x3));

    uint16_t tkl_ext;
    uint8_t tkl_ext_len, tkl;

    if (token_len > 268 && IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        tkl_ext_len = 2;
        tkl_ext = htons(token_len - 269); /* 269 = 255 + 14 */
        tkl = 14;
    }
    else if (token_len > 12 && IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        tkl_ext_len = 1;
        tkl_ext = token_len - 13;
        tkl = 13;
    }
    else {
        tkl = token_len;
        tkl_ext_len = 0;
    }

    memset(hdr, 0, sizeof(coap_hdr_t));
    hdr->ver_t_tkl = (COAP_V1 << 6) | (type << 4) | tkl;
    hdr->code = code;
    hdr->id = htons(id);

    if (tkl_ext_len) {
        memcpy(hdr + 1, &tkl_ext, tkl_ext_len);
    }

    /* Some users build a response packet in the same buffer that contained
     * the request. In this case, the argument token already points inside
     * the target, or more specifically, it is already at the correct place.
     * Having `src` and `dest` in `memcpy(dest, src, len)` overlap is
     * undefined behavior, so have to treat this explicitly. We could use
     * memmove(), but we know that either `src` and `dest` do not overlap
     * at all, or fully. So we can be a bit more efficient here. */
    void *token_dest = coap_hdr_data_ptr(hdr);
    if (token_dest != token) {
        memcpy(token_dest, token, token_len);
    }

    return sizeof(coap_hdr_t) + token_len + tkl_ext_len;
}

void coap_pkt_init(coap_pkt_t *pkt, uint8_t *buf, size_t len, size_t header_len)
{
    memset(pkt, 0, sizeof(coap_pkt_t));
    pkt->hdr = (coap_hdr_t *)buf;
    pkt->payload = buf + header_len;
    pkt->payload_len = len - header_len;
}

/*
 * Decodes a field value in an Option header, either option delta or length.
 *
 * val[in]               Value of a nybble of the first byte of the option
 *                       header. Upper nybble is coded length of delta; lower
 *                       nybble is coded length of value.
 * pkt_pos_ptr[in,out]   in: commonly, first byte of the option's value;
 *                       otherwise, first byte of extended delta/length header
 *                       out: next byte after the value or extended header
 * pkt_end[in]           next byte after all options
 *
 * return                field value
 * return                -ENOSPC if decoded val would extend beyond packet end
 * return                -EBADMSG if val is 0xF, suggesting the full byte is
 *                                the 0xFF payload marker
 */
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

static size_t _encode_uint(uint32_t *val)
{
    uint8_t *tgt = (uint8_t *)val;
    size_t size = 0;

    /* count number of used bytes */
    uint32_t tmp = *val;
    while (tmp) {
        size++;
        tmp >>= 8;
    }

    /* convert to network byte order */
    tmp = htonl(*val);

    /* copy bytewise, starting with first actually used byte */
    *val = 0;
    uint8_t *tmp_u8 = (uint8_t *)&tmp;
    tmp_u8 += (4 - size);
    for (unsigned n = 0; n < size; n++) {
        *tgt++ = *tmp_u8++;
    }

    return size;
}

/*
 * Writes CoAP Option header. Expected to be called twice to write an option:
 *
 * 1. write delta, using offset 1, shift 4
 * 2. write length, using offset n, shift 0, where n is the return value from
 *    the first invocation of this function
 *
 *     0   1   2   3   4   5   6   7
 *   +---------------+---------------+
 *   |  Option Delta | Option Length |   1 byte
 *   +---------------+---------------+
 *   /         Option Delta          /   0-2 bytes
 *   \          (extended)           \
 *   +-------------------------------+
 *   /         Option Length         /   0-2 bytes
 *   \          (extended)           \
 *   +-------------------------------+
 *
 *   From RFC 7252, Figure 8
 *
 * param[out] buf       addr of byte 0 of header
 * param[in]  offset    offset from buf to write any extended header
 * param[in]  shift     bit shift for byte 0 value
 * param[in]  value     delta/length value to write to header
 *
 * return     offset from byte 0 of next byte to write
 */
static unsigned _put_delta_optlen(uint8_t *buf, unsigned offset, unsigned shift,
                                  unsigned val)
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

size_t coap_put_option(uint8_t *buf, uint16_t lastonum, uint16_t onum, const void *odata, size_t olen)
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

static unsigned _size2szx(size_t size)
{
    assert(size <= 1024);

    /* We must wait to subtract the szx offset of 4 until after the assert below.
     * Input should be a power of two, but if not it may have a stray low order
     * '1' bit that would invalidate the subtraction. */
    unsigned szx = bitarithm_lsb(size);

    assert(szx >= 4);
    return szx - 4;
}

static unsigned _slicer2blkopt(coap_block_slicer_t *slicer, bool more)
{
    size_t blksize = slicer->end - slicer->start;
    size_t start = slicer->start;
    unsigned blknum = 0;

    while (start > 0) {
        start -= blksize;
        blknum++;
    }

    return (blknum << 4) | _size2szx(blksize) | (more ? 0x8 : 0);
}

int coap_get_block(coap_pkt_t *pkt, coap_block1_t *block, uint16_t option)
{
    block->more = coap_get_blockopt(pkt, option, &block->blknum, &block->szx);
    if (block->more >= 0) {
        block->offset = block->blknum << (block->szx + 4);
    }
    else {
        block->offset = 0;
    }

    return (block->more >= 0);
}

size_t coap_put_block1_ok(uint8_t *pkt_pos, coap_block1_t *block1, uint16_t lastonum)
{
    if (block1->more >= 1) {
        return coap_put_option_block1(pkt_pos, lastonum, block1->blknum, block1->szx, block1->more);
    }
    else {
        return 0;
    }
}

size_t coap_opt_put_block(uint8_t *buf, uint16_t lastonum, coap_block_slicer_t *slicer,
                          bool more, uint16_t option)
{
    slicer->opt = buf;

    return coap_opt_put_uint(buf, lastonum, option, _slicer2blkopt(slicer, more));
}

size_t coap_opt_put_string_with_len(uint8_t *buf, uint16_t lastonum, uint16_t optnum,
                                    const char *string, size_t len, char separator)
{
    if (len == 0) {
        return 0;
    }

    uint8_t *bufpos = buf;
    char *uripos = (char *)string;

    while (len) {
        size_t part_len;
        if (*uripos == separator) {
            uripos++;
        }
        uint8_t *part_start = (uint8_t *)uripos;

        while (len) {
            /* must decrement separately from while loop test to ensure
             * the value remains non-negative */
            len--;
            if ((*uripos == separator) || (*uripos == '\0')) {
                break;
            }
            uripos++;
        }

        part_len = (uint8_t *)uripos - part_start;

        /* Creates empty option if part for Uri-Path or Uri-Location contains only *
         * a trailing slash, except for root path ("/"). */
        if (part_len || ((separator == '/') && (lastonum == optnum))) {
            bufpos += coap_put_option(bufpos, lastonum, optnum, part_start, part_len);
            lastonum = optnum;
        }
    }

    return bufpos - buf;
}

size_t coap_opt_put_uri_pathquery(uint8_t *buf, uint16_t *lastonum, const char *uri)
{
    size_t len;
    const char *query = strchr(uri, '?');
    uint16_t _lastonum = lastonum ? *lastonum : 0;

    if (query) {
        len = (query == uri) ? 0 : (query - uri - 1);
    } else {
        len = strlen(uri);
    }

    size_t bytes_out = coap_opt_put_string_with_len(buf, _lastonum,
                                                    COAP_OPT_URI_PATH,
                                                    uri, len, '/');
    if (query) {
        buf += bytes_out;
        bytes_out += coap_opt_put_uri_query(buf, COAP_OPT_URI_PATH, query + 1);
        _lastonum = COAP_OPT_URI_QUERY;
    }
    else if (bytes_out) {
        _lastonum = COAP_OPT_URI_PATH;
    }

    if (lastonum) {
        *lastonum = _lastonum;
    }

    return bytes_out;
}

size_t coap_opt_put_uint(uint8_t *buf, uint16_t lastonum, uint16_t onum,
                         uint32_t value)
{
    unsigned uint_len = _encode_uint(&value);

    return coap_put_option(buf, lastonum, onum, (uint8_t *)&value, uint_len);
}

/* Common functionality for addition of an option */
static ssize_t _add_opt_pkt(coap_pkt_t *pkt, uint16_t optnum, const void *val,
                            size_t val_len)
{
    if (pkt->options_len >= CONFIG_NANOCOAP_NOPTS_MAX) {
        return -ENOSPC;
    }

    uint16_t lastonum = (pkt->options_len)
            ? pkt->options[pkt->options_len - 1].opt_num : 0;
    assert(optnum >= lastonum);

    /* calculate option length */
    uint8_t dummy[3] = { 0 };
    size_t optlen = _put_delta_optlen(dummy, 1, 4, optnum - lastonum);
    optlen += _put_delta_optlen(dummy, 0, 0, val_len);
    optlen += val_len;
    if (pkt->payload_len < optlen) {
        return -ENOSPC;
    }

    coap_put_option(pkt->payload, lastonum, optnum, val, val_len);

    pkt->options[pkt->options_len].opt_num = optnum;
    pkt->options[pkt->options_len].offset = pkt->payload - (uint8_t *)pkt->hdr;
    pkt->options_len++;
    pkt->payload += optlen;
    pkt->payload_len -= optlen;

    return optlen;
}

ssize_t coap_opt_add_chars(coap_pkt_t *pkt, uint16_t optnum, const char *chars,
                           size_t chars_len, char separator)
{
    /* chars_len denotes the length of the chars buffer and is
     * gradually decremented below while iterating over the buffer */
    if (!chars_len) {
        return 0;
    }

    char *uripos = (char *)chars;
    char *endpos = ((char *)chars + chars_len);
    size_t write_len = 0;

    while (chars_len) {
        size_t part_len;
        if (*uripos == separator) {
            uripos++;
        }
        uint8_t *part_start = (uint8_t *)uripos;

        while (chars_len) {
            /* must decrement separately from while loop test to ensure
             * the value remains non-negative */
            chars_len--;
            if ((*uripos == separator) || (uripos == endpos)) {
                break;
            }
            uripos++;
        }

        part_len = (uint8_t *)uripos - part_start;

        /* Creates empty option if part for Uri-Path or Uri-Location contains
         * only a trailing slash, except for root path ("/"). */
        if (part_len || ((separator == '/') && write_len)) {
            ssize_t optlen = _add_opt_pkt(pkt, optnum, part_start, part_len);
            if (optlen < 0) {
                return optlen;
            }
            write_len += optlen;
        }
    }

    return write_len;
}

ssize_t coap_opt_add_uri_query2(coap_pkt_t *pkt, const char *key, size_t key_len,
                                const char *val, size_t val_len)
{
    assert(pkt);
    assert(key);
    assert(key_len);
    assert(!val_len || (val && val_len));

    char qs[CONFIG_NANOCOAP_QS_MAX];
    /* length including '=' */
    size_t qs_len = key_len + ((val && val_len) ? (val_len + 1) : 0);

    /* test if the query string fits */
    if (qs_len > CONFIG_NANOCOAP_QS_MAX) {
        return -1;
    }

    memcpy(&qs[0], key, key_len);
    if (val && val_len) {
        qs[key_len] = '=';
        memcpy(&qs[key_len + 1], val, val_len);
    }

    return _add_opt_pkt(pkt, COAP_OPT_URI_QUERY, (uint8_t *)qs, qs_len);
}

ssize_t coap_opt_add_opaque(coap_pkt_t *pkt, uint16_t optnum, const void *val, size_t val_len)
{
    return _add_opt_pkt(pkt, optnum, val, val_len);
}

ssize_t coap_opt_add_uint(coap_pkt_t *pkt, uint16_t optnum, uint32_t value)
{
    uint32_t tmp = value;
    unsigned tmp_len = _encode_uint(&tmp);
    return _add_opt_pkt(pkt, optnum, (uint8_t *)&tmp, tmp_len);
}

ssize_t coap_opt_add_block(coap_pkt_t *pkt, coap_block_slicer_t *slicer,
                           bool more, uint16_t option)
{
    slicer->opt = pkt->payload;

    return coap_opt_add_uint(pkt, option, _slicer2blkopt(slicer, more));
}

ssize_t coap_opt_add_proxy_uri(coap_pkt_t *pkt, const char *uri)
{
    assert(pkt);
    assert(uri);

    return _add_opt_pkt(pkt, COAP_OPT_PROXY_URI, (uint8_t *)uri, strlen(uri));
}

ssize_t coap_opt_finish(coap_pkt_t *pkt, uint16_t flags)
{
    if (flags & COAP_OPT_FINISH_PAYLOAD) {
        if (!pkt->payload_len) {
            return -ENOSPC;
        }

        *pkt->payload++ = COAP_PAYLOAD_MARKER;
        pkt->payload_len--;
    }
    else {
        pkt->payload_len = 0;
    }

    return pkt->payload - (uint8_t *)pkt->hdr;
}

ssize_t coap_opt_remove(coap_pkt_t *pkt, uint16_t opt_num)
{
    assert(pkt);
    coap_optpos_t *optpos = pkt->options;
    coap_optpos_t *prev_opt = NULL;
    unsigned opt_count = pkt->options_len;
    uint8_t *start_old = NULL; /* First byte that is memmoved: content of next
                                * option, payload marker or end of message */
    uint8_t *start_new = NULL; /* memmove destination: content after rewritten
                                * next option header, or offset of removed option */

    while (opt_count--) {
        if ((start_old == NULL) && (optpos->opt_num == opt_num)) {
            unsigned new_hdr_len, old_hdr_len;
            int option_len;
            uint8_t *opt_start;
            uint16_t old_delta, new_delta;

            if (opt_count == 0) {
                /* this is the last option => use payload / end pointer as old start */
                start_old = (pkt->payload_len) ? pkt->payload - 1 : pkt->payload;
                start_new = (uint8_t *)pkt->hdr + optpos->offset;
                break;
            }

            if (prev_opt == NULL) {
                /* this is the first option => new_delta is just opt_num of next option */
                new_delta = optpos[1].opt_num;
            } else {
                new_delta = optpos[1].opt_num - prev_opt->opt_num;
            }
            prev_opt = optpos;
            optpos++;
            opt_count--;
            /* select start of next option */
            opt_start = (uint8_t *)pkt->hdr + optpos->offset;
            start_old = _parse_option(pkt, opt_start, &old_delta, &option_len);
            old_hdr_len = start_old - opt_start;

            /* select start of to be deleted option and set delta/length of next option */
            start_new = (uint8_t *)pkt->hdr + prev_opt->offset;
            *start_new = 0;
            /* write new_delta value to option header: 4 upper bits of header (shift 4) +
             * 1 or 2 optional bytes depending on delta value) */
            new_hdr_len = _put_delta_optlen(start_new, 1, 4, new_delta);
            /* write option length to option header: 4 lower bits of header (shift 0) +
             * 1 or 2 optional bytes depending of the length of the option */
            new_hdr_len = _put_delta_optlen(start_new, new_hdr_len, 0, option_len);
            start_new += new_hdr_len;

            /* account for header length change in next option */
            optpos->offset -= (new_hdr_len - old_hdr_len);
        }
        /* start_old implies start_new */
        if (start_old != NULL) {
            assert(start_new);
            /* adapt options array for removed option */
            memcpy(prev_opt, optpos, sizeof(*prev_opt));
            prev_opt->offset -= (start_old - start_new);
        }
        prev_opt = optpos;
        optpos++;
    }
    if (start_old != NULL) {
        size_t move_size = (pkt->payload - start_old) + pkt->payload_len;

        pkt->options_len--;
        if (move_size > 0) {
            memmove(start_new, start_old, move_size);
        }
        pkt->payload -= (start_old - start_new);
    }
    return (pkt->payload - ((uint8_t *)pkt->hdr)) + pkt->payload_len;
}

ssize_t coap_payload_put_bytes(coap_pkt_t *pkt, const void *data, size_t len)
{
    if (pkt->payload_len < len) {
        return -ENOSPC;
    }

    memcpy(pkt->payload, data, len);
    coap_payload_advance_bytes(pkt, len);

    return len;
}

ssize_t coap_payload_put_char(coap_pkt_t *pkt, char c)
{
    if (pkt->payload_len < 1) {
        return -ENOSPC;
    }

    *pkt->payload++ = c;
    pkt->payload_len--;

    return 1;
}

void coap_block_object_init(coap_block1_t *block, size_t blknum, size_t blksize,
                            int more)
{
    block->szx = _size2szx(blksize);
    block->blknum = blknum;
    block->more = more;
    block->offset = block->blknum << (block->szx + 4);
}

void coap_block_slicer_init(coap_block_slicer_t *slicer, size_t blknum,
                            size_t blksize)
{
    slicer->start = blknum * blksize;
    slicer->end = slicer->start + blksize;
    slicer->cur = 0;
}

void coap_block2_init(coap_pkt_t *pkt, coap_block_slicer_t *slicer)
{
    uint32_t blknum = 0;
    uint8_t szx = 0;

    /* Retrieve the block2 option from the client request */
    if (coap_get_blockopt(pkt, COAP_OPT_BLOCK2, &blknum, &szx) >= 0) {
        /* Use the client requested block size if it is smaller than our own
         * maximum block size */
        if (CONFIG_NANOCOAP_BLOCK_SIZE_EXP_MAX - 4 < szx) {
            szx = CONFIG_NANOCOAP_BLOCK_SIZE_EXP_MAX - 4;
        }
    }

    coap_block_slicer_init(slicer, blknum, coap_szx2size(szx));
}

bool coap_block_finish(coap_block_slicer_t *slicer, uint16_t option)
{
    assert(slicer->opt);

    /* The third parameter for _decode_value() points to the end of the header.
     * We don't know this position, but we know we can read the option because
     * it's already in the buffer. So just point past the option. */
    uint8_t *pos = slicer->opt + 1;
    uint16_t delta = _decode_value(*slicer->opt >> 4, &pos, slicer->opt + 3);

    bool more = slicer->cur > slicer->end;
    uint32_t blkopt = _slicer2blkopt(slicer, more);
    size_t olen = _encode_uint(&blkopt);

    /* ensure that we overwrite the dummy value set by coap_block2_init() */
    if (!olen) {
        olen = 1;
    }

    coap_put_option(slicer->opt, option - delta, option, (uint8_t *)&blkopt, olen);
    return more;
}

ssize_t coap_block2_build_reply(coap_pkt_t *pkt, unsigned code,
                                uint8_t *rbuf, unsigned rlen, unsigned payload_len,
                                coap_block_slicer_t *slicer)
{
    /* Check if the generated data filled the requested block */
    if (slicer->cur < slicer->start) {
        return coap_build_reply(pkt, COAP_CODE_BAD_OPTION, rbuf, rlen, 0);
    }
    coap_block2_finish(slicer);
    return coap_build_reply(pkt, code, rbuf, rlen, payload_len);
}

size_t coap_blockwise_put_char(coap_block_slicer_t *slicer, uint8_t *bufpos, char c)
{
    /* Only copy the char if it is within the window */
    if ((slicer->start <= slicer->cur) && (slicer->cur < slicer->end)) {
        *bufpos = c;
        slicer->cur++;
        return 1;
    }
    slicer->cur++;
    return 0;
}

size_t coap_blockwise_put_bytes(coap_block_slicer_t *slicer, uint8_t *bufpos,
                                const void *c, size_t len)
{
    size_t str_len = 0;    /* Length of the string to copy */

    /* Calculate start offset of the supplied string */
    size_t str_offset = (slicer->start > slicer->cur)
                        ? slicer->start - slicer->cur
                        : 0;

    /* Check for string before or beyond window */
    if ((slicer->cur >= slicer->end) || (str_offset > len)) {
        slicer->cur += len;
        return 0;
    }
    /* Check if string is over the end of the window */
    if ((slicer->cur + len) >= slicer->end) {
        str_len = slicer->end - (slicer->cur + str_offset);
    }
    else {
        str_len = len - str_offset;
    }

    /* Only copy the relevant part of the string to the buffer */
    memcpy(bufpos, (uint8_t *)c + str_offset, str_len);
    slicer->cur += len;
    return str_len;
}

ssize_t coap_well_known_core_default_handler(coap_pkt_t *pkt, uint8_t *buf, \
                                             size_t len, coap_request_ctx_t *context)
{
    (void)context;
    coap_block_slicer_t slicer;
    coap_block2_init(pkt, &slicer);
    uint8_t *payload = buf + coap_get_response_hdr_len(pkt);
    uint8_t *bufpos = payload;
    bufpos += coap_put_option_ct(bufpos, 0, COAP_FORMAT_LINK);
    bufpos += coap_opt_put_block2(bufpos, COAP_OPT_CONTENT_FORMAT, &slicer, 1);

    *bufpos++ = COAP_PAYLOAD_MARKER;

    for (unsigned i = 0; i < coap_resources_numof; i++) {
        if (i) {
            bufpos += coap_blockwise_put_char(&slicer, bufpos, ',');
        }
        bufpos += coap_blockwise_put_char(&slicer, bufpos, '<');
        unsigned url_len = strlen(coap_resources[i].path);
        bufpos += coap_blockwise_put_bytes(&slicer, bufpos,
                (uint8_t*)coap_resources[i].path, url_len);
        bufpos += coap_blockwise_put_char(&slicer, bufpos, '>');
    }

    unsigned payload_len = bufpos - payload;
    return coap_block2_build_reply(pkt, COAP_CODE_205, buf, len, payload_len,
                                   &slicer);
}

unsigned coap_get_len(coap_pkt_t *pkt)
{
    unsigned pktlen = sizeof(coap_hdr_t) + coap_get_token_len(pkt);
    if (pkt->payload) {
        pktlen += pkt->payload_len + 1;
    }
    return pktlen;
}

void coap_request_ctx_init(coap_request_ctx_t *ctx, sock_udp_ep_t *remote)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->remote = remote;
}

const char *coap_request_ctx_get_path(const coap_request_ctx_t *ctx)
{
    return ctx->resource->path;
}

void *coap_request_ctx_get_context(const coap_request_ctx_t *ctx)
{
    return ctx->resource->context;
}

uint32_t coap_request_ctx_get_tl_type(const coap_request_ctx_t *ctx)
{
#ifdef MODULE_GCOAP
    return ctx->tl_type;
#else
    (void)ctx;
    return 0;
#endif
}

const sock_udp_ep_t *coap_request_ctx_get_remote_udp(const coap_request_ctx_t *ctx)
{
    return ctx->remote;
}

const sock_udp_ep_t *coap_request_ctx_get_local_udp(const coap_request_ctx_t *ctx)
{
#if defined(MODULE_SOCK_AUX_LOCAL)
    return ctx->local;
#else
    (void)ctx;
    return NULL;
#endif
}
