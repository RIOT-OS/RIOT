/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "iolist.h"

#include "net/unicoap/constants.h"
#include "net/unicoap/options.h"

/**
 * @addtogroup net_unicoap_message
 * @{
 */

/**
 * @file
 * @brief  CoAP Message API
 * @author Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - CoAP message */
/**
 * @name CoAP message
 * @{
 */
/**
 * @brief Generic CoAP message
 */
typedef struct {
    /**
     * @brief Message options
     *
     * Key-Value entries akin to HTTP headers
     *
     * @see @ref net_unicoap_options
     */
    unicoap_options_t* options;

    /**
     * @brief Message payload
     */
    uint8_t* payload;

    /**
     * @brief Size of message payload
     *
     * Number of bytes in @ref unicoap_message_t.payload
     */
    size_t payload_size;

    union {
        /**
         * @brief CoAP message code
         *
         * A message code is divided into the class bits and detail bits, where `C` is a class bit and `D` is a detail bit.
         * The class bits form a single-digit number, the detail bits represents a two-digit number. Hence,
         * message codes are written as `c.dd` where `c` is the unsigned integer encoded in the three `C` bits and
         * `dd` is the beforementioned two-digit number encoded in the five `D` bits.
         */
        uint8_t code;

        /* Note:
         * Method, status, and signal are guaranteed to have the same size as uint8_t,
         * see static_asserts in definitions.h
         */

        /**
         * @brief CoAP request method
         *
         * Check if this is a request message before reading this property.
         */
        unicoap_method_t method;

        /**
         * @brief CoAP response status
         *
         * Check if this is a response message before reading this property.
         */
        unicoap_status_t status;

        /**
         * @brief CoAP signal
         *
         * Check if this is a signalling message before reading this property.
         */
        unicoap_signal_t signal;
    };
} unicoap_message_t;

/**
 * @brief Properties of a given CoAP message
 */
typedef struct {
    /** @brief CoAP token used to correlate requests to responses */
    uint8_t* token;

    /** @brief Length CoAP message token */
    uint8_t token_length : UNICOAP_TOKEN_LENGTH_FIXED_WIDTH;

    /**
     * @brief Determines if the corresponding message is considered an `Observe` registration
     *
     * This property avoids the need to read the `Observe` option every time this value is needed
     */
    bool is_registration : 1;

    /**
     * @brief Determines if the corresponding message is considered an `Observe` notification
     *
     * This property avoids the need to read the `Observe` option every time this value is needed
     */
    bool is_notification : 1;

    /** @brief RFC 7252 only properties */
    struct {
        /** @brief RFC 7252 message type */
        unicoap_message_type_t type : UNICOAP_MESSAGE_TYPE_FIXED_WIDTH;

        /** @brief RFC 7252 message ID */
        uint16_t id;
    } rfc7252;

    /** @brief CoAP message token */
} unicoap_message_properties_t;

/**
 * @brief Returns a null-terminated label for the CoAP over UDP/DTLS message type
 *
 * @param type CoAP over UDP/DTLS message.
 *
 * @returns Message type label, never `NULL`
 * Possible return values are are:
 * - `"NON"`: A non-confirmable message
 * - `"CON"`: A confirmable message
 * - `"ACK"`: An acknowledgement message
 * - `"RST"`: A reset message
 * - `"?"`: Unknown message type
 */
const char* unicoap_string_from_rfc7252_type(unicoap_message_type_t type);

/**
 * @brief Retrieves options storage buffer
 * @param[in] message Message
 * @returns Pointer to first byte of storage buffer, can be `NULL`
 */
static inline uint8_t* unicoap_message_options_data(const unicoap_message_t* message)
{
    return message->options ? message->options->entries->data : NULL;
}

/**
 * @brief Retrieves total size of options in buffer
 * @param[in] message Message
 * @returns Size of options in buffer in bytes
 */
static inline size_t unicoap_message_options_size(const unicoap_message_t* message)
{
    return message->options ? message->options->storage_size : 0;
}
/** @} */

/* MARK: - CoAP code */
/**
 * @name CoAP code
 * @{
 */
