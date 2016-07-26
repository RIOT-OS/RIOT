/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *               2016 Inria
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

uint8_t cc2420_strobe(const cc2420_t *dev, const uint8_t command)
{
    char res;

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_byte(dev->params.spi, (char)command, (char *)&res);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);

    return res;
}

void cc2420_reg_write(const cc2420_t *dev,
                      const uint8_t addr,
                      const uint16_t value)
{
    uint16_t tmp = byteorder_htons(value).u16;

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_regs(dev->params.spi, CC2420_REG_WRITE | addr,
                      (char *)&tmp, NULL, 2);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);
}

uint16_t cc2420_reg_read(const cc2420_t *dev, const uint8_t addr)
{
    network_uint16_t tmp;

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_regs(dev->params.spi, CC2420_REG_READ | addr,
                      NULL, (char *)&tmp, 2);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);

    return byteorder_ntohs(tmp);
}

void cc2420_ram_read(const cc2420_t *dev, const uint16_t addr,
                     uint8_t *data, const size_t len)
{
    char tmp[] = { (CC2420_RAM      | (addr & 0x7f)),
                   (CC2420_RAM_READ | ((addr >> 1) & 0xc0)) };

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_bytes(dev->params.spi, tmp, NULL, 2);
    spi_transfer_bytes(dev->params.spi, NULL, (char*)data, len);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);
}

void cc2420_ram_write(const cc2420_t *dev, const uint16_t addr,
                      const uint8_t *data, const size_t len)
{
    char tmp[] = { (CC2420_RAM       | (addr & 0x7f)),
                   (CC2420_RAM_WRITE | ((addr >> 1) & 0xc0)) };

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_bytes(dev->params.spi, tmp, NULL, 2);
    spi_transfer_bytes(dev->params.spi, (char*)data, NULL, len);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);
}

void cc2420_fifo_read(const cc2420_t *dev, uint8_t *data, const size_t len)
{
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_regs(dev->params.spi, CC2420_FIFO_READ,
                      NULL, (char *)data, len);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);
}

void cc2420_fifo_write(const cc2420_t *dev, uint8_t *data, const size_t len)
{
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.pin_cs);
    spi_transfer_regs(dev->params.spi, CC2420_FIFO_WRITE,
                      (char *)data, NULL, len);
    gpio_set(dev->params.pin_cs);
    spi_release(dev->params.spi);
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
