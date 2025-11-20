/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests NDP message handling of gnrc stack.
 *
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <kernel_defines.h>
#include <stdio.h>

#include "byteorder.h"
#include "embUnit.h"
#include "embUnit/embUnit.h"
#include "msg.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "net/icmpv6.h"
#include "net/ndp.h"
#include "net/netdev_test.h"
#include "net/netopt.h"
#include "sched.h"
#include "test_utils/expect.h"

#include "pktbuf_static.h"

#include "net/gnrc/ndp.h"

#define TEST_CUR_HL             (194U)
#define TEST_LTIME              (894U)
#define TEST_REACH_TIME         (1597495062U)
#define TEST_RETRANS_TIMER      (1819101160U)
#define TEST_PFX_LEN            (42U)
#define TEST_VALID_LTIME        (223526372U)
#define TEST_PREF_LTIME         (2056660713U)
#define TEST_NDP_OPT_SIZE       (14U)
#define TEST_NDP_OPT_TYPE       (142U)
#define TEST_MTU                (669256124U)

#define TEST_ASSERT_ALLOCATION(pkt, s, t) \
    TEST_ASSERT_NULL((pkt)->next); \
    TEST_ASSERT_NOT_NULL((pkt)->data); \
    TEST_ASSERT_EQUAL_INT((s), (pkt)->size); \
    TEST_ASSERT_EQUAL_INT((t), (pkt)->type)

static const ipv6_addr_t test_dst = { { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x73, 0x25, 0x22, 0xc6, 0xdf, 0x05, 0xf2, 0x6b } };
static const ipv6_addr_t test_src = { { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0xe5, 0x43, 0xb7, 0x74, 0xd7, 0xa9, 0x30, 0x74 } };
static const ipv6_addr_t test_tgt = { { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x31, 0x78, 0x95, 0x84, 0x71, 0x5f, 0x47, 0x53 } };
static const ipv6_addr_t test_pfx = { { 0x47, 0x25, 0xd9, 0x3b, 0x7f, 0xcc, 0x15, 0x6c,
                                        0x64, 0x3e, 0x76, 0x0d, 0x30, 0x10, 0x0d, 0xc8 } };
static const uint8_t test_src_l2[] = { 0xe7, 0x43, 0xb7, 0x74, 0xd7, 0xa9, 0x30, 0x74 };

static gnrc_netif_t *test_netif = NULL;
static gnrc_netif_t _netif;

static void init_pkt_handler(void);
static inline size_t ceil8(size_t size);

static void set_up(void)
{
    gnrc_pktbuf_init();
}

static void fill_pktbuf(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          _align(sizeof(gnrc_pktsnip_t)),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
}

static void test_nbr_sol_build__pktbuf_full(void)
{
    fill_pktbuf();
    TEST_ASSERT_NULL(gnrc_ndp_nbr_sol_build(&test_tgt, NULL));
}

