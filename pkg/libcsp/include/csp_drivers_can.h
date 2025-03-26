/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_libcsp
 * @{
 *
 * @file
 * @brief       LibCSP RIOT types
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef CSP_DRIVERS_CAN_H
#define CSP_DRIVERS_CAN_H

#include <stdint.h>

#include "can/device.h"
#include "csp/csp.h"
#include "csp/csp_interface.h"
#include "csp/interfaces/csp_if_can.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Auto-Initialize CSP can interface address
 */
#define     CSP_CAN_IFACE_ADDR_UNDEF    UINT16_MAX

/**
 * @brief CSP CAN interface parameters
 */
typedef struct csp_can_iface_params {
    int ifnum;                          /**< CAN device interface number */
    uint16_t addr;                      /**< CSP CAN iface address, UIN16_MAX to use defaults */
    uint16_t netmask;                   /**< CSP CAN iface netmask */
} csp_can_iface_params_t;

/**
 * @brief CSP CAN interface type
 */
typedef struct csp_can_iface {
    candev_dev_t *dev;                  /**< can device */
    csp_iface_t iface;                  /**< csp iface */
    csp_can_interface_data_t ifdata;    /**< csp iface data */
    kernel_pid_t pid;                   /**< pid of rx thread */
} csp_can_iface_t;

/**
 * @brief   Creates and starts CSP CAN Iface from a can_dev_t interface number
 *
 * @param[inout] ctx        Already setup CSP CAN iface
 * @param[in]    params     Can interface parameters
 *
 * @return pid of iface thread, NULL on error
 */
int csp_can_iface_init(csp_can_iface_t *ctx, const csp_can_iface_params_t *params);

/**
 * @brief   Creates and starts CSP CAN Iface from a can_dev_t interface number
 *
 * @param[inout] ctx        Already setup CSP CAN iface
 * @param[in]    stack      the receive thread stack
 * @param[in]    stacksize  the receive thread stack size
 * @param[in]    priority   the receive thread priority
 * @param[in]    name       the receive thread name
 *
 * @return pid of iface thread, NULL on error
 */
kernel_pid_t csp_can_iface_create(csp_can_iface_t *ctx, char *stack, int stacksize,
                                  char priority, const char *name);

#ifdef __cplusplus
}
#endif

#endif /* CSP_DRIVERS_CAN_H */
