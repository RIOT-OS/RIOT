/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_extend_dac
 *
 * @{
 *
 * @file
 * @brief       DAC extension not-supported functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#if MODULE_EXTEND_DAC

#include "periph/dac.h"
#include "extend/dac.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int8_t dac_ext_init_notsup(void *dev, dac_t line)
{
    (void)dev;
    (void)line;

    DEBUG("[%s] call for dev %p line %u\n", __func__, dev, line);

    return -1;
}

void dac_ext_set_notsup(void *dev, dac_t line, uint16_t value)
{
    (void)dev;
    (void)line;
    (void)value;

    DEBUG("[%s] call for dev %p line %u value %u\n", __func__, dev, line, value);
}

void dac_ext_poweron_notsup(void *dev, dac_t line)
{
    (void)dev;
    (void)line;

    DEBUG("[%s] call for dev %p line %u\n", __func__, dev, line);
}

void dac_ext_poweroff_notsup(void *dev, dac_t line)
{
    (void)dev;
    (void)line;

    DEBUG("[%s] call for dev %p line %u\n", __func__, dev, line);
}

unsigned int dac_ext_channels_notsup(void *dev)
{
    (void)dev;

    DEBUG("[%s] call for dev %p\n", __func__, dev);
    return 0;
}

/* not-supported driver */
const dac_ext_driver_t dac_ext_notsup_driver = {
    .init = dac_ext_init_notsup,
    .set = dac_ext_set_notsup,
    .poweron = dac_ext_poweron_notsup,
    .poweroff = dac_ext_poweroff_notsup,
    .channels = dac_ext_channels_notsup
};
#endif /* MODULE_EXTEND_DAC */
