/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_extend_dac DAC extension
 * @ingroup     drivers_extend
 * @brief       DAC peripheral extension handling
 *
 * The DAC extension interface makes handling of non-CPU DAC devices invisible
 * to code using the DAC periph API (periph/dac.h). This is accomplished by
 * reserving part of the range of values of dac_t. When a call to the DAC API
 * uses a line that falls within this range, it is parsed into a device ID that
 * is looked up in the DAC extension list and the call is redirected to the
 * corresponding device.
 *
 * @{
 *
 * @file
 * @brief       DAC extension interface definitions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef EXTEND_DAC_H
#define EXTEND_DAC_H

#include <limits.h>
#include <stdint.h>

#include "periph_conf.h"
#include "periph/dac.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DAC_EXT_DEV_MASK
/**
 * @brief   Set device mask in dac_t
 */
#define DAC_EXT_DEV_MASK (dac_t)((UINT_MAX << DAC_EXT_DEV_LOC) & DAC_EXT_THRESH)
#endif

#ifndef DAC_EXT_LINE_MASK
/**
 * @brief   Set line mask in dac_t
 */
#define DAC_EXT_LINE_MASK (dac_t)(~(UINT_MAX << DAC_EXT_DEV_LOC))
#endif

/**
 * @brief   Callback typedef for dac_init
 *
 * @see #dac_init
 */
typedef int8_t (*dac_ext_init_t)(void *dev, dac_t line);

/**
 * @brief   Callback typedef for dac_set
 *
 * @see #dac_set
 */
typedef void (*dac_ext_set_t)(void *dev, dac_t line, uint16_t value);

/**
 * @brief   Callback typedef for dac_poweron
 *
 * @see #dac_poweron
 */
typedef void (*dac_ext_poweron_t)(void *dev, dac_t line);

/**
 * @brief   Callback typedef for dac_poweroff
 *
 * @see #dac_poweroff
 */
typedef void (*dac_ext_poweroff_t)(void *dev, dac_t line);

/**
 * @brief   Callback typedef for dac_channels
 *
 * @see #dac_channels
 */
typedef unsigned int (*dac_ext_channels_t)(void *dev);

/**
 * @brief   Default not supported functions
 * @{
 */
int8_t dac_ext_init_notsup(void *dev, dac_t line);
void dac_ext_set_notsup(void *dev, dac_t line, uint16_t value);
void dac_ext_poweron_notsup(void *dev, dac_t line);
void dac_ext_poweroff_notsup(void *dev, dac_t line);
unsigned int dac_ext_channels(void *dev);
/** @} */

/**
 * @brief   DAC extension driver entry
 */
typedef struct dac_ext_driver {
    dac_ext_init_t const init;          /**< callback for dac_ext_init */
    dac_ext_set_t const set;            /**< callback for dac_ext_set */
    dac_ext_poweron_t const poweron;    /**< callback for dac_ext_poweron */
    dac_ext_poweroff_t const poweroff;  /**< callback for dac_ext_poweron */
    dac_ext_channels_t const channels;  /**< callback for dac_ext_channels */
} dac_ext_driver_t;

/**
 * @brief   DAC extension list entry
 */
typedef struct dac_ext {
    dac_ext_driver_t const *driver;     /**< pointer to driver */
    void *dev;                          /**< pointer to device descriptor */
} dac_ext_t;

/**
 * @brief   Find an entry in the extension list by DAC line number
 *
 * @param[in]   line    DAC to look up
 * @return      pointer to the list entry
 * @return      NULL if no device is listed
 */
dac_ext_t *dac_ext_entry(dac_t line);

/**
 * @brief   Strip encoding from a DAC and return device number
 *
 * @param[in]   line    DAC to strip
 * @return      device number
 */
static inline dac_t dac_ext_dev(dac_t line)
{
    return ((line & DAC_EXT_DEV_MASK) >> DAC_EXT_DEV_LOC);
}

/**
 * @brief   Strip encoding from a DAC and return line number
 *
 * @param[in]   line    DAC to strip
 * @return      line number
 */
static inline dac_t dac_ext_line(dac_t line)
{
    return (line & DAC_EXT_LINE_MASK);
}

#ifdef __cplusplus
}
#endif

#endif /* EXTEND_DAC_H */
/** @} */
