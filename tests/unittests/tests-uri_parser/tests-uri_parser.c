/*
 * Copyright (C) 2022 Bennet Blischke / HAW Hamburg
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
#include "architecture.h"
#include "container.h"
#include "embUnit.h"

#include "uri_parser.h"

#include "unittests-constants.h"
#include "tests-uri_parser.h"

typedef struct {
    char *input_uri;
    char *scheme;
    char *userinfo;
    char *host;
    char *ipv6addr;
    char *zoneid;
    char *port_str;
    uint16_t port;
    char *path;
    char *query;
} expected_uri_result_t;

static void _print_return_expectation(const char *uri, int expected, int observed)
{
    printf("\nURI-Input: %s\n", uri);
    printf("Expected return: %d\n", expected);
    printf("Observed return: %d", observed);
}

/*
 * The uri_parser returns a string-buffer (non zero terminated).
 * This function compares a given string with a given uri_parser buffer + length.
 * Additionally, it takes the original uri (from the test vector) and a name
 * in order to print a precise and helpful error message if the buffer and string
 * aren't identical.
 */
static int _compare_string_buffer(const char *name, const char *input_uri, const char *expected_str,
                                  const char *actual_str, size_t actual_len)
{
    if ((actual_len == strlen(expected_str)) &&
        (strncmp(actual_str, expected_str, actual_len) == 0)) {
        return 0;
    }
    printf("\nWith given input uri '%s', expected %s '%s' with length '%"
           PRIuSIZE "' but got '%.*s' with length '%" PRIuSIZE "'\n",
           input_uri, name, expected_str, strlen(expected_str),
           (unsigned) actual_len, actual_str, actual_len);
    return -1;
}

/*
 * Given a simple valid uri, this test checks that the parsing returns success
 */
static void _successful_parsing_of_valid_uri(void)
{
    char *failure_msg = "Failure: The uri_parser failed to parse a correct and valid uri.";
    int expected_ret = 0;  /* Indicates a successful parsing */

    char *simple_uri_1 = "coap://example.org/foo/bar";
    char *simple_uri_2 = "ftp://riot-os.org/bar/foo";
    char *simple_uri_3 = "ftp://riot-os.org:99/bar/foo";
    char *simple_uri_4 = "http://riot-os.org:99/bar/foo";
    char *simple_uri_5 = "https://riot-os.org/";

    char *legacy_test_case_01 = "coap://RIOT:test@[fe80:db8::1%tap0]:5683/.well-known/core?v=1";
    char *legacy_test_case_02 = "coap://[fe80::1]/foo%20bar";
    char *legacy_test_case_03 = "/.well-known/core?v=1";
    char *legacy_test_case_04 = "coap://R@[2001:db8::1]:5/v=1";
    char *legacy_test_case_05 = "coap://R@[2001:db8::1]:5/:v=1";
    char *legacy_test_case_06 = "cap://R@[2001:db8::1]:5/?v=1";
    char *legacy_test_case_07 = "oap://Y2001:db8::1]:5/av=1";
    char *legacy_test_case_08 = "//Rb[ʰ00J:d/5v=0";
    char *legacy_test_case_09 = "coap:///R@[2008::1]:5own//R@[2008::1]:5own/?v=1";
    char *legacy_test_case_10 = "coaP://R/RZ[2001[8:01[8::1]:5o:1]:5oTMv=1";
    char *legacy_test_case_11 = "coap://R@////////////////7///v=1";
    char *legacy_test_case_12 = "coa[:////[2001:db5ow:5own/Ov=1";
    char *legacy_test_case_13 = "tel:+1-816-555-1212";
    char *legacy_test_case_14 = "sms:+15105550101,+15105550102?body=hello%20there";
    char *legacy_test_case_15 = "a";
    char *legacy_test_case_16 = "mailto:test@example.com";
    char *legacy_test_case_17 = "ftp://ftp.is.co.za/rfc/rfc1808.txt";
    char *legacy_test_case_18 = "http://www.ietf.org/rfc/rfc2396.txt";
    char *legacy_test_case_19 = "ldap://[2001:db8::7]/c=GB?objectClass?one";
    char *legacy_test_case_20 = "mailto:John.Doe@example.com";
    char *legacy_test_case_21 = "news:comp.infosystems.www.servers.unix";
    char *legacy_test_case_22 = "tel:+1-816-555-1212";
    char *legacy_test_case_23 = "telnet://192.0.2.16:80/";
    char *legacy_test_case_24 = "urn:oasis:names:specification:docbook:dtd:xml:4.1.2";
    char *legacy_test_case_25 = "/";
    char *legacy_test_case_26 = "./this:that";
    char *legacy_test_case_27 = "pP://";
    char *legacy_test_case_28 = "A://@";

    char *test_vec[] = {
        simple_uri_1,
        simple_uri_2,
        simple_uri_3,
        simple_uri_4,
        simple_uri_5,
        legacy_test_case_01,
        legacy_test_case_02,
        legacy_test_case_03,
        legacy_test_case_04,
        legacy_test_case_05,
        legacy_test_case_06,
        legacy_test_case_07,
        legacy_test_case_08,
        legacy_test_case_09,
        legacy_test_case_10,
        legacy_test_case_11,
        legacy_test_case_12,
        legacy_test_case_13,
        legacy_test_case_14,
        legacy_test_case_15,
        legacy_test_case_16,
        legacy_test_case_17,
        legacy_test_case_18,
        legacy_test_case_19,
        legacy_test_case_20,
        legacy_test_case_21,
        legacy_test_case_22,
        legacy_test_case_23,
        legacy_test_case_24,
        legacy_test_case_25,
        legacy_test_case_26,
        legacy_test_case_27,
        legacy_test_case_28,
    };

    uri_parser_result_t uri_res;
    int ret = 0;

    for (unsigned int i = 0; i < ARRAY_SIZE(test_vec); ++i) {
        ret = uri_parser_process_string(&uri_res, test_vec[i]);
        if (ret != expected_ret) {
            _print_return_expectation(test_vec[i], expected_ret, ret);
            TEST_FAIL(failure_msg);
        }
    }
}

