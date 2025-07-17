/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "xfa.h"
#include "ztimer.h"
#include "event.h"

#include "net/unicoap/constants.h"
#include "net/unicoap/message.h"
#include "net/unicoap/transport.h"
#include "net/unicoap/util_macros.h"

/**
 * @file
 * @brief   High-level CoAP client and sever API
 * @ingroup net_unicoap
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - Utilities */
/**
 * @name Utilities
 * @{
 */
/**
 * @brief Auxiliary exchange information
 */
typedef struct {
    /**
     * @brief The remote CoAP endpoint in this exchange
     */
    const unicoap_endpoint_t* remote;

    /**
     * @brief The local CoAP endpoint in this exchange
     */
    const unicoap_endpoint_t* local;

    /**
     * @brief Message properties
     */
    const unicoap_message_properties_t* properties;
} unicoap_aux_t;
/** @} */

/**
 * @addtogroup net_unicoap_server
 * @{
 */
/* MARK: - Responding to requests */
/**
 * @name Responding to requests
 * @{
 */
#ifndef DOXYGEN
/* Forward declaration */
struct unicoap_resource;
#endif

/**
 * @brief Resource typealias
 */
typedef struct unicoap_resource unicoap_resource_t;

/**
 * @brief Request context used to send a response to a given request
 */
typedef struct {
    /**
     * @brief Resource this request was directed to
     */
    const unicoap_resource_t* resource;

    /**
     * @brief Internal
     *
     * @warning This is an internal property. Do not access from your application!
     * @private
     */
    void* _packet;

    /**
     * @brief Internal
     *
     * @warning This is an internal property. Do not access from your application!
     * @private
     */
    void* _memo;
} unicoap_request_context_t;

/**
 * @brief Resource request handler
 *
 * This handler is called whenever a request reaches the given resource. If you use a certain
 * handler for more than one resource, you may want to read @ref unicoap_request_context_t.resource.
 *
 * @param[in] request Request, safe to mutate and send response with mutated message.
 * @param[in] aux Auxiliary data associated with the request
 * @param[in] ctx Request context, use to send response.
 * @param[in] arg Argument specified in [resource definition](@ref unicoap_resource_t)
 *
 * If you do not call @ref unicoap_send_response and return a negative value from the handler,
 * an `Internal Server Error` response will be sent.
 *
 * @return `0`, status code or errno.
 * @retval @ref UNICOAP_IGNORING_REQUEST iff you don't want to respond
 * @retval @ref unicoap_status_t for an otherwise empty response
 *
 */
typedef int (*unicoap_request_handler_t)(unicoap_message_t* request, const unicoap_aux_t* aux,
                                          unicoap_request_context_t* ctx, void* arg);

/**
 * @brief Determines if the client is interested in a response
 *
 * @param[in] options Request options
 * @param status The CoAP status code you would respond with
 *
 * @retval `false` if a response with that status code must be sent
 * @retval `true` if sending a response with that status code is not mandatory
 */
bool unicoap_response_is_optional(unicoap_options_t* options, unicoap_status_t status);

/**
 * @brief Error number indicating the resource handler will not respond
 *
 * Return this error number from your request handler if you have determined you don't need to
 * send a response using @ref unicoap_response_is_optional. `unicoap` does not check whether you
 * have checked with @ref unicoap_response_is_optional.
 */
#define UNICOAP_IGNORING_REQUEST (-2042)

/**
 * @brief Sends a response to a request identifier by the given @p context
 *
 * Consumes @p response .
 *
 * @warning You MUST NOT call this method after having deferred a response.
 *
 * @param[in,out] response Response message to send
 * @param[in,out] context Request context to respond in
 *
 * This function's return value can be used as a return value in @ref unicoap_request_handler_t.
 *
 * @retval Zero on success.
 * @retval Negative integer on error
 */
int unicoap_send_response(unicoap_message_t* response, unicoap_request_context_t* context);

/**
 * @brief Maps a given @p errno to a CoAP response status code
 *
 * @param _errno Error number such as `-ENOENT`
 *
 * @return Status code
 */
