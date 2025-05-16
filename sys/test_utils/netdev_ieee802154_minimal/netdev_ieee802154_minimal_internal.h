/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     test_utils_netdev_ieee802154_minimal
 *
 * @{
 * @file
 * @brief       Internal definitions for the netdev_ieee802154_minimal module
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */


#include "net/netdev.h"
#include "event.h"
#include "init_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

typedef struct {
    event_t event;      /**< event to serve ISR */
    netdev_t *dev;      /**< pointer to the device */
} device_reg_entry_t;

extern device_reg_entry_t _devices[NETDEV_IEEE802154_MINIMAL_NUMOF];

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif


/** @} */
