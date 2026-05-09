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
#include "thread.h"
#include "sched.h"
#include "random.h"
#include "event.h"

#include "net/unicoap/config.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

UNICOAP_DECL_RECEIVER_STORAGE;

#if IS_USED(MODULE_UNICOAP_SERVER)
/* Internal variables */
const unicoap_resource_t _unicoap_default_resources[] = {
#  if CONFIG_UNICOAP_WELL_KNOWN_CORE
    /* (unicoap_resource_t) */ { .path = UNICOAP_PATH_RESOURCE_DISCOVERY,
                                 .handler = unicoap_resource_handle_well_known_core,
                                 .methods = UNICOAP_METHODS(UNICOAP_METHOD_GET) }
#  endif /* CONFIG_UNICOAP_WELL_KNOWN_CORE */
};

static unicoap_listener_t _default_listener = {
    .resources = _unicoap_default_resources,
    .resource_count = ARRAY_SIZE(_unicoap_default_resources),
    .request_matcher = unicoap_resource_match_request_default,
    .link_encoder = unicoap_resource_encode_link,
    .next = NULL,
};
#endif /* IS_USED(MODULE_UNICOAP_SERVER) */

#if IS_USED(MODULE_UNICOAP_SERVER_RESOURCE_DECLARATIONS)
XFA_INIT_CONST(unicoap_resource_t, unicoap_resources_xfa);
#endif

static unicoap_state_t _state = {
#if IS_USED(MODULE_UNICOAP_SERVER)
    .listeners = &_default_listener
#endif
};

kernel_pid_t _unicoap_pid = KERNEL_PID_UNDEF;
static event_queue_t _queue;

/* MARK: - State */

static inline void _lock(void) {
    mutex_lock(&_state.lock);
}

static inline void _unlock(void) {
    mutex_unlock(&_state.lock);
}

void unicoap_state_lock(void) {
    _lock();
}

void unicoap_state_unlock(void) {
    _unlock();
}

static unicoap_client_memo_t* _alloc_client(void) {
#if CONFIG_UNICOAP_CLIENT_MEMOS_MAX > 0
    /* Find empty slot in list of transactions */
    for (int i = 0; i < (int)ARRAY_SIZE(_state.client_memos); i += 1) {
        if (_state.client_memos[i].super.endpoint.proto == UNICOAP_PROTO_UNSPECIFIED) {
            return &_state.client_memos[i];
        }
    }
#endif
    _STATE_DEBUG("[client] no space to alloc\n");
    return NULL;
}

static void _free(unicoap_memo_t* memo) {
#if UNICOAP_HAVE_MESSAGING_STATE
    /* On the exchange layer, we're hiding the actual reason from the driver. It should
     * not matter to the messaging layer whether we release state because of an error or because
     * the state object is released as usual. Should the messaging layer rely on this
     * information, the exchange-messaging abstraction has a design flaw. */
    if (memo->messaging.state) {
        unicoap_messaging_notify(memo->messaging.state, UNICOAP_LAYER_NOTIFICATION_STATE_RELEASE,
                                 NULL, memo->endpoint.proto);
    }
#endif
    unicoap_event_cancel(&memo->exchange.timeout);
    memset(memo, 0, sizeof(*memo));
}

static inline bool _is_client(const unicoap_memo_t* memo) {
    (void)memo;
#if CONFIG_UNICOAP_CLIENT_MEMOS_MAX > 0
    return (uintptr_t)memo >= (uintptr_t)&_state.client_memos[0] &&
        (uintptr_t)memo <= (uintptr_t)&_state.client_memos[ARRAY_SIZE(_state.client_memos) - 1];
#else
    return false;
#endif
}

static inline size_t _client_index(unicoap_client_memo_t* memo) {
    /* debugging-only */
#if CONFIG_UNICOAP_CLIENT_MEMOS_MAX > 0
    return ((uintptr_t)memo - (uintptr_t)_state.client_memos) / sizeof(*_state.client_memos);
#else
    (void)memo;
    assert(false);
    return 0;
#endif
}

static void _deinit_client(unicoap_client_memo_t* memo) {
    _STATE_DEBUG("[client #%" PRIuSIZE "] release\n", _client_index(memo));
    memo->callback._any = NULL;
    memo->callback_arg = NULL;
    memo->flags = 0;
}

