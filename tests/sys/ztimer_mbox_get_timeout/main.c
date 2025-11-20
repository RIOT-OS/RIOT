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
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 */

#include <errno.h>
#include <stdio.h>

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
    expect(mbox_try_put(&mbox, arg) == 1);
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
    expect(ztimer_mbox_get_timeout(ZTIMER_USEC, &mbox, &msg, timeout_us) == -ETIMEDOUT);
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
#if defined(CPU_NATIVE)
    /* relax timing on native, as background load can mess with timing */
    const uint32_t wait_timeout_us = 1000 * US_PER_MS;
#else
    const uint32_t wait_timeout_us = 2 * US_PER_MS;
#endif

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

    /* the function should return BEFORE the timeout was triggered */
    expect(stop - start < wait_timeout_us);

#if !defined(CPU_NATIVE)
    /* The function should return AFTER the message was send.
     * This test is flaky on native, at least with LLVM. */
    expect(stop - start >= msg_timeout_us);
#endif

    /* we should have gotten the correct message */
    expect((got.type == msg.type) && (got.content.value == msg.content.value));
}

static WORD_ALIGNED char stack_high_prio_thread[THREAD_STACKSIZE_TINY];
static void * high_prio_thread(void *arg)
{
    uint32_t timeout = (uintptr_t)arg;

    {
        msg_t msg = { .type = MSG_TYPE, .content.value = MSG_VAL };
        mbox_put(&mbox, &msg);
    }

    ztimer_spin(ZTIMER_USEC, timeout);
    return NULL;
}

static void test_msg_race(void)
{
    printf("testing timeout is reached despite message received (race): ");
    const uint32_t wait_timeout_us = 2 * US_PER_MS;
    const uintptr_t spin_timeout_us = 2 * wait_timeout_us;

    msg_t msg = { .type = MSG_TYPE, .content.value = MSG_VAL };
    msg_t got = { .type = 0 };

    thread_create(stack_high_prio_thread, sizeof(stack_high_prio_thread),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_WOUT_YIELD,
                  high_prio_thread, (void *)spin_timeout_us, "high_prio");
    uint32_t start = ztimer_now(ZTIMER_USEC);
    expect(ztimer_mbox_get_timeout(ZTIMER_USEC, &mbox, &got, wait_timeout_us) == 0);
    uint32_t stop = ztimer_now(ZTIMER_USEC);

    /* the high prio thread should prevent us from running while it spins. It
     * will even prevent us from cancelling the timeout. We should still receive
     * the message (which was received prior to the timeout), even though
     * we get CPU time only way after the timeout. */
    expect(stop - start > wait_timeout_us);

    /* we should have gotten the correct message */
    expect((got.type == msg.type) && (got.content.value == msg.content.value));
    printf("OK\n");
}

int main(void)
{
    const unsigned repetitions = 1000;
    printf("Testing ztimer_mbox_get_timeout()\n"
           "=================================\n");
    test_mbox_already_full();
    test_timeout_reached();
    test_msg_race();

    printf("Running test for reception prior timeout %u times: ", repetitions);
    for (unsigned i = 0; i < repetitions; i++) {
        test_msg_prior_timeout();
    }

    printf("OK\n");

    printf("\nALL TESTS SUCCEEDED\n");
    return 0;
}
