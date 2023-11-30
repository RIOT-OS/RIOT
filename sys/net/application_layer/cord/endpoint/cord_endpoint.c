/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_cord_endpoint
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory endpoint implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>

#include "mutex.h"
#include "assert.h"

#include "fmt.h"
#include "luid.h"
#include "clif.h"
#include "net/gcoap.h"
#include "net/ipv6/addr.h"
#include "net/cord/endpoint.h"
#include "net/cord/config.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#ifdef CONFIG_CORD_ENDPOINT
#define cord_endpoint_name    CONFIG_CORD_ENDPOINT
#else
#define PREFIX_LEN      (sizeof(CORD_ENDPOINT_PREFIX))        /* contains \0 char */
#define BUFSIZE         (PREFIX_LEN + CORD_ENDPOINT_SUFFIX_LEN)
static char cord_endpoint_name[BUFSIZE] = { 0 };
#endif

#ifndef MODULE_CORD_ENDPOINT
static const char wkrd[] = "/.well-known/rd";
#endif

int _pkt_add_qstring(coap_pkt_t *pkt)
{
    /* extend the url with some query string options */
    int res = coap_opt_add_uri_query(pkt, "ep", cord_endpoint_name);
    if (res < 0) {
        return res;
    }

    /* [optional] set the lifetime parameter */
#if CONFIG_CORD_LT
    char lt[11];
    lt[fmt_u32_dec(lt, CONFIG_CORD_LT)] = '\0';
    res = coap_opt_add_uri_query(pkt, "lt", lt);
    if (res < 0) {
        return res;
    }
#endif

    /* [optional] set the domain parameter */
#ifdef CORD_D
    res = coap_opt_add_uri_query(pkt, "d", CORD_D);
    if (res < 0) {
        return res;
    }
#endif

#ifdef CONFIG_CORD_EXTRAARGS
    static const char *extra[] = { CONFIG_CORD_EXTRAARGS };
    for (unsigned i = 0; i < ARRAY_SIZE(extra); ++i) {
        res = coap_opt_add_uri_query(pkt, extra[i], NULL);
        if (res < 0) {
            return res;
        }
    }
#endif

    return 0;
}

void _init_epname(void)
{
#ifndef CONFIG_CORD_ENDPOINT
    if (cord_endpoint_name[0] == '\0') {
        uint8_t luid[CORD_ENDPOINT_SUFFIX_LEN / 2];

        if (PREFIX_LEN > 1) {
            memcpy(cord_endpoint_name, CORD_ENDPOINT_PREFIX, (PREFIX_LEN - 1));
        }

        luid_get(luid, sizeof(luid));
        fmt_bytes_hex(&cord_endpoint_name[PREFIX_LEN - 1], luid, sizeof(luid));
        cord_endpoint_name[BUFSIZE - 1] = '\0';
    }
#endif
}

static void _post_state(cord_endpoint_t *cord, cord_state_t state)
{
    cord->state = state;
    /* Notify subscribers of state change */
    event_source_trigger(&cord->ev_source);
    /* submit event */
    event_post(cord->queue, &cord->state_update);
}

static void _post_timeout(cord_endpoint_t *cord, cord_state_t state, uint32_t timeout)
{
    cord->state = state;
    /* submit event */
    event_timeout_set(&cord->retry_timer, timeout);
    /* Notify subscribers of state change */
    event_source_trigger(&cord->ev_source);
}

static void _post_retry(cord_endpoint_t *cord, cord_state_t state)
{
    _post_timeout(cord, state, CONFIG_CORD_FAIL_RETRY_SEC);
}

static void _post_refresh(cord_endpoint_t *cord, cord_state_t state)
{
    _post_timeout(cord, state, CONFIG_CORD_LT);
}

static int _set_rd_settings(cord_endpoint_t *cord, const sock_udp_ep_t *remote, const char *regif)
{
    memcpy(&cord->rd_remote, remote, sizeof(cord->rd_remote));
#ifdef MODULE_CORD_ENDPOINT
    if (regif) {
        if (strlen(regif) < sizeof(cord->rd_regif)) {
            strcpy(cord->rd_regif, regif);
        }
        else {
            return CORD_ENDPOINT_OVERFLOW;
        }
    }
#else
    (void)regif;
#endif
    return CORD_ENDPOINT_OK;
}

static void _clear_regif(cord_endpoint_t *cord)
{
    (void)cord;
#ifdef MODULE_CORD_ENDPOINT
    memset(cord->rd_regif, 0, CORD_URI_MAX);
#endif
}

