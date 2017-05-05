/*
 * Copyright (C) 2017 Dylan Laduranty <dylan.laduranty@mesotic.com>
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

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Only if we actually have any ADCs */
#if ADC_NUMOF

/* Prototypes */
static void _adc_poweroff(void);
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

static void _adc_poweroff(void)
{
    /* Disable */
    ADC_0_DEV->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
    while (ADC_0_DEV->SYNCBUSY.reg);
    /* Disable bandgap */
    if (ADC_0_REF_DEFAULT == ADC_REFCTRL_REFSEL_INTREF) {
        SUPC->VREF.reg &= ~SUPC_VREF_VREFOE;
    }
}

static int _adc_configure(uint32_t resolution)
{
    _adc_poweroff();
    if (ADC_0_DEV->CTRLA.reg & ADC_CTRLA_SWRST ||
        ADC_0_DEV->CTRLA.reg & ADC_CTRLA_ENABLE ) {
        _done();
        DEBUG("adc: not ready\n");
        return -1;
    }
    /* GCLK Setup */
    GCLK->PCHCTRL[ADC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;
    /* Set Voltage Reference */
    ADC_0_DEV->REFCTRL.reg = ADC_0_REF_DEFAULT;
    /* Configure CTRLB & CTRLC Register */
    ADC_0_DEV->CTRLB.reg = ADC_0_PRESCALER;
    ADC_0_DEV->CTRLC.reg |= resolution;
    /* Disable all interrupts */
    ADC_0_DEV->INTENCLR.reg = ADC_INTENCLR_WINMON |ADC_INTENCLR_OVERRUN |
                              ADC_INTENCLR_RESRDY;
    /* Set default calibration from NVM */
    ADC_0_DEV->CALIB.reg =
              ADC_FUSES_BIASCOMP(*(uint32_t*)ADC_FUSES_BIASCOMP_ADDR) |
              ADC_FUSES_BIASREFBUF(*(uint32_t*)ADC_FUSES_BIASREFBUF_ADDR);

    while (ADC_0_DEV->SYNCBUSY.reg);
    /* Enable bandgap if necessary */
    if (ADC_0_REF_DEFAULT == ADC_REFCTRL_REFSEL_INTREF) {
        SUPC->VREF.reg |= SUPC_VREF_VREFOE;
    }
    /*  Enable ADC Module */
    ADC_0_DEV->CTRLA.reg |= ADC_CTRLA_ENABLE;
    /* Wait for sync */
    while (ADC_0_DEV->SYNCBUSY.reg);
    return 0;
}

int adc_init(adc_t line)
{
    _prep();
    gpio_init(adc_channels[line].pin, GPIO_IN);
    gpio_init_mux(adc_channels[line].pin, GPIO_MUX_B);
    MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC;
    _done();
    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    if (line >= ADC_0_CHANNELS) {
        DEBUG("adc: line arg not applicable\n");
        return -1;
    }
    uint32_t resolution;
    switch (res) {
        case ADC_RES_8BIT:
            resolution = ADC_CTRLC_RESSEL_8BIT;
            break;
        case ADC_RES_10BIT:
            resolution = ADC_CTRLC_RESSEL_10BIT;
            break;
        case ADC_RES_12BIT:
            resolution = ADC_CTRLC_RESSEL_12BIT;
            break;
        default:
            resolution = 0xFFFFFFFF;
            DEBUG("adc: Resolution arg not applicable\n");
            return -1;
    }
    _prep();
    if (_adc_configure(resolution) != 0) {
        _done();
        DEBUG("adc: configuration failed\n");
        return -1;
    }
    ADC_0_DEV->INPUTCTRL.reg = adc_channels[line].muxpos | ADC_0_NEG_INPUT;
    /* Wait for sync */
    while (ADC_0_DEV->SYNCBUSY.reg);
    /* Start the conversion */
    ADC_0_DEV->SWTRIG.reg = ADC_SWTRIG_START;
    /* Wait for the result */
    while (!(ADC_0_DEV->INTFLAG.reg & ADC_INTFLAG_RESRDY));
    int result = ADC_0_DEV->RESULT.reg;
    _adc_poweroff();
    _done();
    return result;
}

#endif /* #if ADC_NUMOF */
