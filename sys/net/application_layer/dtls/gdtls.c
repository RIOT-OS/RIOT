/*
 * Copyright (C) 2017 Inria
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * @file
 * @author      Raul Fuentes <raul.fuentes_samaniego@inria.fr>
 *
 */

#include "net/dtls/gdtls.h"

#include <stdio.h>
#include <inttypes.h>
#include <net/ipv6/hdr.h>
#include <net/udp.h>

#include "net/sock/udp.h"
#include "net/ipv6/addr.h"
#include "net/af.h" /* TODO: Remove it */

/* TinyDTLS */
#include "dtls.h"
#include "dtls_debug.h"
#include "tinydtls.h"


/* TODO: Adapt this to tinyDTLS's syslog */
#ifdef GDTLS_DEBUG
#define ENABLE_DEBUG (1)
#else
#define ENABLE_DEBUG (0)
#endif
#include "debug.h"

/*
 * Status of the DTLS session with the remote peer
 * TODO: Removed it by the new var in dtls_remote_peer_t
 */
int8_t dtls_connected = 0;

/* Variables for the DTLS client side.   */
static unsigned char psk_id[PSK_ID_MAXLEN] = PSK_DEFAULT_IDENTITY;
static size_t psk_id_length = sizeof(PSK_DEFAULT_IDENTITY) - 1;
static unsigned char psk_key[PSK_MAXLEN] = PSK_DEFAULT_KEY;
static size_t psk_key_length = sizeof(PSK_DEFAULT_KEY) - 1;

/**
 * @brief Size fore the IPC queue  for the module thread
 *
 * Because of the variable number of DTLS messages to send in the different
 * flights. It's more safe to icnrease the number of threads.
 */
#define DTLS_READER__QUEUE_SIZE (16U)


int try_send(struct dtls_context_t *ctx, session_t *dst, uint8 *buf, size_t len)
{

    int res = 0;

    assert( (ctx != NULL) && (dst != NULL));

    res = dtls_write(ctx, dst, buf, len);

    if (res >= 0) {
        memmove(buf, buf + res, len - res);
        len -= res;
        return len;
    }
    else if (res < 0) {
        dtls_crit("DTLS: dtls_write returned error! %i\n", res );
        return -1;
    }

    return 0; /* No reachable */
}


void dtls_handle_read_sock(dtls_context_t *ctx, uint8_t *packet,
                           size_t size)
{

    static session_t session;

    if (!ctx) {
        DEBUG("No DTLS context\n");
        return;
    }

    if (!dtls_get_app_data(ctx)) {
        DEBUG("No app_data stored!\n");
        return;
    }


    dtls_remote_peer_t *remote_peer;
    remote_peer =  (dtls_remote_peer_t *) dtls_get_app_data(ctx);

    /* session requires the remote socket address */
    session.size = sizeof(uint8_t) * 16 + sizeof(unsigned short);
    session.port = remote_peer->remote->port;

    if (memcpy(&session.addr, &remote_peer->remote->addr.ipv6, 16) == NULL) {
        puts("ERROR: memcpy failed!");
        return;
    }


    dtls_handle_message(ctx, &session, packet, (int) size);

    return;
}

#ifdef DTLS_PSK

int _client_peer_get_psk_info(struct dtls_context_t *ctx,
                              const session_t *session,
                              dtls_credentials_type_t type,
                              const unsigned char *id, size_t id_len,
                              unsigned char *result, size_t result_length)
{
    (void) ctx;
    (void) session;

    switch (type) {
        case DTLS_PSK_IDENTITY:
            if (id_len) {
                dtls_debug("got psk_identity_hint: '%.*s'\n", id_len, id);
            }

            if (result_length < psk_id_length) {
                dtls_warn("cannot set psk_identity -- buffer too small\n");
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }

            memcpy(result, psk_id, psk_id_length);
            return psk_id_length;
        case DTLS_PSK_KEY:
            if (id_len != psk_id_length || memcmp(psk_id, id, id_len) != 0) {
                dtls_warn("PSK for unknown id requested, exiting\n");
                return dtls_alert_fatal_create(DTLS_ALERT_ILLEGAL_PARAMETER);
            }
            else if (result_length < psk_key_length) {
                dtls_warn("cannot set psk -- buffer too small\n");
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }

            memcpy(result, psk_key, psk_key_length);
            return psk_key_length;
        default:
            dtls_warn("unsupported request type: %d\n", type);
    }

    return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
}

