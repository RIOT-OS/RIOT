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
    pktpos += coap_build_udp_hdr(pktpos, sizeof(buf), COAP_TYPE_CON, NULL, 0,
                             COAP_METHOD_GET, msgid);
    pktpos += coap_opt_put_location_path(pktpos, 0, loc_path);
    pktpos += coap_opt_put_uri_path(pktpos, COAP_OPT_LOCATION_PATH, path);

    coap_pkt_t pkt;
    TEST_ASSERT_EQUAL_INT(pktpos - &buf[0], coap_parse_udp(&pkt, &buf[0], pktpos - &buf[0]));

    TEST_ASSERT_EQUAL_INT(msgid, coap_get_id(&pkt));

    int res = coap_get_uri_path(&pkt, path_tmp);
    TEST_ASSERT_EQUAL_INT(sizeof(path), res);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)path_tmp);

    res = coap_get_location_path(&pkt, path_tmp, 64);
    TEST_ASSERT_EQUAL_INT(sizeof(loc_path), res);
    TEST_ASSERT_EQUAL_STRING((char *)loc_path, (char *)path_tmp);
}

/*
 * Validates encoded message ID byte order and put/get URI & Query option.
 */
static void test_nanocoap__hdr_2(void)
{
    uint8_t buf[_BUF_SIZE];
    uint16_t msgid = 0xABCD;
    char path[] = "/test/abcd/efgh?foo=bar&baz=blub";
    unsigned char path_tmp[64] = {0};
    char query_tmp[64] = {0};
    char key[8];
    char value[8];

    uint8_t *pktpos = &buf[0];
    uint16_t lastonum = 0;
    pktpos += coap_build_udp_hdr(pktpos, sizeof(buf), COAP_TYPE_CON, NULL, 0,
                                 COAP_METHOD_GET, msgid);
    pktpos += coap_opt_put_uri_pathquery(pktpos, &lastonum, path);

    coap_pkt_t pkt;
    TEST_ASSERT_EQUAL_INT(pktpos - &buf[0], coap_parse_udp(&pkt, &buf[0], pktpos - &buf[0]));

    TEST_ASSERT_EQUAL_INT(msgid, coap_get_id(&pkt));

    int res = coap_get_uri_path(&pkt, path_tmp);
    TEST_ASSERT_EQUAL_INT(sizeof("/test/abcd/efgh"), res);
    TEST_ASSERT_EQUAL_STRING("/test/abcd/efgh", (char *)path_tmp);

    res = coap_get_uri_query_string(&pkt, query_tmp, sizeof(query_tmp));
    TEST_ASSERT_EQUAL_INT(sizeof("&foo=bar&baz=blub"), res);
    TEST_ASSERT_EQUAL_STRING("&foo=bar&baz=blub", (char *)query_tmp);

    void *pos = NULL;

    res = coap_iterate_uri_query(&pkt, &pos, key, sizeof(key), value, 3);
    TEST_ASSERT_EQUAL_INT(-E2BIG, res);
    res = coap_iterate_uri_query(&pkt, &pos, key, 3, value, sizeof(value));
    TEST_ASSERT_EQUAL_INT(-E2BIG, res);

    pos = NULL;
    res = coap_iterate_uri_query(&pkt, &pos, key, sizeof(key), value, sizeof(value));
    TEST_ASSERT_EQUAL_INT(2, res);
    TEST_ASSERT_EQUAL_STRING("foo", key);
    TEST_ASSERT_EQUAL_STRING("bar", value);
    res = coap_iterate_uri_query(&pkt, &pos, key, sizeof(key), value, sizeof(value));
    TEST_ASSERT_EQUAL_INT(2, res);
    TEST_ASSERT_EQUAL_STRING("baz", key);
    TEST_ASSERT_EQUAL_STRING("blub", value);
    res = coap_iterate_uri_query(&pkt, &pos, key, sizeof(key), value, sizeof(value));
    TEST_ASSERT_EQUAL_INT(0, res);
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

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_GET, msgid);
    TEST_ASSERT_EQUAL_INT(total_hdr_len, len);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    TEST_ASSERT_EQUAL_INT(COAP_METHOD_GET, coap_get_code_decimal(&pkt));
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
    size_t accept_opt_len = 2;

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_PUT, msgid);
    TEST_ASSERT_EQUAL_INT(total_hdr_len, len);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);

    TEST_ASSERT_EQUAL_INT(COAP_METHOD_PUT, coap_get_code_decimal(&pkt));
    TEST_ASSERT_EQUAL_INT(122, pkt.payload_len);

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(uri_opt_len, len);

    len = coap_opt_add_uint(&pkt, COAP_OPT_CONTENT_FORMAT, COAP_FORMAT_TEXT);
    TEST_ASSERT_EQUAL_INT(fmt_opt_len, len);
    TEST_ASSERT_EQUAL_INT(COAP_FORMAT_TEXT, coap_get_content_type(&pkt));

    len = coap_opt_add_uint(&pkt, COAP_OPT_ACCEPT, COAP_FORMAT_CBOR);
    TEST_ASSERT_EQUAL_INT(accept_opt_len, len);
    TEST_ASSERT_EQUAL_INT(COAP_FORMAT_CBOR, coap_get_accept(&pkt));

    len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);
    TEST_ASSERT_EQUAL_INT(total_hdr_len + uri_opt_len + fmt_opt_len + accept_opt_len + 1, len);
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

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

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

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

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

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

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

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

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

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

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

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(path_opt_len, len);

    uint8_t *query_pos = &pkt.payload[0];
    len = coap_opt_add_string(&pkt, COAP_OPT_URI_QUERY, &qs[0], '&');
    TEST_ASSERT_EQUAL_INT(query_opt_len, len);

    char uri[10] = {0};
    coap_get_uri_path(&pkt, (uint8_t *)&uri[0]);
    TEST_ASSERT_EQUAL_STRING((char *)path, (char *)uri);

    char query[10] = {0};
    coap_get_uri_query_string(&pkt, query, sizeof(query));
    /* skip initial '&' from coap_get_uri_query_string() */
    TEST_ASSERT_EQUAL_STRING((char *)qs, &query[1]);

    /* overwrite query to test buffer-based put */
    coap_opt_put_uri_query(query_pos, COAP_OPT_URI_PATH, qs);
    coap_get_uri_query_string(&pkt, query, sizeof(query));
    /* skip initial '&' from coap_get_uri_query_string() */
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
    const char key1[] = "ab";
    const char val1[] = "cde";
    const char key2[] = "f";
    const char qs[] = "ab=cde&f";
    char key[8];
    char value[8];

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

    uint8_t *query_pos = &pkt.payload[0];
    /* first opt header is 2 bytes long */
    ssize_t optlen = coap_opt_add_uri_query(&pkt, key1, val1);
    TEST_ASSERT_EQUAL_INT(8, optlen);
    optlen = coap_opt_add_uri_query(&pkt, key2, NULL);
    TEST_ASSERT_EQUAL_INT(2, optlen);

    char query[20] = {0};
    coap_get_uri_query_string(&pkt, query, sizeof(query));
    /* skip initial '&' from coap_get_uri_query_string() */
    TEST_ASSERT_EQUAL_STRING((char *)qs, &query[1]);

    /* overwrite query to test buffer-based put */
    coap_opt_put_uri_query(query_pos, COAP_OPT_URI_PATH, qs);
    coap_get_uri_query_string(&pkt, query, sizeof(query));
    /* skip initial '&' from coap_get_uri_query_string() */
    TEST_ASSERT_EQUAL_STRING((char *)qs, &query[1]);

    const char *val;
    size_t val_len;
    TEST_ASSERT(coap_find_uri_query(&pkt, "ab", &val, &val_len));
    TEST_ASSERT_EQUAL_INT(3, val_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(val, "cde", val_len));
    TEST_ASSERT(coap_find_uri_query(&pkt, "f", &val, &val_len));
    TEST_ASSERT_EQUAL_INT(0, val_len);
    TEST_ASSERT_EQUAL_INT((uintptr_t)NULL, (uintptr_t)val);
    TEST_ASSERT(!coap_find_uri_query(&pkt, "cde", &val, &val_len));
    TEST_ASSERT(coap_find_uri_query(&pkt, "ab", NULL, 0));

    void *pos = NULL;
    int res = coap_iterate_uri_query(&pkt, &pos, key, sizeof(key), value, sizeof(value));
    TEST_ASSERT_EQUAL_INT(2, res);
    TEST_ASSERT_EQUAL_STRING("ab", key);
    TEST_ASSERT_EQUAL_STRING("cde", value);
    res = coap_iterate_uri_query(&pkt, &pos, key, sizeof(key), value, sizeof(value));
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_STRING("f", key);
    TEST_ASSERT_EQUAL_STRING("", value);
    res = coap_iterate_uri_query(&pkt, &pos, key, sizeof(key), value, sizeof(value));
    TEST_ASSERT_EQUAL_INT(0, res);
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

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

    /* includes key and value */
    char query[20] = {0};
    len = coap_opt_add_uri_query2(&pkt, keys, key1_len, vals, val1_len);
    TEST_ASSERT_EQUAL_INT(query1_opt_len, len);
    coap_get_uri_query_string(&pkt, query, sizeof(query));
    /* skip initial '&' from coap_get_uri_query_string() */
    TEST_ASSERT_EQUAL_STRING((char *)qs1, &query[1]);

    /* includes key only */
    memset(query, 0, 20);
    len = coap_opt_add_uri_query2(&pkt, &keys[2], key2_len, NULL, 0);
    TEST_ASSERT_EQUAL_INT(query2_opt_len, len);
    coap_get_uri_query_string(&pkt, query, sizeof(query));
    /* skip initial '&' from coap_get_uri_query_string() */
    TEST_ASSERT_EQUAL_STRING((char *)qs2, &query[1]);

    /* includes key only; value not NULL but zero length */
    memset(query, 0, 20);
    len = coap_opt_add_uri_query2(&pkt, &keys[2], key2_len, &vals[3], 0);
    TEST_ASSERT_EQUAL_INT(query3_opt_len, len);
    coap_get_uri_query_string(&pkt, query, sizeof(query));
    /* skip initial '&' from coap_get_uri_query_string() */
    TEST_ASSERT_EQUAL_STRING((char *)qs3, &query[1]);

    /* fails an assert, so only run when disabled */
