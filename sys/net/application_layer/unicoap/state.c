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
 * @ingroup net_unicoap
 * @brief   Core functionality
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <errno.h>
#include "container.h"
#include "thread.h"
#include "sched.h"
#include "random.h"
#include "event.h"

#include "net/unicoap/config.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

UNICOAP_DECL_RECEIVER_STORAGE;

/* Internal variables */
const unicoap_resource_t _unicoap_default_resources[] = {
#if CONFIG_UNICOAP_WELL_KNOWN_CORE
    /* (unicoap_resource_t) */ { .path = "/.well-known/core",
                                 .handler = unicoap_resource_handle_well_known_core,
                                 .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET) }
#endif
};

static unicoap_listener_t _default_listener = {
    .resources = _unicoap_default_resources,
    .resource_count = ARRAY_SIZE(_unicoap_default_resources),
    .request_matcher = unicoap_resource_match_request_default,
    .link_encoder = unicoap_resource_encode_link,
    .next = NULL,
};

#if IS_USED(MODULE_UNICOAP_RESOURCES_XFA)
XFA_INIT_CONST(unicoap_resource_t, unicoap_resources_xfa);
#endif

static unicoap_state_t _state = { .listeners = &_default_listener };

kernel_pid_t _unicoap_pid = KERNEL_PID_UNDEF;
static event_queue_t _queue;

static inline void _lock(void)
{
    mutex_lock(&_state.lock);
}

static inline void _unlock(void)
{
    mutex_unlock(&_state.lock);
}

void unicoap_state_lock(void)
{
    _lock();
}

void unicoap_state_unlock(void)
{
    _unlock();
}

/* MARK: - Event Scheduling on Internal Queue */

static void _scheduled_event_callback(void* scheduled_event)
{
    event_post(&_queue, (event_t*)scheduled_event);
}

void unicoap_event_schedule(unicoap_scheduled_event_t* event, unicoap_event_callback_t callback,
                            uint32_t duration)
{
    event->ztimer.callback = _scheduled_event_callback;
    event->ztimer.arg = (void*)event;
    /* This cast is fine because (a) the return types are identical and the function argument is
     * a pointer to a struct, too. */
    event->super.handler = (event_handler_t)callback;
    ztimer_set(UNICOAP_CLOCK, &event->ztimer, duration);
}

void unicoap_event_cancel(unicoap_scheduled_event_t* event)
{
    if (ztimer_is_set(UNICOAP_CLOCK, &event->ztimer)) {
        /* event cancel runs in O(n), so we really only want to call
         * event_cancel if the event has already been posted */
        event_cancel(&_queue, &event->super);
    }
    ztimer_remove(UNICOAP_CLOCK, &event->ztimer);
}

/* MARK: - Lifecycle */

static inline int _init_drivers(event_queue_t* queue)
{
    (void)queue;
#if IS_USED(MODULE_UNICOAP_DRIVER_RFC7252_COMMON)
    if (unicoap_init_rfc7252_common(queue) < 0) {
        return -1;
    }
#endif
#if IS_USED(MODULE_UNICOAP_DRIVER_UDP)
    if (unicoap_init_udp(queue) < 0) {
        return -1;
    }
#endif
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    if (unicoap_init_dtls(queue) < 0) {
        return -1;
    }
#endif
    /* MARK: unicoap_driver_extension_point */
    return 0;
}

static inline int _deinit_drivers(event_queue_t* queue)
{
    (void)queue;
    int res = 0;
#if IS_USED(MODULE_UNICOAP_DRIVER_RFC7252_COMMON)
    res += unicoap_deinit_rfc7252_common(queue);
#endif
#if IS_USED(MODULE_UNICOAP_DRIVER_UDP)
    res += unicoap_deinit_udp(queue);
#endif
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    res += unicoap_deinit_dtls(queue);
#endif
    /* MARK: unicoap_driver_extension_point */
    return res;
}

void* _unicoap_loop_run(void* arg)
{
    (void)arg;

    /* Either someone wants to run the unicoap loop on their own thread
     *    (_unicoap_pid == KERNEL_PID_UNDEF); or
     * we came here via thread_create in unicoap_init (CONFIG_UNICOAP_CREATE_THREAD is
     * enabled is enabled in that case)
     *    (_unicoap_pid == thread_getpid(), so we're on that thread right now).
     */
    assert(_unicoap_pid == KERNEL_PID_UNDEF || _unicoap_pid == thread_getpid());
    event_queue_init(&_queue);
    if (_init_drivers(&_queue) < 0) {
        UNICOAP_DEBUG("driver initialization failed\n");
        return NULL;
    }
    _unicoap_pid = thread_getpid();
    event_loop(&_queue);
    return &_queue;
}

static void* _event_loop_stop(void)
{
    _deinit_drivers(&_queue);
    /* TODO: Figure out how to die. */
    return NULL;
}

