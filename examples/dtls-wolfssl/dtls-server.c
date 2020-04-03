/*
 * Copyright (C) 2019 Daniele Lacamera
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Demonstrating DTLS 1.2 server using wolfSSL
 *
 * @author      Daniele Lacamera <daniele@wolfssl.com>
 * @}
 */

#include <wolfssl/ssl.h>
#include <sock_tls.h>

#include <inttypes.h>

#include <net/sock/udp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#define SERVER_PORT 11111
#define DEBUG 1
extern const unsigned char server_cert[];
extern const unsigned char server_key[];
extern unsigned int server_cert_len;
extern unsigned int server_key_len;

static sock_tls_t skv;
static sock_tls_t *sk = &skv;

static const char Test_dtls_string[] = "DTLS OK!";

#ifdef MODULE_WOLFSSL_PSK
/* identity is OpenSSL testing default for openssl s_client, keep same */
static const char* kIdentityStr = "Client_identity";

static inline unsigned int my_psk_server_cb(WOLFSSL* ssl, const char* identity,
        unsigned char* key, unsigned int key_max_len)
{
    (void)ssl;
    (void)key_max_len;

    /* see internal.h MAX_PSK_ID_LEN for PSK identity limit */
    if (strncmp(identity, kIdentityStr, strlen(kIdentityStr)) != 0)
        return 0;

    if (wolfSSL_GetVersion(ssl) < WOLFSSL_TLSV1_3) {
        /* test key in hex is 0x1a2b3c4d , in decimal 439,041,101 , we're using
           unsigned binary */
        key[0] = 0x1a;
        key[1] = 0x2b;
        key[2] = 0x3c;
        key[3] = 0x4d;

        return 4;   /* length of key in octets or 0 for error */
    }
    else {
        int i;
        int b = 0x01;

        for (i = 0; i < 32; i++, b += 0x22) {
            if (b >= 0x100)
                b = 0x01;
            key[i] = b;
        }

        return 32;   /* length of key in octets or 0 for error */
    }
}
#endif /* MODULE_WOLFSSL_PSK */

#define APP_DTLS_BUF_SIZE 64

int dtls_server(int argc, char **argv)
{
    char buf[APP_DTLS_BUF_SIZE];
    int ret;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = SERVER_PORT;

    (void)argc;
    (void)argv;

    if (sock_dtls_create(sk, &local, NULL, 0, wolfDTLSv1_2_server_method()) != 0) {
        LOG(LOG_ERROR, "ERROR: Unable to create DTLS sock\n");
        return -1;
    }

#ifndef MODULE_WOLFSSL_PSK
    /* Load certificate file for the DTLS server */
    if (wolfSSL_CTX_use_certificate_buffer(sk->ctx, server_cert,
                server_cert_len, SSL_FILETYPE_ASN1 ) != SSL_SUCCESS)
    {
        LOG(LOG_ERROR, "Failed to load certificate from memory.\n");
        return -1;
    }

    /* Load the private key */
    if (wolfSSL_CTX_use_PrivateKey_buffer(sk->ctx, server_key,
                server_key_len, SSL_FILETYPE_ASN1 ) != SSL_SUCCESS)
    {
        LOG(LOG_ERROR, "Failed to load private key from memory.\n");
        return -1;
    }
#else
    wolfSSL_CTX_set_psk_server_callback(sk->ctx, my_psk_server_cb);
    wolfSSL_CTX_use_psk_identity_hint(sk->ctx, "hint");
#endif /* MODULE_WOLFSSL_PSK */

    /* Create the DTLS session */
    ret = sock_dtls_session_create(sk);
    if (ret < 0)
    {
        LOG(LOG_ERROR, "Failed to create DTLS session (err: %s)\n", strerror(-ret));
        return -1;
    }

    LOG(LOG_INFO, "Listening on %d\n", SERVER_PORT);
    while(1) {
        /* Wait until a new client connects */
        ret = wolfSSL_accept(sk->ssl);
        if (ret != SSL_SUCCESS) {
            if (wolfSSL_get_error(sk->ssl, ret) != WOLFSSL_ERROR_WANT_READ) {
                sock_dtls_session_destroy(sk);
                if (sock_dtls_session_create(sk) < 0)
                    return -1;
            }
            continue;
        }

        /* Wait until data is received */
        LOG(LOG_INFO, "Connection accepted\n");
        ret = wolfSSL_read(sk->ssl, buf, APP_DTLS_BUF_SIZE);
        if (ret > 0) {
            buf[ret] = (char)0;
            LOG(LOG_INFO, "Received '%s'\n", buf);
        }

        /* Send reply */
        LOG(LOG_INFO, "Sending 'DTLS OK'...\n");
        wolfSSL_write(sk->ssl, Test_dtls_string, sizeof(Test_dtls_string));

        /* Cleanup/shutdown */
        LOG(LOG_INFO, "Closing connection.\n");
        sock_dtls_session_destroy(sk);
        sock_dtls_close(sk);
        break;
    }
    return 0;
}
