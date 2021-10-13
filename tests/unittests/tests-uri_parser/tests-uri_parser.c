/*
 * Copyright (C) 2020 HAW Hamburg
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

#include <stdio.h>
#include <stdbool.h>
#include "embUnit.h"

#include "uri_parser.h"

#include "unittests-constants.h"
#include "tests-uri_parser.h"

#define VEC(u, f, s, us, h, v6a, z, po, pa, q, e)                           \
    { .uri = u, .full_uri = f, .scheme = s, .userinfo = us, .host = h,      \
      .ipv6addr = v6a, .zoneid = z, .port = po, .path = pa,                 \
      .query = q, .expected = e}

#define VEC_CHECK(comp, i, vec_msg)                                         \
    do {                                                                    \
        if (ures.comp == NULL) {                                            \
            TEST_ASSERT(validate_uris[i].comp[0] == '\0');                  \
        }                                                                   \
        else {                                                              \
            TEST_ASSERT_EQUAL_INT(strlen(validate_uris[i].comp),            \
                                  ures.comp##_len);                         \
            vec_msg[0] = '\0';                                              \
            stdimpl_strcat(vec_msg, "Unexpected " # comp " member \"");     \
            stdimpl_strcat(vec_msg, validate_uris[i].comp);                 \
            stdimpl_strcat(vec_msg, "\" for \"");                           \
            stdimpl_strcat(vec_msg, validate_uris[i].uri);                  \
            stdimpl_strcat(vec_msg, "\"");                                  \
            TEST_ASSERT_MESSAGE(0 ==                                        \
                                  strncmp(ures.comp,                        \
                                          validate_uris[i].comp,            \
                                          strlen(validate_uris[i].comp)),   \
                                vec_msg);                                   \
        }                                                                   \
    } while (0)

#define VEC_MSG_LEN (sizeof("Unexpected userinfo member \"\" for \"\"") + \
                     64U + 8U)

typedef struct {
    char uri[64];
    bool full_uri;
    char scheme[8];
    char userinfo[16];
    char host[24];
    char ipv6addr[16];
    char zoneid[8];
    char port[32];
    char path[48];
    char query[32];
    int expected;
} validate_t;

/*
  VEC(uri_to_parse,
      scheme, userinfo, host, port,
      path, query, expected return value)
*/
static const validate_t validate_uris[] = {
        /* uri to parse */
    VEC("coap://RIOT:test@[fe80:db8::1%tap0]:5683/.well-known/core?v=1",
        /* is URI */
        true,
        /* parsed scheme */
        "coap",
        /* parsed userinfo */
        "RIOT:test",
        /* parsed host */
        "[fe80:db8::1%tap0]",
        /* parsed host without zoneid */
        "fe80:db8::1",
        /* parsed zoneid */
        "tap0",
        /* parsed port */
        "5683",
        /* parsed path */
        "/.well-known/core",
        /* parsed query */
        "v=1",
        /* expected return value */
        0),
    VEC("coap://RIOT:test@[fe80:db8::1%]:5683/.well-known/core?v=1",
        true,
        "coap",
        "RIOT:test",
        "[fe80:db8::1%]",
        "fe80:db8::1",
        "",
        "5683",
        "/.well-known/core",
        "v=1",
        -1),
    VEC("coap://[fe80::1]/foo%20bar",
        true,
        "coap",
        "",
        "[fe80::1]",
        "fe80::1",
        "",
        "",
        "/foo%20bar",
        "",
        0),
    VEC("/.well-known/core?v=1",
        false,
        "",
        "",
        "",
        "",
        "",
        "",
        "/.well-known/core",
        "v=1",
        0),
    VEC("coap://R@[2001:db8::1]:5own/v=1",
        true,
        "coap",
        "R",
        "[2001:db8::1]",
        "2001:db8::1",
        "",
        "5own",
        "/v=1",
        "",
        0),
    VEC("coap://R@[2001:db8::1]:5own/:v=1",
        true,
        "coap",
        "R",
        "[2001:db8::1]",
        "2001:db8::1",
        "",
        "5own",
        "/:v=1",
        "",
        0),
    VEC("cap://R@[2001:db8::1]:5own/?v=1",
        true,
        "cap",
        "R",
        "[2001:db8::1]",
        "2001:db8::1",
        "",
        "5own",
        "/",
        "v=1",
        0),
    VEC("oap://Y2001:db8::1]:5own/av=1",
        true,
        "oap",
        "",
        "Y2001:db8::1]",
        "",
        "",
        "5own",
        "/av=1",
        "",
        0),
    VEC("//Rb[ʰ00J:d/5v=0",
        false,
        "",
        "",
        "",
        "",
        "",
        "",
        "//Rb[ʰ00J:d/5v=0",
        "",
        0),
    VEC("coap://oap://P@[2001:b",
        true,
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        -1),
    VEC("coap:///R@[2008::1]:5own//R@[2008::1]:5own/?v=1",
        true,
        "coap",
        "",
        "",
        "",
        "",
        "",
        "/R@[2008::1]:5own//R@[2008::1]:5own/",
        "v=1",
        0),
    VEC("coaP://R/RZ[2001[8:01[8::1]:5o:1]:5oTMv=1",
        true,
        "coaP",
        "",
        "R",
        "",
        "",
        "",
        "/RZ[2001[8:01[8::1]:5o:1]:5oTMv=1",
        "",
        0),
    VEC("coap://R@////////////////7///v=1",
        true,
        "coap",
        "R",
        "",
        "",
        "",
        "",
        "////////////////7///v=1",
        "",
        0),
    VEC("coa[:////[2001:db5ow:5own/Ov=1",
        false,
        "",
        "",
        "",
        "",
        "",
        "",
        "coa[:////[2001:db5ow:5own/Ov=1",
        "",
        0),
    VEC("tel:+1-816-555-1212",
        true,
        "tel",
        "",
        "",
        "",
        "",
        "",
        "+1-816-555-1212",
        "",
        0),
    VEC("sms:+15105550101,+15105550102?body=hello%20there",
        true,
        "sms",
        "",
        "",
        "",
        "",
        "",
        "+15105550101,+15105550102",
        "body=hello%20there",
        0),
    VEC("a",
        false,
        "",
        "",
        "",
        "",
        "",
        "",
        "a",
        "",
        0),
    VEC("mailto:test@example.com",
        true,
        "mailto",
        "",
        "",
        "",
        "",
        "",
        "test@example.com",
        "",
        0),
    VEC("ftp://ftp.is.co.za/rfc/rfc1808.txt",
        true,
        "ftp",
        "",
        "ftp.is.co.za",
        "",
        "",
        "",
        "/rfc/rfc1808.txt",
        "",
        0),
    VEC("http://www.ietf.org/rfc/rfc2396.txt",
        true,
        "http",
        "",
        "www.ietf.org",
        "",
        "",
        "",
        "/rfc/rfc2396.txt",
        "",
        0),
    VEC("ldap://[2001:db8::7]/c=GB?objectClass?one",
        true,
        "ldap",
        "",
        "[2001:db8::7]",
        "2001:db8::7",
        "",
        "",
        "/c=GB",
        "objectClass?one",
        0),
    VEC("mailto:John.Doe@example.com",
        true,
        "mailto",
        "",
        "",
        "",
        "",
        "",
        "John.Doe@example.com",
        "",
        0),
    VEC("news:comp.infosystems.www.servers.unix",
        true,
        "news",
        "",
        "",
        "",
        "",
        "",
        "comp.infosystems.www.servers.unix",
        "",
        0),
    VEC("tel:+1-816-555-1212",
        true,
        "tel",
        "",
        "",
        "",
        "",
        "",
        "+1-816-555-1212",
        "",
        0),
    VEC("telnet://192.0.2.16:80/",
        true,
        "telnet",
        "",
        "192.0.2.16",
        "",
        "",
        "80",
        "/",
        "",
        0),
    VEC("urn:oasis:names:specification:docbook:dtd:xml:4.1.2",
        true,
        "urn",
        "",
        "",
        "",
        "",
        "",
        "oasis:names:specification:docbook:dtd:xml:4.1.2",
        "",
        0),
    VEC("",
        false,
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        -1),
    VEC("/",
        false,
        "",
        "",
        "",
        "",
        "",
        "",
        "/",
        "",
        0),
    VEC("./this:that",
        false,
        "",
        "",
        "",
        "",
        "",
        "",
        "./this:that",
        "",
        0),
    VEC("pP://",
        true,
        "pP",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        0),
    VEC("A://@",
        true,
        "A",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        0),
};

