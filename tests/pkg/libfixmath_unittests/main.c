/*
 * SPDX-FileCopyrightText: 2014 René Kijewski <rene.kijewski@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup   tests
 * @{
 *
 * @file
 * @brief     execute libfixmath's unittests in RIOT
 *
 * @author    René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

void test_utils_main_exit_cb(int res)
{
    if (res) {
        puts("FAILED");
    } else {
        puts("SUCCESS");
    }
}
