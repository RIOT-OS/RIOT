/*
 * SPDX-FileCopyrightText: Carl Seifert <carl.seifert@tu-dresden.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <string.h>
#include <stddef.h>
#include "errno.h"
#include "tlv.h"
#include "byteorder.h"

#define LOG_UNIT "tlv.simple"
#include "log.h"

static inline size_t simple_tlv_encoded_size(size_t value_length) {
    return (value_length < 0xFF) ? (2 + value_length) : (4 + value_length);
}

#if defined(MODULE_TLV_WRITER)
size_t _simple_tlv_sizer(tlv_tag_t tag, size_t value_length) {
    (void)tag;
    return simple_tlv_encoded_size(value_length);
}
const _tlv_ops_t _tlv_ops_simple = {
    .encoder = simple_tlv_encode,
    .decoder = simple_tlv_decode,
    .sizer = _simple_tlv_sizer
};
#endif

ssize_t simple_tlv_decode(
    const uint8_t* buffer,
    size_t size,
    tlv_tag_t* tag,
    size_t* value_length,
    const uint8_t** value
) {
    assert(buffer);
    const uint8_t* cursor = buffer;

    if (size < 2) {
        return -EBADMSG;
    }

    const tlv_tag_t _tag = (tlv_tag_t)*cursor++;
    if (_tag == 0 || _tag == 0xFF) {
        return -EBADMSG;
    }
    size_t length = *cursor++;
    size -= 2;

    if (length == 0xFF) {
        if (size < 2) {
            return -EBADMSG;
        }
        length = byteorder_bebuftohs(cursor);
        if (length < 0xFF) {
            return -EBADMSG;
        }
        cursor += 2;
        size -= 2;
    }

    if (length > size) {
        return -EBADMSG;
    }
    if (tag) {
        *tag = _tag;
    }
    if (value) {
        *value = cursor;
    }
    if (value_length) {
        *value_length = length;
    }
    cursor += length;
    assert(cursor > buffer);
    return (ssize_t)(ptrdiff_t)(cursor - buffer);
}

ssize_t simple_tlv_encode(
    uint8_t* buffer,
    size_t capacity,
    tlv_tag_t tag,
    size_t length,
    const uint8_t* value
) {
    assert(cursor);
    assert(length <= UINT16_MAX);
    if (tag == 0 || tag >= 0xFF) {
        LOG_ERROR("SIMPLE-TLV tags must be in 0<..<0xFF\n");
        return -EINVAL;
    }
    if (capacity < simple_tlv_encoded_size(length)) {
        return -ENOBUFS;
    }
    uint8_t* cursor = buffer;
    *cursor++ = tag;

    if (length < 0xFF) {
        *cursor++ = (uint8_t)length;
    } else {
        *cursor++ = 0xFF;
        *cursor++ = (uint8_t)(length >> 8);
        *cursor++ = (uint8_t)(length & 0xFF);
    }

    if (length > 0) {
        assert(value);
        memcpy(cursor, value, length);
    }
    cursor += length;
    return (size_t)(ptrdiff_t)(cursor - buffer);
}
