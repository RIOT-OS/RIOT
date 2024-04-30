/*
 * Copyright (C) 2017-2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_cord_lc
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory lookup implementation
 *
 * @author      Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>

#include "assert.h"
#include "mutex.h"
#include "thread_flags.h"

#include "net/gcoap.h"
#include "net/cord/lc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define FLAG_SUCCESS    (0x0001)
#define FLAG_TIMEOUT    (0x0002)
#define FLAG_ERR        (0x0004)
#define FLAG_OVERFLOW   (0x0008)
#define FLAG_NORSC      (0x0010)
#define FLAG_MASK       (0x00ff)

#define BUFSIZE         (CONFIG_GCOAP_PDU_BUF_SIZE)
#define MAX_EXPECTED_ATTRS (6)

/* Filter to limit number of link returned by RD server to 1 */
#define SINGLE_COUNT_FILTER {                                   \
            .key = "count", .key_len = strlen("count"),         \
            .value = "1", .value_len = strlen("1")              \
        }

/* Filter to continue request at page_str */
#define PAGE_FILTER(page_str) {                                 \
            .key = "page", .key_len = strlen("page"),           \
            .value = page_str, .value_len = strlen(page_str)    \
        }

/* Default filters that will be appended to existing filters for each request.
 * Consists of count=1 and page=last_page */
#define DEFAULT_FILTERS(page_str) { SINGLE_COUNT_FILTER, PAGE_FILTER(page_str) }

static void _lock(void);
static int _sync(void);
/* callback for _lookup_raw() request */
static void _on_lookup(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                       const sock_udp_ep_t *remote);
/* callback for _send_rd_init_req() */
static void _on_rd_init(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                        const sock_udp_ep_t *remote);
static ssize_t _add_filters_to_lookup(coap_pkt_t *pkt, cord_lc_filter_t *filters);
static int _send_rd_init_req(coap_pkt_t *pkt, const sock_udp_ep_t *remote,
                             void *buf, size_t maxlen);
/* do a lookup and returns payload of the response */
static ssize_t _lookup_raw(const cord_lc_rd_t *rd, unsigned content_format,
                           unsigned lookup_type, cord_lc_filter_t *filters,
                           void *result, size_t maxlen);

static char *_result_buf;
static size_t _result_buf_len;
static uint8_t reqbuf[CONFIG_GCOAP_PDU_BUF_SIZE] = {0};

static mutex_t _mutex = MUTEX_INIT;
static thread_t *_waiter;

static void _lock(void)
{
    mutex_lock(&_mutex);
    _waiter = thread_get_active();
}

static int _sync(void)
{
    thread_flags_t flags = thread_flags_wait_any(FLAG_MASK);

    if (flags & FLAG_ERR) {
        return CORD_LC_ERR;
    } else if (flags & FLAG_TIMEOUT) {
        return CORD_LC_TIMEOUT;
    } else if (flags & FLAG_OVERFLOW) {
        return CORD_LC_OVERFLOW;
    } else if (flags & FLAG_NORSC) {
        return CORD_LC_NORSC;
    } else {
        return CORD_LC_OK;
    }
}

static void _on_lookup(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                       const sock_udp_ep_t *remote)
{
    (void)remote;

    thread_flags_t flag = FLAG_ERR;

    if (memo->state == GCOAP_MEMO_RESP) {
        unsigned ct = coap_get_content_type(pdu);
        if (ct != COAP_FORMAT_LINK) {
            DEBUG("cord_lc: unsupported content format: %u\n", ct);
            thread_flags_set(_waiter, flag);
        }
        if (pdu->payload_len == 0) {
            flag = FLAG_NORSC;
            thread_flags_set(_waiter, flag);
        }
        if (pdu->payload_len >= _result_buf_len) {
            flag = FLAG_OVERFLOW;
            thread_flags_set(_waiter, flag);
        }
        memcpy(_result_buf, pdu->payload, pdu->payload_len);
        memset(_result_buf + pdu->payload_len, 0,
               _result_buf_len - pdu->payload_len);
        _result_buf_len = pdu->payload_len;
        flag = FLAG_SUCCESS;
    } else if (memo->state == GCOAP_MEMO_TIMEOUT) {
        flag = FLAG_TIMEOUT;
    }

    thread_flags_set(_waiter, flag);
}

