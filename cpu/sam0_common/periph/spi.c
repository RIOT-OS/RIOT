/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2015 FreshTemp, LLC.
 * SPDX-FileCopyrightText: 2022 SSV Software Systems GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "pm_layered.h"
#include "bitfield.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF];

#ifdef MODULE_PERIPH_DMA

#ifndef CONFIG_SPI_DMA_DESC_EXP
#  define CONFIG_SPI_DMA_DESC_EXP       1
#endif

struct dma_state {
    dma_t tx_dma;
    dma_t rx_dma;
    dma_cb_t cb;
    void *arg;
    bool acquired;
};

static struct dma_state _dma_state[SPI_NUMOF];

BITFIELD(_dma_desc, 1u << CONFIG_SPI_DMA_DESC_EXP)[SPI_NUMOF];
static DmacDescriptor DMA_DESCRIPTOR_ATTRS _desc[SPI_NUMOF][1u  << CONFIG_SPI_DMA_DESC_EXP];

static int _dma_get(spi_t bus)
{
    return bf_get_unset(_dma_desc[bus], 1u << CONFIG_SPI_DMA_DESC_EXP);
}

static void _dma_put(spi_t bus, int idx)
{
    bf_unset(_dma_desc[bus], idx);
}
#endif

/**
 * @brief   Shortcut for accessing the used SPI SERCOM device
 */
static inline SercomSpi *dev(spi_t bus)
{
    return (SercomSpi *)spi_config[bus].dev;
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
    } else {
        sercom_clk_en(dev(bus));
    }
}

static inline void poweroff(spi_t bus)
{
    if (_is_qspi(bus)) {
        _qspi_clk_disable();
    } else {
        sercom_clk_dis(dev(bus));
    }
}

static inline void _reset(SercomSpi *dev)
{
    dev->CTRLA.reg |= SERCOM_SPI_CTRLA_SWRST;
    while (dev->CTRLA.reg & SERCOM_SPI_CTRLA_SWRST) {}

#ifdef SERCOM_SPI_STATUS_SYNCBUSY
    while (dev->STATUS.reg & SERCOM_SPI_STATUS_SYNCBUSY) {}
#else
    while (dev->SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_SWRST) {}
#endif
}

