/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Inria
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_suit_v4
 * @{
 *
 * @file
 * @brief       SUIT v4 policy checking code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "suit/v4/suit.h"
#include "suit/v4/policy.h"

#include "log.h"

int suit_v4_policy_check(suit_v4_manifest_t *manifest)
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
