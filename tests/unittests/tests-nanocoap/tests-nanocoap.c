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

#include "embUnit.h"

#include "net/nanocoap.h"

#include "unittests-constants.h"
#include "tests-nanocoap.h"

/*
 * Validates encoded message ID byte order.
 */
static void test_nanocoap__req_msgid(void)
{
    uint8_t buf[128];
    uint16_t msgid = 0xABCD;
    char path[] = "/test";

    uint8_t *pktpos = &buf[0];
    pktpos += coap_build_hdr((coap_hdr_t *)pktpos, COAP_REQ, NULL, 0, COAP_METHOD_GET, msgid);
    pktpos += coap_put_option_uri(pktpos, 0, path, COAP_OPT_URI_PATH);

    coap_pkt_t pkt;
    coap_parse(&pkt, &buf[0], pktpos - &buf[0]);

    TEST_ASSERT_EQUAL_INT(msgid, coap_get_id(&pkt));
}

Test *tests_nanocoap_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nanocoap__req_msgid),
    };

    EMB_UNIT_TESTCALLER(nanocoap_tests, NULL, NULL, fixtures);

    return (Test *)&nanocoap_tests;
}

void tests_nanocoap(void)
{
    TESTS_RUN(tests_nanocoap_tests());
}
/** @} */
