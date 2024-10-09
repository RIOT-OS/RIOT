/*
 * Copyright (C) 2023 Gunar Schorcht
 *               2023 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief   Low-level SDIO/SD/MMC driver for SD Host Controller peripherals
 *
 * The module implements a the low-level SDIO/SD/MMC driver for peripherals
 * that are compliant with the SD Host Controller Simplified Specification,
 * Version 3.00 [[sdcard.org](https://www.sdcard.org)]. It is intended
 * exclusively for use as a low-level driver for the SDIO/SD/MMC API
 * (module `sdmmc`).
 *
 * @note The driver uses the definition of the SD Host Controller interface
 *       from the Atmel SAME54 Series Device Support Package (1.1.134)
 *       [http://packs.download.atmel.com/].
 *
 * @note Some parts of the driver were inspired by the implementation in
 *       https://github.com/alkgrove/initmaker/blob/master/samd5x/src/sd.c.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "macros/math.h"
#include "periph_cpu.h"
#include "periph/gpio.h"
#include "periph/pm.h"
#include "time_units.h"
#include "ztimer.h"

#include "sdmmc/sdmmc.h"
#include "sdmmc/vendor/sdhc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if defined(CPU_SAMD5X) || defined(CPU_SAME5X)

#ifndef SDHC_CLOCK
#define SDHC_CLOCK          SAM0_GCLK_MAIN
#endif

#ifndef SDHC_CLOCK_SLOW
#define SDHC_CLOCK_SLOW     SAM0_GCLK_TIMER
#endif

#else
#error "CPU not supported"
#endif

/* limit the Default and High Speed clock rates for debugging */
#if CONFIG_SDMMC_CLK_MAX_400KHZ
#define CONFIG_SDMMC_CLK_MAX        KHZ(400)
#elif CONFIG_SDMMC_CLK_MAX_1MHZ
#define CONFIG_SDMMC_CLK_MAX        MHZ(1)
#elif CONFIG_SDMMC_CLK_MAX_4MHZ
#define CONFIG_SDMMC_CLK_MAX        MHZ(4)
#elif CONFIG_SDMMC_CLK_MAX_10MHZ
#define CONFIG_SDMMC_CLK_MAX        MHZ(10)
#elif CONFIG_SDMMC_CLK_MAX_20MHZ
#define CONFIG_SDMMC_CLK_MAX        MHZ(20)
#elif CONFIG_SDMMC_CLK_MAX_25MHZ
#define CONFIG_SDMMC_CLK_MAX        MHZ(25)
#else
#define CONFIG_SDMMC_CLK_MAX        MHZ(50)
#endif

/* millisecond timer definitions dependent on active ztimer backend */
#if IS_USED(MODULE_ZTIMER_MSEC)
#define _ZTIMER_ACQUIRE()       ztimer_acquire(ZTIMER_MSEC)
#define _ZTIMER_RELEASE()       ztimer_release(ZTIMER_MSEC)
#define _ZTIMER_NOW()           ztimer_now(ZTIMER_MSEC)
#define _ZTIMER_SLEEP_MS(n)     ztimer_sleep(ZTIMER_MSEC, n)
#elif IS_USED(MODULE_ZTIMER_USEC)
#define _ZTIMER_ACQUIRE()       ztimer_acquire(ZTIMER_USEC)
#define _ZTIMER_RELEASE()       ztimer_release(ZTIMER_USEC)
#define _ZTIMER_NOW()           ztimer_now(ZTIMER_USEC) / US_PER_MS
#define _ZTIMER_SLEEP_MS(n)     ztimer_sleep(ZTIMER_USEC, n * US_PER_MS)
#else
#error "Either module ztimer_msec or ztimer_usec is needed"
#endif

/* Monitor card insertion and removal */
#define SDHC_NISTR_CARD_DETECT   (SDHC_NISTR_CREM | SDHC_NISTR_CINS)
#define SDHC_NISTER_CARD_DETECT  (SDHC_NISTER_CREM | SDHC_NISTER_CINS)
#define SDHC_NISIER_CARD_DETECT  (SDHC_NISIER_CREM | SDHC_NISIER_CINS)

#include "board.h"

/* forward declaration of _driver */
static const sdmmc_driver_t _driver;

/* SDHC device context */
typedef struct {
    sdmmc_dev_t sdmmc_dev;      /**< Inherited sdmmc_dev_t struct */
    const sdhc_conf_t *conf;    /**< SDHC peripheral config reference */
    mutex_t irq_wait;           /**< ISR mutex */
    uint16_t error;             /**< last SDHC error status (EISTR) */
    bool data_transfer;         /**< Transfer active */
} sdhc_dev_t;