static void test_nbr_sol_build__success(void)
{
    gnrc_pktsnip_t *pkt;
    ndp_nbr_sol_t *nbr_sol;

    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ndp_nbr_sol_build(&test_tgt, NULL)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    /* check packet meta-data */
    TEST_ASSERT_ALLOCATION(pkt, sizeof(ndp_nbr_sol_t), GNRC_NETTYPE_ICMPV6);
    /* check packet content */
    nbr_sol = pkt->data;
    TEST_ASSERT_EQUAL_INT(ICMPV6_NBR_SOL, nbr_sol->type);
    TEST_ASSERT_EQUAL_INT(0, nbr_sol->code);
    TEST_ASSERT_EQUAL_INT(0, nbr_sol->resv.u32);
    TEST_ASSERT_MESSAGE(ipv6_addr_equal(&test_tgt, &nbr_sol->tgt),
                        "nbr_sol->tgt != test_tgt");
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_nbr_adv_build__pktbuf_full(void)
{
    fill_pktbuf();
    TEST_ASSERT_NULL(gnrc_ndp_nbr_adv_build(&test_tgt, 0, NULL));
}

static void test_nbr_adv_build__success(uint8_t flags)
{
    gnrc_pktsnip_t *pkt;
    ndp_nbr_adv_t *nbr_adv;

    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ndp_nbr_adv_build(&test_tgt, flags,
                                                       NULL)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    /* check packet meta-data */
    TEST_ASSERT_ALLOCATION(pkt, sizeof(ndp_nbr_adv_t), GNRC_NETTYPE_ICMPV6);
    /* check packet content */
    nbr_adv = pkt->data;
    TEST_ASSERT_EQUAL_INT(ICMPV6_NBR_ADV, nbr_adv->type);
    TEST_ASSERT_EQUAL_INT(0, nbr_adv->code);
    TEST_ASSERT_EQUAL_INT(flags, nbr_adv->flags);
    TEST_ASSERT_EQUAL_INT(0, nbr_adv->resv[0]);
    TEST_ASSERT_EQUAL_INT(0, nbr_adv->resv[1]);
    TEST_ASSERT_EQUAL_INT(0, nbr_adv->resv[2]);
    TEST_ASSERT_MESSAGE(ipv6_addr_equal(&test_tgt, &nbr_adv->tgt),
                        "nbr_adv->tgt != test_tgt");
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_nbr_adv_build__success_without_flags(void)
{
    test_nbr_adv_build__success(0);
}

static void test_nbr_adv_build__success_with_flags(void)
{
    test_nbr_adv_build__success(NDP_NBR_ADV_FLAGS_S);
}

static void test_rtr_sol_build__pktbuf_full(void)
{
    fill_pktbuf();
    TEST_ASSERT_NULL(gnrc_ndp_rtr_sol_build(NULL));
}

static void test_rtr_sol_build__success(void)
{
    gnrc_pktsnip_t *pkt;
    ndp_rtr_sol_t *rtr_sol;

    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ndp_rtr_sol_build(NULL)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    /* check packet meta-data */
    TEST_ASSERT_ALLOCATION(pkt, sizeof(ndp_rtr_sol_t), GNRC_NETTYPE_ICMPV6);
    /* check packet content */
    rtr_sol = pkt->data;
    TEST_ASSERT_EQUAL_INT(ICMPV6_RTR_SOL, rtr_sol->type);
    TEST_ASSERT_EQUAL_INT(0, rtr_sol->code);
    TEST_ASSERT_EQUAL_INT(0, rtr_sol->resv.u32);
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_rtr_adv_build__pktbuf_full(void)
{
    fill_pktbuf();
    TEST_ASSERT_NULL(gnrc_ndp_rtr_adv_build(TEST_CUR_HL, 0, TEST_LTIME,
                                            TEST_REACH_TIME,
                                            TEST_RETRANS_TIMER, NULL));
}

static void test_rtr_adv_build__success(uint8_t flags)
{
    gnrc_pktsnip_t *pkt;
    ndp_rtr_adv_t *rtr_adv;

    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ndp_rtr_adv_build(TEST_CUR_HL, flags,
                                                       TEST_LTIME,
                                                       TEST_REACH_TIME,
                                                       TEST_RETRANS_TIMER,
                                                       NULL)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    /* check packet meta-data */
    TEST_ASSERT_ALLOCATION(pkt, sizeof(ndp_rtr_adv_t), GNRC_NETTYPE_ICMPV6);
    /* check packet content */
    rtr_adv = pkt->data;
    TEST_ASSERT_EQUAL_INT(ICMPV6_RTR_ADV, rtr_adv->type);
    TEST_ASSERT_EQUAL_INT(0, rtr_adv->code);
    TEST_ASSERT_EQUAL_INT(TEST_CUR_HL, rtr_adv->cur_hl);
    TEST_ASSERT_EQUAL_INT(flags, rtr_adv->flags);
    TEST_ASSERT_EQUAL_INT(TEST_LTIME, byteorder_ntohs(rtr_adv->ltime));
    TEST_ASSERT_EQUAL_INT(TEST_REACH_TIME,
                          byteorder_ntohl(rtr_adv->reach_time));
    TEST_ASSERT_EQUAL_INT(TEST_RETRANS_TIMER,
                          byteorder_ntohl(rtr_adv->retrans_timer));
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_rtr_adv_build__success_without_flags(void)
{
    test_rtr_adv_build__success(0);
}

static void test_rtr_adv_build__success_with_flags(void)
{
    test_rtr_adv_build__success(NDP_RTR_ADV_FLAGS_M);
}

static void test_opt_build__pktbuf_full(void)
{
    fill_pktbuf();
    TEST_ASSERT_NULL(gnrc_ndp_opt_build(TEST_NDP_OPT_TYPE, TEST_NDP_OPT_SIZE,
                                        NULL));
}

static void test_opt_build__success(void)
{
    gnrc_pktsnip_t *pkt;
    ndp_opt_t *opt;

    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ndp_opt_build(TEST_NDP_OPT_TYPE,
                                                   TEST_NDP_OPT_SIZE,
                                                   NULL)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    /* check packet meta-data */
    TEST_ASSERT_ALLOCATION(pkt, ceil8(TEST_NDP_OPT_SIZE), GNRC_NETTYPE_UNDEF);
    /* check packet content */
    opt = pkt->data;
    TEST_ASSERT_EQUAL_INT(TEST_NDP_OPT_TYPE, opt->type);
    TEST_ASSERT_EQUAL_INT(ceil8(TEST_NDP_OPT_SIZE) / 8, opt->len);
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_opt_sl2a_build__pktbuf_full(void)
{
    fill_pktbuf();
    TEST_ASSERT_NULL(gnrc_ndp_opt_sl2a_build(test_src_l2, sizeof(test_src_l2),
                                             NULL));
}

static void test_opt_sl2a_build__success(void)
{
    gnrc_pktsnip_t *pkt;
    ndp_opt_t *opt;

    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ndp_opt_sl2a_build(test_src_l2,
                                                        sizeof(test_src_l2),
                                                        NULL)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    /* check packet meta-data */
    TEST_ASSERT_ALLOCATION(pkt, ceil8(sizeof(ndp_opt_t) + sizeof(test_src_l2)),
                           GNRC_NETTYPE_UNDEF);
    /* check packet content */
    opt = pkt->data;
    TEST_ASSERT_EQUAL_INT(NDP_OPT_SL2A, opt->type);
    TEST_ASSERT_EQUAL_INT(ceil8(sizeof(ndp_opt_t) + sizeof(test_src_l2)) / 8,
                          opt->len);
    TEST_ASSERT_MESSAGE(memcmp(test_src_l2, opt + 1, sizeof(test_src_l2)) == 0,
                        "opt->l2addr != test_src_l2");
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_opt_tl2a_build__pktbuf_full(void)
{
    fill_pktbuf();
    TEST_ASSERT_NULL(gnrc_ndp_opt_tl2a_build(test_src_l2, sizeof(test_src_l2),
                                             NULL));
}

static void test_opt_tl2a_build__success(void)
{
    gnrc_pktsnip_t *pkt;
    ndp_opt_t *opt;

    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ndp_opt_tl2a_build(test_src_l2,
                                                        sizeof(test_src_l2),
                                                        NULL)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    /* check packet meta-data */
    TEST_ASSERT_ALLOCATION(pkt, ceil8(sizeof(ndp_opt_t) + sizeof(test_src_l2)),
                           GNRC_NETTYPE_UNDEF);
    /* check packet content */
    opt = pkt->data;
    TEST_ASSERT_EQUAL_INT(NDP_OPT_TL2A, opt->type);
    TEST_ASSERT_EQUAL_INT(ceil8(sizeof(ndp_opt_t) + sizeof(test_src_l2)) / 8,
                          opt->len);
    TEST_ASSERT_MESSAGE(memcmp(test_src_l2, opt + 1, sizeof(test_src_l2)) == 0,
                        "opt->l2addr != test_src_l2");
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_opt_pi_build__pktbuf_full(void)
{
    fill_pktbuf();
    TEST_ASSERT_NULL(gnrc_ndp_opt_pi_build(&test_pfx, TEST_PFX_LEN,
                                           TEST_VALID_LTIME, TEST_PREF_LTIME,
                                           0, NULL));
}

static void test_opt_pi_build__success(uint8_t flags)
{
    gnrc_pktsnip_t *pkt;
    ndp_opt_pi_t *opt;
    ipv6_addr_t exp_pfx = IPV6_ADDR_UNSPECIFIED;

    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ndp_opt_pi_build(&test_pfx, TEST_PFX_LEN,
                                                      TEST_VALID_LTIME,
                                                      TEST_PREF_LTIME, flags,
                                                      NULL)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    /* check packet meta-data */
    TEST_ASSERT_ALLOCATION(pkt, ceil8(sizeof(ndp_opt_pi_t)),
                           GNRC_NETTYPE_UNDEF);
    /* check packet content */
    /* prepare expected prefix (the function MUST remove all the garbage after
     * the prefix) */
    ipv6_addr_init_prefix(&exp_pfx, &test_pfx, TEST_PFX_LEN);
    opt = pkt->data;
    TEST_ASSERT_EQUAL_INT(NDP_OPT_PI, opt->type);
    TEST_ASSERT_EQUAL_INT(NDP_OPT_PI_LEN, opt->len);
    TEST_ASSERT_EQUAL_INT(TEST_PFX_LEN, opt->prefix_len);
    TEST_ASSERT_EQUAL_INT(flags, opt->flags);
    TEST_ASSERT_EQUAL_INT(TEST_VALID_LTIME,
                          byteorder_ntohl(opt->valid_ltime));
    TEST_ASSERT_EQUAL_INT(TEST_PREF_LTIME,
                          byteorder_ntohl(opt->pref_ltime));
    TEST_ASSERT_EQUAL_INT(0, opt->resv.u32);
    TEST_ASSERT_MESSAGE(ipv6_addr_equal(&exp_pfx, &opt->prefix),
                        "opt->prefix != exp_pfx");
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_opt_pi_build__success_without_flags(void)
{
    test_opt_pi_build__success(0);
}

static void test_opt_pi_build__success_with_flags(void)
{
    test_opt_pi_build__success(NDP_OPT_PI_FLAGS_L);
}

static void test_opt_mtu_build__pktbuf_full(void)
{
    fill_pktbuf();
    TEST_ASSERT_NULL(gnrc_ndp_opt_mtu_build(TEST_MTU, NULL));
}

static void test_opt_mtu_build__success(void)
{
    gnrc_pktsnip_t *pkt;
    ndp_opt_mtu_t *opt;

    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ndp_opt_mtu_build(TEST_MTU, NULL)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    /* check packet meta-data */
    TEST_ASSERT_ALLOCATION(pkt, ceil8(sizeof(ndp_opt_mtu_t)), GNRC_NETTYPE_UNDEF);
    /* check packet content */
    opt = pkt->data;
    TEST_ASSERT_EQUAL_INT(NDP_OPT_MTU, opt->type);
    TEST_ASSERT_EQUAL_INT(NDP_OPT_MTU_LEN, opt->len);
    TEST_ASSERT_EQUAL_INT(0, opt->resv.u16);
    TEST_ASSERT_EQUAL_INT(TEST_MTU, byteorder_ntohl(opt->mtu));
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static inline kernel_pid_t _get_iface(gnrc_netif_hdr_t *hdr)
{
    return hdr->if_pid;
}

static inline ipv6_addr_t *_get_ipv6_src(ipv6_hdr_t *hdr)
{
    return &hdr->src;
}

static inline ipv6_addr_t *_get_ipv6_dst(ipv6_hdr_t *hdr)
{
    return &hdr->dst;
}

#define ASSERT_NETIF_HDR(netif, pkt) \
    TEST_ASSERT_NOT_NULL(pkt); \
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_NETIF, pkt->type); \
    TEST_ASSERT_NOT_NULL(pkt->data); \
    TEST_ASSERT_EQUAL_INT(netif->pid, _get_iface(pkt->data))

#define ASSERT_IPV6_HDR(src, dst, pkt) \
    TEST_ASSERT_NOT_NULL(pkt); \
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_IPV6, pkt->type); \
    TEST_ASSERT_NOT_NULL(pkt->data); \
    TEST_ASSERT_MESSAGE(memcmp(src, _get_ipv6_src(pkt->data), \
                        sizeof(ipv6_addr_t)) == 0, "src != pkt->src"); \
    TEST_ASSERT_MESSAGE(memcmp(dst, _get_ipv6_dst(pkt->data), \
                        sizeof(ipv6_addr_t)) == 0, "dst != pkt->dst")

#define ASSERT_ICMPV6_HDR(pkt) \
    TEST_ASSERT_NOT_NULL(pkt); \
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_ICMPV6, pkt->type); \
    TEST_ASSERT_NOT_NULL(pkt->data)

static void test_nbr_sol_send(const ipv6_addr_t *src)
{
    msg_t msg;
    gnrc_pktsnip_t *pkt;
    ndp_nbr_sol_t *nbr_sol;

    while (msg_try_receive(&msg) == 1) {
        /* empty message queue */
    }
    TEST_ASSERT_NOT_NULL(test_netif);
    gnrc_ndp_nbr_sol_send(&test_tgt, test_netif, src, &test_dst, NULL);
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    pkt = msg.content.ptr;
    /* check packet */
    ASSERT_NETIF_HDR(test_netif, pkt);
    if ((src != NULL) && ipv6_addr_is_unspecified(src)) {
        ASSERT_IPV6_HDR(&ipv6_addr_unspecified, &test_dst, pkt->next);
    }
    else {
        ASSERT_IPV6_HDR(&test_src, &test_dst, pkt->next);
    }
    ASSERT_ICMPV6_HDR(pkt->next->next);
    TEST_ASSERT_EQUAL_INT(sizeof(ndp_nbr_sol_t), pkt->next->next->size);
    nbr_sol = pkt->next->next->data;
    TEST_ASSERT_EQUAL_INT(ICMPV6_NBR_SOL, nbr_sol->type);
    TEST_ASSERT_MESSAGE(memcmp(&test_tgt, &nbr_sol->tgt, sizeof(ipv6_addr_t)) == 0,
                        "tgt != nbr_sol->tgt");
    if ((src != NULL) && ipv6_addr_is_unspecified(src)) {
        TEST_ASSERT_NULL(pkt->next->next->next);
    }
    else {
        TEST_ASSERT_NOT_NULL(pkt->next->next->next);
        TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, pkt->next->next->next->type);
        TEST_ASSERT_NOT_NULL(pkt->next->next->next->data);
        ndp_opt_t *opt = pkt->next->next->next->data;
        TEST_ASSERT_EQUAL_INT(NDP_OPT_SL2A, opt->type);
        TEST_ASSERT_MESSAGE(memcmp(&test_src_l2, opt + 1, sizeof(test_src_l2)) == 0,
                            "src_l2 != pkt->l2");
        TEST_ASSERT_NULL(pkt->next->next->next->next);
    }
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_nbr_sol_send__src_NULL(void)
{
    test_nbr_sol_send(NULL);
}

static void test_nbr_sol_send__src_unspecified(void)
{
    test_nbr_sol_send(&ipv6_addr_unspecified);
}

static void test_nbr_sol_send__src_NOT_NULL(void)
{
    test_nbr_sol_send(&test_src);
}

static void test_nbr_sol_send__pktbuf_full1(void)
{
    /* don't be able to fit any more data into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          _align(sizeof(gnrc_pktsnip_t)),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_nbr_sol_send(&test_tgt, test_netif, &test_src, &test_dst, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_nbr_sol_send__pktbuf_full2(void)
{
    /* just be able to fit the SLLAO into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of SLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (2 * _align(sizeof(gnrc_pktsnip_t))) - 16,
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_nbr_sol_send(&test_tgt, test_netif, &test_src, &test_dst, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_nbr_sol_send__pktbuf_full3(void)
{
    /* just be able to fit the SLLAO and NS into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of SLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (3 * _align(sizeof(gnrc_pktsnip_t))) - 16 -
                                          sizeof(ndp_nbr_sol_t),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_nbr_sol_send(&test_tgt, test_netif, &test_src, &test_dst, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_nbr_sol_send__pktbuf_full4(void)
{
    /* just be able to fit the SLLAO, NS, and IPv6 header into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of SLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (4 * _align(sizeof(gnrc_pktsnip_t))) - 16 -
                                          sizeof(ndp_nbr_sol_t) -
                                          sizeof(ipv6_hdr_t),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_nbr_sol_send(&test_tgt, test_netif, &test_src, &test_dst, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_nbr_adv_send(const ipv6_addr_t *tgt, const ipv6_addr_t *dst,
                              bool supply_tl2a, gnrc_pktsnip_t *exp_ext_opts)
{
    msg_t msg;
    gnrc_pktsnip_t *pkt;
    ndp_nbr_adv_t *nbr_adv;

    TEST_ASSERT_NOT_NULL(test_netif);
    gnrc_ndp_nbr_adv_send(tgt, test_netif, dst, supply_tl2a, exp_ext_opts);
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    pkt = msg.content.ptr;
    /* check packet */
    ASSERT_NETIF_HDR(test_netif, pkt);
    if (ipv6_addr_is_unspecified(dst)) {
        ASSERT_IPV6_HDR(&ipv6_addr_unspecified, &ipv6_addr_all_nodes_link_local,
                        pkt->next);
    }
    else {
        ASSERT_IPV6_HDR(&ipv6_addr_unspecified, &test_dst, pkt->next);
    }
    ASSERT_ICMPV6_HDR(pkt->next->next);
    TEST_ASSERT_EQUAL_INT(sizeof(ndp_nbr_adv_t), pkt->next->next->size);
    nbr_adv = pkt->next->next->data;
    TEST_ASSERT_EQUAL_INT(ICMPV6_NBR_ADV, nbr_adv->type);
    TEST_ASSERT_MESSAGE(memcmp(tgt, &nbr_adv->tgt, sizeof(ipv6_addr_t)) == 0,
                        "tgt != nbr_adv->tgt");
    if (supply_tl2a || ipv6_addr_is_unspecified(dst) || (exp_ext_opts != NULL)) {
        gnrc_pktsnip_t *ext_opts = NULL;

        TEST_ASSERT_NOT_NULL(pkt->next->next->next);
        TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, pkt->next->next->next->type);
        TEST_ASSERT_NOT_NULL(pkt->next->next->next->data);
        /* implicitly this is the same */
        if (supply_tl2a || ipv6_addr_is_unspecified(dst)) {
            ndp_opt_t *opt = pkt->next->next->next->data;
            TEST_ASSERT_EQUAL_INT(NDP_OPT_TL2A, opt->type);
            TEST_ASSERT_MESSAGE(memcmp(&test_src_l2, opt + 1, sizeof(test_src_l2)) == 0,
                                "src_l2 != pkt->l2");
            if (exp_ext_opts == NULL) {
                TEST_ASSERT_NULL(pkt->next->next->next->next);
            }
            else {
                /* extra option comes after TL2AO */
                ext_opts = pkt->next->next->next->next;
            }
        }
        else {
            /* extra option comes directly after neighbor advertisement */
            ext_opts = pkt->next->next->next;
        }
        TEST_ASSERT(exp_ext_opts == ext_opts);
    }
    else {
        TEST_ASSERT_NULL(pkt->next->next->next);
    }
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_nbr_adv_send__foreign_tgt_unspecified_dst_no_supply_tl2a_no_ext_opts(void)
{
    test_nbr_adv_send(&test_src, &ipv6_addr_unspecified, false, NULL);
}

static void test_nbr_adv_send__foreign_tgt_unspecified_dst_no_supply_tl2a_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_nbr_adv_send(&test_src, &ipv6_addr_unspecified, false, ext_opts);
}

static void test_nbr_adv_send__foreign_tgt_unspecified_dst_supply_tl2a_no_ext_opts(void)
{
    test_nbr_adv_send(&test_src, &ipv6_addr_unspecified, true, NULL);
}

static void test_nbr_adv_send__foreign_tgt_unspecified_dst_supply_tl2a_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_nbr_adv_send(&test_src, &ipv6_addr_unspecified, true, ext_opts);
}

static void test_nbr_adv_send__foreign_tgt_specified_dst_no_supply_tl2a_no_ext_opts(void)
{
    test_nbr_adv_send(&test_src, &test_dst, false, NULL);
}

static void test_nbr_adv_send__foreign_tgt_specified_dst_no_supply_tl2a_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_nbr_adv_send(&test_src, &test_dst, false, ext_opts);
}

static void test_nbr_adv_send__foreign_tgt_specified_dst_supply_tl2a_no_ext_opts(void)
{
    test_nbr_adv_send(&test_src, &test_dst, true, NULL);
}

static void test_nbr_adv_send__foreign_tgt_specified_dst_supply_tl2a_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_nbr_adv_send(&test_src, &test_dst, true, ext_opts);
}

static void test_nbr_adv_send__src_tgt_unspecified_dst_no_supply_tl2a_no_ext_opts(void)
{
    test_nbr_adv_send(&test_src, &ipv6_addr_unspecified, false, NULL);
}

static void test_nbr_adv_send__src_tgt_unspecified_dst_no_supply_tl2a_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_nbr_adv_send(&test_src, &ipv6_addr_unspecified, false, ext_opts);
}

static void test_nbr_adv_send__src_tgt_unspecified_dst_supply_tl2a_no_ext_opts(void)
{
    test_nbr_adv_send(&test_src, &ipv6_addr_unspecified, true, NULL);
}

static void test_nbr_adv_send__src_tgt_unspecified_dst_supply_tl2a_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_nbr_adv_send(&test_src, &ipv6_addr_unspecified, true, ext_opts);
}

