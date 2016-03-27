/*
 * Copyright (C) 2015 Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup net_gnrc_dns
 * @{
 *
 * @file
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#include <errno.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

#include "net/gnrc/dns/internal.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/udp.h"

/**
 * @brief   Allocate memory for the DNS thread's stack
 */
#if ENABLE_DEBUG
static char _stack[GNRC_DNS_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_DNS_STACK_SIZE];
#endif

static void *_event_loop(void *args);

kernel_pid_t _dns_pid = KERNEL_PID_UNDEF;
static gnrc_dns_request_t *_dns_requests[GNRC_DNS_MAX_SIMULTANEOUS_REQUESTS];
gnrc_dns_servers_t _dns_context; /**< DNS servers object (TODO currently only one, because we only have one interface) */
static uint16_t _request_id;

/* free the request from the running DNS requests */
static void _gnrc_dns_delete_request(gnrc_dns_request_t *req)
{
    for (int idx = 0; idx < GNRC_DNS_MAX_SIMULTANEOUS_REQUESTS; ++idx) {
        if (_dns_requests[idx] == req) {
            _dns_requests[idx] = NULL;
            return;
        }
    }
}

/* free the request from the running DNS requests */
static bool _gnrc_dns_add_request(gnrc_dns_request_t *req)
{
    for (int idx = 0; idx < GNRC_DNS_MAX_SIMULTANEOUS_REQUESTS; ++idx) {
        if (!_dns_requests[idx]) {
            _dns_requests[idx] = req;
            return true;
        }
    }

    return false;
}

/* free the request from the running DNS requests */
static bool _gnrc_dns_get_request(uint16_t id, gnrc_dns_request_t **req)
{
    for (int idx = 0; idx < GNRC_DNS_MAX_SIMULTANEOUS_REQUESTS; ++idx) {
        if (_dns_requests[idx] && _dns_requests[idx]->request_id == id) {
            *req = _dns_requests[idx];
            return true;
        }
    }

    return false;
}

int gnrc_dns_init(void)
{
    /* check if thread is already running */
    if (_dns_pid == KERNEL_PID_UNDEF) {
        /* start DNS thread */
        _dns_pid = thread_create(_stack, sizeof(_stack), GNRC_DNS_PRIO,
                                 THREAD_CREATE_STACKTEST, _event_loop, NULL, "dns");
    }
    return _dns_pid;
}

bool gnrc_dns_get_server(int index, gnrc_dns_server_info_t *info)
{
    timex_t time;
    gnrc_dns_server_t *server = _dns_context.servers + index;

    if (_dns_pid == KERNEL_PID_UNDEF || !info) {
        return false;
    }

    mutex_lock(&_dns_context.mutex);

    /* check if the server exists */
    if (_dns_context.count <= index) {
        mutex_unlock(&_dns_context.mutex);
        return false;
    }

    /* fill the server information object */
    memcpy(info->ip.u8, server->addr.u8, sizeof(ipv6_addr_t));

    /* calculate the expiration time */
    time = timex_from_uint64(xtimer_now64());
    if (server->expires != UINT32_MAX) {
        info->ttl = (time.seconds < server->expires)
                    ? (server->expires - time.seconds)
                    : 0;
    }
    else {
        info->ttl = 0;
    }

    mutex_unlock(&_dns_context.mutex);

    return true;
}