static ssize_t _add_filters_to_lookup(coap_pkt_t *pkt, cord_lc_filter_t *filters)
{
    cord_lc_filter_t *f = filters;
    while (f) {
        for (unsigned i = 0; i < f->len; i++) {
            clif_attr_t *kv = &(f->array[i]);
            if (coap_opt_add_uri_query2(pkt, kv->key, kv->key_len, kv->value, kv->value_len) == -1) {
                return CORD_LC_OVERFLOW;
            }
        }
        f = f->next;
    }
    return CORD_LC_OK;
}

static ssize_t _lookup_raw(const cord_lc_rd_t *rd, unsigned content_format,
                           unsigned lookup_type, cord_lc_filter_t *filters,
                           void *result, size_t maxlen)
{
    assert(rd->remote);

    int res;
    int retval;
    coap_pkt_t pkt;
    ssize_t pkt_len;

    char *lookif = (lookup_type == CORD_LC_RES) ? rd->res_lookif : rd-> ep_lookif;
    res = gcoap_req_init(&pkt, reqbuf, sizeof(reqbuf), COAP_METHOD_GET, lookif);
    if (res < 0) {
        DEBUG("cord_lc: failed gcoap_req_init()\n");
        return CORD_LC_ERR;
    }

    /* save pointer to result */
    _result_buf = result;
    _result_buf_len = maxlen;

    /* add filters */
    res = _add_filters_to_lookup(&pkt, filters);
    if (res != CORD_LC_OK) {
        return res;
    }

    /* set packet options */
    if (content_format != COAP_FORMAT_LINK) {
        DEBUG("cord_lc: unsupported content format\n");
        return CORD_LC_ERR;
    }
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);
    coap_opt_add_uint(&pkt, COAP_OPT_ACCEPT, content_format);

    pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    if (pkt_len < 0) {
        return CORD_LC_ERR;
    }
    res = gcoap_req_send(reqbuf, pkt_len, rd->remote, _on_lookup, NULL, GCOAP_SOCKET_TYPE_UNDEF);
    if (res < 0) {
        return CORD_LC_ERR;
    }
    retval = _sync();
    return (retval == CORD_LC_OK) ? (int)_result_buf_len : retval;
}

static void _on_rd_init(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                       const sock_udp_ep_t *remote)
{
    (void)remote;

    thread_flags_t flag = FLAG_NORSC;
    size_t full_buf_len = _result_buf_len;
    _result_buf_len = 0;

    if (memo->state == GCOAP_MEMO_RESP) {
        unsigned ct = coap_get_content_type(pdu);
        if (ct != COAP_FORMAT_LINK) {
            DEBUG("cord_lc: error payload not in link format: %u\n", ct);
            goto end;
        }
        size_t size = pdu->payload_len;

        if (size == 0) {
            DEBUG("cord_lc: error empty payload\n");
            goto end;
        }
        if (size >= full_buf_len) {
            DEBUG("cord_lc: truncating response from %" PRIuSIZE " to %" PRIuSIZE "\n", size, full_buf_len);
            /* Not setting FLAG_OVERFLOW: There can still be valid
             * .well-known/core lookup data in the usable area, which will be
             * used as long as endpoint and resource lookup are both found */
            size = full_buf_len;
            memcpy(_result_buf, pdu->payload, full_buf_len);
        }
        else {
            memcpy(_result_buf, pdu->payload, size);
        }
        _result_buf_len = size;
        flag = FLAG_SUCCESS;
    } else if (memo->state == GCOAP_MEMO_TIMEOUT) {
        flag = FLAG_TIMEOUT;
    }

end:
    thread_flags_set(_waiter, flag);
}

static int _send_rd_init_req(coap_pkt_t *pkt, const sock_udp_ep_t *remote,
                             void *buf, size_t maxlen)
{

    int res = gcoap_req_init(pkt, buf, maxlen, COAP_METHOD_GET, "/.well-known/core");
    if (res < 0) {
        DEBUG("cord_lc: error gcoap_req_init() %d\n", res);
        return CORD_LC_ERR;
    }

    coap_hdr_set_type(pkt->hdr, COAP_TYPE_CON);
    coap_opt_add_uri_query(pkt, "rt", "core.rd-lookup-*");

    ssize_t pkt_len = coap_opt_finish(pkt, COAP_OPT_FINISH_NONE);
    if (pkt_len < 0) {
        DEBUG("cord_lc: error coap_opt_finish() %" PRIdSIZE "\n", pkt_len);
        return CORD_LC_ERR;
    }

    if (!gcoap_req_send(buf, pkt_len, remote, _on_rd_init, NULL, GCOAP_SOCKET_TYPE_UNDEF)) {
        DEBUG("cord_lc: error gcoap_req_send()\n");
        return CORD_LC_ERR;
    }
    return _sync();
}

