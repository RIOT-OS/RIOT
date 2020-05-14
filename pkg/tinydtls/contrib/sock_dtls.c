/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   tinydtls implementation of @ref net_sock_dtls
 *
 * @author  Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include "dtls.h"
#include "net/sock/dtls.h"
#include "net/credman.h"

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "dtls_debug.h"

#ifdef CONFIG_DTLS_PSK
static int _get_psk_info(struct dtls_context_t *ctx, const session_t *session,
                         dtls_credentials_type_t type,
                         const unsigned char *id, size_t id_len,
                         unsigned char *result, size_t result_length);
#endif /* CONFIG_DTLS_PSK */

#ifdef CONFIG_DTLS_ECC
static int _get_ecdsa_key(struct dtls_context_t *ctx, const session_t *session,
                          const dtls_ecdsa_key_t **result);

static int _verify_ecdsa_key(struct dtls_context_t *ctx,
                             const session_t *session,
                             const unsigned char *other_pub_x,
                             const unsigned char *other_pub_y,
                             size_t key_size);
#endif /* CONFIG_DTLS_ECC */

static int _write(struct dtls_context_t *ctx, session_t *session, uint8_t *buf,
                  size_t len);

static int _read(struct dtls_context_t *ctx, session_t *session, uint8_t *buf,
                 size_t len);
static int _event(struct dtls_context_t *ctx, session_t *session,
                  dtls_alert_level_t level, unsigned short code);

static void _session_to_ep(const session_t *session, sock_udp_ep_t *ep);
static void _ep_to_session(const sock_udp_ep_t *ep, session_t *session);
static uint32_t _update_timeout(uint32_t start, uint32_t timeout);

static dtls_handler_t _dtls_handler = {
    .event = _event,
    .write = _write,
    .read = _read,
#ifdef CONFIG_DTLS_PSK
    .get_psk_info = _get_psk_info,
#endif /* CONFIG_DTLS_PSK */
#ifdef CONFIG_DTLS_ECC
    .get_ecdsa_key = _get_ecdsa_key,
    .verify_ecdsa_key = _verify_ecdsa_key,
#endif /* CONFIG_DTLS_ECC */
};

static int _read(struct dtls_context_t *ctx, session_t *session, uint8_t *buf,
                 size_t len)
{
    (void)session;
    sock_dtls_t *sock = dtls_get_app_data(ctx);

    DEBUG("sock_dtls: decrypted message arrived\n");
    sock->buf = buf;
    sock->buflen = len;
    return len;
}

static int _write(struct dtls_context_t *ctx, session_t *session, uint8_t *buf,
                  size_t len)
{
    sock_dtls_t *sock = (sock_dtls_t *)dtls_get_app_data(ctx);
    sock_udp_ep_t remote;

    _session_to_ep(session, &remote);
    remote.family = AF_INET6;

    ssize_t res = sock_udp_send(sock->udp_sock, buf, len, &remote);
    if (res < 0) {
        DEBUG("sock_dtls: failed to send DTLS record: %d\n", (int)res);
    }
    return res;
}

static int _event(struct dtls_context_t *ctx, session_t *session,
                  dtls_alert_level_t level, unsigned short code)
{
    (void)level;
    (void)session;

    sock_dtls_t *sock = dtls_get_app_data(ctx);
    msg_t msg = { .type = code };
#ifdef ENABLE_DEBUG
    switch (code) {
        case DTLS_EVENT_CONNECT:
            DEBUG("sock_dtls: event connect\n");
            break;
        case DTLS_EVENT_CONNECTED:
            DEBUG("sock_dtls: event connected\n");
            break;
        case DTLS_EVENT_RENEGOTIATE:
            DEBUG("sock_dtls: event renegotiate\n");
            break;
    }
#endif  /* ENABLE_DEBUG */
    mbox_put(&sock->mbox, &msg);
    return 0;
}

