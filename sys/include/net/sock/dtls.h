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

#pragma once

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
 *   2. Create DTLS sock @ref sock_dtls_create() using role
 *      @ref SOCK_DTLS_SERVER.
 *   3. Optionally:
 *          - when using PSK ciphersuites, set a hint @ref sock_dtls_set_server_psk_id_hint()
 *          - add extra credentials @ref sock_dtls_add_credential()
 *          - when using ECC ciphersuites, set a callback for credential selection
 *            @ref sock_dtls_set_rpk_cb()
 *   4. Start listening with @ref sock_dtls_recv()
 * - Client operation
 *   1. Create UDP sock @ref sock_udp_create()
 *   2. Create DTLS sock @ref sock_dtls_create() using role
 *      @ref SOCK_DTLS_CLIENT
 *   3. Optionally:
 *          - add extra credentials @ref sock_dtls_add_credential()
 *          - when using PSK ciphersuites, set a callback for hint reception and credential
 *            selection @ref sock_dtls_set_client_psk_cb()
 *          - when using ECC ciphersuites, set a callback for credential selection
 *            @ref sock_dtls_set_rpk_cb()
 *   4. Start handshake session to server @ref sock_dtls_session_init()
 *   5. Handle incoming handshake packets with @ref sock_dtls_recv()
 *   6. Send packet to server @ref sock_dtls_send()
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
 *                 .client_keys_size = ARRAY_SIZE(other_pubkeys),
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
 * are unsigned integer values used to identify which DTLS sock has
 * access to which credentials. Each DTLS sock will also be assigned a list of tags.
 * As a result, a sock can only use credentials that have the same tag as
 * the ones in the list.
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
 * #define SERVER_ADDR "fe80::aa:bb:cc:dd" // replace with the server address
 * #endif
 *
 * int main(void)
 * {
 *     // Add credentials
 *     // [...]
 *
 *     // initialize client
 *     char rcv[128];
 *     sock_udp_t udp_sock;
 *     sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 *     local.port = 12345;
 *
 *     sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
 *     remote.port = DTLS_DEFAULT_PORT;
 *     remote.netif = gnrc_netif_iter(NULL)->pid;   // only if gnrc_netif_highlander() returns true
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
 *     if (sock_dtls_session_init(&dtls_sock, &remote, &session) < 0) {
 *         puts("Error initiating session");
 *         sock_dtls_close(&dtls_sock);
 *         sock_udp_close(&udp_sock);
 *         return -1;
 *     }
 *
 *     if (sock_dtls_recv(&dtls_sock, &session, rcv, sizeof(rcv),
 *                        SOCK_NO_TIMEOUT) != -SOCK_DTLS_HANDSHAKE) {
 *         puts("Error completing handshake");
 *         sock_dtls_close(&dtls_sock);
 *         sock_udp_close(&udp_sock);
 *         return -1;
 *     }
 *
 *     const char data[] = "HELLO";
 *     int res = sock_dtls_send(&dtls_sock, &session, data, sizeof(data), 0);
 *     if (res >= 0) {
 *         printf("Sent %d bytes\n", res);
 *         res = sock_dtls_recv(&dtls_sock, &session, rcv, sizeof(rcv),
 *                              SOCK_NO_TIMEOUT);
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
 * sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
 * remote.port = DTLS_DEFAULT_PORT;
 * remote.netif = gnrc_netif_iter(NULL)->pid;   // only if gnrc_netif_highlander() returns true
 *
 * if (!ipv6_addr_from_str((ipv6_addr_t *)remote.addr.ipv6, SERVER_ADDR)) {
 *     puts("Error parsing destination address");
 *     return -1;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * After the UDP sock is created, we can proceed with creating the DTLS sock.
 * Before sending the packet, we must first initiate a session handshake with
 * the remote endpoint using @ref sock_dtls_session_init(). We will need to call
 * @ref sock_dtls_recv() to receive and process all the handshake packets.
 * If the handshake is successful and the session is created, we send packets
 * to it using @ref sock_dtls_send(). As we already know the session exists,
 * we can set the timeout to `0` and listen to the reply with @ref sock_dtls_recv().
 *
 * Alternatively, set the timeout to of @ref sock_dtls_send() to the duration we
 * want to wait for the handshake process. We can also set the timeout to
 * @ref SOCK_NO_TIMEOUT to block indefinitely until handshake is complete.
 * After handshake completes, the packet will be sent.
 *
 * @ref sock_dtls_init(), @ref sock_dtls_recv and @ref sock_dtls_close() only
 * manages the DTLS layer. That means we still have to clean up the created
 * UDP sock from before by calling @ref sock_udp_close() on our UDP sock
 * in case of error or we reached the end of the application.
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
 * if (sock_dtls_session_init(&dtls_sock, &remote, &session) < 0) {
 *     puts("Error initiating session");
 *     sock_dtls_close(&dtls_sock);
 *     sock_udp_close(&udp_sock);
 *     return -1;
 * }
 *
 * if (sock_dtls_recv(&dtls_sock, &session, rcv, sizeof(rcv),
 *                    SOCK_NO_TIMEOUT) != -SOCK_DTLS_HANDSHAKE) {
 *     puts("Error completing handshake");
 *     sock_dtls_close(&dtls_sock);
 *     sock_udp_close(&udp_sock);
 *     return -1;
 * }
 *
 * const char data[] = "HELLO";
 * int res = sock_dtls_send(&dtls_sock, &session, data, sizeof(data), 0);
 * if (res >= 0) {
 *     printf("Sent %d bytes: %*.s\n", res, res, data);
 *     res = sock_dtls_recv(&dtls_sock, &session, rcv, sizeof(rcv),
 *                          SOCK_NO_TIMEOUT);
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
 * ### Multi-credential handling
 *
 * Each sock needs at least one credential tag to operate. `sock_dtls_create` allows to optionally
 * assign an initial credential. Extra credentials can be added and removed using
 * @ref sock_dtls_add_credential and @ref sock_dtls_remove_credential respectively (found in
 * `net/sock/dtls/creds.h`).
 *
 * #### Pre-shared Keys Cipher Suites
 * In the case of PSK, a server can optionally indicate a hint to help the client to decide which
 * PSK Identity to use, using @ref sock_dtls_set_server_psk_id_hint
 * (see https://tools.ietf.org/html/rfc4279#section-5.2). The client application can
 * decide which credential to use based on the sent hint and/or the session information, by
 * registering a callback with @ref sock_dtls_set_client_psk_cb. If no callback is registered, or
 * fails to chose a tag (i.e. it returns @ref CREDMAN_TAG_EMPTY), the credential is chosen as
 * follows: if a hint is sent by the server, all credentials registered to the sock are checked for
 * a matching @ref psk_params_t::hint "hint". A credential is selected on matching hint. If no
 * credential matches the hint or no hint is provided, the first PSK credential registered in the
 * sock is used.
 *
 * #### Elliptic Curve Cryptography Cipher Suites
 * When using ECC both client and server applications can register a callback to decide which of
 * the registered credentials should be used, based on the session information. This is done using
 * @ref sock_dtls_set_rpk_cb.
 *
 * In both cases, if no callbacks are registered, the sock implementation will try to find a
 * registered credential in the Sock's credential list, that matches the needed type. The first
 * one that matches is used.
 *
 * #### Public key verification when using ECC
 *
 * By enabling the pseudomodule `sock_dtls_verify_public_key` the DTLS sock will verify the
 * public key of the remote peer. When enabled, the DTLS sock will only accept a connection if
 * the provided public key is in the list of public keys assigned to the specified sock. This only
 * applies when using ECC ciphersuites (i.e., not PSK).
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
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

/* net/sock/async/types.h included by net/sock.h needs to re-typedef the
 * `sock_dtls_t` to prevent cyclic includes */
#if defined (__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wtypedef-redefinition"
#endif

#include "net/sock.h"
#include "net/sock/udp.h"
#include "net/credman.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_sock_dtls_conf    SOCK DTLS compile configuration
 * @ingroup  net_sock_conf
 * @{
 */
/**
 * @brief   Default buffer size for DTLS handshake (as exponent of 2^n)
 *
 * As the buffer size ALWAYS needs to be power of two, this option represents
 * the exponent of 2^n, which will be used as the size of the buffer
 * ( @ref DTLS_HANDSHAKE_BUFSIZE ).
 *
 */
#ifndef CONFIG_DTLS_HANDSHAKE_BUFSIZE_EXP
#define CONFIG_DTLS_HANDSHAKE_BUFSIZE_EXP  8
#endif
/** @} */

/**
 * @brief   Size buffer used in handshake to hold credentials
 */
#ifndef DTLS_HANDSHAKE_BUFSIZE
#define DTLS_HANDSHAKE_BUFSIZE  (1 << CONFIG_DTLS_HANDSHAKE_BUFSIZE_EXP)
#endif

/**
 * @brief   Return value for a successful handshake
 */
#define SOCK_DTLS_HANDSHAKE     (EXDEV)

/**
 * @brief Force extended master secret extension
 */
#ifndef CONFIG_DTLS_FORCE_EXTENDED_MASTER_SECRET
#define CONFIG_DTLS_FORCE_EXTENDED_MASTER_SECRET 1
#endif

/**
 * @brief Force renegotiation info extension
 */
#ifndef CONFIG_DTLS_FORCE_RENEGOTIATION_INFO
#define CONFIG_DTLS_FORCE_RENEGOTIATION_INFO 1
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
 * @note    API implementers: `struct sock_dtls` needs to be defined by
 *          an implementation-specific `sock_dtls_types.h`.
 */
typedef struct sock_dtls sock_dtls_t;

#if defined (__clang__)
# pragma clang diagnostic pop
#endif

/**
 * @brief Information about a created session.
 */
typedef struct sock_dtls_session sock_dtls_session_t;

/**
 * @brief   Auxiliary data provided when receiving using an DTLS sock object
 *
 * @warning Implementations of this API may rely on this type to be compatible
 *          with @ref sock_udp_aux_rx_t. These implementations need to be
 *          updated, if this is no longer the case. Users of this API should
 *          not rely on this compatibility
 */
typedef sock_udp_aux_rx_t sock_dtls_aux_rx_t;

/**
 * @brief   Auxiliary data provided when sending using an DTLS sock object
 *
 * @warning Implementations of this API may rely on this type to be compatible
 *          with @ref sock_udp_aux_rx_t. These implementations need to be
 *          updated, if this is no longer the case. Users of this API should
 *          not rely on this compatibility
 */
typedef sock_udp_aux_tx_t sock_dtls_aux_tx_t;

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
 *                      @ref sock_udp_create() to be used underneath.
 * @param[in] tag       Credential tag of @p sock. The sock will only use
 *                      credentials with the tags registered to it
 *                      (see @ref sock_dtls_add_credential). Set to @ref CREDMAN_TAG_EMPTY to
 *                      create a sock with an empty tag list.
 * @param[in] version   [DTLS version](@ref sock_dtls_prot_version) to use.
 * @param[in] role      [Role](@ref sock_dtls_role) of the endpoint.
 *
 * @return  0 on success.
 * @return  -1 on error
 */
int sock_dtls_create(sock_dtls_t *sock, sock_udp_t *udp_sock,
                     credman_tag_t tag, unsigned version, unsigned role);

/**
 * @brief   Get underlying UDP sock.
 *
 * @pre `sock != NULL`.
 *
 * @param[in] sock  DTLS sock to get UDP sock from.
 *
 * @return  The underlying UDP sock.
 */
sock_udp_t *sock_dtls_get_udp_sock(sock_dtls_t *sock);

/**
 * @brief Initialize session handshake.
 *
 * Sends a ClientHello message to initialize the handshake. Call
 * @ref sock_dtls_recv() to finish the handshake.
 *
 * @param[in] sock      DTLS sock to use
 * @param[in] ep        Remote endpoint to start a handshake with
 * @param[out] remote   Resulting session
 *
 * @return 1, if new handshake is started
 * @return 0, if there is an existing session
 * @return -ENOMEM, not enough memory to allocate for new peer
 * @return -EADDRNOTAVAIL, if the local endpoint of @p sock is not set.
 * @return -EINVAL, if @p remote is invalid or @p sock is not properly
 *         initialized (or closed while sock_udp_recv() blocks).
 */
int sock_dtls_session_init(sock_dtls_t *sock, const sock_udp_ep_t *ep,
                           sock_dtls_session_t *remote);

/**
 * @brief Destroys an existing DTLS session
 *
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * @param[in] sock      @ref sock_dtls_t, which the session is created on
 * @param[in] remote    Remote session to destroy
 *
 * @note For tinyDTLS this function destroys the session object right after notifying the remote
 *       peer about the closing. This is an interim solution, preventing endlessly blocked session
 *       slots, but allows as a consequence truncation attacks.
 *       More details in the [issue](https://github.com/eclipse/tinydtls/issues/95).
 */
void sock_dtls_session_destroy(sock_dtls_t *sock, sock_dtls_session_t *remote);

/**
 * @brief Get the remote UDP endpoint from a session.
 *
 * @pre `(session != NULL) && (ep != NULL)`
 *
 * @param[in]  session   DTLS session
 * @param[out] ep        UDP endpoint
 */
void sock_dtls_session_get_udp_ep(const sock_dtls_session_t *session,
                                  sock_udp_ep_t *ep);

/**
 * @brief Set the remote UDP endpoint from a session.
 *
 * @pre `(session != NULL) && (ep != NULL)`
 *
 * @param[in]   session   DTLS session
 * @param[in]   ep        UDP endpoint
 *
 * @note Function should only be needed when doing a blocking handshake with
 *       @ref sock_dtls_send() to set the remote UDP endpoint.
 */
void sock_dtls_session_set_udp_ep(sock_dtls_session_t *session,
                                  const sock_udp_ep_t *ep);

/**
 * @brief Receive handshake messages and application data from remote peer.
 *
 * @param[in]   sock    DTLS sock to use.
 * @param[out]  remote  Remote DTLS session of the received data.
 *                      Cannot be NULL.
 * @param[out]  data    Pointer where the received data should be stored.
 * @param[in]   maxlen  Maximum space available at @p data.
 * @param[in]   timeout Receive timeout in microseconds.
 *                      If 0 and no data is available, the function returns
 *                      immediately.
 *                      May be SOCK_NO_TIMEOUT to wait until data
 *                      is available.
 * @param[out]  aux     Auxiliary data about the received datagram.
 *                      May be `NULL`, if it is not required by the application.
 *
 * @note Function may block if data is not available and @p timeout != 0
 *
 * @return  The number of bytes received on success
 * @return  -SOCK_DTLS_HANDSHAKE when new handshake is completed
 * @return  -EADDRNOTAVAIL, if the local endpoint of @p sock is not set.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_dtls_recv() blocks).
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          data.
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
ssize_t sock_dtls_recv_aux(sock_dtls_t *sock, sock_dtls_session_t *remote,
                           void *data, size_t maxlen, uint32_t timeout,
                           sock_dtls_aux_rx_t *aux);

/**
 * @brief Receive handshake messages and application data from remote peer.
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
 * @return  The number of bytes received on success
 * @return  -SOCK_DTLS_HANDSHAKE when new handshake is completed
 * @return  -EADDRNOTAVAIL, if the local endpoint of @p sock is not set.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_dtls_recv() blocks).
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          data.
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
static inline ssize_t sock_dtls_recv(sock_dtls_t *sock,
                                     sock_dtls_session_t *remote,
                                     void *data, size_t maxlen,
                                     uint32_t timeout)
{
    return sock_dtls_recv_aux(sock, remote, data, maxlen, timeout, NULL);
}

/**
 * @brief Decrypts and provides stack-internal buffer space containing a
 *        message from a remote peer.
 *
 * @param[in] sock      DTLS sock to use.
 * @param[out] remote   Remote DTLS session of the received data.
 *                      Cannot be NULL.
 * @param[out] data     Pointer to a stack-internal buffer space containing the
 *                      received data.
 * @param[in,out] buf_ctx   Stack-internal buffer context. If it points to a
 *                      `NULL` pointer, the stack returns a new buffer space for
 *                      a new packet. If it does not point to a `NULL` pointer,
 *                      an existing context is assumed to get a next segment in
 *                      a buffer.
 * @param[in] timeout   Receive timeout in microseconds.
 *                      If 0 and no data is available, the function returns
 *                      immediately.
 *                      May be SOCK_NO_TIMEOUT to wait until data
 *                      is available.
 * @param[out]  aux     Auxiliary data about the received datagram.
 *                      May be `NULL`, if it is not required by the application.
 *
 * @experimental    This function is quite new, not implemented for all stacks
 *                  yet, and may be subject to sudden API changes. Do not use in
 *                  production if this is unacceptable.
 *
 * @note Function may block if data is not available and @p timeout != 0
 *
 * @note    Function blocks if no packet is currently waiting.
 *
 * @return  The number of bytes received on success. May not be the complete
 *          payload. Continue calling with the returned @p buf_ctx to get more
 *          buffers until result is 0 or an error.
 * @return  0, if no received data is available, but everything is in order.
 *          If @p buf_ctx was provided, it was released.
 * @return  -EADDRNOTAVAIL, if the local endpoint of @p sock is not set.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_dtls_recv() blocks).
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
ssize_t sock_dtls_recv_buf_aux(sock_dtls_t *sock, sock_dtls_session_t *remote,
                               void **data, void **buf_ctx, uint32_t timeout,
                               sock_dtls_aux_rx_t *aux);

/**
 * @brief Decrypts and provides stack-internal buffer space containing a
 *        message from a remote peer.
 *
 * @param[in] sock      DTLS sock to use.
 * @param[out] remote   Remote DTLS session of the received data.
 *                      Cannot be NULL.
 * @param[out] data     Pointer to a stack-internal buffer space containing the
 *                      received data.
 * @param[in,out] buf_ctx   Stack-internal buffer context. If it points to a
 *                      `NULL` pointer, the stack returns a new buffer space for
 *                      a new packet. If it does not point to a `NULL` pointer,
 *                      an existing context is assumed to get a next segment in
 *                      a buffer.
 * @param[in] timeout   Receive timeout in microseconds.
 *                      If 0 and no data is available, the function returns
 *                      immediately.
 *                      May be SOCK_NO_TIMEOUT to wait until data
 *                      is available.
 *
 * @experimental    This function is quite new, not implemented for all stacks
 *                  yet, and may be subject to sudden API changes. Do not use in
 *                  production if this is unacceptable.
 *
 * @note Function may block if data is not available and @p timeout != 0
 *
 * @note    Function blocks if no packet is currently waiting.
 *
 * @return  The number of bytes received on success. May not be the complete
 *          payload. Continue calling with the returned @p buf_ctx to get more
 *          buffers until result is 0 or an error.
 * @return  0, if no received data is available, but everything is in order.
 *          If @p buf_ctx was provided, it was released.
 * @return  -EADDRNOTAVAIL, if the local endpoint of @p sock is not set.
 * @return  -EAGAIN, if @p timeout is `0` and no data is available.
 * @return  -EINVAL, if @p remote is invalid or @p sock is not properly
 *          initialized (or closed while sock_dtls_recv() blocks).
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
static inline ssize_t sock_dtls_recv_buf(sock_dtls_t *sock,
                                         sock_dtls_session_t *remote,
                                         void **data, void **buf_ctx,
                                         uint32_t timeout)
{
    return sock_dtls_recv_buf_aux(sock, remote, data, buf_ctx, timeout, NULL);
}

/**
 * @brief Encrypts and sends a message to a remote peer with non-continuous payload
 *
 * @param[in]   sock    DTLS sock to use
 * @param[in]   remote  DTLS session to use. A new session will be created
 *                      if no session exist between client and server.
 * @param[in]   snips   List of payload chunks, will be processed in order.
 *                      May be `NULL`.
 * @param[in]   timeout Handshake timeout in microseconds.
 *                      If `timeout > 0`, will start a new handshake if no
 *                      session exists yet. The function will block until
 *                      handshake completed or timed out.
 *                      May be SOCK_NO_TIMEOUT to block indefinitely until
 *                      handshake complete.
 * @param[out] aux      Auxiliary data about the transmission.
 *                      May be `NULL`, if it is not required by the application.
 *
 * @note    When blocking, we will need an extra thread to call
 *          @ref sock_dtls_recv() function to handle the incoming handshake
 *          messages.
 *
 * @return The number of bytes sent on success
 * @return  -ENOTCONN, if `timeout == 0` and no existing session exists with
 *          @p remote
 * @return  -EADDRINUSE, if sock_dtls_t::udp_sock has no local end-point.
 * @return  -EAFNOSUPPORT, if `remote->ep != NULL` and
 *          sock_dtls_session_t::ep::family of @p remote is != AF_UNSPEC and
 *          not supported.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote->ep is an
 *          invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::port of @p remote->ep is 0.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ETIMEDOUT, `0 < timeout < SOCK_NO_TIMEOUT` and timed out.
 */
ssize_t sock_dtls_sendv_aux(sock_dtls_t *sock, sock_dtls_session_t *remote,
                            const iolist_t *snips, uint32_t timeout,
                            sock_dtls_aux_tx_t *aux);

/**
 * @brief Encrypts and sends a message to a remote peer
 *
 * @param[in]   sock    DTLS sock to use
 * @param[in]   remote  DTLS session to use. A new session will be created
 *                      if no session exist between client and server.
 * @param[in]   data    Pointer where the data to be send are stored
 * @param[in]   len     Length of @p data to be send
 * @param[in]   timeout Handshake timeout in microseconds.
 *                      If `timeout > 0`, will start a new handshake if no
 *                      session exists yet. The function will block until
 *                      handshake completed or timed out.
 *                      May be SOCK_NO_TIMEOUT to block indefinitely until
 *                      handshake complete.
 * @param[out] aux      Auxiliary data about the transmission.
 *                      May be `NULL`, if it is not required by the application.
 *
 * @note    When blocking, we will need an extra thread to call
 *          @ref sock_dtls_recv() function to handle the incoming handshake
 *          messages.
 *
 * @return The number of bytes sent on success
 * @return  -ENOTCONN, if `timeout == 0` and no existing session exists with
 *          @p remote
 * @return  -EADDRINUSE, if sock_dtls_t::udp_sock has no local end-point.
 * @return  -EAFNOSUPPORT, if `remote->ep != NULL` and
 *          sock_dtls_session_t::ep::family of @p remote is != AF_UNSPEC and
 *          not supported.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote->ep is an
 *          invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::port of @p remote->ep is 0.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ETIMEDOUT, `0 < timeout < SOCK_NO_TIMEOUT` and timed out.
 */
static inline ssize_t sock_dtls_send_aux(sock_dtls_t *sock,
                                         sock_dtls_session_t *remote,
                                         const void *data, size_t len,
                                         uint32_t timeout,
                                         sock_dtls_aux_tx_t *aux)
{
    const iolist_t snip = {
        .iol_base = (void *)data,
        .iol_len  = len,
    };

    return sock_dtls_sendv_aux(sock, remote, &snip, timeout, aux);
}

/**
 * @brief Encrypts and sends a message to a remote peer
 *
 * @param[in] sock      DTLS sock to use
 * @param[in] remote    DTLS session to use. A new session will be created
 *                      if no session exist between client and server.
 * @param[in] data      Pointer where the data to be send are stored
 * @param[in] len       Length of @p data to be send
 * @param[in] timeout   Handshake timeout in microseconds.
 *                      If `timeout > 0`, will start a new handshake if no
 *                      session exists yet. The function will block until
 *                      handshake completed or timed out.
 *                      May be SOCK_NO_TIMEOUT to block indefinitely until
 *                      handshake complete.
 *
 * @note    When blocking, we will need an extra thread to call
 *          @ref sock_dtls_recv() function to handle the incoming handshake
 *          messages.
 *          An example for a blocking handshake is:
 *              1. Create an empty @ref sock_dtls_session_t object.
 *              2. Set the UDP endpoint of the peer you want to connect to in the
 *                 session object with @ref sock_dtls_session_set_udp_ep().
 *              3. Call @ref sock_dtls_send() with a timeout greater than 0.
 *                 The send function blocks until the handshake completes or the
 *                 timeout expires. If the handshake was successful the data has
 *                 been sent.
 *
 * @return The number of bytes sent on success
 * @return  -ENOTCONN, if `timeout == 0` and no existing session exists with
 *          @p remote
 * @return  -EADDRINUSE, if sock_dtls_t::udp_sock has no local end-point.
 * @return  -EAFNOSUPPORT, if `remote->ep != NULL` and
 *          sock_dtls_session_t::ep::family of @p remote is != AF_UNSPEC and
 *          not supported.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote->ep is an
 *          invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::port of @p remote->ep is 0.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ETIMEDOUT, `0 < timeout < SOCK_NO_TIMEOUT` and timed out.
 */
static inline ssize_t sock_dtls_send(sock_dtls_t *sock,
                                     sock_dtls_session_t *remote,
                                     const void *data, size_t len,
                                     uint32_t timeout)
{
    return sock_dtls_send_aux(sock, remote, data, len, timeout, NULL);
}

/**
 * @brief Encrypts and sends a message to a remote peer with non-continuous payload
 *
 * @param[in] sock      DTLS sock to use
 * @param[in] remote    DTLS session to use. A new session will be created
 *                      if no session exist between client and server.
 * @param[in] snips     List of payload chunks, will be processed in order.
 *                      May be `NULL`.
 * @param[in] timeout   Handshake timeout in microseconds.
 *                      If `timeout > 0`, will start a new handshake if no
 *                      session exists yet. The function will block until
 *                      handshake completed or timed out.
 *                      May be SOCK_NO_TIMEOUT to block indefinitely until
 *                      handshake complete.
 *
 * @note    When blocking, we will need an extra thread to call
 *          @ref sock_dtls_recv() function to handle the incoming handshake
 *          messages.
 *          An example for a blocking handshake is:
 *              1. Create an empty @ref sock_dtls_session_t object.
 *              2. Set the UDP endpoint of the peer you want to connect to in the
 *                 session object with @ref sock_dtls_session_set_udp_ep().
 *              3. Call @ref sock_dtls_send() with a timeout greater than 0.
 *                 The send function blocks until the handshake completes or the
 *                 timeout expires. If the handshake was successful the data has
 *                 been sent.
 *
 * @return The number of bytes sent on success
 * @return  -ENOTCONN, if `timeout == 0` and no existing session exists with
 *          @p remote
 * @return  -EADDRINUSE, if sock_dtls_t::udp_sock has no local end-point.
 * @return  -EAFNOSUPPORT, if `remote->ep != NULL` and
 *          sock_dtls_session_t::ep::family of @p remote is != AF_UNSPEC and
 *          not supported.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote->ep is an
 *          invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::port of @p remote->ep is 0.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ETIMEDOUT, `0 < timeout < SOCK_NO_TIMEOUT` and timed out.
 */
static inline ssize_t sock_dtls_sendv(sock_dtls_t *sock,
                                      sock_dtls_session_t *remote,
                                      const iolist_t *snips,
                                      uint32_t timeout)
{
    return sock_dtls_sendv_aux(sock, remote, snips, timeout, NULL);
}

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

#ifdef MODULE_SOCK_DTLS
#include "sock_dtls_types.h"
#endif

#ifdef __cplusplus
}
#endif

/** @} */
