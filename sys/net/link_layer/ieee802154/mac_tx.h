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
 * @brief Finish the current TX and report status.
 */
void ieee802154_mac_tx_finish_current(ieee802154_mac_t *mac, int status, ieee802154_tx_info_t *info);

/**
 * @brief Transmit a MAC frame to the given destination.
 */
int ieee802154_mac_tx(ieee802154_mac_t *mac, const ieee802154_ext_addr_t *dst_addr);

#ifdef __cplusplus
}
#endif

/** @} */
