/*
 * Copyright (C) 2019 Daniele Lacamera
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/* @defgroup    module sock_tls
 * @ingroup     pkg_wolfssl
 * @brief       Sock submodule for TLS/DTLS sessions
 *
 * How To Use
 * ----------
 * First you need to [include][include-link]
 * a module that implements this API in your application's Makefile.
 *
 * The `sock_tls` module requires the `wolfssl` package.
 *
 * The application's Makefile should at lease contain the following:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {Makefile}
 *
 * USEPKG += wolfssl
 * USEMODULE+=sock_tls
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * ### A Simple DTLS Server
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <wolfssl/ssl.h>
 * #include <sock_tls.h>
 *
 * #include <stdio.h>
 * #include <inttypes.h>
 *
 * #include <net/sock/udp.h>
 *
 * #include <stdio.h>
 * #include <stdlib.h>
 * #include <string.h>
 *
 * #define SERVER_PORT 11111
 * #define DEBUG 1
 * extern const unsigned char server_cert[788];
 * extern const unsigned char server_key[121];
 * extern unsigned int server_cert_len;
 * extern unsigned int server_key_len;
 *
 * static sock_tls_t skv;
 * static sock_tls_t *sk = &skv;
 * static const char Test_dtls_string[] = "DTLS OK!";
 *
 * int main(void)
 * {
 *     char buf[64];
 *     int ret;
 *     sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 *     local.port = SERVER_PORT;
 *
 *     if (sock_dtls_create(sk, &local, NULL, 0, wolfDTLSv1_2_server_method()) != 0) {
 *         printf("Failed to create DTLS socket context\r\n");
 *         return -1;
 *     }
 *     if (wolfSSL_CTX_use_certificate_buffer(sk->ctx, server_cert,
 *                 server_cert_len, SSL_FILETYPE_ASN1 ) != SSL_SUCCESS)
 *     {
 *         printf("Failed to load certificate from memory.\r\n");
 *         return -1;
 *     }
 *
 *     if (wolfSSL_CTX_use_PrivateKey_buffer(sk->ctx, server_key,
 *                 server_key_len, SSL_FILETYPE_ASN1 ) != SSL_SUCCESS)
 *     {
 *         printf("Failed to load private key from memory.\r\n");
 *         return -1;
 *     }
 *     ret = sock_dtls_session_create(sk);
 *     if (ret < 0)
 *     {
 *         printf("Failed to create DTLS session (err: %s)\r\n", strerror(-ret));
 *         return -1;
 *     }
 *     printf("Listening on %d\n", SERVER_PORT);
 *     while(1) {
 *         ret = wolfSSL_accept(sk->ssl);
 *         if (ret != SSL_SUCCESS) {
 *             continue;
 *         }
 *         printf("Connection accepted\r\n");
 *         ret = wolfSSL_read(sk->ssl, buf, 64);
 *         if (ret > 0) {
 *             buf[ret] = (char)0;
 *             printf("Received '%s'\r\n", buf);
 *         }
 *         printf("Sending 'DTLS OK'...\r\n");
 *         wolfSSL_write(sk->ssl, Test_dtls_string, sizeof(Test_dtls_string));
 *         printf("Closing connection.\r\n");
 *         sock_dtls_session_destroy(sk);
 *         sock_dtls_close(sk);
 *         break;
 *     }
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Above you see a simple DTLS echo server. It is important to at least
 * [include][include-link] the IPv6 module of your networking
 * implementation (e.g. `gnrc_ipv6_default` for @ref net_gnrc GNRC) and at least
 * one network device.
 * A separate file should define the buffers used as certificate and private key,
 * in the variables `server_cert`, `private_key` respectively.
 *
 * After including all the needed header files, we use a global object to store
 * the context for incoming DTLS communication. The object contains the reference
 * to the wolfSSL context, the SSL session and the underlying transport socket.
 *
 * For simplicity, we will refer to the address of the object in the static memory,
 * through the pointer `sk`.
 *
 * A constant test string is used later as a reply to incoming connections.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * static sock_tls_t skv;
 * static sock_tls_t *sk = &skv;
 *
 * static const char Test_dtls_string[] = "DTLS OK!";
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * In the same way as a normal @ref net_sock_udp "UDP socket", in order to be able to
 * listen for incoming packets, we bind the `sock` by setting a local endpoint with
 * a port (`11111` in this case).
 *
 * We then proceed to create the `sock`. It is bound to `local` and thus listens
 * for UDP packets with @ref udp_hdr_t::dst_port "destination port" `12345`.
 * Since we don't need any further configuration we set the flags to 0.
 * The method argument determines which kind of wolfSSL context is associated to
 * the socket.
*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
 * local.port = SERVER_PORT;
 *
 *  if (sock_dtls_create(sk, &local, NULL, 0, wolfDTLSv1_2_server_method()) != 0) {
 *      printf("ERROR: Unable to create DTLS sock\r\n");
 *      return -1;
 *  }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * By default, all sock_tls operations in a DTLS context are blocking for a
 * limited amount of time, which depends on the DTLS session timeout. To modify
 * the timeout, use `wolfSSL_dtls_set_timeout_init(sk->ssl)`.
 *
 * Certificate and private key for the server context are loaded from a previously
 * initialized section in memory:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * if (wolfSSL_CTX_use_certificate_buffer(sk->ctx, server_cert,
 *             server_cert_len, SSL_FILETYPE_ASN1 ) != SSL_SUCCESS)
 * {
 *     printf("Failed to load certificate from memory.\r\n");
 *     return -1;
 * }
 *
 * if (wolfSSL_CTX_use_PrivateKey_buffer(sk->ctx, server_key,
 *             server_key_len, SSL_FILETYPE_ASN1 ) != SSL_SUCCESS)
 * {
 *     printf("Failed to load private key from memory.\r\n");
 *     return -1;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Once the context is configured, the SSL session can be initialized.
 *
 * The listening sock automatically takes care of the DTLS handshake.
 * When the session is established, `wolfSSL_accept()` will finally return
 * `SSL_SUCCESS`.
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * ret = sock_dtls_session_create(sk);
 * if (ret < 0)
 * {
 *     printf("Failed to create DTLS session (err: %s)\r\n", strerror(-ret));
 *     return -1;
 * }
 * printf("Listening on %d\n", SERVER_PORT);
 * while(1) {
 *     ret = wolfSSL_accept(sk->ssl);
 *     if (ret != SSL_SUCCESS) {
 *         continue;
 *     }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * At this point, the session is established, and encrypted data can be exchanged
 * using `wolfSSL_read()` and `wolfSSL_write()`:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *    ret = wolfSSL_read(sk->ssl, buf, 64);
 *    if (ret > 0) {
 *        buf[ret] = (char)0;
 *        printf("Received '%s'\r\n", buf);
 *    }
 *    printf("Sending 'DTLS OK'...\r\n");
 *    wolfSSL_write(sk->ssl, Test_dtls_string, sizeof(Test_dtls_string));
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The session is terminated, and the associated socket is closed.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 *         sock_dtls_session_destroy(sk);
 *         sock_dtls_close(sk);
 *         break;
 *     }
 *     return 0;
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * [include-link]: https://guide.riot-os.org/advanced_tutorials/creating_application/#including-modules
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <net/sock.h>
#include <wolfssl/ssl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Creates a new DTLS sock object.
 *
 * @pre `(sock != NULL)`
 * @pre `(remote == NULL) || (remote->port != 0)`
 *
 * @param[out] sock     The resulting sock object.
 * @param[in] local     Local end point for the sock object.
 *                      May be NULL.
 *                      sock_udp_ep_t::netif must either be
 *                      @ref SOCK_ADDR_ANY_NETIF or equal to
 *                      sock_udp_ep_t::netif of @p remote if `remote != NULL`.
 *                      If NULL @ref sock_udp_send() may bind implicitly.
 *                      sock_udp_ep_t::port may also be 0 to bind the `sock` to
 *                      an ephemeral port.
 * @param[in] remote    Remote end point for the sock object.
 *                      May be `NULL` but then the `remote` parameter of
 *                      @ref sock_udp_send() may not be `NULL` or it will
 *                      always error with return value -ENOTCONN.
 *                      sock_udp_ep_t::port must not be 0 if `remote != NULL`.
 *                      sock_udp_ep_t::netif must either be
 *                      @ref SOCK_ADDR_ANY_NETIF or equal to sock_udp_ep_t::netif
 *                      of @p local if `local != NULL`.
 * @param[in] flags     Flags for the sock object. See also
 *                      [sock flags](@ref net_sock_flags).
 *                      May be 0.
 * @param[in] method    Defines the SSL or TLS protocol for the client or server to use.
 *                      There are several options for selecting the desired protocol.
 *                      wolfSSL currently supports DTLS 1.0, and DTLS 1.2. Each of these
 *                      protocols have a corresponding function that can be used as last
 *                      argument to `sock_dtls_create`. The possible client and server
 *                      protocol options are: `wolfDTLSv1_client_method()`, `wolfDTLSv1_server_method()`,
 *                      `wolfDTLSv1_2_client_method()` and `wolfDTLSv1_server_method`.
 *
 * @return  0 on success.
 * @return  -EADDRINUSE, if `local != NULL` and @p local is already used
 *          elsewhere or if `local->port == 0` but the pool of ephemeral ports
 *          is depleted
 * @return  -EAFNOSUPPORT, if `local != NULL` or `remote != NULL` and
 *          sock_udp_ep_t::family of @p local or @p remote is not supported.
 * @return  -EINVAL, if sock_udp_ep_t::addr of @p remote is an invalid address.
 * @return  -EINVAL, if sock_udp_ep_t::netif of @p local or @p remote are not a
 *          valid interfaces or contradict each other (i.e.
 *          `(local->netif != remote->netif) &&
 *          ((local->netif != SOCK_ADDR_ANY_NETIF) ||
 *          (remote->netif != SOCK_ADDR_ANY_NETIF))` if neither is `NULL`).
 * @return  -ENOMEM, if not enough resources can be provided for `sock` to be
 *          created.
 */
