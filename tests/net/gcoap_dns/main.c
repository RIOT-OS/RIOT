/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fmt.h"
#include "net/af.h"
#include "net/coap.h"
#include "net/credman.h"
#include "net/dns/msg.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "od.h"
#include "embUnit.h"
#include "shell.h"

#if IS_USED(MODULE_GCOAP_DTLS)
#include "net/sock/dtls.h"
#endif
#include "net/gcoap/dns.h"

#define PSK_ID_LEN          32U
#define PSK_LEN             32U

#define TEST_TAG            2599U
#define TEST_PSK_ID         "client_identity"
#define TEST_PSK            "secretPSK"

static char line_buf[SHELL_DEFAULT_BUFSIZE];
static char _psk_id[PSK_ID_LEN];
static char _psk[PSK_LEN];
static credman_credential_t _credential = {
    .type = CREDMAN_TYPE_PSK,
    .params = {
        .psk = {
            .id = { .s = _psk_id, .len = 0U, },
            .key = { .s = _psk, .len = 0U, },
        }
    },
};
static uint8_t _mock_response[CONFIG_DNS_MSG_LEN];
static size_t _mock_response_len = 0U;
static uint8_t _resp_code = COAP_CODE_EMPTY;

#if IS_USED(MODULE_GCOAP_DTLS)
static_assert(CONFIG_GCOAP_DNS_CREDS_MAX == CONFIG_DTLS_CREDENTIALS_MAX,
              "CONFIG_GCOAP_DNS_CREDS_MAX and CONFIG_DTLS_CREDENTIALS_MAX "
              "must be equal for this test");
#endif
static_assert(!IS_USED(MODULE_GCOAP_DTLS) ||
             (CONFIG_GCOAP_DNS_CREDS_MAX == CONFIG_CREDMAN_MAX_CREDENTIALS),
              "CONFIG_GCOAP_DNS_CREDS_MAX and CONFIG_CREDMAN_MAX_CREDENTIALS "
              "must be equal for this test");

#define INIT_TEST_PSK(t) \
    _credential.type = CREDMAN_TYPE_PSK; \
    _credential.tag = (t); \
    _credential.params.psk.id.len = sizeof(TEST_PSK_ID); \
    strcpy((char *)_credential.params.psk.id.s, TEST_PSK_ID); \
    _credential.params.psk.key.len = sizeof(TEST_PSK); \
    strcpy((char *)_credential.params.psk.key.s, TEST_PSK)

static void setup(void)
{
    gcoap_dns_server_uri_set(NULL);
    gcoap_dns_cred_reset();
    _credential.tag = CREDMAN_TAG_EMPTY;
    _credential.params.psk.id.len = 0U;
    _credential.params.psk.key.len = 0U;
}

static void test_server_uri_set__success_1(void)
{
    static const char uri[] = "coap://example.org/";
    char res[sizeof(uri)];

    TEST_ASSERT_EQUAL_INT(strlen(uri), gcoap_dns_server_uri_set(uri));
    TEST_ASSERT_EQUAL_INT(strlen(uri), gcoap_dns_server_uri_get(res, sizeof(res)));
    TEST_ASSERT_EQUAL_STRING(uri, res);
    TEST_ASSERT(gcoap_dns_server_uri_is_set());
    TEST_ASSERT_EQUAL_INT(0, gcoap_dns_server_uri_set(NULL));
    TEST_ASSERT(!gcoap_dns_server_uri_is_set());
}

static void test_server_uri_set__success_2(void)
{
    static const char uri[] = "coaps://example.org/";

    if (IS_USED(MODULE_GCOAP_DTLS)) {
        char res[sizeof(uri)];
        TEST_ASSERT_EQUAL_INT(strlen(uri), gcoap_dns_server_uri_set(uri));
        TEST_ASSERT_EQUAL_INT(strlen(uri), gcoap_dns_server_uri_get(res, sizeof(res)));
        TEST_ASSERT_EQUAL_STRING(uri, res);
        TEST_ASSERT(gcoap_dns_server_uri_is_set());
    }
    else {
        TEST_ASSERT_EQUAL_INT(-EINVAL, gcoap_dns_server_uri_set(uri));
        TEST_ASSERT(!gcoap_dns_server_uri_is_set());
    }
}

static void test_server_uri_set__uri_too_long(void)
{
    static const char uri[] = "coap://a.very.long.host-name.org"
        "/this/is/a/very/long/path/to/dns";

    TEST_ASSERT(!gcoap_dns_server_uri_is_set());
    /* would not fit trailing \0 */
    TEST_ASSERT_EQUAL_INT(CONFIG_GCOAP_DNS_SERVER_URI_LEN,
                          strlen(uri));
    INIT_TEST_PSK(TEST_TAG);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, gcoap_dns_server_uri_set(uri));
    TEST_ASSERT(!gcoap_dns_server_uri_is_set());
}

