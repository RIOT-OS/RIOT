
/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "embUnit/embUnit.h"

#include "net/ieee802154/mac.h"
#include "tests-ieee802154_mac.h"


void tests_ieee802154_mac(void)
{
    TESTS_RUN(tests_ieee802154_mac_mac_queue_tests());
}
