/*
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_vbat
 * @{
 *
 * @file
 * @brief       Implementation of STM32 backup battery monitoring
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include "board.h"
#include "periph_conf.h"
#include "periph/adc.h"
#include "periph/vbat.h"

/**
 * @name   Constants depending on CPU line
 * @{
 *
 * @ref VBAT_ADC_SCALE is a scale factor to calculate the right voltage value
 * as documented in the data sheet.
 * @ref VBAT_ADC_MIN_MV <= VBAT[mV] is the smallest voltage level required
 * to power the backup domain.
 */
/* f0 */
#if   defined(CPU_LINE_STM32F031x6) || defined(CPU_LINE_STM32F038xx)    || \
      defined(CPU_LINE_STM32F042x6) || defined(CPU_LINE_STM32F048xx)    || \
      defined(CPU_LINE_STM32F051x8) || defined(CPU_LINE_STM32F058xx)    || \
      defined(CPU_LINE_STM32F071xB) || defined(CPU_LINE_STM32F072xB)    || \
      defined(CPU_LINE_STM32F078xx) || defined(CPU_LINE_STM32F091xC)    || \
      defined(CPU_LINE_STM32F098xx)
#define VBAT_ADC_SCALE        2
#define VBAT_ADC_MIN_MV       1650
/* f2 */
#elif defined(CPU_LINE_STM32F205xx) || defined(CPU_LINE_STM32F207xx)    || \
      defined(CPU_LINE_STM32F215xx) || defined(CPU_LINE_STM32F217xx)
#define VBAT_ADC_SCALE        2
#define VBAT_ADC_MIN_MV       1800
/* f3 */
#elif defined(CPU_LINE_STM32F301x8) || defined(CPU_LINE_STM32F302x8)    || \
      defined(CPU_LINE_STM32F302xC) || defined(CPU_LINE_STM32F302xE)    || \
      defined(CPU_LINE_STM32F303x8) || defined(CPU_LINE_STM32F303xC)    || \
      defined(CPU_LINE_STM32F303xE) || defined(CPU_LINE_STM32F318x8)    || \
      defined(CPU_LINE_STM32F318xx) || defined(CPU_LINE_STM32F328xx)    || \
      defined(CPU_LINE_STM32F334x8) || defined(CPU_LINE_STM32F358xx)    || \
      defined(CPU_LINE_STM32F373xC) || defined(CPU_LINE_STM32F378xx)    || \
      defined(CPU_LINE_STM32F398xx)
#define VBAT_ADC_SCALE        2
#define VBAT_ADC_MIN_MV       1650
/* f4 */
#elif defined(CPU_LINE_STM32F401xC) || defined(CPU_LINE_STM32F401xE)    || \
      defined(CPU_LINE_STM32F410Cx) || defined(CPU_LINE_STM32F410Rx)    || \
      defined(CPU_LINE_STM32F410Tx) || defined(CPU_LINE_STM32F411xE)    || \
      defined(CPU_LINE_STM32F412Cx) || defined(CPU_LINE_STM32F412Rx)    || \
      defined(CPU_LINE_STM32F412Vx) || defined(CPU_LINE_STM32F412Zx)    || \
      defined(CPU_LINE_STM32F413xx) || defined(CPU_LINE_STM32F423xx)    || \
      defined(CPU_LINE_STM32F427xx) || defined(CPU_LINE_STM32F429xx)    || \
      defined(CPU_LINE_STM32F437xx) || defined(CPU_LINE_STM32F439xx)    || \
      defined(CPU_LINE_STM32F446xx) || defined(CPU_LINE_STM32F469xx)    || \
      defined(CPU_LINE_STM32F479xx)
#define VBAT_ADC_SCALE        4
#define VBAT_ADC_MIN_MV       1650
#elif defined(CPU_LINE_STM32F405xx) || defined(CPU_LINE_STM32F407xx)    || \
      defined(CPU_LINE_STM32F415xx) || defined(CPU_LINE_STM32F417xx)
