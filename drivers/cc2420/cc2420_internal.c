/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc2420
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "periph/spi.h"
#include "periph/gpio.h"
#include "cc2420_internal.h"
#include "cc2420_registers.h"

void cc2420_reg_write(const cc2420_t *dev,
                      const uint8_t addr,
                      const uint16_t value)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_regs(dev->spi,
                      CC2420_ACCESS_REG_WRITE | addr,
                      (char *)&value, NULL, 2);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

uint16_t cc2420_reg_read(const cc2420_t *dev, const uint8_t addr)
{
    uint16_t value;

    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_regs(dev->spi,
                      CC2420_ACCESS_REG_READ | addr,
                      NULL, (char *)&value, 2);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);

    return value;
}

uint8_t cc2420_strobe(const cc2420_t *dev, const uint8_t command)
{
    char result;

    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_byte(dev->spi, (char)command, &result);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);

    return (uint8_t)result;
}

void cc2420_ram_read(const cc2420_t *dev, const uint16_t address,
                     uint8_t *data, const size_t len)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_byte(dev->spi, (CC2420_ACCESS_RAM | (address & 0x7f)), NULL);
    spi_transfer_byte(dev->spi,
                      (CC2420_ACCESS_RAM_READ | ((address >> 1) & 0xc0)),
                      NULL);
    spi_transfer_bytes(dev->spi, NULL, (char*)data, len);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

void cc2420_ram_write(const cc2420_t *dev, const uint16_t address,
                      const uint8_t *data, const size_t len)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_byte(dev->spi, (CC2420_ACCESS_RAM | (address & 0x7f)), NULL);
    spi_transfer_byte(dev->spi,
                      (CC2420_ACCESS_RAM_WRITE | ((address >> 1) & 0xc0)),
                      NULL);
    spi_transfer_bytes(dev->spi, (char*)data, NULL, len);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

void cc2420_fb_read(const cc2420_t *dev, uint8_t *data, const size_t len)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_regs(dev->spi, CC2420_REG_RXFIFO | CC2420_ACCESS_REG_READ,
                      NULL, (char *)data, len);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}

void cc2420_fb_write(const cc2420_t *dev,
                       uint8_t *data,
                       const size_t len)
{
    spi_acquire(dev->spi);
    gpio_clear(dev->cs_pin);
    spi_transfer_regs(dev->spi, CC2420_REG_TXFIFO | CC2420_ACCESS_REG_WRITE,
                      (char *)data, NULL, len);
    gpio_set(dev->cs_pin);
    spi_release(dev->spi);
}
