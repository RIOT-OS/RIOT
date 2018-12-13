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
 * @brief       PWM extension redirection functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#if MODULE_EXTEND_PWM

#include "periph/pwm.h"
#include "extend/pwm.h"
#include "pwm_ext_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

pwm_ext_t *pwm_ext_entry(pwm_t dev)
{
    if (dev == PWM_UNDEF) {
        return NULL;
    }

    DEBUG("[%s] list entry is %u\n", __func__, (dev - PWM_NUMOF));

    /* device is greater than number of listed entries */
    if ((dev - PWM_NUMOF) >= PWM_EXT_NUMOF) {
        return NULL;
    }

    /* Cast to discard const */
    return (pwm_ext_t *)&pwm_ext_list[dev - PWM_NUMOF];
}

uint32_t pwm_init_redir(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    DEBUG("[%s] dev %u, mode %u, freq %"PRIu32", res %u\n", __func__,
          dev, mode, freq, res);

    pwm_ext_t *entry = pwm_ext_entry(dev);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for dev %u\n", __func__, dev);
        return 0;
    }

    return entry->driver->init(entry->dev, mode, freq, res);
}


uint8_t pwm_channels_redir(pwm_t dev)
{
    DEBUG("[%s] dev %u\n", __func__, dev);

    pwm_ext_t *entry = pwm_ext_entry(dev);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for dev %u\n", __func__, dev);
        return 0;
    }

    return entry->driver->channels(entry->dev);
}

void pwm_set_redir(pwm_t dev, uint8_t channel, uint16_t value)
{
    DEBUG("[%s] dev %u, chn %u, val %u\n", __func__, dev, channel, value);

    pwm_ext_t *entry = pwm_ext_entry(dev);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for dev %u\n", __func__, dev);
        return;
    }

    entry->driver->set(entry->dev, channel, value);
}

void pwm_poweron_redir(pwm_t dev)
{
    DEBUG("[%s] dev %u\n", __func__, dev);

    pwm_ext_t *entry = pwm_ext_entry(dev);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for dev %u\n", __func__, dev);
        return;
    }

    entry->driver->poweron(entry->dev);
}

void pwm_poweroff_redir(pwm_t dev)
{
    DEBUG("[%s] dev %u\n", __func__, dev);

    pwm_ext_t *entry = pwm_ext_entry(dev);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for dev %u\n", __func__, dev);
        return;
    }

    entry->driver->poweroff(entry->dev);
}


#endif /* MODULE_EXTEND_PWM */
