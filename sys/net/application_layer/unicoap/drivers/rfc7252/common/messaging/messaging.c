/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @file
 * @ingroup net_unicoap_drivers_rfc7252_common
 * @brief   Messaging implementation of common RFC 7252 driver
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <stdint.h>
#include <errno.h>
#include <stdatomic.h>
#include "random.h"
#include "ztimer.h"
#include "container.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

#define MESSAGING_7252_DEBUG(...) _UNICOAP_PREFIX_DEBUG(".messaging.rfc7252", __VA_ARGS__)
#define PDU_7252_DEBUG(...)       _UNICOAP_PREFIX_DEBUG(".pdu.rfc7252", __VA_ARGS__)

/** @brief Message ID print format */
#define UNICOAP_MESSAGE_ID_FORMAT  "[MID %" PRIu16 "] "

#define PACKET_7252_DEBUG(packet)                                                                \
    DEBUG("<%s %s mid=%" PRIu16 " token=",                                                       \
        unicoap_string_from_rfc7252_type(_get_type(packet)),                                     \
        unicoap_string_from_code_class((packet)->message->code),                                 \
        _get_id(packet)                                                                          \
    );                                                                                           \
                                                                                                 \
    _UNICOAP_DEBUG_HEX((packet)->properties.token, (packet)->properties.token_length);           \
                                                                                                 \
    DEBUG(" code=" UNICOAP_CODE_CLASS_DETAIL_FORMAT " %s payload=(%" PRIuSIZE " bytes) "         \
          "options=(%" PRIuSIZE "; %" PRIuSIZE " bytes)>",                                       \
          unicoap_code_class((packet)->message->code),                                           \
          unicoap_code_detail((packet)->message->code),                                          \
          unicoap_string_from_code((packet)->message->code),                                     \
          (packet)->message->payload_size,                                                       \
          (packet)->message->options ? (packet)->message->options->option_count : 0,             \
          (packet)->message->options ? (packet)->message->options->storage_size : 0              \
    )

#define __IOLIST(data, size, next) (iolist_t){ .iol_base = data, .iol_len = size, .iol_next = next }

typedef struct {
    /**
     * @brief Timeout that occurs when waiting for an acknowledgement message
     */
    unicoap_scheduled_event_t ack_timeout;

    /**
     * @brief Remote endpoint
     */
    unicoap_endpoint_t endpoint;

#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    unicoap_sock_dtls_session_t dtls_session;
#endif

    /**
     * @brief Copy of confirmable message PDU for retransmission
     */
    uint8_t* pdu;

    /**
     * @brief Size of @ref unicoap_messaging_state_udp_dtls_t.pdu
     */
    size_t pdu_size;

    /** @brief Message ID */
    uint16_t id;

    /**
     * @brief Number of remaining retransmission attempts of a confirmable (`CON`) message
     */
    uint8_t remaining_retransmissions : 5;

    bool is_used : 1;
} _transmission_t;

typedef struct {
    /** @brief Message ID unicoap is going to use for the next outbound request or response */
    atomic_uint next_message_id;

#if CONFIG_UNICOAP_CARBON_COPIES_MAX > 0
    /** @brief Copy of PDU sent, used for retransmission and deduplication */
    uint8_t carbon_copies[CONFIG_UNICOAP_CARBON_COPIES_MAX][CONFIG_UNICOAP_PDU_SIZE_MAX];
#endif

#if CONFIG_UNICOAP_RFC7252_TRANSMISSIONS_MAX > 0
    _transmission_t transmissions[CONFIG_UNICOAP_RFC7252_TRANSMISSIONS_MAX];
#endif
} _state_t;

static inline void* _transmission_get_session(_transmission_t* transmission)
{
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    return &transmission->dtls_session;
#else
    (void)transmission;
    return NULL;
#endif
}

static inline void _transmission_set_session(_transmission_t* transmission,
                                             const unicoap_sock_dtls_session_t* dtls_session)
{
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    transmission->dtls_session = *dtls_session;
#else
    (void)transmission;
    (void)dtls_session;
#endif
}

static inline uint16_t _get_id(const unicoap_packet_t* packet)
{
    return packet->properties.rfc7252.id;
}

static inline const uint16_t* _get_id_ptr(const unicoap_packet_t* packet)
{
    return &packet->properties.rfc7252.id;
}