unicoap_status_t unicoap_response_status_from_errno(int _errno);
/** @} */

/* MARK: - Registering CoAP resources */
/**
 * @name Registering CoAP resources
 * @{
 */
/**
 * @brief Flags for enabling advanced features in server exchanges
 *
 * Specify these flags when creating a @ref unicoap_resource_t  to modify transmission,
 * block-wise, or resource observation behavior.
 */
typedef enum {
    /**
     * @brief Sets the type of the message to confirmable (`CON`),
     * if an unreliable transport is used.
     *
     * This flag is ignored with reliable transports. For unreliable transports, a message
     * sent with this flag will require an acknowledgement to be sent from the CoAP
     * peer.
     *
     */
    UNICOAP_RESOURCE_FLAG_RELIABLE = 0x0001,

    /**
     * @brief Makes this resource match paths located in the subtree of the given path.
     *
     * Example:
     * A request with path `/laniakea/milky-way/solar-system/pluto`
     * would match a resource with this flag and path `/laniakea/milky-way`.
     */
    UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE = 0x4000,

    /* TODO: Advanced features */
} unicoap_resource_flags_t;

/**
 * @brief Prints resource flags
 *
 * @param flags Resource flags
 */
void unicoap_print_resource_flags(unicoap_resource_flags_t flags);

/**
 * @brief Allowed protocols bit field
 *
 * @note A value of @ref UNICOAP_PROTOCOLS_ALLOW_ALL (`0`) indicates
 * no protocol checking is to be performed
 *
 * @see @ref UNICOAP_PROTOCOL_FLAG
 * @see @ref UNICOAP_PROTOCOLS
 */
typedef uint8_t unicoap_proto_set_t;

/**
 * @brief Prints protocols bitfield
 *
 * @param protocols Set of allowed transports
 */
void unicoap_print_protocols(unicoap_proto_set_t protocols);

/**
 * @brief Allowed methods bit field
 *
 * @note A value of @ref UNICOAP_PROTOCOLS_ALLOW_ALL (`0xff`) indicates
 * all methods are allowed.
 *
 * @see @ref UNICOAP_METHOD_FLAG
 * @see @ref UNICOAP_METHODS
 */
typedef uint8_t unicoap_method_set_t;

/**
 * @brief Prints methods bitfield
 *
 * @param methods Set of CoAP methods
 */
void unicoap_print_methods(unicoap_method_set_t methods);

/**
 * @brief A type representing a CoAP resource
 *
 * This structure models a CoAP resource that can handle requests with a specified set
 * of methods and allowed protocols. Normally, each resource listens for requests matching
 * the given path. If you want to match all paths with a certain path prefix,
 * see @ref UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE.
 */
struct unicoap_resource {
    /**
     * @brief Resource path
     *
     * Must be a null-terminated string with slash-separated path components
     */
    const char* path;

    /**
     * @brief Request handler callback
     *
     * Function that will be executed once a new request directed at this resource reaches the
     * server. Will be called from the `unicoap` thread.
     */
    unicoap_request_handler_t handler;

    /**
     * @brief Opaque optional argument for the request handler (nullable)
     */
    void* handler_arg;

    /**
     * @brief Flags for modifying resource behavior
     *
     * @see @ref unicoap_resource_flags_t
     */
    unicoap_resource_flags_t flags;

    /**
     * @brief Allowed request methods for this resource
     *
     * @see @ref unicoap_method_set_t
     */
    unicoap_method_set_t methods;

    /**
     * @brief Allowed transport protocols this resource can be reached over
     *
     * Use this property to, e.g., limit requests to encrypted transport protocols
     *
     * @see @ref unicoap_proto_set_t
     */
    unicoap_proto_set_t protocols;
};
/** @} */

/**
 * @addtogroup net_unicoap_resources_xfa
 * @{
 */
/* MARK: - Defining a cross-file resource */
/**
 * @name Defining a cross-file resource
 * @{
 */
#if IS_USED(MODULE_UNICOAP_RESOURCES_XFA) || defined(DOXYGEN)
/**
 * @brief CoAP cross-file resource definition
 *
 * @param name  internal name of the resource entry, must be unique
 *
 * You must supply a constant initializer following the macro invocation.
 * The name you supply is needed for technical reasons but has otherwise no meaning.
 */
