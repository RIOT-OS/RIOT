/*
 * Copyright (C) 2015 Eistec AB
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
 * @brief       Test application for the SPI NVRAM driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "board.h"
#include "ztimer.h"
#include "timex.h"
#include "periph/spi.h"
#include "nvram-spi.h"

#ifndef TEST_NVRAM_SPI_DEV
#error "TEST_NVRAM_SPI_DEV not defined"
#endif
#ifndef TEST_NVRAM_SPI_CS
#error "TEST_NVRAM_SPI_CS not defined"
#endif
#ifndef TEST_NVRAM_SPI_SIZE
#error "TEST_NVRAM_SPI_SIZE not defined"
#endif
#ifndef TEST_NVRAM_SPI_ADDRESS_COUNT
#error "TEST_NVRAM_SPI_ADDRESS_COUNT not defined"
#endif

#ifdef TEST_NVRAM_SPI_MODE
#define SPI_MODE    (TEST_NVRAM_SPI_MODE)
#else
#define SPI_MODE    (SPI_MODE_0)
#endif

#ifdef TEST_NVRAM_SPI_SPEED
#define SPI_CLK     (TEST_NVRAM_SPI_SPEED)
#else
#define SPI_CLK     (SPI_CLK_10MHZ)
#endif

/* This will only work on small memories. Modify if you need to test NVRAM
 * memories which do not fit inside free RAM */
static uint8_t buf_out[TEST_NVRAM_SPI_SIZE];
static uint8_t buf_in[TEST_NVRAM_SPI_SIZE];

/**
 * @brief xxd-like printing of a binary buffer
 */
static void print_buffer(const uint8_t * buf, size_t length) {
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
                printf("%02hhx", buf[i + col]);
            } else {
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
                } else {
                    putchar('.');
                }
            } else {
                putchar(' ');
            }
        }
        i += bytes_per_line;
    }
    /* end with a newline */
    puts("");
}

/* weak PRNG for generating "random" test data */
static uint8_t lcg_rand8(void) {
    static const uint32_t a = 1103515245;
    static const uint32_t c = 12345;
    static uint32_t val = 123456; /* seed value */
    val = val * a + c;
    return (val >> 16) & 0xff;
}

int main(void)
{
    uint32_t i;
    nvram_spi_params_t spi_params = {
        .spi = TEST_NVRAM_SPI_DEV,
        .clk = SPI_CLK,
        .cs = TEST_NVRAM_SPI_CS,
        .address_count = TEST_NVRAM_SPI_ADDRESS_COUNT,
    };
    nvram_t dev;
    uint32_t start_delay = 10;

    puts("NVRAM SPI test application starting...");

    puts("Initializing NVRAM SPI device descriptor... ");
    if (nvram_spi_init(&dev, &spi_params, TEST_NVRAM_SPI_SIZE) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("NVRAM SPI init done.\n");

    puts("!!! This test will erase everything on the NVRAM !!!");
    puts("!!! Unplug/reset/halt device now if this is not acceptable !!!");
    puts("Waiting for 10 seconds before continuing...");
    ztimer_sleep(ZTIMER_USEC, start_delay * US_PER_SEC);

    puts("Reading current memory contents...");
    for (i = 0; i < TEST_NVRAM_SPI_SIZE; ++i) {
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
    for (i = 0; i < TEST_NVRAM_SPI_SIZE; ++i) {
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
    if (dev.read(&dev, buf_in, 0, TEST_NVRAM_SPI_SIZE) != TEST_NVRAM_SPI_SIZE) {
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");
    puts("Verifying contents...");
    if (memcmp(buf_in, buf_out, TEST_NVRAM_SPI_SIZE) != 0) {
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");

    puts("Writing blockwise address complement to device");
    for (i = 0; i < TEST_NVRAM_SPI_SIZE; ++i) {
        buf_out[i] = (~(i)) & 0xff;
    }
    if (dev.write(&dev, buf_out, 0, TEST_NVRAM_SPI_SIZE) != TEST_NVRAM_SPI_SIZE) {
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");
    puts("buf_out:");
    print_buffer(buf_out, sizeof(buf_out));
    puts("Reading back blockwise");
    memset(buf_in, 0x00, sizeof(buf_in));
    if (dev.read(&dev, buf_in, 0, TEST_NVRAM_SPI_SIZE) != TEST_NVRAM_SPI_SIZE) {
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");
    puts("Verifying contents...");
    if (memcmp(buf_in, buf_out, TEST_NVRAM_SPI_SIZE) != 0) {
        puts("buf_in:");
        print_buffer(buf_in, sizeof(buf_in));
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");

    puts("Generating pseudo-random test data...");

    for (i = 0; i < TEST_NVRAM_SPI_SIZE; ++i) {
        buf_out[i] = lcg_rand8();
    }

    puts("buf_out:");
    print_buffer(buf_out, sizeof(buf_out));

    puts("Writing blockwise data to device");
    if (dev.write(&dev, buf_out, 0, TEST_NVRAM_SPI_SIZE) != TEST_NVRAM_SPI_SIZE) {
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");

    puts("Reading back blockwise");
    memset(buf_in, 0x00, sizeof(buf_in));
    if (dev.read(&dev, buf_in, 0, TEST_NVRAM_SPI_SIZE) != TEST_NVRAM_SPI_SIZE) {
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");
    puts("Verifying contents...");
    if (memcmp(buf_in, buf_out, TEST_NVRAM_SPI_SIZE) != 0) {
        puts("buf_in:");
        print_buffer(buf_in, sizeof(buf_in));
        puts("[Failed]\n");
        return 1;
    }
    puts("[OK]");

    puts("All tests passed!");

    while(1);

    return 0;
}
