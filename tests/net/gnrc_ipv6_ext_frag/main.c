/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief       Tests IPv6 fragmentation header handling of gnrc stack.
 *
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "byteorder.h"
#include "clist.h"
#include "embUnit.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/ext/frag.h"
#include "net/protnum.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/ext.h"
#include "net/gnrc/ipv6/ext/frag.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif/raw.h"
#include "net/gnrc/udp.h"
#include "net/netdev_test.h"
#include "od.h"
#include "random.h"
#include "shell.h"
#include "test_utils/expect.h"
#include "xtimer.h"

#define TEST_SAMPLE         "This is a test. Failure might sometimes be an " \
                            "option, but not today. "
#define TEST_PORT           (20908U)
#define TEST_FRAG1          { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                              0xab, 0xcf, 0xde, 0xb8, 0x18, 0x48, 0xe3, 0x70, \
                              0x30, 0x1a, 0xba, 0x27, 0xa6, 0xa7, 0xce, 0xeb, \
                              0x4c, 0x8e, 0x64, 0xa1, 0x4d, 0x48, 0x19, 0x48 }
#define TEST_FRAG2          { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                              0x9f, 0x7e, 0xcb, 0x94, 0xe4, 0x63, 0xfa, 0xd9, \
                              0xb5, 0x5d, 0x75, 0x8a, 0xd5, 0xa7, 0x4d, 0xe9, \
                              0x22, 0xc2, 0x8a, 0xb9, 0x4e, 0x03, 0xe5, 0x3f }
#define TEST_FRAG3          { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                              0x6d, 0x07, 0x7c, 0xac, 0xad, 0x1f, 0x97, 0x1c, \
                              0x48, 0x96, 0x34, 0x46, 0xf9, 0xec, 0xbc }
#define TEST_PAYLOAD        { 0xab, 0xcf, 0xde, 0xb8, 0x18, 0x48, 0xe3, 0x70, \
                              0x30, 0x1a, 0xba, 0x27, 0xa6, 0xa7, 0xce, 0xeb, \
                              0x4c, 0x8e, 0x64, 0xa1, 0x4d, 0x48, 0x19, 0x48, \
                              0x9f, 0x7e, 0xcb, 0x94, 0xe4, 0x63, 0xfa, 0xd9, \
                              0xb5, 0x5d, 0x75, 0x8a, 0xd5, 0xa7, 0x4d, 0xe9, \
                              0x22, 0xc2, 0x8a, 0xb9, 0x4e, 0x03, 0xe5, 0x3f, \
                              0x6d, 0x07, 0x7c, 0xac, 0xad, 0x1f, 0x97, 0x1c, \
                              0x48, 0x96, 0x34, 0x46, 0xf9, 0xec, 0xbc }
#define TEST_SRC            { 0x20, 0x01, 0xdb, 0x82, 0xb5, 0xf9, 0xbe, 0x78, \
                              0xb1, 0x4d, 0xcd, 0xe8, 0xa9, 0x53, 0x54, 0xb1 }
#define TEST_DST            { 0x20, 0x01, 0xdb, 0x89, 0xa3, 0x24, 0xfd, 0xab, \
                              0x29, 0x73, 0xde, 0xa4, 0xe4, 0xb1, 0xdb, 0xde }
#define TEST_ID             (0x52dacb1)
#define TEST_FRAG1_OFFSET   (0U)
#define TEST_FRAG2_OFFSET   (24U)
#define TEST_FRAG3_OFFSET   (48U)
#define TEST_PAYLOAD_LEN    (21U)
#define TEST_HL             (64U)

extern int udp_cmd(int argc, char **argv);
/* shell_test_cmd is used to test weird snip configurations,
 * the rest can just use udp_cmd */
static int shell_test_cmd(int argc, char **argv);

static netdev_test_t mock_netdev;
static gnrc_netif_t *eth_netif, *mock_netif;
static gnrc_netif_t _netif;
static ipv6_addr_t *local_addr;
static char mock_netif_stack[THREAD_STACKSIZE_DEFAULT];
static char line_buf[SHELL_DEFAULT_BUFSIZE];

static const ipv6_addr_t _src = { .u8 = TEST_SRC };
static const ipv6_addr_t _dst = { .u8 = TEST_DST };
static const uint8_t _exp_payload[] = TEST_PAYLOAD;
static const uint8_t _test_frag1[] = TEST_FRAG1;
static const uint8_t _test_frag2[] = TEST_FRAG2;
static const uint8_t _test_frag3[] = TEST_FRAG3;

static void tear_down_tests(void)
{
    gnrc_ipv6_ext_frag_init();
    gnrc_pktbuf_init();
}

static void test_ipv6_ext_frag_rbuf_get(void)
{
    static ipv6_hdr_t ipv6 = { .src = { .u8 = TEST_SRC },
                               .dst = { .u8 = TEST_DST } };
    gnrc_ipv6_ext_frag_rbuf_t *rbuf = gnrc_ipv6_ext_frag_rbuf_get(&ipv6,
                                                                  TEST_ID);

    TEST_ASSERT_NOT_NULL(rbuf);
    TEST_ASSERT_EQUAL_INT(TEST_ID, rbuf->id);
    TEST_ASSERT_MESSAGE(&ipv6 == rbuf->ipv6, "IPv6 header is not the same");

    /* check that reassembly buffer never gets full */
    for (unsigned i = 1; i < (2 * CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_SIZE); i++) {
        rbuf = gnrc_ipv6_ext_frag_rbuf_get(
                &ipv6, TEST_ID + i
            );
        TEST_ASSERT_NOT_NULL(rbuf);
        TEST_ASSERT_EQUAL_INT(TEST_ID + i, rbuf->id);
        TEST_ASSERT_MESSAGE(&ipv6 == rbuf->ipv6, "IPv6 header is not the same");
    }
}

static void test_ipv6_ext_frag_rbuf_free(void)
{
    static ipv6_hdr_t ipv6 = { .src = { .u8 = TEST_SRC },
                               .dst = { .u8 = TEST_DST } };
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, &ipv6, sizeof(ipv6),
                                          GNRC_NETTYPE_IPV6);
    gnrc_ipv6_ext_frag_rbuf_t *rbuf = gnrc_ipv6_ext_frag_rbuf_get(pkt->data,
                                                                  TEST_ID);

    rbuf->pkt = pkt;
    gnrc_ipv6_ext_frag_rbuf_free(rbuf);
    TEST_ASSERT_NULL(rbuf->ipv6);
    TEST_ASSERT_NULL(rbuf->limits.next);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_ipv6_ext_frag_rbuf_del(void)
{
    static ipv6_hdr_t ipv6 = { .src = { .u8 = TEST_SRC },
                               .dst = { .u8 = TEST_DST } };
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, &ipv6, sizeof(ipv6),
                                          GNRC_NETTYPE_IPV6);
    gnrc_ipv6_ext_frag_rbuf_t *rbuf = gnrc_ipv6_ext_frag_rbuf_get(pkt->data,
                                                                  TEST_ID);

    rbuf->pkt = pkt;
    gnrc_ipv6_ext_frag_rbuf_del(rbuf);
    TEST_ASSERT_NULL(rbuf->pkt);
    TEST_ASSERT_NULL(rbuf->ipv6);
    TEST_ASSERT_NULL(rbuf->limits.next);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_ipv6_ext_frag_rbuf_gc(void)
{
    static ipv6_hdr_t ipv6 = { .src = { .u8 = TEST_SRC },
                               .dst = { .u8 = TEST_DST } };
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, &ipv6, sizeof(ipv6),
                                          GNRC_NETTYPE_IPV6);
    gnrc_ipv6_ext_frag_rbuf_t *rbuf = gnrc_ipv6_ext_frag_rbuf_get(pkt->data,
                                                                  TEST_ID);

    rbuf->pkt = pkt;
    gnrc_ipv6_ext_frag_rbuf_gc();
    TEST_ASSERT_NOT_NULL(rbuf->pkt);
    TEST_ASSERT_MESSAGE(pkt->data == rbuf->ipv6, "IPv6 header is not the same");

    rbuf->arrival -= CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_TIMEOUT_US;
    gnrc_ipv6_ext_frag_rbuf_gc();
    TEST_ASSERT_NULL(rbuf->pkt);
    TEST_ASSERT_NULL(rbuf->ipv6);
    TEST_ASSERT_NULL(rbuf->limits.next);
}

