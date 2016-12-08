/*
 * Copyright (C)  2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-common
 * @{
 *
 * @file
 * @brief       Shared peripheral MCU configuration for all nucleo32 boards
 *
 * @author      Hauke Petersen  <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_NUCLEO32_H
#define PERIPH_CONF_NUCLEO32_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 0), .dev = ADC_1, .chan = 0 },
    { .pin = GPIO_PIN(PORT_A, 1), .dev = ADC_1, .chan = 1 },
    { .pin = GPIO_PIN(PORT_A, 3), .dev = ADC_1, .chan = 3 },
    { .pin = GPIO_PIN(PORT_A, 4), .dev = ADC_1, .chan = 4 },
    { .pin = GPIO_PIN(PORT_A, 5), .dev = ADC_1, .chan = 5 },
    { .pin = GPIO_PIN(PORT_A, 6), .dev = ADC_1, .chan = 6 },
    { .pin = GPIO_PIN(PORT_A, 7), .dev = ADC_1, .chan = 7 },
    { .pin = GPIO_PIN(PORT_A, 2), .dev = ADC_1, .chan = 2 }
};

#define ADC_NUMOF           (sizeof(adc_config) / sizeof(adc_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_NUCLEO32_H */
/** @} */
