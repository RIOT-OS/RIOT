/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sock_secure  Sock Secure API
 * @ingroup     net_sock
 * @brief      Sock submodule for TLSMAN
 *
 * @{
 *
 * @file
 * @brief       sock secure definition
 *
 * @author      Raul Fuentes <raul.fuentes-samaniego@inria.fr>
 *
 * @}
 */

#ifndef NET_SOCK_SECURE_H
#define NET_SOCK_SECURE_H

#include "net/tlsman.h"

typedef struct sock_secure_session sock_secure_session_t;

/* This is merely a Proof of concept  */
typedef struct tlsman_ep_t sock_secure_ep_t; /**< Endpoint for a sock_secure object (FIXME) */


/* DISCUSSION Duplicate values from TLSMAN ? */
enum {
  SOCK_SECURE_STATE_NON_STARTED = TLSMAN_ERROR_NON_STARTED, /** Resources assigned, non secure session started yet */
  SOCK_SECURE_STATE_FATAL, /** It's impossible to create a secure channel */
  SOCK_SECURE_STATE_NON_INITIALIZED, /** There are not resources asigned for making a new secure channel */
  SOCK_SECURE_STATE_STARTED, /** The (D)TLS handshake already started (current status is unknown) */
  SOCK_SECURE_STATE_ERROR, /** Secure channel not established but is possible to attempt again */
  SOCK_SECURE_STATE_SERVER_LISTENING /** The behavior for a server side is different from a client */
};

struct sock_secure_session {
  uint8_t flag;
  uint8_t state;
  tlsman_driver_t session; /* DISCUSSION: Should be "tlsman"? (to avoid confusion with the "session" term) */
  tlsman_resp_handler_t cb;

};

/**
 * Loads the (D)TLS stack and it(s) cipher suite(s). Must be called once time
 * TODO FIXME Break into two part (load stack  which only require flags and cipher suites)
 * and the rest must be integrated into sock_secure_initialized()
 */
ssize_t sock_secure_load_stack(sock_secure_session_t *sock_secure,
                             uint16_t *ciphers, size_t ciphers_size);

/* Preapare the resources required for a new secure session   */
ssize_t sock_secure_initialized(sock_secure_session_t *sock_secure,
                              tlsman_resp_handler_t callback,
                              void *sock,
                              sock_secure_ep_t *local,
                              sock_secure_ep_t *remote);



/* Updates the status of the (D)TLS session (the secure channel between local
 * and remote).  */
ssize_t sock_secure_update(sock_secure_session_t *sock_secure);

/* Executes a step of the (D)TLS handshake (by processing the last DTLS record
 * or by sending the next one) between the local and remote members of the sock */
// sock_secure_handshake()


/* Send data by means of the secure session */
ssize_t sock_secure_write(sock_secure_session_t *sock_secure,
                          const void* data, size_t len);


/* Check if data was received by means of the secure session */
ssize_t sock_secure_read(sock_secure_session_t *sock_secure);


/* Relase the resources assigned for a secure session (and ends any current active session) */
void sock_secure_release(sock_secure_session_t *sock_secure);

#endif /* NET_SOCK_SECURE_H */
