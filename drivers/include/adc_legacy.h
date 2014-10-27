/*
 * Copyright 2012 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_adc ADC
 * @ingroup     drivers
 * @brief       Generic interface for ADC drivers
 * @deprecated  This interface is obsolete. Use the @ref driver_periph_adc
 *              interface in @ref driver_periph instead.
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @{
 * @file
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize ADC.
 */
void adc_init(void);

/**
 * @brief   Read ADC value of selected channel.
 *
 * Valid channel numbers are from 0 to 2.
 *
 * @return  ADC value of selected channel.
 */
uint16_t adc_read(uint8_t channel);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ADC_H */
