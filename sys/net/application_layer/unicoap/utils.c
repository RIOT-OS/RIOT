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
 * @brief   Buffer and string tools
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#include <errno.h>
#include <stdbool.h>

#include "iolist.h"

#include "net/unicoap/options.h"
#include "net/unicoap/message.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

size_t unicoap_path_component_count(const unicoap_pathspec_t* path) {
    size_t count = 0;
    assert(path);
    if (!path->_components) {
        /* Root */
        return 0;
    }
    for (const char** p = path->_components; *p; p += 1) {
        count += 1;
    }
    return count;
}

void unicoap_path_print(const unicoap_pathspec_t* path) {
    assert(path);
    if (!path->_components || !*path->_components) {
        printf("/");
        return;
    }
    for (const char** p = path->_components; *p; p += 1) {
        printf("/%s", *p);
    }
}

ssize_t unicoap_path_serialize(const unicoap_pathspec_t* path, char* buffer, size_t capacity) {
    if (capacity == 0) {
        return -ENOBUFS;
    }
    if (!path->_components || !*path->_components) {
        *buffer = '/';
        return 1;
    }
    size_t og_capacity = capacity;
    for (const char** component = path->_components; *component; component += 1) {
        size_t length = strlen(*component);
        if (capacity < (length + 1)) {
            return -ENOBUFS;
        }
        capacity -= length + 1;
        *buffer = '/';
        buffer += 1;
        memcpy(buffer, *component, length);
        buffer += length;
    }

    return og_capacity - capacity;
}

bool unicoap_path_is_equal(const unicoap_pathspec_t* lhs, const unicoap_pathspec_t* rhs) {
    assert(lhs);
    assert(rhs);

    /* If one is a root path, the other must be, too. */
    if ((!lhs->_components && (!rhs->_components || !*rhs->_components)) ||
        (!rhs->_components && (!lhs->_components || !*lhs->_components))) {
        return true;
    }
    /* If just one of them is a root path, they cannot be equal. */
    if (!lhs->_components || !rhs->_components) {
        return false;
    }

    const char** l = lhs->_components;
    const char** r = rhs->_components;

    while (*l && *r) {
        if (strcmp(*l, *r) != 0) {
            break;
        }
        l += 1;
        r += 1;
    }

    /* If we reached the end of both paths simultaneously (NULL), they are equal. */
    return !*l && !*r;
}

/** @brief Returns new length of path excluding trailing slashes */
static inline size_t _trim_trailing_slashes(const char* path, size_t length) {
    if (length > 0) {
        size_t i = length - 1;
        while (i > 0) {
            if (path[i] == '/') {
                i -= 1;
            } else {
                break;
            }
        }
        return i + 1;
    } else {
        return length;
    }
}

bool unicoap_path_matches_string(const unicoap_pathspec_t* path,
                                 const char* string, size_t string_length, bool match_subtree) {

    assert(path);
    assert(string);

    char* cursor = (char*)string;

    if (path->_components) {
        for (const char** component = path->_components; *component; component += 1) {
            size_t path_component_length = strlen(*component);

            /* Ignore duplicate slashes */
            while (*cursor == '/') {
                cursor += 1;
                string_length -= 1;
            }

            if (string_length == 0) {
                return false;
            }

            /* Find end of string component */
            size_t string_component_length = string_length;
            for (size_t i = 0; i < string_length; i += 1) {
                if (cursor[i] == '/') {
                    string_component_length = i;
                    break;
                }
            }

            if (path_component_length != string_component_length) {
                return false;
            }

            if (strncmp(*component, cursor, path_component_length) != 0) {
                return false;
            }

            cursor += string_component_length;
            string_length -= string_component_length;
        }
    }

    if (match_subtree) {
        /* There may be more path components in the string options.
         * But this is fine as subpaths are allowed. */
        return true;
    } else {
        /* Make sure we read all path components in the string, i.e., the actual path is not longer
         * than the given path's. */
        for (size_t i = 0; i < string_length; i += 1) {
            if (cursor[i] != '/') {
                return false;
            }
        }
        return true;
    }
}

bool unicoap_path_matches_options(const unicoap_pathspec_t* path,
                                  const unicoap_options_t* options, bool match_subtree)
{
    assert(path);
    assert(options);
    assert(options->entries->data);
    unicoap_options_iterator_t iterator;
    /* Disqualifying the const here is fine as the iterator is only used locally and options
     * are not manipulated. As we only have one iterator concept for both mutable and read-only
     * borrowing access patterns, this is the only way to go. */
    unicoap_options_iterator_init(&iterator, (unicoap_options_t*)options);

    const char* uri_component = NULL;

    if (path->_components) {
        for (const char** component = path->_components; *component; component += 1) {
            size_t path_component_length = strlen(*component);

            int res = -1;
            if ((res = unicoap_options_get_next_uri_path_component(&iterator, &uri_component)) < 0) {
                return false;
            }

            if (path_component_length != (size_t)res) {
                return false;
            }

            if (strncmp(*component, uri_component, path_component_length) != 0) {
                return false;
            }
        }
    }

    if (match_subtree) {
        /* There may be more Uri-Path options. But this is fine as subpaths are allowed. */
        return true;
    } else {
        /* Make sure we read all options, i.e., the actual path is not longer than the given
         * paths's. */
        return unicoap_options_get_next_uri_path_component(&iterator, &uri_component) == -1;
    }
}

static inline void iolist_init(iolist_t* iolist, uint8_t* buffer, size_t size, iolist_t* next)
{
    iolist->iol_base = buffer;
    iolist->iol_len = size;
    iolist->iol_next = next;
}

static inline bool iolist_is_empty(const iolist_t* iolist)
{
    while (iolist) {
        if (iolist->iol_len > 0) {
            return false;
        }
        iolist = iolist->iol_next;
    }

    return true;
}

static void iolist_append(iolist_t** iolist, iolist_t* chunk)
{
    assert(iolist);
    if (*iolist) {
        iolist_t* v = *iolist;
        while (v->iol_next) {
            v = v->iol_next;
        }
        v->iol_next = chunk;
    }
    else {
        *iolist = chunk;
    }
}

bool unicoap_message_payload_is_empty(const unicoap_message_t* message)
{
    switch (message->payload_representation) {
    case UNICOAP_PAYLOAD_NONCONTIGUOUS:
        return iolist_is_empty(message->payload_chunks);
    case UNICOAP_PAYLOAD_CONTIGUOUS:
        return message->payload_size == 0;
    default:
        UNREACHABLE();
        return -1;
    }
}

ssize_t unicoap_message_payload_copy(const unicoap_message_t* message, uint8_t* buffer,
                                     size_t capacity)
{
    switch (message->payload_representation) {
    case UNICOAP_PAYLOAD_NONCONTIGUOUS:
        return iolist_to_buffer(message->payload_chunks, buffer, capacity);
    case UNICOAP_PAYLOAD_CONTIGUOUS:
        if (message->payload_size > capacity) {
            UNICOAP_DEBUG("buf too small " _UNICOAP_NEED_HAVE "\n",
                          message->payload_size, capacity);
            return -ENOBUFS;
        }
        memcpy(buffer, message->payload, message->payload_size);
        return message->payload_size;
    default:
        UNREACHABLE();
        return -1;
    }
}

