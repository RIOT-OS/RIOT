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
#include "net/tlsman/tinydtls_sock.h"
#include "dtls.h"

/* NOTE DISCUSSION: Should be in sys/include/tlsman or in the app directory? */
#include "dtls_keys.h"

#ifdef SOCK_HAS_ASYNC
#include "event/timeout.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#define TINYDTLS_HDSK_TIMEOUT 1 * US_PER_SEC

static int _send_to_peer(struct dtls_context_t *ctx,
                         session_t *session, uint8 *buf, size_t len);
static int _read_from_peer(struct dtls_context_t *ctx,
                           session_t *session,
                           uint8 *data, size_t len);
static int _client_events(struct dtls_context_t *ctx,
                          session_t *session,
                          dtls_alert_level_t level,
                          unsigned short code);

#ifdef DTLS_PSK
/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session. */
static int _get_psk_info(struct dtls_context_t *ctx,
                         const session_t *session,
                         dtls_credentials_type_t type,
                         const unsigned char *id, size_t id_len,
                         unsigned char *result, size_t result_length);
#endif /* DTLS_PSK */
#ifdef DTLS_ECC
static int _peer_verify_ecdsa_key(struct dtls_context_t *ctx,
                                  const session_t *session,
                                  const unsigned char *other_pub_x,
                                  const unsigned char *other_pub_y,
                                  size_t key_size);
static int _peer_get_ecdsa_key(struct dtls_context_t *ctx,
                               const session_t *session,
                               const dtls_ecdsa_key_t **result);
#endif /* DTLS_ECC */
/* Collection of callbacks for tinydtls operation */
static dtls_handler_t _tinydtls_handler_cbs = {
    .write = _send_to_peer,
    .read = _read_from_peer,
    .event = NULL,
#ifdef DTLS_PSK
    .get_psk_info = NULL,
#endif /* DTLS_PSK */
#ifdef DTLS_ECC
    .get_ecdsa_key = _peer_get_ecdsa_key,
    .verify_ecdsa_key = _peer_verify_ecdsa_key
#endif /* DTLS_ECC */
};

#ifdef SOCK_HAS_ASYNC

/* Asynchronous events */
static void _kill_handshake(event_t *event);
static void _stop_listening(event_t *event);
static void _async_tinydtls_handle_read(event_t *arg);

static event_t kill_handshake_event = { .handler = _kill_handshake };
static event_t stop_listening_event = { .handler = _stop_listening };
static event_timeout_t kill_handshake_timeout;
static event_timeout_t stop_listening_timeout;

static uint8_t _async_recv_watchdog;    /* Counter for disabling the listening mode */
static bool _async_rcv_pkt;             /** Flag to determine if a packet was received */

/* NOTE: Global pointers required for current limitations of event.h */
/* NOTE DISCUSSION: This approach is adequate? can be replicated for tinyDTLS internal buffers? */
static ssize_t _tinydtls_packet_received_size;
static uint8_t *_tinydtls_packet_received;
static tinydtls_session_t *_async_session;

/*
 * This event permits to advance on event_get() and terminate it if necessary
 *  to terminate the asynchronous listening status for the handshake.
 */
static void _kill_handshake(event_t *event)
{
    (void) event;

    /* TODO DISCUSSION Risk of race condition? */
    _async_rcv_pkt ? ++_async_recv_watchdog : --_async_recv_watchdog;
    _async_rcv_pkt = false;

    DEBUG("(%s): Event timeout (%i)\n", __func__, _async_recv_watchdog);
    if (_async_recv_watchdog > 0) {
        event_timeout_set(&kill_handshake_timeout, TINYDTLS_HDSK_TIMEOUT);
    }
    else {
        DEBUG("(%s): DTLS Handshake timeout\n", __func__);
        /* FIXME How to cancel the sock_udp_recv() ? */
    }
}

/*
 *Disable the listening mode
 */
static void _stop_listening(event_t *event)
{

    (void) event;
    DEBUG("(%s): Disabling the listening for DTLS records...\n", __func__);
    _async_recv_watchdog = 0;
}

/**
 * This is the generic event for retrieving sock messages.
 * There are two m
 */