static void test_ipv6_ext_frag_reass_in_order(void)
{
    gnrc_pktsnip_t *ipv6_snip = gnrc_ipv6_hdr_build(NULL, &_src, &_dst);
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(ipv6_snip, _test_frag1,
                                          sizeof(_test_frag1),
                                          GNRC_NETTYPE_UNDEF);
    ipv6_hdr_t *ipv6 = ipv6_snip->data;
    ipv6_ext_frag_t *frag = pkt->data;
    gnrc_ipv6_ext_frag_rbuf_t *rbuf;
    gnrc_ipv6_ext_frag_limits_t *ptr;

    ipv6->nh = PROTNUM_IPV6_EXT_FRAG;
    ipv6->hl = TEST_HL;
    ipv6->len = byteorder_htons(pkt->size);
    frag->nh = PROTNUM_UDP;
    frag->resv = 0U;
    ipv6_ext_frag_set_offset(frag, TEST_FRAG1_OFFSET);
    ipv6_ext_frag_set_more(frag);
    frag->id = byteorder_htonl(TEST_ID);

    /* receive 1st fragment */
    TEST_ASSERT_NULL(gnrc_ipv6_ext_frag_reass(pkt));
    TEST_ASSERT_NOT_NULL((rbuf = gnrc_ipv6_ext_frag_rbuf_get(ipv6, TEST_ID)));
    TEST_ASSERT_NOT_NULL(rbuf->pkt);
    TEST_ASSERT_EQUAL_INT(sizeof(_test_frag1) - sizeof(ipv6_ext_frag_t),
                          rbuf->pkt->size);
    TEST_ASSERT_MESSAGE(ipv6 == rbuf->ipv6, "IPv6 header is not the same");
    TEST_ASSERT_EQUAL_INT(TEST_ID, rbuf->id);
    TEST_ASSERT(!rbuf->last);
    ptr = (gnrc_ipv6_ext_frag_limits_t *)rbuf->limits.next;
    TEST_ASSERT_NOT_NULL(ptr);
    ptr = ptr->next;
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL_INT(0, ptr->start);
    TEST_ASSERT_EQUAL_INT(TEST_FRAG2_OFFSET / 8, ptr->end);
    TEST_ASSERT(((clist_node_t *)ptr) == rbuf->limits.next);
    TEST_ASSERT(memcmp(_exp_payload, rbuf->pkt->data, rbuf->pkt->size) == 0);

    /* prepare 2nd fragment */
    ipv6_snip = gnrc_ipv6_hdr_build(NULL, &_src, &_dst);
    pkt = gnrc_pktbuf_add(ipv6_snip, _test_frag2,
                          sizeof(_test_frag2),
                          GNRC_NETTYPE_UNDEF);
    ipv6 = ipv6_snip->data;
    frag = pkt->data;

    ipv6->nh = PROTNUM_IPV6_EXT_FRAG;
    ipv6->hl = TEST_HL;
    ipv6->len = byteorder_htons(pkt->size);
    frag->nh = PROTNUM_UDP;
    frag->resv = 0U;
    ipv6_ext_frag_set_offset(frag, TEST_FRAG2_OFFSET);
    ipv6_ext_frag_set_more(frag);
    frag->id = byteorder_htonl(TEST_ID);

    /* receive 2nd fragment */
    TEST_ASSERT_NULL(gnrc_ipv6_ext_frag_reass(pkt));
    TEST_ASSERT_NOT_NULL(rbuf->pkt);
    TEST_ASSERT_EQUAL_INT(sizeof(_test_frag1) + sizeof(_test_frag2) -
                          (2 * sizeof(ipv6_ext_frag_t)),
                          rbuf->pkt->size);
    TEST_ASSERT_EQUAL_INT(TEST_ID, rbuf->id);
    TEST_ASSERT(!rbuf->last);
    ptr = (gnrc_ipv6_ext_frag_limits_t *)rbuf->limits.next;
    TEST_ASSERT_NOT_NULL(ptr);
    ptr = ptr->next;
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL_INT(0, ptr->start);
    TEST_ASSERT_EQUAL_INT(TEST_FRAG2_OFFSET / 8, ptr->end);
    TEST_ASSERT_NOT_NULL(ptr->next);
    ptr = ptr->next;
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL_INT(TEST_FRAG2_OFFSET / 8, ptr->start);
    TEST_ASSERT_EQUAL_INT(TEST_FRAG3_OFFSET / 8, ptr->end);
    TEST_ASSERT(((clist_node_t *)ptr) == rbuf->limits.next);
    TEST_ASSERT(memcmp(_exp_payload, rbuf->pkt->data, rbuf->pkt->size) == 0);

    /* prepare 3rd fragment */
    ipv6_snip = gnrc_ipv6_hdr_build(NULL, &_src, &_dst);
    pkt = gnrc_pktbuf_add(ipv6_snip, _test_frag3,
                          sizeof(_test_frag3),
                          GNRC_NETTYPE_UNDEF);
    ipv6 = ipv6_snip->data;
    frag = pkt->data;

    ipv6->nh = PROTNUM_IPV6_EXT_FRAG;
    ipv6->hl = TEST_HL;
    ipv6->len = byteorder_htons(pkt->size);
    frag->nh = PROTNUM_UDP;
    frag->resv = 0U;
    ipv6_ext_frag_set_offset(frag, TEST_FRAG3_OFFSET);
    frag->id = byteorder_htonl(TEST_ID);

    /* receive 3rd fragment */
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ipv6_ext_frag_reass(pkt)));
    /* reassembly buffer should be deleted */
    TEST_ASSERT_NULL(rbuf->ipv6);
    TEST_ASSERT_EQUAL_INT(sizeof(_exp_payload), pkt->size);
    TEST_ASSERT(memcmp(_exp_payload, pkt->data, pkt->size) == 0);
    TEST_ASSERT_NOT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_IPV6, pkt->next->type);
    ipv6 = pkt->next->data;
    TEST_ASSERT_EQUAL_INT(PROTNUM_UDP, ipv6->nh);
    TEST_ASSERT_EQUAL_INT(pkt->size, byteorder_ntohs(ipv6->len));
    TEST_ASSERT_NULL(pkt->next->next);
    gnrc_pktbuf_release(pkt);
    /* and packet handled (and thus released) */
    gnrc_pktbuf_is_empty();
}

