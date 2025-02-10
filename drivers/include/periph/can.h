/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef PERIPH_CAN_H
#define PERIPH_CAN_H

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

/**
 * @brief   Convert a `candev_t *` to `can_t`
 * @param[in]   candev      `candev_t` pointer to convert
 * @return  The matching equivalent `can_t` pointer
 * @retval  NULL    Argument @p candev is not a `can_t` pointers
 *
 * @note    Implementations **MUST** check that @ref candev_t::driver indeed
 *          points to the peripheral CAN driver, before casting to `can_t *`.
 */
can_t *candev2periph(candev_t *candev);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CAN_H */
/** @} */
