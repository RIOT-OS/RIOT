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

#include "embUnit.h"

#include "net/nanocoap.h"

#include "unittests-constants.h"
#include "tests-nanocoap.h"

/*
 * Validates encoded message ID byte order and put/get URI option.
 */
static void test_nanocoap__hdr(void)
{
    uint8_t buf[128];
    uint16_t msgid = 0xABCD;
    char path[] = "/test/abcd/efgh";
    unsigned char path_tmp[64] = {0};

    uint8_t *pktpos = &buf[0];
    pktpos += coap_build_hdr((coap_hdr_t *)pktpos, COAP_REQ, NULL, 0, COAP_METHOD_GET, msgid);
    pktpos += coap_put_option_uri(pktpos, 0, path, COAP_OPT_URI_PATH);

    coap_pkt_t pkt;
    coap_parse(&pkt, &buf[0], pktpos - &buf[0]);

    TEST_ASSERT_EQUAL_INT(msgid, coap_get_id(&pkt));

    int res = coap_get_uri(&pkt, path_tmp);
    TEST_ASSERT_EQUAL_INT(sizeof(path), res);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)path_tmp);
}

/*
 * Client GET request with simple path. Test request generation.
 * Request /time resource from libcoap example
 */
static void test_nanocoap__get_req(void)
{
    uint8_t buf[128];
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
    coap_get_uri(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);

    len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    TEST_ASSERT_EQUAL_INT(total_hdr_len + total_opt_len, len);
}

/*
 * Builds on get_req test, to test payload and Content-Format option.
 */
static void test_nanocoap__put_req(void)
{
    uint8_t buf[128];
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
    TEST_ASSERT_EQUAL_INT(&buf[0] + 128 - pkt.payload, pkt.payload_len);
}

/*
 * Builds on get_req test, to test path with multiple segments.
 */
static void test_nanocoap__get_multi_path(void)
{
    uint8_t buf[128];
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
    coap_get_uri(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);
}

/*
 * Builds on get_req test, to test '/' path. This path is the default when
 * otherwise not specified.
 */
static void test_nanocoap__get_root_path(void)
{
    uint8_t buf[128];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/";

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    char uri[10] = {0};
    coap_get_uri(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);
}

/*
 * Builds on get_req test, to test max length path.
 */
static void test_nanocoap__get_max_path(void)
{
    uint8_t buf[128];
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
    coap_get_uri(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);
}

/*
 * Builds on get_req test, to test path longer than NANOCOAP_URI_MAX. We
 * expect coap_get_uri() to return -ENOSPC.
 */
static void test_nanocoap__get_path_too_long(void)
{
    uint8_t buf[128];
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
    int get_len = coap_get_uri(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_INT(-ENOSPC, get_len);
}

Test *tests_nanocoap_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nanocoap__hdr),
        new_TestFixture(test_nanocoap__get_req),
        new_TestFixture(test_nanocoap__put_req),
        new_TestFixture(test_nanocoap__get_multi_path),
        new_TestFixture(test_nanocoap__get_root_path),
        new_TestFixture(test_nanocoap__get_max_path),
        new_TestFixture(test_nanocoap__get_path_too_long),
    };

    EMB_UNIT_TESTCALLER(nanocoap_tests, NULL, NULL, fixtures);

    return (Test *)&nanocoap_tests;
}

void tests_nanocoap(void)
{
    TESTS_RUN(tests_nanocoap_tests());
}
/** @} */
