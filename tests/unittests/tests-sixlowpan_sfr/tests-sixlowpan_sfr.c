/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <string.h>

#include "net/sixlowpan/sfr.h"

#include "tests-sixlowpan_sfr.h"

static void test_sixlowpan_sfr_disp(void)
{
    sixlowpan_sfr_t hdr = { .disp_ecn = 0U };

    TEST_ASSERT(!sixlowpan_sfr_is(&hdr));
    sixlowpan_sfr_rfrag_set_disp(&hdr);
    TEST_ASSERT(sixlowpan_sfr_is(&hdr));
    hdr.disp_ecn = 0U;
    sixlowpan_sfr_ack_set_disp(&hdr);
    TEST_ASSERT(sixlowpan_sfr_is(&hdr));
}

static void test_sixlowpan_sfr_ecn(void)
{
    /* initialize with canary (but ECN unset) */
    sixlowpan_sfr_t hdr = { .disp_ecn = 218U };

    /* ECN should not be set */
    TEST_ASSERT(!sixlowpan_sfr_ecn(&hdr));

    sixlowpan_sfr_set_ecn(&hdr);
    TEST_ASSERT(sixlowpan_sfr_ecn(&hdr));
    /* check if non-ECN field stays unset */
    TEST_ASSERT_EQUAL_INT(218U,
                          hdr.disp_ecn & ~SIXLOWPAN_SFR_ECN);

    sixlowpan_sfr_clear_ecn(&hdr);
    TEST_ASSERT(!sixlowpan_sfr_ecn(&hdr));
    /* check if non-ECN field stays unset */
    TEST_ASSERT_EQUAL_INT(218U,
                          hdr.disp_ecn & ~SIXLOWPAN_SFR_ECN);
}

static void test_sixlowpan_sfr_rfrag_disp(void)
{
    /* initialize with canary bit (but dispatch unset) */
    sixlowpan_sfr_t hdr = { .disp_ecn = 1U };

    TEST_ASSERT(!sixlowpan_sfr_rfrag_is(&hdr));
    sixlowpan_sfr_rfrag_set_disp(&hdr);
    TEST_ASSERT(sixlowpan_sfr_rfrag_is(&hdr));
    /* check if preset value stays the same */
    TEST_ASSERT_EQUAL_INT(1U, hdr.disp_ecn & ~SIXLOWPAN_SFR_DISP_MASK);
    /* check if it stays the same when ECN is cleared */
    sixlowpan_sfr_clear_ecn(&hdr);
    TEST_ASSERT(sixlowpan_sfr_rfrag_is(&hdr));
    /* check if it stays the same when ECN is set */
    sixlowpan_sfr_set_ecn(&hdr);
    TEST_ASSERT(sixlowpan_sfr_rfrag_is(&hdr));
}

static void test_sixlowpan_sfr_rfrag_ack_req(void)
{
    /* initialize with canary bit (but ack_req unset) */
    sixlowpan_sfr_rfrag_t hdr = { .ar_seq_fs = { .u8 = { 0x74, 0x32 } } };

    TEST_ASSERT(!sixlowpan_sfr_rfrag_ack_req(&hdr));
    sixlowpan_sfr_rfrag_set_ack_req(&hdr);
    TEST_ASSERT(sixlowpan_sfr_rfrag_ack_req(&hdr));
    /* check if preset values stay the same */
    TEST_ASSERT_EQUAL_INT(0x74, hdr.ar_seq_fs.u8[0] & ~SIXLOWPAN_SFR_ACK_REQ);
    TEST_ASSERT_EQUAL_INT(0x32, hdr.ar_seq_fs.u8[1]);

    sixlowpan_sfr_rfrag_clear_ack_req(&hdr);
    TEST_ASSERT(!sixlowpan_sfr_rfrag_ack_req(&hdr));
    /* check if preset values stay the same */
    TEST_ASSERT_EQUAL_INT(0x74, hdr.ar_seq_fs.u8[0] & ~SIXLOWPAN_SFR_ACK_REQ);
    TEST_ASSERT_EQUAL_INT(0x32, hdr.ar_seq_fs.u8[1]);
}

