/*
 * Copyright (C) 2016, 2016 Shuguo Zhuo <shuguo.zhuo@inria.fr>
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
#include <string.h>

#include "embUnit.h"

#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/mac/internal.h"

#include "unittests-constants.h"
#include "tests-gnrc_mac_internal.h"

static void set_up(void)
{
    gnrc_pktbuf_init();
}

#if GNRC_MAC_TX_QUEUE_SIZE != 0
/**
 * @brief This function test the `gnrc_mac_queue_tx_packet()`, to see whether it can
 *        correctly queue the packet to the corresponded priority packet queue.
 *
 *        In case when the `gnrc_mac_tx_neighbor_t` structure is in used (indicated by
 *        by `CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0`), `test_gnrc_mac_queue_tx_packet()` successively
 *        queues 4 packets, which are pkt1, pkt2, pkt3 and pkt_bcast, into a defined `tx`
 *        (type of `gnrc_mac_tx_t`). Pkt1, pkt2 have the same destination address of "0x76b6",
 *        , pkt3 is heading for "0x447e", while pkt_bcast is for broadcasting.
 *        Expected results: pkt1 and pkt2 should be queued to `tx::neighbors[1]::queue`,
 *        pkt3 should be queued to `tx::neighbors[2]::queue`, while pkt_bcast should be
 *        queued to `tx::neighbors[0]::queue`.
 *
 *        In case when the `gnrc_mac_tx_neighbor_t` structure is not in used (indicated by
 *        by `CONFIG_GNRC_MAC_NEIGHBOR_COUNT == 0`), `test_gnrc_mac_queue_tx_packet()` successively
 *        queues 4 packets, which are pkt1, pkt2, pkt3 and pkt_bcast, into a defined `tx`
 *        (type of `gnrc_mac_tx_t`). Pkt1, pkt2 have the same destination address of "0x76b6",
 *        , pkt3 is heading for "0x447e", while pkt_bcast is for broadcasting.
 *        Expected results: all packets should be queued to `tx::queue`, and ranking in
 *        `tx::queue` according to their priorities.
 *
 */
static void test_gnrc_mac_queue_tx_packet(void)
{
    gnrc_mac_tx_t tx = GNRC_MAC_TX_INIT;
    gnrc_pktsnip_t *hdr;
    gnrc_netif_hdr_t* netif_hdr;
    uint8_t dst_addr[2];

    dst_addr[0] = 0x76;
    dst_addr[1] = 0xb6;

    hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    gnrc_pktsnip_t *pkt_bcast = gnrc_pktbuf_add(NULL, TEST_STRING12, sizeof(TEST_STRING12),
                                                GNRC_NETTYPE_UNDEF);
    hdr = gnrc_pkt_append(hdr, pkt_bcast);
    pkt_bcast = hdr;

    netif_hdr = hdr->data;
    netif_hdr->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;

    hdr = gnrc_netif_hdr_build(NULL, 0, dst_addr, 2);
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                                           GNRC_NETTYPE_UNDEF);
    hdr = gnrc_pkt_append(hdr, pkt1);
    pkt1 = hdr;

    hdr = gnrc_netif_hdr_build(NULL, 0, dst_addr, 2);
    gnrc_pktsnip_t *pkt2 = gnrc_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8),
                                           GNRC_NETTYPE_UNDEF);
    hdr = gnrc_pkt_append(hdr, pkt2);
    pkt2 = hdr;

    dst_addr[0] = 0x44;
    dst_addr[1] = 0x7e;

    hdr = gnrc_netif_hdr_build(NULL, 0, dst_addr, 2);
    gnrc_pktsnip_t *pkt3 = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                           GNRC_NETTYPE_UNDEF);
    hdr = gnrc_pkt_append(hdr, pkt3);
    pkt3 = hdr;

