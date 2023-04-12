/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_adc_ng  Common ADC API
 * @ingroup     drivers_periph
 *
 * This module contains a platform and hardware independent ADC API.
 * It is intended to address both advanced and simple use cases and allow
 * using both external and internal ADCs transparently.
 *
 * @{
 *
 * @file
 * @brief       Interface definition of the common ADC API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef ADC_NG_H
#define ADC_NG_H

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include "adc_ng_internal.h"
#include "bitarithm.h"
#include "xfa.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Pass this special value as parameter `ref` in @ref adc_ng_init
 *          to select the highest supported reference voltage
 */
#define ADC_NG_MAX_REF                  (0U)

/**
 * @name    Helper functions query supported ADC resolutions
 *
 * @{
 */

/**
 * @brief   Maximum resolution supported by the API
 *
 * @note    The maximum resolution supported by drivers can be and often is
 *          lower than this value
 */
#define ADC_NG_MAX_RES                  (32U)

XFA_USE_CONST(adc_ng_backend_t, adc_ng_backends);
XFA_USE(adc_ng_state_t, adc_ng_states);

/**
 * @brief   Check if the given ADC supports the given resolution
 *
 * @param[in]       adc     ADC device to check
 * @param[in]       res     Resolution to check
 *
 * @retval  1               Resolution is supported
 * @retval  0               Resolution is not supported
 */
static inline int adc_ng_supports_res(uint8_t adc, uint8_t res)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    assert((res <= ADC_NG_MAX_RES) && (res > 0));
    const adc_ng_backend_t be = adc_ng_backends[adc];
    return (be.driver->res_supported & (1 << (res - 1)));
}

/**
 * @brief   Get the highest supported resolution of an ADC
 *
 * @param[in]       adc     ADC to get the highest supported resolution of
 */
static inline uint8_t adc_ng_max_res(uint8_t adc)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    const adc_ng_backend_t be = adc_ng_backends[adc];
    return (uint8_t)(bitarithm_msb(be.driver->res_supported) + 1);
}

/**
 * @brief   Get the highest supported resolution of an ADC
 *
 * @param[in]       adc     ADC to get the highest supported resolution of
 */
static inline uint8_t adc_ng_min_res(uint8_t adc)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    const adc_ng_backend_t be = adc_ng_backends[adc];
    return (uint8_t)(bitarithm_lsb(be.driver->res_supported) + 1);
}

/**
 * @brief   Select a resolution supported by the given ADC that is equal to
 *          or greater than the requested resolution.
 *
 * @param[in]       adc     ADC to select an appropriate resolution of
 * @param[in,out]   res     In: The resolution to select. Out: The resolution
 *                          actually selected
 * @param[out]      shift   The number of bits to shift samples to the right
 *                          to achieve the requested resolution
 *
 * @retval  0               Success
 * @retval  -ENOTSUP        No resolution equal to or greater than the requested
 *                          found.
 *
 * Usage:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * uint8_t res = 10, shift;
 * int32_t sample;
 *
 * if (adc_ng_select_res(adc, &res, &shift) {
 *     return -1;
 * }
 *
 * if (adc_ng_init(adc, res, channel, &reference_voltage) {
 *    return -1;
 * }
 *
 * if (adc_ng_single(adc, &sample)) {
 *     return -1;
 * }
 *
 * adc_ng_off(adc);
 *
 * sample >>= shift;
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note    In case of differential channels, the sample can be negative. In
 *          that case an arithmetic right shift is required. Most (all?) C
 *          compilers do so on signed types, but the C standard leaves it up
 *          to the implementation to choose arithmetic or logical right shift.
 *          For single ended ("normal") ADC channels samples are always positive
 *          (or zero), and both flavours of right shift are fine.
 */
static inline int adc_ng_select_res(uint8_t adc, uint8_t *res, uint8_t *shift)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    assert(res != NULL);
    uint8_t res_selected = *res;
    while (!adc_ng_supports_res(adc, res_selected)) {
        if (++res_selected > ADC_NG_MAX_RES) {
            return -ENOTSUP;
        }
    }

    if (shift) {
        *shift = res_selected - *res;
    }
    *res = res_selected;
    return 0;
}

/** @} */

