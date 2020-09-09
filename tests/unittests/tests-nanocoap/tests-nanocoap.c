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
#include <stdio.h>

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

    char uri[CONFIG_NANOCOAP_URI_MAX] = {0};
    coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);
}

/*
 * Builds on get_req test, to test path longer than CONFIG_NANOCOAP_URI_MAX. We
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

    char uri[CONFIG_NANOCOAP_URI_MAX] = {0};
    int get_len = coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_INT(-ENOSPC, get_len);
}

/*
 * Builds on get_req test, to test Uri-Query option.
 */
static void test_nanocoap__get_query(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/time";
    size_t path_opt_len = 5;
    char qs[] = "ab=cde";
    size_t query_opt_len = 7;

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(path_opt_len, len);

    uint8_t *query_pos = &pkt.payload[0];
    len = coap_opt_add_string(&pkt, COAP_OPT_URI_QUERY, &qs[0], '&');
    TEST_ASSERT_EQUAL_INT(query_opt_len, len);

    char uri[10] = {0};
    coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);

    char query[10] = {0};
    coap_get_uri_query(&pkt, (uint8_t *)&query[0]);
    /* skip initial '&' from coap_get_uri_query() */
    TEST_ASSERT_EQUAL_STRING((char *)qs, &query[1]);

    /* overwrite query to test buffer-based put */
    coap_opt_put_uri_query(query_pos, COAP_OPT_URI_PATH, qs);
    coap_get_uri_query(&pkt, (uint8_t *)&query[0]);
    /* skip initial '&' from coap_get_uri_query() */
    TEST_ASSERT_EQUAL_STRING((char *)qs, &query[1]);
}

/*
 * Builds on get_query test, to test multiple Uri-Query options.
 */
static void test_nanocoap__get_multi_query(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char key1[] = "ab";
    char val1[] = "cde";
    char key2[] = "f";
    char qs[] = "ab=cde&f";

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    uint8_t *query_pos = &pkt.payload[0];
    /* first opt header is 2 bytes long */
    ssize_t optlen = coap_opt_add_uri_query(&pkt, key1, val1);
    TEST_ASSERT_EQUAL_INT(8, optlen);
    optlen = coap_opt_add_uri_query(&pkt, key2, NULL);
    TEST_ASSERT_EQUAL_INT(2, optlen);

    char query[20] = {0};
    coap_get_uri_query(&pkt, (uint8_t *)&query[0]);
    /* skip initial '&' from coap_get_uri_query() */
    TEST_ASSERT_EQUAL_STRING((char *)qs, &query[1]);

    /* overwrite query to test buffer-based put */
    coap_opt_put_uri_query(query_pos, COAP_OPT_URI_PATH, qs);
    coap_get_uri_query(&pkt, (uint8_t *)&query[0]);
    /* skip initial '&' from coap_get_uri_query() */
    TEST_ASSERT_EQUAL_STRING((char *)qs, &query[1]);
}
/*
 * Builds on get_multi_query test, to use coap_opt_add_uri_query2().
 */
static void test_nanocoap__add_uri_query2(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char keys[] = "a;bcd;";
    int key1_len = 1;
    int key2_len = 3;
    char vals[] = "do;re";
    int val1_len = 2;
    char qs1[] = "a=do";
    size_t query1_opt_len = 6;    /* first opt header is 2 bytes long */
    char qs2[] = "a=do&bcd";
    size_t query2_opt_len = 4;
    char qs3[] = "a=do&bcd&bcd";
    size_t query3_opt_len = 4;

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    /* includes key and value */
    char query[20] = {0};
    len = coap_opt_add_uri_query2(&pkt, keys, key1_len, vals, val1_len);
    TEST_ASSERT_EQUAL_INT(query1_opt_len, len);
    coap_get_uri_query(&pkt, (uint8_t *)&query[0]);
    /* skip initial '&' from coap_get_uri_query() */
    TEST_ASSERT_EQUAL_STRING((char *)qs1, &query[1]);

    /* includes key only */
    memset(query, 0, 20);
    len = coap_opt_add_uri_query2(&pkt, &keys[2], key2_len, NULL, 0);
    TEST_ASSERT_EQUAL_INT(query2_opt_len, len);
    coap_get_uri_query(&pkt, (uint8_t *)&query[0]);
    /* skip initial '&' from coap_get_uri_query() */
    TEST_ASSERT_EQUAL_STRING((char *)qs2, &query[1]);

    /* includes key only; value not NULL but zero length */
    memset(query, 0, 20);
    len = coap_opt_add_uri_query2(&pkt, &keys[2], key2_len, &vals[3], 0);
    TEST_ASSERT_EQUAL_INT(query3_opt_len, len);
    coap_get_uri_query(&pkt, (uint8_t *)&query[0]);
    /* skip initial '&' from coap_get_uri_query() */
    TEST_ASSERT_EQUAL_STRING((char *)qs3, &query[1]);

    /* fails an assert, so only run when disabled */
#ifdef NDEBUG
    char qs4[] = "a=do&bcd&bcd&bcd";
    size_t query4_opt_len = 4;

    /* includes key only; value NULL and length > 0 */
    memset(query, 0, 20);
    len = coap_opt_add_uri_query2(&pkt, &keys[2], key2_len, NULL, 1);
    TEST_ASSERT_EQUAL_INT(query4_opt_len, len);
    coap_get_uri_query(&pkt, (uint8_t *)&query[0]);
    /* skip initial '&' from coap_get_uri_query() */
    TEST_ASSERT_EQUAL_STRING((char *)qs4, &query[1]);
#endif
}
/*
 * Builds on get_req test, to test building a PDU that completely fills the
 * buffer, and one that tries to overfill the buffer.
 */
