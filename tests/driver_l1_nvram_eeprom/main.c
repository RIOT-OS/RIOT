/*
 * Copyright (C) 2016 Unwired Devices <info@unwds.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup
 * @{
 *
 * @file
 * @brief       Test application for the STM32L1 Data EEPROM driver
 *
 * @author      EP <ep@unwds.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "board.h"
#include "xtimer.h"
#include "random.h"

#include "l1-nvram-eeprom.h"

#define TEST_NVRAM_SIZE 4096 * 2

/* This will only work on small memories. Modify if you need to test NVRAM
 * memories which do not fit inside free RAM */
static uint8_t buf_out[TEST_NVRAM_SIZE];
static uint8_t buf_in[TEST_NVRAM_SIZE];

/**
 * @brief xxd-like printing of a binary buffer
 */
static void print_buffer(const uint8_t *buf, size_t length)
{
    static const unsigned int bytes_per_line = 16;
    static const unsigned int bytes_per_group = 2;
    unsigned long i = 0;

    while (i < length) {
        unsigned int col;
        for (col = 0; col < bytes_per_line; ++col) {
            /* Print hex data */
            if (col == 0) {
                printf("\n%08lx: ", i);
            }
            else if ((col % bytes_per_group) == 0) {
                putchar(' ');
            }
            if ((i + col) < length) {
                printf("%02x", buf[i + col]);
            }
            else {
                putchar(' ');
                putchar(' ');
            }
        }

        putchar(' ');
        for (col = 0; col < bytes_per_line; ++col) {
            if ((i + col) < length) {
                /* Echo only printable chars */
                if (isprint(buf[i + col])) {
                    putchar(buf[i + col]);
                }
                else {
                    putchar('.');
                }
            }
            else {
                putchar(' ');
            }
        }

        i += bytes_per_line;
    }

    /* end with a new line */
    puts("");
}

int main(void)
{
    uint32_t i;
    nvram_t dev;

    xtimer_init();
    random_init(xtimer_now());

    puts("NVRAM STM32L1 EEPROM test application starting...");
    nvram_l1_eeprom_init(&dev);

    puts("Reading current memory contents...");
    for (i = 0; i < TEST_NVRAM_SIZE; ++i) {
        if (dev.read(&dev, &buf_in[i], i, 1) != 1) {
            puts("[Failed]\n");
            return 1;
        }
    }
    puts("[OK]");
    puts("NVRAM contents before test:");
    print_buffer(buf_in, sizeof(buf_in));

    puts("Writing bytewise 0xFF to device");
    memset(buf_out, 0xff, sizeof(buf_out));
    for (i = 0; i < TEST_NVRAM_SIZE; ++i) {
        if (dev.write(&dev, &buf_out[i], i, 1) != 1) {
            puts("[Failed]\n");
            return 1;
        }
        if (buf_out[i] != 0xff) {
            puts("nvram_spi_write modified *src!");
            printf(" i = %08lx\n", (unsigned long) i);
            puts("[Failed]\n");
            return 1;
        }
    }

    puts("Reading back blockwise");
    memset(buf_in, 0x00, sizeof(buf_in));
    if (dev.read(&dev, buf_in, 0, TEST_NVRAM_SIZE) != TEST_NVRAM_SIZE) {
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");
    puts("Verifying contents...");
    if (memcmp(buf_in, buf_out, TEST_NVRAM_SIZE) != 0) {
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");

    puts("Writing blockwise address complement to device");
    for (i = 0; i < TEST_NVRAM_SIZE; ++i) {
        buf_out[i] = (~(i)) & 0xff;
    }
    if (dev.write(&dev, buf_out, 0, TEST_NVRAM_SIZE) != TEST_NVRAM_SIZE) {
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");

    puts("buf_out:");
    print_buffer(buf_out, sizeof(buf_out));
    puts("Reading back blockwise");
    memset(buf_in, 0x00, sizeof(buf_in));
    if (dev.read(&dev, buf_in, 0, TEST_NVRAM_SIZE) != TEST_NVRAM_SIZE) {
        puts("[Failed]\n");
        return 1;
    }

    puts("[OK]");
    puts("Verifying contents...");
    if (memcmp(buf_in, buf_out, TEST_NVRAM_SIZE) != 0) {
        puts("buf_in:");
        print_buffer(buf_in, sizeof(buf_in));
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");

    puts("Generating pseudo-random test data...");

    for (i = 0; i < TEST_NVRAM_SIZE; ++i) {
        buf_out[i] = random_uint32() & 0xFF; /* Take one byte */
    }

    puts("buf_out:");
    print_buffer(buf_out, sizeof(buf_out));

    puts("Writing blockwise data to device");
    if (dev.write(&dev, buf_out, 0, TEST_NVRAM_SIZE) != TEST_NVRAM_SIZE) {
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");

    puts("Reading back blockwise");
    memset(buf_in, 0x00, sizeof(buf_in));
    if (dev.read(&dev, buf_in, 0, TEST_NVRAM_SIZE) != TEST_NVRAM_SIZE) {
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");
    puts("Verifying contents...");
    if (memcmp(buf_in, buf_out, TEST_NVRAM_SIZE) != 0) {
        puts("buf_in:");
        print_buffer(buf_in, sizeof(buf_in));
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");

    puts("All tests passed!");

    /* Hang with doing nothing */
    while (1) {
    }

    return 0;
}
