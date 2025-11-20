/*
 * SPDX-FileCopyrightText: 2017 Hamburg University of Applied Sciences
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Application for testing the software SPI driver implementations
 *
 *
 * @author      Markus Blechschmidt <Markus.Blechschmidt@haw-hamburg.de>
 * @author      Peter Kietzmann     <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "soft_spi.h"

int main(void)
{
    puts("Minimal test application for the software SPI driver");

    char string[] = "Soft SPI Test String";

    soft_spi_t soft_spi = TEST_SOFT_SPI_DEV;
    soft_spi_cs_t cs = TEST_CS_PIN;

    /* Initialize software SPI device  */
    soft_spi_init(soft_spi);

    /* Initialize CS pin */
    int tmp = soft_spi_init_cs(soft_spi, cs);
    if (tmp != SOFT_SPI_OK) {
        printf("error: unable to initialize the given chip select line %i\n", tmp);
        return 1;
    }

    puts("Send 0xa5 in all four modes");
    soft_spi_acquire(soft_spi, cs, SOFT_SPI_MODE_0, SOFT_SPI_CLK_100KHZ);
    soft_spi_transfer_byte(soft_spi, cs, false, 0xa5);
    soft_spi_release(soft_spi);

    soft_spi_acquire(soft_spi, cs, SOFT_SPI_MODE_1, SOFT_SPI_CLK_100KHZ);
    soft_spi_transfer_byte(soft_spi, cs, false, 0xa5);
    soft_spi_release(soft_spi);

    soft_spi_acquire(soft_spi, cs, SOFT_SPI_MODE_2, SOFT_SPI_CLK_100KHZ);
    soft_spi_transfer_byte(soft_spi, cs, false, 0xa5);
    soft_spi_release(soft_spi);

    soft_spi_acquire(soft_spi, cs, SOFT_SPI_MODE_3, SOFT_SPI_CLK_100KHZ);
    soft_spi_transfer_byte(soft_spi, cs, false, 0xa5);
    soft_spi_release(soft_spi);

    printf("Send %s\n",string);
    soft_spi_acquire(soft_spi, cs, SOFT_SPI_MODE_0, SOFT_SPI_CLK_100KHZ);
    soft_spi_transfer_bytes(soft_spi, cs, false, string, NULL, sizeof string);
    soft_spi_release(soft_spi);

    puts("Soft SPI Test End");
    return 0;
}
