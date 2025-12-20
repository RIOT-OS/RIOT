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

#include "mutex.h"
#include "bitfield.h"
#include "architecture.h"
#include "random.h"

#include "net/unicoap.h"

#include "private/state.h"
#include "private/packet.h"
#include "private/messaging.h"

/**
 * @defgroup net_unicoap_private Private API
 * @ingroup  net_unicoap_internal
 * @brief    Implementation details of `unicoap`
 * @{
 *
 * @warning Do not call any of these APIs and do not interact with any of unicoap's private types,
 * unless you know what you are doing. You risk corrupting `unicoap`'s internal state.
 */

/**
 * @file
 * @brief  Private API
 * @author Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - Debugging */
/**
 * @name Debugging
 * @{
 */
/** @brief Debug log prefix */
#define UNICOAP_DEBUG_PREFIX "coap"

#ifndef DOXYGEN
#  define _UNICOAP_NEED_HAVE "(need %" PRIuSIZE ", have %" PRIuSIZE ")"
#endif

#ifndef DOXYGEN
/**
 * @brief Debug with category prefix
 *
 * Prints `unicoap.category: __VA_ARGS__`
 *
 * @param category Category string
 */
#  define _UNICOAP_PREFIX_DEBUG(category, ...) DEBUG(UNICOAP_DEBUG_PREFIX category ": " __VA_ARGS__)
#  define UNICOAP_DEBUG(...)                   _UNICOAP_PREFIX_DEBUG("", __VA_ARGS__)
#  define OPTIONS_DEBUG(...)                   _UNICOAP_PREFIX_DEBUG(".options", __VA_ARGS__)
#  define MESSAGING_DEBUG(...)                 _UNICOAP_PREFIX_DEBUG(".messaging", __VA_ARGS__)
#  define STATE_DEBUG(...)                     _UNICOAP_PREFIX_DEBUG(".state", __VA_ARGS__)
#  define TRANSPORT_DEBUG(...)                 _UNICOAP_PREFIX_DEBUG(".transport", __VA_ARGS__)
#  define SERVER_DEBUG(...)                    _UNICOAP_PREFIX_DEBUG(".server", __VA_ARGS__)
#  define API_WARNING(message)                 "WARNING: " message "\n"
#  define API_ERROR(message)                   "ERROR: " message "\n"
#  define API_MISUSE(message)                  "API MISUSE: " message "\n"
#  define INTERNAL_ERROR(message)              "BUG: " message "\n"
#  define FIXIT(message)                       "+++ FIXIT: " message "\n"
#endif

#if ENABLE_DEBUG
/**
 * @brief Debug print endpoint
 */
#  define DEBUG_ENDPOINT(endpoint) unicoap_print_endpoint(endpoint)
/**
 * @brief Debug print flags
 */
#  define DEBUG_FLAGS(flags, role) unicoap_print_flags(flags, role)
#else
/**
 * @brief Debug print endpoint
 */
#  define DEBUG_ENDPOINT(endpoint) (void)endpoint
/**
 * @brief Debug print flags
 */
#  define DEBUG_FLAGS(flags, role) \
      (void)flags;                 \
      (void)role
#endif

#ifndef DOXYGEN
#  if IS_ACTIVE(CONFIG_UNICOAP_ASSIST)
#    include <stdio.h>
#  endif
#  define unicoap_assist(...)                     \
      do {                                        \
          if (IS_ACTIVE(CONFIG_UNICOAP_ASSIST)) { \
              printf(__VA_ARGS__);                \
          }                                       \
      } while (0)
#endif

/**
 * @brief Emit a diagnostic stating that a driver is missing
 */
void unicoap_assist_emit_diagnostic_missing_driver(unicoap_proto_t proto);
/** @} */

/* MARK: - Thread */
/**
 * @name Thread
 * @{
 */
/** @brief Name of background thread spawned upon calls to @ref unicoap_init */
#define UNICOAP_THREAD_IDENTIFIER "unicoap"

/** @brief Locks internal state lock */
void unicoap_state_lock(void);

/** @brief Unlocks internal state lock */
void unicoap_state_unlock(void);
/** @} */

/* MARK: - State and initialization */
/**
 * @name State and initialization
 * @{
 */
/**
 * @brief Container the unicoap stacks keeps state in
 */
typedef struct {
    /**
     * @brief Used when allocating listener, transaction, carbon copy, observer, or registration
     */
    mutex_t lock;

    /** @brief Groups of resources */
    unicoap_listener_t* listeners;

    /* TODO: Client and advanced server features: Exchange-layer state objects */
} unicoap_state_t;

/** @brief Initializes the CoAP over UDP driver on the given @p queue */
int unicoap_init_udp(event_queue_t* queue);

/** @brief Deinitializes the CoAP over UDP driver on the given @p queue */
int unicoap_deinit_udp(event_queue_t* queue);

/** @brief Initializes the CoAP over DTLS over UDP driver on the given @p queue */
int unicoap_init_dtls(event_queue_t* queue);

/** @brief Deinitializes the CoAP over DTLS over UDP driver on the given @p queue */
int unicoap_deinit_dtls(event_queue_t* queue);

