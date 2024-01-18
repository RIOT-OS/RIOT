/*
 * Copyright (C) 2024 tanvirBsmrstu
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     module_sock_tls_new_poc
 * @{
 *
 * @file
 * @brief       sock_tls_new_poc implementation
 *
 * @author      tanvirBsmrstu <tanvir.bsmrstu@gmail.com>
 *
 * @}
 */

#include "sock_tls_new_poc.h"
#include "sock_tls.h"

#include "net/ipv6/addr.h"
#include "ztimer.h"
#include <stdio.h>

/* Implementation of the module */

static sock_tls_t internal_tls_socket;

struct TLSContext {
    sock_tls_t *tls_socket;
    int timeout_ms_init;    // initial timeout
    int timeout_ms;         // current timeout
    const char *ca_cert_path;
    const char *device_cert_chain_path;
    const char *device_cert_path;
    const char *private_key_path;
};

static TLSContext tlsContext;

static int initialized = 0, isTLSConneted = 0;

// #define MAIN_QUEUE_SIZE (8)
// static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int tcp_init(void);
int tcp_connect(char *remoteAddress, int port, sock_tcp_t *socket);

int load_cert_from_path(TLSContext *context);
unsigned char *read_file_to_buffer(const char *filename, size_t *buffer_size);

int tls_init(TLSContext *context)
{
    if (initialized) {
        printf("[TLS]: module already initialized\n");
        return 0;
    }

    int res = 0;

    if ((res = tcp_init()) < 0) {
        printf("[TLS]: tcp init failed\n");
        return res;
    }
    // Perform initialization steps if needed
    wolfSSL_Init();


    if (sock_tls_create(context->tls_socket, wolfTLSv1_2_client_method()) < 0) {
        printf("[TLS]: failed to create WOLFSSL_CTX\n");
        return -1;
    }
    wolfSSL_CTX_set_verify(context->tls_socket->ctx, SSL_VERIFY_NONE, 0);

    if (load_cert_from_path(context) < 0) {
        printf("[TLS]: failed to load certificate paths\n");
        return -1;
    }
    printf("[TLS]: module initialized successfully\n");

    initialized = 1;
    return 0;
}

int load_cert_from_path(TLSContext *context)
{
    unsigned char *ca_cert_buf;
    size_t ca_cert_buf_len;

    if (context->ca_cert_path != NULL) {
        ca_cert_buf = read_file_to_buffer(context->ca_cert_path, &ca_cert_buf_len);
        if (ca_cert_buf == NULL) {
            printf("[TLS]: failed to load ca cert from path\n");
            return -1;
        }
        /* Load CA certificates into WOLFSSL_CTX */
        if (wolfSSL_CTX_load_verify_buffer(context->tls_socket->ctx, ca_cert_buf,
                                           ca_cert_buf_len,
                                           WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
            printf("[TLS]: failed to load CA into buffer\n");
            return -1;
        }
    }
    if (context->device_cert_chain_path != NULL) {
        ca_cert_buf = read_file_to_buffer(context->device_cert_chain_path, &ca_cert_buf_len);
        if (ca_cert_buf == NULL) {
            printf("[TLS]: failed to load device cert chain from path\n");
            return -1;
        }
        /* Load client certificate chain into WOLFSSL_CTX */
        if (wolfSSL_CTX_use_certificate_chain_buffer_format(context->tls_socket->ctx, ca_cert_buf,
                                                            ca_cert_buf_len,
                                                            WOLFSSL_FILETYPE_PEM) !=
            WOLFSSL_SUCCESS) {
            printf("[TLS]: failed to load device cert chain into buffer\n");
            return -1;
        }
    }
    if (context->private_key_path != NULL) {
        ca_cert_buf = read_file_to_buffer(context->private_key_path, &ca_cert_buf_len);
        if (ca_cert_buf == NULL) {
            printf("[TLS]: failed to load private key from path\n");
            return -1;
        }
        /* Load private Key into WOLFSSL_CTX */
        if (wolfSSL_CTX_use_PrivateKey_buffer(context->tls_socket->ctx, ca_cert_buf,
                                              ca_cert_buf_len,
                                              WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
            printf("[TLS]: failed to load private key into buffer\n");
            return -1;
        }
    }
    return 0;
}

TLSContext *tls_create_context()
{
    // Initialize the context members
    tlsContext.device_cert_chain_path = NULL;
    tlsContext.device_cert_path = NULL;
    tlsContext.private_key_path = NULL;
    tlsContext.ca_cert_path = NULL;
    tlsContext.timeout_ms_init = gnrc_wolfssl_tls_max_timeout_ms; // ssl handshake needs initial timeout
    tlsContext.timeout_ms = 0;
    tlsContext.tls_socket = &internal_tls_socket;
    return &tlsContext;
}

int tls_set_certificate(TLSContext *ctx, const char *ca_cert_path, const char *cert_chain_path,
                        const char *private_key_path)
{
    if (ctx == NULL) {
        printf("[TLS]: Invalid TLS context\n");
        return -1;
    }

    ctx->ca_cert_path = ca_cert_path;
    ctx->device_cert_chain_path = cert_chain_path;
    ctx->private_key_path = private_key_path;
    return 0;
}

int retrieve_timeout_ms_per_tcp_read(void *_ctx)
{
    if (_ctx == NULL) {
        return -1;
    }
    TLSContext *ctx = (TLSContext *)_ctx;

    if (ctx == NULL) {
        return -1;
    }
    if (ctx->timeout_ms == 0) {
        return ctx->timeout_ms_init;
    }
    else {
        return ctx->timeout_ms;
    }
}
int tls_create_session(TLSContext *context)
{
    setGetTcpRecvTimeoutCallback(retrieve_timeout_ms_per_tcp_read);
    return sock_tls_session_create(context->tls_socket, context);
}
int tls_establish_connection(char *remoteAddress, int port, TLSContext *context)
{
    if (context == NULL) {
        printf("[TLS]: Invalid TLS context\n");
        return -1;
    }
    int res;

    if ((res = tcp_connect(remoteAddress, port, &context->tls_socket->conn.tcp)) < 0) {
        printf("[TLS]: tcp_connect failed\n");
        return res;
    }
    // Perform TLS connection setup using WolfSSL

    if ((res = tls_create_session(context)) < 0) {
        printf("[TLS]: ssl session creation failed\n");
        return res;
    }

    res = wolfSSL_connect(context->tls_socket->ssl);
    if (res != SSL_SUCCESS) {
        // Handle other SSL errors
        char text[100];
        int errCode = wolfSSL_get_error(context->tls_socket->ssl, res);
        wolfSSL_ERR_error_string(errCode, text);
        printf("[TLS]: SSL handshake failed: %d with %s\n", errCode, text);
        tls_close(context);
        return res;
    }
    isTLSConneted = 1;
    return 0;
}

int tls_send(TLSContext *context, unsigned char *buffer, int buffer_len, int timeout_ms)
{
    if (!isTLSConneted) {
        printf("[TLS]: TLS is not connected\n");
        return -1;
    }
    if (context == NULL) {
        printf("[TLS]: Invalid TLS context\n");
        return -1;
    }
    // Perform TLS data receiving using WolfSSL
    if (timeout_ms < 0) {
        printf("[TLS]: Negative timeout value\n");
        return -1;
    }
    else {
        timeout_ms = timeout_ms <
                     gnrc_wolfssl_tls_max_timeout_ms ? timeout_ms : gnrc_wolfssl_tls_max_timeout_ms;
    }

    ztimer_now_t end_time = ztimer_now(ZTIMER_MSEC) + timeout_ms;
    int sentLen = 0, chunk_size, res;

    while ((sentLen < buffer_len) && (ztimer_now(ZTIMER_MSEC) < end_time)) {
        chunk_size = buffer_len - sentLen;
        res =
            wolfSSL_write(context->tls_socket->ssl, (unsigned char *)buffer + sentLen, chunk_size);
        if (res > 0) {
            sentLen += res;
        }
        else if (res <= 0) {
            sentLen = res;
            int err = -wolfSSL_get_error(context->tls_socket->ssl, res);
            switch (err) {
            case WOLFSSL_CBIO_ERR_GENERAL:
            case WOLFSSL_CBIO_ERR_CONN_CLOSE:
                tls_close(context);
                return err;
            default:
                break;
            }
        }
    }
    return sentLen;
}

int tls_receive(TLSContext *context, unsigned char *buffer, int buffer_len, int timeout_ms)
{
    if (!isTLSConneted) {
        printf("[TLS]: TLS is not connected\n");
        return -1;
    }
    if (context == NULL) {
        printf("[TLS]: Invalid TLS context\n");
        return -1;
    }
    if (buffer_len <= 0) {
        return 0;
    }
    // Perform TLS data receiving using WolfSSL
    if (timeout_ms < 0) {
        printf("[TLS]: Negative timeout value\n");
        return -1;
    }
    else if (timeout_ms == 0) {
        context->timeout_ms = context->timeout_ms_init = 0;
    }
    else {
        context->timeout_ms = timeout_ms <
                              gnrc_wolfssl_tls_max_timeout_ms ? timeout_ms :
                              gnrc_wolfssl_tls_max_timeout_ms;
    }
    int res = wolfSSL_read(context->tls_socket->ssl, buffer, buffer_len);

    if (res > 0) {
        return res;
    }
    // Handle other SSL errors
    int err = -wolfSSL_get_error(context->tls_socket->ssl, res); // SOCKET_PEER_CLOSED_E

    switch (err) {
    case WOLFSSL_CBIO_ERR_GENERAL:
    case WOLFSSL_CBIO_ERR_CONN_CLOSE:
        printf("[TLS]: WOLFSSL_CBIO_ERR_CONN_CLOSE from TCP_READ\n");
        tls_close(context);
        break;
    case WOLFSSL_CBIO_ERR_WANT_READ:
        return 0;
    default:
        break;
    }
    return err;
}

void tls_close(TLSContext *context)
{
    if (!isTLSConneted) {
        printf("[TLS]: TLS is not connected so no need for tls_close\n");
        return;
    }
    if (context == NULL) {
        printf("[TLS]: Invalid TLS context\n");
        return;
    }
    isTLSConneted = 0;
    // Perform TLS connection closing using WolfSSL
    sock_tcp_disconnect(&context->tls_socket->conn.tcp);
    printf("[TLS]: TLS module cleaned up sock\n");

    // Free allocated resources
    sock_tls_session_destroy(context->tls_socket);

    if (context->tls_socket->ctx) {
        wolfSSL_CTX_free(context->tls_socket->ctx);
    }
    wolfSSL_Cleanup();
    printf("[TLS]: TLS module cleaned up\n");
    initialized = 0;
}

int tcp_connect(char *remoteAddress, int port, sock_tcp_t *socket)
{
    int res;
    sock_tcp_ep_t remote = SOCK_IPV6_EP_ANY;

    remote.port = port;

    // TODO : remoteIP has to retrieved from remoteAddress
    // DNS64 is not currently configured
    int dps = 1;
    //char *remoteIP = dps ? "64:ff9b::3374:91ca" : "64:ff9b::2871:b0b5";
    char *remoteIP = dps ? "2001:67c:2b0:db32:0:1:2871:b0aa" : "2001:67c:2b0:db32:0:1:2871:b0b5";

    // uint8_t addr[16] = {0};
    ////////////////////////////////
    // if ((res=sock_dns_query(remoteAddress, (ipv6_addr_t *)&remote.addr, AF_INET6)) < 0) {
    //     printf("Error resolving hostname %d\n",res);
    //     return -1;
    // }
    // //////////////////////////////

    ipv6_addr_from_str((ipv6_addr_t *)&remote.addr, remoteIP);

    if ((res = sock_tcp_connect(socket, &remote, 0, 0)) < 0) {
        printf("[TLS]: Error connecting sock %s\n", strerror(res));
    }
    return res;
}

int tcp_init(void)
{
    // if any initialization needed in future
    return 0;
}

/**
 * @brief Read the contents of a file into a buffer.
 * @param[in] filename The name of the file to read.
 * @param[out] buffer_size The size of the buffer.
 * @return A pointer to the buffer on success, NULL on failure.
 */
unsigned char *read_file_to_buffer(const char *filename, size_t *buffer_size)
{
    FILE *file = fopen(filename, "rb"); // Open the file in binary read mode

    if (!file) {
        printf("[TLS]: Failed to open file");
        return NULL;
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);

    fseek(file, 0, SEEK_SET);

    // Allocate memory for the buffer
    unsigned char *buffer = (unsigned char *)malloc(file_size);

    if (!buffer) {
        printf("[TLS]: Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, file);

    fclose(file);

    if (bytes_read != (size_t)file_size) {
        printf("[TLS]: Failed to read file");
        free(buffer);
        return NULL;
    }

    if (buffer_size) {
        *buffer_size = (size_t)file_size;
    }

    return buffer;
}
