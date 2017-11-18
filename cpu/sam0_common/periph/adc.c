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
#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* ADC 0 device configuration */
#define ADC_0_DEV                          ADC
#define ADC_0_IRQ                          ADC_IRQn

/* Prototypes */
static bool _adc_syncing(void);
static void _adc_poweroff(void);
static int _adc_configure(adc_res_t res);

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
#ifdef CPU_SAMD21
    if (ADC_0_DEV->STATUS.reg & ADC_STATUS_SYNCBUSY) {
        return true;
    }
#else /* CPU_SAML21 */
    if (ADC_0_DEV->SYNCBUSY.reg) {
        return true;
    }
#endif
    return false;
}

static void _adc_poweroff(void)
{
    while (_adc_syncing()) {}
    /* Disable */
    ADC_0_DEV->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
    while (_adc_syncing()) {}
    /* Disable bandgap */
#ifdef CPU_SAMD21
    if (ADC_0_REF_DEFAULT == ADC_REFCTRL_REFSEL_INT1V) {
        SYSCTRL->VREF.reg &= ~SYSCTRL_VREF_BGOUTEN;
    }
#else /* CPU_SAML21 */
    if (ADC_0_REF_DEFAULT == ADC_REFCTRL_REFSEL_INTREF) {
        SUPC->VREF.reg &= ~SUPC_VREF_VREFOE;
    }
#endif
}

static int _adc_configure(adc_res_t res)
{
    /* Individual comparison necessary because ADC Resolution Bits are not
     * numerically in order and 16Bit (averaging - not currently supported)
     * falls between 12bit and 10bit.  See datasheet for details */
    assert((res == ADC_RES_8BIT) || (res == ADC_RES_10BIT) ||
           (res == ADC_RES_12BIT));
    _adc_poweroff();
    if (ADC_0_DEV->CTRLA.reg & ADC_CTRLA_SWRST ||
        ADC_0_DEV->CTRLA.reg & ADC_CTRLA_ENABLE ) {
        _done();
        DEBUG("adc: not ready\n");
        return -1;
    }
#ifdef CPU_SAMD21
    /* Power On */
    PM->APBCMASK.reg |= PM_APBCMASK_ADC;
    /* GCLK Setup */
    GCLK->CLKCTRL.reg = (uint32_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
                        (GCLK_CLKCTRL_ID(ADC_GCLK_ID)));
    /* Configure CTRLB Register HERE IS THE RESOLUTION SET! */
    ADC_0_DEV->CTRLB.reg = ADC_0_PRESCALER | res;
    /* Load the fixed device calibration constants */
    ADC_0_DEV->CALIB.reg =
        ADC_CALIB_BIAS_CAL((*(uint32_t*)ADC_FUSES_BIASCAL_ADDR >>
                            ADC_FUSES_BIASCAL_Pos)) |
        ADC_CALIB_LINEARITY_CAL((*(uint64_t*)ADC_FUSES_LINEARITY_0_ADDR >>
                                ADC_FUSES_LINEARITY_0_Pos));
    /* Set Voltage Reference */
    ADC_0_DEV->REFCTRL.reg = ADC_0_REF_DEFAULT;
    /* Disable all interrupts */
    ADC_0_DEV->INTENCLR.reg = (ADC_INTENCLR_SYNCRDY) | (ADC_INTENCLR_WINMON) |
                              (ADC_INTENCLR_OVERRUN) | (ADC_INTENCLR_RESRDY);
    while (_adc_syncing()) {}
    /* Enable bandgap if VREF is internal 1V */
    if (ADC_0_REF_DEFAULT == ADC_REFCTRL_REFSEL_INT1V) {
        SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
    }
#else /* CPU_SAML21 */
    /* Power on */
    MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC;
    /* GCLK Setup */
    GCLK->PCHCTRL[ADC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;
    /* Set Voltage Reference */
    ADC_0_DEV->REFCTRL.reg = ADC_0_REF_DEFAULT;
    /* Configure CTRLB & CTRLC Register */
    ADC_0_DEV->CTRLB.reg = ADC_0_PRESCALER;
    ADC_0_DEV->CTRLC.reg |= res;
    /* Disable all interrupts */
    ADC_0_DEV->INTENCLR.reg = ADC_INTENCLR_WINMON | ADC_INTENCLR_OVERRUN |
                              ADC_INTENCLR_RESRDY;
    /* Set default calibration from NVM */
    ADC_0_DEV->CALIB.reg =
            ADC_FUSES_BIASCOMP((*(uint32_t*)ADC_FUSES_BIASCOMP_ADDR)) >>
            ADC_CALIB_BIASCOMP_Pos |
            ADC_FUSES_BIASREFBUF((*(uint32_t*)ADC_FUSES_BIASREFBUF_ADDR) >>
            ADC_FUSES_BIASREFBUF_Pos);
    while (_adc_syncing()) {}
    /* Enable bandgap if necessary */
    if (ADC_0_REF_DEFAULT == ADC_REFCTRL_REFSEL_INTREF) {
        SUPC->VREF.reg |= SUPC_VREF_VREFOE;
    }
#endif
    /*  Enable ADC Module */
    ADC_0_DEV->CTRLA.reg |= ADC_CTRLA_ENABLE;
    while (_adc_syncing()) {}
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
    if (line >= ADC_NUMOF) {
        DEBUG("adc: line arg not applicable\n");
        return -1;
    }
    _prep();
    if (_adc_configure(res) != 0) {
        _done();
        DEBUG("adc: configuration failed\n");
        return -1;
    }
#ifdef CPU_SAMD21
    ADC_0_DEV->INPUTCTRL.reg = ADC_0_GAIN_FACTOR_DEFAULT |
                               adc_channels[line].muxpos | ADC_0_NEG_INPUT;
#else /* CPU_SAML21 */
    ADC_0_DEV->INPUTCTRL.reg = adc_channels[line].muxpos | ADC_0_NEG_INPUT;
#endif
    while (_adc_syncing()) {}
    /* Start the conversion */
    ADC_0_DEV->SWTRIG.reg = ADC_SWTRIG_START;
    /* Wait for the result */
    while (!(ADC_0_DEV->INTFLAG.reg & ADC_INTFLAG_RESRDY)) {}
    int result = ADC_0_DEV->RESULT.reg;
    _adc_poweroff();
    _done();
    return result;
}
