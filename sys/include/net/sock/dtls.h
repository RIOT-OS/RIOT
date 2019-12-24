/*
 * Copyright (C) 2019 HAW Hamburg
 *                    Freie Universität Berlin
 *                    Inria
 *                    Daniele Lacamera
 *                    Ken Bannister
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sock_dtls    DTLS sock API
 * @ingroup     net_sock net_dtls
 * @brief       Sock submodule for DTLS
 *
 * DTLS sock acts as a wrapper for the underlying DTLS module to provide
 * encryption for applications using the UDP sock API.
 *
 * How To Use
 * ----------
 *
 * ### Summary
 *
 * - Include module implementing the DTLS sock API in the Makefile
 * - Add credentials
 *   1. Fill credman_credential_t with the credential information
 *   2. Add the credential using @ref credman_add()
 * - Server operation
 *   1. Create UDP sock @ref sock_udp_create()
 *   2. Create DTLS sock @ref sock_dtls_create() using role @ref SOCK_DTLS_SERVER
 *   3. Start listening with @ref sock_dtls_recv()
 * - Client operation
 *   1. Create UDP sock @ref sock_udp_create()
 *   2. Create DTLS sock @ref sock_dtls_create() using role @ref SOCK_DTLS_CLIENT
 *   3. Create session to server @ref sock_dtls_session_create()
 *   4. Send packet to server @ref sock_dtls_send()
 *
 * ## Makefile Includes
 *
 * First, we need to [include](@ref including-modules) a module that implements
 * this API in our applications Makefile. For example the module that
 * implements this API for [tinydtls](@ref pkg_tinydtls) is called
 * `tinydtls_sock_dtls'.
 *
 * The corresponding [pkg](@ref pkg) providing the DTLS implementation will be
 * automatically included so there is no need to use `USEPKG` to add the pkg
 * manually.
 *
 * Each DTLS implementation may have its own configuration options and caveat.
 * This can be found at @ref net_dtls.
 *
 * ### Adding credentials
 *
 * Before using this API, either as a server or a client, we first need to
 * add the credentials to be used for the encryption using
 * [credman](@ref net_credman). Note that credman does not copy the credentials
 * given into the system, it only has information about the credentials and
 * where it is located at. So it is your responsibility to make sure that the
 * credential is valid during the lifetime of your application.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <stdio.h>
 *
 * #include "net/credman.h"
 *
 * #define SOCK_DTLS_SERVER_TAG (10)
 * #define SOCK_DTLS_CLIENT_TAG (20)
 *
 * static char *psk_key = "secretPSK";
 * static char *psk_id = "secretID";
 *
 * static const unsigned char server_ecdsa_priv_key[] = {...};
 * static const unsigned char server_ecdsa_pub_key_x[] = {...};
 * static const unsigned char server_ecdsa_pub_key_y[] = {...};
 * static const unsigned char client_pubkey_x[] = {...};
 * static const unsigned char client_pubkey_y[] = {...};
 *
 * static ecdsa_public_key_t other_pubkeys[] = {
 *     { .x = client_pubkey_x, .y = client_pubkey_y },
 * };
 *
 * int main(void)
 * {
 *     credman_credential_t psk_credential = {
 *         .type = CREDMAN_TYPE_PSK,
 *         .tag = SOCK_DTLS_SERVER_TAG,
 *         .params = {
 *             .psk = {
 *                 .key = { .s = psk_key, .len = sizeof(psk_key), },
 *                 .id = { .s = psk_id, .len = sizeof(psk_id), },
 *             },
 *         },
 *     };
 *
 *     if (credman_add(&psk_credential) < 0) {
 *         puts("Error cannot add credential");
 *     }
 *
 *     credman_credential_t ecc_credential = {
 *         .type = CREDMAN_TYPE_ECDSA,
 *         .tag = SOCK_DTLS_SERVER_TAG,
 *         .params = {
 *             .ecdsa = {
 *                 .private_key = server_ecdsa_priv_key,
 *                 .public_key = {
 *                     .x = server_ecdsa_pub_key_x,
 *                     .y = server_ecdsa_pub_key_y,
 *                 },
 *                 .client_keys = other_pubkeys,
 *                 .client_keys_size = sizeof(other_pubkeys) / sizeof(other_pubkeys[0]),
 *             },
 *         },
 *     };
 *
 *     if (credman_add(&ecc_credential) < 0) {
 *         puts("Error cannot add credential");
 *     }
 *
 *     // start server/client
 *     // [...]
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Above we see an example of how to register a PSK and an ECC credential.
 *
 * First, we need to include the header file for the API.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/credman.h"
 *
 * int main(void)
 * {
 *     credman_credential_t psk_credential = {
 *         .type = CREDMAN_TYPE_PSK,
 *         .tag = SOCK_DTLS_SERVER_TAG,
 *         .params = {
 *             .psk = {
 *                 .key = { .s = psk_key, .len = sizeof(psk_key), },
 *                 .id = { .s = psk_id, .len = sizeof(psk_id), },
 *             },
 *         },
 *     };
 *
 *     [...]
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * We tell [credman](@ref net_credman) which credential to add by filling in
 * the credentials information in a struct @ref credman_credential_t. For
 * PSK credentials, we use enum @ref CREDMAN_TYPE_PSK for the
 * [type](@ref credman_credential_t::type).
 *
 * Next, we must assign a [tag](@ref credman_tag_t) for the credential. Tags
 * are unsigned integer value that is used to identify which DTLS sock has
 * access to which credential. Each DTLS sock will also be assigned a tag.
 * As a result, a sock can only use credentials that have the same tag as
 * its assigned tag.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * if (credman_add(&psk_credential) < 0) {
 *     puts("Error cannot add credential");
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * After credential information is filled, we can add it to the credential
 * pool using @ref credman_add().
 *
 * For adding credentials of other types, you can follow the steps above except
 * credman_credential_t::type and credman_credential_t::params depend on the
 * type of credential used.
 *
 * ### Server Operation
 *
 * After credentials are added, we can start the server.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <stdio.h>
 *
 * #include "net/sock/dtls.h"
 *
 * #define SOCK_DTLS_SERVER_TAG (10)
 *
 * int main(void)
 * {
 *     // Add credentials
 *     // [...]
 *
 *     // initialize server
 *     sock_udp_t udp_sock;
 *     sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 *     local.port = 20220;
 *     if (sock_udp_create(&udp_sock, &local, NULL, 0) < 0) {
 *         puts("Error creating UDP sock");
 *         return -1;
 *     }
 *
 *     sock_dtls_t dtls_sock;
 *     if (sock_dtls_create(&dtls_sock, &udp_sock,
 *                          SOCK_DTLS_SERVER_TAG,
 *                          SOCK_DTLS_1_2, SOCK_DTLS_SERVER) < 0) {
 *         puts("Error creating DTLS sock");
 *         return -1;
 *     }
 *
 *     while (1) {
 *         int res;
 *         char buf[128];
 *         sock_dtls_session_t session;
 *
 *         res = sock_dtls_recv(&dtls_sock, &session, buf, sizeof(buf),
 *                              SOCK_NO_TIMEOUT);
 *         if (res > 0) {
 *             printf("Received %d bytes\n", res);
 *             if (sock_dtls_send(&dtls_sock, &session, buf, res) < 0) {
 *                 puts("Error sending reply");
 *             }
 *         }
 *     }
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * This is an example of a DTLS echo server.
 *
 * DTLS sock uses an initialized UDP sock to send and receive encrypted
 * packets. Therefore, the listening port for the server also needs to be
 * set here.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * sock_udp_t udp_sock;
 * sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 * local.port = 20220;
 * if (sock_udp_create(&udp_sock, &local, NULL, 0) < 0) {
 *     puts("Error creating UDP sock");
 *     return -1;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Using the initialized UDP sock, we can then create our DTLS sock. We use
 * SOCK_DTLS_SERVER_TAG, which is defined as `10` in our application code
 * beforehand, as our tag. Using @ref SOCK_DTLS_1_2 and @ref SOCK_DTLS_SERVER,
 * we set our DTLS endpoint to use DTLS version 1.2 and act as a DTLS server.
 *
 * Note that some DTLS implementation do not support earlier versions of DTLS.
 * In this case, @ref sock_dtls_create() will return an error. A list of
 * supported DTLS version for each DTLS implementation can be found at this
 * [page](@ref net_dtls). In case of error, the program is stopped.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #define SOCK_DTLS_SERVER_TAG (10)
 *
 * [...]
 *
 * sock_dtls_t dtls_sock;
 * if (sock_dtls_create(&dtls_sock, &udp_sock,
 *                      SOCK_DTLS_SERVER_TAG,
 *                      SOCK_DTLS_1_2, SOCK_DTLS_SERVER) < 0) {
 *     puts("Error creating DTLS sock");
 *     return -1;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Now we can listen to incoming packets using @ref sock_dtls_recv(). The
 * application waits indefinitely for new packets. If we want to timeout this
 * wait period we could alternatively set the `timeout` parameter of the
 * function to a value != @ref SOCK_NO_TIMEOUT. If an error occurs we just
 * ignore it and continue looping. We can reply to an incoming message using
 * its `session`.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * while (1) {
 *     int res;
 *     char buf[128];
 *     sock_dtls_session_t session;
 *
 *     res = sock_dtls_recv(&dtls_sock, &session, buf, sizeof(buf),
 *                          SOCK_NO_TIMEOUT);
 *     if (res > 0) {
 *         printf("Received %d bytes -- echo!\n", res);
 *         if (sock_dtls_send(&dtls_sock, &session, buf, res) < 0) {
 *             puts("Error sending reply");
 *         }
 *     }
 * }
 * return 0;
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ### Client Operation
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/sock/udp.h"
 * #include "net/sock/dtls.h"
 * #include "net/ipv6/addr.h"
 * #include "net/credman.h"
 *
 * #define SOCK_DTLS_CLIENT_TAG (20)
 *
 * #ifndef SERVER_ADDR
 * #define SERVER_ADDR "fe80::aa:bb:cc:dd" // replace this with the server address
 * #endif
 *
 * int main(void)
 * {
  *    // Add credentials
 *     // [...]
 *
 *     // initialize client
 *     char rcv[128];
 *     sock_udp_t udp_sock;
 *     sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 *     local.port = 12345;
 *
 *     sock_udp_ep_t remote;
 *     remote.port = DTLS_DEFAULT_PORT;
 *     remote.netif = gnrc_netif_iter(NULL)->pid;   // only if GNRC_NETIF_NUMOF == 1
 *
 *     sock_dtls_t dtls_sock;
 *     sock_dtls_session_t session;
 *
 *     if (!ipv6_addr_from_str((ipv6_addr_t *)remote.addr.ipv6, SERVER_ADDR)) {
 *         puts("Error parsing destination address");
 *         return -1;
 *     }
 *
 *     if (sock_udp_create(&udp_sock, &local, NULL, 0) < 0) {
 *         puts("Error creating UDP sock");
 *         return -1;
 *     }
 *
 *     if (sock_dtls_create(&dtls_sock, &udp_sock,
 *                          SOCK_DTLS_CLIENT_TAG,
 *                          SOCK_DTLS_1_2, SOCK_DTLS_CLIENT) < 0) {
 *         puts("Error creating DTLS sock");
 *         sock_udp_close(&udp_sock);
 *         return -1;
 *     }
 *
 *     if (sock_dtls_session_create(&dtls_sock, &remote, &session) < 0) {
 *         puts("Error creating session");
 *         sock_dtls_close(&dtls_sock);
 *         sock_udp_close(&udp_sock);
 *         return -1;
 *     }
 *
 *     const char data[] = "HELLO";
 *     int res = sock_dtls_send(&dtls_sock, &session, data, sizeof(data));
 *     if (res >= 0) {
 *         printf("Sent %d bytes\n", res);
 *         res = sock_dtls_recv(&dtls_sock, &session, rcv, sizeof(rcv), SOCK_NO_TIMEOUT);
 *         if (res > 0) {
 *             printf("Received %d bytes\n", res);
 *         }
 *     }
 *     else {
 *         puts("Error sending data");
 *     }
 *
 *     sock_dtls_session_destroy(&dtls_sock, &session);
 *     sock_dtls_close(&dtls_sock);
 *     sock_udp_close(&udp_sock);
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * This is an example of a DTLS echo client.
 *
 * Like the server, we must first create the UDP sock.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * sock_udp_t udp_sock;
 * sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 * local.port = 12345;
 * sock_udp_create(&udp_sock, &local, NULL, 0);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * After that, we set the address of the remote endpoint and its
 * listening port, which is DTLS_DEFAULT_PORT (20220).
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * sock_udp_ep_t remote;
 * remote.port = DTLS_DEFAULT_PORT;
 * remote.netif = gnrc_netif_iter(NULL)->pid;   // only if GNRC_NETIF_NUMOF == 1
 *
 * if (!ipv6_addr_from_str((ipv6_addr_t *)remote.addr.ipv6, SERVER_ADDR)) {
 *     puts("Error parsing destination address");
 *     return -1;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * After the UDP sock is created, we can proceed with creating the DTLS sock.
 * Before sending the packet, we must first create a session with the remote
 * endpoint using @ref sock_dtls_session_create(). If the handshake is
 * successful and the session is created, we send packets to it using
 * @ref sock_dtls_send(). If the packet is successfully sent, we listen for
 * the response with @ref sock_dtls_recv().
 *
 * @ref sock_dtls_create() and @ref sock_dtls_close() only manages the DTLS
 * layer. That means we still have to clean up the created UDP sock from before
 * by calling @ref sock_udp_close() on our UDP sock in case of error or we
 * reached the end of the application.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * char rcv[128];
 * sock_dtls_t dtls_sock;
 * sock_dtls_session_t session;
 *
 * [...]
 *
 * if (sock_dtls_create(&dtls_sock, &udp_sock,
 *                      SOCK_DTLS_CLIENT_TAG,
 *                      SOCK_DTLS_1_2, SOCK_DTLS_CLIENT) < 0) {
 *     puts("Error creating DTLS sock");
 *     sock_udp_close(&udp_sock);
 *     return -1;
 * }
 *
 * if (sock_dtls_session_create(&dtls_sock, &remote, &session) < 0) {
 *     puts("Error creating session");
 *     sock_dtls_close(&dtls_sock);
 *     sock_udp_close(&udp_sock);
 *     return -1;
 * }
 *
 * const char data[] = "HELLO";
 * int res = sock_dtls_send(&dtls_sock, &session, data, sizeof(data));
 * if (res >= 0) {
 *     printf("Sent %d bytes: %*.s\n", res, res, data);
 *     res = sock_dtls_recv(&dtls_sock, &session, rcv, sizeof(rcv), SOCK_NO_TIMEOUT);
 *     if (res > 0) {
 *         printf("Received %d bytes: %*.s\n", res, res, rcv);
 *     }
 * }
 * else {
 *     puts("Error sending data");
 * }
 *
 * sock_dtls_session_destroy(&dtls_sock, &session);
 * sock_dtls_close(&dtls_sock);
 * sock_udp_close(&udp_sock);
 * return 0;
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief   DTLS sock definitions
 *
 * @author  Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Raul A. Fuentes Samaniego <raul.fuentes-samaniego@inria.fr>
 * @author  Daniele Lacamera <daniele@wolfssl.com>
 * @author  Ken Bannister <kb2ma@runbox.com>
 */

