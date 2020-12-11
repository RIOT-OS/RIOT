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
 * Boards that use this should provide a DAC_NUMOF macro, and redefine the
 * DAC_LINE macro in their `periph_conf.h` header that, for each n wher 0 <= n
 * < DAC_NUMOF, produces the statically configured setup -- or define invalid
 * macros there to flag anything that'd try to use them.
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

/**
 * @brief   Set up a PWM device as a DAC
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
 * If the underlying PWM has channels, the returned dac_t represents its
 * channel 0; other channels can be set independently by adding their channel
 * index to the returned value.
 *
 * @param[in] dev PWM device to be used
 * @param[in] mode PWM mode to be used
 * @param[in] freq Frequency to run PWM with. This needs to be chose high
 *     enough that the low pass filter attached does not let the configured
 *     frequency pass.
 * @param[in] res PWM resolution to be used. Must be a power of 2. Picking a
 *     low number here may be necessary to allow for a sufficiently high
 *     frequency.
 *
 * @return A dac_t usable for dac_set, or DAC_UNDEF if PWM initialization failed
 *
 */
dac_t pwm_dac_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res);

#ifdef __cplusplus
}
#endif

#endif /* PWM_DAC_H */