#  define UNICOAP_RESOURCE(name) \
      XFA_CONST(unicoap_resource_t, unicoap_resources_xfa, 0) CONCAT(unicoap_resource_, name) =
#else
#  define UNICOAP_RESOURCE(name)                                                                   \
      static_assert(false,                                                                         \
                    "The unicoap_resources_xfa module is missing, resource cannot be registered"); \
      unicoap_resource_t CONCAT(unicoap_resource_, name) =
#endif
/** @} */
/** @} */

/* MARK: - Resource discovery */
/**
 * @name Resource discovery
 * @{
 */
/**
 * @brief Builds a string in Constrained RESTful Environments (CoRE) Link Format
 *
 * You use this method to build a stringified list of resources registered with `unicoap`.
 * To register a resource, use @ref unicoap_listener_register or @ref net_unicoap_resources_xfa.
 * The string generated contains only resources available using the specified transport. When
 * handling a request destined for `/.well-known/core`, you should call this method with the
 * transport you received the request over.
 *
 * **Example**:
 * ```
 * </sensors/temp>;rt="temperature-c",
 * </sensors/light>;rt="light-lux"
 * ```
 *
 * @see [Constrained RESTful Environments (CoRE) Link Format](https://datatracker.ietf.org/doc/html/rfc6690)
 *
 * @param[in,out] buffer The buffer that will contain the built string in CoRE Link Format
 * @param capacity The capacity @p buffer in bytes
 * @param proto The `unicoap` protocol number of the transport
 *
 * @returns Length of encoded resource list in bytes or negative integer on error
 */
ssize_t unicoap_resource_core_link_format_build(char* buffer, size_t capacity,
                                                unicoap_proto_t proto);

/**
 * @brief   Context information required to write a resource link
 */
typedef struct {
    /**
     * @brief Expected content format of resource
     */
    unicoap_content_format_t content_format;

    /**
     * @brief Current byte index of encoder
     */
    size_t link_pos;

    /** @brief Boolean value indicating whether to initialize result list for first resource */
    bool uninitialized : 1;
} unicoap_link_encoder_ctx_t;

/**
 * @brief   Handler function to write a resource link
 *
 * @param[in] resource Resource for link
 * @param[out] buffer Buffer on which to write
 * @param[in] capacity Remaining length for @p buffer
 * @param[in] context Contextual information on what/how to write
 *
 * @retval  Number of bytes written to @p buffer
 * @retval  `-1` on error
 */
typedef ssize_t (*unicoap_link_encoder_t)(const unicoap_resource_t* resource, char* buffer,
                                          size_t capacity, unicoap_link_encoder_ctx_t* context);

/**
 * @brief Encodes given resource in Constrained RESTful Environments (CoRE) Link Format
 *
 * @param[in] resource The CoAP resource to encode
 * @param[out] buffer Pointer to where encoded resource string will be written
 * @param capacity The capacity of @p buffer in bytes
 * @param[in,out] context Encoding context
 *
 * @returns Length of encoded resource string in bytes or negative integer on error
 * @retval `ENOBUFS` Buffer too small to encode resources in link format.
 */
ssize_t unicoap_resource_encode_link(const unicoap_resource_t* resource, char* buffer,
                                     size_t capacity, unicoap_link_encoder_ctx_t* context);
/** @} */

/* MARK: - Matching request to resources */
/**
 * @name Matching request to resources
 * @{
 */
/**
 * @brief Typealias for @ref unicoap_listener
 */
typedef struct unicoap_listener unicoap_listener_t;

/**
 * @brief   Handler function for the request matcher strategy
 *
 * @param[in]  path         Requested path
 * @param      path_length  Number of UTF-8 characters in @p path (excluding null-terminator)
 * @param[in]  listener     Listener
 * @param[out] resource     Matching resource
 * @param[in]  request      Request message
 * @param[in]  endpoint     Remote endpoint the request originates from
 *
 * @retval Zero if resource is found and matcher determined request matches resource definition
 * @retval Non-zero CoAP status code appropriate for the mismatch
 */
