/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap_client
 * @brief   Client implementation
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <string.h>
#include <errno.h>

#include "ztimer.h"
#include "mutex.h"
#include "compiler_hints.h"

#if IS_USED(MODULE_DNS)
#  include "net/dns.h"
#endif

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

static void _on_response_timeout(unicoap_scheduled_event_t* timeout) {
    _CLIENT_DEBUG("response timeout\n");
    unicoap_client_callback_failure(unicoap_client_memo_of_timeout(timeout), -ETIMEDOUT);
    unicoap_client_memo_free(unicoap_client_memo_of_timeout(timeout));
}

int unicoap_client_callback_success(unicoap_client_memo_t* memo, const unicoap_packet_t* packet,
                            unicoap_block_option_t block) {
    (void)block;
    _CLIENT_DEBUG("invoking callback with " UNICOAP_CODE_CLASS_DETAIL_FORMAT " response\n",
                 unicoap_code_class(packet->message->code),
                 unicoap_code_detail(packet->message->code));

    unicoap_callback_t callback = memo->callback;
    void* arg = memo->callback_arg;

    unicoap_aux_t aux = { .remote = packet->remote,
                          .local = packet->local,
                          .properties = &packet->properties };

    /* TODO: Block-wise callback */
    if (callback.response) {
        return callback.response(packet->message, &aux, 0, arg);
    }
    return 0;
}

void unicoap_client_callback_failure(unicoap_client_memo_t* memo, int error) {
    assert(error < 0);
    _CLIENT_DEBUG("invoking callback with error: %i (%s)\n", error, strerror(-error));
    unicoap_callback_t callback = memo->callback;
    void* arg = memo->callback_arg;
    /* TODO: Block-wise callback */
    if (callback.response) {
        (void)callback.response(NULL, NULL, error, arg);
    }
}

/* TODO: Block-wise processing */

int unicoap_client_process_response(unicoap_packet_t* packet, unicoap_client_memo_t* memo) {
    int res = 0;

    /* TODO: Block-wise */
    res = unicoap_client_callback_success(memo, packet, UNICOAP_BLOCK_OPTION_NONE);

    unicoap_client_memo_free(memo);
    return res;
}

int unicoap_client_send_request_part(unicoap_packet_t* packet, unicoap_client_memo_t* memo,
                                     unicoap_client_flags_t client_flags) {
    int res = 0;

    if (memo) {
        // Instead of a temporary buffer, it's fine to reference the memo token buffer
        // as the memo outlives or lives as long as the packet.
        packet->properties.token = memo->token;
    }
    assert(packet->properties.token);
    unicoap_generate_token(packet->properties.token);
    packet->properties.token_length = CONFIG_UNICOAP_GENERATED_TOKEN_LENGTH;
    unicoap_messaging_flags_t messaging_flags = _messaging_flags_client(client_flags);
    if (memo) {
        /* State has been allocated, instruct the messaging layer to track success/failures
         * with this transmission. If a NON is sent, this flag tells the RFC 7252 driver
         * to allocate state to watch for RSTs. */
        messaging_flags |= UNICOAP_MESSAGING_FLAG_TRACK;
    }
    /* TODO: OSCORE */
    if ((res = unicoap_messaging_send(packet, messaging_flags, &memo->super)) < 0) {
        _CLIENT_DEBUG("error: could not send message\n");
        return res;
    }
    return 0;
}

int unicoap_client_send_request_body(unicoap_message_t* request,
                                     unicoap_endpoint_t* endpoint,
                                     unicoap_callback_t callback, void* callback_arg,
                                     unicoap_client_flags_t flags)
{
    assert(request);
    assert(endpoint);
    int res = 0;
    unicoap_client_memo_t* memo = NULL;

    /* TODO: Block-wise */
    /* TODO: Observe */
    uint8_t token[CONFIG_UNICOAP_GENERATED_TOKEN_LENGTH];
    unicoap_packet_t packet = { .remote = endpoint,
                                .message = request,
                                .properties = {
                                    .token = token,
                                    .token_length = sizeof(token),
                                } };

    if (unicoap_callback_is_present(callback)) {
        _CLIENT_DEBUG("need a memo\n");
        if (!(memo = unicoap_client_memo_create(endpoint))) {
            return -ENOBUFS;
        }
        memo->callback = callback;
        memo->callback_arg = callback_arg;
        memo->flags = flags;

        unicoap_event_schedule(&memo->super.exchange.timeout, _on_response_timeout,
                               CONFIG_UNICOAP_TIMEOUT_CLIENT_RESPONSE_MS);
    }
    /* TODO: OSCORE */
    if ((res = unicoap_client_send_request_part(&packet, memo, flags)) < 0) {
        goto error;
    }
    if (memo) {
        return unicoap_client_memo_assign_refno(memo);
    }
    return 0;

error:
    unicoap_client_memo_free(memo);
    return res;
}

