/*
 * Copyright (C) Lorenz Hüther, Mathias Detmers
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
  * @ingroup    tests
  * @{
  *
  * @file
  * @brief      Unit tests for pkg cn-cbor.
  *
  * @author     Lorenz Hüther <lorenz.huether@uni-bremen.de>
  * @author     Mathias Detmers <detmat@uni-bremen.de
  */

#define EBUF_SIZE 32
#define NUM_BLOCKS  7

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "cn-cbor/cn-cbor.h"
#include "embUnit.h"
#include "fmt.h"
#include "memarray.h"

typedef struct {
    char *hex;
    cn_cbor_error err;
} cbor_failure;

static size_t test, offs;
static unsigned char ebuf[EBUF_SIZE];
static cn_cbor_errback errb;

/* Block allocator */
static cn_cbor block_storage_data[NUM_BLOCKS];
static memarray_t storage;

/* calloc/free functions */
static void *cbor_calloc(size_t count, size_t size, void *memblock);
static void cbor_free(void *ptr, void *memblock);

/* CN_CBOR block allocator context struct*/
static cn_cbor_context ct =
{
    .calloc_func = cbor_calloc,
    .free_func = cbor_free,
    .context = &storage,
};

static void *cbor_calloc(size_t count, size_t size, void *memblock)
{
    (void)count;
    assert(count == 1); /* Count is always 1 with cn-cbor */
    void *block = memarray_alloc(memblock);
    if (block) {
        memset(block, 0, size);
    }
    return block;
}

static void cbor_free(void *ptr, void *memblock)
{
    memarray_free(memblock, ptr);
}

static void setup_cn_cbor(void)
{
    test = 0;
    offs = 0;
    memset(ebuf, '\0', EBUF_SIZE);
    memarray_init(&storage, block_storage_data, sizeof(cn_cbor), NUM_BLOCKS);
}

static void test_parse(void)
{
    char *tests[] = {
        "00",                       // 0
        "01",                       // 1
        "17",                       // 23
        "1818",                     // 24
        "190100",                   // 256
        "1a00010000",               // 65536
#ifndef CBOR_NO_LL
        "1b0000000100000000",       // 4294967296
#endif /* CBOR_NO_LL */

        "20",                       // -1
        "37",                       // -24
        "3818",                     // -25
        "390100",                   // -257
        "3a00010000",               // -65537
#ifndef CBOR_NO_LL
        "3b0000000100000000",       // -4294967297
#endif /* CBOR_LL */
        "4161",                     // h"a"
        "6161",                     // "a"
        "80",                       // []
        "8100",                     // [0]
        "820102",                   // [1,2]
        "818100",                   // [[0]]
        "a1616100",                 // {"a":0}
        "d8184100",                 // tag
        "f4",                       // false
        "f5",                       // true
        "f6",                       // null
        "f7",                       // undefined
        "f8ff",                     // simple(255)

#ifndef CBOR_NO_FLOAT
        "f93c00",                   // 1.0
        "f9bc00",                   // -1.0
        "f903ff",                   // 6.097555160522461e-05
        "f90400",                   // 6.103515625e-05
        "f907ff",                   // 0.00012201070785522461
        "f90800",                   // 0.0001220703125
        "fa47800000",               // 65536.0
        "fb3ff199999999999a",       // 1.1
        "f97e00",                   // NaN
#endif /* CBOR_NO_FLOAT */

        "5f42010243030405ff",       // (_ h'0102', h'030405')
        "7f61616161ff",             // (_ "a", "a")
        "9fff",                     // [_ ]
        "9f9f9fffffff",             // [_ [_ [_ ]]]
        "9f009f00ff00ff",           // [_ 0, [_ 0], 0]
        "bf61610161629f0203ffff",   // {_ "a": 1, "b": [_ 2, 3]}
    };

    for (test = 0; test < sizeof(tests) / sizeof(char*); test++) {
        unsigned char buf[64] = {0};
        TEST_ASSERT((strlen(tests[test])/2) <= sizeof(buf));

        size_t len = fmt_hex_bytes(buf, tests[test]);
        TEST_ASSERT(len);

        errb.err = CN_CBOR_NO_ERROR;

        cn_cbor *cbor = cn_cbor_decode(buf, len, &ct, &errb);
        TEST_ASSERT_EQUAL_INT(errb.err, CN_CBOR_NO_ERROR);
        TEST_ASSERT_NOT_NULL(cbor);

        cn_cbor_encoder_write(ebuf, 0, sizeof(ebuf), cbor);
        for (offs = 0; offs < len; offs++) {
            TEST_ASSERT_EQUAL_INT(buf[offs], ebuf[offs]);
        }
        cn_cbor_free(cbor, &ct);
    }
}

static void test_errors(void)
{
    cbor_failure tests[] = {
        {"81", CN_CBOR_ERR_OUT_OF_DATA},
        {"0000", CN_CBOR_ERR_NOT_ALL_DATA_CONSUMED},
        {"bf00ff", CN_CBOR_ERR_ODD_SIZE_INDEF_MAP},
        {"ff", CN_CBOR_ERR_BREAK_OUTSIDE_INDEF},
        {"1f", CN_CBOR_ERR_MT_UNDEF_FOR_INDEF},
        {"1c", CN_CBOR_ERR_RESERVED_AI},
        {"7f4100", CN_CBOR_ERR_WRONG_NESTING_IN_INDEF_STRING},
    };

    cn_cbor inv = {CN_CBOR_INVALID, 0, {0}, 0, NULL, NULL, NULL, NULL};

    TEST_ASSERT_EQUAL_INT(-1, cn_cbor_encoder_write(ebuf, 0, sizeof(ebuf),
            &inv));

    for (offs = 0; offs < sizeof(tests) / sizeof(cbor_failure); offs++) {
        unsigned char buf[32] = {0};
        TEST_ASSERT((strlen(tests[offs].hex)/2) <= sizeof(buf));

        size_t len = fmt_hex_bytes(buf, tests[offs].hex);
        TEST_ASSERT(len);

        cn_cbor *cbor = cn_cbor_decode(buf, len, &ct, &errb);
        TEST_ASSERT_NULL(cbor);
        TEST_ASSERT_EQUAL_INT(errb.err, tests[offs].err);
        cn_cbor_free(cbor, &ct);
    }
}

TestRef test_cn_cbor(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_parse),
        new_TestFixture(test_errors)
    };

    EMB_UNIT_TESTCALLER(tests_cn_cbor, setup_cn_cbor, NULL, fixtures);
    return (TestRef) & tests_cn_cbor;
}

void tests_cn_cbor(void)
{
    TESTS_RUN(test_cn_cbor());
}