#ifdef MODULE_CORD_ENDPOINT
static void _clear_rd_location(cord_endpoint_t *cord)
{
    memset(cord->rd_loc, 0, CORD_URI_MAX);
}

static void _on_delete(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                       const sock_udp_ep_t *remote)
{
    cord_endpoint_t *cord = memo->context;
    unsigned req_state = memo->state;
    (void)remote;
    (void)pdu;

    if (req_state == GCOAP_MEMO_RESP) {
        /* Either the registration was successfully deleted, or there was no
         * registration in the first place */
        _clear_rd_location(cord);
        _post_state(cord, CORD_STATE_IDLE);
        return;
    }
    /* Retry if the server is temporarily unavailable */
    _post_retry(cord, CORD_STATE_DELETE);
}

static void _on_update(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                       const sock_udp_ep_t *remote)
{
    cord_endpoint_t *cord = memo->context;
    unsigned req_state = memo->state;
    (void)remote;

    if (req_state == GCOAP_MEMO_RESP) {
        if (pdu->hdr->code == COAP_CODE_CHANGED) {
            _post_refresh(cord, CORD_STATE_REFRESH);
            return;
        }
        else if (coap_get_code_class(pdu) == COAP_CLASS_CLIENT_FAILURE) {
            _clear_rd_location(cord);
            /* reset to registration */
            _post_state(cord, CORD_STATE_REGISTRY);
            return;
        }
    }
    _post_retry(cord, CORD_STATE_REFRESH);
}

static int _send_update_request_with_code(cord_endpoint_t *cord, unsigned code, gcoap_resp_handler_t handle)
{
    coap_pkt_t pkt;

    if (cord->rd_loc[0] == 0) {
        return CORD_ENDPOINT_NO_RD;
    }

    /* build CoAP request packet */
    int res = gcoap_req_init(&pkt, cord->buf, CONFIG_CORD_ENDPOINT_BUFSIZE, code, cord->rd_loc);
    if (res < 0) {
        return CORD_ENDPOINT_ERR;
    }
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);
    ssize_t pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);

    /* send request */
    ssize_t send_len = gcoap_req_send(cord->buf, pkt_len, &cord->rd_remote, handle, cord);
    if (send_len <= 0) {
        return CORD_ENDPOINT_ERR;
    }
    return 0;
}

static void _on_discover(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                         const sock_udp_ep_t *remote)
{
    cord_endpoint_t *cord = memo->context;
    (void)remote;

    if (memo->state == GCOAP_MEMO_RESP) {
        unsigned ct = coap_get_content_type(pdu);
        if (ct != COAP_FORMAT_LINK) {
            goto end;
        }
        if (pdu->payload_len == 0) {
            goto end;
        }

        clif_t link;
        ssize_t res = clif_decode_link(&link, NULL, 0, (char*)pdu->payload, pdu->payload_len);
        if (res > 0) {
            /* TODO: verify size of interface resource identifier */
            if (link.target_len >= sizeof(cord->rd_regif)) {
                goto end;
            }
            memcpy(cord->rd_regif, link.target, link.target_len);
            cord->rd_regif[link.target_len] = '\0';

            _post_state(cord, CORD_STATE_REGISTRY);
            return;
        }
    }
end:
    /* retry later */
    _post_retry(cord, CORD_STATE_DISCOVERY);
    return;
}

static int _start_discovery(cord_endpoint_t *cord)
{
    coap_pkt_t pkt;
    /* do URI discovery for the registration interface */
    int res = gcoap_req_init(&pkt, cord->buf, CONFIG_CORD_ENDPOINT_BUFSIZE, COAP_METHOD_GET,
                             "/.well-known/core");
    if (res < 0) {
        return CORD_ENDPOINT_ERR;
    }
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);
    coap_opt_add_uri_query(&pkt, "rt", "core.rd");
    size_t pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    res = gcoap_req_send(cord->buf, pkt_len, &cord->rd_remote, _on_discover, cord);
    if (res <= 0) {
        _post_retry(cord, CORD_STATE_DISCOVERY);
        return CORD_ENDPOINT_ERR;
    }
    return 0;
}
#endif

static void _on_register(const gcoap_request_memo_t *memo, coap_pkt_t* pdu,
                         const sock_udp_ep_t *remote)
{
    cord_endpoint_t *cord = memo->context;

    if (memo->state == GCOAP_MEMO_RESP) {
        /* use the remote used to respond with as new sock remote */
        memcpy(&cord->rd_remote, remote, sizeof(cord->rd_remote));
        if (pdu->hdr->code == COAP_CODE_CREATED) {
#ifdef MODULE_CORD_ENDPOINT
            /* read the location header and save the RD details on success */
            if (coap_get_location_path(pdu, (uint8_t *)cord->rd_loc,
                                       sizeof(cord->rd_loc)) > 0) {
                _post_refresh(cord, CORD_STATE_REFRESH);
                return;
            }
            /* if copy fails cascade through to retrying later again */
#else
            _post_refresh(cord, CORD_STATE_REGISTRY);
#endif
        }
        else if (coap_get_code_class(pdu) == COAP_CLASS_CLIENT_FAILURE) {
            /* Retry discovery later */
            if (!IS_ACTIVE(MODULE_CORD_ENDPOINT)) {
                _clear_regif(cord);
                _post_retry(cord, CORD_STATE_DISCOVERY);
            }
            else {
                _post_retry(cord, CORD_STATE_REGISTRY);
            }
            return;
        }
    }
    /* Retry registration after some time if other failure */
    _post_retry(cord, CORD_STATE_REGISTRY);
}

static int  _start_registration(cord_endpoint_t *cord)
{
    int res;
    ssize_t pkt_len;
    int retval;
    coap_pkt_t pkt;

    /* build and send CoAP POST request to the RD's registration interface */
    res = gcoap_req_init(&pkt, cord->buf, CONFIG_CORD_ENDPOINT_BUFSIZE, COAP_METHOD_POST,
#ifdef MODULE_CORD_ENDPOINT
            cord->rd_regif);
#else
            wkrd);
#endif
    if (res < 0) {
        retval = CORD_ENDPOINT_ERR;
        goto end;
    }
    /* set hdr type and content format and write query string */
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);

    if (IS_ACTIVE(MODULE_CORD_ENDPOINT)) {
        coap_opt_add_format(&pkt, COAP_FORMAT_LINK);
    }

    res = _pkt_add_qstring(&pkt);
    if (res < 0) {
        retval = CORD_ENDPOINT_ERR;
        goto end;
    }

    if (IS_ACTIVE(MODULE_CORD_ENDPOINT)) {
        pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);

        /* add the resource description as payload */
        res = gcoap_get_resource_list(pkt.payload, pkt.payload_len,
                                      COAP_FORMAT_LINK, GCOAP_SOCKET_TYPE_UNDEF);
        if (res < 0) {
            retval = CORD_ENDPOINT_ERR;
            goto end;
        }
        pkt_len += res;
    }
    else {
        pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    }

    /* send out the request */
    res = gcoap_req_send(cord->buf, pkt_len, &cord->rd_remote, _on_register, cord);
    if (res > 0) {
        /* wait for gcoap result */
        return 0;
    }
    retval = CORD_ENDPOINT_ERR;
end:
    /* retry registration later on */
    _post_retry(cord, CORD_STATE_REGISTRY);
    return retval;
}

int cord_endpoint_register(cord_endpoint_t *cord, const sock_udp_ep_t *remote, const char *regif)
{
    assert(remote);
    int res = CORD_ENDPOINT_OK;
    mutex_lock(&cord->lock);
    cord_state_t state = cord->state;
    if (state == CORD_STATE_IDLE) {
        /* start registration or discovery at remote */
        res = _set_rd_settings(cord, remote, regif);
        if (res == CORD_ENDPOINT_OK) {
            if (regif) {
                _post_state(cord, CORD_STATE_REGISTRY);
            }
            else {
                _post_state(cord, CORD_STATE_DISCOVERY);
            }
        }
    }
    else {
        res = CORD_ENDPOINT_BUSY;
    }
    mutex_unlock(&cord->lock);
    return res;
}

int cord_endpoint_update(cord_endpoint_t *cord)
{
    int res = CORD_ENDPOINT_BUSY;
    mutex_lock(&cord->lock);
    if (cord->state == CORD_STATE_REFRESH) {
        event_timeout_clear(&cord->retry_timer);
        _post_state(cord, CORD_STATE_REFRESH);
        res = CORD_ENDPOINT_OK;
    }
    mutex_unlock(&cord->lock);
    return res;
}

void cord_endpoint_remove(cord_endpoint_t *cord)
{
    mutex_lock(&cord->lock);
    event_timeout_clear(&cord->retry_timer);
    if (IS_ACTIVE(MODULE_CORD_ENDPOINT)) {
        _post_state(cord, CORD_STATE_DELETE);
    }
    else {
        _post_state(cord, CORD_STATE_IDLE);
    }
    mutex_unlock(&cord->lock);
}