/**
 * @brief Reads the code class number encoded in the given code
 *
 * A message code is divided into the class bits and detail bits, where `C` is a class bit and `D` is a detail bit.
 * The class bits form a single-digit number, the detail bits represents a two-digit number. Hence,
 * message codes are written as `c.dd` where `c` is the unsigned integer encoded in the three `C` bits and
 * `dd` is the beforementioned two-digit number encoded in the five `D` bits.
 * ```
 *   Bit
 *   7 6 5   4   3 2 1 0
 * [ C C C | D   D D D D ]
 * ```
 *
 * @param code Message code
 * @return Code class number
 *
 * @see @ref unicoap_code_detail
 */
static inline uint8_t unicoap_code_class(uint8_t code)
{
    return code >> 5;
}

/**
 * @brief Reads the code detail number encoded in the given code
 *
 * A message code is divided into the class bits and detail bits, where `C` is a class bit and `D` is a detail bit.
 * The class bits form a single-digit number, the detail bits represents a two-digit number. Hence,
 * message codes are written as `c.dd` where `c` is the unsigned integer encoded in the three `C` bits and
 * `dd` is the beforementioned two-digit number encoded in the five `D` bits.
 * ```
 *   Bit
 *   7 6 5   4   3 2 1 0
 * [ C C C | D   D D D D ]
 * ```
 *
 * @param code Message code
 * @return Code detail number
 *
 * @see @ref unicoap_code_class
 */
static inline uint8_t unicoap_code_detail(uint8_t code)
{
    return code & 0x1f;
}

/**
 * @brief Prints a `c.dd` class-detail string into the given buffer.
 * @pre @p string must have a capacity of at least 4 characters
 * @param code Message code
 * @param[out] string Buffer to write string into
 *
 */
int unicoap_print_code(uint8_t code, char* string);

/**
 * @brief Returns label for given CoAP message code
 *
 * @param code CoAP message code
 *
 * @returns Label, such as `"POST"`, `"Abort"`, or `"Bad Request"`
 * @returns `"?"` if code is unknown
 */
const char* unicoap_string_from_code(uint8_t code);

/**
 * @brief Returns label for given CoAP message code class
 *
 * @param code CoAP message code
 *
 * @returns Label, `"REQ"`, `"RES"`, `"SIGNAL"`, or `"EMPTY"`
 * @returns `"?"` if code class is unknown
 */
const char* unicoap_string_from_code_class(uint8_t code);
/** @} */

/* MARK: - Message initializers */
/**
 * @name Message initializers
 * @{
 */
/**
 * @brief Initializes message with no payload and no options
 * @param code Message code
 * @param[in,out] message Pre-allocated message structure
 */
static inline void unicoap_message_init_empty(unicoap_message_t* message, uint8_t code)
{
    message->code = code;
    message->options = NULL;
    message->payload = NULL;
    message->payload_size = 0;
}

/**
 * @brief Creates message with no payload and no options
 * @param code Message code
 * @return Message structure
 */
static inline unicoap_message_t unicoap_message_empty(uint8_t code)
{
    return (unicoap_message_t){ .code = code };
}

/**
 * @brief Initializes message with payload but no options
 * @param[in,out] message Pre-allocated message structure
 * @param code Message code
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 */
static inline void unicoap_message_init(unicoap_message_t* message, uint8_t code, uint8_t* payload,
                                        size_t payload_size)
{
    message->code = code;
    message->options = NULL;
    message->payload = payload;
    message->payload_size = payload_size;
}

/**
 * @brief Creates message with no payload and no options
 * @param code Message code
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 * @return Message structure
 */
static inline unicoap_message_t unicoap_message(uint8_t code, uint8_t* payload, size_t payload_size)
{
    return (unicoap_message_t){ .code = code, .payload = payload, .payload_size = payload_size };
}

/**
 * @brief Initializes message with string payload but no options
 * @param[in,out] message Pre-allocated message structure
 * @param code Message code
 * @param[in] payload Payload string (nullable), must be null-terminated
 */
static inline void unicoap_message_init_string(unicoap_message_t* message, uint8_t code,
                                               const char* payload)
{
    message->code = code;
    message->options = NULL;
    message->payload = (uint8_t*)payload;
    message->payload_size = payload ? strlen(payload) : 0;
}

/**
 * @brief Creates message with string payload but no options
 * @param code Message code
 * @param[in] payload Payload string (nullable), must be null-terminated
 * @return Message structure
 */