static void test_server_uri_set__not_coap(void)
{
    static const char uri[] = "https://example.org/";

    TEST_ASSERT(!gcoap_dns_server_uri_is_set());
    INIT_TEST_PSK(TEST_TAG);
    TEST_ASSERT_EQUAL_INT(-EINVAL, gcoap_dns_server_uri_set(uri));
    TEST_ASSERT(!gcoap_dns_server_uri_is_set());
}

static void test_server_uri_get__buf_too_short(void)
{
    static const char uri[] = "coap://example.org/";
    char res[sizeof(uri) - 1];

    TEST_ASSERT_EQUAL_INT(strlen(uri), gcoap_dns_server_uri_set(uri));
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, gcoap_dns_server_uri_get(res, sizeof(res)));
}

static void test_cred_add__success(void)
{
    INIT_TEST_PSK(TEST_TAG);
    if (IS_USED(MODULE_GCOAP_DTLS)) {
        TEST_ASSERT_EQUAL_INT(0, gcoap_dns_cred_add(&_credential));
    }
    else {
        TEST_ASSERT_EQUAL_INT(-ENOTSUP, gcoap_dns_cred_add(&_credential));
    }
}

static void test_cred_add__no_mem(void)
{
#if IS_USED(MODULE_GCOAP_DTLS)
    for (unsigned i = 0; i < CONFIG_GCOAP_DNS_CREDS_MAX; i++) {
        INIT_TEST_PSK(TEST_TAG + i);
        TEST_ASSERT_EQUAL_INT(0, gcoap_dns_cred_add(&_credential));
    }
    INIT_TEST_PSK(TEST_TAG + CONFIG_GCOAP_DNS_CREDS_MAX);
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gcoap_dns_cred_add(&_credential));
#else
    TEST_ASSERT_EQUAL_INT(-ENOTSUP, gcoap_dns_cred_add(&_credential));
#endif
}

static void test_cred_add__credman_error(void)
{
    if (IS_USED(MODULE_GCOAP_DTLS)) {
        INIT_TEST_PSK(TEST_TAG);
        _credential.type = CREDMAN_TYPE_EMPTY;
        TEST_ASSERT_EQUAL_INT(-EBADF, gcoap_dns_cred_add(&_credential));
    }
    else {
        TEST_ASSERT_EQUAL_INT(-ENOTSUP, gcoap_dns_cred_add(&_credential));
    }
}

static void test_cred_remove__success(void)
{
    INIT_TEST_PSK(TEST_TAG);
    if (IS_USED(MODULE_GCOAP_DTLS)) {
        TEST_ASSERT_EQUAL_INT(CREDMAN_NOT_FOUND, credman_get(&_credential,
                                                             _credential.tag, _credential.type));
        TEST_ASSERT_EQUAL_INT(0, gcoap_dns_cred_add(&_credential));
        TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_get(&_credential,
                                                      _credential.tag, _credential.type));
        gcoap_dns_cred_remove(_credential.tag, _credential.type);
        TEST_ASSERT_EQUAL_INT(CREDMAN_NOT_FOUND, credman_get(&_credential,
                                                             _credential.tag, _credential.type));
    }
    else {
        TEST_ASSERT_EQUAL_INT(-ENOTSUP, gcoap_dns_cred_add(&_credential));
    }
}

static int _unittests(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_server_uri_set__success_1),
        new_TestFixture(test_server_uri_set__success_2),
        new_TestFixture(test_server_uri_set__uri_too_long),
        new_TestFixture(test_server_uri_set__not_coap),
        new_TestFixture(test_server_uri_get__buf_too_short),
        new_TestFixture(test_cred_add__success),
        new_TestFixture(test_cred_add__no_mem),
        new_TestFixture(test_cred_add__credman_error),
        new_TestFixture(test_cred_remove__success),
    };

    EMB_UNIT_TESTCALLER(gcoap_dns_tests, setup, NULL, fixtures);
    TESTS_START();
    TESTS_RUN((Test *)&gcoap_dns_tests);
    TESTS_END();
    puts("Be aware that URI and credentials are now reset.");
    return 0;
}

static ssize_t _mock_dns_server(coap_pkt_t *pdu, uint8_t *buf, size_t len, coap_request_ctx_t *ctx)
{
    (void)ctx;
    if ((_resp_code >> 5) == COAP_CLASS_SUCCESS) {
        gcoap_resp_init(pdu, buf, len, _resp_code);
        coap_opt_add_format(pdu, COAP_FORMAT_DNS_MESSAGE);
        size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);
        /* write the RIOT board name in the response buffer */
        if (pdu->payload_len >= _mock_response_len) {
            memcpy(pdu->payload, _mock_response, _mock_response_len);
            return resp_len + _mock_response_len;
        }
        else {
            puts("gcoap_cli: msg buffer too small");
            return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
        }
    }
    else {
        return gcoap_response(pdu, buf, len, _resp_code);
    }
}

