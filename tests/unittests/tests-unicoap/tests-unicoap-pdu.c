/*
 * SPDX-FileCopyrightText: 2024-2025 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup unittests
 * @brief   Unit tests for PDU parsers
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#include <stdio.h>
#include <string.h>

#include "tests-unicoap.h"

#include "net/unicoap/message.h"
#include "net/unicoap/options.h"

#define _TEST_ASSERT_EQUAL_PARSED(parsed, code, payload, token) \
    test_parsed_message((parsed), (uint8_t)code, static_strlen(payload), (uint8_t*)payload, \
    sizeof(token), token)

#define _TEST_ASSERT_EQUAL_PARSED_RFC7252(parsed, code, payload, token, message_type, message_id) \
    _TEST_ASSERT_EQUAL_PARSED((parsed), (code), (payload), (token));                              \
    TEST_ASSERT_EQUAL_INT((parsed)->properties.rfc7252.id, (message_id));                         \
    TEST_ASSERT_EQUAL_INT((parsed)->properties.rfc7252.type, (message_type))

static void test_parsed_message(unicoap_parser_result_t* parsed, uint8_t code, size_t payload_size,
                                const uint8_t* payload, uint8_t token_length, uint8_t* token)
{
    TEST_ASSERT_EQUAL_INT(parsed->message.code, code);
    TEST_ASSERT_EQUAL_INT(parsed->message.payload_size, payload_size);

    if (payload_size > 0) {
        TEST_ASSERT_NOT_NULL(parsed->message.payload);
        _TEST_ASSERT_EQUAL_BYTES(parsed->message.payload, payload, payload_size);
    }

    TEST_ASSERT_EQUAL_INT(parsed->properties.token_length, token_length);
    if (token_length > 0) {
        TEST_ASSERT_NOT_NULL(parsed->properties.token);
        _TEST_ASSERT_EQUAL_BYTES(parsed->properties.token, token, token_length);
    }

    TEST_ASSERT_NOT_NULL(parsed->message.options);
}

static void test_pdu_rfc7252_actuators_round_trip(void)
{
    /* In this order:
     UDP
     Message:
     {
       "type": "Confirmable",
       "code": "POST",
       "id": 65201,
       "token": 0,
       "options": [
         "Uri-Path: actuators",
         "Uri-Path: leds",
         "Content-Format: application/json",
         "Uri-Query: color=g",
         "Accept: application/json"
       ]
     }
     Payload (JSON):
     6D 6F 64 65 3D 6F 6E
     */
    uint8_t pdu[] = {
        0x40, 0x02, 0xfe, 0xb1, 0xb9, 0x61, 0x63, 0x74, 0x75, 0x61, 0x74, 0x6f, 0x72, 0x73, 0x04,
        0x6c, 0x65, 0x64, 0x73, 0x11, 0x32, 0x37, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x3d, 0x67, 0x21,
        0x32, 0xff, 0x6d, 0x6f, 0x64, 0x65, 0x3d, 0x6f, 0x6e
    };

    unicoap_parser_result_t parsed = { 0 };
    TEST_ASSERT_EQUAL_INT(unicoap_pdu_parse_rfc7252_result(pdu, sizeof(pdu), &parsed), 0);

    _TEST_ASSERT_EQUAL_PARSED_RFC7252(&parsed,
                                      UNICOAP_METHOD_POST,
                                      /* payload */
                                      "mode=on",
                                      /* token */
                                      _BYTES(),
                                      UNICOAP_TYPE_CON, 65201);

    TEST_ASSERT_EQUAL_INT(parsed.options.option_count, 5);

    unicoap_content_format_t format = 0;
    unicoap_options_get_content_format(&parsed.options, &format);
    TEST_ASSERT_EQUAL_INT(format, UNICOAP_FORMAT_JSON);

    format = 0;
    unicoap_options_get_accept(&parsed.options, &format);
    TEST_ASSERT_EQUAL_INT(format, UNICOAP_FORMAT_JSON);

    char path[30];
    TEST_ASSERT_EQUAL_INT(unicoap_options_copy_uri_path(&parsed.options, path, sizeof(path)),
                          static_strlen("/actuators/leds"));
    _TEST_ASSERT_EQUAL_BYTES_STRING(path, "/actuators/leds");

    unicoap_options_iterator_t iterator = { 0 };
    const char* component = NULL;
    unicoap_options_iterator_init(&iterator, &parsed.options);

    TEST_ASSERT_EQUAL_INT(unicoap_options_get_next_uri_path_component(&iterator, &component),
                          static_strlen("actuators"));
    _TEST_ASSERT_EQUAL_BYTES(component, "actuators", static_strlen("actuators"));

    TEST_ASSERT_EQUAL_INT(unicoap_options_get_next_uri_path_component(&iterator, &component),
                          static_strlen("leds"));
    _TEST_ASSERT_EQUAL_BYTES(component, "leds", static_strlen("leds"));

    char queries[30];
    TEST_ASSERT_EQUAL_INT(unicoap_options_copy_uri_queries(&parsed.options, queries,
                                                           sizeof(queries)),
                          static_strlen("color=g"));
    _TEST_ASSERT_EQUAL_BYTES_STRING(queries, "color=g");

    unicoap_options_iterator_init(&iterator, &parsed.options);

    TEST_ASSERT_EQUAL_INT(unicoap_options_get_next_uri_query(&iterator, &component),
                          static_strlen("color=g"));
    _TEST_ASSERT_EQUAL_BYTES(component, "color=g", static_strlen("color=g"));
    TEST_ASSERT_EQUAL_INT(unicoap_options_get_next_uri_query(&iterator, &component), -1);

    uint8_t pdu_copy[sizeof(pdu) + 100] = { 0 };
    TEST_ASSERT_EQUAL_INT(unicoap_pdu_build_rfc7252(pdu_copy, sizeof(pdu_copy),
                                                    &parsed.message, &parsed.properties),
                          sizeof(pdu));
    _TEST_ASSERT_EQUAL_BYTES(pdu, pdu_copy, sizeof(pdu));

    /* Try to build a vector */

    memset(pdu_copy, 0, sizeof(pdu_copy));

    iolist_t iolist[UNICOAP_PDU_IOLIST_COUNT];
    uint8_t header[UNICOAP_HEADER_SIZE_MAX];

    TEST_ASSERT_EQUAL_INT(unicoap_pdu_buildv_rfc7252(header, sizeof(header), &parsed.message,
                                                     &parsed.properties, iolist),
                          0);
    TEST_ASSERT_EQUAL_INT(iolist_to_buffer(iolist, pdu_copy, sizeof(pdu_copy)), sizeof(pdu));
    _TEST_ASSERT_EQUAL_BYTES(pdu, pdu_copy, sizeof(pdu));
}