int _server_peer_get_psk_info(struct dtls_context_t *ctx,
                              const session_t *session,
                              dtls_credentials_type_t type,
                              const unsigned char *id, size_t id_len,
                              unsigned char *result, size_t result_length)
{

    (void) ctx;
    (void) session;
    struct keymap_t {
        unsigned char *id;
        size_t id_length;
        unsigned char *key;
        size_t key_length;
    } psk[3] = {
        { (unsigned char *)"Client_identity", 15,
          (unsigned char *)"secretPSK", 9 },
        { (unsigned char *)"default identity", 16,
          (unsigned char *)"\x11\x22\x33", 3 },
        { (unsigned char *)"\0", 2,
          (unsigned char *)"", 1 }
    };

    if (type != DTLS_PSK_KEY) {
        return 0;
    }

    if (id) {
        unsigned int i;
        for (i = 0; i < sizeof(psk) / sizeof(struct keymap_t); i++) {
            if (id_len == psk[i].id_length && memcmp(id, psk[i].id, id_len) == 0) {
                if (result_length < psk[i].key_length) {
                    dtls_warn("buffer too small for PSK");
                    return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
                }

                memcpy(result, psk[i].key, psk[i].key_length);
                return psk[i].key_length;
            }
        }
    }

    return dtls_alert_fatal_create(DTLS_ALERT_DECRYPT_ERROR);
}

#endif /* DTLS_PSK */

#ifdef DTLS_ECC

/* NOTE: The client and server sides are identically for ECC */

int _peer_get_ecdsa_key(struct dtls_context_t *ctx,
                        const session_t *session,
                        const dtls_ecdsa_key_t **result)
{
    (void) ctx;
    (void) session;
    static const dtls_ecdsa_key_t ecdsa_key = {
        .curve = DTLS_ECDH_CURVE_SECP256R1,
        .priv_key = ecdsa_priv_key,
        .pub_key_x = ecdsa_pub_key_x,
        .pub_key_y = ecdsa_pub_key_y
    };

    *result = &ecdsa_key;
    return 0;
}

int _peer_verify_ecdsa_key(struct dtls_context_t *ctx,
                           const session_t *session,
                           const unsigned char *other_pub_x,
                           const unsigned char *other_pub_y,
                           size_t key_size)
{
    (void) ctx;
    (void) session;
    (void) other_pub_x;
    (void) other_pub_y;
    (void) key_size;

    /* As far for tinyDTLS 0.8.2 this is not used */
    return 0;
}
#endif /* DTLS_ECC */



int _send_to_peer(struct dtls_context_t *ctx,
                  session_t *session, uint8 *buf, size_t len)
{
    (void) session;

    assert(dtls_get_app_data(ctx) != NULL);

    dtls_remote_peer_t *remote_peer;
    remote_peer =  (dtls_remote_peer_t *) dtls_get_app_data(ctx);

    int res = sock_udp_send(remote_peer->sock, buf, len, remote_peer->remote);
#if ENABLE_DEBUG == 1
    if (!(res > 0)) {
        DEBUG("Error: Unable to send DTLS record (Error code %i)", res);
    }
#endif

    return res;
}


int _client_events(struct dtls_context_t *ctx,
                   session_t *session,
                   dtls_alert_level_t level,
                   unsigned short code)
{
    (void) ctx;
    (void) session;
    (void) level;

    /* dtls_connect has concluded in a DTLS session established? */
    if (code == DTLS_EVENT_CONNECTED) {
        dtls_connected = 1;
        DEBUG("CLIENT: DTLS Channel established!\n");
    }
#if ENABLE_DEBUG == 1
    /* At least a DTLS Client Hello was prepared? */
    else if (code == DTLS_EVENT_CONNECT) {
        DEBUG("CLIENT: DTLS Channel started\n");
        assert(session != NULL);
    }
#endif
    /*TODO: DTLS_EVENT_RENEGOTIATE */

    return 0;
}


void gnrc_dtls_init(void)
{

    dtls_init(); /*TinyDTLS mandatory starting settings*/

    /* TODO: Create a better syslog initialization */
#if ENABLE_DEBUG == 1
    dtls_set_log_level(DTLS_LOG_DEBUG);
#else
    dtls_set_log_level(DTLS_LOG_INFO);
#endif

#ifdef DTLS_PSK
    DEBUG("DTLS supports PSK\n");
#endif
#ifdef DTLS_ECC
    DEBUG("DTLS supports ECC\n");
#endif

    return;
}