int unicoap_cancel_request(int refno) {
    if (IS_USED(MODULE_UNICOAP_CLIENT_CANCELLATION)) {
        if (refno <= 0) {
            return -EINVAL;
        }
        unicoap_client_memo_t* memo = unicoap_client_memo_find_refno(refno);
        if (!memo) {
            _CLIENT_DEBUG("no request with refno %i\n", refno);
            return -ENOENT;
        }
        _CLIENT_DEBUG("cancelling request with refno %i\n", refno);
        unicoap_client_callback_failure(memo, -ECANCELED);
        unicoap_client_memo_free(memo);
        return 0;
    } else {
        if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) {
            unicoap_assist(API_MISUSE("request cancellation not supported")
                               FIXIT("enable unicoap_client_cancellation"));
        }
        return -ENOTSUP;
    }
}

/* MARK: - URI */

static int _open_request(unicoap_message_t* request,
                          unicoap_destination_t* destination,
                          unicoap_callback_t callback, void* callback_arg,
                          unicoap_client_flags_t flags)
{
    assert(request);
    assert(destination);
    int res = 0;
    switch (destination->type) {
    case UNICOAP_DESTINATION_URI:
        if (IS_USED(MODULE_UNICOAP_CLIENT_URI)) {
            if (!request->options) {
                UNICOAP_OPTIONS_ALLOCA(options, CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY);
                request->options = options;
            }
            unicoap_endpoint_t endpoint = { 0 };
            assert(uri_parser_is_absolute(destination->remote.uri, 
                strlen(destination->remote.uri)));

            uri_parser_result_t parsed = { 0 };
            if ((res = uri_parser_process(
                &parsed, destination->remote.uri, strlen(destination->remote.uri)
            )) < 0) {
                _URI_DEBUG("URI malformed: %i (%s)\n", res, strerror(-res));
                return res;
            }
            if (!parsed.scheme) {
                _URI_DEBUG("URI lacking scheme?!\n");
                return -EINVAL;
            }

            if ((res = unicoap_uri_populate(&parsed, &endpoint, request->options)) < 0) {
                return res;
            }
            return unicoap_client_send_request_body(request, &endpoint, callback,
                                                    callback_arg, flags);
        } else {
            unicoap_assist(API_ERROR("URI passed, but module is missing")
                           FIXIT("add USEMODULE += unicoap_client_uri"));
            assert(false);
            return -ENOTSUP;
        }
    case UNICOAP_DESTINATION_HOST: {
#if IS_USED(MODULE_DNS)
        unicoap_endpoint_t endpoint = { ._tl_ep = { .port = UNICOAP_DEFAULT_COAP_PORT } };
        // FIXME: SVCB?
        if ((res = dns_query(destination->remote.host, &endpoint._tl_ep, 0)) < 0) {
            _CLIENT_DEBUG("DNS resolution of '%s' failed: %i\n", destination->remote.host, res);
            return res;
        }
        return unicoap_client_send_request_body(request, &endpoint, callback,
                                                callback_arg, flags);
#else  /* IS_USED(MODULE_DNS) */
        unicoap_assist(API_ERROR("cannot resolve FQDN, dns module missing")
                       FIXIT("add USEMODULE += dns"));
        assert(false);
        return -ENOTSUP;
#endif /* IS_USED(MODULE_DNS) */
    }
    case UNICOAP_DESTINATION_ENDPOINT:
        return unicoap_client_send_request_body(
            request, destination->remote.endpoint, callback, callback_arg, flags);
    default:
        _CLIENT_DEBUG("illegal destination type %" PRIu8 "\n", destination->type);
        assert(false);
        return -EINVAL;
    }
}

/* MARK: - Sync API */

typedef struct {
    mutex_t roadblock;
    unicoap_message_t* response;
    unicoap_aux_t* aux;
    int res;
} _sync_copy_args_t;

