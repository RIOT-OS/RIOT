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

#ifndef PERIPH_ADC_144_ACCCCC_H
#define PERIPH_ADC_144_ACCCCC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32F412zg order. Instead, we
 * just define 6 ADC channels, for the Nucleo
 * Arduino header pins A0-A5
 *
 * @{
 */
#define ADC_NUMOF          (6U)
#define ADC_CONFIG {              \
    {GPIO_PIN(PORT_A, 3), 0, 3},  \
    {GPIO_PIN(PORT_C, 0), 0, 10}, \
    {GPIO_PIN(PORT_C, 3), 0, 13}, \
    {GPIO_PIN(PORT_C, 1), 0, 11}, \
    {GPIO_PIN(PORT_C, 4), 0, 14}, \
    {GPIO_PIN(PORT_C, 5), 0, 15}, \
}
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_ADC_144_ACCCCC_H */
/** @} */
