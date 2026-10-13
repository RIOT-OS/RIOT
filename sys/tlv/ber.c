/*
 * SPDX-FileCopyrightText: Carl Seifert <carl.seifert@tu-dresden.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <string.h>
#include <stddef.h>
#include "errno.h"
#include "tlv.h"
#include "byteorder.h"
#include "architecture.h"

#define LOG_UNIT "tlv.ber"
#include "log.h"

#define BER_TLV_TAG_NUMBER_MASK (0x1F)
#define BER_TLV_CONTINUED_CONTINUE_FLAG (0x80)
#define BER_TLV_CONTINUED_NUMBER_MASK (0x7f)

#define _BITS_TO_BYTES(_bits) ((_bits + 7) / 8)
#define _BIT_WIDTH(n) sizeof(uint64_t) * 8 - __builtin_clzll((uint64_t)((n) | 1))

static inline size_t ber_tlv_encoded_size(tlv_tag_t tag, size_t value_length) {
    return
        _BITS_TO_BYTES(_BIT_WIDTH(tag)) +
        ((value_length < 0x80) ? 1 : 1 + _BITS_TO_BYTES(_BIT_WIDTH(value_length))) +
        value_length;
}

#if defined(MODULE_TLV_WRITER)
static size_t _ber_tlv_sizer(tlv_tag_t tag, size_t value_length) {
     return ber_tlv_encoded_size(tag, value_length);
}
const _tlv_ops_t _tlv_ops_ber = {
    .encoder = ber_tlv_encode,
    .decoder = ber_tlv_decode,
    .sizer = _ber_tlv_sizer
};
#endif


ssize_t ber_tlv_decode(
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

    tlv_tag_t _tag = (tlv_tag_t)*cursor++;
    size -= 1;
    if ((_tag & BER_TLV_TAG_NUMBER_MASK) == BER_TLV_TAG_NUMBER_MASK) {
        uint8_t i = 1;
        do {
            if (size < 1) {
                return -EBADMSG;
            }
            uint8_t part = *cursor++;
            size -= 1;

            if (i >= sizeof(tlv_tag_t)) {
                LOG_ERROR("tag 0x%"PRIx64".. overflows tlv_tag_t\n", (uint64_t)_tag);
                return -EOVERFLOW;
            } else if (i == 1 && !(part & BER_TLV_CONTINUED_NUMBER_MASK)) {
                LOG_ERROR("tag must not have leading zero group\n");
                return -EBADMSG;
            }

            _tag <<= 8;
            _tag |= (tlv_tag_t)part;
            i += 1;
        } while (_tag & BER_TLV_CONTINUED_CONTINUE_FLAG);
    }

    if (size < 1) {
        return -EBADMSG;
    }
    size_t length = (size_t)*cursor++;
    size -= 1;
    if (length & BER_TLV_CONTINUED_CONTINUE_FLAG) {
        if (length == BER_TLV_CONTINUED_CONTINUE_FLAG) {
            LOG_ERROR("indefinte length not supported\n");
            return -ENOTSUP;
        }
        if (length == 0xFF) {
            return -EBADMSG;
        }
        uint8_t length_size = (uint8_t)(length & BER_TLV_CONTINUED_NUMBER_MASK);
        if (size < (size_t)length_size) {
            return -EBADMSG;
        }
        if (length_size > sizeof(size_t)) {
            LOG_ERROR("length overflows size_t\n");
            return -EOVERFLOW;
        }

        size -= length_size;
        length = 0;
        for (; length_size > 0; length_size -= 1) {
            length <<= 8;
            length |= (uint8_t)*cursor++;
        }
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

ssize_t ber_tlv_encode(
    uint8_t* buffer,
    size_t capacity,
    tlv_tag_t tag,
    size_t length,
    const uint8_t* value
) {
    assert(buffer);
    if (capacity < ber_tlv_encoded_size(tag, length)) {
        return -ENOBUFS;
    }
    uint8_t* cursor = buffer;

    uint8_t tag_size = _BITS_TO_BYTES(_BIT_WIDTH(tag));
    /* Write tag beginning at the rightmost (least significant) byte */
    for (int8_t i = tag_size - 1; i >= 0; tag >>= 8, i -= 1) {
        cursor[i] = (uint8_t)(tag & 0xff);
    }
    cursor += tag_size;

    if (length < 0x80) {
        *cursor++ = (uint8_t)length;
    } else {
        uint8_t length_size = _BITS_TO_BYTES(_BIT_WIDTH(length));
        assert(length_size < BER_TLV_CONTINUED_NUMBER_MASK);
        *cursor++ = (uint8_t)(BER_TLV_CONTINUED_CONTINUE_FLAG | length_size);
        
        size_t _length = length;
        /* Write length beginning at the rightmost (least significant) byte */
        for (int8_t i = length_size - 1; i >= 0; _length >>= 8, i -= 1) {
            cursor[i] = (uint8_t)(_length & 0xff);
        }
        cursor += length_size;
    }

    if (length > 0) {
        assert(value);
        memcpy(cursor, value, length);
    }
    cursor += length;
    return (size_t)(ptrdiff_t)(cursor - buffer);
}
