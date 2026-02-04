/*
 * SPDX-FileCopyrightText: 2021 Gerson Fernando Budke <nandojve@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Main test application for ATxmega
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include "stdio.h"
#include "cpu_tests.h"

int main(void)
{
    puts("Start XMEGA tests");

#if defined(MODULE_ATXMEGA_EBI)
    puts("Start EBI tests");
    ebi_tests();
#endif

    puts("Finished XMEGA tests");

    return 0;
}