static void _async_tinydtls_handle_read(event_t *arg)
{
    /* TODO: Check this */

    sock_event_t *event = (sock_event_t *)arg;
    static session_t session;
    session_t *peer = &_async_session->dst;

    DEBUG("(%s): Event received! (type 0x%08x) \n", __func__, event->type);

    /* TODO: Generate our own TLS_APP_DATA_RECV */
    if (event->type & SOCK_EVENT_RECV) {
        sock_udp_ep_t remote = { .family = AF_INET6 };
        ssize_t res;

        while ((res = sock_udp_recv(event->sock,
                                    _tinydtls_packet_received,
                                    _tinydtls_packet_received_size,
                                    TINYDTLS_HDSK_TIMEOUT, /* WARNING NO SOCK_NO_TIMEOUT*/
                                    &remote)) >= 0) {
            if (res >= 0) {

                _async_rcv_pkt = true; /* Notify to the other events */

                session.size = sizeof(uint8_t) * 16 + sizeof(unsigned short);
                session.port = remote.port;
                if (peer->ifindex == SOCK_ADDR_ANY_NETIF) {
                    session.ifindex = SOCK_ADDR_ANY_NETIF;
                }
                else {
                    session.ifindex = remote.netif;
                }

                if (memcpy(&session.addr, remote.addr.ipv6, 16) == NULL) {
                    DEBUG("ERROR: memcpy failed!");
                    continue;
                }

                if (ENABLE_DEBUG) {
                    DEBUG("(%s): Msg received from [", __func__);
                    ipv6_addr_print(&session.addr);
                    DEBUG("]:%u (netif: %i)\n", session.port, session.ifindex);
                }

                dtls_handle_message(_async_session->context, &session,
                                    _tinydtls_packet_received, (int) res);

            }
            else if (ENABLE_DEBUG) {
                DEBUG("%s: sock_udp_recv error code: %i\n", __func__, res);
            }
        } /* while */

    }
}

void set_async_listening(void)
{
    DEBUG("%s: Launching timer event\n", __func__);
    event_timeout_set(&stop_listening_timeout, TINYDTLS_HDSK_TIMEOUT);
}

void set_async_parameters(tlsman_session_t *session,
                                 event_queue_t *sock_event_queue,
                                 uint8_t *buffer, ssize_t size)
{

    DEBUG("%s: Linking buffer %p (size %u) to sock\n", __func__, buffer, size);

    session->sock_event_queue = sock_event_queue;

    _tinydtls_packet_received = buffer;
    _tinydtls_packet_received_size = size;

    event_queue_init(session->sock_event_queue);
    /* FIXME: Here or in the handshake? */
    event_timeout_init(&kill_handshake_timeout, session->sock_event_queue,
                       &kill_handshake_event);
    event_timeout_init(&stop_listening_timeout, session->sock_event_queue,
                       &stop_listening_event);

    sock_udp_set_event_queue(session->sock,
                             session->sock_event_queue,
                             _async_tinydtls_handle_read);

    DEBUG("%s: gnrc_sock_async set!\n", __func__);

}

#endif /* SOCK_HAS_ASYNC */

#ifdef DTLS_PSK