int gnrc_dns_update_entries(uint32_t lifetime, uint8_t items, ipv6_addr_t *servers)
{
    kernel_pid_t owner = thread_getpid();
    uint32_t first_expires, expires, count = 0;
    int write_index = 0;
    timex_t time;

    if (_dns_pid == KERNEL_PID_UNDEF) {
        return -EINVAL;
    }

    /* remove the DNS ttl timer */
    xtimer_remove(&(_dns_context.ttl));

    /* check if we can hold this many DNS servers */
    if (items > GNRC_DNS_SERVERS_SUPPORT) {
        DEBUG("dns: To many servers to be added to the collection.\n");
        items = GNRC_DNS_SERVERS_SUPPORT;
    }

    /* check if the lifetime is not set to 0 */
    if (!lifetime) {
        DEBUG("dns: The lifetime of the DNS entries is not valid.\n");
        items = 0;
    }

    DEBUG("dns: Update DNS server entries.\n");

    /* update our DNS entries */
    mutex_lock(&_dns_context.mutex);

    /* calculate own expire time */
    time = timex_from_uint64(xtimer_now64());
    if (lifetime != 0 && lifetime != UINT32_MAX) {
        expires = (time.seconds + lifetime);
    }
    else {
        expires = UINT32_MAX;
    }

    first_expires = expires;

    /* update the entries from the current calling thread */
    for (int idx = 0; idx < GNRC_DNS_SERVERS_SUPPORT; ++idx) {
        /* check if server entry is free */
        if (_dns_context.servers[idx].owner != KERNEL_PID_UNDEF
            && _dns_context.servers[idx].owner != owner) {
            first_expires = MIN(first_expires, _dns_context.servers[idx].expires);
            ++count;
            continue;
        }

        /* update the entry with the new entry */
        if (write_index < items) {
            ++count;

            _dns_context.servers[idx].owner = owner;
            _dns_context.servers[idx].expires = expires;
            memcpy(_dns_context.servers[idx].addr.u8, servers + write_index, sizeof(ipv6_addr_t));

            ++write_index;
        }
        else {
            memset(_dns_context.servers + idx, 0, sizeof(gnrc_dns_server_t));
        }
    }

    _dns_context.count = count;

    mutex_unlock(&_dns_context.mutex);

    /* calculate back the first expired lifetime */
    if (first_expires > 0) {
        first_expires -= time.seconds;
        _dns_context.ttl_msg.type = GNRC_DNS_MSG_SERVER_LIFETIME_EXPIRED;
        xtimer_set_msg(&(_dns_context.ttl), first_expires, &(_dns_context.ttl_msg), _dns_pid);
    }

    return write_index;
}

int gnrc_dns_add_server(ipv6_addr_t *server)
{
    int idx;

    /* validate parameters and arguments */
    if (_dns_pid == KERNEL_PID_UNDEF || !server
        || _dns_context.count >= GNRC_DNS_SERVERS_SUPPORT) {
        return -EINVAL;
    }

    mutex_lock(&_dns_context.mutex);

    /* find a spot where we can add this server */
    for (idx = 0; idx < GNRC_DNS_SERVERS_SUPPORT; ++idx) {
        if (!_dns_context.servers[idx].expires) {
            _dns_context.servers[idx].expires = UINT32_MAX;
            _dns_context.servers[idx].owner = thread_getpid();
            memcpy(_dns_context.servers[idx].addr.u8, server, sizeof(ipv6_addr_t));

            ++(_dns_context.count);

            break;
        }
    }

    mutex_unlock(&_dns_context.mutex);

    return idx < GNRC_DNS_SERVERS_SUPPORT
           ? idx
           : -EINVAL;
}

int gnrc_dns_del_server(ipv6_addr_t *server)
{
    kernel_pid_t owner = thread_getpid();
    int idx;

    /* validate parameters and arguments */
    if (_dns_pid == KERNEL_PID_UNDEF || !server) {
        return -EINVAL;
    }

    mutex_lock(&_dns_context.mutex);

    /* find a spot where we can add this server */
    for (idx = 0; idx < GNRC_DNS_SERVERS_SUPPORT; ++idx) {
        if (_dns_context.servers[idx].expires
            && _dns_context.servers[idx].owner == owner
            && ipv6_addr_equal(&(_dns_context.servers[idx].addr), server)) {

            if ((_dns_context.count - 1) > idx) {
                /* if this is the last entry in the list set it to zero */
                memcpy(_dns_context.servers + idx, _dns_context.servers + _dns_context.count - 1, sizeof(gnrc_dns_server_t));
                memset(_dns_context.servers + _dns_context.count - 1,  0, sizeof(gnrc_dns_server_t));
            }
            else {
                /* else copy the last entry over this entry */
                memset(_dns_context.servers + idx, 0, sizeof(gnrc_dns_server_t));
            }

            --(_dns_context.count);

            break;
        }
    }

    mutex_unlock(&_dns_context.mutex);

    return idx < GNRC_DNS_SERVERS_SUPPORT
           ? idx
           : -EINVAL;
}