static void test_nbr_adv_send__src_tgt_specified_dst_no_supply_tl2a_no_ext_opts(void)
{
    test_nbr_adv_send(&test_src, &test_dst, false, NULL);
}

static void test_nbr_adv_send__src_tgt_specified_dst_no_supply_tl2a_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_nbr_adv_send(&test_src, &test_dst, false, ext_opts);
}

static void test_nbr_adv_send__src_tgt_specified_dst_supply_tl2a_no_ext_opts(void)
{
    test_nbr_adv_send(&test_src, &test_dst, true, NULL);
}

static void test_nbr_adv_send__src_tgt_specified_dst_supply_tl2a_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_nbr_adv_send(&test_src, &test_dst, true, ext_opts);
}

static void test_nbr_adv_send__pktbuf_full1(void)
{
    /* don't be able to fit any more data into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          _align(sizeof(gnrc_pktsnip_t)),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_nbr_adv_send(&test_src, test_netif, &test_dst, true, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_nbr_adv_send__pktbuf_full2(void)
{
    /* just be able to fit the TLLAO into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of TLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (2 * _align(sizeof(gnrc_pktsnip_t))) - 16,
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_nbr_adv_send(&test_src, test_netif, &test_dst, true, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_nbr_adv_send__pktbuf_full3(void)
{
    /* just be able to fit the TLLAO and NA into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of TLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (3 * _align(sizeof(gnrc_pktsnip_t))) - 16 -
                                          sizeof(ndp_nbr_adv_t),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_nbr_adv_send(&test_src, test_netif, &test_dst, true, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_nbr_adv_send__pktbuf_full4(void)
{
    /* just be able to fit the TLLAO, NA, and IPv6 header into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of TLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (4 * _align(sizeof(gnrc_pktsnip_t))) - 16 -
                                          sizeof(ndp_nbr_adv_t) -
                                          sizeof(ipv6_hdr_t),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_nbr_adv_send(&test_src, test_netif, &test_dst, true, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_rtr_sol_send(const ipv6_addr_t *dst)
{
    msg_t msg;
    gnrc_pktsnip_t *pkt;
    ndp_rtr_sol_t *rtr_sol;

    TEST_ASSERT_NOT_NULL(test_netif);
    gnrc_ndp_rtr_sol_send(test_netif, dst);
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    pkt = msg.content.ptr;
    /* check packet */
    ASSERT_NETIF_HDR(test_netif, pkt);
    if (dst != NULL) {
        ASSERT_IPV6_HDR(&test_src, dst, pkt->next);
    }
    else {
        ASSERT_IPV6_HDR(&test_src, &ipv6_addr_all_routers_link_local,
                        pkt->next);
    }
    ASSERT_ICMPV6_HDR(pkt->next->next);
    TEST_ASSERT_EQUAL_INT(sizeof(ndp_rtr_sol_t), pkt->next->next->size);
    rtr_sol = pkt->next->next->data;
    TEST_ASSERT_EQUAL_INT(ICMPV6_RTR_SOL, rtr_sol->type);
    if ((dst == NULL) || ipv6_addr_is_link_local(dst)) {
        TEST_ASSERT_NOT_NULL(pkt->next->next->next);
        TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, pkt->next->next->next->type);
        TEST_ASSERT_NOT_NULL(pkt->next->next->next->data);
        ndp_opt_t *opt = pkt->next->next->next->data;
        TEST_ASSERT_EQUAL_INT(NDP_OPT_SL2A, opt->type);
        TEST_ASSERT_MESSAGE(memcmp(&test_src_l2, opt + 1, sizeof(test_src_l2)) == 0,
                            "src_l2 != pkt->l2");
        TEST_ASSERT_NULL(pkt->next->next->next->next);
    }
    else {
        TEST_ASSERT_NULL(pkt->next->next->next);
    }
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_rtr_sol_send__dst_NULL(void)
{
    test_rtr_sol_send(NULL);
}

