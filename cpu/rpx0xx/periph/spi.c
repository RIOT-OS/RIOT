/*
 * Copyright (C) 2023 Frank Engelhardt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rpx0xx
 * @{
 *
 * @file
 * @brief       Implementation of SPI.
 *
 * @author      Frank Engelhardt <frank@f9e.de>
 *
 * @}
 */

#include "assert.h"
#include "bitarithm.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Allocate one lock per SPI device.
 */
static mutex_t locks[SPI_NUMOF];

/**
 * @brief   Save the clock prescaler values for faster bus acquisition.
 */
typedef struct {
    spi_clk_t clk;
    uint8_t cpsdvsr;
    uint8_t scr;
} _pl022_clk_t;
static _pl022_clk_t pl022_clk[SPI_NUMOF];

gpio_t spi_pin_clk(spi_t spi)
{
    assert((unsigned)spi < SPI_NUMOF);
    return spi_config[spi].clk_pin;
}

gpio_t spi_pin_miso(spi_t spi)
{
    assert((unsigned)spi < SPI_NUMOF);
    return spi_config[spi].miso_pin;
}

gpio_t spi_pin_mosi(spi_t spi)
{
    assert((unsigned)spi < SPI_NUMOF);
    return spi_config[spi].mosi_pin;
}

static void _poweron(spi_t spi)
{
    uint32_t reset_bit_mask = (spi_config[spi].dev == SPI0)
        ? RESETS_RESET_spi0_Msk
        : RESETS_RESET_spi1_Msk;

    periph_reset(reset_bit_mask);
    periph_reset_done(reset_bit_mask);
}

static void _poweroff(spi_t spi)
{
    uint32_t reset_bit_mask = (spi_config[spi].dev == SPI0)
        ? RESETS_RESET_spi0_Msk
        : RESETS_RESET_spi1_Msk;

    periph_reset(reset_bit_mask);
}

void spi_init(spi_t spi)
{
    DEBUG("[rpx0xx] Call spi_init(spi=%" PRIdFAST8 ")", spi);

    assert((unsigned)spi < SPI_NUMOF);

    /* initialize device lock */
    mutex_init(&locks[spi]);
    /* trigger pin initialization */
    spi_init_pins(spi);
    /* clock prescaler values must be calculated */
    pl022_clk[spi].clk = 0xff;
}

void spi_init_pins(spi_t spi)
{
    DEBUG("[rpx0xx] Call spi_init_pins(spi=%" PRIdFAST8 ")", spi);

    assert((unsigned)spi < SPI_NUMOF);

    const gpio_pad_ctrl_t mosi_pad_config = {
        /* SPI should typically draw less than 2 mA */
        .drive_strength = DRIVE_STRENGTH_2MA,
    };
    const gpio_io_ctrl_t mosi_io_config = {
        .function_select = FUNCTION_SELECT_SPI,
    };
    const gpio_pad_ctrl_t clk_pad_config = {
        /* SPI should typically draw less than 2 mA */
        .drive_strength = DRIVE_STRENGTH_2MA,
    };
    const gpio_io_ctrl_t clk_io_config = {
        .function_select = FUNCTION_SELECT_SPI,
    };
    const gpio_pad_ctrl_t miso_pad_config = {
        .input_enable = 1,
        .schmitt_trig_enable = 1,
    };
    const gpio_io_ctrl_t miso_io_config = {
        .function_select = FUNCTION_SELECT_SPI,
    };

    if (gpio_is_valid(spi_config[spi].mosi_pin)) {
        gpio_set_pad_config(spi_config[spi].mosi_pin, mosi_pad_config);
        gpio_set_io_config(spi_config[spi].mosi_pin, mosi_io_config);
    }
    if (gpio_is_valid(spi_config[spi].clk_pin)) {
        gpio_set_pad_config(spi_config[spi].clk_pin, clk_pad_config);
        gpio_set_io_config(spi_config[spi].clk_pin, clk_io_config);
    }
    if (gpio_is_valid(spi_config[spi].miso_pin)) {
        gpio_set_pad_config(spi_config[spi].miso_pin, miso_pad_config);
        gpio_set_io_config(spi_config[spi].miso_pin, miso_io_config);
    }

    /* allow access to the bus */
    mutex_unlock(&locks[spi]);
}

void spi_deinit_pins(spi_t spi)
{
    DEBUG("[rpx0xx] Call spi_deinit_pins(spi=%" PRIdFAST8 ")\n", spi);

    /* lock mutex to block usage of the SPI bus */
    mutex_lock(&locks[spi]);

    assert((unsigned)spi < SPI_NUMOF);

    if (gpio_is_valid(spi_config[spi].mosi_pin)) {
        gpio_reset_all_config(spi_config[spi].mosi_pin);
    }

    if (gpio_is_valid(spi_config[spi].miso_pin)) {
        gpio_reset_all_config(spi_config[spi].miso_pin);
    }

    if (gpio_is_valid(spi_config[spi].clk_pin)) {
        gpio_reset_all_config(spi_config[spi].clk_pin);
    }
}

