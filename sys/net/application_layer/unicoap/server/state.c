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

#include "container.h"
#include "event.h"
#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

/* Internal variables */
const unicoap_resource_t _unicoap_default_resources[] = {
#if CONFIG_UNICOAP_WELL_KNOWN_CORE
    /* (unicoap_resource_t) */ { .path = UNICOAP_PATH_RESOURCE_DISCOVERY,
                                 .handler = unicoap_resource_handle_well_known_core,
                                 .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET) }
#endif /* CONFIG_UNICOAP_WELL_KNOWN_CORE */
};

static unicoap_listener_t _default_listener = {
    .resources = _unicoap_default_resources,
    .resource_count = ARRAY_SIZE(_unicoap_default_resources),
    .request_matcher = unicoap_resource_match_request_default,
    .link_encoder = unicoap_resource_encode_link,
    .next = NULL,
};

#if IS_USED(MODULE_UNICOAP_SERVER_RESOURCE_DECLARATIONS)
XFA_INIT_CONST(unicoap_resource_t, unicoap_resources_xfa);
#endif

typedef struct {
    /** @brief Groups of resources */
    unicoap_listener_t* listeners;

    /* TODO: Advanced server features: Exchange-layer state objects */
} _server_state_t;

/**
 * @brief State of the unicoap server.
 *
 * @pre access from the thread with @ref _unicoap_pid, or while it is @ref KERNEL_PID_UNDEF
 */
static inline _server_state_t *_state(void)
{
    static _server_state_t _state = {
        .listeners = &_default_listener
    };

    assert(thread_getpid() == _unicoap_pid || _unicoap_pid == KERNEL_PID_UNDEF);
    return &_state;
}

/** @brief Sets listener array to @p listeners */
static inline void _state_set_listeners(unicoap_listener_t* listeners) {
    _state()->listeners = listeners;
}

/** @brief Gets listener array */
static inline unicoap_listener_t* _state_get_listeners(void) {
    return _state()->listeners;
}

int unicoap_init_server(event_queue_t *queue)
{
    (void)queue;
#if IS_USED(MODULE_UNICOAP_SERVER_RESOURCE_DECLARATIONS)
    /* add CoAP resources from XFA */
    static unicoap_listener_t _xfa_listener = {
        .resources = unicoap_resources_xfa,
    };
    _xfa_listener.resource_count = XFA_LEN(unicoap_resource_t, unicoap_resources_xfa);

    unicoap_listener_register(&_xfa_listener);
    _SERVER_DEBUG("registered %" PRIuSIZE " XFA resources\n", _xfa_listener.resource_count);
#endif /* UNICOAP_SERVER_RESOURCE_DECLARATIONS */

    return 0;
}

void unicoap_exchange_notify_all_server(const unicoap_endpoint_t* endpoint, unicoap_layer_notification_t type, void* arg)
{
    (void)endpoint;
    (void)type;
    (void)arg;
    /* TODO: no server memos yet to iterate through */
}

// MARK: - Listeners

typedef struct {
    unicoap_job_t job;
    unicoap_listener_t *listener;
    int res;
} _listener_args_t;

static void _listener_register(unicoap_job_t* job)
{
    _listener_args_t* a = container_of(job, _listener_args_t, job);
    unicoap_listener_t* listener = a->listener;

    listener->next = _state_get_listeners();
    _state_set_listeners(listener);

    if (!listener->link_encoder) {
        listener->link_encoder = unicoap_resource_encode_link;
    }

    if (!listener->request_matcher) {
        listener->request_matcher = unicoap_resource_match_request_default;
    }

    for (unsigned int i = 0; i < listener->resource_count; i += 1) {
        if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) {
            if (!listener->resources[i].methods) {
                unicoap_print_resource(&listener->resources[i]);
                printf(":\n");
                unicoap_assist(API_WARNING("resource definition is inaccessible")
                                   FIXIT("Set .methods"));
            }

            /* TODO: Advanced features: emit warnings */
        }
    }
}

void unicoap_listener_register(unicoap_listener_t* listener)
{
    assert(listener);
    /* That item will be overridden, ensure that the user expecting different
     * behavior will notice this. */
    assert(listener->next == NULL);

    _listener_args_t args = {
        .job = UNICOAP_JOB(_listener_register),
        .listener = listener,
    };

    if (_unicoap_pid == KERNEL_PID_UNDEF) {
        _listener_register(&args.job);
    }
    else {
        unicoap_loop_enqueue(&args.job);
    }
}

static void _listener_deregister(unicoap_job_t* job)
{
    _listener_args_t* a = container_of(job, _listener_args_t, job);
    unicoap_listener_t* listener = a->listener;

    unicoap_listener_t* current = _state_get_listeners();
    if (current == listener) {
        _state_set_listeners(listener->next);
        a->res = 0;
    }

    while (current) {
        if (current->next == listener) {
            current->next = listener->next;
            a->res = 0;
        }
        current = current->next;
    }
    a->res = -ENOENT;

    UNICOAP_JOB_FINISH(job);
}

int unicoap_listener_deregister(unicoap_listener_t* listener)
{
    assert(listener);

    _listener_args_t args = {
        .job = UNICOAP_JOB(_listener_deregister),
        .listener = listener,
    };
    unicoap_loop_enqueue_and_wait(&args.job);
    return args.res;
}

