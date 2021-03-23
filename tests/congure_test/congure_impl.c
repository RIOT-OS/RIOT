/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
    congure_mock_snd_setup(c);
    return 0;
}

/** @} */
