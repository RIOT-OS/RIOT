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

#include <assert.h>

#include "dtls.h"
#include "log.h"
#include "net/sock/dtls.h"
#include "net/credman.h"

#if SOCK_HAS_ASYNC
#include "net/sock/async.h"
#include "net/sock/async/event.h"
#endif

#define ENABLE_DEBUG 0
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
    sock->buffer.data = buf;
    sock->buffer.datalen = len;
    sock->buffer.session = session;
#ifdef SOCK_HAS_ASYNC
    if (sock->async_cb != NULL) {
        /* reset retrievable event session */
        memset(&sock->async_cb_session, 0, sizeof(sock->async_cb_session));
        sock->async_cb(sock, SOCK_ASYNC_MSG_RECV, sock->async_cb_arg);
    }
#endif
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
    msg_t msg = { .type = code, .content.ptr = session };
    if (IS_ACTIVE(ENABLE_DEBUG)) {
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
    }
    if (!level && (code != DTLS_EVENT_CONNECT)) {
        mbox_put(&sock->mbox, &msg);
    }
#ifdef SOCK_HAS_ASYNC
    if (sock->async_cb != NULL) {
        switch (code) {
            case DTLS_ALERT_CLOSE_NOTIFY:
                /* peer closed their session */
                memcpy(&sock->async_cb_session, session, sizeof(session_t));
                sock->async_cb(sock, SOCK_ASYNC_CONN_FIN, sock->async_cb_arg);
                break;
            case DTLS_EVENT_CONNECTED:
                /* we received a session handshake initialization */
                sock->async_cb(sock, SOCK_ASYNC_CONN_RECV,
                               sock->async_cb_arg);
                break;
            default:
                break;
        }
    }
#endif
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
    (void)session;
    int ret;

    sock_dtls_t *sock = dtls_get_app_data(ctx);

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
        /* return a hint to the client if set */
        c_len = strlen(sock->psk_hint);
        if (c_len) {
            c = sock->psk_hint;
            break;
        }
        else {
            DEBUG("sock_dtls: no hint provided\n");
            return 0;
        }
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
    sock->buffer.data = NULL;
    sock->psk_hint[0] = '\0';
    sock->client_psk_cb = NULL;
#ifdef SOCK_HAS_ASYNC
    sock->async_cb = NULL;
    sock->buf_ctx = NULL;
    memset(&sock->async_cb_session, 0, sizeof(sock->async_cb_session));
#endif /* SOCK_HAS_ASYNC */

    memset(sock->tags, CREDMAN_TAG_EMPTY, CONFIG_DTLS_CREDENTIALS_MAX * sizeof(credman_tag_t));
    sock->tags_len = 1;
    sock->tags[0] = tag;

    sock->role = role;
    sock->dtls_ctx = dtls_new_context(sock);
    if (!sock->dtls_ctx) {
        DEBUG("sock_dtls: error getting DTLS context\n");
        return -1;
    }
    mbox_init(&sock->mbox, sock->mbox_queue, SOCK_DTLS_MBOX_SIZE);
    dtls_set_handler(sock->dtls_ctx, &_dtls_handler);
    return 0;
}

int sock_dtls_set_server_psk_id_hint(sock_dtls_t *sock, const char *hint)
{
    assert(sock);
    if (strlen(hint) > CONFIG_DTLS_PSK_ID_HINT_MAX_SIZE) {
        DEBUG("sock_dtls: could not set hint due to buffer size\n");
        return -1;
    }
    strcpy(sock->psk_hint, hint);
    return 0;
}

int sock_dtls_add_credential(sock_dtls_t *sock, credman_tag_t tag)
{
    assert(sock);
    if (sock->tags_len < CONFIG_DTLS_CREDENTIALS_MAX) {
        DEBUG("sock_dtls: credential added in position %d\n", sock->tags_len);
        sock->tags[sock->tags_len] = tag;
        sock->tags_len++;
        return 0;
    }
    DEBUG("sock_dtls: could not add new credential\n");
    return -1;
}

