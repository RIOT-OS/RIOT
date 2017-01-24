/*
 * Copyright (C) 2016 Freie Universität Berlin
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

#include <errno.h>
#include <stdint.h>

#include "cpu.h"
#include "embUnit/embUnit.h"
#include "tests-flashpage.h"

/* need to define these values before including the header */
#ifndef FLASHPAGE_SIZE
#define FLASHPAGE_SIZE      512
#endif
#ifndef FLASHPAGE_NUMOF
#define FLASHPAGE_NUMOF     128
#endif

#include "periph/flashpage.h"

static void test_flashbase_addr(void)
{
    void *addr;

    addr = flashpage_addr(0);
    TEST_ASSERT_EQUAL_INT((int)CPU_FLASH_BASE, (int)addr);
    addr = flashpage_addr(FLASHPAGE_NUMOF - 1);
    TEST_ASSERT_EQUAL_INT((long)CPU_FLASH_BASE + (((unsigned)FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE), (int)addr);
    addr = flashpage_addr(12);
    TEST_ASSERT_EQUAL_INT((int)CPU_FLASH_BASE + (12 * FLASHPAGE_SIZE), (int)addr);
}

static void test_flashbase_page(void)
{
    int page;

    page = flashpage_page((void *)CPU_FLASH_BASE);
    TEST_ASSERT_EQUAL_INT(0, page);
    page = flashpage_page((void *)(CPU_FLASH_BASE + 1));
    TEST_ASSERT_EQUAL_INT(0, page);
    page = flashpage_page((void *)(CPU_FLASH_BASE + FLASHPAGE_SIZE));
    TEST_ASSERT_EQUAL_INT(1, page);
    page = flashpage_page((void *)(CPU_FLASH_BASE + FLASHPAGE_SIZE + 1));
    TEST_ASSERT_EQUAL_INT(1, page);
    page = flashpage_page((void *)(CPU_FLASH_BASE + ((unsigned)FLASHPAGE_SIZE * (FLASHPAGE_NUMOF - 1))));
    TEST_ASSERT_EQUAL_INT(FLASHPAGE_NUMOF - 1, page);
}

Test *tests_flashpage_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_flashbase_addr),
        new_TestFixture(test_flashbase_page)
    };

    EMB_UNIT_TESTCALLER(flashbase_tests, NULL, NULL, fixtures);

    return (Test *)&flashbase_tests;
}

void tests_flashpage(void)
{
    TESTS_RUN(tests_flashpage_tests());
}
/** @} */
