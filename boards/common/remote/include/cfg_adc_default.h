/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Zolertia SL
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_remote
 * @{
 *
 * @file
 * @brief       Common default ADC configuration for the RE-Mote board revision A
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name ADC configuration
 * @{
 */
#define SOC_ADC_ADCCON3_EREF  SOC_ADC_ADCCON3_EREF_AVDD5

static const adc_conf_t adc_config[] = {
    GPIO_PIN(PORT_A, 5), /**< GPIO_PA5 = ADC1_PIN */
    GPIO_PIN(PORT_A, 4), /**< GPIO_PA4 = ADC2_PIN */
    /* voltage divider with 5/3 relationship to allow 5V sensors */
    GPIO_PIN(PORT_A, 2), /**< GPIO_PA2 = ADC3_PIN */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
