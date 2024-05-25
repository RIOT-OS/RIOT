/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#include "fmt.h"
#include "log.h"
#include "mutex.h"
#include "net/credman.h"
#include "net/gcoap.h"
#include "net/af.h"
#include "net/dns/cache.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "net/sock/dns.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"
#include "random.h"
#include "string_utils.h"
#include "uri_parser.h"
#include "ut_process.h"

#include "net/gcoap/dns.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Context for a DNS query-response-pair.
 */
typedef struct {
    /**
     * @brief   Synchronization mutex to wait for response
     */
    mutex_t resp_wait;
    /**
     * @brief The CoAP request packet
     *
     * Only needs to have coap_pkt_t::payload and coap_pkt_t::payload_len
     * initialized.
     */
    coap_pkt_t *pkt;
#if IS_USED(MODULE_DNS_CACHE) || defined(DOXYGEN)
    /**
     * @brief   The queried hostname
     *
     * Only required for DNS caching and thus only available with module @ref net_dns_cache
     */
    const char *domain_name;
#endif
    void *dns_buf;          /**< The buffer for the DNS message exchange */
    void *addr_out;         /**< Pointer to the resulting address */
    /**
     * @brief   Status for the DNS message exchange
     *
     * - length of _req_ctx_t::addr_out in bytes on success
     * - -EBADMSG, when receiving erroneous response or response containing
     * - -EDESTADDRREQ, if CoAP response was received from an unexpected remote.
     * - -EINVAL, when block-wise transfer can not be completed.
     * - -ENOBUFS, if length of received CoAP body is greater than
     *   @ref CONFIG_DNS_MSG_LEN.
     * - -ENOMSG, if CoAP response did not contain a DNS response.
     * - -ETIMEDOUT, if CoAP request timed out.
     */
    int res;
    uint8_t dns_buf_len;    /**< Length of _req_ctx_t::dns_buf */
    int8_t family;          /**< Address family to resolve */
    /**
     * @brief The current block number for block-wise transfer
     *
     * Leave unset on function call.
     */
    uint8_t cur_blk_num;
#if IS_USED(MODULE_GCOAP_DTLS) || defined(DOXYGEN)
    /**
     * @brief   Request tag to rule out potential request reordering attacks
     */
    uint16_t req_tag;
#endif
} _req_ctx_t;

typedef struct {
    credman_type_t type;    /**< Type of the credential */
    credman_tag_t tag;      /**< Tag of the credential */
} _cred_t;

static mutex_t _client_mutex = MUTEX_INIT;
static char _uri[CONFIG_GCOAP_DNS_SERVER_URI_LEN];
static char _proxy[CONFIG_GCOAP_DNS_SERVER_URI_LEN];
static uri_parser_result_t _uri_comp;
static sock_udp_ep_t _remote;
#if IS_USED(MODULE_GCOAP_DTLS)
static _cred_t _creds[CONFIG_GCOAP_DNS_CREDS_MAX] = { 0 };
#endif
static uint16_t _req_tag;

static inline bool _dns_server_uri_isset(void);
#if IS_USED(MODULE_GCOAP_DTLS)
static void _remove_cred(sock_dtls_t *sock, _cred_t *cred);
#else
#define _remove_cred(sock, cred)     (void)(sock); (void)(cred)
#endif
static inline bool _is_proxied(void);
static int _add_init_block2_opt(coap_pkt_t *pdu);
static int _add_remaining_options(coap_pkt_t *pdu, const char *proxy_uri, _req_ctx_t *context);
static int _dns_query(const char *domain_name, _req_ctx_t *req_ctx);
static ssize_t _send(const void *buf, size_t len, const sock_udp_ep_t *remote,
                     bool lock_resp_wait, _req_ctx_t *context, gcoap_socket_type_t tl_type);

