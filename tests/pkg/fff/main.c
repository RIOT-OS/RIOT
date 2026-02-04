/*
 * SPDX-FileCopyrightText: 2021 Jens Wetterich <jens@wetterich-net.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup    tests
 * @{
 * @file
 * @brief      Unit test to test the usage of @ref pkg_fff module
 * @author     Jens Wetterich <jens@wetterich-net.de>
 */

#define FFF_ARG_HISTORY_LEN  1u
#define FFF_CALL_HISTORY_LEN 1u
#include "fff.h"
#include "periph/i2c.h"
#include <stdio.h>
DEFINE_FFF_GLOBALS
FAKE_VALUE_FUNC(int, i2c_read_bytes, i2c_t, uint16_t, void *, size_t, uint8_t)
FAKE_VOID_FUNC(i2c_acquire, i2c_t)
FAKE_VOID_FUNC(i2c_release, i2c_t)

int test_i2c_basic(void *buffer, size_t len);
int read_fake_impl(i2c_t dev, uint16_t addr, void *data, size_t size, uint8_t flags);

const unsigned int fake_read_len = 6;
const char *fake_read_data = "hello";
const i2c_t device = 0;
const uint16_t address = 4;
const uint8_t flags = 0;

int test_i2c_basic(void *buffer, size_t len)
{
    int read_return_val;
    int failure = 0;

    i2c_acquire(device);
    read_return_val = i2c_read_bytes(device, address, buffer, len, flags);
    if (read_return_val != 0) {
        failure = 1;
    }
    i2c_release(device);
    return -failure;
}

int read_fake_impl(i2c_t dev, uint16_t addr, void *data, size_t size, uint8_t fl)
{
    (void)dev;
    (void)addr;
    (void)size;
    (void)fl;
    memcpy(data, fake_read_data, fake_read_len);
    return 0;
}

int main(void)
{
    uint8_t buffer[fake_read_len];
    int basic_test_return_val;
    int failure = 0;

    puts("Testing fff");
    /* Set fake implementation / return values of the mocks */
    i2c_read_bytes_fake.custom_fake = read_fake_impl;
    /* Run function under test */
    basic_test_return_val = test_i2c_basic(buffer, fake_read_len);
    /* Assert correct interaction of the function under test with the mocked API */
    if (basic_test_return_val != 0) {
        failure = 1;
        puts("The return value of the read_bytes mock didn't match the expected value");
    }
    if (i2c_read_bytes_fake.arg0_val != device) {
        failure = 1;
        puts("The device parameter to the read_bytes mock didn't match the expected value");
    }
    if (i2c_read_bytes_fake.arg1_val != address) {
        failure = 1;
        puts("The address parameter to the read_bytes mock didn't match the expected value");
    }
    if (memcmp(buffer, fake_read_data, fake_read_len) != 0) {
        failure = 1;
        puts("The read_bytes mock didn't write the expected data into the buffer");
    }
    if (i2c_read_bytes_fake.arg3_val != fake_read_len) {
        failure = 1;
        puts("The len parameter to the read_bytes mock didn't match the expected value");
    }
    if (i2c_read_bytes_fake.arg4_val != flags) {
        failure = 1;
        puts("The flags parameter to the read_bytes mock didn't match the expected value");
    }
    if (i2c_read_bytes_fake.call_count != 1) {
        failure = 1;
        puts("The call count for the read_bytes mock didn't match the expected value");
    }
    if (i2c_acquire_fake.call_count != 1) {
        failure = 1;
        puts("The call count for the acquire mock didn't match the expected value");
    }
    if (i2c_release_fake.call_count != 1) {
        failure = 1;
        puts("The call count for the release mock didn't match the expected value");
    }
    printf("FFF test completed: %s\n", failure ? "FAILURE" : "SUCCESS");
    return 0;
}
/** @} */
