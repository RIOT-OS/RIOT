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
 * @brief       LibCSP CAN driver default configuration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef CSP_DRIVERS_CAN_PARAMS_H
#define CSP_DRIVERS_CAN_PARAMS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the CSP_DRIVERS_CAN
 * @{
 */
#ifndef CONFIG_CSP_DRIVERS_CAN_IFNUM
#define CONFIG_CSP_DRIVERS_CAN_IFNUM           (0)
#endif
#ifndef CONFIG_CSP_DRIVERS_CAN_ADDR
#define CONFIG_CSP_DRIVERS_CAN_ADDR            CSP_CAN_IFACE_ADDR_UNDEF
#endif
#ifndef CONFIG_CSP_DRIVERS_CAN_NETMASK
#define CONFIG_CSP_DRIVERS_CAN_NETMASK         (4)
#endif

#ifndef CONFIG_CSP_DRIVERS_CAN_PARAMS_DEFAULT
#define CONFIG_CSP_DRIVERS_CAN_PARAMS_DEFAULT        { .ifnum = CONFIG_CSP_DRIVERS_CAN_IFNUM, \
                                                       .addr = CONFIG_CSP_DRIVERS_CAN_ADDR, \
                                                       .netmask = CONFIG_CSP_DRIVERS_CAN_NETMASK }
#endif
/**@}*/

/**
 * @brief   Default CSP Can interface parameters
 */
static const csp_can_iface_params_t csp_can_iface_params[] = {
    CONFIG_CSP_DRIVERS_CAN_PARAMS_DEFAULT,
};

#ifdef __cplusplus
}
#endif

#endif /* CSP_DRIVERS_CAN_PARAMS_H */
