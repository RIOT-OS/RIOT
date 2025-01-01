/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *               2022 SSV Software Systems GmbH
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
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if MODULE_PERIPH_SPI_ON_QSPI
#  define QSPI_NUMOF 1
#else
#  define QSPI_NUMOF 0
#endif

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

static SercomSpi *_dev(spi_t bus)
{
    return spi_config[bus].dev;
}

static inline bool _is_qspi(spi_t bus)
{
#ifdef MODULE_PERIPH_SPI_ON_QSPI
    return (void*)spi_config[bus].dev == (void*)QSPI;
#else
    (void)bus;
    return false;
#endif
}

static inline void _qspi_clk_enable(void)
{
#ifdef QSPI
    /* enable QSPI clock */
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_QSPI;
#endif
}

static inline void _qspi_clk_disable(void)
{
#ifdef QSPI
    /* disable QSPI clock */
    MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_QSPI;
#endif
}

static inline void poweron(spi_t bus)
{
    if (_is_qspi(bus)) {
        _qspi_clk_enable();
    }
}

static inline void poweroff(spi_t bus)
{
    if (_is_qspi(bus)) {
        _qspi_clk_disable();
    }
}

static inline void _enable(SercomSpi *dev)
{
    dev->CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;

#ifdef SERCOM_SPI_STATUS_SYNCBUSY
    while (dev->STATUS.reg & SERCOM_SPI_STATUS_SYNCBUSY) {}
#else
    while (dev->SYNCBUSY.reg) {}
#endif
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

static inline void _init_dma(spi_t bus, const volatile void *reg_rx, volatile void *reg_tx)
{
    if (!_use_dma(bus)) {
        return;
    }

#ifdef MODULE_PERIPH_DMA
    _dma_state[bus].rx_dma = dma_acquire_channel();
    _dma_state[bus].tx_dma = dma_acquire_channel();

    dma_setup(_dma_state[bus].tx_dma,
              spi_config[bus].tx_trigger, 0, false);
    dma_setup(_dma_state[bus].rx_dma,
              spi_config[bus].rx_trigger, 1, true);

    dma_prepare(_dma_state[bus].rx_dma, DMAC_BTCTRL_BEATSIZE_BYTE_Val,
                (void*)reg_rx, NULL, 1, 0);
    dma_prepare(_dma_state[bus].tx_dma, DMAC_BTCTRL_BEATSIZE_BYTE_Val,
                NULL, (void*)reg_tx, 0, 0);
#else
    (void)reg_rx;
    (void)reg_tx;
#endif
}

/**
 * @brief   QSPI peripheral in SPI mode
 * @{
 */
#ifdef QSPI
static void _init_qspi(spi_t bus)
{
    /* reset the peripheral */
    QSPI->CTRLA.reg |= QSPI_CTRLA_SWRST;

    QSPI->CTRLB.reg = QSPI_CTRLB_MODE_SPI
                    | QSPI_CTRLB_CSMODE_LASTXFER
                    | QSPI_CTRLB_DATALEN_8BITS;

    /* set up DMA channels */
    _init_dma(bus, &QSPI->RXDATA.reg, &QSPI->TXDATA.reg);
}

static void _qspi_acquire(spi_mode_t mode, spi_clk_t clk)
{
    /* datasheet says SCK = MCK / (BAUD + 1) */
    /* but BAUD = 0 does not work, assume SCK = MCK / BAUD */
    uint32_t baud = CLOCK_CORECLOCK > (2 * clk)
                  ? (CLOCK_CORECLOCK + clk - 1) / clk
                  : 1;

    /* bit order is reversed from SERCOM SPI */
    uint32_t _mode = (mode >> 1)
                   | (mode << 1);
    _mode &= 0x3;

    QSPI->CTRLA.reg |= QSPI_CTRLA_ENABLE;
    QSPI->BAUD.reg = QSPI_BAUD_BAUD(baud) | _mode;
}

static inline void _qspi_release(void)
{
    QSPI->CTRLA.reg &= ~QSPI_CTRLA_ENABLE;
}

static void _qspi_blocking_transfer(const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = out_buf ? out_buf[i] : 0;

        /* transmit byte on MOSI */
        QSPI->TXDATA.reg = tmp;

        /* wait until byte has been sampled on MISO */
        while (!(QSPI->INTFLAG.reg & QSPI_INTFLAG_RXC)) {}

        /* consume the byte */
        tmp = QSPI->RXDATA.reg;

        if (in_buf) {
            in_buf[i] = tmp;
        }
    }
}
#else /* !QSPI */
void _init_qspi(spi_t bus);
void _qspi_acquire(spi_mode_t mode, spi_clk_t clk);
void _qspi_release(void);
void _qspi_blocking_transfer(const void *out, void *in, size_t len);
#endif
/** @} */