static inline unicoap_message_t unicoap_message_string(uint8_t code, const char* payload)
{
    return (unicoap_message_t){ .code = code,
                                .payload = (uint8_t*)payload,
                                .payload_size = payload ? strlen(payload) : 0 };
}

/**
 * @brief Initializes message with string payload and options
 * @param[in,out] message Pre-allocated message structure
 * @param code Message code
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 */
static inline void unicoap_message_init_options(unicoap_message_t* message, uint8_t code,
                                                uint8_t* payload, size_t payload_size,
                                                unicoap_options_t* options)
{
    message->code = code;
    message->options = options;
    message->payload = payload;
    message->payload_size = payload_size;
}

/**
 * @brief Creates message with string payload and options
 * @param code Message code
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 * @return Message structure
 */
static inline unicoap_message_t unicoap_message_options(uint8_t code, uint8_t* payload,
                                                        size_t payload_size,
                                                        unicoap_options_t* options)
{
    return (unicoap_message_t){
        .code = code, .options = options, .payload = payload, .payload_size = payload_size
    };
}

/**
 * @brief Initializes message with string payload and options
 * @param[in,out] message Pre-allocated message structure
 * @param code Message code
 * @param[in] payload Payload string (nullable), must be null-terminated
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 */
static inline void unicoap_message_init_string_options(unicoap_message_t* message, uint8_t code,
                                                       const char* payload,
                                                       unicoap_options_t* options)
{
    message->code = code;
    message->options = options;
    message->payload = (uint8_t*)payload;
    message->payload_size = payload ? strlen(payload) : 0;
}

/**
 * @brief Creates message with string payload and options
 * @param code Message code
 * @param[in] payload Payload string (nullable), must be null-terminated
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 * @return Message structure
 */
static inline unicoap_message_t unicoap_message_string_options(uint8_t code, const char* payload,
                                                               unicoap_options_t* options)
{
    return (unicoap_message_t){ .code = code,
                                .options = options,
                                .payload = (uint8_t*)payload,
                                .payload_size = payload ? strlen(payload) : 0 };
}
/** @} */

/* MARK: - Signaling messages */
/**
 * @name Signaling messages
 * @{
 */
/**
 * @brief Determines if the the given message's code is a a signaling code
 *
 * This function internally reads the code class encoded in the code's upper three bits.
 *
 * @param code Message code
 *
 * @return A boolean value indicating whether the given message is a signal
 *
 * @see @ref unicoap_code_class
 *
 * @see @ref unicoap_code_detail
 */
static inline bool unicoap_message_is_signal(uint8_t code)
{
    return unicoap_code_class(code) == UNICOAP_CODE_CLASS_SIGNAL;
}

/**
 * @brief Retrieves signal code from given signal message
 *
 * @param[in] message CoAP signal
 * @returns @ref unicoap_signal_t
 */
static inline unicoap_signal_t unicoap_message_get_signal(const unicoap_message_t* message)
{
    return message->signal;
}

/**
 * @brief Sets signal code of given signal message
 *
 * @param[in,out] message CoAP signal
 * @param signal Signal code
 */
static inline void unicoap_message_set_signal(unicoap_message_t* message, unicoap_signal_t signal)
{
    message->signal = signal;
}

/**
 * @brief Returns label for given CoAP signal code
 *
 * @param signal CoAP signal code
 *
 * @returns Label, such as `"Ping"`, `"Abort"`, or `"CSM"`
 * @returns `"?"` if code is unknown
 */
const char* unicoap_string_from_signal(unicoap_signal_t signal);
/** @} */

/* MARK: - Request messages */
/**
 * @name Request messages
 * @{
 */
/**
 * @brief Determines if the the given message's code is a a request code
 *
 * This function internally reads the code class encoded in the code's upper three bits.
 *
 * @param code Message code
 * @return A boolean value indicating whether the given message is a request
 *
 * @see @ref unicoap_code_class
 * @see @ref unicoap_code_detail
 */
static inline bool unicoap_message_is_request(uint8_t code)
{
    return unicoap_code_class(code) == UNICOAP_CODE_CLASS_REQUEST;
}

/**
 * @brief Obtains request method from the given message's code
 * @param[in] request Request message, pre-initialized
 * @returns Request method
 */