int sock_dtls_create(sock_tls_t *sock, const sock_udp_ep_t *local, const sock_udp_ep_t *remote, uint16_t flags, WOLFSSL_METHOD *method);

/**
 * @brief   Sets the endpoint address for the remote DTLS peer associated to this sock object
 *
 * @pre `(sk != NULL)`
 * @pre `(addr != NULL)`
 *
 * @param[in] sk        The resulting sock object.
 * @param[in] addr      Remote end point for the DTLS session.
 *
 */
void sock_dtls_set_endpoint(sock_tls_t *sk, const sock_udp_ep_t *addr);

/**
 * @brief   Creates a new DTLS session from an existing `sock_tls_t` object.
 *
 * @pre `(sk != NULL)`
 *
 * @param[in] sk      The sock object previously created using @ref sock_dtls_create
 *
 * @return  0 on success.
 * @return  -EINVAL, if @sock is null or the SSL context is not initialized yet.
 * @return  -ENOMEM, if not enough resources can be provided for the session to be
 *          created
 */
int sock_dtls_session_create(sock_tls_t *sk);

/**
 * @brief   Destroys the DTLS session associated to the sock object.
 *
 * @pre `(sk != NULL)`
 *
 * @param[in] sk      The sock object containing the session to destroy.
 *
 */
void sock_dtls_session_destroy(sock_tls_t *sk);

/**
 * @brief   Closes the socket connection associated to the DTLS session.
 *
 * @pre `(sk != NULL)`
 *
 * @param[in] sk      The sock object containing the UDP socket to close.
 *
 */
void sock_dtls_close(sock_tls_t *sk);

#ifdef MODULE_SOCK_TCP
#   error Only support for UDP/IP provided via GNRC stack.
#endif

#ifdef __cplusplus
}
#endif
