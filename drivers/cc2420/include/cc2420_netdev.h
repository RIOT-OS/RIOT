/*
 * Copyright (C) 2016 Inria
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    drivers_cc2420
 * @{
 *
 * @file
 * @brief      Netdev interface for the CC2420
 *
 * @author     Francisco Acosta <francisco.acosta@inria.fr>
 * @author     Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef CC2420_NETDEV_H
#define CC2420_NETDEV_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t cc2420_driver;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CC2420_NETDEV_H */