int sock_dtls_remove_credential(sock_dtls_t *sock, credman_tag_t tag)
{
    assert(sock);
    int pos = -1;
    for (unsigned i = 0; i < sock->tags_len; i++) {
        if (sock->tags[i] == tag) {
            pos = i;
            DEBUG("sock_dtls: found credential to remove in position %i\n", pos);
            break;
        }
    }

    if (pos >= 0) {
        sock->tags_len--;
        for (; (unsigned)pos < sock->tags_len; pos++) {
            sock->tags[pos] = sock->tags[pos + 1];
        }
        return 0;
    }
    else {
        DEBUG("sock_dtls: could not find credential to remove\n");
        return -1;
    }
}

size_t sock_dtls_get_credentials(sock_dtls_t *sock, const credman_tag_t **out)
{
    assert(sock);
    assert(out);

    *out = sock->tags;
    return sock->tags_len;
}

void sock_dtls_set_client_psk_cb(sock_dtls_t *sock, sock_dtls_client_psk_cb_t cb)
{
    assert(sock);
    sock->client_psk_cb = cb;
}

sock_udp_t *sock_dtls_get_udp_sock(sock_dtls_t *sock)
{
    assert(sock);
    return sock->udp_sock;
}

int sock_dtls_session_init(sock_dtls_t *sock, const sock_udp_ep_t *ep,
                           sock_dtls_session_t *remote)
{
    assert(sock);
    assert(ep);
    assert(remote);

    sock_udp_ep_t local;
    if (!sock->udp_sock || (sock_udp_get_local(sock->udp_sock, &local) < 0)) {
        return -EADDRNOTAVAIL;
    }
    if (ep->port == 0) {
        return -EINVAL;
    }
    switch (ep->family) {
        case AF_INET:
 #if IS_ACTIVE(SOCK_HAS_IPV6)
        case AF_INET6:
 #endif
            break;
        default:
            return -EINVAL;
    }

    /* prepare the remote party to connect to */
    _ep_to_session(ep, &remote->dtls_session);

    /* start the handshake */
    int res = dtls_connect(sock->dtls_ctx, &remote->dtls_session);
    if (res < 0) {
        DEBUG("sock_dtls: error establishing a session: %d\n", res);
        return -ENOMEM;
    }
    else if (res == 0) {
        DEBUG("sock_dtls: session already exist. Skip establishing session\n");
        return 0;
    }

    /* New handshake initiated */
    return 1;
}

void sock_dtls_session_destroy(sock_dtls_t *sock, sock_dtls_session_t *remote)
{
    dtls_close(sock->dtls_ctx, &remote->dtls_session);
}

void sock_dtls_session_get_udp_ep(const sock_dtls_session_t *session,
                                  sock_udp_ep_t *ep)
{
    assert(session);
    assert(ep);

    _session_to_ep(&session->dtls_session, ep);
}

void sock_dtls_session_set_udp_ep(sock_dtls_session_t *session,
                                  const sock_udp_ep_t *ep)
{
    assert(session);
    assert(ep);

    _ep_to_session(ep, &session->dtls_session);
}

