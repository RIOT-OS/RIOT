// SPDX-FileCopyrightText: Carl Seifert <carl.seifert@tu-dresden.de>
// SPDX-License-Identifier: LGPL-2.1-only

#include <string.h>
#include <stddef.h>
#include "errno.h"
#include "tlv.h"

static inline size_t tlv_encoded_size(size_t length) {
    return (length < 0xFF) ? (size_t)(2 + length) : (size_t)(4 + length);
}

ssize_t tlv_decode(
    const uint8_t** cursor,
    size_t size,
    uint8_t* tag,
    size_t* value_length,
    const uint8_t** value
) {
    assert(cursor);
    assert(tag);
    const uint8_t* _cursor = *cursor;

    if (size < 2) {
        return -EBADMSG;
    }

    const uint8_t _tag = *_cursor++;
    size_t length = *_cursor++;
    size -= 2;

    if (length == 0xFF) {
        if (size < 2) {
            return -EBADMSG;
        }
        length = (uint16_t)(((uint16_t)_cursor[0] << 8) | _cursor[1]);
        if (length < 0xFF) {
            return -EBADMSG;
        }
        _cursor += 2;
        size -= 2;
    }

    if (length > size) {
        return -EBADMSG;
    }
    if (tag) {
        *tag = _tag;
    }
    if (value) {
        *value = _cursor;
    }
    if (value_length) {
        *value_length = length;
    }
    _cursor += length;
    ssize_t consumed = _cursor - *cursor;
    *cursor = _cursor;
    return consumed;
}


ssize_t tlv_find(
    uint8_t const* buffer,
    size_t size,
    uint8_t tag,
    uint8_t const** value
) {
    ssize_t res = 0;
    size_t value_length = 0;
    uint8_t value_tag = 0;
    while (size > 0) {
        if ((res = tlv_decode(&buffer, size, &value_tag, &value_length, value)) < 0) {
            return res;
        }
        size -= (size_t)res;
        if (value_tag == tag) {
            return (ssize_t)value_length;
        }
    }
    return -ENOENT;
}

ssize_t tlv_encode(
    uint8_t** cursor,
    size_t capacity,
    uint8_t tag,
    size_t length,
    uint8_t const* value
) {
    assert(cursor);
    assert(length <= UINT16_MAX);
    if (capacity < tlv_encoded_size(length)) {
        return -ENOBUFS;
    }
    uint8_t* buffer = *cursor;
    *(*cursor)++ = tag;

    if (length < 0xFF) {
        *(*cursor)++ = (uint8_t)length;
    } else {
        *(*cursor)++ = 0xFF;
        *(*cursor)++ = (uint8_t)(length >> 8);
        *(*cursor)++ = (uint8_t)(length & 0xFF);
    }

    if (length > 0) {
        assert(value);
        memcpy(*cursor, value, length);
        *cursor += length;
    }
    return (size_t)(ptrdiff_t)(*cursor - buffer);
}

ssize_t tlv_update(
    uint8_t* buffer,
    size_t size,
    size_t capacity,
    uint8_t tag,
    uint16_t length,
    const uint8_t* value
) {
    assert(buffer);
    assert(capacity >= size);
    uint8_t* cursor = buffer;
    size_t new_encoded = tlv_encoded_size(length);

    ssize_t res = 0;
    size_t value_length = 0;
    uint8_t value_tag = (uint8_t)~tag;
    size_t read = 0;
    while (read < size) {
        if ((res = tlv_decode(
            (const uint8_t**)&cursor,
            size - read, &value_tag,
            &value_length,
            NULL
        )) < 0) {
            return res;
        }
        read += (size_t)res;
        if (value_tag == tag) {
            break;
        }
    }

    if (value_tag != tag) {
        if ((res = tlv_encode(&cursor, capacity - size, tag, length, value)) < 0) {
            return res;
        }
        return size + new_encoded;
    }

    size_t old_encoded = tlv_encoded_size(value_length);
    size_t new_size  = (size_t)size - old_encoded + new_encoded;

    if (new_size > capacity) {
        return -ENOBUFS;
    }

    memmove(cursor - old_encoded + new_encoded, cursor, size - (size_t)(ptrdiff_t)(cursor - buffer));
    cursor -= old_encoded;
    ssize_t n = tlv_encode(&cursor, new_encoded, tag, length, value);
    assert(n == (ssize_t)new_encoded);
    if (n < 0) {
        return n;
    }
    return (ssize_t)new_size;
}

ssize_t tlv_remove(
    uint8_t* buffer,
    size_t size,
    uint8_t tag
) {
    uint8_t* cursor = (uint8_t*)buffer;
    ssize_t res = 0;
    size_t value_length = 0;
    uint8_t value_tag = 0;
    size_t read = size;
    while (read > 0) {
        if ((res = tlv_decode(
            (const uint8_t**)&cursor,
            read,
            &value_tag,
            &value_length,
            NULL
        )) < 0) {
            return res;
        }
        read -= (size_t)res;
        if (value_tag == tag) {
            size_t encoded = tlv_encoded_size(value_length);
            memmove(cursor - encoded, cursor, read);
            cursor -= encoded;
            size -= encoded;
        }
    }
    return size;
}
