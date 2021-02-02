/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_arduino_api
 * @{
 *
 * @file
 * @brief       Implementation of the Arduino 'SPI' interface
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#ifdef MODULE_PERIPH_SPI

extern "C" {
#include "assert.h"
}

#include "SPI.h"

SPISettings::SPISettings(uint32_t clock_hz, uint8_t bitOrder, uint8_t dataMode)
{
    (void)bitOrder;

    static const spi_clk_t clocks[] = {
        SPI_CLK_10MHZ, SPI_CLK_5MHZ, SPI_CLK_1MHZ, SPI_CLK_400KHZ
    };
    static const uint32_t steps [] = {
        10000000, 5000000, 1000000, 400000
    };

    assert(bitOrder == MSBFIRST);
    switch(dataMode) {
    default:
    case SPI_MODE0:
        mode = SPI_MODE_0;
        break;
    case SPI_MODE1:
        mode = SPI_MODE_1;
        break;
    case SPI_MODE2:
        mode = SPI_MODE_2;
        break;
    case SPI_MODE3:
        mode = SPI_MODE_3;
        break;
    }

    for (uint8_t i = 0; i < ARRAY_SIZE(steps); i++) {
        if (clock_hz >= steps[i]) {
            clock = clocks[i];
            return;
        }
    }

    clock = SPI_CLK_100KHZ;
}

SPIClass::SPIClass(spi_t spi_dev)
{
    /* Check if default SPI interface is valid */
    BUILD_BUG_ON(ARDUINO_SPI_INTERFACE >= SPI_NUMOF);
    this->spi_dev = spi_dev;
    this->settings = SPISettings();
    this->is_transaction = false;
    rmutex_init(&this->mut);
}

void SPIClass::beginTransaction(SPISettings settings)
{
    rmutex_lock(&mut);
    /* Call spi_acquire first to prevent data races */
    int retval = spi_acquire(spi_dev, SPI_CS_UNDEF,
                             settings.mode, settings.clock);
    /* No support for exceptions (at least on AVR), resort to assert() */
    assert(retval == SPI_OK);
    if (retval != SPI_OK) {
        return;
    }
    is_transaction = true;
}

void SPIClass::endTransaction()
{
    is_transaction = false;
    spi_release(spi_dev);
    rmutex_unlock(&mut);
}

void SPIClass::transfer(void *buf, size_t count)
{
    rmutex_lock(&mut);

    if (!is_transaction) {
        int retval = spi_acquire(spi_dev, SPI_CS_UNDEF,
                                 settings.mode, settings.clock);
        /* No support for exceptions (at least on AVR), resort to assert() */
        assert(retval == SPI_OK);
        if (retval != SPI_OK) {
            return;
        }
    }
    spi_transfer_bytes(spi_dev, SPI_CS_UNDEF, false, buf, buf, count);
    if (!is_transaction) {
        spi_release(spi_dev);
    }

    rmutex_unlock(&mut);
}

void SPIClass::setBitOrder(uint8_t order)
{
    (void)order;
    assert(order == MSBFIRST);
}

void SPIClass::setDataMode(uint8_t dataMode)
{
    switch(dataMode) {
    default:
    case SPI_MODE0:
        settings.mode = SPI_MODE_0;
        break;
    case SPI_MODE1:
        settings.mode = SPI_MODE_1;
        break;
    case SPI_MODE2:
        settings.mode = SPI_MODE_2;
        break;
    case SPI_MODE3:
        settings.mode = SPI_MODE_3;
        break;
    }
}

void SPIClass::setClockDivider(uint8_t divider)
{
    static const spi_clk_t clocks[] = {
        SPI_CLK_5MHZ, SPI_CLK_1MHZ, SPI_CLK_400KHZ, SPI_CLK_100KHZ
    };

    assert(divider < ARRAY_SIZE(clocks));
    settings.clock = clocks[divider];
}

SPIClass SPI(SPI_DEV(ARDUINO_SPI_INTERFACE));

#else /* MODULE_PERIPH_SPI */
typedef int dont_be_pedantic;
#endif /* MODULE_PERIPH_SPI */
