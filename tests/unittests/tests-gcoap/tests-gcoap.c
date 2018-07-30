/*
 * Copyright (c) 2016 Ken Bannister. All rights reserved.
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
#include <stdlib.h>

#include "embUnit.h"

#include "net/gcoap.h"

#include "unittests-constants.h"
#include "tests-gcoap.h"

/*
 * A test set of dummy resources. The resource handlers are set to NULL.
 */
static const coap_resource_t resources[] = {
    { .path = "/act/switch", .methods = (COAP_GET | COAP_POST) },
    { .path = "/sensor/temp", .methods = (COAP_GET) },
    { .path = "/test/info/all", .methods = (COAP_GET) },
};

static const coap_resource_t resources_second[] = {
    { .path = "/second/part", .methods = (COAP_GET)},
};

static gcoap_listener_t listener = {
    .resources     = &resources[0],
    .resources_len = (sizeof(resources) / sizeof(resources[0])),
    .next          = NULL
};

static gcoap_listener_t listener_second = {
    .resources     = &resources_second[0],
    .resources_len = (sizeof(resources_second) / sizeof(resources_second[0])),
    .next          = NULL
};

static const char *resource_list_str = "</act/switch>,</sensor/temp>,</test/info/all>,</second/part>";

/*
 * Client GET request success case. Test request generation.
 * Request /time resource from libcoap example
 * Includes token of length GCOAP_TOKENLEN.
 */
static void test_gcoap__client_get_req(void)
{
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    size_t len;
    char path[] = "/time";

    /* Create expected pdu_data, with token length from GCOAP_TOKENLEN. */
    size_t hdr_fixed_len = 4;
    uint8_t hdr_fixed[]  = { 0x52, 0x01, 0xe6, 0x02 };
    size_t options_len   = 5;
    uint8_t options[]    = { 0xb4, 0x74, 0x69, 0x6d, 0x65 };

    uint8_t pdu_data[hdr_fixed_len + GCOAP_TOKENLEN + options_len];

    memcpy(&pdu_data[0], &hdr_fixed[0], hdr_fixed_len);
#if GCOAP_TOKENLEN
    /* actual value is random */
    memset(&pdu_data[hdr_fixed_len], 0x9b, GCOAP_TOKENLEN);
#endif
    memcpy(&pdu_data[hdr_fixed_len + GCOAP_TOKENLEN], &options[0], options_len);

    len = gcoap_request(&pdu, &buf[0], GCOAP_PDU_BUF_SIZE, COAP_METHOD_GET,
                                                           &path[0]);

    TEST_ASSERT_EQUAL_INT(COAP_METHOD_GET, coap_get_code(&pdu));
    TEST_ASSERT_EQUAL_INT(GCOAP_TOKENLEN, coap_get_token_len(&pdu));
    TEST_ASSERT_EQUAL_INT(hdr_fixed_len + GCOAP_TOKENLEN, coap_get_total_hdr_len(&pdu));
    TEST_ASSERT_EQUAL_INT(COAP_TYPE_NON, coap_get_type(&pdu));
    TEST_ASSERT_EQUAL_STRING(&path[0], (char *)&pdu.url[0]);
    TEST_ASSERT_EQUAL_INT(0, pdu.payload_len);
    TEST_ASSERT_EQUAL_INT(sizeof(pdu_data), len);
}

/*
 * Client GET response success case. Test parsing response.
 * Response for /time resource from libcoap example
 * Includes 2-byte token
 */
