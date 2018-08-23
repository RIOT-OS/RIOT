/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    drivers_netdev_driver_glue Glue for legacy netdev drivers
 * @ingroup     drivers_netdev
 * @brief       This provides helper functions to keep legacy network drivers
 *              working
 * @{
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef NET_NETDEV_DRIVER_GLUE_H
#define NET_NETDEV_DRIVER_GLUE_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A glue function to bridge between the new netdev_driver API and the
 *        old
 *
 * Calls @ref netdev_driver::recv "dev->driver->recv()" with `NULL` as buffer
 * and a non-zero length, which was previously used to drop frames.
 */
void netdev_driver_glue_drop(netdev_t *dev);

/**
 * @brief A glue function to bridge between the new netdev_driver API and the
 *        old
 *
 * Calls @ref netdev_driver::recv "dev->driver->recv()" with `NULL` as buffer
 * and a zero length, which was previously used to get the expected length of
 * the received frame without dropping it.
 */
int netdev_driver_glue_size(netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NET_NETDEV_DRIVER_GLUE_H */
/** @} */
