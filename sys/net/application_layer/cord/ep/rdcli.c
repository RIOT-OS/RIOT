/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_rdcli
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory client implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "fmt.h"
#include "mutex.h"
#include "thread_flags.h"

#include "net/gcoap.h"
#include "net/ipv6/addr.h"
#include "net/rdcli.h"
#include "net/cord/common.h"
#include "net/rdcli_config.h"

#ifdef MODULE_RDCLI_STANDALONE
#include "net/rdcli_standalone.h"
#endif

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define FLAG_SUCCESS        (0x0001)
#define FLAG_TIMEOUT        (0x0002)
#define FLAG_ERR            (0x0004)
#define FLAG_OVERFLOW       (0x0008)
#define FLAG_MASK           (0x000f)

#define BUFSIZE             (512U)

static char *_regif_buf;
static size_t _regif_buf_len;

static char _rd_loc[NANOCOAP_URI_MAX];
static char _rd_regif[NANOCOAP_URI_MAX];
static sock_udp_ep_t _rd_remote;

static mutex_t _mutex = MUTEX_INIT;
static volatile thread_t *_waiter;

static uint8_t buf[BUFSIZE];

static void _lock(void)
{
    mutex_lock(&_mutex);
    _waiter = sched_active_thread;
}

static int _sync(void)
{
    thread_flags_t flags = thread_flags_wait_any(FLAG_MASK);

    if (flags & FLAG_ERR) {
        return RDCLI_ERR;
    }
    else if (flags & FLAG_TIMEOUT) {
        return RDCLI_TIMEOUT;
    }
    else if (flags & FLAG_OVERFLOW) {
        return RDCLI_OVERFLOW;
    }
    else {
        return RDCLI_OK;
    }
}

static void _on_register(unsigned req_state, coap_pkt_t* pdu,
                        sock_udp_ep_t *remote)
{
    thread_flags_t flag = FLAG_ERR;

    if ((req_state == GCOAP_MEMO_RESP) &&
        (pdu->hdr->code == COAP_CODE_CREATED)) {
        /* read the location header and save the RD details on success */
        if (coap_get_location_path(pdu, (uint8_t *)_rd_loc,
                                   sizeof(_rd_loc)) > 0) {
            memcpy(&_rd_remote, remote, sizeof(_rd_remote));
            flag = FLAG_SUCCESS;
        }
        else {
            /* reset RD entry */
            flag = FLAG_OVERFLOW;
        }
    }
    else if (req_state == GCOAP_MEMO_TIMEOUT) {
        flag = FLAG_TIMEOUT;
    }

    thread_flags_set((thread_t *)_waiter, flag);
}

static void _on_update_remove(unsigned req_state, coap_pkt_t *pdu, uint8_t code)
{
    thread_flags_t flag = FLAG_ERR;

    if ((req_state == GCOAP_MEMO_RESP) && (pdu->hdr->code == code)) {
        flag = FLAG_SUCCESS;
    }
    else if (req_state == GCOAP_MEMO_TIMEOUT) {
        flag = FLAG_TIMEOUT;
    }

    thread_flags_set((thread_t *)_waiter, flag);
}

static void _on_update(unsigned req_state, coap_pkt_t *pdu, sock_udp_ep_t *remote)
{
    (void)remote;
    _on_update_remove(req_state, pdu, COAP_CODE_CHANGED);
}

static void _on_remove(unsigned req_state, coap_pkt_t *pdu, sock_udp_ep_t *remote)
{
    (void)remote;
    _on_update_remove(req_state, pdu, COAP_CODE_DELETED);
}

static int _update_remove(unsigned code, gcoap_resp_handler_t handle)
{
    coap_pkt_t pkt;

    if (_rd_loc[0] == 0) {
        return RDCLI_NORD;
    }

    /* build CoAP request packet */
    int res = gcoap_req_init(&pkt, buf, sizeof(buf), code, _rd_loc);
    if (res < 0) {
        return RDCLI_ERR;
    }
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);
    ssize_t pkt_len = gcoap_finish(&pkt, 0, COAP_FORMAT_NONE);

    /* send request */
    gcoap_req_send2(buf, pkt_len, &_rd_remote, handle);

    /* synchronize response */
    return _sync();
}

static void _on_discover(unsigned req_state, coap_pkt_t *pdu,
                         sock_udp_ep_t *remote)
{
    thread_flags_t flag = RDCLI_NORD;
    (void)remote;

    if (req_state == GCOAP_MEMO_RESP) {
        unsigned ct = coap_get_content_type(pdu);
        if (ct != COAP_FORMAT_LINK) {
            goto end;
        }
        if (pdu->payload_len == 0) {
            goto end;
        }
        /* do simplified parsing of registration interface location */
        char *start = (char *)pdu->payload;
        char *end;
        char *limit = (char *)(pdu->payload + pdu->payload_len);
        while ((*start != '<') && (start < limit)) {
            start++;
        }
        if (*start != '<') {
            goto end;
        }
        end = ++start;
        while ((*end != '>') && (end < limit)) {
            end++;
        }
        if (*end != '>') {
            goto end;
        }
        /* TODO: verify size of interface resource identifier */
        size_t uri_len = (size_t)(end - start);
        if (uri_len >= _regif_buf_len) {
            goto end;
        }
        memcpy(_regif_buf, start, uri_len);
        memset((_regif_buf + uri_len), 0, (_regif_buf_len - uri_len));
        flag = FLAG_SUCCESS;
    }
    else if (req_state == GCOAP_MEMO_TIMEOUT) {
        flag = FLAG_TIMEOUT;
    }

end:
    thread_flags_set((thread_t *)_waiter, flag);
}

