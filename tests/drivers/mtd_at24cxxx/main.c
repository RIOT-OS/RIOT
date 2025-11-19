/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <string.h>
#include <errno.h>

#include "embUnit.h"

#include "mtd_at24cxxx.h"
#include "at24cxxx_params.h"

#define MTD_DEV             (&(_mtd.base))
#define TEST_ADDRESS        (uint16_t)((MTD_DEV->sector_count - 1) * \
                            MTD_DEV->page_size)

static at24cxxx_t _at24cxxx;
static mtd_at24cxxx_t _mtd = MTD_AT24CXXX_INIT(&_at24cxxx,
                                               &at24cxxx_params[0]);

static void setup(void)
{
    int ret = mtd_init(MTD_DEV);
    TEST_ASSERT_EQUAL_INT(0, ret);
    mtd_erase(MTD_DEV, TEST_ADDRESS,
              MTD_DEV->pages_per_sector * MTD_DEV->page_size);
}

static void teardown(void)
{
    mtd_erase(MTD_DEV, TEST_ADDRESS,
              MTD_DEV->pages_per_sector * MTD_DEV->page_size);
}

static void test_mtd_init(void)
{
    int ret = mtd_init(MTD_DEV);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_erase(void)
{
    int ret = mtd_erase(MTD_DEV, TEST_ADDRESS, MTD_DEV->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_write_erase(void)
{
    uint8_t buf_empty[] = {0, 0, 0};
    const char buf[] = "MTD_AT24CXXX_TEST_WRITE_ERASE";

    char buf_read[sizeof(buf) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    int ret = mtd_write(MTD_DEV, buf, TEST_ADDRESS, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = mtd_erase(MTD_DEV, TEST_ADDRESS,
                    MTD_DEV->pages_per_sector * MTD_DEV->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    uint8_t expected[sizeof(buf_read)];
    memset(expected, 0, sizeof(expected));
    ret = mtd_read(MTD_DEV, buf_read, TEST_ADDRESS, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(expected, buf_read, sizeof(buf_read)));
}

static void test_mtd_write_read(void)
{
    uint8_t buf_empty[] = {0, 0, 0};
    const char buf[] = "MTD_AT24CXXX_TEST_WRITE_READ";

    char buf_read[sizeof(buf) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    /* Basic write / read */
    int ret = mtd_write(MTD_DEV, buf, TEST_ADDRESS, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = mtd_read(MTD_DEV, buf_read, TEST_ADDRESS, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read + sizeof(buf),
                          sizeof(buf_empty)));

    ret = mtd_erase(MTD_DEV, TEST_ADDRESS,
                    MTD_DEV->pages_per_sector * MTD_DEV->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

Test *tests_mtd_at24cxxx_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mtd_init),
        new_TestFixture(test_mtd_erase),
        new_TestFixture(test_mtd_write_erase),
        new_TestFixture(test_mtd_write_read),
    };

    EMB_UNIT_TESTCALLER(mtd_at24cxxx_tests, setup, teardown, fixtures);

    return (Test *)&mtd_at24cxxx_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_mtd_at24cxxx_tests());
    TESTS_END();
    return 0;
}
/** @} */
