/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef CAN_PARAMS_H
#define CAN_PARAMS_H

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
#endif /* CAN_PARAMS_H */