static char _failure_msg[VEC_MSG_LEN];

static void test_uri_parser__validate(void)
{
    uri_parser_result_t ures;
    for (unsigned i = 0; i < ARRAY_SIZE(validate_uris); i++) {
        int res = uri_parser_process_string(&ures, validate_uris[i].uri);
        TEST_ASSERT_EQUAL_INT(validate_uris[i].full_uri,
               uri_parser_is_absolute_string(validate_uris[i].uri));
        TEST_ASSERT_EQUAL_INT(validate_uris[i].expected, res);
        if (res == 0) {
            VEC_CHECK(scheme, i, _failure_msg);
            VEC_CHECK(userinfo, i, _failure_msg);
            VEC_CHECK(host, i, _failure_msg);
            VEC_CHECK(ipv6addr, i, _failure_msg);
            VEC_CHECK(zoneid, i, _failure_msg);
            VEC_CHECK(port, i, _failure_msg);
            VEC_CHECK(path, i, _failure_msg);
            VEC_CHECK(query, i, _failure_msg);
        }
    }
}

static void test_uri_parser__unterminated_string(void)
{
    uri_parser_result_t ures;
    char uri[64];
    /* initialize with a non-null character */
    memset(uri, 'Z', sizeof(uri));

    memcpy(uri, validate_uris[0].uri, strlen(validate_uris[0].uri));

    int res = uri_parser_process(&ures, uri, strlen(validate_uris[0].uri));

    TEST_ASSERT_EQUAL_INT(0, res);
    VEC_CHECK(scheme, 0, _failure_msg);
    VEC_CHECK(userinfo, 0, _failure_msg);
    VEC_CHECK(host, 0, _failure_msg);
    VEC_CHECK(ipv6addr, 0, _failure_msg);
    VEC_CHECK(zoneid, 0, _failure_msg);
    VEC_CHECK(port, 0, _failure_msg);
    VEC_CHECK(path, 0, _failure_msg);
    VEC_CHECK(query, 0, _failure_msg);
}

