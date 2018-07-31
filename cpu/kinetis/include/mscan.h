/*
 * Copyright (C) 2018 HERE Deutschland GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_kinetis_mscan MSCAN driver
 * @ingroup     cpu_kinetis
 * @brief       Implementation of the driver for CAN controller in Kinetis KEA
 *              MCUs
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the MSCAN driver.
 *
 * @author      Anton Gerasimov <anton.gerasimov@here.com>
 */

#ifndef MSCAN_H
#define MSCAN_H

#include "can/candev.h"
#include "can/device.h"

#ifndef CANDEV_KEA_DEFAULT_BITRATE
/**
 * Default bitrate setup
 */
#define CANDEV_KEA_DEFAULT_BITRATE (50000)
#endif

#ifndef CANDEV_KEA_DEFAULT_SPT
/**
 * Default sampling point setup
 */
#define CANDEV_KEA_DEFAULT_SPT (875)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Board-specific MSCAN configuration
 */
typedef struct candev_kea_conf {
    uint32_t clock_freq;    /**< input clock frequency */
    candev_params_t params; /**< device parameters */
} candev_kea_conf_t;

/**
 * @brief MSCAN-specific "subclass" of candev_t
 */
typedef struct candev_kea {
    candev_t candev;               /**< candev base structure */
    canopt_state_t mode;           /**< power state */
    const candev_kea_conf_t *conf; /**< device configuration */

} candev_kea_t;

/**
 * @brief Initialize device with configuration
 *
 * @return 0 on success
 * @return <0 on error
 */
int candev_kea_init(candev_kea_t *dev, const candev_kea_conf_t *conf);

#ifdef __cplusplus
}
#endif

#endif /* MSCAN_H */

/** @} */
