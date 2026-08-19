/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdint.h>
#include <assert.h>

#include "architecture.h"

#include "net/unicoap/options.h"

/**
 * @defgroup net_unicoap_blockwise_kit Block-wise transfer implementation kit kit
 * @brief Functions and accessors for handling Block1 and Block2 CoAP options
 * @ingroup net_unicoap_blockwise
 * @{
 */

/**
 * @file
 * @brief Block-wise helpers
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - The Block1 and Block2 options */
/**
 * @name The Block1 and Block2 options
 * @{
 */
/**
 * @brief Info extracted from `Block1` and `Block2` option value
 *
 * @note Note that the last block (the last payload chunk) in a block-wise transfer may be smaller than the block @p size .
 */
typedef struct {
    /** @brief Block size */
    size_t size;

    /** @brief Block offset from start of full body*/
    size_t offset;

    /** @brief Block number */
    uint32_t number;

    /** @brief A boolean value indicating whether more blocks are available. `false` if this is the last block */
    bool more_available;

    /** @brief A boolean value determining if a single payload is allowed to encompass multiple blocks. (BERT) */
    bool bert;
} unicoap_block_info_t;

/**
 * @brief Block1 and Block2 `szx` values
 */
typedef enum {
    /** @brief Block size: 16 bytes */
    UNICOAP_BLOCK_SZX_16 = 0,

    /** @brief Block size: 32 bytes */
    UNICOAP_BLOCK_SZX_32 = 1,

    /** @brief Block size: 64 bytes */
    UNICOAP_BLOCK_SZX_64 = 2,

    /** @brief Block size: 128 bytes */
    UNICOAP_BLOCK_SZX_128 = 3,

    /** @brief Block size: 256 bytes */
    UNICOAP_BLOCK_SZX_256 = 4,

    /** @brief Block size: 512 bytes */
    UNICOAP_BLOCK_SZX_512 = 5,

    /** @brief Block size: 1024 bytes */
    UNICOAP_BLOCK_SZX_1024 = 6,

    /** @brief Block size: 1024 bytes + BERT (allows multiple blocks per message) */
    UNICOAP_BLOCK_SZX_1024_BERT = 7,
} __attribute__((__packed__)) unicoap_block_szx_t;

#ifndef DOXYGEN
#  define UNICOAP_BERT_BLOCK_SIZE        (1024)
#  define UNICOAP_BERT_BLOCK_SZX         (6)
#  define _UNICOAP_BLOCK_SZX_MASK        (0x7)
#  define _UNICOAP_BLOCK_NUMBER_MAX     (0xfffff)
#  define _UNICOAP_BLOCK_NUMBER_OFFSET   (4)
#  define _UNICOAP_BLOCK_NUMBER_MASK     (0xfffff << _UNICOAP_BLOCK_NUMBER_OFFSET)
#  define _UNICOAP_BLOCK_MORE_FLAG       (0x8)
#  define _UNICOAP_BLOCK_FIXED_BIT_WIDTH (24)
#  define _UNICOAP_BLOCK_NONE_FLAG       (UNICOAP_BLOCK_OPTION_NONE)
#endif

/**
 * @brief Dumps block option
 *
 * @param dump Dump function, e.g. @ref DEBUG
 * @param block Block option value
 */
#define UNICOAP_DUMP_BLOCK(dump, block)                                           \
    dump("<#%" PRIu32 ", szx=%u, size=%" PRIuSIZE ", BERT=%c, M=%c>",             \
         unicoap_block_get_number(block), unicoap_block_get_szx(block),           \
         unicoap_block_get_size(block),                                           \
         unicoap_block_get_szx(block) == UNICOAP_BLOCK_SZX_1024_BERT ? '1' : '0', \
         unicoap_block_get_more(block) ? '1' : '0')
/** @} */

/* MARK: - Block Option Accessors */
/**
 * @name Block Option Accessors
 * @{
 */
/**
 * @brief Returns `M` flag
 * Safe to call even if option is @ref UNICOAP_BLOCK_OPTION_NONE
 * @param option Block1/Block2 option value
 * @returns `M` flag
 */
