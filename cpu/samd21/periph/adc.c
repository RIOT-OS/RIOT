/*
 * Copyright (C) 2017 Dan Evans <photonthunder@gmail.com>
 * Copyright (C) 2017 Travis Griggs <travisgriggs@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include "cpu.h"
#include "periph/gpio.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "mutex.h"

/* Only if we actually have any ADCs */
#if ADC_NUMOF

/* Prototypes */
static bool _adc_syncing(void);
static void _adc_powerOff(void);
static int _adc_configure(uint32_t resolution);

static mutex_t _lock = MUTEX_INIT;

static inline void _prep(void)
{
    mutex_lock(&_lock);
}

static inline void _done(void)
{
    mutex_unlock(&_lock);
}

static bool _adc_syncing(void)
{
    if (ADC_0_DEV->STATUS.reg & ADC_STATUS_SYNCBUSY) {
        return true;
    }
    return false;
}

static void _adc_powerOff(void)
{
    while (_adc_syncing()) {}
    /* Disable */
    ADC_0_DEV->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
    /* Reset */
    ADC_0_DEV->CTRLA.reg |= ADC_CTRLA_SWRST;
    /* Disable bandgap */
    if (ADC_0_REF_DEFAULT == ADC_REFCTRL_REFSEL_INT1V) {
        SYSCTRL->VREF.reg &= ~SYSCTRL_VREF_BGOUTEN;
    }
}

static int _adc_configure(uint32_t resolution)
{
    while (_adc_syncing()) {}
    /*  Disable ADC Module before init */
    ADC_0_DEV->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
    /* Power On */
    PM->APBCMASK.reg |= PM_APBCMASK_ADC;
    if (ADC_0_DEV->CTRLA.reg & ADC_CTRLA_SWRST) {
        _done();
        return -1;
    }
    if (ADC_0_DEV->CTRLA.reg & ADC_CTRLA_ENABLE) {
        _done();
        return -1;
    }
    /* GCLK Setup */
    GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (ADC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
    /* Set Voltage Reference */
    ADC_0_DEV->REFCTRL.reg = ADC_0_REF_DEFAULT;
    /* Configure CTRLB Register HERE IS THE RESOLUTION SET! */
    ADC_0_DEV->CTRLB.reg = ADC_0_PRESCALER | resolution;
    /* Disable all interrupts */
    ADC_0_DEV->INTENCLR.reg =
        (1 << ADC_INTENCLR_SYNCRDY_Pos) |
        (1 << ADC_INTENCLR_WINMON_Pos) |
        (1 << ADC_INTENCLR_OVERRUN_Pos) |
        (1 << ADC_INTENCLR_RESRDY_Pos);
    /* Load the fixed device calibration constants */
    ADC_0_DEV->CALIB.reg =
        ADC_CALIB_BIAS_CAL((*(uint32_t*)ADC_FUSES_BIASCAL_ADDR >> ADC_FUSES_BIASCAL_Pos)) |
        ADC_CALIB_LINEARITY_CAL((*(uint64_t*)ADC_FUSES_LINEARITY_0_ADDR >> ADC_FUSES_LINEARITY_0_Pos));
    while (_adc_syncing()) {}
    /* Enable bandgap if VREF is internal 1V */
    if (ADC_0_REF_DEFAULT == ADC_REFCTRL_REFSEL_INT1V) {
        SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
    }
    /*  Enable ADC Module */
    ADC_0_DEV->CTRLA.reg |= ADC_CTRLA_ENABLE;
    return 0;
}

int adc_init(adc_t line)
{
    _prep();
    gpio_init(adc_channels[line].pin, GPIO_IN);
    gpio_init_mux(adc_channels[line].pin, GPIO_MUX_B);
    _done();
    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    if (line >= ADC_0_CHANNELS) {
        return -1;
    }
    uint32_t resolution;
    switch (res) {
        case ADC_RES_8BIT:
            resolution = ADC_CTRLB_RESSEL_8BIT;
            break;
        case ADC_RES_10BIT:
            resolution = ADC_CTRLB_RESSEL_10BIT;
            break;
        case ADC_RES_12BIT:
            resolution = ADC_CTRLB_RESSEL_12BIT;
            break;
        default:
            resolution = 0xFFFFFFFF;
            return -1;
    }
    _prep();
    if (_adc_configure(resolution) != 0) {
        _done();
        return -1;
    }
    ADC_0_DEV->INPUTCTRL.reg = ADC_0_GAIN_FACTOR_DEFAULT | adc_channels[line].muxpos | ADC_0_NEG_INPUT;
    while (_adc_syncing()) {}
    /* Start the conversion */
    ADC_0_DEV->SWTRIG.reg = ADC_SWTRIG_START;
    /* Wait for the result */
    while (!(ADC_0_DEV->INTFLAG.reg & ADC_INTFLAG_RESRDY)) {}
    int result = ADC_0_DEV->RESULT.reg;
    _adc_powerOff();
    _done();
    return result;
}

#endif /* #if ADC_NUMOF */