static inline unicoap_method_t unicoap_request_get_method(const unicoap_message_t* request)
{
    return request->method;
}

/**
 * @brief Sets request method
 * @param[in,out] request Request message, pre-allocated
 * @param method Request method to set
 */
static inline void unicoap_request_set_method(unicoap_message_t* request, unicoap_method_t method)
{
    request->method = method;
}

/**
 * @brief Obtains label for given request method
 *
 * Returns `"GET"`, `"PUT"`, `"POST"`, ...
 *
 * @param method Request method to return label for
 * @returns String label
 */
const char* unicoap_string_from_method(unicoap_method_t method);

/**
 * @brief Initializes request with no payload and no options
 * @param method Request method
 * @param[in,out] request Pre-allocated request structure
 */
static inline void unicoap_request_init_empty(unicoap_message_t* request, unicoap_method_t method)
{
    unicoap_message_init_empty(request, (uint8_t)method);
}

/**
 * @brief Creates request with no payload and no options
 * @param method Request method
 * @return Request structure
 */
static inline unicoap_message_t unicoap_request_empty(unicoap_method_t method)
{
    return unicoap_message_empty((uint8_t)method);
}

/**
 * @brief Initializes request with payload but no options
 * @param[in,out] request Pre-allocated request structure
 * @param method Request method
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 */
static inline void unicoap_request_init(unicoap_message_t* request, unicoap_method_t method,
                                        uint8_t* payload, size_t payload_size)
{
    unicoap_message_init(request, (uint8_t)method, payload, payload_size);
}

/**
 * @brief Creates request with no payload and no options
 * @param method Request method
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 * @return Request structure
 */
static inline unicoap_message_t unicoap_request(unicoap_method_t method, uint8_t* payload,
                                                size_t payload_size)
{
    return unicoap_message((uint8_t)method, payload, payload_size);
}

/**
 * @brief Initializes request with string payload but no options
 * @param[in,out] request Pre-allocated request structure
 * @param method Request method
 * @param[in] payload Payload string (nullable), must be null-terminated
 */
static inline void unicoap_request_init_string(unicoap_message_t* request, unicoap_method_t method,
                                               const char* payload)
{
    unicoap_message_init_string(request, (uint8_t)method, payload);
}

/**
 * @brief Creates request with string payload but no options
 * @param method Request method
 * @param[in] payload Payload string (nullable), must be null-terminated
 * @return Request structure
 */
static inline unicoap_message_t unicoap_request_string(unicoap_method_t method, const char* payload)
{
    return unicoap_message_string((uint8_t)method, payload);
}

/**
 * @brief Initializes request with string payload and options
 * @param[in,out] request Pre-allocated request structure
 * @param method Request method
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 */
static inline void unicoap_request_init_options(unicoap_message_t* request, unicoap_method_t method,
                                                uint8_t* payload, size_t payload_size,
                                                unicoap_options_t* options)
{
    unicoap_message_init_options(request, (uint8_t)method, payload, payload_size, options);
}

/**
 * @brief Creates request with string payload and options
 * @param method Request method
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 * @return Request structure
 */
static inline unicoap_message_t unicoap_request_options(unicoap_method_t method, uint8_t* payload,
                                                        size_t payload_size,
                                                        unicoap_options_t* options)
{
    return unicoap_message_options((uint8_t)method, payload, payload_size, options);
}

/**
 * @brief Initializes request with string payload and options
 * @param[in,out] request Pre-allocated request structure
 * @param method Request method
 * @param[in] payload Payload string (nullable), must be null-terminated
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 */
static inline void unicoap_request_init_string_options(unicoap_message_t* request,
                                                       unicoap_method_t method, const char* payload,
                                                       unicoap_options_t* options)
{
    unicoap_message_init_string_options(request, (uint8_t)method, payload, options);
}

/**
 * @brief Creates request with string payload and options
 * @param method Request method
 * @param[in] payload Payload string (nullable), must be null-terminated
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 * @return Request structure
 */
static inline unicoap_message_t unicoap_request_string_options(unicoap_method_t method,
                                                               const char* payload,
                                                               unicoap_options_t* options)
{
    return unicoap_message_string_options((uint8_t)method, payload, options);
}
/** @} */

