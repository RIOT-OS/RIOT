/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests backtrace module.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "backtrace.h"

int main(void)
{
    printf("BACKTRACE_SIZE: %d\n", backtrace_len());
    printf("\n## backtrace_print: print addresses\n");
    backtrace_print();
    printf("\n## backtrace_print_symbols: print symbol information\n");
    backtrace_print_symbols();
    return 0;
}
