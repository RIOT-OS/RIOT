/*
 * Copyright (C) 2020 Locha Inc
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

#include "embUnit.h"
#include "byteorder.h"

#include "net/rfc5444.h"

#define SEQ_NUM     (2048U) /**< Sequence number for testing */
#define TLV_TYPE    (1U)    /**< TLV type */
#define TLV_LEN     (1U)    /**< TLV length */
#define TLV_VALUE   (0xCA)  /**< 1-byte TLV value */

static void test_pkt_hdr_version__RFC5444_VERSION(void)
{
    const rfc5444_pkt_hdr_t hdr = {
        .version_and_flags = RFC5444_VERSION << 4,
    };

    TEST_ASSERT_EQUAL_INT(RFC5444_VERSION, rfc5444_pkt_hdr_version(&hdr));
}

static void test_pkt_hdr_flags__all(void)
{
    const rfc5444_pkt_hdr_t hdr = {
        .version_and_flags = (RFC5444_VERSION << 4)
                           | RFC5444_PKT_HDR_FLAG_HAS_SEQ_NUM
                           | RFC5444_PKT_HDR_FLAG_HAS_TLV,
    };

    const uint8_t flags = rfc5444_pkt_hdr_flags(&hdr);

    TEST_ASSERT_EQUAL_INT(RFC5444_PKT_HDR_FLAG_HAS_SEQ_NUM,
                          flags & RFC5444_PKT_HDR_FLAG_HAS_SEQ_NUM);
    TEST_ASSERT_EQUAL_INT(RFC5444_PKT_HDR_FLAG_HAS_TLV,
                          flags & RFC5444_PKT_HDR_FLAG_HAS_TLV);
}

static void test_pkt_hdr_extra__no_seq_num_no_tlv_block(void)
{
    rfc5444_pkt_hdr_t hdr = {
        .version_and_flags = RFC5444_VERSION << 4,
    };

    /* doesn't provide a seqnum so it's 0 */
    TEST_ASSERT_EQUAL_INT(-1, rfc5444_pkt_hdr_seq_num(&hdr));
    TEST_ASSERT_NULL(rfc5444_pkt_hdr_tlv_block(&hdr));
}

static void test_pkt_hdr_extra__seq_num(void)
{
    uint8_t pkt[3];
    const rfc5444_pkt_hdr_t *hdr = (rfc5444_pkt_hdr_t *)pkt;

    pkt[0] = (RFC5444_VERSION << 4)
           | RFC5444_PKT_HDR_FLAG_HAS_SEQ_NUM;

    byteorder_htobebufs(&pkt[1], SEQ_NUM);
    TEST_ASSERT_EQUAL_INT(SEQ_NUM, rfc5444_pkt_hdr_seq_num(hdr));
}

static void test_pkt_hdr_extra__tlv_block(void)
{
    rfc5444_tlv_block_t *tlv_block;
    rfc5444_tlv_iter_t iter;
    rfc5444_tlv_t *tlv;
    uint8_t pkt[] = {
        (RFC5444_VERSION << 4) | RFC5444_PKT_HDR_FLAG_HAS_TLV,
        /* length of TLV block */
        0,
        4,
        /* TLV */
        TLV_TYPE,
        RFC5444_TLV_FLAG_HAS_VALUE,
        TLV_LEN,
        TLV_VALUE,
    };
    rfc5444_pkt_hdr_t *hdr = (rfc5444_pkt_hdr_t *)pkt;

    tlv_block = rfc5444_pkt_hdr_tlv_block(hdr);
    TEST_ASSERT_NOT_NULL(tlv_block);
    TEST_ASSERT_EQUAL_INT(6, rfc5444_tlv_block_sizeof(tlv_block));
    TEST_ASSERT_EQUAL_INT(4, rfc5444_tlv_block_len(tlv_block));

    rfc5444_tlv_iter_init(&iter, tlv_block);
    tlv = rfc5444_tlv_iter_next(&iter);
    TEST_ASSERT_NOT_NULL(tlv);

    /* we don't have more TLVs */
    TEST_ASSERT_NULL(rfc5444_tlv_iter_next(&iter));

    TEST_ASSERT_EQUAL_INT(TLV_TYPE, tlv->type);
    TEST_ASSERT_EQUAL_INT(TLV_LEN, rfc5444_tlv_len(tlv));
    TEST_ASSERT_EQUAL_INT(TLV_VALUE, *rfc5444_tlv_value(tlv));
}

static void test_pkt_hdr_extra__seq_num_tlv_block(void)
{
    rfc5444_tlv_block_t *tlv_block;
    rfc5444_tlv_iter_t iter;
    rfc5444_tlv_t *tlv;
    uint8_t pkt[] = {
          (RFC5444_VERSION << 4)
        | RFC5444_PKT_HDR_FLAG_HAS_TLV
        | RFC5444_PKT_HDR_FLAG_HAS_SEQ_NUM,
        /* seqnum, set with byteorder functions */
        0,
        0,
        /* length of TLV block */
        0,
        4,
        /* TLV */
        TLV_TYPE,
        RFC5444_TLV_FLAG_HAS_VALUE,
        TLV_LEN,
        TLV_VALUE,
    };
    rfc5444_pkt_hdr_t *hdr = (rfc5444_pkt_hdr_t *)pkt;

    byteorder_htobebufs(&pkt[1], SEQ_NUM);
    TEST_ASSERT_EQUAL_INT(SEQ_NUM, rfc5444_pkt_hdr_seq_num(hdr));

    tlv_block = rfc5444_pkt_hdr_tlv_block(hdr);
    TEST_ASSERT_NOT_NULL(tlv_block);
    TEST_ASSERT_EQUAL_INT(6, rfc5444_tlv_block_sizeof(tlv_block));
    TEST_ASSERT_EQUAL_INT(4, rfc5444_tlv_block_len(tlv_block));

    rfc5444_tlv_iter_init(&iter, tlv_block);
    tlv = rfc5444_tlv_iter_next(&iter);
    TEST_ASSERT_NOT_NULL(tlv);

    /* we don't have more TLVs */
    TEST_ASSERT_NULL(rfc5444_tlv_iter_next(&iter));

    TEST_ASSERT_EQUAL_INT(TLV_TYPE, tlv->type);
    TEST_ASSERT_EQUAL_INT(TLV_LEN, rfc5444_tlv_len(tlv));
    TEST_ASSERT_EQUAL_INT(TLV_VALUE, *rfc5444_tlv_value(tlv));
}

Test *tests_rfc5444_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pkt_hdr_version__RFC5444_VERSION),
        new_TestFixture(test_pkt_hdr_flags__all),
        new_TestFixture(test_pkt_hdr_extra__no_seq_num_no_tlv_block),
        new_TestFixture(test_pkt_hdr_extra__seq_num),
        new_TestFixture(test_pkt_hdr_extra__tlv_block),
        new_TestFixture(test_pkt_hdr_extra__seq_num_tlv_block),
    };

    EMB_UNIT_TESTCALLER(rfc5444_tests, NULL, NULL, fixtures);

    return (Test *)&rfc5444_tests;
}

void tests_rfc5444(void)
{
    TESTS_RUN(tests_rfc5444_tests());
}

/** @} */
