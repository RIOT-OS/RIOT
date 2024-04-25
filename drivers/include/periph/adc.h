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
 * # (Low-) Power Implications
 *
 * The ADC peripheral(s) **should** only be powered on while adc_sample() is
 * active. For implementing adc_sample() this means, that the peripheral should
 * be powered on (i.e. through peripheral clock gating) at the beginning of the
 * function and it should be powered back off at the end of the function.
 *
 * If the adc_sample() function is implemented in a way, that it will put the
 * active thread to sleep for a certain amount of time, the implementation
 * might need to block certain power states.
 *
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
#include <stdint.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define default ADC type identifier
 */
#ifndef HAVE_ADC_T
typedef uint_fast8_t adc_t;
#endif

/**
 * @brief   Default ADC undefined value
 */
#ifndef ADC_UNDEF
#define ADC_UNDEF           (UINT_FAST8_MAX)
#endif

/**
 * @brief   Default ADC line access macro
 */
#ifndef ADC_LINE
#define ADC_LINE(x)          (x)
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
 * @brief   Initialize the given ADC line
 *
 * The ADC line is initialized in synchronous, blocking mode.
 *
 * @param[in] line          line to initialize
 *
 * @return                  0 on success
 * @return                  -1 on invalid ADC line
 */
int adc_init(adc_t line);

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
int32_t adc_sample(adc_t line, adc_res_t res);

/**
 * @brief   Capture multiple ADC samples from an ADC line
 *
 * @note requires the `periph_adc_continuous` feature
 *
 * @param[in]  lines        line(s) to sample
 * @param[in]  lines_numof  number of ADC lines to sample
 * @param[out] bufs         target buffer(s)
 * @param[in]  buf_len      number of samples to sample per line
 * @param[in]  res          resolution to use for conversion
 * @param[in]  f_adc        frequency to use for conversion, leave 0 for default
 */
void adc_sample_multi(const adc_t *lines, uint8_t lines_numof,
                      uint16_t **bufs, size_t buf_len,
                      adc_res_t res, uint32_t f_adc);

/**
 * @brief   Configure the ADC with a given resolution for continuous sampling
 *
 * @note requires the `periph_adc_continuous` feature
 *
 * @param[in] res           resolution to use for conversion
 * @param[in] f_adc         frequency to use for conversion, leave 0 for default
 */
void adc_continuous_begin(adc_res_t res, uint32_t f_adc);

/**
 * @brief   Sample an ADC line without powering off the ADC afterwards
 *
 * @note requires the `periph_adc_continuous` feature
 *
 * @param[in] line          line to sample
 *
 * @return                  the sampled value on success
 */
int32_t adc_continuous_sample(adc_t line);

/**
 * @brief   Capture multiple ADC samples from an ADC line
 *
 * @note requires the `periph_adc_continuous` feature
 *
 * @param[in]  line         line to sample
 * @param[out] buf          target buffer
 * @param[in]  len          number of samples to sample
 */
void adc_continuous_sample_multi(adc_t line, uint16_t *buf, size_t len);

/**
 * @brief   Configure the ADC with a given resolution for continuous sampling
 *          on two lines.
 *
 * @note requires the `periph_adc_continuous` feature
 *
 * @param[in] res           resolution to use for conversion
 * @param[in] f_adc         frequency to use for conversion, leave 0 for default
 */
void adc_dual_continuous_begin(adc_res_t res, uint32_t f_adc);

/**
 * @brief   Capture multiple ADC samples from two ADC lines at the same time
 *
 * @note The hardware needs to be capable of sampling two lines at the same time,
 *       e.g. they need to be configured to two separate ADC instances.
 *
 * @note requires the `periph_adc_continuous` feature
 *
 * @param[in]  line         lines to sample
 * @param[out] buf          target buffers
 * @param[in]  len          number of samples to sample
 */
void adc_dual_continuous_sample_multi(adc_t line[2], uint16_t *buf[2], size_t len);

/**
 * @brief   Disable the ADC to save power
 *
 * @note requires the `periph_adc_continuous` feature
 */
void adc_continuous_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_ADC_H */
/** @} */
