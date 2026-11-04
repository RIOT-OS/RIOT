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

#include <alloca.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "net/unicoap/message.h"
#include "private/state.h"
#include "thread.h"
#include "compiler_hints.h"
#include "modules.h"

#include "net/unicoap/server.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

/*
 * WARNING
 * It is essential _send_notifications is never invoked outside unicoap's event queue.
 * Internal state is being accessed repeatedly and must stay consistent between iterations.
 * Execute _send_notifications only as part of a job scheduled by unicoap.
 */

static int _send_notifications(unicoap_packet_t* packet, const unicoap_resource_t* resource) {
    int res = 0;
    (void)packet;
    (void)resource;

    bool has_any_subscribers = false;
    unicoap_server_memo_t* memo = NULL;
    while ((memo = unicoap_server_memo_next_with_resource(resource, memo))) {
        if (IS_ACTIVE(CONFIG_UNICOAP_DEBUG_LOGGING)) {
            _OBSERVATION_DEBUG("found a registration for resource ");
            unicoap_print_path(&resource->path);
            DEBUG(" from ");
            DEBUG_ENDPOINT(&memo->super.endpoint);
            DEBUG("\n");
        }

        /* We are reusing the same packet struct, but obviously cannot reuse transport session. */
        _packet_clear_sessions(packet);
        packet->remote = &memo->super.endpoint;
        packet->properties.token = memo->token;
        packet->properties.token_length = memo->token_length;
        packet->properties._nontraditional = true;

        /* Generate the seqno once for this batch of notifications. */
        if (!has_any_subscribers) {
            if ((res = unicoap_options_set_observe_generated(packet->message->options)) < 0) {
                return res;
            }
            has_any_subscribers = true;
        }

        unicoap_server_memo_realloc(memo);
        if ((res = unicoap_server_send_notification_body(packet, memo, resource)) < 0) {
            _OBSERVATION_DEBUG("could not notify ");
            DEBUG_ENDPOINT(&memo->super.endpoint);
            DEBUG("\n");
        }
    }
    return has_any_subscribers ? 0 : UNICOAP_OBSERVATION_NO_SUBSCRIBERS;
}

int unicoap_send_notification(unicoap_message_t* response, const unicoap_resource_t* resource) {
    assert(response);
    assert(resource);

    if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) {
        if (!(resource->flags & UNICOAP_RESOURCE_FLAG_OBSERVABLE)) {
            unicoap_assist(API_MISUSE("cannot send a notification from non-observable resource")
                               FIXIT("add OBSERVABLE flag to resource"));
        }
    }
    assert(resource->flags & UNICOAP_RESOURCE_FLAG_OBSERVABLE);

    if (!response->options) {
        unicoap_message_t* _response = alloca(sizeof(unicoap_message_t));
        if (!_response) {
            _OBSERVATION_DEBUG("failed to alloca message\n");
            return -ENOMEM;
        }
        *_response = *response;
        response = _response;
        uint8_t* storage = alloca(_UNICOAP_OPTIONS_STORAGE_CAPACITY_JUST_GENERATED_OBSERVE);
        if (!storage) {
            _OBSERVATION_DEBUG("failed to alloca options\n");
            return -ENOMEM;
        }
        unicoap_options_t* options = alloca(sizeof(unicoap_options_t));
        if (!options) {
            _OBSERVATION_DEBUG("failed to alloca options\n");
            return -ENOMEM;
        }
        unicoap_options_init(options, storage, _UNICOAP_OPTIONS_STORAGE_CAPACITY_JUST_GENERATED_OBSERVE);
        response->options = options;
    }
    unicoap_packet_t packet = {
        .message = response
        /* Remainder is going to be set by _send_notifications. */
    };
    return _send_notifications(&packet, resource);
}

bool unicoap_observation_resource_has_observers(const unicoap_resource_t* resource) {
    assert(resource);
    return unicoap_server_memo_next_with_resource(resource, NULL) != NULL;
}

int unicoap_observation_register(const unicoap_endpoint_t* remote, const unicoap_resource_t* resource,
                                 const unicoap_message_properties_t* properties) {
    assert(resource);
    assert(remote);
    assert(properties);

    if (properties->token_length > sizeof(((unicoap_server_memo_t*)NULL)->token)) {
        _OBSERVATION_DEBUG("provided token too long, " _UNICOAP_NEED_HAVE "\n",
                           (size_t)properties->token_length,
                           (size_t)sizeof(((unicoap_server_memo_t*)NULL)->token));
        return -E2BIG;
    }

    /* Find memo. If not found, allocate. If no free memo, NULL. */
    unicoap_server_memo_t* memo = unicoap_server_memo_with_token(remote, resource,
        properties->token, properties->token_length,
        /* allocate: */ true);

    if (!memo) {
        _OBSERVATION_DEBUG("failed to alloc memo\n");
        return -ENOBUFS;
    }

    _OBSERVATION_DEBUG("adding client subscription\n");
    return 0;
}

int unicoap_observation_deregister(const unicoap_endpoint_t* remote, const unicoap_resource_t* resource,
                                   const unicoap_message_properties_t* properties) {
    assert(resource);
    assert(remote);
    assert(properties);

    unicoap_server_memo_t* memo = unicoap_server_memo_with_token(remote, resource,
        properties->token, properties->token_length,
        /* allocate: */ false);

    if (memo) {
        _OBSERVATION_DEBUG("removing client subscription\n");
        unicoap_server_memo_free(memo, 0);
        return 0;
    }

    _OBSERVATION_DEBUG("no client subscription to remove\n");
    return -ENOENT;
}
