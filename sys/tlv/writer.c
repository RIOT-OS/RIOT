/*
 * SPDX-FileCopyrightText: Carl Seifert <carl.seifert@tu-dresden.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <string.h>
#include <stddef.h>
#include "errno.h"
#include "tlv.h"
#include "byteorder.h"
#include "compiler_hints.h"

#define LOG_UNIT "tlv"
#include "log.h"

ssize_t tlv_writer_set(
    tlv_writer_t* writer,
    tlv_tag_t tag,
    size_t length,
    const uint8_t* value
) {
    assert(writer);
    assert(writer->buffer);
    assert(writer->ops);
    assert(writer->capacity >= writer->size);
    uint8_t* cursor = writer->buffer;

    ssize_t res = 0;
    size_t value_length = 0;
    tlv_tag_t value_tag = 0;
    size_t read = 0;
    uint8_t* value_og = NULL;
    while (read < writer->size) {
        if ((res = writer->ops->decoder(
            cursor,
            writer->size - read,
            &value_tag, &value_length, (const uint8_t**)&value_og
        )) < 0) {
            return res;
        }
        read += (size_t)res;
        cursor += (ptrdiff_t)res;
        if (value_tag == tag) {
            break;
        }
    }

    if (value_tag != tag) {
        if ((res = writer->ops->encoder(
            writer->buffer + writer->size, 
            writer->capacity - writer->size, 
            tag, length, value)) < 0
        ) {
            return res;
        }
        writer->size += (size_t)res;
        return writer->size;
    }

    if (likely(value_length == length)) {
        /* If the value length doesn't change, just copy. */
        memcpy(value_og, value, value_length);
        return writer->size;
    }

    size_t old_encoded = (size_t)res;
    size_t new_encoded = writer->ops->sizer(tag, length);

    size_t new_size  = (size_t)writer->size - old_encoded + new_encoded;
    if (new_size > writer->capacity) {
        return -ENOBUFS;
    }

    memmove(
        /* End of updated record: */
        cursor - old_encoded + new_encoded,
        /* Start of next TLV record: */
        cursor,
        /* Size of remainder: */
        writer->size - (size_t)(ptrdiff_t)(cursor - writer->buffer)
    );
    cursor -= old_encoded;
    ssize_t n = writer->ops->encoder(cursor, new_encoded, tag, length, value);
    assert(n == (ssize_t)new_encoded);
    if (n < 0) {
        return n;
    }
    writer->size = new_size;
    return new_size;
}

ssize_t tlv_writer_remove(
    tlv_writer_t* writer,
    tlv_tag_t tag
) {
    assert(writer);
    assert(writer->buffer);
    assert(writer->capacity >= writer->size);

    uint8_t* cursor = writer->buffer;
    ssize_t res = 0;
    tlv_tag_t value_tag = 0;
    size_t remainder = writer->size;
    while (remainder > 0) {
        if ((res = writer->ops->decoder(
            cursor,
            remainder,
            &value_tag, NULL,NULL)) < 0
        ) {
            return res;
        }
        remainder -= (size_t)res;
        if (value_tag == tag) {
            size_t encoded = (size_t)res;
            memmove(cursor, cursor + encoded, remainder);
            writer->size -= encoded;
        } else {
            cursor += (ptrdiff_t)res;
        }
    }
    return writer->size;
}
