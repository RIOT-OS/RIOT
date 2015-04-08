/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_dev_eth Ethernet
 * @ingroup     net
 * @brief       Low-level ethernet driver for native tap interfaces
 * @{
 *
 * @file
 * @brief       Definitions for low-level ethernet driver for native tap interfaces
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef DEV_ETH_TAP_H
#define DEV_ETH_TAP_H

#include <stdint.h>
#include "net/dev_eth.h"

#include "net/if.h"

typedef struct dev_eth_tap {
    dev_eth_t ethdev;
    char tap_name[IFNAMSIZ];            /**< host dev file name */
    int tap_fd;                         /**< host file descriptor for the TAP */
    uint8_t addr[NG_ETHERNET_ADDR_LEN]; /**< The MAC address of the TAP */
    uint8_t promiscous;                 /**< Flag for promiscous mode */
} dev_eth_tap_t;

extern dev_eth_tap_t dev_eth_tap;

void dev_eth_tap_setup(dev_eth_tap_t *dev, const char *name);

extern void dev_eth_tap_isr(void);
/**
 * @}
 */
#endif /* DEV_ETH_TAP_H */
