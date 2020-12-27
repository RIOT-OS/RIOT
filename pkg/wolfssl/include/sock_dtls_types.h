
/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief   WolfSSL-specific types and functions definitions
 *
 * @author  Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#ifndef SOCK_DTLS_TYPES_H
#define SOCK_DTLS_TYPES_H

#include "net/sock/udp.h"
#include "net/credman.h"
#ifdef SOCK_HAS_ASYNC
#include "net/sock/async/types.h"
#endif

#include <wolfssl/ssl.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sock_dtls {
    sock_udp_t *udp_sock;           /**< Underlying UDP sock to use */
    credman_tag_t tag;              /**< Credential tag of a registered
                                         (D)TLS credential */
    WOLFSSL_CTX *ctx;               /**< WOLFSSL_CTX struct object */
};

struct sock_dtls_session {
    WOLFSSL *ssl;                   /**< WOLFSSL struct object */
    sock_dtls_t *dtls_sock;         /**< DTLS sock of the session */
    sock_udp_ep_t ep;               /**< Remote endpoint */
    uint32_t timeout;               /**< Timeout for send/recv in usec */
    sock_dtls_aux_rx_t *rx_aux;     /**< Pointer to RX auxiliarry struct */
    sock_dtls_aux_tx_t *tx_aux;     /**< Pointer to TX auxiliarry struct */
};

#ifdef __cplusplus
}
#endif

#endif /* SOCK_DTLS_TYPES_H */
/** @} */
