/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_adc
 * @{
 *
 * @file
 * @brief       Global variables used in the common ADC API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef ADC_NG_GLOBALS_H
#define ADC_NG_GLOBALS_H

#include "adc_ng_internal.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(ADC_NG_NUMOF) || defined(DOXYGEN)
/**
 * @brief   Number of ADC devices supported
 *
 * If a board supports more than one ADC, it has to define `ADC_NG_NUMOF` in
 * `perpih_conf.h` and implement (but not declare)
 * `adc_ng_driver_t *adc_ng_drivers[ADC_NG_NUMOF]` and
 * `void *adc_ng_handles[ADC_NG_NUMOF]` (e.g. in `board.c`).
 */
#define ADC_NG_NUMOF                    (1U)
#endif

/**
 * @brief   Array containing the backends available
 */
extern const adc_ng_backend_t adc_ng_backends[ADC_NG_NUMOF];
/**
 * @brief   Currently selected reference voltage in mV
 */
extern int16_t adc_ng_refs[ADC_NG_NUMOF];
/**
 * @brief   Currently selected resolution
 */
extern uint8_t adc_ng_res[ADC_NG_NUMOF];

#ifdef __cplusplus
}
#endif

#endif /* ADC_NG_GLOBALS_H */
/** @} */
