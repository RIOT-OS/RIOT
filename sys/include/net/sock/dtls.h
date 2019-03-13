/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sock_dtls    DTLS sock API
 * @ingroup     net_sock
 * @brief       Sock submodule for DTLS
 * @{
 *
 * @file
 * @brief   DTLS sock definitions
 *
 * @author  Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#ifndef NET_SOCK_DTLS_H
#define NET_SOCK_DTLS_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include "net/sock.h"
#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Methods for sock_dtls_create.
 * Defines the DTLS protocol version to use.
 */
#define DTLSv1_SERVER_METHOD
#define DTLSv1_CLIENT_METHOD
#define DTLSv12_SERVER_METHOD
#define DTLSv12_CLIENT_METHOD

/**
 * @brief
 */
typedef struct sock_dtls sock_dtls_t;

/**
 * @brief Information about the established session with
 *        the remote endpoint. Used when sending and
 *        receiving data to the endpoint
 */
typedef struct sock_dtls_session sock_dtls_session_t;

/**
 * @brief Queue to keep track of all established sessions.
 */
typedef struct sock_dtls_queue sock_dtls_queue_t;

/**
 * @brief Call this once before using other the API
 */
void sock_dtls_init(void);

/**
 * @brief Creates a new DTLS sock object
 *
 * @param[out] sock     The resulting DTLS sock object
 * @param[in] udp_sock  Existing UDP sock to be used
 * @param[in] method    Defines the method for the client or server to use.
 *
 * @return  0 on success.
 * @return value < 0 on error
 */
int sock_dtls_create(sock_dtls_t *sock, sock_udp_t *udp_sock, unsigned method);

/**
 * @brief Initialises the server to listen for incoming connections
 *
 * @pre `(queue != NULL) && (queue_array != NULL) && (len > 0)`
 *
 * @param[in] sock          DTLS sock to listen to
 * @param[in] queue         The resulting sessions queue
 * @param[in] queue_array   Array of session objects
 * @param[in] len           Length of @p queue_array / Max allowed session
 *                          at any time
 */
void sock_dtls_init_server(sock_dtls_t *sock, sock_dtls_queue_t *queue,
                           sock_dtls_session_t *queue_array, unsigned len);

/**
 * @brief Establish DTLS session to a server. Execute the handshake step in DTLS.
 *
 * @param[in]  sock      DLTS sock to use
 * @param[in]  ep        Endpoint to establish session with
 * @param[out] remote    The established session, cannot be NULL
 *
 * @return 0 on success
 * @return value < 0 on error
 */
int sock_dtls_establish_session(sock_dtls_t *sock, sock_udp_ep_t *ep,
                                sock_dtls_session_t *session);

/**
 * @brief Close an existing DTLS session
 *
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * @param[in] sock      sock, which the session is established on
 * @param[in] remote    Remote session to close
 */
void sock_dtls_close_session(sock_dtls_t *sock, sock_dtls_session_t *remote);

/**
 * @brief Decrypts and reads a message from a remote peer.
 *
 * @param[in] sock      DTLS sock to use
 * @param[out] remote   Remote DTLS session of the received data
 *                      Cannot be NULL
 * @param[out] buf      Pointer where the data should be stored
 * @param[in] maxlen    Maximum space available at @p data
 * @param[in] timeout   Timeout for receive in microseconds.
 *                      If 0 and no data is available, the function returns
 *                      immediately.
 *                      May be SOCK_NO_TIMEOUT for no timeout (wait until data
 *                      is available).
 *
 * @return The number of bytes received on success
 * @return value < 0 on error
 * @return  -EADDRNOTAVAIL, if local of @p sock->udp_sock is not given.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_udp_recv() blocks).
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          data.
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
ssize_t sock_dtls_recv(sock_dtls_t *sock, sock_dtls_session_t *remote,
                       void *data, size_t max_len, uint32_t timeout);

/**
 * @brief Encrypts and sends a message to a remote peer
 *
 * @param[in] sock      DTLS sock to use
 * @param[in] remote    DTLS session to use. Must not be NULL and
 *                      the session must be already established with
 *                      `sock_dtls_establish_session()` or
 *                      use the session returned by `sock_dtls_recv`.
 * @param[in] data      Pointer where the data to be send are stored
 * @param[in] len       Length of @p data to be send
 *
 * @return The number of bytes sent on success
 * @return value < 0 on error
 * @return  -EADDRINUSE, if `sock` has no local end-point.
 * @return  -EAFNOSUPPORT, if `remote->remote_ep != NULL` and
 *          sock_udp_ep_t::family of @p remote->remote_ep is != AF_UNSPEC and
 *          not supported.
 * @return  -EHOSTUNREACH, if sock_dtls_t::sock_udp_ep_t of @p remote is not
 *          reachable.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote->remote_ep is an invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::port of @p remote is 0.
 * @return  -ENOMEM, if no memory was available to send @p data.
 */
ssize_t sock_dtls_send(sock_dtls_t *sock, sock_dtls_session_t *remote,
                       const void *data, size_t len);

/**
 * @brief Destroys DTLS sock created by `sock_dtls_create`
 *
 * @pre `(sock != NULL)`
 *
 * @param sock          DTLS sock to destroy
 */
void sock_dtls_destroy(sock_dtls_t *sock);

#include "sock_dtls_types.h"

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_DTLS_H */
/** @} */