ssize_t sock_dtls_send_aux(sock_dtls_t *sock, sock_dtls_session_t *remote,
                           const void *data, size_t len, uint32_t timeout,
                           sock_dtls_aux_tx_t *aux)
{
    (void)aux;
    int res;

    assert(sock);
    assert(remote);
    assert(data);

    /* check if session exists, if not create session first then send */
    if (!dtls_get_peer(sock->dtls_ctx, &remote->dtls_session)) {
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

    res = dtls_write(sock->dtls_ctx, &remote->dtls_session,
                     (uint8_t *)data, len);
#ifdef SOCK_HAS_ASYNC
    if ((res >= 0) && (sock->async_cb != NULL)) {
        sock->async_cb(sock, SOCK_ASYNC_MSG_SENT, sock->async_cb_arg);
    }
#endif /* SOCK_HAS_ASYNC */
    return res;
}

#if SOCK_HAS_ASYNC
/**
 * @brief   Checks for and iterates for more data chunks within the network
 *          stacks anternal packet buffer
 *
 * When no more chunks exists, `data_ctx` assures cleaning up the internal
 * buffer state and `sock_udp_recv_buf()` returns 0.
 *
 * @see @ref sock_udp_recv_buf().
 */
static void _check_more_chunks(sock_udp_t *udp_sock, void **data,
                               void **data_ctx, sock_udp_ep_t *remote)
{
    ssize_t res;

    while ((res = sock_udp_recv_buf(udp_sock, data, data_ctx, 0, remote)) > 0) {
        /* TODO: remove and adapt _copy_buffer() to add remaining data when
         * tinydtls supports chunked datagram payload */
        if (IS_ACTIVE(DEVELHELP)) {
            LOG_ERROR("sock_dtls: Chunked datagram payload currently not "
                      "supported yet by tinydtls\n");
        }
    }
}
#endif

static inline void _copy_session(sock_dtls_t *sock, sock_dtls_session_t *remote)
{
    memcpy(&remote->dtls_session, sock->buffer.session,
           sizeof(remote->dtls_session));
}

static ssize_t _copy_buffer(sock_dtls_t *sock, sock_dtls_session_t *remote,
                            void *data, size_t max_len)
{
    uint8_t *buf = sock->buffer.data;
    size_t buflen = sock->buffer.datalen;

    sock->buffer.data = NULL;
    if (buflen > max_len) {
        return -ENOBUFS;
    }
#if SOCK_HAS_ASYNC
    sock_udp_ep_t ep;
    _session_to_ep(&remote->dtls_session, &ep);

    if (sock->buf_ctx != NULL) {
        memcpy(data, buf, sock->buffer.datalen);
        _copy_session(sock, remote);
        _check_more_chunks(sock->udp_sock, (void **)&buf, &sock->buf_ctx,
                           &ep);
        if (sock->async_cb &&
            /* is there a message in the sock's mbox? */
            mbox_avail(&sock->mbox)) {
            if (sock->buffer.data) {
                sock->async_cb(sock, SOCK_ASYNC_MSG_RECV,
                               sock->async_cb_arg);
            }
            else {
                sock->async_cb(sock, SOCK_ASYNC_CONN_RECV,
                               sock->async_cb_arg);
            }
        }
        return buflen;
    }
#else
    (void)remote;
#endif
    /* use `memmove()` as tinydtls reuses `data` to store decrypted data with an
     * offset in `buf`. This prevents problems with overlapping buffers. */
    memmove(data, buf, buflen);
    _copy_session(sock, remote);
    return buflen;
}

static ssize_t _complete_handshake(sock_dtls_t *sock,
                                   sock_dtls_session_t *remote,
                                   const session_t *session)
{
    memcpy(&remote->dtls_session, session, sizeof(remote->dtls_session));
#ifdef SOCK_HAS_ASYNC
    if (sock->async_cb) {
        sock_async_flags_t flags = SOCK_ASYNC_CONN_RDY;

        if (mbox_avail(&sock->mbox)) {
            if (sock->buffer.data) {
                flags |= SOCK_ASYNC_MSG_RECV;
            }
            else {
                flags |= SOCK_ASYNC_CONN_RECV;
            }
        }
        memcpy(&sock->async_cb_session, session, sizeof(session_t));
        sock->async_cb(sock, flags, sock->async_cb_arg);
    }
#else
    (void)sock;
#endif
    return -SOCK_DTLS_HANDSHAKE;
}

ssize_t sock_dtls_recv_aux(sock_dtls_t *sock, sock_dtls_session_t *remote,
                           void *data, size_t max_len, uint32_t timeout,
                           sock_dtls_aux_rx_t *aux)
{
    assert(sock);
    assert(data);
    assert(remote);

    sock_udp_ep_t ep;

    /* loop breaks when timeout or application data read */
    while (1) {
        ssize_t res;
        uint32_t start_recv = xtimer_now_usec();
        msg_t msg;

        if (sock->buffer.data != NULL) {
            return _copy_buffer(sock, remote, data, max_len);
        }
        else if (mbox_try_get(&sock->mbox, &msg) &&
                 msg.type == DTLS_EVENT_CONNECTED) {
            return _complete_handshake(sock, remote, msg.content.ptr);
        }
        /* Crude way to somewhat test that `sock_dtls_aux_rx_t` and
         * `sock_udp_aux_rx_t` remain compatible: */
        static_assert(sizeof(sock_dtls_aux_rx_t) == sizeof(sock_udp_aux_rx_t),
                      "sock_dtls_aux_rx_t became incompatible with "
                      "sock_udp_aux_rx_t");
        res = sock_udp_recv_aux(sock->udp_sock, data, max_len, timeout,
                                &ep, (sock_udp_aux_rx_t *)aux);
        if (res <= 0) {
            DEBUG("sock_dtls: error receiving UDP packet: %d\n", (int)res);
            return res;
        }

        _ep_to_session(&ep, &remote->dtls_session);
        res = dtls_handle_message(sock->dtls_ctx, &remote->dtls_session,
                                  (uint8_t *)data, res);

        if ((timeout != SOCK_NO_TIMEOUT) && (timeout != 0)) {
            timeout = _update_timeout(start_recv, timeout);
        }
        if (timeout == 0) {
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
    ep->family = AF_INET6;
    memcpy(&ep->addr.ipv6, &session->addr, sizeof(ipv6_addr_t));
}

static inline uint32_t _update_timeout(uint32_t start, uint32_t timeout)
{
    uint32_t diff = (xtimer_now_usec() - start);
    return (diff > timeout) ? 0: timeout - diff;
}

#ifdef SOCK_HAS_ASYNC
bool sock_dtls_get_event_session(sock_dtls_t *sock,
                                 sock_dtls_session_t *session)
{
    assert(sock);
    assert(session);
    if (sock->async_cb_session.size > 0) {
        memcpy(&session->dtls_session, &sock->async_cb_session,
               sizeof(sock->async_cb_session));
        return true;
    }
    return false;
}

void _udp_cb(sock_udp_t *udp_sock, sock_async_flags_t flags, void *ctx)
{
    sock_dtls_t *sock = ctx;

    if (flags & SOCK_ASYNC_MSG_RECV) {
        session_t remote;
        sock_udp_ep_t remote_ep;
        void *data = NULL;
        void *data_ctx = NULL;

        ssize_t res = sock_udp_recv_buf(udp_sock, &data, &data_ctx, 0,
                                        &remote_ep);
        if (res <= 0) {
            DEBUG("sock_dtls: error receiving UDP packet: %d\n", (int)res);
            return;
        }

        /* prevent overriding already set `buf_ctx` */
        if (sock->buf_ctx != NULL) {
            DEBUG("sock_dtls: unable to store buffer asynchronously\n");
            _check_more_chunks(udp_sock, &data, &data_ctx, &remote_ep);
            return;
        }
        _ep_to_session(&remote_ep, &remote);
        sock->buf_ctx = data_ctx;
        res = dtls_handle_message(sock->dtls_ctx, &remote,
                                  data, res);
        if (sock->buffer.data == NULL) {
            _check_more_chunks(udp_sock, &data, &data_ctx, &remote_ep);
            sock->buf_ctx = NULL;
        }
    }
    if ((flags & SOCK_ASYNC_PATH_PROP) && sock->async_cb) {
        /* just hand this event type up the stack */
        sock->async_cb(sock, SOCK_ASYNC_PATH_PROP, sock->async_cb_arg);
    }
}

void sock_dtls_set_cb(sock_dtls_t *sock, sock_dtls_cb_t cb, void *cb_arg)
{
    sock->async_cb = cb;
    sock->async_cb_arg = cb_arg;
    if (IS_USED(MODULE_SOCK_ASYNC_EVENT)) {
        sock_async_ctx_t *ctx = sock_dtls_get_async_ctx(sock);
        if (ctx->queue) {
            sock_udp_event_init(sock->udp_sock, ctx->queue, _udp_cb, sock);
            return;
        }
    }
    sock_udp_set_cb(sock->udp_sock, _udp_cb, sock);
}

#ifdef SOCK_HAS_ASYNC_CTX
sock_async_ctx_t *sock_dtls_get_async_ctx(sock_dtls_t *sock)
{
    return &sock->async_ctx;
}
#endif  /* SOCK_HAS_ASYNC_CTX */
#endif  /* SOCK_HAS_ASYNC */

/** @} */
