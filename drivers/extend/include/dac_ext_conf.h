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
 * @brief       DAC extension default / example list
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#ifndef DAC_EXT_CONF_H
#define DAC_EXT_CONF_H

#include <stddef.h>

#include "extend/dac.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference the driver struct
 */
extern dac_ext_driver_t dac_ext_notsup_driver;

/**
 * @brief   DAC expansion default list if not defined
 */
static const dac_ext_t dac_ext_list[] =
{
    {
        .driver = &dac_ext_notsup_driver,
        .dev = NULL,
    },
};

#ifdef __cplusplus
}
#endif

#endif /* DAC_EXT_CONF_H */
/** @} */
