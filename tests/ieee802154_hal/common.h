/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Common header for ieee802154_hal tests.
 *
 * @author  José I. Alamos <jose.alamos@haw-hamburg.de>
 */
#ifndef COMMON_H
#define COMMON_H

#include "net/ieee802154/radio.h"
#if IS_USED(MODULE_SOCKET_ZEP)
#include "socket_zep_params.h"
#else
#define SOCKET_ZEP_MAX  0
#endif

#define RADIOS_NUMOF IS_USED(MODULE_CC2538_RF) + \
                     IS_USED(MODULE_NRF802154) + \
                     SOCKET_ZEP_MAX

#if RADIOS_NUMOF == 0
#error "Radio is not supported"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Application-internal functions and variables for ieee802154_hal tests
 * @internal
 * @{
 */
typedef enum {
    IEEE802154_DEV_TYPE_CC2538_RF,
    IEEE802154_DEV_TYPE_NRF802154,
    IEEE802154_DEV_TYPE_SOCKET_ZEP,
} ieee802154_dev_type_t;

typedef ieee802154_dev_t* (*ieee802154_dev_cb_t)(ieee802154_dev_type_t type,
                           void *opaque);

void ieee802154_hal_test_init_devs(ieee802154_dev_cb_t cb, void *opaque);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */
/** @} */