int cord_lc_rd_init(cord_lc_rd_t *rd, void *buf, size_t maxlen,
                    const sock_udp_ep_t *remote)
{
    assert(remote);

    coap_pkt_t pkt;
    rd->remote = remote;

    _lock();
    memset(buf, '0', maxlen);
    _result_buf = buf;
    _result_buf_len = maxlen;

    int retval = _send_rd_init_req(&pkt, remote, buf, maxlen);
    if (retval != CORD_LC_OK) {
        DEBUG("cord_lc: failed to send req %d\n", retval);
        goto end;
    }

    /* Parse the payload */
    clif_t lookif;
    clif_attr_t attrs[MAX_EXPECTED_ATTRS];
    unsigned attrs_used = 0;
    size_t parsed_len = 0;
    /* Quitting the loop once everything we are interested in was found allows
     * us to succeed even if the data was truncated by a too small buffer */
    while ((!rd->res_lookif || !rd->ep_lookif) &&
           (parsed_len != _result_buf_len)) {

        ssize_t ret = clif_decode_link(&lookif, attrs + attrs_used,
                                       ARRAY_SIZE(attrs) - attrs_used,
                                       _result_buf + parsed_len,
                                       _result_buf_len - parsed_len);
        if (ret < 0) {
            DEBUG("cord_lc: error decoding payload %" PRIdSIZE "\n", ret);
            retval = CORD_LC_ERR;
            goto end;
        }

        attrs_used += lookif.attrs_len;
        parsed_len += ret;

        /* check if we found ep_lookif or res_lookif */
        for (unsigned i = 0; i < lookif.attrs_len; i++) {
            clif_attr_t *current_attr = (lookif.attrs + i);
            if (current_attr->value == NULL) {
                /* skip attributes that have no value */
                continue;
            }

            if (!strncmp(current_attr->value, "core.rd-lookup-res",
                         current_attr->value_len)) {
                rd->res_lookif = lookif.target;
                rd->res_lookif[lookif.target_len] = '\0';
            } else if (!strncmp(current_attr->value, "core.rd-lookup-ep",
                                current_attr->value_len)) {
                rd->ep_lookif = lookif.target;
                rd->ep_lookif[lookif.target_len] = '\0';
            }
        }
    }

    if (!rd->res_lookif && !rd->ep_lookif) {
        DEBUG("cord_lc: no lookup interfaces found\n");
        retval = CORD_LC_NORSC;
    } else {
        retval = parsed_len;
    }
end:
    mutex_unlock(&_mutex);
    return retval;
}

ssize_t cord_lc_raw(const cord_lc_rd_t *rd, unsigned content_format,
                    unsigned lookup_type, cord_lc_filter_t *filters,
                    void *result, size_t maxlen)
{
    _lock();
    ssize_t retval = _lookup_raw(rd, content_format, lookup_type, filters,
                                 result, maxlen);
    mutex_unlock(&_mutex);
    return retval;
}

ssize_t _lookup_result(cord_lc_rd_t *rd, cord_lc_res_t *result,
                       cord_lc_filter_t *filters, void *buf, size_t maxlen,
                       unsigned type)
{
    int retval;

    _lock();
    unsigned *page_ptr = (type == CORD_LC_EP)
                       ? &rd->ep_last_page : &rd->res_last_page;
    /* int will always fit in an 12-char array */
    char page_str[12];
    snprintf(page_str, sizeof(page_str), "%u", (*page_ptr)++);

    /* Append given filters to default filters (page, count).
     * If same filter are also specified by filters, assume the RD server will
     * use the value from last filter in the filter list */
    clif_attr_t default_attrs[] = DEFAULT_FILTERS(page_str);
    cord_lc_filter_t *all_filters = &(cord_lc_filter_t) {
        .array = default_attrs,
        .len = ARRAY_SIZE(default_attrs),
        .next = filters,
    };

    retval = _lookup_raw(rd, COAP_FORMAT_LINK, type, all_filters, buf, maxlen);
    if (retval < 0) {
        if (retval == CORD_LC_NORSC) {
            *page_ptr = 0;
        }
        DEBUG("cord_lc: error ep lookup failed\n");
        mutex_unlock(&_mutex);
        return retval;
    }

    /* parse the result */
    retval = clif_decode_link(&result->link, result->attrs, result->max_attrs,
                              buf, retval);
    if (retval < 0) {
        DEBUG("cord_lc: no endpoint link found\n");
        retval = CORD_LC_ERR;
    }
    mutex_unlock(&_mutex);
    return retval;
}
