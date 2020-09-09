/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"
#include "pm_layered.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF];

#ifdef MODULE_PERIPH_DMA
struct dma_state {
    dma_t tx_dma;
    dma_t rx_dma;
};

static struct dma_state _dma_state[SPI_NUMOF];

static DmacDescriptor DMA_DESCRIPTOR_ATTRS tx_desc[SPI_NUMOF];
static DmacDescriptor DMA_DESCRIPTOR_ATTRS rx_desc[SPI_NUMOF];
#endif

/**
 * @brief   Shortcut for accessing the used SPI SERCOM device
 */
static inline SercomSpi *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

static inline void poweron(spi_t bus)
{
    sercom_clk_en(dev(bus));
}

static inline void poweroff(spi_t bus)
{
    sercom_clk_dis(dev(bus));
}

static inline bool _use_dma(spi_t bus)
{
#ifdef MODULE_PERIPH_DMA
    return (spi_config[bus].tx_trigger != DMA_TRIGGER_DISABLED) &&
           (spi_config[bus].rx_trigger != DMA_TRIGGER_DISABLED);
#else
    (void)bus;
    return false;
#endif
}

void spi_init(spi_t bus)
{
    /* make sure given bus is good */
    assert(bus < SPI_NUMOF);

    /* initialize the device lock */
    mutex_init(&locks[bus]);

    /* configure pins and their muxes */
    spi_init_pins(bus);

    /* wake up device */
    poweron(bus);

    /* reset all device configuration */
    dev(bus)->CTRLA.reg |= SERCOM_SPI_CTRLA_SWRST;
    while ((dev(bus)->CTRLA.reg & SERCOM_SPI_CTRLA_SWRST) ||
           (dev(bus)->SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_SWRST)) {}

    /* configure base clock: using GLK GEN 0 */
    sercom_set_gen(dev(bus), spi_config[bus].gclk_src);

    /* enable receiver and configure character size to 8-bit
     * no synchronization needed, as SERCOM device is not enabled */
    dev(bus)->CTRLB.reg = (SERCOM_SPI_CTRLB_CHSIZE(0) | SERCOM_SPI_CTRLB_RXEN);

#ifdef MODULE_PERIPH_DMA
    if (_use_dma(bus)) {
        _dma_state[bus].rx_dma = dma_acquire_channel();
        _dma_state[bus].tx_dma = dma_acquire_channel();
        dma_setup(_dma_state[bus].tx_dma,
                  spi_config[bus].tx_trigger, 0, false);
        dma_setup(_dma_state[bus].rx_dma,
                  spi_config[bus].rx_trigger, 1, true);
        dma_prepare(_dma_state[bus].rx_dma, DMAC_BTCTRL_BEATSIZE_BYTE_Val,
                    (void*)&dev(bus)->DATA.reg, NULL, 1, 0);
        dma_prepare(_dma_state[bus].tx_dma, DMAC_BTCTRL_BEATSIZE_BYTE_Val,
                    NULL, (void*)&dev(bus)->DATA.reg, 0, 0);
    }
#endif
    /* put device back to sleep */
    poweroff(bus);

}

void spi_init_pins(spi_t bus)
{
    /* MISO must always have PD/PU, see #5968. This is a ~65uA difference */
    gpio_init(spi_config[bus].miso_pin, GPIO_IN_PD);
    gpio_init(spi_config[bus].mosi_pin, GPIO_OUT);
    gpio_init(spi_config[bus].clk_pin, GPIO_OUT);
    gpio_init_mux(spi_config[bus].miso_pin, spi_config[bus].miso_mux);
    gpio_init_mux(spi_config[bus].mosi_pin, spi_config[bus].mosi_mux);
    /* clk_pin will be muxed during acquire / release */

    mutex_unlock(&locks[bus]);
}

