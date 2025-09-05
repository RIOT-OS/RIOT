/**
 * @file tls.c
 * @brief Implementation of TLS over TCP socket using wolfSSL for RIOT OS
 * @author Bilal-Alali
 * @date 2025-05-21 12:37:35
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "net/sock/tcp.h"
#include "net/sock/tls.h"
#include <wolfssl/ssl.h>
#include <wolfssl/error-ssl.h>
#include "ztimer.h"
#include "debug.h"

#define TLS_DEFAULT_TIMEOUT (5000)  /* 5 seconds timeout */
#define CHUNK_SIZE 256              /* Maximum chunk size for read/write operations */
#define MAX_RETRIES 3                 /* Maximum number of retry attempts */

static int _wolfssl_tcp_receive(WOLFSSL* ssl, char* buf, int sz, void* ctx) {
    (void)ssl;
    sock_tls_tcp_t *sock = (sock_tls_tcp_t *)ctx;
    int total_received = 0;
    int remaining = sz;
    int tries = 0;

    while (remaining > 0 && tries < MAX_RETRIES) {
        int chunk_size = (remaining > CHUNK_SIZE) ? CHUNK_SIZE : remaining;
        int ret = sock_tcp_read(sock->tcp_sock, buf + total_received, chunk_size, TLS_DEFAULT_TIMEOUT);

        if (ret > 0) {
            total_received += ret;
            remaining -= ret;
            tries = 0;
            continue;
        }

        if (ret == -ETIMEDOUT) {
            printf("TCP Read timeout after %d ms\n", (int)TLS_DEFAULT_TIMEOUT);
            tries++;
            if (total_received > 0) return total_received;
            if (tries >= MAX_RETRIES) return WOLFSSL_CBIO_ERR_TIMEOUT;
            continue;
        }

        if (ret < 0) {
            printf("TCP Read error: %d\n", ret);
            if (total_received > 0) return total_received;
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }
    return total_received > 0 ? total_received : WOLFSSL_CBIO_ERR_GENERAL;
}

static int _wolfssl_tcp_send(WOLFSSL* ssl, char* buf, int sz, void* ctx) {
    (void)ssl;
    sock_tls_tcp_t *sock = (sock_tls_tcp_t *)ctx;
    int total_sent = 0;
    int remaining = sz;
    int tries = 0;

    while (remaining > 0 && tries < MAX_RETRIES) {
        int chunk_size = (remaining > CHUNK_SIZE) ? CHUNK_SIZE : remaining;
        int ret = sock_tcp_write(sock->tcp_sock, buf + total_sent, chunk_size);

        if (ret > 0) {
            total_sent += ret;
            remaining -= ret;
            tries = 0;
            continue;
        }

        if (ret == -EAGAIN || ret == -EWOULDBLOCK) {
            tries++;
            if (tries >= MAX_RETRIES) {
                printf("TCP Send failed after %d tries\n", MAX_RETRIES);
                if (total_sent > 0) return total_sent;
                return WOLFSSL_CBIO_ERR_GENERAL;
            }
            ztimer_sleep(ZTIMER_MSEC, 100 * tries);
            continue;
        }

        if (ret < 0) {
            if (total_sent > 0) return total_sent;
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }
    return total_sent > 0 ? total_sent : WOLFSSL_CBIO_ERR_GENERAL;
}

int sock_tls_tcp_create(sock_tls_tcp_t *sock, WOLFSSL_METHOD *method, const unsigned char *ca_cert, unsigned int ca_cert_len) {
    if (!sock || !method) return -EINVAL;

    memset(sock, 0, sizeof(sock_tls_tcp_t));
    sock->ctx = wolfSSL_CTX_new(method);
    if (!sock->ctx) return -ENOMEM;

    if (wolfSSL_CTX_set_cipher_list(sock->ctx, "AES128-SHA256") != SSL_SUCCESS) {
        wolfSSL_CTX_free(sock->ctx);
        return -EINVAL;
    }

    if (wolfSSL_CTX_load_verify_buffer(sock->ctx, ca_cert, ca_cert_len, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        wolfSSL_CTX_free(sock->ctx);
        return -EINVAL;
    }

    wolfSSL_SetIORecv(sock->ctx, _wolfssl_tcp_receive);
    wolfSSL_SetIOSend(sock->ctx, _wolfssl_tcp_send);

    sock->ssl = wolfSSL_new(sock->ctx);
    if (!sock->ssl) {
        wolfSSL_CTX_free(sock->ctx);
        sock->ctx = NULL;
        return -ENOMEM;
    }

    wolfSSL_set_verify(sock->ssl, SSL_VERIFY_PEER, NULL);
    return 0;
}

int sock_tls_tcp_connect(sock_tls_tcp_t *sock, const sock_tcp_ep_t *remote, uint16_t local_port, uint16_t flags) {
    if (!sock || !remote) return -EINVAL;

    sock->tcp_sock = malloc(sizeof(sock_tcp_t));
    if (!sock->tcp_sock) return -ENOMEM;

    int ret = sock_tcp_connect(sock->tcp_sock, remote, local_port, flags);
    if (ret < 0) {
        free(sock->tcp_sock);
        return ret;
    }

    wolfSSL_SetIOReadCtx(sock->ssl, sock);
    wolfSSL_SetIOWriteCtx(sock->ssl, sock);

    ret = wolfSSL_connect(sock->ssl);
    if (ret != SSL_SUCCESS) {
        int err = wolfSSL_get_error(sock->ssl, ret);
        char error_buffer[80];
        printf("SSL connect error: %d (0x%x) - %s\n", err, err, wolfSSL_ERR_error_string(err, error_buffer));
        sock_tcp_disconnect(sock->tcp_sock);
        free(sock->tcp_sock);
        return -ECONNRESET;
    }
    return 0;
}

int sock_tls_tcp_accept(sock_tls_tcp_queue_t *queue, sock_tls_tcp_t **sock, uint32_t timeout) {
    if (!queue || !sock) return -EINVAL;

    sock_tcp_t *tcp_sock = NULL;
    int ret = sock_tcp_accept(&queue->tcp_queue, &tcp_sock, timeout);
    if (ret < 0) return ret;

    *sock = malloc(sizeof(sock_tls_tcp_t));
    if (!*sock) {
        sock_tcp_disconnect(tcp_sock);
        free(tcp_sock);
        return -ENOMEM;
    }

    memset(*sock, 0, sizeof(sock_tls_tcp_t));
    (*sock)->tcp_sock = tcp_sock;
    (*sock)->ctx = queue->ctx;
    (*sock)->ssl = wolfSSL_new(queue->ctx);

    if (!(*sock)->ssl) {
        free(*sock);
        sock_tcp_disconnect(tcp_sock);
        free(tcp_sock);
        return -ENOMEM;
    }

    wolfSSL_SetIOReadCtx((*sock)->ssl, *sock);
    wolfSSL_SetIOWriteCtx((*sock)->ssl, *sock);

    ret = wolfSSL_accept((*sock)->ssl);
    if (ret != SSL_SUCCESS) {
        int err = wolfSSL_get_error((*sock)->ssl, ret);
        char error_buffer[80];
        printf("SSL accept error: %d (%s)\n", err, wolfSSL_ERR_error_string(err, error_buffer));
        wolfSSL_free((*sock)->ssl);
        free(*sock);
        sock_tcp_disconnect(tcp_sock);
        free(tcp_sock);
        return -EPROTO;
    }
    return 0;
}

int sock_tls_tcp_listen(sock_tls_tcp_queue_t *queue, const sock_tcp_ep_t *local,
                       sock_tcp_t *queue_array, unsigned queue_len, uint16_t flags,
                       WOLFSSL_METHOD *method, const unsigned char *cert_buf,
                       unsigned int cert_len, const unsigned char *key_buf,
                       unsigned int key_len) {
    if (!queue || !local || !queue_array || queue_len == 0 || !method ||
        !cert_buf || !cert_len || !key_buf || !key_len) {
        return -EINVAL;
    }

    queue->ctx = wolfSSL_CTX_new(method);
    if (!queue->ctx) return -ENOMEM;

    if (wolfSSL_CTX_set_cipher_list(queue->ctx, "AES128-SHA256") != SSL_SUCCESS) {
        wolfSSL_CTX_free(queue->ctx);
        return -EINVAL;
    }

    wolfSSL_CTX_set_verify(queue->ctx, SSL_VERIFY_NONE, NULL);

    if (wolfSSL_CTX_use_certificate_buffer(queue->ctx, cert_buf, cert_len, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        wolfSSL_CTX_free(queue->ctx);
        return -EINVAL;
    }

    if (wolfSSL_CTX_use_PrivateKey_buffer(queue->ctx, key_buf, key_len, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        wolfSSL_CTX_free(queue->ctx);
        return -EINVAL;
    }

    wolfSSL_SetIORecv(queue->ctx, _wolfssl_tcp_receive);
    wolfSSL_SetIOSend(queue->ctx, _wolfssl_tcp_send);

    return sock_tcp_listen(&queue->tcp_queue, local, queue_array, queue_len, flags);
}

ssize_t sock_tls_tcp_read(sock_tls_tcp_t *sock, void *data, size_t max_len) {
    if (!sock || !data || max_len == 0) return -EINVAL;

    int ret = wolfSSL_read(sock->ssl, data, max_len);
    if (ret <= 0) {
        int err = wolfSSL_get_error(sock->ssl, ret);
        if (err == SSL_ERROR_ZERO_RETURN) return 0;
        return -ECONNRESET;
    }
    return ret;
}

ssize_t sock_tls_tcp_write(sock_tls_tcp_t *sock, const void *data, size_t len) {
    if (!sock || !data || len == 0) return -EINVAL;

    int ret = wolfSSL_write(sock->ssl, data, len);
    if (ret <= 0) return -ECONNRESET;
    return ret;
}

void sock_tls_tcp_disconnect(sock_tls_tcp_t *sock) {
    if (!sock) return;

    if (sock->ssl) {
        wolfSSL_shutdown(sock->ssl);
        wolfSSL_free(sock->ssl);
        sock->ssl = NULL;
    }

    if (sock->tcp_sock) {
        sock_tcp_disconnect(sock->tcp_sock);
        free(sock->tcp_sock);
        sock->tcp_sock = NULL;
    }
}
