/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests extension header handling of gnrc stack.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "embUnit.h"
#include "shell.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/ext.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/rpl/srh.h"
#include "net/gnrc/ipv6/ext/rh.h"

#define IPV6_DST            {{ 0x20, 0x01, 0xab, 0xcd, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x01 }}
#define IPV6_ADDR1          {{ 0x20, 0x01, 0xab, 0xcd, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x02 }}
#define IPV6_ADDR2          {{ 0x20, 0x01, 0xab, 0xcd, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x03 }}
#define IPV6_MCAST_ADDR     {{ 0xff, 0x05, 0xab, 0xcd, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x00, \
                               0x00, 0x00, 0x00, 0x03 }}

#define IPV6_ADDR1_ELIDED   { 0x00, 0x00, 0x02 }
#define IPV6_ADDR2_ELIDED   { 0x00, 0x00, 0x03 }
#define IPV6_ELIDED_PREFIX  (13)

#define SRH_SEG_LEFT        (2)
#define MAX_BUF_SIZE        ((sizeof(gnrc_rpl_srh_t) + 2) + sizeof(ipv6_addr_t))

static ipv6_hdr_t hdr;
static uint8_t buf[MAX_BUF_SIZE];
static char line_buf[SHELL_DEFAULT_BUFSIZE];
static gnrc_netreg_entry_t ip_entry = GNRC_NETREG_ENTRY_INIT_PID(
        0, KERNEL_PID_UNDEF
    );

static void set_up_tests(void)
{
    memset(&hdr, 0, sizeof(hdr));
    memset(buf, 0, sizeof(buf));
}

static inline void _init_hdrs(gnrc_rpl_srh_t **srh, uint8_t **vec,
                              const ipv6_addr_t *dst)
{
    *srh = (gnrc_rpl_srh_t *)buf;
    *vec = (uint8_t *)(*srh + 1);
    memcpy(&hdr.dst, dst, sizeof(hdr.dst));
}

static void test_rpl_srh_dst_multicast(void)
{
    static const ipv6_addr_t a1 = IPV6_ADDR1, a2 = IPV6_ADDR2;
    static const ipv6_addr_t mcast = IPV6_MCAST_ADDR;
    gnrc_rpl_srh_t *srh;
    uint8_t *vec;
    void *err_ptr = NULL;
    int res;

    _init_hdrs(&srh, &vec, &mcast);
    srh->len = (2 * sizeof(ipv6_addr_t)) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    memcpy(vec, &a1, sizeof(a1));
    memcpy(vec + sizeof(a1), &a2, sizeof(a2));

    res = gnrc_rpl_srh_process(&hdr, srh, &err_ptr);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_ERROR);
    TEST_ASSERT_NULL(err_ptr);
}

static void test_rpl_srh_route_multicast(void)
{
    static const ipv6_addr_t a1 = IPV6_ADDR1;
    static const ipv6_addr_t mcast = IPV6_MCAST_ADDR;
    static const ipv6_addr_t dst = IPV6_DST;
    gnrc_rpl_srh_t *srh;
    uint8_t *vec;
    void *err_ptr = NULL;
    int res;

    _init_hdrs(&srh, &vec, &dst);
    srh->len = (2 * sizeof(ipv6_addr_t)) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    memcpy(vec, &mcast, sizeof(mcast));
    memcpy(vec + sizeof(mcast), &a1, sizeof(a1));

    res = gnrc_rpl_srh_process(&hdr, srh, &err_ptr);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_ERROR);
    TEST_ASSERT_NULL(err_ptr);
}

static void test_rpl_srh_inconsistent_hdr(void)
{
    static const ipv6_addr_t dst = IPV6_DST;
    gnrc_rpl_srh_t srh;
    void *err_ptr;
    int res;

    memset(&srh, 0, sizeof(srh));
    memcpy(&hdr.dst, &dst, sizeof(hdr.dst));
    srh.nh = 128U;
    srh.len = 0U;
    srh.type = 3U;
    srh.seg_left = 220U;
    srh.compr = 0xc0;
    srh.pad_resv = 0xf0;

    res = gnrc_rpl_srh_process(&hdr, &srh, &err_ptr);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_ERROR);
    TEST_ASSERT((&srh.len) == err_ptr);
}

static void test_rpl_srh_too_many_seg_left(void)
{
    static const ipv6_addr_t a1 = IPV6_ADDR1;
    static const ipv6_addr_t dst = IPV6_DST;
    gnrc_rpl_srh_t *srh;
    uint8_t *vec;
    void *err_ptr;
    int res;

    _init_hdrs(&srh, &vec, &dst);
    srh->len = sizeof(ipv6_addr_t) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    memcpy(vec, &a1, sizeof(a1));

    res = gnrc_rpl_srh_process(&hdr, srh, &err_ptr);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_ERROR);
    TEST_ASSERT((&srh->seg_left) == err_ptr);
}