int gcoap_dns_query(const char *domain_name, void *addr_out, int family)
{
    int res;

    if ((res = dns_cache_query(domain_name, addr_out, family)) > 0) {
        return res;
    }

    static uint8_t coap_buf[CONFIG_GCOAP_DNS_PDU_BUF_SIZE];
    static uint8_t dns_buf[CONFIG_DNS_MSG_LEN];
    coap_pkt_t pdu;
    _req_ctx_t req_ctx = {
        .resp_wait = MUTEX_INIT,
        .pkt = &pdu,
        .dns_buf = dns_buf,
        .addr_out = addr_out,
        .family = family,
    };
    mutex_lock(&_client_mutex);
    pdu.payload = coap_buf;
    pdu.payload_len = sizeof(coap_buf);
    res = _dns_query(domain_name, &req_ctx);
    if (res > 0) {
        /* wait for req_ctx.addr_out to be set */
        mutex_lock(&req_ctx.resp_wait);
        res = req_ctx.res;
    }
    mutex_unlock(&_client_mutex);
    return res;
}

int gcoap_dns_server_uri_set(const char *uri)
{
    int res;

    if (!uri) {
        mutex_lock(&_client_mutex);
        _uri[0] = '\0';
        mutex_unlock(&_client_mutex);
        return 0;
    }
    if (IS_USED(MODULE_GCOAP_DTLS)) {
        /* reinitialize request tag */
        _req_tag = (uint16_t)random_uint32();
    }
    if ((strncmp(uri, "coap:", sizeof("coap:") - 1) != 0) &&
        /* if gcoap_dtls is used, URIs not starting with coaps: are also invalid */
        (!IS_USED(MODULE_GCOAP_DTLS) || (strncmp(uri, "coaps:", sizeof("coaps:") - 1) != 0))) {
        return -EINVAL;
    }
    res = strlen(uri);
    if ((((unsigned)res + 1) >= CONFIG_GCOAP_DNS_SERVER_URI_LEN)) {
        return -ENOBUFS;
    }
    mutex_lock(&_client_mutex);
    strcpy(_uri, uri);
    if (uri_parser_process(&_uri_comp, _uri, res) < 0) {
        res = -EINVAL;
        _uri[0] = '\0';
    }
    mutex_unlock(&_client_mutex);
    return res;
}

bool gcoap_dns_server_uri_is_set(void)
{
    mutex_lock(&_client_mutex);
    bool res = _dns_server_uri_isset();
    mutex_unlock(&_client_mutex);
    return res;
}

ssize_t gcoap_dns_server_uri_get(char *uri, size_t uri_len)
{
    ssize_t res = 0;
    mutex_lock(&_client_mutex);
    if (_dns_server_uri_isset()) {
        res = strlen(_uri);
        if ((size_t)(res + 1) > uri_len) {
            /* account for trailing \0 */
            res = -ENOBUFS;
        }
        else {
            strcpy(uri, _uri);
        }
    }
    mutex_unlock(&_client_mutex);
    return res;
}

void gcoap_dns_cred_reset(void)
{
#if IS_USED(MODULE_GCOAP_DTLS)
    sock_dtls_t *sock = gcoap_get_sock_dtls();
    for (unsigned i = 0; i < ARRAY_SIZE(_creds); i++) {
        if (_creds[i].type != CREDMAN_TYPE_EMPTY) {
            _remove_cred(sock, &_creds[i]);
        }
    }
#endif
}

