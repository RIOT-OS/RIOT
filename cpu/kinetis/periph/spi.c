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
#include "periph/gpio.h"
#include "periph/spi.h"
#include "bitarithm.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/* DIV_UP is division which rounds up instead of down */
#define SPI_DIV_UP(a, b)    (((a) + ((b) - 1)) / (b))

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
    switch ((uint32_t)dev(bus)) {
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

    switch ((uint32_t)dev(bus)) {
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
    gpio_init_port(spi_config[bus].pin_clk, spi_config[bus].pcr);
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
        if ((cs >= SPI_HWCS_NUMOF) || !gpio_is_valid(spi_config[bus].pin_cs[cs])) {
            return SPI_NOCS;
        }
        gpio_init_port(spi_config[bus].pin_cs[cs], spi_config[bus].pcr);
    }

    return SPI_OK;
}

spi_clk_t spi_get_clk(spi_t bus, uint32_t freq)
{
    (void)bus;
    /* Possible prescalers: 2, 3, 5, 7
     * Possible scalers: 2, 4, 6 (sic!), 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
     *
     * SCK baud rate = CLOCK_CORECLOCK / (PBR * BR) */

    uint32_t target_divider, divider, scaler;
    uint8_t prescaler, br, pbr = 0, cssck, pcssck = 0;

    /* bound divider from 4 to 229376 */
    if (freq > CLOCK_BUSCLOCK / 4) {
        freq = CLOCK_BUSCLOCK / 4;
    }
    assert(freq >= SPI_DIV_UP(CLOCK_BUSCLOCK, 229376));

    /* baudrate scalers */
    target_divider = SPI_DIV_UP(CLOCK_BUSCLOCK, freq);
    divider = 4;
    prescaler = 2;
    scaler = 2;
    while (divider < target_divider && divider != 229376) {
        switch (divider) {
            case 4: prescaler = 3; break;
            case 6: prescaler = 2; scaler = 4; break;
            case 16: prescaler = 3; scaler = 6; break;
            case 18: prescaler = 5; scaler = 4; break;
            case 28: prescaler = 5; scaler = 6; break;
            case 30: prescaler = 2; scaler = 16; break;
            case 40: prescaler = 7; scaler = 6; break;
            case 42: prescaler = 3; scaler = 16; break;
            case 114688: prescaler = 5; scaler = 32768; break;
            case 196608: prescaler = 7; scaler = 32768; break;
            default: switch (prescaler) {
                case 2: prescaler = 5; scaler /= 2; break;
                case 5: prescaler = 3; scaler *= 2; break;
                case 3: prescaler = 7; scaler /= 2; break;
                case 7: prescaler = 2; scaler *= 4; break;
            }
            break;
        }
        divider = prescaler * scaler;
    }
    switch (prescaler) {
        case 2: pbr = 0; break;
        case 3: pbr = 1; break;
        case 5: pbr = 2; break;
        case 7: pbr = 3; break;
    }
    br = scaler < 6 ? bitarithm_msb(scaler) - 1 : bitarithm_msb(scaler);

    /* delay scalers
     * Possible prescalers: 1, 3, 5, 7
     * Possible scalers: 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536
     */
    divider = 2;
    prescaler = 1;
    scaler = 2;
    while ( divider < target_divider) {
        switch (divider) {
            case 2: scaler = 4; break;
            case 4: prescaler = 3; scaler = 2; break;
            case 6: prescaler = 1; scaler = 8; break;
            default: switch (prescaler) {
                case 1: prescaler = 5; scaler /= 4; break;
                case 5: prescaler = 3; scaler *= 2; break;
                case 3: prescaler = 7; scaler /= 2; break;
                case 7: prescaler = 1; scaler *= 8; break;
            }
            break;
        }
        divider = prescaler * scaler;
    }
    switch (prescaler) {
        case 1: pcssck = 0; break;
        case 3: pcssck = 1; break;
        case 5: pcssck = 2; break;
        case 7: pcssck = 3; break;
    }
    cssck = bitarithm_msb(scaler) - 1;

    return
        SPI_CTAR_PBR(pbr) | SPI_CTAR_BR(br) |
        SPI_CTAR_PCSSCK(pcssck) | SPI_CTAR_CSSCK(cssck) |
        SPI_CTAR_PASC(pcssck) | SPI_CTAR_ASC(cssck) |
        SPI_CTAR_PDT(pcssck) | SPI_CTAR_DT(cssck);
}

uint32_t spi_get_freq(spi_t bus, spi_clk_t clk)
{
    (void)bus;

    static const int br_scalers[16] = {
        2,     4,     6,     8,    16,    32,    64,   128,
        256,   512,  1024,  2048,  4096,  8192, 16384, 32768
    };
    static const int br_prescalers[4] = {2, 3, 5, 7};

    int pbr = br_prescalers[clk >> 16 & 0x3];
    int br = br_scalers[clk & 0xf];

    return CLOCK_BUSCLOCK / (pbr * br);
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
    dev(bus)->CTAR[0] = (mode | SPI_CTAR_FMSZ(7) | clk);
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