#if CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0

    gnrc_pktsnip_t *pkt_head;
    TEST_ASSERT(gnrc_mac_queue_tx_packet(&tx, 1, pkt1));
    pkt_head = gnrc_priority_pktqueue_head(&tx.neighbors[1].queue);
    TEST_ASSERT(pkt_head == pkt1);
    TEST_ASSERT(1 == gnrc_priority_pktqueue_length(&tx.neighbors[1].queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, pkt_head->next->data);

    TEST_ASSERT(gnrc_mac_queue_tx_packet(&tx, 0, pkt2));
    pkt_head = gnrc_priority_pktqueue_head(&tx.neighbors[1].queue);
    TEST_ASSERT(pkt_head == pkt2);
    TEST_ASSERT(2 == gnrc_priority_pktqueue_length(&tx.neighbors[1].queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt_head->next->data);

    pkt_head = gnrc_priority_pktqueue_pop(&tx.neighbors[1].queue);
    TEST_ASSERT(pkt_head == pkt2);
    TEST_ASSERT(1 == gnrc_priority_pktqueue_length(&tx.neighbors[1].queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt_head->next->data);

    pkt_head = gnrc_priority_pktqueue_head(&tx.neighbors[1].queue);
    TEST_ASSERT(pkt_head == pkt1);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, pkt_head->next->data);

    TEST_ASSERT(gnrc_mac_queue_tx_packet(&tx, 0, pkt3));
    pkt_head = gnrc_priority_pktqueue_head(&tx.neighbors[2].queue);
    TEST_ASSERT(pkt_head == pkt3);
    TEST_ASSERT(1 == gnrc_priority_pktqueue_length(&tx.neighbors[2].queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, pkt_head->next->data);

    TEST_ASSERT(gnrc_mac_queue_tx_packet(&tx, 0, pkt_bcast));
    pkt_head = gnrc_priority_pktqueue_head(&tx.neighbors[0].queue);
    TEST_ASSERT(pkt_head == pkt_bcast);
    TEST_ASSERT(1 == gnrc_priority_pktqueue_length(&tx.neighbors[0].queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING12, pkt_head->next->data);

#else

    TEST_ASSERT(gnrc_mac_queue_tx_packet(&tx, 1, pkt1));
    TEST_ASSERT(1 == gnrc_priority_pktqueue_length(&tx.queue));
    gnrc_pktsnip_t *pkt_head;
    pkt_head = gnrc_priority_pktqueue_head(&tx.queue);
    TEST_ASSERT(pkt_head == pkt1);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, pkt_head->next->data);

    TEST_ASSERT(gnrc_mac_queue_tx_packet(&tx, 1, pkt2));
    TEST_ASSERT(2 == gnrc_priority_pktqueue_length(&tx.queue));
    pkt_head = gnrc_priority_pktqueue_head(&tx.queue);
    TEST_ASSERT(pkt_head == pkt1);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, pkt_head->next->data);

    TEST_ASSERT(gnrc_mac_queue_tx_packet(&tx, 0, pkt3));
    TEST_ASSERT(3 == gnrc_priority_pktqueue_length(&tx.queue));
    pkt_head = gnrc_priority_pktqueue_head(&tx.queue);
    TEST_ASSERT(pkt_head == pkt3);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, pkt_head->next->data);

    TEST_ASSERT(gnrc_mac_queue_tx_packet(&tx, 0, pkt_bcast));
    TEST_ASSERT(4 == gnrc_priority_pktqueue_length(&tx.queue));
    pkt_head = gnrc_priority_pktqueue_head(&tx.queue);
    TEST_ASSERT(pkt_head == pkt3);

    pkt_head = gnrc_priority_pktqueue_pop(&tx.queue);
    TEST_ASSERT(pkt_head == pkt3);
    TEST_ASSERT(3 == gnrc_priority_pktqueue_length(&tx.queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, pkt_head->next->data);

    pkt_head = gnrc_priority_pktqueue_pop(&tx.queue);
    TEST_ASSERT(pkt_head == pkt_bcast);
    TEST_ASSERT(2 == gnrc_priority_pktqueue_length(&tx.queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING12, pkt_head->next->data);

    pkt_head = gnrc_priority_pktqueue_pop(&tx.queue);
    TEST_ASSERT(pkt_head == pkt1);
    TEST_ASSERT(1 == gnrc_priority_pktqueue_length(&tx.queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, pkt_head->next->data);

    pkt_head = gnrc_priority_pktqueue_pop(&tx.queue);
    TEST_ASSERT(pkt_head == pkt2);
    TEST_ASSERT(0 == gnrc_priority_pktqueue_length(&tx.queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt_head->next->data);

#endif /* CONFIG_GNRC_MAC_NEIGHBOR_COUNT != 0 */
}
#endif /* GNRC_MAC_TX_QUEUE_SIZE != 0 */

#if GNRC_MAC_RX_QUEUE_SIZE != 0
/**
 * @brief This function test the `gnrc_mac_queue_rx_packet()`, to see whether it can
 *        correctly queue the packets to `rx::queue` according to their priorities.
 *
 *        `test_gnrc_mac_queue_tx_packet()` successively queues 3 packets, which are
 *        pkt1, pkt2, pkt3, into a defined `rx` (type of `gnrc_mac_rx_t`).
 *        Pkt1, pkt2 have the same priority of "1", while pkt3 has the priority of "0".
 *        Expected results: after all the packets are queued, in `rx::queue`, them should
 *        be ranked as (from high priority to low): pkt3, pkt1 and pkt2.
 *
 */
static void test_gnrc_mac_queue_rx_packet(void)
{
    gnrc_mac_rx_t rx = GNRC_MAC_RX_INIT;
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                                           GNRC_NETTYPE_UNDEF);
    gnrc_pktsnip_t *pkt2 = gnrc_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8),
                                           GNRC_NETTYPE_UNDEF);
    gnrc_pktsnip_t *pkt3 = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                           GNRC_NETTYPE_UNDEF);

    TEST_ASSERT(gnrc_mac_queue_rx_packet(&rx, 1, pkt1));
    TEST_ASSERT(1 == gnrc_priority_pktqueue_length(&rx.queue));

    gnrc_pktsnip_t *pkt_head;
    pkt_head = gnrc_priority_pktqueue_head(&rx.queue);

    TEST_ASSERT(pkt_head == pkt1);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, pkt_head->data);

    TEST_ASSERT(gnrc_mac_queue_rx_packet(&rx, 1, pkt2));
    TEST_ASSERT(2 == gnrc_priority_pktqueue_length(&rx.queue));

    pkt_head = gnrc_priority_pktqueue_head(&rx.queue);

    TEST_ASSERT(pkt_head == pkt1);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, pkt_head->data);

    TEST_ASSERT(gnrc_mac_queue_rx_packet(&rx, 0, pkt3));
    TEST_ASSERT(3 == gnrc_priority_pktqueue_length(&rx.queue));

    pkt_head = gnrc_priority_pktqueue_head(&rx.queue);

    TEST_ASSERT(pkt_head == pkt3);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, pkt_head->data);

    pkt_head = gnrc_priority_pktqueue_pop(&rx.queue);
    TEST_ASSERT(pkt_head == pkt3);
    TEST_ASSERT(2 == gnrc_priority_pktqueue_length(&rx.queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, pkt_head->data);

    pkt_head = gnrc_priority_pktqueue_pop(&rx.queue);
    TEST_ASSERT(pkt_head == pkt1);
    TEST_ASSERT(1 == gnrc_priority_pktqueue_length(&rx.queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, pkt_head->data);

    pkt_head = gnrc_priority_pktqueue_pop(&rx.queue);
    TEST_ASSERT(pkt_head == pkt2);
    TEST_ASSERT(0 == gnrc_priority_pktqueue_length(&rx.queue));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt_head->data);
}
#endif /* GNRC_MAC_RX_QUEUE_SIZE != 0 */