static inline bool unicoap_block_get_more(unicoap_block_option_t option) {
    return option & _UNICOAP_BLOCK_MORE_FLAG;
}

/**
 * @brief Sets `M` flag
 * Safe to call even if option is @ref UNICOAP_BLOCK_OPTION_NONE
 * @param option Block1/Block2 option value
 * @param more `M ` flag
 */
static inline void unicoap_block_set_more(unicoap_block_option_t* option, bool more) {
    *option = more ? (*option | _UNICOAP_BLOCK_MORE_FLAG) : (*option & ~_UNICOAP_BLOCK_MORE_FLAG);
}

/**
 * @brief Gets block number
 * @param option Block option
 * @returns Block number
 */
static inline uint32_t unicoap_block_get_number(unicoap_block_option_t option) {
    return (option & _UNICOAP_BLOCK_NUMBER_MASK) >> _UNICOAP_BLOCK_NUMBER_OFFSET;
}

/**
 * @brief Sets block number
 * @param option Block option
 * @param number Block number
 */
static inline void unicoap_block_set_number(unicoap_block_option_t* option, uint32_t number) {
    assert(number <= _UNICOAP_BLOCK_NUMBER_MAX);
    *option = (*option & ~(_UNICOAP_BLOCK_NUMBER_MASK | _UNICOAP_BLOCK_NONE_FLAG)) |
              (number << _UNICOAP_BLOCK_NUMBER_OFFSET);
}

/**
 * @brief Gets `szx` from block option
 * @param option Block option
 * @returns `szx` value
 */
static inline unicoap_block_szx_t unicoap_block_get_szx(unicoap_block_option_t option) {
    return option & _UNICOAP_BLOCK_SZX_MASK;
}

/**
 * @brief Sets `szx` of a block option
 * @param option Block option
 * @param szx `szx` value to set
 */
static inline void unicoap_block_set_szx(unicoap_block_option_t* option, unicoap_block_szx_t szx) {
    *option = (*option & ~(_UNICOAP_BLOCK_SZX_MASK | _UNICOAP_BLOCK_NONE_FLAG)) | szx;
}

/**
 * @brief Converts `szx` value to size in bytes
 * @param szx `szx` value
 * @returns Block size in bytes
 */
static inline size_t unicoap_block_size_from_szx(unicoap_block_szx_t szx) {
    return szx == UNICOAP_BLOCK_SZX_1024_BERT ? UNICOAP_BERT_BLOCK_SIZE : (1 << 4) << szx;
}

/**
 * @brief Converts block size in bytes to `szx` value
 * @param size Block size in bytes
 * @returns `szx` value
 */
unicoap_block_szx_t unicoap_block_szx_from_size(size_t size);

/**
 * @brief Gets block size in bytes from block option
 * @param option Block option
 * @returns Block size in bytes
 */
static inline size_t unicoap_block_get_size(unicoap_block_option_t option) {
    return unicoap_block_size_from_szx(unicoap_block_get_szx(option));
}

/**
 * @brief Sets block size in bytes
 * @param option Block option
 * @param size Block size in bytes to set
 */
static inline void unicoap_block_set_size(unicoap_block_option_t* option, size_t size) {
    unicoap_block_set_szx(option, unicoap_block_szx_from_size(size));
}

/**
 * @brief Gets the block size as a power of two
 * @param szx Block `szx` value
 * @returns Size of a block as a power of two
 *
 * `Block size in bytes = 2^exponents bytes`
 */
static inline uint32_t _unicoap_block_get_size_exponent(unicoap_block_szx_t szx) {
    return (szx == UNICOAP_BLOCK_SZX_1024_BERT) ? (UNICOAP_BERT_BLOCK_SZX + 4) : szx + 4;
}

/**
 * @brief Gets the block's offset in bytes in the original body
 * @param option
 * @returns Offset of block payload in the original payload body in bytes
 */