/* SDHC device context array */
static sdhc_dev_t _sdhc_devs[] = {
    {
        .sdmmc_dev = {
                        .driver = &_driver,
                     },
        .conf = &sdhc_config[0],
        .irq_wait = MUTEX_INIT_LOCKED,
    },
#if SDHC_CONFIG_NUMOF > 1
    {
        .sdmmc_dev = {
                        .driver = &_driver,
                     },
        .conf = &sdhc_config[1],
        .irq_wait = MUTEX_INIT_LOCKED,
    }
#endif
};

/* sanity check of configuration */
static_assert(SDHC_CONFIG_NUMOF == ARRAY_SIZE(sdhc_config),
              "SDHC_CONFIG_NUMOF and the number of elements in sdhc_config differ");
static_assert(SDHC_CONFIG_NUMOF == ARRAY_SIZE(_sdhc_devs),
              "SDHC_CONFIG_NUMOF and the number of elements in sdhc_devs differ");

/* check that the number of devices does not exhaust the number of available devices */
#ifdef SDHC1
static_assert(SDHC_CONFIG_NUMOF < 3, "MCU supports only 2 SDHC peripherals");
#else
static_assert(SDHC_CONFIG_NUMOF < 2, "MCU supports only 1 SDHC peripheral");
#endif

XFA_CONST(sdmmc_devs, 0) sdmmc_dev_t * const _sdmmc_0 = (sdmmc_dev_t * const)&_sdhc_devs[0];
#if SDHC_CONFIG_NUMOF > 1
XFA_CONST(sdmmc_devs, 0) sdmmc_dev_t * const _sdmmc_1 = (sdmmc_dev_t * const)&_sdhc_devs[1];
#endif

static int _set_clock_rate(sdmmc_dev_t *dev, sdmmc_clock_rate_t rate);

/* forward declaration of internal functions */
static void _core_init(sdhc_dev_t *sdhc_dev);
static void _init_pins(sdhc_dev_t *sdhc_dev);

static void _reset_sdhc(sdhc_dev_t *sdhc_dev, uint8_t type);
static int _wait_sdhc_busy(sdhc_t *sdhc);
static bool _wait_for_event(sdhc_dev_t *sdhc_dev,
                            uint16_t event, uint16_t error_mask,
                            uint8_t reset);
static int _enable_sd_clk(sdhc_dev_t *sdhc_dev);
static int _disable_sd_clk(sdhc_dev_t *sdhc_dev);
static int _sdhc_to_sdmmc_err_code(uint16_t code);
static void _isr(sdhc_dev_t *dev);

static sdhc_dev_t *isr_ctx_0;
#ifdef SDHC1
static sdhc_dev_t *isr_ctx_1;
#endif

static void _init(sdmmc_dev_t *dev)
{
    sdhc_dev_t *sdhc_dev = container_of(dev, sdhc_dev_t, sdmmc_dev);

    assert(sdhc_dev);
    assert(sdhc_dev->conf);
    assert(sdhc_dev->conf->sdhc);

    sdhc_t *sdhc = sdhc_dev->conf->sdhc;

#ifdef SDHC1
    assert((sdhc == (void *)SDHC0) || (sdhc == (void *)SDHC1));
#else
    assert(sdhc == (void *)SDHC0);
#endif

    _core_init(sdhc_dev);

    /* pins have to initialized after enabling the clock for the SDHC core */
    _init_pins(sdhc_dev);

    /* Enable all the status bits in NISTR and EISTR */
    sdhc->NISTER.reg = SDHC_NISTER_MASK;
    sdhc->EISTER.reg = SDHC_EISTER_MASK;

    sdhc->PCR.bit.SDBVSEL = SDHC_PCR_SDBVSEL_3V3_Val;
    sdhc->PCR.bit.SDBPWR = 1;

    if (sdhc == (void *)SDHC0) {
        isr_ctx_0 = sdhc_dev;
        NVIC_ClearPendingIRQ(SDHC0_IRQn);
        NVIC_SetPriority(SDHC0_IRQn, 1);
        NVIC_EnableIRQ(SDHC0_IRQn);
    }
#ifdef SDHC1
    else if (sdhc == (void *)SDHC1) {
        isr_ctx_1 = sdhc_dev;
        NVIC_EnableIRQ(SDHC1_IRQn);
        NVIC_SetPriority(SDHC1_IRQn, 1);
        NVIC_EnableIRQ(SDHC1_IRQn);
    }
#endif

    sdhc->TCR.reg = 14;                         /* max timeout is 14 or about 1sec */
    sdhc->PCR.reg = SDHC_PCR_SDBVSEL_3V3;

    sdhc->NISTER.reg = SDHC_NISTER_MASK;        /* enable all normal interrupt status flags */
    sdhc->EISTER.reg = SDHC_EISTER_MASK;        /* enable all error interrupt status flags */

    sdhc->NISIER.reg = SDHC_NISIER_CARD_DETECT; /* enable card detection interrupt signals */

    /* set the clock rate to enable the internal clock of the SDHC which is
     * needed for card detection interrupts. */
    _set_clock_rate(dev, SDMMC_CLK_400K);

    dev->present = true;
}

static int _send_cmd(sdmmc_dev_t *dev, uint8_t cmd_idx, uint32_t arg,
                     uint8_t resp_type, uint32_t *resp)
{
    /* ensure that `sdmmc_send_acmd` is used for application specific commands */
    assert((cmd_idx & SDMMC_ACMD_PREFIX) == 0);

    sdhc_dev_t *sdhc_dev = container_of(dev, sdhc_dev_t, sdmmc_dev);

    assert(sdhc_dev);
    assert(sdhc_dev->conf);

    sdhc_t *sdhc = sdhc_dev->conf->sdhc;

    DEBUG("[sdmmc] send CMD%d with R%d%s\n", cmd_idx,
          resp_type & SDMMC_RESP_IDX, (resp_type & SDMMC_RESP_BUSY) ? "B" : "");

    uint32_t cmd;

    /* since the SD bus power is automatically turned off when the card is
     * removed, it has to be turned on when a command is sent if needed */
    if (!(sdhc->PCR.reg & SDHC_PCR_SDBPWR)) {
        sdhc->PCR.reg |= SDHC_PCR_SDBPWR;
    }

    /* enable the clock to the card if needed */
    if (IS_USED(MODULE_PERIPH_SDMMC_AUTO_CLK) && _enable_sd_clk(sdhc_dev)) {
        return -EIO;
    }

    /* wait if card is still busy */
    if (_wait_sdhc_busy(sdhc)) {
        /* if timeout occurs, there is a serious situation, in this case
         * reset the entire peripheral */
        _reset_sdhc(sdhc_dev, SDHC_SRR_SWRSTALL);
        return -ETIMEDOUT;
    }

    /* clear command related normal interrupt status flags */
    sdhc->NISTR.reg = SDHC_NISTR_TRFC | SDHC_NISTR_CMDC;

    cmd = SDHC_CR_CMDIDX(cmd_idx) | SDHC_CR_CMDTYP_NORMAL;

    switch (resp_type) {
    case SDMMC_NO_R:
        break;
    case SDMMC_R2:
        cmd |= SDHC_CR_RESPTYP_136_BIT;
        break;
    case SDMMC_R1B:
        cmd |= SDHC_CR_RESPTYP_48_BIT_BUSY;
        break;
    default:
        cmd |= SDHC_CR_RESPTYP_48_BIT;
        break;
    }

    if (sdhc_dev->data_transfer) {
        /* command is part of a data transfer, TMR and BCR are already
         * prepared in _xfer_prepare and must not be overwritten */
        cmd |= SDHC_CR_DPSEL_DATA;
    }
    else {
        /* reset TMR and BCR otherwise */
        sdhc->TMR.reg = 0;
        sdhc->BCR.reg = 0;
    }

#if defined(CPU_SAMD5X) || defined(CPU_SAME5X)
    Sdhc *sam0_sdhc = (Sdhc *)sdhc;
    /* CMD0, CMD1, CMD2, CMD3 and CMD8 are broadcast commands */
    if ((cmd_idx <= SDMMC_CMD3) || (cmd_idx == SDMMC_CMD8)) {
        sam0_sdhc->MC1R.reg |= SDHC_MC1R_OPD;
    }
    else {
        sam0_sdhc->MC1R.reg &= ~SDHC_MC1R_OPD;
    }
#endif

    sdhc_dev->error = 0;

    /* used error interrupts */
    uint16_t eis = SDHC_EISTR_CMDTEO | SDHC_EISTR_CMDEND | SDHC_EISTR_CMDIDX;

    eis |= (resp_type & SDMMC_RESP_CRC) ? SDHC_EISTR_CMDCRC : 0;
    eis |= (resp_type & SDMMC_RESP_BUSY) ? SDHC_EISTR_DATTEO : 0;

    if (sdhc_dev->data_transfer) {
        /* if command is starts a data transfer, also DAT related
         * error interrupts are used */
        eis |= SDHC_EISTR_DATTEO | SDHC_EISTR_DATEND | SDHC_EISTR_DATCRC;
    }

    /* use TRFC (Transfer Complete) interrupt in case of R1b response with busy
     * and CMDC (Command Complete) otherwise */
    uint16_t nis = (resp_type == SDMMC_R1B) ? SDHC_NISTR_TRFC
                                            : SDHC_NISTR_CMDC;

    sdhc->ARG1R.reg = arg;      /* setup the argument register */
    sdhc->CR.reg = cmd;         /* send command */

    /* wait until the command is completed or an error occurred */
    if (!_wait_for_event(sdhc_dev, nis, eis, SDHC_SRR_SWRSTCMD)) {
        return _sdhc_to_sdmmc_err_code(sdhc_dev->error);
    }

    if ((resp_type == SDMMC_R1) || (resp_type == SDMMC_R1B)) {
        dev->status = sdhc->RR[0].reg;
    }

    if (resp) {
        if (resp_type == SDMMC_R2) {
            resp[0] = (sdhc->RR[3].reg << 8) | (sdhc->RR[2].reg >> 24);
            resp[1] = (sdhc->RR[2].reg << 8) | (sdhc->RR[1].reg >> 24);
            resp[2] = (sdhc->RR[1].reg << 8) | (sdhc->RR[0].reg >> 24);
            resp[3] = (sdhc->RR[0].reg << 8);
        }
        else if (resp_type != SDMMC_NO_R) {
            resp[0] = sdhc->RR[0].reg;
        }
    }

    return 0;
}

static int _set_bus_width(sdmmc_dev_t *dev, sdmmc_bus_width_t width)
{
    sdhc_dev_t *sdhc_dev = container_of(dev, sdhc_dev_t, sdmmc_dev);

    assert(sdhc_dev);
    assert(sdhc_dev->conf);

    sdhc_t *sdhc = sdhc_dev->conf->sdhc;

#ifdef SDHC_HC1R_EXTDW
    switch (width) {
    case SDMMC_BUS_WIDTH_1BIT:
        sdhc->HC1R.bit.EXTDW = 0;
        sdhc->HC1R.bit.DW = 0;
        break;
    case SDMMC_BUS_WIDTH_4BIT:
        sdhc->HC1R.bit.EXTDW = 0;
        sdhc->HC1R.bit.DW = 1;
        break;
    case SDMMC_BUS_WIDTH_8BIT:
        sdhc->HC1R.bit.EXTDW = 1;
        sdhc->HC1R.bit.DW = 0;
        break;
    }
#else
    switch (width) {
    case SDMMC_BUS_WIDTH_1BIT:
        sdhc->HC1R.bit.DW = 0;
        break;
    case SDMMC_BUS_WIDTH_4BIT:
        sdhc->HC1R.bit.DW = 1;
        break;
    case SDMMC_BUS_WIDTH_8BIT:
        DEBUG("[sdmmc] Bus width of 8 bit not supported\n");
        return -ENOTSUP;
    }
#endif

    return 0;
}

static int _set_clock_rate(sdmmc_dev_t *dev, sdmmc_clock_rate_t rate)
{
    sdhc_dev_t *sdhc_dev = container_of(dev, sdhc_dev_t, sdmmc_dev);

    assert(sdhc_dev);
    assert(sdhc_dev->conf);

    sdhc_t *sdhc = sdhc_dev->conf->sdhc;

    uint32_t fsdhc = (rate > CONFIG_SDMMC_CLK_MAX) ? CONFIG_SDMMC_CLK_MAX
                                                   : rate;
    if (IS_USED(MODULE_SDMMC_MMC) &&
        (dev->type == SDMMC_CARD_TYPE_MMC) && (fsdhc > MHZ(25))) {
        /* maximum frequency supported for MMCs */
        fsdhc = MHZ(25);
    }
    else if (fsdhc > MHZ(50)) {
        /* maximum frequency supported for SD/SDIO High Speed */
        fsdhc = MHZ(50);
    }

    /* disable the clock to the card if already active */
    if (_disable_sd_clk(sdhc_dev)) {
        return -EIO;
    }

    uint32_t base_clk = sdhc->CA0R.bit.BASECLKF;
    uint32_t clk_mult = sdhc->CA1R.bit.CLKMULT;

#if defined(CPU_SAMD5X) || defined(CPU_SAME5X)
    /* if CA1R.CLKMULT is 0, programmable clock is not supported */
    assert(clk_mult);
    base_clk = (base_clk == 0) ? sam0_gclk_freq(SDHC_CLOCK) / 2 : MHZ(base_clk);
#endif

    uint32_t div;

    /* divider for programmable clock: f_sdclk = f_baseclk / (div + 1) */
    div = DIV_ROUND_UP(base_clk * (clk_mult + 1), fsdhc);
    div = (div) ? div - 1 : div;

    /* enable programmable clock if multiplier is defined */
    if (clk_mult) {
        sdhc->CCR.reg |= SDHC_CCR_CLKGSEL;
    }

    /* write the 10 bit clock divider */
    sdhc->CCR.reg &= ~(SDHC_CCR_USDCLKFSEL_Msk | SDHC_CCR_SDCLKFSEL_Msk);
    sdhc->CCR.reg |= SDHC_CCR_SDCLKFSEL(div) | SDHC_CCR_USDCLKFSEL(div >> 8);
    sdhc->CCR.reg |= SDHC_CCR_INTCLKEN;  /* enable internal clock       */
    while (!sdhc->CCR.bit.INTCLKS) {}    /* wait for clock to be stable */

#if 0
    /* for testing purposes if it is necessary to enable the SD clock when
     * the clock rate is changed */
    if (!IS_USED(MODULE_PERIPH_SDMMC_AUTO_CLK)) {
        /* if periph_sdmmc_auto_clk is not used, enable the clock to the card here */
        if (_enable_sd_clk(sdhc_dev)) {
            return -EIO;
        }
    }
#endif

    return 0;
}

#if !IS_USED(MODULE_PERIPH_SDMMC_AUTO_CLK)
int _enable_clock(sdmmc_dev_t *dev, bool enable)
{
    DEBUG("[sdmmc] %s clock\n", enable ? "enable" : "disable");

    sdhc_dev_t *sdhc_dev = container_of(dev, sdhc_dev_t, sdmmc_dev);

    return (enable) ? _enable_sd_clk(sdhc_dev)
                    : _disable_sd_clk(sdhc_dev);
}
#endif

static int _xfer_prepare(sdmmc_dev_t *dev, sdmmc_xfer_desc_t *xfer)
{
    sdhc_dev_t *sdhc_dev = container_of(dev, sdhc_dev_t, sdmmc_dev);

    assert(sdhc_dev);
    assert(sdhc_dev->conf);

    sdhc_t *sdhc = sdhc_dev->conf->sdhc;

    /* SDHC uses 32-bit words */
    /* TODO: at the moment only 32-bit words supported */
    assert((xfer->block_size % sizeof(uint32_t)) == 0);

    /* indicate that a data transfer is prepared */
    sdhc_dev->data_transfer = true;

    uint32_t tmr;

    tmr  = xfer->write ? SDHC_TMR_DTDSEL_WRITE : SDHC_TMR_DTDSEL_READ;

    if (xfer->block_num > 1) {
        tmr |= SDHC_TMR_MSBSEL_MULTIPLE | SDHC_TMR_BCEN;
        tmr |= IS_USED(MODULE_PERIPH_SDMMC_AUTO_CMD12) ? SDHC_TMR_ACMDEN_CMD12 : 0;
    }
    else {
        tmr |= SDHC_TMR_MSBSEL_SINGLE;
    }

    sdhc->TMR.reg = tmr;
    sdhc->BSR.reg = SDHC_BSR_BLOCKSIZE(xfer->block_size) | SDHC_BSR_BOUNDARY_4K;
    sdhc->BCR.reg = (xfer->block_num == 1) ? 0 : SDHC_BCR_BCNT(xfer->block_num);

    return 0;
}

static int _xfer_execute(sdmmc_dev_t *dev, sdmmc_xfer_desc_t *xfer,
                         const void *data_wr, void *data_rd,
                         uint16_t *done)
{
    assert(xfer);
    assert((xfer->write && data_wr) || (!xfer->write && data_rd));

    /* check the alignment required for the buffers */
    assert(HAS_ALIGNMENT_OF(data_wr, SDMMC_CPU_DMA_ALIGNMENT));
    assert(HAS_ALIGNMENT_OF(data_rd, SDMMC_CPU_DMA_ALIGNMENT));

    sdhc_dev_t *sdhc_dev = container_of(dev, sdhc_dev_t, sdmmc_dev);

    assert(sdhc_dev);
    assert(sdhc_dev->conf);

    sdhc_t *sdhc = sdhc_dev->conf->sdhc;

    uint32_t ret = 0;

    if (dev->status & SDMMC_CARD_STATUS_ERRORS) {
        ret = -EIO;
        goto out;
    }

    int num_words = (xfer->block_num * xfer->block_size) >> 2;

    if (xfer->write) {
        const uint32_t *data_to_write = data_wr;
        do {
            /* wait until there is space in the write buffer */
            if (!_wait_for_event(sdhc_dev, SDHC_NISIER_BWRRDY,
                                 SDHC_EISTR_DATTEO | SDHC_EISTR_DATEND | SDHC_EISTR_DATCRC,
                                 SDHC_SRR_SWRSTDAT)) {
                ret = _sdhc_to_sdmmc_err_code(sdhc_dev->error);
                goto out;
            }

            /* write data to the buffer as long there is space */
            while (sdhc->PSR.bit.BUFWREN && num_words && !sdhc->EISTR.reg) {
                sdhc->BDPR.reg = *data_to_write++;
                num_words--;
            }
        } while (num_words);
    }
    else {
        uint32_t *data_to_read = data_rd;
        do {
            /* wait until there is data in the read buffer */
            if (!_wait_for_event(sdhc_dev, SDHC_NISIER_BRDRDY,
                                 SDHC_EISTR_DATTEO | SDHC_EISTR_DATEND | SDHC_EISTR_DATCRC,
                                 SDHC_SRR_SWRSTDAT)) {
                ret = _sdhc_to_sdmmc_err_code(sdhc_dev->error);
                goto out;
            }

            /* read all data that is available */
            while (sdhc->PSR.bit.BUFRDEN && num_words /* && !sdhc->EISTR.reg */) {
                *data_to_read++ = sdhc->BDPR.reg;
                num_words--;
            }
        } while (num_words);
    }

    /* wait for transfer complete */
    if (!_wait_for_event(sdhc_dev, SDHC_NISTR_TRFC,
                         SDHC_EISTR_DATTEO | SDHC_EISTR_DATEND | SDHC_EISTR_DATCRC,
                         SDHC_SRR_SWRSTALL)) {
        ret = _sdhc_to_sdmmc_err_code(sdhc_dev->error);
        goto out;
    }

out:
    sdhc_dev->data_transfer = false;
    sdhc_dev->error = 0;

    if (done) {
        *done = xfer->block_num - sdhc->BCR.reg;
    }

    return ret;
}

static int _xfer_finish(sdmmc_dev_t *dev, sdmmc_xfer_desc_t *xfer)
{
    (void)xfer;

    sdhc_dev_t *sdhc_dev = container_of(dev, sdhc_dev_t, sdmmc_dev);

    assert(sdhc_dev);
    assert(sdhc_dev->conf);

    sdhc_t *sdhc = sdhc_dev->conf->sdhc;
    sdhc->NISTR.reg = (SDHC_NISTR_BWRRDY | SDHC_NISTR_BRDRDY);

    if (IS_USED(MODULE_PERIPH_SDMMC_AUTO_CLK)) {
        /* disable the clock to the card */
        return _disable_sd_clk(sdhc_dev);
    }

    return 0;
}

/* Internal functions */
#if defined(CPU_SAMD5X) || defined(CPU_SAME5X)

void _core_init(sdhc_dev_t *sdhc_dev)
{
    sam0_gclk_enable(SDHC_CLOCK_SLOW);
    sam0_gclk_enable(SDHC_CLOCK);

    if (sdhc_dev->conf->sdhc == SDHC0) {
        GCLK->PCHCTRL[SDHC0_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                                         | GCLK_PCHCTRL_GEN(SDHC_CLOCK);
        GCLK->PCHCTRL[SDHC0_GCLK_ID_SLOW].reg = GCLK_PCHCTRL_CHEN
                                              | GCLK_PCHCTRL_GEN(SDHC_CLOCK_SLOW);
        MCLK->AHBMASK.bit.SDHC0_ = 1;
    }
#ifdef SDHC1
    else if (sdhc_dev->conf->sdhc == SDHC1) {
        GCLK->PCHCTRL[SDHC1_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                                         | GCLK_PCHCTRL_GEN(SDHC_CLOCK);
        GCLK->PCHCTRL[SDHC1_GCLK_ID_SLOW].reg = GCLK_PCHCTRL_CHEN
                                              | GCLK_PCHCTRL_GEN(SDHC_CLOCK_SLOW);
        MCLK->AHBMASK.bit.SDHC1_ = 1;
    }
#endif /* SDHC1 */
}

static void _init_pins(sdhc_dev_t *sdhc_dev)
{
    const sdhc_conf_t *conf = sdhc_dev->conf;

    if (gpio_is_valid(conf->cd)) {
        gpio_init_mux(conf->cd, SAM0_SDHC_MUX);
    }
    else {
        sdhc_dev->sdmmc_dev.present = true;
    }

    if (gpio_is_valid(conf->wp)) {
        gpio_init_mux(conf->wp, SAM0_SDHC_MUX);
    }

    if (conf->sdhc == SDHC0) {
        /* data pins are fixed */
        gpio_init_mux(SAM0_SDHC0_PIN_SDDAT0, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC0_PIN_SDDAT1, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC0_PIN_SDDAT2, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC0_PIN_SDDAT3, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC0_PIN_SDCMD, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC0_PIN_SDCK, SAM0_SDHC_MUX);
    }

#ifdef SDHC1
    if (conf->sdhc == SDHC1) {
        /* data pins are fixed */
        gpio_init_mux(SAM0_SDHC1_PIN_SDDAT0, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC1_PIN_SDDAT1, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC1_PIN_SDDAT2, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC1_PIN_SDDAT3, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC1_PIN_SDCMD, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC1_PIN_SDCK, SAM0_SDHC_MUX);
    }
#endif /* SDHC1 */

    sdhc_dev->sdmmc_dev.bus_width = SDMMC_BUS_WIDTH_4BIT;
}

#endif /* defined(CPU_SAMD5X) || defined(CPU_SAME5X) */

/**
 * @brief   Reset the entire SDHC peripheral or a part of it
 * @param   type  SDHC_SRR_SWRSTALL | SDHC_SRR_SWRSTCMD | SDHC_SRR_SWRSTDAT
 */
static void _reset_sdhc(sdhc_dev_t *sdhc_dev, uint8_t type)
{
    sdhc_t *sdhc = sdhc_dev->conf->sdhc;

    sdhc->SRR.reg = type;
    while (sdhc->SRR.reg & type) {}

    if (type == SDHC_SRR_SWRSTALL) {
        sdmmc_dev_t *sdmmc_dev = &sdhc_dev->sdmmc_dev;

        /* peripheral needs a complete re-initialization */
        sdmmc_dev->driver->init(sdmmc_dev);
        /* trigger card_init */
        sdmmc_dev->init_done = false;
    }

    sdhc_dev->data_transfer = 0;

    sdhc->NISIER.reg = SDHC_NISIER_CARD_DETECT;  /* enable card detection interrupt signals */
    sdhc->EISIER.reg = 0;
}

#define SDHC_BUSY_TIMEOUT   500     /* limit SDHC busy time to 500 ms */

static int _wait_sdhc_busy(sdhc_t *sdhc)
{
    uint32_t start = _ZTIMER_NOW(); /* waiting start time in msec */
    uint32_t now = start;

    _ZTIMER_ACQUIRE();
    while ((sdhc->PSR.reg & (SDHC_PSR_CMDINHC | SDHC_PSR_CMDINHD)) &&
           ((now - start) < SDHC_BUSY_TIMEOUT)) {
        now = _ZTIMER_NOW();
    }
    _ZTIMER_RELEASE();
    return ((now - start) >= SDHC_BUSY_TIMEOUT) ? -ETIMEDOUT : 0;
}

/**
 * @brief   Wait for a given event while checking for errors
 *
 * @param   state       SDHC device context
 * @param   event       Event to wait for [SDHC_NISTR_*]
 * @param   error_mask  Mask of errors to be checked [SDHC_EISTR_*]
 * @param   reset       Reset type in case of errors
 *                      [SDHC_SRR_SWRSTALL | SDHC_SRR_SWRSTCMD | SDHC_SRR_SWRSTDAT]
 *
 * @return  true if the given event has occurred, or false if an error has occurred.
 */
static bool _wait_for_event(sdhc_dev_t *sdhc_dev,
                            uint16_t event, uint16_t error_mask,
                            uint8_t reset)
{
    sdhc_t *sdhc = sdhc_dev->conf->sdhc;

    sdhc_dev->error = 0;

    /* TODO: timeout */
    sdhc->NISIER.reg |= event;
    sdhc->EISIER.reg |= error_mask;

    /* block IDLE so that the CPU clock does not stop */

#if defined(CPU_SAMD5X) || defined(CPU_SAME5X)
    pm_block(SAM0_PM_IDLE);
#endif
    mutex_lock(&sdhc_dev->irq_wait);
#if defined(CPU_SAMD5X) || defined(CPU_SAME5X)
    pm_unblock(SAM0_PM_IDLE);
#endif

    sdhc->NISIER.reg &= ~event;
    sdhc->EISIER.reg &= ~error_mask;

    if (sdhc_dev->error & error_mask) {
        if (IS_USED(ENABLE_DEBUG)) {
            DEBUG("[sdmmc] SDHC error: EISTR=%04x, ", sdhc_dev->error);
            switch (reset) {
            case SDHC_SRR_SWRSTCMD:
                DEBUG("reset CMD\n");
                break;
            case SDHC_SRR_SWRSTDAT:
                DEBUG("reset DAT\n");
                break;
            case SDHC_SRR_SWRSTALL:
                DEBUG("reset ALL\n");
                break;
            default:
                assert(false);
                break;
            }
        }
        _reset_sdhc(sdhc_dev, reset);
        return false;
    }

    return true;
}

static int _enable_sd_clk(sdhc_dev_t *sdhc_dev)
{
    sdhc_t *sdhc = sdhc_dev->conf->sdhc;

    if (!(sdhc->CCR.reg & SDHC_CCR_SDCLKEN)) {
        DEBUG("[sdmmc] enable SDCLK\n");
        /* TODO timeout handling */
        sdhc->CCR.reg |= SDHC_CCR_SDCLKEN;   /* enable clock to card        */

        /* a very small delay is required after clock changing */
        volatile unsigned count = (CLOCK_CORECLOCK / US_PER_SEC) * 10;
        while (--count) {}
    }

    return 0;
}

static int _disable_sd_clk(sdhc_dev_t *sdhc_dev)
{
    sdhc_t *sdhc = sdhc_dev->conf->sdhc;

    /* TODO timeout handling */
    if (sdhc->CCR.bit.SDCLKEN) {
        DEBUG("[sdmmc] disable SDCLK\n");
        /* wait for command/data to go inactive */
        while (sdhc->PSR.reg & (SDHC_PSR_CMDINHC | SDHC_PSR_CMDINHD)) {}
        /* disable the clock to card */
        sdhc->CCR.reg &= ~SDHC_CCR_SDCLKEN;
    }
    return 0;

}

static int _sdhc_to_sdmmc_err_code(uint16_t code)
{
    if (code & SDHC_EISTR_CMDIDX) {
        DEBUG("[sdmmc] CMD index error\n");
        return -ENOTSUP;
    }
    else if (code & (SDHC_EISTR_CMDTEO | SDHC_EISTR_DATTEO)) {
        DEBUG("[sdmmc] Timeout error\n");
        return -ETIMEDOUT;
    }
    else if (code & (SDHC_EISTR_CMDCRC | SDHC_EISTR_DATCRC)) {
        DEBUG("[sdmmc] CRC error\n");
        return -EBADMSG;
    }
    else {
        DEBUG("[sdmmc] Other error\n");
        return -EIO;
    }
}

static void _isr(sdhc_dev_t *sdhc_dev)
{
    sdhc_t *sdhc = sdhc_dev->conf->sdhc;

    if (sdhc->NISTR.reg & SDHC_NISTR_CARD_DETECT) {
        DEBUG_PUTS("[sdmmc] card presence changed");

        sdhc->NISTR.reg = SDHC_NISTR_CARD_DETECT;

        sdmmc_dev_t *sdmmc_dev = &sdhc_dev->sdmmc_dev;

        sdmmc_dev->init_done = false;
        sdmmc_dev->present = sdhc->PSR.reg & SDHC_PSR_CARDINS;

        if (sdmmc_dev->event_cb) {
            sdmmc_dev->event_cb(sdmmc_dev,
                                sdmmc_dev->present ? SDMMC_EVENT_CARD_INSERTED
                                                   : SDMMC_EVENT_CARD_REMOVED);
        }
    }

    if (sdhc->NISTR.reg & SDHC_NISTR_ERRINT) {
        sdhc->NISTR.reg = SDHC_NISTR_ERRINT;
        sdhc_dev->error = sdhc->EISTR.reg;
        sdhc->EISTR.reg = SDHC_EISTR_MASK;
        DEBUG_PUTS("[sdmmc] error");
        mutex_unlock(&sdhc_dev->irq_wait);
    }

    if (sdhc->NISTR.reg & (sdhc->NISIER.reg & ~SDHC_NISTR_CARD_DETECT)) {
        sdhc->NISTR.reg = (sdhc->NISIER.reg & ~SDHC_NISTR_CARD_DETECT);
        DEBUG_PUTS("[sdmmc] unlock");
        mutex_unlock(&sdhc_dev->irq_wait);
    }

#ifdef MODULE_CORTEXM_COMMON
    cortexm_isr_end();
#endif
}

void isr_sdhc0(void)
{
    _isr(isr_ctx_0);
}

#ifdef SDHC1
void isr_sdhc1(void)
{
    _isr(isr_ctx_1);
}
#endif

static const sdmmc_driver_t _driver = {
    .init = _init,
    .card_init = NULL,  /* no own card init function */
    .send_cmd = _send_cmd,
    .set_bus_width = _set_bus_width,
    .set_clock_rate = _set_clock_rate,
#if !IS_USED(MODULE_PERIPH_SDMMC_AUTO_CLK)
    .enable_clock = _enable_clock,
#endif
    .xfer_prepare = _xfer_prepare,
    .xfer_execute = _xfer_execute,
    .xfer_finish = _xfer_finish,
};
