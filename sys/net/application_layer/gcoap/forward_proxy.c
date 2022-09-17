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

#include "net/gcoap.h"
#include "net/gcoap/forward_proxy.h"
#include "uri_parser.h"
#include "net/nanocoap/cache.h"

#define ENABLE_DEBUG    0
#include "debug.h"

typedef struct {
    bool in_use;
    uint8_t req_etag_len;
    sock_udp_ep_t ep;
#if IS_USED(MODULE_NANOCOAP_CACHE)
    uint8_t req_etag[COAP_ETAG_LENGTH_MAX];
#endif
} client_ep_t;

static uint8_t proxy_req_buf[CONFIG_GCOAP_PDU_BUF_SIZE];
static client_ep_t _client_eps[CONFIG_GCOAP_REQ_WAITING_MAX];

static int _request_matcher_forward_proxy(gcoap_listener_t *listener,
                                          const coap_resource_t **resource,
                                          coap_pkt_t *pdu);
static ssize_t _forward_proxy_handler(coap_pkt_t* pdu, uint8_t *buf,
                                      size_t len, coap_request_ctx_t *ctx);

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

void gcoap_forward_proxy_init(void)
{
    gcoap_register_listener(&forward_proxy_listener);
}

static client_ep_t *_allocate_client_ep(const sock_udp_ep_t *ep)
{
    client_ep_t *cep;
    for (cep = _client_eps;
         cep < (_client_eps + CONFIG_GCOAP_REQ_WAITING_MAX);
         cep++) {
        if (!cep->in_use) {
            cep->in_use = true;
            cep->req_etag_len = 0U;
            memcpy(&cep->ep, ep, sizeof(*ep));
            return cep;
        }
    }
    return NULL;
}

static void _free_client_ep(client_ep_t *cep)
{
    memset(cep, 0, sizeof(*cep));
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

    pdu_len = gcoap_forward_proxy_request_process(pdu, remote);

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

    if (urip->port_len) {
        /* copy port string into scratch for atoi */
        memcpy(scratch, urip->port, urip->port_len);
        scratch[urip->port_len] = '\0';

        remote->port = atoi(scratch);

        if (remote->port == 0) {
          return false;
        }
    }
    else {
        remote->port = COAP_PORT;
    }

    return true;
}

static void _forward_resp_handler(const gcoap_request_memo_t *memo,
                                  coap_pkt_t* pdu,
                                  const sock_udp_ep_t *remote)
{
    (void) remote; /* this is the origin server */
    client_ep_t *cep = (client_ep_t *)memo->context;
    size_t buf_len = (pdu->payload - (uint8_t *)pdu->hdr) + pdu->payload_len;


    if (memo->state == GCOAP_MEMO_RESP) {
#if IS_USED(MODULE_NANOCOAP_CACHE)
        /* req_tag in cep is pre-processor guarded so we need to as well */
        if (cep->req_etag_len > 0) {
            uint8_t *resp_etag;

            /* check if we can just send 2.03 Valid instead */
            if ((cep->req_etag_len == coap_opt_get_opaque(pdu, COAP_OPT_ETAG, &resp_etag)) &&
                (memcmp(cep->req_etag, resp_etag, cep->req_etag_len) == 0)) {
                gcoap_resp_init(pdu, (uint8_t *)pdu->hdr, buf_len, COAP_CODE_VALID);
                coap_opt_add_opaque(pdu, COAP_OPT_ETAG, cep->req_etag, cep->req_etag_len);
                coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
            }
        }
        /* we do not need to check if valid came from upstream as this is already automatically
         * converted by the client-side to the cached response */
#endif
        /* else forward the response packet as-is to the client */
    }
    else if (memo->state == GCOAP_MEMO_RESP_TRUNC) {
        /* the response was truncated, so there should be enough space
         * to allocate an empty error message instead (with a potential Observe option) if not,
         * _listen_buf is _way_ too short ;-) */
        assert(buf_len >= (sizeof(*pdu->hdr) + 4U));
        gcoap_resp_init(pdu, (uint8_t *)pdu->hdr, buf_len, COAP_CODE_INTERNAL_SERVER_ERROR);
        coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
    }
    /* don't use buf_len here, in case the above `gcoap_resp_init`s changed `pdu` */
    gcoap_forward_proxy_dispatch((uint8_t *)pdu->hdr,
                                 (pdu->payload -
                                  (uint8_t *)pdu->hdr + pdu->payload_len),
                                 &cep->ep);
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
                /* add slack to maybe add an ETag on stale cache hit later, as is done in gcoap_req_send()
                 * (which we circumvented in _gcoap_forward_proxy_via_coap()) */
                if (coap_opt_add_opaque(pkt, COAP_OPT_ETAG, tmp, sizeof(tmp))) {
                    etag_added = true;
                }
            }
