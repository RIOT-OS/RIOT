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

#include "net/ieee802154/mac.h"
#include "mac_queue.h"
#include "tests-ieee802154_mac.h"

static ieee802154_mac_txq_t txq;
static ieee802154_mac_indirect_q_t indirect_q;

static void set_up(void)
{
    memset(&txq, 0, sizeof(txq));
    memset(&indirect_q, 0, sizeof(indirect_q));
    indirect_q.free_mask = (1U << IEEE802154_MAC_TX_INDIRECTQ_SIZE) - 1U;
}

static void test_ieee802154_mac_tx_full_empty_queue(void)
{
    TEST_ASSERT_EQUAL_INT(0, ieee802154_mac_tx_full(&txq));
}

static void test_ieee802154_mac_tx_full_full_queue(void)
{
    for (unsigned i=0; i<IEEE802154_MAC_TXQ_LEN; i++){
        (void)ieee802154_mac_tx_reserve(&txq);
        ieee802154_mac_tx_commit(&txq);
    }

    TEST_ASSERT_EQUAL_INT(1, ieee802154_mac_tx_full(&txq));
}

static void test_ieee802154_mac_tx_empty_empty_queue(void)
{
    TEST_ASSERT_EQUAL_INT(1, ieee802154_mac_tx_empty(&txq));
}

static void test_ieee802154_mac_tx_empty_nonempty_queue(void)
{
    (void)ieee802154_mac_tx_reserve(&txq);
    ieee802154_mac_tx_commit(&txq);

    TEST_ASSERT_EQUAL_INT(0, ieee802154_mac_tx_empty(&txq));
}

static void test_ieee802154_mac_tx_reserve_null(void)
{
    TEST_ASSERT_NULL(ieee802154_mac_tx_reserve(NULL));
}

static void test_ieee802154_mac_tx_reserve_full_queue(void)
{
    for (unsigned i=0; i<IEEE802154_MAC_TXQ_LEN; i++){
        (void)ieee802154_mac_tx_reserve(&txq);
        ieee802154_mac_tx_commit(&txq);
    }

    TEST_ASSERT_NULL(ieee802154_mac_tx_reserve(&txq));
}

static void test_ieee802154_mac_tx_reserve_success(void)
{
    ieee802154_mac_tx_desc_t *d;

    d = ieee802154_mac_tx_reserve(&txq);

    TEST_ASSERT_NOT_NULL(d);
    TEST_ASSERT(&txq.q[0]==d);
    TEST_ASSERT_EQUAL_INT(1, d->in_use);
}

static void test_ieee802154_mac_tx_commit_increments_tail_and_count(void)
{
    ieee802154_mac_tx_commit(&txq);

    TEST_ASSERT_EQUAL_INT(1, txq.tail);
    TEST_ASSERT_EQUAL_INT(1, txq.cnt);
}

static void test_ieee802154_mac_tx_commit_wraps_tail(void)
{
    for (unsigned i=0; i<IEEE802154_MAC_TXQ_LEN; i++){
        (void)ieee802154_mac_tx_reserve(&txq);
        ieee802154_mac_tx_commit(&txq);
    }
    ieee802154_mac_tx_pop(&txq);

    (void)ieee802154_mac_tx_reserve(&txq);
    ieee802154_mac_tx_commit(&txq);

    TEST_ASSERT_EQUAL_INT(1, txq.tail);
    TEST_ASSERT_EQUAL_INT(IEEE802154_MAC_TXQ_LEN, txq.cnt);
}

static void test_ieee802154_mac_tx_peek_null(void)
{
    TEST_ASSERT_NULL(ieee802154_mac_tx_peek(NULL));
}

static void test_ieee802154_mac_tx_peek_empty_queue(void)
{
    TEST_ASSERT_NULL(ieee802154_mac_tx_peek(&txq));
}

static void test_ieee802154_mac_tx_peek_success(void)
{
    ieee802154_mac_tx_desc_t *d_res;
    ieee802154_mac_tx_desc_t *d_peek;

    d_res = ieee802154_mac_tx_reserve(&txq);
    ieee802154_mac_tx_commit(&txq);

    d_peek = ieee802154_mac_tx_peek(&txq);

    TEST_ASSERT_NOT_NULL(d_peek);
    TEST_ASSERT(d_res==d_peek);
}

static void test_ieee802154_mac_tx_pop_null(void)
{
    ieee802154_mac_tx_pop(NULL);
    TEST_ASSERT(1);
}

static void test_ieee802154_mac_tx_pop_empty_queue(void)
{
    ieee802154_mac_tx_pop(&txq);

    TEST_ASSERT_EQUAL_INT(0, txq.head);
    TEST_ASSERT_EQUAL_INT(0, txq.cnt);
}

static void test_ieee802154_mac_tx_pop_success(void)
{
    (void)ieee802154_mac_tx_reserve(&txq);
    ieee802154_mac_tx_commit(&txq);

    ieee802154_mac_tx_pop(&txq);

    TEST_ASSERT_EQUAL_INT(1, txq.head);
    TEST_ASSERT_EQUAL_INT(0, txq.cnt);
    TEST_ASSERT_EQUAL_INT(0, txq.q[0].in_use);
    TEST_ASSERT_EQUAL_INT(0, txq.q[0].handle);
}