static inline void _set_id(unicoap_packet_t* packet, uint16_t id)
{
    packet->properties.rfc7252.id = id;
}

static inline unicoap_rfc7252_message_type_t _get_type(const unicoap_packet_t* packet)
{
    return packet->properties.rfc7252.type;
}

static inline void _set_type(unicoap_packet_t* packet, unicoap_rfc7252_message_type_t type)
{
    packet->properties.rfc7252.type = type;
}

static inline const unicoap_endpoint_t* _transmission_get_endpoint(const _transmission_t* transmission) {
    return &transmission->endpoint;
}

static _state_t _state = { 0 };

int unicoap_init_rfc7252_common(event_queue_t* queue)
{
    (void)queue;
    atomic_init(&_state.next_message_id, (unsigned int)random_uint32());
    return 0;
}

int unicoap_deinit_rfc7252_common(event_queue_t* queue)
{
    (void)queue;
    memset(&_state, 0, sizeof(_state));
    return 0;
}

static uint16_t _generate_message_id(void)
{
    return (uint16_t)atomic_fetch_add(&_state.next_message_id, 1);
}

/* MARK: - (Re-)Transmissions */

static uint8_t* _carbon_copy_alloc_unsafe(void)
{
#if CONFIG_UNICOAP_CARBON_COPIES_MAX > 0
    /* Find empty slot in list of open requests. */
    for (int i = 0; i < (int)ARRAY_SIZE(_state.carbon_copies); i += 1) {
        /* Will never be zero due to CoAP version 1 bit. Zero indicates 'free' buffer */
        if (*_state.carbon_copies[i] == 0) {
            /* Claim that buffer by setting first byte to 0xff. Later, we adjust that. */
            *_state.carbon_copies[i] = 0xff;
            return _state.carbon_copies[i];
        }
    }
#endif
    MESSAGING_7252_DEBUG("no space to alloc PDU copy\n");
    return NULL;
}

static uint8_t* _carbon_copy_alloc(void)
{
    unicoap_state_lock();
    uint8_t* carbon_copy = _carbon_copy_alloc_unsafe();
    unicoap_state_unlock();
    return carbon_copy;
}

static inline void _carbon_copy_free(uint8_t* carbon_copy)
{
    /* Freeing does not require lock. We have sole access until the following line. */
    *carbon_copy = 0;
}

static inline _transmission_t* _transmission_alloc_unsafe(void)
{
#if CONFIG_UNICOAP_RFC7252_TRANSMISSIONS_MAX > 0
    /* Find empty slot in list of open requests. */
    for (int i = 0; i < (int)ARRAY_SIZE(_state.transmissions); i += 1) {
        if (!_state.transmissions[i].is_used) {
            _state.transmissions[i].is_used = true;
            return &_state.transmissions[i];
        }
    }
#endif
    MESSAGING_7252_DEBUG("no space to alloc transmission state\n");
    return NULL;
}

static _transmission_t* _transmission_create(const unicoap_endpoint_t* endpoint,
                                            unicoap_packet_t* packet)
{
    assert(packet);

    unicoap_state_lock();
    _transmission_t* transmission = _transmission_alloc_unsafe();
    unicoap_state_unlock();
    if (!transmission) {
        return NULL;
    }
    transmission->id = _get_id(packet);

    /* need to store endpoint by value. transmission may outlive endpoint's lifetime. */
    transmission->endpoint = *endpoint; /* use compiler-built-in copy primitive */

    /* transmission stores session by value for retransmissions.
     * packet stores session by reference.
     * if we already have a session,
     * reuse that session here. If we don't have one, the DTLS driver will _initialize_ one.
     * we still have to store one by value */
    if (_packet_get_dtls_session(packet)) {
        /* got session, copy into transmission for later retransmission */
        _transmission_set_session(transmission, _packet_get_dtls_session(packet));
    }
    else {
        /* transmission is memset' to all zero in _transmission_free,
         * thus DTLS sessions stored by value is, too.
         * We didn't get an existing session (likely a client request, so no session exists),
         * so give the transport layer a transmission to initialize (remember, memset' to 0)
         * This also has the benefit not needing to copy the sessions into the transmission
         * afterwards, because the transport layer already initialized that memory region.
         * Sounds more complicated than it is. */
        _packet_set_dtls_session(packet, _transmission_get_session(transmission));
    }

    return transmission;
}