static void test_rtr_sol_send__dst_local(void)
{
    test_rtr_sol_send(&test_dst);
}

static void test_rtr_sol_send__dst_global(void)
{
    ipv6_addr_t dst;

    memcpy(&dst, &test_dst, sizeof(dst));
    ipv6_addr_init_prefix(&dst, &test_pfx, 64);
    test_rtr_sol_send(&test_dst);
}

static void test_rtr_sol_send__pktbuf_full1(void)
{
    /* don't be able to fit any more data into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          _align(sizeof(gnrc_pktsnip_t)),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_rtr_sol_send(test_netif, &test_dst);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_rtr_sol_send__pktbuf_full2(void)
{
    /* just be able to fit the SLLAO into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of SLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (2 * _align(sizeof(gnrc_pktsnip_t))) - 16,
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_rtr_sol_send(test_netif, &test_dst);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_rtr_sol_send__pktbuf_full3(void)
{
    /* just be able to fit the SLLAO and RS into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of SLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (3 * _align(sizeof(gnrc_pktsnip_t))) - 16 -
                                          sizeof(ndp_rtr_sol_t),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_rtr_sol_send(test_netif, &test_dst);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_rtr_sol_send__pktbuf_full4(void)
{
    /* just be able to fit the SLLAO, RS, and IPv6 header into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of SLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (4 * _align(sizeof(gnrc_pktsnip_t))) - 16 -
                                          sizeof(ndp_rtr_sol_t) -
                                          sizeof(ipv6_hdr_t),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_rtr_sol_send(test_netif, &test_dst);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
static void test_rtr_adv_send(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                              bool fin, gnrc_pktsnip_t *exp_ext_opts)
{
    msg_t msg;
    gnrc_pktsnip_t *pkt;
    ndp_rtr_adv_t *rtr_adv;

    TEST_ASSERT_NOT_NULL(test_netif);
    gnrc_ndp_rtr_adv_send(test_netif, src, dst, fin, exp_ext_opts);
    msg_receive(&msg);
    TEST_ASSERT_EQUAL_INT(GNRC_NETAPI_MSG_TYPE_SND, msg.type);
    pkt = msg.content.ptr;
    /* check packet */
    ASSERT_NETIF_HDR(test_netif, pkt);
    /* testing for unspecified source is complicated so we skip it here and
     * do it in later integration tests */
    if (dst != NULL) {
        ASSERT_IPV6_HDR(&test_src, dst, pkt->next);
    }
    else {
        ASSERT_IPV6_HDR(&test_src, &ipv6_addr_all_nodes_link_local,
                        pkt->next);
    }
    ASSERT_ICMPV6_HDR(pkt->next->next);
    TEST_ASSERT_EQUAL_INT(sizeof(ndp_rtr_adv_t), pkt->next->next->size);
    rtr_adv = pkt->next->next->data;
    TEST_ASSERT_EQUAL_INT(ICMPV6_RTR_ADV, rtr_adv->type);
    if (fin) {
        TEST_ASSERT_EQUAL_INT(0, rtr_adv->ltime.u16);
    }
    else {
        TEST_ASSERT_MESSAGE(rtr_adv->ltime.u16 != 0, "rtr_adv->ltime == 0");
    }
    /* check for SLLAO */
    TEST_ASSERT_NOT_NULL(pkt->next->next->next);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, pkt->next->next->next->type);
    TEST_ASSERT_NOT_NULL(pkt->next->next->next->data);
    ndp_opt_t *opt = pkt->next->next->next->data;
    TEST_ASSERT_EQUAL_INT(NDP_OPT_SL2A, opt->type);
    TEST_ASSERT_MESSAGE(memcmp(&test_src_l2, opt + 1, sizeof(test_src_l2)) == 0,
                        "src_l2 != pkt->l2");
    TEST_ASSERT_MESSAGE(pkt->next->next->next->next == exp_ext_opts,
                        "ext_opts set wrong");
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_rtr_adv_send__src_NULL_dst_NULL_no_fin_no_ext_opts(void)
{
    test_rtr_adv_send(NULL, NULL, false, NULL);
}

