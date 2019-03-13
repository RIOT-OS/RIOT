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
 * @brief       tinydtls implementation of @ref net_sock_dtls
 *
 * @author  Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include "dtls.h"
#include "net/sock/dtls.h"

#define ENABLE_DEBUG (1)
#include "debug.h"
#include "dtls_debug.h"

#define DTLS_EVENT_READ         (0x01E0)
#define DTLS_EVENT_TIMEOUT      (0x01E1)

#define DTLS_HANDSHAKE_TIMEOUT  (1000000U)
#define DTLS_HANDSHAKE_BUFSIZE  (512U)

static void _timeout_callback(void *arg);
static int _is_timed_out(uint32_t timeout);

#ifdef DTLS_PSK
static int _get_psk_info(struct dtls_context_t *ctx, const session_t *session,
                         dtls_credentials_type_t type,
                         const unsigned char *id, size_t id_len,
                         unsigned char *result, size_t result_length);
#endif /* DTLS_PSK */

#ifdef DTLS_ECC
static int _get_ecdsa_key(struct dtls_context_t *ctx, const session_t *session,
                          const dtls_ecdsa_key_t **result);

static int _verify_ecdsa_key(struct dtls_context_t *ctx,
                             const session_t *session,
                             const unsigned char *other_pub_x,
                             const unsigned char *other_pub_y,
                             size_t key_size);
#endif /* DTLS_ECC */

static int _write(struct dtls_context_t *ctx, session_t *session, uint8_t *buf,
                  size_t len);

static int _read(struct dtls_context_t *ctx, session_t *session, uint8_t *buf,
                 size_t len);
static int _event(struct dtls_context_t *ctx, session_t *session,
                  dtls_alert_level_t level, unsigned short code);

static void _session_to_ep(const session_t *session, sock_udp_ep_t *ep);
static void _ep_to_session(const sock_udp_ep_t *ep, session_t *session);

static dtls_handler_t _dtls_handler = {
    .event = _event,
#ifdef DTLS_PSK
    .get_psk_info = _get_psk_info,
#endif /* DTLS_PSK */
#ifdef DTLS_ECC
    .get_ecdsa_key = _get_ecdsa_key,
    .verify_ecdsa_key = _verify_ecdsa_key,
#endif /* DTLS_ECC */
    .write = _write,
    .read = _read,
};

static int _read(struct dtls_context_t *ctx, session_t *session, uint8_t *buf,
                 size_t len)
{
    (void)session;
    msg_t msg = { .type = DTLS_EVENT_READ };
    sock_dtls_t *sock = dtls_get_app_data(ctx);

    DEBUG("Decrypted message arrived\n");
    if (sock->buflen < len && sock->buf) {
        DEBUG("Not enough place on buffer for decrypted message\n");
        msg.content.value = -ENOBUFS;
    }
    else {
        memcpy(sock->buf, buf, len);
        msg.content.value = len;
    }
    mbox_put(&sock->mbox, &msg);

    /* this will be ignored anyway */
    return 0;
}

