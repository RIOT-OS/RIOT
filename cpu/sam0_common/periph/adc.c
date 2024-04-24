/*
 * Copyright (C) 2017 Dan Evans <photonthunder@gmail.com>
 * Copyright (C) 2017 Travis Griggs <travisgriggs@gmail.com>
 * Copyright (C) 2017 Dylan Laduranty <dylanladuranty@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @}
 */
#include <stdint.h>
#include "cpu.h"
#include "periph/gpio.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "macros/utils.h"
#include "mutex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef ADC_GCLK_SRC
    #define ADC_GCLK_SRC SAM0_GCLK_MAIN
#endif

#ifndef ADC_GAIN_FACTOR_DEFAULT
    #define ADC_GAIN_FACTOR_DEFAULT (0)
#endif

#ifndef ADC_NEG_INPUT
    #define ADC_NEG_INPUT (0)
#endif

/* Prototypes */
static void _adc_poweroff(Adc *dev);
static void _setup_clock(Adc *dev, uint32_t f_tgt);
static void _setup_calibration(Adc *dev);
static int _adc_configure(Adc *dev, adc_res_t res, uint32_t f_tgt);

static mutex_t _lock = MUTEX_INIT;

static inline void _wait_syncbusy(Adc *dev)
{
#ifdef ADC_STATUS_SYNCBUSY
    while (dev->STATUS.reg & ADC_STATUS_SYNCBUSY) {}
#else
    /* Ignore the ADC SYNCBUSY.SWTRIG status
     * The ADC SYNCBUSY.SWTRIG gets stuck to '1' after wake-up from Standby Sleep mode.
     * SAMD5x/SAME5x errata: DS80000748 (page 10)
     */
    while (dev->SYNCBUSY.reg & ~ADC_SYNCBUSY_SWTRIG) {}
#endif
}

static void _adc_poweroff(Adc *dev)
{
    _wait_syncbusy(dev);

    /* Disable */
    dev->CTRLA.reg = 0;
    _wait_syncbusy(dev);

    /* Disable bandgap */
#ifdef SYSCTRL_VREF_BGOUTEN
    if (ADC_REF_DEFAULT == ADC_REFCTRL_REFSEL_INT1V) {
        SYSCTRL->VREF.reg &= ~SYSCTRL_VREF_BGOUTEN;
    }
#else
    if (ADC_REF_DEFAULT == ADC_REFCTRL_REFSEL_INTREF) {
        SUPC->VREF.reg &= ~SUPC_VREF_VREFOE;
    }
#endif
}

static uint32_t _absdiff(uint32_t a, uint32_t b)
{
    return a > b ? a - b : b - a;
}

static void _find_presc(uint32_t f_src, uint32_t f_tgt,
                        uint8_t *prescale, uint8_t *samplen)
{
    uint32_t _best_match = UINT32_MAX;

#if defined(ADC_CTRLB_PRESCALER_DIV2) || defined(ADC_CTRLB_PRESCALER_DIV2)
    uint8_t start = 1;
#else
    uint8_t start = 2;
#endif
    uint8_t end = start + 8;
    for (uint8_t i = start; i < end; ++i) {
        for (uint8_t _samplen = 32; _samplen > 0; --_samplen) {
            unsigned diff = _absdiff((f_src >> i) / _samplen, f_tgt);
            if (diff < _best_match) {
                _best_match = diff;
                *samplen  = _samplen;
                *prescale = i;
            }
        }
    }
}

#ifdef ADC_CTRLB_PRESCALER_Pos
#define ADC_PRESCALER_Pos   ADC_CTRLB_PRESCALER_Pos
#else
#define ADC_PRESCALER_Pos   ADC_CTRLA_PRESCALER_Pos
#endif

static void _setup_clock(Adc *dev, uint32_t f_tgt)
{
    /* Enable gclk in case we are the only user */
    sam0_gclk_enable(ADC_GCLK_SRC);

#ifdef PM_APBCMASK_ADC
    /* Power On */
    PM->APBCMASK.reg |= PM_APBCMASK_ADC;
    /* GCLK Setup */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN
                      | GCLK_CLKCTRL_GEN(ADC_GCLK_SRC)
                      | GCLK_CLKCTRL_ID(ADC_GCLK_ID);
#else
    /* Power on */
    #ifdef MCLK_APBCMASK_ADC
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_ADC;
    #else
        #ifdef MCLK_APBDMASK_ADC0
            if (dev == ADC0) {
                MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC0;
            } else {
                MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC1;
            }
        #else
            MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC;
        #endif
    #endif

    #ifdef ADC0_GCLK_ID
        /* GCLK Setup */
        if (dev == ADC0) {
            GCLK->PCHCTRL[ADC0_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                    | GCLK_PCHCTRL_GEN(ADC_GCLK_SRC);
        }
        else {
            GCLK->PCHCTRL[ADC1_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                    | GCLK_PCHCTRL_GEN(ADC_GCLK_SRC);
        }
    #else
        /* GCLK Setup */
        GCLK->PCHCTRL[ADC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                | GCLK_PCHCTRL_GEN(ADC_GCLK_SRC);
    #endif
#endif

    uint8_t prescaler = ADC_PRESCALER >> ADC_PRESCALER_Pos;
    uint8_t sampllen  = 0;

    if (f_tgt) {
        _find_presc(sam0_gclk_freq(ADC_GCLK_SRC), f_tgt,
                    &prescaler, &sampllen);
    }

    /* Configure prescaler */
#ifdef ADC_CTRLB_PRESCALER
    dev->CTRLB.reg = prescaler << ADC_CTRLB_PRESCALER_Pos;
#else
    dev->CTRLA.reg = prescaler << ADC_CTRLA_PRESCALER_Pos;
#endif
    dev->SAMPCTRL.reg = sampllen;
}

static void _setup_calibration(Adc *dev)
{
#ifdef ADC_CALIB_BIAS_CAL
    /* Load the fixed device calibration constants */
    dev->CALIB.reg =
        ADC_CALIB_BIAS_CAL((*(uint32_t*)ADC_FUSES_BIASCAL_ADDR >>
                            ADC_FUSES_BIASCAL_Pos)) |
        ADC_CALIB_LINEARITY_CAL((*(uint64_t*)ADC_FUSES_LINEARITY_0_ADDR >>
                                ADC_FUSES_LINEARITY_0_Pos));
#else
    /* Set default calibration from NVM */
    #ifdef ADC0_FUSES_BIASCOMP_ADDR
        if (dev == ADC0) {
            dev->CALIB.reg =
                ADC0_FUSES_BIASCOMP((*(uint32_t*)ADC0_FUSES_BIASCOMP_ADDR)) >>
                ADC_CALIB_BIASCOMP_Pos |
                ADC0_FUSES_BIASREFBUF((*(uint32_t*)ADC0_FUSES_BIASREFBUF_ADDR) >>
                ADC0_FUSES_BIASREFBUF_Pos);
        }
        else {
            dev->CALIB.reg =
                ADC1_FUSES_BIASCOMP((*(uint32_t*)ADC1_FUSES_BIASCOMP_ADDR)) >>
                ADC_CALIB_BIASCOMP_Pos |
                ADC1_FUSES_BIASREFBUF((*(uint32_t*)ADC1_FUSES_BIASREFBUF_ADDR) >>
                ADC1_FUSES_BIASREFBUF_Pos);
        }
    #else
        dev->CALIB.reg =
                ADC_FUSES_BIASCOMP((*(uint32_t*)ADC_FUSES_BIASCOMP_ADDR)) >>
                ADC_CALIB_BIASCOMP_Pos |
                ADC_FUSES_BIASREFBUF((*(uint32_t*)ADC_FUSES_BIASREFBUF_ADDR) >>
                ADC_FUSES_BIASREFBUF_Pos);
    #endif
#endif
}

static int _adc_configure(Adc *dev, adc_res_t res, uint32_t f_tgt)
{
    if ((res == ADC_RES_6BIT) || (res == ADC_RES_14BIT)) {
        return -1;
    }

    dev->CTRLA.reg = 0;
    _wait_syncbusy(dev);

    while (dev->CTRLA.reg & ADC_CTRLA_SWRST ||
           dev->CTRLA.reg & ADC_CTRLA_ENABLE) {
        DEBUG("adc: not ready\n");
    }

    _setup_clock(dev, f_tgt);
    _setup_calibration(dev);

    /* Set ADC resolution */
#ifdef ADC_CTRLC_RESSEL
    /* Reset resolution bits in CTRLC */
    dev->CTRLC.bit.RESSEL = res & 0x3;
#else
    /* Reset resolution bits in CTRLB */
    dev->CTRLB.bit.RESSEL = res & 0x3;
#endif

    /* Set Voltage Reference */
    dev->REFCTRL.reg = ADC_REF_DEFAULT;
    /* Disable all interrupts */
    dev->INTENCLR.reg = 0xFF;

#ifdef SYSCTRL_VREF_BGOUTEN
    /* Enable bandgap if VREF is internal 1V */
    if (ADC_REF_DEFAULT == ADC_REFCTRL_REFSEL_INT1V) {
        SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
    }
#else
    /* Enable bandgap if necessary */
    if (ADC_REF_DEFAULT == ADC_REFCTRL_REFSEL_INTREF) {
        SUPC->VREF.reg |= SUPC_VREF_VREFOE;
    }
#endif
#ifdef ADC_REFCTRL_REFSEL_AREFA
    if (ADC_REF_DEFAULT == ADC_REFCTRL_REFSEL_AREFA) {
        gpio_init_mux(ADC_REFSEL_AREFA_PIN, GPIO_MUX_B);
    }
#endif
#ifdef ADC_REFCTRL_REFSEL_AREFB
    if (ADC_REF_DEFAULT == ADC_REFCTRL_REFSEL_AREFB) {
        gpio_init_mux(ADC_REFSEL_AREFB_PIN, GPIO_MUX_B);
    }
#endif
#ifdef ADC_REFCTRL_REFSEL_AREFC
    if (ADC_REF_DEFAULT == ADC_REFCTRL_REFSEL_AREFC) {
        gpio_init_mux(ADC_REFSEL_AREFC_PIN, GPIO_MUX_B);
    }
#endif

    if ((res & 0x3) == 1) {
        dev->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM(res >> 2);
    } else {
        dev->AVGCTRL.reg = 0;
    }

    /*  Enable ADC Module */
    dev->CTRLA.reg |= ADC_CTRLA_ENABLE;
    _wait_syncbusy(dev);
    return 0;
}

int adc_init(adc_t line)
{
    if (line >= ADC_NUMOF) {
        DEBUG("adc: line arg not applicable\n");
        return -1;
    }

#ifdef ADC0
    const uint8_t adc = adc_channels[line].dev == ADC1 ? 1 : 0;
#else
    const uint8_t adc = 0;
#endif

    mutex_lock(&_lock);

    uint8_t muxpos = (adc_channels[line].inputctrl & ADC_INPUTCTRL_MUXPOS_Msk)
                   >> ADC_INPUTCTRL_MUXPOS_Pos;
    uint8_t muxneg = (adc_channels[line].inputctrl & ADC_INPUTCTRL_MUXNEG_Msk)
                   >> ADC_INPUTCTRL_MUXNEG_Pos;

    /* configure positive input pin */
    if (muxpos < 0x18) {
        assert(muxpos < ARRAY_SIZE(sam0_adc_pins[adc]));
        gpio_init(sam0_adc_pins[adc][muxpos], GPIO_IN);
        gpio_init_mux(sam0_adc_pins[adc][muxpos], GPIO_MUX_B);
    }

    /* configure negative input pin */
    if (adc_channels[line].inputctrl & ADC_INPUTCTRL_DIFFMODE) {
        assert(muxneg < ARRAY_SIZE(sam0_adc_pins[adc]));
        gpio_init(sam0_adc_pins[adc][muxneg], GPIO_IN);
        gpio_init_mux(sam0_adc_pins[adc][muxneg], GPIO_MUX_B);
    }

     mutex_unlock(&_lock);

    return 0;
}

static Adc *_dev(adc_t line)
{
    /* The SAMD5x/SAME5x family has two ADCs: ADC0 and ADC1. */
#ifdef ADC0
    return adc_channels[line].dev;
#else
    (void)line;
    return ADC;
#endif
}

static Adc *_adc(uint8_t dev)
{
    /* The SAMD5x/SAME5x family has two ADCs: ADC0 and ADC1. */
#ifdef ADC0
    switch (dev) {
    case 0:
        return ADC0;
    case 1:
        return ADC1;
    default:
        return NULL;
    }
#else
    (void)dev;
    return ADC;
#endif
}

static inline void _config_line(Adc *dev, adc_t line, bool diffmode, bool freerun)
{
    dev->INPUTCTRL.reg = ADC_GAIN_FACTOR_DEFAULT
                       | adc_channels[line].inputctrl
                       | (diffmode ? 0 : ADC_NEG_INPUT);
#if defined(ADC_CTRLB_DIFFMODE)
    dev->CTRLB.bit.DIFFMODE = diffmode;
#elif defined(ADC_CTRLC_DIFFMODE)
    dev->CTRLC.bit.DIFFMODE = diffmode;
#endif

#if defined(ADC_CTRLB_FREERUN)
    dev->CTRLB.bit.FREERUN = freerun;
#else
    dev->CTRLC.bit.FREERUN = freerun;
#endif
    _wait_syncbusy(dev);

    /* clear stale flag */
    dev->INTFLAG.reg = ADC_INTFLAG_RESRDY;
}

static int32_t _sample_dev(Adc *dev, bool diffmode)
{
    uint16_t sample = dev->RESULT.reg;
    int result;

    /* in differential mode we lose one bit for the sign */
    if (diffmode) {
        result = 2 * (int16_t)sample;
    } else {
        result = sample;
    }

    return result;
}

static int32_t _sample(adc_t line)
{
    Adc *dev = _dev(line);
    bool diffmode = adc_channels[line].inputctrl & ADC_INPUTCTRL_DIFFMODE;

    /* configure ADC line */
    _config_line(dev, line, diffmode, 0);

    /* Start the conversion */
    dev->SWTRIG.reg = ADC_SWTRIG_START;

    /* Wait for the result */
    while (!(dev->INTFLAG.reg & ADC_INTFLAG_RESRDY)) {}

    return _sample_dev(dev, diffmode);
}

static int8_t _shift_from_res(adc_res_t res)
{
    /* 16 bit mode is implemented as oversampling */
    if ((res & 0x3) == 1) {
        /* ADC does automatic right shifts beyond 16 samples */
        return 4 - MIN(4, res >> 2);
    }
    return 0;
}

static void _get_adcs(bool *adc0, bool *adc1)
{
#ifndef ADC1
    *adc0 = true;
    *adc1 = false;
    return;
#else
    *adc0 = false;
    *adc1 = false;
    for (unsigned i = 0; i < ADC_NUMOF; ++i) {
        if (adc_channels[i].dev == ADC0) {
            *adc0 = true;
        } else if (adc_channels[i].dev == ADC1) {
            *adc1 = true;
        }
    }
#endif
}

static uint8_t _shift;
void adc_continuous_begin(adc_res_t res, uint32_t f_adc)
{
    bool adc0, adc1;
    _get_adcs(&adc0, &adc1);

    mutex_lock(&_lock);

    if (adc0) {
        _adc_configure(_adc(0), res, f_adc);
    }
    if (adc1) {
        _adc_configure(_adc(1), res, f_adc);
    }

    _shift = _shift_from_res(res);
}

int32_t adc_continuous_sample(adc_t line)
{
    assert(line < ADC_NUMOF);
    assert(mutex_trylock(&_lock) == 0);

    return _sample(line) << _shift;
}

void adc_continuous_sample_multi(adc_t line, uint16_t *buf, size_t len)
{
    assert(line < ADC_NUMOF);
    assert(mutex_trylock(&_lock) == 0);

    Adc *dev = _dev(line);
    bool diffmode = adc_channels[line].inputctrl & ADC_INPUTCTRL_DIFFMODE;

    /* configure ADC line */
    _config_line(dev, line, diffmode, 1);

    /* Start the conversion */
    dev->SWTRIG.reg = ADC_SWTRIG_START;

    while (len--) {

        /* Wait for the result */
        while (!(dev->INTFLAG.reg & ADC_INTFLAG_RESRDY)) {}

        *buf++ = _sample_dev(dev, diffmode) << _shift;
        dev->INTFLAG.reg = ADC_INTFLAG_RESRDY;
    }
}

#if defined(ADC0) && defined(ADC1)
void adc_dual_continuous_begin(adc_res_t res, uint32_t f_adc)
{
    mutex_lock(&_lock);

    _adc_configure(ADC0, res, f_adc);
    _adc_configure(ADC1, res, f_adc);

    ADC1->CTRLA.reg = 0;
    _wait_syncbusy(ADC1);

    ADC1->CTRLA.reg = ADC_CTRLA_SLAVEEN
                    | ADC_CTRLA_ENABLE;

    _shift = _shift_from_res(res);
}

void adc_dual_continuous_sample_multi(adc_t line[2], uint16_t *buf[2], size_t len)
{
    assert(line[0] < ADC_NUMOF);
    assert(line[1] < ADC_NUMOF);
    assert(mutex_trylock(&_lock) == 0);

    Adc *dev[2];
    dev[0] = _dev(line[0]);
    dev[1] = _dev(line[1]);
    assert(dev[0] != dev[1]);

    bool diffmode[2];
    diffmode[0] = adc_channels[line[0]].inputctrl & ADC_INPUTCTRL_DIFFMODE;
    diffmode[1] = adc_channels[line[1]].inputctrl & ADC_INPUTCTRL_DIFFMODE;

    /* configure ADC lines */
    _config_line(dev[0], line[0], diffmode[0], 1);
    _config_line(dev[1], line[1], diffmode[1], 1);

    /* Start the conversion */
    ADC0->SWTRIG.reg = ADC_SWTRIG_START;

    while (len--) {

        /* Wait for the result */
        while (!(dev[0]->INTFLAG.reg & ADC_INTFLAG_RESRDY)) {}

        *buf[0]++ = _sample_dev(dev[0], diffmode[0]) << _shift;
        *buf[1]++ = _sample_dev(dev[1], diffmode[1]) << _shift;

        dev[0]->INTFLAG.reg = ADC_INTFLAG_RESRDY;
        dev[1]->INTFLAG.reg = ADC_INTFLAG_RESRDY;
    }
}
#endif

void adc_continuous_stop(void)
{
    bool adc0, adc1;
    _get_adcs(&adc0, &adc1);

    if (adc0) {
        _adc_poweroff(_adc(0));
    }
    if (adc1) {
        _adc_poweroff(_adc(1));
    }

    mutex_unlock(&_lock);
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    if (line >= ADC_NUMOF) {
        DEBUG("adc: line arg not applicable\n");
        return -1;
    }

    mutex_lock(&_lock);

    Adc *dev = _dev(line);

    if (_adc_configure(dev, res, 0) != 0) {
        DEBUG("adc: configuration failed\n");
        mutex_unlock(&_lock);
        return -1;
    }

    int val = _sample(line) << _shift_from_res(res);

    _adc_poweroff(dev);
    mutex_unlock(&_lock);

    return val;
}
