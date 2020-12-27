/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   WolfSSL implementation of @ref net_sock_dtls
 *
 * @author  Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */


#include "net/sock/dtls.h"
#include "net/credman.h"

/* for WOLFSSL struct */
#include <wolfssl/internal.h>

#if SOCK_HAS_ASYNC
#include "net/sock/async.h"
#include "net/sock/async/event.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

static int _recv(WOLFSSL *ssl, char *buf, int sz, void *ctx);
static int _send(WOLFSSL *ssl, char *buf, int sz, void *ctx);
static void _wolfssl_error(WOLFSSL *ssl, int ret);

static char errbuf[80];  /**< Buffer for wolfssl error string */

/**
 * Callback to retrieve client PSK keying material
 *
 * @param[in] ssl           SSL session object
 * @param[in] hint          client hint
 * @param[out] identity     client identity
 * @param[in] id_max_len    maximum size of @p identity
 * @param[out] key          client key
 * @param[in] key_max_len   maximum size of @p key
 *
 * @return  length of key in bytes or 0 for error
 */
static unsigned psk_client_cb(WOLFSSL *ssl, const char *hint, char *identity,
                              unsigned int id_max_len, unsigned char *key,
                              unsigned int key_max_len)
{
    (void)hint;

    /* FIXME: use struct gnrc_wolfssl_ctx? */
    sock_dtls_session_t *session = ssl->IOCB_ReadCtx;
    sock_dtls_t *sock = session->dtls_sock;

    credman_credential_t c;
    credman_get(&c, sock->tag, CREDMAN_TYPE_PSK);
    if (id_max_len < c.params.psk.id.len) {
        return 0;
    }
    strncpy(identity, c.params.psk.id.s, c.params.psk.id.len);

    if (key_max_len < c.params.psk.key.len) {
        return 0;
    }
    memcpy(key, c.params.psk.key.s, c.params.psk.key.len);

    return c.params.psk.key.len;   /* length of key in octets or 0 for error */
}

/**
 * Callback to retrieve server PSK keying materials
 *
 * @param[in] ssl           SSL session object
 * @param[in] identity      identity of client connecting
 * @param[out] key          buffer to hold key
 * @param[in] key_max_len   maximum size of @p key
 *
 * @return  size of key set or 0 if error
 */
static unsigned psk_server_cb(WOLFSSL *ssl, const char *identity,
                              unsigned char *key, unsigned int key_max_len)
{
    (void)identity;

    /* FIXME: use struct gnrc_wolfssl_ctx? */
    sock_dtls_session_t *session = ssl->IOCB_ReadCtx;
    sock_dtls_t *sock = session->dtls_sock;

    credman_credential_t c;
    credman_get(&c, sock->tag, CREDMAN_TYPE_PSK);
    if (XSTRNCMP(identity, c.params.psk.id.s, c.params.psk.id.len) != 0) {
        return 0;
    }

    if (key_max_len < c.params.psk.key.len) {
        return 0;
    }
    memcpy(key, c.params.psk.key.s, c.params.psk.key.len);

    return c.params.psk.key.len;
}

void sock_dtls_init(void)
{
    DEBUG("calling sock_dtls_init()\n");
    wolfSSL_Init();
    wolfSSL_Debugging_ON();
}

static WOLFSSL_METHOD *_get_wolfssl_method(unsigned version, unsigned role)
{
    switch (role) {
    case SOCK_DTLS_CLIENT: {
        switch (version) {
        case SOCK_DTLS_1_0:
            return wolfDTLSv1_client_method();
        case SOCK_DTLS_1_2:
            return wolfDTLSv1_2_client_method();
        case SOCK_DTLS_1_3:
        /* not supported */
        default:
            return NULL;
        }
    }
    case SOCK_DTLS_SERVER: {
        switch (version) {
        case SOCK_DTLS_1_0:
            return wolfDTLSv1_server_method();
        case SOCK_DTLS_1_2:
            return wolfDTLSv1_2_server_method();
        case SOCK_DTLS_1_3:
        /* not supported */
        default:
            return NULL;
        }
    }
    default:
        return NULL;
    }
}