bool gnrc_dns_resolve(const char *url, gnrc_dns_type_t request,
                      gnrc_dns_response_t *response)
{
    msg_t snd, recv;

    /* check if we have an DNS server */
    if (!_dns_context.count) {
        return false;
    }

    /* check if the query string is not to long */
    if (strlen(url) > (GNRC_DNS_MAX_QUERY_STR_LEN - 1)) {
        return false;
    }

    /* place the arguments in an request */
    gnrc_dns_request_t req;
    req.query_str = url;
    req.query_type = request;
    req.tries_left = GNRC_DNS_MAX_RETRIES;
    req.response = response;

    /* build request */
    snd.type = ((int) (GNRC_DNS_MSG_RESOLVE_A - GNRC_DNS_TYPE_A)) + request;
    snd.content.ptr = (void *) &req;
    snd.sender_pid = thread_getpid();

    /* resolve DNS entry in the DNS thread */
    if (!msg_send_receive(&snd, &recv, _dns_pid)) {
        response->type = GNRC_DNS_ERROR_MSG;
        return false;
    }

    /* response is valid if the response doesn't have an error */
    return !(response->type & GNRC_DNS_ERROR_MASK);
}

static void _received_reponse(gnrc_pktsnip_t *pkt)
{
    gnrc_dns_request_t *req = NULL;
    dns_header_t *header = (dns_header_t *) pkt->data;
    uint8_t *data;

    /* process the DNS header */
    uint16_t id = NTOHS(header->id);
    uint16_t queries = NTOHS(header->qd_count);
    uint16_t answeres = NTOHS(header->an_count);

    /* search the matching DNS request */
    if (!_gnrc_dns_get_request(id, &req)) {
        return;
    }

    /* remove our timeout timer */
    xtimer_remove(&req->timeout_timer);

    DEBUG("dns: received DNS response");

    /* process the DNS question sections */
    data = (uint8_t *) (header + 1);
    for (int idx = 0; idx < queries; ++idx) {
        data = _gnrc_dns_parse_name(pkt->data, data, req->response->full_name);

        /* stop processing on an error */
        if (!data) {
            answeres = 0;
            break;
        }

        data += sizeof(dns_qdata_footer_t);
    }

    /* process the DNS answer sections */
    int answere_idx = 0;
    req->response->responses = answeres;
    for (int idx = 0; idx < answeres && data; ++idx) {
        dns_query_respones_t *resp = (dns_query_respones_t *) data;

        if (resp->compressed.u8[0] != 0xc0) {
            /* new domain name found, decode the string back to a domain name */

            /* TODO maybe store this somewhere ? */
            resp = (dns_query_respones_t *)(_gnrc_dns_parse_name(pkt->data, data, NULL) - sizeof(resp->compressed));

            /* stop processing on an error */
            if (!resp) {
                data = NULL;
                break;
            }
        }
        else if (resp->compressed.u8[1] != sizeof(dns_header_t)) {
            /* compressed domain name found, but not the one in the query */

            /* TODO maybe store this somewhere ? _parse_query_domain(data + resp->compressed.u8[1], NULL); */
        }

        switch (resp->type) {
            case DNS_A:
                req->response->type = GNRC_DNS_TYPE_A;
                memcpy(req->response->data.a[answere_idx++].u8, resp->data,
                       HTONS(resp->data_len.u16));
                break;

            case DNS_AAAA:
                req->response->type = GNRC_DNS_TYPE_AAAA;
                memcpy(req->response->data.aaaa[answere_idx++].u8, resp->data,
                       HTONS(resp->data_len.u16));
                break;

            case DNS_TXT:
                req->response->type = GNRC_DNS_TYPE_TXT;
                memcpy(req->response->data.txt, resp->data + 1, *(resp->data));
                *((req->response->data.txt) + *(resp->data)) = 0;
                break;

            case DNS_SRV:
            {
                dns_srv_response_t *srv_resp = (dns_srv_response_t *)resp->data;

                req->response->type = GNRC_DNS_TYPE_SRV;

                /* process the SRV part in the response */
                req->response->data.srv[answere_idx].priority  = byteorder_ntohs(srv_resp->priority);
                req->response->data.srv[answere_idx].weight = byteorder_ntohs(srv_resp->weight);
                req->response->data.srv[answere_idx].port = byteorder_ntohs(srv_resp->port);
                data = _gnrc_dns_parse_name(pkt->data, srv_resp->target, req->response->data.srv[answere_idx].target);
                ++answere_idx;
            } break;

            case DNS_CNAME:
                --req->response->responses;
                data = _gnrc_dns_parse_name(pkt->data, resp->data, req->response->full_name);
                break;
        }
    }

    if (answeres && data) {
        msg_t m;
        m.sender_pid = req->request_thread;

        /* free the request from the running DNS requests */
        _gnrc_dns_delete_request(req);

        /* create the reply message */
        msg_t r;
        r.sender_pid = thread_getpid();
        r.content.value = 1;
        r.type = GNRC_DNS_MSG_QUERY_RESPONSE;

        msg_reply(&m, &r);
    }
    else {
        msg_t msg;
        msg.content.ptr = (void *) req;
        if (!_gnrc_dns_handle_request_retry(&msg, GNRC_DNS_ERROR_MSG)) {
            _gnrc_dns_delete_request(req);
        }
    }
}