void spi_deinit_pins(spi_t bus)
{
    mutex_lock(&locks[bus]);

    gpio_disable_mux(spi_config[bus].miso_pin);
    gpio_disable_mux(spi_config[bus].mosi_pin);
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;

    /* configure bus clock, in synchronous mode its calculated from
     * BAUD.reg = (f_ref / (2 * f_bus) - 1)
     * with f_ref := CLOCK_CORECLOCK as defined by the board
     * to mitigate the rounding error due to integer arithmetic, the
     * equation is modified to
     * BAUD.reg = ((f_ref + f_bus) / (2 * f_bus) - 1) */
    const uint8_t baud = ((sam0_gclk_freq(spi_config[bus].gclk_src) + clk) / (2 * clk) - 1);

    /* configure device to be master and set mode and pads,
     *
     * NOTE: we could configure the pads already during spi_init, but for
     * efficiency reason we do that here, so we can do all in one single write
     * to the CTRLA register */
    const uint32_t ctrla = SERCOM_SPI_CTRLA_MODE(0x3)       /* 0x3 -> master */
                           | SERCOM_SPI_CTRLA_DOPO(spi_config[bus].mosi_pad)
                           | SERCOM_SPI_CTRLA_DIPO(spi_config[bus].miso_pad)
                           | (mode << SERCOM_SPI_CTRLA_CPHA_Pos);

    /* get exclusive access to the device */
    mutex_lock(&locks[bus]);

    /* power on the device */
    poweron(bus);

    /* first configuration or reconfiguration after altered device usage */
    if (dev(bus)->BAUD.reg != baud || dev(bus)->CTRLA.reg != ctrla) {
        /* disable the device */
        dev(bus)->CTRLA.reg &= ~(SERCOM_SPI_CTRLA_ENABLE);
        while (dev(bus)->SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_ENABLE) {}

        dev(bus)->BAUD.reg = baud;
        dev(bus)->CTRLA.reg = ctrla;
        /* no synchronization needed here, the enable synchronization below
         * acts as a write-synchronization for both registers */
    }

    /* finally enable the device */
    dev(bus)->CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
    while (dev(bus)->SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_ENABLE) {}

    /* mux clk_pin to SPI peripheral */
    gpio_init_mux(spi_config[bus].clk_pin, spi_config[bus].clk_mux);

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    /* Demux clk_pin back to GPIO_OUT function. Otherwise it will get HIGH-Z
     * and lead to unexpected current draw by SPI salves. */
    gpio_disable_mux(spi_config[bus].clk_pin);

    /* disable the device */
    dev(bus)->CTRLA.reg &= ~(SERCOM_SPI_CTRLA_ENABLE);
    while (dev(bus)->SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_ENABLE) {}

    /* power off the device */
    poweroff(bus);

    /* release access to the device */
    mutex_unlock(&locks[bus]);
}

static void _blocking_transfer(spi_t bus, const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    for (int i = 0; i < (int)len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;
        dev(bus)->DATA.reg = tmp;
        while (!(dev(bus)->INTFLAG.reg & SERCOM_SPI_INTFLAG_RXC)) {}
        tmp = (uint8_t)dev(bus)->DATA.reg;
        if (in_buf) {
            in_buf[i] = tmp;
        }
    }
}

#ifdef MODULE_PERIPH_DMA

static void _dma_execute(spi_t bus)
{
#if defined(CPU_COMMON_SAMD21)
    pm_block(SAMD21_PM_IDLE_1);
#endif
    dma_start(_dma_state[bus].rx_dma);
    dma_start(_dma_state[bus].tx_dma);

    dma_wait(_dma_state[bus].rx_dma);
#if defined(CPU_COMMON_SAMD21)
    pm_unblock(SAMD21_PM_IDLE_1);
#endif
}

static void _dma_transfer(spi_t bus, const uint8_t *out, uint8_t *in,
                          size_t len)
{
    uint8_t tmp = 0;
    const uint8_t *out_addr = out ? out + len : &tmp;
    uint8_t *in_addr = in ? in + len : &tmp;
    dma_prepare_dst(_dma_state[bus].rx_dma, in_addr, len, in ? true : false);
    dma_prepare_src(_dma_state[bus].tx_dma, out_addr, len, out ? true : false);
    _dma_execute(bus);
}

static void _dma_transfer_regs(spi_t bus, uint8_t reg, const uint8_t *out,
                               uint8_t *in, size_t len)
{
    uint8_t tmp;
    const uint8_t *out_addr = out ? out + len : &tmp;
    uint8_t *in_addr = in ? in + len : &tmp;

    dma_prepare_dst(_dma_state[bus].rx_dma, &tmp, 1, false);
    dma_prepare_src(_dma_state[bus].tx_dma, &reg, 1, false);

    dma_append_dst(_dma_state[bus].rx_dma, &rx_desc[bus], in_addr,
                   len, in ? true : false);
    dma_append_src(_dma_state[bus].tx_dma, &tx_desc[bus], out_addr,
                   len, out ? true : false);

    _dma_execute(bus);
}
void spi_transfer_regs(spi_t bus, spi_cs_t cs,
                       uint8_t reg, const void *out, void *in, size_t len)
{
    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    if (_use_dma(bus)) {
        /* The DMA promises not to modify the const out data */
        _dma_transfer_regs(bus, reg, out, in, len);
    }
    else {
        _blocking_transfer(bus, &reg, NULL, 1);
        _blocking_transfer(bus, out, in, len);
    }

    if (cs != SPI_CS_UNDEF) {
        gpio_set((gpio_t)cs);
    }
}

uint8_t spi_transfer_reg(spi_t bus, spi_cs_t cs, uint8_t reg, uint8_t out)
{
    uint8_t res;
    spi_transfer_regs(bus, cs, reg, &out, &res, 1);
    return res;
}

#endif /* MODULE_PERIPH_DMA */


void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    assert(out || in);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }
    if (_use_dma(bus)) {
#ifdef MODULE_PERIPH_DMA
        /* The DMA promises not to modify the const out data */
        _dma_transfer(bus, out, in, len);
#endif
    }
    else {
        _blocking_transfer(bus, out, in, len);
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}
