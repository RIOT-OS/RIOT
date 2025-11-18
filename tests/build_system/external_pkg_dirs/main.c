/*
 * SPDX-FileCopyrightText: 2021 Niklaus Leuenberger
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       This is a test for the EXTERNAL_PKG_DIRS variable.
 *
 * @author      Niklaus Leuenberger <niklaus.leuenb@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#ifndef TEST_EXTERNAL_PKG
    #error "Required external package not included."
#endif

#ifdef TEST_EXTERNAL_PKG_NOT_USED
    #error "External package included that shouldn't be."
#endif

int main(void)
{
    puts("If it compiles, it works!");
    return 0;
}
