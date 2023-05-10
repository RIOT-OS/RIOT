/*
 * Copyright (C) 2015 Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Thread test application
 *
 * @author Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "embUnit/embUnit.h"
#include "embUnit.h"
#include "msg.h"
#include "thread.h"

#define MSG_QUEUE_LENGTH                (8)

static msg_t msg_queue[MSG_QUEUE_LENGTH];

static void test_msg_queue_capacity(void)
{
    TEST_ASSERT_EQUAL_INT(0, msg_queue_capacity(thread_getpid()));
    msg_init_queue(msg_queue, MSG_QUEUE_LENGTH);
    TEST_ASSERT_EQUAL_INT(MSG_QUEUE_LENGTH, msg_queue_capacity(thread_getpid()));
}

static Test *tests_msg_queue_capacity_suite(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_msg_queue_capacity),
    };

    EMB_UNIT_TESTCALLER(tests, NULL, NULL, fixtures);

    return (Test *)&tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_msg_queue_capacity_suite());
    TESTS_END();
    return 0;
}
