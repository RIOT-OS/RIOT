/*
 * SPDX-FileCopyrightText: 2017 Dan Evans <photonthunder@gmail.com>
 * SPDX-FileCopyrightText: 2017 Travis Griggs <travisgriggs@gmail.com>
 * SPDX-FileCopyrightText: 2017 Dylan Laduranty <dylanladuranty@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "macros/units.h"
#include "macros/utils.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#if CPU_COMMON_SAMD5X && (ADC_REFCTRL_REFSEL_INTREF == ADC_REF_DEFAULT)
#  if MODULE_ZTIMER_USEC || MODULE_ZTIMER_MSEC
#    include "ztimer.h"
#  else
#    include "busy_wait.h"
#  endif
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef ADC_GCLK_SRC
#  define ADC_GCLK_SRC SAM0_GCLK_MAIN
#endif

#ifndef ADC_GAIN_FACTOR_DEFAULT
#  define ADC_GAIN_FACTOR_DEFAULT (0)
#endif

#ifndef ADC_NEG_INPUT
#  define ADC_NEG_INPUT (0)
#endif

typedef struct {
    uint32_t f_adc;
    uint32_t R_src;
    uint8_t samplen;
} adc_multi_cfg_t;

/* Prototypes */
static void _adc_poweroff(Adc *dev);
static void _setup_clock(Adc *dev, adc_res_t res, adc_multi_cfg_t *mcfg);
static void _setup_calibration(Adc *dev);
static int _adc_configure(Adc *dev, adc_res_t res, adc_multi_cfg_t *mcfg);

static mutex_t _lock = MUTEX_INIT;

static uint8_t _shift, _bits;

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

static inline uint8_t _res_bits(adc_res_t res)
{
    switch (res) {
    case ADC_RES_8BIT:
        return 8;
    case ADC_RES_10BIT:
        return 10;
    case ADC_RES_12BIT:
    default:    /* 16 bit is multiple 12 bit samples */
        return 12;
    }
}

#define MATH_LN_2   0.693147181f    /**< ln(2) */

static uint8_t _calc_samplen(uint32_t R_src, uint32_t f_adc, uint8_t bits)
{
    return (SAM0_ADC_R_SAMPLE + R_src) * SAM0_ADC_C_SAMPLE * (bits + 2) * MATH_LN_2
           * f_adc;
}

static void _find_presc(uint32_t f_src, adc_res_t res, uint32_t f_tgt,
                        uint8_t *prescale, uint8_t *samplen, uint32_t R_src)
{
    uint32_t _best_match = UINT32_MAX;
    uint32_t diff = UINT32_MAX;

    /* ADC Module GCLK max input freq */
    assert(f_src <= MHZ(100));

    /* minimal prescaler right shift */
#if defined(ADC_CTRLA_PRESCALER_DIV2) || defined(ADC_CTRLB_PRESCALER_DIV2)
    const uint8_t start = 1;  /* DIV2 is smallest prescaler */
#else
    const uint8_t start = 2;  /* DIV4 is smallest prescaler */
#endif
    uint8_t end = start + 8;
    uint8_t bits = _res_bits(res);

    for (uint8_t i = start; i < end; ++i) {

        uint32_t f_adc = f_src >> i;
        if (f_adc < SAM0_ADC_CLOCK_FREQ_MIN ||
            f_adc > SAM0_ADC_CLOCK_FREQ_MAX) {
            /* frequency outside valid ADC Clock Period window */
            continue;
        }
#ifdef CPU_COMMON_SAMD21
        /* SAM D2x counts in half CLK_ADC cycles */
        f_adc <<= 1;
#endif
        uint8_t samplen_min = _calc_samplen(R_src, f_adc, bits) + 1;
        assert(samplen_min <= 32);

        /* SAMPLEN register is offset by one (SAMPLEN+1) */
        for (uint8_t _samplen = 32; _samplen > samplen_min; --_samplen) {
            diff = _absdiff(f_adc / (_samplen + bits), f_tgt);
            if (diff < _best_match) {
                _best_match = diff;
                *samplen  = _samplen - 1;
                *prescale = i - start;
            }
        }
    }

     DEBUG("adc.c: f_src=%"PRIu32", f_tgt=%"PRIu32", diff=%"PRIu32", prescaler=%u, samplen=%u, bits=%u\n",
           f_src, f_tgt, diff, *prescale, *samplen, bits);
}

#ifdef ADC_CTRLB_PRESCALER_Pos
#define ADC_PRESCALER_Pos   ADC_CTRLB_PRESCALER_Pos
#else
#define ADC_PRESCALER_Pos   ADC_CTRLA_PRESCALER_Pos
#endif

static void _setup_clock(Adc *dev, adc_res_t res, adc_multi_cfg_t *mcfg)
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

    if (mcfg) {
        _find_presc(sam0_gclk_freq(ADC_GCLK_SRC), res, mcfg->f_adc,
                    &prescaler, &mcfg->samplen, mcfg->R_src);
    }

    /* Configure prescaler */