int spi_init_cs(spi_t spi, spi_cs_t cs)
{
    DEBUG("[rpx0xx] Call spi_init_cs(spi=%" PRIdFAST8 ", cs=%" PRIu32 ")\n",
          spi, cs);

    if (spi >= SPI_NUMOF) {
        return SPI_NODEV;
    }
    if (!gpio_is_valid(cs)) {
        return SPI_NOCS;
    }

    gpio_init((gpio_t)cs, GPIO_OUT);
    gpio_set((gpio_t)cs);

    return SPI_OK;
}

int spi_deinit_cs(spi_t spi, spi_cs_t cs)
{
    DEBUG("[rpx0xx] Call spi_deinit_cs(spi=%" PRIdFAST8 ", cs=%" PRIu32 ")\n",
          spi, cs);

    if (spi >= SPI_NUMOF) {
        return SPI_NODEV;
    }
    if (!gpio_is_valid(cs)) {
        return SPI_NOCS;
    }

    gpio_reset_all_config((gpio_t)cs);

    return SPI_OK;
}

static inline void _check_best_clk_result(uint16_t target_dvsr,
                                          uint16_t cur_dvsr,
                                          uint16_t scr,
                                          uint8_t cpsdvsr,
                                          uint16_t *best_diff,
                                          uint16_t *best_scr,
                                          uint8_t *best_cpsdvsr)
{
    uint16_t diff = (target_dvsr > cur_dvsr)
                    ? target_dvsr - cur_dvsr
                    : cur_dvsr - target_dvsr;

    if (diff < *best_diff) {
        *best_diff = diff;
        *best_cpsdvsr = cpsdvsr;
        *best_scr = scr;
    }
}

static void _calc_pl022_clk(_pl022_clk_t *pl022_clk, spi_clk_t clk)
{
    uint16_t dvsr = CLOCK_PERIPH / clk;
    /* The divisor must be split into two 8-bit divisors, cpsdvsr and scr,
     * dvsr = cpsdvsr*scr. cpsdvsr must be an even number greater than 0. */
    uint8_t cpsdvsr = 2, best_cpsdvsr = 2;
    uint16_t scr = 256, best_scr = 256;
    uint16_t best_diff = dvsr;
    uint16_t cur_dvsr = scr * cpsdvsr;

    /* Come from above with cpsdvsr and from below with scr.
     * Check all intermediate combinations and use the best. */
    _check_best_clk_result(dvsr, cur_dvsr, scr, cpsdvsr, &best_diff,
                           &best_scr, &best_cpsdvsr);
    while (scr > 0 && cpsdvsr < 254 && best_diff != 0) {
        while (dvsr > cur_dvsr && cpsdvsr < 254) {
            cpsdvsr += 2;
            cur_dvsr += 2 * scr;
            _check_best_clk_result(dvsr, cur_dvsr, scr, cpsdvsr, &best_diff,
                                   &best_scr, &best_cpsdvsr);
        }
        while (dvsr < cur_dvsr && scr > 0) {
            scr -= 1;
            cur_dvsr -= cpsdvsr;
            _check_best_clk_result(dvsr, cur_dvsr, scr, cpsdvsr, &best_diff,
                                   &best_scr, &best_cpsdvsr);
        }
    }
    uint32_t resulting_clk_hz = CLOCK_PERIPH / (best_cpsdvsr * best_scr);

    pl022_clk->clk = clk;
    pl022_clk->cpsdvsr = best_cpsdvsr;
    /* For scr, +1 is added internally. */
    pl022_clk->scr = (best_scr - 1);
    DEBUG("[rpx0xx] Values for spi clock divider registers CPSDVSR=%" PRIu16
          ", SCR=%" PRIu16 ". Resulting clock is %" PRIu32 " Hz.\n",
          best_cpsdvsr, best_scr - 1, resulting_clk_hz);
}

void spi_acquire(spi_t spi, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    DEBUG("[rpx0xx] Call spi_acquire(spi=%" PRIuFAST8 ", cs=%" PRIu32
          ", mode=%" PRIu16 ", clk=%" PRIu32 ")\n", spi, cs, mode, clk);

    assert((unsigned)spi < SPI_NUMOF);
    assert(clk == SPI_CLK_100KHZ || clk == SPI_CLK_10MHZ || clk == SPI_CLK_1MHZ
           || clk == SPI_CLK_400KHZ || clk == SPI_CLK_5MHZ);
    (void)cs;

    /* lock bus */
    mutex_lock(&locks[spi]);

    _poweron(spi);

    SPI0_Type *dev = spi_config[spi].dev;

    io_reg_write_dont_corrupt(&dev->SSPCR0,
                              0x7 << SPI0_SSPCR0_DSS_Pos,
                              SPI0_SSPCR0_DSS_Msk);
    /* uncomment the following line for debug loopback mode */
    /* io_reg_atomic_set(&dev->SSPCR1, SPI0_SSPCR1_LBM_Msk); */

    /* set SPI mode */
    switch (mode) {
    case SPI_MODE_0:
        io_reg_atomic_clear(&dev->SSPCR0, SPI0_SSPCR0_SPO_Msk);
        io_reg_atomic_clear(&dev->SSPCR0, SPI0_SSPCR0_SPH_Msk);
        break;
    case SPI_MODE_1:
        io_reg_atomic_clear(&dev->SSPCR0, SPI0_SSPCR0_SPO_Msk);
        io_reg_atomic_set(&dev->SSPCR0, SPI0_SSPCR0_SPH_Msk);
        break;
    case SPI_MODE_2:
        io_reg_atomic_set(&dev->SSPCR0, SPI0_SSPCR0_SPO_Msk);
        io_reg_atomic_clear(&dev->SSPCR0, SPI0_SSPCR0_SPH_Msk);
        break;
    case SPI_MODE_3:
        io_reg_atomic_set(&dev->SSPCR0, SPI0_SSPCR0_SPO_Msk);
        io_reg_atomic_set(&dev->SSPCR0, SPI0_SSPCR0_SPH_Msk);
        break;
    }

    /* set clock speed */
    if (clk != pl022_clk[spi].clk) {
        _calc_pl022_clk(&pl022_clk[spi], clk);
    }
    io_reg_write_dont_corrupt(&dev->SSPCPSR,
                              pl022_clk[spi].cpsdvsr << SPI0_SSPCPSR_CPSDVSR_Pos,
                              SPI0_SSPCPSR_CPSDVSR_Msk);
    io_reg_write_dont_corrupt(&dev->SSPCR0,
                              pl022_clk[spi].scr << SPI0_SSPCR0_SCR_Pos,
                              SPI0_SSPCR0_SCR_Msk);

    /* enable SPI */
    io_reg_atomic_set(&dev->SSPCR1, SPI0_SSPCR1_SSE_Msk);
}

void spi_release(spi_t spi)
{
    DEBUG("[rpx0xx] Call spi_release(spi=%" PRIdFAST8 ")\n", spi);

    assert((unsigned)spi < SPI_NUMOF);

    SPI0_Type *dev = spi_config[spi].dev;

    /* disable SPI */
    io_reg_atomic_clear(&dev->SSPCR1, SPI0_SSPCR1_SSE_Msk);

    _poweroff(spi);

    mutex_unlock(&locks[spi]);
}

static inline void _wait_for_end(spi_t spi)
{
    DEBUG("[rpx0xx] Call _wait_for_end(spi=%" PRIdFAST8 ")\n", spi);
    SPI0_Type *dev = spi_config[spi].dev;

    while (!(dev->SSPSR & SPI0_SSPSR_TFE_Msk)) {}
    while (dev->SSPSR & SPI0_SSPSR_BSY_Msk) {}
}

static void _transfer_no_dma(spi_t spi, const void *out, void *in, size_t len)
{
    DEBUG("[rpx0xx] Call _transfer_no_dma(spi=%" PRIdFAST8
          ", out=%p, in=%p, len=%" PRIu16 ")\n",
          spi, out, in, len);

    const uint8_t *outbuf = out;
    uint8_t *inbuf = in;
    SPI0_Type *dev = spi_config[spi].dev;

    /* transfer data, use shortpath if only sending data */
    if (!inbuf) {
        for (size_t i = 0; i < len; i++) {
            while (!(dev->SSPSR & SPI0_SSPSR_TFE_Msk)) {}
            dev->SSPDR = outbuf[i];
        }
        /* wait until everything is finished and empty the receive buffer */
        while (!(dev->SSPSR & SPI0_SSPSR_TFE_Msk)) {}
        while (dev->SSPSR & SPI0_SSPSR_BSY_Msk) {}
        while (dev->SSPSR & SPI0_SSPSR_RNE_Msk) {
            dev->SSPDR;
        }
    }
    else if (!outbuf) {
        for (size_t i = 0; i < len; i++) {
            while (!(dev->SSPSR & SPI0_SSPSR_TFE_Msk)) {}
            dev->SSPDR = 0;
            while (!(dev->SSPSR & SPI0_SSPSR_RNE_Msk)) {}
            inbuf[i] = dev->SSPDR & 0xffUL;
        }
    }
    else {
        for (size_t i = 0; i < len; i++) {
            while (!(dev->SSPSR & SPI0_SSPSR_TFE_Msk)) {}
            dev->SSPDR = outbuf[i];
            while (!(dev->SSPSR & SPI0_SSPSR_RNE_Msk)) {}
            inbuf[i] = dev->SSPDR & 0xffUL;
        }
    }

    _wait_for_end(spi);
}

void spi_transfer_bytes(spi_t spi, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    DEBUG("[rpx0xx] Call spi_transfer_bytes(spi=%" PRIdFAST8
          ", out=%p, in=%p, len=%" PRIu16 ")\n",
          spi, out, in, len);

    /* make sure at least one input or one output buffer is given */
    assert(out || in);

    /* activate the given chip select line */
    if (gpio_is_valid(cs)) {
        gpio_clear((gpio_t)cs);
    }

    _transfer_no_dma(spi, out, in, len);

    /* release the chip select if not specified differently */
    if ((!cont) && gpio_is_valid(cs)) {
        gpio_set((gpio_t)cs);
    }
}
