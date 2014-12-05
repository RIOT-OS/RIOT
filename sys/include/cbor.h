/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Kevin Funk <kfunk@kde.org>
 * Copyright (C) 2014 Jana Cavojska <jana.cavojska9@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cbor
 * @{
 */

/**
 * @file
 * @brief       Implementation of a CBOR serializer/deserializer in C
 *
 * @author      Kevin Funk <kfunk@kde.org>
 * @author      Jana Cavojska <jana.cavojska9@gmail.com>
 *
 * This is an implementation suited for constrained devices
 * Characteristics:
 * - No dynamic memory allocation (i.e. no calls to @e malloc, @e free) used
 *   throughout the implementation
 * - User may allocate static buffers, this implementation uses the space
 *   provided by them (cf. @ref cbor_stream_t)
 *
 * @par Supported types (categorized by major type (MT)):
 *
 * - Major type 0 (unsigned integer): Full support. Relevant functions:
 *   - cbor_serialize_int(), cbor_deserialize_int()
 *   - cbor_serialize_uint64_t(), cbor_deserialize_uint64_t()
 *
 * - Major type 1 (negative integer): Full support. Relevant functions:
 *   - cbor_serialize_int(), cbor_deserialize_int()
 *   - cbor_serialize_int64_t(), cbor_deserialize_int64_t()
 *
 * - Major type 2 (byte string): Full support. Relevant functions:
 *   - cbor_serialize_byte_string(), cbor_deserialize_byte_string()
 *
 * - Major type 3 (unicode string): Basic support (see below). Relevant functions:
 *   - cbor_serialize_unicode_string(), cbor_deserialize_unicode_string()
 *
 * - Major type 4 (array of data items): Full support. Relevant functions:
 *   - cbor_serialize_array(), cbor_deserialize_array()
 *   - cbor_serialize_indefinite_array(), cbor_deserialize_indefinite_array(),
 *     cbor_at_break()
 *
 * - Major type 5 (map of pairs of data items): Full support. Relevant functions:
 *   - cbor_serialize_map(), cbor_deserialize_map()
 *   - cbor_serialize_indefinite_map(), cbor_deserialize_indefinite_map(),
 *     cbor_at_break()
 *
 * - Major type 6 (optional semantic tagging of other major types): Basic
 *   support (see below). Relevant functions:
 *   - cbor_write_tag()
 *   - cbor_deserialize_date_time()
 *   - cbor_serialize_date_time()
 *
 * - Major type 7 (floating-point numbers and values with no content): Basic
 *   support (see below). Relevant functions:
 *   - cbor_serialize_float_half(), cbor_deserialize_float_half()
 *   - cbor_serialize_float(), cbor_deserialize_float()
 *   - cbor_serialize_double(), cbor_deserialize_double()
 *   - cbor_serialize_bool(), cbor_deserialize_bool()
 *
 * @par Notes about major type 3:
 * Since we do not have a standardised C type for representing Unicode code
 * points, we just provide API to serialize/deserialize @e char* arrays. The
 * user then has to transform that into a meaningful representation
 *
 * @par Notes about major type 6 (cf. https://tools.ietf.org/html/rfc7049#section-2.4):
 * Encoding date and time: date/time strings that follow the standard format
 * described in Section 3.3 of [RFC3339]:
 *   2003-12-13T18:30:02Z          - supported
 *   2003-12-13T18:30:02.25Z       - not supported
 *   2003-12-13T18:30:02+01:00     - not supported
 *   2003-12-13T18:30:02.25+01:00  - not supported
 * Since we do not have C types for representing bignums/bigfloats/decimal-fraction
 * we do not provide API to serialize/deserialize them at all.
 * You can still read out the actual data item behind the tag (via
 * cbor_deserialize_byte_string()) and interpret it yourself.
 *
 * @par Notes about major type 7 and simple values
 *      (cf. https://tools.ietf.org/html/rfc7049#section-2.3)
 * Simple values:
 * -   0-19: (Unassigned)    - No support
 * -  20,21: True, False     - Supported (see cbor_serialize_bool(),
 *                                        cbor_deserialize_bool())
 * -  22,23: Null, Undefined - No support (what's the use-case?)
 * -  24-31: (Reserved)      - No support
 * - 32-255: (Unassigned)    - No support
 *
 * TODO: API for Indefinite-Length Byte Strings and Text Strings
 *       (see https://tools.ietf.org/html/rfc7049#section-2.2.2)
 */

#ifndef CBOR_H
#define CBOR_H

#ifndef CBOR_NO_CTIME
/* 'strptime' is only declared when this macro is defined */
#define _XOPEN_SOURCE
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef CBOR_NO_CTIME
#include <time.h>
#endif /* CBOR_NO_CTIME */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Struct containing CBOR-encoded data
 *
 * A typical usage of CBOR looks like:
 * @code
 * unsigned char data[1024];
 * cbor_stream_t stream;
 * cbor_init(&stream, data, sizeof(data));
 *
 * cbor_serialize_int(&stream, 5);
 * (...)
 * <data contains CBOR encoded items now>
 *
 * cbor_destroy(&stream);
 * @endcode
 *
 * @sa cbor_init
 * @sa cbor_clear
 * @sa cbor_destroy
 */
typedef struct cbor_stream_t {
    /** Array containing CBOR encoded data */
    unsigned char *data;
    /** Size of the array */
    size_t size;
    /** Index to the next free byte */
    size_t pos;
} cbor_stream_t;

/**
 * @brief Initialize cbor struct
 *
 * @note Does *not* take ownership of @p buffer
 * @param[in] stream The cbor struct to initialize
 * @param[in] buffer The buffer used for storing CBOR-encoded data
 * @param[in] size   The size of buffer @p buffer
 */
void cbor_init(cbor_stream_t *stream, unsigned char *buffer, size_t size);

/**
 * @brief Clear cbor struct
 *
 *        Sets pos to zero
 *
 * @param[in, out] stream Pointer to the cbor struct
 */
void cbor_clear(cbor_stream_t *stream);

/**
 * @brief Destroy the cbor struct
 *
 * @note Does *not* free data
 *
 * @param[in, out] stream Pointer to the cbor struct
 */
void cbor_destroy(cbor_stream_t *stream);

#ifndef CBOR_NO_PRINT
/**
 * @brief Print @p stream in hex representation
 *
 * @param[in] stream Pointer to the cbor struct
 */
void cbor_stream_print(const cbor_stream_t *stream);

/**
 * @brief Decode CBOR from @p stream
 *
 * This method interprets the data and prints each item in its natural
 * representation
 *
 * Example output:
 * @code
 * Data:
 * (int, 1)
 * (bool, 1)
 * (float, 1.099609)
 * (tag: 0, date/time string: "Mon Jul 14 19:07:40 2014")
 * (tag: 1, date/time epoch: 1405357660)
 * @endcode
 *
 * @param[in] stream Pointer to the cbor struct
 */
void cbor_stream_decode(cbor_stream_t *stream);
#endif /* CBOR_NO_PRINT */

size_t cbor_serialize_int(cbor_stream_t *s, int val);
size_t cbor_deserialize_int(const cbor_stream_t *stream, size_t offset,
                            int *val);
size_t cbor_serialize_uint64_t(cbor_stream_t *s, uint64_t val);
size_t cbor_deserialize_uint64_t(const cbor_stream_t *stream, size_t offset,
                                 uint64_t *val);
size_t cbor_serialize_int64_t(cbor_stream_t *s, int64_t val);
size_t cbor_deserialize_int64_t(const cbor_stream_t *stream, size_t offset,
                                int64_t *val);
size_t cbor_serialize_bool(cbor_stream_t *s, bool val);
size_t cbor_deserialize_bool(const cbor_stream_t *stream, size_t offset,
                             bool *val);
#ifndef CBOR_NO_FLOAT
size_t cbor_serialize_float_half(cbor_stream_t *s, float val);
size_t cbor_deserialize_float_half(const cbor_stream_t *stream, size_t offset,
                                   float *val);
size_t cbor_serialize_float(cbor_stream_t *s, float val);
size_t cbor_deserialize_float(const cbor_stream_t *stream, size_t offset,
                              float *val);
size_t cbor_serialize_double(cbor_stream_t *s, double val);
size_t cbor_deserialize_double(const cbor_stream_t *stream, size_t offset,
                               double *val);
#endif /* CBOR_NO_FLOAT */

size_t cbor_serialize_byte_string(cbor_stream_t *s, const char *val);

/**
 * @brief Deserialize bytes from @p stream to @p val
 *
 * @param[in] stream The stream to deserialize
 * @param[in] offset The offset within the stream where to start deserializing
 * @param[in] val    Pointer to destination array
 * @param[in] length Length of destination array
 *
 * @return Number of bytes written into @p val
 */
size_t cbor_deserialize_byte_string(const cbor_stream_t *stream, size_t offset,
                                    char *val, size_t length);

size_t cbor_serialize_unicode_string(cbor_stream_t *stream, const char *val);

/**
 * @brief Deserialize unicode string from @p stream to @p val
 *
 * @param[in] stream The stream to deserialize
 * @param[in] offset The offset within the stream where to start deserializing
 * @param[in] val    Pointer to destination array
 * @param[in] length Length of destination array
 *
 * @return Number of bytes written into @p val
 */
size_t cbor_deserialize_unicode_string(const cbor_stream_t *stream,
                                       size_t offset, char *val, size_t length);

/**
 * @brief Serialize array of length @p array_length
 *
 * Basic usage:
 * @code
 * cbor_serialize_array(&stream, 2); // array of length 2 follows
 * cbor_serialize_int(&stream, 1)); // write item 1
 * cbor_serialize_int(&stream, 2)); // write item 2
 * @endcode
 *
 * @note You have to make sure to serialize the correct amount of items.
 * If you exceed the length @p array_length, items will just be appened as normal
 *
 * @param[out] stream       The destination stream for serializing the array
 * @param[in]  array_length Length of the array of items which follows
 *
 * @return Number of bytes written to stream @p stream
 */
size_t cbor_serialize_array(cbor_stream_t *stream, size_t array_length);

/**
 * @brief Deserialize array of items
 *
 * Basic usage:
 * @code
 * size_t array_length;
 * // read out length of the array
 * size_t offset = cbor_deserialize_array(&stream, 0, &array_length);
 * int i1, i2;
 * offset += cbor_deserialize_int(&stream, offset, &i1); // read item 1
 * offset += cbor_deserialize_int(&stream, offset, &i2); // read item 2
 * @endcode
 *
 * @param[in] stream       The stream to deserialize
 * @param[in] offset       The offset within the stream
 * @param[in] array_length Where the array length is stored
 *
 * @return Number of deserialized bytes from stream
 */
size_t cbor_deserialize_array(const cbor_stream_t *stream, size_t offset,
                              size_t *array_length);

size_t cbor_serialize_array_indefinite(cbor_stream_t *stream);

size_t cbor_deserialize_array_indefinite(const cbor_stream_t *stream, size_t offset);

/**
 * @brief Serialize map of length @p map_length
 *
 * Basic usage:
 * @code
 * cbor_serialize_map(&stream, 2); // map of length 2 follows
 * cbor_serialize_int(&stream, 1)); // write key 1
 * cbor_serialize_byte_string(&stream, "1")); // write value 1
 * cbor_serialize_int(&stream, 2)); // write key 2
 * cbor_serialize_byte_string(&stream, "2")); // write value 2
 * @endcode
 *
 * @param[out] stream  The destination stream for serializing the map
 * @param map_length Length of the map of items which follows
 *
 * @return Number of bytes written to stream @p stream
 */
size_t cbor_serialize_map(cbor_stream_t *stream, size_t map_length);

/**
 * @brief Deserialize map of items
 *
 * Basic usage:
 * @code
 * size_t map_length;
 * // read out length of the map
 * size_t offset = cbor_deserialize_map(&stream, 0, &map_length);
 * int key1, key1;
 * char value1[8], value2[8];
 * // read key 1
 * offset += cbor_deserialize_int(&stream, offset, &key1);
 * // read value 1
 * offset += cbor_deserialize_byte_string(&stream, offset, value1, sizeof(value));
 * // read key 2
 * offset += cbor_deserialize_int(&stream, offset, &key2);
 * // read value 2
 * offset += cbor_deserialize_byte_string(&stream, offset, value2, sizeof(value));
 * @endcode
 *
 * @param[in] stream     The stream to deserialize
 * @param[in] offset     The offset within the stream where to start deserializing
 * @param[in] map_length Where the array length is stored
 *
 * @return Number of deserialized bytes from @p stream
 */
size_t cbor_deserialize_map(const cbor_stream_t *stream, size_t offset,
                            size_t *map_length);

size_t cbor_serialize_map_indefinite(cbor_stream_t *stream);

size_t cbor_deserialize_map_indefinite(const cbor_stream_t *stream, size_t offset);

#ifndef CBOR_NO_SEMANTIC_TAGGING
#ifndef CBOR_NO_CTIME
/**
 * @brief Serialize date and time
 *
 * Basic usage:
 * @code
 * struct tm val;
 * val.tm_year = 114;
 * val.tm_mon = 6;
 * val.tm_mday = 1;
 * val.tm_hour = 15;
 * val.tm_min = 0;
 * val.tm_sec = 0;
 * mktime(&val);
 * cbor_serialize_date_time(&stream, &val);
 * @endcode
 *
 * @param[out] stream  The destination stream for serializing the date_time
 * @param[in] val      tm struct containing the date/time info to be encoded
 *
 * @return Number of bytes written to stream @p stream
 */
size_t cbor_serialize_date_time(cbor_stream_t *stream, struct tm *val);

/**
 * @brief Deserialize date and time
 *
 * Basic usage:
 * @code
 * struct tm val;
 * cbor_deserialize_date_time(&stream, 0, &val);
 * @endcode
 *
 * @param[in] stream The stream to deserialize
 * @param[in] offset The offset within the stream where to start deserializing
 * @param[in] val    tm struct where the decoded date/time will be stored
 *
 * @return The number of deserialized bytes
 */
size_t cbor_deserialize_date_time(const cbor_stream_t *stream, size_t offset, struct tm *val);

size_t cbor_serialize_date_time_epoch(cbor_stream_t *stream, time_t val);

size_t cbor_deserialize_date_time_epoch(const cbor_stream_t *stream, size_t offset, time_t *val);

#endif /* CBOR_NO_CTIME */

/**
 * @brief Write a tag to give the next CBOR item additional semantics
 *
 * Also see https://tools.ietf.org/html/rfc7049#section-2.4 (Optional Tagging of Items)
 *
 * @param[in, out] stream Pointer to the cbor struct
 * @param[in]      tag    The tag to write
 *
 * @return Always 1
 */
size_t cbor_write_tag(cbor_stream_t *stream, unsigned char tag);

/**
 * @brief Whether we are at a tag symbol in stream @p stream at offset @p offset
 *
 * @param[in] stream Pointer to the cbor struct
 * @param[in] offset The offset within @p stream
 *
 * @return True in case there is a tag symbol at the current offset
 */
bool cbor_at_tag(const cbor_stream_t *stream, size_t offset);

#endif /* CBOR_NO_SEMANTIC_TAGGING */

/**
 * @brief Write a break symbol at the current offset in stream @p stream
 *
 * Used for marking the end of indefinite length CBOR items
 *
 * @param[in] stream  Pointer to the cbor struct
 *
 * @return Always 1
 */
size_t cbor_write_break(cbor_stream_t *stream);

/**
 * @brief Whether we are at a break symbol in stream @p stream at offset @p offset
 *
 * @param[in] stream  Pointer to the cbor struct
 * @param[in] offset  The offset within @p stream
 *
 * @return True in case the there is a break symbol at the current offset
 */
bool cbor_at_break(const cbor_stream_t *stream, size_t offset);

/**
 * @brief Whether we are at the end of the stream @p stream at offset @p offset
 *
 * Useful for abort conditions in loops while deserializing CBOR items
 *
 * @param[in] stream  Pointer to the cbor struct
 * @param[in] offset  The offset within @p stream
 *
 * @return True in case @p offset marks the end of the stream
 */
bool cbor_at_end(const cbor_stream_t *stream, size_t offset);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
