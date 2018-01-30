/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc1200
 * @{
 *
 * @file
 * @brief       TI Chipcon CC1200 spi driver
 *
 * @author    Anon Mall <anon.mall@gt-arc.com>
 * @}
 */

#include <stdio.h>

#include "cc1200.h"
#include "cc1200-spi.h"
#include "cc1200-internal.h"
#include "cc1200-defines.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "xtimer.h"
#include "irq.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define SPI_CLK         SPI_CLK_5MHZ
#define SPI_MODE        SPI_MODE_0

/**********************************************************************
 *                      CC1200 spi access
 **********************************************************************/

static inline void lock(cc1200_t *dev)
{
    spi_acquire(dev->params.spi, dev->params.cs, SPI_MODE, SPI_CLK);
}


void cc1200_writeburst_reg(cc1200_t *dev, uint16_t addr, const char *src, uint8_t count)
{
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    if((addr >> 8) == 0x2F){
        spi_transfer_byte(dev->params.spi, dev->params.cs, true, 0x2F | CC1200_WRITE_BURST);
    }
    spi_transfer_regs(dev->params.spi, dev->params.cs,
                      (addr | CC1200_WRITE_BURST), src, NULL, count);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
}

void cc1200_readburst_reg(cc1200_t *dev, uint16_t addr, char *buffer, uint8_t count)
{
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    if((addr >> 8) == 0x2F){
        spi_transfer_byte(dev->params.spi, dev->params.cs, true, 0x2F | CC1200_READ_BURST);
    }
    int i = 0;
    spi_transfer_byte(dev->params.spi, dev->params.cs, true,
                      (addr | CC1200_READ_BURST));
    while (i < count) {
        buffer[i] = (char)spi_transfer_byte(dev->params.spi, dev->params.cs,
                                            true, CC1200_NOBYTE);
        i++;
    }
    gpio_set(dev->params.cs);
    //spi_transfer_regs(dev->params.spi, dev->params.cs, (addr|CC1200_READ_BURST), NULL, buffer, count);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
}

void cc1200_write_reg(cc1200_t *dev, uint16_t addr, uint8_t value)
{
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    if((addr >> 8) == 0x2F){
        spi_transfer_byte(dev->params.spi, dev->params.cs, true, 0x2F);
    }

    spi_transfer_reg(dev->params.spi, dev->params.cs, addr, value);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
}

uint8_t cc1200_read_reg(cc1200_t *dev, uint16_t addr)
{
    uint8_t result;
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    if((addr >> 8) == 0x2F){
        spi_transfer_byte(dev->params.spi, dev->params.cs, true, 0x2F | CC1200_READ_SINGLE);
    }

    result = spi_transfer_reg(dev->params.spi, dev->params.cs,
                              (addr | CC1200_READ_SINGLE), CC1200_NOBYTE);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
    return result;
}

uint8_t cc1200_read_status(cc1200_t *dev, uint16_t addr)
{
    uint8_t result;
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    if((addr >> 8) == 0x2F){
        spi_transfer_byte(dev->params.spi, dev->params.cs, true, 0x2F | CC1200_READ_BURST);
    }

    result = spi_transfer_reg(dev->params.spi, dev->params.cs,
                              (addr | CC1200_READ_BURST), CC1200_NOBYTE);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
    return (uint8_t) result;
}

uint8_t cc1200_get_reg_robust(cc1200_t *dev, uint16_t addr)
{
    uint8_t res1, res2;
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();

    do
    {
        if ((addr >> 8) == 0x2F)
        {
            spi_transfer_byte(dev->params.spi, dev->params.cs, true, 0x2F | CC1200_READ_BURST);
        }
        res1 = spi_transfer_reg(dev->params.spi, dev->params.cs,
                                (addr | CC1200_READ_BURST), CC1200_NOBYTE);
        if ((addr >> 8) == 0x2F)
        {
            spi_transfer_byte(dev->params.spi, dev->params.cs, true, 0x2F | CC1200_READ_BURST);
        }
        res2 = spi_transfer_reg(dev->params.spi, dev->params.cs,
                                (addr | CC1200_READ_BURST), CC1200_NOBYTE);
    } while (res1 != res2);
    irq_restore(cpsr);
    spi_release(dev->params.spi);
    return res1;
}

uint8_t cc1200_strobe(cc1200_t *dev, uint8_t c)
{
#ifdef CC1200_DONT_RESET
    if (c == CC1200_SRES) {
        return 0;
    }
#endif

    uint8_t result;
    unsigned int cpsr;
    lock(dev);
    cpsr = irq_disable();
    result = spi_transfer_byte(dev->params.spi, dev->params.cs, false,  c);
    if(c == CC1200_SRES){
        result = spi_transfer_byte(dev->params.spi, dev->params.cs, true,  c);
        xtimer_spin(xtimer_ticks_from_usec(RESET_WAIT_TIME));
        gpio_set(dev->params.cs);
    }else{
        result = spi_transfer_byte(dev->params.spi, dev->params.cs, false,  c);
    }
    irq_restore(cpsr);
    spi_release(dev->params.spi);
    return result;
}
