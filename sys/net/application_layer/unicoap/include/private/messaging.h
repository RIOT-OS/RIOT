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

#include "net/unicoap.h"

/**
 * @addtogroup net_unicoap_private
 * @{
 */

/**
 * @file
 * @brief  Messaging API
 * @author Carl Seifert <carl.seifert@tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - Private Messaging API */
/**
 * @name Private Messaging API
 * @{
 */

/**
 * @brief Messaging layer result after initial treatment of an inbound message
 *
 * ```
 *             /
 *             |      Application
 *             /          /|\
 *     Stack  <            |   API
 *             \          \|/
 *             |  Request/response layer  ------+
 *             \           ^                     \   Events
 *                         |                     /   (truncated, expected, deferred, ...)
 *             /  Messaging layer (driver)  <---+
 *             |           ^
 *             /           |
    Driver(s) <      Parser (driver)
 *             \           ^
 *             |           |
 *             \     Network (driver)
 *
 * Fig. 1: Inbound message path through the stack
 * ```
 *
 * @remark You can always use a compile-time bitwise OR to check for multiple flags to check
 * for generally acceptable or unacceptable messages.
 */

/**
 * @brief Flags for sending a message communicated by the unicoap stack to the messaging driver
 *
 * Each time the stack wants to send a message, @ref unicoap_messaging_send will be called, which,
 * in turn, invokes the messaging driver implementation.
 *
 * @warning Bits other than those defined as enum cases are RFU. Do not use them or make
 * assumptions on their value.
 *
 * These flags are intended for messaging and transmission options. Messaging options may change
 * the CoAP message flow (e.g., waiting for an acknowledgement) or transmission
 * behavior (e.g., using an optional reliability layer of the underlying transport protocol or
 * implementation).
 *
 * ### Relationship with Exchange-Layer Flags
 * Messaging flags are provided as part of client and resource flags. The
 * (TODO: ref) unicoap_client_flags_t and @ref unicoap_resource_flags_t are separated into
 * - exchange flags needed for features building on top the request/response level and
 * - messaging flags.
 *
 * The stack communicates only the messaging flags to the driver, in the form of this bitfield.
 */
typedef enum {

    /**
     * @brief Messaging flag indicating the stack wants this message to be transmitted reliably
     *
     * For transport protocol that already provide reliability, the messaging driver can ignore this
     * flag. If the CoAP messaging layer of your driver supports optional reliability, such as the
     * RFC 7252 driver, turn on this behavior if you are given this flag.
     *
     * ## Example
     * The RFC 7252 (CoAP over UDP and DTLS) supports optional reliability in the form of
     * confirmable message. In contrast to non-confirmable messages, confirmable messages
     * elicit an acknowledgement message sent by the receiver. The sender retransmits the original
     * confirmable message using an exponential back-of mechanism until it receives the
     * acknowledgement.
     */
    UNICOAP_MESSAGING_FLAG_RELIABLE = 0x01,

    /* MARK: unicoap_driver_extension_point */
} __attribute__((__packed__)) unicoap_messaging_flags_t;

/**
 * @brief The messaging flags the stack uses when no source for messaging flags is available
 *
 * These flags are used if the server needs to send a 4.04 Path Not Found response,
 * or the message is truncated.
 */
#define UNICOAP_MESSAGING_FLAGS_DEFAULT (0)

/**
 * @brief Argument passed to exchange-layer processing function
 */
typedef union {
    /** @brief Resource */
    const unicoap_resource_t* resource;

} unicoap_exchange_arg_t;

/**
 * @brief Result of @ref unicoap_exchange_preprocess
 */
typedef enum {
    /**
     * @brief The given message's code class is not valid
     *
     * Currently considered valid:
     *  - `0.xx` Requests, except `0.00`
     *  - `2.xx`, `4.xx`, and `5.xx` Responses
     *  - `7.xx` Signals
     */
    UNICOAP_PREPROCESSING_ERROR_INVALID_CODE_CLASS = -0x1001,

    /**
     * @brief The given message cannot be processed due to being unsupported
     */
    UNICOAP_PREPROCESSING_ERROR_UNSUPPORTED = -0x1002,

    /**
     * @brief The given message is unexpected and cannot be processed
     *
     * There's no known client exchange running.
     */
    UNICOAP_PREPROCESSING_ERROR_RESPONSE_UNEXPECTED = -0x1004,

    /**
     * @brief Unexpected notification, no known client exchange (observation)
     *
     * There's no known client exchange, i.e., observation, running.
     */
    UNICOAP_PREPROCESSING_ERROR_NOTIFICATION_UNEXPECTED = -0x100c,

    /**
     * @brief The given message is truncated and can thus not be handled.
     */
    UNICOAP_PREPROCESSING_ERROR_TRUNCATED = -0x1010,

    /**
     * @brief The given message cannot be handled, disregarded
     */
    UNICOAP_PREPROCESSING_ERROR_REQUEST = -0x1020,

    /**
     * @brief The given message can be processed
     *
     * There's an active client exchange.
     */
    UNICOAP_PREPROCESSING_SUCCESS_RESPONSE = 0x1001,

    /**
     * @brief The given message can be processed
     *
     * There's a resource that can handle this request
     */
    UNICOAP_PREPROCESSING_SUCCESS_REQUEST = 0x1002,

} unicoap_preprocessing_result_t;