int gcoap_dns_cred_add(credman_credential_t *creds)
{
    _cred_t *c = NULL;

    if (!IS_USED(MODULE_GCOAP_DTLS)) {
        return -ENOTSUP;
    }
    assert(creds != NULL);
#if IS_USED(MODULE_GCOAP_DTLS)
    for (unsigned i = 0; i < ARRAY_SIZE(_creds); i++) {
        if (!c &&
            (((_creds[i].type == creds->type) && (_creds[i].tag == creds->tag)) ||
             (_creds[i].type == CREDMAN_TYPE_EMPTY))) {
            c = &_creds[i];
            break;
        }
    }
#endif
    if (c == NULL) {
        DEBUG("gcoap_dns: no space to store credential tag\n");
        return -ENOMEM;
    }
    int res = credman_add(creds);
    if ((res < 0) && (res != CREDMAN_EXIST)) {
        /* ignore duplicate credentials */
        DEBUG("gcoap_dns: cannot add credential to system: %d\n", res);
        return -EBADF;
    }
    if (res == CREDMAN_OK) {
#if IS_USED(MODULE_GCOAP_DTLS)
        /* functions used in here are only available with module gcoap_dtls, so guard this
         * section */
        sock_dtls_t *gcoap_sock_dtls = gcoap_get_sock_dtls();

        res = sock_dtls_add_credential(gcoap_sock_dtls, creds->tag);
        if (res < 0) {
            DEBUG("gcoap_dns: cannot add credential to GCoAPs DTLS sock\n");
            return -EBADF;
        }
#endif
        c->tag = creds->tag;
        c->type = creds->type;
    }
    return 0;
}

void gcoap_dns_cred_remove(credman_tag_t tag, credman_type_t type)
{
#if IS_USED(MODULE_GCOAP_DTLS)
    sock_dtls_t *sock = gcoap_get_sock_dtls();
    for (unsigned i = 0; i < ARRAY_SIZE(_creds); i++) {
        if ((_creds[i].tag == tag) && (_creds[i].type == type)) {
            _remove_cred(sock, &_creds[i]);
            break;
        }
    }
#else
    (void)tag;
    (void)type;
#endif
}

void gcoap_dns_server_proxy_reset(void)
{
    if (IS_USED(MODULE_GCOAP_DNS_PROXIED)) {
        mutex_lock(&_client_mutex);
        _proxy[0] = '\0';
        mutex_unlock(&_client_mutex);
    }
}

int gcoap_dns_server_proxy_set(const char *proxy)
{
    int res;

    if (!IS_USED(MODULE_GCOAP_DNS_PROXIED)) {
        return -ENOTSUP;
    }
    res = strlen(proxy);

    if ((unsigned)(res + 1) >= CONFIG_GCOAP_DNS_SERVER_URI_LEN) {
        return -EINVAL;
    }
    mutex_lock(&_client_mutex);
    strcpy(_proxy, proxy);
    mutex_unlock(&_client_mutex);
    return res;
}

ssize_t gcoap_dns_server_proxy_get(char *proxy, size_t proxy_len)
{
    ssize_t res = 0;
    mutex_lock(&_client_mutex);
    if (_dns_server_uri_isset()) {
        res = strscpy(proxy, _proxy, proxy_len);
        if (res == -E2BIG) {
            res = -ENOBUFS;
        }
    }
    mutex_unlock(&_client_mutex);
    return res;
}

static inline bool _dns_server_uri_isset(void)
{
    return _uri[0] != '\0';
}

#if IS_USED(MODULE_GCOAP_DTLS)
static void _remove_cred(sock_dtls_t *sock, _cred_t *cred)
{
    sock_dtls_remove_credential(sock, cred->tag);
    credman_delete(cred->tag, cred->type);
    cred->type = CREDMAN_TYPE_EMPTY;
    cred->tag = CREDMAN_TAG_EMPTY;
}
#endif

static inline bool _is_proxied(void)
{
    return IS_USED(MODULE_GCOAP_DNS_PROXIED) && _proxy[0] != '\0';
}

static int _add_init_block2_opt(coap_pkt_t *pdu)
{
    if (CONFIG_GCOAP_DNS_PDU_BUF_SIZE < CONFIG_DNS_MSG_LEN) {
        /* If our largest DNS message fits in the DNS PDU BUF, there is no point in sending Block2
         * in the initial message---a huge response might still overwhelm our PDU buffer, but if
         * that happens we could not have processed it as a DNS message if it came in in fragments
         * either. */
        coap_block1_t block;

        coap_block_object_init(&block, 0, CONFIG_GCOAP_DNS_BLOCK_SIZE, 0);
        return coap_opt_add_block2_control(pdu, &block);
    }
    return 0;
}