static void test_ipv6_ext_frag_reass_out_of_order(void)
{
    gnrc_pktsnip_t *ipv6_snip = gnrc_ipv6_hdr_build(NULL, &_src, &_dst);
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(ipv6_snip, _test_frag3,
                                          sizeof(_test_frag3),
                                          GNRC_NETTYPE_UNDEF);
    ipv6_hdr_t *ipv6 = ipv6_snip->data;
    ipv6_ext_frag_t *frag = pkt->data;
    gnrc_ipv6_ext_frag_rbuf_t *rbuf;
    gnrc_ipv6_ext_frag_limits_t *ptr;

    ipv6->nh = PROTNUM_IPV6_EXT_FRAG;
    ipv6->hl = TEST_HL;
    ipv6->len = byteorder_htons(pkt->size);
    frag->nh = PROTNUM_UDP;
    frag->resv = 0U;
    ipv6_ext_frag_set_offset(frag, TEST_FRAG3_OFFSET);
    frag->id = byteorder_htonl(TEST_ID);

    /* receive 3rd fragment */
    TEST_ASSERT_NULL(gnrc_ipv6_ext_frag_reass(pkt));
    TEST_ASSERT_NOT_NULL((rbuf = gnrc_ipv6_ext_frag_rbuf_get(ipv6, TEST_ID)));
    TEST_ASSERT_NOT_NULL(rbuf->pkt);
    TEST_ASSERT_EQUAL_INT(sizeof(_exp_payload), rbuf->pkt->size);
    TEST_ASSERT_EQUAL_INT(TEST_ID, rbuf->id);
    TEST_ASSERT(rbuf->last);
    ptr = (gnrc_ipv6_ext_frag_limits_t *)rbuf->limits.next;
    TEST_ASSERT_NOT_NULL(ptr);
    ptr = ptr->next;
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL_INT(TEST_FRAG3_OFFSET / 8, ptr->start);
    TEST_ASSERT_EQUAL_INT(sizeof(_exp_payload) / 8, ptr->end);
    TEST_ASSERT(((clist_node_t *)ptr) == rbuf->limits.next);
    TEST_ASSERT(memcmp(&_exp_payload[TEST_FRAG3_OFFSET],
                       (uint8_t *)rbuf->pkt->data + TEST_FRAG3_OFFSET,
                       rbuf->pkt->size - TEST_FRAG3_OFFSET) == 0);

    /* prepare 2nd fragment */
    ipv6_snip = gnrc_ipv6_hdr_build(NULL, &_src, &_dst);
    pkt = gnrc_pktbuf_add(ipv6_snip, _test_frag2,
                          sizeof(_test_frag2),
                          GNRC_NETTYPE_UNDEF);
    ipv6 = ipv6_snip->data;
    frag = pkt->data;

    ipv6->nh = PROTNUM_IPV6_EXT_FRAG;
    ipv6->hl = TEST_HL;
    ipv6->len = byteorder_htons(pkt->size);
    frag->nh = PROTNUM_UDP;
    frag->resv = 0U;
    ipv6_ext_frag_set_offset(frag, TEST_FRAG2_OFFSET);
    ipv6_ext_frag_set_more(frag);
    frag->id = byteorder_htonl(TEST_ID);

    /* receive 2nd fragment */
    TEST_ASSERT_NULL(gnrc_ipv6_ext_frag_reass(pkt));
    TEST_ASSERT_NOT_NULL(rbuf->pkt);
    TEST_ASSERT_EQUAL_INT(sizeof(_exp_payload), rbuf->pkt->size);
    TEST_ASSERT(rbuf->last);
    ptr = (gnrc_ipv6_ext_frag_limits_t *)rbuf->limits.next;
    TEST_ASSERT_NOT_NULL(ptr);
    ptr = ptr->next;
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL_INT(TEST_FRAG2_OFFSET / 8, ptr->start);
    TEST_ASSERT_EQUAL_INT(TEST_FRAG3_OFFSET / 8, ptr->end);
    ptr = ptr->next;
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL_INT(TEST_FRAG3_OFFSET / 8, ptr->start);
    TEST_ASSERT_EQUAL_INT(sizeof(_exp_payload) / 8, ptr->end);
    TEST_ASSERT_NOT_NULL(ptr->next);
    TEST_ASSERT(((clist_node_t *)ptr) == rbuf->limits.next);
    TEST_ASSERT(memcmp(&_exp_payload[TEST_FRAG2_OFFSET],
                       (uint8_t *)rbuf->pkt->data + TEST_FRAG2_OFFSET,
                       rbuf->pkt->size - TEST_FRAG2_OFFSET) == 0);

    /* prepare 1st fragment */
    ipv6_snip = gnrc_ipv6_hdr_build(NULL, &_src, &_dst);
    pkt = gnrc_pktbuf_add(ipv6_snip, _test_frag1,
                          sizeof(_test_frag2),
                          GNRC_NETTYPE_UNDEF);
    ipv6 = ipv6_snip->data;
    frag = pkt->data;

    ipv6->nh = PROTNUM_IPV6_EXT_FRAG;
    ipv6->hl = TEST_HL;
    ipv6->len = byteorder_htons(pkt->size);
    frag->nh = PROTNUM_UDP;
    frag->resv = 0U;
    ipv6_ext_frag_set_offset(frag, TEST_FRAG1_OFFSET);
    ipv6_ext_frag_set_more(frag);
    frag->id = byteorder_htonl(TEST_ID);
    /* receive 1st fragment */
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ipv6_ext_frag_reass(pkt)));
    /* reassembly buffer should be deleted */
    TEST_ASSERT_NULL(rbuf->ipv6);
    TEST_ASSERT_EQUAL_INT(sizeof(_exp_payload), pkt->size);
    TEST_ASSERT(memcmp(_exp_payload, pkt->data, pkt->size) == 0);
    TEST_ASSERT_NOT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_IPV6, pkt->next->type);
    ipv6 = pkt->next->data;
    TEST_ASSERT_EQUAL_INT(PROTNUM_UDP, ipv6->nh);
    TEST_ASSERT_EQUAL_INT(pkt->size, byteorder_ntohs(ipv6->len));
    TEST_ASSERT_NULL(pkt->next->next);
    gnrc_pktbuf_release(pkt);
    /* and packet handled (and thus released) */
    gnrc_pktbuf_is_empty();
}

