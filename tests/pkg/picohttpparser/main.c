/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests the PicoHTTPParser package
 *
 * The tests in this file only cover the basic functionality of the
 * PicoHTTPParser package. The package itself is tested more extensively in its
 * own repository.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdbool.h>
#include <string.h>

#include "embUnit.h"
#include "picohttpparser.h"

#define MAX_HEADERS (8)

/**
 * @brief   Compares a buffer with an expected string
 *
 * The use of `strncmp` is avoided, because `strncmp` with a length taken from
 * the buffer provides prefix matching, not equality.
 */
static bool _str_is(const char *buf, size_t len, const char *expected)
{
    return strlen(expected) == len && memcmp(buf, expected, len) == 0;
}

static void test_picohttpparser_request_simple(void)
{
    static const char request[] = "GET / HTTP/1.1\r\n\r\n";

    const char *method;
    size_t method_len;
    const char *path;
    size_t path_len;
    int minor_version;
    struct phr_header headers[MAX_HEADERS];
    size_t num_headers = MAX_HEADERS;

    int res = phr_parse_request(request, sizeof(request) - 1, &method, &method_len,
                                &path, &path_len, &minor_version, headers,
                                &num_headers, 0);

    TEST_ASSERT_EQUAL_INT((int)(sizeof(request) - 1), res);
    TEST_ASSERT(_str_is(method, method_len, "GET"));
    TEST_ASSERT(_str_is(path, path_len, "/"));
    TEST_ASSERT_EQUAL_INT(1, minor_version);
    TEST_ASSERT_EQUAL_INT(0, (int)num_headers);
}

static void test_picohttpparser_request_headers(void)
{
    static const char request[] =
        "POST /api/data HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 13\r\n"
        "X-Custom-Header: CustomValue\r\n"
        "\r\n";

    const char *method;
    size_t method_len;
    const char *path;
    size_t path_len;
    int minor_version;
    struct phr_header headers[MAX_HEADERS];
    size_t num_headers = MAX_HEADERS;

    int res = phr_parse_request(request, sizeof(request) - 1, &method, &method_len,
                                &path, &path_len, &minor_version, headers,
                                &num_headers, 0);

    TEST_ASSERT_EQUAL_INT((int)(sizeof(request) - 1), res);
    TEST_ASSERT(_str_is(method, method_len, "POST"));
    TEST_ASSERT(_str_is(path, path_len, "/api/data"));
    TEST_ASSERT_EQUAL_INT(4, (int)num_headers);

    TEST_ASSERT(_str_is(headers[0].name, headers[0].name_len, "Host"));
    TEST_ASSERT(_str_is(headers[0].value, headers[0].value_len, "example.com"));
    TEST_ASSERT(_str_is(headers[1].name, headers[1].name_len, "Content-Type"));
    TEST_ASSERT(_str_is(headers[1].value, headers[1].value_len, "application/json"));
    TEST_ASSERT(_str_is(headers[2].name, headers[2].name_len, "Content-Length"));
    TEST_ASSERT(_str_is(headers[2].value, headers[2].value_len, "13"));
    TEST_ASSERT(_str_is(headers[3].name, headers[3].name_len, "X-Custom-Header"));
    TEST_ASSERT(_str_is(headers[3].value, headers[3].value_len, "CustomValue"));
}

static void test_picohttpparser_request_partial(void)
{
    static const char request[] = "GET / HTTP/1.1\r\nHost: example.com\r\n";

    const char *method;
    size_t method_len;
    const char *path;
    size_t path_len;
    int minor_version;
    struct phr_header headers[MAX_HEADERS];
    size_t num_headers = MAX_HEADERS;

    int res = phr_parse_request(request, sizeof(request) - 1, &method, &method_len,
                                &path, &path_len, &minor_version, headers,
                                &num_headers, 0);

    TEST_ASSERT_EQUAL_INT(-2, res);
}

static void test_picohttpparser_request_body(void)
{
    static const char request[] =
        "POST /api/data HTTP/1.1\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";

    const char *method;
    size_t method_len;
    const char *path;
    size_t path_len;
    int minor_version;
    struct phr_header headers[MAX_HEADERS];
    size_t num_headers = MAX_HEADERS;

    int res = phr_parse_request(request, sizeof(request) - 1, &method, &method_len,
                                &path, &path_len, &minor_version, headers,
                                &num_headers, 0);

    TEST_ASSERT(res > 0);
    TEST_ASSERT_EQUAL_INT(1, (int)num_headers);
    TEST_ASSERT(_str_is(headers[0].name, headers[0].name_len, "Content-Length"));
    TEST_ASSERT(_str_is(headers[0].value, headers[0].value_len, "13"));

    const char *body = request + res;
    size_t body_len = (sizeof(request) - 1) - (size_t)res;

    TEST_ASSERT(_str_is(body, body_len, "Hello, World!"));
}

static void test_picohttpparser_response_simple(void)
{
    static const char response[] = "HTTP/1.1 200 OK\r\n\r\n";

    int minor_version;
    int status;
    const char *msg;
    size_t msg_len;
    struct phr_header headers[MAX_HEADERS];
    size_t num_headers = MAX_HEADERS;

    int res = phr_parse_response(response, sizeof(response) - 1, &minor_version,
                                 &status, &msg, &msg_len, headers, &num_headers, 0);

    TEST_ASSERT_EQUAL_INT((int)(sizeof(response) - 1), res);
    TEST_ASSERT_EQUAL_INT(1, minor_version);
    TEST_ASSERT_EQUAL_INT(200, status);
    TEST_ASSERT(_str_is(msg, msg_len, "OK"));
    TEST_ASSERT_EQUAL_INT(0, (int)num_headers);
}

