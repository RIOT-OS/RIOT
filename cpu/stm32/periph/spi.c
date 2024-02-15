/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2014-2017 Freie Universität Berlin
 *               2016-2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>

#include "compiler_hints.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "pm_layered.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/**
 * @brief   Number of bits to shift the BR value in the CR1 register
 */
#define BR_SHIFT            (3U)
#define BR_MAX              (7U)

#ifdef SPI_CR2_FRXTH
/* configure SPI for 8-bit data width */
#define SPI_CR2_SETTINGS    (SPI_CR2_FRXTH |\
                             SPI_CR2_DS_0 |\
                             SPI_CR2_DS_1 |\
                             SPI_CR2_DS_2)
#else
#define SPI_CR2_SETTINGS    0
#endif

/**
 * @brief   Allocate one lock per SPI device
 */
static mutex_t locks[SPI_NUMOF];

/**
 * @brief   Clock configuration cache
 */
static uint32_t clocks[SPI_NUMOF];

/**
 * @brief   Clock prescaler cache
 */
static uint8_t prescalers[SPI_NUMOF];

static inline SPI_TypeDef *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

#ifdef MODULE_PERIPH_DMA
static inline bool _use_dma(const spi_conf_t *conf)
{
    return conf->tx_dma != DMA_STREAM_UNDEF && conf->rx_dma != DMA_STREAM_UNDEF;
}
#endif

static uint8_t _get_prescaler(const spi_conf_t *conf, uint32_t clock)
{
    uint32_t bus_clock = periph_apb_clk(conf->apbbus);

    uint8_t prescaler = 0;
    uint32_t prescaled_clock = bus_clock >> 1;
    const uint8_t prescaler_max = SPI_CR1_BR_Msk >> SPI_CR1_BR_Pos;
    for (; (prescaled_clock > clock) && (prescaler < prescaler_max); prescaler++) {
        prescaled_clock >>= 1;
    }

    /* If the callers asks for an SPI frequency of at most x, bad things will
     * happen if this cannot be met. So let's have a blown assertion
     * rather than runtime failures that require a logic analyzer to
     * debug. */
    assume(prescaled_clock <= clock);

    return prescaler;
}

void spi_init(spi_t bus)
{
    assume(bus < SPI_NUMOF);

    /* initialize device lock (as locked, spi_init_pins() will unlock it */
    locks[bus] = (mutex_t)MUTEX_INIT_LOCKED;
    /* trigger pin initialization */
    spi_init_pins(bus);

    periph_clk_en(spi_config[bus].apbbus, spi_config[bus].rccmask);
    /* reset configuration */
    dev(bus)->CR1 = 0;
#ifdef SPI_I2SCFGR_I2SE
    dev(bus)->I2SCFGR = 0;
#endif
    dev(bus)->CR2 = SPI_CR2_SETTINGS;
    periph_clk_dis(spi_config[bus].apbbus, spi_config[bus].rccmask);
}

void spi_init_pins(spi_t bus)
{
    assume(bus < SPI_NUMOF);
#ifdef CPU_FAM_STM32F1

    if (gpio_is_valid(spi_config[bus].sclk_pin)) {
        gpio_init_af(spi_config[bus].sclk_pin, GPIO_AF_OUT_PP);
    }

    if (gpio_is_valid(spi_config[bus].mosi_pin)) {
        gpio_init_af(spi_config[bus].mosi_pin, GPIO_AF_OUT_PP);
    }

    if (gpio_is_valid(spi_config[bus].miso_pin)) {
        gpio_init(spi_config[bus].miso_pin, GPIO_IN);
    }
#else
    if (gpio_is_valid(spi_config[bus].mosi_pin)) {
        gpio_init(spi_config[bus].mosi_pin, GPIO_OUT);
        gpio_init_af(spi_config[bus].mosi_pin, spi_config[bus].mosi_af);
    }

    if (gpio_is_valid(spi_config[bus].miso_pin)) {
        gpio_init(spi_config[bus].miso_pin, GPIO_IN);
        gpio_init_af(spi_config[bus].miso_pin, spi_config[bus].miso_af);
    }

    if (gpio_is_valid(spi_config[bus].sclk_pin)) {
        gpio_init(spi_config[bus].sclk_pin, GPIO_OUT);
        gpio_init_af(spi_config[bus].sclk_pin, spi_config[bus].sclk_af);
    }
#endif
    mutex_unlock(&locks[bus]);
}

void spi_deinit_pins(spi_t bus)
{
    assume(bus < SPI_NUMOF);
    mutex_lock(&locks[bus]);

    if (gpio_is_valid(spi_config[bus].sclk_pin)) {
        gpio_init(spi_config[bus].sclk_pin, GPIO_IN);
    }

    if (gpio_is_valid(spi_config[bus].mosi_pin)) {
        gpio_init(spi_config[bus].mosi_pin, GPIO_IN);
    }

    if (gpio_is_valid(spi_config[bus].miso_pin)) {
        gpio_init(spi_config[bus].miso_pin, GPIO_IN);
    }
}

gpio_t spi_pin_miso(spi_t bus)
{
    assume(bus < SPI_NUMOF);
    return spi_config[bus].miso_pin;
}

gpio_t spi_pin_mosi(spi_t bus)
{
    assume(bus < SPI_NUMOF);
    return spi_config[bus].mosi_pin;
}

gpio_t spi_pin_clk(spi_t bus)
{
    assume(bus < SPI_NUMOF);
    return spi_config[bus].sclk_pin;
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    if (bus >= SPI_NUMOF) {
        return SPI_NODEV;
    }
    if (!gpio_is_valid(cs) ||
        (((cs & SPI_HWCS_MASK) == SPI_HWCS_MASK) && (cs & ~(SPI_HWCS_MASK)))) {
        return SPI_NOCS;
    }

    if (cs == SPI_HWCS_MASK) {
        if (!gpio_is_valid(spi_config[bus].cs_pin)) {
            return SPI_NOCS;
        }
#ifdef CPU_FAM_STM32F1
        gpio_init_af(spi_config[bus].cs_pin, GPIO_AF_OUT_PP);
#else
        gpio_init(spi_config[bus].cs_pin, GPIO_OUT);
        gpio_init_af(spi_config[bus].cs_pin, spi_config[bus].cs_af);
#endif
    }
    else {
        gpio_init((gpio_t)cs, GPIO_OUT);
        gpio_set((gpio_t)cs);
    }

    return SPI_OK;
}

#ifdef MODULE_PERIPH_SPI_GPIO_MODE
int spi_init_with_gpio_mode(spi_t bus, const spi_gpio_mode_t* mode)
{
    assume(bus < SPI_NUMOF);

    int ret = 0;

#ifdef CPU_FAM_STM32F1
    /* This has no effect on STM32F1 */
    return ret;
#else
    if (gpio_is_valid(spi_config[bus].mosi_pin)) {
        ret += gpio_init(spi_config[bus].mosi_pin, mode->mosi);
        gpio_init_af(spi_config[bus].mosi_pin, spi_config[bus].mosi_af);
    }

    if (gpio_is_valid(spi_config[bus].miso_pin)) {
        ret += gpio_init(spi_config[bus].miso_pin, mode->miso);
        gpio_init_af(spi_config[bus].miso_pin, spi_config[bus].miso_af);
    }

    if (gpio_is_valid(spi_config[bus].sclk_pin)) {
        ret += gpio_init(spi_config[bus].sclk_pin, mode->sclk);
        gpio_init_af(spi_config[bus].sclk_pin, spi_config[bus].sclk_af);
    }
    return ret;
#endif
}
#endif

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    assume((unsigned)bus < SPI_NUMOF);

    /* lock bus */
    mutex_lock(&locks[bus]);
#ifdef STM32_PM_STOP
    /* block STOP mode */
    pm_block(STM32_PM_STOP);
#endif
    /* enable SPI device clock */
    periph_clk_en(spi_config[bus].apbbus, spi_config[bus].rccmask);
    /* enable device */
    if (clk != clocks[bus]) {
        prescalers[bus] = _get_prescaler(&spi_config[bus], clk);
        clocks[bus] = clk;
    }
    uint8_t br = prescalers[bus];

    DEBUG("[spi] acquire: requested clock: %" PRIu32
          " Hz, resulting clock: %" PRIu32 " Hz, BR prescaler: %u\n",
          clk,
          periph_apb_clk(spi_config[bus].apbbus) >> (br + 1),
          (unsigned)br);

    uint16_t cr1 = ((br << BR_SHIFT) | mode | SPI_CR1_MSTR | SPI_CR1_SPE);
    /* Settings to add to CR2 in addition to SPI_CR2_SETTINGS */
    uint16_t cr2 = SPI_CR2_SETTINGS;
    if (cs != SPI_HWCS_MASK) {
        cr1 |= (SPI_CR1_SSM | SPI_CR1_SSI);
    }
    else {
        cr2 = (SPI_CR2_SSOE);
    }

#ifdef MODULE_PERIPH_DMA
    if (_use_dma(&spi_config[bus])) {
        cr2 |= SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN;

        dma_acquire(spi_config[bus].tx_dma);
        dma_setup(spi_config[bus].tx_dma,
                  spi_config[bus].tx_dma_chan,
                  (uint32_t*)&(dev(bus)->DR),
                  DMA_MEM_TO_PERIPH,
                  DMA_DATA_WIDTH_BYTE,
                  0);

        dma_acquire(spi_config[bus].rx_dma);
        dma_setup(spi_config[bus].rx_dma,
                  spi_config[bus].rx_dma_chan,
                  (uint32_t*)&(dev(bus)->DR),
                  DMA_PERIPH_TO_MEM,
                  DMA_DATA_WIDTH_BYTE,
                  0);
    }
#endif
    dev(bus)->CR1 = cr1;
    dev(bus)->CR2 = cr2;
}

void spi_release(spi_t bus)
{
#ifdef MODULE_PERIPH_DMA
    if (_use_dma(&spi_config[bus])) {
        dma_release(spi_config[bus].tx_dma);
        dma_release(spi_config[bus].rx_dma);
    }
#endif
    /* disable device and release lock */
    dev(bus)->CR1 = 0;
    dev(bus)->CR2 = SPI_CR2_SETTINGS; /* Clear the DMA and SSOE flags */
    periph_clk_dis(spi_config[bus].apbbus, spi_config[bus].rccmask);
#ifdef STM32_PM_STOP
    /* unblock STOP mode */
    pm_unblock(STM32_PM_STOP);
#endif
    mutex_unlock(&locks[bus]);
}

static inline void _wait_for_end(spi_t bus)
{
    /* make sure the transfer is completed before continuing, see reference
     * manual(s) -> section 'Disabling the SPI' */
    while (!(dev(bus)->SR & SPI_SR_TXE)) {}
    while (dev(bus)->SR & SPI_SR_BSY) {}
}

#ifdef MODULE_PERIPH_DMA
static void _transfer_dma(spi_t bus, const void *out, void *in, size_t len)
{
    uint8_t tmp = 0;

    if (out) {
        dma_prepare(spi_config[bus].tx_dma, (void*)out, len, 1);
    }
    else {
        dma_prepare(spi_config[bus].tx_dma, &tmp, len, 0);
    }
    if (in) {
        dma_prepare(spi_config[bus].rx_dma, in, len, 1);
    }
    else {
        dma_prepare(spi_config[bus].rx_dma, &tmp, len, 0);
    }

    /* Start RX first to ensure it is active before the SPI transfers are
     * triggered by the TX dma activity */
    dma_start(spi_config[bus].rx_dma);
    dma_start(spi_config[bus].tx_dma);

    dma_wait(spi_config[bus].rx_dma);
    dma_wait(spi_config[bus].tx_dma);

#ifdef DMA_CCR_EN
    dma_stop(spi_config[bus].rx_dma);
    dma_stop(spi_config[bus].tx_dma);
#endif
    _wait_for_end(bus);
}
#endif

static void _transfer_no_dma(spi_t bus, const void *out, void *in, size_t len)
{
    const uint8_t *outbuf = out;
    uint8_t *inbuf = in;

    /* we need to recast the data register to uint_8 to force 8-bit access */
    volatile uint8_t *DR = (volatile uint8_t*)&(dev(bus)->DR);

    /* transfer data, use shortpath if only sending data */
    if (!inbuf) {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->SR & SPI_SR_TXE)) {}
            *DR = outbuf[i];
        }
    }
    else if (!outbuf) {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->SR & SPI_SR_TXE)) { /* busy wait */ }
            *DR = 0;
            while (!(dev(bus)->SR & SPI_SR_RXNE)) { /* busy wait */ }
            inbuf[i] = *DR;
        }
    }
    else {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->SR & SPI_SR_TXE)) { /* busy wait */ }
            *DR = outbuf[i];
            while (!(dev(bus)->SR & SPI_SR_RXNE)) { /* busy wait */ }
            inbuf[i] = *DR;
        }
    }

    /* wait until everything is finished and empty the receive buffer */
    while (!(dev(bus)->SR & SPI_SR_TXE)) {}
    while (dev(bus)->SR & SPI_SR_BSY) {}
    while (dev(bus)->SR & SPI_SR_RXNE) {
        /* make sure to "read" any data, so the RXNE is indeed clear.
         * Otherwise we risk reading stale data in the next transfer */
        (void)*DR;
    }

    _wait_for_end(bus);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    /* make sure at least one input or one output buffer is given */
    assume(out || in);

    /* active the given chip select line */
    if ((cs != SPI_HWCS_MASK) && gpio_is_valid(cs)) {
        gpio_clear((gpio_t)cs);
    }
    else {
        dev(bus)->CR2 |= SPI_CR2_SSOE;
    }

#ifdef MODULE_PERIPH_DMA
    if (_use_dma(&spi_config[bus]) && len > CONFIG_SPI_DMA_THRESHOLD_BYTES) {
        _transfer_dma(bus, out, in, len);
    }
    else {
#endif
        _transfer_no_dma(bus, out, in, len);
#ifdef MODULE_PERIPH_DMA
    }
#endif

    /* release the chip select if not specified differently */
    if ((!cont) && gpio_is_valid(cs)) {
        if (cs != SPI_HWCS_MASK) {
            gpio_set((gpio_t)cs);
        }
        else {
            dev(bus)->CR2 &= ~(SPI_CR2_SSOE);
        }
    }
}