static void test_ipv6_ext_frag_reass_out_of_order_rbuf_full(void)
{
    gnrc_pktsnip_t *ipv6_snip = gnrc_ipv6_hdr_build(NULL, &_src, &_dst);
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(ipv6_snip, _test_frag3,
                                          sizeof(_test_frag3),
                                          GNRC_NETTYPE_UNDEF);
    ipv6_hdr_t *ipv6 = ipv6_snip->data;
    ipv6_ext_frag_t *frag = pkt->data;
    gnrc_ipv6_ext_frag_rbuf_t *rbuf;
    gnrc_ipv6_ext_frag_limits_t *ptr;
    static const uint32_t foreign_id = TEST_ID + 44U;

    TEST_ASSERT_EQUAL_INT(1, CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_SIZE);
    /* prepare fragment from a from a foreign datagram */
    ipv6->nh = PROTNUM_IPV6_EXT_FRAG;
    ipv6->hl = TEST_HL;
    ipv6->len = byteorder_htons(pkt->size);
    frag->nh = PROTNUM_UDP;
    frag->resv = 0U;
    ipv6_ext_frag_set_offset(frag, TEST_FRAG3_OFFSET);
    frag->id = byteorder_htonl(foreign_id);

    /* receive a fragment from a foreign datagram first */
    TEST_ASSERT_NULL(gnrc_ipv6_ext_frag_reass(pkt));
    TEST_ASSERT_NOT_NULL((rbuf = gnrc_ipv6_ext_frag_rbuf_get(ipv6,
                                                             foreign_id)));
    TEST_ASSERT_NOT_NULL(rbuf->pkt);
    TEST_ASSERT_EQUAL_INT(sizeof(_exp_payload), rbuf->pkt->size);
    TEST_ASSERT_EQUAL_INT(foreign_id, rbuf->id);
    TEST_ASSERT(rbuf->last);
    ptr = (gnrc_ipv6_ext_frag_limits_t *)rbuf->limits.next;
    TEST_ASSERT_NOT_NULL(ptr);
    ptr = ptr->next;
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL_INT(TEST_FRAG3_OFFSET / 8, ptr->start);
    TEST_ASSERT_EQUAL_INT(sizeof(_exp_payload) / 8, ptr->end);
    TEST_ASSERT(((clist_node_t *)ptr) == rbuf->limits.next);
    TEST_ASSERT(memcmp(&_exp_payload[TEST_FRAG3_OFFSET],
                       (uint8_t *)rbuf->pkt->data + TEST_FRAG3_OFFSET,
                       rbuf->pkt->size - TEST_FRAG3_OFFSET) == 0);

    /* redo test_ipv6_ext_frag_reass_one_frag but now rbuf is full and oldest
     * entry should be cycled out */
    test_ipv6_ext_frag_reass_out_of_order();
}