int sock_dtls_create(sock_dtls_t *sock, sock_udp_t *udp_sock,
                     credman_tag_t tag, unsigned version, unsigned role)
{
    XMEMSET(sock, 0, sizeof(sock_dtls_t));

    sock->ctx = wolfSSL_CTX_new(_get_wolfssl_method(version, role));
    if (sock->ctx == NULL) {
        DEBUG("sock_dtls: failed to create new ctx\n");
        return -ENOMEM;
    }

    sock->tag = tag;
    sock->udp_sock = udp_sock;
    wolfSSL_CTX_SetIORecv(sock->ctx, _recv);
    wolfSSL_CTX_SetIOSend(sock->ctx, _send);

    switch (role) {
    case SOCK_DTLS_CLIENT:
        DEBUG("Setting psk client callback\n");
        wolfSSL_CTX_set_psk_client_callback(sock->ctx, psk_client_cb);
        break;
    case SOCK_DTLS_SERVER:
        DEBUG("Setting psk server callback\n");
        wolfSSL_CTX_set_psk_server_callback(sock->ctx, psk_server_cb);
        wolfSSL_CTX_use_psk_identity_hint(sock->ctx, "hint");
        break;
    default:
        DEBUG("sock_dtls: unknown method\n");
        return -1;
    }

    /* print list of ciphers used */
    char ciphers[256];
    if (wolfSSL_get_ciphers(ciphers, sizeof(ciphers)) != SSL_SUCCESS) {
        DEBUG("sock_dtls: failed to get cipher list\n");
    }
    else {
        DEBUG("sock_dtls: built-in ciphers: %s\n", ciphers);
    }

    DEBUG("sock_dtls: finished creating new DTLS sock\n");

    return 0;
}

sock_udp_t *sock_dtls_get_udp_sock(sock_dtls_t *sock)
{
    return sock->udp_sock;
}

static int _allocate_session(sock_dtls_t *sock, sock_dtls_session_t *session)
{
    session->ssl = wolfSSL_new(sock->ctx);
    if (!session->ssl) {
        DEBUG("sock_dtls: create new ssl session failed\n");
        return -1;
    }

    wolfSSL_SetIOReadCtx(session->ssl, session);
    wolfSSL_SetIOWriteCtx(session->ssl, session);

    return 0;
}

int sock_dtls_session_init(sock_dtls_t *sock, const sock_udp_ep_t *ep,
                           sock_dtls_session_t *session)
{
    /* check ClientHello already sent */
    if (session->ssl && session->ssl->options.clientState > NULL_STATE) {
        return 0;
    }

    session->ssl = NULL;
    int ret = _allocate_session(sock, session);
    if (ret < 0) {
        return -1;
    }

    session->dtls_sock = sock;
    memcpy(&session->ep, ep, sizeof(sock_udp_ep_t));

    session->timeout = 0;
    wolfSSL_set_using_nonblock(session->ssl, 1);
    ret = wolfSSL_connect(session->ssl);
    if (ret != SSL_SUCCESS) {
        int err = wolfSSL_get_error(session->ssl, ret);
        if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
            DEBUG("error = %d, %s\n", err,
                  wolfSSL_ERR_error_string(err, errbuf));
            return -1;
        }
    }
    wolfSSL_set_using_nonblock(session->ssl, 0);

    return 1;
}


void sock_dtls_session_destroy(sock_dtls_t *sock, sock_dtls_session_t *session)
{
    (void)sock;

    int ret = wolfSSL_shutdown(session->ssl);
    if (ret != SSL_SUCCESS) {
        if (ret == WOLFSSL_SHUTDOWN_NOT_DONE) {
            /* do a bidirectional shutdown */
            wolfSSL_shutdown(session->ssl);
        }
        else {
            DEBUG("sock_dtls: closing session failed\n");
            return;
        }
    }

    wolfSSL_free(session->ssl);
    memset(session, 0, sizeof(sock_dtls_session_t));
}

void sock_dtls_close(sock_dtls_t *sock)
{
    wolfSSL_CTX_free(sock->ctx);
}

