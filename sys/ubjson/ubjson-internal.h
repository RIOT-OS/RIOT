/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef UBJSON__INTERNAL_H__
#define UBJSON__INTERNAL_H__

/* compare http://ubjson.org/type-reference/ */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UBJSON_MARKER_NULL         = 'Z',
    UBJSON_MARKER_NOOP         = 'N',

    UBJSON_MARKER_TRUE         = 'T',
    UBJSON_MARKER_FALSE        = 'F',

    UBJSON_MARKER_INT8         = 'i',
    UBJSON_MARKER_UINT8        = 'U',
    UBJSON_MARKER_INT16        = 'I',
    UBJSON_MARKER_INT32        = 'l',
    UBJSON_MARKER_INT64        = 'L',

    UBJSON_MARKER_FLOAT32      = 'd',
    UBJSON_MARKER_FLOAT64      = 'D',
    UBJSON_MARKER_HP_NUMBER    = 'H',

    UBJSON_MARKER_CHAR         = 'C',
    UBJSON_MARKER_STRING       = 'S',

    UBJSON_MARKER_ARRAY_START  = '[',
    UBJSON_MARKER_ARRAY_END    = ']',

    UBJSON_MARKER_OBJECT_START = '{',
    UBJSON_MARKER_OBJECT_END   = '}',

    UBJSON_MARKER_COUNT        = '#',
    UBJSON_MARKER_TYPE         = '$',
} ubjson_marker_t;

#ifdef __cplusplus
}
#endif

#endif /* ifndef UBJSON__INTERNAL_H__ */