#define VBAT_ADC_SCALE        2
#define VBAT_ADC_MIN_MV       1650
/* f7 */
#elif defined(CPU_LINE_STM32F722xx) || defined(CPU_LINE_STM32F723xx)    || \
      defined(CPU_LINE_STM32F730xx) || defined(CPU_LINE_STM32F732xx)    || \
      defined(CPU_LINE_STM32F733xx) || defined(CPU_LINE_STM32F745xx)    || \
      defined(CPU_LINE_STM32F746xx) || defined(CPU_LINE_STM32F750xx)    || \
      defined(CPU_LINE_STM32F756xx) || defined(CPU_LINE_STM32F765xx)    || \
      defined(CPU_LINE_STM32F767xx) || defined(CPU_LINE_STM32F769xx)    || \
      defined(CPU_LINE_STM32F777xx) || defined(CPU_LINE_STM32F779xx)
#define VBAT_ADC_SCALE        4
#define VBAT_ADC_MIN_MV       1650
/* g0 */
#elif defined(CPU_LINE_STM32G030xx) || defined(CPU_LINE_STM32G031xx)    || \
      defined(CPU_LINE_STM32G041xx) || defined(CPU_LINE_STM32G050xx)    || \
      defined(CPU_LINE_STM32G051xx) || defined(CPU_LINE_STM32G061xx)    || \
      defined(CPU_LINE_STM32G070xx) || defined(CPU_LINE_STM32G071xx)    || \
      defined(CPU_LINE_STM32G081xx) || defined(CPU_LINE_STM32G0B0xx)    || \
      defined(CPU_LINE_STM32G0B1xx) || defined(CPU_LINE_STM32G0C1xx)
#define VBAT_ADC_SCALE        3
#define VBAT_ADC_MIN_MV       1550
/* g4 */
#elif defined(CPU_LINE_STM32G431xx) || defined(CPU_LINE_STM32G441xx)    || \
      defined(CPU_LINE_STM32G471xx) || defined(CPU_LINE_STM32G473xx)    || \
      defined(CPU_LINE_STM32G474xx) || defined(CPU_LINE_STM32G483xx)    || \
      defined(CPU_LINE_STM32G484xx) || defined(CPU_LINE_STM32G491xx)    || \
      defined(CPU_LINE_STM32G4A1xx) || defined(CPU_LINE_STM32G441xx)    || \
      defined(CPU_LINE_STM32GBK1CB)
#define VBAT_ADC_SCALE        3
#define VBAT_ADC_MIN_MV       1550
/* l4 */
#elif defined(CPU_LINE_STM32L412xx) || defined(CPU_LINE_STM32L422xx)    || \
      defined(CPU_LINE_STM32L431xx) || defined(CPU_LINE_STM32L432xx)    || \
      defined(CPU_LINE_STM32L433xx) || defined(CPU_LINE_STM32L442xx)    || \
      defined(CPU_LINE_STM32L443xx) || defined(CPU_LINE_STM32L451xx)    || \
      defined(CPU_LINE_STM32L452xx) || defined(CPU_LINE_STM32L462xx)    || \
      defined(CPU_LINE_STM32L471xx) || defined(CPU_LINE_STM32L475xx)    || \
      defined(CPU_LINE_STM32L476xx) || defined(CPU_LINE_STM32L485xx)    || \
      defined(CPU_LINE_STM32L486xx) || defined(CPU_LINE_STM32L496xx)    || \
      defined(CPU_LINE_STM32L4A6xx) || defined(CPU_LINE_STM32L4P5xx)    || \
      defined(CPU_LINE_STM32L4Q5xx) || defined(CPU_LINE_STM32L4R5xx)    || \
      defined(CPU_LINE_STM32L4R7xx) || defined(CPU_LINE_STM32L4R9xx)    || \
      defined(CPU_LINE_STM32L4S5xx) || defined(CPU_LINE_STM32L4S7xx)    || \
      defined(CPU_LINE_STM32L4S9xx)