static void _uri_usage(const char *cmd)
{
    printf("usage: %s -d\n", cmd);
    printf("       %s <uri>\n", cmd);
}

static int _set_uri(int argc, char **argv)
{
    int res;

    if ((argc > 1) && (strcmp(argv[1], "-d") == 0)) {
        gcoap_dns_server_uri_set(NULL);
        puts("Successfully reset URI\n");
        return 0;
    }
    else if (argc == 1) {
        static char uri[CONFIG_GCOAP_DNS_SERVER_URI_LEN];
        ssize_t res;

        if ((res = gcoap_dns_server_uri_get(uri, sizeof(uri))) < 0) {
            errno = -res;
            perror("Unable to get URI");
            return 1;
        }
        else {
            if (res > 0) {
                puts(uri);
                return 0;
            }
            else {
                _uri_usage(argv[0]);
                return 1;
            }
            return 0;
        }
    }
    /* argc > 1 can be assumed since argc == 1 returns above */
    res = gcoap_dns_server_uri_set(argv[1]);
    if (res < 0) {
        errno = -res;
        perror("Unable to set URI");
        return errno;
    }
    printf("Successfully added URI %s\n", argv[1]);
    return 0;
}

static void _creds_usage(const char *cmd)
{
    printf("usage: %s -d <cred_tag>\n", cmd);
    printf("       %s <cred_tag> <psk_id> <psk>\n", cmd);
}

static int _creds(int argc, char **argv)
{
    if ((argc > 2) && (strcmp(argv[1], "-d") == 0)) {
        credman_tag_t tag = atoi(argv[2]);

        if (tag == 0) {
            _creds_usage(argv[0]);
            return 1;
        }
        gcoap_dns_cred_remove(tag, CREDMAN_TYPE_PSK);
        printf("Successfully removed credentials with tag %u\n", tag);
        return 0;
    }
    if (argc < 4) {
        _creds_usage(argv[0]);
        return 1;
    }
    if ((_credential.tag = atoi(argv[1])) == 0) {
        _creds_usage(argv[0]);
        return 1;
    }
    if ((_credential.params.psk.id.len = strlen(argv[2])) > PSK_ID_LEN) {
        printf("PSK ID too long (max. %u bytes allowed)\n", PSK_ID_LEN);
        return 1;
    }
    if ((_credential.params.psk.key.len = strlen(argv[3])) > PSK_LEN) {
        printf("PSK too long (max. %u bytes allowed)\n", PSK_LEN);
        return 1;
    }
    _credential.type = CREDMAN_TYPE_PSK;
    strcpy((char *)_credential.params.psk.id.s, argv[2]);
    strcpy((char *)_credential.params.psk.key.s, argv[3]);

    int res = gcoap_dns_cred_add(&_credential);

    if (res < 0) {
        errno = -res;
        perror("Unable to add credential");
        return errno;
    }
    printf("Successfully added creds: %u, %s, %s\n", _credential.tag, argv[2], argv[3]);
    return 0;
}

static int _proxy(int argc, char **argv)
{
    if (argc < 2) {
        static char proxy[CONFIG_GCOAP_DNS_SERVER_URI_LEN];
        int res;

        if ((res = gcoap_dns_server_proxy_get(proxy, sizeof(proxy))) < 0) {
            errno = -res;
            perror("Unable to get URI");
            return 1;
        }
        else {
            if (res > 0) {
                puts(proxy);
                return 0;
            }
            else {
                 printf("usage: %s [<proxy URI>|-]\n", argv[0]);
                return 1;
            }
            return 0;
        }
    }
    else if (argv[1][0] == '-') {
        gcoap_dns_server_proxy_reset();
        puts("Successfully reset proxy URI\n");
    }
    else {
        int res = gcoap_dns_server_proxy_set(argv[1]);

        if (res < 0) {
            errno = -res;
            perror("Unable to set proxy URI");
            return errno;
        }
        printf("Successfully added proxy URI %s\n", argv[1]);
    }
    return 0;
}
static void _query_usage(const char *cmd)
{
    printf("usage: %s <hostname> [inet|inet6]\n", cmd);
}

int _parse_af(const char *family_name)
{
    if (strcmp("inet6", family_name) == 0) {
        return AF_INET6;
    }
    else if (strcmp("inet", family_name) == 0) {
        return AF_INET;
    }
    else {
        printf("Unexpected family %s\n", family_name);
        return AF_UNSPEC;
    }
}

