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
#include "modules.h"

#include "net/unicoap/server.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

static inline void _free_client(unicoap_endpoint_t* client) {
    client->proto = UNICOAP_PROTO_UNSPECIFIED;
}

static inline void _free_registration(unicoap_observation_registration_t* registration) {
    assert(registration);
    assert(registration->client);
    registration->resource = NULL;
    _OBSERVATION_DEBUG("free registration\n");

    if (!unicoap_observation_client_has_any_registrations(registration->client)) {
        /* no remaining registrations for client,
         * mark element in list of clients unused */
        _OBSERVATION_DEBUG("client has no more registrations, freeing client\n");
        _free_client(registration->client);
    }

    registration->client = NULL;
}

/* We don't declare these in a header because technically the point of state.c
 * is that every _state-accesing function goes in there, but putting observation in state.c
 * would litter that file. The assumption that there's only a single unicoap_state_t is
 * an implementation detail that should not be assumed beyond the scope of state.c.
 * This file marks an exception due to its length. */

extern unicoap_endpoint_t* unicoap_state_observers(void);
extern unicoap_observation_registration_t* unicoap_state_observation_registrations(void);

static int _send_notifications(unicoap_packet_t* packet, const unicoap_resource_t* resource) {
    int res = 0;
    (void)packet;
    (void)resource;

#if CONFIG_UNICOAP_OBSERVATION_REGISTRATIONS_MAX > 0
    bool have_seqno = false;
    unicoap_observation_registration_t* observation_registrations = unicoap_state_observation_registrations();
    for (size_t i = 0; i < CONFIG_UNICOAP_OBSERVATION_REGISTRATIONS_MAX; i += 1) {
        unicoap_observation_registration_t* r = &observation_registrations[i];
        if (!r->resource || r->resource != resource) {
            continue;
        }

        if (IS_ACTIVE(CONFIG_UNICOAP_DEBUG_LOGGING)) {
            _OBSERVATION_DEBUG("found a registration for resource ");
            unicoap_path_print(&resource->path);
            DEBUG(" from ");
            DEBUG_ENDPOINT(r->client);
            DEBUG("\n");
        }

        _packet_set_dtls_session(packet, NULL);
        packet->remote = r->client;
        packet->properties.token = r->token;
        packet->properties.token_length = r->token_length;
        packet->properties.observe = true;

        /* Only generate a seqno if we really need it. */
        if (!have_seqno) {
            if ((res = unicoap_options_set_observe_generated(packet->message->options)) < 0) {
                return res;
            }
            have_seqno = true;
        }

        if ((res = unicoap_server_send_notification_body(packet, resource)) < 0) {
            _OBSERVATION_DEBUG("could not notify ");
            DEBUG_ENDPOINT(r->client);
            DEBUG("\n");
            continue;
        }

        if (IS_USED(MODULE_UNICOAP_DRIVER_RFC7252_COMMON)) {
            if (r->client->proto == UNICOAP_PROTO_UDP || r->client->proto == UNICOAP_PROTO_DTLS) {
                r->last_notification_id = packet->properties.rfc7252.id;
            }
        }
    }
#endif
    return 0;
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

    unicoap_message_t _response;
    UNICOAP_OPTIONS_ALLOC(options, _UNICOAP_OPTIONS_STORAGE_CAPACITY_JUST_GENERATED_OBSERVE);
    if (!response->options) {
        _response = *response;
        response = &_response;
        response->options = &options;
    }
    unicoap_packet_t packet = {
        .message = response
        /* Remainder is going to be set by _send_notifications. */
    };
    return _send_notifications(&packet, resource);
}

unicoap_observation_client_result_t unicoap_observation_find_client(const unicoap_endpoint_t* remote,
                                                            unicoap_endpoint_t** client) {
    *client = NULL;
#if CONFIG_UNICOAP_OBSERVATION_CLIENTS_MAX > 0
    unicoap_endpoint_t* observers = unicoap_state_observers();
    for (size_t i = 0; i < CONFIG_UNICOAP_OBSERVATION_CLIENTS_MAX; i += 1) {
        if (observers[i].proto == UNICOAP_PROTO_UNSPECIFIED) {
            *client = &observers[i];
            continue;
        }
        else if (unicoap_endpoint_is_equal(remote, &observers[i])) {
            *client = &observers[i];
            return UNICOAP_OBSERVATION_CLIENT_KNOWN;
        }
    }
#endif
    if (*client) {
        _OBSERVATION_DEBUG("client not known yet, could add\n");
        return UNICOAP_OBSERVATION_CLIENT_AVAILABLE;
    }
    else {
        _OBSERVATION_DEBUG("client not known, cannot add\n");
        return UNICOAP_OBSERVATION_CLIENT_UNAVAILABLE;
    }
}

