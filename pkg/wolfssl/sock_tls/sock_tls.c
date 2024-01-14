/*
 * Copyright (C) 2019 Daniele Lacamera
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>
#include <stdlib.h>

#include <net/sock.h>
#include <wolfssl/ssl.h>
#include <wolfssl/internal.h>

#include <log.h>

void sock_dtls_close(sock_tls_t *sk)
{
    sock_udp_close(&sk->conn.udp);
}

void sock_dtls_set_endpoint(sock_tls_t *sk, const sock_udp_ep_t *addr)
{
    LOG(LOG_INFO, "wolfSSL: Setting peer address and port\n");
    XMEMCPY(&sk->peer_addr, addr, sizeof (sock_udp_ep_t));
}

int sock_dtls_create(sock_tls_t *sock, const sock_udp_ep_t *local, const sock_udp_ep_t *remote, uint16_t flags, WOLFSSL_METHOD *method)
{
    int ret;
    if (!sock)
        return -EINVAL;
    XMEMSET(sock, 0, sizeof(sock_tls_t));
    sock->ctx = wolfSSL_CTX_new(method);
    if (!sock->ctx)
        return -ENOMEM;

    ret = sock_udp_create(&sock->conn.udp, local, remote, flags);
    if (ret < 0) {
        XFREE(sock->ctx, NULL, 0);
        return ret;
    }
    if (remote) {
        XMEMCPY(&sock->peer_addr, remote, sizeof(sock_udp_ep_t));
    }
    return 0;
}

static int tls_session_create(sock_tls_t *sk)
{
    if (!sk || !sk->ctx)
        return -EINVAL;
    sk->ssl = wolfSSL_new(sk->ctx);
    if (sk->ssl == NULL) {
        LOG(LOG_ERROR, "Error allocating ssl session\n");
        return -ENOMEM;
    }
    wolfSSL_SetIOReadCtx(sk->ssl, sk);
    wolfSSL_SetIOWriteCtx(sk->ssl, sk);
    sk->ssl->gnrcCtx = sk;
    return 0;
}

static void tls_session_destroy(sock_tls_t *sk)
{
    if (!sk || !sk->ssl)
        return;
    wolfSSL_free(sk->ssl);
}

int sock_dtls_session_create(sock_tls_t *sk)
{
    return tls_session_create(sk);
}

void sock_dtls_session_destroy(sock_tls_t *sk)
{
    tls_session_destroy(sk);
}

#ifdef MODULE_SOCK_TCP

#include <sock_tls.h>

GetTcpRecvTimeoutCallback get_tcp_recv_timeout_callback = NULL;

void setGetTcpRecvTimeoutCallback(GetTcpRecvTimeoutCallback callback) {
    get_tcp_recv_timeout_callback = callback;
}

/* Custom I/O send function */
int GNRC_SendTo_tcp(WOLFSSL *ssl, char *buf, int sz, void *_ctx) {
    (void)_ctx;
   
    sock_tls_t* _sock_tls = ssl->gnrcCtx;

    if (!_sock_tls)
        return WOLFSSL_CBIO_ERR_GENERAL;

    int ret = sock_tcp_write(&_sock_tls->conn.tcp, buf, sz);
    if (ret > 0)
        return ret;

    switch (ret) {
        case -ENOTCONN:
            return WOLFSSL_CBIO_ERR_CONN_CLOSE;
        case 0:
            return WOLFSSL_CBIO_ERR_WANT_WRITE;
        case -EAGAIN:
            return 0;
        default:
            printf("[TLS]: TCP Error while sending data: %zd\n", ret);
            break;
    }
    return ret;
}

int GNRC_ReceiveFrom_tcp(WOLFSSL *ssl, char *buf, int sz, void *_ctx) {

    sock_tls_t* _sock_tls = ssl->gnrcCtx;

    if (!_sock_tls)
        return WOLFSSL_CBIO_ERR_GENERAL;

    int timeout = 0;
    if(get_tcp_recv_timeout_callback){
        timeout = get_tcp_recv_timeout_callback(_ctx); //Let the application decide the timeout
    }

    int recvLen = 0;
    ztimer_now_t endTimestamp = ztimer_now(ZTIMER_MSEC) + timeout;
    ssize_t bytesRead;
    do {
        bytesRead = sock_tcp_read(&_sock_tls->conn.tcp, buf + recvLen, sz - recvLen, 0);
        if (bytesRead > 0) {
            recvLen += bytesRead;
        }

    } while ((recvLen < sz) && (ztimer_now(ZTIMER_MSEC) < endTimestamp));

    if (recvLen > 0) {
        return recvLen;
    } else {
        switch (bytesRead) {
            case -ETIMEDOUT:
                return WOLFSSL_CBIO_ERR_WANT_READ;
            case -EAGAIN:
                return WOLFSSL_CBIO_ERR_WANT_READ;
            case 0:
                printf("Connection closed by the peer\n");
                return WOLFSSL_CBIO_ERR_CONN_CLOSE;
            default:
                printf("Error while receiving data: %zd\n", bytesRead);
                break;
        }
    }
    return bytesRead;
}

int sock_tls_session_create(sock_tls_t *sk, void *ctx)
{
    if (!sk || !sk->ctx)
        return -EINVAL;
    sk->ssl = wolfSSL_new(sk->ctx);
    if (sk->ssl == NULL) {
        printf("Error allocating ssl session\n");
        return -ENOMEM;
    }
    wolfSSL_SetIOReadCtx(sk->ssl, ctx);
    wolfSSL_SetIOWriteCtx(sk->ssl, ctx);
    sk->ssl->gnrcCtx = sk;
    return 0;
    
}

int sock_tls_create(sock_tls_t *sock, WOLFSSL_METHOD *method)
{
    if (!sock)
        return -EINVAL;
    XMEMSET(sock, 0, sizeof(sock_tls_t));
    sock->ctx = wolfSSL_CTX_new(method);
    if (!sock->ctx)
        return -ENOMEM;
    /* Set custom I/O callbacks and context */
    wolfSSL_CTX_SetIORecv(sock->ctx, GNRC_ReceiveFrom_tcp);
    wolfSSL_CTX_SetIOSend(sock->ctx, GNRC_SendTo_tcp);
    return 0;
}

void sock_tls_session_destroy(sock_tls_t *sk)
{
    tls_session_destroy(sk);
}
#endif

#include <ctype.h>
int strncasecmp(const char *s1, const char * s2, unsigned int sz)
{
    unsigned int i;
    for( i = 0; i < sz; i++) {
        int res;
        const unsigned char *us1 = (const unsigned char *)s1;
        const unsigned char *us2 = (const unsigned char *)s2;
        res = toupper(us1[i]) - toupper(us2[i]);
        if (res != 0)
            return res;
    }
    return 0;
}