#if GNRC_MAC_DISPATCH_BUFFER_SIZE != 0
static void test_gnrc_mac_dispatch(void)
{
    gnrc_mac_rx_t rx = GNRC_MAC_RX_INIT;

    for (size_t i = 0; i < GNRC_MAC_DISPATCH_BUFFER_SIZE; i++) {
        rx.dispatch_buffer[i] = gnrc_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                                                GNRC_NETTYPE_UNDEF);
    }

    gnrc_mac_dispatch(&rx);

    for (size_t i = 0; i < GNRC_MAC_DISPATCH_BUFFER_SIZE; i++) {
        TEST_ASSERT_NULL(rx.dispatch_buffer[i]);
    }
}
#endif /* GNRC_MAC_DISPATCH_BUFFER_SIZE != 0 */

Test *tests_gnrc_mac_internal_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
#if GNRC_MAC_TX_QUEUE_SIZE != 0
        new_TestFixture(test_gnrc_mac_queue_tx_packet),
#endif /* GNRC_MAC_TX_QUEUE_SIZE != 0 */
#if GNRC_MAC_RX_QUEUE_SIZE != 0
        new_TestFixture(test_gnrc_mac_queue_rx_packet),
#endif /* GNRC_MAC_RX_QUEUE_SIZE != 0 */
#if GNRC_MAC_DISPATCH_BUFFER_SIZE != 0
        new_TestFixture(test_gnrc_mac_dispatch),
#endif /* GNRC_MAC_DISPATCH_BUFFER_SIZE != 0 */
    };

    EMB_UNIT_TESTCALLER(gnrc_mac_internal_tests, set_up, NULL, fixtures);

    return (Test *)&gnrc_mac_internal_tests;
}

void tests_gnrc_mac_internal(void)
{
    TESTS_RUN(tests_gnrc_mac_internal_tests());
}
/** @} */
