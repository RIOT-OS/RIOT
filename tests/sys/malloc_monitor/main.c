/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for MODULE_MALLOC_MONITOR
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "embUnit.h"

#include "malloc_monitor.h"

#define MALLOC_SIZE 1

#define TEST_MALLOC_MONITOR_SAVE \
    size_t curr = malloc_monitor_get_usage_current(); \
    size_t water = malloc_monitor_get_usage_high_watermark();

#define TEST_ASSERT_CURRENT(num_alloc) TEST_ASSERT_EQUAL_INT(curr+num_alloc*MALLOC_SIZE, malloc_monitor_get_usage_current());
#define TEST_ASSERT_WATERMARK(num_alloc) TEST_ASSERT_EQUAL_INT(water+num_alloc*MALLOC_SIZE, malloc_monitor_get_usage_high_watermark());

/*
 * malloc and free should be reflected by `malloc_monitor_get_usage_current()`.
 * `malloc_monitor_get_usage_high_watermark()` should only be decreased on a call to
 * `malloc_monitor_reset_high_watermark()`
 */
static void test_malloc_free(void)
{
    TEST_MALLOC_MONITOR_SAVE

    TEST_ASSERT_CURRENT(0);
    TEST_ASSERT_WATERMARK(0);

    void *volatile alloc1 = malloc(MALLOC_SIZE);
    TEST_ASSERT_NOT_NULL(alloc1);

    TEST_ASSERT_CURRENT(1);
    TEST_ASSERT_WATERMARK(1);

    void *volatile alloc2 = malloc(MALLOC_SIZE);
    TEST_ASSERT_NOT_NULL(alloc2);

    TEST_ASSERT_CURRENT(2);
    TEST_ASSERT_WATERMARK(2);

    free(alloc1);

    TEST_ASSERT_CURRENT(1);
    TEST_ASSERT_WATERMARK(2);

    malloc_monitor_reset_high_watermark();

    TEST_ASSERT_CURRENT(1);
    TEST_ASSERT_WATERMARK(1);

    free(alloc2);

    TEST_ASSERT_CURRENT(0);
    TEST_ASSERT_WATERMARK(1);

    malloc_monitor_reset_high_watermark();

    TEST_ASSERT_CURRENT(0);
    TEST_ASSERT_WATERMARK(0);
}
/*
 * using calloc instead of malloc should be reflected correctly
 */
static void test_calloc(void)
{
    TEST_MALLOC_MONITOR_SAVE

    TEST_ASSERT_CURRENT(0);
    TEST_ASSERT_WATERMARK(0);

    void *volatile alloc1 = calloc(1, MALLOC_SIZE);
    TEST_ASSERT_NOT_NULL(alloc1);

    TEST_ASSERT_CURRENT(1);
    TEST_ASSERT_WATERMARK(1);

    void *volatile alloc2 = calloc(2, MALLOC_SIZE);
    TEST_ASSERT_NOT_NULL(alloc2);

    TEST_ASSERT_CURRENT(3);
    TEST_ASSERT_WATERMARK(3);

    free(alloc1);

    TEST_ASSERT_CURRENT(2);
    TEST_ASSERT_WATERMARK(3);

    malloc_monitor_reset_high_watermark();

    TEST_ASSERT_CURRENT(2);
    TEST_ASSERT_WATERMARK(2);

    free(alloc2);

    TEST_ASSERT_CURRENT(0);
    TEST_ASSERT_WATERMARK(2);

    malloc_monitor_reset_high_watermark();

    TEST_ASSERT_CURRENT(0);
    TEST_ASSERT_WATERMARK(0);
}

/*
 * using realloc instead of malloc/free should be reflected correctly
 */
static void test_realloc(void)
{
    TEST_MALLOC_MONITOR_SAVE

    TEST_ASSERT_CURRENT(0);
    TEST_ASSERT_WATERMARK(0);

    void *volatile alloc1 = realloc(NULL, MALLOC_SIZE);
    TEST_ASSERT_NOT_NULL(alloc1);

    TEST_ASSERT_CURRENT(1);
    TEST_ASSERT_WATERMARK(1);

    alloc1 = realloc(alloc1, 2*MALLOC_SIZE);
    TEST_ASSERT_NOT_NULL(alloc1);

    TEST_ASSERT_CURRENT(2);
    TEST_ASSERT_WATERMARK(2);

    alloc1 = realloc(alloc1, 1*MALLOC_SIZE);

    TEST_ASSERT_CURRENT(1);
    TEST_ASSERT_WATERMARK(2);

    malloc_monitor_reset_high_watermark();

    TEST_ASSERT_CURRENT(1);
    TEST_ASSERT_WATERMARK(1);

    alloc1 = realloc(alloc1, 0*MALLOC_SIZE);

    TEST_ASSERT_CURRENT(0);
    TEST_ASSERT_WATERMARK(1);

    malloc_monitor_reset_high_watermark();

    TEST_ASSERT_CURRENT(0);
    TEST_ASSERT_WATERMARK(0);
}

/*
 * freeing NULL shouldn't change anything
 */
static void test_free_NULL(void)
{
    TEST_MALLOC_MONITOR_SAVE

    free(NULL);

    TEST_ASSERT_CURRENT(0);
    TEST_ASSERT_WATERMARK(0);

    void *volatile alloc1 = malloc(MALLOC_SIZE);
    TEST_ASSERT_NOT_NULL(alloc1);

    TEST_ASSERT_CURRENT(1);
    TEST_ASSERT_WATERMARK(1);

    free(alloc1);

    TEST_ASSERT_CURRENT(0);
    TEST_ASSERT_WATERMARK(1);
}

static Test *tests_malloc_monitor(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_malloc_free),
        new_TestFixture(test_calloc),
        new_TestFixture(test_realloc),
        new_TestFixture(test_free_NULL),
    };

    EMB_UNIT_TESTCALLER(tests, NULL, NULL, fixtures);
    return (Test *)&tests;
}

int main(void)
{
    puts("malloc_monitor test");
    TESTS_START();
    TESTS_RUN(tests_malloc_monitor());
    TESTS_END();

    return 0;
}
