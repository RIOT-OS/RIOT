/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_sock_dodtls     DNS over DTLS sock API
 * @ingroup     net_sock
 *
 * @brief       Sock DNS over DTLS client
 *
 * @see         [RFC 8094](https://datatracker.ietf.org/doc/html/rfc8094)
 *
 * @experimental    This implementation is in an experimental state.
 *                  RFC 8094 requires DNS over TLS (DoT) as a fall-back for the
 *                  [PMTU issues](https://datatracker.ietf.org/doc/html/rfc8094#section-5)).
 *                  This fallback is not in place in this implementation.
 *                  Consequently, [EDNS(0)](https://datatracker.ietf.org/doc/html/rfc6891)
 *                  to negotiate maximum response size is also not in place.
 *
 * @{
 *
 * @file
 * @brief   DNS over DTLS sock definitions
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */


#include "net/sock/dtls.h"
#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name DNS over DTLS defines
 * @{
 */
#define SOCK_DODTLS_PORT                    (853)   /**< Default DNS over DTLS server port */

/**
 * @defgroup    net_sock_dodtls_conf DNS over DTLS compile-time configuration
 * @ingroup     config
 * @{
 */
#ifndef CONFIG_SOCK_DODTLS_RETRIES
#define CONFIG_SOCK_DODTLS_RETRIES          (2)     /**< Number of DNS over DTLS query retries */
#endif

/**
 * @brief   Timeout for DNS over DTLS queries in milliseconds
 */
#ifndef CONFIG_SOCK_DODTLS_TIMEOUT_MS
#define CONFIG_SOCK_DODTLS_TIMEOUT_MS       (1000U)
#endif
/** @} */

/**
 * @brief   Maximum name length for a DNS over DTLS query
 */
#define SOCK_DODTLS_MAX_NAME_LEN            (CONFIG_DNS_MSG_LEN - sizeof(dns_hdr_t) - 4)
/** @} */

/**
 * @brief Get IP address for DNS name
 *
 * This function will synchronously try to resolve a DNS A or AAAA record by contacting
 * the DNS server specified in the global variable @ref sock_dns_server.
 *
 * By supplying AF_INET, AF_INET6 or AF_UNSPEC in @p family requesting of A
 * records (IPv4), AAAA records (IPv6) or both can be selected.
 *
 * This function will return the first DNS record it receives. IF both A and
 * AAAA are requested, AAAA will be preferred.
 *
 * @note @p addr_out needs to provide space for any possible result!
 *       (4byte when family==AF_INET, 16byte otherwise)
 *
 * @param[in]   domain_name     DNS name to resolve into address
 * @param[out]  addr_out        buffer to write result into
 * @param[in]   family          Either AF_INET, AF_INET6 or AF_UNSPEC
 *
 * @return      the size of the resolved address on success
 * @return      -ECONNREFUSED, when a DNS over DTLS server  is not configured.
 * @return      -ENOSPC, when the length of @p domain_name is greater than @ref
 *              SOCK_DODTLS_MAX_NAME_LEN.
 * @return      -EBADSG, when the DNS reply is not parseable.
 */
int sock_dodtls_query(const char *domain_name, void *addr_out, int family);

/**
 * @brief       Get currently configured DNS over DTLS server endpoint
 *
 * @param[out] server   The currently configured DNS over DTLS server endpoint.
 *                      May not be NULL on input.
 *
 * @return  0 if @p server was set.
 * @return  -ENOTCONN, when currently no server is configured.
 */
int sock_dodtls_get_server(sock_udp_ep_t *server);

/**
 * @brief   Return the sock used by the DNS over DTLS client
 *
 * @return  The sock used by the DNS over DTLS client
 */
sock_dtls_t *sock_dodtls_get_dtls_sock(void);

/**
 * @brief   Return the DTLS ssession used by the DNS over DTLS client
 *
 * @return  The DTLS session used by the DNS over DTLS client
 */
sock_dtls_session_t *sock_dodtls_get_server_session(void);

/**
 * @brief       Configure and establish session with DNS over DTLS server
 *
 * @param[in] server    A DNS over DTLS server endpoint. May be NULL to
 *                      destroy the session with and unset the currently
 *                      configured server.
 * @param[in] creds     DTLS credentials for the server (see @ref net_credman).
 *                      May be NULL, when @p server is also NULL.
 *
 * @return  0 on success.
 * @return  -EINVAL, if @p cred contains invalid values.
 * @return  -ENOSPC, if @p cred does not fit into @ref net_credman.
 * @return  Any other negative errno potentially returned by @ref sock_udp_create(),
 *          @ref sock_dtls_create(), @ref sock_dtls_session_init() or
 *          @ref sock_dtls_recv().
 */
int sock_dodtls_set_server(const sock_udp_ep_t *server,
                           const credman_credential_t *creds);

#ifdef __cplusplus
}
#endif

/** @} */
