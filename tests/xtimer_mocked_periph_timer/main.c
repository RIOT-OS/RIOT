/*
 * Copyright (C) 2018 Freie Universität Berlin
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
 * @brief       xtimer mocked periph_timer test application
 *
 * Test xtimer using a mocked implementation of periph_timer
 *
 * @author      Gaëtan Harter <gaetan.harter@fu-berlin.de>
 * @}
 */
#include <stdio.h>

#define ENABLE_DEBUG 0
#include "debug.h"

#include "embUnit.h"

/*
 * Mock 'xtimer' to use our periph_timer mock implementation
 * The '.c' files must be included to have access to private functions and
 * mock with the macros
 */
#define timer_init timer_init_mock
#define timer_read timer_read_mock
#define timer_set_absolute  timer_set_absolute_mock
#include "xtimer.h"
#include "xtimer.c"
#include "xtimer_core.c"

/* Allow configuring the mock through here */
#include "periph_timer_mock.h"


static void test_timer_cb(void *arg)
{
    printf("test_timer_cb(%p)\n", arg);
}



/* This is a magic that should tell that it is the first call in
 * `_xtimer_set`
 */
#define TIMER_SET_TICK_COUNT (1)

/*
 * A magic value that is meant to make it work with the current
 * implementation
 */
#define TIMER_SET_XTIMER_SET_OFFSET (XTIMER_BACKOFF)
#define TIMER_SET_DESCHEDULE_TIME_TICK (TIMER_SET_XTIMER_SET_OFFSET +1)

/* Simulate descheduling the task for XTIMER_BACKOFF after `_xtimer_now` in
 * `_xtimer_set_absolute`
 *
 * Simulating the de-scheduling with this method assumes we are not in masked
 * interrupts, which we currently are.
 * A proper test should just unlock a thread but this is for next step.
 */
static int timer_set_deschedule_triggered = 0;
static void _simulate_deschedule_in_timer_set(uint32_t now)
{
    if (now == TIMER_SET_TICK_COUNT && !timer_set_deschedule_triggered) {
        /* This is countered by being in an interrupt context */
        DEBUG("De-schedule the thread at %" PRIu32 " for %u\n",
               now, TIMER_SET_DESCHEDULE_TIME_TICK);
        timer_set_deschedule_triggered = 1;
        timer_mock_set(now + TIMER_SET_DESCHEDULE_TIME_TICK);
    }

}
static void test_show_xtimer_set_call_target_in_the_past(void)
{
    xtimer_init();
    timer_mock_set_post_read_cb(_simulate_deschedule_in_timer_set);

    xtimer_t timer_low_offset = {.callback = test_timer_cb};

    /* TODO I only handle Freq 1MHz for now in this test */
    TEST_ASSERT_MESSAGE(XTIMER_HZ == 1000000,
                        "Only 1Mhz handled here for this first test");


    xtimer_set(&timer_low_offset, TIMER_SET_XTIMER_SET_OFFSET);

    /* The test should not be in the `long_list_head` as it is close by */
    int was_in_list = _remove_timer_from_list(&long_list_head, &timer_low_offset);
    TEST_ASSERT_MESSAGE(was_in_list == 0,
                       "The timer has been placed in the long_list_head for a small sleep time");
}



/* This is a magic that should tell that it is the first call in
 * `_xtimer_set_absolute`
 */
#define SET_ABSOLUTE_TICK_COUNT (2)

/*
 * A magic value that is meant to make it work with the current
 * implementation
 */
#define SET_ABSOLUTE_XTIMER_SET_OFFSET (XTIMER_BACKOFF)
#define SET_ABSOLUTE_DESCHEDULE_TIME_TICK (SET_ABSOLUTE_XTIMER_SET_OFFSET)

/* Simulate descheduling the task for XTIMER_BACKOFF after `_xtimer_now` in
 * `_xtimer_set_absolute`
 *
 * Simulating the de-scheduling with this method assumes we are not in masked
 * interrupts, which we currently are.
 * A proper test should just unlock a thread but this is for next step.
 */
static int set_absolute_deschedule_triggered = 0;
static int set_absolute_full_loop = 0;
static void _simulate_deschedule_in_set_absolute(uint32_t now)
{
    if (now == SET_ABSOLUTE_TICK_COUNT && !set_absolute_deschedule_triggered) {
        /* This is countered by being in an interrupt context */
        DEBUG("De-schedule the thread at %" PRIu32 " for %u\n",
               now, SET_ABSOLUTE_DESCHEDULE_TIME_TICK);
        set_absolute_deschedule_triggered = 1;
        timer_mock_set(now + SET_ABSOLUTE_DESCHEDULE_TIME_TICK);
    }

    if (now == 200) {
        puts("Stuck in xtimer_spin_until for a full loop, jump out");
        set_absolute_full_loop = 1;
        /* Step a lot in the future */
        timer_mock_set((((uint64_t)1) << XTIMER_WIDTH) - 2);
    }
}



/* This test adds delay after the _xtimer_now() in `_xtimer_set_absolute` as if
 * it was de-scheduled enough time to make `target` in the past.
 *
 * This makes `xtimer_spin_until` be called with a target already elapsed.
 */
static void test_show_xtimer_set_does_not_handle_overhead(void)
{
    xtimer_init();
    timer_mock_set_post_read_cb(_simulate_deschedule_in_set_absolute);

    xtimer_t timer_low_offset = {.callback = test_timer_cb};

    /* TODO I only handle Freq 1MHz for now in this test */
    TEST_ASSERT_MESSAGE(XTIMER_HZ == 1000000,
                        "Only 1Mhz handled here for this first test");


    xtimer_set(&timer_low_offset, SET_ABSOLUTE_XTIMER_SET_OFFSET);
    DEBUG("Outside of xtimer_set\n");
    TEST_ASSERT(!set_absolute_full_loop);
}



static TestRef tests_xtimer_low_level(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_show_xtimer_set_call_target_in_the_past),
        new_TestFixture(test_show_xtimer_set_does_not_handle_overhead),

    };
    EMB_UNIT_TESTCALLER(xtimer_low_level, NULL, NULL, fixtures);

    return (Test *)&xtimer_low_level;
}

int main(void)
{
    printf("XTIMER_WIDTH: %u\n", XTIMER_WIDTH);
    printf("XTIMER_HZ: %lu\n", XTIMER_HZ);
    printf("XTIMER_BACKOFF: %u\n", XTIMER_BACKOFF);

    TESTS_START();
    TESTS_RUN(tests_xtimer_low_level());
    TESTS_END();
    return 0;
}
