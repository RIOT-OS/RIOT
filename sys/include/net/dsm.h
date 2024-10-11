/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_dsm DTLS Session Management (DSM)
 * @ingroup     net net_dtls
 * @brief       This module provides functionality to store and retrieve session
 *              information of DTLS connections.
 *
 * dsm allows to store necessary session information so that not every application
 * has to provide the potentially maximum number of possible session objects.
 * Session storage can be offloaded to this generic module.
 *
 * @{\
 *
 * @file
 * @brief       DTLS session management module definition
 *
 * @note        This module does not accept or close DTLS sessions, it merely
 *              provides a place to store session objects.
 *
 * @author      János Brodbeck <janos.brodbeck@ml-pa.com>
 */

#ifndef NET_DSM_H
#define NET_DSM_H

#include <stdint.h>

#include "dtls.h"
#include "net/sock/dtls.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of maintained DTLS sessions (tinyDTLS)
 */
#ifndef CONFIG_DSM_PEER_MAX
#define CONFIG_DSM_PEER_MAX   (CONFIG_DTLS_PEER_MAX)
#endif

/**
 * @brief Session management states
 */
typedef enum {
    NO_SPACE = -1,
    SESSION_STATE_NONE = 0,
    SESSION_STATE_HANDSHAKE,
    SESSION_STATE_ESTABLISHED
} dsm_state_t;

/**
 * @brief   Initialize the DTLS session management
 *
 * Must call once before first use.
 */
void dsm_init(void);

/**
 * @brief   Stores a session
 *
 * Stores a given session in the internal storage of the session management.
 * If the session is already stored only the state will be updated when the session
 * gets established.
 *
 * @param[in]   sock        @ref sock_dtls_t, which the session is created on
 * @param[in]   session     Session to store
 * @param[in]   new_state   New state of the session
 * @param[in]   restore     Indicates, whether the session object should be restored
 *                          when an already established session is found
 *
 * @return Previous state of the session. If no session existed before it returns
 *         SESSION_STATE_NONE. If no space is available it returns NO_SPACE.
 */
dsm_state_t dsm_store(sock_dtls_t *sock, sock_dtls_session_t *session,
                      dsm_state_t new_state, bool restore);

/**
 * @brief   Removes a session
 *
 * Removes a given session in the internal storage of the session management.
 *
 * @param[in]  sock         @ref sock_dtls_t, which the session is created on
 * @param[in]  session      Session to store
 */
void dsm_remove(sock_dtls_t *sock, sock_dtls_session_t *session);

/**
 * @brief   Returns the maximum number of sessions slots
 *
 * @return  Number of session slots.
 */
uint8_t dsm_get_num_maximum_slots(void);

/**
 * @brief   Returns the number of available session slots
 *
 * @return  Number of available session slots in the session management.
 */
uint8_t dsm_get_num_available_slots(void);

/**
 * @brief   Returns the least recently used session
 *
 * @param[in]   sock        @ref sock_dtls_t, which the session is created on
 * @param[out]  session     Oldest used session
 *
 * @return   1, on success
 * @return   -1, when no session is stored
 */
ssize_t dsm_get_least_recently_used_session(sock_dtls_t *sock, sock_dtls_session_t *session);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_DSM_H */