static int _write(struct dtls_context_t *ctx, session_t *session, uint8_t *buf,
                  size_t len)
{
    sock_dtls_t *sock = (sock_dtls_t *)dtls_get_app_data(ctx);
    sock_udp_ep_t remote;

    _session_to_ep(session, &remote);
    remote.family = AF_INET6;

    ssize_t res = sock_udp_send(sock->udp_sock, buf, len, &remote);
    if (res <= 0) {
        DEBUG("Error: Failed to send DTLS record: %d\n", res);
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
    switch(code) {
        case DTLS_EVENT_CONNECT:
            DEBUG("Event connect\n");
            break;
        case DTLS_EVENT_CONNECTED:
            DEBUG("Event connected\n");
            break;
        case DTLS_EVENT_RENEGOTIATE:
            DEBUG("Event renegotiate\n");
            break;
    }
#endif  /* ENABLE_DEBUG */
    mbox_put(&sock->mbox, &msg);
    return 0;
}

#ifdef DTLS_PSK
static int _get_psk_info(struct dtls_context_t *ctx, const session_t *session,
                         dtls_credentials_type_t type,
                         const unsigned char *id, size_t id_len,
                         unsigned char *result, size_t result_length)
{
    (void)session;
    sock_dtls_t *sock = (sock_dtls_t *)dtls_get_app_data(ctx);
    sock_dtls_session_t _session;
    sock_udp_ep_t ep;

    _session_to_ep(session, &ep);
    memcpy(&_session.remote_ep, &ep, sizeof(sock_udp_ep_t));
    memcpy(&_session.dtls_session, session, sizeof(session_t));
    switch(type) {
        case DTLS_PSK_HINT:
            if (sock->psk.psk_hint_storage) {
                return sock->psk.psk_hint_storage(sock, &_session, result,
                                                  result_length);
            }
            return 0;

        case DTLS_PSK_IDENTITY:
            DEBUG("psk id request\n");
            if (sock->psk.psk_id_storage) {
                return sock->psk.psk_id_storage(sock, &_session, id, id_len,
                                                result, result_length);
            }
            return 0;
        case DTLS_PSK_KEY:
            if (sock->psk.psk_key_storage) {
                return sock->psk.psk_key_storage(sock, &_session, id, id_len,
                                                 result, result_length);
            }
            return 0;
        default:
            DEBUG("Unsupported request type: %d\n", type);
            return 0;
    }
}
#endif

#ifdef DTLS_ECC
static int _get_ecdsa_key(struct dtls_context_t *ctx, const session_t *session,
                          const dtls_ecdsa_key_t **result)
{
    dtls_ecdsa_key_t *key;
    sock_dtls_t *sock = (sock_dtls_t *)dtls_get_app_data(ctx);
    sock_dtls_session_t _session;
    sock_udp_ep_t ep;
    if (sock->ecdsa.ecdsa_storage) {
        _session_to_ep(session, &ep);
        memcpy(&_session.remote_ep, &ep, sizeof(sock_udp_ep_t));
        memcpy(&_session.dtls_session, session, sizeof(session_t));
        if (sock->ecdsa.ecdsa_storage(sock, &_session, &key) < 0) {
            DEBUG("Could not get the ECDSA key\n");
            return -1;
        }
        *result = key;
        return 0;
    }
    DEBUG("no ecdsa storage registered\n");
    return -1;
}

static int _verify_ecdsa_key(struct dtls_context_t *ctx,
                             const session_t *session,
                             const unsigned char *other_pub_x,
                             const unsigned char *other_pub_y, size_t key_size)
{
    sock_dtls_session_t _session;
    sock_udp_ep_t ep;
    sock_dtls_t *sock = (sock_dtls_t *)dtls_get_app_data(ctx);
    if (sock->ecdsa.ecdsa_verify) {
        _session_to_ep(session, &ep);
        memcpy(&_session.remote_ep, &ep, sizeof(sock_udp_ep_t));
        memcpy(&_session.dtls_session, session, sizeof(session_t));
        if (sock->ecdsa.ecdsa_verify(sock, &_session, other_pub_x, other_pub_y,
                                     key_size)) {
            DEBUG("Could not verify ECDSA public keys\n");
            return -1;
        }
    }
    return 0;
}
#endif /* DTLS_ECC */

void sock_dtls_init(void)
{
    dtls_init();
    /* uncomment this for debug log */
    //dtls_set_log_level(DTLS_LOG_DEBUG);
}

int sock_dtls_create(sock_dtls_t *sock, sock_udp_t *udp_sock, unsigned method)
{
    (void)method;
    assert(sock && udp_sock);
    sock->udp_sock = udp_sock;
    sock->dtls_ctx = dtls_new_context(sock);
    sock->role = DTLS_CLIENT;
    sock->queue = NULL;
    if (!sock->dtls_ctx) {
        DEBUG("Error while getting a DTLS context\n");
        return -1;
    }
    mbox_init(&sock->mbox, sock->mbox_queue, SOCK_DTLS_MBOX_SIZE);
    dtls_set_handler(sock->dtls_ctx, &_dtls_handler);
    return 0;
}

void sock_dtls_init_server(sock_dtls_t *sock, sock_dtls_queue_t *queue,
                           sock_dtls_session_t *queue_array, unsigned len)
{
    assert(queue && queue_array && (len > 0));

    queue->array = queue_array;
    queue->len = len;
    queue->used = 0;
    sock->queue = queue;
    sock->role = DTLS_SERVER;
}

int sock_dtls_establish_session(sock_dtls_t *sock, sock_udp_ep_t *ep,
                                sock_dtls_session_t *remote)
{
    uint8_t rcv_buffer[DTLS_HANDSHAKE_BUFSIZE];
    msg_t msg;

    assert(sock && ep && remote);

    /* prepare a the remote party to connect to */
    memcpy(&remote->remote_ep, ep, sizeof(sock_udp_ep_t));
    memcpy(&remote->dtls_session.addr, &ep->addr.ipv6, sizeof(ipv6_addr_t));
    _ep_to_session(ep, &remote->dtls_session);

    /* start a handshake */
    DEBUG("Starting handshake\n");
    if (dtls_connect(sock->dtls_ctx, &remote->dtls_session) < 0) {
        DEBUG("Error establishing a session\n");
        return -1;
    }
    DEBUG("Waiting for ClientHello to be sent\n");
    mbox_get(&sock->mbox, &msg);
    if (msg.type != DTLS_EVENT_CONNECT) {
        DEBUG("DTLS handshake was not started\n");
        return -1;
    }
    DEBUG("ClientHello sent, waiting for handshake\n");
    int udp_count = 0;

    /* receive all handshake messages or timeout if timer expires */
    while (!mbox_try_get(&sock->mbox, &msg)) {
        ssize_t rcv_len = sock_udp_recv(sock->udp_sock, rcv_buffer,
                                        sizeof(rcv_buffer),
                                        DTLS_HANDSHAKE_TIMEOUT,
                                        &remote->remote_ep);
        if (rcv_len >= 0) {
            udp_count++;
            dtls_handle_message(sock->dtls_ctx, &remote->dtls_session, rcv_buffer,
                                rcv_len);
        }
        else {
            DEBUG("Error receiving handshake messages\n");
            return rcv_len;
        }
    }

    if (msg.type == DTLS_EVENT_CONNECTED) {
        DEBUG("DTLS handshake successful\n");
        return 0;
    }
    else {
        DEBUG("DTLS handshake was not successful\n");
        return -1;
    }
}

void sock_dtls_close_session(sock_dtls_t *sock, sock_dtls_session_t *remote)
{
    dtls_close(sock->dtls_ctx, &remote->dtls_session);
}

ssize_t sock_dtls_send(sock_dtls_t *sock, sock_dtls_session_t *remote,
                       const void *data, size_t len)
{
    assert(sock && remote && data);
    return dtls_write(sock->dtls_ctx, &remote->dtls_session, (uint8_t *)data, len);
}

ssize_t sock_dtls_recv(sock_dtls_t *sock, sock_dtls_session_t *remote,
                       void *data, size_t max_len, uint32_t timeout)
{
    ssize_t res;
    msg_t msg;
    xtimer_t timeout_timer;

    assert(sock && data && remote);

    if (_is_timed_out(timeout)) {
        timeout_timer.callback = _timeout_callback;
        timeout_timer.arg = sock;
        xtimer_set(&timeout_timer, timeout);
    }

    /* save location to result buffer */
    sock->buf = data;
    sock->buflen = max_len;

    uint32_t start_recv;
    while (1) {
        start_recv = xtimer_now_usec();
        res = sock_udp_recv(sock->udp_sock, data, max_len, timeout,
                            &remote->remote_ep);
        if (res < 0) {
            DEBUG("Error receiving UDP packet: %d\n", res);
            return res;
        }

        if (_is_timed_out(timeout)) {
            timeout = timeout - (xtimer_now_usec() - start_recv);
            DEBUG("timeout left: %u\n", timeout);
        }

        _ep_to_session(&remote->remote_ep, &remote->dtls_session);
        res = dtls_handle_message(sock->dtls_ctx, &remote->dtls_session,
                            (uint8_t *)data, res);

        if (mbox_try_get(&sock->mbox, &msg)) {
            switch(msg.type) {
                case DTLS_EVENT_READ:
                    xtimer_remove(&timeout_timer);
                    return msg.content.value;
                case DTLS_EVENT_TIMEOUT:
                    DEBUG("Error: timed out while decrypting message\n");
                    return -ETIMEDOUT;
                default:
                    break;
            }
        }
        else if (timeout == 0) {
            return -ETIMEDOUT;
        }
    }

}

void sock_dtls_destroy(sock_dtls_t *sock)
{
    sock_udp_close(sock->udp_sock);
    dtls_free_context(sock->dtls_ctx);
}

static void _ep_to_session(const sock_udp_ep_t *ep, session_t *session)
{
    session->port = ep->port;
    session->size = sizeof(ipv6_addr_t) + sizeof(unsigned short);
    /* FIXME: currently ignoring the interface session is established on */
    session->ifindex = SOCK_ADDR_ANY_NETIF;
    memcpy(&session->addr, &ep->addr.ipv6, sizeof(ipv6_addr_t));
}

static void _session_to_ep(const session_t *session, sock_udp_ep_t *ep)
{
    ep->port = session->port;
    ep->netif = session->ifindex;
    memcpy(&ep->addr.ipv6, &session->addr, sizeof(ipv6_addr_t));
}

static void _timeout_callback(void *arg)
{
    msg_t timeout_msg = { .type = DTLS_EVENT_TIMEOUT };
    sock_dtls_t *sock = arg;
    mbox_try_put(&sock->mbox, &timeout_msg);
}

static int _is_timed_out(uint32_t timeout)
{
    return (timeout != SOCK_NO_TIMEOUT) && (timeout != 0);
}

/** @} */