ssize_t unicoap_message_payload_make_contiguous(unicoap_message_t* message, uint8_t* buffer,
                                                size_t capacity)
{
    switch (message->payload_representation) {
    case UNICOAP_PAYLOAD_NONCONTIGUOUS: {
        assert(buffer);
        ssize_t res = iolist_to_buffer(message->payload_chunks, buffer, capacity);
        if (res < 0) {
            UNICOAP_DEBUG("buf too small " _UNICOAP_NEED_HAVE "\n",
                          iolist_size(message->payload_chunks), capacity);
            return res;
        }
        message->payload_representation = UNICOAP_PAYLOAD_CONTIGUOUS;
        message->payload = buffer;
        message->payload_size = res;
        return res;
    }
    case UNICOAP_PAYLOAD_CONTIGUOUS:
        return message->payload_size;
    default:
        UNREACHABLE();
        return -1;
    }
}

void unicoap_message_payload_append_chunk(unicoap_message_t* message, iolist_t* chunk)
{
    assert(message->payload_representation == UNICOAP_PAYLOAD_NONCONTIGUOUS);
    if (message->payload_chunks) {
        iolist_append(&message->payload_chunks, chunk);
    }
    else {
        message->payload_chunks = chunk;
    }
}

static inline iolist_t* _append_payload_to_iolist(const unicoap_message_t* message,
                                                  iolist_t* element)
{
    switch (message->payload_representation) {
    case UNICOAP_PAYLOAD_NONCONTIGUOUS:
        return message->payload_chunks;
    case UNICOAP_PAYLOAD_CONTIGUOUS:
        iolist_init(element, message->payload, message->payload_size, NULL);
        return element;
    default:
        UNREACHABLE();
        return NULL;
    }
}

static const uint8_t _payload_separator = 0xff;

int unicoap_pdu_buildv_options_and_payload(uint8_t* header, size_t header_size,
                                           const unicoap_message_t* message,
                                           iolist_t iolists[UNICOAP_PDU_IOLIST_COUNT])
{
    assert(header);
    assert(message);
    assert(iolists);
    iolist_init(iolists, header, header_size, NULL);

    iolist_t* element = iolists;
    if (message->options && message->options->option_count > 0) {
        element->iol_next = element + 1;
        element += 1;
        iolist_init(element, unicoap_message_options_data(message),
                    unicoap_message_options_size(message), NULL);
    }

    if (unicoap_message_payload_get_size(message) > 0) {
        element->iol_next = element + 1;
        element += 1;

        iolist_init(element, (uint8_t*)&_payload_separator, 1, element + 1);

        element->iol_next = _append_payload_to_iolist(message, element + 1);
    }

    return 0;
}

ssize_t unicoap_pdu_build_options_and_payload(uint8_t* pdu, size_t capacity,
                                              const unicoap_message_t* message)
{
    assert(pdu);
    assert(message);

    if (message->options && message->options->option_count > 0) {
        if (capacity < unicoap_message_options_size(message)) {
            return -ENOBUFS;
        }
        memcpy(pdu, unicoap_message_options_data(message), unicoap_message_options_size(message));
        pdu += unicoap_message_options_size(message);
        capacity -= unicoap_message_options_size(message);
    }

    if (unicoap_message_payload_get_size(message) > 0) {
        *pdu = UNICOAP_PAYLOAD_MARKER;
        pdu += 1;
        capacity -= 1;

        ssize_t payload_size = 0;
        if ((payload_size = unicoap_message_payload_copy(message, pdu, capacity)) < 0) {
            return -ENOBUFS;
        }

        return unicoap_message_options_size(message) + 1 + payload_size;
    }
    else {
        return unicoap_message_options_size(message);
    }
}

bool unicoap_message_code_is_response(uint8_t code)
{
    switch (unicoap_code_class(code)) {
    case UNICOAP_CODE_CLASS_RESPONSE_SUCCESS:
    case UNICOAP_CODE_CLASS_RESPONSE_CLIENT_FAILURE:
    case UNICOAP_CODE_CLASS_RESPONSE_SERVER_FAILURE:
        return true;

    default:
        return false;
    }
}