kernel_pid_t unicoap_init(void)
{
    if (_unicoap_pid != KERNEL_PID_UNDEF) {
        return -EEXIST;
    }

    mutex_init(&_state.lock);

#if IS_USED(MODULE_UNICOAP_RESOURCES_XFA)
    /* add CoAP resources from XFA */
    XFA_USE_CONST(unicoap_resource_t, unicoap_resources_xfa);
    static unicoap_listener_t _xfa_listener = {
        .resources = unicoap_resources_xfa,
    };

    _xfa_listener.resource_count = XFA_LEN(unicoap_resource_t, unicoap_resources_xfa);
    unicoap_listener_register(&_xfa_listener);
    UNICOAP_DEBUG("registered %" PRIuSIZE " XFA resources\n", _xfa_listener.resource_count);
#endif

#if IS_ACTIVE(CONFIG_UNICOAP_CREATE_THREAD)
    static char _unicoap_thread_stack[UNICOAP_STACK_SIZE];
    _unicoap_pid = thread_create(_unicoap_thread_stack, sizeof(_unicoap_thread_stack),
                                 THREAD_PRIORITY_MAIN - 1, 0, _unicoap_loop_run,
                                 NULL, UNICOAP_THREAD_IDENTIFIER);
#endif

    return _unicoap_pid;
}

int unicoap_deinit(void)
{
    if (_unicoap_pid == KERNEL_PID_UNDEF) {
        return -ENOENT;
    }

    _event_loop_stop();
    _unicoap_pid = KERNEL_PID_UNDEF;

    _deinit_drivers(&_queue);
    memset(&_state, 0, sizeof(_state));

    return 0;
}

// MARK: - Listeners

void unicoap_listener_register(unicoap_listener_t* listener)
{
    assert(listener);
    /* That item will be overridden, ensure that the user expecting different
     * behavior will notice this. */
    assert(listener->next == NULL);

    listener->next = _state.listeners;
    _state.listeners = listener;

    if (!listener->link_encoder) {
        listener->link_encoder = unicoap_resource_encode_link;
    }

    if (!listener->request_matcher) {
        listener->request_matcher = unicoap_resource_match_request_default;
    }

    for (unsigned int i = 0; i < listener->resource_count; i += 1) {
        if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) {
            if (!listener->resources[i].methods) {
                unicoap_assist(API_WARNING("resource '%s' is inaccessible")
                               FIXIT("Set .methods"),
                               listener->resources[i].path);
            }

            /* TODO: Advanced features: emit warnings */
        }
    }
}

void unicoap_listener_deregister(unicoap_listener_t* listener)
{
    assert(listener);

    unicoap_listener_t* l = _state.listeners;
    while (l) {
        if (l->next == listener) {
            l->next = listener->next;
            break;
        }
        l = l->next;
    }
}

/* MARK: - Intersection with messaging */

void unicoap_generate_token(uint8_t* token)
{
    for (size_t i = 0; i < CONFIG_UNICOAP_GENERATED_TOKEN_LENGTH; i += 4) {
        uint32_t _random = random_uint32_range(1, UINT32_MAX);
        memcpy(&token[i], &_random,
               (CONFIG_UNICOAP_GENERATED_TOKEN_LENGTH - i >= 4) ?
                   4 :
                   CONFIG_UNICOAP_GENERATED_TOKEN_LENGTH - i);
    }
}

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

