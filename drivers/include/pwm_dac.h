/*
 * Copyright (C) 2020 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pwm_dac
 * @{
 *
 * @file
 * @brief       DAC implementation based on a PWM device
 *
 * When this driver is used, a PWM pin can be accessed as a DAC to produce
 * analog voltages. While the voltage produced by the pin is oscillating, a low
 * pass filter connected to the pin (often implemented as a resitor and a
 * capacitor) can produce an actual analog voltage.
 *
 * For expressing the channel in @ref dac_t objects, the implementation uses
 * 32-bit long IDs and not rely on int to be long enough), and encodes the
 * PWM's resolution, the PWM device and PWM channel in one value to remove the
 * need for any additional allocation. In that, it relies on pwm_t itself using
 * no more than its lower 16 bits.
 *
 * Boards that use this should modify the input to @ref PWM_DAC_PARAMS or
 * provide a full value; the @ref DAC_LINE macro can then be used up to @ref
 * DAC_NUMOF.
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 * @}
 */

#ifndef PWM_DAC_H
#define PWM_DAC_H

#include <periph/dac.h>
#include <periph/pwm.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /** @brief PWM device to be used */
    pwm_t dev;
    /** @brief Frequency to run PWM with
     *
     * This needs to be chose high enough that the low pass filter attached
     * does not let the configured frequency pass.
     */
    uint32_t freq;
    /** @brief PWM mode to be used
     *
     * The previce mode usually does not matter; what matters is that it is
     * supported by the hardware.
     * */
    pwm_mode_t mode;
    /** @brief PWM resolution to be used
     *
     * Indicates the number of bits utilized in the PWM device. While higher
     * numbers are preferable for dynamic resolution, picking a lower number
     * may be necessary to allow for a sufficiently high frequency on the PWM
     * device.
     */
    uint8_t resbits;
} pwm_dac_params_t;

/**
 * @brief   Set up a PWM device as a DAC
 *
 * This is typically done in the course of auto-initialization.
 *
 * In this step, the frequency and resolution of the PWM are set; when later a
 * DAC value is configured, it is expressed as a duty cycle.
 *
 * Note that this does *not* call @ref dac_init on the resulting DAC (which
 * would power it on and leave it at 0).
 *
 * There is no necessity to call `dac_init` with the current implementation
 * (calling @ref dac_set followed by @ref dac_poweron), but that's considered
 * an implementation detail that may change. Therefore, still use @ref dac_init
 * -- if its zero initializing value causes trouble, follow up on
 *  <https://github.com/RIOT-OS/RIOT/issues/15121>.
 *
 * Note that of the full PWM device, only channel 0 gets used; the other
 * channels can not be used as not only would they need a cloned device entry,
 * they'd also necessitate more complicated coordination of power management as
 * turning any single one of them off wouldn't allow turning off the complete
 * PWM driver.
 *
 * @param   line    DAC peripheral line to initialize
 *
 * @retval  0       Success
 * @retval  -EINVAL The parameters were not valid for the underlying PWM device
 *
 */
int pwm_dac_init(dac_t line);

#ifdef __cplusplus
}
#endif

#endif /* PWM_DAC_H */
