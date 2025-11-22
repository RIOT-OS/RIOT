/*
 * SPDX-FileCopyrightText: 2018 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_periph_can CAN
 * @ingroup     drivers_periph
 * @brief       Low-level CAN peripheral driver interface
 *
 * This is a basic abstract peripheral to be used with candev.
 *
 * The @p can_t type should be compatible with @p candev_t to be used by the
 * candev interface.
 *
 * See auto init for more details how the can_t is used.
 *
 * @{
 *
 * @file
 * @brief       Low-level CAN peripheral driver interface definitions
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#include "periph_cpu.h"
#include "can/candev.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_CAN_T
/**
 * @brief   CAN device descriptor identifier
 */
typedef candev_t can_t;
#endif

#ifndef HAVE_CAN_CONF_T
/**
 * @brief   CAN configuration identifier
 */
typedef int can_conf_t;
#endif

/**
 * @brief   Initialize a periph CAN device with the given configuration
 *
 * @param[in,out]   dev     device descriptor
 * @param[in]       conf    device vonfiguration
 */
void can_init(can_t *dev, const can_conf_t *conf);

#ifdef __cplusplus
}
#endif

/** @} */
