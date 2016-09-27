/*
 * Copyright (C) 2015 Lucas Jenß
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
 * @brief       Tests the MCI interface
 *
 * @author      Lucas Jenß <lucas@x3ro.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <diskio.h>
#include <stdbool.h>
#include <math.h>

#include "embUnit.h"
#include "xtimer.h"
#include "lpm.h"

#define SECTOR_SIZE 512

unsigned char teststring1[SECTOR_SIZE + 1];
unsigned char teststring2[SECTOR_SIZE + 1];
unsigned char read_buffer[SECTOR_SIZE + 1];

void _zero_end_strings(void) {
    teststring1[SECTOR_SIZE] = '\0';
    teststring2[SECTOR_SIZE] = '\0';
    read_buffer[SECTOR_SIZE] = '\0';
}

void fill_test_strings(void) {
    for(int i=0; i<SECTOR_SIZE; i++) {
        sprintf((char*)teststring1 + i, "%d", i%10);
    }
    for(int i=0; i<SECTOR_SIZE; i++) {
        sprintf((char*)teststring2 + i, "%d", i%2);
    }

    teststring1[SECTOR_SIZE] = teststring2[SECTOR_SIZE] = '\0';
}

// Probably the worst way to convert a double to string, but some
// platforms don't have that (*cough* msba2 *cough*)
void sprint_double(char *buffer, double x, int precision) {
    if(precision > 10) {
        printf("Precision > 10 not supported");
        precision = 10;
    }

    long integral_part = (long) x;

    char buf[11];
    int i = 0;
    while(precision > 0) {
        x *= 10;
        short current_decimal = (short) x;
        sprintf(buf+i, "%d", current_decimal);
        i++;
        precision--;
    }

    buf[i] = '\0';
    sprintf(buffer, "%ld.%s", integral_part, buf);
}

static void write_test_sector(int sector_number) {
    diskio_sta_t status;
    printf("About to write the following to sector %d:\n%s\n", sector_number, teststring2);
    status = mci_write(teststring2, sector_number, 1);
    printf("MCI write status: 0x%hhx\n", status);
    TEST_ASSERT_EQUAL_INT(0, status);
}

static void read_test_sector(int sector_number, bool test_empty) {
    diskio_sta_t status;
    printf("About to read from SDCard sector %d\n", sector_number);
    status = mci_read(read_buffer, sector_number, 1);
    _zero_end_strings();
    printf("MCI read status: 0x%hhx\n", status);
    printf("Read the following:\n'%s'\n", read_buffer);
    TEST_ASSERT_EQUAL_INT(0, status);
    _zero_end_strings();

    if(test_empty) {
        TEST_ASSERT(strcmp((char*) read_buffer, (char*)teststring2) != 0);
    } else {
        TEST_ASSERT(strcmp((char*) read_buffer, (char*)teststring2) == 0);
    }
}

static void erase_block(int block_number) {
    diskio_sta_t status;
    printf("About to erase block %d\n", block_number);
    unsigned long params[2];
    params[0] = 1024*block_number; // start
    params[1] = 1024*(block_number+1)-1; // end
    status = mci_ioctl(CTRL_ERASE_SECTOR, &params);
    TEST_ASSERT_EQUAL_INT(0, status);
    printf("MCI read status: 0x%hhx\n", status);
}


static void main_test(void)
{
    diskio_sta_t status = mci_initialize();
    printf("MCI status: 0x%hhx\n", status);

    if(status == DISKIO_STA_NOINIT) {
        printf("Could not initialize MCI interface :(\n");
    } else if(status == DISKIO_STA_NODISK) {
        printf("NO SDCard detected. Aborting\n");
    } else if(status == DISKIO_STA_PROTECT) {
        printf("SDCard is in read-only mode\n");
    }

    TEST_ASSERT_EQUAL_INT(0, status);

    unsigned long sector_count = 0;
    mci_ioctl(GET_SECTOR_COUNT, &sector_count);
    printf("sector_count: %lu\n", sector_count);

    unsigned short sector_size = 0;
    mci_ioctl(GET_SECTOR_SIZE, &sector_size);
    printf("sector_size: %hu\n", sector_size);

    unsigned long block_size = 0;
    mci_ioctl(GET_BLOCK_SIZE, &block_size);
    printf("block_size: %lu\n", block_size);

    double capacity = (1.0 * sector_size * sector_count) / (1024 * 1024 * 1024);
    char capacity_buffer[8];
    sprint_double(capacity_buffer, capacity, 2);
    printf("SDcard capacity: %sGiB\n", capacity_buffer);

#ifdef BOARD_NATIVE
    // Test virtual MCI parameters
    TEST_ASSERT_EQUAL_INT(16384, sector_count);
    TEST_ASSERT_EQUAL_INT(512, sector_size);
    TEST_ASSERT_EQUAL_INT(524288, block_size);
#endif

    /*
     * Perform a read-write check on the SDCard
     */

    if(sector_size > SECTOR_SIZE) {
        printf("Does not support a sector size larger than %d, write test aborted.\n", SECTOR_SIZE);
        return;
    }

    erase_block(0);

    char sprint_buffer[16];
    timex_t then;
    timex_t now;
    timex_t elapsed;

    printf("write\n");
    for(int i=0; i<10; i++) {
        xtimer_now_timex(&then);
        status = mci_write(teststring2, i, 1);
        TEST_ASSERT_EQUAL_INT(0, status);
        xtimer_now_timex(&now);
        elapsed = timex_sub(now, then);
        timex_to_str(elapsed, sprint_buffer);
        printf("%s, \n", sprint_buffer);
    }

    printf("read\n");
    for(int i=0; i<10; i++) {
        xtimer_now_timex(&then);
        status = mci_read(teststring2, i, 1);
        TEST_ASSERT_EQUAL_INT(0, status);
        xtimer_now_timex(&now);
        elapsed = timex_sub(now, then);
        timex_to_str(elapsed, sprint_buffer);
        printf("%s, \n", sprint_buffer);
    }

    erase_block(0);

    unsigned char op_sector_count = 1; // Read/write single sector

    printf("About to write the following sector 0:\n%s\n", teststring1);
    status = mci_write(teststring1, 0, op_sector_count);
    printf("MCI write status: 0x%hhx\n", status);
    TEST_ASSERT_EQUAL_INT(0, status);

    write_test_sector(1);

    printf("About to read from SDCard sector 0\n");
    status = mci_read(read_buffer, 0, op_sector_count);
    _zero_end_strings();
    printf("MCI read status: 0x%hhx\n", status);
    printf("Read the following:\n'%s'\n", read_buffer);
    TEST_ASSERT_EQUAL_INT(0, status);
    _zero_end_strings();
    TEST_ASSERT(strcmp((char*) read_buffer, (char*)teststring1) == 0);


    read_test_sector(1, false);
    erase_block(0);
    read_test_sector(1, true);

    write_test_sector(1023);
    read_test_sector(1023, false);

    write_test_sector(1024);
    read_test_sector(1024, false);

    write_test_sector(2047);
    read_test_sector(2047, false);

    write_test_sector(2048);
    read_test_sector(2048, false);

    erase_block(1);

    read_test_sector(1023, false);
    read_test_sector(1024, true);
    read_test_sector(2047, true);
    read_test_sector(2048, false);
}


Test *tests(void) {
    fill_test_strings();

    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(main_test),
    };

    EMB_UNIT_TESTCALLER(results, NULL, NULL, fixtures);
    return (Test *)&results;
}


int main(void)
{
#ifdef OUTPUT
    TextUIRunner_setOutputter(OUTPUTTER);
#endif

    TESTS_START();
    TESTS_RUN(tests());
    TESTS_END();

    puts("xtimer_wait()");
    xtimer_usleep(100000);

    lpm_set(LPM_POWERDOWN);
    return 0;
}