static void test_gcoap__client_get_resp(void)
{
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    int res;
    size_t hdr_fixed_len = 4;
    char exp_payload[]   = "Oct 22 10:46:48";
    size_t exp_tokenlen  = 2;

    uint8_t pdu_data[] = {
        0x52, 0x45, 0xe6, 0x02, 0x9b, 0xce, 0xc0, 0x21,
        0x01, 0xff, 0x4f, 0x63, 0x74, 0x20, 0x32, 0x32,
        0x20, 0x31, 0x30, 0x3a, 0x34, 0x36, 0x3a, 0x34,
        0x38
    };
    memcpy(buf, pdu_data, sizeof(pdu_data));

    res = coap_parse(&pdu, &buf[0], sizeof(pdu_data));

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(COAP_CLASS_SUCCESS, coap_get_code_class(&pdu));
    TEST_ASSERT_EQUAL_INT(exp_tokenlen, coap_get_token_len(&pdu));
    TEST_ASSERT_EQUAL_INT(hdr_fixed_len + exp_tokenlen, coap_get_total_hdr_len(&pdu));
    TEST_ASSERT_EQUAL_INT(COAP_TYPE_NON, coap_get_type(&pdu));
    TEST_ASSERT_EQUAL_INT(strlen(exp_payload), pdu.payload_len);

    for (size_t i = 0; i < strlen(exp_payload); i++) {
        TEST_ASSERT_EQUAL_INT(exp_payload[i], pdu.payload[i]);
    }
}

/*
 * Helper for server_get tests below.
 * Request from libcoap example for gcoap_cli /cli/stats resource
 * Include 2-byte token and Uri-Host option.
 */
static int _read_cli_stats_req(coap_pkt_t *pdu, uint8_t *buf)
{
    uint8_t pdu_data[] = {
        0x52, 0x01, 0x20, 0xb6, 0x35, 0x61, 0x3d, 0x10,
        0x66, 0x65, 0x38, 0x30, 0x3a, 0x3a, 0x38, 0x63,
        0x32, 0x3a, 0x31, 0x33, 0x66, 0x66, 0x3a, 0x66,
        0x65, 0x63, 0x30, 0x3a, 0x35, 0x65, 0x31, 0x32,
        0x25, 0x74, 0x61, 0x70, 0x30, 0x83, 0x63, 0x6c,
        0x69, 0x05, 0x73, 0x74, 0x61, 0x74, 0x73
    };
    memcpy(buf, pdu_data, sizeof(pdu_data));

    return coap_parse(pdu, buf, sizeof(pdu_data));
}

/* Server GET request success case. Validate request example. */
static void test_gcoap__server_get_req(void)
{
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;

    int res = _read_cli_stats_req(&pdu, &buf[0]);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(COAP_METHOD_GET, coap_get_code(&pdu));
    TEST_ASSERT_EQUAL_INT(2, coap_get_token_len(&pdu));
    TEST_ASSERT_EQUAL_INT(4 + 2, coap_get_total_hdr_len(&pdu));
    TEST_ASSERT_EQUAL_INT(COAP_TYPE_NON, coap_get_type(&pdu));
    TEST_ASSERT_EQUAL_INT(0, pdu.payload_len);
    TEST_ASSERT_EQUAL_STRING("/cli/stats", (char *) &pdu.url[0]);
}

/*
 * Server GET response success case. Test writing response.
 * Response for libcoap example for gcoap_cli /cli/stats resource
 */
static void test_gcoap__server_get_resp(void)
{
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;

    /* read request */
    _read_cli_stats_req(&pdu, &buf[0]);

    /* generate response */
    gcoap_resp_init(&pdu, &buf[0], sizeof(buf), COAP_CODE_CONTENT);
    char resp_payload[]  = "2";
    memcpy(&pdu.payload[0], &resp_payload[0], strlen(resp_payload));
    ssize_t res = gcoap_finish(&pdu, strlen(resp_payload), COAP_FORMAT_TEXT);

    uint8_t resp_data[] = {
        0x52, 0x45, 0x20, 0xb6, 0x35, 0x61, 0xc0, 0xff,
        0x32
    };

    TEST_ASSERT_EQUAL_INT(COAP_CLASS_SUCCESS, coap_get_code_class(&pdu));
    TEST_ASSERT_EQUAL_INT(2, coap_get_token_len(&pdu));
    TEST_ASSERT_EQUAL_INT(4 + 2, coap_get_total_hdr_len(&pdu));
    TEST_ASSERT_EQUAL_INT(COAP_TYPE_NON, coap_get_type(&pdu));
    TEST_ASSERT_EQUAL_INT(strlen(resp_payload), pdu.payload_len);
    TEST_ASSERT_EQUAL_INT(sizeof(resp_data), res);

    for (size_t i = 0; i < strlen(resp_payload); i++) {
        TEST_ASSERT_EQUAL_INT(resp_payload[i], pdu.payload[i]);
    }
}

