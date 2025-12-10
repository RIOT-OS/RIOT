/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "congure/mock.h"

#include "congure_impl.h"

int congure_test_snd_setup(congure_test_snd_t *c, unsigned id)
{
    if (id > 0) {
        return -1;
    }
    congure_mock_snd_setup(c, NULL);
    return 0;
}

/** @} */
