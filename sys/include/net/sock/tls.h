/**
 * @file tls.h
 * @brief TLS over TCP socket implementation using wolfSSL for RIOT OS
 * @author Bilal-Alali
 * @date 2025-05-21 12:35:22
 */

#ifndef NET_SOCK_TLS_H
#define NET_SOCK_TLS_H

#include "net/sock/tcp.h"
#include <wolfssl/ssl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TLS Socket structure encapsulating TCP and wolfSSL components
 */
typedef struct sock_tls_tcp {
    sock_tcp_t *tcp_sock;    /**< Underlying TCP socket for network communication */
    WOLFSSL_CTX *ctx;        /**< WolfSSL context for TLS configuration */
    WOLFSSL *ssl;            /**< WolfSSL session for active TLS connection */
} sock_tls_tcp_t;

/**
 * @brief TLS Socket queue structure for managing incoming connections
 */
typedef struct sock_tls_tcp_queue {
    sock_tcp_queue_t tcp_queue;  /**< Underlying TCP queue for connection handling */
    WOLFSSL_CTX *ctx;           /**< WolfSSL context shared across connections */
} sock_tls_tcp_queue_t;

/**
 * @brief Creates and initializes a new TLS socket
 *
 * @param[out] sock Pointer to TLS socket structure to initialize
 * @param[in] method WolfSSL method (client or server) to use
 * @param[in] ca_cert Pointer to CA certificate buffer (PEM format)
 * @param[in] ca_cert_len Length of the CA certificate buffer
 * @return int 0 on success, negative errno on failure
 */
int sock_tls_tcp_create(sock_tls_tcp_t *sock, WOLFSSL_METHOD *method, const unsigned char *ca_cert, unsigned int ca_cert_len);

/**
 * @brief Establishes a TLS connection to a remote server
 *
 * @param[in,out] sock Initialized TLS socket
 * @param[in] remote Remote endpoint details
 * @param[in] local_port Local port to bind (0 for random)
 * @param[in] flags Connection flags
 * @return int 0 on success, negative errno on failure
 */
int sock_tls_tcp_connect(sock_tls_tcp_t *sock, const sock_tcp_ep_t *remote,
                        uint16_t local_port, uint16_t flags);

/**
 * @brief Initializes TLS server and starts listening for connections
 *
 * @param[in,out] queue TLS queue structure to initialize
 * @param[in] local Local endpoint to listen on
 * @param[in] queue_array Array of TCP sockets for the queue
 * @param[in] queue_len Length of queue_array
 * @param[in] flags Listening flags
 * @param[in] method Server method to use
 * @param[in] cert_buf Server certificate in PEM format
 * @param[in] cert_len Length of certificate buffer
 * @param[in] key_buf Server private key in PEM format
 * @param[in] key_len Length of private key buffer
 * @return int 0 on success, negative errno on failure
 */
int sock_tls_tcp_listen(sock_tls_tcp_queue_t *queue, const sock_tcp_ep_t *local,
                       sock_tcp_t *queue_array, unsigned queue_len, uint16_t flags,
                       WOLFSSL_METHOD *method, const unsigned char *cert_buf,
                       unsigned int cert_len, const unsigned char *key_buf,
                       unsigned int key_len);

/**
 * @brief Accepts a new TLS connection from the queue
 *
 * @param[in] queue Queue to accept connection from
 * @param[out] sock Pointer to allocated TLS socket structure pointer
 * @param[in] timeout Accept timeout in microseconds
 * @return int 0 on success, negative errno on failure
 */
int sock_tls_tcp_accept(sock_tls_tcp_queue_t *queue, sock_tls_tcp_t **sock,
                       uint32_t timeout);

/**
 * @brief Reads data from an established TLS connection
 *
 * @param[in] sock TLS socket to read from
 * @param[out] data Buffer to store read data
 * @param[in] max_len Maximum number of bytes to read
 * @return ssize_t Number of bytes read or negative errno on failure
 */
ssize_t sock_tls_tcp_read(sock_tls_tcp_t *sock, void *data, size_t max_len);

/**
 * @brief Writes data to an established TLS connection
 *
 * @param[in] sock TLS socket to write to
 * @param[in] data Data buffer to write
 * @param[in] len Number of bytes to write
 * @return ssize_t Number of bytes written or negative errno on failure
 */
ssize_t sock_tls_tcp_write(sock_tls_tcp_t *sock, const void *data, size_t len);

/**
 * @brief Closes and cleanup a TLS connection
 *
 * @param[in] sock TLS socket to disconnect and cleanup
 */
void sock_tls_tcp_disconnect(sock_tls_tcp_t *sock);

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_TLS_H */