static void test_pdu_rfc7252_method_not_allowed_ack_round_trip(void)
{
    /*
     UDP
     Message:
     {
       "type": "Acknowledgement",
       "code": "MethodNotAllowed",
       "id": 3132,
       "token": 3516372673,
       "options": []
     }
     */
    /* from https://wiki.wireshark.org/samplecaptures */
    const uint8_t pdu[] = { 0x64, 0x85, 0x0c, 0x3c, 0xd1, 0x97, 0x96, 0xc1 };

    unicoap_parser_result_t parsed = { 0 };
    TEST_ASSERT_EQUAL_INT(unicoap_pdu_parse_rfc7252_result((uint8_t*)pdu, sizeof(pdu), &parsed), 0);

    _TEST_ASSERT_EQUAL_PARSED_RFC7252(&parsed,
                                      UNICOAP_STATUS_METHOD_NOT_ALLOWED,
                                      "",
                                      _BYTES(0xd1, 0x97, 0x96, 0xc1),
                                      UNICOAP_TYPE_ACK, 3132);

    char path[30];
    TEST_ASSERT_EQUAL_INT(unicoap_options_copy_uri_path(&parsed.options, path, sizeof(path)),
                          static_strlen("/"));
    _TEST_ASSERT_EQUAL_BYTES_STRING(path, "/");

    uint8_t pdu_copy[sizeof(pdu) + 100] = { 0 };
    TEST_ASSERT_EQUAL_INT(unicoap_pdu_build_rfc7252(pdu_copy, sizeof(pdu_copy),
                                                    &parsed.message, &parsed.properties),
                          sizeof(pdu));
    _TEST_ASSERT_EQUAL_BYTES(pdu, pdu_copy, sizeof(pdu));

    /* Try to build a vector */

    memset(pdu_copy, 0, sizeof(pdu_copy));

    iolist_t iolist[UNICOAP_PDU_IOLIST_COUNT];
    uint8_t header[UNICOAP_HEADER_SIZE_MAX];

    TEST_ASSERT_EQUAL_INT(unicoap_pdu_buildv_rfc7252(header, sizeof(header), &parsed.message,
                                                     &parsed.properties, iolist),
                          0);
    TEST_ASSERT_EQUAL_INT(iolist_to_buffer(iolist, pdu_copy, sizeof(pdu_copy)), sizeof(pdu));
    _TEST_ASSERT_EQUAL_BYTES(pdu, pdu_copy, sizeof(pdu));
}