/* MARK: - Response messages */
/**
 * @name Response messages
 * @{
 */
/**
 * @brief Determines if the the given message's code is a a response code
 *
 * This function internally reads the code class encoded in the code's upper three bits.
 *
 * @param code Message code
 *
 * @return A boolean value indicating whether the given message is a response
 *
 * @see @ref unicoap_code_class
 *
 * @see @ref unicoap_code_detail
 */
bool unicoap_message_is_response(uint8_t code);

/**
 * @brief Obtains response status from the given message's code
 * @param[in] response Response message, pre-initialized
 * @returns Response status
 */
static inline unicoap_status_t unicoap_response_get_status(const unicoap_message_t* response)
{
    return response->status;
}

/**
 * @brief Sets response status
 * @param[in,out] response Response message, pre-allocated
 * @param status Response status to set
 */
static inline void unicoap_response_set_status(unicoap_message_t* response, unicoap_status_t status)
{
    response->status = status;
}

/**
 * @brief Generates short descriptive label from CoAP status code
 * @param status CoAP status code
 * @returns Status string or `"?"` if status code is unknown
 */
const char* unicoap_string_from_status(unicoap_status_t status);

/**
 * @brief Initializes response with no payload and no options
 * @param status Response status
 * @param[in,out] response Pre-allocated response structure
 */
static inline void unicoap_response_init_empty(unicoap_message_t* response, unicoap_status_t status)
{
    unicoap_message_init_empty(response, (uint8_t)status);
}

/**
 * @brief Creates response with no payload and no options
 * @param status Response status
 * @return Response structure
 */
static inline unicoap_message_t unicoap_response_empty(unicoap_status_t status)
{
    return unicoap_message_empty((uint8_t)status);
}

/**
 * @brief Initializes response with payload but no options
 * @param[in,out] response Pre-allocated response structure
 * @param status Response status
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 */
static inline void unicoap_response_init(unicoap_message_t* response, unicoap_status_t status,
                                         uint8_t* payload, size_t payload_size)
{
    unicoap_message_init(response, (uint8_t)status, payload, payload_size);
}

/**
 * @brief Creates response with no payload and no options
 * @param status Response status
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 * @return Response structure
 */
static inline unicoap_message_t unicoap_response(unicoap_status_t status, uint8_t* payload,
                                                 size_t payload_size)
{
    return unicoap_message((uint8_t)status, payload, payload_size);
}

/**
 * @brief Initializes response with string payload but no options
 * @param[in,out] response Pre-allocated response structure
 * @param status Response status
 * @param[in] payload Payload string (nullable), must be null-terminated
 */
static inline void unicoap_response_init_string(unicoap_message_t* response,
                                                unicoap_status_t status, const char* payload)
{
    unicoap_message_init_string(response, (uint8_t)status, payload);
}

/**
 * @brief Creates response with string payload but no options
 * @param status Response status
 * @param[in] payload Payload string (nullable), must be null-terminated
 * @return Response structure
 */
static inline unicoap_message_t unicoap_response_string(unicoap_status_t status,
                                                        const char* payload)
{
    return unicoap_message_string((uint8_t)status, payload);
}

/**
 * @brief Initializes response with string payload and options
 * @param[in,out] response Pre-allocated response structure
 * @param status Response status
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 */
static inline void unicoap_response_init_options(unicoap_message_t* response,
                                                 unicoap_status_t status, uint8_t* payload,
                                                 size_t payload_size, unicoap_options_t* options)
{
    unicoap_message_init_options(response, (uint8_t)status, payload, payload_size, options);
}

/**
 * @brief Creates response with string payload and options
 * @param status Response status
 * @param[in] payload Payload bytes (nullable)
 * @param payload_size Payload size
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 * @return Response structure
 */
static inline unicoap_message_t unicoap_response_options(unicoap_status_t status, uint8_t* payload,
                                                         size_t payload_size,
                                                         unicoap_options_t* options)
{
    return unicoap_message_options((uint8_t)status, payload, payload_size, options);
}

/**
 * @brief Initializes response with string payload and options
 * @param[in,out] response Pre-allocated response structure
 * @param status Response status
 * @param[in] payload Payload string (nullable), must be null-terminated
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 */
static inline void unicoap_response_init_string_options(unicoap_message_t* response,
                                                        unicoap_status_t status,
                                                        const char* payload,
                                                        unicoap_options_t* options)
{
    unicoap_message_init_string_options(response, (uint8_t)status, payload, options);
}

