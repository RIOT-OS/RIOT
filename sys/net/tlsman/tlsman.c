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

#ifdef MODULE_TLSMAN_TINYDTLS
#include "net/tlsman/tinydtls.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 *  @brieff Last known error message.
 */
static ssize_t _tlsman_last_known_error = TLSMAN_ERROR_NON_STARTED;

ssize_t tlsman_load_stack(int *ciphersuites_list, size_t total, uint8_t flags)
{

#ifdef MODULE_TLSMAN_TINYDTLS

    bool match = false;

    if (!(flags & TLSMAN_FLAG_STACK_DTLS)) {
        DEBUG("TinyDTLS only support UDP!\n");
        _tlsman_last_known_error = TLSMAN_ERROR_TRANSPORT_NOT_SUPPORTED;
        return _tlsman_last_known_error;
    }
    /* NOTE: tinyDTLS can only select cipher suites at the compilation time */
    size_t aux;
    for (aux = 0; aux <  total; aux++) {
        if (is_cipher_tinydtls(ciphersuites_list[aux])) {
            match = true;
            break;
        }
    }

    if (!match) {
        DEBUG("TinyDTLS Cipher suite not supported!\n");
        _tlsman_last_known_error = TLSMAN_ERROR_CIPHER_NOT_SUPPORTED;
        return _tlsman_last_known_error;
    }

    dtls_init(); /*TinyDTLS mandatory starting settings*/

#ifdef TINYDTLS_LOG_LVL
    dtls_set_log_level(TINYDTLS_LOG_LVL);
#endif

    /* TODO Upgrade to avoid DTLS_PSK and DTLS_ECC */
#ifdef DTLS_PSK
    DEBUG("tinyDTLS compiled with PSK\n");
#endif
#ifdef DTLS_ECC
    DEBUG("tinyDTLS compiled with ECC\n");
#endif

    return 0;

#endif /* MODULE_TLSMAN_TINYDTLS */

#if MODULE_TLSMAN_WOLFSSL

    /* TODO */
    _tlsman_last_known_error = TLSMAN_ERROR_FATAL;
    return _tlsman_last_known_error;

#endif

    return 0;

}

