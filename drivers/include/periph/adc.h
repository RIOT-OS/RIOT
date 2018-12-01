/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_adc ADC
 * @ingroup     drivers_periph
 * @brief       Low-level ADC peripheral driver interface
 *
 * This is a very simple ADC interface to allow platform independent access to
 * a MCU's ADC unit(s). This interface is intentionally designed as simple as
 * possible, to allow for very easy implementation and maximal portability.
 *
 * As of now, the interface does not allow for any advanced ADC concepts (e.g.
 * continuous mode, scan sequences, injections). It is to be determined, if
 * these features will ever be integrated in this interface, or if it does make
 * more sense to create a second, advanced ADC interface for this.
 *
 * The ADC driver interface is built around the concept of ADC lines. An ADC
 * line in this context is a tuple consisting out of a hardware ADC device (an
 * ADC functional unit on the MCU) and an ADC channel connected to pin.
 *
 * If a MCU has more than one hardware ADC unit, the ADC lines can be mapped in
 * a way, that it is possible to sample multiple lines in parallel, given that
 * the ADC implementation allows for interruption of the program flow while
 * waiting for the result of a conversion (e.g. through putting the calling
 * thread to sleep while waiting for the conversion results).
 *
 * @todo        Extend interface for continuous mode?
 *
 * @{
 *
 * @file
 * @brief       Low-level ADC peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_ADC_H
#define PERIPH_ADC_H

#include <limits.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define default ADC type identifier
 */
#ifndef HAVE_ADC_T
typedef unsigned int adc_t;
#endif

/**
 * @brief   Default ADC undefined value
 */
#ifndef ADC_UNDEF
#define ADC_UNDEF           (UINT_MAX)
#endif

/**
 * @brief   Default ADC line access macro
 */
#ifndef ADC_LINE
#define ADC_LINE(x)         (x)
#endif

/**
 * @brief   Default number of onboard ADC channels
 *
 * It has to be overridden by board definitions.
 */
#ifndef ADC_NUMOF
#define ADC_NUMOF           (0U)
#endif

/**
 * @brief   Set device ID location in adc_t when using extensions
 */
#ifndef ADC_EXT_DEV_LOC
#define ADC_EXT_DEV_LOC     (8*sizeof(adc_t) - 8)
#endif

/**
 * @brief   Line numbers greater than ADC_EXT_THRESH use extensions
 */
#ifndef ADC_EXT_THRESH
#define ADC_EXT_THRESH    ((adc_t)(UINT_MAX) >> 1)
#endif

/**
 * @brief   Convert (device, line) tuple to #adc_t value
 */
#ifndef ADC_EXT_LINE
#define ADC_EXT_LINE(x, y) \
    (adc_t)(~ADC_EXT_THRESH | (x << ADC_EXT_DEV_LOC) | ADC_LINE(y))
#endif

/**
 * @brief   Possible ADC resolution settings
 */
#ifndef HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT = 0,       /**< ADC resolution: 6 bit */
    ADC_RES_8BIT,           /**< ADC resolution: 8 bit */
    ADC_RES_10BIT,          /**< ADC resolution: 10 bit */
    ADC_RES_12BIT,          /**< ADC resolution: 12 bit */
    ADC_RES_14BIT,          /**< ADC resolution: 14 bit */
    ADC_RES_16BIT,          /**< ADC resolution: 16 bit */
} adc_res_t;
#endif

/**
 * @brief   Low-level versions of the ADC functions
 *
 * These are for cpu adc.c implementation and should not be called directly.
 * @{
 */
int adc_init_ll(adc_t line);
int adc_sample_ll(adc_t line, adc_res_t res);
unsigned int adc_channels_ll(void);
/** @} */

#if MODULE_EXTEND_ADC || DOXYGEN
/**
 * @brief   Redirecting versions of the ADC functions
 *
 * These are for the extension interface and should not be called directly.
 * @{
 */
int adc_init_redir(adc_t line);
int adc_sample_redir(adc_t line, adc_res_t res);
unsigned int adc_channels_redir(adc_t line);
/** @} */
#endif /* MODULE_EXTEND_ADC || DOXYGEN */

/**
 * @brief   Initialize the given ADC line
 *
 * The ADC line is initialized in synchronous, blocking mode.
 *
 * @param[in] line          line to initialize
 *
 * @return                  0 on success
 * @return                  -1 on invalid ADC line
 */

static inline int adc_init(adc_t line)
{
#ifdef MODULE_EXTEND_ADC
    if (line > ADC_EXT_THRESH) {
        return adc_init_redir(line);
    }
#endif
#ifdef MODULE_PERIPH_ADC
    return adc_init_ll(line);
#else
    return -1;
#endif
}

/**
 * @brief   Sample a value from the given ADC line
 *
 * This function blocks until the conversion has finished. Please note, that if
 * more than one line share the same ADC device, and if these lines are sampled
 * at the same time (e.g. from different threads), the one called secondly waits
 * for the first to finish before its conversion starts.
 *
 * @param[in] line          line to sample
 * @param[in] res           resolution to use for conversion
 *
 * @return                  the sampled value on success
 * @return                  -1 if resolution is not applicable
 */
static inline int adc_sample(adc_t line, adc_res_t res)
{
#ifdef MODULE_EXTEND_ADC
    if (line > ADC_EXT_THRESH) {
        return adc_sample_redir(line, res);
    }
#endif
#ifdef MODULE_PERIPH_ADC
    return adc_sample_ll(line, res);
#else
    return -1;
#endif
}

/**
 * @brief   Returns the number of channels of the device
 *
 * This function returns the number of channels of the device that provides
 * the channel identified by the line. If there is no device or no channel
 * that corresponds to the line, the function returns 0
 *
 * @param[in] line  identifies the device
 * @retval  the number of channels on success or 0 on error
 */
static inline unsigned int adc_channels(adc_t line)
{
#ifdef MODULE_EXTEND_ADC
    if (line > ADC_EXT_THRESH) {
        return adc_channels_redir(line);
    }
#endif
#if defined(MODULE_PERIPH_ADC)
    return ADC_NUMOF;
#endif
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_ADC_H */
/** @} */