static void test_ieee802154_mac_tx_pop_wraps_head(void)
{
    txq.cnt = 1;
    txq.head = IEEE802154_MAC_TXQ_LEN - 1;
    txq.q[IEEE802154_MAC_TXQ_LEN - 1].in_use = true;

    ieee802154_mac_tx_pop(&txq);

    TEST_ASSERT_EQUAL_INT(0, txq.head);
    TEST_ASSERT_EQUAL_INT(0, txq.cnt);
}

static void test_ieee802154_indirectq_alloc_slot_no_free_slot(void)
{
    indirect_q.free_mask = 0;

    TEST_ASSERT_EQUAL_INT(-1, ieee802154_indirectq_alloc_slot(&indirect_q));
}

static void test_ieee802154_indirectq_alloc_slot_returns_first_slot(void)
{
    int slot = ieee802154_indirectq_alloc_slot(&indirect_q);

    TEST_ASSERT_EQUAL_INT(0, slot);
}

static void test_ieee802154_indirectq_alloc_slot_clears_allocated_bit(void)
{
    int slot = ieee802154_indirectq_alloc_slot(&indirect_q);

    TEST_ASSERT_EQUAL_INT(0, slot);
    TEST_ASSERT_EQUAL_INT(((1U << IEEE802154_MAC_TX_INDIRECTQ_SIZE) - 1U) & ~(1U << 0),
                          indirect_q.free_mask);
}

static void test_ieee802154_indirectq_alloc_slot_returns_next_free_slot(void)
{
    indirect_q.free_mask &= ~(1U << 0);

    int slot = ieee802154_indirectq_alloc_slot(&indirect_q);

    TEST_ASSERT_EQUAL_INT(1, slot);
}

static void test_ieee802154_indirectq_free_slot_sets_slot_free(void)
{
    indirect_q.free_mask &= ~(1U << 2);

    ieee802154_indirectq_free_slot(&indirect_q, 2);

    TEST_ASSERT(indirect_q.free_mask & (1U << 2));
}

static void test_ieee802154_indirectq_free_slot_clears_queue_content(void)
{
    indirect_q.q[1].cnt = 3;
    indirect_q.q[1].head = 1;
    indirect_q.q[1].tail = 2;
    indirect_q.q[1].has_dst_addr = true;
    indirect_q.free_mask &= ~(1U << 1);

    ieee802154_indirectq_free_slot(&indirect_q, 1);

    TEST_ASSERT_EQUAL_INT(0, indirect_q.q[1].cnt);
    TEST_ASSERT_EQUAL_INT(0, indirect_q.q[1].head);
    TEST_ASSERT_EQUAL_INT(0, indirect_q.q[1].tail);
    TEST_ASSERT_EQUAL_INT(0, indirect_q.q[1].has_dst_addr);
}

static void test_ieee802154_indirectq_empty_returns_true_when_all_slots_free(void)
{
    TEST_ASSERT(ieee802154_indirectq_empty(&indirect_q));
}

static void test_ieee802154_indirectq_empty_returns_false_when_one_slot_used(void)
{
    indirect_q.free_mask &= ~(1U << 0);

    TEST_ASSERT(!ieee802154_indirectq_empty(&indirect_q));
}

Test *tests_ieee802154_mac_mac_queue_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ieee802154_mac_tx_full_empty_queue),
        new_TestFixture(test_ieee802154_mac_tx_full_full_queue),

        new_TestFixture(test_ieee802154_mac_tx_empty_empty_queue),
        new_TestFixture(test_ieee802154_mac_tx_empty_nonempty_queue),

        new_TestFixture(test_ieee802154_mac_tx_reserve_null),
        new_TestFixture(test_ieee802154_mac_tx_reserve_full_queue),
        new_TestFixture(test_ieee802154_mac_tx_reserve_success),

        new_TestFixture(test_ieee802154_mac_tx_commit_increments_tail_and_count),
        new_TestFixture(test_ieee802154_mac_tx_commit_wraps_tail),

        new_TestFixture(test_ieee802154_mac_tx_peek_null),
        new_TestFixture(test_ieee802154_mac_tx_peek_empty_queue),
        new_TestFixture(test_ieee802154_mac_tx_peek_success),

        new_TestFixture(test_ieee802154_mac_tx_pop_null),
        new_TestFixture(test_ieee802154_mac_tx_pop_empty_queue),
        new_TestFixture(test_ieee802154_mac_tx_pop_success),
        new_TestFixture(test_ieee802154_mac_tx_pop_wraps_head),

        new_TestFixture(test_ieee802154_indirectq_alloc_slot_no_free_slot),
        new_TestFixture(test_ieee802154_indirectq_alloc_slot_returns_first_slot),
        new_TestFixture(test_ieee802154_indirectq_alloc_slot_clears_allocated_bit),
        new_TestFixture(test_ieee802154_indirectq_alloc_slot_returns_next_free_slot),

        new_TestFixture(test_ieee802154_indirectq_free_slot_sets_slot_free),
        new_TestFixture(test_ieee802154_indirectq_free_slot_clears_queue_content),

        new_TestFixture(test_ieee802154_indirectq_empty_returns_true_when_all_slots_free),
        new_TestFixture(test_ieee802154_indirectq_empty_returns_false_when_one_slot_used)
    };

    EMB_UNIT_TESTCALLER(ieee802154_mac_txq_tests, set_up, NULL, fixtures);
    return (Test *)&ieee802154_mac_txq_tests;
}

