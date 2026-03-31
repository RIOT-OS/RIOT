/*
 * SPDX-FileCopyrightText: 2015-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_cc2420
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include "periph/spi.h"
#include "periph/gpio.h"
#include "xtimer.h"

#include "cc2420_internal.h"
#include "cc2420_registers.h"

#define SPI_BUS             (dev->params.spi)
#define SPI_CS              (dev->params.pin_cs)
#define SPI_MODE            (SPI_MODE_0)
#define SPI_CLK             (dev->params.spi_clk)

uint8_t cc2420_strobe(const cc2420_t *dev, const uint8_t command)
{
    uint8_t res;

    spi_acquire(SPI_BUS, SPI_CS, SPI_MODE, SPI_CLK);
    res = spi_transfer_byte(SPI_BUS, SPI_CS, false, command);
    spi_release(SPI_BUS);

    return res;
}

void cc2420_reg_write(const cc2420_t *dev,
                      const uint8_t addr,
                      const uint16_t value)
{
    uint16_t tmp = byteorder_htons(value).u16;

    spi_acquire(SPI_BUS, SPI_CS, SPI_MODE, SPI_CLK);
    spi_transfer_regs(SPI_BUS, SPI_CS, (CC2420_REG_WRITE | addr), &tmp, NULL, 2);
    spi_release(SPI_BUS);
}

uint16_t cc2420_reg_read(const cc2420_t *dev, const uint8_t addr)
{
    network_uint16_t tmp;

    spi_acquire(SPI_BUS, SPI_CS, SPI_MODE, SPI_CLK);
    spi_transfer_regs(SPI_BUS, SPI_CS, (CC2420_REG_READ | addr),NULL, &tmp, 2);
    spi_release(SPI_BUS);

    return byteorder_ntohs(tmp);
}

void cc2420_ram_read(const cc2420_t *dev, const uint16_t addr,
                     uint8_t *data, const size_t len)
{
    uint8_t tmp[] = { (CC2420_RAM      | (addr & 0x7f)),
                      (CC2420_RAM_READ | ((addr >> 1) & 0xc0)) };

    spi_acquire(SPI_BUS, SPI_CS, SPI_MODE, SPI_CLK);
    spi_transfer_bytes(SPI_BUS, SPI_CS, true, tmp, NULL, 2);
    spi_transfer_bytes(SPI_BUS, SPI_CS, false, NULL, data, len);
    spi_release(SPI_BUS);
}

void cc2420_ram_write(const cc2420_t *dev, const uint16_t addr,
                      const uint8_t *data, const size_t len)
{
    uint8_t tmp[] = { (CC2420_RAM       | (addr & 0x7f)),
                      (CC2420_RAM_WRITE | ((addr >> 1) & 0xc0)) };

    spi_acquire(SPI_BUS, SPI_CS, SPI_MODE, SPI_CLK);
    spi_transfer_bytes(SPI_BUS, SPI_CS, true, tmp, NULL, 2);
    spi_transfer_bytes(SPI_BUS, SPI_CS, false, data, NULL, len);
    spi_release(SPI_BUS);
}

void cc2420_fifo_read(const cc2420_t *dev, uint8_t *data, const size_t len)
{
    spi_acquire(SPI_BUS, SPI_CS, SPI_MODE, SPI_CLK);
    spi_transfer_regs(SPI_BUS, SPI_CS, CC2420_FIFO_READ, NULL, data, len);
    spi_release(SPI_BUS);
}

void cc2420_fifo_write(const cc2420_t *dev, uint8_t *data, const size_t len)
{
    spi_acquire(SPI_BUS, SPI_CS, SPI_MODE, SPI_CLK);
    spi_transfer_regs(SPI_BUS, SPI_CS, CC2420_FIFO_WRITE, data, NULL, len);
    gpio_set(dev->params.pin_cs);
    spi_release(SPI_BUS);
}

uint8_t cc2420_status(cc2420_t *dev)
{
    return cc2420_strobe(dev, CC2420_STROBE_NOP);
}

uint8_t cc2420_state(cc2420_t *dev)
{
    return (uint8_t)cc2420_reg_read(dev, CC2420_REG_FSMSTATE);
}

void cc2420_en_xosc(cc2420_t *dev)
{
    cc2420_strobe(dev, CC2420_STROBE_XOSCON);
    xtimer_usleep(CC2420_XOSCON_DELAY);
}