static void test_ipv6_ext_frag_reass_one_frag(void)
{
    gnrc_pktsnip_t *ipv6_snip = gnrc_ipv6_hdr_build(NULL, &_src, &_dst);
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(ipv6_snip, _test_frag1,
                                          sizeof(_test_frag1),
                                          GNRC_NETTYPE_UNDEF);
    ipv6_hdr_t *ipv6 = ipv6_snip->data;
    ipv6_ext_frag_t *frag = pkt->data;

    ipv6->nh = PROTNUM_IPV6_EXT_FRAG;
    ipv6->hl = TEST_HL;
    ipv6->len = byteorder_htons(pkt->size);
    frag->nh = PROTNUM_UDP;
    frag->resv = 0U;
    ipv6_ext_frag_set_offset(frag, TEST_FRAG1_OFFSET);
    frag->id = byteorder_htonl(TEST_ID);

    /* receive 1st fragment */
    TEST_ASSERT_NOT_NULL((pkt = gnrc_ipv6_ext_frag_reass(pkt)));
    /* reassembly buffer already consumed */
    TEST_ASSERT_EQUAL_INT(sizeof(_test_frag1) - sizeof(ipv6_ext_frag_t),
                          pkt->size);
    TEST_ASSERT(memcmp(_exp_payload, pkt->data, pkt->size) == 0);
    TEST_ASSERT_NOT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_IPV6, pkt->next->type);
    ipv6 = pkt->next->data;
    TEST_ASSERT_EQUAL_INT(PROTNUM_UDP, ipv6->nh);
    TEST_ASSERT_EQUAL_INT(pkt->size, byteorder_ntohs(ipv6->len));
    TEST_ASSERT_NULL(pkt->next->next);
    gnrc_pktbuf_release(pkt);
    /* and packet handled (and thus released) */
    gnrc_pktbuf_is_empty();
}