static _transmission_t* _transmission_find(const unicoap_endpoint_t* endpoint, uint16_t id)
{
    (void)endpoint;
    (void)id;
#if CONFIG_UNICOAP_RFC7252_TRANSMISSIONS_MAX > 0
    for (int i = 0; i < (int)ARRAY_SIZE(_state.transmissions); i += 1) {
        _transmission_t* transmission = &_state.transmissions[i];
        if (transmission->is_used &&
            transmission->id == id &&
            unicoap_endpoint_is_equal(&transmission->endpoint, endpoint)
        ) {
            return transmission;
        }
    }
#endif
    return NULL;
}

static inline void _transmission_free(_transmission_t* transmission)
{
    MESSAGING_7252_DEBUG(UNICOAP_MESSAGE_ID_FORMAT "transmission ended\n", transmission->id);
    if (transmission->pdu) {
        _carbon_copy_free(transmission->pdu);
    }
    unicoap_event_cancel(&transmission->ack_timeout);
    memset(transmission, 0, sizeof(_transmission_t));
}

static int _sendv(iolist_t* list, const unicoap_endpoint_t* remote, const unicoap_endpoint_t* local,
                  const unicoap_sock_dtls_session_t* dtls_session)
{
    (void)list;
    (void)local;
    (void)dtls_session;
    assert(list);
    assert(remote);

    switch (remote->proto) {
#if IS_USED(MODULE_UNICOAP_DRIVER_UDP)
    case UNICOAP_PROTO_UDP: {
        extern int unicoap_transport_sendv_udp(iolist_t * iolist, const sock_udp_ep_t* remote,
                                               const sock_udp_ep_t* local);
        return unicoap_transport_sendv_udp(
            list, unicoap_endpoint_get_udp((unicoap_endpoint_t*)remote),
            local ? unicoap_endpoint_get_udp((unicoap_endpoint_t*)local) : NULL);
    }
#endif

#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    case UNICOAP_PROTO_DTLS: {
        extern int unicoap_transport_sendv_dtls(iolist_t * iolist, const sock_udp_ep_t* remote,
                                                const sock_udp_ep_t* local,
                                                unicoap_sock_dtls_session_t* session);
        return unicoap_transport_sendv_dtls(
            list, unicoap_endpoint_get_udp((unicoap_endpoint_t*)remote),
            local ? unicoap_endpoint_get_udp((unicoap_endpoint_t*)local) : NULL,
            (unicoap_sock_dtls_session_t*)dtls_session);
    }
#endif

    default:
        MESSAGING_7252_DEBUG("unsupported protocol number\n");
        unicoap_assist_emit_diagnostic_missing_driver(remote->proto);
        return -ENOTSUP;
    }
}

static inline int _send(uint8_t* pdu, size_t size, const unicoap_endpoint_t* remote,
                        const unicoap_endpoint_t* local,
                        const unicoap_sock_dtls_session_t* dtls_session)
{
    iolist_t list = __IOLIST(pdu, size, NULL);
    return _sendv(&list, remote, local, dtls_session);
}

static ssize_t _build_and_send_pdu(unicoap_packet_t* packet, uint8_t* carbon_copy)
{
    assert(packet);
    assert(packet->message);
    assert(packet->remote);
    MESSAGING_7252_DEBUG("sending ");
    PACKET_7252_DEBUG(packet);
    DEBUG("\n");

    ssize_t res = 0;
    uint8_t header[UNICOAP_HEADER_SIZE_MAX];
    iolist_t lists[UNICOAP_PDU_IOLIST_COUNT];
    ssize_t size = 0;

    /* By default, we build a vector of CoAP PDU chunks. This saves another copy, as the network
     * backend is going to copy anyway. However, if we need a carbon copy to later retransmit
     * the very same message, it makes no sense to write the PDU into a vector, and to then copy all
     * vector chunks into a larger buffer. In this case, we write the PDU directly into a contiguous
     * buffer, the carbon copy buffer. To save space, we use the same _sendv function.
     * In the contiguous case, that vector has only one element. This is fine as we allocate
     * the iolists anyway. If we wanted to avoid this, we would need to dynamically allocate memory.
     */

    if (carbon_copy) {
        if ((size = unicoap_pdu_build_rfc7252(carbon_copy, sizeof(_state.carbon_copies[0]),
                                              packet->message, &packet->properties)) < 0) {
            lists->iol_base = NULL;
            lists->iol_len = 0;
            return size;
        }

        lists->iol_base = carbon_copy;
        lists->iol_len = size;
        lists->iol_next = NULL;
    } else {
        if ((size = unicoap_pdu_buildv_rfc7252(header, sizeof(header), packet->message,
                                               &packet->properties, &lists[0])) < 0) {
            return size;
        }
    }

    if ((res = _sendv(&lists[0], packet->remote, packet->local, _packet_get_dtls_session(packet))) < 0) {
        return res;
    }
    return size;
}

