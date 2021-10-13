/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2014 PHYTEC Messtechnik GmbH
 *               2015 Eistec AB
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_spi
 *
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/**
 * @brief   We use this mask to differentiate between SPI_HWCS() and GPIO_PIN()
 */
#define SWCS_MASK           (0xfff0)

/**
 * @brief   Allocation device locks
 */
static mutex_t locks[SPI_NUMOF];

static inline SPI_Type *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

static inline void poweron(spi_t bus)
{
    switch((uint32_t)dev(bus)) {
        case (uint32_t)SPI0:
        case (uint32_t)SPI1:
            SIM->SCGC6 |= (spi_config[bus].simmask);
            break;
#ifdef SPI2
        case (uint32_t)SPI2:
            SIM->SCGC3 |= (spi_config[bus].simmask);
            break;
#endif
    }

    /* Enable the module */
    dev(bus)->MCR &= ~(SPI_MCR_MDIS_MASK);
}

static inline void poweroff(spi_t bus)
{
    /* Disable the module */
    dev(bus)->MCR |= SPI_MCR_MDIS_MASK;

    switch((uint32_t)dev(bus)) {
        case (uint32_t)SPI0:
        case (uint32_t)SPI1:
            SIM->SCGC6 &= ~(spi_config[bus].simmask);
            break;
#ifdef SPI2
        case (uint32_t)SPI2:
            SIM->SCGC3 &= ~(spi_config[bus].simmask);
            break;
#endif
    }
}

void spi_init(spi_t bus)
{
    /* make sure given bus device is valid */
    assert(bus < SPI_NUMOF);

    /* initialize the buses lock */
    mutex_init(&locks[bus]);
    /* trigger pin initialization */
    spi_init_pins(bus);
    /* power on the bus temporarily */
    poweron(bus);

    /* make the base configuration: configure as SPI master, set CS inactive
     * state (for HWCS lines) and clear FIFO counters and disable FIFOs */
    dev(bus)->MCR = (SPI_MCR_MSTR_MASK | SPI_MCR_PCSIS_MASK |
                     SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK |
                     SPI_MCR_DIS_RXF_MASK | SPI_MCR_DIS_TXF_MASK |
                     SPI_MCR_DOZE_MASK | SPI_MCR_HALT_MASK);

    /* disable all DMA and interrupt requests */
    dev(bus)->RSER = 0;

    /* Wait for the hardware to acknowledge the halt command */
    while (dev(bus)->SR & SPI_SR_TXRXS_MASK) {}

    /* and power off the bus until it is actually used */
    poweroff(bus);
}

void spi_init_pins(spi_t bus)
{
    gpio_init_port(spi_config[bus].pin_miso, spi_config[bus].pcr);
    gpio_init_port(spi_config[bus].pin_mosi, spi_config[bus].pcr);
    gpio_init_port(spi_config[bus].pin_clk , spi_config[bus].pcr);
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    if (bus >= SPI_NUMOF) {
        return SPI_NODEV;
    }
    if (cs == SPI_CS_UNDEF) {
        return SPI_NOCS;
    }

    if (cs & SWCS_MASK) {
        gpio_init((gpio_t)cs, GPIO_OUT);
    }
    else {
        if ((cs >= SPI_HWCS_NUMOF) ||
            (spi_config[bus].pin_cs[cs] == GPIO_UNDEF)) {
            return SPI_NOCS;
        }
        gpio_init_port(spi_config[bus].pin_cs[cs], spi_config[bus].pcr);
    }

    return SPI_OK;
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);
    /* lock and power on the bus */
    mutex_lock(&locks[bus]);
    poweron(bus);

    /* enable the device */
    dev(bus)->MCR &= ~(SPI_MCR_HALT_MASK | SPI_MCR_MDIS_MASK);

    /* configure clock and mode */
    dev(bus)->CTAR[0] = (mode | SPI_CTAR_FMSZ(7) | spi_clk_config[clk]);
}

void spi_release(spi_t bus)
{
    /* Halt transfers */
    dev(bus)->MCR |= SPI_MCR_HALT_MASK;

    /* Wait for the module to acknowledge the stop */
    while (dev(bus)->SR & SPI_SR_TXRXS_MASK) {}

    /* Disable the module */
    poweroff(bus);
    mutex_unlock(&locks[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;
    uint32_t flags = SPI_PUSHR_CONT_MASK;

    assert(out_buf || in_buf);

    /* handle chip select */
    if (cs != SPI_CS_UNDEF) {
        if (cs & SWCS_MASK) {
            gpio_clear((gpio_t)cs);
        }
        else {
            flags |= (uint32_t)(1 << (cs + SPI_PUSHR_PCS_SHIFT));
        }
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;
        if ((i == (len - 1)) && (!cont)) {
            flags &= ~(SPI_PUSHR_CONT_MASK);
        }

        while (!(dev(bus)->SR & SPI_SR_TFFF_MASK)) {}
        dev(bus)->PUSHR = (tmp | flags);

        while (!(dev(bus)->SR & SPI_SR_RXCTR_MASK)) {}
        tmp = dev(bus)->POPR;
        if (in_buf) {
            in_buf[i] = tmp;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF) && (cs & SWCS_MASK)) {
        gpio_set((gpio_t)cs);
    }
}