static int _add_proxy_uri_opt(coap_pkt_t *pdu, const char *proxy_uri)
{
    if (_is_proxied()) {
        return coap_opt_add_proxy_uri(pdu, proxy_uri);
    }
    return 0;
}

static int _add_req_tag_opt(coap_pkt_t *pdu, _req_ctx_t *context)
{
#if IS_USED(MODULE_GCOAP_DTLS)
    if (CONFIG_GCOAP_DNS_PDU_BUF_SIZE < CONFIG_DNS_MSG_LEN) {
        return coap_opt_add_opaque(pdu, 292, (uint8_t *)&context->req_tag,
                                   sizeof(context->req_tag));
    }
#else
    (void)pdu;
    (void)context;
#endif
    return 0;
}

static int _add_remaining_options(coap_pkt_t *pdu, const char *proxy_uri, _req_ctx_t *context)
{
    if (_add_proxy_uri_opt(pdu, proxy_uri) < 0) {
        DEBUG("gcoap_dns: unable to add Proxy-URI option to request\n");
        return -ENOBUFS;
    }
    /* add request tag to distinguish multiple blockwise requests on-the-air */
    if (_add_req_tag_opt(pdu, context) < 0) {
        DEBUG("gcoap_dns: unable to add Request-Tag option to request");
        return -ENOBUFS;
    }
    return 0;
}

static size_t _dns_msg_compose(void *dns_buf, const char *domain_name,
                               int family)
{
    return dns_msg_compose_query(dns_buf, domain_name, 0, family);
}

static int _set_remote(const uri_parser_result_t *uri_comp,
                       sock_udp_ep_t *remote) {
    assert(uri_comp->host != NULL);
    memset(remote, 0, sizeof(*remote));

    if (uri_comp->ipv6addr != NULL) {
        if (ipv6_addr_from_buf((ipv6_addr_t *)remote->addr.ipv6,
                               uri_comp->ipv6addr,
                               uri_comp->ipv6addr_len) == NULL) {
            DEBUG("gcoap_dns: unable to parse IPv6 address %*.s\n",
                  uri_comp->ipv6addr_len, uri_comp->ipv6addr);
            return -EINVAL;
        }
        remote->family = AF_INET6;
        if (uri_comp->zoneid != NULL) {
            netif_t *netif = netif_get_by_name_buffer(uri_comp->zoneid,
                                                      uri_comp->zoneid_len);
            if (netif == NULL) {
                DEBUG("gcoap_dns: unable to interpret zoneid %*.s\n",
                      uri_comp->zoneid_len, uri_comp->zoneid);
                return -ENOENT;
            }
            remote->netif = netif_get_id(netif);
        }
        else {
            remote->netif = SOCK_ADDR_ANY_NETIF;
        }
    }
    else if (ipv4_addr_from_buf((ipv4_addr_t *)remote->addr.ipv4,
                                uri_comp->host, uri_comp->host_len) != NULL) {
        remote->family = AF_INET;
    }
    else if (IS_USED(MODULE_SOCK_DNS)) {
        char hostname[uri_comp->host_len + 1];

        strncpy(hostname, uri_comp->host, uri_comp->host_len);
        hostname[uri_comp->host_len] = '\0';
        int res = sock_dns_query(hostname, remote->addr.ipv6, AF_UNSPEC);
        if (res < 0) {
            DEBUG("gcoap_dns: unable to resolve hostname %s for remote\n",
                  hostname);
            return -EHOSTUNREACH;
        }
        if (res == sizeof(remote->addr.ipv4)) {
            remote->family = AF_INET;
        }
        else if (res == sizeof(remote->addr.ipv6)) {
            remote->family = AF_INET6;
        }
        else {
            DEBUG("gcoap_dns: Unable to determine address family of hostname %s.\n",
                  hostname);
            return -EHOSTUNREACH;
        }
    }
    else {
        DEBUG("gcoap_dns: unable to resolve hostname in URI %s for remote\n",
              _uri);
        return -EHOSTUNREACH;
    }
    if (uri_comp->port == 0) {
        if (strncmp(_uri, "coap:", sizeof("coap:") - 1) == 0) {
            remote->port = CONFIG_GCOAP_PORT;
        }
        else {
            remote->port = CONFIG_GCOAPS_PORT;
        }
    }
    else {
        remote->port = uri_comp->port;
    }
    return 0;
}

