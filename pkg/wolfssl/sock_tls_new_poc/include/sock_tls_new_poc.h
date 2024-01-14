/*
 * Copyright (C) 2024 tanvirBsmrstu
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    module sock_tls_new_poc
 * @ingroup     pkg_wolfssl
 * @brief       TLS POC using wolfssl and GNRC
 *
 * @{
 *
 * @file
 *
 * @author      tanvirBsmrstu <tanvir.bsmrstu@gmail.com>
 */

#ifndef SOCK_TLS_NEW_POC_H
#define SOCK_TLS_NEW_POC_H

#include "wolfssl/ssl.h"
#include <net/sock/tcp.h>

// Can be decided later
#define gnrc_wolfssl_tls_max_timeout_ms 4000

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TLS context forward decleartion.
 */
struct TLSContext;
typedef struct TLSContext TLSContext;

/**
 * @brief Create a new TLS context.
 *
 * This function creates a new TLS context for a server or a client.
 * 
 * @return A pointer to the TLS context.
 */
TLSContext *tls_create_context();

/**
 * @brief Initialize the TLS module.
 *
 * This function should be called once at the beginning to initialize the TLS module.
 *
 * @param context The TLS context object configuration.
 * @return 0 on success, negative value on failure.
 */
int tls_init(TLSContext *context);

/**
 * @brief Set the CA certificate, device certificate chain, and private key for the TLS context.
 *
 * @param ctx The TLS context.
 * @param ca_cert_path Path to the CA certificate file.
 * @param cert_chain_path Path to the device certificate chain file.
 * @param private_key_path Path to the private key file.
 * @return 0 on success, negative value on failure.
 */
int tls_set_certificate(TLSContext *ctx, const char *ca_cert_path, const char *cert_chain_path,
                        const char *private_key_path);

/**
 * @brief Establish a TLS connection.
 *
 * @param remoteAddress remote endpoint address
 * @param port remote port to connect
 * @param context The TLS context created with @ref `tls_create_context`.
 * @return 0 on success, negative value on failure.
 */
int tls_establish_connection(char *remoteAddress, int port, TLSContext *context);

/**
 * @brief Send data over the TLS connection.
 *
 * @param ctx The TLS context.
 * @param data Pointer to the data to be sent.
 * @param size Size of the data to be sent.
 * @return Number of bytes sent on success, negative value on failure.
 */
int tls_send(TLSContext *ctx, unsigned char *buffer, int buffer_len, int timeout_ms);

/**
 * @brief Receive data from the TLS connection.
 *
 * @param ctx The TLS context.
 * @param buffer Pointer to the buffer for received data.
 * @param size Size of the buffer.
 * @return Number of bytes received on success, negative value on failure.
 */
int tls_receive(TLSContext *ctx, unsigned char *buffer, int buffer_len, int timeout_ms);

/**
 * @brief Close the TLS connection and free resources.
 *
 * @param ctx The TLS context.
 */
void tls_close(TLSContext *ctx);

#ifdef __cplusplus
}
#endif

#endif /* SOCK_TLS_NEW_POC_H */
/** @} */