static void test_rpl_srh_nexthop_no_prefix_elided(void)
{
    static const ipv6_addr_t a1 = IPV6_ADDR1, a2 = IPV6_ADDR2, dst = IPV6_DST;
    static const ipv6_addr_t expected1 = IPV6_ADDR1, expected2 = IPV6_ADDR2;
    gnrc_rpl_srh_t *srh;
    uint8_t *vec;
    void *err_ptr;
    int res;

    _init_hdrs(&srh, &vec, &dst);
    srh->len = (2 * sizeof(ipv6_addr_t)) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    memcpy(vec, &a1, sizeof(a1));
    memcpy(vec + sizeof(a1), &a2, sizeof(a2));

    /* first hop */
    res = gnrc_rpl_srh_process(&hdr, srh, &err_ptr);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_FORWARDED);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 1, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected1));

    /* second hop */
    res = gnrc_rpl_srh_process(&hdr, srh, &err_ptr);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_FORWARDED);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 2, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected2));
}

static void test_rpl_srh_nexthop_prefix_elided(void)
{
    static const ipv6_addr_t dst = IPV6_DST;
    static const ipv6_addr_t expected1 = IPV6_ADDR1, expected2 = IPV6_ADDR2;
    gnrc_rpl_srh_t *srh;
    uint8_t *vec;
    void *err_ptr;
    int res;
    static const uint8_t a1[3] = IPV6_ADDR1_ELIDED;
    static const uint8_t a2[3] = IPV6_ADDR2_ELIDED;

    _init_hdrs(&srh, &vec, &dst);
    srh->len = (sizeof(a1) + sizeof(a2) + 2) / 8;
    srh->seg_left = SRH_SEG_LEFT;
    srh->compr = (IPV6_ELIDED_PREFIX << 4) | IPV6_ELIDED_PREFIX;
    srh->pad_resv = 2 << 4;
    memcpy(vec, &a1, sizeof(a1));
    memcpy(vec + sizeof(a1), &a2, sizeof(a2));

    /* first hop */
    res = gnrc_rpl_srh_process(&hdr, srh, &err_ptr);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_FORWARDED);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 1, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected1));

    /* second hop */
    res = gnrc_rpl_srh_process(&hdr, srh, &err_ptr);
    TEST_ASSERT_EQUAL_INT(res, GNRC_IPV6_EXT_RH_FORWARDED);
    TEST_ASSERT_EQUAL_INT(SRH_SEG_LEFT - 2, srh->seg_left);
    TEST_ASSERT(ipv6_addr_equal(&hdr.dst, &expected2));
}

/* tools for external interaction */
static inline void _ipreg_usage(char *cmd)
{
    printf("Usage: %s {reg|unreg}", cmd);
}

static int _ipreg(int argc, char **argv)
{
    if (argc < 2) {
        _ipreg_usage(argv[0]);
        return 1;
    }
    else if (strcmp("reg", argv[1]) == 0) {
        if (ip_entry.target.pid != KERNEL_PID_UNDEF) {
            puts("Already registered to protocol number 59");
            return 1;
        }
        gnrc_netreg_entry_init_pid(&ip_entry, PROTNUM_IPV6_NONXT,
                                   gnrc_pktdump_pid);
        gnrc_netreg_register(GNRC_NETTYPE_IPV6, &ip_entry);
        puts("Registered to protocol number 59");
    }
    else if (strcmp("unreg", argv[1]) == 0) {
        puts("Unregistered from protocol number 59");
        gnrc_netreg_unregister(GNRC_NETTYPE_IPV6, &ip_entry);
        gnrc_netreg_entry_init_pid(&ip_entry, 0, KERNEL_PID_UNDEF);
    }
    else {
        _ipreg_usage(argv[0]);
        return 1;
    }

    return 0;
}

static void run_unittests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_rpl_srh_dst_multicast),
        new_TestFixture(test_rpl_srh_route_multicast),
        new_TestFixture(test_rpl_srh_inconsistent_hdr),
        new_TestFixture(test_rpl_srh_too_many_seg_left),
        new_TestFixture(test_rpl_srh_nexthop_no_prefix_elided),
        new_TestFixture(test_rpl_srh_nexthop_prefix_elided),
    };

    EMB_UNIT_TESTCALLER(rpl_srh_tests, set_up_tests, NULL, fixtures);
    TESTS_START();
    TESTS_RUN((Test *)&rpl_srh_tests);
    TESTS_END();
}

static int _unittests(int argc, char** argv)
{
    (void) argc;
    (void) argv;

    run_unittests();
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "ip", "Registers pktdump to protocol number 59 (no next header)", _ipreg },
    { "unittests", "Runs unitest", _unittests},
    { NULL, NULL, NULL }
};

int main(void)
{
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
