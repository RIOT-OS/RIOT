/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @ingroup     cpu_lpc11u34
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @todo        this implementation needs to be generalized in some aspects,
 *              e.g. clock configuration
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF];

static inline LPC_SSPx_Type *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

static inline void poweron(spi_t bus)
{
    /* de-assert SPIx, enable clock and set clock div */
    LPC_SYSCON->PRESETCTRL |= (spi_config[bus].preset_bit);
    LPC_SYSCON->SYSAHBCLKCTRL |= (spi_config[bus].ahb_bit);
}

static inline void poweroff(spi_t bus)
{
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(spi_config[bus].ahb_bit);
    LPC_SYSCON->PRESETCTRL &= ~(spi_config[bus].preset_bit);
}

void spi_init(spi_t bus)
{
    /* check device */
    assert(bus <= SPI_NUMOF);

    /* initialize device lock */
    mutex_init(&locks[bus]);

    /* set clock div for all SPI devices to 1 -> 48MHz */
    LPC_SYSCON->SSP0CLKDIV = 1;
    LPC_SYSCON->SSP1CLKDIV = 1;

    /* trigger the pin configuration */
    spi_init_pins(bus);

    /* power on the bus for the duration of initialization */
    poweron(bus);
    /* reset configuration */
    dev(bus)->CR1 = 0;
    /* configure base clock frequency to 12 MHz CLOCK_CORECLOCK / 4 */
    dev(bus)->CPSR = 4;
    /* and power off the bus again */
    poweroff(bus);
}

void spi_init_pins(spi_t bus)
{
    /* this is hacky as hell -> integrate this into the GPIO module */
    switch (bus) {
        case SPI_DEV(0):
            /* SPI0 : MISO */
            LPC_IOCON->PIO0_8 |= 1;
            /* SPI0 : MOSI */
            LPC_IOCON->PIO0_9 |= 1;
            /* SPI0 : SCK */
            LPC_IOCON->SWCLK_PIO0_10 |= 2;
            break;
        case SPI_DEV(1):
            /* SPI1 : MISO */
            LPC_IOCON->PIO1_21 |= 2;
            /* SPI1 : MOSI */
            LPC_IOCON->PIO0_21 |= 2;
            /* SPI1 : SCK */
            LPC_IOCON->PIO1_20 |= 2;
        default:
            break;
    }
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    /* lock an power on the bus */
    mutex_lock(&locks[bus]);
    poweron(bus);

    /* configure bus clock and mode and set to 8-bit transfer */
    dev(bus)->CR0 = ((clk << 8) | (mode << 6) | 0x07);
    /* enable the bus */
    dev(bus)->CR1 = (1 << 1);
    /* wait until ready and flush RX FIFO */
    while(dev(bus)->SR & (1 << 4)) {}
    while(dev(bus)->SR & (1 << 2)) {
        dev(bus)->DR;
    }

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    /* disable device, power off and release lock */
    dev(bus)->CR1 = 0;
    poweroff(bus);
    mutex_unlock(&locks[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    uint8_t *out_buf = (uint8_t *)out;
    uint8_t *in_buf = (uint8_t *)in;

    assert(out_buf || in_buf);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;
        while(dev(bus)->SR & (1 << 4)) {}       /* wait for BUSY clear */
        *((volatile uint8_t *)(&dev(bus)->DR)) = tmp;
        while(!(dev(bus)->SR & (1 << 2))) {}    /* wait RXNE */
        tmp = *((volatile uint8_t *)(&dev(bus)->DR));
        if (in_buf) {
            in_buf[i] = tmp;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}
