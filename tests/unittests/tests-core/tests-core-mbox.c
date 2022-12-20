/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>

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

    for (unsigned i = 0; i < ARRAY_SIZE(queue); i++) {
        msg.type = i;
        msg.content.value = gen_val(i);
        TEST_ASSERT_EQUAL_INT(1, mbox_try_put(&mbox, &msg));
    }

    msg.type = 4242;
    msg.content.value = 4242;
    TEST_ASSERT_EQUAL_INT(0, mbox_try_put(&mbox, &msg));

    for (unsigned i = 0; i < ARRAY_SIZE(queue); i++) {
        TEST_ASSERT_EQUAL_INT(i, queue[i].type);
        TEST_ASSERT_EQUAL_INT(gen_val(i), queue[i].content.value);
    }

    for (unsigned i = 0; i < ARRAY_SIZE(queue); i++) {
        TEST_ASSERT_EQUAL_INT(1, mbox_try_get(&mbox, &msg));
        TEST_ASSERT_EQUAL_INT(i, msg.type);
        TEST_ASSERT_EQUAL_INT(gen_val(i), msg.content.value);
    }

    TEST_ASSERT_EQUAL_INT(0, mbox_try_get(&mbox, &msg));
}

static void test_mbox_try_get_with_type(void)
{
    mbox_t mbox;
    msg_t queue[QUEUE_SIZE];
    const unsigned last_idx = ARRAY_SIZE(queue) - 1;
    msg_t msg = { .type = 0 };
    mbox_init(&mbox, queue, ARRAY_SIZE(queue));
    TEST_ASSERT_EQUAL_INT(0, mbox_avail(&mbox));

    for (unsigned i = 0; i <= last_idx; i++) {
        msg.type = i;
        msg.content.value = gen_val(i);
        TEST_ASSERT_EQUAL_INT(1, mbox_try_put(&mbox, &msg));
    }

    /* try get non-existing item */
    msg.content.value = 0x4242;
    msg.type = 0x4242;
    TEST_ASSERT_EQUAL_INT(0, mbox_try_get_with_type(&mbox, &msg, last_idx + 1));
    /* verify msg is unchanged when get failed */
    TEST_ASSERT_EQUAL_INT(0x4242, msg.content.value);
    TEST_ASSERT_EQUAL_INT(0x4242, msg.type);

    /* get item in the middle */
    TEST_ASSERT_EQUAL_INT(1, mbox_try_get_with_type(&mbox, &msg, 1));
    TEST_ASSERT_EQUAL_INT(1, msg.type);
    TEST_ASSERT_EQUAL_INT(gen_val(1), msg.content.value);
    /* and check that item before and after are still intact */
    TEST_ASSERT_EQUAL_INT(0, queue[1].type);
    TEST_ASSERT_EQUAL_INT(gen_val(0), queue[0].content.value);
    TEST_ASSERT_EQUAL_INT(2, queue[2].type);
    TEST_ASSERT_EQUAL_INT(gen_val(2), queue[2].content.value);

    /* get item in the front */
    TEST_ASSERT_EQUAL_INT(1, mbox_try_get_with_type(&mbox, &msg, 0));
    TEST_ASSERT_EQUAL_INT(0, msg.type);
    TEST_ASSERT_EQUAL_INT(gen_val(0), msg.content.value);
    /* and check that item after is still intact */
    TEST_ASSERT_EQUAL_INT(2, queue[2].type);
    TEST_ASSERT_EQUAL_INT(gen_val(2), queue[2].content.value);

    /* get item in the back */
    TEST_ASSERT_EQUAL_INT(1, mbox_try_get_with_type(&mbox, &msg, last_idx));
    TEST_ASSERT_EQUAL_INT(last_idx, msg.type);
    TEST_ASSERT_EQUAL_INT(gen_val(last_idx), msg.content.value);

    /* and check that front and back of the expected queue are intact */
    TEST_ASSERT_EQUAL_INT(2, queue[3].type);
    TEST_ASSERT_EQUAL_INT(gen_val(2), queue[3].content.value);
    TEST_ASSERT_EQUAL_INT(last_idx - 1, queue[last_idx].type);
    TEST_ASSERT_EQUAL_INT(gen_val(last_idx - 1), queue[last_idx].content.value);

    /* get remaining items, but leave front in place */
    for (unsigned i = 3; i < last_idx; i++) {
        TEST_ASSERT_EQUAL_INT(1, mbox_try_get_with_type(&mbox, &msg, i));
        TEST_ASSERT_EQUAL_INT(i, msg.type);
        TEST_ASSERT_EQUAL_INT(gen_val(i), msg.content.value);
    }

    /* get last remaining item */
    TEST_ASSERT_EQUAL_INT(1, mbox_try_get_with_type(&mbox, &msg, 2));
    TEST_ASSERT_EQUAL_INT(2, msg.type);
    TEST_ASSERT_EQUAL_INT(gen_val(2), msg.content.value);

    /* check that mbox is drained and that no messages can be retrieved */
    TEST_ASSERT_EQUAL_INT(0, mbox_avail(&mbox));
    msg.content.value = 0x4242;
    msg.type = 0x4242;
    TEST_ASSERT_EQUAL_INT(0, mbox_try_get_with_type(&mbox, &msg, 0));
    /* verify msg is unchanged when get failed */
    TEST_ASSERT_EQUAL_INT(0x4242, msg.content.value);
    TEST_ASSERT_EQUAL_INT(0x4242, msg.type);
}

Test *tests_core_mbox_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mbox_put_get),
        new_TestFixture(test_mbox_try_get_with_type),
    };

    EMB_UNIT_TESTCALLER(core_mbox_tests, NULL, NULL, fixtures);

    return (Test *)&core_mbox_tests;
}