static int _gen_uri(uri_parser_result_t *uri_comp)
{
    const char *uri = (_is_proxied()) ? _proxy : _uri;
    int res = uri_parser_process_string(uri_comp, uri);

    if (res < 0) {
        return -EINVAL;
    }
    return strlen(uri);
}

static ssize_t _req_init(coap_pkt_t *pdu, uri_parser_result_t *uri_comp, bool con)
{
    gcoap_req_init_path_buffer(pdu, pdu->payload, pdu->payload_len, COAP_METHOD_FETCH,
                               uri_comp->path, uri_comp->path_len);
    if (con) {
        coap_hdr_set_type(pdu->hdr, COAP_TYPE_CON);
    }

    if (coap_opt_add_format(pdu, COAP_FORMAT_DNS_MESSAGE) < 0) {
        DEBUG("gcoap_dns: unable to add Content-Format option to request\n");
    }
    if (coap_opt_add_accept(pdu, COAP_FORMAT_DNS_MESSAGE) < 0) {
        DEBUG("gcoap_dns: unable to add Accept option to request\n");
    }
    if (IS_USED(MODULE_GCOAP_DTLS) &&
        (uri_comp->scheme_len == (sizeof("coaps") - 1)) &&
        (strncmp(uri_comp->scheme, "coaps", uri_comp->scheme_len) == 0)) {
        return GCOAP_SOCKET_TYPE_DTLS;
    }
    return GCOAP_SOCKET_TYPE_UDP;
}

static int _do_block(coap_pkt_t *pdu, const sock_udp_ep_t *remote,
                                _req_ctx_t *context)
{
    gcoap_socket_type_t tl_type;
    ssize_t len;
    bool more;
    coap_block_slicer_t slicer;

    coap_block_slicer_init(&slicer, context->cur_blk_num++,
                           CONFIG_GCOAP_DNS_BLOCK_SIZE);
    tl_type = _req_init(pdu, &_uri_comp, true);
    if (tl_type == GCOAP_SOCKET_TYPE_UNDEF) {
        return -EINVAL;
    }
    more = (CONFIG_GCOAP_DNS_PDU_BUF_SIZE >= CONFIG_DNS_MSG_LEN) ||
           (slicer.end < context->dns_buf_len);
    if (!more && (_add_init_block2_opt(pdu) < 0)) {
        DEBUG("gcoap_dns: unable to add Block2 option to request\n");
        return -ENOBUFS;
    }
    if (coap_opt_add_block1(pdu, &slicer, context->dns_buf_len > CONFIG_GCOAP_DNS_BLOCK_SIZE) < 0) {
        DEBUG("gcoap_dns: unable to add Block1 option to request\n");
        return -ENOBUFS;
    }
    if (_add_remaining_options(pdu, _uri, context) < 0) {
        return -ENOBUFS;
    }
    len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    len += coap_blockwise_put_bytes(&slicer, pdu->payload,
                                    context->dns_buf,
                                    context->dns_buf_len);

    coap_block1_finish(&slicer);

    if ((len = _send(pdu->hdr, len, remote, slicer.start == 0, context, tl_type)) <= 0) {
        DEBUG("gcoap_dns: msg send failed: %" PRIdSIZE "\n", len);
        return len;
    }
    return len;
}