static inline void _handle_ack(const unicoap_endpoint_t* remote, uint16_t id)
{
    MESSAGING_7252_DEBUG(UNICOAP_MESSAGE_ID_FORMAT "received ACK, ", id);

    _transmission_t* transmission = _transmission_find(remote, id);
    if (transmission) {
        DEBUG("stopping retransmission\n");
        _transmission_free(transmission);
    }
    else {
        DEBUG("no message with ID, ignoring\n");
    }
}

static void _handle_reset(const unicoap_endpoint_t* remote, uint16_t id)
{
    MESSAGING_7252_DEBUG(UNICOAP_MESSAGE_ID_FORMAT "received RST", id);

    _transmission_t* transmission = _transmission_find(remote, id);

    if (transmission) {
        DEBUG("\n");
        _transmission_free(transmission);
    }
    else {
        DEBUG(", no message with ID, ignoring\n");
    }
}

static void _on_ack_timeout(event_t* _event)
{
    _transmission_t* transmission = container_of(_event, _transmission_t, ack_timeout.super);
    assert(transmission);
    assert(transmission->pdu);
    assert(transmission->pdu_size > 0);
    int res = 0;

    if (transmission->remaining_retransmissions == 0) {
        MESSAGING_7252_DEBUG(UNICOAP_MESSAGE_ID_FORMAT
                             "ACK timeout, max retransmissions exceeded\n", transmission->id);
        res = -ETIMEDOUT;
        goto error;
    }

    /* reduce retries remaining, double timeout and resend */
    transmission->remaining_retransmissions -= 1;
    unsigned int i = CONFIG_UNICOAP_RETRANSMISSIONS_MAX - transmission->remaining_retransmissions;
    uint32_t duration = (uint32_t)CONFIG_UNICOAP_TIMEOUT_ACK_MS << i;
    if (CONFIG_UNICOAP_RANDOM_FACTOR_1000 > 1000) {
        duration = random_uint32_range(duration, UNICOAP_TIMEOUT_ACK_RANGE_UPPER << i);
    }

    MESSAGING_7252_DEBUG(UNICOAP_MESSAGE_ID_FORMAT
                         "ACK timeout, retransmitting now, waiting %" PRIu32 " ms, "
                         "%u attempts remaining\n",
                         transmission->id, duration, transmission->remaining_retransmissions);

    if ((res = _send(transmission->pdu, transmission->pdu_size,
                     _transmission_get_endpoint(transmission), NULL,
                     _transmission_get_session(transmission))) < 0) {
        goto error;
    }

    unicoap_event_reschedule(&transmission->ack_timeout, duration);
    return;

error:
    /* TODO: Client: Signal failure to application waiting for response */
    MESSAGING_7252_DEBUG("error while on ACK timeout\n");
    return;
}

/* MARK: - Empty Messages */

static int _send_empty_message(unicoap_packet_t* packet)
{
    packet->message->code = UNICOAP_CODE_EMPTY;
    packet->message->options = NULL;
    packet->message->payload = NULL;
    packet->message->payload_size = 0;

    packet->properties.token = NULL;
    packet->properties.token_length = 0;

    return (int)_build_and_send_pdu(packet, NULL);
}

static inline int _acknowledge(unicoap_packet_t* packet)
{
    MESSAGING_7252_DEBUG(UNICOAP_MESSAGE_ID_FORMAT "sending empty ACK\n", _get_id(packet));
    _set_type(packet, UNICOAP_TYPE_ACK);
    return _send_empty_message(packet);
}

static inline int _reset(unicoap_packet_t* packet)
{
    MESSAGING_7252_DEBUG(UNICOAP_MESSAGE_ID_FORMAT "sending RST\n", _get_id(packet));
    _set_type(packet, UNICOAP_TYPE_RST);
    return _send_empty_message(packet);
}