static void run_unittests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv6_ext_frag_rbuf_get),
        new_TestFixture(test_ipv6_ext_frag_rbuf_free),
        new_TestFixture(test_ipv6_ext_frag_rbuf_del),
        new_TestFixture(test_ipv6_ext_frag_rbuf_gc),
        new_TestFixture(test_ipv6_ext_frag_reass_in_order),
        new_TestFixture(test_ipv6_ext_frag_reass_out_of_order),
        new_TestFixture(test_ipv6_ext_frag_reass_out_of_order_rbuf_full),
        new_TestFixture(test_ipv6_ext_frag_reass_one_frag),
    };

    EMB_UNIT_TESTCALLER(ipv6_ext_frag_tests, NULL, tear_down_tests, fixtures);
    TESTS_START();
    TESTS_RUN((Test *)&ipv6_ext_frag_tests);
    TESTS_END();
}

static gnrc_pktsnip_t *_build_udp_packet(const ipv6_addr_t *dst,
                                         unsigned payload_size,
                                         gnrc_pktsnip_t *payload)
{
    udp_hdr_t *udp_hdr;
    ipv6_hdr_t *ipv6_hdr;
    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *hdr;

    if (payload == NULL) {
        uint8_t *data;

        payload = gnrc_pktbuf_add(NULL, NULL, payload_size, GNRC_NETTYPE_UNDEF);
        if (payload == NULL) {
            return NULL;
        }
        data = payload->data;
        while (payload_size) {
            unsigned test_sample_len = sizeof(TEST_SAMPLE) - 1;

            if (test_sample_len > payload_size) {
                test_sample_len = payload_size;
            }

            memcpy(data, TEST_SAMPLE, test_sample_len);
            data += test_sample_len;
            payload_size -= test_sample_len;
        }
    }
    hdr = gnrc_udp_hdr_build(payload, TEST_PORT, TEST_PORT);
    if (hdr == NULL) {
        gnrc_pktbuf_release(payload);
        return NULL;
    }
    udp_hdr = hdr->data;
    udp_hdr->length = byteorder_htons(gnrc_pkt_len(hdr));
    payload = hdr;
    hdr = gnrc_ipv6_hdr_build(payload, local_addr, dst);
    if (hdr == NULL) {
        gnrc_pktbuf_release(payload);
        return NULL;
    }
    ipv6_hdr = hdr->data;
    ipv6_hdr->len = byteorder_htons(gnrc_pkt_len(payload));
    ipv6_hdr->nh = PROTNUM_UDP;
    ipv6_hdr->hl = CONFIG_GNRC_NETIF_DEFAULT_HL;
    gnrc_udp_calc_csum(payload, hdr);
    payload = hdr;
    hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    if (hdr == NULL) {
        gnrc_pktbuf_release(payload);
        return NULL;
    }
    netif_hdr = hdr->data;
    gnrc_netif_hdr_set_netif(netif_hdr, eth_netif);
    netif_hdr->flags |= GNRC_NETIF_HDR_FLAGS_MULTICAST;
    hdr->next = payload;
    return hdr;
}

static void test_ipv6_ext_frag_send_pkt_single_frag(const ipv6_addr_t *dst)
{
    gnrc_pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL(local_addr);
    pkt = _build_udp_packet(dst, sizeof(TEST_SAMPLE) - 1, NULL);
    TEST_ASSERT_NOT_NULL(pkt);
    gnrc_ipv6_ext_frag_send_pkt(pkt, eth_netif->ipv6.mtu);
}