#ifdef NDEBUG
    char qs4[] = "a=do&bcd&bcd&bcd";
    size_t query4_opt_len = 4;

    /* includes key only; value NULL and length > 0 */
    memset(query, 0, 20);
    len = coap_opt_add_uri_query2(&pkt, &keys[2], key2_len, NULL, 1);
    TEST_ASSERT_EQUAL_INT(query4_opt_len, len);
    coap_get_uri_query_string(&pkt, query, sizeof(query));
    /* skip initial '&' from coap_get_uri_query_string() */
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

    ssize_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                     &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(uri_opt_len, len);

    /* shrink buffer to attempt overfill */
    coap_pkt_init(&pkt, &buf[0], sizeof(buf) - 1, len);

    len = coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, &path[0], '/');
    TEST_ASSERT_EQUAL_INT(-ENOSPC, len);
}

static void __test_option_remove(uint16_t stride)
{
    const char payload[] = "My test payload";
    /* header 4, token 2, options (8 - 1) * 4 = 28, payload marker 1 + payload */
    uint8_t buf[4U + 2U + 28U + 1U + sizeof(payload)];
    coap_pkt_t pkt;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};

    ssize_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                     &token[0], 2, COAP_METHOD_GET, msgid);
    /* shrink buffer to attempt overfill */
    coap_pkt_init(&pkt, &buf[0], sizeof(buf) - 1, len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

    /* add seven options of options 1 to 7 */
    for (uint16_t count = 1; count < 8; count++) {
        len = coap_opt_add_uint(&pkt, (uint16_t)(count * stride), count);
        if (stride < 13) {
            TEST_ASSERT_EQUAL_INT(2U, len);
        }
        else if (stride < 269) {
            TEST_ASSERT_EQUAL_INT(3U, len);
        }
        else {
            TEST_ASSERT_EQUAL_INT(4U, len);
        }
    }
    /* header 4, token 2, options (8 - 1) * opt_len, payload marker 1 */
    unsigned exp_len = 4U + 2U + ((8 - 1) * ((stride < 13) ? 2U : ((stride < 269) ? 3U : 4U))) + 1U;
    /* finish with payload marker */
    len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);
    TEST_ASSERT_EQUAL_INT(exp_len, len);
    /* add payload to check move of payload */
    memcpy(pkt.payload, payload, sizeof(payload));
    pkt.payload_len = sizeof(payload);

    /* remove option number 3 */
    len = coap_opt_remove(&pkt, (3U * stride));
    /* one option was removed so remove from expected length based on stride */
    exp_len -= (stride < 13) ? 2U : ((stride < 269) ? 3U : 4U);
    if (((stride >= 7) && (stride < 13)) || ((stride >= 135) && (stride < 269))) {
        /* account for growing delta size */
        exp_len += 1;
    }
    TEST_ASSERT_EQUAL_INT(exp_len + sizeof(payload), len);

    /* check if all but option number 3 are still the same */
    for (uint16_t count = 1; count < 8; count++) {
        uint32_t value;
        len = coap_opt_get_uint(&pkt, (uint16_t)(count * stride), &value);
        if (count == 3U) {
            TEST_ASSERT_EQUAL_INT(-ENOENT, len);
        }
        else {
            TEST_ASSERT_EQUAL_INT(0, len);
            TEST_ASSERT_EQUAL_INT(count, value);
        }
    }
    /* check payload */
    TEST_ASSERT_EQUAL_STRING(payload, (char *)pkt.payload);

    /* remove non-existent option */
    len = coap_opt_remove(&pkt, (14U * stride));
    /* no option was removed so same as before */
    TEST_ASSERT_EQUAL_INT(exp_len + sizeof(payload), len);
    /* and everything should still be the same */
    for (uint16_t count = 1; count < 8; count++) {
        uint32_t value;
        len = coap_opt_get_uint(&pkt, (uint16_t)(count * stride), &value);
        if (count == 3U) {
            TEST_ASSERT_EQUAL_INT(-ENOENT, len);
        }
        else {
            TEST_ASSERT_EQUAL_INT(0, len);
            TEST_ASSERT_EQUAL_INT(count, value);
        }
    }
    TEST_ASSERT_EQUAL_STRING(payload, (char *)pkt.payload);

    /* remove first option */
    len = coap_opt_remove(&pkt, (1U * stride));
    /* one option was removed so remove from expected length based on stride */
    exp_len -= (stride < 13) ? 2U : ((stride < 269) ? 3U : 4U);
    if (((stride >= 7) && (stride < 13)) || ((stride >= 135) && (stride < 269))) {
        /* account for growing delta size */
        exp_len += 1;
    }
    TEST_ASSERT_EQUAL_INT(exp_len + sizeof(payload), len);
    /* and everything should still be the same */
    for (uint16_t count = 1; count < 8; count++) {
        uint32_t value;
        len = coap_opt_get_uint(&pkt, (uint16_t)(count * stride), &value);
        if ((count == 1U) || (count == 3U)) {
            TEST_ASSERT_EQUAL_INT(-ENOENT, len);
        }
        else {
            TEST_ASSERT_EQUAL_INT(0, len);
            TEST_ASSERT_EQUAL_INT(count, value);
        }
    }

    TEST_ASSERT_EQUAL_STRING(payload, (char *)pkt.payload);
    /* remove last option */
    len = coap_opt_remove(&pkt, (7U * stride));
    /* one option was removed so remove from expected length based on stride */
    exp_len -= (stride < 13) ? 2U : ((stride < 269) ? 3U : 4U);
    TEST_ASSERT_EQUAL_INT(exp_len + sizeof(payload), len);
    /* and everything should still be the same */
    for (uint16_t count = 1; count < 8; count++) {
        uint32_t value;
        len = coap_opt_get_uint(&pkt, (uint16_t)(count * stride), &value);
        if ((count == 1U) || (count == 3U) || (count == 7U)) {
            TEST_ASSERT_EQUAL_INT(-ENOENT, len);
        }
        else {
            TEST_ASSERT_EQUAL_INT(0, len);
            TEST_ASSERT_EQUAL_INT(count, value);
        }
    }
    TEST_ASSERT_EQUAL_STRING(payload, (char *)pkt.payload);
}

