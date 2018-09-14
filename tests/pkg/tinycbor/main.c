/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests tinycbor package
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>

#include "embUnit.h"
#include "embUnit/embUnit.h"
#include "fmt.h"
#include "cbor.h"

const char *tests[] = {
    "00",                  /* 0                       */
    "01",                  /* 1                       */
    "17",                  /* 23                      */
    "1818",                /* 24                      */
    "190100",              /* 256                     */
    "1a00010000",          /* 65536                   */
    "20",                  /* -1                      */
    "37",                  /* -24                     */
    "3818",                /* -25                     */
    "390100",              /* -257                    */
    "3a00010000",          /* -65537                  */
    "4161",                /* h"a"                    */
    "6161",                /* "a"                     */
    "80",                  /* []                      */
    "8100",                /* [0]                     */
    "820102",              /* [1,2]                   */
    "818100",              /* [[0]]                   */
    "a1616100",            /* {"a":0}                 */
    "d8184100",            /* tag                     */
    "f4",                  /* false                   */
    "f5",                  /* true                    */
    "f6",                  /* null                    */
    "f7",                  /* undefined               */
    "f8ff",                /* simple(255)             */

    "f93c00",              /* 1.0                     */
    "f9bc00",              /* -1.0                    */
    "f903ff",              /* 6.097555160522461e-05   */
    "f90400",              /* 6.103515625e-05         */
    "f907ff",              /* 0.00012201070785522461  */
    "f90800",              /* 0.0001220703125         */
    "fa47800000",          /* 65536.0                 */
    "fb3ff199999999999a",  /* 1.1                     */
    "f97e00",              /* NaN                     */

    "8300810000",          /* [ 0, [ 0], 0]           */
    "a26161016162820203",  /* { "a": 1, "b": [ 2, 3]} */
};

static void test_parse(void)
{
    static size_t idx;
    for (idx = 0; idx < sizeof(tests) / sizeof(char*); idx++) {
        CborParser parser;
        CborValue it;
        unsigned char buf[64] = {0};
        TEST_ASSERT((strlen(tests[idx])/2) <= sizeof(buf));

        size_t len = fmt_hex_bytes(buf, tests[idx]);
        TEST_ASSERT(len);

        CborError err = cbor_parser_init(buf, len, CborValidateStrictMode, &parser, &it);
        TEST_ASSERT_EQUAL_INT(err, 0);
    }
}

TestRef tests_tinycbor(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_parse),
    };

    EMB_UNIT_TESTCALLER(tests, NULL, NULL, fixtures);

    return (TestRef)&tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_tinycbor());
    TESTS_END();

    return 0;
}
