/*
 * Copyright (c) 2018 Ken Bannister. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "embUnit.h"

#include "net/nanocoap.h"

#include "unittests-constants.h"
#include "tests-nanocoap.h"


#define _BUF_SIZE (128U)

/*
 * Validates encoded message ID byte order and put/get URI option.
 */
static void test_nanocoap__hdr(void)
{
    uint8_t buf[_BUF_SIZE];
    uint16_t msgid = 0xABCD;
    char path[] = "/test/abcd/efgh";
    char loc_path[] = "/foo/bar";
    unsigned char path_tmp[64] = {0};

    uint8_t *pktpos = &buf[0];
    pktpos += coap_build_hdr((coap_hdr_t *)pktpos, COAP_TYPE_CON, NULL, 0,
                             COAP_METHOD_GET, msgid);
    pktpos += coap_opt_put_location_path(pktpos, 0, loc_path);
    pktpos += coap_opt_put_uri_path(pktpos, COAP_OPT_LOCATION_PATH, path);

    coap_pkt_t pkt;
    coap_parse(&pkt, &buf[0], pktpos - &buf[0]);

    TEST_ASSERT_EQUAL_INT(msgid, coap_get_id(&pkt));

    int res = coap_get_uri_path(&pkt, path_tmp);
    TEST_ASSERT_EQUAL_INT(sizeof(path), res);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)path_tmp);

    res = coap_get_location_path(&pkt, path_tmp, 64);
    TEST_ASSERT_EQUAL_INT(sizeof(loc_path), res);
    TEST_ASSERT_EQUAL_STRING((char *)loc_path, (char *)path_tmp);
}

/*
 * Client GET request with simple path. Test request generation.
 * Request /time resource from libcoap example
 */
static void test_nanocoap__get_req(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/time";
    size_t total_hdr_len = 6;
    size_t total_opt_len = 5;

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);
    TEST_ASSERT_EQUAL_INT(total_hdr_len, len);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    TEST_ASSERT_EQUAL_INT(COAP_METHOD_GET, coap_get_code(&pkt));
    TEST_ASSERT_EQUAL_INT(2, coap_get_token_len(&pkt));
    TEST_ASSERT_EQUAL_INT(total_hdr_len, coap_get_total_hdr_len(&pkt));
    TEST_ASSERT_EQUAL_INT(COAP_TYPE_NON, coap_get_type(&pkt));
    TEST_ASSERT_EQUAL_INT(122, pkt.payload_len);

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(total_opt_len, len);

    char uri[10] = {0};
    coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);

    len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    TEST_ASSERT_EQUAL_INT(total_hdr_len + total_opt_len, len);
}

/*
 * Builds on get_req test, to test payload and Content-Format option.
 */
static void test_nanocoap__put_req(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/value";
    size_t total_hdr_len = 6;
    size_t uri_opt_len = 6;
    size_t fmt_opt_len = 1;

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_PUT, msgid);
    TEST_ASSERT_EQUAL_INT(total_hdr_len, len);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    TEST_ASSERT_EQUAL_INT(COAP_METHOD_PUT, coap_get_code(&pkt));
    TEST_ASSERT_EQUAL_INT(122, pkt.payload_len);

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(uri_opt_len, len);

    len = coap_opt_add_uint(&pkt, COAP_OPT_CONTENT_FORMAT, COAP_FORMAT_TEXT);
    TEST_ASSERT_EQUAL_INT(fmt_opt_len, len);
    TEST_ASSERT_EQUAL_INT(COAP_FORMAT_TEXT, coap_get_content_type(&pkt));

    len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);
    TEST_ASSERT_EQUAL_INT(total_hdr_len + uri_opt_len + fmt_opt_len + 1, len);
    TEST_ASSERT_EQUAL_INT(0xFF, *(pkt.payload - 1));
    TEST_ASSERT_EQUAL_INT(&buf[0] + _BUF_SIZE - pkt.payload, pkt.payload_len);
}

/*
 * Builds on get_req test, to test path with multiple segments.
 */
static void test_nanocoap__get_multi_path(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/ab/cde";
    size_t uri_opt_len = 7;

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(uri_opt_len, len);

    char uri[10] = {0};
    coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);
}

/*
 * Builds on get_req test, to test path with trailing slash.
 */
static void test_nanocoap__get_path_trailing_slash(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/time/";
    size_t uri_opt_len = 6;

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(uri_opt_len, len);

    char uri[10] = {0};
    coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);
}
/*
 * Builds on get_req test, to test '/' path. This path is the default when
 * otherwise not specified.
 */
static void test_nanocoap__get_root_path(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/";

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    char uri[10] = {0};
    coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);
}

/*
 * Builds on get_req test, to test max length path.
 */
static void test_nanocoap__get_max_path(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/23456789012345678901234567890123456789012345678901234567890123";
    /* includes extra byte for option length > 12 */
    size_t uri_opt_len = 64;

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(uri_opt_len, len);

    char uri[NANOCOAP_URI_MAX] = {0};
    coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);
}

/*
 * Builds on get_req test, to test path longer than NANOCOAP_URI_MAX. We
 * expect coap_get_uri_path() to return -ENOSPC.
 */
