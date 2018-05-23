/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tlsman
 * @{
 *
 * @file
 * @brief       tlsman utility functions implementation
 *
 * @author      Raul Fuentes <raul.fuentes-samaniego@inria.fr>
 *
 * @}
 */

#include "net/tlsman.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Defined in the net/tlsman/[a-z]*.h library */
extern ssize_t load_stack(uint16_t *ciphersuites_list, size_t total, uint8_t flags);
extern ssize_t init_context(tlsman_session_t *session, tlsman_resp_handler_t resp_cb,
                     const uint8_t flags);
extern ssize_t connect(tlsman_session_t *session, uint8_t flags,
                uint8_t *packet_buff, size_t packet_size);

extern bool is_channel_ready(tlsman_session_t *session);

extern ssize_t send_data_app(tlsman_session_t *session, const void *data,
                                  size_t len);

extern ssize_t retrieve_data_app(tlsman_session_t *session, void *data, size_t max_len);
extern void release_resources(tlsman_session_t *dtls_session);
extern ssize_t close_channel(tlsman_session_t *session);

extern ssize_t renegotiate(tlsman_session_t *session, uint8_t flags,
                                   uint8_t *pkt_buff, size_t pkt_size);

extern ssize_t rehandshake(tlsman_session_t *session, uint8_t flags,
                             uint8_t *pkt_buff, size_t pkt_size);
extern ssize_t listening(tlsman_session_t *session, uint8_t flags,
                         uint8_t *pkt_buff, size_t pkt_size);

#ifdef SOCK_HAS_ASYNC
extern void set_async_parameters(tlsman_session_t *session,
                                 event_queue_t *sock_event_queue,
                                 uint8_t *buffer, ssize_t size);
extern void set_async_listening(void);
#endif /* SOCK_HAS_ASYNC */

ssize_t tlsman_load_stack(tlsman_driver_t *session,
                                   uint16_t *ciphersuites_list, size_t total,
                                   uint8_t flags)
{

    session->tlsman_load_stack = load_stack;
    session->tlsman_init_context = init_context;
    session->tlsman_create_channel = connect;
#ifdef SOCK_HAS_ASYNC
    session->tlsman_set_async_parameters = set_async_parameters;
    session->tlsman_set_async_listening = set_async_listening;
#endif /* SOCK_HAS_ASYNC */
    session->tlsman_is_channel_ready = is_channel_ready;
    session->tlsman_send_data_app = send_data_app;
    session->tlsman_retrieve_data_app = retrieve_data_app;
    session->tlsman_release_resources = release_resources;
    session->tlsman_close_channel = close_channel;
    session->tlsman_listening = listening;
    session->tlsman_renegotiate = renegotiate;
    session->tlsman_rehandshake = rehandshake;

    /* Loads the (D)TLS stack */
    return session->tlsman_load_stack(ciphersuites_list, total, flags);
}

ssize_t tlsman_return_last_known_error(tlsman_driver_t *tlsman_session)
{
    DEBUG("%s: Last known error code: %i\n", __func__,
                            tlsman_session->session.last_known_error);
    if (tlsman_session->session.last_known_error != TLSMAN_ERROR_NON_STARTED) {
        return tlsman_session->session.last_known_error;
    }

    return 0;
}

bool tlsman_process_is_error_code_nonfatal(ssize_t error_id)
{
    switch (error_id) {
        case TLSMAN_ERROR_FATAL:
        case TLSMAN_ERROR_UNABLE_CONTEXT:
        case TLSMAN_ERROR_TRANSPORT_NOT_SUPPORTED: /* NOTE: Or isn't? */
        case TLSMAN_ERROR_CIPHER_NOT_SUPPORTED:
            return true;
    }

    return false;
}
