/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_sock_dodtls
 * @{
 * @file
 * @brief   sock DNS client implementation
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#include "mutex.h"
#include "net/credman.h"
#include "net/dns.h"
#include "net/dns/cache.h"
#include "net/dns/msg.h"
#include "net/iana/portrange.h"
#include "net/sock/dtls.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"
#include "net/sock/dodtls.h"
#include "random.h"
#include "ztimer.h"

#define ENABLE_DEBUG    0
#include "debug.h"

/* min domain name length is 1, so minimum record length is 7 */
#define SOCK_DODTLS_MIN_REPLY_LEN           (unsigned)(sizeof(dns_hdr_t) + 7)
/* see https://datatracker.ietf.org/doc/html/rfc8094#section-3.1 */
#define SOCK_DODTLS_SESSION_TIMEOUT_MS      (15U * MS_PER_SEC)
#define SOCK_DODTLS_SESSION_RECV_TIMEOUT_MS (1U * MS_PER_SEC)

/* Socks to the DNS over DTLS server */
static uint8_t _dns_buf[CONFIG_DNS_MSG_LEN];
static sock_udp_t _udp_sock;
static sock_dtls_t _dtls_sock;
static sock_dtls_session_t _server_session;
/* Mutex to access server sock */
static mutex_t _server_mutex = MUTEX_INIT;
/* Type of the server credentials, stored for eventual credential deletion */
static credman_type_t _cred_type = CREDMAN_TYPE_EMPTY;
/* Tag of the server credentials, stored for eventual credential deletion */
static credman_tag_t _cred_tag = CREDMAN_TAG_EMPTY;
static uint16_t _id = 0;

static inline bool _server_set(void);
static int _connect_server(const sock_udp_ep_t *server,
                           const credman_credential_t *creds);
static int _disconnect_server(void);
static uint32_t _now_ms(void);
static void _sleep_ms(uint32_t delay);

int sock_dodtls_query(const char *domain_name, void *addr_out, int family)
{
    int res;
    uint16_t id;

    if (strlen(domain_name) > SOCK_DODTLS_MAX_NAME_LEN) {
        return -ENOSPC;
    }
    res = dns_cache_query(domain_name, addr_out, family);
    if (res) {
        return res;
    }
    if (!_server_set()) {
        return -ECONNREFUSED;
    }

    mutex_lock(&_server_mutex);
    id = _id++;
    for (int i = 0; i < CONFIG_SOCK_DODTLS_RETRIES; i++) {
        uint32_t timeout = CONFIG_SOCK_DODTLS_TIMEOUT_MS * US_PER_MS;
        uint32_t start, send_duration;
        size_t buflen = dns_msg_compose_query(_dns_buf, domain_name, id,
                                              family);

        start = _now_ms();
        res = sock_dtls_send(&_dtls_sock, &_server_session,
                             _dns_buf, buflen, timeout);
        send_duration = _now_ms() - start;
        if (send_duration > CONFIG_SOCK_DODTLS_TIMEOUT_MS) {
            return -ETIMEDOUT;
        }
        timeout -= send_duration;
        if (res <= 0) {
            _sleep_ms(timeout);
            continue;
        }
        res = sock_dtls_recv(&_dtls_sock, &_server_session,
                             _dns_buf, sizeof(_dns_buf), timeout);
        if (res > 0) {
            if (res > (int)SOCK_DODTLS_MIN_REPLY_LEN) {
                uint32_t ttl = 0;

                if ((res = dns_msg_parse_reply(_dns_buf, res, family,
                                               addr_out, &ttl)) > 0) {
                    dns_cache_add(domain_name, addr_out, res, ttl);
                    goto out;
                }
            }
            else {
                res = -EBADMSG;
            }
        }
    }

out:
    memset(_dns_buf, 0, sizeof(_dns_buf));  /* flush-out unencrypted data */
    mutex_unlock(&_server_mutex);
    return res;
}

int sock_dodtls_get_server(sock_udp_ep_t *server)
{
    int res = -ENOTCONN;

    assert(server != NULL);
    mutex_lock(&_server_mutex);
    if (_server_set()) {
        sock_udp_get_remote(&_udp_sock, server);
        res = 0;
    }
    mutex_unlock(&_server_mutex);
    return res;
}

sock_dtls_t *sock_dodtls_get_dtls_sock(void)
{
    return &_dtls_sock;
}

sock_dtls_session_t *sock_dodtls_get_server_session(void)
{
    return &_server_session;
}

int sock_dodtls_set_server(const sock_udp_ep_t *server,
                           const credman_credential_t *creds)
{
    return (server == NULL)
         ? _disconnect_server()
         : _connect_server(server, creds);
}

static inline bool _server_set(void)
{
    return _cred_type != CREDMAN_TYPE_EMPTY;
}

static void _close_session(credman_tag_t creds_tag, credman_type_t creds_type)
{
    sock_dtls_session_destroy(&_dtls_sock, &_server_session);
    sock_dtls_close(&_dtls_sock);
    credman_delete(creds_tag, creds_type);
    sock_udp_close(&_udp_sock);
}

static int _connect_server(const sock_udp_ep_t *server,
                           const credman_credential_t *creds)
{
    int res;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;

    /* server != NULL is checked in sock_dodtls_set_server() */
    assert(creds != NULL);
    mutex_lock(&_server_mutex);

    res = credman_add(creds);
    if (res < 0 && res != CREDMAN_EXIST) {
        DEBUG("Unable to add credential to credman\n");
        switch (res) {
            case CREDMAN_NO_SPACE:
                res = -ENOSPC;
                break;
            case CREDMAN_ERROR:
            case CREDMAN_INVALID:
            case CREDMAN_TYPE_UNKNOWN:
            default:
                res = -EINVAL;
                break;
        }
        goto exit;
    }

    res = sock_dtls_establish_session(&_udp_sock, &_dtls_sock, &_server_session,
                                      creds->tag, &local, server, _dns_buf,
                                      sizeof(_dns_buf));
    _cred_type = creds->type;
    _cred_tag = creds->tag;
    _id = (uint16_t)(random_uint32() & 0xffff);
exit:
    mutex_unlock(&_server_mutex);
    return (res > 0) ? 0 : res;
}

static int _disconnect_server(void)
{
    int res = 0;

    mutex_lock(&_server_mutex);
    if (!_server_set()) {
        goto exit;
    }
    _close_session(_cred_tag, _cred_type);
    _cred_tag = CREDMAN_TAG_EMPTY;
    _cred_type = CREDMAN_TYPE_EMPTY;
exit:
    mutex_unlock(&_server_mutex);
    return res;
}

static uint32_t _now_ms(void)
{
    return ztimer_now(ZTIMER_MSEC);
}

static void _sleep_ms(uint32_t delay)
{
    ztimer_sleep(ZTIMER_MSEC, delay);
}
