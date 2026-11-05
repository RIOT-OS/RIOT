/*
 * SPDX-FileCopyrightText: 2026 Martine Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup     tests
 * @{
 *
 * @file
 * @brief   Provides @ref drivers_netdev mocks
 *
 * @author  Martine Lenders <mlenders@riot-os.org>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize a mock Ethernet device
 */
void netdev_mock_init_eth(void);

/**
 * @brief   Initialize a mock IEEE 802.15.4 device
 */
void netdev_mock_init_ieee802154(void);

#ifdef __cplusplus
}
#endif

/** @} */