ssize_t unicoap_resource_core_link_format_build(char* buffer, size_t capacity,
                                                unicoap_proto_t proto)
{
    unicoap_listener_t* listener = _state_get_listeners();

    char* out = buffer;
    size_t pos = 0;

    unicoap_link_encoder_ctx_t ctx = {
        .content_format = UNICOAP_FORMAT_LINK,
        /* indicate initial link for the list */
        .uninitialized = true,
    };

    /* write payload */
    for (; listener != NULL; listener = listener->next) {
        if (!listener->link_encoder) {
            continue;
        }

        if (!unicoap_match_proto(listener->protocols, proto)) {
            continue;
        }
        ctx.link_pos = 0;

        for (; ctx.link_pos < listener->resource_count; ctx.link_pos += 1) {
            if (!unicoap_match_proto(listener->resources[ctx.link_pos].protocols, proto)) {
                continue;
            }
            ssize_t res;
            if (out) {
                res = listener->link_encoder(&listener->resources[ctx.link_pos], &out[pos],
                                             capacity - pos, &ctx);
            }
            else {
                res = listener->link_encoder(&listener->resources[ctx.link_pos], NULL, 0, &ctx);
            }

            if (res > 0) {
                pos += res;
                ctx.uninitialized = false;
            }
            else {
                break;
            }
        }
    }

    return pos;
}

int unicoap_resource_find(const unicoap_packet_t* packet, const unicoap_resource_t** resource_ptr,
                          const unicoap_listener_t** listener_ptr)
{
    assert(packet);
    int ret = UNICOAP_STATUS_PATH_NOT_FOUND;

    for (unicoap_listener_t* listener = _state_get_listeners(); listener; listener = listener->next) {
        const unicoap_resource_t* resource;
        int res;

        if (!unicoap_match_proto(listener->protocols, unicoap_packet_proto(packet))) {
            _SERVER_DEBUG("ignoring listener, proto %s not in set\n",
                            unicoap_string_from_proto(unicoap_packet_proto(packet)));
            continue;
        }

        res = listener->request_matcher(listener, &resource, packet->message, packet->remote);
        switch (res) {
        case UNICOAP_STATUS_PATH_NOT_FOUND:
            /* check next resource on mismatch */
            continue;
        case UNICOAP_STATUS_METHOD_NOT_ALLOWED:
            *resource_ptr = resource;
            *listener_ptr = listener;
            ret = res;
            /* found a resource, but method/proto do not match */
            continue;
        case 0:
            *resource_ptr = resource;
            *listener_ptr = listener;

            if (IS_ACTIVE(ENABLE_DEBUG)) {
                _SERVER_DEBUG("<");
                unicoap_print_path(&resource->path);
                DEBUG("%s>: found\n",
                        (*resource_ptr)->flags & UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE ? "/**" : "");
            }

            return 0;
        default:
            _SERVER_DEBUG("error: resource matcher failed\n");
            /* res is probably UNICOAP_RESOURCE_ERROR or some other
                    * unhandled error */
            return res;
        }
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        switch (ret) {
        case UNICOAP_STATUS_METHOD_NOT_ALLOWED:
            _SERVER_DEBUG("<");
            unicoap_print_path(&(*resource_ptr)->path);
            DEBUG("%s>: method %s not allowed\n",
                    (*resource_ptr)->flags & UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE ? "/**" : "",
                    unicoap_string_from_method(unicoap_request_get_method(packet->message)));
            break;
        case UNICOAP_STATUS_PATH_NOT_FOUND:
            _SERVER_DEBUG("<");
            unicoap_options_print_uri_path(packet->message->options);
            DEBUG(">: resource not found\n");
            break;
        default:
            break;
        }
    }

    return ret;
}

// todo: not in any header
void unicoap_print_listeners(void)
{
    printf("\n\t- listeners:\n");
    size_t i = 0;
    for (const unicoap_listener_t* listener = _state_get_listeners(); listener;
            listener = listener->next) {
        printf("\t\t- listener #%" PRIuSIZE "\n", i);

        printf("\t\t\t- request_matcher=");
        if (listener->request_matcher == unicoap_resource_match_request_default) {
            printf("<default>\n");
        }
        else {
            printf("<func at %p>\n", listener->request_matcher);
        }

        printf("\t\t\t- link_encoder=<func at %p>\n", listener->link_encoder);

        printf("\t\t\t- protocols=");
        unicoap_print_protocols(listener->protocols);
        printf("\n");

        printf("\t\t\t- resources (%" PRIuSIZE "):\n", listener->resource_count);
        for (size_t k = 0; k < listener->resource_count; k += 1) {
            const unicoap_resource_t* resource = &listener->resources[k];

            printf("\t\t\t\t- resource #%" PRIuSIZE " ", k);
            unicoap_print_path(&resource->path);
            printf("\n");

            printf("\t\t\t\t\t- flags=");
            unicoap_print_resource_flags(resource->flags);
            printf("\n");

            printf("\t\t\t\t\t- methods=");
            unicoap_print_methods(resource->methods);
            printf("\n");

            printf("\t\t\t\t\t- protocols=");
            unicoap_print_protocols(resource->protocols);
            printf("\n");

            printf("\t\t\t\t\t- handler=<fn at %p>\n", resource->handler);
            printf("\t\t\t\t\t- argument=<obj at %p>\n", resource->handler_arg);
        }

        i += 1;
    }
}
