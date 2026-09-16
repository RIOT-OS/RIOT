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

#include <string.h>

#include "embUnit.h"
#include "picohttpparser.h"

#define MAX_HEADERS (8)

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
    TEST_ASSERT(strncmp(method, "GET", method_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("GET"), (int)method_len);
    TEST_ASSERT(strncmp(path, "/", path_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("/"), (int)path_len);
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
    TEST_ASSERT(strncmp(method, "POST", method_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("POST"), (int)method_len);
    TEST_ASSERT(strncmp(path, "/api/data", path_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("/api/data"), (int)path_len);
    TEST_ASSERT_EQUAL_INT(4, (int)num_headers);

    TEST_ASSERT(strncmp(headers[0].name, "Host", headers[0].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Host"), (int)headers[0].name_len);
    TEST_ASSERT(strncmp(headers[0].value, "example.com", headers[0].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("example.com"), (int)headers[0].value_len);

    TEST_ASSERT(strncmp(headers[1].name, "Content-Type", headers[1].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Content-Type"), (int)headers[1].name_len);
    TEST_ASSERT(strncmp(headers[1].value, "application/json", headers[1].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("application/json"), (int)headers[1].value_len);

    TEST_ASSERT(strncmp(headers[2].name, "Content-Length", headers[2].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Content-Length"), (int)headers[2].name_len);
    TEST_ASSERT(strncmp(headers[2].value, "13", headers[2].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("13"), (int)headers[2].value_len);

    TEST_ASSERT(strncmp(headers[3].name, "X-Custom-Header", headers[3].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("X-Custom-Header"), (int)headers[3].name_len);
    TEST_ASSERT(strncmp(headers[3].value, "CustomValue", headers[3].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("CustomValue"), (int)headers[3].value_len);
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

    TEST_ASSERT(strncmp(headers[0].name, "Content-Length", headers[0].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Content-Length"), (int)headers[0].name_len);
    TEST_ASSERT(strncmp(headers[0].value, "13", headers[0].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("13"), (int)headers[0].value_len);

    const char *body = request + res;
    size_t body_len = (sizeof(request) - 1) - (size_t)res;

    TEST_ASSERT(strncmp(body, "Hello, World!", body_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Hello, World!"), (int)body_len);
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
    TEST_ASSERT(strncmp(msg, "OK", msg_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("OK"), (int)msg_len);
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
    TEST_ASSERT(strncmp(msg, "Not Found", msg_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Not Found"), (int)msg_len);
    TEST_ASSERT_EQUAL_INT(3, (int)num_headers);

    TEST_ASSERT(strncmp(headers[0].name, "Content-Type", headers[0].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Content-Type"), (int)headers[0].name_len);
    TEST_ASSERT(strncmp(headers[0].value, "text/plain", headers[0].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("text/plain"), (int)headers[0].value_len);

    TEST_ASSERT(strncmp(headers[1].name, "Content-Length", headers[1].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Content-Length"), (int)headers[1].name_len);
    TEST_ASSERT(strncmp(headers[1].value, "9", headers[1].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("9"), (int)headers[1].value_len);

    TEST_ASSERT(strncmp(headers[2].name, "X-Custom-Header", headers[2].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("X-Custom-Header"), (int)headers[2].name_len);
    TEST_ASSERT(strncmp(headers[2].value, "CustomValue", headers[2].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("CustomValue"), (int)headers[2].value_len);
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

    TEST_ASSERT(strncmp(headers[0].name, "Content-Length", headers[0].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Content-Length"), (int)headers[0].name_len);
    TEST_ASSERT(strncmp(headers[0].value, "13", headers[0].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("13"), (int)headers[0].value_len);

    const char *body = response + res;
    size_t body_len = (sizeof(response) - 1) - (size_t)res;

    TEST_ASSERT(strncmp(body, "Hello, World!", body_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Hello, World!"), (int)body_len);
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

    TEST_ASSERT(strncmp(headers[0].name, "Host", headers[0].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Host"), (int)headers[0].name_len);
    TEST_ASSERT(strncmp(headers[0].value, "example.com", headers[0].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("example.com"), (int)headers[0].value_len);

    TEST_ASSERT(strncmp(headers[1].name, "Connection", headers[1].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("Connection"), (int)headers[1].name_len);
    TEST_ASSERT(strncmp(headers[1].value, "close", headers[1].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("close"), (int)headers[1].value_len);

    TEST_ASSERT(strncmp(headers[2].name, "X-Custom-Header", headers[2].name_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("X-Custom-Header"), (int)headers[2].name_len);
    TEST_ASSERT(strncmp(headers[2].value, "CustomValue", headers[2].value_len) == 0);
    TEST_ASSERT_EQUAL_INT((int)strlen("CustomValue"), (int)headers[2].value_len);
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
