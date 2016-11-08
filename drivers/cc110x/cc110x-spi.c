/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       TI Chipcon CC110x spi driver
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "cc110x.h"
#include "cc110x-spi.h"
#include "cc110x-internal.h"
#include "cc110x-defines.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "xtimer.h"
#include "irq.h"

#define SPI_CLK         SPI_CLK_5MHZ
#define SPI_MODE        SPI_MODE_0

/**********************************************************************
 *                      CC110x spi access
 **********************************************************************/

static inline void lock(cc110x_t *dev)
{
    spi_acquire(dev->params.spi, dev->params.cs, SPI_MODE, SPI_CLK);
}

void cc110x_cs(cc110x_t *dev)
{
    volatile int retry_count = 0;
    /* Switch MISO/GDO1 to GPIO input mode */
#ifndef GPIO_READS_SPI_PINS
    gpio_init(dev->params.gdo1, GPIO_IN);
#endif
    /* CS to low */
    gpio_clear(dev->params.cs);
    /* Wait for SO to go low (voltage regulator
     * has stabilized and the crystal is running) */
    while (gpio_read(dev->params.gdo1)) {
        /* Wait ~500us and try again */
        xtimer_usleep(CS_SO_WAIT_TIME);

        if (gpio_read(dev->params.gdo1)) {
            retry_count++;

            if (retry_count > CC110X_GDO1_LOW_RETRY) {
                puts("[CC110X spi] fatal error\n");
                break;
            }

            gpio_set(dev->params.cs);
            gpio_clear(dev->params.cs);
        }
    }
    /* Switch MISO/GDO1 to spi mode */
#ifndef GPIO_READS_SPI_PINS
    spi_init_pins(dev->params.spi);
#endif
}

void cc110x_writeburst_reg(cc110x_t *dev, uint8_t addr, const char *src, uint8_t count)
{
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    cc110x_cs(dev);
    spi_transfer_regs(dev->params.spi, SPI_CS_UNDEF,
                      (addr | CC110X_WRITE_BURST), src, NULL, count);
    gpio_set(dev->params.cs);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
}

void cc110x_readburst_reg(cc110x_t *dev, uint8_t addr, char *buffer, uint8_t count)
{
    int i = 0;
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    cc110x_cs(dev);
    spi_transfer_byte(dev->params.spi, SPI_CS_UNDEF, false,
                      (addr | CC110X_READ_BURST));
    while (i < count) {
        buffer[i] = (char)spi_transfer_byte(dev->params.spi, SPI_CS_UNDEF,
                                            false, CC110X_NOBYTE);
        i++;
    }
    gpio_set(dev->params.cs);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
}

void cc110x_write_reg(cc110x_t *dev, uint8_t addr, uint8_t value)
{
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    cc110x_cs(dev);
    spi_transfer_reg(dev->params.spi, SPI_CS_UNDEF, addr, value);
    gpio_set(dev->params.cs);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
}

uint8_t cc110x_read_reg(cc110x_t *dev, uint8_t addr)
{
    uint8_t result;
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    cc110x_cs(dev);
    result = spi_transfer_reg(dev->params.spi, SPI_CS_UNDEF,
                              (addr | CC110X_READ_SINGLE), CC110X_NOBYTE);
    gpio_set(dev->params.cs);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
    return result;
}

uint8_t cc110x_read_status(cc110x_t *dev, uint8_t addr)
{
    uint8_t result;
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    cc110x_cs(dev);
    result = spi_transfer_reg(dev->params.spi, SPI_CS_UNDEF,
                              (addr | CC110X_READ_BURST), CC110X_NOBYTE);
    gpio_set(dev->params.cs);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
    return (uint8_t) result;
}

uint8_t cc110x_get_reg_robust(cc110x_t *dev, uint8_t addr)
{
    uint8_t res1, res2;
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    cc110x_cs(dev);
    do {
        res1 = spi_transfer_reg(dev->params.spi, SPI_CS_UNDEF,
                                (addr | CC110X_READ_BURST), CC110X_NOBYTE);
        res2 = spi_transfer_reg(dev->params.spi, SPI_CS_UNDEF,
                                (addr | CC110X_READ_BURST), CC110X_NOBYTE);
    } while (res1 != res2);
    gpio_set(dev->params.cs);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
    return res1;
}

uint8_t cc110x_strobe(cc110x_t *dev, uint8_t c)
{
#ifdef CC110X_DONT_RESET
    if (c == CC110X_SRES) {
        return 0;
    }
#endif

    uint8_t result;
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    cc110x_cs(dev);
    result = spi_transfer_byte(dev->params.spi, SPI_CS_UNDEF, false,  c);
    gpio_set(dev->params.cs);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
    return result;
}