int unicoap_messaging_send(unicoap_packet_t* packet, unicoap_messaging_flags_t flags)
{
    (void)packet;
    (void)flags;

    assert(packet);
    assert(packet->remote);
    assert(packet->message);

    MESSAGING_DEBUG("sending ");
    _debug_packet(packet);

    switch (unicoap_packet_proto(packet)) {
#  if IS_USED(MODULE_UNICOAP_DRIVER_RFC7252_COMMON)
    case UNICOAP_PROTO_UDP:
    case UNICOAP_PROTO_DTLS:
        return unicoap_messaging_send_rfc7252(packet, flags);
#  endif
    /* MARK: unicoap_driver_extension_point */
    default:
        MESSAGING_DEBUG("missing driver for proto\n");
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
    int res = 0;

    MESSAGING_DEBUG("received ");
    _debug_packet(packet);

    switch (unicoap_code_class(packet->message->code)) {
    case UNICOAP_CODE_CLASS_REQUEST: {
        if (truncated) {
            SERVER_DEBUG("truncated, not processing, sending Size1\n");
            UNICOAP_OPTIONS_ALLOC(response_options, 5);
            unicoap_options_set_size1(&response_options, CONFIG_UNICOAP_BLOCK_SIZE);
            unicoap_response_init_with_options(message, UNICOAP_STATUS_REQUEST_ENTITY_TOO_LARGE,
                                               NULL, 0, &response_options);
            goto server_prewarming_error;
        }

        const unicoap_resource_t* resource = NULL;
        const unicoap_listener_t* listener = NULL;
        if ((res = unicoap_resource_find(packet, &resource, &listener)) != 0) {
            unicoap_response_init_empty(message, res < 0 ? UNICOAP_STATUS_INTERNAL_SERVER_ERROR :
                                                           (unicoap_status_t)res);
            goto server_prewarming_error;
        }
        arg->resource = resource;
        *flags = _messaging_flags_resource(resource->flags);
        return UNICOAP_PREPROCESSING_SUCCESS_REQUEST;

server_prewarming_error:
        unicoap_messaging_send(packet, UNICOAP_MESSAGING_FLAGS_DEFAULT);
        return UNICOAP_PREPROCESSING_ERROR_REQUEST;
    }

    case UNICOAP_CODE_CLASS_RESPONSE_SUCCESS:
    case UNICOAP_CODE_CLASS_RESPONSE_CLIENT_FAILURE:
    case UNICOAP_CODE_CLASS_RESPONSE_SERVER_FAILURE:
        /* TODO: Client: Process response */
        return UNICOAP_PREPROCESSING_ERROR_RESPONSE_UNEXPECTED;

    case UNICOAP_CODE_CLASS_SIGNAL:
        /* TODO: Signaling */
        return UNICOAP_PREPROCESSING_ERROR_UNSUPPORTED;

    default:
        MESSAGING_DEBUG("illegal code class %" PRIu8 "\n", unicoap_code_class(message->code));
        return UNICOAP_PREPROCESSING_ERROR_INVALID_CODE_CLASS;
    }
}

int unicoap_exchange_process(unicoap_packet_t* packet, unicoap_exchange_arg_t arg) {
    switch (unicoap_code_class(packet->message->code)) {
    case UNICOAP_CODE_CLASS_REQUEST:
        return unicoap_server_process_request(packet, arg.resource);

    case UNICOAP_CODE_CLASS_RESPONSE_SUCCESS:
    case UNICOAP_CODE_CLASS_RESPONSE_CLIENT_FAILURE:
    case UNICOAP_CODE_CLASS_RESPONSE_SERVER_FAILURE:
        /* TODO: Client: Process response */
        return -1;

    case UNICOAP_CODE_CLASS_SIGNAL:
        /* TODO: Signaling */
        return -1;

    default:
        UNREACHABLE();
        return -1;
    }
}

void unicoap_exchange_forget_endpoint(const unicoap_endpoint_t* endpoint)
{
    (void)endpoint;
    /* TODO: Client and advanced server features: Elaborate state management */
    /* TODO: Observe: Remove potential registrations */
    return;
}

/* These must be in state.c as it reads the _state object. */

ssize_t unicoap_resource_core_link_format_build(char* buffer, size_t capacity,
                                                unicoap_proto_t proto)
{
    unicoap_listener_t* listener = _state.listeners;

    char* out = (char*)buffer;
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

    char path[CONFIG_UNIOCOAP_PATH_LENGTH_MAX];
    ssize_t path_length = 0;

    /* Copying Uri-Paths once followed by calls to strcmp is cheaper than
     * repeatedly parsing options and not allocating a buffer */
    if ((path_length =
         unicoap_options_copy_uri_path(packet->message->options, path, sizeof(path))) <= 0) {
        /* The Uri-Path options are longer than
         * CONFIG_UNIOCOAP_PATH_SIZE_MAX, and thus do not match anything
         * that could be found by this handler. */
        SERVER_DEBUG("could not copy Uri-Path\n");
        return UNICOAP_STATUS_PATH_NOT_FOUND;
    }

    for (unicoap_listener_t* listener = _state.listeners; listener; listener = listener->next) {
        const unicoap_resource_t* resource;
        int res;

        if (!unicoap_match_proto(listener->protocols, unicoap_packet_proto(packet))) {
            SERVER_DEBUG("ignoring listener, proto %s not in set\n",
                         unicoap_string_from_proto(unicoap_packet_proto(packet)));
            continue;
        }

        res = listener->request_matcher(path, (size_t)path_length, listener,
                                        &resource, packet->message, packet->remote);
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
            SERVER_DEBUG("%s%s: found\n", (*resource_ptr)->path,
                         (*resource_ptr)->flags & UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE ? "/**" : "");
            return 0;
        default:
            SERVER_DEBUG("error: resource matcher failed\n");
            /* res is probably UNICOAP_RESOURCE_ERROR or some other
                 * unhandled error */
            return res;
        }
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        switch (ret) {
        case UNICOAP_STATUS_METHOD_NOT_ALLOWED:
            SERVER_DEBUG("%s%s: method %s not allowed\n", (*resource_ptr)->path,
                         (*resource_ptr)->flags & UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE ? "/**" : "",
                         unicoap_string_from_method(unicoap_request_get_method(packet->message)));
            break;
        case UNICOAP_STATUS_PATH_NOT_FOUND:
            SERVER_DEBUG("%.*s: resource not found\n", (int)path_length, path);
            break;
        default:
            break;
        }
    }

    return ret;
}

void unicoap_print_listeners(void)
{
    printf("\n\t- listeners:\n");
    size_t i = 0;
    for (const unicoap_listener_t* listener = _state.listeners; listener;
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

            printf("\t\t\t\t- resource #%" PRIuSIZE " %s\n", k, resource->path);

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