static inline size_t unicoap_block_get_offset(unicoap_block_option_t option) {
    return unicoap_block_get_number(option)
           << _unicoap_block_get_size_exponent(unicoap_block_get_szx(option));
}

/**
 * @brief Sets the block's number using the block's payload offset in the original payload body
 * @param option Block option whose number to set
 * @param offset The offset in bytes of the block in the larger payload body
 */
static inline void unicoap_block_set_number_from_offset(unicoap_block_option_t* option,
                                                        size_t offset) {
    /* offset = number * 2^(4 + SZX)
     * number = offset / 2^(4 + SZX)
     * number = offset * 2^-(4 +SZX)
     * number = offset >> (4 + SZX) */
    unicoap_block_set_number(option, (uint32_t)offset >> _unicoap_block_get_size_exponent(
                                                             unicoap_block_get_szx(*option)));
}

/**
 * @brief Generate debugging info structure from block option
 * @param option Block option
 * @param[out] info Pre-allocated info structure
 */
void unicoap_block_get_info(unicoap_block_option_t option, unicoap_block_info_t* info);

/**
 * @brief Builds block option from `szx`, block number, and `M` flag
 * @param szx `szx` value
 * @param number Block number
 * @param more `M` flag
 * @returns Block option value
 */
static inline unicoap_block_option_t unicoap_block_from_szx(unicoap_block_szx_t szx,
                                                            uint32_t number, bool more) {
    assert(szx <= 7);
    return number << _UNICOAP_BLOCK_NUMBER_OFFSET | (more ? _UNICOAP_BLOCK_MORE_FLAG : 0) | szx;
}

/**
 * @brief Builds block option from block size, block number, and `M` flag
 * @param size Block size in bytes
 * @param number Block number
 * @param more `M` flag
 * @returns Block option value
 */
static inline unicoap_block_option_t unicoap_block_from_size(size_t size, uint32_t number,
                                                             bool more) {
    return unicoap_block_from_szx(unicoap_block_szx_from_size(size), number, more);
}

/**
 * @brief Builds block option from debugging info structure
 * @param[in] info Pre-populated info structure
 * @returns Block option
 */
static inline unicoap_block_option_t unicoap_block_from_info(const unicoap_block_info_t* info) {
    assert(!info->bert || info->size == UNICOAP_BERT_BLOCK_SIZE);
    return unicoap_block_from_szx(info->bert ? UNICOAP_BLOCK_SZX_1024_BERT :
                                               unicoap_block_szx_from_size(info->size),
                                  info->number, info->more_available);
}
/** @} */

/* MARK: - Tracking a Block-wise Transfer */
/**
 * @name Tracking a Block-wise Transfer
 * @{
 */
/**
 * Helper struct for slicing data into blocks, collecting and keeping track of blocks received, and optionally reassembling blocks.
 * @see @ref unicoap_blockwise_iterator_init
 */
typedef struct {
    /**
     * @brief Buffer for entire payload
     * @private
     */
    uint8_t* body;

    /** 
     * @brief @p body size or capacity
     * @private
     */
    size_t body_size;

    /** 
     * @brief Current offset in @p body 
     * @private
     */
    size_t offset;

    /** 
     * @brief Block option
     * @private
     */
    unicoap_block_option_t _block_option;
} unicoap_blockwise_iterator_t;

