/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_SOCK_DTLS_CREDS_H
#define NET_SOCK_DTLS_CREDS_H

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

/**
 * @brief   Default buffer size for TLS credential tags
 */
#ifndef CONFIG_DTLS_CREDENTIALS_MAX
#define CONFIG_DTLS_CREDENTIALS_MAX     4
#endif
/** @} */

/**
 * @brief Pre-Shared Key client callback. Called during handshake to determine session credential.
 *
 * @param[in] sock      DTLS sock object
 * @param[in] ep        Remove UDP endpoint representing the session
 * @param[in] tags      List of credential tags available for @p sock
 * @param[in] tags_len  Number of credentials in @p tags
 * @param[in] hint      Hint sent by the server. May be NULL
 * @param[in] hint_len  Length of @p hint
 *
 * @return Tag of the credential to use when a suitable one is found
 * @retval CREDMAN_TAG_EMPTY otherwise
 */
typedef credman_tag_t (*sock_dtls_client_psk_cb_t)(sock_dtls_t *sock, sock_udp_ep_t *ep,
                                                   credman_tag_t tags[], unsigned tags_len,
                                                   const char* hint, size_t hint_len);

/**
 * @brief Raw Public Key callback. Called during handshake to determine the session credential.
 *
 * @param[in] sock      DTLS sock object
 * @param[in] ep        Remote UDP endpoint representing the session
 * @param[in] tags      List of credential tags available for @p sock
 * @param[in] tags_len  Number of credentials in @p tags
 *
 * @return Tag of the credential to use when a suitable one is found
 * @retval CREDMAN_TAG_EMPTY otherwise
 */
typedef credman_tag_t (*sock_dtls_rpk_cb_t)(sock_dtls_t *sock, sock_udp_ep_t *ep,
                                            credman_tag_t tags[], unsigned tags_len);

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

/**
 * @brief Adds a credential tag to list of available credentials for @p sock.
 *
 * @pre sock != NULL
 *
 * @param[in] sock      DTLS sock object
 * @param[in] tag       Tag of the credential to add
 *
 * @retval 0 on success
 * @retval -1 otherwise
 */
int sock_dtls_add_credential(sock_dtls_t *sock, credman_tag_t tag);

/**
 * @brief Removes a credential tag of the list of available credentials for @p sock.
 *
 * @pre sock != NULL
 *
 * @param[in] sock      DTLS sock object
 * @param[in] tag       Tag of the credential to remove
 *
 * @retval 0 on success
 * @retval -1 otherwise
 */
int sock_dtls_remove_credential(sock_dtls_t *sock, credman_tag_t tag);

/**
 * @brief Returns an array of tags of the registered credentials in @p sock.
 *
 * @param[in]  sock      DTLS sock object
 * @param[out] out       Pointer to place the reference to a read-only array of @ref credman_tag_t
 *
 * @return Number of registered credentials
 */
size_t sock_dtls_get_credentials(sock_dtls_t *sock, const credman_tag_t **out);

/**
 * @brief Sets the callback function for clients to specify a credential to use
 *        for a given connection.
 *
 * @param[in] sock      The DTLS sock object to set the callback to.
 * @param[in] cb        Callback to set.
 */
void sock_dtls_set_client_psk_cb(sock_dtls_t *sock, sock_dtls_client_psk_cb_t cb);

/**
 * @brief Sets the callback function to specify a credential to use for a given connection,
 *        when using Raw Public Keys.
 *
 * @param[in] sock      The DTLS sock object to set the callback to.
 * @param[in] cb        Callback to set.
 */
void sock_dtls_set_rpk_cb(sock_dtls_t *sock, sock_dtls_rpk_cb_t cb);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_SOCK_DTLS_CREDS_H */
