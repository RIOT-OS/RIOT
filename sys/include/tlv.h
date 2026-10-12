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
 * @defgroup sys_tlv Type-Length Encoded Values (TLV)
 * @ingroup sys
 * @brief Decode and encode type-length-value objects
 * @{
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
 * ssize_t res = 0;
 * uint8_t* value = NULL;
 * if ((res = tlv_find(tlv, sizeof(tlv), 0x42, &value)) < 0) {
 *   // Handle error...
 * }
 * printf("tag=0x42 length="PRIdSIZE" value[0]=%02x\n", res, value[0]);
 *
 * uint8_t buffer[50] = {};
 * char message[] = "Hello, World!";
 * if ((res = tlv_append(buffer, sizeof(buffer), 0x42, sizeof(message) - 1, (uint8_t*)message)) < 0) {
 *   // Handle error...
 * }
 * ```
 */

/**
 * @file
 * @brief TLV header
 * @author Carl Seifert <carl.seifert@tu-dresden.de>
 */

/**
 * @name Reading
 * @{
 */

/**
 * @brief Reads one TLV entry and decodes it into tag, length, and value
 *
 * @param[in,out] buffer Buffer pointer to the start of the TLV entry
 * @param size Total number of valid TLV bytes
 * @param[in,out] tag The tag read
 * @param[in,out] value_length Number of bytes belonging to @p value
 * @param[in,out] value Value pointer
 *
 * @returns Number of read bytes this TLV entry consists of, incl. header and
 *          value, or negative error number
 *
 * @retval -EBADMSG The given TLV entry is malformed
 */
ssize_t tlv_decode(
    const uint8_t* buffer,
    size_t size,
    uint8_t* tag,
    size_t* value_length,
    const uint8_t** value
);

/**
 * @brief Tries to find a value with the given tag
 *
 * @param[in] buffer Pointer to buffer containing consecutive TLV entries
 * @param size Total number of valid TLV bytes in @p buffer
 * @param tag The tag to find
 * @param[in,out] value Value pointer
 *
 * @returns Length of value or negative error number
 *
 * @retval -EBADMSG The given TLV entry is malformed
 * @retval -ENOENT No value with the given tag was found
 */
ssize_t tlv_find(
    uint8_t const* buffer,
    size_t size,
    uint8_t tag,
    const uint8_t** value
);

/** @} */

/**
 * @name Mutating
 * @{
 */

/**
 * @brief Writes one TLV entry, encoding tag, length, and value
 *
 * @param[in,out] buffer Cursor pointer to where TLV entry is to be written
 * @param capacity Available capacity in bytes in the buffer @p cursor points into
 * @param tag The tag to write
 * @param value_length The length of @p value in bytes
 * @param[in] value The value to write
 *
 * @returns Number of written bytes this TLV entry consists of, incl. header
 *          and value, or negative error number
 *
 * @retval -ENOBUFS The buffer has insufficient capacity to write this TLV entry
 */
ssize_t tlv_encode(
    uint8_t* buffer,
    size_t capacity,
    uint8_t tag,
    size_t value_length,
    const uint8_t* value
);

/**
 * @brief Adds one TLV entry at the end of a buffer
 *
 * @param[in,out] buffer Buffer whose end to write TLV entry is to be written
 * @param size Number of used bytes in @p buffer
 * @param capacity Available capacity in bytes in @p buffer
 * @param tag The tag to write
 * @param value_length The length of @p value in bytes
 * @param[in] value The value to write
 *
 * @pre The @p buffer has a @p capacity of at least @p size bytes.
 *
 * This method exists analogous to @ref tlv_update. Whenever you know,
 * a value with this tag has not already been written to @p buffer, use
 * this method. Otherwise, refer to @ref tlv_update.
 *
 * @returns New size of @p buffer in bytes, now including the new TLV entry;
 *          or negative error number
 *
 * @retval -ENOBUFS The buffer has insufficient capacity to write this TLV entry
 */
static inline ssize_t tlv_append(
    uint8_t* buffer,
    size_t size,
    size_t capacity,
    uint8_t tag,
    uint16_t value_length,
    const uint8_t* value
) {
    assert(size <= capacity);
    ssize_t encoded = tlv_encode(buffer + size, capacity - size, tag, value_length, value);
    return encoded < 0 ? encoded : (ssize_t)size + encoded;
}

/**
 * @brief Sets or adds one value with the given tag
 *
 * @param[in,out] buffer Buffer in which to update tag value
 * @param size Number of used bytes in @p buffer
 * @param capacity Available capacity in bytes in @p buffer
 * @param tag The tag to write
 * @param value_length The length of @p value in bytes
 * @param[in] value The value to write
 *
 * @pre The @p buffer has a @p capacity of at least @p size bytes.
 *
 * This method exists analogous to @ref tlv_append. Whenever you know,
 * a value with this tag has not already been written to @p buffer, use
 * this method. Otherwise, refer to @ref tlv_append.
 *
 * @returns New size of @p buffer in bytes, now including the new or updated
 *          TLV entry; or negative error number
 *
 * @retval -EBADMSG A TLV entry in @p is malformed
 * @retval -ENOBUFS The buffer has insufficient capacity to write this TLV entry
 */
ssize_t tlv_update(
    uint8_t* buffer,
    size_t size,
    size_t capacity,
    uint8_t tag,
    uint16_t value_length,
    const uint8_t* value
);

/**
 * @brief Tries to remove a value with the given tag
 *
 * @param[in,out] buffer Pointer to buffer containing consecutive TLV entries
 * @param size Total number of valid TLV bytes in @p buffer
 * @param tag The tag to find
 *
 * @returns New size of @p buffer in bytes, now including the removed
 *          TLV entry; or negative error number
 *
 * @retval -EBADMSG The given TLV entry is malformed
 * @retval -ENOENT No value with the given tag was found
 */
ssize_t tlv_remove(
    uint8_t* buffer,
    size_t size,
    uint8_t tag
);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif
