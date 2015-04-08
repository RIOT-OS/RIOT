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
 * @brief       Low-level ethernet driver interface
 * @{
 *
 * @file
 * @brief       Definitions low-level ethernet driver interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef DEV_ETH_H
#define DEV_ETH_H

#include <stdint.h>
#include "ng_ethernet/hdr.h"

typedef struct dev_eth {
    struct eth_driver *driver;
} dev_eth_t;

typedef struct eth_driver {
    int (*send)(dev_eth_t *dev, char* buf, int len);
    int (*recv)(dev_eth_t *dev, char* buf, int len);
    void (*get_mac_addr)(dev_eth_t *dev, uint8_t *buf);
    int (*get_promiscous)(dev_eth_t *dev);
    int (*set_promiscous)(dev_eth_t *dev, int value);
    int (*init)(dev_eth_t *dev);
    void (*isr)(dev_eth_t *dev);
} eth_driver_t;

static inline int dev_eth_init(dev_eth_t *dev) {
    return dev->driver->init(dev);
}

extern void dev_eth_isr(dev_eth_t *dev);
extern void dev_eth_rx_handler(dev_eth_t *dev);
extern void dev_eth_linkstate_handler(dev_eth_t *dev, int newstate);

#endif /* DEV_ETH_H */
