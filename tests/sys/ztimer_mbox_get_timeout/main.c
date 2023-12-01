/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       testing ztimer_mbox_get_timeout function
 *
 *
 * @author      Marian Buschsieweke
 *
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "mbox.h"
#include "test_utils/expect.h"
#include "time_units.h"
#include "ztimer.h"

#define MSG_TYPE    42
#define MSG_VAL     1337

static msg_t queue[4];
static mbox_t mbox = MBOX_INIT(queue, ARRAY_SIZE(queue));

static void cb_mbox_put(void *arg)
{
    mbox_try_put(&mbox, arg);
}

static void test_mbox_already_full(void)
{
    printf("testing mbox already full prior call: ");
    msg_t msg = { .type = MSG_TYPE, .content.value = MSG_VAL };
    mbox_put(&mbox, &msg);
    uint32_t start = ztimer_now(ZTIMER_USEC);
    expect(ztimer_mbox_get_timeout(ZTIMER_USEC, &mbox, &msg, US_PER_SEC) == 0);
    uint32_t stop = ztimer_now(ZTIMER_USEC);
    /* Returning immediately should with nothing else running in this test app
     * should not take more than a millisecond */
    expect(stop - start < US_PER_MS);
    printf("OK\n");
}

static void test_timeout_reached(void)
{
    const uint32_t timeout_us = 10 * US_PER_MS;
    printf("testing timeout is reached: ");
    msg_t msg = { .type = MSG_TYPE - 1, .content.value = MSG_VAL - 1 };
    uint32_t start = ztimer_now(ZTIMER_USEC);
    expect(ztimer_mbox_get_timeout(ZTIMER_USEC, &mbox, &msg, timeout_us) == -ECANCELED);
    uint32_t stop = ztimer_now(ZTIMER_USEC);
    /* This may take longer than the timeout due to overhead, background tasks,
     * etc. But it MUST NOT return early. */
    expect(stop - start >= timeout_us);
    /* But it should also not take way too long */
    expect(stop - start < 2 * timeout_us);
    /* msg must not be changed */
    expect((msg.type == MSG_TYPE - 1) && (msg.content.value == MSG_VAL - 1));
    printf("OK\n");
}

static void test_msg_prior_timeout(void)
{
    const uint32_t msg_timeout_us = 1 * US_PER_MS;
    const uint32_t wait_timeout_us = 2 * US_PER_MS;

    msg_t msg = { .type = MSG_TYPE, .content.value = MSG_VAL };
    msg_t got = { .type = 0 };
    ztimer_t t = {
        .callback = cb_mbox_put,
        .arg = &msg,
    };

    ztimer_set(ZTIMER_USEC, &t, msg_timeout_us);
    uint32_t start = ztimer_now(ZTIMER_USEC);
    expect(ztimer_mbox_get_timeout(ZTIMER_USEC, &mbox, &got, wait_timeout_us) == 0);
    uint32_t stop = ztimer_now(ZTIMER_USEC);

    /* the function should return AFTER the message was send, but BEFORE the
     * timeout was triggered */
    expect(stop - start >= msg_timeout_us);
    expect(stop - start < wait_timeout_us);

    /* we should have gotten the correct message */
    expect((got.type == msg.type) && (got.content.value == msg.content.value));
}

int main(void)
{
    const unsigned repetitions = 1000;
    printf("Testing ztimer_mbox_get_timeout()\n"
           "=================================\n");
    test_mbox_already_full();
    test_timeout_reached();
    printf("repeating test, but with sporadic THREAD_FLAG_TIMEOUT set...\n");
    thread_flags_set(thread_get_active(), THREAD_FLAG_TIMEOUT);
    test_timeout_reached();

    printf("Running test for reception prior timeout %u times\n", repetitions);
    for (unsigned i = 0; i < repetitions; i++) {
        printf("%08u: ", i);
        test_msg_prior_timeout();
        printf("OK\n");
    }

    printf("ALL TESTS SUCCEEDED\n");
    return 0;
}