const char* unicoap_string_from_code_class(uint8_t code)
{
    if (code == UNICOAP_CODE_EMPTY) {
        return "EMPTY";
    }

    switch (unicoap_code_class(code)) {
    case UNICOAP_CODE_CLASS_REQUEST:
        return "REQ";

    case UNICOAP_CODE_CLASS_RESPONSE_SUCCESS:
    case UNICOAP_CODE_CLASS_RESPONSE_CLIENT_FAILURE:
    case UNICOAP_CODE_CLASS_RESPONSE_SERVER_FAILURE:
        return "RESP";

    case UNICOAP_CODE_CLASS_SIGNAL:
        return "SIGNAL";

    default:
        return "?";
    }
}

const char* unicoap_string_from_code(uint8_t code)
{
    if (code == UNICOAP_CODE_EMPTY) {
        return "Empty";
    }

    switch (unicoap_code_class(code)) {
    case UNICOAP_CODE_CLASS_REQUEST:
        return unicoap_string_from_method((unicoap_method_t)code);

    case UNICOAP_CODE_CLASS_RESPONSE_SUCCESS:
    case UNICOAP_CODE_CLASS_RESPONSE_CLIENT_FAILURE:
    case UNICOAP_CODE_CLASS_RESPONSE_SERVER_FAILURE:
        return unicoap_string_from_status(code);

    case UNICOAP_CODE_CLASS_SIGNAL:
        return unicoap_string_from_signal(code);

    default:
        return "?";
    }
}

const char* unicoap_string_from_signal(unicoap_signal_t signal)
{
    switch (signal) {
    case UNICOAP_SIGNAL_CAPABILITIES_SETTINGS:
        return "CSM";
    case UNICOAP_SIGNAL_PING:
        return "Ping";
    case UNICOAP_SIGNAL_PONG:
        return "Pong";
    case UNICOAP_SIGNAL_ABORT:
        return "Abort";
    case UNICOAP_SIGNAL_RELEASE:
        return "Release";
    default:
        return "?";
    }
}

const char* unicoap_string_from_method(unicoap_method_t method)
{
    switch (method) {
    case UNICOAP_METHOD_GET:
        return "GET";
    case UNICOAP_METHOD_PUT:
        return "PUT";
    case UNICOAP_METHOD_POST:
        return "POST";
    case UNICOAP_METHOD_PATCH:
        return "PATCH";
    case UNICOAP_METHOD_IPATCH:
        return "iPATCH";
    case UNICOAP_METHOD_FETCH:
        return "FETCH";
    case UNICOAP_METHOD_DELETE:
        return "DELETE";
    default:
        return "?";
    }
}

const char* unicoap_string_from_status(unicoap_status_t status)
{
    switch (status) {
    case UNICOAP_STATUS_CREATED:
        return "Created";
    case UNICOAP_STATUS_DELETED:
        return "Deleted";
    case UNICOAP_STATUS_VALID:
        return "Valid";
    case UNICOAP_STATUS_CHANGED:
        return "Changed";
    case UNICOAP_STATUS_CONTENT:
        return "Content";
    case UNICOAP_STATUS_CONTINUE:
        return "Continue";
    case UNICOAP_STATUS_BAD_REQUEST:
        return "Bad Request";
    case UNICOAP_STATUS_UNAUTHORIZED:
        return "Unauthorized";
    case UNICOAP_STATUS_BAD_OPTION:
        return "Bad Option";
    case UNICOAP_STATUS_FORBIDDEN:
        return "Forbidden";
    case UNICOAP_STATUS_PATH_NOT_FOUND:
        return "Not Found";
    case UNICOAP_STATUS_METHOD_NOT_ALLOWED:
        return "Method Not Allowed";
    case UNICOAP_STATUS_NOT_ACCEPTABLE:
        return "Not Acceptable";
    case UNICOAP_STATUS_REQUEST_ENTITY_INCOMPLETE:
        return "Request Entity Incomplete";
    case UNICOAP_STATUS_CONFLICT:
        return "Conflict";
    case UNICOAP_STATUS_PRECONDITION_FAILED:
        return "Precondition Failed";
    case UNICOAP_STATUS_REQUEST_ENTITY_TOO_LARGE:
        return "Request Entity Too Large";
    case UNICOAP_STATUS_UNSUPPORTED_CONTENT_FORMAT:
        return "Unsupported Content Format";
    case UNICOAP_STATUS_UNPROCESSABLE_ENTITY:
        return "Unprocessable Entity";
    case UNICOAP_STATUS_TOO_MANY_REQUESTS:
        return "Too Many Requests";
    case UNICOAP_STATUS_INTERNAL_SERVER_ERROR:
        return "Internal Server Error";
    case UNICOAP_STATUS_NOT_IMPLEMENTED:
        return "Not Implemented";
    case UNICOAP_STATUS_BAD_GATEWAY:
        return "Bad Gateway";
    case UNICOAP_STATUS_SERVICE_UNAVAILABLE:
        return "Service Unavailable";
    case UNICOAP_STATUS_GATEWAY_TIMEOUT:
        return "Gateway Timeout";
    case UNICOAP_STATUS_PROXYING_NOT_SUPPORTED:
        return "Proxying Not Supported";
    default:
        return "?";
    }
}

