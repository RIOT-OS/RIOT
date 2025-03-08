/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup drivers_candev_linux
 * @{
 */

/**
 * @file
 * @brief  Default linux can config
 * @author Vincent Dupont <vincent@otakeys.com>
 */
#ifndef CAN_PARAMS_H
#define CAN_PARAMS_H

#include "candev_linux.h"
#include "can/device.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default parameters (device names)
 */
static const candev_params_t candev_params[] = {
#if CAN_DLL_NUMOF >= 1
    { .name = "can0", },
#endif
#if CAN_DLL_NUMOF >= 2
    { .name = "can1", },
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* CAN_PARAMS_H */
/** @} */