/**
 * @brief   SERCOM peripheral in SPI mode
 * @{
 */
static void _init_spi(spi_t bus)
{
    SercomSpi *dev = _dev(bus);

    /* set up DMA channels */
    _init_dma(bus, &dev->DATA.reg, &dev->DATA.reg);
}

static void _spi_acquire(spi_t bus, spi_mode_t mode, spi_clk_t clk)
{
    sercom_t sercom = sercom_id(spi_config[bus].dev);
    DEBUG("[spi] acquire bus %u on SERCOM %u\n",
          (unsigned)bus, (unsigned)sercom);

    /* clock can't be higher than source clock */
    uint32_t gclk_src = sam0_gclk_freq(spi_config[bus].gclk_src);
    if (clk > gclk_src) {
        clk = gclk_src;
    }

    /* configure bus clock, in synchronous mode its calculated from
     * BAUD.reg = (f_ref / (2 * f_bus) - 1)
     * with f_ref := CLOCK_CORECLOCK as defined by the board
     * to mitigate the rounding error due to integer arithmetic, the
     * equation is modified to
     * BAUD.reg = ((f_ref + f_bus) / (2 * f_bus) - 1) */
    uint8_t baud = (gclk_src + clk) / (2 * clk) - 1;

    /* configure device to be master and set mode and pads,
     *
     * NOTE: we could configure the pads already during spi_init, but for
     * efficiency reason we do that here, so we can do all in one single write
     * to the CTRLA register */
    uint32_t ctrla = SERCOM_SPI_CTRLA_MODE(0x3)       /* 0x3 -> master */
                   | SERCOM_SPI_CTRLA_DOPO(spi_config[bus].mosi_pad)
                   | SERCOM_SPI_CTRLA_DIPO(spi_config[bus].miso_pad)
                   | (mode << SERCOM_SPI_CTRLA_CPHA_Pos);
    uint32_t ctrlb = SERCOM_SPI_CTRLB_CHSIZE(0) | SERCOM_SPI_CTRLB_RXEN;

    sercom_acquire(sercom, spi_config[bus].gclk_src, NULL, NULL);

    SercomSpi *dev = _dev(bus);
    dev->CTRLA.reg = ctrla;
    dev->CTRLB.reg = ctrlb;
    dev->BAUD.reg = baud;

    /* finally enable the device */
    _enable(dev);
}

static void _spi_blocking_transfer(spi_t bus, const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;
    SercomSpi *dev = _dev(bus);

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;

        /* transmit byte on MOSI */
        dev->DATA.reg = tmp;

        /* wait until byte has been sampled on MISO */
        while (!(dev->INTFLAG.reg & SERCOM_SPI_INTFLAG_RXC)) {}

        /* consume the byte */
        tmp = dev->DATA.reg;

        if (in_buf) {
            in_buf[i] = tmp;
        }
    }
}
/** @} */

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

    if (_is_qspi(bus)) {
        _init_qspi(bus);
    } else {
        _init_spi(bus);
    }

    /* put device back to sleep */
    poweroff(bus);
}

static void _attach_pins(spi_t bus)
{
    if (gpio_is_valid(spi_config[bus].mosi_pin)) {
        gpio_init_mux(spi_config[bus].mosi_pin, spi_config[bus].mosi_mux);
    }

    if (gpio_is_valid(spi_config[bus].miso_pin)) {
        gpio_init_mux(spi_config[bus].miso_pin, spi_config[bus].miso_mux);
    }

    if (gpio_is_valid(spi_config[bus].clk_pin)) {
        gpio_init_mux(spi_config[bus].clk_pin, spi_config[bus].clk_mux);
    }
}