#ifndef NET_SOCK_DTLS_H
#define NET_SOCK_DTLS_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include "net/sock.h"
#include "net/sock/udp.h"
#include "net/credman.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DTLS version number
 * @anchor sock_dtls_prot_version
 * @{
 */
enum {
    SOCK_DTLS_1_0 = 1,  /**< DTLS version 1.0 */
    SOCK_DTLS_1_2 = 2,  /**< DTLS version 1.2 */
    SOCK_DTLS_1_3 = 3,  /**< DTLS version 1.3 */
};
/** @} */

/**
 * @brief DTLS role
 * @anchor sock_dtls_role
 * @{
 */
enum {
    SOCK_DTLS_CLIENT = 1,   /**< Endpoint client role */
    SOCK_DTLS_SERVER = 2,   /**< Endpoint server role */
};
/** @} */

/**
 * @brief   Type for a DTLS sock object
 *
 * @note    API implementors: `struct sock_dtls` needs to be defined by
 *          an implementation-specific `sock_dtls_types.h`.
 */
typedef struct sock_dtls sock_dtls_t;

/**
 * @brief Information about a created session.
 */
typedef struct sock_dtls_session sock_dtls_session_t;

/**
 * @brief Called exactly once during `auto_init`.
 *
 * Calls the initialization function required by the DTLS stack used.
 */
void sock_dtls_init(void);

/**
 * @brief Creates a new DTLS sock object
 *
 * Takes an initialized UDP sock and uses it for the transport.
 * Memory allocation functions required by the underlying DTLS
 * stack can be called in this function.
 *
 * @see net_credman.
 *
 * @param[out] sock     The resulting DTLS sock object
 * @param[in] udp_sock  Existing UDP sock initialized with
 *                      @ref sock_udp_create()to be used underneath.
 * @param[in] tag       Credential tag of @p sock. The sock will only use
 *                      credentials with the same tag given here.
 * @param[in] version   [DTLS version](@ref sock_dtls_prot_version) to use.
 * @param[in] role      [Role](@ref sock_dtls_role) of the endpoint.
 *
 * @return  0 on success.
 * @return  -1 on error
 */
int sock_dtls_create(sock_dtls_t *sock, sock_udp_t *udp_sock,
                     credman_tag_t tag, unsigned version, unsigned role);

/**
 * @brief Creates a new DTLS session
 *
 * Initializes handshake process with a DTLS server  at @p ep.
 *
 * @param[in]  sock     DLTS sock to use
 * @param[in]  ep       Remote endpoint of the session
 * @param[out] remote   The created session, cannot be NULL
 *
 * @return  0 on success
 * @return  -EAGAIN, if DTLS_HANDSHAKE_TIMEOUT is `0` and no data is available.
 * @return  -EADDRNOTAVAIL, if the local endpoint of @p sock is not set.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_udp_recv() blocks).
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          credentials.
 * @return  -ETIMEDOUT, if timed out when trying to create session.
 */
int sock_dtls_session_create(sock_dtls_t *sock, const sock_udp_ep_t *ep,
                             sock_dtls_session_t *remote);

/**
 * @brief Destroys an existing DTLS session
 *
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * @param[in] sock      @ref sock_dtls_t, which the session is created on
 * @param[in] remote    Remote session to destroy
 */
void sock_dtls_session_destroy(sock_dtls_t *sock, sock_dtls_session_t *remote);

/**
 * @brief Decrypts and reads a message from a remote peer.
 *
 * @param[in] sock      DTLS sock to use.
 * @param[out] remote   Remote DTLS session of the received data.
 *                      Cannot be NULL.
 * @param[out] data     Pointer where the received data should be stored.
 * @param[in] maxlen    Maximum space available at @p data.
 * @param[in] timeout   Receive timeout in microseconds.
 *                      If 0 and no data is available, the function returns
 *                      immediately.
 *                      May be SOCK_NO_TIMEOUT to wait until data
 *                      is available.
 *
 * @note Function may block if data is not available and @p timeout != 0
 *
 * @return The number of bytes received on success
 * @return  -EADDRNOTAVAIL, if the local endpoint of @p sock is not set.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_dtls_recv() blocks).
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          data.
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
ssize_t sock_dtls_recv(sock_dtls_t *sock, sock_dtls_session_t *remote,
                       void *data, size_t maxlen, uint32_t timeout);

/**
 * @brief Encrypts and sends a message to a remote peer
 *
 * @param[in] sock      DTLS sock to use
 * @param[in] remote    DTLS session to use. A new session will be created
 *                      if no session exist between client and server.
 * @param[in] data      Pointer where the data to be send are stored
 * @param[in] len       Length of @p data to be send
 *
 * @note Function may block until a session is created if there is no
 *       existing session with @p remote.
 *
 * @return The number of bytes sent on success
 * @return  -EADDRINUSE, if sock_dtls_t::udp_sock has no local end-point.
 * @return  -EAFNOSUPPORT, if `remote->ep != NULL` and
 *          sock_dtls_session_t::ep::family of @p remote is != AF_UNSPEC and
 *          not supported.
 * @return  -EHOSTUNREACH, if sock_dtls_session_t::ep of @p remote is not
 *          reachable.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote->ep is an
 *          invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::port of @p remote->ep is 0.
 * @return  -ENOMEM, if no memory was available to send @p data.
 */
ssize_t sock_dtls_send(sock_dtls_t *sock, sock_dtls_session_t *remote,
                       const void *data, size_t len);

/**
 * @brief Closes a DTLS sock
 *
 * Releases any memory allocated by @ref sock_dtls_create(). This function does
 * NOT close the UDP sock used by the DTLS sock. After the call to this
 * function, user will have to call @ref sock_udp_close() to close the UDP
 * sock.
 *
 * @pre `(sock != NULL)`
 *
 * @param sock          DTLS sock to close
 */
void sock_dtls_close(sock_dtls_t *sock);

#include "sock_dtls_types.h"

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_DTLS_H */
/** @} */
