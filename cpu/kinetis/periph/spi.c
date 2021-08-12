/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2014 PHYTEC Messtechnik GmbH
 *               2015 Eistec AB
 *               2016 Freie Universität Berlin
 *               2021-2023 Hugues Larrive
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
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "macros/math.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "bitarithm.h"

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
    /* Baud rate:
     *
     * Possible prescalers: 2, 3, 5, 7
     * Possible scalers: 2, 4, 6, 8, 16, 32, 64, 128, 256, 512, 1024,
     *                   2048, 4096, 8192, 16384, 32768
     *
     * SCK baud rate = CLOCK_BUSCLOCK / (PBR * BR)
     *
     * Delays:
     *
     * Possible prescalers: 1, 3, 5, 7
     * Possible scalers: 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048,
     *                   4096, 8192, 16384, 32768, 65536
     *
     * tCSC = (1 / CLOCK_BUSCLOCK) * PCSSCK * CSSCK
     * tASC = (1 / CLOCK_BUSCLOCK) * PASC * ASC
     * tDT = (1 / CLOCK_BUSCLOCK) * PDT * DT
     */

    const uint32_t target_divider = DIV_ROUND_UP(CLOCK_BUSCLOCK, freq);
    uint32_t divider = 4, scaler = 2;
    uint8_t prescaler = 2, pbr = 0, br, pdt, dt;

    /* bound divider to 229376 */
    if (freq < DIV_ROUND_UP(CLOCK_BUSCLOCK, 229376)) {
        return (spi_clk_t){ .err = -EDOM };
    }

    /* Baudrate scalers
     *
     * Sorting out dividers:
     *  previous              previous
     * prescaler   prescaler   scaler             divider
     *     7        -> 2     *  (0.5  * 4) = 2 * 2 = 4
     *     2        -> 5     *   (2   / 2) = 5 * 1 = 5 scaler underflow
     *     5        -> 3     *   (1   * 2) = 3 * 2 = 6
     *     3        -> 7     *   (2   / 2) = 7 * 1 = 7 scaler underflow
     *     7        -> 2     *   (1   * 4) = 2 * 4 = 8
     *     2        -> 5     *   (4   / 2) = 5 * 2 = 10
     *     5        -> 3     *   (2   * 2) = 3 * 4 = 12
     *     3        -> 7     *   (4   / 2) = 7 * 2 = 14
     *     7        -> 2     *   (2   * 4) = 2 * 8 = 16
     * first exception for scaler 6:         3 * 6 = 18
     *     2        -> 5     *   (8   / 2) = 5 * 4 = 20
     *     5        -> 3     *   (4   * 2) = 3 * 8 = 24
     *     3        -> 7     *   (8   / 2) = 7 * 4 = 28
     * second exception for scaler 6:        5 * 6 = 30
     *     7        -> 2     *   (4   * 4) = 2 * 16 = 32
     *     2        -> 5     *  (16   / 2) = 5 * 8 = 40
     * third exception for scaler 6:         7 * 6 = 42
     *     5        -> 3     *   (8   * 2) = 3 * 16 = 48
     * from now on, the scaler will never go back below 8
     *  prescaler 3 -> 7      scaler /= 2
     *  prescaler 7 -> 2      scaler *= 4
     *  prescaler 2 -> 5      scaler /= 2
     *  prescaler 5 -> 3      scaler *= 2
     *              |                 |
     *     3        -> 7     * (32768 / 2) = 7 * 16384 = 114688
     *     7        -> 2     * (16384 * 4) = 2 * 65536 = 131072 scaler overflow
     *     2        -> 5     * (65536 / 2) = 5 * 32768 = 163840
     *     5        -> 3     * (32768 * 2) = 3 * 65536 = 196608 scaler overflow
     *     3        -> 7     * (65536 / 2) = 7 * 32768 = 229376
     */
    while (divider < target_divider) {
        switch (divider) {
            /* do not underflow the scaler */
            case 4: prescaler = 3; break;
            case 6: prescaler = 2; scaler = 4; break;
            /* first exception for the scaler 6 */
            case 16: prescaler = 3; scaler = 6; break;
            case 18: prescaler = 5; scaler = 4; break;
            /* second exception for the scaler 6 */
            case 28: prescaler = 5; scaler = 6; break;
            case 30: prescaler = 2; scaler = 16; break;
            /* third exception for the scaler 6 */
            case 40: prescaler = 7; scaler = 6; break;
            case 42: prescaler = 3; scaler = 16; break;
            /* do not overflow the scaler */
            case 114688: prescaler = 5; scaler = 32768; break;
            case 163840: prescaler = 7; scaler = 32768; break;
            /* default progress */
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
    /* convert prescaler to pbr */
    switch (prescaler) {
        case 2: pbr = 0; break;
        case 3: pbr = 1; break;
        case 5: pbr = 2; break;
        case 7: pbr = 3; break;
    }
    /* convert scaler to br */
    br = scaler < 6 ? bitarithm_msb(scaler) - 1 : bitarithm_msb(scaler);

    /* Delay scalers
     *
     * We will use delays of at least one clock period.
     *
     * The delay scalers computation is practically the same as the
     * baudrate scalers so we will reuse the previous results.
     *
     * There is the scaler 65536 but we will not handle it as this would
     * lead into delays less than one clock period.
     */
    switch (divider) {
        /* handle the 3 exceptions (there is not the scaler 6) */
        case 18: pdt = 2; dt = 1; break; /* -> 20 */
        case 30: pdt = 0; dt = 4; break; /* -> 32 */
        case 42: pdt = 1; dt = 3; break; /* -> 48 */
        /* add 1 to br when pbr is 0 (the first prescaler is 1 instead of 2) */
        default: pdt = pbr; dt = pbr ? br : br + 1; break;
    }
    if (br > 2) {
        /* there is not the scaler 6 */
        --dt;
    }

    return (spi_clk_t){
        .clk =  SPI_CTAR_PBR(pbr) | SPI_CTAR_BR(br) |
                SPI_CTAR_PCSSCK(pdt) | SPI_CTAR_CSSCK(dt) |
                SPI_CTAR_PASC(pdt) | SPI_CTAR_ASC(dt) |
                SPI_CTAR_PDT(pdt) | SPI_CTAR_DT(dt)
    };
}

int32_t spi_get_freq(spi_t bus, spi_clk_t clk)
{
    (void)bus;
    if (clk.err) { return -EINVAL; }

    static const int br_scalers[16] = {
        2,     4,     6,     8,    16,    32,    64,   128,
        256,   512,  1024,  2048,  4096,  8192, 16384, 32768
    };
    static const int br_prescalers[4] = {2, 3, 5, 7};

    int pbr = br_prescalers[clk.clk >> 16 & 0x3];
    int br = br_scalers[clk.clk & 0xf];

    return CLOCK_BUSCLOCK / (pbr * br);
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);
    if (clk.err) { return; }

    /* lock and power on the bus */
    mutex_lock(&locks[bus]);
    poweron(bus);

    /* enable the device */
    dev(bus)->MCR &= ~(SPI_MCR_HALT_MASK | SPI_MCR_MDIS_MASK);

    /* configure clock and mode */
    dev(bus)->CTAR[0] = (mode | SPI_CTAR_FMSZ(7) | clk.clk);
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