static void test_rtr_adv_send__src_NULL_dst_NULL_no_fin_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_rtr_adv_send(NULL, NULL, false, ext_opts);
}

static void test_rtr_adv_send__src_NULL_dst_NULL_fin_no_ext_opts(void)
{
    test_rtr_adv_send(NULL, NULL, true, NULL);
}

static void test_rtr_adv_send__src_NULL_dst_NULL_fin_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_rtr_adv_send(NULL, NULL, true, ext_opts);
}

static void test_rtr_adv_send__src_NULL_dst_no_fin_no_ext_opts(void)
{
    test_rtr_adv_send(NULL, &test_dst, false, NULL);
}

static void test_rtr_adv_send__src_NULL_dst_no_fin_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_rtr_adv_send(NULL, &test_dst, false, ext_opts);
}

static void test_rtr_adv_send__src_NULL_dst_fin_no_ext_opts(void)
{
    test_rtr_adv_send(NULL, &test_dst, true, NULL);
}

static void test_rtr_adv_send__src_NULL_dst_fin_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_rtr_adv_send(NULL, &test_dst, true, ext_opts);
}

static void test_rtr_adv_send__src_dst_NULL_no_fin_no_ext_opts(void)
{
    test_rtr_adv_send(&test_src, NULL, false, NULL);
}

