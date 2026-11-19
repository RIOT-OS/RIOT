/*
 * SPDX-FileCopyrightText: 2024 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_samd5x
 * @brief           CPU specific definitions for CAN controllers
 * @{
 *
 * @file
 * @brief           SAMD5x CAN controller (M_CAN Bosch) default device parameters
 *
 * @author          Firas Hamdi <firas.hamdi@ml-pa.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "can/device.h"

/** Default SAMD5x CAN devices parameters */
static const candev_params_t candev_params[] = {
    {
        .name = "can_samd5x_0",
    },
    {
        .name = "can_samd5x_1",
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