/* MARK: - Message Processing */

#define _IGNORED (-7252)

static int _process_messaging_layer(unicoap_packet_t* packet)
{
    assert(packet);
    assert(packet->remote);
    assert(packet->message);

    MESSAGING_7252_DEBUG("received ");
    PACKET_7252_DEBUG(packet);
    DEBUG("\n");

    unicoap_message_t* message = packet->message;

    /* RFC 7252 (CoAP), Section 4.2
     * https://datatracker.ietf.org/doc/html/rfc7252#section-4.2
     * Rejecting an
     * Acknowledgement or Reset message (including the case where the
     * Acknowledgement carries a request or a code with a reserved class, or
     * the Reset message is not Empty) is effected by silently ignoring it.
     * More generally, recipients of Acknowledgement and Reset messages MUST
     * NOT respond with either Acknowledgement or Reset messages.
     */
    if (message->code == UNICOAP_CODE_EMPTY && message->payload_size != 0) {
        MESSAGING_7252_DEBUG("received code 0.00, but message not empty, ignoring\n");
        return -EPROTO;
    }

    switch (_get_type(packet)) {
    case UNICOAP_TYPE_ACK:
        _handle_ack(packet->remote, _get_id(packet));
        if (message->code == UNICOAP_CODE_EMPTY) {
            return _IGNORED;
        }

        else if (!unicoap_message_is_response(message->code)) {
            MESSAGING_7252_DEBUG(
                UNICOAP_MESSAGE_ID_FORMAT
                "received ACK, expected code 0.00 or response code, got "
                UNICOAP_CODE_CLASS_DETAIL_FORMAT
                ", ignoring\n",
                _get_id(packet),
                unicoap_code_class(message->code),
                unicoap_code_detail(message->code)
            );
            return -EPROTO;
        }
        break;

    case UNICOAP_TYPE_RST:
        _handle_reset(packet->remote, _get_id(packet));
        if (message->code != UNICOAP_CODE_EMPTY) {
            MESSAGING_7252_DEBUG(
                UNICOAP_MESSAGE_ID_FORMAT
                "received RST, expected 0.00, got "
                UNICOAP_CODE_CLASS_DETAIL_FORMAT
                ", ignoring\n",
                _get_id(packet),
                unicoap_code_class(message->code),
                unicoap_code_detail(message->code)
            );
            return -EPROTO;
        }

        /* if this RST is due to the client not being interested
         * in receiving observe notifications anymore, deregister */
        /* TODO: Observe: Deregister by message ID */
        return _IGNORED;

    case UNICOAP_TYPE_CON:
        if (message->code == UNICOAP_CODE_EMPTY) {
            MESSAGING_7252_DEBUG(UNICOAP_MESSAGE_ID_FORMAT
                                 "received empty CON (ping), sending RST (pong)\n",
                                 _get_id(packet));
            _reset(packet);
            return _IGNORED;
        }
        break;

    case UNICOAP_TYPE_NON:
        if (message->code == UNICOAP_CODE_EMPTY) {
            MESSAGING_7252_DEBUG(UNICOAP_MESSAGE_ID_FORMAT "received empty NON, ignoring\n", _get_id(packet));
            return _IGNORED;
        }
        break;
    }
    return 0;
}