static inline void _disable(SercomSpi *dev)
{
    dev->CTRLA.reg = 0;

#ifdef SERCOM_SPI_STATUS_SYNCBUSY
    while (dev->STATUS.reg & SERCOM_SPI_STATUS_SYNCBUSY) {}
#else
    while (dev->SYNCBUSY.reg) {}
#endif
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

static inline bool _dma_supported(spi_t bus)
{
#ifdef MODULE_PERIPH_DMA
    return (spi_config[bus].tx_trigger != DMA_TRIGGER_DISABLED) &&
           (spi_config[bus].rx_trigger != DMA_TRIGGER_DISABLED);
#else
    (void)bus;
    return false;
#endif
}

static inline bool _dma_valid(spi_t bus)
{
#ifdef MODULE_PERIPH_DMA
    return _dma_state[bus].rx_dma != UINT8_MAX && _dma_state[bus].tx_dma != UINT8_MAX;
#else
    (void)bus;
    return false;
#endif
}

static inline bool _dma_acquired(spi_t bus)
{
#ifdef MODULE_PERIPH_DMA
    return _dma_state[bus].acquired;
#else
    (void)bus;
    return false;
#endif
}

#ifdef MODULE_PERIPH_DMA
const volatile void *_reg_rx_data(spi_t bus)
{
    if (_is_qspi(bus)) {
#ifdef QSPI
        return &QSPI->RXDATA.reg;
#endif
    }
    else {
        return &dev(bus)->DATA.reg;
    }
}

volatile void *_reg_tx_data(spi_t bus)
{
    if (_is_qspi(bus)) {
#ifdef QSPI
        return &QSPI->TXDATA.reg;
#endif
    }
    else {
        return &dev(bus)->DATA.reg;
    }
}

static void _unlock(void *ctx)
{
    mutex_unlock(ctx);
}

static void _dma_cb(void *ctx)
{
    struct dma_state *state = (struct dma_state *)ctx;
    if (state->cb) {
        state->cb(state->arg);
    }
}
#endif

static inline void _init_dma(spi_t bus)
{
    if (!_dma_supported(bus)) {
        return;
    }
#ifdef MODULE_PERIPH_DMA
    _dma_state[bus].rx_dma = dma_acquire_channel();
    _dma_state[bus].tx_dma = dma_acquire_channel();
    assert(_dma_state[bus].rx_dma != UINT8_MAX);
    assert(_dma_state[bus].tx_dma != UINT8_MAX);
#endif
    (void)bus;
}

static inline void _deinit_dma(spi_t bus)
{
#ifdef MODULE_PERIPH_DMA
    dma_release_channel(_dma_state[bus].rx_dma);
    dma_release_channel(_dma_state[bus].tx_dma);
    _dma_state[bus].rx_dma = UINT8_MAX;
    _dma_state[bus].tx_dma = UINT8_MAX;
#endif
    (void)bus;
}

#if defined(DMAC_CHCTRLB_TRIGACT_BURST_Val) || defined(DMAC_CHCTRLA_TRIGACT_BURST_Val)
#define SPI_DMA_TRIGACT     DMA_TRIGACT_BURST
#else
#define SPI_DMA_TRIGACT     DMA_TRIGACT_BEAT
#endif

static inline int _acquire_dma(spi_t bus)
{
#ifdef MODULE_PERIPH_DMA
    const volatile void *rx = _reg_rx_data(bus);
    volatile void *tx = _reg_tx_data(bus);
    if (!_dma_acquired(bus)) {
        /* setup default DMA configuration, but can be changed as needed */
        dma_setup(_dma_state[bus].tx_dma, SPI_DMA_TRIGACT,
                  spi_config[bus].tx_trigger, 0, NULL, NULL);
        dma_setup(_dma_state[bus].rx_dma, SPI_DMA_TRIGACT,
                  spi_config[bus].rx_trigger, 1, _dma_cb, &_dma_state[bus]);

        dma_prepare(_dma_state[bus].rx_dma, DMAC_BTCTRL_BEATSIZE_BYTE_Val,
                    (const void *)rx, NULL, 1, 0, DMA_BLOCKACT_NONE);
        dma_prepare(_dma_state[bus].tx_dma, DMAC_BTCTRL_BEATSIZE_BYTE_Val,
                    NULL, (void *)tx, 0, 0, DMA_BLOCKACT_NONE);
        _dma_state[bus].acquired = true;
    }
    return 0;
#else
    (void)bus;
    return -ENOTSUP;
#endif
}

static inline void _release_dma(spi_t bus)
{
#ifdef MODULE_PERIPH_DMA
    _dma_state[bus].acquired = false;
#endif
    (void)bus;
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
    /* acquire DMA channels */
    _init_dma(bus);
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
static void _init_spi(spi_t bus, SercomSpi *dev)
{
    /* reset all device configuration */
    _reset(dev);

    /* configure base clock */
    sercom_set_gen(dev, spi_config[bus].gclk_src);

    /* enable receiver and configure character size to 8-bit
     * no synchronization needed, as SERCOM device is not enabled */
    dev->CTRLB.reg = SERCOM_SPI_CTRLB_CHSIZE(0) | SERCOM_SPI_CTRLB_RXEN;
    /* acquire DMA channels */
    _init_dma(bus);
}

static void _spi_acquire(spi_t bus, spi_mode_t mode, spi_clk_t clk)
{
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
    const uint8_t baud = (gclk_src + clk) / (2 * clk) - 1;
#if ENABLE_DEBUG
    /* compute actual SPI clock */
    uint32_t spi_actual = gclk_src / (2U * (baud + 1U));
    DEBUG("SPI bus %u: requested %lu Hz, gclk %lu Hz, BAUD %u -> actual %lu Hz\n",
          bus,
          (unsigned long)clk,
          (unsigned long)gclk_src,
          baud,
          (unsigned long)spi_actual);
#endif
    /* configure device to be master and set mode and pads,
     *
     * NOTE: we could configure the pads already during spi_init, but for
     * efficiency reason we do that here, so we can do all in one single write
     * to the CTRLA register */
    const uint32_t ctrla = SERCOM_SPI_CTRLA_MODE(0x3)       /* 0x3 -> master */
                         | SERCOM_SPI_CTRLA_DOPO(spi_config[bus].mosi_pad)
                         | SERCOM_SPI_CTRLA_DIPO(spi_config[bus].miso_pad)
                         | (mode << SERCOM_SPI_CTRLA_CPHA_Pos);

    /* first configuration or reconfiguration after altered device usage */
    if (dev(bus)->BAUD.reg != baud || dev(bus)->CTRLA.reg != ctrla) {
        /* disable the device */
        _disable(dev(bus));

        dev(bus)->BAUD.reg = baud;
        dev(bus)->CTRLA.reg = ctrla;
        /* no synchronization needed here, the enable synchronization below
         * acts as a write-synchronization for both registers */
    }

    /* finally enable the device */
    _enable(dev(bus));
}

static inline void _spi_release(spi_t bus)
{
    /* disable the device */
    _disable(dev(bus));
}

static void _spi_blocking_transfer(spi_t bus, const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;

        /* transmit byte on MOSI */
        dev(bus)->DATA.reg = tmp;

        /* wait until byte has been sampled on MISO */
        while (!(dev(bus)->INTFLAG.reg & SERCOM_SPI_INTFLAG_RXC)) {}

        /* consume the byte */
        tmp = dev(bus)->DATA.reg;

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
        _init_spi(bus, dev(bus));
    }

    /* put device back to sleep */
    poweroff(bus);
}

int spi_init_with_gpio_mode(spi_t bus, const spi_gpio_mode_t* mode)
{
    assert(bus < SPI_NUMOF);

    if (gpio_is_valid(spi_config[bus].mosi_pin)) {
        gpio_init(spi_config[bus].miso_pin, mode->mosi);
        gpio_init_mux(spi_config[bus].miso_pin, spi_config[bus].miso_mux);
    }

    if (gpio_is_valid(spi_config[bus].miso_pin)) {
        gpio_init(spi_config[bus].mosi_pin, mode->miso);
        gpio_init_mux(spi_config[bus].mosi_pin, spi_config[bus].mosi_mux);
    }

    if (gpio_is_valid(spi_config[bus].clk_pin)) {
        /* clk_pin will be muxed during acquire / release */
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
    mutex_lock(&locks[bus]);

    if (gpio_is_valid(spi_config[bus].miso_pin)) {
        gpio_disable_mux(spi_config[bus].miso_pin);
    }
    gpio_disable_mux(spi_config[bus].mosi_pin);
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);

    /* get exclusive access to the device */
    mutex_lock(&locks[bus]);

    /* power on the device */
    poweron(bus);

    if (_is_qspi(bus)) {
        _qspi_acquire(mode, clk);
    } else {
        _spi_acquire(bus, mode, clk);
    }

    /* mux clk_pin to SPI peripheral */
    gpio_init_mux(spi_config[bus].clk_pin, spi_config[bus].clk_mux);

#if IS_USED(MODULE_PERIPH_DMA) && !IS_USED(MODULE_PERIPH_SPI_DMA)
    spi_acquire_dma(bus);
#endif
}

void spi_release(spi_t bus)
{
#if IS_USED(MODULE_PERIPH_DMA) && !IS_USED(MODULE_PERIPH_SPI_DMA)
    spi_release_dma(bus);
#endif
    /* Demux clk_pin back to GPIO_OUT function. Otherwise it will get HIGH-Z
     * and lead to unexpected current draw by SPI salves. */
    gpio_disable_mux(spi_config[bus].clk_pin);

    if (_is_qspi(bus)) {
        _qspi_release();
    } else {
        _spi_release(bus);
    }

    /* power off the device */
    poweroff(bus);

    /* release access to the device */
    mutex_unlock(&locks[bus]);
}

static void _blocking_transfer(spi_t bus, const void *out, void *in, size_t len)
{
    if (_is_qspi(bus)) {
        _qspi_blocking_transfer(out, in, len);
    } else {
        _spi_blocking_transfer(bus, out, in, len);
    }
}

#ifdef MODULE_PERIPH_DMA

static void _dma_execute_blocking(spi_t bus)
{
#if IS_ACTIVE(MODULE_PM_LAYERED) && defined(SAM0_SPI_PM_BLOCK)
    pm_block(SAM0_SPI_PM_BLOCK);
#endif

    mutex_t lock = MUTEX_INIT_LOCKED;
    spi_dma_set_cb(bus, _unlock);
    spi_dma_set_cb_arg(bus, &lock);

    dma_start(_dma_state[bus].rx_dma);
    dma_start(_dma_state[bus].tx_dma);

    mutex_lock(&lock);
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
    _dma_execute_blocking(bus);
}

static void _dma_transfer_regs(spi_t bus, uint8_t reg, const uint8_t *out,
                               uint8_t *in, size_t len)
{
    uint8_t tmp;
    const uint8_t *out_addr = out ? out + len : &tmp;
    uint8_t *in_addr = in ? in + len : &tmp;

    dma_prepare_dst(_dma_state[bus].rx_dma, &tmp, 1, false);
    dma_prepare_src(_dma_state[bus].tx_dma, &reg, 1, false);

    int rx = _dma_get(bus);
    assert(rx >= 0);
    int tx = _dma_get(bus);
    assert(tx >= 0);
    DmacDescriptor *desc_rx =  &_desc[bus][rx];
    DmacDescriptor *desc_tx =  &_desc[bus][tx];
    dma_append_dst(_dma_state[bus].rx_dma, desc_rx, in_addr,
                   len, in ? true : false);
    dma_append_src(_dma_state[bus].tx_dma, desc_tx, out_addr,
                   len, out ? true : false);

    _dma_execute_blocking(bus);
    spi_dma_put_desc(bus, desc_rx);
    spi_dma_put_desc(bus, desc_tx);
}

void spi_transfer_regs(spi_t bus, spi_cs_t cs,
                       uint8_t reg, const void *out, void *in, size_t len)
{
    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    if (_dma_acquired(bus)) {
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

    if (_dma_acquired(bus) && len > CONFIG_SPI_DMA_THRESHOLD_BYTES) {
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

#if MODULE_PERIPH_SPI_DMA
int spi_acquire_dma(spi_t bus)
{
    if (_dma_valid(bus)) {
        return _acquire_dma(bus);
    }
    return -1;
}

void spi_release_dma(spi_t bus)
{
    if (_dma_valid(bus)) {
        _release_dma(bus);
    }
}

void spi_dma_set_cb(spi_t bus, dma_cb_t cb)
{
    _dma_state[bus].cb = cb;
}

void spi_dma_set_cb_arg(spi_t bus, void *arg)
{
    _dma_state[bus].arg = arg;
}

void *spi_dma_get_desc(spi_t bus)
{
    int idx = _dma_get(bus);
    return idx >= 0 ? &_desc[bus][idx] : NULL;
}

void spi_dma_put_desc(spi_t bus, const void *desc)
{
    int idx = ((DmacDescriptor *)desc - (DmacDescriptor *)_desc[bus]);
    assert(idx >= 0);
    assert((unsigned)idx < (1u << CONFIG_SPI_DMA_DESC_EXP));
    _dma_put(bus, idx);
}

static const uint8_t _out_null = 0;
static uint8_t _in_null;

void spi_dma_prepare_desc(spi_t bus, void *desc_rx, void *desc_tx,
                          const void *out, void *in, size_t len,
                          dma_blockact_t block_act_rx, dma_blockact_t block_act_tx)
{
    const volatile void *rx = _reg_rx_data(bus);
    volatile void *tx = _reg_tx_data(bus);

    const uint8_t *out_addr = out ? out + len : &_out_null;
    uint8_t *in_addr = in ? in + len : &_in_null;
    if (desc_rx) {
        dma_prepare_descriptor(desc_rx, DMAC_BTCTRL_BEATSIZE_BYTE_Val,
                               (const void *)rx, in_addr, len,
                               in ? DMA_INCR_DEST : DMA_INCR_NONE, block_act_rx);
    }
    if (desc_tx) {
        dma_prepare_descriptor(desc_tx, DMAC_BTCTRL_BEATSIZE_BYTE_Val,
                               out_addr, (void *)tx, len,
                               out ? DMA_INCR_SRC : DMA_INCR_NONE, block_act_tx);
    }
}

void spi_dma_prepare(spi_t bus, const void *out, void *in, size_t len,
                     dma_blockact_t block_act_rx, dma_blockact_t block_act_tx)
{
    const volatile void *rx = _reg_rx_data(bus);
    volatile void *tx = _reg_tx_data(bus);

    const uint8_t *out_addr = out ? out + len : &_out_null;
    uint8_t *in_addr = in ? in + len : &_in_null;
    dma_prepare(_dma_state[bus].rx_dma, DMAC_BTCTRL_BEATSIZE_BYTE_Val,
                (const void *)rx, in_addr, len, !!in, block_act_rx);
    dma_prepare(_dma_state[bus].tx_dma, DMAC_BTCTRL_BEATSIZE_BYTE_Val,
                out_addr, (void *)tx, len, !!out, block_act_tx);
}

void spi_dma_setup(spi_t bus,
                   unsigned trigger_rx, dma_trigact_t trigact_rx, dma_cb_t cb_rx, void *arg_rx,
                   unsigned trigger_tx, dma_trigact_t trigact_tx, dma_cb_t cb_tx, void *arg_tx)
{
    dma_setup(_dma_state[bus].tx_dma, trigact_tx,
              trigger_tx, 0, cb_tx, arg_tx);
    dma_setup(_dma_state[bus].rx_dma, trigact_rx,
              trigger_rx, 1, cb_rx, arg_rx);
}

void spi_dma_append(spi_t bus, void *desc_rx, void *desc_tx,
                    const void *out, void *in, size_t len)
{
    const uint8_t *out_addr = out ? out + len : &_out_null;
    uint8_t *in_addr = in ? in + len : &_in_null;
    if (desc_rx) {
        dma_append_dst(_dma_state[bus].rx_dma, desc_rx, in_addr, len, !!in);
    }
    if (desc_tx) {
        dma_append_src(_dma_state[bus].tx_dma, desc_tx, out_addr, len, !!out);
    }
}

void spi_dma_set_desc_rx(spi_t bus, void *desc)
{
    *(DmacDescriptor *)dma_descriptor(_dma_state[bus].rx_dma) = *(DmacDescriptor *)desc;
}

void spi_dma_set_desc_tx(spi_t bus, void *desc)
{
    *(DmacDescriptor *)dma_descriptor(_dma_state[bus].tx_dma) = *(DmacDescriptor *)desc;
}

const void *spi_dma_get_desc_rx(spi_t bus)
{
    return dma_descriptor(_dma_state[bus].rx_dma);
}

const void *spi_dma_get_desc_tx(spi_t bus)
{
    return dma_descriptor(_dma_state[bus].tx_dma);
}

void spi_dma_start_rx(spi_t bus)
{
    dma_start(_dma_state[bus].rx_dma);
}

void spi_dma_start_tx(spi_t bus)
{
    dma_start(_dma_state[bus].tx_dma);
}

dma_t spi_dma_rx(spi_t bus)
{
    return _dma_state[bus].rx_dma;
}

dma_t spi_dma_tx(spi_t bus)
{
    return _dma_state[bus].tx_dma;
}
#endif /* MODULE_PERIPH_SPI_DMA */
