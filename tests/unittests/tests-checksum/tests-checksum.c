/*
 * Copyright 2016 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "tests-checksum.h"

void tests_checksum(void)
{
    TESTS_RUN(tests_checksum_crc16_ccitt_tests());
    TESTS_RUN(tests_checksum_fletcher16_tests());
    TESTS_RUN(tests_checksum_fletcher32_tests());
}
