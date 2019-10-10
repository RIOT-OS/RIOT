/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "embUnit.h"
#include "tests-tasklet.h"

#include "tasklet.h"

#define TASKLET_FAIL        (0U)
#define TASKLET_SUCCESS     (1U)
#define TASKLET_SUCCESS_ALT (2U)

static int tasklet_arg = TASKLET_FAIL;
static int tasklet_arg_alt = TASKLET_FAIL;
static tasklet_t tasklet;
static tasklet_t tasklet_alt;
static tasklet_t tasklet_sched;

static void tasklet_func(void *arg)
{
    int *t = arg;
    *t = TASKLET_SUCCESS;
}

static void tasklet_func_alt(void *arg)
{
    int *t = arg;
    *t = TASKLET_SUCCESS_ALT;
}

static void tasklet_func_schedule(void *arg)
{
    (void) arg;
    tasklet_schedule(&tasklet);
}

static void set_up(void)
{
    tasklet_list_reset();
    memset(&tasklet, '\0', sizeof(tasklet_t));
    memset(&tasklet_alt, '\0', sizeof(tasklet_t));
    memset(&tasklet_sched, '\0', sizeof(tasklet_t));

    tasklet_init(&tasklet, tasklet_func, &tasklet_arg);
    tasklet_init(&tasklet_alt, tasklet_func_alt, &tasklet_arg_alt);
    tasklet_init(&tasklet_sched, tasklet_func_schedule, NULL);

    tasklet_arg = TASKLET_FAIL;
    tasklet_arg_alt = TASKLET_FAIL;
}

static void test_tasklet_init(void)
{
    TEST_ASSERT_NULL(tasklet.tasklet_node.next);
    TEST_ASSERT(tasklet.task == tasklet_func);
    TEST_ASSERT(tasklet.arg == &tasklet_arg);
}

static void test_tasklet_schedule(void)
{
    tasklet_schedule(&tasklet);
    TEST_ASSERT(tasklet_arg == TASKLET_SUCCESS);
    TEST_ASSERT_NULL(tasklet.tasklet_node.next);
}

static void test_tasklet_schedule_task_is_removed(void)
{
    tasklet_schedule(&tasklet);
    TEST_ASSERT_NULL(tasklet_pop());
}

static void test_tasklet_add_pop(void)
{
    tasklet_t tasklet = {0};
    TEST_ASSERT_NULL(tasklet_pop());

    tasklet_add(&tasklet);
    TEST_ASSERT(((tasklet_t*) tasklet.tasklet_node.next) == &tasklet);

    tasklet_t *pop = tasklet_pop();

    TEST_ASSERT(pop == &tasklet);
    TEST_ASSERT_NULL(pop->tasklet_node.next);
    TEST_ASSERT_NULL(tasklet_pop());
}

static void test_tasklet_schedule_multiple(void)
{
    tasklet_schedule(&tasklet);
    TEST_ASSERT(tasklet_arg == TASKLET_SUCCESS);
    TEST_ASSERT_NULL(tasklet.tasklet_node.next);

    tasklet_schedule(&tasklet_alt);
    TEST_ASSERT(tasklet_arg_alt == TASKLET_SUCCESS_ALT);
    TEST_ASSERT_NULL(tasklet_alt.tasklet_node.next);
}

static void test_tasklet_schedule_in_tasklet(void)
{
    tasklet_schedule(&tasklet_sched);
    TEST_ASSERT(tasklet_arg == TASKLET_SUCCESS);
}

static void test_tasklet_add_pop_multiple(void)
{
    tasklet_t tasklet = {0};
    tasklet_t tasklet_alt = {0};

    tasklet_add(&tasklet);
    tasklet_add(&tasklet_alt);
    TEST_ASSERT(((tasklet_t*) tasklet.tasklet_node.next) == &tasklet_alt);
    TEST_ASSERT(((tasklet_t*) tasklet_alt.tasklet_node.next) == &tasklet);

    TEST_ASSERT(tasklet_pop() == &tasklet);
    TEST_ASSERT(tasklet_pop() == &tasklet_alt);
    TEST_ASSERT_NULL(tasklet_pop());
    TEST_ASSERT_NULL(tasklet.tasklet_node.next);
    TEST_ASSERT_NULL(tasklet_alt.tasklet_node.next);
}

Test *test_tasklet_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_tasklet_init),
        new_TestFixture(test_tasklet_schedule),
        new_TestFixture(test_tasklet_schedule_task_is_removed),
        new_TestFixture(test_tasklet_add_pop),
        new_TestFixture(test_tasklet_add_pop_multiple),
        new_TestFixture(test_tasklet_schedule_multiple),
        new_TestFixture(test_tasklet_schedule_in_tasklet),
    };

    EMB_UNIT_TESTCALLER(tasklet_tests, set_up, NULL,
                        fixtures);

    return (Test *)&tasklet_tests;
}

void tests_tasklet(void)
{
    TESTS_RUN(test_tasklet_tests());
}