static void test_nanocoap__option_add_buffer_max(void)
{
    uint8_t buf[70];    /* header 4, token 2, path 64 */
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[] = "/23456789012345678901234567890123456789012345678901234567890123";

    size_t uri_opt_len = 64;    /* option hdr 2, option value 62 */

    ssize_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                 &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(uri_opt_len, len);

    /* shrink buffer to attempt overfill */
    coap_pkt_init(&pkt, &buf[0], sizeof(buf) - 1, len);

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(-ENOSPC, len);
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

static void test_nanocoap__server_option_count_overflow_check(void)
{
    /* this test passes a forged CoAP packet containing 42 options (provided by
     * @nmeum in #10753, 42 is a random number which just needs to be higher
     * than CONFIG_NANOCOAP_NOPTS_MAX) to coap_parse().  The used coap_pkt_t is part
     * of a struct, followed by an array of 42 coap_option_t.  The array is
     * cleared before the call to coap_parse().  If the overflow protection is
     * working, the array must still be clear after parsing the packet, and the
     * proper error code (-ENOMEM) is returned.  Otherwise, the parsing wrote
     * past scratch.pkt, thus the array is not zeroed anymore.
     */

     static uint8_t pkt_data[] = {
        0x40, 0x01, 0x09, 0x26, 0x01, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17,
        0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17,
        0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17,
        0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17,
        0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17,
        0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17,
        0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17,
        0x11, 0x17, 0x11, 0x17 };

    /* ensure CONFIG_NANOCOAP_NOPTS_MAX is actually lower than 42 */
    TEST_ASSERT(CONFIG_NANOCOAP_NOPTS_MAX < 42);

    struct {
      coap_pkt_t pkt;
      uint8_t guard_data[42 * sizeof(coap_optpos_t)];
    } scratch;

    memset(&scratch, 0, sizeof(scratch));

    int res = coap_parse(&scratch.pkt, pkt_data, sizeof(pkt_data));

    /* check if any byte of the guard_data array is non-zero */
    int dirty = 0;
    volatile uint8_t *pos = scratch.guard_data;
    for (size_t i = 0; i < sizeof(scratch.guard_data); i++) {
        if (*pos) {
            dirty = 1;
            break;
        }
    }

    TEST_ASSERT_EQUAL_INT(0, dirty);
    TEST_ASSERT_EQUAL_INT(-ENOMEM, res);
}

/*
 * Verifies that coap_parse() recognizes inclusion of too many options.
 */
static void test_nanocoap__server_option_count_overflow(void)
{
    /* base pkt is a GET for /riot/value, which results in two options for the
     * path, but only 1 entry in the options array.
     * Size buf to accept an extra 2-byte option */
    unsigned base_len = 17;
    uint8_t buf[17 + (2 * CONFIG_NANOCOAP_NOPTS_MAX)] = {
        0x42, 0x01, 0xbe, 0x16, 0x35, 0x61, 0xb4, 0x72,
        0x69, 0x6f, 0x74, 0x05, 0x76, 0x61, 0x6c, 0x75,
        0x65
    };
    coap_pkt_t pkt;

    /* nonsense filler option that contains a single byte of data */
    uint8_t fill_opt[] = { 0x11, 0x01 };

    /* fill pkt with maximum options; should succeed */
    int i = 0;
    for (; i < (2 * (CONFIG_NANOCOAP_NOPTS_MAX - 1)); i+=2) {
        memcpy(&buf[base_len+i], fill_opt, 2);
    }

    /* don't read final two bytes, where overflow option will be added later */
    int res = coap_parse(&pkt, buf, sizeof(buf) - 2);
    TEST_ASSERT_EQUAL_INT(0, res);

    /* add option to overflow */
    memcpy(&buf[base_len+i], fill_opt, 2);

    res = coap_parse(&pkt, buf, sizeof(buf));
    TEST_ASSERT(res < 0);
}

/*
 * Helper for options tests below.
 * POST request to a CoRE RD server to update the entries for a node
 * from RIOT cord_ep example. Generated by RIOT.
 * Includes 4 options:
 *   Uri-Path: resourcedirectory
 *   Content-Format: 40 (0x28)
 *   Uri-Query: ep-RIOT-0C49232323232323
 *   Uri-Query: lt=60
 * Payload: </node/info> (absent if omit_payload)
 */
static int _read_rd_post_req(coap_pkt_t *pkt, bool omit_payload)
{
     static uint8_t pkt_data[] = {
        0x42, 0x02, 0x20, 0x92, 0xb9, 0x27, 0xbd, 0x04,
        0x72, 0x65, 0x73, 0x6f, 0x75, 0x72, 0x63, 0x65,
        0x64, 0x69, 0x72, 0x65, 0x63, 0x74, 0x6f, 0x72,
        0x79, 0x11, 0x28, 0x3d, 0x0b, 0x65, 0x70, 0x3d,
        0x52, 0x49, 0x4f, 0x54, 0x2d, 0x30, 0x43, 0x34,
        0x39, 0x32, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32,
        0x33, 0x32, 0x33, 0x32, 0x33, 0x05, 0x6c, 0x74,
        0x3d, 0x36, 0x30, 0xff, 0x3c, 0x2f, 0x6e, 0x6f,
        0x64, 0x65, 0x2f, 0x69, 0x6e, 0x66, 0x6f, 0x3e
     };

    size_t len = omit_payload ? 59 : sizeof(pkt_data);
    return coap_parse(pkt, pkt_data, len);
}

/*
 * Tests use of coap_opt_get_next() to iterate over options.
 */
static void test_nanocoap__options_iterate(void)
{
    coap_pkt_t pkt;
    int res = _read_rd_post_req(&pkt, true);
    TEST_ASSERT_EQUAL_INT(0, res);

    /* read all options */
    coap_optpos_t opt = {0, 0};
    uint8_t *value;
    ssize_t exp_len[] = {17, 1, 24, 5, -ENOENT};
    ssize_t exp_optnum[] = {COAP_OPT_URI_PATH, COAP_OPT_CONTENT_FORMAT,
                            COAP_OPT_URI_QUERY, COAP_OPT_URI_QUERY};

    for (int i = 0; i < 5; i++) {
        ssize_t optlen = coap_opt_get_next(&pkt, &opt, &value, !i);
        TEST_ASSERT_EQUAL_INT(exp_len[i], optlen);
        if (optlen >= 0) {
            TEST_ASSERT_EQUAL_INT(exp_optnum[i], opt.opt_num);
        }
        else {
            TEST_ASSERT_EQUAL_INT(4, i);
        }
    }

    /* test with no payload to verify end of options handling */
    memset(&pkt, 0, sizeof(pkt));
    res = _read_rd_post_req(&pkt, false);
    TEST_ASSERT_EQUAL_INT(0, res);

    for (int i = 0; i < 5; i++) {
        ssize_t optlen = coap_opt_get_next(&pkt, &opt, &value, !i);
        TEST_ASSERT_EQUAL_INT(exp_len[i], optlen);
        if (optlen >= 0) {
            TEST_ASSERT_EQUAL_INT(exp_optnum[i], opt.opt_num);
        }
        else {
            TEST_ASSERT_EQUAL_INT(4, i);
        }
    }
}

/*
 * Tests use of coap_opt_get_opaque() to find an option as a byte array, and
 * coap_opt_get_next() to find a second option with the same option number.
 */
static void test_nanocoap__options_get_opaque(void)
{
    coap_pkt_t pkt;
    int res = _read_rd_post_req(&pkt, true);
    TEST_ASSERT_EQUAL_INT(0, res);

    /* read Uri-Query options */
    uint8_t *value;
    ssize_t optlen = coap_opt_get_opaque(&pkt, COAP_OPT_URI_QUERY, &value);
    TEST_ASSERT_EQUAL_INT(24, optlen);

    coap_optpos_t opt = {0, value + optlen - (uint8_t *)pkt.hdr};

    optlen = coap_opt_get_next(&pkt, &opt, &value, false);
    TEST_ASSERT_EQUAL_INT(0, opt.opt_num);
    TEST_ASSERT_EQUAL_INT(5, optlen);

    optlen = coap_opt_get_next(&pkt, &opt, &value, false);
    TEST_ASSERT_EQUAL_INT(-ENOENT, optlen);
}

/*
 * Validates empty message parsing.
 */
static void test_nanocoap__empty(void)
{
    /* first four bytes are valid empty msg; include 5th byte for test */
    static uint8_t pkt_data[] = {
        0x40, 0x00, 0xAB, 0xCD, 0x00
    };

    uint16_t msgid = 0xABCD;

    coap_pkt_t pkt;
    int res = coap_parse(&pkt, pkt_data, 4);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(0, coap_get_code_raw(&pkt));
    TEST_ASSERT_EQUAL_INT(msgid, coap_get_id(&pkt));
    TEST_ASSERT_EQUAL_INT(0, coap_get_token_len(&pkt));
    TEST_ASSERT_EQUAL_INT(0, pkt.payload_len);

    /* too short */
    memset(&pkt, 0, sizeof(coap_pkt_t));
    res = coap_parse(&pkt, pkt_data, 3);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, res);

    /* too long */
    memset(&pkt, 0, sizeof(coap_pkt_t));
    res = coap_parse(&pkt, pkt_data, 5);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, res);
}

