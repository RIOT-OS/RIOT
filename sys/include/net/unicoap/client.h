/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @brief   Client APIs
 * @ingroup net_unicoap_client
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup net_unicoap_client
 * @{
 */
/* MARK: - Customizing request behavior */
/**
 * @name Customizing request behavior
 * @{
 */
/**
 * @brief Flags for enabling advanced features in client exchanges
 *
 * Pass these flags to one the `unicoap_send_request` functions to modify transmission,
 * block-wise, or resource observation behavior.
 */
typedef enum {
    /**
     * @brief Sets the type of the message to confirmable (`CON`),
     * if the endpoint is an UDP or DTLS endpoint.
     *
     * This flag is ignored with reliable transports.. For unreliable transports, a message
     * sent with this flag will require an acknowledgement to be sent from the CoAP
     * peer.
     *
     */
    UNICOAP_CLIENT_FLAG_RELIABLE = 0x0001,
} unicoap_client_flags_t;

/**
 * @brief Prints client flags
 */
void unicoap_print_client_flags(unicoap_client_flags_t flags);
/** @} */
/** @} */

/**
 * @addtogroup net_unicoap_private
 * @{
 */
/* MARK: - Private request API */
/**
 * @name Private request API
 */
/**
 * @brief Sends off client message and creates a state object for pending request
 * @private
 *
 * @param request Request message
 * @param destination Request destination
 * @param callback Callback -- either response or block callback, see @ref unicoap_callback_t
 * @param callback_arg Opaque argument passed to callback
 * @param flags Client flags
 * @param profile Profile, e.g., OSCORE profile
 *
 * @returns Zero on success
 * @returns Negative integer on error
 */
int _unicoap_open_request(unicoap_message_t* request,
                          unicoap_destination_t* destination,
                          void* callback, void* callback_arg,
                          unicoap_client_flags_t flags);
/** @} */
/** @} */

/**
 * @addtogroup net_unicoap_client
 * @{
 */
/* MARK: - Sending a request */
/**
 * @name Sending a request
 * @{
 */
/**
 * @brief Response or error callback
 *
 * Called either synchronously or asynchronously. If the block-wise reassembly flag was present
 * when you invoked `unicoap_request_send_*`, the given message will contain the
 * reassembled payload.
 *
 * @code
 * static on_response(const unicoap_message_t* response, const unicoap_aux_t* aux, int error, void* arg) {
 *     if (error) {
 *         printf("Something went wrong, error %i (%s)\n", error, strerror(-error));
 *         return error;
 *     }
 *
 *     unicoap_status_t status = unicoap_response_get_status(response);
 *     printf("Received response, status %s", unicoap_string_from_status(status));
 *     return 0;
 * }
 * @endcode
 *
 * @remark Always check the @p error parameter first.
 *
 * @param[in] response Response message
 * @param[in] aux Auxiliary response information
 * @param error Error if negative, zero if successful
 * @param[in,out] arg Argument supplied to the initial `send_request` function
 */
typedef int (*unicoap_response_callback_t)(const unicoap_message_t* response,
                                           const unicoap_aux_t* aux, int error, void* arg);

/**
 * @brief Sends a request asynchronously
 *
 * The callback will be executed on the internal CoAP thread. You are in charge of
 * ensuring thread-safety. If your application cannot continue running without a result, consider
 * using @ref unicoap_send_request_sync or @ref unicoap_send_request_sync_copy instead to
 * avoid nesting your application code in callbacks.
 *
 * Consumes @p request .
 *
 * ### Block-wise transfers
 * To automatically slice request payloads too large to be transmitted in a single message,
 * pass the @ref UNICOAP_CLIENT_FLAG_SLICE. You can avoid the copying overhead by using
 * @ref UNICOAP_CLIENT_FLAG_SLICE_NO_COPY. If you want to receive a large response in a single
 * message rather than needing to retrieve each block individually, specify
 * @ref UNICOAP_CLIENT_FLAG_REASSEMBLE. See the [Block-wise transfers module](@ref net_unicoap_blockwise)
 * for details on compile-time configuration settings.
 *
 * ### Observation of resources
 * Some CoAP resources and servers are capable of sending additional responses, i.e., notifications,
 * if the resource has changed. For example a sensor resource might send a notification if the
 * measured quantity changes. To observe the resource, pass in the  @ref UNICOAP_CLIENT_FLAG_OBSERVE flag.
 * To cancel an observation, pass the @ref UNICOAP_CLIENT_FLAG_CANCEL_OBSERVATION flag.
 * For each notification, the given callback will be called.
 *
 * @note Observing resources consumes stack resources longer than might be necessary.
 *
 * ### Transmission behavior
 * If you want to send a request to an UDP or DTLS CoAP endpoint reliably, pass in
 * @ref UNICOAP_CLIENT_FLAG_RELIABLE. This will result in a confirmable request message being
 * sent rather than a non-confirmable message.
 *
 * @param[in,out] request Initialized request message to send
 * @param destination URI or endpoint. Use @ref unicoap_destination_uri_string or @ref unicoap_destination_endpoint
 * @param callback Function executed when the entire response is available or if an error occurred
 * @param callback_arg Optional argument passed to the callback (nullable)
 * @param flags Client flags
 * @param profile Profile
 *
 * @returns Zero on success
 * @returns Negative integer on failure
 */
static inline int unicoap_send_request_async(unicoap_message_t* request,
                                             unicoap_destination_t* destination,
                                             unicoap_response_callback_t callback, void* callback_arg,
                                             unicoap_client_flags_t flags)
{
    return _unicoap_open_request(request, destination, callback, callback_arg, flags);
}

/**
 * @brief Sends a request synchronously
 *
 * This client method will not return until either an error is thrown or the response is available.
 * If blocking the current thread is unacceptable in your scenario, consider switching to
 * @ref unicoap_send_request_async.
 *
 * Consumes @p request .
 *
 * ### Block-wise transfers
 * To automatically slice request payloads too large to be transmitted in a single message,
 * pass the @ref UNICOAP_CLIENT_FLAG_SLICE. This method will block throughout the block-wise
 * transfer, thereby preventing copying overhead when the slice flag is present.
 * If you want to receive a large response in a single message rather than needing to retrieve each block
 * individually, specify
 * @ref UNICOAP_CLIENT_FLAG_REASSEMBLE. See the [Block-wise transfers module](@ref net_unicoap_blockwise)
 * for details on compile-time configuration settings.
 *
 * ### Observation of Resources
 * @note This client method does not support observation of resources due to the asynchronous
 * nature of notifications. To make use of observation, please refer to @ref unicoap_send_request_async.
 * If you do pass in resource observation flags, an assertion failure will be triggered when assertions
 * are enabled.
 *
 * ### Transmission behavior
 * If you want to send a request to an UDP or DTLS CoAP endpoint reliably, pass in
 * @ref UNICOAP_CLIENT_FLAG_RELIABLE. This will result in a confirmable request message being
 * sent rather than a non-confirmable message.
 *
 * @param[in,out] request Initialized request message to send
 * @param destination URI or endpoint. Use @ref unicoap_destination_uri_string or @ref unicoap_destination_endpoint
 * @param callback Function executed when the entire response is available or if an error occurred
 * @param callback_arg Optional argument passed to the callback (nullable)
 * @param flags Client flags
 * @param profile Profile
 *
 * @returns Zero on success
 * @returns Negative integer on failure
 */
int unicoap_send_request_sync(unicoap_message_t* request,
                              unicoap_destination_t* destination,
                              unicoap_response_callback_t callback, void* callback_arg,
                              unicoap_client_flags_t flags);

/**
 * @brief Sends a request synchronously and copies the request to the given buffer
 *
 * This client method will not return until either an error is thrown or the response is available.
 * If blocking the current thread is unacceptable in your scenario, consider switching to
 * @ref unicoap_send_request_async. When the entire response is available, it will be copied
 * into the buffer. If the buffer capacity provided by you is too small for the response, this method will
 * fail with `-ENOBUFS`. Once method returns zero, you can start reading from @p response and @p aux.
 *
 * Consumes @p request .
 *
 * @remark To avoid the overhead of copying the response received by stack, use
 * @ref unicoap_send_request_sync or @ref unicoap_send_request_async.
 *
 * ### Block-wise transfers
 * To automatically slice request payloads too large to be transmitted in a single message,
 * pass the @ref UNICOAP_CLIENT_FLAG_SLICE. This method will block throughout the block-wise
 * transfer, thereby preventing copying overhead when the slice flag is present.
 * If you want to receive a large response in a single message rather than needing to retrieve each block
 * individually, specify
 * @ref UNICOAP_CLIENT_FLAG_REASSEMBLE. See the [Block-wise transfers module](@ref net_unicoap_blockwise)
 * for details on compile-time configuration settings.
 *
 * @note If you use this API with automatic block-wise transfers, make sure to provide an
 * appropriate-sized buffer as the entire reassembled payload will need to be copied.
 *
 * ### Observation of Resources
 * @note This client method does not support observation of resources due to the asynchronous
 * nature of notifications. To make use of observation, please refer to @ref unicoap_send_request_async.
 * If you do pass in resource observation flags, an assertion failure will be triggered when assertions
 * are enabled.
 *
 * ### Transmission behavior
 * If you want to send a request to an UDP or DTLS CoAP endpoint reliably, pass in
 * @ref UNICOAP_CLIENT_FLAG_RELIABLE. This will result in a confirmable request message being
 * sent rather than a non-confirmable message.
 *
 * @param[in,out] request Initialized request message to send
 * @param destination URI or endpoint. Use @ref unicoap_destination_uri_string or @ref unicoap_destination_endpoint
 * @param[in,out] response Pre-allocated response message, will be initialized when a response is received successfully
 * @param[in,out] buffer Provide a buffer to copy
 * @param[in,out] buffer_capacity Number of free bytes available in the given @p buffer
 * @param flags Client flags
 * @param profile Profile
 * @param[in,out] aux Pre-allocated auxiliary information structure, will be initialized when a response is received successfully
 *
 * @returns Zero on success
 * @returns Negative integer on failure
 */
int unicoap_send_request_sync_copy(unicoap_message_t* request,
                                   unicoap_destination_t* destination,
                                   unicoap_message_t* response, uint8_t* buffer,
                                   size_t buffer_capacity, unicoap_client_flags_t flags,
                                   unicoap_aux_t* aux);

/**
 * @brief Shorthand for @ref unicoap_send_request_sync_copy
 *
 * If you want to try out something quickly with the CoAP stack, use this method.
 * In other cases, the explicit sync or async methods are a better fit.
 *
 * @warning Do not use this method in production environments in favor of one these methods:
 * - @ref unicoap_send_request_async
 * - @ref unicoap_send_request_sync
 * - @ref unicoap_send_request_sync_copy
 * - @ref unicoap_send_request_blockwise_async
 * - @ref unicoap_send_request_blockwise_sync
 */
static inline int unicoap_send_request(unicoap_message_t* request,
                                       unicoap_destination_t* destination,
                                       unicoap_message_t* response, uint8_t* buffer,
                                       size_t buffer_capacity, unicoap_client_flags_t flags)
{
    return unicoap_send_request_sync_copy(request, destination, response, buffer,
                                          buffer_capacity, flags, NULL);
}
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif
