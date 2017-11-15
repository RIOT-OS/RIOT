/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_nucleo
 * @brief       XX
 * @{
 *
 * @file
 * @brief       XXX
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_ADC_144_ACCFFF_H
#define PERIPH_ADC_144_ACCFFF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32F429zi order. Instead, we
 * just define 6 ADC channels, for the Nucleo
 * Arduino header pins A0-A5
 *
 * @{
 */
#define ADC_NUMOF          (6U)
#define ADC_CONFIG {               \
    {GPIO_PIN(PORT_A,  3), 2,  3}, \
    {GPIO_PIN(PORT_C,  0), 2, 10}, \
    {GPIO_PIN(PORT_C,  3), 2, 13}, \
    {GPIO_PIN(PORT_F,  3), 2,  9}, \
    {GPIO_PIN(PORT_F,  5), 2, 15}, \
    {GPIO_PIN(PORT_F, 10), 2,  8}, \
}
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_ADC_144_ACCFFF_H */
/** @} */
