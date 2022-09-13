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
 * This explicit version has the advantage, that in case of a test failure,
 * the failed line number is exactly on point:
 * uri_parser_tests.test_process_string_error_if_port_str_is_too_long \
 * (tests/unittests/tests-uri_parser/tests-uri_parser.c 87) Failure: \
 * The uri_parser did *NOT* detect an invalid port-length as invalid.
 */

/*
 * This test checks if the uri parser raises an error if the port length
 * (as a string) is invalid, that is, longer than 5 characters
 */
static void _error_if_port_str_is_too_long(void)
{
    uri_parser_result_t uri_res;
    int ret = 0;
    int expected_ret = -1; // Indicates an invalid uri

    char *too_many_digits = "https://example.org:123456/foo/bar";

    ret = uri_parser_process_string(&uri_res, too_many_digits);
    if (ret != expected_ret) {
        _print_return_expectation(too_many_digits, expected_ret, ret);
        TEST_FAIL("Failure: The uri_parser did not detect an invalid port-length as invalid.");
    }

    char *leading_zeroes_arent_ignored = "https://example.org:000456/foo/bar";

    ret = uri_parser_process_string(&uri_res, leading_zeroes_arent_ignored);
    if (ret != expected_ret) {
        _print_return_expectation(leading_zeroes_arent_ignored, expected_ret, ret);
        TEST_FAIL("Failure: The uri_parser did not detect an invalid port-length as invalid.");
    }

    char *too_much_whitespace = "https://example.org: 23456/foo/bar";

    ret = uri_parser_process_string(&uri_res, leading_zeroes_arent_ignored);
    if (ret != expected_ret) {
        _print_return_expectation(too_much_whitespace, expected_ret, ret);
        TEST_FAIL("Failure: The uri_parser did not detect an invalid port-length as invalid.");
    }

    // Just for demonstration of the failure output in this PR: This one fails.
    char *valid_this_fails_the_test_on_purpose = "https://example.org:1234/foo/bar";

    ret = uri_parser_process_string(&uri_res, valid_this_fails_the_test_on_purpose);
    if (ret != expected_ret) {
        _print_return_expectation(valid_this_fails_the_test_on_purpose, expected_ret, ret);
        TEST_FAIL("Failure: The uri_parser did not detect an invalid port-length as invalid.");
    }
}

/*
 * This compact version has the disadvantage, that in case of a test failure,
 * the failed line number is inconclusive and it is less concise:
 * uri_parser_tests.test_process_string_error_if_port_str_is_too_long \
 * (tests/unittests/tests-uri_parser/tests-uri_parser.c 139) Failure: \
 * The uri_parser did *NOT* detect an invalid port-length as invalid.
 *
 * However, it is much shorter which might be a worthwhile trade-off.
 */

/*
 * This test checks if the uri parser raises an error if the port length
 * (as a string) is invalid, that is, longer than 5 characters
 */
static void _error_if_port_str_is_too_long_array(void)
{
    char *failure_msg = "Failure: The uri_parser did not detect an invalid port-length as invalid.";
    int expected_ret = -1; // Indicates an invalid uri

    char *leading_zeroes_arent_ignored = "https://example.org:000456/foo/bar";
    char *too_many_digits = "https://example.org:123456/foo/bar";
    char *too_much_whitespace = "https://example.org: 23456/foo/bar";
    // Just for demonstration of the failure output in this PR: This one fails.
    char *valid_this_fails_the_test_on_purpose = "https://example.org:1234/foo/bar";

    /*
     * Why don't you just add the strings to the array? We don't need an
     * extra variable for that!
     *
     * Glad you are asking, imagine seeing this test vector:
     * ftp://123@example.org:/foo
     * What is being tested? What is the edge case that is explored?
     * You don't know. If we would name that string, it gets easy:
     * char *username_must_not_start_with_a_number = ftp://123@example.org:/foo
     */
    char *test_vec[] = {
        too_many_digits,
        leading_zeroes_arent_ignored,
        too_much_whitespace,
        valid_this_fails_the_test_on_purpose
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

static void _successfull_parsing_of_valid_uri_with_scheme_and_path(void)
{
    char *failure_msg = "Failure: The uri_parser failed to parse a correct uri.";
    int expected_ret = 0;

    char *simple_uri_0 = "coap://example.org/foo/bar";
    char *simple_uri_1 = "ftp://riot-os.org/bar/foo";
    // Just for demonstration of the failure output in this PR: This one fails.
    char *invalid_this_fails_the_test_uri = "ftp://example.org:99999/foo/bar";

    char *test_vec[] = {
        simple_uri_0,
        simple_uri_1,
        invalid_this_fails_the_test_uri,
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
    // Just for demonstration of the failure output in this PR: This one fails.
    expected_uri_result_t invalid_scheme = {
        .input_uri = "coap://example.org:99999/foo/bar",
        .scheme = "coap",
    };

    expected_uri_result_t *test_vec[] = {
        &very_long_scheme,
        &normal_scheme,
        &short_scheme,
        &invalid_scheme,
    };

    uri_parser_result_t uri_res;
    int ret = 0;

    for (unsigned int i = 0; i < ARRAY_SIZE(test_vec); ++i) {
        ret = uri_parser_process_string(&uri_res, test_vec[i]->input_uri);
        /* One should only test for success OR failure. Here, we provide valid uri.
         * If you are interested in testing for failure using broken schemes,
         * write a separate test */
        if (ret != expected_ret) {
            _print_return_expectation(test_vec[i]->input_uri, expected_ret, ret);
            TEST_FAIL(failure_msg);
        }
        else {
            /* if the uri_parser return indicates success
             * the length of the schemes must match.
             * we can't use strlen on the result scheme as it is not null terminated,
             * but the uri_provides the length separately */
            if (uri_res.scheme_len != strlen(test_vec[i]->scheme)) {
                printf(
                    "With given input uri '%s', expected a scheme with the length '%d' "
                    "but got '%d'\n",
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

Test *tests_uri_parser_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(_error_if_port_str_is_too_long),
        new_TestFixture(_error_if_port_str_is_too_long_array),
        new_TestFixture(_successfull_parsing_of_valid_uri_with_scheme_and_path),
        new_TestFixture(_result_component_scheme_matches_input_scheme),
    };

    EMB_UNIT_TESTCALLER(uri_parser_tests, NULL, NULL, fixtures);

    return (Test *)&uri_parser_tests;
}

void tests_uri_parser(void)
{
    TESTS_RUN(tests_uri_parser_tests());
}
/** @} */