static void test_rtr_adv_send__src_dst_NULL_no_fin_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_rtr_adv_send(&test_src, NULL, false, ext_opts);
}

static void test_rtr_adv_send__src_dst_NULL_fin_no_ext_opts(void)
{
    test_rtr_adv_send(&test_src, NULL, true, NULL);
}

static void test_rtr_adv_send__src_dst_NULL_fin_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_rtr_adv_send(&test_src, NULL, true, ext_opts);
}

static void test_rtr_adv_send__src_dst_no_fin_no_ext_opts(void)
{
    test_rtr_adv_send(&test_src, &test_dst, false, NULL);
}

static void test_rtr_adv_send__src_dst_no_fin_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_rtr_adv_send(&test_src, &test_dst, false, ext_opts);
}

static void test_rtr_adv_send__src_dst_fin_no_ext_opts(void)
{
    test_rtr_adv_send(&test_src, &test_dst, true, NULL);
}

static void test_rtr_adv_send__src_dst_fin_ext_opts(void)
{
    gnrc_pktsnip_t *ext_opts = gnrc_pktbuf_add(NULL, NULL, 8U, GNRC_NETTYPE_UNDEF);
    test_rtr_adv_send(&test_src, &test_dst, true, ext_opts);
}

static void test_rtr_adv_send__pktbuf_full1(void)
{
    /* don't be able to fit any more data into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          _align(sizeof(gnrc_pktsnip_t)),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_rtr_adv_send(test_netif, &test_src, &test_dst, false, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_rtr_adv_send__pktbuf_full2(void)
{
    /* just be able to fit the SLLAO into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of SLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (2 * _align(sizeof(gnrc_pktsnip_t))) - 16,
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_rtr_adv_send(test_netif, &test_src, &test_dst, false, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_rtr_adv_send__pktbuf_full3(void)
{
    /* just be able to fit the SLLAO and RA into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of SLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (3 * _align(sizeof(gnrc_pktsnip_t))) - 16 -
                                          sizeof(ndp_rtr_adv_t),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_rtr_adv_send(test_netif, &test_src, &test_dst, false, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_rtr_adv_send__pktbuf_full4(void)
{
    /* just be able to fit the SLLAO, RA, and IPv6 header into packet buffer
     * - sizeof(gnrc_pktsnip_t) + pktbuf internal padding
     * - 16 == size of SLLAO for IEEE 802.15.4 */
    gnrc_pktsnip_t *tmp = gnrc_pktbuf_add(NULL, NULL,
                                          CONFIG_GNRC_PKTBUF_SIZE -
                                          (4 * _align(sizeof(gnrc_pktsnip_t))) - 16 -
                                          sizeof(ndp_rtr_adv_t) -
                                          sizeof(ipv6_hdr_t),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(tmp);
    gnrc_ndp_rtr_adv_send(test_netif, &test_src, &test_dst, false, NULL);
    gnrc_pktbuf_release(tmp);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}
#endif

static Test *tests_gnrc_ndp_build(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nbr_sol_build__pktbuf_full),
        new_TestFixture(test_nbr_sol_build__success),
        new_TestFixture(test_nbr_adv_build__pktbuf_full),
        new_TestFixture(test_nbr_adv_build__success_without_flags),
        new_TestFixture(test_nbr_adv_build__success_with_flags),
        new_TestFixture(test_rtr_sol_build__pktbuf_full),
        new_TestFixture(test_rtr_sol_build__success),
        new_TestFixture(test_rtr_adv_build__pktbuf_full),
        new_TestFixture(test_rtr_adv_build__success_without_flags),
        new_TestFixture(test_rtr_adv_build__success_with_flags),
        new_TestFixture(test_opt_build__pktbuf_full),
        new_TestFixture(test_opt_build__success),
        new_TestFixture(test_opt_sl2a_build__pktbuf_full),
        new_TestFixture(test_opt_sl2a_build__success),
        new_TestFixture(test_opt_tl2a_build__pktbuf_full),
        new_TestFixture(test_opt_tl2a_build__success),
        new_TestFixture(test_opt_pi_build__pktbuf_full),
        new_TestFixture(test_opt_pi_build__success_without_flags),
        new_TestFixture(test_opt_pi_build__success_with_flags),
        new_TestFixture(test_opt_mtu_build__pktbuf_full),
        new_TestFixture(test_opt_mtu_build__success),
    };

    EMB_UNIT_TESTCALLER(tests, set_up, NULL, fixtures);

    return (Test *)&tests;
}

