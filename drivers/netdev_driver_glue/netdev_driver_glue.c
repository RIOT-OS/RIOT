/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_netdev_driver_glue
 * @{
 *
 * @file
 * @brief       Glue code to keep legacy netdev drivers working
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include "net/netdev.h"
#include "net/netdev_driver_glue.h"

void netdev_driver_glue_drop(netdev_t *dev)
{
    dev->driver->recv(dev, NULL, 1, NULL);
}

int netdev_driver_glue_size(netdev_t *dev)
{
    return dev->driver->recv(dev, NULL, 0, NULL);
}