/** @brief Initializes the common RFC 7252 driver on the given @p queue */
int unicoap_init_rfc7252_common(event_queue_t* queue);

/** @brief DeiInitializes the common RFC 7252 driver on the given @p queue */
int unicoap_deinit_rfc7252_common(event_queue_t* queue);

/* MARK: unicoap_driver_extension_point */
/** @} */

/* MARK: - Private Server Utils */
/**
 * @name Resource-request matching
 * @{
 */
/**
 * @brief Tries to find a resource for the given packet
 *
 * The resource and listener variables passed by reference will be set to the respective resource
 * and encompassing listener, if found.
 *
 * This method calls each listeners @ref unicoap_listener_t.request_matcher to check whether
 * the given resource can be matched to the given packet.
 *
 * @note The default request matcher is @ref unicoap_resource_match_request_default
 *
 * @param[in] packet Packet to find resource for
 * @param[in] resource_ptr Pointer to a resource variable
 * @param[in] listener_ptr Pointer to a listener variable
 *
 * @retval `0` if found
 * @retval An @ref unicoap_status_t if not found, method mismatch, or server error
 */
int unicoap_resource_find(const unicoap_packet_t* packet, const unicoap_resource_t** resource_ptr,
                          const unicoap_listener_t** listener_ptr);

/**
 * @brief Default request-resource matcher for listeners
 *
 * The resource variable passed by reference will be set to the respective resource, if found.
 *
 * @param[in] listener Listener
 * @param[out] resource_ptr Pointer to a resource variable
 * @param[in] request Request message
 * @param[in] endpoint Remote endpoint the request originates from
 *
 * @retval `0` if found
 * @retval An @ref unicoap_status_t if not found, method mismatch, or server error
 */
int unicoap_resource_match_request_default(const unicoap_listener_t* listener,
                                           const unicoap_resource_t** resource_ptr,
                                           const unicoap_message_t* request,
                                           const unicoap_endpoint_t* endpoint);

/**
 * @brief Default resource handler for `/.well-known/core`
 *
 * @see @ref unicoap_request_handler_t
 */
int unicoap_resource_handle_well_known_core(unicoap_message_t* message, const unicoap_aux_t* aux,
                                            unicoap_request_context_t* ctx, void* arg);
/** @} */

/* MARK: - Other Utils */
/**
 * @name Other Utils
 * @{
 */

/**
 * @brief Declares internal receiver storage buffer
 *
 * This buffer is used by sock network backends not emitting contiguous data
 */
#define UNICOAP_DECL_RECEIVER_STORAGE \
    uint8_t unicoap_receiver_buffer[CONFIG_UNICOAP_PDU_SIZE_MAX]

/**
 * @brief `extern` declaration of @ref UNICOAP_DECL_RECEIVER_STORAGE
 */
#define UNICOAP_DECL_RECEIVER_STORAGE_EXTERN extern UNICOAP_DECL_RECEIVER_STORAGE

/**
 * @brief Determines whether the chunk of the given size must be truncated to fit into a buffer of
 * size @ref CONFIG_UNICOAP_PDU_SIZE_MAX
 *
 * Use this method when receiving fragmented/scattered data, e.g., when reading from a socket.
 *
 * This method returns `false` if a chunk of the given @p chunk_size can still be copied into a
 * buffer of size @ref CONFIG_UNICOAP_PDU_SIZE_MAX when @p received bytes are already stored in
 * that buffer.
 *
 * If said buffer cannot store a chunk of the given size while also holding @p received bytes
 * already saved, the chunk needs to be truncated. In this case, `true` is returned and the chunk
 * size is truncated to the largest chunk size that is still acceptable. Stop your receiver loop as
 * further calls to this function will, consequently, also return `true` and yield a chunk_size of
 * zero (buffer is full).
 *
 * @param[in,out] chunk_size Number of bytes in the currently received chunk. Written to if chunk
 *                           does not fit into buffer.
 *
 * @param received Number of bytes already stored in the buffer (received byte count)
 *
 * @returns Boolean value indicating whether the chunk, and therefore also its size, needed to be
 * truncated.
 */
static inline bool unicoap_transport_truncate_received(size_t* chunk_size, size_t received)
{
    /* cannot use sizeof() here, because we want this function to be inlinable,
     * extern decl would affect transport.c, too */
    if (received + *chunk_size > CONFIG_UNICOAP_PDU_SIZE_MAX) {
        /* Limit chunk size to remaining available storage capacity */
        TRANSPORT_DEBUG("truncated\n");
        TRANSPORT_DEBUG("warning: recv storage too small, need at least %" PRIdSIZE "\n",
                        received + *chunk_size);
        *chunk_size = CONFIG_UNICOAP_PDU_SIZE_MAX - received;
        return true;
    }
    return false;
}
/** @} */

#ifndef DOXYGEN
static inline void __debug_hex(const uint8_t* buffer, size_t size)
{
    for (size_t i = 0; i < size; i += 1) {
        printf("%02X", buffer[i]);
    }
}

#  define _UNICOAP_DEBUG_HEX(bytes, size) \
      do {                                \
          if (ENABLE_DEBUG) {             \
              __debug_hex(bytes, size);   \
          }                               \
      } while (0)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
