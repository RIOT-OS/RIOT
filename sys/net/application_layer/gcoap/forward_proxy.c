/*
 * Copyright (C) 2020 HAW Hamburg
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include <stdbool.h>

#include "event.h"
#include "kernel_defines.h"
#include "net/gcoap.h"
#include "net/gcoap/forward_proxy.h"
#include "uri_parser.h"
#include "net/nanocoap/cache.h"
#include "ztimer.h"

#include "forward_proxy_internal.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define CLIENT_EP_FLAGS_IN_USE          0x80
#define CLIENT_EP_FLAGS_RESP_TYPE_MASK  0x30
#define CLIENT_EP_FLAGS_RESP_TYPE_POS   4U
#define CLIENT_EP_FLAGS_ETAG_LEN_MASK   0x0f
#define CLIENT_EP_FLAGS_ETAG_LEN_POS    0U

extern kernel_pid_t forward_proxy_pid;

extern uint16_t gcoap_next_msg_id(void);
extern void gcoap_forward_proxy_post_event(void *arg);

static uint8_t proxy_req_buf[CONFIG_GCOAP_PDU_BUF_SIZE];
static client_ep_t _client_eps[CONFIG_GCOAP_REQ_WAITING_MAX];

static int _request_matcher_forward_proxy(gcoap_listener_t *listener,
                                          const coap_resource_t **resource,
                                          coap_pkt_t *pdu);
static ssize_t _forward_proxy_handler(coap_pkt_t* pdu, uint8_t *buf,
                                      size_t len, coap_request_ctx_t *ctx);
static bool _cep_in_use(client_ep_t *cep);
static void _cep_set_in_use(client_ep_t *cep);
static uint8_t _cep_get_response_type(client_ep_t *cep);
static void _cep_set_response_type(client_ep_t *cep, uint8_t resp_type);
static uint8_t _cep_get_req_etag_len(client_ep_t *cep);

/**
 * @brief   Store the given ETag in the given client endpoint
 * @param[out]  cep         client endpoint to store the ETag in
 * @param[in]   etag        ETag to store
 * @param[in]   etag_len    length of @p etag in bytes
 */
static void _cep_set_req_etag(client_ep_t *cep, const void *etag,
                              uint8_t etag_len);

const coap_resource_t forward_proxy_resources[] = {
    { "/", COAP_IGNORE, _forward_proxy_handler, NULL },
};

gcoap_listener_t forward_proxy_listener = {
    &forward_proxy_resources[0],
    ARRAY_SIZE(forward_proxy_resources),
    GCOAP_SOCKET_TYPE_UDP,
    NULL,
    NULL,
    _request_matcher_forward_proxy
};

static void _cep_set_timeout(client_ep_t *cep, ztimer_t *timer, uint32_t timeout_ms,
                             event_handler_t handler)
{
    assert(!ztimer_is_set(ZTIMER_MSEC, timer));
    timer->callback = gcoap_forward_proxy_post_event;
    timer->arg = &cep->event;
    cep->event.handler = handler;
    ztimer_set(ZTIMER_MSEC, timer, timeout_ms);
}


void gcoap_forward_proxy_init(void)
{
    gcoap_register_listener(&forward_proxy_listener);
    if (IS_ACTIVE(MODULE_GCOAP_FORWARD_PROXY_THREAD)) {
        gcoap_forward_proxy_thread_init();
    }
}

static client_ep_t *_allocate_client_ep(const sock_udp_ep_t *ep)
{
    client_ep_t *cep;
    for (cep = _client_eps;
         cep < (_client_eps + CONFIG_GCOAP_REQ_WAITING_MAX);
         cep++) {
        if (!_cep_in_use(cep)) {
            _cep_set_in_use(cep);
            _cep_set_req_etag(cep, NULL, 0);
            memcpy(&cep->ep, ep, sizeof(*ep));
            DEBUG("Client_ep is allocated %p\n", (void *)cep);
            return cep;
        }
    }
    return NULL;
}

static void _free_client_ep(client_ep_t *cep)
{
    ztimer_remove(ZTIMER_MSEC, &cep->empty_ack_timer);
    /* timer removed but event could be queued */
    cep->flags = 0;
    DEBUG("Client_ep is freed %p\n", (void *)cep);
}

static int _request_matcher_forward_proxy(gcoap_listener_t *listener,
                                          const coap_resource_t **resource,
                                          coap_pkt_t *pdu)
{
    (void) listener;

    char *offset;

    if (coap_get_proxy_uri(pdu, &offset) > 0) {
        *resource = &listener->resources[0];
        return GCOAP_RESOURCE_FOUND;
    }

    return GCOAP_RESOURCE_NO_PATH;
}

static ssize_t _forward_proxy_handler(coap_pkt_t *pdu, uint8_t *buf,
                                      size_t len, coap_request_ctx_t *ctx)
{
    int pdu_len;
    const sock_udp_ep_t *remote = coap_request_ctx_get_remote_udp(ctx);
    const sock_udp_ep_t *local = coap_request_ctx_get_local_udp(ctx);

    pdu_len = gcoap_forward_proxy_request_process(pdu, remote, local);

    /* Out of memory, reply with 5.00 */
    if (pdu_len == -ENOMEM) {
        return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
    }
    /* Proxy-Uri malformed, reply with 4.02 */
    else if (pdu_len == -EINVAL) {
        return gcoap_response(pdu, buf, len, COAP_CODE_BAD_OPTION);
    }
    /* scheme not supported */
    else if (pdu_len == -EPERM) {
        return gcoap_response(pdu, buf, len, COAP_CODE_PROXYING_NOT_SUPPORTED);
    }

    return pdu_len;
}

static bool _parse_endpoint(sock_udp_ep_t *remote,
                            uri_parser_result_t *urip)
{
    char scratch[8];
    ipv6_addr_t addr;
    remote->family = AF_INET6;

    /* support IPv6 only for now */
    if (!urip->ipv6addr) {
        return false;
    }

    /* check for interface */
    if (urip->zoneid) {
        /* only works with integer based zoneids */

        if (urip->zoneid_len > (ARRAY_SIZE(scratch) - 1)) {
            return false;
        }

        memcpy(scratch, urip->zoneid, urip->zoneid_len);

        scratch[urip->zoneid_len] = '\0';

        int pid = atoi(scratch);

        if (gnrc_netif_get_by_pid(pid) == NULL) {
            return false;
        }
        remote->netif = pid;
    }
    /* no interface present */
    else {
        if (gnrc_netif_numof() == 1) {
            /* assign the single interface found in gnrc_netif_numof() */
            remote->netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
        else {
            remote->netif = SOCK_ADDR_ANY_NETIF;
        }
    }

    /* parse destination address */
    if (ipv6_addr_from_buf(&addr, urip->ipv6addr, urip->ipv6addr_len) == NULL) {
        return false;
    }
    if ((remote->netif == SOCK_ADDR_ANY_NETIF) &&
        ipv6_addr_is_link_local(&addr)) {
        return false;
    }
    memcpy(&remote->addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));

    if (urip->port != 0) {
        remote->port = urip->port;
    }
    else if (!strncmp(urip->scheme, "coaps", 5)) {
        remote->port = COAPS_PORT;
    }
    else {
        remote->port = COAP_PORT;
    }

    return true;
}

static ssize_t _dispatch_msg(const void *buf, size_t len, sock_udp_ep_t *remote,
                             const sock_udp_ep_t *local)
{
    /* Yes it's not a request -- but turns out there is nothing in
     * gcoap_req_send that is actually request specific, especially if we
     * don't assign a callback. */
    ssize_t res = gcoap_req_send(buf, len, remote, local, NULL, NULL,
                                 GCOAP_SOCKET_TYPE_UDP);
    if (res <= 0) {
        DEBUG("gcoap_forward_proxy: unable to dispatch message: %d\n", -res);
    }
    return res;
}

static void _send_empty_ack(event_t *event)
{
    coap_hdr_t buf;
    client_ep_t *cep = container_of(event, client_ep_t, event);

    if (_cep_get_response_type(cep) != COAP_TYPE_ACK) {
        return;
    }
    _cep_set_response_type(cep, COAP_TYPE_CON);

    /* Flipping byte order as unlike in the other places where mid is
     * used, coap_build_hdr would actually flip it back */
    coap_build_hdr(&buf, COAP_TYPE_ACK, NULL, 0, 0, ntohs(cep->mid));
    _dispatch_msg(&buf, sizeof(buf), &cep->ep, &cep->proxy_ep);
}

static void _set_response_type(coap_pkt_t *pdu, uint8_t resp_type)
{
    coap_hdr_set_type(pdu->hdr, resp_type);
    if (resp_type == COAP_TYPE_CON) {
        pdu->hdr->id = htons(gcoap_next_msg_id());
    }
}

static void _forward_resp_handler(const gcoap_request_memo_t *memo,
                                  coap_pkt_t* pdu,
                                  const sock_udp_ep_t *remote)
{
    (void) remote; /* this is the origin server */
    client_ep_t *cep = (client_ep_t *)memo->context;
    size_t buf_len;

    /* No harm done in removing a timer that's not active */
    ztimer_remove(ZTIMER_MSEC, &cep->empty_ack_timer);
    buf_len = coap_get_total_len(pdu);
    assert(memo->state == GCOAP_MEMO_RESP ||
           memo->state == GCOAP_MEMO_RESP_TRUNC ||
           memo->state == GCOAP_MEMO_TIMEOUT);
    if (memo->state == GCOAP_MEMO_RESP) {
        uint8_t req_etag_len = _cep_get_req_etag_len(cep);

        if (req_etag_len > 0) {
            /* req_tag in cep is pre-processor guarded so we need to as well */
#if IS_USED(MODULE_NANOCOAP_CACHE)
            uint8_t *resp_etag;

            /* check if we can just send 2.03 Valid instead */
            if ((req_etag_len == coap_opt_get_opaque(pdu, COAP_OPT_ETAG, &resp_etag)) &&
                (memcmp(cep->req_etag, resp_etag, req_etag_len) == 0)) {
                uint32_t max_age;

                if (coap_opt_get_uint(pdu, COAP_OPT_MAX_AGE, &max_age) < 0) {
                    /* use default,
                     * see https://datatracker.ietf.org/doc/html/rfc7252#section-5.10.5 */
                    max_age = 60U;
                }
                gcoap_resp_init(pdu, (uint8_t *)pdu->hdr, buf_len, COAP_CODE_VALID);
                coap_opt_add_opaque(pdu, COAP_OPT_ETAG, cep->req_etag, req_etag_len);
                if (max_age != 60U) {
                    /* only include Max-Age option if it is not the default value */
                    coap_opt_add_uint(pdu, COAP_OPT_MAX_AGE, max_age);
                }
                coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
            }
#endif
        }
        _set_response_type(pdu, _cep_get_response_type(cep));
        /* we do not need to check if valid came from upstream as this is already automatically
         * converted by the client-side to the cached response */
        /* else forward the response packet as-is to the client */
    }
    else if (memo->state == GCOAP_MEMO_RESP_TRUNC) {
        /* the response was truncated, so there should be enough space
         * to allocate an empty error message instead (with a potential Observe option) if not,
         * _listen_buf is _way_ too short ;-) */
        assert(buf_len >= (sizeof(*pdu->hdr) + 4U));
        gcoap_resp_init(pdu, (uint8_t *)pdu->hdr, buf_len, COAP_CODE_INTERNAL_SERVER_ERROR);
        coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
        _set_response_type(pdu, _cep_get_response_type(cep));
    }
    else if (memo->state == GCOAP_MEMO_TIMEOUT) {
        /* send RST */
        gcoap_resp_init(pdu, (uint8_t *)pdu->hdr, buf_len, COAP_CODE_EMPTY);
        coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
    }
    /* don't use buf_len here, in case the above `gcoap_resp_init`s changed `pdu` */
    _dispatch_msg(pdu->hdr, coap_get_total_len(pdu), &cep->ep, &cep->proxy_ep);
    _free_client_ep(cep);
}

static int _gcoap_forward_proxy_add_uri_path(coap_pkt_t *pkt,
                                             uri_parser_result_t *urip)
{
    ssize_t res = coap_opt_add_chars(pkt, COAP_OPT_URI_PATH,
                                     urip->path, urip->path_len, '/');
    if (res < 0) {
        return -EINVAL;
    }

    if (urip->query) {
        res = coap_opt_add_chars(pkt, COAP_OPT_URI_QUERY,
                                 urip->query, urip->query_len, '&');
        if (res < 0) {
            return -EINVAL;
        }
    }

    return 0;
}