const char* unicoap_string_from_option_number(unicoap_option_number_t number)
{
    switch (number) {
    case UNICOAP_OPTION_SIZE1:
        return "Size1";
    case UNICOAP_OPTION_SIZE2:
        return "Size2";
    case UNICOAP_OPTION_BLOCK1:
        return "Block1";
    case UNICOAP_OPTION_BLOCK2:
        return "Block2";
    case UNICOAP_OPTION_Q_BLOCK1:
        return "Q-Block1";
    case UNICOAP_OPTION_Q_BLOCK2:
        return "Q-Block2";
    case UNICOAP_OPTION_ECHO:
        return "Echo";
    case UNICOAP_OPTION_ETAG:
        return "ETag";
    case UNICOAP_OPTION_EDHOC:
        return "EDHOC";
    case UNICOAP_OPTION_ACCEPT:
        return "Accept";
    case UNICOAP_OPTION_OSCORE:
        return "OSCORE";
    case UNICOAP_OPTION_OBSERVE:
        return "Observe";
    case UNICOAP_OPTION_MAX_AGE:
        return "Max-Age";
    case UNICOAP_OPTION_IF_MATCH:
        return "If-Match";
    case UNICOAP_OPTION_IF_NONE_MATCH:
        return "If-None-Match";
    case UNICOAP_OPTION_LOCATION_PATH:
        return "Location-Path";
    case UNICOAP_OPTION_LOCATION_QUERY:
        return "Location-Query";
    case UNICOAP_OPTION_URI_HOST:
        return "Uri-Host";
    case UNICOAP_OPTION_URI_PORT:
        return "Uri-Port";
    case UNICOAP_OPTION_URI_QUERY:
        return "Uri-Query";
    case UNICOAP_OPTION_URI_PATH:
        return "Uri-Path";
    case UNICOAP_OPTION_HOP_LIMIT:
        return "Hop-Limit";
    case UNICOAP_OPTION_PROXY_URI:
        return "Proxy-Uri";
    case UNICOAP_OPTION_PROXY_SCHEME:
        return "Proxy-Scheme";
    case UNICOAP_OPTION_NO_RESPONSE:
        return "No-Response";
    case UNICOAP_OPTION_REQUEST_TAG:
        return "Request-Tag";
    case UNICOAP_OPTION_CONTENT_FORMAT:
        return "Content-Format";
    default:
        return "?";
    }
}

int unicoap_print_code(uint8_t code, char* string)
{
    return sprintf(string, UNICOAP_CODE_CLASS_DETAIL_FORMAT, unicoap_code_class(code),
                   unicoap_code_detail(code));
}