static void test_nanocoap__option_remove_delta_1(void)
{
    /* base line */
    __test_option_remove(1U);
}

static void test_nanocoap__option_remove_delta_7(void)
{
    /* delta goes above 13 when removing option => option header of next option grows */
    __test_option_remove(7U);
}

static void test_nanocoap__option_remove_delta_13(void)
{
    __test_option_remove(13U);
}

static void test_nanocoap__option_remove_delta_32(void)
{
    __test_option_remove(32U);
}

static void test_nanocoap__option_remove_delta_135(void)
{
    /* delta goes above 269 when removing option => option header of next option grows */
    __test_option_remove(135U);
}

static void test_nanocoap__option_remove_delta_269(void)
{
    __test_option_remove(269U);
}

static void test_nanocoap__option_remove_delta_512(void)
{
    __test_option_remove(512U);
}

static void test_nanocoap__option_remove_no_payload(void)
{
    /* header 4, token 2, option length 3, 0 payload marker 1 */
    uint8_t buf[4U + 2U + 4U];
    coap_pkt_t pkt;
    uint32_t value;
    uint16_t msgid = 0xABCD;
    uint8_t token[2] = {0xDA, 0xEC};

    ssize_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                     &token[0], 2, COAP_METHOD_GET, msgid);
    /* shrink buffer to attempt overfill */
    coap_pkt_init(&pkt, &buf[0], sizeof(buf) - 1, len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

    len = coap_opt_add_uint(&pkt, 1U, 500U);
    TEST_ASSERT_EQUAL_INT(3U, len);
    /* header 4, token 2, options (8 - 1) * opt_len */
    unsigned exp_len = 4U + 2U + 3U;
    /* finish with payload marker */
    len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    TEST_ASSERT_EQUAL_INT(exp_len, len);
    /* add payload to check move of payload */
    TEST_ASSERT_EQUAL_INT(0, pkt.payload_len);

    /* remove option number 3 */
    len = coap_opt_remove(&pkt, 1U);
    exp_len -= 3U;
    TEST_ASSERT_EQUAL_INT(exp_len, len);
    len = coap_opt_get_uint(&pkt, 1U, &value);
    TEST_ASSERT_EQUAL_INT(-ENOENT, len);
    TEST_ASSERT_EQUAL_INT(0, pkt.payload_len);
}

