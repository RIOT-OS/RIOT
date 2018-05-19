/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2014 PHYTEC Messtechnik GmbH
 *               2015 Eistec AB
 *               2016 Freie Universität Berlin
 *               2018 Ishraq Ibne Ashraf
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
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"

#define ENABLE_DEBUG        (0)
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

#if !defined(_MKE02Z4_H_)
            SIM->SCGC6 |= (spi_config[bus].simmask);
#else /* !defined(_MKE02Z4_H_) */
            SIM->SCGC |= (spi_config[bus].simmask);
#endif /* !defined(_MKE02Z4_H_) */

            break;

#ifdef SPI2
        case (uint32_t)SPI2:
            SIM->SCGC3 |= (spi_config[bus].simmask);
            break;
#endif
    }
}

static inline void poweroff(spi_t bus)
{
    switch((uint32_t)dev(bus)) {
        case (uint32_t)SPI0:
        case (uint32_t)SPI1:

#if !defined(_MKE02Z4_H_)
            SIM->SCGC6 &= ~(spi_config[bus].simmask);
#else /* !defined(_MKE02Z4_H_) */
            SIM->SCGC &= ~(spi_config[bus].simmask);
#endif /* !defined(_MKE02Z4_H_) */

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

#if !defined(_MKE02Z4_H_)
    /* trigger pin initialization */
    spi_init_pins(bus);
#endif /* !defined(_MKE02Z4_H_) */

    /* power on the bus temporarily */
    poweron(bus);

    /* make the base configuration: configure as SPI master, set CS inactive
     * state (for HWCS lines) and clear FIFO counters and disable FIFOs */
#if !defined(_MKE02Z4_H_)
    dev(bus)->MCR = (SPI_MCR_MSTR_MASK | SPI_MCR_PCSIS_MASK |
                     SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK |
                     SPI_MCR_DIS_RXF_MASK | SPI_MCR_DIS_TXF_MASK |
                     SPI_MCR_DOZE_MASK | SPI_MCR_HALT_MASK | SPI_MCR_MDIS_MASK);

    /* disable all DMA and interrupt requests */
    dev(bus)->RSER = 0;
#else /* !defined(_MKE02Z4_H_) */
    dev(bus)->C1 |= SPI_C1_MSTR_MASK; /* Operate in SPI master mode */
    dev(bus)->C1 &= ~(SPI_C1_SPIE_MASK); /* Disable interrupts: RX buffer full and mode fault */
    dev(bus)->C1 &= ~(SPI_C1_SPTIE_MASK); /* Disable interrupts: TX buffer empty */
    dev(bus)->C2 &= ~(SPI_C2_SPMIE_MASK); /* Disable interrupts: RX buffer match */

    /* Initially configure and use HWCS pins */
    dev(bus)->C1 |= SPI_C1_SSOE_MASK; /* Enable SS output */
    dev(bus)->C2 |= SPI_C2_MODFEN_MASK; /* Enable master mode fault function */

    /* Disable SPI unit */
    dev(bus)->C1 &= ~(SPI_C1_SPE_MASK);
#endif /* !defined(_MKE02Z4_H_) */

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

#if !defined(_MKE02Z4_H_)
        gpio_init_port(spi_config[bus].pin_cs[cs], spi_config[bus].pcr);
#endif /* !defined(_MKE02Z4_H_) */
    }

    return SPI_OK;
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void) cs;
    /* lock and power on the bus */
    mutex_lock(&locks[bus]);
    poweron(bus);

    /* enable the device */
#if !defined(_MKE02Z4_H_)
    dev(bus)->MCR &= ~(SPI_MCR_HALT_MASK | SPI_MCR_MDIS_MASK);
#else /* !defined(_MKE02Z4_H_) */
    dev(bus)->C1 |= (SPI_C1_SPE_MASK);
#endif /* !defined(_MKE02Z4_H_) */

    /* configure clock and mode */
#if !defined(_MKE02Z4_H_)
    dev(bus)->CTAR[0] = (mode | SPI_CTAR_FMSZ(7) | spi_clk_config[clk]);
#else /* !defined(_MKE02Z4_H_) */
    dev(bus)->BR = spi_clk_config[clk];

    if (mode == SPI_MODE_0) {
        dev(bus)->C1 &= ~(SPI_C1_CPOL_MASK | SPI_C1_CPHA_MASK);
    }
    else if (mode == SPI_MODE_1) {
        dev(bus)->C1 &= ~(SPI_C1_CPOL_MASK);
        dev(bus)->C1 |= (SPI_C1_CPHA_MASK);
    }
    else if (mode == SPI_MODE_2) {
        dev(bus)->C1 &= ~(SPI_C1_CPHA_MASK);
        dev(bus)->C1 |= (SPI_C1_CPOL_MASK);
    }
    else if (mode == SPI_MODE_3) {
        dev(bus)->C1 |= (SPI_C1_CPOL_MASK | SPI_C1_CPHA_MASK);
    }

#endif /* !defined(_MKE02Z4_H_) */

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    /* disable, power off, and unlock the bus */
#if !defined(_MKE02Z4_H_)
    dev(bus)->MCR |= (SPI_MCR_HALT_MASK | SPI_MCR_MDIS_MASK);
#else /* !defined(_MKE02Z4_H_) */
    dev(bus)->C1 &= ~(SPI_C1_SPE_MASK); /* SPI unit disabled */
#endif /* !defined(_MKE02Z4_H_) */

    poweroff(bus);
    mutex_unlock(&locks[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

#if !defined(_MKE02Z4_H_)
    uint32_t flags = SPI_PUSHR_CONT_MASK;
#endif /* !defined(_MKE02Z4_H_) */

    assert(out_buf || in_buf);

    /* handle chip select */
    if (cs != SPI_CS_UNDEF) {
        if (cs & SWCS_MASK) {
            gpio_clear((gpio_t)cs);
        }
        else {
#if !defined(_MKE02Z4_H_)
            flags |= (uint32_t)(1 << (cs + SPI_PUSHR_PCS_SHIFT));
#endif /* !defined(_MKE02Z4_H_) */
        }
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;
        if ((i == (len - 1)) && (!cont)) {
#if !defined(_MKE02Z4_H_)
            flags &= ~(SPI_PUSHR_CONT_MASK);
#endif /* !defined(_MKE02Z4_H_) */
        }

#if !defined(_MKE02Z4_H_)
        while (!(dev(bus)->SR & SPI_SR_TFFF_MASK)) {}
        dev(bus)->PUSHR = (tmp | flags);

        while (!(dev(bus)->SR & SPI_SR_RXCTR_MASK)) {}
        tmp = dev(bus)->POPR;
#else /* !defined(_MKE02Z4_H_) */
        while (!(dev(bus)->S & SPI_S_SPTEF_MASK)) {}
        dev(bus)->D = tmp;

        while (!(dev(bus)->S & SPI_S_SPRF_MASK)) {}
        tmp = dev(bus)->D;
#endif /* !defined(_MKE02Z4_H_) */

        if (in_buf) {
            in_buf[i] = tmp;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF) && (cs & SWCS_MASK)) {
        gpio_set((gpio_t)cs);
    }
}