static int _gcoap_forward_proxy_copy_options(coap_pkt_t *pkt,
                                             coap_pkt_t *client_pkt,
                                             client_ep_t *cep,
                                             uri_parser_result_t *urip)
{
    /* copy all options from client_pkt to pkt */
    coap_optpos_t opt = {0, 0};
    uint8_t *value;
    bool uri_path_added = false;
    bool etag_added = false;

    for (int i = 0; i < client_pkt->options_len; i++) {
        ssize_t optlen = coap_opt_get_next(client_pkt, &opt, &value, !i);
        /* wrt to ETag option slack: we always have at least the Proxy-URI option in the client_pkt,
         * so we should hit at least once (and it's opt_num is also >= COAP_OPT_ETAG) */
        if (optlen >= 0) {
            if (IS_USED(MODULE_NANOCOAP_CACHE) && !etag_added && (opt.opt_num >= COAP_OPT_ETAG)) {
                static const uint8_t tmp[COAP_ETAG_LENGTH_MAX] = { 0 };
                /* add slack to maybe add an ETag on stale cache hit later, as is done in
                 * gcoap_req_send() (which we circumvented in _gcoap_forward_proxy_via_coap()) */
                if (coap_opt_add_opaque(pkt, COAP_OPT_ETAG, tmp, sizeof(tmp))) {
                    etag_added = true;
                }
            }
            if (IS_USED(MODULE_NANOCOAP_CACHE) && opt.opt_num == COAP_OPT_ETAG) {
                if (_cep_get_req_etag_len(cep) == 0) {
                    /* TODO: what to do on multiple ETags? */
                    _cep_set_req_etag(cep, value, optlen);
                }
                /* skip original ETag of request, otherwise we might accidentally fill the cache
                 * with 2.03 Valid responses which would require additional handling.
                 * For upstream validation, gcoap_req_send() will add an ETag, if the response
                 * was in cache */
                continue;
            }
            /* add URI-PATH before any larger opt num */
            if (!uri_path_added && (opt.opt_num > COAP_OPT_URI_PATH)) {
                if (_gcoap_forward_proxy_add_uri_path(pkt, urip) == -EINVAL) {
                    return -EINVAL;
                }
                uri_path_added = true;
            }
            /* skip PROXY-URI in new packet */
            if (opt.opt_num == COAP_OPT_PROXY_URI) {
                continue;
            }
            /* the actual copy operation */
            coap_opt_add_opaque(pkt, opt.opt_num, value, optlen);
        }
    }

    ssize_t len = coap_opt_finish(pkt,
                                  (client_pkt->payload_len ?
                                   COAP_OPT_FINISH_PAYLOAD :
                                   COAP_OPT_FINISH_NONE));

    /* copy payload from client_pkt to pkt */
    memcpy(pkt->payload, client_pkt->payload, client_pkt->payload_len);
    pkt->payload_len = client_pkt->payload_len;
    len += client_pkt->payload_len;

    return len;
}

int gcoap_forward_proxy_req_send(client_ep_t *cep)
{
    int len;
    if ((len = gcoap_req_send((uint8_t *)cep->pdu.hdr, coap_get_total_len(&cep->pdu),
                             &cep->server_ep, NULL, _forward_resp_handler, cep,
                             GCOAP_SOCKET_TYPE_UNDEF)) <= 0) {
        DEBUG("gcoap_forward_proxy_req_send(): gcoap_req_send failed %d\n", len);
        _free_client_ep(cep);
    }
    return len;
}

static int _gcoap_forward_proxy_via_coap(coap_pkt_t *client_pkt,
                                         client_ep_t *client_ep,
                                         uri_parser_result_t *urip)
{
    ssize_t len;
    gcoap_request_memo_t *memo = NULL;

    if (!_parse_endpoint(&client_ep->server_ep, urip)) {
        _free_client_ep(client_ep);
        return -EINVAL;
    }

    /* do not forward requests if they already exist, e.g., due to CON
       and retransmissions. In the future, the proxy should set an
       empty ACK message to stop the retransmissions of a client */
    gcoap_forward_proxy_find_req_memo(&memo, client_pkt, &client_ep->server_ep);
    if (memo) {
        DEBUG("gcoap_forward_proxy: request already exists, ignore!\n");
        _free_client_ep(client_ep);
        return 0;
    }

    if (coap_get_type(client_pkt) == COAP_TYPE_CON) {
        _cep_set_timeout(client_ep, &client_ep->empty_ack_timer,
                         CONFIG_GCOAP_FORWARD_PROXY_EMPTY_ACK_MS, _send_empty_ack);
    }

    unsigned token_len = coap_get_token_len(client_pkt);

    coap_pkt_init(&client_ep->pdu, proxy_req_buf, CONFIG_GCOAP_PDU_BUF_SIZE,
                  sizeof(coap_hdr_t) + token_len);

    client_ep->pdu.hdr->ver_t_tkl = client_pkt->hdr->ver_t_tkl;
    client_ep->pdu.hdr->code = client_pkt->hdr->code;
    client_ep->pdu.hdr->id = client_pkt->hdr->id;

    if (token_len) {
        memcpy(coap_get_token(&client_ep->pdu), coap_get_token(client_pkt), token_len);
    }

    /* copy all options from client_pkt to pkt */
    len = _gcoap_forward_proxy_copy_options(&client_ep->pdu, client_pkt, client_ep, urip);

    if (len < 0) {
        _free_client_ep(client_ep);
        return -EINVAL;
    }
    if (IS_USED(MODULE_GCOAP_FORWARD_PROXY_THREAD)) {
        /* WORKAROUND: DTLS communication is blocking the gcoap thread,
         * therefore the communication should be handled in the proxy thread */

        msg_t msg = {   .type = GCOAP_FORWARD_PROXY_MSG_SEND,
                        .content.ptr = client_ep
                    };
        msg_send(&msg, forward_proxy_pid);
    }
    else {
        len = gcoap_forward_proxy_req_send(client_ep);
    }

    return len;
}