/*
 * Given a simple invalid uri, this test checks that the parsing returns an error
 */
static void _successful_rejecting_of_invalid_uri(void)
{
    char *failure_msg = "Failure: The uri_parser failed to reject an invalid uri.";
    int expected_ret = -1;  /* Indicates an invalid uri */

    char *trailing_percent = "coap://RIOT:test@[fe80:db8::1%]:5683/.well-known/core?v=1";
    char *invalid_port = "coap://R@[2001:db8::1]:5own/v=1";
    char *invalid_port_colon = "coap://R@[2001:db8::1]:5own/:v=1";
    char *invalid_port_short_scheme = "cap://R@[2001:db8::1]:5own/?v=1";
    char *invalid_host = "oap://Y2001:db8::1]:5own/av=1";
    char *double_scheme = "coap://oap://P@[2001:b";
    char *empty_input = "";
    char *port_too_long = "coap://example.com:1234568910";
    char *white_space_in_port = "coap://example.com: 12/foo";
    char *hex_port = "coap://example.com:0x12/foo";

    char *test_vec[] = {
        trailing_percent,
        invalid_port,
        invalid_port_colon,
        invalid_port_short_scheme,
        invalid_host,
        double_scheme,
        empty_input,
        port_too_long,
        white_space_in_port,
        hex_port,
    };

    uri_parser_result_t uri_res;
    int ret = 0;

    for (unsigned int i = 0; i < ARRAY_SIZE(test_vec); ++i) {
        ret = uri_parser_process_string(&uri_res, test_vec[i]);
        if (ret != expected_ret) {
            _print_return_expectation(test_vec[i], expected_ret, ret);
            TEST_FAIL(failure_msg);
        }
    }
}

/*
 * This test checks if the uri_parser returns an error if the port length
 * (as a string) is invalid, that is, longer than 5 characters.
 * This differs from the RFC, where the port in an uri can be zero to infinite
 * number of characters long; ABNF: *DIGIT
 */
static void _error_if_port_str_is_too_long(void)
{
    char *failure_msg = "Failure: The uri_parser did not detect an invalid port-length as invalid.";
    int expected_ret = -1; /* Indicates an invalid uri */

    char *leading_zeroes_arent_ignored = "https://example.org:000456/foo/bar";
    char *too_many_digits = "https://example.org:123456/foo/bar";
    char *too_much_whitespace = "https://example.org: 23456/foo/bar";

    char *test_vec[] = {
        too_many_digits,
        leading_zeroes_arent_ignored,
        too_much_whitespace,
    };

    uri_parser_result_t uri_res;
    int ret = 0;

    for (unsigned int i = 0; i < ARRAY_SIZE(test_vec); ++i) {
        ret = uri_parser_process_string(&uri_res, test_vec[i]);
        if (ret != expected_ret) {
            _print_return_expectation(test_vec[i], expected_ret, ret);
            TEST_FAIL(failure_msg);
        }
    }
}

