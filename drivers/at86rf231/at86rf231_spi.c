/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "at86rf231_spi.h"
#include "at86rf231_arch.h"
#include "at86rf231.h"

void at86rf231_reg_write(uint8_t addr, uint8_t value)
{
    // Start the SPI transfer
    at86rf231_spi_select();

    // Send first byte being the command and address
    at86rf231_spi_transfer_byte(AT86RF231_ACCESS_REG | AT86RF231_ACCESS_WRITE | addr);

    // Send value
    at86rf231_spi_transfer_byte(value);

    // End the SPI transfer
    at86rf231_spi_unselect();
}

uint8_t at86rf231_reg_read(uint8_t addr)
{
    uint8_t value;

    // Start the SPI transfer
    at86rf231_spi_select();

    // Send first byte being the command and address
    at86rf231_spi_transfer_byte(AT86RF231_ACCESS_REG | AT86RF231_ACCESS_READ | addr);

    // Send value
    value = at86rf231_spi_transfer_byte(0);

    // End the SPI transfer
    at86rf231_spi_unselect();

    return value;
}

void at86rf231_read_fifo(uint8_t *data, uint8_t length)
{
    // Start the SPI transfer
    at86rf231_spi_select();

    // Send Frame Buffer Write access
    at86rf231_spi_transfer_byte(AT86RF231_ACCESS_FRAMEBUFFER | AT86RF231_ACCESS_READ);

    at86rf231_spi_transfer(0, data, length);

    // End the SPI transfer
    at86rf231_spi_unselect();
}

void at86rf231_write_fifo(const uint8_t *data, uint8_t length)
{
    // Start the SPI transfer
    at86rf231_spi_select();

    // Send Frame Buffer Write access
    at86rf231_spi_transfer_byte(AT86RF231_ACCESS_FRAMEBUFFER | AT86RF231_ACCESS_WRITE);

    at86rf231_spi_transfer(data, 0, length);

    // End the SPI transfer
    at86rf231_spi_unselect();
}