int unicoap_messaging_process_rfc7252(const uint8_t* pdu, size_t size, bool truncated,
                                      unicoap_packet_t* packet)
{
    unicoap_options_t options = { 0 };
    unicoap_message_t message = { .options = &options };
    packet->message = &message;

    int res = 0;

    if ((res = unicoap_pdu_parse_rfc7252((uint8_t*)pdu, size, &message, &packet->properties)) < 0) {
        PDU_7252_DEBUG("parsing error: %i (%s)\n", res, strerror(-res));
        return res;
    }

    if ((res = _process_messaging_layer(packet)) < 0) {
        if (res != _IGNORED) {
            MESSAGING_7252_DEBUG("messaging error: %i (%s)\n", res, strerror(-res));
        }
        return res;
    }

    /* Successfully parsed message, i.e., truncated message could be parsed to
     * to a certain degree (cutoff directly after an option or somewhere in
     * payload). The exchange (request/response) layer will discard that
     * message anyway, but in case of truncated requests, the next layer will
     * at least send a Size1 response indicating the request was too large
     * and should've been transmitted block-wise.
     *
     * If the exchange layer is indeed incapable of handling a truncated
     * message, we'll be notified via the event callback.
     */
    unicoap_exchange_arg_t arg;
    unicoap_messaging_flags_t flags;

    switch ((res = unicoap_exchange_preprocess(packet, &flags, &arg, truncated))) {
    case UNICOAP_PREPROCESSING_SUCCESS_REQUEST:
        break;

    /* This is a response the exchange layer expected.
     * We may have attached a transmission, free that transmission here. */
    case UNICOAP_PREPROCESSING_SUCCESS_RESPONSE: {
        _transmission_t* transmission = _transmission_find(packet->remote, _get_id(packet));
        if (transmission) {
            _transmission_free(transmission);

            if (_get_type(packet) == UNICOAP_TYPE_CON) {
                MESSAGING_7252_DEBUG(UNICOAP_MESSAGE_ID_FORMAT
                                     "sending empty ACK for expected response\n", _get_id(packet));

                /* We're goint to need the response below, don't override it. */
                unicoap_message_t* message = packet->message;
                unicoap_message_t m = *packet->message;
                packet->message = &m;
                _acknowledge(packet);
                packet->message = message;
            }
        }
        break;
    }

    /* From RFC 7641, Section 3.6. Cancellation:
     * A client that is no longer interested in receiving notifications for
     * a resource can simply "forget" the observation.  When the server then
     * sends the next notification, the client will not recognize the token
     * in the message and thus will return a Reset message.  This causes the
     * server to remove the associated entry from the list of observers.
     * The entries in lists of observers are effectively "garbage collected"
     * by the server. */
    case UNICOAP_PREPROCESSING_ERROR_TRUNCATED:
    case UNICOAP_PREPROCESSING_ERROR_NOTIFICATION_UNEXPECTED:
        /* In the case of a non-confirmable notification,
         * rejecting the message with a Reset message is OPTIONAL. */
        goto reset;

    case UNICOAP_PREPROCESSING_ERROR_RESPONSE_UNEXPECTED:
        MESSAGING_7252_DEBUG(UNICOAP_MESSAGE_ID_FORMAT "received unknown response\n", _get_id(packet));
        /* From RFC 7252, Section 4.3 Messages Transmitted without Reliability
         * A Non-confirmable message always carries either a request or response and
         * MUST NOT be Empty.  A Non-confirmable message MUST NOT be
         * acknowledged by the recipient.  A recipient MUST reject the message
         * if it lacks context to process the message properly, including the
         * case where the message is Empty, uses a code with a reserved class
         * (1, 6, or 7), or has a message format error.  Rejecting a Non-
         * confirmable message MAY involve sending a matching Reset message, and
         * apart from the Reset message the rejected message MUST be silently
         * ignored. */
        /* unicoap chooses not to send a reset message for NONs */
        if (_get_type(packet) == UNICOAP_TYPE_CON) {
            /* From RFC 7252, Section 4.2 Messages Transmitted Reliably
             * Confirmable message
             * always carries either a request or response, unless it is used only
             * to elicit a Reset message, in which case it is Empty.  A recipient
             * MUST either (a) acknowledge a Confirmable message with an
             * Acknowledgement message or (b) reject the message if the recipient
             * lacks context to process the message properly, [...]  */
            goto reset;
        }
        return -EPROTO;

    default:
        if (res < 0) {
            return res;
        }
        break;
    }

    /* processor is prewarmed, let's throw the message in the processor */
    if ((res = unicoap_exchange_process(packet, arg)) < 0) {
        return res;
    }

    return 0;

reset:
    /* using gotos here to avoid overhead of multiple identical function calls */
    _reset(packet);
    return -EPROTO;
}

/* MARK: - Message Transmission */

static void _format_separate(unicoap_packet_t* packet, unicoap_messaging_flags_t flags)
{
    /* requests and separate responses */
    _set_id(packet, _generate_message_id());
    _set_type(packet,
              flags & UNICOAP_MESSAGING_FLAG_RELIABLE ? UNICOAP_TYPE_CON : UNICOAP_TYPE_NON);
}