#if IS_USED(MODULE_NANOCOAP_CACHE)
            /* req_tag in cep is pre-processor guarded so we need to as well */
            if (opt.opt_num == COAP_OPT_ETAG) {
                if (cep->req_etag_len == 0) {
                    /* TODO: what to do on multiple ETags? */
                    cep->req_etag_len = (uint8_t)optlen;
                    memcpy(cep->req_etag, value, optlen);
                }
                /* skip original ETag of request, otherwise we might accidentally fill the cache
                 * with 2.03 Valid responses which would require additional handling.
                 * For upstream validation, gcoap_req_send() will add an ETag, if the response
                 * was in cache */
                continue;
            }
#else
            (void)cep;
#endif
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
    len += client_pkt->payload_len;

    return len;
}

static int _gcoap_forward_proxy_via_coap(coap_pkt_t *client_pkt,
                                         client_ep_t *client_ep,
                                         uri_parser_result_t *urip)
{
    coap_pkt_t pkt;
    sock_udp_ep_t origin_server_ep;

    ssize_t len;
    gcoap_request_memo_t *memo = NULL;

    if (!_parse_endpoint(&origin_server_ep, urip)) {
        return -EINVAL;
    }

    /* do not forward requests if they already exist, e.g., due to CON
       and retransmissions. In the future, the proxy should set an
       empty ACK message to stop the retransmissions of a client */
    gcoap_forward_proxy_find_req_memo(&memo, client_pkt, &origin_server_ep);
    if (memo) {
        DEBUG("gcoap_forward_proxy: request already exists, ignore!\n");
        _free_client_ep(client_ep);
        return 0;
    }

    unsigned token_len = coap_get_token_len(client_pkt);

    coap_pkt_init(&pkt, proxy_req_buf, CONFIG_GCOAP_PDU_BUF_SIZE,
                  sizeof(coap_hdr_t) + token_len);

    pkt.hdr->ver_t_tkl = client_pkt->hdr->ver_t_tkl;
    pkt.hdr->code = client_pkt->hdr->code;
    pkt.hdr->id = client_pkt->hdr->id;

    if (token_len) {
        memcpy(coap_get_token(&pkt), coap_get_token(client_pkt), token_len);
    }

    /* copy all options from client_pkt to pkt */
    len = _gcoap_forward_proxy_copy_options(&pkt, client_pkt, client_ep, urip);

    if (len == -EINVAL) {
        return -EINVAL;
    }

    len = gcoap_req_send((uint8_t *)pkt.hdr, len,
                         &origin_server_ep,
                         _forward_resp_handler, (void *)client_ep);
    return len;
}

int gcoap_forward_proxy_request_process(coap_pkt_t *pkt,
                                        const sock_udp_ep_t *client) {
    char *uri;
    uri_parser_result_t urip;
    ssize_t optlen = 0;

    client_ep_t *cep = _allocate_client_ep(client);

    if (!cep) {
        return -ENOMEM;
    }

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
    if (!strncmp("coap", urip.scheme, urip.scheme_len)) {
        int res = _gcoap_forward_proxy_via_coap(pkt, cep, &urip);
        if (res < 0) {
            _free_client_ep(cep);
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

/** @} */
