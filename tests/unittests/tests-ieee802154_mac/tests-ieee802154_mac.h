/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``ieee802154`` module
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

void tests_ieee802154_mac(void);

/**
 * @brief   The entry point of this test suite.
 */
Test *tests_ieee802154_mac_mac_queue_tests(void);

#ifdef __cplusplus
}
#endif

/** @} */