static void _detach_pins(spi_t bus)
{
    if (gpio_is_valid(spi_config[bus].mosi_pin)) {
        gpio_disable_mux(spi_config[bus].mosi_pin);
    }

    if (gpio_is_valid(spi_config[bus].miso_pin)) {
        gpio_disable_mux(spi_config[bus].miso_pin);
    }

    if (gpio_is_valid(spi_config[bus].clk_pin)) {
        gpio_disable_mux(spi_config[bus].clk_pin);
    }
}

int spi_init_with_gpio_mode(spi_t bus, const spi_gpio_mode_t* mode)
{
    assert(bus < SPI_NUMOF);

    if (gpio_is_valid(spi_config[bus].mosi_pin)) {
        gpio_init(spi_config[bus].miso_pin, mode->mosi);
    }

    if (gpio_is_valid(spi_config[bus].miso_pin)) {
        gpio_init(spi_config[bus].mosi_pin, mode->miso);
    }

    if (gpio_is_valid(spi_config[bus].clk_pin)) {
        gpio_init(spi_config[bus].clk_pin, mode->sclk);
    }
    mutex_unlock(&locks[bus]);

    return 0;
}

void spi_init_pins(spi_t bus)
{
    const spi_gpio_mode_t gpio_modes = {
        .mosi = GPIO_OUT,
        /* MISO must always have PD/PU, see #5968. This is a ~65uA difference */
        .miso = GPIO_IN_PD,
        .sclk = GPIO_OUT,
    };
    spi_init_with_gpio_mode(bus, &gpio_modes);
}

void spi_deinit_pins(spi_t bus)
{
    /* pins are detached by default, so that other users of the SERCOM can
     * route pins to the SERCOM. We just need to make sure calls to
     * `spi_acquire()` will block until pins are returned to the SPI bus, which
     * we use the mutex for. */
    mutex_lock(&locks[bus]);
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);

    /* Get exclusive access to the SPI pins. (Exclusive access to the SERCOM
     * is internally already managed, but spi_deinit_pins() will get GPIO
     * access to the SPI pins exclusively.)
     */
    mutex_lock(&locks[bus]);

    /* power on the device */
    poweron(bus);

    if (_is_qspi(bus)) {
        _qspi_acquire(mode, clk);
    }
    else {
        _spi_acquire(bus, mode, clk);
        _attach_pins(bus);
    }

}

void spi_release(spi_t bus)
{
    if (_is_qspi(bus)) {
        DEBUG("[spi] release bus %u on QSPI\n", (unsigned)bus);
        _qspi_release();

        /* power off the device */
        poweroff(bus);
    }
    else {
        sercom_t sercom = sercom_id(spi_config[bus].dev);
        DEBUG("[spi] release bus %u on SERCOM %u\n",
              (unsigned)bus, (unsigned)sercom);
        _detach_pins(bus);
        sercom_release(sercom);
    }

    /* release access to the device */
    mutex_unlock(&locks[bus]);
}

static void _blocking_transfer(spi_t bus, const void *out, void *in, size_t len)
{
    if (_is_qspi(bus)) {
        _qspi_blocking_transfer(out, in, len);
    }
    else {
        _spi_blocking_transfer(bus, out, in, len);
    }
}

#ifdef MODULE_PERIPH_DMA

static void _dma_execute(spi_t bus)
{
#if IS_ACTIVE(MODULE_PM_LAYERED) && defined(SAM0_SPI_PM_BLOCK)
    pm_block(SAM0_SPI_PM_BLOCK);
#endif
    dma_start(_dma_state[bus].rx_dma);
    dma_start(_dma_state[bus].tx_dma);

    dma_wait(_dma_state[bus].rx_dma);
#if IS_ACTIVE(MODULE_PM_LAYERED) && defined(SAM0_SPI_PM_BLOCK)
    pm_unblock(SAM0_SPI_PM_BLOCK);
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
    DEBUG("[spi] bus %u: transferring %u regs at %x\n",
          (unsigned)bus, (unsigned)len, (unsigned)reg);

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
    DEBUG("[spi] bus %u: transferring %u B\n",
          (unsigned)bus, (unsigned)len);
    assert(out || in);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    if (_use_dma(bus) && len > CONFIG_SPI_DMA_THRESHOLD_BYTES) {
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