/*
 * Test adding a path from an unterminated string.
 */
static void test_nanocoap__add_path_unterminated_string(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char path[16] = "/time";
    size_t path_len = strlen("/time");

    /* some random non-zero character at the end of /time */
    path[path_len] = 'Z';

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    coap_opt_add_chars(&pkt, COAP_OPT_URI_PATH, &path[0], path_len, '/');

    char uri[10] = {0};
    ssize_t parsed_path_len = coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);

    /* we subtract one byte for '\0' at the end from parsed_uri_path */
    TEST_ASSERT_EQUAL_INT(path_len, parsed_path_len - 1);
    TEST_ASSERT_EQUAL_INT(0, strncmp(path, uri, path_len));
}

/*
 * Test adding and retrieving the Proxy-URI option to and from a request.
 */
static void test_nanocoap__add_get_proxy_uri(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};
    char proxy_uri[60] = "coap://[2001:db8::1]:5683/.well-known/core";

    size_t len = coap_build_hdr((coap_hdr_t *)&buf[0], COAP_TYPE_NON,
                                &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    len = coap_opt_add_proxy_uri(&pkt, proxy_uri);

    /* strlen + 1 byte option number + 2 bytes length */
    TEST_ASSERT_EQUAL_INT(strlen(proxy_uri) + 3, len);

    char *uri;
    len = coap_get_proxy_uri(&pkt, (char **) &uri);

    TEST_ASSERT_EQUAL_INT(strlen(proxy_uri), len);
    TEST_ASSERT_EQUAL_INT(0, strncmp((char *) proxy_uri, (char *) uri, len));
}

