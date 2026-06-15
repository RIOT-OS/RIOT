/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap_server
 * @brief   Server implementation
 * @author  Carl <carl.seifert@tu-dresden.de>
 */

#include <string.h>

#include "thread.h"
#include "compiler_hints.h"

#include "net/unicoap/server.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

int unicoap_resource_match_request_default(const unicoap_listener_t* listener,
                                           const unicoap_resource_t** resource,
                                           const unicoap_message_t* request,
                                           const unicoap_endpoint_t* endpoint)
{
    assert(listener);
    assert(resource);
    assert(request);
    assert(endpoint);

    int res = UNICOAP_STATUS_PATH_NOT_FOUND;
    for (unsigned int i = 0; i < listener->resource_count; i += 1) {
        *resource = &listener->resources[i];
        if (!unicoap_match_proto((*resource)->protocols, endpoint->proto)) {
            _SERVER_DEBUG("ignoring resource <");
            if (IS_ACTIVE(ENABLE_DEBUG)) {
                unicoap_path_print(&(*resource)->path);
            }
            DEBUG(">, proto %s not in allowed set\n", unicoap_string_from_proto(endpoint->proto));
            continue;
        }

        if (!unicoap_resource_match_path_options(*resource,
                                                 (const unicoap_options_t*)request->options)) {
            /* URI mismatch */
            continue;
        }

        /* potential match, check for method */
        if (!unicoap_resource_match_method((*resource)->methods,
                                           unicoap_request_get_method(request))) {
            /* record wrong method error for next iteration, in case
             * another resource with the same URI and correct method
             * exists */
            res = UNICOAP_STATUS_METHOD_NOT_ALLOWED;
            continue;
        }
        else {
            return 0;
        }
    }
    return res;
}

ssize_t unicoap_resource_encode_link(const unicoap_resource_t* resource, char* buffer,
                                     size_t capacity, unicoap_link_encoder_ctx_t* context)
{
    assert(buffer);
    /* count target separators and any link separator, path is at least one character (`/`) */
    size_t exp_size = 2 + (context->uninitialized ? 0 : 1);

    unsigned int pos = 0;
    if (capacity < exp_size) {
        return -ENOBUFS;
    }

    if (!context->uninitialized) {
        buffer[pos++] = ',';
    }
    buffer[pos++] = '<';
    ssize_t res = 0;
    if ((res = unicoap_path_stringify(&resource->path, buffer + pos, capacity - exp_size)) < 0) {
        return res;
    }
    pos += res;
    buffer[pos] = '>';

    return exp_size + res;
}

/**
 * @brief Handler for `/.well-known/core`. Lists registered handlers, except for
 * `/.well-known/core` itself.
 */
int unicoap_resource_handle_well_known_core(unicoap_message_t* message, const unicoap_aux_t* aux,
                                            unicoap_request_context_t* ctx, void* arg)
{
    (void)arg;
    UNICOAP_OPTIONS_ALLOC(options, 2);
    if (unicoap_options_set_content_format(&options, UNICOAP_FORMAT_LINK) < 0) {
        return -1;
    }

    char links[CONFIG_UNICOAP_WELL_KNOWN_CORE_SIZE_MAX];
    size_t size = unicoap_resource_core_link_format_build(links, sizeof(links), aux->remote->proto);

    unicoap_response_init_with_options(message, UNICOAP_STATUS_CONTENT, (uint8_t*)links, size,
                                       &options);
    return unicoap_send_response(message, ctx);
}

