/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_candev_linux
 * @{
 *
 * @file
 * @brief       Default linux can config
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef CANDEV_LINUX_PARAMS_H
#define CANDEV_LINUX_PARAMS_H

#include "candev_linux.h"
#include "can/device.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default parameters (device names)
 */
static candev_params_t candev_linux_params[] = {
    { .name = "can0", },
    { .name = "can1", },
};

#ifdef __cplusplus
}
#endif

#endif /* CANDEV_LINUX_PARAMS_H */
/** @} */
