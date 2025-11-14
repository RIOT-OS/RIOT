/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
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
 * @brief    AT24CXXX test application
 *
 * Tested with bluepill and AT24C256
 *
 * @}
 */

#include <inttypes.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "architecture.h"
#include "at24cxxx.h"
#include "at24cxxx_params.h"

#ifndef AT24CXXX_ERASE
#define AT24CXXX_ERASE          (0)
#endif

#define WRITE_BYTE_POSITION     (12U)
#define WRITE_BYTE_CHARACTER    'A'

#define WRITE_POSITION          (AT24CXXX_EEPROM_SIZE - \
                                3 * AT24CXXX_PAGE_SIZE - 4)
#define WRITE_CHARACTERS        { 'B', 'E', 'E', 'R', '4', \
                                'F', 'R', 'E', 'E', '\0' }

#define SET_POSITION            (AT24CXXX_EEPROM_SIZE - \
                                7 * AT24CXXX_PAGE_SIZE - 4)
#define SET_CHARACTER           'G'
#define SET_LEN                 (20U)

int main(void)
{
    puts("Starting tests for module at24cxxx");

    at24cxxx_t at24cxxx_dev;
    int check;

    printf("EEPROM size: %u byte\n", AT24CXXX_EEPROM_SIZE);
    printf("Page size  : %u byte\n", AT24CXXX_PAGE_SIZE);

    /* Test: Init */
    check = at24cxxx_init(&at24cxxx_dev, &at24cxxx_params[0]);
    if (check != AT24CXXX_OK) {
        printf("[FAILURE] at24cxxx_init: (%d)\n", check);
        return 1;
    }
    else {
        puts("[SUCCESS] at24cxxx_init");
    }

    /* erase EEPROM to exclude side effects from prior test runs */
#if AT24CXXX_ERASE
    check = at24cxxx_erase(&at24cxxx_dev);
    if (check != AT24CXXX_OK) {
        printf("[FAILURE] at24cxxx_erase: %d (EEPROM size = %" PRId32 ")\n",
               check, at24cxxx_dev.params.eeprom_size);
        return 1;
    }
    else {
        puts("[SUCCESS] at24cxxx_erase");
    }
#endif

    /* Test: Write/Read Byte */
    check = at24cxxx_write_byte(&at24cxxx_dev, WRITE_BYTE_POSITION,
                                WRITE_BYTE_CHARACTER);
    if (check != AT24CXXX_OK) {
        printf("[FAILURE] at24cxxx_write_byte: %d\n", check);
        return 1;
    }
    else {
        puts("[SUCCESS] at24cxxx_write_byte");
    }

    uint8_t c;
    check = at24cxxx_read_byte(&at24cxxx_dev, WRITE_BYTE_POSITION, &c);
    if (check < 0) {
        printf("[FAILURE] at24cxxx_read_byte: %d\n", check);
        return 1;
    }
    else {
        puts("[SUCCESS] at24cxxx_read_byte");
    }

    if (c != WRITE_BYTE_CHARACTER) {
        printf("[FAILURE] write_byte/read_byte: (%" PRId8 " != %d)\n", c,
               WRITE_BYTE_CHARACTER);
        return 1;
    }
    else {
        puts("[SUCCESS] write_byte/read_byte");
    }

    /* Test: Write */
    uint8_t expected_write_data[] = WRITE_CHARACTERS;

    check = at24cxxx_write(&at24cxxx_dev, WRITE_POSITION, expected_write_data,
                         sizeof(expected_write_data));
    if (check != AT24CXXX_OK) {
        printf("[FAILURE] at24cxxx_write: %d (size = %" PRIuSIZE ")\n", check,
               sizeof(expected_write_data));
        return 1;
    }
    else {
        puts("[SUCCESS] at24cxxx_write");
    }

    /* Test: Read */
    uint8_t actual_write_data[sizeof(expected_write_data)];

    check = at24cxxx_read(&at24cxxx_dev, WRITE_POSITION, actual_write_data,
                        sizeof(actual_write_data));
    if (check != AT24CXXX_OK) {
        printf("[FAILURE] at24cxxx_read: %d\n", check);
        return 1;
    }
    else {
        puts("[SUCCESS] at24cxxx_read");
    }

    if (memcmp(actual_write_data, expected_write_data,
               sizeof(actual_write_data)) != 0) {
        printf("[FAILURE] write/read: (%s != %s)\n", actual_write_data,
               expected_write_data);
        return 1;
    }
    else {
        puts("[SUCCESS] write/read");
    }

    /* Test: Set */
    uint8_t expected_set_data[SET_LEN];
    memset(expected_set_data, SET_CHARACTER, SET_LEN);

    uint8_t actual_set_data[sizeof(expected_set_data)];

    check = at24cxxx_set(&at24cxxx_dev, SET_POSITION, SET_CHARACTER, SET_LEN);
    if (check != AT24CXXX_OK) {
        printf("[FAILURE] at24cxxx_set: %d (size = %u)\n", check, SET_LEN);
        return 1;
    }
    else {
        puts("[SUCCESS] at24cxxx_set");
    }

    check = at24cxxx_read(&at24cxxx_dev, SET_POSITION,
                          actual_set_data, SET_LEN);
    if (check != AT24CXXX_OK) {
        printf("[FAILURE] set/read: %d\n", check);
        return 1;
    }
    else if (memcmp(actual_set_data, expected_set_data, SET_LEN) != 0) {
        printf("[FAILURE] set/read: (%s != %s)\n", actual_set_data,
               expected_set_data);
        return 1;
    }
    else {
        puts("[SUCCESS] set/read");
    }

    puts("Finished tests for module at24cxxx");

    return 0;
}
