/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap
 * @brief   Core functionality
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <errno.h>

#include "modules.h"
#include "private/state.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

/* MARK: Layered notification system */

void unicoap_exchange_notify(void* state, unicoap_layer_notification_t type, void* arg)
{
    (void)arg;
    if (!state) {
        return;
    }
    unicoap_memo_t* memo = (unicoap_memo_t*)state;
    if (memo->endpoint.proto == UNICOAP_PROTO_UNSPECIFIED) {
        _STATE_NOTIF_DEBUG("use of released state obj\n");
        return;
    }
    if (type & UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE) {
        _STATE_NOTIF_DEBUG("messaging layer encountered error %i (type %i)\n",
                           unicoap_layer_notification_async_failure_to_errno(type), type);
#if UNICOAP_HAVE_MESSAGING_STATE
        memo->messaging.state = NULL;
#endif
    }
    else if (type == UNICOAP_LAYER_NOTIFICATION_STATE_RELEASE) {
        _STATE_NOTIF_DEBUG("messaging layer released state (type %i)\n", type);
#if UNICOAP_HAVE_MESSAGING_STATE
        memo->messaging.state = NULL;
#endif
    }
    else if (type == UNICOAP_LAYER_NOTIFICATION_STATE_ALLOC) {
        _STATE_NOTIF_DEBUG("messaging layer allocated state (type %i)\n", type);
        assert(arg);
#if UNICOAP_HAVE_MESSAGING_STATE
        memo->messaging.state = arg;
#endif
    }

    if (IS_USED(MODULE_UNICOAP_CLIENT) && unicoap_memo_is_client(memo)) {
        if (type & UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE) {
            unicoap_client_callback_failure(unicoap_client_memo_of_super(memo),
                                            unicoap_layer_notification_async_failure_to_errno(type));

            /* Messaging layer failed, our signal to release state. Error occurred on messaging
             * layer and not on exchange layer, so use 0 instead of error number here. */
            unicoap_client_memo_free(unicoap_client_memo_of_super(memo), 0);
        }
    }
}

void unicoap_exchange_notify_all(const unicoap_endpoint_t* endpoint, unicoap_layer_notification_t type, void* arg)
{
    if (IS_USED(MODULE_UNICOAP_SERVER)) {
        unicoap_exchange_notify_all_server(endpoint, type, arg);
    }
    if (IS_USED(MODULE_UNICOAP_CLIENT)) {
        unicoap_exchange_notify_all_client(endpoint, type, arg);
    }
}

void unicoap_messaging_notify(void* state, unicoap_layer_notification_t type, void* arg, unicoap_proto_t proto)
{
    assert(proto != UNICOAP_PROTO_UNSPECIFIED);
    if (!state) {
        return;
    }
    (void)arg;
    (void)type;
    switch (proto) {
#if IS_USED(MODULE_UNICOAP_DRIVER_RFC7252_COMMON)
    case UNICOAP_PROTO_UDP:
    case UNICOAP_PROTO_DTLS: {
        extern void unicoap_messaging_notify_rfc7252(void* state, unicoap_layer_notification_t type);
        return unicoap_messaging_notify_rfc7252(state, type);
    }
#endif
    default:
        break;
    }
}

/* MARK: - Driver dispatch */

static void _debug_packet(const unicoap_packet_t* packet)
{
    DEBUG("in channel:");

    DEBUG("\n\tremote=");
    DEBUG_ENDPOINT(packet->remote);

    if (packet->local) {
        DEBUG("\n\tlocal=");
        DEBUG_ENDPOINT(packet->local);
    }

    DEBUG("\n");
}

int unicoap_messaging_send(unicoap_packet_t* packet, unicoap_messaging_flags_t flags, void* exchange)
{
    (void)packet;
    (void)flags;
    (void)exchange;

    assert(packet);
    assert(packet->remote);
    assert(packet->message);

    _MESSAGING_DEBUG("sending ");
    _debug_packet(packet);

    switch (unicoap_packet_proto(packet)) {
#if IS_USED(MODULE_UNICOAP_DRIVER_RFC7252_COMMON)
    case UNICOAP_PROTO_UDP:
    case UNICOAP_PROTO_DTLS:
    case UNICOAP_PROTO_SLIPMUX:
        return unicoap_messaging_send_rfc7252(packet, flags, exchange);
#endif
    /* MARK: unicoap_driver_extension_point */
    default:
        _MESSAGING_DEBUG("missing driver for proto %s\n",
                         unicoap_string_from_proto(unicoap_packet_proto(packet)));
        unicoap_assist_emit_diagnostic_missing_driver(packet->remote->proto);
        return -ENOTSUP;
    }
}

unicoap_preprocessing_result_t unicoap_exchange_preprocess(unicoap_packet_t* packet,
                                                           unicoap_messaging_flags_t* flags,
                                                           unicoap_exchange_arg_t* arg,
                                                           bool truncated)
{
    *flags = UNICOAP_MESSAGING_FLAGS_DEFAULT;
    unicoap_message_t* message = packet->message;

    _MESSAGING_DEBUG("received ");
    _debug_packet(packet);

    switch (unicoap_code_class(packet->message->code)) {
    case UNICOAP_CODE_CLASS_REQUEST:
        return IS_USED(MODULE_UNICOAP_SERVER) ?
                   unicoap_server_preprocess_request(packet, flags, arg, truncated) :
                   UNICOAP_PREPROCESSING_ERROR_UNSUPPORTED;

    case UNICOAP_CODE_CLASS_RESPONSE_SUCCESS:
    case UNICOAP_CODE_CLASS_RESPONSE_CLIENT_FAILURE:
    case UNICOAP_CODE_CLASS_RESPONSE_SERVER_FAILURE:
        return IS_USED(MODULE_UNICOAP_CLIENT) ?
                   unicoap_client_preprocess_response(packet, flags, arg, truncated) :
                   UNICOAP_PREPROCESSING_ERROR_UNSUPPORTED;

    case UNICOAP_CODE_CLASS_SIGNAL:
        /* TODO: Signaling */
        return UNICOAP_PREPROCESSING_ERROR_UNSUPPORTED;

    default:
        _MESSAGING_DEBUG("illegal code class %" PRIu8 "\n", unicoap_code_class(message->code));
        return UNICOAP_PREPROCESSING_ERROR_INVALID_CODE_CLASS;
    }
}

int unicoap_exchange_process(unicoap_packet_t* packet, unicoap_exchange_arg_t arg)
{
    switch (unicoap_code_class(packet->message->code)) {
    case UNICOAP_CODE_CLASS_REQUEST:
        return IS_USED(MODULE_UNICOAP_SERVER) ?
                   unicoap_server_process_request(packet, arg.resource) :
                   -ENOTSUP;

    case UNICOAP_CODE_CLASS_RESPONSE_SUCCESS:
    case UNICOAP_CODE_CLASS_RESPONSE_CLIENT_FAILURE:
    case UNICOAP_CODE_CLASS_RESPONSE_SERVER_FAILURE:
        return IS_USED(MODULE_UNICOAP_CLIENT) ?
                   unicoap_client_process_response(packet, arg.client) :
                   -ENOTSUP;

    case UNICOAP_CODE_CLASS_SIGNAL:
        /* TODO: Signaling */
        return -1;

    default:
        UNREACHABLE();
        return -1;
    }
}