unicoap_client_memo_t* unicoap_client_memo_create(const unicoap_endpoint_t* endpoint) {
    assert(endpoint);
    assert(endpoint->proto != UNICOAP_PROTO_UNSPECIFIED);
    _lock();
    unicoap_client_memo_t* memo = _alloc_client();
    if (memo) {
        _STATE_DEBUG("[client #%" PRIuSIZE "] alloc\n", _client_index(memo));
        memo->super.endpoint = *endpoint;
    }
    _unlock();
    return memo;
}

void unicoap_client_memo_free(unicoap_client_memo_t* memo) {
    _deinit_client(memo);
    _free(&memo->super);
}

unicoap_client_memo_t* unicoap_client_memo_find(const unicoap_endpoint_t* endpoint,
                                                  const uint8_t* token, size_t token_length) {
    (void)endpoint;
    (void)token;
    (void)token_length;
    assert(endpoint);
    assert(token);

#if CONFIG_UNICOAP_CLIENT_MEMOS_MAX > 0
    for (int i = 0; i < (int)ARRAY_SIZE(_state.client_memos); i += 1) {
        unicoap_client_memo_t* memo = &_state.client_memos[i];

        if (unicoap_endpoint_is_equal(&memo->super.endpoint, endpoint) &&
            token_length == sizeof(memo->token) &&
            memcmp(memo->token, token, token_length) == 0) {
            return memo;
        }
    }
#endif
    return NULL;
}

void unicoap_exchange_notify(void* state, unicoap_layer_notification_t type, void* arg) {
    if (!state) { return; }
    unicoap_memo_t* memo = (unicoap_memo_t*)state;
    if (memo->endpoint.proto == UNICOAP_PROTO_UNSPECIFIED) {
        _STATE_DEBUG("[NOTIF] use of released state obj\n");
        return;
    }
    assert(memo->endpoint.proto != UNICOAP_PROTO_UNSPECIFIED);
    if (type & UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE) {
        _STATE_DEBUG("[NOTIF] messaging layer encountered error %i\n",
                     unicoap_layer_notification_async_failure_to_errno(type));
#if UNICOAP_HAVE_MESSAGING_STATE
        memo->messaging.state = NULL;
#endif
    } else if (type == UNICOAP_LAYER_NOTIFICATION_STATE_RELEASE) {
        _STATE_DEBUG("[NOTIF] messaging layer released state\n");
#if UNICOAP_HAVE_MESSAGING_STATE
        memo->messaging.state = NULL;
#endif
    } else if (type == UNICOAP_LAYER_NOTIFICATION_STATE_ALLOC) {
        _STATE_DEBUG("[NOTIF] messaging layer allocated state\n");
        assert(arg);
#if UNICOAP_HAVE_MESSAGING_STATE
        memo->messaging.state = arg;
#endif
    }

    if (_is_client(memo)) {
        if (type & UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE) {
            unicoap_client_callback_failure(unicoap_client_memo_of_super(memo),
                                          unicoap_layer_notification_async_failure_to_errno(type));
            unicoap_client_memo_free(unicoap_client_memo_of_super(memo));
        }
    }
}

void unicoap_exchange_notify_all(const unicoap_endpoint_t* endpoint, unicoap_layer_notification_t type, void* arg) {
    unicoap_memo_t* memo = NULL;
    (void)memo;
    (void)endpoint;
    (void)type;
    (void)arg;
#if CONFIG_UNICOAP_CLIENT_MEMOS_MAX > 0
    for (int i = 0; i < CONFIG_UNICOAP_CLIENT_MEMOS_MAX; i += 1) {
        memo = &_state.client_memos[i].super;
        if (unicoap_endpoint_is_equal(endpoint, &memo->endpoint)) {
            unicoap_exchange_notify(memo, type, arg);
        }
    }
#endif
}