#define VBAT_ADC_SCALE        3
#define VBAT_ADC_MIN_MV       1550
/* l5 */
#elif defined(CPU_LINE_STM32L552xx) || defined(CPU_LINE_STM32L562xx)
#define VBAT_ADC_SCALE        3
#define VBAT_ADC_MIN_MV       1550
/* u5 */
#elif defined(CPU_LINE_STM32U575xx) || defined(CPU_LINE_STM32U585xx)
#define VBAT_ADC_SCALE        4
#define VBAT_ADC_MIN_MV       1650
/* wb */
#elif defined(CPU_LINE_STM32WB10xx) || defined(CPU_LINE_STM32WB15xx)    || \
      defined(CPU_LINE_STM32WB30xx) || defined(CPU_LINE_STM32WB35xx)    || \
      defined(CPU_LINE_STM32WB50xx) || defined(CPU_LINE_STM32WB55xx)    || \
      defined(CPU_LINE_STM32WB5Mxx)
#define VBAT_ADC_SCALE        3
#define VBAT_ADC_MIN_MV       1550
/* wl */
#elif defined(CPU_LINE_STM32WL54xx) || defined(CPU_LINE_STM32WL55xx)    || \
      defined(CPU_LINE_STM32WLE4xx) || defined(CPU_LINE_STM32WLE5xx)
#define VBAT_ADC_SCALE        3
#define VBAT_ADC_MIN_MV       1550
#else
#error "VBAT: CPU line is not supported so far."
#endif
/** @} */

/**
 * @name    VBAT enable register
 * @{
 */
#if defined(CPU_LINE_STM32F373xC) || defined(CPU_LINE_STM32F378xx)
#define ADC_CCR_REG (SYSCFG->CFGR1) /* ADCx_COMMON is also defined */
#elif defined(ADC_COMMON)
#define ADC_CCR_REG (ADC_COMMON->CCR)
#elif defined(ADC1_COMMON)
#define ADC_CCR_REG (ADC1_COMMON->CCR)
#elif defined(ADC12_COMMON)
#define ADC_CCR_REG (ADC12_COMMON->CCR)
#elif defined(ADC123_COMMON)
#define ADC_CCR_REG (ADC123_COMMON->CCR)
#elif defined(ADC12_COMMON_NS)
#define ADC_CCR_REG (ADC12_COMMON_NS->CCR)
#elif defined(ADC)
#define ADC_CCR_REG (ADC->CCR)
#else
#error "VBAT: CPU line is not supported so far."
#endif
/** @} */

/**
 * @name    VBAT enable flag
 * @{
 */
#if defined(ADC_CCR_VBATEN)
#define VBAT_ENABLE ADC_CCR_VBATEN;
#elif defined(ADC_CCR_VBATE)
#define VBAT_ENABLE ADC_CCR_VBATE;
#elif defined(ADC_CCR_VBATSEL)
#define VBAT_ENABLE ADC_CCR_VBATSEL;
#elif defined(SYSCFG_CFGR1_VBAT)
#define VBAT_ENABLE SYSCFG_CFGR1_VBAT;
#else
#error "VBAT: CPU line is not supported so far."
#endif
/** @} */

#ifndef CONFIG_VBAT_ADC_VREF_MV
#define CONFIG_VBAT_ADC_VREF_MV 3300            /**< ADC reference voltage */
#endif

/**
 * @brief   Override this function if you know how to retrieve the accurate
 *          ADC supply voltage in mV for your board. The default behaviour is
 *          to return @ref CONFIG_VBAT_ADC_VREF_MV.
 *          Once there is a driver to sample VREFINT, this function is likely
 *          to be changed.
 */
int32_t __attribute__((weak)) vref_mv(void) {
    return CONFIG_VBAT_ADC_VREF_MV;
}

#ifndef VBAT_ADC
#error "VBAT: Add internal VBAT ADC line to adc_config[] and #define VBAT_ADC."
#endif

int vbat_init(void)
{
    return adc_init(VBAT_ADC);
}

void vbat_enable(void)
{
    ADC_CCR_REG |= VBAT_ENABLE;
}

void vbat_disable(void)
{
    ADC_CCR_REG &= ~VBAT_ENABLE;
}

int32_t vbat_sample_mv(void)
{
    int32_t mv = adc_sample(VBAT_ADC, VBAT_ADC_RES);
    mv = (mv * VBAT_ADC_SCALE * vref_mv()) / VBAT_ADC_MAX;
    return mv;
}

bool vbat_is_empty(void)
{
    return VBAT_ADC_MIN_MV > vbat_sample_mv();
}
