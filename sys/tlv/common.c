/*
 * SPDX-FileCopyrightText: Carl Seifert <carl.seifert@tu-dresden.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <string.h>
#include <stddef.h>
#include "errno.h"
#include "tlv.h"
#include "byteorder.h"

#define LOG_UNIT "tlv"
#include "log.h"

ssize_t _tlv_find(
    const uint8_t* buffer,
    size_t size,
    tlv_tag_t tag,
    const uint8_t** value,
    _tlv_decoder_t decoder
) {
    ssize_t res = 0;
    size_t value_length = 0;
    tlv_tag_t tag_found = 0;
    const uint8_t* _value = NULL;
    while (size > 0) {
        if ((res = decoder(buffer, size, &tag_found, &value_length, &_value)) < 0) {
            return res;
        }
        size -= (size_t)res;
        buffer += (ptrdiff_t)res;
        if (tag_found == tag) {
            *value = _value;
            return (ssize_t)value_length;
        }
    }
    return -ENOENT;
}
