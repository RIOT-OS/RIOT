/*
 * Copyright (C) Lucas Jenß
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
 * @brief       Test application for flash memory simulation
 *
 * @author      Lucas Jenß <lucas@x3ro.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "embUnit.h"
#include "xtimer.h"
#include "lpm.h"
#include "storage/flash_sim.h"

flash_sim fs;
int ret;
char read_buffer[512];
char write_buffer[512];
char expect_buffer[512];

void _reset(void) {
    memset(read_buffer, 0x0, 512);
    memset(write_buffer, 0x0, 512);
    memset(expect_buffer, 0x0, 512);
}

static void test_uninitialized(void) {
    ret = flash_sim_format(&fs);
    TEST_ASSERT_EQUAL_INT(-ENODEV, ret);

    ret = flash_sim_read(&fs, read_buffer, 0);
    TEST_ASSERT_EQUAL_INT(-ENODEV, ret);

    ret = flash_sim_write(&fs, write_buffer, 0);
    TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

static void test_init(void)
{
    _reset();

    ret = flash_sim_init(&fs);
    TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

    fs.page_size = 512;
    fs.block_size = 1024*512; // 512 KiB
    fs.storage_size = 1024*1024*64; // 64MiB

    ret = flash_sim_init(&fs);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(131072, fs.pages);
}




static void test_format(void) {
    ret = flash_sim_format(&fs);
    TEST_ASSERT_EQUAL_INT(0, ret);
}



static void test_basic_read_write(void) {
    _reset();

    ret = flash_sim_read(&fs, read_buffer, 0);
    TEST_ASSERT_EQUAL_INT(0, ret);
    memset(expect_buffer, 0xFF, 512);
    TEST_ASSERT_EQUAL_INT(0, strncmp(read_buffer, expect_buffer, 512));

    memset(write_buffer, 0xAB, 512);
    ret = flash_sim_write(&fs, write_buffer, 0);
    TEST_ASSERT_EQUAL_INT(0, ret);

    _reset();
    memset(expect_buffer, 0xAB, 512);
    ret = flash_sim_read(&fs, read_buffer, 0);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, strncmp(read_buffer, expect_buffer, 512));
}


static void test_erase(void) {
    _reset();

    ret = flash_sim_erase(&fs, 0);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = flash_sim_read(&fs, read_buffer, 0);
    TEST_ASSERT_EQUAL_INT(0, ret);
    memset(expect_buffer, 0xFF, 512);
    TEST_ASSERT_EQUAL_INT(0, strncmp(read_buffer, expect_buffer, 512));
}


static void test_multiple_writes(void) {
    _reset();

    memset(write_buffer, 0xAB, 512);
    ret = flash_sim_write(&fs, write_buffer, 0);
    TEST_ASSERT_EQUAL_INT(0, ret);

    // Issuing the same write twice is valid because no bits in the page are changed
    ret = flash_sim_write(&fs, write_buffer, 0);
    TEST_ASSERT_EQUAL_INT(0, ret);

    // Setting any bits back from 0 -> 1 is invalid, though
    memset(write_buffer, 0xFF, 1);
    ret = flash_sim_write(&fs, write_buffer, 0);
    TEST_ASSERT_EQUAL_INT(-EIO, ret);
}


static void test_multiple_writes_to_zero(void) {
    _reset();

    memset(write_buffer, 0xAB, 512);
    memset(expect_buffer, 0xAB, 512);
    ret = flash_sim_write(&fs, write_buffer, 0);
    TEST_ASSERT_EQUAL_INT(0, ret);

    // Settings bit from 1 -> 0 is valid even if page is not erased first
    write_buffer[0] = 0x00;
    ret = flash_sim_write(&fs, write_buffer, 0);
    TEST_ASSERT_EQUAL_INT(0, ret);

    expect_buffer[0] = 0x00;
    ret = flash_sim_read(&fs, read_buffer, 0);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, strncmp(read_buffer, expect_buffer, 512));
}

static void test_partial_read_write(void) {
    _reset();

    uint64_t write_test = 0x1234ABCD1234ABCD;
    uint64_t read_test = 0;

    uint32_t page = 24;
    flash_sim_erase(&fs, 0);

    // offset=8, length=8
    ret = flash_sim_write_partial(&fs, (char*)&write_test, page, 8, 8);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = flash_sim_read_partial(&fs, (char*)&read_test, page, 8, 8);
    TEST_ASSERT_EQUAL_INT(0, ret);

    printf("%llu <- read | write -> %llu\n", read_test, write_test);
    TEST_ASSERT(read_test == 0x1234ABCD1234ABCD);

    memset(expect_buffer, 0xFF, 512);
    *((uint64_t*)expect_buffer+1) = 0x1234ABCD1234ABCD;
    ret = flash_sim_read(&fs, read_buffer, page);
    TEST_ASSERT_EQUAL_INT(0, strncmp(read_buffer, expect_buffer, 512));
}


static void test_erroneous_reads(void) {
    ret = flash_sim_read(&fs, read_buffer, 999999);
    TEST_ASSERT_EQUAL_INT(-EFAULT, ret);
}

static void test_erroneous_writes(void) {
    ret = flash_sim_write(&fs, write_buffer, 999999);
    TEST_ASSERT_EQUAL_INT(-EFAULT, ret);
}


Test *testsrunner(void) {
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_uninitialized),
        new_TestFixture(test_init),
        new_TestFixture(test_format),
        new_TestFixture(test_basic_read_write),
        new_TestFixture(test_erase),
        new_TestFixture(test_multiple_writes),
        new_TestFixture(test_multiple_writes_to_zero),
        new_TestFixture(test_partial_read_write),
        new_TestFixture(test_erroneous_reads),
        new_TestFixture(test_erroneous_writes),
    };

    EMB_UNIT_TESTCALLER(tests, NULL, NULL, fixtures);
    return (Test *)&tests;
}

int main(void)
{
#ifdef OUTPUT
    TextUIRunner_setOutputter(OUTPUTTER);
#endif

    TESTS_START();
    TESTS_RUN(testsrunner());
    TESTS_END();

    puts("xtimer_wait()");
    xtimer_usleep(100000);

    lpm_set(LPM_POWERDOWN);
    return 0;
}
