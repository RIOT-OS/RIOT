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
 * @brief       ADC extension default / example list
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#ifndef ADC_EXT_CONF_H
#define ADC_EXT_CONF_H

#include <stddef.h>

#include "extend/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference the driver struct
 */
extern adc_ext_driver_t adc_ext_notsup_driver;

/**
 * @brief   ADC expansion default list if not defined
 */
static const adc_ext_t adc_ext_list[] =
{
    {
        .driver = &adc_ext_notsup_driver,
        .dev = NULL,
    },
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_EXT_CONF_H */
/** @} */