int unicoap_server_process_request(unicoap_packet_t* packet, const unicoap_resource_t* resource)
{
    assert(packet);
    assert(packet->remote);
    int res = 0;

    unicoap_options_t* options = packet->message->options;
    if (IS_USED(MODULE_UNICOAP_SERVER_OBSERVATION) &&
        resource->flags & UNICOAP_RESOURCE_FLAG_OBSERVABLE
        && packet->message->method == UNICOAP_METHOD_GET
    ) {
        /* handle Observe option */
        uint32_t observe_value = 0;
        if (unicoap_options_get_observe(options, &observe_value) >= 0) {
            _SERVER_DEBUG("Observe instruction in request: %" PRIu32 "\n", observe_value);
            switch (observe_value) {
            case UNICOAP_OBSERVE_OPTION_REGISTER:
                packet->properties.observe = true;
                unicoap_observation_register(packet->remote, resource, &packet->properties);
                break;
            case UNICOAP_OBSERVE_OPTION_DEREGISTER:
                unicoap_observation_deregister(packet->remote, resource, &packet->properties);
                break;
            default:
                _SERVER_DEBUG("malformed Observe option\n");
                unicoap_response_init_string(packet->message, UNICOAP_STATUS_BAD_OPTION, "Observe");
                goto error;
            }
        }
    }

    unicoap_request_context_t context = {
        .resource = resource, ._packet = packet
    };

    unicoap_aux_t aux = {
        .remote = packet->remote,
        .local = packet->local,
        .properties = &packet->properties,
    };

    _SERVER_DEBUG("invoking handler\n");
    res = resource->handler(packet->message, &aux, &context, resource->handler_arg);

    if (res > 0) {
        _SERVER_DEBUG("sending response " UNICOAP_CODE_CLASS_DETAIL_FORMAT
                     " from return value\n",
                     unicoap_code_class((uint8_t)res), unicoap_code_detail((uint8_t)res));

        if (IS_ACTIVE(CONFIG_UNICOAP_PREVENT_OPTIONAL_RESPONSES)) {
            if (unicoap_response_is_optional(packet->message->options, (unicoap_status_t)res)) {
                _SERVER_DEBUG("response " UNICOAP_CODE_CLASS_DETAIL_FORMAT
                             " is optional, not responding\n",
                             unicoap_code_class((uint8_t)res),
                             unicoap_code_detail((uint8_t)res));
                return 0;
            }
        }

        unicoap_response_init_empty(packet->message, (unicoap_status_t)res);

        /* This should get optimised out if Observation is not imported.
         * Alternative would be alloca. But this buffer is 4 bytes. */
        uint8_t options_storage[_UNICOAP_OPTIONS_STORAGE_CAPACITY_JUST_GENERATED_OBSERVE];
        if (IS_USED(MODULE_UNICOAP_SERVER_OBSERVATION) && packet->properties.observe) {
            _SERVER_DEBUG("initial empty GET response to observable resource, Observe=seqno\n");
            unicoap_options_init(options, options_storage, sizeof(options_storage));
            res = unicoap_options_set_observe_generated(packet->message->options);
            assert(res >= 0);
            packet->message->options = options;
        }
        return unicoap_server_send_response_body(packet, resource);
    }
    else if (context._packet) {
        /* application didn't send a response or deferred response,
         * otherwise, _packet would be NULL here */
        if (res != UNICOAP_IGNORING_REQUEST) {
            /* handler does not want to send response (provided, No-Response is set at all) */
            /* the decision whether to honour No-Response must be made by the handler */

            if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) {
                unicoap_assist(API_MISUSE("handler did not respond")
                               FIXIT("set USEMODULE += unicoap_deferred_response and"
                                     "call unicoap_defer_response")
                               FIXIT("ignore request by returning UNICOAP_IGNORING_REQUEST"));
                unicoap_response_init_string(packet->message,
                                             UNICOAP_STATUS_INTERNAL_SERVER_ERROR, "application");
                goto error;
            }
        }

        /* TODO: Advanced server features: Free exchange-layer state */
        return 0;
    }
    return 0;

error:
    if ((res = unicoap_server_send_response_body(packet, resource)) < 0) {
        return res;
    }

    /* TODO: Advanced server features: Free exchange-layer state */
    return 0;
}

/**
 * @brief Common function for @ref unicoap_send_response and @ref unicoap_send_response_deferred
 */
int unicoap_server_send_body(unicoap_packet_t* packet, const unicoap_resource_t* resource,
                                      unicoap_messaging_flags_t messaging_flags)
{
    (void)resource;
    int res = 0;
    if ((res = unicoap_messaging_send(packet, messaging_flags, NULL)) < 0) {
        _SERVER_DEBUG("error: could not send response\n");
        goto error;
    }
    return 0;

error:
    /* TODO: Client and advanced server features: free allocated state */
    return res;
}

int unicoap_send_response(unicoap_message_t* response, unicoap_request_context_t* context)
{
    assert(response);
    assert(context);
    assert(context->resource);
    int res = 0;

    if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) {
        if (!context->_packet) {
            unicoap_assist(API_MISUSE("cannot send response, already sent"));
            return -ECANCELED;
        }
    }

    assert(context->_packet);
    _SERVER_DEBUG("sending immediate response\n");

    /* This should get optimised out if Observation is not imported. Alternative would be alloca. */
    UNICOAP_OPTIONS_ALLOC(options, _UNICOAP_OPTIONS_STORAGE_CAPACITY_JUST_GENERATED_OBSERVE);
    unicoap_message_t _response;
    if (IS_USED(MODULE_UNICOAP_SERVER_OBSERVATION) &&
        /* Observe is only set in process_request if OBSERVABLE flag and Observe option present. */
        ((unicoap_packet_t*)context->_packet)->properties.observe
    ) {
        assert(context->resource->flags & UNICOAP_RESOURCE_FLAG_OBSERVABLE);
        _SERVER_DEBUG("initial empty GET response to observable resource, Observe=seqno\n");
        if (!response->options) {
            _response = *response;
            response = &_response;
            response->options = &options;
        }
        res = unicoap_options_set_observe_generated(response->options);
        assert(res >= 0);
    }

    /* reuse the packet, stack-allocated, we're still inside resource handler */
    ((unicoap_packet_t*)context->_packet)->message = response;
    /* prevent context from being used for sending a response again.
     * If sending response on lower layer fails, then res < 0,
     * and allow calling send_response again to retry. Otherwise prevent calling again. */
    if ((res = unicoap_server_send_response_body((unicoap_packet_t*)context->_packet,
                                                 context->resource
    )) >= 0) {
        context->_packet = NULL;
    }
    return res;
}
