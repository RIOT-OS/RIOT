/*
 * Copyright (C) 2018 HAW Hamburg
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
 * @brief       lora-serialization tests
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @author      Jose Ignacio Alamos <jose.alamos@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>
#include "lora_serialization.h"

#if defined(BOARD_NATIVE)
#define TEST_01_EXPECTED    { 0x1f, 0x4c, 0x0e, 0x27 }
#define TEST_02_EXPECTED    { 0x65, 0xa6, 0xfa, 0xfd, \
                              0x6a, 0x24, 0x04, 0x09, \
                              0x1d, 0x4b, 0x7a, 0x57 }
#else
#define TEST_01_EXPECTED    { 0x1f, 0x4c, 0x0f, 0x27 }
#if defined(__AVR__)
#define TEST_02_EXPECTED    { 0x64, 0xa6, 0xfa, 0xfd, \
                              0x60, 0x24, 0x04, 0x09, \
                              0x1d, 0x4b, 0x7a, 0x57 }
#else
#define TEST_02_EXPECTED    { 0x64, 0xa6, 0xfa, 0xfd, \
                              0x6a, 0x24, 0x04, 0x09, \
                              0x1d, 0x4b, 0x7a, 0x57 }
#endif
#endif

uint8_t test01Expected[] = TEST_01_EXPECTED;
uint8_t test02Expected[] = TEST_02_EXPECTED;

static void print_buffer(const uint8_t *buffer, size_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        printf(" %02x", buffer[i]);
    }
}

/* Test 1: Add temperature and humidity */
static void test01(lora_serialization_t *serialization)
{
    puts("Test 1");
    puts("Temperature and humidity");
    puts("---------------------------------");
    lora_serialization_reset(serialization); /* Always reset */

    puts("- Writing temperature: 80.12");
    lora_serialization_write_temperature(serialization, 80.12);

    puts("- Writing humidity: 99.99");
    lora_serialization_write_humidity(serialization, 99.99);

    printf("- Encoded: ");
    print_buffer(serialization->buffer,
                 LORA_SERIALIZATION_TEMPERATURE_SIZE +
                 LORA_SERIALIZATION_HUMIDITY_SIZE);
    puts("");

    printf("- Expected:");
    print_buffer(test01Expected, sizeof(test01Expected));
    puts("");
    puts("---------------------------------");

    if (memcmp(serialization->buffer, test01Expected,
               sizeof(test01Expected)) != 0) {
        puts("FAILED");
    }
    else {
        puts("SUCCESS");
    }
}

/* Test 2: Add GPS coordinates and unix time */
static void test02(lora_serialization_t *serialization)
{
    puts("Test 2");
    puts("Coordinates and unix time");
    puts("---------------------------------");
    lora_serialization_reset(serialization); /* Always reset */

    puts("- Writing coordinates: -33.905052, 151.26641");
    lora_serialization_write_coordinates(serialization, -33.905052, 151.26641);

    puts("- Writing unix time: 1467632413");
    lora_serialization_write_unix_time(serialization, 1467632413);

    printf("- Encoded: ");
    print_buffer(serialization->buffer,
                 LORA_SERIALIZATION_GPS_SIZE +
                 LORA_SERIALIZATION_UNIX_TIME_SIZE);
    puts("");

    printf("- Expected:");
    print_buffer(test02Expected, sizeof(test02Expected));
    puts("");
    puts("---------------------------------");

    if (memcmp(serialization->buffer, test02Expected,
               sizeof(test02Expected)) != 0) {
        puts("FAILED");
    }
    else {
        puts("SUCCESS");
    }
}

int main(void)
{
    lora_serialization_t serialization;

    puts("Lora Serialization test\n");

    test01(&serialization);
    puts("");
    test02(&serialization);

    return 0;
}
