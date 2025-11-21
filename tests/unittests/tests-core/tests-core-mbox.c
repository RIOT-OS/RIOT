/*
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <limits.h>

#include "container.h"

#include "embUnit.h"

#include "mbox.h"

#include "tests-core.h"

#define QUEUE_SIZE  8

static unsigned gen_val(unsigned i) {
    return i + 1337;
}

static void test_mbox_put_get(void)
{
    mbox_t mbox;
    msg_t queue[QUEUE_SIZE];
    msg_t msg = { .type = 0 };
    mbox_init(&mbox, queue, ARRAY_SIZE(queue));
    TEST_ASSERT_EQUAL_INT(0, mbox_avail(&mbox));

    /* Filling the queue up to capacity one item at a time. This should
     * succeed every single time. */
    for (unsigned i = 0; i < ARRAY_SIZE(queue); i++) {
        msg.type = i;
        msg.content.value = gen_val(i);
        TEST_ASSERT_EQUAL_INT(1, mbox_try_put(&mbox, &msg));
    }

    /* Adding one item over capacity must fail. */
    msg.type = 4242;
    msg.content.value = 4242;
    TEST_ASSERT_EQUAL_INT(0, mbox_try_put(&mbox, &msg));

    /* The queue must contain the items we filled in and in that order. */
    for (unsigned i = 0; i < ARRAY_SIZE(queue); i++) {
        TEST_ASSERT_EQUAL_INT(i, queue[i].type);
        TEST_ASSERT_EQUAL_INT(gen_val(i), queue[i].content.value);
    }

    /* Now we drain the queue one item at a time. We expect to get the exact
     * items we filled in and in that order. */
    for (unsigned i = 0; i < ARRAY_SIZE(queue); i++) {
        TEST_ASSERT_EQUAL_INT(1, mbox_try_get(&mbox, &msg));
        TEST_ASSERT_EQUAL_INT(i, msg.type);
        TEST_ASSERT_EQUAL_INT(gen_val(i), msg.content.value);
    }

    /* The queue is now empty. Getting one more item (non-blocking) must fail */
    TEST_ASSERT_EQUAL_INT(0, mbox_try_get(&mbox, &msg));
}

Test *tests_core_mbox_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mbox_put_get),
    };

    EMB_UNIT_TESTCALLER(core_mbox_tests, NULL, NULL, fixtures);

    return (Test *)&core_mbox_tests;
}