int _print_addr(const char *hostname, const uint8_t *addr, int addr_len)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    switch (addr_len) {
        case sizeof(ipv4_addr_t):
            printf("Hostname %s resolves to %s (IPv4)\n",
                   hostname, ipv4_addr_to_str(addr_str, (ipv4_addr_t *)addr,
                                              sizeof(addr_str)));
            break;
        case sizeof(ipv6_addr_t):
            printf("Hostname %s resolves to %s (IPv6)\n",
                   hostname, ipv6_addr_to_str(addr_str, (ipv6_addr_t *)addr,
                                              sizeof(addr_str)));
            break;
        default:
            printf("Unexpected address format resolved for hostname %s\n",
                   hostname);
            return 1;
    }
    return 0;
}

static int _query(int argc, char **argv)
{
    uint8_t addr_out[sizeof(ipv6_addr_t)];
    const char *hostname;
    int family = AF_INET6;
    int res;
    int offset = 0;

    if (argc < 2) {
        _query_usage(argv[0]);
        return 1;
    }
    if (argc < (offset + 2)) {
        _query_usage(argv[0]);
        return 1;
    }
    if (argc == (offset + 3)) {
        if ((family = _parse_af(argv[2 + offset])) == AF_UNSPEC) {
            return 1;
        }
    }
    hostname = argv[offset + 1];
    res = gcoap_dns_query(hostname, addr_out, family);
    if (res < 0) {
        errno = -res;
        perror("Unable to resolve query");
        return errno;
    }
    return _print_addr(hostname, addr_out, res);
}

static ssize_t _copy_mock_response(const char *str)
{
    bool msn = true;   /* most significant nibble */
    ssize_t start = _mock_response_len;

    for (unsigned i = 0; i < strlen(str); i++) {
        uint8_t nibble = scn_u32_hex(&str[i], 1);

        if (_mock_response_len == CONFIG_DNS_MSG_LEN) {
            break;
        }
        if (msn) {
            _mock_response[_mock_response_len] = nibble << 4;
        }
        else {
            _mock_response[_mock_response_len++] |= nibble;
        }
        msn = !msn;
    }
    if (!msn) {
        _mock_response_len++;
    }
    if (_mock_response_len >= CONFIG_DNS_MSG_LEN) {
        printf("Too many bytes added to response");
        return -ENOBUFS;
    }
    return _mock_response_len - start;
}

static int _resp(int argc, char **argv)
{
    if (argc < 2) {
        od_hex_dump(_mock_response, _mock_response_len, OD_WIDTH_DEFAULT);
        return 0;
    }
    if (argc > 2 && (strcmp(argv[1], "-c") == 0)) {
        ssize_t size = _copy_mock_response(argv[2]);
        if (size >= 0) {
            printf("Successfully continued response\n");
            od_hex_dump(&_mock_response[_mock_response_len - size], size,
                        OD_WIDTH_DEFAULT);
            return 0;
        }
        else {
            return 1;
        }
    }
    _resp_code = atoi(argv[1]);
    if ((argc > 2) && ((_resp_code >> 5) == 2)) {
        _mock_response_len = 0;
        if (_copy_mock_response(argv[2]) < 0) {
            return 1;
        }
        printf("Successfully set response with code %u.%02u\n", _resp_code >> 5, _resp_code & 0x1f);
        od_hex_dump(_mock_response, _mock_response_len, OD_WIDTH_DEFAULT);
    }
    else {
        printf("Successfully set response code %u.%02u\n", _resp_code >> 5, _resp_code & 0x1f);
        _mock_response_len = 0;
    }
    return 0;
}

static int _has_dns_cache(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (IS_USED(MODULE_DNS_CACHE)) {
        puts("DNS cache exists");
    }
    else {
        puts("DNS cache does not exist");
    }
    return 0;
}

static const coap_resource_t _resources[] = {
    { "/", COAP_FETCH, _mock_dns_server, NULL },
};

static gcoap_listener_t _listener = {
    &_resources[0],
    ARRAY_SIZE(_resources),
    GCOAP_SOCKET_TYPE_UNDEF,
    NULL,
    NULL,
    NULL
};

static const shell_command_t _shell_commands[] = {
    { "unittests", "Runs unittests", _unittests},
    { "uri", "Sets URI to DoC server", _set_uri},
    { "creds", "Adds/removes credentials for DoC server", _creds},
    { "proxy", "Sets proxy URI for DoC queries", _proxy},
    { "query", "Sends DoC query for a hostname", _query},
    { "resp", "Set static response for mock DoC server", _resp},
    { "has_dns_cache", "Check if DNS cache is activated", _has_dns_cache},
    { NULL, NULL, NULL }
};

int main(void)
{
    gcoap_register_listener(&_listener);
    shell_run(_shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}

/** @} */