/*
 * Helper for server_con_* tests below.
 * Confirmable request from libcoap example for gcoap_cli /cli/stats resource.
 * Include 2-byte token.
 */
static int _read_cli_stats_req_con(coap_pkt_t *pdu, uint8_t *buf)
{
    uint8_t pdu_data[] = {
        0x42, 0x01, 0x8e, 0x03, 0x35, 0x61, 0xb3, 0x63,
        0x6c, 0x69, 0x05, 0x73, 0x74, 0x61, 0x74, 0x73
    };
    memcpy(buf, pdu_data, sizeof(pdu_data));

    return coap_parse(pdu, buf, sizeof(pdu_data));
}

/* Server CON GET request success case. Validate request is confirmable. */
static void test_gcoap__server_con_req(void)
{
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;

    int res = _read_cli_stats_req_con(&pdu, &buf[0]);

    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_INT(COAP_METHOD_GET, coap_get_code(&pdu));
    TEST_ASSERT_EQUAL_INT(COAP_TYPE_CON, coap_get_type(&pdu));
}

/*
 * Server CON GET response success case. Test response is ACK.
 * Response for libcoap example for gcoap_cli /cli/stats resource
 */
static void test_gcoap__server_con_resp(void)
{
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;

    /* read request */
    _read_cli_stats_req_con(&pdu, &buf[0]);

    /* generate response */
    gcoap_resp_init(&pdu, &buf[0], sizeof(buf), COAP_CODE_CONTENT);
    char resp_payload[]  = "2";
    memcpy(&pdu.payload[0], &resp_payload[0], strlen(resp_payload));
    ssize_t res = gcoap_finish(&pdu, strlen(resp_payload), COAP_FORMAT_TEXT);

    uint8_t resp_data[] = {
        0x62, 0x45, 0x8e, 0x03, 0x35, 0x61, 0xc0, 0xff,
        0x30
    };

    TEST_ASSERT_EQUAL_INT(COAP_CLASS_SUCCESS, coap_get_code_class(&pdu));
    TEST_ASSERT_EQUAL_INT(COAP_TYPE_ACK, coap_get_type(&pdu));
    TEST_ASSERT_EQUAL_INT(sizeof(resp_data), res);
}

/*
 * Test the export of configured resources as CoRE link format string
 */
static void test_gcoap__server_get_resource_list(void)
{
    char res[128];
    int size = 0;

    gcoap_register_listener(&listener);
    gcoap_register_listener(&listener_second);

    size = gcoap_get_resource_list(NULL, 0, COAP_CT_LINK_FORMAT);
    TEST_ASSERT_EQUAL_INT(strlen(resource_list_str), size);

    res[0] = 'A';
    size = gcoap_get_resource_list(res, 0, COAP_CT_LINK_FORMAT);
    TEST_ASSERT_EQUAL_INT(0, size);
    TEST_ASSERT_EQUAL_INT((int)'A', (int)res[0]);

    size = gcoap_get_resource_list(res, 127, COAP_CT_LINK_FORMAT);
    res[size] = '\0';
    TEST_ASSERT_EQUAL_INT(strlen(resource_list_str), size);
    TEST_ASSERT_EQUAL_STRING(resource_list_str, (char *)res);
}

Test *tests_gcoap_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_gcoap__client_get_req),
        new_TestFixture(test_gcoap__client_get_resp),
        new_TestFixture(test_gcoap__server_get_req),
        new_TestFixture(test_gcoap__server_get_resp),
        new_TestFixture(test_gcoap__server_con_req),
        new_TestFixture(test_gcoap__server_con_resp),
        new_TestFixture(test_gcoap__server_get_resource_list)
    };

    EMB_UNIT_TESTCALLER(gcoap_tests, NULL, NULL, fixtures);

    return (Test *)&gcoap_tests;
}

void tests_gcoap(void)
{
    TESTS_RUN(tests_gcoap_tests());
}
/** @} */