static int _copy_callback(const unicoap_message_t* response, const unicoap_aux_t* aux, int error,
                          void* _arg
) {
    _sync_copy_args_t *args = _arg;
    if (error) {
        goto out;
    }

    args->response->code = response->code;

    unicoap_options_t* dest_options = args->response->options;
    uint8_t* dest_payload = args->response->payload;
    size_t dest_payload_capacity = args->response->payload_size;
    assert(args->response->payload_representation == UNICOAP_PAYLOAD_CONTIGUOUS);

    if (response->options) {
        if (!dest_options) {
            _CLIENT_DEBUG("no options buffer provided to copy response options\n");
            error = -ENOBUFS;
            goto out;
        }

        if (unicoap_options_size(response->options) > response->options->storage_capacity) {
            _CLIENT_DEBUG("no buffer space to copy options, " _UNICOAP_NEED_HAVE "\n",
                     unicoap_options_size(response->options), dest_options->storage_capacity);
            error = -ENOBUFS;
            goto out;
        }

        uint8_t* storage = dest_options->entries->data;
        size_t capacity = dest_options->storage_capacity;
        *dest_options = *response->options;
        unicoap_options_swap_storage(dest_options, storage, capacity);
    }

    if (response->payload) {
        if (!args->response->payload) {
            _CLIENT_DEBUG("no payload buffer provided\n");
            error = -ENOBUFS;
            goto out;
        }

        if (args->response->payload_size < response->payload_size) {
            _CLIENT_DEBUG("no buffer space to copy payload, " _UNICOAP_NEED_HAVE "\n",
                     response->payload_size, dest_payload_capacity);
        }
        memcpy(dest_payload, response->payload, response->payload_size);
    }

    if (args->aux) {
        if (args->aux->local && aux->local) {
            *(unicoap_endpoint_t*)args->aux->local = *aux->local;
        }
        if (args->aux->remote && aux->remote) {
            *(unicoap_endpoint_t*)args->aux->remote = *aux->remote;
        }
        if (args->aux->properties && aux->properties) {
            *(unicoap_message_properties_t*)args->aux->properties = *aux->properties;
        }
    }

out:
    mutex_unlock(&args->roadblock);
    return error;
}

int unicoap_send_request_sync_copy(unicoap_message_t* request,
                                   unicoap_destination_t* destination,
                                   unicoap_message_t* response, unicoap_client_flags_t flags,
                                   unicoap_aux_t* aux)
{
    assert(response);
    assert(response);

    /* Prevent this function from deadlocking the unicoap thread. No one besides the unicoap
     * thread can unlock the mutex below (via the _sync_callback). You cannot have
     * a function that waits for the response to arrive while blocking the very same
     * thread would process the response.
     *
     * This is particularly true for applications running unicoap in sync mode. */
    if (thread_getpid() == _unicoap_pid) {
        if (IS_ACTIVE(CONFIG_UNICOAP_ASSIT)) {
            unicoap_assist(API_MISUSE("Attempted to call blocking '%s' on unicoap thread, "
                                      "would block processing loop")
                               FIXIT("Use 'unicoap_send_request_async' instead"),
                           __func__);
        }
        _CLIENT_DEBUG("Attempted to open request on unicoap thread, not blocking.");
        assert(false);
        return -1;
    }
    
    _sync_copy_args_t args = (_sync_copy_args_t) {
        .response = response,
        .aux = aux,
        .roadblock = MUTEX_INIT_LOCKED
    };

    int res = _open_request(request, destination,
        (unicoap_callback_t) { .response = _copy_callback }, &args, flags);

    if (res < 0) {
        return res;
    }
    /* Block until callback calls unlock. */
    mutex_lock(&args.roadblock);
    return args.res;
}

typedef struct {
    unicoap_response_callback_t callback;
    void *caller_arg;
    int res;
    mutex_t roadblock;
} _sync_args_t;

static int _sync_callback(const unicoap_message_t *response, const unicoap_aux_t *aux, int error,
                          void *args)
{
    _sync_args_t* a = (_sync_args_t*)args;
    a->res = a->callback(response, aux, error, a->caller_arg);
    mutex_unlock(&a->roadblock);
    return 0;
}

int unicoap_send_request_sync(unicoap_message_t* request,
                              unicoap_destination_t* destination,
                              unicoap_response_callback_t callback, void* callback_arg,
                              unicoap_client_flags_t flags)
{
    /* Prevent this function from deadlocking the unicoap thread. No one besides the unicoap
     * thread can unlock the mutex below (via the _sync_callback). You cannot have
     * a function that waits for the response to arrive while blocking the very same
     * thread would process the response.
     *
     * This is particularly true for applications running unicoap in sync mode. */
    if (thread_getpid() == _unicoap_pid) {
        if (IS_ACTIVE(CONFIG_UNICOAP_ASSIT)) {
            unicoap_assist(API_MISUSE("Attempted to call blocking '%s' on unicoap thread, "
                                      "would block processing loop")
                               FIXIT("Use 'unicoap_send_request_async' instead"),
                           __func__);
        }
        _CLIENT_DEBUG("Attempted to open request on unicoap thread, not blocking.");
        assert(false);
        return -1;
    }

    _sync_args_t args = { 
        .callback = callback, 
        .caller_arg = callback_arg, 
        .roadblock = MUTEX_INIT_LOCKED 
    };

    int res = _open_request(request, destination, 
        (unicoap_callback_t) { .response = _sync_callback }, &args, flags);

    if (res < 0) {
        return res;
    }
    /* Block until callback calls unlock. */
    mutex_lock(&args.roadblock);
    return args.res;
}

int unicoap_send_request_async(unicoap_message_t* request,
                               unicoap_destination_t* destination,
                               unicoap_response_callback_t callback, void* callback_arg,
                               unicoap_client_flags_t flags) {
    return _open_request(request, destination, 
        (unicoap_callback_t) { .response = callback }, callback_arg, flags);
}