Test *tests_uri_parser_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_uri_parser__validate),
        new_TestFixture(test_uri_parser__unterminated_string),
    };

    EMB_UNIT_TESTCALLER(uri_parser_tests, NULL, NULL, fixtures);

    return (Test *)&uri_parser_tests;
}

#define PARAMS_NUMOF                (4U)
#define INIT_URI_RESULTS(q) \
    _uri_results.query = (q); \
    _uri_results.query_len = sizeof(q) - 1
#define TEST_ASSERT_PARAM(exp, idx, comp) \
    TEST_ASSERT_EQUAL_INT(sizeof(exp) - 1, _params[idx].comp ## _len); \
    TEST_ASSERT_MESSAGE(!strncmp(exp, _params[idx].comp, _params[idx].comp ## _len), \
                        #comp " was not " exp);

static uri_parser_query_param_t _params[4U];
static uri_parser_result_t _uri_results;

static void _setup_query(void)
{
    memset(_params, 0, sizeof(_params));
    memset(&_uri_results, 0, sizeof(_uri_results));
}

static void test_split_query__broken_input(void)
{
    int res;

    INIT_URI_RESULTS("&");
    res = uri_parser_split_query(&_uri_results, _params, ARRAY_SIZE(_params));
    TEST_ASSERT_EQUAL_INT(-1, res);

    INIT_URI_RESULTS("=&");
    res = uri_parser_split_query(&_uri_results, _params, ARRAY_SIZE(_params));
    TEST_ASSERT_EQUAL_INT(-1, res);

    INIT_URI_RESULTS("=&&");
    res = uri_parser_split_query(&_uri_results, _params, ARRAY_SIZE(_params));
    TEST_ASSERT_EQUAL_INT(-1, res);

    INIT_URI_RESULTS("==");
    res = uri_parser_split_query(&_uri_results, _params, ARRAY_SIZE(_params));
    TEST_ASSERT_EQUAL_INT(-1, res);

    INIT_URI_RESULTS("key=value&name=value=another");
    res = uri_parser_split_query(&_uri_results, _params, ARRAY_SIZE(_params));
    TEST_ASSERT_EQUAL_INT(-1, res);
}

void test_split_query__truncated(void)
{
    int res;

    INIT_URI_RESULTS("this=0&is=1&a=very&long=3&query=foo");
    TEST_ASSERT_EQUAL_INT(4, ARRAY_SIZE(_params));
    res = uri_parser_split_query(&_uri_results, _params, ARRAY_SIZE(_params));
    TEST_ASSERT_EQUAL_INT(-2, res);
    TEST_ASSERT_PARAM("this", 0, name);
    TEST_ASSERT_PARAM("0", 0, value);
    TEST_ASSERT_PARAM("is", 1, name);
    TEST_ASSERT_PARAM("1", 1, value);
    TEST_ASSERT_PARAM("a", 2, name);
    TEST_ASSERT_PARAM("very", 2, value);
    TEST_ASSERT_PARAM("long", 3, name);
    TEST_ASSERT_PARAM("3", 3, value);
}

void test_split_query__success(void)
{
    int res;

    INIT_URI_RESULTS("foo=&=&bar=1");
    res = uri_parser_split_query(&_uri_results, _params, ARRAY_SIZE(_params));
    TEST_ASSERT_EQUAL_INT(3, res);
    TEST_ASSERT_PARAM("foo", 0, name);
    TEST_ASSERT_PARAM("", 0, value);
    TEST_ASSERT_PARAM("", 1, name);
    TEST_ASSERT_PARAM("", 1, value);
    TEST_ASSERT_PARAM("bar", 2, name);
    TEST_ASSERT_PARAM("1", 2, value);
    TEST_ASSERT_EQUAL_INT(0, _params[3].name_len);
    TEST_ASSERT_NULL(_params[3].name);
    TEST_ASSERT_EQUAL_INT(0, _params[3].value_len);
    TEST_ASSERT_NULL(_params[3].value);
}

Test *tests_query_split_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_split_query__broken_input),
        new_TestFixture(test_split_query__truncated),
        new_TestFixture(test_split_query__success),
    };

    EMB_UNIT_TESTCALLER(query_split_tests, _setup_query, NULL, fixtures);

    return (Test *)&query_split_tests;
}

void tests_uri_parser(void)
{
    TESTS_RUN(tests_uri_parser_tests());
    TESTS_RUN(tests_query_split_tests());
}
/** @} */
