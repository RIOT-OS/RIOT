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

/* Prototypes */
static void _adc_poweroff(Adc *dev);
static void _setup_clock(Adc *dev);
static void _setup_calibration(Adc *dev);
static int _adc_configure(Adc *dev, adc_res_t res);

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
    dev->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
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

static void _setup_clock(Adc *dev)
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
    /* Configure prescaler */
    dev->CTRLB.reg = ADC_PRESCALER;
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
        /* Configure prescaler */
        dev->CTRLA.reg = ADC_PRESCALER;
    #else
        /* GCLK Setup */
        GCLK->PCHCTRL[ADC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                | GCLK_PCHCTRL_GEN(ADC_GCLK_SRC);
        /* Configure prescaler */
        dev->CTRLB.reg = ADC_PRESCALER;
    #endif
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

static int _adc_configure(Adc *dev, adc_res_t res)
{
    if ((res == ADC_RES_6BIT) || (res == ADC_RES_14BIT)) {
        return -1;
    }

    _adc_poweroff(dev);

    if (dev->CTRLA.reg & ADC_CTRLA_SWRST ||
        dev->CTRLA.reg & ADC_CTRLA_ENABLE ) {
        DEBUG("adc: not ready\n");
        return -1;
    }

    _setup_clock(dev);
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

static int32_t _sample(adc_t line)
{
    Adc *dev = _dev(line);
    bool diffmode = adc_channels[line].inputctrl & ADC_INPUTCTRL_DIFFMODE;

    dev->INPUTCTRL.reg = ADC_GAIN_FACTOR_DEFAULT
                       | adc_channels[line].inputctrl
                       | (diffmode ? 0 : ADC_NEG_INPUT);
#ifdef ADC_CTRLB_DIFFMODE
    if (diffmode) {
        dev->CTRLB.reg |= ADC_CTRLB_DIFFMODE;
    }
    else {
        dev->CTRLB.reg &= ~ADC_CTRLB_DIFFMODE;
    }
#endif
    _wait_syncbusy(dev);

    /* Start the conversion */
    dev->SWTRIG.reg = ADC_SWTRIG_START;

    /* Wait for the result */
    while (!(dev->INTFLAG.reg & ADC_INTFLAG_RESRDY)) {}

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

static uint8_t _shift_from_res(adc_res_t res)
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
void adc_continuous_begin(adc_res_t res)
{
    bool adc0, adc1;
    _get_adcs(&adc0, &adc1);

    mutex_lock(&_lock);

    if (adc0) {
        _adc_configure(_adc(0), res);
    }
    if (adc1) {
        _adc_configure(_adc(1), res);
    }

    _shift = _shift_from_res(res);
}

int32_t adc_continuous_sample(adc_t line)
{
    assert(line < ADC_NUMOF);
    assert(mutex_trylock(&_lock) == 0);

    return _sample(line) << _shift;
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

    if (_adc_configure(dev, res) != 0) {
        DEBUG("adc: configuration failed\n");
        mutex_unlock(&_lock);
        return -1;
    }

    int val = _sample(line) << _shift_from_res(res);

    _adc_poweroff(dev);
    mutex_unlock(&_lock);

    return val;
}