static int _recv(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    (void)ssl;

    sock_dtls_session_t *session = ctx;
    if (!session) {
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    uint32_t timeout = session->timeout;
    /* timeout in wolfssl is a bit different
     * this is a workaround to make it behave
     * similarly to RIOT */
    if (timeout > (uint32_t)ssl->dtls_timeout_max) {
        wolfSSL_dtls_set_timeout_max(ssl, timeout);
        wolfSSL_dtls_set_timeout_init(ssl, timeout);
    }
    else {
        wolfSSL_dtls_set_timeout_init(ssl, timeout);
        wolfSSL_dtls_set_timeout_max(ssl, timeout);
    }

    /* Crude way to somewhat test that `sock_dtls_aux_rx_t` and
     * `sock_udp_aux_rx_t` remain compatible: */
    static_assert(sizeof(sock_dtls_aux_rx_t) == sizeof(sock_udp_aux_rx_t),
                  "sock_dtls_aux_rx_t became incompatible with "
                  "sock_udp_aux_rx_t");
    int ret = sock_udp_recv_aux(session->dtls_sock->udp_sock, buf, sz, timeout,
                                &session->ep,
                                (sock_udp_aux_rx_t *)session->rx_aux);
    if (ret == -ETIMEDOUT) {
        DEBUG("sock_dtls: recv timed out: %d\n", ret);
        return WOLFSSL_CBIO_ERR_TIMEOUT;
    }
    else if (ret == -EAGAIN) {
        DEBUG("sock_dtls: recv non-blocking no data: %d\n", ret);
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }
    else if (ret <= 0) {
        DEBUG("sock_dtls: recv failed %d\n", ret);
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    DEBUG("_recv: new packet size: %u\n", ret);
    return ret;
}

ssize_t sock_dtls_recv_aux(sock_dtls_t *sock, sock_dtls_session_t *session,
                           void *data, size_t maxlen, uint32_t timeout,
                           sock_dtls_aux_rx_t *aux)
{
    (void)aux;
    int ret;

    if (!session) {
        return -EINVAL;
    }

    /* check if session is already allocated*/
    if (!session->ssl) {
        DEBUG("No existing session, creating a new one...\n");
        if (_allocate_session(sock, session) < 0) {
            return -1;
        }
    }

    wolfSSL_set_using_nonblock(session->ssl, !timeout);
    session->timeout = timeout;
    session->dtls_sock = sock;
    session->rx_aux = aux;

    if (!session->ssl->options.handShakeDone) {
        DEBUG("Negotiating new connection...\n");
        ret = wolfSSL_negotiate(session->ssl);
        if (ret == SSL_SUCCESS) {
            DEBUG("Handshake done\n");
            return -SOCK_DTLS_HANDSHAKE;
        }
        else {
            _wolfssl_error(session->ssl, ret);

            // FIXME: is it okay to do this?
            session->ssl->error = 0;
            return -ETIMEDOUT;
        }
    }

    ret = wolfSSL_read(session->ssl, data, maxlen);
    if (ret < 0) {
        _wolfssl_error(session->ssl, ret);
        // FIXME: is it okay to do this?
        session->ssl->error = 0;
        return ret;
    }
    return ret;
}

static int _send(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    (void)ssl;

    sock_dtls_session_t *session = ctx;
    if (!session) {
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    /* Crude way to somewhat test that `sock_dtls_aux_rx_t` and
     * `sock_udp_aux_rx_t` remain compatible: */
    static_assert(sizeof(sock_dtls_aux_tx_t) == sizeof(sock_udp_aux_tx_t),
                  "sock_dtls_aux_tx_t became incompatible with "
                  "sock_udp_aux_tx_t");
    int ret = sock_udp_send_aux(session->dtls_sock->udp_sock, buf, sz,
                                &session->ep,
                                (sock_udp_aux_tx_t *)session->tx_aux);
    if (ret <= 0) {
        DEBUG("sock_dtls: send packet failed %d\n", ret);
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    DEBUG("_send: sent packet size: %u\n", ret);
    return ret;
}

ssize_t sock_dtls_send_aux(sock_dtls_t *sock, sock_dtls_session_t *session,
                           const void *data, size_t len, uint32_t timeout,
                           sock_dtls_aux_tx_t *aux)
{
    if (len == 0) {
        return -EINVAL;
    }
    session->dtls_sock = sock;
    session->tx_aux = aux;
    session->timeout = timeout;
    return wolfSSL_write(session->ssl, data, len);
}

void _wolfssl_error(WOLFSSL *ssl, int ret)
{
    int err = wolfSSL_get_error(ssl, ret);

    wolfSSL_ERR_error_string(err, errbuf);
    DEBUG("wolfSSL error: %d: %s\n", err, errbuf);
}