/**
 * @brief Performs initial processing of a CoAP packet after it has been parsed
 *
 * The processing of a CoAP message is divided into two parts: preprocessing and the actual
 * processing. This design was chosen to allow the messaging driver to implement networking logic
 * in between.
 *
 * *Example:*
 * The RFC 7252 messaging driver sends an ACK for every confirmable response received at the client.
 * Waiting for the application to handle the response could delay the ACK, possibly lead to
 * retransmissions or even open a timing-based side channel.
 *
 * @param[in,out] packet Packet received
 * @param[out] flags Messaging flags associated with resource or client exchange
 * @param[out] arg Argument to be passed to the processor
 * @param truncated A boolean value indicating whether the given PDU was truncated, but could still
 *                  be parsed (i.e., a valid message)
 *
 * @returns @ref unicoap_preprocessing_result_t
 */
unicoap_preprocessing_result_t unicoap_exchange_preprocess(unicoap_packet_t* packet,
                                                           unicoap_messaging_flags_t* flags,
                                                           unicoap_exchange_arg_t* arg,
                                                           bool truncated);

/**
 * @brief Processes message
 *
 * Call this after you have preprocessed the message using @ref unicoap_exchange_preprocess.
 *
 * @param[in,out] packet CoAP packet
 * @param[in] arg Argument returned (via out parameter) by @ref unicoap_exchange_preprocess
 *
 * @retval Zero if processing succeeds
 * @retval Negative error number if processing failed
 */
int unicoap_exchange_process(unicoap_packet_t* packet, unicoap_exchange_arg_t arg);

/**
 * @brief Releases all buffers and state allocated in connection with the given endpoint
 *
 * Call this function from your driver if you encounter a severe error with a given endpoint
 *
 * @param[in] endpoint Remote endpoint to release buffers for
 *
 * @return Negative error integer or zero on success.
 * @retval `-ENOENT` if no state is currently associated with the given endpoint
 */
int unicoap_exchange_release_endpoint_state(const unicoap_endpoint_t* endpoint);

/**
 * @brief Internal RFC 7252 messaging inbound processor
 * @param[in] pdu Buffer containing PDU
 * @param size Size of PDU in bytes
 * @param truncated A boolean value indicating whether the message has been truncated by the
 *                  transport layer
 * @param[in] packet Packet to process
 *
 * @returns Negative error number in case of a failure, zero otherwise.
 *
 * This function forwards the `truncated` characteristic to the exchange layer, which can handle
 * that scenario appropriately, such as by setting a Size option.
 *
 * @remark While it is not advised to call private API, you might want to consider calling this
 * function in a very constrained environment or when using `sock` is not an option.
 */
int unicoap_messaging_process_rfc7252(const uint8_t* pdu, size_t size, bool truncated,
                                      unicoap_packet_t* packet);

/* MARK: unicoap_driver_extension_point */

/**
 * @brief Forwards packet to messaging layer and the corresponding driver for further message
 * processing, encoding, and transport I/O.
 *
 * The driver needs to encode the given message into a serialized PDU and use its own means
 * of transmitting the packet to the packet's remote endpoint.
 *
 * This function calls the individual driver messaging implementation.
 *
 * @param[in,out] packet Packet to send
 * @param flags Messaging flags
 * @returns Zero on success or negative error value. See @ref unicoap_messaging_send_rfc7252.
 */
int unicoap_messaging_send(unicoap_packet_t* packet, unicoap_messaging_flags_t flags);

/** @brief Sends CoAP over UDP or DTLS packet, see @ref unicoap_messaging_send */
int unicoap_messaging_send_rfc7252(unicoap_packet_t* packet, unicoap_messaging_flags_t flags);

/**
 * @brief Generates new token
 *
 * Token will be @ref CONFIG_UNICOAP_GENERATED_TOKEN_LENGTH bytes long.
 *
 * @param[in,out] token Buffer for generated token, must have a
 * minimum capacity of @ref CONFIG_UNICOAP_GENERATED_TOKEN_LENGTH
 */
void unicoap_generate_token(uint8_t* token);

/**
 * @brief Retrieves the part of a resource flags bitfield relevant for the messaging driver.
 *
 * @param resource_flags Resource flags
 * @return Messaging flags extracted from the given bitfield
 */
static inline unicoap_messaging_flags_t
_messaging_flags_resource(unicoap_resource_flags_t resource_flags)
{
    /* We documented other flags are RFU, hence downcasting to the messaging
     flags bitfield width is fine here */
    return (unicoap_messaging_flags_t)resource_flags;
}
/** @}  */

/* MARK: - Private Exchange-Layer Server API */
/**
 * @name Server
 * @{
 */
/**
 * @brief Informs the unicoap server a new packet has been preprocessed and can be handled.
 *
 * This function handles block-wise transfers.
 *
 * @param[in,out] packet Packet that will be processed by the server
 * @param[in,out] resource Resource
 *
 * @retval `0` on success
 * @retval Negative errno on failure
 */
int unicoap_server_process_request(unicoap_packet_t* packet, const unicoap_resource_t* resource);

/**
 * @brief Sends entire response body, may be split into parts and then sent
 *
 * @param[in,out] packet Response packet to send
 * @param[in] resource mandatory resource which is sending this response
 *
 * @returns Zero on success, negative integer otherwise
 */
int unicoap_server_send_response_body(unicoap_packet_t* packet,
                                      const unicoap_resource_t* resource);
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