/**
 * @brief Initializes the iterator with the given buffer.
 * @note Multiple blocks form a "body", which is RFC terminology for the payload before it's been sliced or after it has been reassembled.
 *
 * ### Usage
 *
 * 1. **Slicing**:
 * *Slice the entire payload into block-wise chunks.*
 *
 *  1. First, pass the entire payload as the iterator's @p body and
 * the entire payload size as @p body_size .
 *
 *  2. Then refer to the [Block1 slice](@ref unicoap_blockwise_slice_block1) *or*
 * [Block2 slice](@ref unicoap_blockwise_slice_block2) functions.
 *
 * 2. **Collecting**:
 * *Keep track of block-wise chunks without reassembling them.*
 *
 *  1. First, pass `NULL` for @p body and `0` for `body_size`.
 *
 *  2. Then refer to the [Block1 collect](@ref unicoap_blockwise_collect_block1) *or*
 * [Block2 collect](@ref unicoap_blockwise_collect_block2) functions.
 *
 * 3. **Reassembling**:
 * *Keep track of block-wise chunks **and** rebuild the entire payload from block-wise chunks.*
 *
 *  1. First, pass the buffer in @p body where the full body is going to be
 * written into, and the buffer **capacity** in @p body_size .
 *
 *  2. Then refer to the [Block1 collect](@ref unicoap_blockwise_collect_block1) *or*
 * [Block2 collect](@ref unicoap_blockwise_collect_block2) functions.
 *
 *  3. When the last block has benn processed successfully, the iterator's `body_size` will be
 * set to the **true size** of the body. If the response consists of just a single block
 * (with or without a Block2 option), the implementation does not copy the @p chunk into the
 * `body` buffer of the collector to avoid unnecessary copying.
 *
 * @param iterator Slicer, collector or reassembler
 * @param max_szx Size of an individual block. Real block size may be lower than this value due to negotiation with client/server.
 * @param body The buffer for the body whose blocks you iterate over. **See usage.**
 * @param body_size **See usage.**
 *
 * @see [RFC 7959 (Block-wise Transfers)](https://datatracker.ietf.org/doc/html/rfc7959)
 */
void unicoap_blockwise_iterator_init(unicoap_blockwise_iterator_t* iterator,
                                     unicoap_block_szx_t max_szx, uint8_t* body,
                                     size_t body_size);

/**
 * @brief Prints Block-wise iterator info
 */
void unicoap_print_blockwise_iterator(const unicoap_blockwise_iterator_t* iterator);

static inline unicoap_block_option_t unicoap_blockwise_iterator_current_option(
    const unicoap_blockwise_iterator_t* iterator
) {
    return iterator->_block_option & 0x1ffffff; 
    /* 1 more bit for UNICOAP_BLOCK_OPTION_NONE sentinel value. */
}

/**
 * @brief Collects a Block2 chunk received from the server.
 *
 * Use this API at the client to keep track of blocks received and, optionally, to reassemble them.
 *
 * @warning Do not call this API more than once for a given response.
 *
 * @param[in,out] collector The block-wise iterator that's used as a block-wise collector.
 * @param[in] block The block option value extracted from a Block2 response. May be `NULL`
 * the first you call this API.
 * @param[in] chunk The block chunk received from the server. This is the payload attached to
 * the response carrying the Block2 option. May be `NULL` the first time you call this API.
 * @param chunk_size Size of @p chunk. May be zero the first time you call this API.
 *
 * For the first request you send to the server, you may or may not include a Block2 option which
 * would be used as a block size suggestion.
 * If you do want to include a block size suggestion with the block size you passed to the initializer
 * and provide a Block2 option, call this API before the first request, and then set the
 * request's Block2 option to `collector.block` as follows.
 *
 * ```c
 * // First request
 * res = unicoap_blockwise_collect_block2(&collector, NULL, NULL, 0);
 * // handle error
 * res = unicoap_options_set_block2(request->options, &collector.block);
 * // handle error
 * ```
 *
 * For every response you receive (in response to your request), first, retrieve the Block2 option and
 * then call this API as follows. If the option is not present, supply `NULL` in place of the @p block
 * parameter. This may be acceptable if this is the first response. After this function returns, check if
 * the Block2 option's `M` flag is set (`block.more_available`) and, if true, send another request with
 * its Block2 options set to `collector.block`.
 *
 * @note For response bodys that are not transmitted block-wise, the Block2 option might
 * also be absent. Callers are encouraged to use this  API even if the Block2 option is not present in a
 * response, as this API is prepared to handle these cases and implements appropriate debug logging.
 *
 * ```c
 * unicoap_block_t block_option = {0};
 * res = unicoap_options_get_block2(response->options, &block_option);
 * // handle error
 * res = unicoap_blockwise_collect_block2(&collector,
 *     (res < 0) ? NULL : &block_option, response->payload, response->payload_size);
 * // handle error
 * if (block_option.more_available) {
 *     // send the same request again with a Block2 option
 *     res = unicoap_options_set_block2(request->options, &collector.block);
 *     // send ...
 * }
 * ```
 *
 * @return `0` on success
 * @return Negative errno on failure
 *
 * @see [Retrieving the Block2 option](@ref unicoap_options_get_block2)
 * @see [Setting the Block2 option](@ref unicoap_options_set_block2)
 * @see [Initializing the iterator as a collector or reassembler](@ref unicoap_blockwise_iterator_init)
 */