unicoap_observation_registration_result_t unicoap_observation_find_registration(
    const unicoap_endpoint_t* client, const unicoap_resource_t* resource,
    const unicoap_message_properties_t* properties, unicoap_observation_registration_t** registration) {
    (void)client;
    (void)resource;
    (void)properties;
    *registration = NULL;
    if (properties->token_length > 0 && !properties->token) {
        return -EINVAL;
    }
#if CONFIG_UNICOAP_OBSERVATION_REGISTRATIONS_MAX > 0
    unicoap_observation_registration_t* observation_registrations = unicoap_state_observation_registrations();
    for (size_t i = 0; i < CONFIG_UNICOAP_OBSERVATION_REGISTRATIONS_MAX; i += 1) {
        unicoap_observation_registration_t* r = &observation_registrations[i];
        if (r->resource == NULL) {
            /* remember unused registrations */
            *registration = r;
            continue;
        }

        assert(r->client);

        if (r->resource != resource) {
            continue;
        }

        /* It's okay to compare pointers here as clients are stored in the
         * client array */
        if (r->client != client) {
            /* This resource is already registered with another client
             * This can be either due to the new client missing from
             * the client array (registered_client == NULL)
             * or exisiting in the client array (e.g., by having registered
             * for another resource) (registered_client != NULL). */
            if (IS_ACTIVE(CONFIG_UNICOAP_OBSERVATION_SINGLE_CLIENT_PER_RESOURCE)) {
                _OBSERVATION_DEBUG("resource alread has observer, resource in use\n");
                /* Reject registration attempts for resource in single registration mode */
                *registration = NULL;
                return UNICOAP_OBSERVATION_REGISTRATION_RESOURCE_IN_USE;
            }
            else {
                /* Try to find a free registration */
                continue;
            }
        }

        *registration = r;
        if (r->token_length && (r->token_length == properties->token_length) &&
            memcmp(r->token, properties->token, r->token_length) == 0) {
            _OBSERVATION_DEBUG("client already registered, exists\n");
            return UNICOAP_OBSERVATION_REGISTRATION_EXISTS;
        }

        /* Reject duplicate registration attempts, client has already registered */
        _OBSERVATION_DEBUG("client already registered, token mismatch\n");
        return UNICOAP_OBSERVATION_REGISTRATION_TOKEN_MISMATCH;
    }
#endif
    if (*registration) {
        _OBSERVATION_DEBUG("client not registered yet, registration is available\n");
        return UNICOAP_OBSERVATION_REGISTRATION_AVAILABLE;
    }
    else {
        _OBSERVATION_DEBUG("client not registered, no space to alloc\n");
        return UNICOAP_OBSERVATION_REGISTRATION_UNAVAILABLE;
    }
}

bool unicoap_observation_client_has_any_registrations(const unicoap_endpoint_t* client) {
    (void)client;
#if CONFIG_UNICOAP_OBSERVATION_REGISTRATIONS_MAX > 0
    unicoap_observation_registration_t* observation_registrations = unicoap_state_observation_registrations();
    for (unsigned i = 0; i < CONFIG_UNICOAP_OBSERVATION_REGISTRATIONS_MAX; i += 1) {
        unicoap_observation_registration_t* r = &observation_registrations[i];
        if (r->resource == NULL) {
            /* unused registration */
            continue;
        }
        assert(r->client);
        if (r->client == client) {
            return true;
        }
    }
#endif
    return false;
}