static void _gnrc_dns_handle_request_message(msg_t *m, dns_query_type t)
{
    gnrc_dns_request_t *req = (gnrc_dns_request_t *) m->content.ptr;

    /* search place for the DNS request */
    if (!_gnrc_dns_add_request(req)) {
        msg_t r;
        r.type = GNRC_DNS_MSG_SERVER_LIFETIME_EXPIRED;
        msg_reply(m, &r);
        return;
    }

    /* fill the rest of the DNS request object */
    req->request_thread = m->sender_pid;
    req->query_type = t;
    req->request_id = ++_request_id;

    _gnrc_dns_send_request(m);
}

/* Function for the DNS thread */
void *_event_loop(void *args)
{
    msg_t msg;

    (void) args;

    /* create our netreg entry to register our listening port */
    gnrc_netreg_entry_t entry;
    entry.next = NULL;
    entry.pid = thread_getpid();
    entry.demux_ctx = GNRC_DNS_RESOLVE_SRC_PORT;

    /* register our DNS response listener */
    if (gnrc_netreg_register(GNRC_NETTYPE_UDP, &entry)) {
        DEBUG("dns: error starting server.");
        return NULL;
    }

    /* main processing loop */
    for (;; ) {
        /* wait for a message */
        msg_receive(&msg);

        /* process the received message */
        switch (msg.type) {
            case GNRC_DNS_MSG_RESOLVE_A:
                DEBUG("GNRC_DNS_MSG_RESOLVE_A\n");
                _gnrc_dns_handle_request_message(&msg, DNS_A);
                break;

            case GNRC_DNS_MSG_RESOLVE_AAAA:
                DEBUG("GNRC_DNS_MSG_RESOLVE_AAAA\n");
                _gnrc_dns_handle_request_message(&msg, DNS_AAAA);
                break;

            case GNRC_DNS_MSG_RESOLVE_TXT:
                DEBUG("GNRC_DNS_MSG_RESOLVE_TXT\n");
                _gnrc_dns_handle_request_message(&msg, DNS_TXT);
                break;

            case GNRC_DNS_MSG_RESOLVE_SRV:
                DEBUG("GNRC_DNS_MSG_RESOLVE_SRV\n");
                _gnrc_dns_handle_request_message(&msg, DNS_SRV);
                break;

            case GNRC_DNS_MSG_RESPONSE_TIMEDOUT:
                DEBUG("GNRC_DNS_MSG_RESPONSE_TIMEDOUT\n");
                if (!_gnrc_dns_handle_request_retry(&msg, GNRC_DNS_ERROR_TIMEDOUT)) {
                    _gnrc_dns_delete_request((gnrc_dns_request_t *) msg.content.ptr);
                }
                break;

            case GNRC_DNS_MSG_SERVER_LIFETIME_EXPIRED:
                DEBUG("dns: server lifetime's expired, removing DNS servers.\n");
                gnrc_dns_update_entries(0, 0, NULL);
                break;

            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("dns: GNRC_NETAPI_MSG_TYPE_RCV\n");
                _received_reponse((gnrc_pktsnip_t *) msg.content.ptr);
                break;

            default:
                DEBUG("dns: Received un-recognized message.\n");
                break;
        }
    }

    return NULL;
}

/**
 * @}
 */
