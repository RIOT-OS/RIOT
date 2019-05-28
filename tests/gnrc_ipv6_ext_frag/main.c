/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Tests extension header handling of gnrc stack.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * @}
 */

#include <stddef.h>

#include "byteorder.h"
#include "clist.h"
#include "embUnit.h"
#include "net/ipv6/ext/frag.h"
#include "net/protnum.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/ext.h"
#include "net/gnrc/ipv6/ext/frag.h"
#include "net/gnrc/ipv6/hdr.h"
#include "shell.h"

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

static char line_buf[SHELL_DEFAULT_BUFSIZE];
static const shell_command_t shell_commands[] = {
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
    { NULL, NULL, NULL }
};

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
    for (unsigned i = 1; i < (2 * GNRC_IPV6_EXT_FRAG_RBUF_SIZE); i++) {
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

    rbuf->arrival -= GNRC_IPV6_EXT_FRAG_RBUF_TIMEOUT_US;
    gnrc_ipv6_ext_frag_rbuf_gc();
    TEST_ASSERT_NULL(rbuf->pkt);
    TEST_ASSERT_NULL(rbuf->ipv6);
    TEST_ASSERT_NULL(rbuf->limits.next);
}

static void test_ipv6_ext_frag_reass_in_order(void)
{
    static const ipv6_addr_t src = { .u8 = TEST_SRC };
    static const ipv6_addr_t dst = { .u8 = TEST_DST };
    static const uint8_t exp_payload[] = TEST_PAYLOAD;
    static const uint8_t test_frag1[] = TEST_FRAG1;
    static const uint8_t test_frag2[] = TEST_FRAG2;
    static const uint8_t test_frag3[] = TEST_FRAG3;
    gnrc_pktsnip_t *ipv6_snip = gnrc_ipv6_hdr_build(NULL, &src, &dst);
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(ipv6_snip, test_frag1,
                                          sizeof(test_frag1),
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
    TEST_ASSERT_EQUAL_INT(sizeof(test_frag1) - sizeof(ipv6_ext_frag_t),
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
    TEST_ASSERT(memcmp(exp_payload, rbuf->pkt->data, rbuf->pkt->size) == 0);

    /* prepare 2nd fragment */
    ipv6_snip = gnrc_ipv6_hdr_build(NULL, &src, &dst);
    pkt = gnrc_pktbuf_add(ipv6_snip, test_frag2,
                          sizeof(test_frag2),
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
    TEST_ASSERT_EQUAL_INT(sizeof(test_frag1) + sizeof(test_frag2) -
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
    TEST_ASSERT(memcmp(exp_payload, rbuf->pkt->data, rbuf->pkt->size) == 0);

    /* prepare 3rd fragment */
    ipv6_snip = gnrc_ipv6_hdr_build(NULL, &src, &dst);
    pkt = gnrc_pktbuf_add(ipv6_snip, test_frag3,
                          sizeof(test_frag3),
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
    TEST_ASSERT_EQUAL_INT(sizeof(exp_payload), pkt->size);
    TEST_ASSERT(memcmp(exp_payload, pkt->data, pkt->size) == 0);
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
    static const ipv6_addr_t src = { .u8 = TEST_SRC };
    static const ipv6_addr_t dst = { .u8 = TEST_DST };
    static const uint8_t exp_payload[] = TEST_PAYLOAD;
    static const uint8_t test_frag1[] = TEST_FRAG1;
    static const uint8_t test_frag2[] = TEST_FRAG2;
    static const uint8_t test_frag3[] = TEST_FRAG3;
    gnrc_pktsnip_t *ipv6_snip = gnrc_ipv6_hdr_build(NULL, &src, &dst);
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(ipv6_snip, test_frag3,
                                          sizeof(test_frag3),
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
    TEST_ASSERT_EQUAL_INT(sizeof(exp_payload), rbuf->pkt->size);
    TEST_ASSERT_EQUAL_INT(TEST_ID, rbuf->id);
    TEST_ASSERT(rbuf->last);
    ptr = (gnrc_ipv6_ext_frag_limits_t *)rbuf->limits.next;
    TEST_ASSERT_NOT_NULL(ptr);
    ptr = ptr->next;
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL_INT(TEST_FRAG3_OFFSET / 8, ptr->start);
    TEST_ASSERT_EQUAL_INT(sizeof(exp_payload) / 8, ptr->end);
    TEST_ASSERT(((clist_node_t *)ptr) == rbuf->limits.next);
    TEST_ASSERT(memcmp(&exp_payload[TEST_FRAG3_OFFSET],
                       (uint8_t *)rbuf->pkt->data + TEST_FRAG3_OFFSET,
                       rbuf->pkt->size - TEST_FRAG3_OFFSET) == 0);

    /* prepare 2nd fragment */
    ipv6_snip = gnrc_ipv6_hdr_build(NULL, &src, &dst);
    pkt = gnrc_pktbuf_add(ipv6_snip, test_frag2,
                          sizeof(test_frag2),
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
    TEST_ASSERT_EQUAL_INT(sizeof(exp_payload), rbuf->pkt->size);
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
    TEST_ASSERT_EQUAL_INT(sizeof(exp_payload) / 8, ptr->end);
    TEST_ASSERT_NOT_NULL(ptr->next);
    TEST_ASSERT(((clist_node_t *)ptr) == rbuf->limits.next);
    TEST_ASSERT(memcmp(&exp_payload[TEST_FRAG2_OFFSET],
                       (uint8_t *)rbuf->pkt->data + TEST_FRAG2_OFFSET,
                       rbuf->pkt->size - TEST_FRAG2_OFFSET) == 0);

    /* prepare 1st fragment */
    ipv6_snip = gnrc_ipv6_hdr_build(NULL, &src, &dst);
    pkt = gnrc_pktbuf_add(ipv6_snip, test_frag1,
                          sizeof(test_frag2),
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
    TEST_ASSERT_EQUAL_INT(sizeof(exp_payload), pkt->size);
    TEST_ASSERT(memcmp(exp_payload, pkt->data, pkt->size) == 0);
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

static void test_ipv6_ext_frag_reass_one_frag(void)
{
    static const ipv6_addr_t src = { .u8 = TEST_SRC };
    static const ipv6_addr_t dst = { .u8 = TEST_DST };
    static const uint8_t exp_payload[] = TEST_PAYLOAD;
    static const uint8_t test_frag1[] = TEST_FRAG1;
    gnrc_pktsnip_t *ipv6_snip = gnrc_ipv6_hdr_build(NULL, &src, &dst);
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(ipv6_snip, test_frag1,
                                          sizeof(test_frag1),
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
    TEST_ASSERT_EQUAL_INT(sizeof(test_frag1) - sizeof(ipv6_ext_frag_t),
                          pkt->size);
    TEST_ASSERT(memcmp(exp_payload, pkt->data, pkt->size) == 0);
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
        new_TestFixture(test_ipv6_ext_frag_reass_one_frag),
    };

    EMB_UNIT_TESTCALLER(ipv6_ext_frag_tests, NULL, tear_down_tests, fixtures);
    TESTS_START();
    TESTS_RUN((Test *)&ipv6_ext_frag_tests);
    TESTS_END();
}

int main(void)
{
    run_unittests();
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
