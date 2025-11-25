/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_dw1000
 *
 * @{
 *
 * @file
 * @brief       Abstraction layer for RIOT adaption
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */
#ifndef UWB_DW1000_H
#define UWB_DW1000_H

#include <stdint.h>

#include "dw1000/dw1000_dev.h"
#include "dw1000/dw1000_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct dw1000_dev_cfg dw1000_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    dw1000_dev_instance_t dev;         /**< dwDevice parent struct */
} uwb_dw1000_t;

/**
 * @brief   Sets device linked list to 0, not really needed...
 */
void uwb_dw1000_init(void);

/**
 * @brief   Sets the tx and rx buffer for the uwb_dev in the dw1000 instance
 *
 * @note If this is not set before uwb_dw1000_setup() is called then
 *       the buffers will be dynamically allocated.
 *
 * @param[in]   dev     dw1000 device instance pointer
 * @param[in]   tx_buf  transmit buffer
 * @param[in]   rx_buf  receive buffer
 */
void uwb_dw1000_set_buffs(dw1000_dev_instance_t* dev, uint8_t* tx_buf,
                          uint8_t* rx_buf);

/**
 * @brief   Setup a dw1000 device
 *
 * This will setup the dw1000 dev instance and the uwb_dev instance within.
 *
 * @param[out]  dev     dw1000 device descriptor
 * @param[in]   params  receive buffer
 */
void uwb_dw1000_setup(dw1000_dev_instance_t* dev, dw1000_params_t* params);

/**
 * @brief   Configure and start the dw1000
 *
 * This will wakeup and setup the dw1000 device, configure the mac and
 * phy. Setting up the mac will also call dw1000_tasks_init() that will
 * handle the device interrupts.
 *
 * @param[out]  dev     dw1000 device descriptor
 */
void uwb_dw1000_config_and_start(dw1000_dev_instance_t* dev);

/**
 * @brief   Updates tx_power config from OTP is any
 *
 *
 * @param[out]  dev     dw1000 device descriptor
 */
void uwb_dw1000_update_config_from_otp(dw1000_dev_instance_t* dev);

#ifdef __cplusplus
}
#endif

#endif /* UWB_DW1000_H */