static void test_picohttpparser_response_headers(void)
{
    static const char response[] =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 9\r\n"
        "X-Custom-Header: CustomValue\r\n"
        "\r\n";

    int minor_version;
    int status;
    const char *msg;
    size_t msg_len;
    struct phr_header headers[MAX_HEADERS];
    size_t num_headers = MAX_HEADERS;

    int res = phr_parse_response(response, sizeof(response) - 1, &minor_version,
                                 &status, &msg, &msg_len, headers, &num_headers, 0);

    TEST_ASSERT_EQUAL_INT((int)(sizeof(response) - 1), res);
    TEST_ASSERT_EQUAL_INT(1, minor_version);
    TEST_ASSERT_EQUAL_INT(404, status);
    TEST_ASSERT(_str_is(msg, msg_len, "Not Found"));
    TEST_ASSERT_EQUAL_INT(3, (int)num_headers);

    TEST_ASSERT(_str_is(headers[0].name, headers[0].name_len, "Content-Type"));
    TEST_ASSERT(_str_is(headers[0].value, headers[0].value_len, "text/plain"));
    TEST_ASSERT(_str_is(headers[1].name, headers[1].name_len, "Content-Length"));
    TEST_ASSERT(_str_is(headers[1].value, headers[1].value_len, "9"));
    TEST_ASSERT(_str_is(headers[2].name, headers[2].name_len, "X-Custom-Header"));
    TEST_ASSERT(_str_is(headers[2].value, headers[2].value_len, "CustomValue"));
}

static void test_picohttpparser_response_partial(void)
{
    static const char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n";

    int minor_version;
    int status;
    const char *msg;
    size_t msg_len;
    struct phr_header headers[MAX_HEADERS];
    size_t num_headers = MAX_HEADERS;

    int res = phr_parse_response(response, sizeof(response) - 1, &minor_version,
                                 &status, &msg, &msg_len, headers, &num_headers, 0);

    TEST_ASSERT_EQUAL_INT(-2, res);
}

static void test_picohttpparser_response_body(void)
{
    static const char response[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";

    int minor_version;
    int status;
    const char *msg;
    size_t msg_len;
    struct phr_header headers[MAX_HEADERS];
    size_t num_headers = MAX_HEADERS;

    int res = phr_parse_response(response, sizeof(response) - 1, &minor_version,
                                 &status, &msg, &msg_len, headers, &num_headers, 0);

    TEST_ASSERT(res > 0);
    TEST_ASSERT_EQUAL_INT(1, (int)num_headers);
    TEST_ASSERT(_str_is(headers[0].name, headers[0].name_len, "Content-Length"));
    TEST_ASSERT(_str_is(headers[0].value, headers[0].value_len, "13"));

    const char *body = response + res;
    size_t body_len = (sizeof(response) - 1) - (size_t)res;

    TEST_ASSERT(_str_is(body, body_len, "Hello, World!"));
}

static void test_picohttpparser_headers_simple(void)
{
    static const char raw_headers[] =
        "Host: example.com\r\n"
        "Connection: close\r\n"
        "X-Custom-Header: CustomValue\r\n"
        "\r\n";

    struct phr_header headers[MAX_HEADERS];
    size_t num_headers = MAX_HEADERS;

    int res = phr_parse_headers(raw_headers, sizeof(raw_headers) - 1, headers,
                                &num_headers, 0);

    TEST_ASSERT_EQUAL_INT((int)(sizeof(raw_headers) - 1), res);
    TEST_ASSERT_EQUAL_INT(3, (int)num_headers);

    TEST_ASSERT(_str_is(headers[0].name, headers[0].name_len, "Host"));
    TEST_ASSERT(_str_is(headers[0].value, headers[0].value_len, "example.com"));
    TEST_ASSERT(_str_is(headers[1].name, headers[1].name_len, "Connection"));
    TEST_ASSERT(_str_is(headers[1].value, headers[1].value_len, "close"));
    TEST_ASSERT(_str_is(headers[2].name, headers[2].name_len, "X-Custom-Header"));
    TEST_ASSERT(_str_is(headers[2].value, headers[2].value_len, "CustomValue"));
}

Test *tests_picohttpparser(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_picohttpparser_request_simple),
        new_TestFixture(test_picohttpparser_request_headers),
        new_TestFixture(test_picohttpparser_request_partial),
        new_TestFixture(test_picohttpparser_request_body),
        new_TestFixture(test_picohttpparser_response_simple),
        new_TestFixture(test_picohttpparser_response_headers),
        new_TestFixture(test_picohttpparser_response_partial),
        new_TestFixture(test_picohttpparser_response_body),
        new_TestFixture(test_picohttpparser_headers_simple),
    };

    EMB_UNIT_TESTCALLER(picohttpparser_tests, NULL, NULL, fixtures);

    return (Test *)&picohttpparser_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_picohttpparser());
    TESTS_END();

    return 0;
}
