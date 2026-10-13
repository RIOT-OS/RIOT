// SPDX-FileCopyrightText: Carl Seifert <carl.seifert@tu-dresden.de>
// SPDX-License-Identifier: LGPL-2.1-only

#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup sys_tlv Type-Length-Encoded Values (TLV)
 * @ingroup sys
 * @brief Decode and encode type-length-value objects
 * @{
 * Module. Specify `USEMODULE += tlv` in your application Makefile.
 *
 * The TLV module supports two encoding schemes:
 * - SIMPLE-TLV (1-byte tags, values of max 65535 bytes)
 * - BER-TLV (Basic Encoding Rules used in ASN.1, variable-length tags and values).
 *
 * Specify `USEMODULE += tlv_simple` and/or `USEMODULE += tlv_ber` otherwise.
 * By default, `tlv_simple` is imported. Note that unused functions
 * may be stripped anymore if you compile with link-time optimizations enabled.
 *
 * @remark
 * In BER-TLV-encoded buffers, only tag lengths up to 32 bits are supported.
 * If you need a bigger integer type, you may define @ref TLV_TAG_TYPE, e.g.,
 * by setting `CFLAGS += -DTLV_TAG_TYPE=uint64_t`.
 * Indefinite-length TLV records and those with values longer than would
 * fit a `size_t` are not supported either.
 *
 * ## Encoding
 * A TLV-encoded object consists of a 1-byte tag, a length field,
 * and the value. The length occupies a single byte, or three bytes
 * if the value is longer than 254 bytes.
 * - `TAG LENGTH VALUE...` (`LENGTH` < 254)
 * - `TAG FF LENGTH_MSB LENGTH_LSB VALUE...` (255 >= `LENGTH` <= 0xffff)
 *
 * ## Usage Example
 * ```c
 * uint8_t tlv[] = {
 *     0x01, 3, 'H', 'i', '!',
 *     0x42, 1, 0x42
 * };
 *
 * // Retrieve individual tags
 * ssize_t res = 0;
 * uint8_t* value = NULL;
 * if ((res = simple_tlv_find(tlv, sizeof(tlv), 0x42, &value)) < 0) {
 *   // Handle error...
 * }
 * printf("tag=0x42 length=%"PRIdSIZE" value[0]=%02x\n", res, value[0]);
 *
 * // Iterate over all tags
 * uint8_t tag = 0;
 * size_t length = 0;
 * SIMPLE_TLV_ITER(tlv, sizeof(tlv), &tag, &length, &value) {
 *   printf("tag=%02x length=%"PRIuSIZE" value[0]=%02x\n", tag, length, value[0]);
 * }
 *
 * // Write tags
 * uint8_t buffer[50] = {};
 * char message[] = "Hello, World!";
 * if ((res = simple_tlv_encode(buffer, sizeof(buffer), 0x42, sizeof(message) - 1, (uint8_t*)message)) < 0) {
 *   // Handle error...
 * }
 * ```
 *
 * ## Buffer operations
 * If you have a need to update already encoded TLV records, or a more streamlined
 * encoding process, refer to @ref sys_net_tlv_writer.
 */

/**
 * @file
 * @brief TLV header
 * @author Carl Seifert <carl.seifert@tu-dresden.de>
 */

/**
 * @name Decoding
 * @{
 */

#if !defined(TLV_TAG_TYPE) || defined(DOXYGEN)
/**
 * @brief TLV tag type
 *
 * You may redefine this constant to another unsigned integer type to widen
 * or narrow the maximum permitted tag bit width while encoding and decoding.
 * This will affect all coding schemes. Note that some coding schemes, such
 * as SIMPLE-TLV, are restricted to 8-bit tags anyway. Widening the tag type
 * will not have an effect there.
 *
 * @warning
 * Defining a narrower tag type may break implementations in RIOT
 * where the TLV module is used and tags are encoded or expected to be decoded
 * that are wider than the tag type you defined.
 */
#  define TLV_TAG_TYPE uint32_t
#endif

 /** @brief TLV tag */
typedef TLV_TAG_TYPE tlv_tag_t;

/**
 * @brief Reads one SIMPLE-TLV record and decodes it into tag, length, and value
 *
 * @param[in,out] buffer Buffer pointer to the start of the TLV record
 * @param size Total number of valid TLV bytes
 * @param[in,out] tag The tag read
 * @param[in,out] value_length Number of bytes belonging to @p value
 * @param[in,out] value Value pointer
 *
 * @returns Number of read bytes this TLV record consists of, incl. header and
 *          value, or negative error number
 *
 * @retval -EBADMSG The given TLV record is malformed
 */
ssize_t simple_tlv_decode(
    const uint8_t* buffer,
    size_t size,
    tlv_tag_t* tag,
    size_t* value_length,
    const uint8_t** value
);

/**
 * @brief Reads one BER-TLV record and decodes it into tag, length, and value
 *
 * @param[in,out] buffer Buffer pointer to the start of the TLV record
 * @param size Total number of valid TLV bytes
 * @param[in,out] tag The tag read
 * @param[in,out] value_length Number of bytes belonging to @p value
 * @param[in,out] value Value pointer
 *
 * @returns Number of read bytes this TLV record consists of, incl. header and
 *          value, or negative error number
 *
 * @retval -EBADMSG The given TLV record is malformed
 */
ssize_t ber_tlv_decode(
    const uint8_t* buffer,
    size_t size,
    tlv_tag_t* tag,
    size_t* value_length,
    const uint8_t** value
);

/** @cond */
typedef __typeof__(simple_tlv_decode) *_tlv_decoder_t;
ssize_t _tlv_find(
    const uint8_t* buffer,
    size_t size,
    tlv_tag_t tag,
    const uint8_t** value,
    _tlv_decoder_t decoder
);
/** @endcond */

/**
 * @brief Tries to find a SIMPLE-TLV value with the given tag
 *
 * @param[in] buffer Pointer to buffer containing consecutive TLV records
 * @param size Total number of valid TLV bytes in @p buffer
 * @param tag The tag to find
 * @param[in,out] value Value pointer
 *
 * @returns Length of value or negative error number
 *
 * @retval -EBADMSG The given TLV record is malformed
 * @retval -ENOENT No value with the given tag was found
 */
static inline ssize_t simple_tlv_find(
    const uint8_t* buffer,
    size_t size,
    tlv_tag_t tag,
    const uint8_t** value
) {
    return _tlv_find(buffer, size, tag, value, simple_tlv_decode);
}

/**
 * @brief Tries to find a BER-TLV value with the given tag
 *
 * @param[in] buffer Pointer to buffer containing consecutive TLV records
 * @param size Total number of valid TLV bytes in @p buffer
 * @param tag The tag to find
 * @param[in,out] value Value pointer
 *
 * @returns Length of value or negative error number
 *
 * @retval -EBADMSG The given TLV record is malformed
 * @retval -ENOENT No value with the given tag was found
 */
static inline ssize_t ber_tlv_find(
    const uint8_t* buffer,
    size_t size,
    tlv_tag_t tag,
    const uint8_t** value
) {
    return _tlv_find(buffer, size, tag, value, ber_tlv_decode);
}

/**
 * @brief For-loop clause for iterating over all TLV tags and their values
 *
 * @param[in,out] buffer Buffer pointer to the start of the TLV record
 * @param size Total number of valid TLV bytes
 * @param[in,out] tag The tag
 * @param[in,out] value_length Number of bytes belonging to @p value
 * @param[in,out] value Value pointer
 *
 * ## Example
 * ```c
 * uint8_t tag = 0;
 * size_t length = 0;
 * const uint8_t* value = NULL;
 *
 * TLV_ITER(tlv, sizeof(tlv), &tag, &length, &value) {
 *   printf("tag=%02x length=%"PRIuSIZE" value=[", tag, length);
 *   for (size_t i = 0; i < length; i += 1) {
 *     printf("%02x", value[i]);
 *   }
 *   printf("]\n");
 * }
 * ```
 */
#define TLV_ITER(buffer, size, tag, length, value_length, value) \
    for (ssize_t tlv_off = 0, res = 0;                                        \
         tlv_off < (ssize_t)(size) &&                                         \
         (res = tlv_decode((buffer) + tlv_off, (size) - (size_t)tlv_off, tag, \
                            length, value)) > 0;                              \
         tlv_off += (size_t)res)

/** @} */

/**
 * @name Encoding
 * @{
 */

/**
 * @brief Writes one SIMPLE-TLV record, encoding tag, length, and value
 *
 * @param[in,out] buffer Cursor pointer to where TLV record is to be written
 * @param capacity Available capacity in bytes in the buffer @p cursor points into
 * @param tag The tag to write
 * @param value_length The length of @p value in bytes
 * @param[in] value The value to write
 *
 * @returns Number of written bytes this TLV record consists of, incl. header
 *          and value, or negative error number
 *
 * @retval -ENOBUFS The buffer has insufficient capacity to write this TLV record
 */
ssize_t simple_tlv_encode(
    uint8_t* buffer,
    size_t capacity,
    tlv_tag_t tag,
    size_t value_length,
    const uint8_t* value
);

/**
 * @brief Writes one BER-TLV record, encoding tag, length, and value
 *
 * @param[in,out] buffer Cursor pointer to where TLV record is to be written
 * @param capacity Available capacity in bytes in the buffer @p cursor points into
 * @param tag The tag to write
 * @param value_length The length of @p value in bytes
 * @param[in] value The value to write
 *
 * @returns Number of written bytes this TLV record consists of, incl. header
 *          and value, or negative error number
 *
 * @retval -ENOBUFS The buffer has insufficient capacity to write this TLV record
 */
ssize_t ber_tlv_encode(
    uint8_t* buffer,
    size_t capacity,
    tlv_tag_t tag,
    size_t value_length,
    const uint8_t* value
);

/** @cond */
typedef __typeof__(simple_tlv_encode) *_tlv_encoder_t;
/** @endcond */

/** @} */

/**
 * @defgroup sys_tlv_writer TLV Buffer Writer
 * @ingroup sys_tlv
 * @brief Perform TLV operations on a TLV buffer with a size and capacity
 *
 * Module. Specify `USEMODULE += tlv_writer` in your application Makefile.
 * @{
 *
 * Use this module to update values of TLV records already encoded
 * or to remove specific tags.
 *
 * @remark
 * Note that this module will use additional
 * stack memory for the @ref tlv_writer_t and ROM to allocate a dynamic
 * dispatch table for the individual TLV coder's operations.
 *
 * ## Usage example
 * ```c
 * ssize_t res;
 * uint8_t tlv[64] = {
 *  0x9F, 0x02, 
 *     6,
 *     0, 0, 0, 0, 0x12, 0x34
 * };
 * tlv_writer_t w = BER_TLV_WRITER(tlv, 9, sizeof(tlv));
 * 
 * // Amount Authorised
 * const uint8_t amount[] = { 0x00, 0x00, 0x00, 0x00, 0x12, 0x34 };
 * if ((res = tlv_writer_set(&w, 0x9F02, sizeof(amount), amount)) < 0) {
 *     // handle error
 * }
 * 
 * // Transaction Currency Code (0978 = EUR)
 * const uint8_t currency[] = { 0x09, 0x78 };
 * if ((res = tlv_writer_add(&w, 0x5F2A, sizeof(currency), currency)) < 0) {
 *     // handle error
 * }
 * 
 * // Terminal Country Code (0276 = Germany)
 * const uint8_t country[] = { 0x02, 0x76 };
 * if ((res = tlv_writer_add(&w, 0x9F1A, sizeof(country), country)) < 0) {
 *     // handle error
 * }
 * 
 * // Transaction Type (0000 = purchase)
 * const uint8_t type = 0x00;
 * if ((res = tlv_writer_add(&w, 0x9C, sizeof(type), type)) < 0) {
 *     // handle error
 * }
 *
 * // ... Later ...
 * 
 * const uint8_t* value = NULL;
 * if (tlv_writer_get(&w, 0x5F2A, &value) == sizeof(currency)) {
 *     printf("currency code %02X%02X\n", value[0], value[1]);
 * }
 * 
 * if ((res = tlv_writer_remove(&w, 0x9C)) < 0) {
 *     // handle error
 * }
 * ```
 */

/**
 * @name Creating a buffer writer
 * @{
 */

/** @cond */
typedef struct {
    _tlv_encoder_t encoder;
    _tlv_decoder_t decoder;
    size_t (*sizer)(tlv_tag_t tag, size_t value_length);
} _tlv_ops_t;

#define _TLV_WRITER(_buffer, _size, _capacity, _ops) \
    ((tlv_writer_t) { \
        .buffer = _buffer, .size = _size, .capacity = _capacity, .ops = _ops \
     })
/** @endcond */

#if defined(MODULE_TLV_SIMPLE) && defined(MODULE_TLV_WRITER) || defined(DOXYGEN)
/** @cond */
extern const _tlv_ops_t _tlv_ops_simple;
/** @endcond */

/**
 * @brief SIMPLE-TLV buffer writer
 *
 * @param[in,out] buffer Buffer to read and write TLV records from
 * @param size Number of used TLV bytes in @p buffer, may be zero
 * @param capacity Number of available bytes in @p buffer, must be at least @p size
 *
 * @returns Initialized @ref tlv_writer_t struct
 */
#  define SIMPLE_TLV_WRITER(buffer, size, capacity) \
    _TLV_WRITER(buffer, size, capacity, &_tlv_ops_simple)
#endif

#if defined(MODULE_TLV_BER) && defined(MODULE_TLV_WRITER) || defined(DOXYGEN)
/** @cond */
extern const _tlv_ops_t _tlv_ops_ber;
/** @endcond */

/**
 * @brief BER-TLV buffer writer
 *
 * @param[in,out] buffer Buffer to read and write TLV records from
 * @param size Number of used TLV bytes in @p buffer, may be zero
 * @param capacity Number of available bytes in @p buffer, must be at least @p size
 *
 * @returns Initialized @ref tlv_writer_t struct
 */
#  define BER_TLV_WRITER(buffer, size, capacity) \
    _TLV_WRITER(buffer, size, capacity, &_tlv_ops_ber)
#endif

/** @} */

/**
 * @name Using the buffer writer
 * @{
 */

/**
 * @brief A type for writing TLV records into a buffer
 */
typedef struct {
    /**
     * @brief Buffer base pointer
     * @private
     */
    uint8_t* buffer;

    /**
     * @brief Occupied TLV bytes 
     * @private
     */
    size_t size;

    /**
     * @brief Total available TLV bytes 
     * @private
     */
    size_t capacity;

    /**
     * @brief TLV coding operations
     * @private
     */
    const _tlv_ops_t* ops;
} tlv_writer_t;

/**
 * @brief Retrieve current buffer size from a TLV writer
 *
 * @param[in] writer TLV buffer writer
 *
 * @returns Sum of all TLV record sizes in bytes
 */
static inline size_t tlv_writer_size(const tlv_writer_t* writer) {
    return writer->size;
}

/**
 * @brief Retrieve current buffer capacity from a TLV writer
 *
 * @param[in] writer TLV buffer writer
 *
 * @returns Total capacity of buffer in bytes
 */
static inline size_t tlv_writer_capacity(const tlv_writer_t* writer) {
    return writer->capacity;
}

/**
 * @brief Appends one TLV record at the end of a buffer
 *
 * @memberof tlv_writer_t
 *
 * @param[in,out] writer TLV buffer writer
 * @param tag The tag to write
 * @param value_length The length of @p value in bytes
 * @param[in] value The value to write
 *
 * @pre The @p writer has a capacity of at least its current size.
 *
 * This method exists analogous to @ref tlv_writer_t::tlv_writer_set.
 * Whenever you know, a value with this tag has not already been written to
 * @p writer, use this method. Otherwise, refer to
 * @ref tlv_writer_t::tlv_writer_set.
 *
 * @returns New size of @p writer in bytes, now including the new TLV record;
 *          or negative error number
 *
 * @retval -ENOBUFS The buffer has insufficient capacity to write this TLV record
 */
static inline ssize_t tlv_writer_add(
    tlv_writer_t* writer,
    tlv_tag_t tag,
    size_t value_length,
    const uint8_t* value
) {
    assert(writer);
    assert(writer->buffer);
    assert(writer->ops);
    assert(writer->size <= writer->capacity);
    ssize_t encoded = writer->ops->encoder(
        writer->buffer + writer->size,
        writer->capacity - writer->size,
        tag, value_length, value);
    if (encoded < 0) {
        return encoded;
    }
    writer->size += encoded;
    return writer->size;
}

/**
 * @brief Updates or appends one value with the given tag
 *
 * @memberof tlv_writer_t
 *
 * @param[in,out] writer TLV buffer writer
 * @param tag The tag to write
 * @param value_length The length of @p value in bytes
 * @param[in] value The value to write
 *
 * @pre The @p writer has a capacity of at least its current size.
 *
 * This method exists analogous to @ref tlv_writer_t::tlv_writer_set.
 * Whenever you know, a value with this tag has already been written to
 * @p writer, use this method. Otherwise, refer to
 * @ref tlv_writer_t::tlv_writer_add.
 *
 * @returns New size of @p writer in bytes, now including the new or updated
 *          TLV record; or negative error number
 *
 * @retval -EBADMSG A TLV record in @p writer is malformed
 * @retval -ENOBUFS The buffer has insufficient capacity to write this TLV record
 */
ssize_t tlv_writer_set(
    tlv_writer_t* writer,
    tlv_tag_t tag,
    size_t value_length,
    const uint8_t* value
);

/**
 * @brief Tries to remove a value with the given tag
 *
 * @memberof tlv_writer_t
 *
 * @param[in,out] writer TLV buffer writer
 * @param tag The tag to find
 *
 * @returns New size of @p writer in bytes, now including the removed
 *          TLV record; or negative error number
 *
 * @retval -EBADMSG The given TLV record is malformed
 * @retval -ENOENT No value with the given tag was found
 */
ssize_t tlv_writer_remove(
    tlv_writer_t* writer,
    tlv_tag_t tag
);

/**
 * @brief Tries to find a TLV value with the given tag
 *
 * @memberof tlv_writer_t
 *
 * @param[in] writer TLV buffer writer
 * @param tag The tag to find
 * @param[in,out] value Value pointer
 *
 * @returns Length of value or negative error number
 *
 * @retval -EBADMSG The given TLV record is malformed
 * @retval -ENOENT No value with the given tag was found
 */
static inline ssize_t tlv_writer_get(
    tlv_writer_t* writer,
    tlv_tag_t tag,
    const uint8_t** value
) {
    assert(writer);
    assert(writer->buffer);
    return _tlv_find(writer->buffer, writer->size, tag, value, writer->ops->decoder);
}

/** @} */

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif
