/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_extend_adc ADC extension
 * @ingroup     drivers_extend
 * @brief       ADC peripheral extension handling
 *
 * The ADC extension interface makes handling of non-CPU ADC devices invisible
 * to code using the ADC periph API (periph/adc.h). This is accomplished by
 * reserving part of the range of values of adc_t. When a call to the ADC API
 * uses a line that falls within this range, it is parsed into a device ID that
 * is looked up in the ADC extension list and the call is redirected to the
 * corresponding device.
 *
 * @{
 *
 * @file
 * @brief       ADC extension interface definitions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef EXTEND_ADC_H
#define EXTEND_ADC_H

#include <limits.h>
#include <stdint.h>

#include "periph_conf.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ADC_EXT_DEV_MASK
/**
 * @brief   Set device mask in adc_t
 */
#define ADC_EXT_DEV_MASK (adc_t)((UINT_MAX << ADC_EXT_DEV_LOC) & ADC_EXT_THRESH)
#endif

#ifndef ADC_EXT_LINE_MASK
/**
 * @brief   Set line mask in adc_t
 */
#define ADC_EXT_LINE_MASK (adc_t)(~(UINT_MAX << ADC_EXT_DEV_LOC))
#endif

/**
 * @brief   Callback typedef for adc_init
 *
 * @see #adc_init
 */
typedef int (*adc_ext_init_t)(void *dev, adc_t line);

/**
 * @brief   Callback typedef for adc_sample
 *
 * @see #adc_sample
 */
typedef int (*adc_ext_sample_t)(void *dev, adc_t line, adc_res_t res);

/**
 * @brief   Callback typedef for adc_channels
 *
 * @see #adc_channels
 */
typedef unsigned int (*adc_ext_channels_t)(void *dev);

/**
 * @brief   Default not supported functions
 * @{
 */
int adc_ext_init_notsup(void *dev, adc_t line);
int adc_ext_sample_notsup(void *dev, adc_t line, adc_res_t res);
unsigned int adc_ext_channels(void *dev);
/** @} */

/**
 * @brief   ADC extension driver entry
 */
typedef struct adc_ext_driver {
    adc_ext_init_t const init;          /**< callback for adc_ext_init */
    adc_ext_sample_t const sample;      /**< callback for adc_ext_sample */
    adc_ext_channels_t const channels;  /**< callback for adc_ext_channels */
} adc_ext_driver_t;

/**
 * @brief   ADC extension list entry
 */
typedef struct adc_ext {
    adc_ext_driver_t const *driver;     /**< pointer to driver */
    void *dev;                          /**< pointer to device descriptor */
} adc_ext_t;

/**
 * @brief   Find an entry in the extension list by ADC line number
 *
 * @param[in]   line    ADC to look up
 * @return      pointer to the list entry
 * @return      NULL if no device is listed
 */
adc_ext_t *adc_ext_entry(adc_t line);

/**
 * @brief   Strip encoding from a ADC and return device number
 *
 * @param[in]   line    ADC to strip
 * @return      device number
 */
static inline adc_t adc_ext_dev(adc_t line)
{
    return ((line & ADC_EXT_DEV_MASK) >> ADC_EXT_DEV_LOC);
}

/**
 * @brief   Strip encoding from a ADC and return line number
 *
 * @param[in]   line    ADC to strip
 * @return      line number
 */
static inline adc_t adc_ext_line(adc_t line)
{
    return (line & ADC_EXT_LINE_MASK);
}

#ifdef __cplusplus
}
#endif

#endif /* EXTEND_ADC_H */
/** @} */