/*
 * Helper for server_get tests below.
 * GET Request for nanocoap server example /riot/value resource.
 * Includes 2-byte token; non-confirmable.
 * Generated with libcoap.
 */
static ssize_t _read_riot_value_req(coap_pkt_t *pkt, uint8_t *buf)
{
    uint8_t pkt_data[] = {
        0x52, 0x01, 0x9e, 0x6b, 0x35, 0x61, 0xb4, 0x72,
        0x69, 0x6f, 0x74, 0x05, 0x76, 0x61, 0x6c, 0x75,
        0x65
    };
    memcpy(buf, pkt_data, sizeof(pkt_data));

    return coap_parse_udp(pkt, buf, sizeof(pkt_data));
}

/* Server GET request success case. */
static void test_nanocoap__server_get_req(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;
    char path[] = "/riot/value";

    ssize_t res = _read_riot_value_req(&pkt, &buf[0]);

    TEST_ASSERT(res > 0);
    TEST_ASSERT_EQUAL_INT(COAP_METHOD_GET, coap_get_code_decimal(&pkt));
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

    ssize_t res = _read_riot_value_req(&pkt, &buf[0]);

    coap_reply_simple(&pkt, COAP_CODE_CONTENT, buf, _BUF_SIZE,
                      COAP_FORMAT_TEXT, (uint8_t *)payload, 1);

    TEST_ASSERT(res > 0);
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
static ssize_t _read_riot_value_req_con(coap_pkt_t *pkt, uint8_t *buf)
{
    uint8_t pkt_data[] = {
        0x42, 0x01, 0xbe, 0x16, 0x35, 0x61, 0xb4, 0x72,
        0x69, 0x6f, 0x74, 0x05, 0x76, 0x61, 0x6c, 0x75,
        0x65
    };
    memcpy(buf, pkt_data, sizeof(pkt_data));

    return coap_parse_udp(pkt, buf, sizeof(pkt_data));
}

/* Builds on test_nanocoap__server_get_req to test confirmable request. */
static void test_nanocoap__server_get_req_con(void)
{
    uint8_t buf[_BUF_SIZE];
    coap_pkt_t pkt;

    ssize_t res = _read_riot_value_req_con(&pkt, &buf[0]);

    TEST_ASSERT(res > 0);
    TEST_ASSERT_EQUAL_INT(COAP_METHOD_GET, coap_get_code_decimal(&pkt));
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
     * than CONFIG_NANOCOAP_NOPTS_MAX) to coap_parse_udp().  The used coap_pkt_t is part
     * of a struct, followed by an array of 42 coap_option_t.  The array is
     * cleared before the call to coap_parse_udp().  If the overflow protection is
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

    ssize_t res = coap_parse_udp(&scratch.pkt, pkt_data, sizeof(pkt_data));

    /* check if any byte of the guard_data array is non-zero */
    int dirty = 0;
    uint8_t *pos = scratch.guard_data;
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
 * Verifies that coap_parse_udp() recognizes inclusion of too many options.
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
    ssize_t res = coap_parse_udp(&pkt, buf, sizeof(buf) - 2);
    TEST_ASSERT_EQUAL_INT(sizeof(buf) - 2, res);

    /* add option to overflow */
    memcpy(&buf[base_len+i], fill_opt, 2);

    res = coap_parse_udp(&pkt, buf, sizeof(buf));
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
static ssize_t _read_rd_post_req(coap_pkt_t *pkt, bool omit_payload)
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
    return coap_parse_udp(pkt, pkt_data, len);
}

