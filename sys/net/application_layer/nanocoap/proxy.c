/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap_proxy
 * @{
 *
 * @file
 * @brief       A trivial reverse proxy on top of nanocoap
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include <stdlib.h>
#include <string.h>

#include "bitfield.h"
#include "container.h"
#include "event.h"
#include "fmt.h"
#include "log.h"
#include "net/nanocoap.h"
#include "net/nanocoap_proxy.h"
#include "random.h"
#include "tiny_strerror.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CONFIG_NANOCOAP_PROXY_RESPONSE_HEADER_MAX_SIZE
#  define CONFIG_NANOCOAP_PROXY_RESPONSE_HEADER_MAX_SIZE 32
#endif

#ifndef CONFIG_NANOCOAP_PROXY_REQUEST_PACKET_TYPE
#  define CONFIG_NANOCOAP_PROXY_REQUEST_PACKET_TYPE COAP_TYPE_NON
#endif

static int _request_done_cb(void *arg, coap_pkt_t *pkt)
{
    const char *err_buf_overflown_msg = "resp hdr overflown";
    nanocoap_rproxy_forward_ctx_t *conn = arg;
    uint8_t rbuf[CONFIG_NANOCOAP_PROXY_RESPONSE_HEADER_MAX_SIZE];

    ssize_t hdr_len = nanocoap_server_build_separate(&conn->response_ctx,
                                                     rbuf, sizeof(rbuf),
                                                     coap_get_code_raw(pkt),
                                                     COAP_TYPE_NON, random_uint32());
    if (hdr_len < 0) {
        return hdr_len;
    }

    uint8_t *pktpos = rbuf + hdr_len;
    uint8_t *pktend = rbuf + sizeof(rbuf);
    uint16_t lastonum = 0;
    for (unsigned i = 0; i < pkt->options_len; i++) {
        bool forward_option = false;
        uint16_t onum = pkt->options[i].opt_num;
        switch (onum) {
        /* convert URI-path: trim off prefix uses to identify target endpoint */
        case COAP_OPT_BLOCK1:
        case COAP_OPT_BLOCK2:
            /* Block1 and Block2 are critical, so we cannot silently elide them.
             * The are also unsafe to forward, so normally we would give up
             * rather than forwarding them as opaque. But, yolo */
            forward_option = true;
            break;
        default:
            if (onum & 0x2) {
                /* Option is not safe for forwarding. If it is critical, we
                 * give up. Otherwise we just drop the option and continue. */
                if (onum & 0x1) {
                    /* option is critical, we cannot just elide it */
                    DEBUG("[reverse proxy] Critical Option %u in response not handled --> 5.00\n", (unsigned)onum);
                    char str_onum[8];
                    return nanocoap_server_send_separate(&conn->response_ctx,
                                                         COAP_CODE_INTERNAL_SERVER_ERROR,
                                                         COAP_TYPE_NON, str_onum,
                                                         fmt_u16_dec(str_onum, onum));
                }
            }
            else {
                forward_option = true;
            }
            break;
        }

        if (forward_option) {
            /* option is safe for forwarding, we copy-paste it */
            coap_optpos_t pos = pkt->options[i];
            uint8_t *optval;
            size_t optlen = coap_opt_get_next(pkt, &pos, &optval, false);
            /* worst case option header is 5 bytes + option length */
            if (pktpos + 5 + optlen > pktend) {
                /* option (potentially) overflows buffer */
                DEBUG_PUTS("[reverse proxy] Buffer too small for CoAP Option --> 5.00");
                return nanocoap_server_send_separate(&conn->response_ctx,
                                                     COAP_CODE_INTERNAL_SERVER_ERROR,
                                                     COAP_TYPE_NON, err_buf_overflown_msg,
                                                     strlen(err_buf_overflown_msg));
            }
            pktpos += coap_put_option(pktpos, lastonum, onum, optval, optlen);
            lastonum = onum;
        }
    }

    if (pkt->payload_len) {
        if (pktpos + 1 > pktend) {
            return nanocoap_server_send_separate(&conn->response_ctx,
                                                 COAP_CODE_INTERNAL_SERVER_ERROR,
                                                 COAP_TYPE_NON, err_buf_overflown_msg,
                                                 strlen(err_buf_overflown_msg));
        }
        *pktpos++ = 0xff;
    }

    iolist_t data = {
        .iol_base = pkt->payload,
        .iol_len  = pkt->payload_len,
    };

    iolist_t head = {
        .iol_next = &data,
        .iol_base = rbuf,
        .iol_len = (uintptr_t)pktpos - (uintptr_t)rbuf,
    };

    DEBUG_PUTS("[reverse proxy] Forwarding reply");
    return nanocoap_server_sendv_separate(&conn->response_ctx, &head);
}

static void _disconnect(nanocoap_rproxy_forward_ctx_t *conn)
{
    nanocoap_sock_close(&conn->client);
    bf_unset(conn->proxy->forwards_used, index_of(conn->proxy->forwards, conn));

}

static void _forward_request_handler(event_t *ev)
{
    nanocoap_rproxy_forward_ctx_t *conn = container_of(ev, nanocoap_rproxy_forward_ctx_t, ev);
    DEBUG_PUTS("[reverse proxy] Forwarding request ...");
    ssize_t err = nanocoap_sock_request_cb(&conn->client, &conn->req, _request_done_cb, conn);
    DEBUG("[reverse proxy] Forwarded request: %s\n", (err < 0) ? tiny_strerror(err) : "OK");
    if (err < 0) {
        const char *errmsg = tiny_strerror(err);
        nanocoap_server_send_separate(&conn->response_ctx,
                                      COAP_CODE_INTERNAL_SERVER_ERROR,
                                      COAP_TYPE_NON, errmsg, strlen(errmsg));
    }
    _disconnect(conn);
}

static bool _is_duplicate(nanocoap_rproxy_ctx_t *ctx, coap_pkt_t *pkt, const coap_request_ctx_t *req)
{
    for (unsigned i = 0; i < CONFIG_NANOCOAP_RPROXY_PARALLEL_FORWARDS; i++) {
        if (bf_isset(ctx->forwards_used, i)) {
            if (nanocoap_is_duplicate_in_separate_ctx(&ctx->forwards[i].response_ctx, pkt, req)) {
                return true;
            }
        }
    }

    return false;
}

static int _connect(nanocoap_rproxy_forward_ctx_t **dest, nanocoap_rproxy_ctx_t *ctx,
                    const char *ep, size_t ep_len)
{
    if (ep_len >= sizeof(ctx->forwards[0].ep)) {
        return -EINVAL;
    }

    unsigned idx_free = CONFIG_NANOCOAP_RPROXY_PARALLEL_FORWARDS;
    for (unsigned i = 0; i < CONFIG_NANOCOAP_RPROXY_PARALLEL_FORWARDS; i++) {
        if (!bf_isset(ctx->forwards_used, i)) {
            idx_free = i;
            break;
        }
    }

    if (idx_free >= CONFIG_NANOCOAP_RPROXY_PARALLEL_FORWARDS) {
        return -EAGAIN;
    }

    char uri[CONFIG_NANOCOAP_URI_MAX];
    size_t scheme_len = strlen(ctx->scheme);
    if (scheme_len + ep_len + 1 > sizeof(uri)) {
        /* not enough space in uri to write scheme + ep + terminating zero byte */
        return -EOVERFLOW;
    }
    char *pos = uri;
    memcpy(pos, ctx->scheme, scheme_len);
    pos += scheme_len;
    memcpy(pos, ep, ep_len);
    pos += ep_len;
    *pos = '\0';

    nanocoap_rproxy_forward_ctx_t *conn = &ctx->forwards[idx_free];

    int err = nanocoap_sock_url_connect(uri, &conn->client);
    if (err) {
        LOG_WARNING("Reverse proxy: Failed to connect to \"%s\"\n", uri);
        return -EHOSTUNREACH;
    }

    DEBUG("[reverse proxy] Connected to \"%s\"\n", uri);

    bf_set(ctx->forwards_used, idx_free);
    memcpy(conn->ep, ep, ep_len);
    conn->ep[ep_len] = '\0';
    conn->proxy = ctx;

    *dest = conn;
    return 0;
}

ssize_t nanocoap_rproxy_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                                coap_request_ctx_t *ctx)
{
    nanocoap_rproxy_ctx_t *proxy = coap_request_ctx_get_context(ctx);

    if (_is_duplicate(proxy, pkt, ctx)) {
        DEBUG_PUTS("[reverse proxy] Got duplicate --> empty ACK (if needed)");
        return coap_reply_empty_ack(pkt, buf, len);
    }

    char uri[CONFIG_NANOCOAP_URI_MAX] = "";
    coap_get_uri_path(pkt, (void *)uri);

    const char *my_path = coap_request_ctx_get_path(ctx);
    /* If this does not hold the subtree matching should not have happened */
    assume(strlen(uri) >= strlen(my_path));

    const char *uri_remainder = uri + strlen(my_path);
    const char *ep = proxy->target_ep;
    const char *ep_end;

    if (ep) {
        /* endpoint provided by the application */
        ep_end = ep + strlen(ep);
    }
    else {
        /* endpoint not provided by application --> target to parse from
         * the request URI */
        ep = uri_remainder;
        if (*ep != '/') {
            DEBUG_PUTS("[reverse proxy] No endpoint specified --> 4.04");
            return coap_reply_simple(pkt, COAP_CODE_404, buf, len, COAP_FORMAT_NONE,
                                     NULL, 0);
        }
        ep++;
        ep_end = strchr(ep, '/');
        if (ep_end) {
            uri_remainder = ep_end;
        }
        else {
            ep_end = ep + strlen(ep);
        }
    }

    nanocoap_rproxy_forward_ctx_t *conn = NULL;
    {
        int err = _connect(&conn, proxy, ep, (uintptr_t)ep_end - (uintptr_t)ep);
        const char *msg_invalid_ep = "invalid ep";
        switch (err) {
        case 0:
            /* no error */
            break;
        case -EAGAIN:
            DEBUG_PUTS("[reverse proxy] No free slot for connection --> 4.29");
            return coap_reply_simple(pkt, COAP_CODE_TOO_MANY_REQUESTS, buf, len,
                                     COAP_FORMAT_NONE, NULL, 0);
        case -EINVAL:
            DEBUG_PUTS("[reverse proxy] Invalid EP --> 4.00");
            return coap_reply_simple(pkt, COAP_CODE_BAD_REQUEST, buf, len,
                                     COAP_FORMAT_NONE, msg_invalid_ep, strlen(msg_invalid_ep));
        case -EOVERFLOW:
            DEBUG_PUTS("[reverse proxy] URI buffer too small to connect --> 5.00");
            return coap_reply_simple(pkt, COAP_CODE_BAD_REQUEST, buf, len,
                                     COAP_FORMAT_NONE, NULL, 0);
        case -EHOSTUNREACH:
            DEBUG_PUTS("[reverse proxy] Failed to connect --> 4.04");
            return coap_reply_simple(pkt, COAP_CODE_404, buf, len,
                                     COAP_FORMAT_NONE, NULL, 0);
        default:
            DEBUG("[reverse proxy] Unhandled error %d in _connect --> 5.00\n", err);
            return coap_reply_simple(pkt, COAP_CODE_BAD_REQUEST, buf, len,
                                     COAP_FORMAT_NONE, NULL, 0);
        }
    }

    if (nanocoap_server_prepare_separate(&conn->response_ctx, pkt, ctx)) {
        DEBUG_PUTS("[reverse proxy] Failed to prepare response context --> RST");
        /* Send a RST message: We don't support extended tokens here */
        return coap_build_reply(pkt, 0, buf, len, 0);
    }

    ssize_t hdr_len = nanocoap_sock_build_pkt(&conn->client, &conn->req,
                                              conn->buf, sizeof(conn->buf),
                                              CONFIG_NANOCOAP_PROXY_REQUEST_PACKET_TYPE,
                                              coap_get_token(pkt), coap_get_token_len(pkt),
                                              coap_get_code_raw(pkt));
    if (hdr_len < 0) {
        DEBUG("[reverse proxy] Failed to build req to forward: %" PRIdSIZE " --> 5.00", hdr_len);
        _disconnect(conn);
        return coap_reply_simple(pkt, COAP_CODE_INTERNAL_SERVER_ERROR,
                                 buf, len, COAP_FORMAT_NONE, NULL, 0);
    }

    uint8_t *pktpos = conn->buf + hdr_len;
    uint8_t *pktend = conn->buf + len;
    uint16_t lastonum = 0;
    for (unsigned i = 0; i < pkt->options_len; i++) {
        bool forward_option = false;
        uint16_t onum = pkt->options[i].opt_num;
        switch (onum) {
        /* convert URI-path: trim off prefix uses to identify target endpoint */
        case COAP_OPT_URI_PATH:
            if (uri_remainder) {
                /* CoAP option header can be 3 bytes: Even if Uri-Path (11)
                 * would be the first option (Option Delta would be 11), it
                 * fits into 4 bits. Option Length can be extended (+2 bytes
                 * in worst case */
                if (pktpos + 3 + strlen(uri_remainder) > pktend) {
                    /* URI-Path (potentially) overflows buffer */
                    DEBUG_PUTS("[reverse proxy] Buffer too small for URI --> 5.00");
                    _disconnect(conn);
                    return coap_reply_simple(pkt, COAP_CODE_INTERNAL_SERVER_ERROR,
                                             buf, len, COAP_FORMAT_NONE, NULL, 0);
                }
                pktpos += coap_opt_put_uri_pathquery(pktpos, &lastonum, uri_remainder);
            }
            break;
        case COAP_OPT_BLOCK1:
        case COAP_OPT_BLOCK2:
            /* Block1 and Block2 are critical, so we cannot silently elide them.
             * The are also unsafe to forward, so normally we would give up
             * rather than forwarding them as opaque. But, yolo */
            forward_option = true;
            break;
        default:
            if (onum & 0x2) {
                /* Option is not safe for forwarding. If it is critical, we
                 * give up. Otherwise we just drop the option and continue. */
                if (onum & 0x1) {
                    /* option is critical, we cannot just elide it */
                    DEBUG("[reverse proxy] Option %u not handled --> 4.02\n", (unsigned)onum);
                    _disconnect(conn);
                    char str_onum[8];
                    return coap_reply_simple(pkt, COAP_CODE_BAD_OPTION,
                                             buf, len, COAP_FORMAT_NONE,
                                             str_onum, fmt_u32_dec(str_onum, onum));

                }
            }
            else {
                forward_option = true;
            }
            break;
        }

        if (forward_option) {
            /* option is safe for forwarding, we copy-paste it */
            coap_optpos_t pos = pkt->options[i];
            uint8_t *optval;
            size_t optlen = coap_opt_get_next(pkt, &pos, &optval, false);
            /* worst case option header is 5 bytes + option length */
            if (pktpos + 5 + optlen > pktend) {
                /* option (potentially) overflows buffer */
                _disconnect(conn);
                DEBUG_PUTS("[reverse proxy] Buffer too small for CoAP Option --> 5.00");
                return coap_reply_simple(pkt, COAP_CODE_INTERNAL_SERVER_ERROR,
                                         buf, len, COAP_FORMAT_NONE, NULL, 0);
            }
            DEBUG("[reverse proxy] Adding option %u, sized %u\n",
                  (unsigned)onum, (unsigned)optlen);
            pktpos += coap_put_option(pktpos, lastonum, onum, optval, optlen);
            lastonum = onum;
        }
    }

    conn->req.payload = pktpos;
    conn->req.payload_len = pkt->payload_len;
    if (pkt->payload_len) {
        if (pktpos + 1 + pkt->payload_len > pktend) {
            _disconnect(conn);
            DEBUG_PUTS("[reverse proxy] Buffer too small for payload --> 5.00");
            return coap_reply_simple(pkt, COAP_CODE_INTERNAL_SERVER_ERROR,
                                     buf, len, COAP_FORMAT_NONE, NULL, 0);
        }
        *pktpos++ = 0xff; /* payload marker */
        conn->req.payload = pktpos;
        memcpy(pktpos, pkt->payload, pkt->payload_len);
    }

    assume(conn->req.payload + conn->req.payload_len <= pktend);
    conn->ev.handler = _forward_request_handler;
    DEBUG("[reverse proxy] Scheduled forwarding request for URI \"%s\" to %.*s\n",
          uri_remainder, (int)((uintptr_t)ep_end - (uintptr_t)ep), ep);
    event_post(proxy->evq, &conn->ev);
    DEBUG_PUTS("[reverse proxy] Request to forward queued --> ACK (if needed)");
    return coap_reply_empty_ack(pkt, buf, len);
}