static void test_pdu_rfc7252_cbor_request_round_trip(void)
{
    /*
     UDP
     Message:
     {
       "type": "Confirmable",
       "code": "POST",
       "id": 3134,
       "token": 3516372675,
       "options": [
         "Content-Format: application/cbor"
       ]
     }
     */
    /* from https://wiki.wireshark.org/samplecaptures */
    const uint8_t pdu[] = {
        0x44, 0x02, 0x0c, 0x3e, 0xd1, 0x97, 0x96, 0xc3, 0xc1, 0x3c, 0xff, 0x0a
    };

    unicoap_parser_result_t parsed = { 0 };
    TEST_ASSERT_EQUAL_INT(unicoap_pdu_parse_rfc7252_result((uint8_t*)pdu, sizeof(pdu), &parsed), 0);

    _TEST_ASSERT_EQUAL_PARSED_RFC7252(&parsed,
                                      UNICOAP_METHOD_POST,
                                      "\x0a",
                                      _BYTES(0xd1, 0x97, 0x96, 0xc3),
                                      UNICOAP_TYPE_CON, 3134);

    uint8_t pdu_copy[sizeof(pdu) + 100] = { 0 };
    TEST_ASSERT_EQUAL_INT(unicoap_pdu_build_rfc7252(pdu_copy, sizeof(pdu_copy), &parsed.message,
                                                    &parsed.properties),
                          sizeof(pdu));
    _TEST_ASSERT_EQUAL_BYTES(pdu, pdu_copy, sizeof(pdu));

    /* Try to build a vector */

    memset(pdu_copy, 0, sizeof(pdu_copy));

    iolist_t iolist[UNICOAP_PDU_IOLIST_COUNT];
    uint8_t header[UNICOAP_HEADER_SIZE_MAX];

    TEST_ASSERT_EQUAL_INT(unicoap_pdu_buildv_rfc7252(header, sizeof(header),
                                                     &parsed.message, &parsed.properties, iolist),
                          0);
    TEST_ASSERT_EQUAL_INT(iolist_to_buffer(iolist, pdu_copy, sizeof(pdu_copy)), sizeof(pdu));
    _TEST_ASSERT_EQUAL_BYTES(pdu, pdu_copy, sizeof(pdu));
}

Test* tests_unicoap_pdu(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(test_pdu_rfc7252_actuators_round_trip),
        new_TestFixture(test_pdu_rfc7252_method_not_allowed_ack_round_trip),
        new_TestFixture(test_pdu_rfc7252_cbor_request_round_trip),
    };

    EMB_UNIT_TESTCALLER(test_unicoap, NULL, NULL, fixtures);

    return (Test*)&test_unicoap;
}