int unicoap_blockwise_collect_block2(unicoap_blockwise_iterator_t* collector,
                                     unicoap_block_option_t block, uint8_t* chunk,
                                     size_t chunk_size);

/**
 * @brief Slices off a Block2 chunk to be sent to the client.
 *
 * Use this API at the server to divide a large resource body into smaller chunks.
 *
 * @param[in,out] slicer The block-wise iterator that's used as a block-wise slicer.
 * @param[in] block The block option value extracted from a Block2 request. May be `NULL`
 * the first you call this API.
 * @param[out] chunk The block chunk to send next. May be `0` the first time you call this API.
 *
 * For every request you receive, first, retrieve the Block2 option and then call this API as follows.
 * If the option is not present, supply `NULL` in place of the @p block parameter. This may be
 * acceptable if this is the first request. After this function returns, check the slicer has more block
 * chunks available by reading the `slicer.block.more_available` property. If true, send
 * a response with its Block2 option set to `slicer.block` and its payload set to the @p chunk
 * out parameter.
 *
 * @note The first request you receive from the client may not have a Block2 option attached in case
 * the client does not want to suggest a block size. Callers are encouraged to use this API even if
 * the Block2 option is not present in a request, as this API is prepared to handle these cases and
 * implements appropriate debug logging.
 *
 * ```c
 * unicoap_block_t block_option = {0};
 * res = unicoap_options_get_block2(request->options, &block_option);
 * // handle error
 * uint8_t* chunk = NULL;
 * res = unicoap_blockwise_slice_block2(&slicer,
 *     (res < 0) ? NULL : &block_option, &chunk);
 * // handle error
 * if (slicer.block.more_available) {
 *     // send block-wise response
 *     response->payload = chunk;
 *     response->payload_size = res;
 *     res = unicoap_options_set_block2(response->options, &slicer.block);
 *     // send ...
 * }
 * ```
 *
 * @returns @p chunk  size on success
 * @returns Negative errno on failure
 *
 * @see [Retrieving the Block2 option](@ref unicoap_options_get_block2)
 * @see [Setting the Block2 option](@ref unicoap_options_set_block2)
 * @see [Initializing the iterator as a slicer](@ref unicoap_blockwise_iterator_init)
 */
ssize_t unicoap_blockwise_slice_block2(unicoap_blockwise_iterator_t* slicer,
                                       unicoap_block_option_t block, uint8_t** chunk);

/**
 * @brief Processes a Block2 request
 *
 * Next, call @ref unicoap_blockwise_slice_block2_produce.
 *
 * @see @ref unicoap_blockwise_slice_block2
 */
int unicoap_blockwise_slice_block2_process(unicoap_blockwise_iterator_t* slicer,
                                           unicoap_block_option_t block);

/**
 * @brief Slices off a payload block to be sent to the client
 *
 * Call this method after you have called @ref unicoap_blockwise_slice_block2_process.
 *
 * @see @ref unicoap_blockwise_slice_block2
 */
ssize_t unicoap_blockwise_slice_block2_produce(unicoap_blockwise_iterator_t* slicer,
                                               uint8_t** chunk);
