/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 * Copyright (C) 2018 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l4
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "xtimer.h"

/**
 * @brief map CPU specific register/value names
 */
#if defined(CPU_MODEL_STM32L476RG)
#define ADC_CR_REG      CR
#define ADC_ISR_REG     ISR
#define ADC_PERIPH_CLK  AHB2
/* on stm32-l476rg all ADC clocks are are enabled by this bit
   further clock config is possible over CKMODE[1:0] bits in ADC_CCR reg */
#define ADC_CLK_EN_MASK   (RCC_AHB2ENR_ADCEN)
/* referring to Datasheet Section 6.3.18 (ADC characteristics) the minimum
   achievable sampling rate is 4.21 Msps (12 Bit resolution on slow channel)
   we use that worst case for configuring the sampling time to be sure it
   works on all channels.
   TCONV = Sampling time + 12.5 ADC clock cycles.
   At 80MHz this means we need to set SMP to 001 (6.5 ADC clock cycles) to
   stay within specs. (80000000/(6.5+12.5)) = 4210526   */
#define ADC_SMP_MIN_VAL      (0x1)

/* The sampling time can be specified for each channel over SMPR1 and SMPR2.
   This specifies the first channel that goes to SMPR2 instead of SMPR1. */
#define ADC_SMP_BIT_WIDTH    (3)

/* The sampling time can be specified for each channel over SMPR1 and SMPR2.
   This specifies the first channel that goes to SMPR2 instead of SMPR1. */
#define ADC_SMPR2_FIRST_CHAN (10)
#endif

/**
 * @brief   Load the ADC configuration
 */
static const adc_conf_t adc_config[] = ADC_CONFIG;

/**
 * @brief   Allocate locks for all three available ADC devices
 */
static mutex_t locks[ADC_DEVS];

static inline ADC_TypeDef *dev(adc_t line)
{
    return (ADC_TypeDef *)(ADC1_BASE + (adc_config[line].dev << 8));
}

static inline void prep(adc_t line)
{
    mutex_lock(&locks[adc_config[line].dev]);
    periph_clk_en(ADC_PERIPH_CLK, ADC_CLK_EN_MASK);
}

static inline void done(adc_t line)
{
/* on STM32L476RG (TODO: maybe true for other L4's? - haven't checked yet)
   all adc devices are controlled by this one bit.
   So don't disable the clock as other devices may still use it */
#if !defined(CPU_MODEL_STM32L476RG)
    periph_clk_dis(ADC_PERIPH_CLK, ADC_CLK_EN_MASK);
#endif
    mutex_unlock(&locks[adc_config[line].dev]);
}

/**
 * @brief   Extract the port base address from the given pin identifier
 */
static inline GPIO_TypeDef *_port(gpio_t pin)
{
    return (GPIO_TypeDef *)(pin & ~(0x0f));
}

/**
 * @brief   Extract the pin number from the last 4 bit of the pin identifier
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

int adc_init(adc_t line)
{
    /* check if the line is valid */
    if (line >= ADC_NUMOF) {
        return -1;
    }

    /* lock device and enable its peripheral clock */
    prep(line);

    /* set prescaler to 0 to let the ADC run with maximum speed */
    ADC123_COMMON->CCR &= ~(ADC_CCR_PRESC);

    /* Setting ADC clock to HCLK/1 is only allowed if AHB clock prescaler is 1*/
    if (!(RCC->CFGR & RCC_CFGR_HPRE_3)) {
        /* set ADC clock to HCLK/1 */
        ADC123_COMMON->CCR |= (ADC_CCR_CKMODE_0);
    }
    else {
        /* set ADC clock to HCLK/2 otherwise */
        ADC123_COMMON->CCR |= (ADC_CCR_CKMODE_1);
    }

    /* configure the pin */
    gpio_init_analog(adc_config[line].pin);

#if defined(CPU_MODEL_STM32L476RG) || defined(CPU_MODEL_STM32L475VG)
    /* On STM32L475xx/476xx/486xx devices, before any conversion of an input channel coming
       from GPIO pads, it is necessary to configure the corresponding GPIOx_ASCR register in
       the GPIO, in addition to the I/O configuration in analog mode. */
    _port(adc_config[line].pin)->ASCR |= (1 << _pin_num(adc_config[line].pin));
#endif

    /* init ADC line only if it wasn't already initialized */
    if (!(dev(line)->ADC_CR_REG & (ADC_CR_ADEN))) {
        /* reset state of bit DEEPPWD is 1 -> so first leave deep-power down mode */
        dev(line)->ADC_CR_REG &= ~(ADC_CR_DEEPPWD);

        /* enable ADC internal voltage regulator and wait for startup period */
        dev(line)->ADC_CR_REG |= (ADC_CR_ADVREGEN);
        xtimer_usleep(ADC_T_ADCVREG_STUP_US);

        /* configure calibration for single ended input */
        dev(line)->ADC_CR_REG &= ~(ADC_CR_ADCALDIF);

        /* ´start automatic calibration and wait for it to complete */
        dev(line)->ADC_CR_REG |= ADC_CR_ADCAL;
        while (dev(line)->ADC_CR_REG  & ADC_CR_ADCAL) {}

        /* clear ADRDY by writing it*/
        dev(line)->ADC_ISR_REG |= (ADC_ISR_ADRDY);

        /* enable ADC and wait for it to be ready */
        dev(line)->ADC_CR_REG |= (ADC_CR_ADEN);
        while ((dev(line)->ADC_ISR_REG & ADC_ISR_ADRDY) == 0) {}

        /* set sequence length to 1 conversion */
        dev(line)->SQR1 |= (0 & ADC_SQR1_L);
    }

    /* configure sampling time for the given channel */
    if (adc_config[line].chan < ADC_SMPR2_FIRST_CHAN) {
        dev(line)->SMPR1 =  (ADC_SMP_MIN_VAL << (adc_config[line].chan *
                                                 ADC_SMP_BIT_WIDTH));
    }
    else {
        dev(line)->SMPR2 =  (ADC_SMP_MIN_VAL << ((adc_config[line].chan -
                                                  ADC_SMPR2_FIRST_CHAN)
                                                 * ADC_SMP_BIT_WIDTH));
    }

    /* free the device again */
    done(line);
    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* check if resolution is applicable */
    if (res & 0x3) {
        return -1;
    }

    /* lock and power on the ADC device  */
    prep(line);

    /* first clear resolution */
    dev(line)->CFGR &= ~(ADC_CFGR_RES);

    /* then set resolution to the required value*/
    dev(line)->CFGR |= res;

    /* specify channel for regular conversion */
    dev(line)->SQR1 = (adc_config[line].chan << ADC_SQR1_SQ1_Pos);

    /* start conversion and wait for it to complete */
    dev(line)->ADC_CR_REG |= ADC_CR_ADSTART;
    while (!(dev(line)->ISR & ADC_ISR_EOC)) {}

    /* read the sample */
    sample = (int)dev(line)->DR;

    /* free the device again */
    done(line);

    return sample;
}
