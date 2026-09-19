
#ifndef GNRC_WOLFSSL_TLS_H
#define GNRC_WOLFSSL_TLS_H

#include "wolfssl/ssl.h"
#include <net/sock/tcp.h>
#define gnrc_wolfssl_tls_max_timeout_ms 4000

struct TLSContext;
typedef struct TLSContext TLSContext;

/**
 * @brief Initialize the TLS module.
 *
 * This function should be called once at the beginning to initialize the TLS module.
 *
 * @param context The TLS context.
 * @return 0 on success, negative value on failure.
 */
int tls_init(TLSContext *context);

/**
 * @brief Log a message.
 *
 * @param format The format string for the log message.
 * @param ... Additional arguments for the format string.
 */
void tls_log(const char *format, ...);

/**
 * @brief Create a new TLS context.
 *
 * This function creates a new TLS context for a server or a client.
 * @param log_callback A function pointer for logging.
 *
 * @return A pointer to the TLS context on success, NULL on failure.
 */
TLSContext *tls_create_context(void (*_log_callback)(const char *message));

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
 * @param context The TLS context.
 * @param socket_fd File descriptor of the underlying TCP socket.
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

#endif /* TLS_MODULE_H */