#ifdef ADC_CTRLB_PRESCALER
    dev->CTRLB.reg = prescaler << ADC_CTRLB_PRESCALER_Pos;
#else
    dev->CTRLA.reg = prescaler << ADC_CTRLA_PRESCALER_Pos;
#endif
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

static int _adc_configure(Adc *dev, adc_res_t res, adc_multi_cfg_t *mcfg)
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

    _setup_clock(dev, res, mcfg);
    _setup_calibration(dev);

    /* Set ADC resolution */
#ifdef ADC_CTRLC_RESSEL
    /* Reset resolution bits in CTRLC */
    uint32_t ctrlc = dev->CTRLC.reg;
    dev->CTRLC.reg = ((ctrlc & ~ADC_CTRLC_RESSEL_Msk) | ADC_CTRLC_RESSEL(res));
#else
    /* Reset resolution bits in CTRLB */
    uint32_t ctrlb = dev->CTRLB.reg;
    dev->CTRLB.reg = ((ctrlb & ~ADC_CTRLB_RESSEL_Msk) | ADC_CTRLB_RESSEL(res));
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
    _bits = _res_bits(res);

    /*  Enable ADC Module */
    dev->CTRLA.reg |= ADC_CTRLA_ENABLE;
    _wait_syncbusy(dev);

#if CPU_COMMON_SAMD5X && (ADC_REFCTRL_REFSEL_INTREF == ADC_REF_DEFAULT)
    /* From the errata
     * > 2.1.6 Internal Bandgap Reference
     * > ================================
     * >
     * > If the internal bandgap voltage reference is selected
     * > (REFCTRL.REFSEL = 0x0), ADC conversions may never complete
     * > (INTFLAG.RESRDY = 0).
     * >
     * > Workaround
     * > ----------
     * >
     * > If CTRLA.ONDEMAND = 0: Add a delay of minimum 40 μs between the
     * > enable of the ADC (CTRLA.ENABLE) and the start of the first conversion.
     * > [...]
     *
     * We do so using ztimer if used anyway, or busy waiting otherwise.
     */
#  if MODULE_ZTIMER_USEC
    ztimer_sleep(ZTIMER_USEC, 40);
#  elif MODULE_ZTIMER_MSEC
    ztimer_sleep(ZTIMER_MSEC, 1);
#  else
    /* busy_wait_us() is not super accurate. We just wait for twice the
     * time to be extra sure the delay is enough. */
    busy_wait_us(2 * 40);
#  endif
#endif

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