int unicoap_messaging_send_rfc7252(unicoap_packet_t* packet, unicoap_messaging_flags_t flags)
{
    assert(packet);
    assert(packet->remote);
    int res = 0;
    uint8_t* carbon_copy = NULL;
    size_t* carbon_copy_size = NULL;
    _transmission_t* transmission = NULL;

    if (packet->properties.is_notification) {
        /* notification, always use separate response style */
        _format_separate(packet, flags);
    }
    else if (unicoap_message_is_response(packet->message->code) &&
             _get_type(packet) == UNICOAP_TYPE_CON) {
        /* piggybacked response, immediate response style */
        _set_type(packet, UNICOAP_TYPE_ACK);
    }
    else {
        _format_separate(packet, flags);
    }

    switch (_get_type(packet)) {
    case UNICOAP_TYPE_NON:
        /* TODO: Client: remember message ID and watch out for RSTs */
        break;

    case UNICOAP_TYPE_CON:
        /* If we send a confirmable message, we always need a transmission
         * for retransmitting the original PDU and for tracking ACK timeouts
         * (exponential back-off mechanism). */
        transmission = _transmission_create(packet->remote, packet);

        /* need a carbon copy buffer for storing the PDU copy for retransmission */
        if (!(carbon_copy = _carbon_copy_alloc())) {
            res = -ENOBUFS;
            goto error;
        }

        transmission->remaining_retransmissions = CONFIG_UNICOAP_RETRANSMISSIONS_MAX;
        transmission->pdu = carbon_copy;

        /* need to know the total size of the PDU we are going to build
         * down below, PDU builder writes size into this location */
        carbon_copy_size = &transmission->pdu_size;

        uint32_t duration = CONFIG_UNICOAP_TIMEOUT_ACK_MS;
        if (CONFIG_UNICOAP_RANDOM_FACTOR_1000 > 1000) {
            duration = random_uint32_range(duration, UNICOAP_TIMEOUT_ACK_RANGE_UPPER);
        }
        unicoap_event_schedule(&transmission->ack_timeout, _on_ack_timeout, duration);
        break;

    case UNICOAP_TYPE_ACK:
        break;

    default:
        MESSAGING_7252_DEBUG("attempt to send unexpected message %s",
                             unicoap_string_from_rfc7252_type(_get_type(packet)));
        assert(false);
        break;
    }

    if ((res = (int)_build_and_send_pdu(packet, carbon_copy)) < 0) {
        goto error;
    }

    if (carbon_copy_size) {
        MESSAGING_7252_DEBUG("<carbon_copy size=%i>\n", res);
        *carbon_copy_size = res;
    }
    return 0;

error:
    MESSAGING_7252_DEBUG("sending failed\n");
    if (transmission) {
        _transmission_free(transmission);
    }
    return res;
}

void unicoap_messaging_print_rfc7252_state(void)
{
#if ENABLE_DEBUG
    printf("\n\t- RFC 7252 transmissions (%" PRIuSIZE " total):\n",
           (size_t)CONFIG_UNICOAP_RFC7252_TRANSMISSIONS_MAX);

#  if CONFIG_UNICOAP_RFC7252_TRANSMISSIONS_MAX > 0
    for (size_t i = 0; i < ARRAY_SIZE(_state.transmissions); i += 1) {
        _transmission_t* transmission = &_state.transmissions[i];
        printf("\t\t- transmission #%" PRIuSIZE "\n", i);

        if (!transmission->is_used) {
            continue;
        }

        printf("\t\t\t- MID=%" PRIu16 "\n", transmission->id);
        printf("\t\t\t- remaining_retransmissions=%u\n", transmission->remaining_retransmissions);
        printf("\t\t\t- pdu=<carbon_copy at %p>\n", transmission->pdu);
        printf("\t\t\t- pdu_size=%" PRIuSIZE "\n", transmission->pdu_size);
    }
#  endif

    printf("\n\t- RFC 7252 carbon copies (%" PRIuSIZE " total):\n",
           (size_t)CONFIG_UNICOAP_CARBON_COPIES_MAX);

#  if CONFIG_UNICOAP_CARBON_COPIES_MAX > 0
    for (size_t i = 0; i < ARRAY_SIZE(_state.carbon_copies); i += 1) {
        printf("\t\t- carbon_copy #%" PRIuSIZE " capacity=%" PRIuSIZE "B used=%u\n", i,
               (size_t)CONFIG_UNICOAP_PDU_SIZE_MAX, *_state.carbon_copies[i] != 0);
    }
#  endif
#endif
}