#ifdef CONFIG_DTLS_PSK
static int _get_psk_info(struct dtls_context_t *ctx, const session_t *session,
                         dtls_credentials_type_t type,
                         const unsigned char *desc, size_t desc_len,
                         unsigned char *result, size_t result_length)
{
    (void)ctx;
    (void)desc;
    (void)desc_len;
    int ret;
    sock_dtls_session_t _session;
    sock_udp_ep_t ep;
    sock_dtls_t *sock = (sock_dtls_t *)dtls_get_app_data(ctx);

    _session_to_ep(session, &ep);
    memcpy(&_session.ep, &ep, sizeof(sock_udp_ep_t));
    memcpy(&_session.dtls_session, session, sizeof(session_t));

    credman_credential_t credential;
    ret = credman_get(&credential, sock->tag, CREDMAN_TYPE_PSK);
    if (ret < 0) {
        DEBUG("sock_dtls: no matching PSK credential found\n");
        return dtls_alert_fatal_create(DTLS_ALERT_DECRYPT_ERROR);
    }

    const void *c = NULL;
    size_t c_len = 0;
    switch (type) {
    case DTLS_PSK_HINT:
        DEBUG("sock_dtls: psk hint request\n");
        /* Ignored. See https://tools.ietf.org/html/rfc4279#section-5.2 */
        return 0;
    case DTLS_PSK_IDENTITY:
        DEBUG("sock_dtls: psk id request\n");
        c = credential.params.psk.id.s;
        c_len = credential.params.psk.id.len;
        break;
    case DTLS_PSK_KEY:
        DEBUG("sock_dtls: psk key request\n");
        c = credential.params.psk.key.s;
        c_len = credential.params.psk.key.len;
        break;
    default:
        DEBUG("sock:dtls unsupported request type: %d\n", type);
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    if (c_len > result_length) {
        DEBUG("sock_dtls: not enough memory for credential type: %d\n", type);
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }
    if (c == NULL || c_len == 0) {
        DEBUG("sock_dtls: invalid credential params for type %d\n", type);
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }
    memcpy(result, c, c_len);
    return c_len;
}
#endif /* CONFIG_DTLS_PSK */

#ifdef CONFIG_DTLS_ECC
static int _get_ecdsa_key(struct dtls_context_t *ctx, const session_t *session,
                          const dtls_ecdsa_key_t **result)
{
    (void)session;
    int ret;
    sock_dtls_t *sock = (sock_dtls_t *)dtls_get_app_data(ctx);

    credman_credential_t credential;
    ret = credman_get(&credential, sock->tag, CREDMAN_TYPE_ECDSA);
    if (ret < 0) {
            DEBUG("sock_dtls: no matching ecdsa credential found\n");
            return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    static dtls_ecdsa_key_t key;
    key.curve = DTLS_ECDH_CURVE_SECP256R1;
    key.priv_key = credential.params.ecdsa.private_key;
    key.pub_key_x = credential.params.ecdsa.public_key.x;
    key.pub_key_y = credential.params.ecdsa.public_key.y;
    *result = &key;
    return 0;
}

static int _verify_ecdsa_key(struct dtls_context_t *ctx,
                             const session_t *session,
                             const unsigned char *other_pub_x,
                             const unsigned char *other_pub_y, size_t key_size)
{
    (void) ctx;
    (void) session;
    (void) other_pub_y;
    (void) other_pub_x;
    (void) key_size;

    return 0;
}
#endif /* CONFIG_DTLS_ECC */

int sock_dtls_create(sock_dtls_t *sock, sock_udp_t *udp_sock,
                     credman_tag_t tag, unsigned version, unsigned role)
{
    assert(sock);
    assert(udp_sock);

    if (role != SOCK_DTLS_CLIENT && role != SOCK_DTLS_SERVER) {
        DEBUG("sock_dtls: invalid role\n");
        return -1;
    }

    /* check if tinydtls compiled with wanted DTLS version */
    if (version < SOCK_DTLS_1_0 || version > SOCK_DTLS_1_3) {
        DEBUG("sock_dtls: invalid version\n");
        return -1;
    }
    else if ((version == SOCK_DTLS_1_2) &&
        (DTLS_VERSION != 0xfefd)) {
        DEBUG("sock_dtls: tinydtls not compiled with support for DTLS 1.2\n");
        return -1;
    }
    else if (version == SOCK_DTLS_1_0 || version == SOCK_DTLS_1_3) {
        DEBUG("sock_dtls: tinydtls only support DTLS 1.2\n");
        return -1;
    }

    sock->udp_sock = udp_sock;
    sock->buf = NULL;
    sock->role = role;
    sock->tag = tag;
    sock->dtls_ctx = dtls_new_context(sock);
    if (!sock->dtls_ctx) {
        DEBUG("sock_dtls: error getting DTLS context\n");
        return -1;
    }
    mbox_init(&sock->mbox, sock->mbox_queue, SOCK_DTLS_MBOX_SIZE);
    dtls_set_handler(sock->dtls_ctx, &_dtls_handler);
    return 0;
}

int sock_dtls_session_create(sock_dtls_t *sock, const sock_udp_ep_t *ep,
                             sock_dtls_session_t *remote)
{
    uint8_t rcv_buffer[DTLS_HANDSHAKE_BUFSIZE];
    msg_t msg;
    ssize_t res;

    assert(sock);
    assert(ep);
    assert(remote);

    /* prepare a the remote party to connect to */
    memcpy(&remote->ep, ep, sizeof(sock_udp_ep_t));
    memcpy(&remote->dtls_session.addr, &ep->addr.ipv6, sizeof(ipv6_addr_t));
    _ep_to_session(ep, &remote->dtls_session);

    /* start a handshake */
    DEBUG("sock_dtls: starting handshake\n");
    res = dtls_connect(sock->dtls_ctx, &remote->dtls_session);
    if (res < 0) {
        DEBUG("sock_dtls: error establishing a session: %d\n", (int)res);
        return -ENOMEM;
    }
    else if (res == 0) {
        DEBUG("sock_dtls: session already exist. Skip establishing session\n");
        return 0;
    }

    /* receive all handshake messages or timeout if timer expires */
    while (!mbox_try_get(&sock->mbox, &msg) ||
            msg.type != DTLS_EVENT_CONNECTED) {
        res = sock_udp_recv(sock->udp_sock, rcv_buffer, sizeof(rcv_buffer),
                            DTLS_HANDSHAKE_TIMEOUT, &remote->ep);
        if (res <= 0) {
            DEBUG("sock_dtls: error receiving handshake messages: %d\n", (int)res);
            /* deletes peer created in dtls_connect() */
            dtls_peer_t *peer = dtls_get_peer(sock->dtls_ctx,
                                              &remote->dtls_session);
            dtls_reset_peer(sock->dtls_ctx, peer);
            return -ETIMEDOUT;
        }

        res = dtls_handle_message(sock->dtls_ctx, &remote->dtls_session,
                                  rcv_buffer, res);
        /* stop handshake if received fatal level alert */
        if (res == -1) {
            return res;
        }
    }
    return 0;
}

void sock_dtls_session_destroy(sock_dtls_t *sock, sock_dtls_session_t *remote)
{
    dtls_close(sock->dtls_ctx, &remote->dtls_session);
}

ssize_t sock_dtls_send(sock_dtls_t *sock, sock_dtls_session_t *remote,
                       const void *data, size_t len, uint32_t timeout)
{
    assert(sock);
    assert(remote);
    assert(data);

    /* check if session exists, if not create session first then send */
    if (!dtls_get_peer(sock->dtls_ctx, &remote->dtls_session)) {
        int res;

        if (timeout == 0) {
            return -ENOTCONN;
        }

        /* no session with remote, creating new session.
         * This will also create new peer for this session */
        res = dtls_connect(sock->dtls_ctx, &remote->dtls_session);
        if (res < 0) {
            DEBUG("sock_dtls: error initiating handshake\n");
            return -ENOMEM;
        }
        else if (res > 0) {
            /* handshake initiated, wait until connected or timed out */

            msg_t msg;
            bool is_timed_out = false;
            do {
                uint32_t start = xtimer_now_usec();
                res = xtimer_msg_receive_timeout(&msg, timeout);

                if (timeout != SOCK_NO_TIMEOUT) {
                    timeout = _update_timeout(start, timeout);
                    is_timed_out = (res < 0) || (timeout == 0);
                }
            }
            while (!is_timed_out && (msg.type != DTLS_EVENT_CONNECTED));
            if (is_timed_out &&  (msg.type != DTLS_EVENT_CONNECTED)) {
                DEBUG("sock_dtls: handshake process timed out\n");

                /* deletes peer created in dtls_connect() before */
                dtls_peer_t *peer = dtls_get_peer(sock->dtls_ctx,
                                                  &remote->dtls_session);
                dtls_reset_peer(sock->dtls_ctx, peer);
                return -ETIMEDOUT;
            }
        }
    }

    return dtls_write(sock->dtls_ctx, &remote->dtls_session, (uint8_t *)data, len);
}

static ssize_t _copy_buffer(sock_dtls_t *sock, void *data, size_t max_len)
{
    uint8_t *buf = sock->buf;
    size_t buflen = sock->buflen;

    sock->buf = NULL;
    if (buflen > max_len) {
        return -ENOBUFS;
    }
    /* use `memmove()` as tinydtls reuses `data` to store decrypted data with an
     * offset in `buf`. This prevents problems with overlapping buffers. */
    memmove(data, buf, buflen);
    return buflen;
}

ssize_t sock_dtls_recv(sock_dtls_t *sock, sock_dtls_session_t *remote,
                       void *data, size_t max_len, uint32_t timeout)
{
    assert(sock);
    assert(data);
    assert(remote);

    if (sock->buf != NULL) {
        /* there is already decrypted data available */
        return _copy_buffer(sock, data, max_len);
    }

    /* loop breaks when timeout or application data read */
    while (1) {
        uint32_t start_recv = xtimer_now_usec();
        ssize_t res = sock_udp_recv(sock->udp_sock, data, max_len, timeout,
                                    &remote->ep);
        if (res <= 0) {
            DEBUG("sock_dtls: error receiving UDP packet: %d\n", (int)res);
            return res;
        }

        _ep_to_session(&remote->ep, &remote->dtls_session);
        res = dtls_handle_message(sock->dtls_ctx, &remote->dtls_session,
                                  (uint8_t *)data, res);

        if ((timeout != SOCK_NO_TIMEOUT) && (timeout != 0)) {
            uint32_t time_passed = (xtimer_now_usec() - start_recv);
            timeout = (time_passed > timeout) ? 0: timeout - time_passed;
        }

        if (sock->buf != NULL) {
            return _copy_buffer(sock, data, max_len);
        }
        else if (timeout == 0) {
            DEBUG("sock_dtls: timed out while decrypting message\n");
            return -ETIMEDOUT;
        }
    }

}

void sock_dtls_close(sock_dtls_t *sock)
{
    dtls_free_context(sock->dtls_ctx);
}

void sock_dtls_init(void)
{
    dtls_init();
    dtls_set_log_level(TINYDTLS_LOG_LVL);
}

static void _ep_to_session(const sock_udp_ep_t *ep, session_t *session)
{
    session->port = ep->port;
    session->size = sizeof(ipv6_addr_t) +       /* addr */
                    sizeof(unsigned short);     /* port */
    session->ifindex = ep->netif;
    memcpy(&session->addr, &ep->addr.ipv6, sizeof(ipv6_addr_t));
}

static void _session_to_ep(const session_t *session, sock_udp_ep_t *ep)
{
    ep->port = session->port;
    ep->netif = session->ifindex;
    memcpy(&ep->addr.ipv6, &session->addr, sizeof(ipv6_addr_t));
}

static inline uint32_t _update_timeout(uint32_t start, uint32_t timeout)
{
    uint32_t diff = (xtimer_now_usec() - start);
    return (diff > timeout) ? 0: timeout - diff;
}

/** @} */