static Test *tests_gnrc_ndp_send(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nbr_sol_send__src_NULL),
        new_TestFixture(test_nbr_sol_send__src_unspecified),
        new_TestFixture(test_nbr_sol_send__src_NOT_NULL),
        new_TestFixture(test_nbr_sol_send__pktbuf_full1),
        new_TestFixture(test_nbr_sol_send__pktbuf_full2),
        new_TestFixture(test_nbr_sol_send__pktbuf_full3),
        new_TestFixture(test_nbr_sol_send__pktbuf_full4),
        new_TestFixture(test_nbr_adv_send__foreign_tgt_unspecified_dst_no_supply_tl2a_no_ext_opts),
        new_TestFixture(test_nbr_adv_send__foreign_tgt_unspecified_dst_no_supply_tl2a_ext_opts),
        new_TestFixture(test_nbr_adv_send__foreign_tgt_unspecified_dst_supply_tl2a_no_ext_opts),
        new_TestFixture(test_nbr_adv_send__foreign_tgt_unspecified_dst_supply_tl2a_ext_opts),
        new_TestFixture(test_nbr_adv_send__foreign_tgt_specified_dst_no_supply_tl2a_no_ext_opts),
        new_TestFixture(test_nbr_adv_send__foreign_tgt_specified_dst_no_supply_tl2a_ext_opts),
        new_TestFixture(test_nbr_adv_send__foreign_tgt_specified_dst_supply_tl2a_no_ext_opts),
        new_TestFixture(test_nbr_adv_send__foreign_tgt_specified_dst_supply_tl2a_ext_opts),
        new_TestFixture(test_nbr_adv_send__src_tgt_unspecified_dst_no_supply_tl2a_no_ext_opts),
        new_TestFixture(test_nbr_adv_send__src_tgt_unspecified_dst_no_supply_tl2a_ext_opts),
        new_TestFixture(test_nbr_adv_send__src_tgt_unspecified_dst_supply_tl2a_no_ext_opts),
        new_TestFixture(test_nbr_adv_send__src_tgt_unspecified_dst_supply_tl2a_ext_opts),
        new_TestFixture(test_nbr_adv_send__src_tgt_specified_dst_no_supply_tl2a_no_ext_opts),
        new_TestFixture(test_nbr_adv_send__src_tgt_specified_dst_no_supply_tl2a_ext_opts),
        new_TestFixture(test_nbr_adv_send__src_tgt_specified_dst_supply_tl2a_no_ext_opts),
        new_TestFixture(test_nbr_adv_send__src_tgt_specified_dst_supply_tl2a_ext_opts),
        new_TestFixture(test_nbr_adv_send__pktbuf_full1),
        new_TestFixture(test_nbr_adv_send__pktbuf_full2),
        new_TestFixture(test_nbr_adv_send__pktbuf_full3),
        new_TestFixture(test_nbr_adv_send__pktbuf_full4),
        new_TestFixture(test_rtr_sol_send__dst_NULL),
        new_TestFixture(test_rtr_sol_send__dst_local),
        new_TestFixture(test_rtr_sol_send__dst_global),
        new_TestFixture(test_rtr_sol_send__pktbuf_full1),
        new_TestFixture(test_rtr_sol_send__pktbuf_full2),
        new_TestFixture(test_rtr_sol_send__pktbuf_full3),
        new_TestFixture(test_rtr_sol_send__pktbuf_full4),
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER)
        new_TestFixture(test_rtr_adv_send__src_NULL_dst_NULL_no_fin_no_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_NULL_dst_NULL_no_fin_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_NULL_dst_NULL_fin_no_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_NULL_dst_NULL_fin_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_NULL_dst_no_fin_no_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_NULL_dst_no_fin_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_NULL_dst_fin_no_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_NULL_dst_fin_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_dst_NULL_no_fin_no_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_dst_NULL_no_fin_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_dst_NULL_fin_no_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_dst_NULL_fin_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_dst_no_fin_no_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_dst_no_fin_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_dst_fin_no_ext_opts),
        new_TestFixture(test_rtr_adv_send__src_dst_fin_ext_opts),
        new_TestFixture(test_rtr_adv_send__pktbuf_full1),
        new_TestFixture(test_rtr_adv_send__pktbuf_full2),
        new_TestFixture(test_rtr_adv_send__pktbuf_full3),
        new_TestFixture(test_rtr_adv_send__pktbuf_full4),