/*
 * Tests use of coap_opt_get_next() to iterate over options.
 */
static void test_nanocoap__options_iterate(void)
{
    coap_pkt_t pkt;
    ssize_t res = _read_rd_post_req(&pkt, true);
    TEST_ASSERT(res > 0);

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
    TEST_ASSERT(res > 0);

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
    ssize_t res = _read_rd_post_req(&pkt, true);
    TEST_ASSERT(res > 0);

    /* read Uri-Query options */
    uint8_t *value;
    ssize_t optlen = coap_opt_get_opaque(&pkt, COAP_OPT_URI_QUERY, &value);
    TEST_ASSERT_EQUAL_INT(24, optlen);

    coap_optpos_t opt = {0, value + optlen - pkt.buf};

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
    ssize_t res = coap_parse_udp(&pkt, pkt_data, 4);

    TEST_ASSERT(res > 0);
    TEST_ASSERT_EQUAL_INT(0, coap_get_code_raw(&pkt));
    TEST_ASSERT_EQUAL_INT(msgid, coap_get_id(&pkt));
    TEST_ASSERT_EQUAL_INT(0, coap_get_token_len(&pkt));
    TEST_ASSERT_EQUAL_INT(0, pkt.payload_len);

    /* too short */
    memset(&pkt, 0, sizeof(coap_pkt_t));
    res = coap_parse_udp(&pkt, pkt_data, 3);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, res);

    /* too long */
    memset(&pkt, 0, sizeof(coap_pkt_t));
    res = coap_parse_udp(&pkt, pkt_data, 5);
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

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));
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

    size_t len = coap_build_udp_hdr(buf, sizeof(buf), COAP_TYPE_NON,
                                    &token[0], 2, COAP_METHOD_GET, msgid);

    coap_pkt_init(&pkt, &buf[0], sizeof(buf), len);
    TEST_ASSERT_EQUAL_INT(len, coap_get_total_hdr_len(&pkt));

    len = coap_opt_add_proxy_uri(&pkt, proxy_uri);

    /* strlen + 1 byte option number + 2 bytes length */
    TEST_ASSERT_EQUAL_INT(strlen(proxy_uri) + 3, len);

    char *uri;
    len = coap_get_proxy_uri(&pkt, (char **) &uri);

    TEST_ASSERT_EQUAL_INT(strlen(proxy_uri), len);
    TEST_ASSERT_EQUAL_INT(0, strncmp((char *) proxy_uri, (char *) uri, len));
}