static int _get_psk_info(struct dtls_context_t *ctx,
                         const session_t *session,
                         dtls_credentials_type_t type,
                         const unsigned char *id, size_t id_len,
                         unsigned char *result, size_t result_length)
{

    (void) ctx;
    (void) session;
    /* Client side  */
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


static int
_server_get_psk_info(struct dtls_context_t *ctx, const session_t *session,
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
        /* FIXME */
        { (unsigned char *)psk_id, psk_id_length,
          (unsigned char *)psk_key, psk_key_length },
        { (unsigned char *)"default identity", 16,  /* FIXME */
          (unsigned char *)"\x11\x22\x33", 3 },     /* FIXME */
        { (unsigned char *)"\0", 2,                 /* FIXME */
          (unsigned char *)"", 1 }                  /* FIXME */
    };

    if (type != DTLS_PSK_KEY) {
        return 0;
    }

    if (id) {
        uint8_t i;
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
static int _peer_get_ecdsa_key(struct dtls_context_t *ctx,
                               const session_t *session,
                               const dtls_ecdsa_key_t **result)
{
    (void) ctx;
    (void) session;

    /*TODO: Load the key from external source */

    static const dtls_ecdsa_key_t ecdsa_key = {
        .curve = DTLS_ECDH_CURVE_SECP256R1,
        .priv_key = ecdsa_priv_key,
        .pub_key_x = ecdsa_pub_key_x,
        .pub_key_y = ecdsa_pub_key_y
    };

    *result = &ecdsa_key;
    return 0;
}

static int _peer_verify_ecdsa_key(struct dtls_context_t *ctx,
                                  const session_t *session,
                                  const unsigned char *other_pub_x,
                                  const unsigned char *other_pub_y,
                                  size_t key_size)
{
    (void) ctx;
    (void) session;
    (void) other_pub_y;
    (void) other_pub_x;
    (void) key_size;

    /* TODO: As far for tinyDTLS 0.8.2 this is not used */

    return 0;
}
#endif /* DTLS_ECC */

/*
 * Handles the reception of the DTLS flights.
 *
 * Handles all the packets arriving at the node and identifies those that are
 * DTLS records. Also, it determines if said DTLS record is coming from a new
 * peer or a currently established peer.
 *
 * TODO (return values)
 *
 */
ssize_t _tinydtls_handle_read(tinydtls_session_t *tinydtls_session,
                              uint8_t *pkt_buff, size_t pkt_buff_size)
{

    ssize_t pckt_rcvd_size;
    static session_t session;
    session_t *peer = &tinydtls_session->dst;
    static sock_udp_ep_t remote = { .family = AF_INET6 };

    pckt_rcvd_size = sock_udp_recv(tinydtls_session->sock, pkt_buff, pkt_buff_size,
                                   TINYDTLS_HDSK_TIMEOUT, &remote);

    if (pckt_rcvd_size <= 0) {
        DEBUG("%s: sock_udp_recv error code: %i\n", __func__, pckt_rcvd_size);

        return TINYDTLS_HANDSHAKE_NON_RECORD;
    }

    /* session requires the remote socket address (and netif) */
    session.size = sizeof(uint8_t) * 16 + sizeof(unsigned short);
    session.port = remote.port;
    if (peer->ifindex == SOCK_ADDR_ANY_NETIF) {
        /*
         * TODO DISCUSSION: Is this approach correct?
         * The IP%netif:PORT combo is used for identifying different sessions
         * This approach tries to have an "universal" netif
         */
        session.ifindex = SOCK_ADDR_ANY_NETIF;
    }
    else {
        session.ifindex = remote.netif;
    }

    if (memcpy(&session.addr, remote.addr.ipv6, 16) == NULL) {
        DEBUG("ERROR: memcpy failed!");
        return TINYDTLS_HANDSHAKE_NON_RECORD;
    }

    if (ENABLE_DEBUG) {
        DEBUG("(%s): Msg received from [", __func__);
        ipv6_addr_print(&session.addr);
        DEBUG("]:%u (netif: %i)\n", session.port, session.ifindex);
    }

    ssize_t res = dtls_handle_message(tinydtls_session->context, &session, pkt_buff, (int) pckt_rcvd_size);

    if (res < 0) {
        /* Are we getting noise from a previous DTLS ALert record? */
        if (res == dtls_alert_fatal_create(DTLS_ALERT_DECRYPT_ERROR)) {
            DEBUG("%s: Got DTLS_ALERT_DECRYPT_ERROR!\n", __func__);
            return TINYDTLS_HANDSHAKE_RESTART_SVP;
        }
        /* TODO: Try to determine other valid reasons to re-start the handshake */
        return TINYDTLS_HANDSHAKE_NON_RECORD; /* TODO: or FATAL?*/
    }

    return TINYDTLS_HANDSHAKE_NEW_RECORD;
}

/*
 * @brief Determine if the list of cipher suites is valid
 *
 * The only two cipher suites supported by tinyDTLS are selected at compilation
 * time. This only verifies if said cipher suites are present in the compiled
 * code and at least one of them matches the cipher suite listed.
 *
 * @param[in] cipher    Cipher suite to check
 *
 * @return true if cipher suite is supported
 */
bool _is_cipher_tinydtls(uint16_t cipher)
{

#if defined(DTLS_PSK)
    if (cipher == TLS_PSK_WITH_AES_128_CCM_8) {
        DEBUG("DTLS_PSK cipher suite Detected\n");
        return true;
    }
#endif

#if defined(DTLS_ECC)
    if (cipher == TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8) {
        DEBUG("DTLS_ECC cipher suite Detected\n");
        return true;
    }
#endif

    return false;
}

/**
 * @brief Handles the DTLS communication with the other peer.
 */
static int _send_to_peer(struct dtls_context_t *ctx,
                         session_t *session, uint8 *buf, size_t len)
{
    (void) session;
    assert(dtls_get_app_data(ctx));

    tinydtls_session_t *remote_peer;
    remote_peer = (tinydtls_session_t *) dtls_get_app_data(ctx);

    if (ENABLE_DEBUG) {
        printf("(%s): Sending DTLS record to [", __func__);
        ipv6_addr_print(&session->addr);
        printf("]:%u (netif: %i)", session->port, session->ifindex);
        printf(" Size of packet:%zu\n", len);
    }

    if (ipv6_addr_is_unspecified(&session->addr)) {
        puts("Error: Unable to send DTLS record (Session is [::]!\n)");
        return 0;
    }

    ssize_t res = sock_udp_send(remote_peer->sock, buf, len, remote_peer->remote);

    if ((res < 0) && ENABLE_DEBUG) {
        printf("Error: Unable to send DTLS record (Error code: %zd\n)", res);
    }

    return res;
}

/**
 * @brief Reception of a DTLS Application data record.
 */
static int _read_from_peer(struct dtls_context_t *ctx,
                           session_t *session,
                           uint8 *data, size_t len)
{
    (void) session;

    tinydtls_session_t *user_session;
    user_session = (tinydtls_session_t *) dtls_get_app_data(ctx);

    user_session->is_data_app = true;

#if ENABLE_DEBUG
    size_t i;
    printf("%s: DTLS Data Application record (raw hex) -- ", __func__);
    for (i = 0; i < len; i++) {
        printf("%2X ", data[i]);
    }
    printf(" -- (Total: %zu bytes)\n", len );
    if (!user_session->resp_handler) {
        printf("%s: No callback was provided by the user!\n", __func__);
    }
#endif /* ENABLE_DEBUG */

    if (user_session->resp_handler) {
        DEBUG("%s: Launching callback...\n", __func__);
        (*user_session->resp_handler)((uint8_t *)data, len, (void *) user_session->sock);

    }

    return TLSMAN_ERROR_SUCCESS;
}

/**
 * @brief TinyDTLS callback for detecting the state of the DTLS channel.
 *
 * Exclusive of the TinyDTLS Client (For now).
 *
 * NOTE DISCUSSION Only for client side?
 *
 * @note Once renegotiation is included this is also affected.
 */
static int _client_events(struct dtls_context_t *ctx,
                          session_t *session,
                          dtls_alert_level_t level,
                          unsigned short code)
{
    (void) ctx;
    (void) session;
    (void) level;

    tinydtls_session_t *remote_peer;
    remote_peer = (tinydtls_session_t *) dtls_get_app_data(ctx);

    if (code == DTLS_EVENT_CONNECTED) {
        DEBUG("CLIENT: DTLS Channel established!\n");
        remote_peer->is_connected = TINYDTLS_EVENT_FINISHED;

#ifdef SOCK_HAS_ASYNC
        _async_recv_watchdog = -1; /* FIXME: Mutex? */
#endif /* SOCK_HAS_ASYNC */
    }
    /* At least a DTLS Client Hello was prepared? */
    else if (code == DTLS_EVENT_CONNECT) {
        DEBUG("CLIENT: DTLS Channel started\n");
        remote_peer->is_connected = TINYDTLS_EVENT_START;
    }
    else if (code == DTLS_EVENT_RENEGOTIATE) {
        DEBUG("CLIENT: DTLS Renegotiates!\n");
        remote_peer->is_connected = TINYDTLS_EVENT_FINISHED;
    }

    return TLSMAN_ERROR_SUCCESS;
}


ssize_t _tinydtls_send(tlsman_session_t *dtls_session,
                             uint8_t *data, size_t data_size,
                             uint8_t flags)
{
    ssize_t len = data_size;

    /* TODO: Test this with longer buffers */
    /* TODO: Sock_async support */
    (void) flags;

    while (len > 0) {
        /* DEBUG("%s: Sending %zu bytes...\n", __func__, len); */
        ssize_t res = dtls_write(dtls_session->context, &dtls_session->dst, (uint8 *)data, len);
        if (res >= 0) {
            /* DISCUSSION: This is appropriate? */
            memmove(data, data + res, len - res);
            len -= res;

            if (res == 0) {
                return 0;
            }
        }
        else {
            DEBUG("%s: dtls_write() Error code: %zd\n", __func__, res);
            return TLSMAN_ERROR_DATA_APP_WRITE;
        }
    } /* while */

    return 0;
}

size_t _tintdytls_rcv(tinydtls_session_t *dtls_session,
                            void *pkt_buff,
                            size_t pkt_size)
{
    /* TODO: Handle the flag argument? */

#ifdef SOCK_HAS_ASYNC
    (void) pkt_buff;
    (void) pkt_size;

    /*
     * This behavior is different from the Handshake.
     * Not event_timeout is settled here.
     */

    _async_session = dtls_session;  /* FIXME: Mutex here? */
    _async_recv_watchdog = 1;       /* We are to use it as boolean now */

    DEBUG("%s: Starting NON-blocking Listening for DTLS records!\n", __func__);
    while (_async_recv_watchdog > 0) {
        event_t *event = event_get(dtls_session->sock_event_queue);
        if (event) {
            event->handler(event);
        }
        xtimer_usleep(200); /* FIXME: Magic number for now */
    }
    event_timeout_clear(&stop_listening_timeout);
    DEBUG("%s: Stopping NON-blocking Listening for DTLS records!\n", __func__);
    return 0;

#else /* SOCK_HAS_ASYNC */
    /* NOTE: This should be OK for sock or socket */
    DEBUG("%s: Starting blocking Listening for DTLS records!\n", __func__);

    dtls_session->is_data_app = false;
    /* Extract any type of DTLS record until finding the first Data App */
    do {
        ssize_t res = _tinydtls_handle_read(dtls_session, pkt_buff, pkt_size);
        if (res == TINYDTLS_HANDSHAKE_RESTART_SVP) {
            return TLSMAN_ERROR_REMOTE_RESET;
        }
        else if (res == TINYDTLS_HANDSHAKE_NON_RECORD) {
            DEBUG("%s: Not more DTLS records available in the buffer\n", __func__);
            return 0;
        }
    } while (!dtls_session->is_data_app);

    return 0;
#endif /* SOCK_HAS_ASYNC */
}

ssize_t _tinydtls_handshake(tinydtls_session_t *session,
                            uint8_t *pkt_buff, size_t pkt_size, uint8_t flags)
{
    if (flags & TLSMAN_FLAG_NONBLOCKING)  {
        DEBUG("%s: Starting NON-blocking DTLS handshake!\n", __func__);
#ifdef SOCK_HAS_ASYNC
        (void) pkt_buff;
        (void) pkt_size;
        /* Set the timer event */
        _async_session = session; /* FIXME: Mutex here? */
        _async_recv_watchdog = TLSMAN_HANDSHAKE_MAX_TRY;
        _async_rcv_pkt = false;

        event_timeout_set(&kill_handshake_timeout, TINYDTLS_HDSK_TIMEOUT);

        while ((session->is_connected != TINYDTLS_EVENT_FINISHED) &&
               (_async_recv_watchdog > 0)) {
            /*
             * We are using kill_handshake_timeout to be able to advance in
             * the event queue. It's possible to ignore kill_handshake_timeout()
             * however, we need to give enough time for the other side to send
             * a DTLS record.
             *
             * The current configuration let's to await for the timeout at the
             * same time that we can answer very fast at any UDP packet received
             *
             * NOTE: event_get() is non-blocking. However, _async_tinydtls_handle_read()
             *       it's blocking (sock_udp_recv() set to SOCK_NO_TIMEOUT).
             */
            event_t *event = event_get(session->sock_event_queue);
            if (event) {
                event->handler(event);
            }

            if (session->is_connected == TINYDTLS_EVENTS_ZERO) {
                DEBUG("%s: session->is_connected is TINYDTLS_EVENTS_ZERO!\n",__func__);
                event_timeout_clear(&kill_handshake_timeout); /* Reset the timer */
                dtls_connect(session->context, &session->dst);
                event_timeout_set(&kill_handshake_timeout, TINYDTLS_HDSK_TIMEOUT);
            }
            xtimer_usleep(500);  /* FIXME: Magic number for now */
        }

        DEBUG("%s:NON-blocking DTLS handshake finished!\n", __func__);

        /* TODO: Disable temporary the events queue? */
        event_timeout_clear(&kill_handshake_timeout);
        event_cancel(session->sock_event_queue, &kill_handshake_event);

        if (session->is_connected != TINYDTLS_EVENT_FINISHED) {
            return TLSMAN_ERROR_HANDSHAKE_TIMEOUT;
        }

#else   /* SOCK_HAS_ASYNC */
        /* TODO DISCUSSION: Throw error instead of this? */
        puts("WARNING: module gnrc_sock_async not enabled. Behavior is synchronous!");
#endif  /* SOCK_HAS_ASYNC */
    }

#ifndef SOCK_HAS_ASYNC
    DEBUG("%s: Starting blocking DTLS handshake!\n", __func__);

    uint8_t watchdog = TLSMAN_HANDSHAKE_MAX_TRY;

    while ((session->is_connected != TINYDTLS_EVENT_FINISHED) &&
           (watchdog > 0U)) {

        ssize_t res = _tinydtls_handle_read(session, pkt_buff, pkt_size);

        /* NOTE: There is an implicit sleep() function here */
        if (res == TINYDTLS_HANDSHAKE_NEW_RECORD) {
            watchdog++; /* NOTE: Maybe too much? */
        }
        else if (res == TINYDTLS_HANDSHAKE_RESTART_SVP) {
            watchdog = 1U;
            /* TODO: New handshake error for tlsman? */
        }

        if (session->is_connected == TINYDTLS_EVENTS_ZERO) {
            /* NOTE: Sometimes tinydtls don't send the first msg */
            DEBUG("%s: Sending a new DTLS Client Hello\n", __func__);
            dtls_connect(session->context, &session->dst);
        }

        if ((--watchdog == 0U) || (res == TINYDTLS_HANDSHAKE_IS_FATAL)) {
            DEBUG("%s: DTLS handshake timeout!\n", __func__);
            return TLSMAN_ERROR_HANDSHAKE_TIMEOUT;
        }

        xtimer_usleep(500);
    }
#endif  /* !SOCK_HAS_ASYNC */

    return 0;
}

ssize_t _tinydtls_renegotiate(tinydtls_session_t *session,
                                    uint8_t *pkt_buff, size_t pkt_size,
                                    uint8_t flags)
{

    if (dtls_renegotiate(session->context, &session->dst) < 0) {
        DEBUG("%s: DTLS renegotiate failed!\n", __func__);
        return -1;
    }

    /* This is very similar to the first connect, but the context is ready */
    session->is_connected = TINYDTLS_EVENTS_ZERO;
    return _tinydtls_handshake(session, pkt_buff, pkt_size, flags);
}

ssize_t _tinydtls_rehandshake(tinydtls_session_t *dtls_session,
                                    uint8_t *pkt_buff, size_t pkt_size,
                                    uint8_t flags)
{

    if (dtls_session->cache == NULL) {
        /*
         * NOTE: We cannot free the current context as it will notify
         * the Server to close the connection (which we do not want).
         */
        dtls_session->cache = dtls_session->context;
        dtls_session->is_connected = TINYDTLS_EVENTS_ZERO;

        /* NOTE: Create a new context and attempt to initiate a handshake. */
        dtls_session->context = dtls_new_context(dtls_session);
        if (!dtls_session->context) {
            dtls_emerg("cannot create context\n");
            return TLSMAN_ERROR_UNABLE_CONTEXT;
        }

        dtls_set_handler(dtls_session->context, &_tinydtls_handler_cbs);
        if (dtls_connect(dtls_session->context, &dtls_session->dst)) {
            DEBUG("%s: Unable to send the first ClientHello!\n", __func__);
            /* NOTE: _tinydtls_handshake will try to send it again */
        }
        return _tinydtls_handshake(dtls_session, pkt_buff, pkt_size, flags);

    }

    return TLSMAN_ERROR_UNABLE_CONTEXT; /* TODO: New error type? */
}

ssize_t load_stack(uint16_t *ciphersuites_list, size_t total, uint8_t flags){
    bool match = false;

    if (!(flags & TLSMAN_FLAG_STACK_DTLS)) {
        DEBUG("%s: TinyDTLS only support UDP!\n", __func__);
        return TLSMAN_ERROR_TRANSPORT_NOT_SUPPORTED;
    }

    /* NOTE: tinyDTLS can only select cipher suites at the compilation time */
    size_t aux;
    for (aux = 0; aux <  total; aux++) {
        if (_is_cipher_tinydtls(ciphersuites_list[aux])) {
            match = true;
            break;
        }
    }

    if (!match) {
        DEBUG("%s: TinyDTLS Cipher suite not supported!\n", __func__);
        return TLSMAN_ERROR_CIPHER_NOT_SUPPORTED;
    }

    dtls_init(); /*TinyDTLS mandatory starting settings*/

#ifdef TINYDTLS_LOG_LVL
    dtls_set_log_level(TINYDTLS_LOG_LVL);
#endif

    return 0;
}


ssize_t init_context(tlsman_session_t *session, tlsman_resp_handler_t resp_cb,
                     const uint8_t flags)
{
    /*
     * FIXME DISCUSSION: A more elegant solution need to be used!
     *
     * We are dereferencing a structure which the first time should be
     * pointing to null.
     *
     * This also will change if tinydtls_session_t is modified.
     * Including compilation with PSK or ECC
     *
     */
    /*
       if ((session->tinydtls_context.context != (void *)0x1) &&
        (session->tinydtls_context.context != NULL)){
        DEBUG("tinyDTLS: DTLS context already exist!\n");
        return 0;
       }
     */

    assert(session);
    /* assert(session->local); */
    assert(session->remote);
    assert(session->sock);
    DEBUG("tinyDTLS: Creating context (sock)\n");

    session->context = NULL;
    session_t *dst = &session->dst;
    sock_udp_ep_t *remote = session->remote;

    /* FIXME: Risk of race condition? */
    session->last_known_error = TLSMAN_ERROR_NON_STARTED;
    session->is_connected = TINYDTLS_EVENTS_ZERO;

    DEBUG("%s: Linking DTLS session to UDP sock\n", __func__);

    session->dst.size = sizeof(uint8_t) * 16 + sizeof(unsigned short);
    session->dst.port = session->remote->port;

    /* NOTE: Parsing iface for the remote */
    if (gnrc_netif_numof() == 1) {
        /* assign the single interface found in gnrc_netif_numof() */
        session->dst.ifindex = (uint16_t)gnrc_netif_iter(NULL)->pid;
        session->remote->netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
    }
    else {
        /* FIXME This probably is not valid with multiple interfaces */
        session->dst.ifindex = session->remote->netif;
    }

    /* NOTE: remote.addr and dst->addr are different structures. */
    if (memcpy(&dst->addr, &remote->addr.ipv6, 16) == NULL) {
        DEBUG("ERROR: memcpy failed!");
        return -1;
    }

    session->resp_handler = resp_cb;
    session->context = dtls_new_context(session);

    if (flags & TLSMAN_FLAG_SIDE_SERVER) {
        _tinydtls_handler_cbs.event = NULL; /* FIXME REMOVE(?) */
#ifdef DTLS_PSK
        _tinydtls_handler_cbs.get_psk_info = _server_get_psk_info;
#endif  /* DTLS_PSK */
    }
    else {
        _tinydtls_handler_cbs.event = _client_events;
#ifdef DTLS_PSK
        _tinydtls_handler_cbs.get_psk_info = _get_psk_info;
#endif  /* DTLS_PSK */
    }

    if (session->context) {
        dtls_set_handler(session->context, &_tinydtls_handler_cbs);
        return 0;
    }

    session->last_known_error = TLSMAN_ERROR_UNABLE_CONTEXT;
    return TLSMAN_ERROR_UNABLE_CONTEXT;
}

ssize_t connect(tlsman_session_t *session, uint8_t flags,
                uint8_t *pkt_buff, size_t pkt_size)
{
    /* NOTE: Irrelevant if socks or sockets */
    if (session->is_connected == TINYDTLS_EVENT_RENEGOTIATE) {
        /* TODO FIXME: Do we need to do something to our current peer? */
    }
    if (dtls_connect(session->context, &session->dst) < 0) {
        DEBUG("ERROR: Unable to start a DTLS channel!\n");
        return TLSMAN_ERROR_FATAL;
    }

    ssize_t res =  _tinydtls_handshake(session, pkt_buff, pkt_size, flags);
    session->last_known_error = res;
    return res;
}

bool is_channel_ready(tlsman_session_t *session) {

    if (session->is_connected == TINYDTLS_EVENT_FINISHED) {
        return true;
    }

    return false;
}


ssize_t send_data_app(tlsman_session_t *session, const void *data, size_t len)
{

    DEBUG("%s: Starting to send upper layer data (Size: %zu)\n", __func__, len);

    ssize_t res;

    /* TODO DISCUSSION: UDP fragmentation handled here or by the client? */
    res = _tinydtls_send(session, (uint8 *) data, len, 0);

    session->last_known_error = res;

    return res;
}

ssize_t retrieve_data_app(tlsman_session_t *session, void *pkt_buff, size_t max_len)
{
    /* DEBUG("%s: checking if there is (D)TLS Data Application data\n", __func__); */

    ssize_t res = _tintdytls_rcv(session, pkt_buff, max_len);

    session->last_known_error = res;

    return res;
}

void release_resources(tlsman_session_t *session)
{

    dtls_free_context(session->context);
    dtls_free_context(session->cache);
    session->is_connected = TINYDTLS_EVENTS_ZERO;

#ifdef SOCK_HAS_ASYNC
    event_timeout_clear(&kill_handshake_timeout);
    event_timeout_clear(&stop_listening_timeout);
    sock_udp_set_event_queue(session->sock, NULL, NULL);
#endif /* SOCK_HAS_ASYNC */

    DEBUG("%s: Resources released\n", __func__);
}

ssize_t close_channel(tlsman_session_t *session)
{
    if (dtls_close(session->context, &session->dst) < 0) {
        DEBUG("ERROR: Unable to close DTLS channel!\n");
        session->last_known_error = TLSMAN_ERROR_UNABLE_CLOSE;
        return TLSMAN_ERROR_UNABLE_CLOSE;
    }

    /* NOTE DISCUSSION: This state is correct? or should be TINYDTLS_EVENTS_ZERO?  */
    session->is_connected = TINYDTLS_EVENT_RENEGOTIATE;

    session->last_known_error = 0;
    return 0;
}


ssize_t renegotiate(tlsman_session_t *session, uint8_t flags,
                                   uint8_t *pkt_buff, size_t pkt_size)
{

    ssize_t res;

    res = _tinydtls_renegotiate(session, pkt_buff, pkt_size, flags);
    if (res < 0) {
        DEBUG("%s: DTLS renegotiation failed!\n", __func__);
        session->last_known_error = TLSMAN_ERROR_SESSION_REN;
        return TLSMAN_ERROR_SESSION_REN;
    }
    session->last_known_error = 0;
    return 0;
}

ssize_t rehandshake(tlsman_session_t *session, uint8_t flags,
                             uint8_t *pkt_buff, size_t pkt_size)
{
    ssize_t res = _tinydtls_rehandshake(session, pkt_buff, pkt_size, flags);
    if (res < 0) {
        DEBUG("%s: DTLS attempt for a new handshake failed!\n", __func__);
        session->last_known_error = TLSMAN_ERROR_SESSION_REN;
        return TLSMAN_ERROR_SESSION_REN;
    }

    session->last_known_error = 0;

    return 0;
}

ssize_t listening(tlsman_session_t *session, uint8_t flags,
                         uint8_t *pkt_buff, size_t pkt_size)
{

    (void) flags; /* TODO */

    /*
     * This intended to handle handshake and reception of real data
     * WITHOUT timeouts (and only for the server side)
     */
    /* TODO Be able to kill it */
    /* TODO Handle errors */

#ifdef  SOCK_HAS_ASYNC
    (void) pkt_buff;
    (void) pkt_size;
    DEBUG("%s: Starting (Async) listening for DTLS records!\n", __func__);
    _async_session = session;  /* FIXME: Mutex here? */
    _async_recv_watchdog = 1;       /* We are to use it as boolean now */

    while (_async_recv_watchdog > 0) {
        event_t *event = event_get(session->sock_event_queue);
        if (event) {
            event->handler(event);
        }
        xtimer_usleep(200); /* FIXME: Magic number for now */
    }
    DEBUG("%s: Stopping (Async) listening for DTLS records!\n", __func__);

#else /* SOCK_HAS_ASYNC */

    ssize_t pckt_rcvd_size;
    session_t *peer = &session->dst;
    static sock_udp_ep_t remote = { .family = AF_INET6 };

    /* NOTE FIXME Blocking? one second?  */
    pckt_rcvd_size = sock_udp_recv(session->sock, pkt_buff, pkt_size,
                                   0, &remote);
    if (pckt_rcvd_size <= 0) {
        if ((pckt_rcvd_size != -ETIMEDOUT)
            && ((pckt_rcvd_size != -EAGAIN))
            && ENABLE_DEBUG) {
            DEBUG("%s: sock_udp_recv error code is %i\n", __func__, pckt_rcvd_size);
            return 0;
        }
        return 1;
    }

    /* We need to update the remote (and the session) */
    session->remote = &remote;

    /* Equivalent to  _tinydtls_handle_read() */
    peer->size = sizeof(uint8_t) * 16 + sizeof(unsigned short);
    peer->port = remote.port;

    /* TESTING : Override behavior for the server? */
    peer->ifindex = SOCK_ADDR_ANY_NETIF;

    if (memcpy(&peer->addr, remote.addr.ipv6, 16) == NULL) {
        DEBUG("ERROR: memcpy failed!");
        return TINYDTLS_HANDSHAKE_NON_RECORD;
    }

    if (ENABLE_DEBUG) {
        DEBUG("(%s): Msg received from [", __func__);
        ipv6_addr_print(&peer->addr);
        DEBUG("]:%u (netif: %i)\n", peer->port, peer->ifindex);
    }

    /* Server don't care about the result */
    dtls_handle_message(session->context, peer, pkt_buff, (int) pckt_rcvd_size);

#endif

    return 1;

}