/**
 * @brief Creates response with string payload and options
 * @param status Response status
 * @param[in] payload Payload string (nullable), must be null-terminated
 * @param[in] options Message options, must be pre-allocated and pre-initialized
 * @return Response structure
 */
static inline unicoap_message_t unicoap_response_string_options(unicoap_status_t status,
                                                                const char* payload,
                                                                unicoap_options_t* options)
{
    return unicoap_message_string_options((uint8_t)status, payload, options);
}
/** @} */
/** @} */

/* MARK: - Parsing and Serialization ------------------------------------------------------- */
/**
 * @addtogroup net_unicoap_pdu
 * @{
 */
/* MARK: - Parsing */
/**
 * @name Parsing
 * @{
 */
/**
 * @brief Common PDU parser
 *
 * The parser should call @ref unicoap_pdu_parse_options_and_payload when it's finished parsing the
 * PDU header.
 *
 * @param[in] pdu PDU buffer
 * @param size PDU size
 * @param[in,out] message Pre-allocated message
 * @param[in,out] properties Pre-allocated properties
 *
 */
typedef ssize_t (*unicoap_parser_t)(const uint8_t* pdu, size_t size, unicoap_message_t* message,
                                    unicoap_message_properties_t* properties);

/**
 * @brief Helper structure for parsing PDUs manually
 *
 * Use this struct to allocate all necessary artifacts for invoking a parser
 */
typedef struct {
    /** @brief Message */
    unicoap_message_t message;

    /** @brief Parsed options */
    unicoap_options_t options;

    /** @brief Parsed message properties */
    unicoap_message_properties_t properties;
} unicoap_parser_result_t;

/**
 * @brief Helper method for manually parsing a PDU
 *
 * @param[in] pdu PDU buffer
 * @param size PDU size
 * @param[out] parsed Pre-allocated parsed message structure
 * @param parser Parser to use
 *
 * @returns @ref unicoap_parser_t result
 *
 * This function is marked `static inline`, thus entails no runtime overhead
 */
static inline ssize_t unicoap_pdu_parse(const uint8_t* pdu, size_t size, unicoap_parser_t parser,
                                        unicoap_parser_result_t* parsed)
{
    parsed->message.options = &parsed->options;
    return parser(pdu, size, &parsed->message, &parsed->properties);
}

/**
 * @brief Parses PDU starting at options
 *
 * Call this method after you have parsed the CoAP header.
 *
 * @param[in] cursor Start of options buffer
 * @param[out] end Pointer to after last buffer element
 * @param[in,out] message Pre-allocated message two write options and payload into. Must have valid options pointer
 *
 * @returns `0` on success
 * @returns Negative errno on failure
 */
ssize_t unicoap_pdu_parse_options_and_payload(uint8_t* cursor, const uint8_t* end,
                                              unicoap_message_t* message);

/**
 * @brief Parses RFC 7252 PDU
 *
 * @param pdu Buffer containing PDU to parse
 * @param size Size of PDU in bytes
 * @param[out] message Pre-allocated message to populate, should have options set
 * @param[out] properties Pre-allocated properties structure to populate
 *
 * @returns `0` on success
 * @returns Negative errno on failure
 *
 * @remark To allocate everything needed in one go, use @ref unicoap_pdu_parse instead.
 */
ssize_t unicoap_pdu_parse_rfc7252(const uint8_t* pdu, size_t size, unicoap_message_t* message,
                                  unicoap_message_properties_t* properties);

/**
 * @brief Helper method for manually parsing a PDU
 *
 * @param[in] pdu PDU buffer
 * @param size PDU size
 * @param[out] parsed Pre-allocated parsed message structure
 *
 * @returns @ref unicoap_parser_t result
 *
 * This function is marked `static inline`, thus entails no runtime overhead
 */
static inline ssize_t unicoap_pdu_parse_rfc7252_result(const uint8_t* pdu, size_t size, unicoap_parser_result_t* parsed)
{
    return unicoap_pdu_parse(pdu, size, unicoap_pdu_parse_rfc7252, parsed);
}
/** @} */