static inline void _config_line(Adc *dev, adc_t line, bool diffmode, bool freerun,
                                uint8_t samplen)
{
    dev->SAMPCTRL.reg = samplen;
    dev->INPUTCTRL.reg = ADC_GAIN_FACTOR_DEFAULT
                       | adc_channels[line].inputctrl
                       | (diffmode ? 0 : ADC_NEG_INPUT);
    if (diffmode) {
#if   defined(ADC_CTRLB_DIFFMODE)
        dev->CTRLB.reg |= ADC_CTRLB_DIFFMODE;
#elif defined(ADC_CTRLC_DIFFMODE)
        dev->CTRLC.reg |= ADC_CTRLC_DIFFMODE;
#endif
    } else {
#if   defined(ADC_CTRLB_DIFFMODE)
        dev->CTRLB.reg &= ~ADC_CTRLB_DIFFMODE;
#elif defined(ADC_CTRLC_DIFFMODE)
        dev->CTRLC.reg &= ~ADC_CTRLC_DIFFMODE;
#endif
    }

    if (freerun) {
#ifdef ADC_CTRLB_FREERUN
        dev->CTRLB.reg |= ADC_CTRLB_FREERUN;
#else
        dev->CTRLC.reg |= ADC_CTRLC_FREERUN;
#endif
    } else {
#ifdef ADC_CTRLB_FREERUN
        dev->CTRLB.reg &= ~ADC_CTRLB_FREERUN;
#else
        dev->CTRLC.reg &= ~ADC_CTRLC_FREERUN;
#endif
    }

    _wait_syncbusy(dev);
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

    /* 0 is DIV2, on MCUs where 0 is DIV4, the formula uses half clock cylces,
       so it works out the same */
    uint8_t clock_shift = (ADC_PRESCALER >> ADC_PRESCALER_Pos) + 1;
    uint32_t f_adc = sam0_gclk_freq(ADC_GCLK_SRC) >> clock_shift;

    uint8_t samplen = _calc_samplen(adc_channels[line].R_src, f_adc, _bits);

    /* configure ADC line */
    _config_line(dev, line, diffmode, 0, samplen);

    /* clear stale flag */
    dev->INTFLAG.reg = ADC_INTFLAG_RESRDY;

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

void adc_continuous_begin(adc_res_t res)
{
    bool adc0, adc1;
    _get_adcs(&adc0, &adc1);

    mutex_lock(&_lock);

    if (adc0) {
        _adc_configure(_adc(0), res, 0);
    }
    if (adc1) {
        _adc_configure(_adc(1), res, 0);
    }

    _shift = _shift_from_res(res);
}

int32_t adc_continuous_sample(adc_t line)
{
    assert(line < ADC_NUMOF);
    assert(mutex_trylock(&_lock) == 0);

    return _sample(line) << _shift;
}

static void _has_adcs(bool *adc0, bool *adc1,
                      const adc_t *lines, uint8_t lines_numof)
{
#ifndef ADC1
    (void)lines;
    (void)lines_numof;
    *adc0 = true;
    *adc1 = false;
    return;
#else
    *adc0 = false;
    *adc1 = false;
    for (unsigned i = 0; i < lines_numof; ++i) {
        if (_dev(lines[i]) == ADC0) {
            *adc0 = true;
        } else if (_dev(lines[i]) == ADC1) {
            *adc1 = true;
        }
    }
#endif
}

/* when sampling multiple lines, use the largest sample length for all to
   ensure consistent sample times for all lines */
static uint8_t _max_R_src(uint8_t lines_numof, const adc_t lines[lines_numof])
{
    uint32_t R_src = 0;
    for (unsigned i = 0; i < lines_numof; ++i) {
        adc_t line  = lines[i];
        if (R_src < adc_channels[line].R_src) {
            R_src = adc_channels[line].R_src;
        }
    }

    return R_src;
}

void adc_sample_multi(uint8_t lines_numof, const adc_t lines[lines_numof],
                      size_t buf_len, uint16_t bufs[lines_numof][buf_len],
                      adc_res_t res, uint32_t f_adc)
{
    mutex_lock(&_lock);

    _shift = _shift_from_res(res);

    bool adc0, adc1;
    _has_adcs(&adc0, &adc1, lines, lines_numof);

    adc_multi_cfg_t mcfg = {
        .f_adc = f_adc,
        .R_src = _max_R_src(lines_numof, lines),
    };

    if (adc0) {
        _adc_configure(_adc(0), res, &mcfg);
    }
    if (adc1) {
        _adc_configure(_adc(1), res, &mcfg);
    }

    bool lockstep = false;
#ifdef ADC1
    if (lines_numof == 2 &&
        _dev(lines[0]) != _dev(lines[1])) {
        /* let ADC0 control ADC1 in lock-step */
        ADC1->CTRLA.reg = 0;
        _wait_syncbusy(ADC1);

        ADC1->CTRLA.reg = ADC_CTRLA_SLAVEEN
                        | ADC_CTRLA_ENABLE;
        lockstep = true;
    }
#endif

    Adc *dev[lines_numof];
    bool diffmode[lines_numof];
    for (unsigned i = 0; i < lines_numof; ++i) {
        dev[i] = _dev(lines[i]);
        diffmode[i] = adc_channels[lines[i]].inputctrl & ADC_INPUTCTRL_DIFFMODE;
    }

    if (lockstep || lines_numof == 1) {

        for (unsigned i = 0; i < lines_numof; ++i) {
            /* configure ADC line */
            _config_line(dev[i], lines[i], diffmode[i], 1, mcfg.samplen);

            /* Start the conversion */
            dev[i]->SWTRIG.reg = ADC_SWTRIG_START;
        }

        for (size_t s = 0; s < buf_len; ++s) {
            /* Wait for the result */
            while (!(dev[0]->INTFLAG.reg & ADC_INTFLAG_RESRDY)) {}
            dev[0]->INTFLAG.reg = ADC_INTFLAG_RESRDY;

            for (unsigned i = 0; i < lines_numof; ++i) {
                bufs[i][s] = _sample_dev(dev[i], diffmode[i]) << _shift;
            }
        }
    } else {
        for (size_t s = 0; s < buf_len; ++s) {
            for (unsigned i = 0; i < lines_numof; ++i) {
                /* configure ADC line */
                _config_line(dev[i], lines[i], diffmode[i], 1, mcfg.samplen);

                /* Start the conversion */
                dev[i]->SWTRIG.reg = ADC_SWTRIG_START;

                /* Wait for the result */
                while (!(dev[i]->INTFLAG.reg & ADC_INTFLAG_RESRDY)) {}
                dev[i]->INTFLAG.reg = ADC_INTFLAG_RESRDY;
                bufs[i][s] = _sample_dev(dev[i], diffmode[i]) << _shift;
            }
        }
    }

    if (adc0) {
        _adc_poweroff(_adc(0));
    }
    if (adc1) {
        _adc_poweroff(_adc(1));
    }

    mutex_unlock(&_lock);
}

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