/*
 * This test checks if the uri_parser returns an error if the port
 * (as a string) is invalid, that is, containing illegal characters
 */
static void _error_if_port_str_contains_illegal_characters(void)
{
    char *failure_msg = "Failure: The uri_parser did not detect an invalid port-string as invalid.";
    int expected_ret = -1; /* Indicates an invalid uri */

    char *letters_within_a_number = "https://example.org:12ff34/foo/bar";
    char *hex_number_as_port = "https://example.org:0x1234/foo/bar";
    char *bin_number_as_port = "https://example.org:0b1010/foo/bar";
    char *negativ_port = "https://example.org:-12/foo/bar";
    char *whitespace_within_port = "https://example.org:12 34/foo/bar";
    char *bigger_than_max_port = "https://example.org:65536/foo/bar";

    char *test_vec[] = {
        letters_within_a_number,
        hex_number_as_port,
        bin_number_as_port,
        negativ_port,
        whitespace_within_port,
        bigger_than_max_port
    };

    uri_parser_result_t uri_res;
    int ret = 0;

    for (unsigned int i = 0; i < ARRAY_SIZE(test_vec); ++i) {
        ret = uri_parser_process_string(&uri_res, test_vec[i]);
        if (ret != expected_ret) {
            _print_return_expectation(test_vec[i], expected_ret, ret);
            TEST_FAIL(failure_msg);
        }
    }
}

/*
 * Given a uri which contains a scheme, this test checks that the
 * uri_parser can isolate that scheme successful.
 */
static void _result_component_scheme_matches_input_scheme(void)
{
    char *failure_msg = "Failure: The uri_parser did not parse the scheme correctly.";
    int expected_ret = 0;

    expected_uri_result_t very_long_scheme = {
        .input_uri = "wowlongcoap://example.org/foo/bar",
        .scheme = "wowlongcoap",
    };
    expected_uri_result_t normal_scheme = {
        .input_uri = "coap://example.org/foo/bar",
        .scheme = "coap",
    };
    expected_uri_result_t short_scheme = {
        .input_uri = "a://example.org/foo/bar",
        .scheme = "a",
    };
    expected_uri_result_t digit_scheme = {
        .input_uri = "a1b2://example.org/foo/bar",
        .scheme = "a1b2",
    };

    expected_uri_result_t *test_vec[] = {
        &very_long_scheme,
        &normal_scheme,
        &short_scheme,
        &digit_scheme,
    };

    uri_parser_result_t uri_res;
    int ret = 0;

    for (unsigned int i = 0; i < ARRAY_SIZE(test_vec); ++i) {
        ret = uri_parser_process_string(&uri_res, test_vec[i]->input_uri);
        if (ret != expected_ret) {
            /* if the uri_parser return indicates an error, this test fails */
            _print_return_expectation(test_vec[i]->input_uri, expected_ret, ret);
            TEST_FAIL(failure_msg);
        }
        else {
            /* if the uri_parser return indicates success
             * the length of the schemes must match.
             * we can't use strlen on the result scheme as it is not null terminated,
             * but the uri_parser provides the length separately*/
            if (uri_res.scheme_len != strlen(test_vec[i]->scheme)) {
                printf(
                    "With given input uri '%s', expected a scheme with the length '%" PRIuSIZE "' but got '%d'\n",
                    test_vec[i]->input_uri, strlen(test_vec[i]->scheme), uri_res.scheme_len);
                TEST_FAIL(failure_msg);
            }
            else {
                /* If the schemes have the same length, they also should look identical */
                if (strncmp(uri_res.scheme, test_vec[i]->scheme, uri_res.scheme_len) != 0) {
                    printf("With given input uri '%s', expected scheme '%s' but got '%.*s'\n",
                           test_vec[i]->input_uri, test_vec[i]->scheme, uri_res.scheme_len,
                           uri_res.scheme);
                    TEST_FAIL(failure_msg);
                }
            }
        }
    }
}