bool unicoap_response_is_optional(unicoap_options_t* options, unicoap_status_t status)
{
    uint8_t no_response;
    if (unicoap_options_get_no_response(options, &no_response) >= 0) {
        const uint8_t no_response_index = (status >> 5) - 1;
        /* if the handler code misbehaved here, we'd face UB otherwise */
        assert(no_response_index < 7);
        const uint8_t mask = 1 << no_response_index;

        /* option contains bitmap of disinterest */
        if (no_response & mask) {
            return true;
        }
    }
    return false;
}

void unicoap_options_dump_all(const unicoap_options_t* options)
{
    unicoap_options_iterator_t iterator = { 0 };
    unicoap_options_iterator_init(&iterator, (unicoap_options_t*)options);
    const uint8_t* value = NULL;
    ssize_t size = -1;
    unicoap_option_number_t number = 0;

    while ((size = unicoap_options_get_next(&iterator, &number, &value)) >= 0) {
        printf("<%s nr=%u size=%" PRIuSIZE " hex=", unicoap_string_from_option_number(number),
               number, size);
        for (int i = 0; i < size; i += 1) {
            printf("%02X", value[i]);
        }
        printf(">\n");
    }
}

void unicoap_options_print_uri_path(const unicoap_options_t* options)
{
    assert(options);
    unicoap_options_iterator_t iterator;
    /* Disqualifying the const here is fine as the iterator is only used locally and options
     * are not manipulated. As we only have one iterator concept for both mutable and read-only
     * borrowing access patterns, this is the only way to go. */
    unicoap_options_iterator_init(&iterator, (unicoap_options_t*)options);

    const char* uri_component = NULL;
    int res = -2;
    while ((res = unicoap_options_get_next_uri_path_component(&iterator, &uri_component)) >= 0) {
        printf("/%.*s", (int)res, uri_component);
    }

    if (res == -2) {
        /* Root path, so no Uri-Path options. */
        printf("/");
    }
}

void unicoap_print_protocols(unicoap_proto_set_t protocols)
{
    if (likely(protocols == UNICOAP_PROTOCOLS_ALLOW_ALL)) {
        printf("<all>");
    }
    else {
        printf("[ ");
        for (unicoap_proto_t p = 1; p < (sizeof(unicoap_proto_t) * 8); p += 1) {
            if (protocols & UNICOAP_PROTOCOL_FLAG(p)) {
                printf("%s ", unicoap_string_from_proto(p));
            }
        }
        printf("]");
    }
}

void unicoap_print_methods(unicoap_method_set_t methods)
{
    printf("[ ");
    for (unicoap_method_t m = 1; m < (sizeof(unicoap_method_t) * 8); m += 1) {
        if (methods & UNICOAP_METHOD_FLAG(m)) {
            printf("%s ", unicoap_string_from_method(m));
        }
    }
    printf("]");
}

void unicoap_print_resource_flags(unicoap_resource_flags_t flags)
{
    printf("[ ");
    if (flags & UNICOAP_RESOURCE_FLAG_RELIABLE) {
        printf("RELIABLE ");
    }
    if (flags & UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE) {
        printf("MATCH_SUBTREE ");
    }
    printf("]");
}

void unicoap_assist_emit_diagnostic_missing_driver(unicoap_proto_t proto)
{
    if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST) || IS_ACTIVE(ENABLE_DEBUG)) {
        switch (proto) {
        case UNICOAP_PROTO_UDP:
            unicoap_assist(API_ERROR("CoAP over UDP driver missing")
                           FIXIT("USEMODULE += unicoap_driver_udp"));
            break;

        case UNICOAP_PROTO_DTLS:
            unicoap_assist(API_ERROR("CoAP over DTLS driver missing")
                           FIXIT("USEMODULE += unicoap_driver_dtls"));
            break;

        default:
            break;
        }
    }
}
