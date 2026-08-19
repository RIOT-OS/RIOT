/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdint.h>

#include "net/unicoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup net_unicoap_private
 * @{
 */

/**
 * @file
 * @brief Automatic block-wise transfer private API
 * @author Carl Seifert <carl.seifert@tu-dresden.de>
 */

/* MARK: - Block-wise currency helper types */
/**
 * @name Block-wise currency helper types
 * @{
 */
/**
 * @brief Number of bits required to represent @ref unicoap_blockwise_stage_t
 */
#define UNICOAP_BLOCKWISE_STATE_FIXED_WIDTH 1

/**
 * @brief Stage a given block-wise transfer is in
 */
typedef enum {
    /**
     * @brief A larger payload body being sliced and transmitted by unicoap
     *
     * This may be a request that's transmitted in a Block1 transfer or a response
     * sent in a Block2 transfer.
     */
    UNICOAP_BLOCKWISE_STAGE_SLICE = 0,

    /**
     * @brief A larger payload body being received and collected by unicoap.
     *
     * This may be a response that's transmitted in a Block2 transfer or a request sent in
     * a Block1 transfer.
     *
     * Collecting may also involve reassembling, i.e., copying chunks into a larger buffer, thus
     * forming a contiguous, reassembled payload body.
     */
    UNICOAP_BLOCKWISE_STAGE_COLLECT = 1
} __attribute__((__packed__)) unicoap_blockwise_stage_t;


/**
 * @brief Block-wise flags
 *
 * This type corresponds to a specific region in @ref unicoap_request_flags_t and @ref unicoap_resource_flags_t.
 */
typedef enum {
    /** @brief Message must not be copied, outlives exchange */
    UNICOAP_BLOCKWISE_FLAG_DURABLE_MESSAGE = 0x0010,

    /** @brief Slice body into blocks */
    UNICOAP_BLOCKWISE_FLAG_SLICE = 0x0020,

    /** @brief Reassemble blocks, recreating original body */
    UNICOAP_BLOCKWISE_FLAG_REASSEMBLE = 0x0040,
} unicoap_blockwise_flags_t;

/**
 * @brief Returns block-wise flags from request flags
 */
static inline unicoap_blockwise_flags_t _blockwise_flags_client(
    unicoap_request_flags_t request_flags) {
    /* We documented other flags are RFU, hence downcasting to the messaging
     flags bitfield width is fine here */
    return (unicoap_blockwise_flags_t)request_flags;
}

/**
 * @brief Returns block-wise flags from resource flags
 */
static inline unicoap_blockwise_flags_t _blockwise_flags_resource(
    unicoap_resource_flags_t resource_flags) {
    /* We documented other flags are RFU, hence downcasting to the messaging
     flags bitfield width is fine here */
    return (unicoap_blockwise_flags_t)resource_flags;
}
/** @} */


/* MARK: - Block-wise state */
/**
 * @name Block-wise state
 * @{
 */

/**
 * @brief Block-wise transfer state
 */
typedef struct {
    /** @brief Iterator for slicing/collecting/reassembling */
    unicoap_blockwise_iterator_t iterator;

    /** @brief Options used throughout Block1/Block2 transfer */
    unicoap_options_t options;

    /** @brief Temporary buffer for options */
    uint8_t options_storage[CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY];

    /** @brief Buffer of size @ref CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY for slicing and reassembling */
    uint8_t* buffer;

    /** @brief Request method or response status */
    uint8_t code;

    /**
     * @brief Indicates whether this transfer is being used.
     *
     * If `false`, this transfer structure may be used for another managed block-wise transfer.
     */
    bool is_active : 1;

    /**
     * @brief The Block-wise stage this transfer is in.
     */
    unicoap_blockwise_stage_t stage : UNICOAP_BLOCKWISE_STATE_FIXED_WIDTH;

    /** @brief Length of ETag */
    uint8_t etag_length : UNICOAP_ETAG_LENGTH_FIXED_WIDTH;

    /** @brief Content-Format of message received */
    unicoap_content_format_t content_format;

    /** @brief ETag of message received */
    uint8_t etag[UNICOAP_ETAG_LENGTH_MAX];
} unicoap_blockwise_transfer_t;

/**
 * @brief Validates the `ETag` and `Content-Format` if a body is being collected
 */
int unicoap_blockwise_transfer_collector_validate(unicoap_message_t* message,
                                                  unicoap_blockwise_transfer_t* transfer);

/**
 * @brief Sets up block-wise transfer
 */
int unicoap_blockwise_transfer_setup(unicoap_message_t* outbound_message,
                                     unicoap_blockwise_transfer_t* transfer,
                                     unicoap_blockwise_flags_t blockwise_flags,
                                     unicoap_proto_t proto, bool init);
/** @} */

#ifdef __cplusplus
}
#endif