static void _on_state_update(event_t *event)
{
    cord_endpoint_t *cord = container_of(event, cord_endpoint_t, state_update);
    mutex_lock(&cord->lock);
    switch (cord->state) {
        case CORD_STATE_IDLE:
            /* Do nothing */
            break;
#ifdef MODULE_CORD_ENDPOINT
        case CORD_STATE_DISCOVERY:
            _start_discovery(cord);
            break;
#endif
        case CORD_STATE_REGISTRY:
            _start_registration(cord);
            break;
#ifdef MODULE_CORD_ENDPOINT
        case CORD_STATE_REFRESH:
            /* Send refresh */
            _send_update_request_with_code(cord, COAP_METHOD_POST, _on_update);
            break;
        case CORD_STATE_DELETE:
            _send_update_request_with_code(cord, COAP_METHOD_DELETE, _on_delete);
            break;
#endif
        default:
            DEBUG("[CORD] State: %d\n", cord->state);
            assert(false);
    }
    mutex_unlock(&cord->lock);
}

int cord_endpoint_init(cord_endpoint_t *cord, event_queue_t *queue, const sock_udp_ep_t *remote, const char *regif)
{
    _init_epname();

    memset(cord, 0, sizeof(cord_endpoint_t));
    cord->queue = queue;
    cord->state_update.handler = _on_state_update;
    event_timeout_ztimer_init(&cord->retry_timer, ZTIMER_SEC, queue, &cord->state_update);

    int res = CORD_ENDPOINT_OK;

    if (!remote) {
        _post_state(cord, CORD_STATE_IDLE);
        return res;
    }

    res = _set_rd_settings(cord, remote, regif);
    if (IS_ACTIVE(MODULE_CORD_ENDPOINT)) {
        if (remote && regif) {
            _post_state(cord, CORD_STATE_REGISTRY);
        } else {
            _post_state(cord, CORD_STATE_DISCOVERY);
        }
    }
    else {
        _post_state(cord, CORD_STATE_REGISTRY);
    }
    return res;
}

ssize_t cord_endpoint_get_name(char *buf, size_t buf_len)
{
    size_t ep_len = strlen(cord_endpoint_name) + 1;
    if (ep_len > buf_len) {
        return CORD_ENDPOINT_OVERFLOW;
    }
    memcpy(buf, cord_endpoint_name, ep_len);
    return ep_len;
}

static ssize_t _copy_cord_str(cord_endpoint_t *cord, const char *src, char *dest, size_t buf_len)
{
    mutex_lock(&cord->lock);
    size_t src_len = strlen(src) + 1;
    if (src_len > buf_len) {
        return CORD_ENDPOINT_OVERFLOW;
    }
    memcpy(dest, src, src_len);
    mutex_unlock(&cord->lock);
    return src_len;
}

ssize_t cord_endpoint_get_location(cord_endpoint_t *cord, char *buf, size_t buf_len)
{
    return _copy_cord_str(cord,
#ifdef MODULE_CORD_ENDPOINT
            cord->rd_loc,
#else
            wkrd,
#endif
            buf, buf_len);
}

ssize_t cord_endpoint_get_interface(cord_endpoint_t *cord, char *buf, size_t buf_len)
{
    return _copy_cord_str(cord,
#ifdef MODULE_CORD_ENDPOINT
            cord->rd_regif,
#else
            wkrd,
#endif
            buf, buf_len);
}

cord_state_t cord_endpoint_get_state(cord_endpoint_t *cord)
{
    mutex_lock(&cord->lock);
    cord_state_t state = cord->state;
    mutex_unlock(&cord->lock);
    return state;
}

void cord_endpoint_dump_status(const cord_endpoint_t *cord)
{
    puts("CoAP RD connection status:");

#ifdef MODULE_CORD_ENDPOINT
    if (cord->rd_loc[0] == 0) {
        puts("  --- not registered with any RD ---");
    }
    else {
#endif
        /* get address string */
        char addr[IPV6_ADDR_MAX_STR_LEN];
        ipv6_addr_to_str(addr, (ipv6_addr_t *)&cord->rd_remote.addr, sizeof(addr));

        printf("RD address: coap://[%s]:%"PRIu16"\n", addr, cord->rd_remote.port);
        printf("   ep name: %s\n", cord_endpoint_name);
        printf("  lifetime: %is\n", (int)CONFIG_CORD_LT);
#ifdef MODULE_CORD_ENDPOINT
        printf("    reg if: %s\n", cord->rd_regif);
        printf("  location: %s\n", cord->rd_loc);
    }
#endif
}