/*
 * Verifies that coap_parse_udp() recognizes token length bigger than allowed.
 */
static void test_nanocoap__token_length_over_limit(void)
{
    /* RFC8974 states that TKL must not be 15:
     * 15: Reserved. This value MUST NOT be sent and MUST be processed
     * as a message-format error.
     */
    uint16_t msgid = 0xABCD;
    uint8_t buf_invalid[] = {
        0x4F, 0x01, 0xAB, 0xCD,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99
    };
    uint8_t buf_valid[] = {
        0x48, 0x01, 0xAB, 0xCD,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
    };
    coap_pkt_t pkt;

    /* Valid packet (TKL = 8) */
    ssize_t res = coap_parse_udp(&pkt, buf_valid, sizeof(buf_valid));

    TEST_ASSERT_EQUAL_INT(sizeof(buf_valid), res);
    TEST_ASSERT_EQUAL_INT(1, coap_get_code_raw(&pkt));
    TEST_ASSERT_EQUAL_INT(msgid, coap_get_id(&pkt));
    TEST_ASSERT_EQUAL_INT(8, coap_get_token_len(&pkt));
    TEST_ASSERT_EQUAL_INT(0, pkt.payload_len);

    /* Invalid packet (TKL = 15) */
    res = coap_parse_udp(&pkt, buf_invalid, sizeof(buf_invalid));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, res);
}

/*
 * Verifies that coap_parse_udp() recognizes 8 bit extended token length
 */
