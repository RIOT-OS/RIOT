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
 * @ingroup net_unicoap_server
 * @brief   Server implementation
 * @author  Carl <carl.seifert@tu-dresden.de>
 */

#include <string.h>

#include "thread.h"
#include "compiler_hints.h"

#include "net/unicoap/application.h"

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
            SERVER_DEBUG("ignoring resource <");
            if (IS_ACTIVE(ENABLE_DEBUG)) {
                unicoap_path_print(&(*resource)->path);
            }
            DEBUG(">, proto %s not in allowed set\n", unicoap_string_from_proto(endpoint->proto));
            continue;
        }
        assert(request->options->entries->data);

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
    if ((res = unicoap_path_serialize(&resource->path, buffer + pos, capacity - exp_size)) < 0) {
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

    unicoap_request_context_t context = {
        .resource = resource, ._packet = packet
    };

    unicoap_aux_t aux = {
        .remote = packet->remote,
        .local = packet->local,
        .properties = &packet->properties,
    };

    SERVER_DEBUG("invoking handler\n");
    res = resource->handler(packet->message, &aux, &context, resource->handler_arg);

    if (res > 0) {
        SERVER_DEBUG("sending response " UNICOAP_CODE_CLASS_DETAIL_FORMAT
                     " from return value\n",
                     unicoap_code_class((uint8_t)res), unicoap_code_detail((uint8_t)res));

        if (IS_ACTIVE(CONFIG_UNICOAP_PREVENT_OPTIONAL_RESPONSES)) {
            if (unicoap_response_is_optional(packet->message->options, (unicoap_status_t)res)) {
                SERVER_DEBUG("response " UNICOAP_CODE_CLASS_DETAIL_FORMAT
                             " is optional, not responding\n",
                             unicoap_code_class((uint8_t)res),
                             unicoap_code_detail((uint8_t)res));
                return 0;
            }
        }

        unicoap_response_init_empty(packet->message, (unicoap_status_t)res);
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
int unicoap_server_send_response_body(unicoap_packet_t* packet,
                                      const unicoap_resource_t* resource)
{
    int res = 0;
    if ((res = unicoap_messaging_send(packet, _messaging_flags_resource(resource->flags))) < 0) {
        SERVER_DEBUG("error: could not send response\n");
        goto error;
    }
    return 0;

error:
    _SERVER_DEBUG("failed to send response, trying to send 5.00 unreliably\n");

    /* try to send 5.00,  */
    unicoap_response_init_empty(packet->message, UNICOAP_STATUS_INTERNAL_SERVER_ERROR);
    unicoap_messaging_send(packet, _messaging_flags_resource(resource->flags) &
                                         ~UNICOAP_MESSAGING_FLAG_RELIABLE);

    /* TODO: Client and advanced server features: free allocated state */
    return res;
}

int unicoap_send_response(unicoap_message_t* response, unicoap_request_context_t* context)
{
    assert(response);
    assert(context);
    assert(context->resource);

    if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) {
        if (!context->_packet) {
            unicoap_assist(API_MISUSE("cannot send response, already sent"));
            return -ECANCELED;
        }
    }

    assert(context->_packet);
    SERVER_DEBUG("sending immediate response\n");

    /* reuse the packet, stack-allocated, we're still inside resource handler */
    ((unicoap_packet_t*)context->_packet)->message = response;
    int res = unicoap_server_send_response_body((unicoap_packet_t*)context->_packet,
                                                context->resource);

    /* prevent context from being used for sending a response again */
    context->_packet = NULL;
    return res;
}
