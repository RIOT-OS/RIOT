/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_extend_pwm
 *
 * @{
 *
 * @file
 * @brief       PWM extension not-supported functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#if MODULE_EXTEND_PWM

#include <inttypes.h>

#include "periph/pwm.h"
#include "extend/pwm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

uint32_t pwm_ext_init_notsup(void *dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    (void)dev;
    (void)mode;
    (void)freq;
    (void)res;

    DEBUG("[%s] call for dev=%p mode=%u freq=%"PRIu32" res=%u\n", __func__,
          dev, mode, freq, res);
    return 0;
}

uint8_t pwm_ext_channels_notsup(void *dev)
{
    (void)dev;

    DEBUG("[%s] call for dev=%p\n", __func__, dev);
    return 0;
}

void pwm_ext_set_notsup(void *dev, uint8_t channel, uint16_t value)
{
    (void)dev;
    (void)channel;
    (void)value;

    DEBUG("[%s] call for dev=%p chn=%u val=%u\n", __func__, dev, channel, value);
}

void pwm_ext_poweron_notsup(void *dev)
{
    (void)dev;

    DEBUG("[%s] call for dev=%p\n", __func__, dev);
}

void pwm_ext_poweroff_notsup(void *dev)
{
    (void)dev;

    DEBUG("[%s] call for dev=%p\n", __func__, dev);
}

/* not-supported driver */
const pwm_ext_driver_t pwm_ext_notsup_driver = {
    .init = pwm_ext_init_notsup,
    .channels = pwm_ext_channels_notsup,
    .set = pwm_ext_set_notsup,
    .poweron = pwm_ext_poweron_notsup,
    .poweroff = pwm_ext_poweroff_notsup,
};
#endif /* MODULE_EXTEND_PWM */