static void test_nanocoap__get_path_too_long(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/234567890123456789012345678901234567890123456789012345678901234";
    /* includes extra byte for option length > 12 */
    size_t uri_opt_len = 65;

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(uri_opt_len, len);

    char uri[NANOCOAP_URI_MAX] = {0};
    int get_len = coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_INT(-ENOSPC, get_len);
}

/*
 * Helper for server_get tests below.
 * GET Request for nanocoap server example /riot/value resource.
 * Includes 2-byte token; non-confirmable.
 * Generated with libcoap.
 */
static int _read_riot_value_req(coap_pkt_t *pkt, uint8_t *buf)
{
    uint8_t pkt_data[] = {
        0x52, 0x01, 0x9e, 0x6b, 0x35, 0x61, 0xb4, 0x72,
        0x69, 0x6f, 0x74, 0x05, 0x76, 0x61, 0x6c, 0x75,
        0x65
    };
    memcpy(buf, pkt_data, sizeof(pkt_data));

    return coap_parse(pkt, buf, sizeof(pkt_data));
}

/* Server GET request success case. */
static void test_nanocoap__server_get_req(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    char path[] = "/riot/value";

    int res = _read_riot_value_req(&pkt, &buf[0]);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(COAP_METHOD_GET, coap_get_code(&pkt));
    TEST_ASSERT_EQUAL_INT(2, coap_get_token_len(&pkt));
    TEST_ASSERT_EQUAL_INT(4 + 2, coap_get_total_hdr_len(&pkt));
    TEST_ASSERT_EQUAL_INT(COAP_TYPE_NON, coap_get_type(&pkt));
    TEST_ASSERT_EQUAL_INT(0, pkt.payload_len);

    char uri[64] = {0};
    coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);
}

/* Response for server GET request using coap_reply_simple(). */
static void test_nanocoap__server_reply_simple(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    char *payload = "0";

    int res = _read_riot_value_req(&pkt, &buf[0]);

    coap_reply_simple(&pkt, COAP_CODE_CONTENT, buf, _BUF_SIZE,
                      COAP_FORMAT_TEXT, (uint8_t *)payload, 1);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(COAP_CODE_CONTENT, coap_get_code_raw(&pkt));
    TEST_ASSERT_EQUAL_INT(2, coap_get_token_len(&pkt));
    TEST_ASSERT_EQUAL_INT(4 + 2, coap_get_total_hdr_len(&pkt));
    TEST_ASSERT_EQUAL_INT(COAP_TYPE_NON, coap_get_type(&pkt));
}

/*
 * Helper for server_get tests below.
 * GET request for nanocoap server example /riot/value resource.
 * Includes 2-byte token; confirmable.
 * Generated with libcoap.
 */
static int _read_riot_value_req_con(coap_pkt_t *pkt, uint8_t *buf)
{
    uint8_t pkt_data[] = {
        0x42, 0x01, 0xbe, 0x16, 0x35, 0x61, 0xb4, 0x72,
        0x69, 0x6f, 0x74, 0x05, 0x76, 0x61, 0x6c, 0x75,
        0x65
    };
    memcpy(buf, pkt_data, sizeof(pkt_data));

    return coap_parse(pkt, buf, sizeof(pkt_data));
}

/* Builds on test_nanocoap__server_get_req to test confirmable request. */
static void test_nanocoap__server_get_req_con(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;

    int res = _read_riot_value_req_con(&pkt, &buf[0]);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(COAP_METHOD_GET, coap_get_code(&pkt));
    TEST_ASSERT_EQUAL_INT(COAP_TYPE_CON, coap_get_type(&pkt));
}

/* Builds on test_nanocoap__server_reply_simple to test confirmable request. */
static void test_nanocoap__server_reply_simple_con(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    char *payload = "0";

    _read_riot_value_req_con(&pkt, &buf[0]);

    coap_reply_simple(&pkt, COAP_CODE_CONTENT, buf, _BUF_SIZE,
                      COAP_FORMAT_TEXT, (uint8_t *)payload, 1);

    TEST_ASSERT_EQUAL_INT(COAP_TYPE_ACK, coap_get_type(&pkt));
}

Test *tests_nanocoap_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nanocoap__hdr),
        new_TestFixture(test_nanocoap__get_req),
        new_TestFixture(test_nanocoap__put_req),
        new_TestFixture(test_nanocoap__get_multi_path),
        new_TestFixture(test_nanocoap__get_path_trailing_slash),
        new_TestFixture(test_nanocoap__get_root_path),
        new_TestFixture(test_nanocoap__get_max_path),
        new_TestFixture(test_nanocoap__get_path_too_long),
        new_TestFixture(test_nanocoap__server_get_req),
        new_TestFixture(test_nanocoap__server_reply_simple),
        new_TestFixture(test_nanocoap__server_get_req_con),
        new_TestFixture(test_nanocoap__server_reply_simple_con),
    };

    EMB_UNIT_TESTCALLER(nanocoap_tests, NULL, NULL, fixtures);

    return (Test *)&nanocoap_tests;
}

void tests_nanocoap(void)
{
    TESTS_RUN(tests_nanocoap_tests());
}
/** @} */