void unicoap_messaging_notify(void* state, unicoap_layer_notification_t type, void* arg, unicoap_proto_t proto) {
    assert(proto != UNICOAP_PROTO_UNSPECIFIED);
    if (!state) { return; }
    (void)arg;
    (void)type;
    switch (proto) {
#if IS_USED(MODULE_UNICOAP_DRIVER_UDP)
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

/* MARK: - Event Scheduling on Internal Queue */

static void _scheduled_event_callback(void* scheduled_event) {
    event_post(&_queue, (event_t*)scheduled_event);
}

void unicoap_event_schedule(unicoap_scheduled_event_t* event, unicoap_event_callback_t callback,
                            uint32_t duration) {
    event->ztimer.callback = _scheduled_event_callback;
    event->ztimer.arg = (void*)event;
    /* This cast is fine because (a) the return types are identical and the function argument is
     * a pointer to a struct, too. */
    event->super.handler = (event_handler_t)callback;
    ztimer_set(UNICOAP_CLOCK, &event->ztimer, duration);
}

void unicoap_event_cancel(unicoap_scheduled_event_t* event) {
    if (ztimer_is_set(UNICOAP_CLOCK, &event->ztimer)) {
        /* event cancel runs in O(n), so we really only want to call
         * event_cancel if the event has already been posted */
        event_cancel(&_queue, &event->super);
    }
    ztimer_remove(UNICOAP_CLOCK, &event->ztimer);
}

/* MARK: - Lifecycle */

static inline int _init_drivers(event_queue_t* queue) {
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

static inline int _deinit_drivers(event_queue_t* queue) {
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

void* _unicoap_loop_run(void* arg) {
    (void)arg;
    /* Now we set the already-initialized queue's waiter thread. See unicoap_init below. */
    /* _queue.waiter == NULL is asserted by event_queue_claim */
    event_queue_claim(&_queue);
    _unicoap_pid = thread_getpid();
    event_loop(&_queue);
    return &_queue;
}

int unicoap_loop_enqueue(unicoap_job_t* job) {
    event_post(&_queue, &job->super);
    return 0;
}

kernel_pid_t unicoap_init(void) {
    if (_unicoap_pid != KERNEL_PID_UNDEF) {
        return -EEXIST;
    }

    mutex_init(&_state.lock);

#if IS_USED(MODULE_UNICOAP_SERVER_RESOURCE_DECLARATIONS)
    /* add CoAP resources from XFA */
    XFA_USE_CONST(unicoap_resource_t, unicoap_resources_xfa);
    static unicoap_listener_t _xfa_listener = {
        .resources = unicoap_resources_xfa,
    };

    _xfa_listener.resource_count = XFA_LEN(unicoap_resource_t, unicoap_resources_xfa);
    unicoap_listener_register(&_xfa_listener);
    _SERVER_DEBUG("registered %" PRIuSIZE " XFA resources\n", _xfa_listener.resource_count);
#endif /* UNICOAP_SERVER_RESOURCE_DECLARATIONS */

    event_queue_init_detached(&_queue);

    if (_init_drivers(&_queue) < 0) {
        UNICOAP_DEBUG("driver initialization failed\n");
        return -1;
    }

#if IS_ACTIVE(CONFIG_UNICOAP_CREATE_THREAD)
    static char _unicoap_thread_stack[UNICOAP_STACK_SIZE];
    _unicoap_pid = thread_create(_unicoap_thread_stack, sizeof(_unicoap_thread_stack),
                                 THREAD_PRIORITY_MAIN - 1, 0, _unicoap_loop_run,
                                 NULL, UNICOAP_THREAD_IDENTIFIER);
#endif

    return _unicoap_pid;
}

int unicoap_deinit(void) {
    if (_unicoap_pid == KERNEL_PID_UNDEF) {
        return -ENOENT;
    }

    _deinit_drivers(&_queue);
    _unicoap_pid = KERNEL_PID_UNDEF;
    memset(&_state, 0, sizeof(_state));
    memset(&_queue, 0, sizeof(_queue));

    return 0;
}

// MARK: - Listeners

void unicoap_listener_register(unicoap_listener_t* listener) {
    assert(listener);
    /* That item will be overridden, ensure that the user expecting different
     * behavior will notice this. */
    assert(listener->next == NULL);

    listener->next = unicoap_get_listeners(&_state);
    unicoap_set_listeners(&_state, listener);

    if (!listener->link_encoder) {
        listener->link_encoder = unicoap_resource_encode_link;
    }

    if (!listener->request_matcher) {
        listener->request_matcher = unicoap_resource_match_request_default;
    }

    for (unsigned int i = 0; i < listener->resource_count; i += 1) {
        if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) {
            if (!listener->resources[i].methods) {
                char path[CONFIG_UNICOAP_PATH_LENGTH_MAX];
                ssize_t length =
                    unicoap_path_stringify(&listener->resources[i].path, path, sizeof(path));
                if (length < 0) {
                    continue;
                }
                unicoap_assist(API_WARNING("resource '%.*s' is inaccessible")
                               FIXIT("Set .methods"),
                               (int)length, path);
            }

            /* TODO: Advanced features: emit warnings */
        }
    }
}

int unicoap_listener_deregister(unicoap_listener_t* listener) {
    assert(listener);

    unicoap_listener_t* current = unicoap_get_listeners(&_state);
    if (current == listener) {
        unicoap_set_listeners(&_state, listener->next);
        return 0;
    }

    while (current) {
        if (current->next == listener) {
            current->next = listener->next;
            return 0;
        }
        current = current->next;
    }
    return -ENOENT;
}

/* MARK: - Intersection with messaging */

void unicoap_generate_token(uint8_t* token) {
    for (size_t i = 0; i < CONFIG_UNICOAP_GENERATED_TOKEN_LENGTH; i += 4) {
        uint32_t _random = random_uint32_range(1, UINT32_MAX);
        memcpy(&token[i], &_random,
               (CONFIG_UNICOAP_GENERATED_TOKEN_LENGTH - i >= 4) ?
                   4 :
                   CONFIG_UNICOAP_GENERATED_TOKEN_LENGTH - i);
    }
}

static void _debug_packet(const unicoap_packet_t* packet) {
    DEBUG("in channel:");

    DEBUG("\n\tremote=");
    DEBUG_ENDPOINT(packet->remote);

    if (packet->local) {
        DEBUG("\n\tlocal=");
        DEBUG_ENDPOINT(packet->local);
    }

    DEBUG("\n");
}

int unicoap_messaging_send(unicoap_packet_t* packet, unicoap_messaging_flags_t flags, void* exchange) {
    (void)packet;
    (void)flags;

    assert(packet);
    assert(packet->remote);
    assert(packet->message);

    _MESSAGING_DEBUG("sending ");
    _debug_packet(packet);

    switch (unicoap_packet_proto(packet)) {
#if IS_USED(MODULE_UNICOAP_DRIVER_RFC7252_COMMON)
    case UNICOAP_PROTO_UDP:
    case UNICOAP_PROTO_DTLS:
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
                                                           bool truncated) {
    *flags = UNICOAP_MESSAGING_FLAGS_DEFAULT;
    unicoap_message_t* message = packet->message;
    int res = 0;

    _MESSAGING_DEBUG("received ");
    _debug_packet(packet);

    switch (unicoap_code_class(packet->message->code)) {
    case UNICOAP_CODE_CLASS_REQUEST: {
        if (IS_USED(MODULE_UNICOAP_SERVER)) {
            if (truncated) {
                _SERVER_DEBUG("truncated, not processing, sending Size1\n");
                UNICOAP_OPTIONS_ALLOC(response_options, 5);
                unicoap_options_set_size1(&response_options, CONFIG_UNICOAP_BLOCK_SIZE);
                unicoap_response_init_with_options(message, UNICOAP_STATUS_REQUEST_ENTITY_TOO_LARGE,
                                                   NULL, 0, &response_options);
                unicoap_messaging_send(packet, UNICOAP_MESSAGING_FLAGS_DEFAULT, NULL);
                return UNICOAP_PREPROCESSING_ERROR_REQUEST;
            }

            const unicoap_resource_t* resource = NULL;
            const unicoap_listener_t* listener = NULL;
            if ((res = unicoap_resource_find(packet, &resource, &listener)) != 0) {
                unicoap_response_init_empty(message, res < 0 ? UNICOAP_STATUS_INTERNAL_SERVER_ERROR :
                                            (unicoap_status_t)res);
                unicoap_messaging_send(packet, UNICOAP_MESSAGING_FLAGS_DEFAULT, NULL);
                return UNICOAP_PREPROCESSING_ERROR_REQUEST;
            }
            arg->resource = resource;
            *flags = _messaging_flags_resource(resource->flags);
            return UNICOAP_PREPROCESSING_SUCCESS_REQUEST;
        } else {
            return UNICOAP_PREPROCESSING_ERROR_UNSUPPORTED;
        }
    }

    case UNICOAP_CODE_CLASS_RESPONSE_SUCCESS:
    case UNICOAP_CODE_CLASS_RESPONSE_CLIENT_FAILURE:
    case UNICOAP_CODE_CLASS_RESPONSE_SERVER_FAILURE: {
        if (IS_USED(MODULE_UNICOAP_CLIENT)) {
            unicoap_client_memo_t* memo = unicoap_client_memo_find(packet->remote,
                packet->properties.token, packet->properties.token_length);

            if (!memo) {
                if (unicoap_options_contains(packet->message->options, UNICOAP_OPTION_OBSERVE)) {
                    /* We are no longer interested in notifications */
                    _CLIENT_DEBUG("received unknown NON notification\n");
                    return UNICOAP_PREPROCESSING_ERROR_NOTIFICATION_UNEXPECTED;
                }
                else {
                    _CLIENT_DEBUG("received unknown response\n");
                    return UNICOAP_PREPROCESSING_ERROR_RESPONSE_UNEXPECTED;
                }
            }

            if (truncated) {
                _CLIENT_DEBUG("truncated, not processing\n");
                unicoap_client_callback_failure(memo, -ENOBUFS);
                unicoap_client_memo_free(memo);
                return UNICOAP_PREPROCESSING_ERROR_TRUNCATED;
            }
            unicoap_event_cancel(&memo->super.exchange.timeout);
            arg->client = memo;
            *flags = _messaging_flags_client(memo->flags);
            return UNICOAP_PREPROCESSING_SUCCESS_RESPONSE;
        } else {
            return UNICOAP_PREPROCESSING_ERROR_UNSUPPORTED;
        }
    }

    case UNICOAP_CODE_CLASS_SIGNAL:
        /* TODO: Signaling */
        return UNICOAP_PREPROCESSING_ERROR_UNSUPPORTED;

    default:
        _MESSAGING_DEBUG("illegal code class %" PRIu8 "\n", unicoap_code_class(message->code));
        return UNICOAP_PREPROCESSING_ERROR_INVALID_CODE_CLASS;
    }
}

int unicoap_exchange_process(unicoap_packet_t* packet, unicoap_exchange_arg_t arg) {
    switch (unicoap_code_class(packet->message->code)) {
        case UNICOAP_CODE_CLASS_REQUEST:
            return IS_USED(MODULE_UNICOAP_SERVER) ?
                unicoap_server_process_request(packet, arg.resource) : -ENOTSUP;

        case UNICOAP_CODE_CLASS_RESPONSE_SUCCESS:
        case UNICOAP_CODE_CLASS_RESPONSE_CLIENT_FAILURE:
        case UNICOAP_CODE_CLASS_RESPONSE_SERVER_FAILURE:
            return IS_USED(MODULE_UNICOAP_CLIENT) ?
                unicoap_client_process_response(packet, arg.client) : -ENOTSUP;

        case UNICOAP_CODE_CLASS_SIGNAL:
            /* TODO: Signaling */
            return -1;

        default:
            UNREACHABLE();
            return -1;
    }
}

/* These must be in state.c as it reads the _state object. */

ssize_t unicoap_resource_core_link_format_build(char* buffer, size_t capacity,
                                                unicoap_proto_t proto) {
    unicoap_listener_t* listener = unicoap_get_listeners(&_state);

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
                          const unicoap_listener_t** listener_ptr) {
    assert(packet);
    int ret = UNICOAP_STATUS_PATH_NOT_FOUND;

    if (IS_USED(MODULE_UNICOAP_SERVER)) {
        for (unicoap_listener_t* listener = unicoap_get_listeners(&_state); listener; listener = listener->next) {
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
                        unicoap_path_print(&resource->path);
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
                    unicoap_path_print(&(*resource_ptr)->path);
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
    }

    return ret;
}

void unicoap_print_listeners(void) {
    if (IS_USED(MODULE_UNICOAP_SERVER)) {
        printf("\n\t- listeners:\n");
        size_t i = 0;
        for (const unicoap_listener_t* listener = unicoap_get_listeners(&_state); listener;
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
                unicoap_path_print(&resource->path);
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
}