void unicoap_observation_deregister_by_id(const unicoap_endpoint_t* remote, uint16_t message_id){
    /* find observer entry from remote */
    unicoap_endpoint_t* client;
    if (unicoap_observation_find_client(remote, &client) != UNICOAP_OBSERVATION_CLIENT_KNOWN) {
        return;
    }

    if (!client) {
        return;
    }

    unicoap_observation_registration_t* registration = NULL;

#if CONFIG_UNICOAP_OBSERVATION_REGISTRATIONS_MAX > 0
    unicoap_observation_registration_t* observation_registrations = unicoap_state_observation_registrations();
    for (unsigned i = 0; i < CONFIG_UNICOAP_OBSERVATION_REGISTRATIONS_MAX; i += 1) {
        if (!observation_registrations[i].resource) {
            continue;
        }
        if ((observation_registrations[i].client == client) &&
            (observation_registrations[i].last_notification_id == message_id)) {
            registration = &observation_registrations[i];
            break;
        }
    }
#endif
    if (!registration) {
        return;
    }

    _free_registration(registration);
    _OBSERVATION_DEBUG("removed client registration with last notification ID '%" PRIu16 "'\n",
                  message_id);
}

int unicoap_observation_register(const unicoap_endpoint_t* remote, const unicoap_resource_t* resource,
                             const unicoap_message_properties_t* properties) {
    assert(resource);
    assert(remote);
    assert(properties);

    unicoap_endpoint_t* client = NULL;
    int client_res = unicoap_observation_find_client(remote, &client);

    switch (client_res) {
    case UNICOAP_OBSERVATION_CLIENT_KNOWN:
    case UNICOAP_OBSERVATION_REGISTRATION_AVAILABLE:
        break;
    case UNICOAP_OBSERVATION_REGISTRATION_UNAVAILABLE:
        return -1;
    }

    unicoap_observation_registration_t* registration = NULL;

    if (properties->token_length > sizeof(registration->token)) {
        _OBSERVATION_DEBUG("provided token too long, " _UNICOAP_NEED_HAVE "\n",
                           (size_t)properties->token_length, (size_t)sizeof(registration->token));
        return -ENOBUFS;
    }

    int res = unicoap_observation_find_registration(client, resource, properties, &registration);

    if (res == UNICOAP_OBSERVATION_REGISTRATION_AVAILABLE) {
        *client = *remote;
        registration->client = client;
        registration->token_length = properties->token_length;
        registration->resource = resource;
        registration->last_notification_id = 0;
        memcpy(registration->token, properties->token, properties->token_length);
        if (IS_ACTIVE(CONFIG_UNICOAP_DEBUG_LOGGING)) {
            _OBSERVATION_DEBUG("added client registration for resource ");
            unicoap_path_print(&resource->path);
            DEBUG("\n");
        }
    }
    return res;
}

int unicoap_observation_deregister(const unicoap_endpoint_t* remote, const unicoap_resource_t* resource,
                               const unicoap_message_properties_t* properties) {
    assert(resource);
    assert(remote);
    assert(properties);
    int res;

    unicoap_endpoint_t* client = NULL;
    res = unicoap_observation_find_client(remote, &client);

    switch (res) {
    case UNICOAP_OBSERVATION_CLIENT_KNOWN:
        /* registered with some resource, need to remove registration
             * and possibly client form list of known client, provided the resource
             * is the client's only observed resource */
        break;
    case UNICOAP_OBSERVATION_REGISTRATION_UNAVAILABLE:
    case UNICOAP_OBSERVATION_REGISTRATION_AVAILABLE:
        /* not registered with any resource, no work not to do */
        _OBSERVATION_DEBUG("client unknown, nothing to remove\n");
        return 0;
    }

    unicoap_observation_registration_t* registration = NULL;
    res = unicoap_observation_find_registration(client, resource, properties, &registration);

    if (res == UNICOAP_OBSERVATION_REGISTRATION_EXISTS ||
        res == UNICOAP_OBSERVATION_REGISTRATION_TOKEN_MISMATCH) {
        if (IS_ACTIVE(CONFIG_UNICOAP_DEBUG_LOGGING)) {
            _OBSERVATION_DEBUG("removing client registration for resource ");
            unicoap_path_print(&resource->path);
            DEBUG("\n");
        }
        _free_registration(registration);
    }
    else {
        _OBSERVATION_DEBUG("no registration to remove\n");
    }
    return 0;
}