typedef int (*unicoap_request_matcher_t)(const char* path, size_t path_length,
                                         const unicoap_listener_t* listener,
                                         const unicoap_resource_t** resource,
                                         const unicoap_message_t* request,
                                         const unicoap_endpoint_t* endpoint);
/**
 * @brief   A modular collection of resources for a server
 */
struct unicoap_listener {
    /**
     * @brief Reference to contiguous array of resource belonging to this listener
     */
    const unicoap_resource_t* resources;

    /**
     * @brief The number of resources belonging in this listener
     */
    size_t resource_count;

    /**
     * @brief  Function that picks a suitable request handler from a
     * request.
     *
     * @note Leaving this `NULL` selects the default strategy that picks
     * handlers by matching their `Uri-Path` to resource paths (as per
     * the documentation of the @ref resources and @ref resource_count
     * fields). Alternative handlers may cast the @ref resources and
     * @ref resource_count fields to fit their needs.
     */
    unicoap_request_matcher_t request_matcher;

    /**
     * @brief Encoder for Constrained RESTful Environments (CoRE) Link Format
     */
    unicoap_link_encoder_t link_encoder;

    /**
     * @brief Next listener in linked list
     */
    unicoap_listener_t* next;

    /**
     * @brief Allowed protocols for the resources grouped together by this resource
     *
     * Example: Set to `UNICOAP_PROTOCOL_FLAG(UNICOAP_PROTO_UDP) | UNICOAP_PROTOCOL_FLAG(UNICOAP_PROTO_DTLS)`
     * to allow only CoAP over UDP and DTLS. Alternatively you can use
     * `UNICOAP_PROTOCOLS(UNICOAP_PROTO_UDP, UNICOAP_PROTO_DTLS)`.
     *
     * @see @ref unicoap_proto_set_t
     * @see @ref UNICOAP_PROTOCOL_FLAG
     * @see @ref UNICOAP_PROTOCOLS
     * @see @ref UNICOAP_PROTOCOLS_ALLOW_ALL
     * @see @ref UNICOAP_PROTOCOLS_ALLOW_NONE
     */
    unicoap_proto_set_t protocols;
};

/**
 * @brief Makes unicoap listen for resources contained in listener
 *
 * @pre @p listener is a valid pointer to a single listener (that is,
 * `listener->next == NULL`). You must ensure the memory pointed at remains initialized until
 * the listener is deregistered. This may never happen or happen implicitly when `unicoap` is
 * deinitialized. We recommend you statically allocate the listener and its resources.
 *
 * @note
 * If you are tempted to register a pre-linked chain of listeners,
 * consider placing all their resources in the resources array of a
 * single listener instead.
 *
 * @param[in] listener  Listener containing the resources.
 *
 */
void unicoap_listener_register(unicoap_listener_t* listener);

/**
 * @brief Removes listener from unicoap
 *
 * @pre @p listener is a valid pointer to a single listener
 *
 * @param[in] listener  Listener containing the resources.
 */
void unicoap_listener_deregister(unicoap_listener_t* listener);

/**
 * @brief Determines whether the complete Uri-Path matches the resources path.
 *
 * @param[in] resource Resource
 * @param[in] path URI path such as `/foo/bar`
 * @param length Number of UTF-8 characters in @p path
 *
 * @see @ref UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE
 *
 * @return > 0 if resource paths match
 */
bool unicoap_resource_match_path_string(const unicoap_resource_t* resource, const char* path, size_t length);

/**
 * @brief Determines whether the complete Uri-Path matches the resources path.
 *
 * @param[in] resource Resource to check for matching path
 * @param[in] options Options to read URI path from
 *
 * @remark
 * If you want to call this function in a loop, consider reading the Uri-Path with
 * @ref unicoap_options_copy_uri_path and then calling @ref unicoap_resource_match_path_string
 * repeatedly. This is useful when you want to match a given request against a numer of resources.
 *
 * This function obeys the @ref UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE flag.
 *
 * @returns A boolean value indicating whether the given resource matches the path in @p options.
 */