/*
 * Given a valid uri, this test checks that the
 * uri_parser can isolate all parts of it successfully.
 */
static void _result_components_matches_input(void)
{
    char *failure_msg = "Failure: The uri_parser did not parse the uri correctly.";
    int expected_ret = 0;

    expected_uri_result_t uri_0 = {
        .input_uri = "coap://example.org/foo/bar",
        .scheme = "coap",
        .userinfo = "", /* This is an empty string because no userinfo has been set in this uri */
        .host = "example.org",
        .ipv6addr = "", /* This is an empty string because a hostname was used instead in this uri */
        .zoneid = "",   /* Not applicable without ipv6 */
        .port_str = "", /* This is an empty string because no port has been set in this uri */
        .port = 0,      /* Remains zero when no port is given */
        .path = "/foo/bar",
        .query = ""     /* This is an empty string because no query is present in this uri */
    };
    expected_uri_result_t uri_trailing_slash = {
        .input_uri = "coap://example.org/",
        .scheme = "coap",
        .userinfo = "",
        .host = "example.org",
        .ipv6addr = "",
        .zoneid = "",
        .port_str = "",
        .port = 0,
        .path = "/",
        .query = ""
    };
    expected_uri_result_t uri_without_trailing_slash = {
        .input_uri = "coap://example.org",
        .scheme = "coap",
        .userinfo = "",
        .host = "example.org",
        .ipv6addr = "",
        .zoneid = "",
        .port_str = "",
        .port = 0,
        .path = "",
        .query = ""
    };
    expected_uri_result_t uri_with_userinfo = {
        .input_uri = "coap://user@example.org",
        .scheme = "coap",
        .userinfo = "user",
        .host = "example.org",
        .ipv6addr = "",
        .zoneid = "",
        .port_str = "",
        .port = 0,
        .path = "",
        .query = ""
    };
    expected_uri_result_t uri_with_ipv6 = {
        .input_uri = "coap://user@[2001:db8::1]",
        .scheme = "coap",
        .userinfo = "user",
        .host = "[2001:db8::1]",
        .ipv6addr = "2001:db8::1",
        .zoneid = "",
        .port_str = "",
        .port = 0,
        .path = "",
        .query = ""
    };
    expected_uri_result_t uri_with_ipv6_and_port = {
        .input_uri = "coap://user@[2001:db8::1]:12345",
        .scheme = "coap",
        .userinfo = "user",
        .host = "[2001:db8::1]",
        .ipv6addr = "2001:db8::1",
        .zoneid = "",
        .port_str = "12345",
        .port = 12345,
        .path = "",
        .query = ""
    };
    expected_uri_result_t uri_with_ipv6_and_port_and_zoneid = {
        .input_uri = "coap://user@[2001:db8::1%eth0]:12345",
        .scheme = "coap",
        .userinfo = "user",
        .host = "[2001:db8::1%eth0]",
        .ipv6addr = "2001:db8::1",
        .zoneid = "eth0",
        .port_str = "12345",
        .port = 12345,
        .path = "",
        .query = ""
    };

    expected_uri_result_t *test_vec[] = {
        &uri_0,
        &uri_trailing_slash,
        &uri_without_trailing_slash,
        &uri_with_userinfo,
        &uri_with_ipv6,
        &uri_with_ipv6_and_port,
        &uri_with_ipv6_and_port_and_zoneid,
    };

    uri_parser_result_t uri_res;
    int ret = 0;

    for (unsigned int i = 0; i < ARRAY_SIZE(test_vec); ++i) {
        expected_uri_result_t *this_vec = test_vec[i];

        ret = uri_parser_process_string(&uri_res, this_vec->input_uri);
        if (ret != expected_ret) {
            /* if the uri_parser return indicates an error, this test fails */
            _print_return_expectation(this_vec->input_uri, expected_ret, ret);
            TEST_FAIL(failure_msg);
        }
        else {
            /* if the uri_parser return indicates success */
            if (_compare_string_buffer("scheme", this_vec->input_uri, this_vec->scheme,
                                       uri_res.scheme, uri_res.scheme_len) != 0) {
                TEST_FAIL(failure_msg);
            }
            if (_compare_string_buffer("userinfo", this_vec->input_uri, this_vec->userinfo,
                                       uri_res.userinfo, uri_res.userinfo_len) != 0) {
                TEST_FAIL(failure_msg);
            }
            if (_compare_string_buffer("host", this_vec->input_uri, this_vec->host, uri_res.host,
                                       uri_res.host_len) != 0) {
                TEST_FAIL(failure_msg);
            }
            if (_compare_string_buffer("ipv6addr", this_vec->input_uri, this_vec->ipv6addr,
                                       uri_res.ipv6addr, uri_res.ipv6addr_len) != 0) {
                TEST_FAIL(failure_msg);
            }
            if (_compare_string_buffer("zoneid", this_vec->input_uri, this_vec->zoneid,
                                       uri_res.zoneid, uri_res.zoneid_len) != 0) {
                TEST_FAIL(failure_msg);
            }
            if (_compare_string_buffer("port_str", this_vec->input_uri, this_vec->port_str,
                                       uri_res.port_str, uri_res.port_str_len) != 0) {
                TEST_FAIL(failure_msg);
            }
            if (_compare_string_buffer("path", this_vec->input_uri, this_vec->path, uri_res.path,
                                       uri_res.path_len) != 0) {
                TEST_FAIL(failure_msg);
            }
            if (_compare_string_buffer("query", this_vec->input_uri, this_vec->query, uri_res.query,
                                       uri_res.query_len) != 0) {
                TEST_FAIL(failure_msg);
            }
        }
    }
}

