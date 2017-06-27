/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief    cbor stream decode test application
 *
 * @author   Pieter Willemsen <pieter.willemsen@altran.com>
 *
 * @}
 */

#include <stdio.h>
#include "cbor.h"

static unsigned char stream_data[1024];
static cbor_stream_t stream = {stream_data, sizeof(stream_data), 0};

void test_stream_decode(void)
{
    cbor_clear(&stream);

    cbor_serialize_int(&stream, 1);
    cbor_serialize_uint64_t(&stream, 2llu);
    cbor_serialize_int64_t(&stream, 3);
    cbor_serialize_int64_t(&stream, -5);
    cbor_serialize_bool(&stream, true);
    cbor_serialize_float_half(&stream, 1.1f);
    cbor_serialize_float(&stream, 1.5f);
    cbor_serialize_double(&stream, 2.0);
    cbor_serialize_byte_string(&stream, "abc");
    cbor_serialize_unicode_string(&stream, "def");

    cbor_serialize_array(&stream, 2);
    cbor_serialize_int(&stream, 0);
    cbor_serialize_int(&stream, 1);

    cbor_serialize_array_indefinite(&stream);
    cbor_serialize_int(&stream, 10);
    cbor_serialize_int(&stream, 11);
    cbor_write_break(&stream);

    cbor_serialize_map(&stream, 2);
    cbor_serialize_int(&stream, 1);
    cbor_serialize_byte_string(&stream, "1");
    cbor_serialize_int(&stream, 2);
    cbor_serialize_byte_string(&stream, "2");

    cbor_serialize_map_indefinite(&stream);
    cbor_serialize_int(&stream, 10);
    cbor_serialize_byte_string(&stream, "10");
    cbor_serialize_int(&stream, 11);
    cbor_serialize_byte_string(&stream, "11");
    cbor_write_break(&stream);

    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    cbor_serialize_date_time(&stream, timeinfo);
    cbor_serialize_date_time_epoch(&stream, rawtime);

    /* decoder should skip the tag and print 'unsupported' here */
    cbor_write_tag(&stream, 2);
    cbor_serialize_byte_string(&stream, "1");

    cbor_stream_decode(&stream);
}

int main(void)
{
    test_stream_decode();
    return 0;
}
