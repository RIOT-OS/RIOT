/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup drivers_candev_linux
 * @{
 */

/**
 * @file
 * @brief  Default linux can config
 * @author Vincent Dupont <vincent@otakeys.com>
 */

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

/** @} */