/*
 * Verifies that coap_parse() recognizes token length bigger than allowed.
 */
static void test_nanocoap__token_length_over_limit(void)
{
    /* RFC7252 states that TKL must be within 0-8:
     * "Lengths 9-15 are reserved, MUST NOT be sent,
     * and MUST be processed as a message format error."
     */
    uint16_t msgid = 0xABCD;
    uint8_t buf_invalid[] = {
        0x49, 0x01, 0xAB, 0xCD,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99
    };
    uint8_t buf_valid[] = {
        0x48, 0x01, 0xAB, 0xCD,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
    };
    coap_pkt_t pkt;

    /* Valid packet (TKL = 8) */
    int res = coap_parse(&pkt, buf_valid, sizeof(buf_valid));

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(1, coap_get_code_raw(&pkt));
    TEST_ASSERT_EQUAL_INT(msgid, coap_get_id(&pkt));
    TEST_ASSERT_EQUAL_INT(8, coap_get_token_len(&pkt));
    TEST_ASSERT_EQUAL_INT(0, pkt.payload_len);

    /* Invalid packet (TKL = 9) */
    res = coap_parse(&pkt, buf_invalid, sizeof(buf_invalid));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, res);
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
        new_TestFixture(test_nanocoap__get_query),
        new_TestFixture(test_nanocoap__get_multi_query),
        new_TestFixture(test_nanocoap__add_uri_query2),
        new_TestFixture(test_nanocoap__option_add_buffer_max),
        new_TestFixture(test_nanocoap__options_get_opaque),
        new_TestFixture(test_nanocoap__options_iterate),
        new_TestFixture(test_nanocoap__server_get_req),
        new_TestFixture(test_nanocoap__server_reply_simple),
        new_TestFixture(test_nanocoap__server_get_req_con),
        new_TestFixture(test_nanocoap__server_reply_simple_con),
        new_TestFixture(test_nanocoap__server_option_count_overflow_check),
        new_TestFixture(test_nanocoap__server_option_count_overflow),
        new_TestFixture(test_nanocoap__empty),
        new_TestFixture(test_nanocoap__add_path_unterminated_string),
        new_TestFixture(test_nanocoap__add_get_proxy_uri),
        new_TestFixture(test_nanocoap__token_length_over_limit),
    };

    EMB_UNIT_TESTCALLER(nanocoap_tests, NULL, NULL, fixtures);

    return (Test *)&nanocoap_tests;
}

void tests_nanocoap(void)
{
    TESTS_RUN(tests_nanocoap_tests());
}
/** @} */
