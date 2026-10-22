/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_suit
 * @{
 *
 * @file
 * @brief       SUIT update policy checking code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "log.h"
#include "suit/policy.h"
#include "suit.h"

int suit_policy_check(suit_manifest_t *manifest)
{
    if (SUIT_DEFAULT_POLICY & ~(manifest->validated)) {
        LOG_INFO("SUIT policy check failed!\n");
        return -1;
    }
    else {
        LOG_INFO("SUIT policy check OK.\n");
        return 0;
    }
}