static void test_sixlowpan_sfr_rfrag_seq(void)
{
    /* initialize with canary bit (but seq unset) */
    sixlowpan_sfr_rfrag_t hdr = { .ar_seq_fs = { .u8 = { 0x83, 0xaa } } };

    TEST_ASSERT_EQUAL_INT(0U, sixlowpan_sfr_rfrag_get_seq(&hdr));

    sixlowpan_sfr_rfrag_set_seq(&hdr, SIXLOWPAN_SFR_SEQ_MAX);
    TEST_ASSERT_EQUAL_INT(SIXLOWPAN_SFR_SEQ_MAX,
                          sixlowpan_sfr_rfrag_get_seq(&hdr));
    /* check if preset values stay the same */
    TEST_ASSERT_EQUAL_INT(0x83, hdr.ar_seq_fs.u8[0] & ~SIXLOWPAN_SFR_SEQ_MASK);
    TEST_ASSERT_EQUAL_INT(0xaa, hdr.ar_seq_fs.u8[1]);

    sixlowpan_sfr_rfrag_set_seq(&hdr, 19U);
    TEST_ASSERT_EQUAL_INT(19U, sixlowpan_sfr_rfrag_get_seq(&hdr));
    /* check if preset values stay the same */
    TEST_ASSERT_EQUAL_INT(0x83, hdr.ar_seq_fs.u8[0] & ~SIXLOWPAN_SFR_SEQ_MASK);
    TEST_ASSERT_EQUAL_INT(0xaa, hdr.ar_seq_fs.u8[1]);

    sixlowpan_sfr_rfrag_set_seq(&hdr, 7U);
    TEST_ASSERT_EQUAL_INT(7U, sixlowpan_sfr_rfrag_get_seq(&hdr));
    /* check if preset values stay the same */
    TEST_ASSERT_EQUAL_INT(0x83, hdr.ar_seq_fs.u8[0] & ~SIXLOWPAN_SFR_SEQ_MASK);
    TEST_ASSERT_EQUAL_INT(0xaa, hdr.ar_seq_fs.u8[1]);

    sixlowpan_sfr_rfrag_set_seq(&hdr, 0U);
    TEST_ASSERT_EQUAL_INT(0U, sixlowpan_sfr_rfrag_get_seq(&hdr));
    /* check if preset values stay the same */
    TEST_ASSERT_EQUAL_INT(0x83, hdr.ar_seq_fs.u8[0] & ~SIXLOWPAN_SFR_SEQ_MASK);
    TEST_ASSERT_EQUAL_INT(0xaa, hdr.ar_seq_fs.u8[1]);
}

static void test_sixlowpan_sfr_rfrag_frag_size(void)
{
    /* initialize with canary bit (but seq unset) */
    sixlowpan_sfr_rfrag_t hdr = { .ar_seq_fs = { .u8 = { 0x6c, 0x00 } } };

    TEST_ASSERT_EQUAL_INT(0U, sixlowpan_sfr_rfrag_get_frag_size(&hdr));

    sixlowpan_sfr_rfrag_set_frag_size(&hdr, SIXLOWPAN_SFR_FRAG_SIZE_MAX);
    TEST_ASSERT_EQUAL_INT(SIXLOWPAN_SFR_FRAG_SIZE_MAX,
                          sixlowpan_sfr_rfrag_get_frag_size(&hdr));
    /* check if preset values stay the same */
    TEST_ASSERT_EQUAL_INT(0x6c,
                          hdr.ar_seq_fs.u8[0] &
                          (SIXLOWPAN_SFR_ACK_REQ | SIXLOWPAN_SFR_SEQ_MASK));

    sixlowpan_sfr_rfrag_set_frag_size(&hdr, 113);
    TEST_ASSERT_EQUAL_INT(113U, sixlowpan_sfr_rfrag_get_frag_size(&hdr));
    /* check if preset values stay the same */
    TEST_ASSERT_EQUAL_INT(0x6c,
                          hdr.ar_seq_fs.u8[0] &
                          (SIXLOWPAN_SFR_ACK_REQ | SIXLOWPAN_SFR_SEQ_MASK));

    sixlowpan_sfr_rfrag_set_frag_size(&hdr, 964);
    TEST_ASSERT_EQUAL_INT(964, sixlowpan_sfr_rfrag_get_frag_size(&hdr));
    /* check if preset values stay the same */
    TEST_ASSERT_EQUAL_INT(0x6c,
                          hdr.ar_seq_fs.u8[0] &
                          (SIXLOWPAN_SFR_ACK_REQ | SIXLOWPAN_SFR_SEQ_MASK));

    sixlowpan_sfr_rfrag_set_frag_size(&hdr, 0U);
    TEST_ASSERT_EQUAL_INT(0U, sixlowpan_sfr_rfrag_get_frag_size(&hdr));
    /* check if preset values stay the same */
    TEST_ASSERT_EQUAL_INT(0x6c,
                          hdr.ar_seq_fs.u8[0] &
                          (SIXLOWPAN_SFR_ACK_REQ | SIXLOWPAN_SFR_SEQ_MASK));
}

