/*
 * SPDX-FileCopyrightText: 2020 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Common header for ieee802154_hal tests.
 *
 * @author  José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#include "net/ieee802154/radio.h"
#if IS_USED(MODULE_SOCKET_ZEP)
#include "socket_zep_params.h"
#else
#define SOCKET_ZEP_MAX  0
#endif

#define RADIOS_NUMOF IS_USED(MODULE_CC2538_RF) + \
                     IS_USED(MODULE_NRF802154) + \
                     SOCKET_ZEP_MAX + \
                     IS_USED(MODULE_MRF24J40) + \
                     IS_USED(MODULE_KW2XRF) + \
                     IS_USED(MODULE_ESP_IEEE802154)

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
    IEEE802154_DEV_TYPE_KW2XRF,
    IEEE802154_DEV_TYPE_MRF24J40,
    IEEE802154_DEV_TYPE_ESP_IEEE802154,
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

/** @} */
