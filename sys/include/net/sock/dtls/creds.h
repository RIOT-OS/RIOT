/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sock_dtls_creds    DTLS sock credentials API
 * @ingroup     net_sock_dtls
 * @brief       Credential handling for DTLS sock
 * @{
 *
 * @file
 * @brief   DTLS sock definitions
 *
 * @author  Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef NET_SOCK_DTLS_CREDS_H
#define NET_SOCK_DTLS_CREDS_H

#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup  net_sock_dtls_conf
 * @{
 */
/**
 * @brief   Default maximum size for the PSK Identity hint string
 */
#ifndef CONFIG_DTLS_PSK_ID_HINT_MAX_SIZE
#define CONFIG_DTLS_PSK_ID_HINT_MAX_SIZE    32
#endif
/** @} */

/**
 * @brief Sets the PSK Identity hint to be sent to clients during handshake.
 *
 * This hint is optional. It helps clients to decide which PSK Identity to use.
 *
 * @param[in, out] sock The DTLS sock object to set the hint to.
 * @param[in] hint      PSK Identity hint as NULL-terminated string.
 *
 * @retval 0 on success
 * @retval -1 on error
 */
int sock_dtls_set_server_psk_id_hint(sock_dtls_t *sock, const char *hint);

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_DTLS_CREDS_H */
/** @} */
