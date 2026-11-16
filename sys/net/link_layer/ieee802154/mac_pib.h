/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @{
 *
 * @file
 * @author Moritz Holzer <moritz.holzer@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/ieee802154/mac.h"

/**
 * @brief Initialize the MAC PIB defaults.
 */
void ieee802154_mac_pib_init(ieee802154_mac_t *mac);
/**
 * @brief Handle a MAC MLME-SET request.
 */
void ieee802154_mac_mlme_set(ieee802154_mac_t *mac,
                            ieee802154_pib_attr_t attr,
                            const ieee802154_pib_value_t *in);
/**
 * @brief Handle a MAC MLME-GET request.
 */
void ieee802154_mac_mlme_get(ieee802154_mac_t *mac,
                            ieee802154_pib_attr_t attr,
                            ieee802154_pib_value_t *out);

#ifdef __cplusplus
}
#endif

/** @} */