/**
 * @brief   Initialize and power up the ADC channel @p chan of device @p adc
 *
 * @param[in]       adc     ADC device to initialize a channel of
 * @param[in]       chan    Channel to initialize
 * @param[in]       res     Resolution to sample at
 * @param[in,out]   ref     Reference voltage in mV to use (***see details!***)
 *
 * @retval  0               Success
 * @retval  -ENOTSUP        Requested resolution not supported
 * @retval  -ENXIO          No such channel
 * @retval  -ERANGE         Requested reference voltage is higher than all
 *                          available references
 * @retval  -EALREADY       The ADC is already powered and configured
 * @retval  <0              A driver specific error occurred
 *
 * @note    On success, the ADC is powered up and consumes additional power
 *          until @ref adc_ng_off is called.
 *
 * The reference voltage to use is given with @p ref in millivolt. The driver
 * will pick a reference voltage that is as close to @p ref as possible, but
 * doesn't reduce the range. (The absolute value of the chosen reference is not
 * smaller than the absolute value of the requested reference and it has the
 * same sign as the requested reference.) The actually chosen reference voltage
 * is stored in @p ref.
 *
 * E.g. if a device supports reference voltages -2.56V, -1.1V, 1.1V, and 2.56V
 * and the user requests a reference voltage of 1.2V (`*ref == 1200`), the
 * reference 2.56V is chosen even though 1.1V is a closer match. Otherwise
 * input voltages above 1.1V would no longer be distinguishable. Similar if
 * -1.3V is requested -2.56V is chosen over -1.1V.
 */
int adc_ng_init(uint8_t adc, uint8_t chan, uint8_t res, int16_t *ref);

/**
 * @brief   Turn of the given ADC device
 *
 * @param[in]       adc     ADC device to turn off
 */
static inline void adc_ng_off(uint8_t adc)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    const adc_ng_backend_t be = adc_ng_backends[adc];
    if (be.driver->off) {
        be.driver->off(be.handle);
    }
}

/**
 * @brief   Return the number of the highest single ended channel of the
 *          given ADC
 *
 * @param[in]       adc     ADC to get the number of the highest single
 *                          ended channel of
 *
 * @details All channel from zero to the returned value are single ended
 *          channels
 * @note    Some ADCs might have "holes" in the channel map, e.g. channels
 *          0, 1 and 3 might be available, but channel 2 is not supported.
 *          Checking for the return value `-ENXIO` in @ref adc_ng_init can be
 *          used to detect these "holes".
 *
 * A single ended input is the "normal" ADC channel where a single input
 * voltage is converted into an unsigned integer with the value 0 referring to
 * an input voltage of 0 V and the highest value referring to an input voltage
 * equal to the voltage reference (or higher).
 */
static inline uint8_t adc_ng_highest_single_ended_channel(uint8_t adc)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    const adc_ng_backend_t be = adc_ng_backends[adc];
    return be.driver->highest_single_ended_channel;
}

/**
 * @name    Functions access to access the raw ADC output
 *
 * These functions return the binary value returned by the ADC, rather than
 * meaningful physical units.
 *
 * @{
 */

/**
 * @brief   Perform a single conversion using the specified ADC channel
 *
 * @param[in]       adc     ADC device to use
 * @param[out]      dest    The result is stored here
 *
 * @pre     The ADC @p adc is initialized using @ref adc_ng_init
 *
 * @retval  0               Success
 * @retval  <0              A device specific error occurred
 *
 * @note    The sample is returned as signed integer to allow for differential
 *          two ended channels: If the input voltage on the positive input is
 *          lower than the voltage at the negative input the result should be
 *          negative. For "normal" single ended inputs the result must always
 *          be positive or zero.
 */
static inline int adc_ng_single(uint8_t adc, int32_t *dest)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    assert(dest);
    const adc_ng_backend_t be = adc_ng_backends[adc];
    return be.driver->single(be.handle, dest);
}

/**
 * @brief   Initialize an ADC channel, perform a single conversion with
 *          maximum resolution and range, and power it off again
 *
 * @param[in]       adc     ADC device to use
 * @param[in]       chan    Channel to use
 * @param[out]      dest    The result is stored here
 *
 * @retval  0               Success
 * @retval  <0              A driver specific error occurred
 *
 * Refer to the documentation of @ref adc_init for details on @p ref
 */
static inline int adc_ng_quick(uint8_t adc, uint8_t chan,
                               int32_t *dest)
{
    int16_t ref = ADC_NG_MAX_REF;
    int retval = adc_ng_init(adc, chan, adc_ng_max_res(adc), &ref);
    if (retval) {
        return retval;
    }
    retval = adc_ng_single(adc, dest);
    adc_ng_off(adc);
    return retval;
}

/** @} */

/**
 * @name    Functions to get measurements in voltage levels
 *
 * @{
 */

/**
 * @brief   Run a single measurements and get the result in mV
 *
 * @param[in]       adc     ADC to use
 * @param[out]      dest_mv Write the measurement result in mV here
 *
 * @pre     The state given in @p state has been initialized (see
 *          @ref adc_ng_init) and the ADC currently is powered
 *
 * @retval  0               Success
 * @retval  <0              A driver specific error occurred
 *
 * @note    Please note that a while a precision roughly in the order of µV
 *          is with the right setup and ADC technically feasible, the accuracy
 *          of the result is more likely in the order of mV.
 */
static inline int adc_ng_voltage(uint8_t adc, int16_t *dest_mv)
{
    int32_t sample;
    int retval = adc_ng_single(adc, &sample);
    if (retval) {
        return retval;
    }

    *dest_mv = adc_ng_convert(adc, sample);
    return 0;
}
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ADC_NG_H */
/** @} */
