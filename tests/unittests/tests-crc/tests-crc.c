/*
 * Copyright 2016 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "tests-crc.h"

void tests_crc(void)
{
    TESTS_RUN(tests_crc_crc16_ccitt_tests());
}