/**
 * @brief Slices off a Block1 chunk to be sent to the server.
 *
 * Use this API at the client to divide a large request body into smaller chunks.
 *
 * @param[in,out] slicer The block-wise iterator that's used as a block-wise slicer.
 * @param[in] block The block option value extracted from a Block1 response. Must be `NULL`
 * the first you call this API.
 * @param[out] chunk The block chunk to send next. Must never be `NULL`.
 *
 * Do not include a @p block option the first time you call this API.
 *
 * ```c
 * // First request
 * res = unicoap_blockwise_slice_block1(&slicer, NULL, &request->payload);
 * // handle error
 * request->payload_size = res;
 * res = unicoap_options_set_block1(request->options, &slicer.block);
 * // handle error
 * ```
 *
 * For every response you receive, first, retrieve the Block1 option and
 * then call this API as follows. If the option is not present, supply `NULL` in place of the @p block
 * parameter.  After this function returns, check the slicer has more block chunks available by reading the
 * `slicer.block.more_available` property. If true, resend your request with its Block1 option set to
 * `slicer.block` and its payload set to the @p chunk out parameter.
 *
 * @note Callers are encouraged to use this API even if the Block1 option is not present in a
 * response, as this API is prepared to handle these cases and implements appropriate debug logging.
 *
 * ```c
 * unicoap_block_t block_option = {0};
 * res = unicoap_options_get_block1(response->options, &block_option);
 * // handle error
 * uint8_t* chunk = NULL;
 * res = unicoap_blockwise_slice_block1(&slicer,
 *     (res < 0) ? NULL : &block_option, &chunk);
 * // handle error
 * if (slicer.block.more_available) {
 *     // send another block-wise request
 *     request->payload = chunk;
 *     request->payload_size = res;
 *     res = unicoap_options_set_block1(request->options, &slicer.block);
 *     // send ...
 * }
 * ```
 *
 * @returns @p chunk  size on success
 * @returns Negative errno on failure
 *
 * @see [Retrieving the Block1 option](@ref unicoap_options_get_block1)
 * @see [Setting the Block1 option](@ref unicoap_options_set_block1)
 * @see [Initializing the iterator as a slicer](@ref unicoap_blockwise_iterator_init)
 */
ssize_t unicoap_blockwise_slice_block1(unicoap_blockwise_iterator_t* slicer,
                                       unicoap_block_option_t block, uint8_t** chunk);

/**
 * @brief Collects a Block1 chunk received from the client.
 *
 * Use this API at the server to keep track of blocks received and, optionally, to reassemble them.
 *
 * @note You can safely call this API more than once for a given request.
 *
 * @param[in,out] collector The block-wise iterator that's used as a block-wise collector.
 * @param[in] block The block option value extracted from a Block1 request. May be `NULL`
 * the first you call this API.
 * @param[in] chunk The block chunk received from the client. This is the payload attached to
 * the request carrying the Block1 option.
 * @param chunk_size Size of @p chunk.
 *
 * For every request you receive, first, retrieve the Block1 option and
 * then call this API as follows. If the option is not present, supply `NULL` in place of the @p block
 * parameter. This may be acceptable if this is the first and only request. After this function returns, check if
 * the Block1 option's `M` flag is set (`block.more_available`) and, if true, send a response with
 * [status `CONTINUE`](@ref UNICOAP_STATUS_CONTINUE) and its Block1 options set to
 * `collector.block` to ask the client to send the next request block.
 *
 * @note The first request you receive from the client may not have a Block1 option attached in case
 * the request body is not transmitted block-wise, i.e., the entire body is contained
 * in a single request. Callers are encouraged to use this API even if the Block1 option is not present in a
 * request, as this API is prepared to handle these cases and implements appropriate debug logging.
 *
 * ```c
 * unicoap_block_t block_option = {0};
 * res = unicoap_options_get_block1(request->options, &block);
 * // handle error
 * res = unicoap_blockwise_collect_block1(&collector,
 *     (res < 0) ? NULL : &block_option, request->payload, request->payload_size);
 * // handle error
 * if (block_option.more_available) {
 *     // send a 2.31 Continue response
 *     res = unicoap_options_set_block1(response->options, &collector.block);
 *     // send ...
 * }
 * ```
 *
 * @return `0` on success
 * @return Negative errno on failure
 *
 * @see [Retrieving the Block1 option](@ref unicoap_options_get_block1)
 * @see [Setting the Block1 option](@ref unicoap_options_set_block1)
 * @see [Initializing the iterator as a collector or reassembler](@ref unicoap_blockwise_iterator_init)
 */
