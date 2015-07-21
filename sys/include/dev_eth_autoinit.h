/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_net_dev_eth dev_eth auto setup
 * @ingroup     net_ng_ethernet
 * @file
 * @brief       Automatically setup available ethernet devices
 * @{
 *
 * @brief       header for dev_eth automatic initialization
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef DEV_ETH_AUTOINIT_H
#define DEV_ETH_AUTOINIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief enum for available ethernet devices
 */
enum {
#ifdef MODULE_NG_NATIVENET
    DEV_ETH_TAP,
#endif
#ifdef MODULE_ENCX24J600
    DEV_ETH_ENCX24J600,
#endif
    /* must be last: */
    NUM_DEV_ETH
};

/**
 * @brief Array of const pointers to available ethernet devices
 */
extern dev_eth_t *const dev_eth_devices[];

/**
 * @brief Automatically sets up available dev_eth ethernet devices
 *
 * ... by calling the respective *_setup() functions if available.
 */
void dev_eth_autoinit(void);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* DEV_ETH_AUTOINIT_H */