static void test_uri_parser__unterminated_string(void)
{
    char *failure_msg =
        "Failure: The uri_parser did not parse an unterminated uri string correctly.";
    expected_uri_result_t this_vec = {
        .input_uri = "coap://RIOT:test@[fe80:db8::1%eth2]:5683/.well-known/core?v=1",
        .scheme = "coap",
        .userinfo = "RIOT:test",
        .host = "[fe80:db8::1%eth2]",
        .ipv6addr = "fe80:db8::1",
        .zoneid = "eth2",
        .port_str = "5683",
        .port = 5683,
        .path = "/.well-known/core",
        .query = "v=1"
    };

    uri_parser_result_t ures;
    char uri[64];

    /* initialize with a non-null character  */
    memset(uri, 'Z', sizeof(uri));

    memcpy(uri, this_vec.input_uri, strlen(this_vec.input_uri));

    int res = uri_parser_process(&ures, uri, strlen(this_vec.input_uri));

    TEST_ASSERT_EQUAL_INT(0, res);

    if (_compare_string_buffer("userinfo", this_vec.input_uri, this_vec.userinfo, ures.userinfo,
                               ures.userinfo_len) != 0) {
        TEST_FAIL(failure_msg);
    }
    if (_compare_string_buffer("host", this_vec.input_uri, this_vec.host, ures.host,
                               ures.host_len) != 0) {
        TEST_FAIL(failure_msg);
    }
    if (_compare_string_buffer("ipv6addr", this_vec.input_uri, this_vec.ipv6addr, ures.ipv6addr,
                               ures.ipv6addr_len) != 0) {
        TEST_FAIL(failure_msg);
    }
    if (_compare_string_buffer("zoneid", this_vec.input_uri, this_vec.zoneid, ures.zoneid,
                               ures.zoneid_len) != 0) {
        TEST_FAIL(failure_msg);
    }
    if (_compare_string_buffer("port_str", this_vec.input_uri, this_vec.port_str, ures.port_str,
                               ures.port_str_len) != 0) {
        TEST_FAIL(failure_msg);
    }
    if (_compare_string_buffer("path", this_vec.input_uri, this_vec.path, ures.path,
                               ures.path_len) != 0) {
        TEST_FAIL(failure_msg);
    }
    if (_compare_string_buffer("query", this_vec.input_uri, this_vec.query, ures.query,
                               ures.query_len) != 0) {
        TEST_FAIL(failure_msg);
    }
}

Test *tests_uri_parser_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(_successful_parsing_of_valid_uri),
        new_TestFixture(_successful_rejecting_of_invalid_uri),
        new_TestFixture(_error_if_port_str_is_too_long),
        new_TestFixture(_error_if_port_str_contains_illegal_characters),
        new_TestFixture(_result_component_scheme_matches_input_scheme),
        new_TestFixture(_result_components_matches_input),
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