int unicoap_blockwise_collect_block1(unicoap_blockwise_iterator_t* collector,
                                     unicoap_block_option_t block, uint8_t* chunk,
                                     size_t chunk_size);
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

/* MARK: - Automatic Block-wise Transfers */
/**
 * @addtogroup net_unicoap_blockwise_automatic
 * @{
 */
/**
 * @name Customizing automatic block-wise transfer support
 * @{
 */
/**
 * @brief Maximum number of Block-wise transfers active at a time.
 *
 * **Default**: Only a single transfer active at a time
 *
 * TODO: description
 */
#if !defined(CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY) || defined(DOXYGEN)
#  if IS_USED(MODULE_UNICOAP_BLOCKWISE)
#    define CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY (2)
#  else
#    define CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY (0)
#  endif
#endif

static_assert(CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY <=
    (CONFIG_UNICOAP_CLIENT_MEMOS_CAPACITY + CONFIG_UNICOAP_SERVER_MEMOS_CAPACITY),
              "CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY must not exceed "
              "(CONFIG_UNICOAP_CLIENT_MEMOS_CAPACITY + CONFIG_UNICOAP_SERVER_MEMOS_CAPACITY)");

#if IS_USED(MODULE_UNICOAP_BLOCKWISE)
#  if CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY == 0
#    error "CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY is zero. Auto-reassembly, auto-slicing, and unicoap_send_request_blockwise_* functions are impossible to use."
#  endif
#endif

/**
 * @brief Maximum number of Block-wise buffers for slicing and reassembling.
 *
 * **Default**: @ref CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY (a buffer for each transfer)
 *
 * Set this constant to zero if you always use the [no-copy slice flag](@ref UNICOAP_CLIENT_FLAG_SLICE_NO_COPY)
 * and never let unicoap [reassemble](@ref UNICOAP_CLIENT_FLAG_REASSEMBLE) requests/responses (client)
 * and if you always use the [no-copy slice flag](@ref UNICOAP_RESOURCE_FLAG_SLICE_NO_COPY)
 * and never let unicoap [reassemble](@ref UNICOAP_RESOURCE_FLAG_REASSEMBLE) requests/responses (client)
 *
 */
#if !defined(CONFIG_UNICOAP_BLOCKWISE_BUFFERS_POOL_CAPACITY) || defined(DOXYGEN)
#  define CONFIG_UNICOAP_BLOCKWISE_BUFFERS_POOL_CAPACITY CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY
#endif

static_assert(CONFIG_UNICOAP_BLOCKWISE_BUFFERS_POOL_CAPACITY <= CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY,
              "CONFIG_UNICOAP_BLOCKWISE_BUFFERS_POOL_CAPACITY must not exceed "
              "CONFIG_UNICOAP_BLOCKWISE_TRANSFERS_CAPACITY");

/**
 * @brief Block-wise buffer capacity.
 *
 * **Default**: 1024
 *
 * This constant represents the maximum size of
 * - a resource body (entire response) that can be auto-sliced by the server ([except you use no-copy mode](@ref UNICOAP_RESOURCE_FLAG_SLICE_NO_COPY)),
 * - a resource body (entire response) that can be auto-reassembled by the client,
 * - a request body that can be auto-sliced by the client ([except you use no-copy mode](@ref UNICOAP_CLIENT_FLAG_SLICE_NO_COPY)), or
 * - a request body that can be auto-reassembled by the server.
 */
#if !defined(CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY) || defined(DOXYGEN)
#  define CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY (1024)
#endif

#if IS_USED(MODULE_UNICOAP_BLOCKWISE) && CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY == 0
#  warning "CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY is zero. Auto-reassembly is impossible, auto-slicing is limited to UNICOAP_FLAG_SLICE_NO_COPY, otherwise impossible to use"
#endif

