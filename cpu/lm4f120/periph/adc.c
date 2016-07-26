/*
 * Copyright (C) 2016 Marc Poulhiès
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * The current ADC driver implementation only supports ADC0.
 *
 * @author      Marc Poulhiès <dkm@kataplop.net>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"

/*
 * @brief   ADC sequence used by this driver and oversampling settings
 * @{
 */
#define SEQ             (3)
#define OVERSAMPLE      (64)
/** @} */

/**
 * @brief   pin configuration parameters
 */
struct adc_gpio_cfg_s {
    unsigned long gpio_base;
    unsigned long gpio_sysctl;
    unsigned short gpio_pin;
};

/**
 * @brief   Fixed ADC pin configuration
 */
static const struct adc_gpio_cfg_s adc0_gpio[ADC_NUMOF] = {
    { GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_3 }, /**< AIN0 */
    { GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_2 }, /**< AIN1 */
    { GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_1 }, /**< AIN2 */
    { GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_0 }, /**< AIN3 */
    { GPIO_PORTD_BASE, SYSCTL_PERIPH_GPIOD, GPIO_PIN_3 }, /**< AIN4 */
    { GPIO_PORTD_BASE, SYSCTL_PERIPH_GPIOD, GPIO_PIN_2 }, /**< AIN5 */
    { GPIO_PORTD_BASE, SYSCTL_PERIPH_GPIOD, GPIO_PIN_1 }, /**< AIN6 */
    { GPIO_PORTD_BASE, SYSCTL_PERIPH_GPIOD, GPIO_PIN_0 }, /**< AIN7 */
    { GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_5 }, /**< AIN8 */
    { GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_4 }, /**< AIN9 */
    { GPIO_PORTB_BASE, SYSCTL_PERIPH_GPIOB, GPIO_PIN_4 }, /**< AIN10 */
    { GPIO_PORTB_BASE, SYSCTL_PERIPH_GPIOB, GPIO_PIN_5 }, /**< AIN11 */
};

/**
 * @brief   Lock to prevent concurrent access to the ADC
 */
static mutex_t lock = MUTEX_INIT;

static inline void prep(void)
{
    mutex_lock(&lock);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
}

static inline void done(void)
{
    ROM_SysCtlPeripheralDisable(SYSCTL_PERIPH_ADC0);
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    /* make sure the given ADC line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    prep();

    ROM_SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);
    ROM_ADCHardwareOversampleConfigure(ADC0_BASE, OVERSAMPLE);

    ROM_SysCtlPeripheralEnable(adc0_gpio[line].gpio_sysctl);
    ROM_GPIOPinTypeADC(adc0_gpio[line].gpio_base, adc0_gpio[line].gpio_pin);

    done();

    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    int value[2];

    if ((res != ADC_RES_10BIT) && (res != ADC_RES_12BIT)) {
        return -1;
    }

    prep();

    /* set channel */
    ROM_ADCSequenceConfigure(ADC0_BASE, SEQ, ADC_TRIGGER_PROCESSOR, 0);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, SEQ, 0, line | ADC_CTL_IE | ADC_CTL_END);
    /* set resolution */
    ROM_ADCResolutionSet(ADC0_BASE, (unsigned long)res);

    /* start conversion and wait for results */
    ROM_ADCSequenceEnable(ADC0_BASE, SEQ);
    ROM_ADCIntClear(ADC0_BASE, SEQ);
    ROM_ADCProcessorTrigger(ADC0_BASE, SEQ);
    while (!ROM_ADCIntStatus(ADC0_BASE, SEQ, false)) {}

    /* get results */
    ROM_ADCSequenceDataGet(ADC0_BASE, SEQ, (unsigned long *) value);

    /* disable device again */
    ROM_ADCSequenceDisable(ADC0_BASE, SEQ);
    done();

    return value[0];
}
