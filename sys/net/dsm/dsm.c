/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_dsm
 * @{
 *
 * @file
 * @brief       DTLS Session Management module implementation
 *
 * @author      János Brodbeck <janos.brodbeck@ml-pa.com>
 *
 * @}
 */

#include "net/dsm.h"
#include "mutex.h"
#include "net/sock/util.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

typedef struct {
    sock_dtls_t *sock;
    sock_dtls_session_t session;
    dsm_state_t state;
    uint32_t last_used_sec;
} dsm_session_t;

static int _find_session(sock_dtls_t *sock, sock_dtls_session_t *to_find,
                         dsm_session_t **session);

static mutex_t _lock;
static dsm_session_t _sessions[DTLS_PEER_MAX];
static uint8_t _available_slots;

void dsm_init(void)
{
    mutex_init(&_lock);
    _available_slots = DTLS_PEER_MAX;
}

dsm_state_t dsm_store(sock_dtls_t *sock, sock_dtls_session_t *session,
                      dsm_state_t new_state, bool restore)
{
    sock_udp_ep_t ep;
    dsm_session_t *session_slot = NULL;
    dsm_state_t prev_state = NO_SPACE;
    mutex_lock(&_lock);

    ssize_t res = _find_session(sock, session, &session_slot);
    if (res < 0) {
        DEBUG("dsm: no space for session to store\n");
        goto out;
    }

    prev_state = session_slot->state;
    if (session_slot->state != SESSION_STATE_ESTABLISHED) {
        session_slot->state = new_state;
    }

    /* no existing session found */
    if (res == 0) {
        DEBUG("dsm: no existing session found, storing as new session\n")
        sock_dtls_session_get_udp_ep(session, &ep);
        sock_dtls_session_set_udp_ep(&session_slot->session, &ep);
        session_slot->sock = sock;
        _available_slots--;
    }

    /* existing session found and session should be restored */
    if (res == 1 && restore) {
        DEBUG("dsm: existing session found, restoring\n")
        memcpy(session, &session_slot->session, sizeof(sock_dtls_session_t));
    }
    session_slot->last_used_sec = (uint32_t)(xtimer_now_usec64() / US_PER_SEC);

out:
    mutex_unlock(&_lock);
    return prev_state;
}

void dsm_remove(sock_dtls_t *sock, sock_dtls_session_t *session)
{
    dsm_session_t *session_slot = NULL;
    mutex_lock(&_lock);
    if (_find_session(sock, session, &session_slot) == 1) {
        if (session_slot->state == SESSION_STATE_NONE) {
            /* session has already been removed. Can happen when we remove the session
            before we get the close ACK of the remote peer (e.g. force reset of peer)
            and then get an ACK (= SOCK_ASYNC_CONN_FIN event) of the remote and
            call this function again. */
            goto out;
        }

        session_slot->state = SESSION_STATE_NONE;
        _available_slots++;
        DEBUG("dsm: removed session\n");
    } else {
        DEBUG("dsm: could not find session to remove, it was probably already removed\n");
    }
out:
    mutex_unlock(&_lock);
}

uint8_t dsm_get_num_available_slots(void)
{
    return _available_slots;
}

uint8_t dsm_get_num_maximum_slots(void)
{
    return DTLS_PEER_MAX;
}

ssize_t dsm_get_least_recently_used_session(sock_dtls_t *sock, sock_dtls_session_t *session)
{
    int res = -1;
    dsm_session_t *session_slot = NULL;

    if (dsm_get_num_available_slots() == DTLS_PEER_MAX) {
        return res;
    }

    mutex_lock(&_lock);
    for (uint8_t i=0; i < DTLS_PEER_MAX; i++) {
        if (_sessions[i].state != SESSION_STATE_ESTABLISHED) {
            continue;
        }
        if (_sessions[i].sock != sock) {
            continue;
        }

        if (session_slot == NULL ||
            session_slot->last_used_sec > _sessions[i].last_used_sec) {
            session_slot = &_sessions[i];
        }
    }

    if (session_slot) {
        memcpy(session, &session_slot->session, sizeof(sock_dtls_session_t));
        res = 1;
    }
    mutex_unlock(&_lock);
    return res;
}

/* Search for existing session or empty slot for new one
 * Returns 1, if existing session found
 * Returns 0, if empty slot found
 * Returns -1, if no existing or empty session found */
static int _find_session(sock_dtls_t *sock, sock_dtls_session_t *to_find,
                         dsm_session_t **session)
{

    /* FIXME: optimize search / data structure */
    sock_udp_ep_t to_find_ep, curr_ep;
    dsm_session_t *empty_session = NULL;

    sock_dtls_session_get_udp_ep(to_find, &to_find_ep);
    for (uint8_t i=0; i < DTLS_PEER_MAX; i++) {
        if (_sessions[i].state == SESSION_STATE_NONE) {
            empty_session = &_sessions[i];
            continue;
        }

        sock_dtls_session_get_udp_ep(&_sessions[i].session, &curr_ep);
        if (sock_udp_ep_equal(&curr_ep, &to_find_ep) && _sessions[i].sock == sock) {
            /* found existing session */
            *session = &_sessions[i];
            return 1;
        }
    }

    if (empty_session) {
        *session = empty_session;
        return 0;
    }
    return -1;
}