/**
 * @brief Indicates the maximum number of BERT blocks sent in a single message by unicoap.
 *
 * **Default**: 4 blocks in a single reliable transport message
 *
 */
#if !defined(CONFIG_UNICOAP_BLOCKWISE_BERT_BLOCKS_PER_MESSAGE_MAX) || defined(DOXYGEN)
#  define CONFIG_UNICOAP_BLOCKWISE_BERT_BLOCKS_PER_MESSAGE_MAX (4)
#endif

/**
 * @brief Block size unicoap will suggest for Block1 and Block2 transfers
 * in auto-slice mode ([client](@ref UNICOAP_CLIENT_FLAG_SLICE)/[server](@ref UNICOAP_RESOURCE_FLAG_SLICE)),
 * auto-reassemble mode ([client](@ref UNICOAP_CLIENT_FLAG_REASSEMBLE)/[server](@ref UNICOAP_RESOURCE_FLAG_REASSEMBLE)),
 * and when using [Block-wise client](@ref unicoap_send_request_blockwise_async) [callback functions](@ref unicoap_send_request_blockwise_sync).
 *
 * **Default**: 32 bytes
 */
#if !defined(CONFIG_UNICOAP_BLOCK_SIZE) || defined(DOXYGEN)
#  define CONFIG_UNICOAP_BLOCK_SIZE (32)
#endif

#ifdef CONFIG_UNICOAP_BLOCK_SZX
#  error "CONFIG_UNICOAP_BLOCK_SZX must not be configured manually."
#endif

/**
 * @brief Enables the Block-wise extension for Reliable Transports (BERT)
 * @see [RFC 8323](https://datatracker.ietf.org/doc/html/rfc8323)
 */
#if !defined(CONFIG_UNICOAP_BERT) || defined(DOXYGEN)
#  define CONFIG_UNICOAP_BERT 1
#endif

#ifndef DOXYGEN
#  ifdef CONFIG_UNICOAP_BLOCK_SZX
#    error CONFIG_UNICOAP_BLOCK_SZX must not be configured manually.
#  endif
#  if CONFIG_UNICOAP_BLOCK_SIZE == 1024
#    define CONFIG_UNICOAP_BLOCK_SZX (6)
#  elif CONFIG_UNICOAP_BLOCK_SIZE == 512
#    define CONFIG_UNICOAP_BLOCK_SZX (5)
#  elif CONFIG_UNICOAP_BLOCK_SIZE == 256
#    define CONFIG_UNICOAP_BLOCK_SZX (4)
#  elif CONFIG_UNICOAP_BLOCK_SIZE == 128
#    define CONFIG_UNICOAP_BLOCK_SZX (3)
#  elif CONFIG_UNICOAP_BLOCK_SIZE == 64
#    define CONFIG_UNICOAP_BLOCK_SZX (2)
#  elif CONFIG_UNICOAP_BLOCK_SIZE == 32
#    define CONFIG_UNICOAP_BLOCK_SZX (1)
#  elif CONFIG_UNICOAP_BLOCK_SIZE == 16
#    define CONFIG_UNICOAP_BLOCK_SZX (0)
#  else
#    error CONFIG_UNICOAP_BLOCK_SIZE must be 1024, 512, 256, 128, 64, 32, or 16
#  endif
#endif

static_assert(
    CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY == 0 ||
        CONFIG_UNICOAP_BLOCK_SIZE < CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY,
    "CONFIG_UNICOAP_BLOCK_SIZE must be smaller than CONFIG_UNICOAP_BLOCKWISE_BODY_CAPACITY");

#if CONFIG_UNICOAP_BLOCK_SIZE > CONFIG_UNICOAP_PDU_SIZE_MAX
#  warning "CONFIG_UNICOAP_BLOCK_SIZE exceeds CONFIG_UNICOAP_PDU_SIZE_MAX, inbound messages might get truncated"
#endif
/** @} */
/** @} */
