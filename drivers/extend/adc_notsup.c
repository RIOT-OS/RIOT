/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_extend_adc
 *
 * @{
 *
 * @file
 * @brief       ADC extension not-supported functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#if MODULE_EXTEND_ADC

#include "periph/adc.h"
#include "extend/adc.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int adc_ext_init_notsup(void *dev, adc_t line)
{
    (void)dev;
    (void)line;

    DEBUG("[%s] call for dev %p line %u\n", __func__, dev, line);
    return -1;
}

int adc_ext_sample_notsup(void *dev, adc_t line, adc_res_t res)
{
    (void)dev;
    (void)line;

    DEBUG("[%s] call for dev %p line %u res %u\n", __func__, dev, line, res);
    return -1;
}

unsigned int adc_ext_channels_notsup(void *dev)
{
    (void)dev;

    DEBUG("[%s] call for dev %p\n", __func__, dev);
    return 0;
}

/* not-supported driver */
const adc_ext_driver_t adc_ext_notsup_driver = {
    .init = adc_ext_init_notsup,
    .sample = adc_ext_sample_notsup,
    .channels = adc_ext_channels_notsup
};
#endif /* MODULE_EXTEND_ADC */