static ssize_t _req(_req_ctx_t *context)
{
    coap_pkt_t *pdu = context->pkt;
    ssize_t len;

    if ((len = _gen_uri(&_uri_comp)) < 0) {
        DEBUG("gcoap_dns: unable to parse URI\n");
        return len;
    }
    if ((len = _set_remote(&_uri_comp, &_remote)) < 0) {
        return len;
    }
    if (context->dns_buf_len > CONFIG_GCOAP_DNS_BLOCK_SIZE) {
        context->cur_blk_num = 0U;
        return _do_block(pdu, &_remote, context);
    }
    else {
        gcoap_socket_type_t tl_type = _req_init(pdu, &_uri_comp, true);
        if (tl_type == GCOAP_SOCKET_TYPE_UNDEF) {
            return -EINVAL;
        }
        if (_add_init_block2_opt(pdu) < 0) {
            DEBUG("gcoap_dns: unable to add Block2 option to request\n");
            return -ENOBUFS;
        }
        if (_add_remaining_options(pdu, _uri, context) < 0) {
            DEBUG("gcoap_dns: unable to add Proxy-URI option to request\n");
            return -ENOBUFS;
        }
        len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);
        memcpy(pdu->payload, context->dns_buf, context->dns_buf_len);
        return _send(pdu->hdr, len + context->dns_buf_len, &_remote, true, context, tl_type);
    }
}

static int _dns_query(const char *domain_name, _req_ctx_t *req_ctx)
{
    int res;

    assert(domain_name != NULL);
    if (!_dns_server_uri_isset()) {
        DEBUG("gcoap_dns: no URI template provided\n");
        return -ECONNREFUSED;
    }
    req_ctx->dns_buf_len = _dns_msg_compose(
        req_ctx->dns_buf,
        domain_name,
        req_ctx->family
    );
#if IS_USED(MODULE_DNS_CACHE)
    req_ctx->domain_name = domain_name;
#endif
#if IS_USED(MODULE_GCOAP_DTLS)
    req_ctx->req_tag = _req_tag++;
#endif
    res = _req(req_ctx);
    if (res <= 0) {
        DEBUG("gcoap_dns: unable to send request\n");
        if (res == 0) {
            res = -ENOTRECOVERABLE;
        }
    }
    return res;
}

static const char *_domain_name_from_ctx(_req_ctx_t *context)
{
#if IS_USED(MODULE_DNS_CACHE)
    return context->domain_name;
#else
    (void)context;
    return NULL;
#endif
}

