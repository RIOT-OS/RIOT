/*
 * Copyright (C) 2024 tanvirBsmrstu
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum timeout for TLS operations.
 */
#define gnrc_wolfssl_tls_max_timeout_ms 4000

/**
 * @brief TLS context forward declaration to encapsulate.
 */
struct TLSContext;

/**
 * @brief TLS context type definition.
 */
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
 * @param context The TLS context created with `tls_create_context`.
 * @return 0 on success, negative value on failure.
 */
int tls_establish_connection(char *remoteAddress, int port, TLSContext *context);

/**
 * @brief Send data over the TLS connection.
 *
 * @param ctx The TLS context.
 * @param buffer Pointer to the data to be sent.
 * @param buffer_len Size of the data to be sent.
 * @param timeout_ms Timeout in milliseconds (will be provided by MQTT).
 * @return Number of bytes sent on success, negative value on failure.
 */
int tls_send(TLSContext *ctx, unsigned char *buffer, int buffer_len, int timeout_ms);

/**
 * @brief Receive data from the TLS connection.
 *
 * @param ctx The TLS context.
 * @param buffer Pointer to the buffer for received data.
 * @param buffer_len Size of the buffer.
 * @param timeout_ms Timeout in milliseconds (will be provided by MQTT).
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