#endif
    };

    EMB_UNIT_TESTCALLER(tests, set_up, NULL, fixtures);
    return (Test *)&tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_gnrc_ndp_build());
    init_pkt_handler();
    TESTS_RUN(tests_gnrc_ndp_send());
    TESTS_END();

    return 0;
}

#define MSG_QUEUE_SIZE          (2)

static char test_netif_stack[THREAD_STACKSIZE_DEFAULT];
static msg_t msg_queue_main[MSG_QUEUE_SIZE];
static gnrc_netreg_entry_t netreg_entry;
static netdev_test_t dev;

static int _test_netif_send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    (void)netif;
    gnrc_pktbuf_release(pkt);
    return 0;
}

static gnrc_pktsnip_t *_test_netif_recv(gnrc_netif_t *netif)
{
    (void)netif;
    return NULL;
}

static int _test_netif_set(gnrc_netif_t *netif, const gnrc_netapi_opt_t *opt)
{
    (void)netif;
    (void)opt;
    return -ENOTSUP;
}

static const gnrc_netif_ops_t _test_netif_ops = {
    .init = gnrc_netif_default_init,
    .send = _test_netif_send,
    .recv = _test_netif_recv,
    .get = gnrc_netif_get_from_netdev,
    .set = _test_netif_set,
};

static int _netdev_test_address_long(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    expect(max_len >= sizeof(test_src_l2));
    memcpy(value, test_src_l2, sizeof(test_src_l2));
    return sizeof(test_src_l2);
}

static int _netdev_test_proto(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
     expect(max_len == sizeof(gnrc_nettype_t));
     *((gnrc_nettype_t *)value) = GNRC_NETTYPE_UNDEF;
     return sizeof(gnrc_nettype_t);
}

static int _netdev_test_src_len(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
     expect(max_len == sizeof(uint16_t));
     *((uint16_t *)value) = sizeof(test_src_l2);
     return sizeof(uint16_t);
}

static int _netdev_test_max_pdu_size(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
     expect(max_len == sizeof(uint16_t));
     *((uint16_t *)value) = 100U;
     return sizeof(uint16_t);
}

static int _netdev_test_device_type(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
     expect(max_len == sizeof(uint16_t));
     *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
     return sizeof(uint16_t);
}

static void init_pkt_handler(void)
{
    msg_init_queue(msg_queue_main, MSG_QUEUE_SIZE);
    gnrc_netreg_entry_init_pid(&netreg_entry, GNRC_NETREG_DEMUX_CTX_ALL,
                               thread_getpid());
    gnrc_netreg_register(GNRC_NETTYPE_NDP, &netreg_entry);
    netdev_test_setup(&dev, NULL);
    netdev_test_set_get_cb(&dev, NETOPT_ADDRESS_LONG,
                           _netdev_test_address_long);
    netdev_test_set_get_cb(&dev, NETOPT_PROTO, _netdev_test_proto);
    netdev_test_set_get_cb(&dev, NETOPT_SRC_LEN, _netdev_test_src_len);
    netdev_test_set_get_cb(&dev, NETOPT_MAX_PDU_SIZE,
                           _netdev_test_max_pdu_size);
    netdev_test_set_get_cb(&dev, NETOPT_DEVICE_TYPE, _netdev_test_device_type);
    int res = gnrc_netif_create(&_netif, test_netif_stack, sizeof(test_netif_stack),
                                   GNRC_NETIF_PRIO, "test-netif",
                                   &dev.netdev.netdev, &_test_netif_ops);
    test_netif = &_netif;
    TEST_ASSERT_MESSAGE(res == 0,
                        "Unable to start test interface");
    memcpy(&test_netif->ipv6.addrs[0], &test_src,
           sizeof(test_netif->ipv6.addrs[0]));
    test_netif->ipv6.addrs_flags[0] = GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID;
    memcpy(test_netif->l2addr, test_src_l2, sizeof(test_netif->l2addr));
    test_netif->l2addr_len = sizeof(test_src_l2);
}

static inline size_t ceil8(size_t size)
{
    if (size % 8) {
        return ((size / 8) + 1) * 8;
    }
    else {
        return size;
    }
}