static void _resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                          const sock_udp_ep_t *remote)
{
    coap_block1_t block;
    _req_ctx_t *context = memo->context;
    void *data;
    size_t data_len;
    int family = context->family;

    if (memo->state == GCOAP_MEMO_TIMEOUT) {
        DEBUG("gcoap_dns: CoAP request timed out\n");
        context->res = -ETIMEDOUT;
        goto unlock;
    }
    else if (memo->state != GCOAP_MEMO_RESP) {
        DEBUG("gcoap_dns: error in response\n");
        context->res = -EBADMSG;
        goto unlock;
    }

    if ((remote->port != _remote.port) ||
        (remote->family != _remote.family) ||
        (!ipv6_addr_is_unspecified((ipv6_addr_t *)_remote.addr.ipv6) &&
         (memcmp(remote->addr.ipv6, _remote.addr.ipv6,
                 sizeof(_remote.addr.ipv6)) != 0))) {
        DEBUG("gcoap_dns: unexpected remote for reply\n");
        context->res = -EDESTADDRREQ;
        goto unlock;
    }
    if (coap_get_code_class(pdu) != COAP_CLASS_SUCCESS) {
        DEBUG("gcoap_dns: unsuccessful response: %1u.%02u\n",
              coap_get_code_class(pdu), coap_get_code_detail(pdu));
        context->res = -EBADMSG;
        goto unlock;
    }
    if (coap_get_code_raw(pdu) == COAP_CODE_CONTINUE) {
        int res = _do_block(pdu, remote, context);

        if (res < 0) {
            context->res = res;
            goto unlock;
        }
        return;
    }
    if (coap_get_block2(pdu, &block)) {
        uint8_t *dns_buf = context->dns_buf;

        if ((block.offset + pdu->payload_len) > CONFIG_DNS_MSG_LEN) {
            DEBUG("gcoap_dns: No buffer space for block-wise transfer "
                  "(%" PRIuSIZE " + %u) > %u\n", block.offset,
                  pdu->payload_len, CONFIG_DNS_MSG_LEN);
            context->res = -ENOBUFS;
            goto unlock;
        }
        memcpy(&dns_buf[block.offset], pdu->payload, pdu->payload_len);
        if (block.blknum == 0) {
            context->dns_buf_len = pdu->payload_len;
            if (block.more && strlen(_uri) == 0) {
                DEBUG("gcoap_dns: Cannot complete block-wise\n");
                context->res = -EINVAL;
                goto unlock;
            }
        }
        else {
            context->dns_buf_len += pdu->payload_len;
        }
        if (block.more) {
            gcoap_socket_type_t tl_type;
            unsigned msg_type = coap_get_type(pdu);
            int len;

            pdu->payload = (uint8_t *)pdu->hdr;
            pdu->payload_len = CONFIG_GCOAP_DNS_PDU_BUF_SIZE;
            tl_type = _req_init(pdu, &_uri_comp, msg_type == COAP_TYPE_ACK);
            block.blknum++;
            if (coap_opt_add_block2_control(pdu, &block) < 0) {
                DEBUG("gcoap_dns: unable to add Block2 option to request\n");
                context->res = -ENOBUFS;
                goto unlock;
            }
            if (_add_remaining_options(pdu, _uri, context) < 0) {
                context->res = -ENOBUFS;
                goto unlock;
            }
            len = coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
            if ((len = _send((uint8_t *)pdu->hdr, len, remote, false, context, tl_type)) <= 0) {
                DEBUG("gcoap_dns: Unable to request next block: %d\n", len);
                context->res = len;
                goto unlock;
            }
            return;
        }
        data = context->dns_buf;
        data_len = context->dns_buf_len;
    }
    else {
        data = pdu->payload;
        data_len = pdu->payload_len;
    }
    switch (coap_get_content_type(pdu)) {
        case COAP_FORMAT_DNS_MESSAGE:
        case COAP_FORMAT_NONE: {
            uint32_t ttl = 0;

            context->res = dns_msg_parse_reply(data, data_len, family,
                                               context->addr_out, &ttl);
            if (IS_USED(MODULE_DNS_CACHE) && (context->res > 0)) {
                uint32_t max_age;

                if (coap_opt_get_uint(pdu, COAP_OPT_MAX_AGE, &max_age) < 0) {
                    max_age = 60;
                }
                ttl += max_age;
                dns_cache_add(_domain_name_from_ctx(context), context->addr_out, context->res, ttl);
            }
            else if (ENABLE_DEBUG && (context->res < 0)) {
                DEBUG("gcoap_dns: Unable to parse DNS reply: %d\n",
                      context->res);
            }
            break;
        }
        default:
            context->res = -ENOMSG;
            break;
    }
unlock:
    mutex_unlock(&context->resp_wait);
}

static ssize_t _send(const void *buf, size_t len, const sock_udp_ep_t *remote,
                     bool lock_resp_wait, _req_ctx_t *context, gcoap_socket_type_t tl_type)
{
    if (lock_resp_wait) {
        mutex_lock(&context->resp_wait);
    }
    return gcoap_req_send(buf, len, remote, NULL, _resp_handler, context, tl_type);
}
/** @} */
