/*
 * Copyright (C) 2015 Loci Controls Inc.
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  cpu_cc2538
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Ian Martin <ian@locicontrols.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"

/**
 * @brief   Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF];

static inline cc2538_ssi_t *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

static inline void poweron(spi_t bus)
{
    SYS_CTRL_RCGCSSI |= (1 << bus);
    SYS_CTRL_SCGCSSI |= (1 << bus);
    SYS_CTRL_DCGCSSI |= (1 << bus);
}

static inline void poweroff(spi_t bus)
{
    SYS_CTRL_RCGCSSI &= ~(1 << bus);
    SYS_CTRL_SCGCSSI &= ~(1 << bus);
    SYS_CTRL_DCGCSSI &= ~(1 << bus);
}

void spi_init(spi_t bus)
{
    assert(bus <= SPI_NUMOF);

    /* temporarily power on the device */
    poweron(bus);
    /* configure device to be a master and disable SSI operation mode */
    dev(bus)->CR1 = 0;
    /* configure system clock as SSI clock source */
    dev(bus)->CC = SSI_SS_IODIV;
    /* and power off the bus again */
    poweroff(bus);

    /* trigger SPI pin configuration */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    switch ((uintptr_t)spi_config[bus].dev) {
        case (uintptr_t)SSI0:
            IOC_PXX_SEL[spi_config[bus].mosi_pin] = SSI0_TXD;
            IOC_PXX_SEL[spi_config[bus].sck_pin ] = SSI0_CLKOUT;
            IOC_PXX_SEL[spi_config[bus].cs_pin  ] = SSI0_FSSOUT;

            IOC_SSIRXD_SSI0 = spi_config[bus].miso_pin;
            break;

        case (uintptr_t)SSI1:
            IOC_PXX_SEL[spi_config[bus].mosi_pin] = SSI1_TXD;
            IOC_PXX_SEL[spi_config[bus].sck_pin ] = SSI1_CLKOUT;
            IOC_PXX_SEL[spi_config[bus].cs_pin  ] = SSI1_FSSOUT;

            IOC_SSIRXD_SSI1 = spi_config[bus].miso_pin;
            break;
    }

    IOC_PXX_OVER[spi_config[bus].mosi_pin] = IOC_OVERRIDE_OE;
    IOC_PXX_OVER[spi_config[bus].miso_pin] = IOC_OVERRIDE_DIS;
    IOC_PXX_OVER[spi_config[bus].sck_pin ] = IOC_OVERRIDE_OE;
    IOC_PXX_OVER[spi_config[bus].cs_pin  ] = IOC_OVERRIDE_OE;

    gpio_hardware_control(spi_config[bus].mosi_pin);
    gpio_hardware_control(spi_config[bus].miso_pin);
    gpio_hardware_control(spi_config[bus].sck_pin);
    gpio_hardware_control(spi_config[bus].cs_pin);
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    /* lock the bus */
    mutex_lock(&locks[bus]);
    /* power on device */
    poweron(bus);
    /* configure SCR clock field, data-width and mode */
    dev(bus)->CR0 = 0;
    dev(bus)->CPSR = (spi_clk_config[clk].cpsr);
    dev(bus)->CR0 = ((spi_clk_config[clk].scr << 8) | mode | SSI_CR0_DSS(8));
    /* enable SSI device */
    dev(bus)->CR1 = SSI_CR1_SSE;

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    /* disable and power off device */
    dev(bus)->CR1 = 0;
    poweroff(bus);
    /* and release lock... */
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

    if (!in_buf) {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->SR & SSI_SR_TNF)) {}
            dev(bus)->DR = out_buf[i];
        }
        /* flush RX FIFO while busy*/
        while ((dev(bus)->SR & SSI_SR_BSY)) {
            dev(bus)->DR;
        }
    }
    else if (!out_buf) { /*TODO this case is currently untested */
        size_t in_cnt = 0;
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->SR & SSI_SR_TNF)) {}
            dev(bus)->DR = 0;
            if (dev(bus)->SR & SSI_SR_RNE) {
                in_buf[in_cnt++] = dev(bus)->DR;
            }
        }
        /* get remaining bytes */
        while (dev(bus)->SR & SSI_SR_RNE) {
            in_buf[in_cnt++] = dev(bus)->DR;
        }
    }
    else {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->SR & SSI_SR_TNF)) {}
            dev(bus)->DR = out_buf[i];
            while (!(dev(bus)->SR & SSI_SR_RNE)){}
            in_buf[i] = dev(bus)->DR;
        }
    /* wait until no more busy */
    while ((dev(bus)->SR & SSI_SR_BSY)) {}
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}