static void test_nanocoap__token_length_ext_16(void)
{
    const char *token = "0123456789ABCDEF";

    uint8_t buf[32];
    coap_udp_hdr_t *hdr = (void *)buf;

    /* build a request with an overlong token (that mandates the use of
     * an 8 bit extended token length field) */
    TEST_ASSERT_EQUAL_INT(21, coap_build_hdr(hdr, COAP_TYPE_CON,
                                             (void *)token, strlen(token),
                                             COAP_METHOD_DELETE, 23));

    /* parse the packet build, and verify it parses back as expected */
    coap_pkt_t pkt;
    ssize_t res = coap_parse_udp(&pkt, buf, 21);

    TEST_ASSERT_EQUAL_INT(21, res);
    TEST_ASSERT_EQUAL_INT(21, coap_get_total_hdr_len(&pkt));
    TEST_ASSERT_EQUAL_INT(COAP_METHOD_DELETE, coap_get_code_raw(&pkt));
    TEST_ASSERT_EQUAL_INT(23, coap_get_id(&pkt));
    TEST_ASSERT_EQUAL_INT(strlen(token), coap_get_token_len(&pkt));
    TEST_ASSERT_EQUAL_INT(0, memcmp(coap_get_token(&pkt), token, strlen(token)));
    TEST_ASSERT_EQUAL_INT(0, pkt.payload_len);
    TEST_ASSERT_EQUAL_INT(13, hdr->ver_t_tkl & 0xf);

    /* now build the corresponding reply and check that it parses back as
     * expected */
    uint8_t rbuf[sizeof(buf)];
    ssize_t len = coap_build_reply_header(&pkt, COAP_CODE_DELETED, rbuf,
                                          sizeof(rbuf), 0, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(21, len);
    res = coap_parse_udp(&pkt, rbuf, 21);
    TEST_ASSERT_EQUAL_INT(21, res);
    TEST_ASSERT_EQUAL_INT(21, coap_get_total_hdr_len(&pkt));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_DELETED, coap_get_code_raw(&pkt));
    TEST_ASSERT_EQUAL_INT(23, coap_get_id(&pkt));
    TEST_ASSERT_EQUAL_INT(strlen(token), coap_get_token_len(&pkt));
    TEST_ASSERT_EQUAL_INT(0, memcmp(coap_get_token(&pkt), token, strlen(token)));
    TEST_ASSERT_EQUAL_INT(0, pkt.payload_len);
    TEST_ASSERT_EQUAL_INT(13, hdr->ver_t_tkl & 0xf);
}

/*
 * Verifies that coap_parse_udp() recognizes 16 bit extended token length
 */
static void test_nanocoap__token_length_ext_269(void)
{
    const char *token = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr,"
                        "sed diam nonumy eirmod tempor invidunt ut labore et dolore"
                        "magna aliquyam erat, sed diam voluptua. At vero eos et accusam"
                        "et justo duo dolores et ea rebum. Stet clita kasd gubergren,"
                        "no sea takimata sanctus est Lore.";
    uint8_t buf[280];
    coap_udp_hdr_t *hdr = (void *)buf;

    /* build a request with an overlong token (that mandates the use of
     * an 16 bit extended token length field) */
    TEST_ASSERT_EQUAL_INT(275, coap_build_hdr(hdr, COAP_TYPE_CON,
                                             (void *)token, strlen(token),
                                             COAP_METHOD_DELETE, 23));

    /* parse the packet build, and verify it parses back as expected */
    coap_pkt_t pkt;
    ssize_t res = coap_parse_udp(&pkt, buf, 275);

    TEST_ASSERT_EQUAL_INT(275, res);
    TEST_ASSERT_EQUAL_INT(275, coap_get_total_hdr_len(&pkt));
    TEST_ASSERT_EQUAL_INT(COAP_METHOD_DELETE, coap_get_code_raw(&pkt));
    TEST_ASSERT_EQUAL_INT(23, coap_get_id(&pkt));
    TEST_ASSERT_EQUAL_INT(strlen(token), coap_get_token_len(&pkt));
    TEST_ASSERT_EQUAL_INT(0, memcmp(coap_get_token(&pkt), token, strlen(token)));
    TEST_ASSERT_EQUAL_INT(0, pkt.payload_len);
    TEST_ASSERT_EQUAL_INT(14, hdr->ver_t_tkl & 0xf);

    /* now build the corresponding reply and check that it parses back as
     * expected */
    uint8_t rbuf[sizeof(buf)];
    ssize_t len = coap_build_reply_header(&pkt, COAP_CODE_DELETED, rbuf,
                                          sizeof(rbuf), 0, NULL, NULL);

    TEST_ASSERT_EQUAL_INT(275, len);
    res = coap_parse_udp(&pkt, rbuf, 275);
    TEST_ASSERT_EQUAL_INT(275, res);
    TEST_ASSERT_EQUAL_INT(275, coap_get_total_hdr_len(&pkt));
    TEST_ASSERT_EQUAL_INT(COAP_CODE_DELETED, coap_get_code_raw(&pkt));
    TEST_ASSERT_EQUAL_INT(23, coap_get_id(&pkt));
    TEST_ASSERT_EQUAL_INT(strlen(token), coap_get_token_len(&pkt));
    TEST_ASSERT_EQUAL_INT(0, memcmp(coap_get_token(&pkt), token, strlen(token)));
    TEST_ASSERT_EQUAL_INT(0, pkt.payload_len);
    TEST_ASSERT_EQUAL_INT(14, hdr->ver_t_tkl & 0xf);
}

/*
 * Test that a RST message can be generated and parsed
 */
