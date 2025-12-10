/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "embUnit/embUnit.h"

#include "byteorder.h"
#include "net/ieee802154.h"

#include "unittests-constants.h"
#include "tests-ieee802154.h"

static void test_ieee802154_set_frame_hdr_flags0(void)
{
    const le_uint16_t src_pan = byteorder_htols(0);
    const le_uint16_t dst_pan = byteorder_htols(0);
    const uint8_t exp[] = { 0x00, IEEE802154_FCF_VERS_V1, TEST_UINT8 };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = 0;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, NULL, 0,
                                                   NULL, 0,
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_flags0_non_beacon_non_ack(void)
{
    const le_uint16_t src_pan = byteorder_htols(0);
    const le_uint16_t dst_pan = byteorder_htols(0);
    const uint8_t flags = IEEE802154_FCF_TYPE_DATA;
    uint8_t res[2];

    TEST_ASSERT_EQUAL_INT(0,
                          ieee802154_set_frame_hdr(res, NULL, 0,
                                                   NULL, 0,
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
}

static void test_ieee802154_set_frame_hdr_bcast_src0(void)
{
    const le_uint16_t src_pan = byteorder_htols(0);
    const le_uint16_t dst_pan = byteorder_htols(TEST_UINT16);
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { 0x00,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_SHORT,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            ieee802154_addr_bcast[0], ieee802154_addr_bcast[1] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = 0;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, NULL, 0,
                                                   ieee802154_addr_bcast,
                                                   IEEE802154_ADDR_BCAST_LEN,
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_bcast_src2(void)
{
    const network_uint16_t src = byteorder_htons(TEST_UINT16);
    const le_uint16_t src_pan = byteorder_htols(TEST_UINT16 + 1);
    const le_uint16_t dst_pan = byteorder_htols(TEST_UINT16 + 2);
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { 0x00,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_SHORT |
                                IEEE802154_FCF_SRC_ADDR_SHORT,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            ieee802154_addr_bcast[0], ieee802154_addr_bcast[1],
                            src_pan.u8[0], src_pan.u8[1],
                            src.u8[1], src.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = 0;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, src.u8, sizeof(src),
                                                   ieee802154_addr_bcast,
                                                   IEEE802154_ADDR_BCAST_LEN,
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_bcast_src8(void)
{
    const network_uint64_t src = byteorder_htonll(TEST_UINT64);
    const le_uint16_t src_pan = byteorder_htols(TEST_UINT16);
    const le_uint16_t dst_pan = byteorder_htols(TEST_UINT16 + 1);
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { 0x00,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_SHORT |
                                IEEE802154_FCF_SRC_ADDR_LONG,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            ieee802154_addr_bcast[0], ieee802154_addr_bcast[1],
                            src_pan.u8[0], src_pan.u8[1],
                            src.u8[7], src.u8[6], src.u8[5], src.u8[4],
                            src.u8[3], src.u8[2], src.u8[1], src.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = 0;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, src.u8, sizeof(src),
                                                   ieee802154_addr_bcast,
                                                   IEEE802154_ADDR_BCAST_LEN,
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_dst2_src0(void)
{
    const network_uint16_t dst = byteorder_htons(TEST_UINT16);
    const le_uint16_t src_pan = byteorder_htols(0);
    const le_uint16_t dst_pan = byteorder_htols(TEST_UINT16 + 1);
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { IEEE802154_FCF_TYPE_DATA,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_SHORT |
                                IEEE802154_FCF_SRC_ADDR_VOID,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            dst.u8[1], dst.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = IEEE802154_FCF_TYPE_DATA;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, NULL, 0,
                                                   dst.u8, sizeof(dst),
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_dst2_src2(void)
{
    const network_uint16_t src = byteorder_htons(TEST_UINT16);
    const le_uint16_t src_pan = byteorder_htols(TEST_UINT16 + 1);
    const network_uint16_t dst = byteorder_htons(TEST_UINT16 + 2);
    const le_uint16_t dst_pan = byteorder_htols(TEST_UINT16 + 3);
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { IEEE802154_FCF_TYPE_DATA,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_SHORT |
                                IEEE802154_FCF_SRC_ADDR_SHORT,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            dst.u8[1], dst.u8[0],
                            src_pan.u8[0], src_pan.u8[1],
                            src.u8[1], src.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = IEEE802154_FCF_TYPE_DATA;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, src.u8, sizeof(src),
                                                   dst.u8, sizeof(dst),
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_dst2_src2_pancomp(void)
{
    const network_uint16_t src = byteorder_htons(TEST_UINT16);
    const le_uint16_t src_pan = byteorder_htols(TEST_UINT16 + 1);
    const network_uint16_t dst = byteorder_htons(TEST_UINT16 + 2);
    const le_uint16_t dst_pan = src_pan;
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { IEEE802154_FCF_TYPE_DATA | IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_SHORT |
                                IEEE802154_FCF_SRC_ADDR_SHORT,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            dst.u8[1], dst.u8[0],
                            /* src_pan compressed (and assumed equal to dst_pan) */
                            src.u8[1], src.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = IEEE802154_FCF_TYPE_DATA;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, src.u8, sizeof(src),
                                                   dst.u8, sizeof(dst),
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_dst2_src8(void)
{
    const network_uint64_t src = byteorder_htonll(TEST_UINT64);
    const le_uint16_t src_pan = byteorder_htols(TEST_UINT16);
    const network_uint16_t dst = byteorder_htons(TEST_UINT16 + 1);
    const le_uint16_t dst_pan = byteorder_htols(TEST_UINT16 + 2);
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { IEEE802154_FCF_TYPE_DATA,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_SHORT |
                                IEEE802154_FCF_SRC_ADDR_LONG,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            dst.u8[1], dst.u8[0],
                            src_pan.u8[0], src_pan.u8[1],
                            src.u8[7], src.u8[6], src.u8[5], src.u8[4],
                            src.u8[3], src.u8[2], src.u8[1], src.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = IEEE802154_FCF_TYPE_DATA;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, src.u8, sizeof(src),
                                                   dst.u8, sizeof(dst),
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_dst2_src8_pancomp(void)
{
    const network_uint64_t src = byteorder_htonll(TEST_UINT64);
    const le_uint16_t src_pan = byteorder_htols(TEST_UINT16);
    const network_uint16_t dst = byteorder_htons(TEST_UINT16 + 1);
    const le_uint16_t dst_pan = src_pan;
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { IEEE802154_FCF_TYPE_DATA | IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_SHORT |
                                IEEE802154_FCF_SRC_ADDR_LONG,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            dst.u8[1], dst.u8[0],
                            /* src_pan compressed (and assumed equal to dst_pan) */
                            src.u8[7], src.u8[6], src.u8[5], src.u8[4],
                            src.u8[3], src.u8[2], src.u8[1], src.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = IEEE802154_FCF_TYPE_DATA;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, src.u8, sizeof(src),
                                                   dst.u8, sizeof(dst),
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_dst8_src0(void)
{
    const network_uint64_t dst = byteorder_htonll(TEST_UINT64);
    const le_uint16_t src_pan = byteorder_htols(0);
    const le_uint16_t dst_pan = byteorder_htols(TEST_UINT16);
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { IEEE802154_FCF_TYPE_BEACON,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_LONG |
                                IEEE802154_FCF_SRC_ADDR_VOID,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            dst.u8[7], dst.u8[6], dst.u8[5], dst.u8[4],
                            dst.u8[3], dst.u8[2], dst.u8[1], dst.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = IEEE802154_FCF_TYPE_BEACON;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, NULL, 0,
                                                   dst.u8, sizeof(dst),
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_dst8_src2(void)
{
    const network_uint16_t src = byteorder_htons(TEST_UINT16);
    const le_uint16_t src_pan = byteorder_htols(TEST_UINT16 + 1);
    const network_uint64_t dst = byteorder_htonll(TEST_UINT64);
    const le_uint16_t dst_pan = byteorder_htols(TEST_UINT16 + 2);
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { IEEE802154_FCF_TYPE_BEACON,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_LONG |
                                IEEE802154_FCF_SRC_ADDR_SHORT,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            dst.u8[7], dst.u8[6], dst.u8[5], dst.u8[4],
                            dst.u8[3], dst.u8[2], dst.u8[1], dst.u8[0],
                            src_pan.u8[0], src_pan.u8[1],
                            src.u8[1], src.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = IEEE802154_FCF_TYPE_BEACON;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, src.u8, sizeof(src),
                                                   dst.u8, sizeof(dst),
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_dst8_src2_pancomp(void)
{
    const network_uint16_t src = byteorder_htons(TEST_UINT16);
    const le_uint16_t src_pan = byteorder_htols(TEST_UINT16 + 1);
    const network_uint64_t dst = byteorder_htonll(TEST_UINT64);
    const le_uint16_t dst_pan = src_pan;
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { IEEE802154_FCF_TYPE_BEACON | IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_LONG |
                                IEEE802154_FCF_SRC_ADDR_SHORT,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            dst.u8[7], dst.u8[6], dst.u8[5], dst.u8[4],
                            dst.u8[3], dst.u8[2], dst.u8[1], dst.u8[0],
                            /* src_pan compressed (and assumed equal to dst_pan) */
                            src.u8[1], src.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = IEEE802154_FCF_TYPE_BEACON;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, src.u8, sizeof(src),
                                                   dst.u8, sizeof(dst),
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_dst8_src8(void)
{
    const network_uint64_t src = byteorder_htonll(TEST_UINT64);
    const le_uint16_t src_pan = byteorder_htols(TEST_UINT16);
    const network_uint64_t dst = byteorder_htonll(TEST_UINT64);
    const le_uint16_t dst_pan = byteorder_htols(TEST_UINT16 + 1);
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { IEEE802154_FCF_TYPE_BEACON,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_LONG |
                                IEEE802154_FCF_SRC_ADDR_LONG,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            dst.u8[7], dst.u8[6], dst.u8[5], dst.u8[4],
                            dst.u8[3], dst.u8[2], dst.u8[1], dst.u8[0],
                            src_pan.u8[0], src_pan.u8[1],
                            src.u8[7], src.u8[6], src.u8[5], src.u8[4],
                            src.u8[3], src.u8[2], src.u8[1], src.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = IEEE802154_FCF_TYPE_BEACON;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, src.u8, sizeof(src),
                                                   dst.u8, sizeof(dst),
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_set_frame_hdr_dst8_src8_pancomp(void)
{
    const network_uint64_t src = byteorder_htonll(TEST_UINT64);
    const le_uint16_t src_pan = byteorder_htols(TEST_UINT16);
    const network_uint64_t dst = byteorder_htonll(TEST_UINT64);
    const le_uint16_t dst_pan = src_pan;
    /* IEEE 802.15.4 is little endian! */
    const uint8_t exp[] = { IEEE802154_FCF_TYPE_BEACON | IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_VERS_V1 |
                                IEEE802154_FCF_DST_ADDR_LONG |
                                IEEE802154_FCF_SRC_ADDR_LONG,
                            TEST_UINT8,
                            dst_pan.u8[0], dst_pan.u8[1],
                            dst.u8[7], dst.u8[6], dst.u8[5], dst.u8[4],
                            dst.u8[3], dst.u8[2], dst.u8[1], dst.u8[0],
                            /* src_pan compressed (and assumed equal to dst_pan) */
                            src.u8[7], src.u8[6], src.u8[5], src.u8[4],
                            src.u8[3], src.u8[2], src.u8[1], src.u8[0] };
    uint8_t res[sizeof(exp)];
    const uint8_t flags = IEEE802154_FCF_TYPE_BEACON;

    TEST_ASSERT_EQUAL_INT(sizeof(exp),
                          ieee802154_set_frame_hdr(res, src.u8, sizeof(src),
                                                   dst.u8, sizeof(dst),
                                                   src_pan, dst_pan,
                                                   flags, TEST_UINT8));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp, res, sizeof(exp)));
}

static void test_ieee802154_get_frame_hdr_len_dst0_src0(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA, IEEE802154_FCF_DST_ADDR_VOID |
                                                      IEEE802154_FCF_SRC_ADDR_VOID };

    /* either source or destination are required, so expect an error */
    TEST_ASSERT_EQUAL_INT(0, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dstr(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA, IEEE802154_FCF_DST_ADDR_RESV };

    TEST_ASSERT_EQUAL_INT(0, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_srcr(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA, IEEE802154_FCF_DST_ADDR_RESV };

    TEST_ASSERT_EQUAL_INT(0, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dst2_src0(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA, IEEE802154_FCF_DST_ADDR_SHORT |
                                                      IEEE802154_FCF_SRC_ADDR_VOID };

    TEST_ASSERT_EQUAL_INT(7, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dst8_src0(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA, IEEE802154_FCF_DST_ADDR_LONG };

    TEST_ASSERT_EQUAL_INT(13, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dst0_src2(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA, IEEE802154_FCF_SRC_ADDR_SHORT };

    TEST_ASSERT_EQUAL_INT(7, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dst0_src2_pancomp(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA | IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_SRC_ADDR_SHORT };

    TEST_ASSERT_EQUAL_INT(0, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dst0_src8(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA, IEEE802154_FCF_SRC_ADDR_LONG };

    TEST_ASSERT_EQUAL_INT(13, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dst0_src8_pancomp(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA | IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_SRC_ADDR_LONG };

    TEST_ASSERT_EQUAL_INT(0, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dst2_src2(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA,
                            IEEE802154_FCF_DST_ADDR_SHORT | IEEE802154_FCF_SRC_ADDR_SHORT };

    TEST_ASSERT_EQUAL_INT(11, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dst8_src2(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA,
                            IEEE802154_FCF_DST_ADDR_LONG | IEEE802154_FCF_SRC_ADDR_SHORT };

    TEST_ASSERT_EQUAL_INT(17, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dst8_src8(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA,
                            IEEE802154_FCF_DST_ADDR_LONG | IEEE802154_FCF_SRC_ADDR_LONG };

    TEST_ASSERT_EQUAL_INT(23, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dst2_src2_pancomp(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA | IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_SHORT | IEEE802154_FCF_SRC_ADDR_SHORT };

    TEST_ASSERT_EQUAL_INT(9, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_frame_hdr_len_dst8_src8_pancomp(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_TYPE_DATA | IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_LONG | IEEE802154_FCF_SRC_ADDR_LONG };

    TEST_ASSERT_EQUAL_INT(21, ieee802154_get_frame_hdr_len(mhr));
}

static void test_ieee802154_get_src_dstr(void)
{
    const uint8_t mhr[] = { 0x00, IEEE802154_FCF_DST_ADDR_RESV };
    uint8_t res_addr;
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ieee802154_get_src(mhr, &res_addr, &res_pan));
}

static void test_ieee802154_get_src_srcr(void)
{
    const uint8_t mhr[] = { 0x00, IEEE802154_FCF_SRC_ADDR_RESV };
    uint8_t res_addr;
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ieee802154_get_src(mhr, &res_addr, &res_pan));
}

static void test_ieee802154_get_src_dst0_pancomp(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_VOID };
    uint8_t res_addr;
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ieee802154_get_src(mhr, &res_addr, &res_pan));
}

static void test_ieee802154_get_src_dst0_src0(void)
{
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_VOID |
                            IEEE802154_FCF_SRC_ADDR_VOID,
                            TEST_UINT8 };
    uint8_t res_addr;
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(0,
                          ieee802154_get_src(mhr, &res_addr, &res_pan));
}

static void test_ieee802154_get_src_dst0_src2(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_VOID |
                            IEEE802154_FCF_SRC_ADDR_SHORT,
                            TEST_UINT8,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_src(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_src_dst0_src8(void)
{
    const network_uint64_t exp_addr = byteorder_htonll(TEST_UINT64);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16);
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_VOID |
                            IEEE802154_FCF_SRC_ADDR_LONG,
                            TEST_UINT8,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[7], exp_addr.u8[6],
                            exp_addr.u8[5], exp_addr.u8[4],
                            exp_addr.u8[3], exp_addr.u8[2],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_src(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_src_dst2_src0(void)
{
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_SHORT |
                            IEEE802154_FCF_SRC_ADDR_VOID,
                            TEST_UINT8,
                            TEST_UINT8 + 1, TEST_UINT8 + 2,
                            TEST_UINT8 + 3, TEST_UINT8 + 4 };
    uint8_t res_addr;
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(0,
                          ieee802154_get_src(mhr, &res_addr, &res_pan));
}

static void test_ieee802154_get_src_dst2_src0_pancomp(void)
{
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_SHORT |
                            IEEE802154_FCF_SRC_ADDR_VOID,
                            TEST_UINT8,
                            /* source PAN is dest. PAN due to compression */
                            exp_pan.u8[0], exp_pan.u8[1] };
    uint8_t res_addr;
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(0,
                          ieee802154_get_src(mhr, &res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_src_dst2_src2(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_SHORT |
                            IEEE802154_FCF_SRC_ADDR_SHORT,
                            TEST_UINT8,
                            TEST_UINT8 + 1, TEST_UINT8 + 2,
                            TEST_UINT8 + 3, TEST_UINT8 + 4,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_src(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_src_dst2_src2_pancomp(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_SHORT |
                            IEEE802154_FCF_SRC_ADDR_SHORT,
                            TEST_UINT8,
                            /* source PAN is dest. PAN due to compression */
                            exp_pan.u8[0], exp_pan.u8[1],
                            TEST_UINT8 + 1, TEST_UINT8 + 2,
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_src(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_src_dst2_src8(void)
{
    const network_uint64_t exp_addr = byteorder_htonll(TEST_UINT64);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16);
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_SHORT |
                            IEEE802154_FCF_SRC_ADDR_LONG,
                            TEST_UINT8,
                            TEST_UINT8 + 1, TEST_UINT8 + 2,
                            TEST_UINT8 + 3, TEST_UINT8 + 4,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[7], exp_addr.u8[6],
                            exp_addr.u8[5], exp_addr.u8[4],
                            exp_addr.u8[3], exp_addr.u8[2],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_src(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_src_dst2_src8_pancomp(void)
{
    const network_uint64_t exp_addr = byteorder_htonll(TEST_UINT64);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16);
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_SHORT |
                            IEEE802154_FCF_SRC_ADDR_LONG,
                            TEST_UINT8,
                            /* source PAN is dest. PAN due to compression */
                            exp_pan.u8[0], exp_pan.u8[1],
                            TEST_UINT8 + 1, TEST_UINT8 + 2,
                            exp_addr.u8[7], exp_addr.u8[6],
                            exp_addr.u8[5], exp_addr.u8[4],
                            exp_addr.u8[3], exp_addr.u8[2],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_src(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_src_dst8_src0(void)
{
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_LONG |
                            IEEE802154_FCF_SRC_ADDR_VOID,
                            TEST_UINT8,
                            TEST_UINT8 + 1, TEST_UINT8 + 2,
                            TEST_UINT8 + 3, TEST_UINT8 + 4,
                            TEST_UINT8 + 5, TEST_UINT8 + 6,
                            TEST_UINT8 + 7, TEST_UINT8 + 8,
                            TEST_UINT8 + 9, TEST_UINT8 + 10 };
    uint8_t res_addr;
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(0,
                          ieee802154_get_src(mhr, &res_addr, &res_pan));
}

static void test_ieee802154_get_src_dst8_src0_pancomp(void)
{
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_LONG |
                            IEEE802154_FCF_SRC_ADDR_VOID,
                            TEST_UINT8,
                            /* source PAN is dest. PAN due to compression */
                            exp_pan.u8[0], exp_pan.u8[1] };
    uint8_t res_addr;
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(0,
                          ieee802154_get_src(mhr, &res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_src_dst8_src2(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16);
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_LONG |
                            IEEE802154_FCF_SRC_ADDR_SHORT,
                            TEST_UINT8,
                            TEST_UINT8 + 1, TEST_UINT8 + 2,
                            TEST_UINT8 + 3, TEST_UINT8 + 4,
                            TEST_UINT8 + 5, TEST_UINT8 + 6,
                            TEST_UINT8 + 7, TEST_UINT8 + 8,
                            TEST_UINT8 + 9, TEST_UINT8 + 10,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_src(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_src_dst8_src2_pancomp(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16);
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_LONG |
                            IEEE802154_FCF_SRC_ADDR_SHORT,
                            TEST_UINT8,
                            /* source PAN is dest. PAN due to compression */
                            exp_pan.u8[0], exp_pan.u8[1],
                            TEST_UINT8 + 1, TEST_UINT8 + 2,
                            TEST_UINT8 + 3, TEST_UINT8 + 4,
                            TEST_UINT8 + 5, TEST_UINT8 + 6,
                            TEST_UINT8 + 7, TEST_UINT8 + 8,
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_src(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_src_dst8_src8(void)
{
    const network_uint64_t exp_addr = byteorder_htonll(TEST_UINT64);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16);
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_LONG |
                            IEEE802154_FCF_SRC_ADDR_LONG,
                            TEST_UINT8,
                            TEST_UINT8 + 1, TEST_UINT8 + 2,
                            TEST_UINT8 + 3, TEST_UINT8 + 4,
                            TEST_UINT8 + 5, TEST_UINT8 + 6,
                            TEST_UINT8 + 7, TEST_UINT8 + 8,
                            TEST_UINT8 + 9, TEST_UINT8 + 10,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[7], exp_addr.u8[6],
                            exp_addr.u8[5], exp_addr.u8[4],
                            exp_addr.u8[3], exp_addr.u8[2],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_src(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_src_dst8_src8_pancomp(void)
{
    const network_uint64_t exp_addr = byteorder_htonll(TEST_UINT64);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16);
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_LONG |
                            IEEE802154_FCF_SRC_ADDR_LONG,
                            TEST_UINT8,
                            /* source PAN is dest. PAN due to compression */
                            exp_pan.u8[0], exp_pan.u8[1],
                            TEST_UINT8 + 1, TEST_UINT8 + 2,
                            TEST_UINT8 + 3, TEST_UINT8 + 4,
                            TEST_UINT8 + 5, TEST_UINT8 + 6,
                            TEST_UINT8 + 7, TEST_UINT8 + 8,
                            exp_addr.u8[7], exp_addr.u8[6],
                            exp_addr.u8[5], exp_addr.u8[4],
                            exp_addr.u8[3], exp_addr.u8[2],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_src(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_dst_dstr(void)
{
    const uint8_t mhr[] = { 0x00, IEEE802154_FCF_DST_ADDR_RESV,
                            TEST_UINT8 };
    uint8_t res_addr;
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ieee802154_get_src(mhr, &res_addr, &res_pan));
}

static void test_ieee802154_get_dst_dst0(void)
{
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_VOID,
                            TEST_UINT8 };
    uint8_t res_addr;
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(0,
                          ieee802154_get_dst(mhr, &res_addr, &res_pan));
}

static void test_ieee802154_get_dst_dst0_pancomp(void)
{
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_VOID,
                            TEST_UINT8 };
    uint8_t res_addr;
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ieee802154_get_src(mhr, &res_addr, &res_pan));
}

static void test_ieee802154_get_dst_dst2(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_SHORT,
                            TEST_UINT8,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_dst(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_dst_dst2_pancomp(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_SHORT,
                            TEST_UINT8,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_dst(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_dst_dst8(void)
{
    const network_uint64_t exp_addr = byteorder_htonll(TEST_UINT64);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16);
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_LONG,
                            TEST_UINT8,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[7], exp_addr.u8[6],
                            exp_addr.u8[5], exp_addr.u8[4],
                            exp_addr.u8[3], exp_addr.u8[2],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_dst(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_get_dst_dst8_pancomp(void)
{
    const network_uint64_t exp_addr = byteorder_htonll(TEST_UINT64);
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16);
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_LONG,
                            TEST_UINT8,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[7], exp_addr.u8[6],
                            exp_addr.u8[5], exp_addr.u8[4],
                            exp_addr.u8[3], exp_addr.u8[2],
                            exp_addr.u8[1], exp_addr.u8[0] };
    uint8_t res_addr[sizeof(exp_addr)];
    le_uint16_t res_pan;

    TEST_ASSERT_EQUAL_INT(sizeof(exp_addr),
                          ieee802154_get_dst(mhr, res_addr, &res_pan));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_addr.u8, res_addr, sizeof(exp_addr)));
    TEST_ASSERT_EQUAL_INT(exp_pan.u16, res_pan.u16);
}

static void test_ieee802154_dst_filter_pan_fail(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const eui64_t long_addr = {.uint64.u64 = TEST_UINT64};
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_SHORT,
                            TEST_UINT8,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[1], exp_addr.u8[0] };
    TEST_ASSERT_EQUAL_INT(1, ieee802154_dst_filter(mhr,
                                                   exp_pan.u16 + 1,
                                                   exp_addr,
                                                   &long_addr));
}

static void test_ieee802154_dst_filter_short_addr_fail(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const eui64_t long_addr = {.uint64.u64 = TEST_UINT64};
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_SHORT,
                            TEST_UINT8,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[1], exp_addr.u8[0] };
    TEST_ASSERT_EQUAL_INT(1, ieee802154_dst_filter(mhr,
                                                   exp_pan.u16,
                                                   byteorder_htons(exp_addr.u16 + 1),
                                                   &long_addr));
}

static void test_ieee802154_dst_filter_long_addr_fail(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const eui64_t long_addr = {.uint64.u64 = TEST_UINT64};
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const eui64_t long_addr_fail =  {.uint64.u64 = TEST_UINT64 + 1};
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_LONG,
                            TEST_UINT8,
                            exp_pan.u8[0], exp_pan.u8[1],
                            long_addr.uint8[7], long_addr.uint8[6],
                            long_addr.uint8[5], long_addr.uint8[4],
                            long_addr.uint8[3], long_addr.uint8[2],
                            long_addr.uint8[1], long_addr.uint8[0] };
    TEST_ASSERT_EQUAL_INT(1, ieee802154_dst_filter(mhr,
                                                   exp_pan.u16,
                                                   exp_addr,
                                                   &long_addr_fail));
}

static void test_ieee802154_dst_filter_pan_short(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const eui64_t long_addr = {.uint64.u64 = TEST_UINT64};
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_SHORT,
                            TEST_UINT8,
                            exp_pan.u8[0], exp_pan.u8[1],
                            exp_addr.u8[1], exp_addr.u8[0] };
    TEST_ASSERT_EQUAL_INT(0, ieee802154_dst_filter(mhr,
                                                   exp_pan.u16,
                                                   exp_addr,
                                                   &long_addr));
}

static void test_ieee802154_dst_filter_bcast_short(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const eui64_t long_addr = {.uint64.u64 = TEST_UINT64};
    const uint8_t pan_bcast[] = IEEE802154_PANID_BCAST;
    const uint8_t mhr[] = { 0,
                            IEEE802154_FCF_DST_ADDR_SHORT,
                            TEST_UINT8,
                            pan_bcast[0], pan_bcast[1],
                            exp_addr.u8[1], exp_addr.u8[0] };
    TEST_ASSERT_EQUAL_INT(0, ieee802154_dst_filter(mhr,
                                                   TEST_UINT16,
                                                   exp_addr,
                                                   &long_addr));
}

static void test_ieee802154_dst_filter_pan_long(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const eui64_t long_addr = {.uint64.u64 = TEST_UINT64};
    const le_uint16_t exp_pan = byteorder_htols(TEST_UINT16 + 1);
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_LONG,
                            TEST_UINT8,
                            exp_pan.u8[0], exp_pan.u8[1],
                            long_addr.uint8[7], long_addr.uint8[6],
                            long_addr.uint8[5], long_addr.uint8[4],
                            long_addr.uint8[3], long_addr.uint8[2],
                            long_addr.uint8[1], long_addr.uint8[0] };
    TEST_ASSERT_EQUAL_INT(0, ieee802154_dst_filter(mhr,
                                                   exp_pan.u16,
                                                   exp_addr,
                                                   &long_addr));
}

static void test_ieee802154_dst_filter_bcast_long(void)
{
    const network_uint16_t exp_addr = byteorder_htons(TEST_UINT16);
    const eui64_t long_addr = {.uint64.u64 = TEST_UINT64};
    const uint8_t pan_bcast[] = IEEE802154_PANID_BCAST;
    const uint8_t mhr[] = { IEEE802154_FCF_PAN_COMP,
                            IEEE802154_FCF_DST_ADDR_LONG,
                            TEST_UINT8,
                            pan_bcast[0], pan_bcast[1],
                            long_addr.uint8[7], long_addr.uint8[6],
                            long_addr.uint8[5], long_addr.uint8[4],
                            long_addr.uint8[3], long_addr.uint8[2],
                            long_addr.uint8[1], long_addr.uint8[0] };
    TEST_ASSERT_EQUAL_INT(0, ieee802154_dst_filter(mhr,
                                                   TEST_UINT16,
                                                   exp_addr,
                                                   &long_addr));
}

static void test_ieee802154_get_seq(void)
{
    const uint8_t mhr[] = { 0x00, 0x00, TEST_UINT8 };

    TEST_ASSERT_EQUAL_INT(TEST_UINT8, ieee802154_get_seq(mhr));
}

static void test_ieee802154_get_iid_addr_len_0(void)
{
    const uint8_t addr[] = { 0x01, 0x23 };
    eui64_t iid;

    TEST_ASSERT_NULL(ieee802154_get_iid(&iid, addr, 0));
}

static void test_ieee802154_get_iid_addr_len_SIZE_MAX(void)
{
    const uint8_t addr[] = { 0x01, 0x23 };
    eui64_t iid;

    TEST_ASSERT_NULL(ieee802154_get_iid(&iid, addr, SIZE_MAX));
}

static void test_ieee802154_get_iid_addr_len_2(void)
{
    const uint8_t addr[] = { 0x01, 0x23 };
    const uint8_t exp[] = { 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x01, 0x23 };
    eui64_t iid;

    TEST_ASSERT_NOT_NULL(ieee802154_get_iid(&iid, addr, sizeof(addr)));
    TEST_ASSERT_EQUAL_INT(0, memcmp((const char *)exp, (char *) &iid, sizeof(iid)));
}

static void test_ieee802154_get_iid_addr_len_4(void)
{
    const uint8_t addr[] = { 0x01, 0x23, 0x45, 0x67 };
    const uint8_t exp[] = { 0x03, 0x23, 0x00, 0xff, 0xfe, 0x00, 0x45, 0x67 };
    eui64_t iid;

    TEST_ASSERT_NOT_NULL(ieee802154_get_iid(&iid, addr, sizeof(addr)));
    TEST_ASSERT_EQUAL_INT(0, memcmp((const char *)exp, (char *) &iid, sizeof(iid)));
}

static void test_ieee802154_get_iid_addr_len_8(void)
{
    const uint8_t addr[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
    const uint8_t exp[] = { 0x03, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
    eui64_t iid;

    TEST_ASSERT_NOT_NULL(ieee802154_get_iid(&iid, addr, sizeof(addr)));
    TEST_ASSERT_EQUAL_INT(0, memcmp((const char *)exp, (char *) &iid, sizeof(iid)));
}

static void test_ieee802154_dbm_to_rssi(void)
{
    /* RF Power below -174 is represented with RSSI zero.
     * RF power above 80 is represented with RSSI 254 */
    const int16_t dbm[] = {0, -73, -180, 85};
    const uint8_t expected[] = {174, 101, 0, 254};

    TEST_ASSERT_EQUAL_INT(expected[0], ieee802154_dbm_to_rssi(dbm[0]));
    TEST_ASSERT_EQUAL_INT(expected[1], ieee802154_dbm_to_rssi(dbm[1]));
    TEST_ASSERT_EQUAL_INT(expected[2], ieee802154_dbm_to_rssi(dbm[2]));
    TEST_ASSERT_EQUAL_INT(expected[3], ieee802154_dbm_to_rssi(dbm[3]));

}

static void test_ieee802154_rssi_to_dbm(void)
{
    const uint8_t rssi[] = {174, 101, 0, 254};
    const int16_t expected[]= {0, -73, -174, 80};

    TEST_ASSERT_EQUAL_INT(expected[0], ieee802154_rssi_to_dbm(rssi[0]));
    TEST_ASSERT_EQUAL_INT(expected[1], ieee802154_rssi_to_dbm(rssi[1]));
    TEST_ASSERT_EQUAL_INT(expected[2], ieee802154_rssi_to_dbm(rssi[2]));
    TEST_ASSERT_EQUAL_INT(expected[3], ieee802154_rssi_to_dbm(rssi[3]));
}

Test *tests_ieee802154_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ieee802154_set_frame_hdr_flags0),
        new_TestFixture(test_ieee802154_set_frame_hdr_flags0_non_beacon_non_ack),
        new_TestFixture(test_ieee802154_set_frame_hdr_bcast_src0),
        new_TestFixture(test_ieee802154_set_frame_hdr_bcast_src2),
        new_TestFixture(test_ieee802154_set_frame_hdr_bcast_src8),
        new_TestFixture(test_ieee802154_set_frame_hdr_dst2_src0),
        new_TestFixture(test_ieee802154_set_frame_hdr_dst2_src2),
        new_TestFixture(test_ieee802154_set_frame_hdr_dst2_src2_pancomp),
        new_TestFixture(test_ieee802154_set_frame_hdr_dst2_src8),
        new_TestFixture(test_ieee802154_set_frame_hdr_dst2_src8_pancomp),
        new_TestFixture(test_ieee802154_set_frame_hdr_dst8_src0),
        new_TestFixture(test_ieee802154_set_frame_hdr_dst8_src2),
        new_TestFixture(test_ieee802154_set_frame_hdr_dst8_src2_pancomp),
        new_TestFixture(test_ieee802154_set_frame_hdr_dst8_src8),
        new_TestFixture(test_ieee802154_set_frame_hdr_dst8_src8_pancomp),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst0_src0),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dstr),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_srcr),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst2_src0),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst8_src0),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst0_src2),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst0_src2_pancomp),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst0_src8),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst0_src8_pancomp),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst2_src2),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst8_src2),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst8_src8),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst2_src2_pancomp),
        new_TestFixture(test_ieee802154_get_frame_hdr_len_dst8_src8_pancomp),
        new_TestFixture(test_ieee802154_get_src_dstr),
        new_TestFixture(test_ieee802154_get_src_srcr),
        new_TestFixture(test_ieee802154_get_src_dst0_pancomp),
        new_TestFixture(test_ieee802154_get_src_dst0_src0),
        new_TestFixture(test_ieee802154_get_src_dst0_src2),
        new_TestFixture(test_ieee802154_get_src_dst0_src8),
        new_TestFixture(test_ieee802154_get_src_dst2_src0),
        new_TestFixture(test_ieee802154_get_src_dst2_src0_pancomp),
        new_TestFixture(test_ieee802154_get_src_dst2_src2),
        new_TestFixture(test_ieee802154_get_src_dst2_src2_pancomp),
        new_TestFixture(test_ieee802154_get_src_dst2_src8),
        new_TestFixture(test_ieee802154_get_src_dst2_src8_pancomp),
        new_TestFixture(test_ieee802154_get_src_dst8_src0),
        new_TestFixture(test_ieee802154_get_src_dst8_src0_pancomp),
        new_TestFixture(test_ieee802154_get_src_dst8_src2),
        new_TestFixture(test_ieee802154_get_src_dst8_src2_pancomp),
        new_TestFixture(test_ieee802154_get_src_dst8_src8),
        new_TestFixture(test_ieee802154_get_src_dst8_src8_pancomp),
        new_TestFixture(test_ieee802154_get_dst_dstr),
        new_TestFixture(test_ieee802154_get_dst_dst0),
        new_TestFixture(test_ieee802154_get_dst_dst0_pancomp),
        new_TestFixture(test_ieee802154_get_dst_dst2),
        new_TestFixture(test_ieee802154_get_dst_dst2_pancomp),
        new_TestFixture(test_ieee802154_get_dst_dst8),
        new_TestFixture(test_ieee802154_get_dst_dst8_pancomp),
        new_TestFixture(test_ieee802154_dst_filter_pan_fail),
        new_TestFixture(test_ieee802154_dst_filter_short_addr_fail),
        new_TestFixture(test_ieee802154_dst_filter_long_addr_fail),
        new_TestFixture(test_ieee802154_dst_filter_pan_short),
        new_TestFixture(test_ieee802154_dst_filter_bcast_short),
        new_TestFixture(test_ieee802154_dst_filter_pan_long),
        new_TestFixture(test_ieee802154_dst_filter_bcast_long),
        new_TestFixture(test_ieee802154_get_seq),
        new_TestFixture(test_ieee802154_get_iid_addr_len_0),
        new_TestFixture(test_ieee802154_get_iid_addr_len_SIZE_MAX),
        new_TestFixture(test_ieee802154_get_iid_addr_len_2),
        new_TestFixture(test_ieee802154_get_iid_addr_len_4),
        new_TestFixture(test_ieee802154_get_iid_addr_len_8),
        new_TestFixture(test_ieee802154_rssi_to_dbm),
        new_TestFixture(test_ieee802154_dbm_to_rssi),
    };

    EMB_UNIT_TESTCALLER(ieee802154_tests, NULL, NULL, fixtures);

    return (Test *)&ieee802154_tests;
}

void tests_ieee802154(void)
{
    TESTS_RUN(tests_ieee802154_tests());
}
/** @} */
