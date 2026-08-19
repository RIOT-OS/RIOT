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
                unicoap_print_path(&(*resource)->path);
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

static void _on_blockwise_transfer_timeout(unicoap_scheduled_event_t* timeout) {
    _SERVER_BLOCKWISE_DEBUG("block-wise memory period expired, forgetting\n");
    unicoap_server_memo_free(unicoap_server_memo_of_timeout(timeout));
}

int unicoap_server_process_request(unicoap_packet_t* packet, const unicoap_resource_t* resource)
{
    assert(packet);
    assert(packet->remote);
    int res = 0;
    unicoap_server_memo_t* memo = NULL;

    if (IS_USED(MODULE_UNICOAP_SERVER_BLOCKWISE)) {
        /* Try to find an active block-wise transfer. */
        unicoap_blockwise_transfer_t* transfer = NULL;

        if (resource->flags & (UNICOAP_RESOURCE_FLAG_REASSEMBLE | UNICOAP_RESOURCE_FLAG_SLICE)) {
            /* From RFC 7959, Section 2.4
             * The Block2 Option provides no way for a single endpoint to perform
             * multiple concurrently proceeding block-wise response payload transfer
             * (e.g., GET) operations to the same resource */
            bool block1 = unicoap_options_contains(packet->message->options, UNICOAP_OPTION_BLOCK1);
            memo = unicoap_server_memo_find_blockwise(packet->remote, resource);
            if (block1 && memo && unicoap_memo_blockwise_transfer_get(&memo->super)->stage == 
                UNICOAP_BLOCKWISE_STAGE_SLICE
            ) {
                /* We see a new Block1 option in the request, meaning the client wants to transfer
                 * another request body. The previous transfer in the Block2 / slice stage is 
                 * thus obsolete. In fact, the Block2/response body may change due to a new
                 * request (with a new Block1/request body). */
                _SERVER_BLOCKWISE_DEBUG("client starting over, forgetting transfer\n");
                unicoap_server_memo_free(memo);
                memo = NULL;
            }

            if (memo) {
                transfer = unicoap_memo_blockwise_transfer_get(&memo->super);
                unicoap_event_cancel(&memo->super.exchange.timeout);
            }
            else {
                if ((resource->flags & UNICOAP_RESOURCE_FLAG_REASSEMBLE && block1) ||
                    /* this check is only for the first request to a resource when the client
                     * suggests a block SZX */
                    (resource->flags & UNICOAP_RESOURCE_FLAG_SLICE &&
                     unicoap_options_contains(packet->message->options, UNICOAP_OPTION_BLOCK2))) {

                    if (!(memo = unicoap_server_memo_create(packet->remote, resource)) ||
                        !(transfer = unicoap_blockwise_transfer_create(
                              &memo->super, _blockwise_flags_resource(resource->flags)))) {
                        res = UNICOAP_STATUS_INTERNAL_SERVER_ERROR;
                        unicoap_response_init_empty(packet->message,
                                                    UNICOAP_STATUS_INTERNAL_SERVER_ERROR);
                        goto error;
                    }

                    transfer->stage = block1 ? UNICOAP_BLOCKWISE_STAGE_COLLECT :
                                               UNICOAP_BLOCKWISE_STAGE_SLICE;
                    if ((res = unicoap_blockwise_transfer_setup(
                        NULL, transfer, _blockwise_flags_resource(resource->flags),
                        packet->remote->proto, true
                    )) < 0) {
                        res = UNICOAP_STATUS_INTERNAL_SERVER_ERROR;
                        unicoap_response_init_empty(packet->message,
                                                    UNICOAP_STATUS_INTERNAL_SERVER_ERROR);
                        goto error;
                    }
                }
            }

            if (transfer) {
                /* Only do block-wise processing if we have a block-wise transfer.
                 * For example, if the request doesn't carry a Block1/2 option,
                 * there isn't a block-wise transfer yet. */

                if ((res = unicoap_server_process_request_blockwise(
                    packet, memo, resource
                )) < 0) {
                    unicoap_response_init_empty(packet->message,
                                                unicoap_response_status_from_errno(res));
                    goto error;
                }
                if (res > 0) {
                    /* If the Block1 exchange with the client is still ongoing,
                     * do not call the resource handler. Similarly, if the handler
                     * was already called and a Block2 exchange is therefore in
                     * progress, don't call the handler again. */

                    unicoap_event_schedule(&memo->super.exchange.timeout, 
                                           _on_blockwise_transfer_timeout,
                                           CONFIG_UNICOAP_TIMEOUT_SERVER_BLOCKWISE_MS,
                                           "server.blockwise.amnesia");
                    return 0;
                }
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
        return unicoap_server_send_response_body(packet, NULL, resource);
    }
    else if (context._packet) {
        /* application didn't send a response or deferred response,
         * otherwise, _packet would be NULL here */
        if (res != UNICOAP_IGNORING_REQUEST) {
            /* handler does not want to send response (provided No-Response is set at all) */
            /* the decision whether to honour No-Response must be made by the handler */

            if (res >= 0) {
                /* Handler did not fail but did not send response. */
                unicoap_assist(API_MISUSE("handler did not respond")
                               FIXIT("set USEMODULE += unicoap_deferred_response and"
                                     "call unicoap_defer_response")
                               FIXIT("ignore request by returning UNICOAP_IGNORING_REQUEST"));
            }
            unicoap_response_init_string(packet->message,
                                         UNICOAP_STATUS_INTERNAL_SERVER_ERROR, "application");
            goto error;
        }

        /* TODO: Advanced server features: Free exchange-layer state */
        return 0;
    }
    return 0;

error:
    if ((res = unicoap_server_send_response_body(packet, NULL, resource)) < 0) {
        return res;
    }

    if (memo) {
        unicoap_server_memo_free(memo);
    }
    return 0;
}

int unicoap_server_send_response_part(unicoap_packet_t* packet, unicoap_server_memo_t* memo,
                                      unicoap_resource_flags_t resource_flags) {
    int res = 0;
    /* TODO: OSCORE */
    if ((res = unicoap_messaging_send(
        packet, _messaging_flags_resource(resource_flags), &memo->super
    )) < 0) {
        _SERVER_DEBUG("error: could not send message\n");
    }
    return res;
}

/**
 * @brief Common function for @ref unicoap_send_response and @ref unicoap_send_response_deferred
 */
int unicoap_server_send_response_body(unicoap_packet_t* packet,
                                      unicoap_server_memo_t* memo,
                                      const unicoap_resource_t* resource)
{
    int res = 0;
    /* TODO: OSCORE */
    if (IS_USED(MODULE_UNICOAP_SERVER_BLOCKWISE)) {
        if (resource->flags & UNICOAP_RESOURCE_FLAG_SLICE &&
            unicoap_message_payload_get_size(packet->message) > CONFIG_UNICOAP_BLOCK_SIZE) {
            if (!memo &&
                !(memo = unicoap_server_memo_find_blockwise(packet->remote, resource))) {
                if (!(memo = unicoap_server_memo_create(packet->remote, resource))) {
                    res = -ENOBUFS;
                    goto internal_server_error;
                }
            }

            if ((res = unicoap_server_prepare_response_blockwise(
                packet, memo, _blockwise_flags_resource(resource->flags)
            )) < 0) {
                goto internal_server_error;
            }
        }
    }

    unicoap_resource_flags_t flags = resource->flags;
    if ((res = unicoap_server_send_response_part(packet, memo, flags)) < 0) {
        goto error;
    }

    if (memo && !unicoap_memo_blockwise_transfer_get(&memo->super)) {
        _SERVER_DEBUG("finished sending response body\n");
        unicoap_server_memo_free(memo);
    }
    return 0;

internal_server_error:
    _SERVER_DEBUG("internal server error, sending CoAP response\n");
    unicoap_response_init_empty(packet->message, UNICOAP_STATUS_INTERNAL_SERVER_ERROR);
    unicoap_messaging_send(
        packet, 
        _messaging_flags_resource(resource->flags) & ~UNICOAP_MESSAGING_FLAG_RELIABLE,
        NULL
    );

error:
    _SERVER_DEBUG("failed to send response (%i, %s)\n", res, strerror(-res));
    unicoap_server_memo_free(memo);
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
    _SERVER_DEBUG("sending immediate response\n");

    /* reuse the packet, stack-allocated, we're still inside resource handler */
    ((unicoap_packet_t*)context->_packet)->message = response;
    int res = unicoap_server_send_response_body((unicoap_packet_t*)context->_packet,
                                                NULL, context->resource);

    /* prevent context from being used for sending a response again.
     * If sending response on lower layer fails, then res < 0,
     * and allow calling send_response again to retry. Otherwise prevent calling again. */
    if (res >= 0) {
        context->_packet = NULL;
    }
    return res;
}