static void test_nanocoap___rst_message(void)
{
    static const uint8_t rst_expected[4] = {
        0x70, /* Version = 0b01, Type = 0b11 (RST), Token Length = 0b0000 */
        0x00, /* Code = 0x00 */
        0x13, 0x37 /* Message ID = 0x1337 */
    };

    uint8_t buf[16];
    /* trivial case: build a reset message */
    memset(buf, 0x55, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(rst_expected),
                          coap_build_hdr((void *)buf, COAP_TYPE_RST, NULL, 0,
                                         0, 0x1337));
    TEST_ASSERT(0 == memcmp(rst_expected, buf, sizeof(rst_expected)));
    /* did it write past the expected bytes? */
    TEST_ASSERT_EQUAL_INT(0x55, buf[sizeof(rst_expected)]);

    /* now check that parsing it back works */
    coap_pkt_t pkt;
    TEST_ASSERT_EQUAL_INT(sizeof(rst_expected), coap_parse_udp(&pkt, buf, sizeof(rst_expected)));
    TEST_ASSERT_EQUAL_INT(COAP_TYPE_RST, coap_get_type(&pkt));
    TEST_ASSERT_EQUAL_INT(0, coap_get_code_raw(&pkt));
    TEST_ASSERT_EQUAL_INT(0, coap_get_token_len(&pkt));

    /* now check that generating a RST reply works */
    static uint8_t con_request[8] = {
        0x44, /* Version = 0b01, Type = 0b00 (CON), Token Length = 0b0100 */
        0x01, /* Code = 0.01 (GET) */
        0x13, 0x37, /* Message ID = 0x1337 */
        0xde, 0xed, 0xbe, 0xef, /* Token = 0xdeadbeef */
    };
    memset(buf, 0x55, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(con_request), coap_parse_udp(&pkt, con_request, sizeof(con_request)));
    TEST_ASSERT_EQUAL_INT(sizeof(rst_expected), coap_build_reply(&pkt, 0, buf, sizeof(buf), 0));
    TEST_ASSERT(0 == memcmp(rst_expected, buf, sizeof(rst_expected)));
    TEST_ASSERT_EQUAL_INT(0x55, buf[sizeof(rst_expected)]);
}

/*
 * Test that invalid encoding of CoAP option is caught early, so that
 * later access to CoAP option does indeed not need to perform bound
 * checking.
 */
static void test_nanocoap__out_of_bounds_option(void)
{
    uint8_t invalid_msg_udp[] = {
        (COAP_V1 << 6) | (COAP_TYPE_CON << 4) | 3, /* version = 1, type = CON, Token Len = 3 */
        COAP_METHOD_GET,
        0x13, 0x37, /* Message ID = 0x1337 */
        0xca, 0xfe, 0x42, /* Token = 0xcafe42 */
         /* Option Delta: 11 (11 + 0 = 11 = URI-Path)
          * Option Length: 8 */
        (COAP_OPT_URI_PATH << 4) | (8),
        0x13, 0x37, 0x42, 0x42 /* 4 bytes Option Data */
        /* End of packet - 4 bytes before the claimed end of option */
    };

    coap_pkt_t pkt;
    TEST_ASSERT_EQUAL_INT(-EBADMSG, coap_parse_udp(&pkt, invalid_msg_udp, sizeof(invalid_msg_udp)));
}

Test *tests_nanocoap_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nanocoap__hdr),
        new_TestFixture(test_nanocoap__hdr_2),
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
        new_TestFixture(test_nanocoap__option_remove_delta_1),
        new_TestFixture(test_nanocoap__option_remove_delta_7),
        new_TestFixture(test_nanocoap__option_remove_delta_13),
        new_TestFixture(test_nanocoap__option_remove_delta_32),
        new_TestFixture(test_nanocoap__option_remove_delta_135),
        new_TestFixture(test_nanocoap__option_remove_delta_269),
        new_TestFixture(test_nanocoap__option_remove_delta_512),
        new_TestFixture(test_nanocoap__option_remove_no_payload),
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
        new_TestFixture(test_nanocoap__token_length_ext_16),
        new_TestFixture(test_nanocoap__token_length_ext_269),
        new_TestFixture(test_nanocoap___rst_message),
        new_TestFixture(test_nanocoap__out_of_bounds_option),
    };

    EMB_UNIT_TESTCALLER(nanocoap_tests, NULL, NULL, fixtures);

    return (Test *)&nanocoap_tests;
}

void tests_nanocoap(void)
{
    TESTS_RUN(tests_nanocoap_tests());
}
/** @} */