bool unicoap_resource_match_path_options(const unicoap_resource_t* resource,
                                         const unicoap_options_t* options);
/** @} */

/**
 * @name Matching methods and protocols
 * @{
 */
/**
 * @brief A bit set at the position dictated by @p method
 * @param method The method to turn into a flag
 * @returns `1 << method`
 */
#define UNICOAP_METHOD_FLAG(method)        (1 << (method))

/**
 * @brief Macro that builds a bit field where the i-th bit indicates the i-th request method (`0.0i`)
 * @see @ref unicoap_resource_t.flags
 *
 * Use this macro to create a bitfield of allowed methods for a given
 * CoAP resource:
 * ```c
 * // Example: Allow GET and PUT
 * UNICOAP_METHODS(UNICOAP_METHOD_GET, UNICOAP_METHOD_PUT)
 * ```
 *
 * You can also use regular bitwise operators, such as OR, AND, XOR, and NOT. `UNICOAP_METHODS` is a
 * homomorphism preserving the `|` operator.
 * ```
 * UNICOAP_METHODS(UNICOAP_METHOD_GET) | UNICOAP_METHODS(UNICOAP_METHOD_PUT, UNICOAP_METHOD_POST)
 * ==
 * UNICOAP_METHODS(UNICOAP_METHOD_GET, UNICOAP_METHOD_PUT, UNICOAP_METHOD_POST)
 * ```
 */
#define UNICOAP_METHODS(first_method, ...) UNICOAP_BITFIELD(first_method, __VA_ARGS__)

/**
 * @brief @ref unicoap_method_set_t value indicating all methods are allowed
 */
#define UNICOAP_METHODS_ALL                (0xff)

/**
 * @brief Returns whether the method is allowed according to the given @p methods
 * @param methods Methods bit field, each set bit's position corresponds to a method allowed
 * @param method Method to be checked
 */
static inline bool unicoap_resource_match_method(unicoap_method_set_t methods, unicoap_method_t method)
{
    return (methods & UNICOAP_METHOD_FLAG(method)) != 0;
}

/**
 * @brief A bit set at the position dictated by @p proto
 * @param proto The protocol number to turn into a flag
 * @returns `1 << proto`
 */
#define UNICOAP_PROTOCOL_FLAG(proto)        (1 << (proto))

/**
 * @brief Macro creating a bit field describing the specified protocols.
 * @see @ref unicoap_resource_t.flags
 *
 * Use this macro to create a bitfield of allowed protocols for a given
 * CoAP resource:
 * ```c
 * // Example: Allow this resource to only be accessed over UDP and TCP
 * UNICOAP_PROTOCOLS(UNICOAP_PROTO_UDP, UNICOAP_PROTO_TCP)
 * ```
 */
#define UNICOAP_PROTOCOLS(first_proto, ...) UNICOAP_BITFIELD(first_proto, __VA_ARGS__)

/**
 * @brief @ref unicoap_proto_set_t value indicating all protocols are allowed
 */
#define UNICOAP_PROTOCOLS_ALLOW_ALL         (0)

/**
 * @brief @ref unicoap_proto_set_t value indicating no protocol is allowed
 */
#define UNICOAP_PROTOCOLS_ALLOW_NONE        (1)

/**
 * @brief Returns whether the given @p proto is allowed according to the @p protocols argument
 * @param protocols Protocols bit field, each set bit's position corresponds to a proto allowed
 * @param proto Proto to be checked
 *
 * @see @ref UNICOAP_PROTOCOLS_ALLOW_ALL
 */
static inline bool unicoap_match_proto(unicoap_proto_set_t protocols, unicoap_proto_t proto)
{
    return protocols  == UNICOAP_PROTOCOLS_ALLOW_ALL ||
        (protocols & UNICOAP_PROTOCOL_FLAG(proto)) != 0;
}
/** @} */
/** @} */

#if !defined(DOXYGEN)
extern uint8_t unicoap_receiver_buffer[CONFIG_UNICOAP_PDU_SIZE_MAX];
#endif

#ifdef __cplusplus
}
#endif