int gcoap_forward_proxy_request_process(coap_pkt_t *pkt,
                                        const sock_udp_ep_t *client, const sock_udp_ep_t *local) {
    char *uri;
    uri_parser_result_t urip;
    ssize_t optlen = 0;

    client_ep_t *cep = _allocate_client_ep(client);
    cep->proxy_ep = local ? *local : (sock_udp_ep_t){ 0 };
    if (!cep) {
        return -ENOMEM;
    }

    cep->mid = pkt->hdr->id;
    _cep_set_response_type(
        cep,
        (coap_get_type(pkt) == COAP_TYPE_CON) ? COAP_TYPE_ACK : COAP_TYPE_NON
    );

    optlen = coap_get_proxy_uri(pkt, &uri);

    if (optlen < 0) {
        /* -ENOENT, -EINVAL */
        _free_client_ep(cep);
        return optlen;
    }

    int ures = uri_parser_process(&urip, (const char *) uri, optlen);

    /* cannot parse Proxy-URI option, or URI is relative */
    if (ures || (!uri_parser_is_absolute((const char *) uri, optlen))) {
        _free_client_ep(cep);
        return -EINVAL;
    }

    /* target is using CoAP */
    if (!strncmp("coap", urip.scheme, urip.scheme_len) ||
        !strncmp("coaps", urip.scheme, urip.scheme_len)) {
        /* client context ownership is passed to gcoap_forward_proxy_req_send() */
        int res = _gcoap_forward_proxy_via_coap(pkt, cep, &urip);
        if (res < 0) {
            return -EINVAL;
        }
    }
    /* no other scheme supported for now */
    else {
        _free_client_ep(cep);
        return -EPERM;
    }

    return 0;
}

static bool _cep_in_use(client_ep_t *cep)
{
    return cep->flags & CLIENT_EP_FLAGS_IN_USE;
}

static void _cep_set_in_use(client_ep_t *cep)
{
    cep->flags |= CLIENT_EP_FLAGS_IN_USE;
}

static uint8_t _cep_get_response_type(client_ep_t *cep)
{
    return (cep->flags & CLIENT_EP_FLAGS_RESP_TYPE_MASK) >> CLIENT_EP_FLAGS_RESP_TYPE_POS;
}

static void _cep_set_response_type(client_ep_t *cep, uint8_t resp_type)
{
    cep->flags &= ~CLIENT_EP_FLAGS_RESP_TYPE_MASK;
    cep->flags |= (resp_type << CLIENT_EP_FLAGS_RESP_TYPE_POS) & CLIENT_EP_FLAGS_RESP_TYPE_MASK;
}

static uint8_t _cep_get_req_etag_len(client_ep_t *cep)
{
    if (IS_USED(MODULE_NANOCOAP_CACHE)) {
        return (cep->flags & CLIENT_EP_FLAGS_ETAG_LEN_MASK) >> CLIENT_EP_FLAGS_ETAG_LEN_POS;
    }
    return 0;
}

static void _cep_set_req_etag(client_ep_t *cep, const void *etag,
                              uint8_t etag_len)
{
    (void)cep;
    (void)etag;
    (void)etag_len;
#if MODULE_NANOCOAP_CACHE
    if (etag_len <= COAP_ETAG_LENGTH_MAX) {
        cep->flags &= ~CLIENT_EP_FLAGS_ETAG_LEN_MASK;
        cep->flags |= (etag_len << CLIENT_EP_FLAGS_ETAG_LEN_POS)
                      & CLIENT_EP_FLAGS_ETAG_LEN_MASK;
        memcpy(cep->req_etag, etag, etag_len);
    }
#endif
}

/** @} */
