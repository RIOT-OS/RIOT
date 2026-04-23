/*
 * Copyright (C) 2023 Prime Controls
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_onewire
 *
 * @{
 * @file
 * @brief       Default configuration for 1-Wire driver
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 */

#ifndef ONEWIRE_PARAMS_H
#define ONEWIRE_PARAMS_H

#include "board.h"

#include "container.h"
#include "onewire.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ONEWIRE_PARAMS_DRIVER
#define ONEWIRE_PARAMS_DRIVER NULL
#endif

#ifndef ONEWIRE_PARAMS_LLDEV
#define ONEWIRE_PARAMS_LLDEV NULL
#endif

#ifndef ONEWIRE_PARAMS_LLDEV_PARAMS
#define ONEWIRE_PARAMS_LLDEV_PARAMS NULL
#endif

#ifndef ONEWIRE_PARAMS
#define ONEWIRE_PARAMS \
    { \
        .driver = ONEWIRE_PARAMS_DRIVER, \
        .lldev = ONEWIRE_PARAMS_LLDEV, \
        .lldev_params = ONEWIRE_PARAMS_LLDEV_PARAMS, \
    }
#endif

static const onewire_params_t onewire_params[] = {
    ONEWIRE_PARAMS
};

#define ONEWIRE_NUMOF ARRAY_SIZE(onewire_params)

/** board's 1-Wire buses masters */
extern onewire_t onewire_buses[ONEWIRE_NUMOF];

#ifdef __cplusplus
}
#endif

#endif /* ONEWIRE_PARAMS_H */
/** @} */