ssize_t tlsman_return_last_known_error(void)
{
    if (_tlsman_last_known_error != TLSMAN_ERROR_NON_STARTED) {
        return _tlsman_last_known_error;
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

ssize_t tlsman_init_context(tlsman_ep_t *local, tlsman_ep_t *remote,
                            tlsman_session_t *session, void *sock,
                            tlsman_resp_handler_t cb, const uint8_t flags)
{

#ifdef MODULE_TLSMAN_TINYDTLS

    /*
     * FIXME DISCUSSION: A more elegant solution need to be used!
     *
     * We are dereferencing a structure which the first time should be
     * pointing to null.
     *
     * This also will change if tinydtls_session_t is modified.
     * Inclunding compilation with PSK or ECC
     *
     */
    /*
       if ((session->tinydtls_context.context != (void *)0x1) &&
        (session->tinydtls_context.context != NULL)){
        DEBUG("tinyDTLS: DTLS context already exist!\n");
        return 0;
       }
     */

#ifdef WITH_RIOT_GNRC
    assert(local);
    assert(remote);
    assert(session);
    DEBUG("tinyDTLS: Creating context (sock)\n");

    session->tinydtls_context.local = local;
    session->tinydtls_context.remote = remote;
    session->tinydtls_context.sock = sock;
    session->tinydtls_context.context = NULL;
    session->tinydtls_context.cache = NULL;
#endif /* WITH_RIOT_GNRC */

#ifdef WITH_RIOT_SOCKETS
    (void) local;
    (void) remote;

    assert(sock);
    DEBUG("tinyDTLS: Creating context (socket)\n")
#warning "tinyDTLS socket struct not defined yet"

#endif /* WITH_RIOT_SOCKETS */

    return tlsman_tinydtls_init_context(&session->tinydtls_context, cb, flags);

#endif /* MODULE_TLSMAN_TINYDTLS */

#ifdef MODULE_TLSMAN_WOLFSSL
#warning "WolfSSL not supported yet"
#endif /* MODULE_WOLFSSL */

    return 0;
}

ssize_t tlsman_create_channel(tlsman_session_t *session, uint8_t flags,
                              uint8_t *packet_buff, size_t packet_size)
{

    DEBUG("%s: Starting (D)TLS Channel\n", __func__);

    ssize_t res = 0;
#ifdef MODULE_TLSMAN_TINYDTLS

    res = tlsman_tinydtls_connect(&session->tinydtls_context, packet_buff,
                                  packet_size, flags);
#endif

#ifdef MODULE_TLSMAN_WOLFSSL
#warning "WolfSSL not supported yet"
#endif /* MODULE_WOLFSSL */

    if (res == TLSMAN_ERROR_HANDSHAKE_TIMEOUT) {
        _tlsman_last_known_error = res;
    }

    return res;
}

bool tlsman_is_channel_ready(tlsman_session_t *session)
{

    /* NOTE DISCUSSION: Add a loop here? */

#ifdef MODULE_TLSMAN_TINYDTLS
    if (session->tinydtls_context.is_connected == TINYDTLS_EVENT_FINISHED) {
        return true;
    }
#endif /* MODULE_TLSMAN_TINYDTLS */

#ifdef MODULE_WOLFSSL
#warning "Not tested"
#endif /* MODULE_WOLFSSL */

    return false;
}

ssize_t tlsman_send_data_app(tlsman_session_t *session, const void *data, size_t len)
{

    DEBUG("%s: Starting to send upper layer data (Size: %zu)\n", __func__, len);

#ifdef MODULE_TLSMAN_TINYDTLS
    tinydtls_session_t *aux = &session->tinydtls_context;
    ssize_t res;

    /* TODO DISCUSSION: UDP fragmentation handled here or by the client? */
    res = tlsman_tinydtls_send(aux, (uint8 *) data, len, 0);

    if (res != 0) {
        _tlsman_last_known_error = res;
    }

    return res;

#ifdef WITH_RIOT_SOCKETS
#warning "Not tested"
#endif /* WITH_RIOT_SOCKETS */

    return 0;

#endif /* MODULE_TLSMAN_TINYDTLS */

#ifdef MODULE_TLSMAN_WOLFSSL
#warning "WolfSSL not supported yet"
#endif /* MODULE_WOLFSSL */

    return -1; /* Should not be reachable */
}

ssize_t tlsman_retrieve_data_app(tlsman_session_t *session,
                                 void *data,
                                 size_t max_len)
{
    DEBUG("%s: checking if there is (D)TLS Data Application data\n", __func__);

#ifdef MODULE_TLSMAN_TINYDTLS

    return tlsman_tintdytls_rcv(&session->tinydtls_context, data, max_len);

#endif /* MODULE_TLSMAN_TINYDTLS */

#ifdef MODULE_WOLFSSL
#warning "WolfSSL not supported yet"
#endif /* MODULE_WOLFSSL */

    return 0;

}

void tlsman_release_resources(tlsman_session_t *session)
{

    DEBUG("%s: Releasing resources\n", __func__);

#ifdef MODULE_TLSMAN_TINYDTLS
    tlsman_tinydtls_free_context(&session->tinydtls_context);
#endif /* MODULE_TLSMAN_TINYDTLS */

#ifdef MODULE_WOLFSSL
#warning "WolfSSL not supported yet"
#endif /* MODULE_WOLFSSL */

}

ssize_t tlsman_close_channel(tlsman_session_t *session)
{

#ifdef MODULE_TLSMAN_TINYDTLS
    tinydtls_session_t *tiny;
    tiny = (tinydtls_session_t *) &session->tinydtls_context;
    if (dtls_close(tiny->context, &tiny->dst) < 0) {
        DEBUG("ERROR: Unable to close DTLS channel!");
        return TLSMAN_ERROR_UNABLE_CLOSE;
    }

    /* NOTE DISCUSSION: This state is correct? or should be TINYDTLS_EVENTS_ZERO?  */
    tiny->is_connected = TINYDTLS_EVENT_RENEGOTIATE;

#endif /* MODULE_TLSMAN_TINYDTLS */

#ifdef MODULE_WOLFSSL
#warning "WolfSSL not supported yet"
#endif /* MODULE_WOLFSSL */

    return 0;
}

ssize_t tlsman_renegotiate_session(tlsman_session_t *session, uint8_t flags,
                                   uint8_t *pkt_buff, size_t pkt_size)
{

#ifdef MODULE_TLSMAN_TINYDTLS
    ssize_t res;

    res = tlsman_tinydtls_renegotiate(&session->tinydtls_context, pkt_buff, pkt_size, flags);
    if (res < 0) {
        DEBUG("%s: DTLS renegotiation failed!\n", __func__);
        _tlsman_last_known_error = TLSMAN_ERROR_SESSION_REN;
        return TLSMAN_ERROR_SESSION_REN;
    }

#endif /* MODULE_TLSMAN_TINYDTLS */

#ifdef MODULE_TLSMAN_WOLFSSL
#warning "WolfSSL not supported yet"
#endif /* MODULE_TLSMAN_WOLFSSL */

    return 0;
}

ssize_t tlsman_new_handshake(tlsman_session_t *session, uint8_t flags,
                             uint8_t *pkt_buff, size_t pkt_size)
{
#ifdef MODULE_TLSMAN_TINYDTLS
    ssize_t res;

    res = tlsman_tinydtls_rehandshake(&session->tinydtls_context, pkt_buff, pkt_size, flags);
    if (res < 0) {
        DEBUG("%s: DTLS attempt for a new handshake failed!\n", __func__);
        _tlsman_last_known_error = TLSMAN_ERROR_SESSION_REN;
        return TLSMAN_ERROR_SESSION_REN;
    }

#endif /* MODULE_TLSMAN_TINYDTLS */

#ifdef MODULE_TLSMAN_WOLFSSL
#warning "WolfSSL not supported yet"
#endif /* MODULE_TLSMAN_WOLFSSL */

    return 0;
}

ssize_t tlsman_listening(tlsman_session_t *session, uint8_t flags,
                         uint8_t *pkt_buff, size_t pkt_size)
{

    (void) flags; /* TODO */

#ifdef MODULE_TLSMAN_TINYDTLS

    tlsman_tinydtls_listening(&session->tinydtls_context, pkt_buff, pkt_size);
    /* TODO Handle errors */

#ifdef WITH_RIOT_SOCKETS
    /* TODO FIXME */
#warning "No tested"
#endif  /* WITH_RIOT_SOCKETS */

#endif  /* MODULE_TLSMAN_TINYDTLS */

#ifdef MODULE_TLSMAN_WOLFSSL
#warning "WolfSSL not supported yet"
#endif /* MODULE_TLSMAN_WOLFSSL */

    return 1;

}

#ifdef SOCK_HAS_ASYNC

void tlsman_set_async_parameters(tlsman_session_t *session,
                                 event_queue_t *sock_event_queue,
                                 uint8_t *buffer, ssize_t size)
{

    DEBUG("%s: Linking buffer %p (size %u) to sock\n", __func__, buffer, size);
#ifdef MODULE_TLSMAN_TINYDTLS

    tlsman_tinydtls_set_async_parameters(&session->tinydtls_context,
                                         sock_event_queue, buffer, size);

#endif /*MODULE_TLSMAN_TINYDTLS */

    /* TODO: WolfSSL support */

}

void tlsman_set_async_listening(void)
{

#ifdef MODULE_TLSMAN_TINYDTLS
    DEBUG("%s: Launching timer event\n", __func__);
    tlsman_tinydtls_set_listening_timeout();
#endif
}

#endif /* SOCK_HAS_ASYNC */