static void test_ipv6_ext_frag_payload_snips_not_divisible_of_8(const ipv6_addr_t *dst)
{
    gnrc_pktsnip_t *pkt, *payload = NULL;
    unsigned payload_size = 0;

    TEST_ASSERT_NOT_NULL(local_addr);
    /* TEST_SAMPLE's string length is not a multiple of 8*/
    TEST_ASSERT((sizeof(TEST_SAMPLE) - 1) & 0x7);

    while (payload_size <= eth_netif->ipv6.mtu) {
        pkt = gnrc_pktbuf_add(payload, TEST_SAMPLE, sizeof(TEST_SAMPLE) - 1,
                              GNRC_NETTYPE_UNDEF);
        TEST_ASSERT_NOT_NULL(pkt);
        payload_size += pkt->size;
        payload = pkt;
    }
    pkt = _build_udp_packet(dst, 0, payload);
    TEST_ASSERT_NOT_NULL(pkt);
    gnrc_ipv6_ext_frag_send_pkt(pkt, eth_netif->ipv6.mtu);
}

static int shell_test_cmd(int argc, char **argv)
{
    static ipv6_addr_t dst;
    static void (* const _shell_tests[])(const ipv6_addr_t *) = {
        test_ipv6_ext_frag_send_pkt_single_frag,
        test_ipv6_ext_frag_payload_snips_not_divisible_of_8,
    };
    int test_num;

    if ((argc < 3) || (ipv6_addr_from_str(&dst, argv[1]) == NULL)) {
        puts("usage: test <dst_addr> [<num>]");
        return 1;
    }
    test_num = atoi(argv[2]);
    if ((unsigned)test_num >= ARRAY_SIZE(_shell_tests)) {
        printf("<num> must be between 0 and %u\n",
               (unsigned)ARRAY_SIZE(_shell_tests) - 1);
        return 1;
    }
    printf("Running test %d\n", test_num);
    _shell_tests[test_num](&dst);
    return 0;
}

static int send_test_pkt(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    printf("Sending UDP test packets to port %u\n", TEST_PORT);
    for (unsigned i = 0; i < CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        if (ipv6_addr_is_link_local(&eth_netif->ipv6.addrs[i])) {
            local_addr = &eth_netif->ipv6.addrs[i];
        }
    }
    return 0;
}

static int unittests(int argc, char** argv)
{
    (void) argc;
    (void) argv;

    run_unittests();
    return 0;
}

/* TODO: test if forwarded packet is not fragmented */

static int mock_get_device_type(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    expect(max_len == sizeof(uint16_t));
    *((uint16_t *)value) = NETDEV_TYPE_TEST;
    return sizeof(uint16_t);
}

static int mock_get_max_packet_size(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    expect(max_len == sizeof(uint16_t));
    expect(eth_netif != NULL);
    *((uint16_t *)value) = eth_netif->ipv6.mtu - 8;
    return sizeof(uint16_t);
}

static int mock_send(netdev_t *dev, const iolist_t *iolist)
{
    (void)dev;
    int res = 0;
    while(iolist != NULL) {
        od_hex_dump(iolist->iol_base, iolist->iol_len,
                    OD_WIDTH_DEFAULT);
        res += iolist->iol_len;
        iolist = iolist->iol_next;
    }
    return res;
}

static const shell_command_t shell_commands[] = {
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
    { "unittests", "Runs unitest", unittests},
    { "test", "sends data according to a specified numeric test", shell_test_cmd },
    { "send-test-pkt", "start sendig UDP test packets to TEST_PORT", send_test_pkt },
    { NULL, NULL, NULL }
};

int main(void)
{
    eth_netif = gnrc_netif_iter(NULL);
    /* create mock netif to test forwarding too large fragments */
    netdev_test_setup(&mock_netdev, 0);
    netdev_test_set_get_cb(&mock_netdev, NETOPT_DEVICE_TYPE,
                            mock_get_device_type);
    netdev_test_set_get_cb(&mock_netdev, NETOPT_MAX_PDU_SIZE,
                           mock_get_max_packet_size);
    netdev_test_set_send_cb(&mock_netdev, mock_send);
    int res = gnrc_netif_raw_create(&_netif, mock_netif_stack,
                                    sizeof(mock_netif_stack),
                                    GNRC_NETIF_PRIO, "mock_netif",
                                    &mock_netdev.netdev.netdev);
    mock_netif = &_netif;
    expect(res == 0);
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
