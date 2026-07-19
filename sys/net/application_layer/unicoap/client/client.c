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
#include <alloca.h>

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
    _UNICOAP_CHECKPOINT;

    /* TODO: Block-wise */
    res = unicoap_client_callback_success(memo, packet, UNICOAP_BLOCK_OPTION_NONE);
    _UNICOAP_CHECKPOINT;

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
    packet->properties.token_length = sizeof(memo->token);
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

int unicoap_client_cancel(int refno) {
    if (IS_ACTIVE(CONFIG_UNICOAP_CLIENT_CANCELLABLE)) {
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
                               FIXIT("enable CONFIG_UNICOAP_CLIENT_CANCELLABLE"));
        }
        return -ENOTSUP;
    }
}

/* MARK: - URI */

int _unicoap_open_request(unicoap_message_t* request,
                          unicoap_destination_t* destination,
                          void* _callback, void* callback_args,
                          unicoap_client_flags_t flags)
{
    assert(request);
    assert(destination);
    unicoap_callback_t callback = { ._any = _callback };
    int res = 0;
    switch (destination->type) {
    case UNICOAP_DESTINATION_URI:
        if (IS_USED(MODULE_UNICOAP_CLIENT_URI)) {
            if (!request->options) {
                request->options = (unicoap_options_t *)alloca(sizeof(unicoap_options_t));
                memset(request->options, 0, sizeof(unicoap_options_t));
            }
            if (request->options->storage_capacity == 0) {
                /* Uri-Path and Uri-Host */
                if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) {
                    if (strlen(destination->remote.uri) + 4 > CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY) {
                        unicoap_assist(API_WARNING("No options buffer supplied, allocating small default buffer")
                                           FIXIT("Increase CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY"));
                    }
                }

                request->options->entries->data = (uint8_t *)alloca(CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY);
                request->options->storage_capacity = CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY;
            }

            unicoap_endpoint_t endpoint = { 0 };
            res = unicoap_uri_parse(destination->remote.uri,
                                    strlen(destination->remote.uri), &endpoint,
                                    request->options);

            if (res < 0) {
                _CLIENT_DEBUG("URI/DNS/options failure (%i, %s)\n", res, strerror(-res));
                return res;
            }

            return unicoap_client_send_request_body(request, &endpoint, callback,
                                                    callback_args, flags);
        }
        else {
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
                                                callback_args, flags);
#else  /* IS_USED(MODULE_DNS) */
        unicoap_assist(API_ERROR("cannot resolve FQDN, dns module missing")
                       FIXIT("add USEMODULE += unicoap_client_uri"));
        assert(false);
        return -ENOTSUP;
#endif /* IS_USED(MODULE_DNS) */
    }
    case UNICOAP_DESTINATION_ENDPOINT:
        return unicoap_client_send_request_body(
            request, destination->remote.endpoint, callback, callback_args, flags);
    default:
        _CLIENT_DEBUG("illegal resource identifier type %" PRIu8 "\n", destination->type);
        assert(false);
        return -EINVAL;
    }
}

/* MARK: - Sync API */

typedef struct {
    mutex_t roadblock;
    unicoap_message_t* response;
    unicoap_aux_t* aux;
    uint8_t* buffer;
    size_t capacity;
    int res;
} _sync_copy_args_t;

static inline int _copy(unicoap_message_t* response, uint8_t* buffer, size_t capacity, int error)
{
    if (error != 0) {
        return error;
    }

    if (capacity <
        (sizeof(unicoap_options_t) + response->payload_size + response->options->storage_size)) {
        _CLIENT_DEBUG("no buffer space to copy options and payload, " _UNICOAP_NEED_HAVE "\n",
                     sizeof(unicoap_options_t) + response->payload_size +
                         response->options->storage_size,
                     capacity);

        return -ENOBUFS;
    }

    /* copy options struct itself */
    *(unicoap_options_t *)((void *)buffer) = *response->options;
    response->options = (unicoap_options_t *)((void *)buffer);
    buffer += sizeof(unicoap_options_t);

    /* copy options */
    unicoap_options_swap_storage(response->options, buffer, response->options->storage_size);
    buffer += response->options->storage_size;

    /* copy payload */
    memcpy(buffer, response->payload, response->payload_size);
    return 0;
}

static int _copy_callback(const unicoap_message_t *response, const unicoap_aux_t *aux, int error,
                          void *_arg)
{
    _sync_copy_args_t *args = _arg;
    *args->response = *response;

    args->res = _copy(args->response, args->buffer, args->capacity, error);
    if (args->res < 0) {
        /* something went wrong, don't hand over some half-baked response
         * struct with potentially corrupted pointers inside */
        *args->response = (unicoap_message_t){ 0 };
        if (args->aux) {
            *args->aux = (unicoap_aux_t){ 0 };
        }
    }
    else {
        /* response populated by _copy */
        if (args->aux) {
            *args->aux = *aux;
        }
    }

    mutex_unlock(args->roadblock);
    return 0;
}

int unicoap_send_request_sync_copy(unicoap_message_t* request,
                                   unicoap_destination_t* destination,
                                   unicoap_message_t* response, uint8_t* buffer,
                                   size_t buffer_capacity, unicoap_client_flags_t flags,
                                   unicoap_aux_t* aux)
{
    assert(buffer);
    assert(buffer_capacity > 0);
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
        .buffer = buffer,
        .capacity = buffer_capacity 
    };
    mutex_init_locked(&args.roadblock);

    int res = _unicoap_open_request(request, destination, _copy_callback, &args, flags);
    if (res < 0) {
        return res;
    }
    /* Block until callback calls unlock. */
    mutex_lock(&roadblock);
    return args.res;
}

typedef struct {
    unicoap_response_callback_t callback;
    void *caller_arg;
    int res;
    mutex_t roadblock;
} _args_t;

static int _sync_callback(const unicoap_message_t *response, const unicoap_aux_t *aux, int error,
                          void *args)
{
    _args_t *a = (_args_t *)args;
    a->res = a->callback(response, aux, error, a->caller_arg);
    mutex_unlock(&a->roadblock);
    return 0;
}

extern kernel_pid_t _unicoap_pid;

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

    _args_t args =
        (_args_t){ .callback = callback, .caller_arg = callback_arg };
    mutex_init_locked(&args.roadblock);

    int res = _unicoap_open_request(request, destination, _sync_callback, &args, flags);
    if (res < 0) {
        return res;
    }
    /* Block until callback calls unlock. */
    mutex_lock(&args.roadblock);
    return args.res < 0 ? args.res : 0;
}