/* MARK: - Serializing */
/**
 * @name Serializing
 * @{
 */
/**
 * @brief Populates the given buffer with options and payload
 *
 * Call this method with the remaining capacity after you have written the CoAP header into a PDU buffer
 *
 * @param[in] cursor Pointer to first byte after header
 * @param remaining_capacity Capacity remaining for options, payload marker, and payload
 * @param[in] message Message containing options and payload
 *
 * @returns Number of bytes written
 * @returns `-ENOBUFS` if buffer is too small
 */
ssize_t unicoap_pdu_build_options_and_payload(uint8_t* cursor, size_t remaining_capacity, const unicoap_message_t* message);

/**
 * @brief Writes RFC 7252 PDU header in the given buffer
 *
 * @param[in,out] header Buffer the header will be written into
 * @param capacity Number of usable bytes in the @p header buffer
 * @param[in] message Message to construct header from (use code or payload_size)
 * @param[in] properties Message properties to serialize into the header
 *
 * @returns Header size
 */
ssize_t unicoap_pdu_build_header_rfc7252(uint8_t* header, size_t capacity,
                                         const unicoap_message_t* message,
                                         const unicoap_message_properties_t* properties);

/**
 * @brief Writes RFC 7252 PDU into buffer
 *
 * @param[in,out] pdu Buffer
 * @param capacity PDU buffer capacity
 * @param[in] message Message
 * @param[in] properties Message properties containing ID and type
 *
 * @returns Size of PDU
 * @returns Negative integer one error
 */
static inline ssize_t unicoap_pdu_build_rfc7252(uint8_t* pdu, size_t capacity,
                                                const unicoap_message_t* message,
                                                const unicoap_message_properties_t* properties)
{
    ssize_t res = 0;
    assert(capacity > UNICOAP_HEADER_SIZE_MAX);

    if ((res = unicoap_pdu_build_header_rfc7252(pdu, capacity, message, properties)) < 0) {
        return res;
    }
    return unicoap_pdu_build_options_and_payload(pdu + res, capacity - res, message) + res;
}

/* MARK: unicoap_driver_extension_point */

/** @brief Number iolists in the iolist buffer that must be passed to @ref unicoap_pdu_buildv_options_and_payload */
#define UNICOAP_PDU_IOLIST_COUNT (4)

/**
 * @brief Populates the given iolist with header, options, payload separator and payload
 *
 * Use this method to construct a vector message for vectored send functions in your transport
 * driver.
 *
 * The iolist is squashed if a carbon copy buffer is present, i.e., all iolist chunks are copied into the
 * carbon copy buffer. In that case, the first iolist element will hold the reference to the carbon copy
 * buffer.
 *
 * @param[in] header Encoded CoAP header
 * @param header_size Size of @p header
 * @param[in] message Message containing options and payload
 * @param[in,out] iolists Buffer of iolists, pre-allocated, size must be be @ref UNICOAP_PDU_IOLIST_COUNT
 *
 * @returns `0` on success
 * @returns `-ENOBUFS` if carbon copy buffer is too small
 *
 */
int unicoap_pdu_buildv_options_and_payload(uint8_t* header, size_t header_size, const unicoap_message_t* message,
                                           iolist_t* iolists);

/**
 * @brief Populates iolist with header, options, and payload
 *
 * @param[in] header Header buffer
 * @param header_capacity Capacity of header buffer
 * @param[in] message Message containing options and payload
 * @param[in] properties Message properties containing ID and type
 * @param[in,out] iolists Buffer of iolists, pre-allocated, size must be be @ref UNICOAP_PDU_IOLIST_COUNT
 *
 * @returns `0` on success
 * @returns Negative integer one error
 */
static inline ssize_t unicoap_pdu_buildv_rfc7252(uint8_t* header, size_t header_capacity, const unicoap_message_t* message, const unicoap_message_properties_t* properties, iolist_t* iolists)
{
    ssize_t res = 0;
    assert(header_capacity >= UNICOAP_HEADER_SIZE_MAX);

    if ((res = unicoap_pdu_build_header_rfc7252(header, header_capacity, message, properties)) < 0) {
        return res;
    }
    return unicoap_pdu_buildv_options_and_payload(header, res, message, iolists);
}

/* MARK: unicoap_driver_extension_point */

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif
