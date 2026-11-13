/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include "net/ieee802154/mac.h"
#include "net/ieee802154/radio.h"

#ifdef __cplusplus
extern "C" {
#endif

int ieee802154_mac_test_init_devs(ieee802154_dev_t *radio,
                                  ieee802154_dev_type_t *dev_type);

#ifdef __cplusplus
}
#endif