static int _discover_internal(const sock_udp_ep_t *remote,
                              char *regif, size_t maxlen)
{
    coap_pkt_t pkt;

    /* save pointer to result buffer */
    _regif_buf = regif;
    _regif_buf_len = maxlen;

    /* do URI discovery for the registration interface */
    int res = gcoap_req_init(&pkt, buf, sizeof(buf), COAP_METHOD_GET,
                             "/.well-known/core");
    if (res < 0) {
        return RDCLI_ERR;
    }
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);
    gcoap_add_qstring(&pkt, "rt", "core.rd");
    size_t pkt_len = gcoap_finish(&pkt, 0, COAP_FORMAT_NONE);
    res = gcoap_req_send2(buf, pkt_len, remote, _on_discover);
    if (res < 0) {
        return RDCLI_ERR;
    }
    return _sync();
}

int rdcli_discover_regif(const sock_udp_ep_t *remote, char *regif, size_t maxlen)
{
    assert(remote && regif);

    _lock();
    int res = _discover_internal(remote, regif, maxlen);
    mutex_unlock(&_mutex);
    return res;
}

int rdcli_register(const sock_udp_ep_t *remote, const char *regif)
{
    assert(remote);

    int res;
    ssize_t pkt_len;
    int retval;
    coap_pkt_t pkt;

    _lock();

    /* if no registration interface is given, we will need to trigger a URI
     * discovery for it first (see section 5.2) */
    if (regif == NULL) {
        retval = _discover_internal(remote, _rd_regif, sizeof(_rd_regif));
        if (retval != RDCLI_OK) {
            goto end;
        }
    }
    else {
        if (strlen(_rd_regif) >= sizeof(_rd_regif)) {
            retval = RDCLI_OVERFLOW;
            goto end;
        }
        strncpy(_rd_regif, regif, sizeof(_rd_regif));
    }

    /* build and send CoAP POST request to the RD's registration interface */
    res = gcoap_req_init(&pkt, buf, sizeof(buf), COAP_METHOD_POST, _rd_regif);
    if (res < 0) {
        retval = RDCLI_ERR;
        goto end;
    }
    /* set some packet options and write query string */
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);
    cord_common_add_qstring(&pkt);

    /* add the resource description as payload */
    res = gcoap_get_resource_list(pkt.payload, pkt.payload_len,
                                  COAP_FORMAT_LINK);
    if (res < 0) {
        retval = RDCLI_ERR;
        goto end;
    }

    /* finish up the packet */
    pkt_len = gcoap_finish(&pkt, res, COAP_FORMAT_LINK);

    /* send out the request */
    res = gcoap_req_send2(buf, pkt_len, remote, _on_register);
    if (res < 0) {
        retval = RDCLI_ERR;
        goto end;
    }
    retval = _sync();

end:
    /* if we encountered any error, we mark the client as not connected */
    if (retval != RDCLI_OK) {
        _rd_loc[0] = '\0';
    }
#ifdef MODULE_RDCLI_STANDALONE
    else {
        rdcli_standalone_signal(true);
    }
#endif

    mutex_unlock(&_mutex);
    return retval;
}

int rdcli_update(void)
{
    _lock();
    int res = _update_remove(COAP_METHOD_POST, _on_update);
    if (res != RDCLI_OK) {
        /* in case we are not able to reach the RD, we drop the association */
#ifdef MODULE_RDCLI_STANDALONE
        rdcli_standalone_signal(false);
#endif
        _rd_loc[0] = '\0';
    }
    mutex_unlock(&_mutex);
    return res;
}

int rdcli_remove(void)
{
    _lock();
    if (_rd_loc[0] == '\0') {
        mutex_unlock(&_mutex);
        return RDCLI_NORD;
    }
#ifdef MODULE_RDCLI_STANDALONE
    rdcli_standalone_signal(false);
#endif
    _update_remove(COAP_METHOD_DELETE, _on_remove);
    /* we actually do not care about the result, we drop the RD local RD entry
     * in any case */
    _rd_loc[0] = '\0';
    mutex_unlock(&_mutex);
    return RDCLI_OK;
}

void rdcli_dump_status(void)
{
    puts("CoAP RD connection status:");

    if (_rd_loc[0] == 0) {
        puts("  --- not registered with any RD ---");
    }
    else {
        /* get address string */
        char addr[IPV6_ADDR_MAX_STR_LEN];
        ipv6_addr_to_str(addr, (ipv6_addr_t *)&_rd_remote.addr, sizeof(addr));

        printf("RD address: coap://[%s]:%i\n", addr, (int)_rd_remote.port);
        printf("   ep name: %s\n", cord_common_get_ep());
        printf("  lifetime: %is\n", (int)RDCLI_LT);
        printf("    reg if: %s\n", _rd_regif);
        printf("  location: %s\n", _rd_loc);
    }
}