static void test_sixlowpan_sfr_rfrag_offset(void)
{
    /* initialize with canary bit (but seq unset) */
    sixlowpan_sfr_rfrag_t hdr = { .offset = { .u16 = 0 } };

    TEST_ASSERT_EQUAL_INT(0U, sixlowpan_sfr_rfrag_get_offset(&hdr));

    sixlowpan_sfr_rfrag_set_offset(&hdr, UINT16_MAX);
    TEST_ASSERT_EQUAL_INT(UINT16_MAX, sixlowpan_sfr_rfrag_get_offset(&hdr));

    sixlowpan_sfr_rfrag_set_offset(&hdr, 58937U);
    TEST_ASSERT_EQUAL_INT(58937U, sixlowpan_sfr_rfrag_get_offset(&hdr));

    sixlowpan_sfr_rfrag_set_offset(&hdr, 173U);
    TEST_ASSERT_EQUAL_INT(173U, sixlowpan_sfr_rfrag_get_offset(&hdr));

    sixlowpan_sfr_rfrag_set_offset(&hdr, 0U);
    TEST_ASSERT_EQUAL_INT(0U, sixlowpan_sfr_rfrag_get_offset(&hdr));
}

static void test_sixlowpan_sfr_ack_disp(void)
{
    /* initialize with canary bit (but dispatch unset) */
    sixlowpan_sfr_t hdr = { .disp_ecn = 1U };

    TEST_ASSERT(!sixlowpan_sfr_ack_is(&hdr));
    sixlowpan_sfr_ack_set_disp(&hdr);
    TEST_ASSERT(sixlowpan_sfr_ack_is(&hdr));
    /* check if preset value stays the same */
    TEST_ASSERT_EQUAL_INT(1U, hdr.disp_ecn & ~SIXLOWPAN_SFR_DISP_MASK);
    /* check if it stays the same when ECN is cleared */
    sixlowpan_sfr_clear_ecn(&hdr);
    TEST_ASSERT(sixlowpan_sfr_ack_is(&hdr));
    /* check if it stays the same when ECN is set */
    sixlowpan_sfr_set_ecn(&hdr);
    TEST_ASSERT(sixlowpan_sfr_ack_is(&hdr));
}

static Test *tests_sixlowpan_sfr_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sixlowpan_sfr_disp),
        new_TestFixture(test_sixlowpan_sfr_ecn),
        new_TestFixture(test_sixlowpan_sfr_rfrag_disp),
        new_TestFixture(test_sixlowpan_sfr_rfrag_ack_req),
        new_TestFixture(test_sixlowpan_sfr_rfrag_seq),
        new_TestFixture(test_sixlowpan_sfr_rfrag_frag_size),
        new_TestFixture(test_sixlowpan_sfr_rfrag_offset),
        new_TestFixture(test_sixlowpan_sfr_ack_disp),
    };

    EMB_UNIT_TESTCALLER(sixlowpan_sfr_tests, NULL, NULL, fixtures);

    return (Test *)&sixlowpan_sfr_tests;
}

void tests_sixlowpan_sfr(void)
{
    TESTS_RUN(tests_sixlowpan_sfr_tests());
}

/** @} */
