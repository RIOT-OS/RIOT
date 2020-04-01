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
#include "embUnit.h"

#include "uri_parser.h"

#include "unittests-constants.h"
#include "tests-uri_parser.h"

#define VEC(u, s, us, h, po, pa, q, e)                                  \
    { .uri = u, .scheme = s, .userinfo = us, .host = h, .port = po,     \
      .path = pa, .query = q, .expected = e}

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
    char scheme[8];
    char userinfo[16];
    char host[16];
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
static const validate_t validate_uris[26] = {
        /* uri to parse */
    VEC("coap://RIOT:test@[2001:db8::1]:5683/.well-known/core?v=1",
        /* parsed scheme */
        "coap",
        /* parsed userinfo */
        "RIOT:test",
        /* parsed host */
        "[2001:db8::1]",
        /* parsed port */
        "5683",
        /* parsed path */
        "/.well-known/core",
        /* parsed query */
        "v=1",
        /* expected return value */
        0),
    VEC("/.well-known/core?v=1",
        "",
        "",
        "",
        "",
        "/.well-known/core",
        "v=1",
        0),
    VEC("coap://R@[2001:db8::1]:5own/v=1",
        "coap",
        "R",
        "[2001:db8::1]",
        "5own",
        "/v=1",
        "",
        0),
    VEC("coap://R@[2001:db8::1]:5own/:v=1",
        "coap",
        "R",
        "[2001:db8::1]",
        "5own",
        "/:v=1",
        "",
        0),
    VEC("cap://R@[2001:db8::1]:5own/?v=1",
        "cap",
        "R",
        "[2001:db8::1]",
        "5own",
        "/",
        "v=1",
        0),
    VEC("oap://Y2001:db8::1]:5own/av=1",
        "oap",
        "",
        "Y2001:db8::1]",
        "5own",
        "/av=1",
        "",
        0),
    VEC("//Rb[ʰ00J:d/5v=0",
        "",
        "",
        "",
        "",
        "//Rb[ʰ00J:d/5v=0",
        "",
        0),
    VEC("coap://oap://P@[2001:b",
        "",
        "",
        "",
        "",
        "",
        "",
        -1),
    VEC("coap:///R@[2008::1]:5own//R@[2008::1]:5own/?v=1",
        "coap",
        "",
        "",
        "",
        "/R@[2008::1]:5own//R@[2008::1]:5own/",
        "v=1",
        0),
    VEC("coaP://R/RZ[2001[8:01[8::1]:5o:1]:5oTMv=1",
        "coaP",
        "",
        "R",
        "",
        "/RZ[2001[8:01[8::1]:5o:1]:5oTMv=1",
        "",
        0),
    VEC("coap://R@////////////////7///v=1",
        "",
        "",
        "",
        "",
        "",
        "",
        -1),
    VEC("coa[:////[2001:db5ow:5own/Ov=1",
        "coa[",
        "",
        "",
        "",
        "//[2001:db5ow:5own/Ov=1",
        "",
        0),
    VEC("tel:+1-816-555-1212",
        "tel",
        "",
        "",
        "",
        "+1-816-555-1212",
        "",
        0),
    VEC("sms:+15105550101,+15105550102?body=hello%20there",
        "sms",
        "",
        "",
        "",
        "+15105550101,+15105550102",
        "body=hello%20there",
        0),
    VEC("a",
        "",
        "",
        "",
        "",
        "a",
        "",
        0),
    VEC("mailto:test@example.com",
        "mailto",
        "",
        "",
        "",
        "test@example.com",
        "",
        0),
    VEC("ftp://ftp.is.co.za/rfc/rfc1808.txt",
        "ftp",
        "",
        "ftp.is.co.za",
        "",
        "/rfc/rfc1808.txt",
        "",
        0),
    VEC("http://www.ietf.org/rfc/rfc2396.txt",
        "http",
        "",
        "www.ietf.org",
        "",
        "/rfc/rfc2396.txt",
        "",
        0),
    VEC("ldap://[2001:db8::7]/c=GB?objectClass?one",
        "ldap",
        "",
        "[2001:db8::7]",
        "",
        "/c=GB",
        "objectClass?one",
        0),
    VEC("mailto:John.Doe@example.com",
        "mailto",
        "",
        "",
        "",
        "John.Doe@example.com",
        "",
        0),
    VEC("news:comp.infosystems.www.servers.unix",
        "news",
        "",
        "",
        "",
        "comp.infosystems.www.servers.unix",
        "",
        0),
    VEC("tel:+1-816-555-1212",
        "tel",
        "",
        "",
        "",
        "+1-816-555-1212",
        "",
        0),
    VEC("telnet://192.0.2.16:80/",
        "telnet",
        "",
        "192.0.2.16",
        "80",
        "/",
        "",
        0),
    VEC("urn:oasis:names:specification:docbook:dtd:xml:4.1.2",
        "urn",
        "",
        "",
        "",
        "oasis:names:specification:docbook:dtd:xml:4.1.2",
        "",
        0),
    VEC("",
        "",
        "",
        "",
        "",
        "",
        "",
        -1),
    VEC("/",
        "",
        "",
        "",
        "",
        "/",
        "",
        0),
};

static char _failure_msg[VEC_MSG_LEN];

static void test_uri_parser__validate(void)
{
    uri_parser_result_t ures;
    for (unsigned i = 0; i < ARRAY_SIZE(validate_uris); i++) {
        int res = uri_parser_process_string(&ures, validate_uris[i].uri);
        TEST_ASSERT_EQUAL_INT(validate_uris[i].expected, res);
        if (res == 0) {
            VEC_CHECK(scheme, i, _failure_msg);
            VEC_CHECK(userinfo, i, _failure_msg);
            VEC_CHECK(host, i, _failure_msg);
            VEC_CHECK(port, i, _failure_msg);
            VEC_CHECK(path, i, _failure_msg);
            VEC_CHECK(query, i, _failure_msg);
        }
    }
}

Test *tests_uri_parser_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_uri_parser__validate),
    };

    EMB_UNIT_TESTCALLER(uri_parser_tests, NULL, NULL, fixtures);

    return (Test *)&uri_parser_tests;
}

void tests_uri_parser(void)
{
    TESTS_RUN(tests_uri_parser_tests());
}
/** @} */
