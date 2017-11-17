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

#ifndef PERIPH_ADC_144_AC_H
#define PERIPH_ADC_144_AC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC configuration
 *
 * We need to define the following fields:
 * PIN, device (ADCx), channel
 * @{
 */
#define ADC_CONFIG {              \
    {GPIO_PIN(PORT_A, 3), 0, 3},  \
    {GPIO_PIN(PORT_C, 0), 1, 0}  \
}
#define ADC_NUMOF          (2)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_ADC_144_AC_H */
/** @} */
